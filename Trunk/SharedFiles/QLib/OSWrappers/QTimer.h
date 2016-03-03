/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (timer)                                 *
 * Module Description: This class implement an asynchronous timer   *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/08/2001                                           *
 * Last upate: 27/08/2001                                           *
 ********************************************************************/

#ifndef _Q_TIMER_H_
#define _Q_TIMER_H_

// OS specific headers
#ifdef OS_VXWORKS
#include <time.h> 
#include <tickLib.h>
#endif

#include <vector>
#include "QOSWrapper.h"
#include "QTypes.h"


// Return the current system time in ticks
#ifdef OS_VXWORKS
#define QGetTicks tickGet
#elif defined(OS_WINDOWS)
#include <windows.h>
#define QGetTicks() static_cast<TQTicks>(GetTickCount())
#endif


// Exception class for QThread class
class EQTimer : public EQOSWrapper {
  public:
    EQTimer(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};


class CQTimer {
  private:
    TGenericCallback m_UserCallback;
    TGenericCockie m_UserCockie;

    // Flag for indicating that a timer operation is currently pending
    bool m_Pending;

    // When this flag is true "Cancel" does nothing
    bool m_ProtectCancel;

#ifdef OS_WINDOWS
    // Windows timer ID
    unsigned m_TimerID;

    static void CALLBACK TimerCallbackRoutine(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);
#elif defined(OS_VXWORKS)
    // OS timer ID
    timer_t m_TimerID;

    // True if the timer initialized Ok
    bool m_CreateStatus;

    static void TimerCallbackRoutine(CQTimer *TimerInstance);
#endif

  public:

    // Constructor
    CQTimer(void);

    // Destructor
    ~CQTimer(void);

    // Wind a timer callback
    void TriggerTimer(unsigned DelayInMS,TGenericCallback UserCallback,TGenericCockie UserCockie);

    // Cancel the timer
    void Cancel(void);

    // Return true if the timer is currently pending
    bool IsPending(void) {
      return m_Pending;
    }
};

#endif
