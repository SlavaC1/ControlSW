/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Mobitor)                               *
 * Module Description: Cross platform simple monitor services.      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 12/08/2001                                           *
 ********************************************************************/

#ifndef _Q_MONITOR_H_
#define _Q_MONITOR_H_

#include "QComponent.h"
#include "QOSWrapper.h"


// Exception class for all the QMonitor
class EQMonitor : public EQOSWrapper {
  public:
    EQMonitor(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Writing to application log Monitor messages 
typedef void (*TMonitorErrorLoggingCallback)(QString Message);

// Used for the IsMessageSafe() method to communicate with / service the front-end
typedef bool (*DelegateIsWizardRunning)(void);
typedef void (*DelegateNotifyWizardUnsafe)(QString Message);

// Where does the call originates from - within a wizard's UI or other  
typedef enum {
	ORIGIN_WIZARD_PAGE = 0,
	ORIGIN_OTHER_MODULE = 1
} TQCallerOrigin;

// Monitor class
class CQMonitor : public CQComponent {
  private:
    // When mask is true, no monitor prints will be displayed
    bool m_Mask;

	DelegateIsWizardRunning m_IsWizardRunning;
	DelegateNotifyWizardUnsafe m_NotifyWizardUnsafe;

#ifdef OS_WINDOWS
    bool m_Visible;
#endif

    // Set/Get for the print mask property
    void SetMask(bool Mask) {
      m_Mask = Mask;
    }
    bool GetMask(void) {
      return m_Mask;
    }

    // Set/Get for the StrOut mask property
    void SetStrOut(QString Str) {
      Print(Str.c_str());
    }
    QString GetStrOut(void) {
      return "";
	}

	TMonitorErrorLoggingCallback m_MonitorErrorLoggingCallback;

  private:
	// Checks whether it is okay to display the message on GUI.
	// Displaying it unexpectedly while a wizard is running can pause/block the
	// wizard's logic and cause a bug.
	bool IsMessageSafe(QString Msg, int MsgType, TQCallerOrigin CallerId) const;

  public:
    // Constructor
    CQMonitor(void);

	// Used for the IsMessageSafe() method to communicate with / service the front-end
	void CQMonitor::SetServiceCallsCheckingMsgSafety(
		DelegateIsWizardRunning, DelegateNotifyWizardUnsafe);

    // Simple output to the monitor (non-formatted)
    void Print(const QString Str,bool Maskable = true);

    // Simple output to the monitor (non-formatted)
    void Print(const char *Str,bool Maskable = true);

    // Formatted output to the monitor (always maskable)
    void Printf(const char *FormatStr,...);

    // Formatted output to the monitor (non maskable)
    void PrintfNonMaskable(const char *FormatStr,...);

    // Mask property
    DEFINE_PROPERTY(CQMonitor,bool,Mask);

    // Alternative way for monitor prints: an string assignment to this property prints the string
    // on the monitor.
    DEFINE_PROPERTY(CQMonitor,QString,StrOut);

    // Print an error message
	// Intended calls from within a Wizard must set the correct TQCallerOrigin
	void ErrorMessage(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeErrorMessage,QString,int);

    // Print a warning message
	// Intended calls from within a Wizard must set the correct TQCallerOrigin
	void WarningMessage(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeWarningMessage,QString,int /*Called from a Wizard*/);

    // Print a notification message
	// Intended calls from within a Wizard must set the correct TQCallerOrigin
	void NotificationMessage(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeNotificationMessage,QString,int /*Called from a Wizard*/);

    // Print an error message and wait for user approve
	// Intended calls from within a Wizard must set the correct TQCallerOrigin
	void ErrorMessageWaitOk(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeErrorMessageWaitOk,QString,int /*Called from a Wizard*/);

    // Print a warning message and wait for user approve
	// Intended calls from within a Wizard must set the correct TQCallerOrigin
	void WarningMessageWaitOk(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeWarningMessageWaitOk,QString,int /*Called from a Wizard*/);

    // Print a notification message and wait for user approve
	// Intended calls from within a Wizard must set the correct TQCallerOrigin
	void NotificationMessageWaitOk(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	void NotificationMessageWait(QString Str, TQCallerOrigin Sender = ORIGIN_OTHER_MODULE);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeNotificationMessageWaitOk,QString,int /*Called from a Wizard*/);
	DEFINE_METHOD_2(CQMonitor,TQErrCode,SafeNotificationMessageWait,QString,int /*Called from a Wizard*/);

    // Request a string from the user
    DEFINE_METHOD_1(CQMonitor,QString,GetString,QString);
	
	// Request a string from the user
	DEFINE_METHOD_1(CQMonitor,QString,GetNonEmptyString,QString);

    // Ask the user a yes/no question (retrun true if yes)
    DEFINE_METHOD_1(CQMonitor,bool,AskYesNo,QString);

    // Ask the user a ok/cancel question (retrun true if yes)
    DEFINE_METHOD_1(CQMonitor,bool,AskOKCancel,QString);

    // Get a single character from the monitor
    char GetChar(void);

    // Request a number from the user (integer version)
    bool GetNumber(const char *Prompt,int& Number);

    // Request a number from the user (float version)
    bool GetNumber(const char *Prompt,double& Number);

#ifdef OS_WINDOWS
    // Set the visibility of the monitor form
    void SetVisible(bool Visible);
#endif

	void SetMonitorErrorLoggingCallback(TMonitorErrorLoggingCallback MonitorErrorLoggingCallback);
};

// The monitor implemented as a global object
extern CQMonitor QMonitor;

#endif
