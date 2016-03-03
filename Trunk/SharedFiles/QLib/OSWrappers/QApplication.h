/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Application base class)                *
 * Module Description: Cross platform main application objects.     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/08/2001                                           *
 * Last upate: 27/07/2003                                           *
 ********************************************************************/

#ifndef _Q_APPLICATION_H_
#define _Q_APPLICATION_H_

#include "QComponent.h"
#include "QEvent.h"


// Exception class for CQApplication
class EQApplication : public EQOSWrapper {
  public:
    EQApplication(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};


// Forward declaration for the thread class (avoid headers dependency)
class CQThread;

// Application object (implemented as a singleton with Init/DeInit functions)
class CQApplication : public CQComponent {
  private:
    // Event for application terminate notification
    CQEvent m_TerminateEvent;

    // Gather information about the system
    void GatherSystemInfo(void);

    // Initialize system level related components the
    void SystemInit(void);

    // User init function
    virtual bool AppInit(void) { return true; }

    // User start function
    virtual void AppStart(void) {}

    // User stop function
    virtual void AppStop(void) {}

  public:

    // Constructor
    CQApplication(void);

    // Destructor
    ~CQApplication(void);

    // Initialization function, return true is initialized Ok
    bool Init(void);

    // Start application, return true is started Ok
    bool Start(void);

    // Stop application, return true is stopped Ok
    bool Stop(void);

    // Terminate the application
    DEFINE_METHOD(CQApplication,TQErrCode,Terminate);
	
    // Terminate the application and shutdown the PC
    DEFINE_METHOD(CQApplication,TQErrCode,TerminateAndShutdown);

    // Lunch an external file by its name 
    DEFINE_METHOD_2(CQApplication,TQErrCode,LaunchFile,QString /*FileName*/,QString /*CmdLineParam*/);

    // Compare two version strings -
    //  1 : VerA > VerB
    //  0 : VerA = VerB
    // -1 : VerA < VerB
    DEFINE_METHOD_2(CQApplication,int,CompareVersions,QString /*VerA*/,QString /*VerB*/);

    // Shutdown system (turn off computer)
    DEFINE_METHOD(CQApplication,TQErrCode,SystemShutdown);

    // Give some process time to main application thread (process messages (Windows))
    DEFINE_METHOD(CQApplication,TQErrCode,YieldMainThread);

    // Return true if already terminated
    DEFINE_VAR_PROPERTY(bool,Terminated);

    // Property for the application file path (including the last '\')
    DEFINE_VAR_PROPERTY(QString,AppFilePath);

    // Property for the application file name
    DEFINE_VAR_PROPERTY(QString,AppFileName);

    // Property for the full application version
    DEFINE_VAR_PROPERTY(QString,AppVersion);

    // Property for indicating that the application is initialized Ok
    DEFINE_VAR_PROPERTY(bool,Initialized);

    // Property for indicating that the application has started Ok
    DEFINE_VAR_PROPERTY(bool,Started);

    // The computer (network) name
    DEFINE_VAR_PROPERTY(QString,ComputerName);

    // Wait for the termination of the application
    void WaitForTermination(void);

    // A fatal error prints a monitor error and terminate the application
    void FatalError(const char *ErrMsg);

    // Generic callbacks implementation routines for uncatched thread exceptions
    static void QExceptionCallback(CQThread *ThreadPtr,EQException ExceptionObject,TGenericCockie Cockie);
    static void UnexpectedExceptionCallback(CQThread *ThreadPtr,TGenericCockie Cockie);
};

#endif
