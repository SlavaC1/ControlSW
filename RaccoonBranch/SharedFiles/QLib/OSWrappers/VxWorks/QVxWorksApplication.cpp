/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Application base class)                *
 * Module Description: VxWorks implementation of the CQApplication  *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/08/2001                                           *
 * Last upate: 27/01/2002                                           *
 ********************************************************************/

#include <vxWorks.h>
#include <sysLib.h>

#include <algorithm>
#include "QThread.h"
#include "QMonitor.h"
#include "QErrors.h"
#include "QApplication.h"


// Constructor
CQApplication::CQApplication(void) : CQComponent("Application")
{
  // Initialize the objects roster
  CQObjectsRoster::Init();

  // Register the QApplication object
  CQObjectsRoster::RegisterComponent(this);

  // Initialize properties and methods
  INIT_VAR_PROPERTY(Terminated,false);
  INIT_VAR_PROPERTY(AppFilePath,"");
  INIT_VAR_PROPERTY(AppFileName,"qapp");
  INIT_VAR_PROPERTY(Initialized,false);
  INIT_VAR_PROPERTY(Started,false);

  GatherSystemInfo();
}

// Destructor
CQApplication::~CQApplication(void)
{
  // DeInitialize the objects roster
  CQObjectsRoster::DeInit();
}

// Gather some information about the system
void CQApplication::GatherSystemInfo(void)
{
}

// Generic callbacks implementation routines for uncatched thread exceptions
void CQApplication::QExceptionCallback(CQThread *ThreadPtr,EQException ExceptionObject,TGenericCockie Cockie)
{
  QMonitor.PrintfNonMaskable("Thread '%s' throw exception: '%s' , (Code %d)",ThreadPtr->Name().c_str(),
                             ExceptionObject.GetErrorMsg().c_str(),ExceptionObject.GetErrorCode());
}

void CQApplication::UnexpectedExceptionCallback(CQThread *ThreadPtr,TGenericCockie Cockie)
{
  QMonitor.PrintfNonMaskable("Thread '%s' throw unexpected exception",ThreadPtr->Name().c_str());
}

// Terminate the application
TQErrCode CQApplication::Terminate(void)
{
  Terminated = true;

  // Stop application before terminating
  Stop();

  // Raise termination flag
  m_TerminateEvent.SetEvent();

  return Q_NO_ERROR;
}

// Wait for the termination of the application
void CQApplication::WaitForTermination(void)
{
  m_TerminateEvent.WaitFor();
}

// A fatal error prints a monitor error and terminate the application
void CQApplication::FatalError(const char *ErrMsg)
{
  QMonitor.ErrorMessage(ErrMsg);
  Terminate();
}

// Initilize the application
bool CQApplication::Init(void)
{
  try
  {
    SystemInit();

    // Call the user initialization method
    AppInit();

    // Update internal property
    Initialized = true;

  } catch(EQException& Error)
    {
      QMonitor.ErrorMessage(Error.GetErrorMsg().c_str());
    }
    catch(...)
    {
      QMonitor.ErrorMessage("Unexpected initialization error");
    }

  return Initialized;
}

// Start the application
bool CQApplication::Start(void)
{
  try
  {
    // Call the user start method
    AppStart();

    // Update internal property
    Started = true;

  } catch(EQException& Error)
    {
      QMonitor.ErrorMessage(Error.GetErrorMsg().c_str());
    }
    catch(...)
    {
      QMonitor.ErrorMessage("Unexpected application start error");
    }

  return Started;
}

// Stop application, return true is stopped Ok
bool CQApplication::Stop(void)
{
  // If not started, (or already stopped) do nothing
  if(!Started)
    return true;

  try
  {
    // Call the user start method
    AppStop();

    Started = false;

  } catch(EQException& Error)
    {
      QMonitor.ErrorMessage(Error.GetErrorMsg().c_str());
    }
    catch(...)
    {
      QMonitor.ErrorMessage("Unexpected application stop error");
    }

  return Started;
}

// Initialize system level related components the
void CQApplication::SystemInit(void)
{
  if(sysClkRateSet(TICKS_PER_SECOND) != OK)
    throw EQApplication("Can not set system timer ticks",errno);
}
