/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Application base class)                *
 * Module Description: Windows implementation of the CQApplication  *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Standard C++, BCB                                   *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/08/2001                                           *
 * Last upate: 27/10/2003                                           *
 ********************************************************************/

 // In windows, this module must be compiled with C++ builder
#ifndef __BCPLUSPLUS__
#error QLib: The QApplication module must be compiled with Borland C++ Builder
#endif

// The monitor dialog is initialized when the QApplication is initialized
#include "QMonitorDlg.h"

#include <algorithm>
#include "QThread.h"
#include "QMonitor.h"
#include "QErrors.h"
#include "QUtils.h"
#include "QApplication.h"
#include "QVersionInfo.h"


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
  INIT_VAR_PROPERTY(AppFileName,"");
  INIT_VAR_PROPERTY(Initialized,false);
  INIT_VAR_PROPERTY(Started,false);
  INIT_VAR_PROPERTY(ComputerName,"");
  INIT_VAR_PROPERTY(AppVersion,QGetAppVersionStr());

  INIT_METHOD(CQApplication,Terminate);
  INIT_METHOD(CQApplication,TerminateAndShutdown);
  INIT_METHOD(CQApplication,LaunchFile);
  INIT_METHOD(CQApplication,CompareVersions);
  INIT_METHOD(CQApplication,SystemShutdown);
  INIT_METHOD(CQApplication,YieldMainThread);

  GatherSystemInfo();
}

// Destructor
CQApplication::~CQApplication(void)
{
  // If not already terminated, terminate the application
  if(!Terminated)
    Terminate();

  // DeInitialize the objects roster
  CQObjectsRoster::DeInit();
}

// Wait for the termination of the application
void CQApplication::WaitForTermination(void)
{
  m_TerminateEvent.WaitFor();
}

// Gather some information about the system
void CQApplication::GatherSystemInfo(void)
{
  // Get the exe file path (including the last '\')
  AppFilePath.Value() = QExtractFilePath(Application->ExeName.c_str());

  // Get the exe file name
  AppFileName.Value() = QExtractFileNameWithoutExt(Application->ExeName.c_str());

  char CompNameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
  ULONG CompNameBufferLength = sizeof(CompNameBuffer);

  // Get computer name
  if(::GetComputerName(CompNameBuffer,&CompNameBufferLength) != 0)
  {
    CompNameBuffer[CompNameBufferLength] = NULL;

    // Save in the property variable
    ComputerName.Value() = CompNameBuffer;
  } else
      ComputerName.Value() = "__Unknown__"; 
}
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
// Generic callbacks implementation routines for uncatched thread exceptions
void CQApplication::QExceptionCallback(CQThread *ThreadPtr,EQException ExceptionObject,TGenericCockie Cockie)
{
  QMonitor.ErrorMessage(QFormatStr("Thread '%s' throw exception: '%s' , (Code %d)",ThreadPtr->Name().c_str(),
                        ExceptionObject.GetErrorMsg().c_str(),ExceptionObject.GetErrorCode()));
}

