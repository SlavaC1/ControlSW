/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (thread)                                *
 * Module Description: This class implement the QThread abstraction *
 *                     for VxWorks.                                 *
 *                                                                  *
 * Compilation: Standard C++, VxWorks.                              *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 11/07/2001                                           *
 * Last upate: 10/09/2001                                           *
 ********************************************************************/

#include <taskLib.h>
#include "QApplication.h"
#include "QThreadUtils.h"
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
  INIT_METHOD(CQThread,Suspend);
  INIT_METHOD(CQThread,Terminate);
  INIT_METHOD(CQThread,Kill);

  // Init other variables
  m_FreeOnTerminate = false;

  // Connect thread exception callbacks to the QApplication handling routines as default
  m_QExceptionCallback = CQApplication::QExceptionCallback;
  m_UnexpectedExceptionCallback = CQApplication::UnexpectedExceptionCallback;

  // If we are not in "create suspended" mode, set the running flag before the task spawn
  if(!CreateSuspended )
    Running = true;

  // Create the physical thread
  m_ThreadHandle = taskSpawn(const_cast<char *>(Name.c_str()),Q_PRIORITY_NORMAL,ThreadOptions,StackSize,(FUNCPTR)CreateTask,
                             (int)CreateSuspended,(int)this,0,0,0,0,0,0,0,0);


  // Check if error
  if(m_ThreadHandle == ERROR)
    throw EQThread("Can not create thread " + Name,errno);
}

// Destructor
CQThread::~CQThread(void)
{
  if(m_ThreadHandle != ERROR)
  {
    // If the thread is not already terminated, we have to terminate it first
    // before leaving. This class implement two stages termination: first the
    // thread is marked terminated by using the Terminate() function. If after
    // some time the thread is not stopped the Kill() function is called.

    if(!Terminated)
    {
      // Signal termination
      Terminate();

      // Wait some time for termination
      if(WaitFor(WAIT_TIME_BEFORE_KILL) == wrTimeout)
      {
        // Use brute force
        Kill();
        return;
      }
    }

    // Delete the task before leaving
    taskDelete(m_ThreadHandle);
  }
}

// Resume the execution of the thread
TQErrCode CQThread::Resume(void)
{
  Running = true;

  // Resume the thread and test for error
  if(taskResume(m_ThreadHandle) == ERROR)
    throw EQThread("Can not resume thread " + Name(),errno);

  return Q_NO_ERROR;
}

// Suspend the execution of the thread
TQErrCode CQThread::Suspend(void)
{
  Running = false;
  
  // Suspend the thread and test for error
  if(taskSuspend(m_ThreadHandle) == ERROR)
    throw EQThread("Can not suspend thread " + Name(),errno);

  return Q_NO_ERROR;
}

// Terminate the execution of the thread
TQErrCode CQThread::Terminate(void)
{
  // Raise the termination flag
  Terminated = true;
  return Q_NO_ERROR;
}

// "Kill" the execution of the thread
TQErrCode CQThread::Kill(void)
{
  // Brute force termination
  if(taskDeleteForce(m_ThreadHandle) == ERROR)
    throw EQThread("Can not kill thread " + Name(),errno);

  // Update status properties
  Running = false;
  Terminated = true;

  return Q_NO_ERROR;
}

// The actual thread routine (passed as a function pointer to the create thread API)
int CQThread::CreateTask(int CreateSuspended,int InstancePtr)
{
  CQThread *ThreadPtr = (CQThread *)InstancePtr;

  try
  {
    if(CreateSuspended && !ThreadPtr->Running)
      ThreadPtr->Suspend();

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

  // Trigger the termination event
  ThreadPtr->m_TerminationEvent.SetEvent();

  // Check if we need to delete the thread object
  if(ThreadPtr->m_FreeOnTerminate)
    delete ThreadPtr;

  return 0;
}

// Get the thread priority
int CQThread::GetPriority(void)
{
  int ThreadPriority;

  // Query the thread priority and check if error
  if(taskPriorityGet(m_ThreadHandle,&ThreadPriority) == ERROR)
    throw EQThread("Can not get thread \"" + Name() + "\" priority",errno);

  return ThreadPriority;
}

// Set the thread priority
void CQThread::SetPriority(int NewPriority)
{
  // Set new thread priority
  if(taskPrioritySet(m_ThreadHandle,NewPriority) == ERROR)
    throw EQThread("Can not set thread \"" + Name() + "\" priority",errno);
}

// Wait for thread completion
TQWaitResult CQThread::WaitFor(TQWaitTime Timeout)
{
  // If the thread is not running, don't wait at all
  if(!Running)
    return wrSignaled;

  return m_TerminationEvent.WaitFor(Timeout);
}

