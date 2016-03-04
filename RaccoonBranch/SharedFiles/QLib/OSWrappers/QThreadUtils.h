/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Collection of various utility functions and  *
 *                     declarations related to threads.             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 21/02/2001                                           *
 ********************************************************************/

#ifndef _Q_THREAD_UTILS_H_
#define _Q_THREAD_UTILS_H_

#include "QTypes.h"
#include "QOSWrapper.h"

// Return the current system time in ticks
#ifdef OS_VXWORKS
#define TBD...
#elif defined(OS_WINDOWS)
#include <windows.h>
#define QGetCurrentThreadId GetCurrentThreadId
#endif

// Cross platform definitions of several thread priorities
#ifdef OS_WINDOWS
const int Q_PRIORITY_VERY_LOW  = -2;
const int Q_PRIORITY_LOW       = -1;
const int Q_PRIORITY_NORMAL    =  0;
const int Q_PRIORITY_HIGH      =  1;
const int Q_PRIORITY_VERY_HIGH =  2;
const int Q_PRIORITY_CRITICAL  =  3;
#elif defined(OS_VXWORKS)
const int Q_PRIORITY_VERY_LOW  = 120;
const int Q_PRIORITY_LOW       = 110;
const int Q_PRIORITY_NORMAL    = 100;
const int Q_PRIORITY_HIGH      =  90;
const int Q_PRIORITY_VERY_HIGH =  80;
const int Q_PRIORITY_CRITICAL  =  70;
#endif

// Thread sleep in system ticks
void QSleepTicks(TQWaitTime SleepTime);

// Thread sleep in ms
#define QSleep(SleepTimeInMs) QSleepTicks(QMsToTicks(SleepTimeInMs))

#define QSleepSec(SleepTimeInSec) QSleepTicks(QSecondsToTicks(SleepTimeInSec))
#define QSleepMin(SleepTimeInMin) QSleepTicks(QMinutesToTicks(SleepTimeInMin))

#endif


