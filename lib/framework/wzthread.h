/*
    This file is part of Warzone 2100.
    Copyright (C) 1999-2004  Eidos Interactive
    Copyright (C) 2005-2011  Warzone 2100 Project

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
/** @file wzthread.h
 *  Threading related functions.
 */

#ifndef __INCLUDED_WZTHREAD_H__
#define __INCLUDED_WZTHREAD_H__

// Qt Core
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>

struct _wzThread : public QThread
{
    _wzThread(int (*threadFunc_)(void *), void *data_) : threadFunc(threadFunc_), data(data_) {}
    void run()
    {
        ret = (*threadFunc)(data);
    }
    int (*threadFunc)(void *);
    void *data;
    int ret;
};

// This one couldn't be easier...
struct _wzMutex : public QMutex
{
};

struct _wzSemaphore : public QSemaphore
{
    _wzSemaphore(int startValue = 0) : QSemaphore(startValue) {}
};

typedef struct _wzThread WZ_THREAD;
typedef struct _wzMutex WZ_MUTEX;
typedef struct _wzSemaphore WZ_SEMAPHORE;

WZ_THREAD *wzThreadCreate(int (*threadFunc)(void *), void *data);
int wzThreadJoin(WZ_THREAD *thread);
void wzThreadStart(WZ_THREAD *thread);
bool wzIsThreadDone(WZ_THREAD *thread);
void wzYieldCurrentThread(void);
WZ_MUTEX *wzMutexCreate(void);
void wzMutexDestroy(WZ_MUTEX *mutex);
void wzMutexLock(WZ_MUTEX *mutex);
void wzMutexUnlock(WZ_MUTEX *mutex);
WZ_SEMAPHORE *wzSemaphoreCreate(int startValue);
void wzSemaphoreDestroy(WZ_SEMAPHORE *semaphore);
void wzSemaphoreWait(WZ_SEMAPHORE *semaphore);
void wzSemaphorePost(WZ_SEMAPHORE *semaphore);
int wzSemaphoreAvailable(WZ_SEMAPHORE *semaphore);

#endif // #ifndef __INCLUDED_WZTHREAD_H__