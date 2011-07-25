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
/** @file wzthread.cpp
 *  Threading related functions.
 */

// Self
#include <lib/framework/wzthread.h>

// Framework
#include "lib/framework/frame.h"

WZ_THREAD *wzThreadCreate(int (*threadFunc)(void *), void *data)
{
    return new WZ_THREAD(threadFunc, data);
}

int wzThreadJoin(WZ_THREAD *thread)
{
    thread->wait();
    int ret = thread->ret;
    delete thread;
    return ret;
}

void wzThreadStart(WZ_THREAD *thread)
{
    thread->start();
}

bool wzIsThreadDone(WZ_THREAD *thread)
{
    return thread->isFinished();
}

void wzYieldCurrentThread()
{
#if QT_VERSION >= 0x040500
    QThread::yieldCurrentThread();
#endif
}

WZ_MUTEX *wzMutexCreate()
{
    return new WZ_MUTEX;
}

void wzMutexDestroy(WZ_MUTEX *mutex)
{
    delete mutex;
}

void wzMutexLock(WZ_MUTEX *mutex)
{
    mutex->lock();
}

void wzMutexUnlock(WZ_MUTEX *mutex)
{
    mutex->unlock();
}

WZ_SEMAPHORE *wzSemaphoreCreate(int startValue)
{
    return new WZ_SEMAPHORE(startValue);
}

void wzSemaphoreDestroy(WZ_SEMAPHORE *semaphore)
{
    delete semaphore;
}

void wzSemaphoreWait(WZ_SEMAPHORE *semaphore)
{
    semaphore->acquire();
}

void wzSemaphorePost(WZ_SEMAPHORE *semaphore)
{
    semaphore->release();
}

int wzSemaphoreAvailable(WZ_SEMAPHORE *semaphore)
{
    return semaphore->available();
}