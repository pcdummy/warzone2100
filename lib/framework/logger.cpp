/*
    This file is part of Warzone 2100.
    Copyright (c) 2010, Razvan Petru
    Copyright (C) 2011  Warzone 2100 Project

    Warzone 2100 is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Warzone 2100 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Warzone 2100; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "logger.h"
#include "loggerdest.h"

// QT
#include <QtCore/QMutex>
#include <QtCore/QList>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

// LOG_POUP
#include <QtGui/QMessageBox>

#include <lib/framework/frame.h>

typedef QList<LoggerDestination*> LoggerDestinationList;
static const QString fmtDateTime("hh:mm:ss");

class LoggerImpl
{
public:
    LoggerImpl() :
        useInputBuffer(true),
        popupTitle("We detected a problem."),
        popupBody("A non fatal error has occurred.\n\n%1\n\n"),
        fatalTitle("Application terminated unexpectedly"),
        fatalBody("Please check the console output."),
        alwaysFlush(false)
    {
        // This MUST be in the same order as the Levels Enum!
        levelNames.append("all");
        levelStatus.append(false);

        levelNames.append("trace");
        levelStatus.append(false);

        levelNames.append("debug");
        levelStatus.append(false);

        levelNames.append("info");
        levelStatus.append(true);

        levelNames.append("warning");
        levelStatus.append(false);

        levelNames.append("error");
        levelStatus.append(true);

        levelNames.append("fatal");
        levelStatus.append(true);

        // Last of "levels" for setLevelStatus
        levelNames.append("never");
        levelStatus.append(false);

        levelNames.append("popup");
        levelStatus.append(true);
    }

    int addLoggingLevel(const QString& name, bool status)
    {
        if (levelNames.contains(name))
        {
            int index = levelNames.indexOf(name);
            levelStatus[index] = status;
            return index;
        }

        QMutexLocker lock(&logMutex);
        levelNames.append(name);
        levelStatus.append(levelStatus.at(LOG_ALL) ? true : status);

        return levelNames.indexOf(name);
    }

    QMutex             logMutex;
    QStringList        levelNames;
    QList<bool>        levelStatus;
    LoggerDestinationList    destList;

    // Input buffer
    bool                useInputBuffer;
    qint32              repeated;
    qint32              next;
    qint32              prev;
    QString             lastMessage;

    // LOG_POPUP
    const char         *popupTitle;
    const char         *popupBody;

    // LOG_FATAL popup
    const char         *fatalTitle;
    const char         *fatalBody;

    // Flushing
    bool                alwaysFlush;
};

Logger::Logger() :
    d(new LoggerImpl)
{
}

Logger::~Logger()
{
    delete d;
}

void Logger::addDestination(LoggerDestination* destination)
{
    assert(destination);

    destination->setAlwaysFlush(d->alwaysFlush);
    d->destList.push_back(destination);
}

int Logger::addLoggingLevel(const QString& name, bool status)
{
    return d->addLoggingLevel(name, status);
}

int Logger::getLoggingLevel(const QString& name)
{
    return d->levelNames.indexOf(name);
}

bool Logger::setLevelStatus(const QString& name, bool status)
{
    int level = getLoggingLevel(name);
    if (level == -1) {
        return false;
    }

    return setLevelStatus(level, status);
}

bool Logger::setLevelStatus(int level, bool status)
{
    if (d->levelStatus.size() <= level)
    {
        return false;
    }

    QMutexLocker lock(&d->logMutex);

    if (level == LOG_ALL)
    {
        for (int i=0; i < d->levelStatus.size(); i++)
        {
            d->levelStatus[i] = status;
        }

        return true;
    }
    else if (level < LOG_NEVER)
    {
        for (int i=level; i < LOG_NEVER; i++)
        {
            d->levelStatus[i] = status;
        }

        return true;
    }

    d->levelStatus[level] = status;

    return true;
}

bool Logger::checkLevel(int level)
{
    if (d->levelStatus.size() <= level)
    {
        assert(d->levelStatus.size() <= level);
        return false;
    }
    return d->levelStatus[level];
}

void Logger::setInputBuffer(bool state)
{
    d->useInputBuffer = state;
    if (!state)
    {
        d->next = d->repeated = d->prev = 0;
        d->lastMessage.clear();
    }
}

void Logger::setAlwaysFlush(bool state)
{
    d->alwaysFlush = state;

    foreach(LoggerDestination* dest, d->destList)
    {
        if (!dest)
        {
            assert(!"null log destination");
            continue;
        }

        dest->setAlwaysFlush(state);
    }
}

void Logger::setPopupTitleBody(const char* title, const char* body)
{
    d->popupTitle = title;
    d->popupBody = body;
}

void Logger::setFatalTitleBody(const char* title, const char* body)
{
    d->fatalTitle = title;
    d->fatalBody = body;
}

void Logger::Helper::sprintfMe(const char* cformat, ...)
{
    char *buff;

    va_list ap;
    va_start(ap, cformat);
    vasprintf(&buff, cformat, ap);
    va_end(ap);

    buffer = buff;
    free(buff);
}

//! creates the complete log message and passes it to the logger
void Logger::Helper::writeToLog()
{
    Logger& logger = Logger::instance();

    // Remove quotes, FIXME: There should be a better way to do this.
    if (buffer.startsWith("\"") && buffer.endsWith("\" "))
    {
        buffer.remove(0, 1).chop(2);
    }

    if (buffer.endsWith("\n"))
    {
        buffer.chop(1);
    }

    if (level == LOG_POPUP)
    {
        QMessageBox msgBox(
            QMessageBox::Warning,
            logger.d->popupTitle,
            QString(logger.d->popupBody).arg(buffer)
        );
        msgBox.exec();
    }
    else if(level == LOG_FATAL)
    {
        QMessageBox msgBox(
            QMessageBox::Critical,
            logger.d->fatalTitle,
            QString(logger.d->fatalBody).arg(buffer)
        );
        msgBox.exec();
    }

    logger.write(logger.d->levelNames.at(level), function, buffer);
}

Logger::Helper::~Helper()
{
    writeToLog();
}

void Logger::write(const QString &message)
{
    QMutexLocker lock(&d->logMutex);
    foreach(LoggerDestination* d, d->destList)
    {
        if (!d)
        {
            assert(!"null log destination");
            continue;
        }

        d->write(message);
    }
}

//! sends the message to all the destinations
void Logger::write(const QString &level, const QString &function, const QString &message)
{
    QString output;

    // Inputbuffer algo by warzone 2100: lib/framework/debug.cpp
    if (d->useInputBuffer && message == d->lastMessage)
    {
        // Received again the same line
        d->repeated++;
        if (d->repeated == d->next)
        {
            if (d->repeated > 2)
            {
                output = QString("last message repeated %1 times (total %2 repeats)")
                                .arg(d->repeated - d->prev).arg(d->repeated);
            }
            else
            {
                output = QString("last message repeated %1 times")
                                .arg(d->repeated - d->prev);
            }
            d->prev = d->repeated;
            d->next *= 2;
        }
    }
    else
    {
        // Received another line, cleanup the old
        if (d->repeated > 0 && d->repeated != d->prev && d->repeated != 1)
        {
            /* just repeat the previous message when only one repeat occurred */
            if (d->repeated > 2)
            {
                output = QString("last message repeated %1 times (total %2 repeats)")
                                .arg(d->repeated - d->prev).arg(d->repeated);
            }
            else
            {
                output = QString("last message repeated %1 times")
                                .arg(d->repeated - d->prev);
            }
        }
        d->repeated = 0;
        d->next = 2;
        d->prev = 0;
    }

    if (!output.isEmpty())
    {
        output = QString("%1 |%2: %3 %4")
            .arg(level, -8)
            .arg(QDateTime::currentDateTime().toString(fmtDateTime))
            .arg(QString("[%1]").arg(function), -25)
            .arg(output);

        QMutexLocker lock(&d->logMutex);
        foreach(LoggerDestination* dest, d->destList)
        {
            if (!dest)
            {
                assert(!"null log destination");
                continue;
            }

            dest->write(output);
        }
    }

    if (!d->repeated)
    {
        d->lastMessage = message;

        output = QString("%1 |%2: %3 %4")
            .arg(level, -8)
            .arg(QDateTime::currentDateTime().toString(fmtDateTime))
            .arg(QString("[%1]").arg(function), -25)
            .arg(message);

        QMutexLocker lock(&d->logMutex);
        foreach(LoggerDestination* dest, d->destList)
        {
            if (!dest)
            {
                assert(!"null log destination");
                continue;
            }

            dest->write(output);
        }
    }
}
