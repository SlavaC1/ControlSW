/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: This header define OS related constants and  *
 *                     types.                                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/07/2001                                           *
 * Last upate: 22/08/2001                                           * 
 ********************************************************************/

#ifndef _Q_OS_WRAPPER_H_
#define _Q_OS_WRAPPER_H_

#include "QException.h"
#include "QTypes.h"

// Base exception class for all the OS wrappers classes
class EQOSWrapper : public EQException {
  public:
    EQOSWrapper(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Wait time type and Infinite time constants
#ifdef OS_WINDOWS
  typedef ULONG TQWaitTime;
  const ULONG Q_INFINITE_TIME = 0xFFFFFFFF;
#elif defined(OS_VXWORKS)
  typedef int TQWaitTime;
  const int Q_INFINITE_TIME = WAIT_FOREVER;
#endif

// Conversion functionf from various time units to machine ticks and vis-versa
#ifdef OS_WINDOWS

// Time units to ticks
#define QMsToTicks(x)      static_cast<TQTicks>(x)
#define QSecondsToTicks(x) (QMsToTicks(x) * 1000)
#define QMinutesToTicks(x) (QSecondsToTicks(x) * 60)
#define QHoursToTicks(x)   (QMinutesToTicks(x) * 60)
#define QDaysToTicks(x)    (QHoursToTicks(x) * 24)

// Ticks to time units
#define QTicksToMs(x)      static_cast<TQMilliseconds>(x)
#define QTicksToSeconds(x) (QTicksToMs(x) / 1000)
#define QTicksToMinutes(x) (QTicksToSeconds(x) / 60)
#define QTicksToHours(x)   (QTicksToMinutes(x) / 60)
#define QTicksToDays(x)    (QTicksToHours(x) / 24)

#elif defined(OS_VXWORKS)

// In VxWorks we need to set the number of ticks per second
#define TICKS_PER_SECOND 100

// Time units to ticks
#define QMsToTicks(x)      ((x) / (1000 / TICKS_PER_SECOND))
#define QSecondsToTicks(x) (QMsToTicks(x) * 1000)
#define QMinutesToTicks(x) (QSecondsToTicks(x) * 60)
#define QHoursToTicks(x)   (QMinutesToTicks(x) * 60)
#define QDaysToTicks(x)    (QHoursToTicks(x) * 24)

// Ticks to time units
#define QTicksToMs(x)      ((x) * (1000 / TICKS_PER_SECOND))
#define QTicksToSeconds(x) (QTicksToMs(x) / 1000)
#define QTicksToMinutes(x) (QTicksToSeconds(x) / 60)
#define QTicksToHours(x)   (QTicksToMinutes(x) / 60)
#define QTicksToDays(x)    (QTicksToHours(x) / 24)

#endif

namespace QLib
{
// Type for the synchronizations objects wait result
typedef enum {wrSignaled,wrTimeout,wrError,wrReleased} TQWaitResult;
};

// Operating system error code type
#ifdef OS_WINDOWS
typedef long TOSErr;
#elif defined(OS_VXWORKS)
typedef STATUS TOSErr;
#endif

#endif




