/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (thread)                                *
 * Module Description: This class implement the QThread abstraction *
 *                     for windows.                                 *
 *                                                                  *
 * Compilation: Standard C++, Win32.                                *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 11/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include "QApplication.h"
#include "QErrors.h"
#include "QThread.h"


// Constructor
CQThread::CQThread(bool CreateSuspended,const QString Name,bool RosterRegistration,
                   ULONG StackSize,TThreadOptions ThreadOptions) : CQComponent(Name,RosterRegistration)
{
  // Initialize properties and methods
  INIT_VAR_PROPERTY(Terminated,false);
  INIT_VAR_PROPERTY(Running,false);
  INIT_PROPERTY(CQThread,Priority,SetPriority,GetPriority);
  INIT_METHOD(CQThread,Resume);
  INIT_METHOD(CQThread,Pause);
  INIT_METHOD(CQThread,Suspend);
  INIT_METHOD(CQThread,Terminate);
  INIT_METHOD(CQThread,Kill);
  INIT_METHOD(CQThread,GetThreadID);


  // Init other variables
  m_FreeOnTerminate = false;

  // Connect thread exception callbacks to the QApplication handling routines as default
  m_QExceptionCallback = CQApplication::QExceptionCallback;
  m_UnexpectedExceptionCallback = CQApplication::UnexpectedExceptionCallback;

  // If we are in create suspended mode, add the CREATE_SUSPENDED flag to the thread options
  if(CreateSuspended)
    ThreadOptions |= CREATE_SUSPENDED;

  // Create the physical thread
  m_ThreadHandle = ::CreateThread(NULL,StackSize,(ULONG (__stdcall *)(void *) )CQThread::CreateTask,
                 (LPVOID)this,ThreadOptions,(ULONG *)&m_ThreadID);

  // An error occured
  if(m_ThreadHandle == NULL)
    throw EQThread("Can not create thread " + Name,GetLastError());
}

// Destructor
CQThread::~CQThread(void)
{
  // If the thread is not already terminated, we have to terminate it first
  // before leaving. This class implement two stages termination: first the
  // thread is marked terminated by using the Terminate() function. If after
  // some time the thread is not stopped the Kill() function is called.

  if(Running)
  {
    // Signal termination
    Terminate();

    // Wait some time for termination
    if(WaitFor(WAIT_TIME_BEFORE_KILL) == QLib::wrTimeout)
      // Use brute force
      Kill();
  }
}

// Resume the execution of the thread
int CQThread::Resume(void)
{
  // Resume the thread and test for error
  if(::ResumeThread(m_ThreadHandle) == 0xFFFFFFFF)
    throw EQThread("Can not resume thread " + Name(),GetLastError());

  Running = true;
  return Q_NO_ERROR;
}

// Suspend the execution of the thread
TQErrCode CQThread::Suspend(void)
{
  // If already terminated, do nothing
  if(!Terminated)
  {
    Running = false;

    // Suspend the thread and test for error
    if(::SuspendThread(m_ThreadHandle) == 0xFFFFFFFF)
      throw EQThread("Can not suspend thread " + Name(),GetLastError());
  }

  return Q_NO_ERROR;
}

// pause = suspend a thread at a safe point (not in a middle of any sequence)
TQErrCode CQThread::Pause(void)
{

  m_SyncEventWaitForPausePoint.SetEvent();

  QLib::TQWaitResult WaitResult = m_SyncEventReachedPausePoint.WaitFor();

  if(WaitResult == QLib::wrSignaled)
  {
    //m_SyncEventReachedPausePoint.ResetEvent();
    Suspend();

    m_SyncEventSuspended.SetEvent();
  }

  //the function will exit onlt when the thread is suspended
  return Q_NO_ERROR;
}

// pause point - call this function in a point at which no harm is done if the thread is suspended
void CQThread::PausePoint(ULONG WaitTime)
{
  QLib::TQWaitResult WaitResult = m_SyncEventWaitForPausePoint.WaitFor( WaitTime );

  if(WaitResult == QLib::wrSignaled)
  {
    //m_SyncEventWaitForPausePoint.ResetEvent();
    m_SyncEventReachedPausePoint.SetEvent();

    WaitResult = m_SyncEventSuspended.WaitFor();
  }
}

// Terminate the execution of the thread
TQErrCode CQThread::Terminate(void)
{
  // Raise the termination flag
  Terminated = true; // we don't set Running = false   because the thread does not stop immediately after the call to terminate, and the WaitFor function returns wrSignalled when Running == false.
  return Q_NO_ERROR;
}

// "Kill" the execution of the thread
TQErrCode CQThread::Kill(void)
{
  if(::TerminateThread(m_ThreadHandle,0) == 0)
    throw EQThread("Can not kill thread " + Name(),GetLastError());

  // Update status properties
  Running = false;
  Terminated = true;

  return Q_NO_ERROR;
}

int CQThread::GetThreadID(void)
{
  return m_ThreadID;
}

// The actual thread routine (passed as a function pointer to the create thread API)
long CQThread::CreateTask(PVOID InstancePtr)
{
  CQThread *ThreadPtr = (CQThread *)InstancePtr;

  // Set as running
  ThreadPtr->Running = true;

  try
  {
    // Call the user override version of the Execute() method
    ThreadPtr->Execute();
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    // If an exception handler callback is installed, call it
    if(ThreadPtr->m_QExceptionCallback)
      (*ThreadPtr->m_QExceptionCallback)(ThreadPtr,QException,ThreadPtr->m_QExceptionCallbackCockie);
  }

  // Catch unexpected exceptions
  catch(...)
  {
    // If an undefined exception handler callback is installed, call it
    if(ThreadPtr->m_UnexpectedExceptionCallback)
      (*ThreadPtr->m_UnexpectedExceptionCallback)(ThreadPtr,ThreadPtr->m_UnexpectedExceptionCallbackCockie);
  }

  // Set as not-running and terminated
  ThreadPtr->Running = false;
  ThreadPtr->Terminated = true;

  // Check if we need to delete the thread object
  if(ThreadPtr->m_FreeOnTerminate)
    delete ThreadPtr;

  return 0L;
}

// Get the thread priority
int CQThread::GetPriority(void)
{
  // Query the thread priority
  int ThreadPriority = ::GetThreadPriority(m_ThreadHandle);

  // Check if error
  if(ThreadPriority == THREAD_PRIORITY_ERROR_RETURN)
    throw EQThread("Can not get thread \"" + Name() + "\"",GetLastError());

  return ThreadPriority;
}

// Set the thread priority
void CQThread::SetPriority(int NewPriority)
{
  // Set new thread priority
  if(::SetThreadPriority(m_ThreadHandle,NewPriority) == 0)
    throw EQThread("Can not set thread \"" + Name() + "\" priority",GetLastError());
}

// Wait for the thread completion
QLib::TQWaitResult CQThread::WaitFor(TQWaitTime Timeout)
{
  QLib::TQWaitResult Result;

  // If the thread is not running, don't wait at all
  if(!Running)
	return QLib::wrSignaled;

  switch(::WaitForSingleObject(m_ThreadHandle,Timeout))
  {
    case WAIT_OBJECT_0:
	  Result = QLib::wrSignaled;
      break;

    case WAIT_TIMEOUT:
	  Result = QLib::wrTimeout;
      break;

    case WAIT_FAILED:
    case WAIT_ABANDONED:
        Result = QLib::wrError;
  }

  return Result;
}

