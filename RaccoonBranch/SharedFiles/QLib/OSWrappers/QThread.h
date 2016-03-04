/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (thread)                                *
 * Module Description: This class implement a simple cross-platform *
 *                     abstraction to thread (or a task).           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 11/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _Q_THREAD_H_
#define _Q_THREAD_H_

// OS specific headers
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include "QComponent.h"
#include "QProperty.h"
#include "QMethod.h"
#include "QOSWrapper.h"
#include "QEvent.h"


// Platform dependant constants and types
#ifdef OS_WINDOWS

const ULONG DEFAULT_THREAD_STACK_SIZE = 0L;   // In windows 0 means default stack
const ULONG DEFAULT_THREAD_OPTIONS = 0L;

#elif defined(OS_VXWORKS)

const ULONG DEFAULT_THREAD_STACK_SIZE = 16384L;
const ULONG DEFAULT_THREAD_OPTIONS = VX_FP_TASK;   // Floating-point support by default
#endif

// This constant define the time in ms before the thread will be killed. See destructor comment
// for further details.
const TQWaitTime WAIT_TIME_BEFORE_KILL = 5000;

// This type is used for options
typedef ULONG TThreadOptions;

// Exception class for QThread class
class EQThread : public EQOSWrapper {
  public:
    EQThread(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Forward for class CQThread
class CQThread;

// This type is used for QThread un-catched QException callback
typedef void (*TQExceptionCallback)(CQThread *ThreadPtr,EQException ExceptionObject,TGenericCockie Cockie);

// This type is used for QThread un-catched un-expected generic exceptions callback
typedef void (*TUnexpectedExceptionCallback)(CQThread *ThreadPtr,TGenericCockie Cockie);

// Define thread handle type
#ifdef OS_WINDOWS
typedef HANDLE TThreadHandle;
#elif defined(OS_VXWORKS)
typedef int TThreadHandle;
#endif

// A thread is also a component
class CQThread : public CQComponent {
  private:

    // Exception handler callbacks and their associated cockies
    TQExceptionCallback m_QExceptionCallback;
    TUnexpectedExceptionCallback m_UnexpectedExceptionCallback;

    // Cockies for the above callbacks
    TGenericCockie m_QExceptionCallbackCockie;
    TGenericCockie m_UnexpectedExceptionCallbackCockie;

    // If this flag is true the thread object will be deleted when the thread terminates
    bool m_FreeOnTerminate;

    // Thread OS handle
    TThreadHandle m_ThreadHandle;

    // event for synchronizing the pause
    CQEvent m_SyncEventWaitForPausePoint;

    // event for signaling the arrival to pause point
    CQEvent m_SyncEventReachedPausePoint;

    // event for signaling suspend
    CQEvent m_SyncEventSuspended;

#ifdef OS_WINDOWS
    // In windows each thread has an handle and an ID
    ULONG m_ThreadID;

    // Thread creation function
    static long CreateTask(PVOID InstancePtr);

#elif defined(OS_VXWORKS)
    // In VxWorks we can not wait directly for termination of threads, so additional event object
    // is required.
    CQEvent m_TerminationEvent;

    // Thread creation function
    static int CreateTask(int CreateSuspended,int InstancePtr);
#endif

    // Get the thread priority
    int GetPriority(void);

    // Set the thread priority
    void SetPriority(int NewPriority);

  protected:

    // Override this function to implement a costum thread class
    virtual void Execute(void) = 0;

    // pause point - call this function in a point at which no harm is done if the thread is suspended
    virtual void PausePoint(ULONG WaitTime);

  public:
    // Constructor
    CQThread(bool CreateSuspended = false,                           // If true the thread is created in suspend mode
             const QString Name = "Thread",                          // Thread component name.
             bool RosterRegistration = true,                         // Register the thread as a component in
                                                                     // the objects roster.
             ULONG StackSize = DEFAULT_THREAD_STACK_SIZE,            // Stack size.
             TThreadOptions ThreadOptions = DEFAULT_THREAD_OPTIONS); // Additional options (OS dependent).

    // Destructor
    virtual ~CQThread(void);

    // Property for indicating that the thread is terminated
    DEFINE_VAR_PROPERTY(bool,Terminated);

    // Property for indicating that the thread is running
    DEFINE_VAR_PROPERTY(bool,Running);

    // Property for the thread priority
    DEFINE_PROPERTY(CQThread,int,Priority);

    // Resume the execution of the thread
    DEFINE_METHOD(CQThread,TQErrCode,Resume);

    // Pause = suspend a thread at a safe point (not in a middle of any sequence)
    DEFINE_METHOD(CQThread,TQErrCode,Pause);

    // Suspend the execution of the thread
    DEFINE_METHOD(CQThread,TQErrCode,Suspend);

    // Terminate the execution of the thread
    DEFINE_METHOD(CQThread,TQErrCode,Terminate);

    // "Kill" the execution of the thread
    DEFINE_METHOD(CQThread,TQErrCode,Kill);

    // Get the ID of the Thread.
    DEFINE_METHOD(CQThread,int,GetThreadID);

    // If TerminateMode is true, the thread object will be deleted when the thread terminates
    void FreeOnTerminate(bool TerminateMode) {
      m_FreeOnTerminate = TerminateMode;
    }

    // Wait for the thread completion
    QLib::TQWaitResult WaitFor(TQWaitTime Timeout = Q_INFINITE_TIME);

    // Set exception handler callbacks for handling thread uncatched exceptions
    void SetExceptionCallbacks(TQExceptionCallback QExceptionCallback,
                               TGenericCockie QExceptionCallbackCockie = 0,
                               TUnexpectedExceptionCallback UnexpectedExceptionCallback = 0,
                               TGenericCockie UnexpectedExceptionCallbackCockie = 0) {
      // Remember handlers in class members
      m_QExceptionCallback = QExceptionCallback;
      m_QExceptionCallbackCockie = QExceptionCallbackCockie;
      m_UnexpectedExceptionCallback = UnexpectedExceptionCallback;
      m_UnexpectedExceptionCallbackCockie = UnexpectedExceptionCallbackCockie;
    }

    // Get an handle to the OS object
    TThreadHandle Handle(void) {
      return m_ThreadHandle;
    }
};

#endif