void CQApplication::UnexpectedExceptionCallback(CQThread *ThreadPtr,TGenericCockie Cockie)
{
  QMonitor.ErrorMessage(QFormatStr("Thread '%s' threw an unexpected exception",ThreadPtr->Name().c_str()));
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
// Terminate the application and shutdown the PC
TQErrCode CQApplication::TerminateAndShutdown()
{
  Terminated = true;

  // Stop application before terminating
  Stop();

  // Raise termination flag
  m_TerminateEvent.SetEvent();

  // Terminate the application
  Application->Terminate();

  // Shutdown the PC
  SystemShutdown();

  return Q_NO_ERROR;
}

// Terminate the application
TQErrCode CQApplication::Terminate(void)
{
  Terminated = true;

  // Stop application before terminating
  Stop();

  // Raise termination flag
  m_TerminateEvent.SetEvent();

  // Terminate the application
  Application->Terminate();

  return Q_NO_ERROR;
}

TQErrCode CQApplication::LaunchFile(QString FileName,QString CmdLineParam)
{
  int OSErr = (int)::ShellExecute(Application->Handle,"open" ,FileName.c_str(),CmdLineParam.c_str(),NULL,SW_NORMAL);

  TQErrCode QErr = Q_NO_ERROR;

  if(OSErr <= 32)
  {
    switch(OSErr)
    {
      case ERROR_FILE_NOT_FOUND:
      case ERROR_PATH_NOT_FOUND:
        QErr = Q_FILE_NOT_FOUND_ERR;
        break;

      default:
        QErr = Q_OS_ERR;
    }
  }

  return QErr;
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

    // Register the monitor in the objects roster
    CQObjectsRoster::Instance()->RegisterComponent(&QMonitor);

    // Call the user initialization method and Update internal properties
    Initialized = AppInit();

  } catch(EQException& Error)
    {
      QMonitor.ErrorMessageWaitOk(Error.GetErrorMsg().c_str());
    }
    catch(...)
    {
      QMonitor.ErrorMessageWaitOk("Unexpected initialization error");
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

// Initialize system level related components
void CQApplication::SystemInit()
{
  // Initialize the QMonitor form when the QApplication is initialized
  TQMonitorForm::Init();
}

// Compare two version strings -
// > 0 : VerA > VerB
//   0 : VerA = VerB
// < 0 : VerA < VerB
int CQApplication::CompareVersions(QString VerA,QString VerB)
{
  // Check minimum version strings length
  if((VerA.length() != 7) || (VerB.length() != 7))
    throw EQException("Invalid version string length");

  int VerADigits[4],VerBDigits[4];

  if(sscanf(VerA.c_str(),"%d.%d.%d.%d",VerADigits + 3,VerADigits + 2,VerADigits + 1,VerADigits) != 4)
    throw EQException("Invalid version string \"" + VerA + "\"");

  if(sscanf(VerB.c_str(),"%d.%d.%d.%d",VerBDigits + 3,VerBDigits + 2,VerBDigits + 1,VerBDigits) != 4)
    throw EQException("Invalid version string \"" + VerB + "\"");

  int DigitsLookup[4] = {1,10,100,1000};
  int NumA = 0,NumB = 0;

  // Create two integer numbers
  for(int i = 0; i < 4; i++)
  {
    NumA += VerADigits[i] * DigitsLookup[i];
    NumB += VerBDigits[i] * DigitsLookup[i];
  }

  return (NumA - NumB);
}

// Shutdown system (turn off computer)
TQErrCode CQApplication::SystemShutdown(void)
{
  HANDLE hProc;
  HANDLE hToken;
  LUID mLUID;
  TOKEN_PRIVILEGES mPriv;
  TOKEN_PRIVILEGES mNewPriv;
  ULONG RetLength;

  hProc = ::GetCurrentProcess();
  ::OpenProcessToken(hProc,TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken);
  ::LookupPrivilegeValue("","SeShutdownPrivilege",&mLUID);
  mPriv.PrivilegeCount = 1;
  mPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  mPriv.Privileges[0].Luid = mLUID;
  
  if(!::AdjustTokenPrivileges(hToken,0,&mPriv,4 + (12 * mPriv.PrivilegeCount),&mNewPriv,&RetLength))
    throw EQApplication(QFormatStr("Can not get privilege token (Error %d)",GetLastError()));

  if(!::ExitWindowsEx(EWX_FORCE | EWX_POWEROFF,0))
    throw EQApplication(QFormatStr("Can not shutdown windows (Error %d)",GetLastError()));

  return Q_NO_ERROR;  
}

// Give some process time to main application thread (process messages (Windows))
TQErrCode CQApplication::YieldMainThread(void)
{
  Application->ProcessMessages();
  return Q_NO_ERROR;
}
