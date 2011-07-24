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

// self
#include "loggerdest.h"

// QT
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// Platform specific console output
#if defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(Q_OS_UNIX)
#include <cstdio>
#endif

LoggerFileDestination::LoggerFileDestination(const QString& filePath)
{
    mFile.setFileName(filePath);
    mFile.open(QFile::WriteOnly | QFile::Text); //fixme: should throw on failure
    mOutputStream.setDevice(&mFile);
}

void LoggerFileDestination::write(const QString& message)
{
    mOutputStream << message << endl;
    if (alwaysFlush)
    {
        mOutputStream.flush();
    }
}



#if defined(Q_OS_WIN)
void LoggerWinDebugDestition::write(const QString& message )
{
    OutputDebugStringW(reinterpret_cast<const WCHAR*>(message.utf16()));
    OutputDebugStringW(L"\n");
}
#endif

void LoggerDebugOutputDestination::write(const QString& message )
{
    fprintf(stderr, "%s\n", qPrintable(message));
    if (alwaysFlush)
    {
        fflush(stderr);
    }
}