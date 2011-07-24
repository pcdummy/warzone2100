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

#ifndef WZLOG_LOGGER_H
#define WZLOG_LOGGER_H

#include "loggerdest.h"

#include <QtCore/QString>
#include <QtCore/QtDebug>

// This MUST be in the same order as in the LoggerImpl initalizer!
enum LEVELS {
    LOG_ALL = 0,        // Will automaticaly enable levels higher than this.
    LOG_TRACE,          // -""-, but not custom levels.
    LOG_DEBUG,          // -""-
    LOG_INFO,           // -""-, enabled by default.
    LOG_WARNING,           // -""-, enabled by default.
    LOG_ERROR,          // -""-, enabled by default.
    LOG_FATAL,          // -""-, enabled by default.
    LOG_NEVER,          // Special: Needs to be last for setLevelStatus.
    LOG_POPUP           // Special: Shows a popup window
};

class LoggerImpl; // d pointer
class Logger
{
public:
    static Logger& instance()
    {
        static Logger staticLog;
        return staticLog;
    }

    //! Adds a log message destination. Don't add null destinations.
    void addDestination(LoggerDestination* destination);

    //! Dynamicaly add another level.
    int addLoggingLevel(const QString& name, bool status = false);
    //! Return the identifier of a level
    int getLoggingLevel(const QString& name);

    //! Enable/disable logging for this level.
    bool setLevelStatus(int level, bool status = false);
    bool setLevelStatus(const QString& name, bool status = false);

    //! Returns if the given level is enabled
    bool checkLevel(int level);

    //! Enable/disable the input buffer
    void setInputBuffer(bool state);

    //! Force flush on every message on the destinations.
    void setAlwaysFlush(bool state);

    //! Set the popups title and message body, the body should accept one parameter ("%1")
    void setPopupTitleBody(const char* title, const char* body);
    void setFatalTitleBody(const char* title, const char* body);

    //! Direct access to the logdestinations (for traces as example).
    void write(const QString &message);

    //! The helper forwards the streaming to QDebug and builds the final
    //! log message.
    class Helper
    {
    public:
        explicit Helper(int logLevel, const char* callerFunc) :
            level(logLevel),
            function(callerFunc),
            qtDebug(&buffer) {}
        ~Helper();
        QDebug& stream(){ return qtDebug; }
        void sprintfMe(const char* cformat, ...);

    private:
        void writeToLog();

        int level;
        QString function;
        int line;
        QString buffer;
        QDebug qtDebug;
    };

private:
   Logger();
   Logger(const Logger&);
   Logger& operator=(const Logger&);
   ~Logger();

   void write(const QString &level, const QString &function, const QString &message);

   LoggerImpl* d;
}; // class Logger

#endif // WZLOG_LOGGER_H
