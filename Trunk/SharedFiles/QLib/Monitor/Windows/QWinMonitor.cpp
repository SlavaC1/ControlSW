/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Monitor)                               *
 * Module Description: Windows implementation for the CQMonitor     *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Standard C++, Win32                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 25/06/2003                                           *
 ********************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include "QMonitorDlg.h"
#include "QErrors.h"
#include "QMonitor.h"


// The monitor implemented as a global object
CQMonitor QMonitor;

// The maximum size of a formatted print string
const int MAX_FORMATTED_PRINT_SIZE = 128;


// Constructor
CQMonitor::CQMonitor(void) : CQComponent("Monitor")
{
  // Initialize properties and methods
  INIT_METHOD(CQMonitor,SafeErrorMessage);
  INIT_METHOD(CQMonitor,SafeWarningMessage);
  INIT_METHOD(CQMonitor,SafeNotificationMessage);
  INIT_METHOD(CQMonitor,SafeErrorMessageWaitOk);
  INIT_METHOD(CQMonitor,SafeWarningMessageWaitOk);
  INIT_METHOD(CQMonitor,SafeNotificationMessageWaitOk);
  INIT_METHOD(CQMonitor,SafeNotificationMessageWait);
  INIT_METHOD(CQMonitor,GetNonEmptyString);

  INIT_METHOD(CQMonitor,GetString);
  INIT_METHOD(CQMonitor,AskYesNo);
  INIT_METHOD(CQMonitor,AskOKCancel);
  INIT_PROPERTY(CQMonitor,Mask,SetMask,GetMask);
  INIT_PROPERTY(CQMonitor,StrOut,SetStrOut,GetStrOut);

  m_Mask = false;
  m_Visible = true;

  m_MonitorErrorLoggingCallback = NULL;
  m_IsWizardRunning    = 0;
  m_NotifyWizardUnsafe = 0;
}
void CQMonitor::SetServiceCallsCheckingMsgSafety(
	DelegateIsWizardRunning IsWizardRunningFunc,
	DelegateNotifyWizardUnsafe NotifyWizardUnsafeFunc)
{
  m_IsWizardRunning    = IsWizardRunningFunc;
  m_NotifyWizardUnsafe = NotifyWizardUnsafeFunc;
}


// Simple output to the monitor (non-formatted)
void CQMonitor::Print(const QString Str,bool Maskable)
{
  // If the print request is non-maskable, always print
  if(!Maskable || !m_Mask)
  {
#ifdef USE_STDIO_MONITOR
    printf("%s\n",Str.c_str());
#else
    TQMonitorForm *MonForm = TQMonitorForm::Instance();

    if(MonForm)
    {
      // Set the form to be visible if currently not visible
      if(m_Visible)
      {
        if(!MonForm->Visible)
          MonForm->SetVisible(true);
      }

      MonForm->WriteToMonitor(Str);
    }
#endif    
  }
}

// Simple output to the monitor (non-formatted)
void CQMonitor::Print(const char *Str,bool Maskable)
{
  // If the print request is non-maskable, always print
  if(!Maskable || !m_Mask)
  {
#ifdef USE_STDIO_MONITOR
    printf("%s\n",Str);
#else
    TQMonitorForm *MonForm = TQMonitorForm::Instance();

    if(MonForm)
    {
      // Set the form to be visible if currently not visible
      if(m_Visible)
      {
        if(!MonForm->Visible)
          MonForm->SetVisible(true);
      }

      MonForm->WriteToMonitor(Str);
    }
#endif    
  }
}

// Formatted output to the monitor (always maskable)
void CQMonitor::Printf(const char *FormatStr,...)
{
  va_list ArgPtr;
  va_start(ArgPtr,FormatStr);

  if(!m_Mask)
  {
    // Print the formatted string to a temporary buffer
    char Buffer[MAX_FORMATTED_PRINT_SIZE];

    // Prepare the formatted string
    vsprintf(Buffer,FormatStr,ArgPtr);

    TQMonitorForm *MonForm = TQMonitorForm::Instance();

#ifdef USE_STDIO_MONITOR
    printf("%s\n",Buffer);
#else
    if(MonForm)
    {
      // Set the form to be visible if currently not visible
      if(m_Visible)
      {
        if(!MonForm->Visible)
          MonForm->SetVisible(true);
      }

      MonForm->WriteToMonitor(Buffer);
    }
#endif    
  }

  va_end(ArgPtr);
}

// Formatted output to the monitor (non maskable)
void CQMonitor::PrintfNonMaskable(const char *FormatStr,...)
{
  va_list ArgPtr;
  va_start(ArgPtr,FormatStr);

  // Print the formatted string to a temporary buffer
  char Buffer[MAX_FORMATTED_PRINT_SIZE];

  // Prepare the formatted string
  vsprintf(Buffer,FormatStr,ArgPtr);

  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(MonForm)
  {
    // Set the form to be visible if currently not visible
    if(m_Visible)
    {
      if(!MonForm->Visible)
        MonForm->SetVisible(true);
    }

    MonForm->WriteToMonitor(Buffer);
  }

  va_end(ArgPtr);
}
// Print an error message
void CQMonitor::ErrorMessage(QString Str, TQCallerOrigin Sender)
{
	SafeErrorMessage(Str, Sender);
}
TQErrCode CQMonitor::SafeErrorMessage(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

if(MonForm && IsMessageSafe(Str,mtError,static_cast<TQCallerOrigin>(CallerId)))
    MonForm->ErrorMessage(Str,false);

  return Q_NO_ERROR;
}
void CQMonitor::WarningMessage(QString Str, TQCallerOrigin Sender)
{
	SafeWarningMessage(Str, Sender);
}
// Print a warning message
TQErrCode CQMonitor::SafeWarningMessage(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

  if(MonForm && IsMessageSafe(Str,mtWarning,static_cast<TQCallerOrigin>(CallerId)))
    MonForm->WarningMessage(Str,false);

  return Q_NO_ERROR;
}
// Print a notification message
void CQMonitor::NotificationMessage(QString Str, TQCallerOrigin Sender)
{
	SafeNotificationMessage(Str, Sender);
}

// Print a notification message
TQErrCode CQMonitor::SafeNotificationMessage(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

  if(MonForm && IsMessageSafe(Str,mtInformation,static_cast<TQCallerOrigin>(CallerId)))
	MonForm->NotificationMessage(Str,false);

  return Q_NO_ERROR;
}
// Print an error message and wait for user approve
void CQMonitor::ErrorMessageWaitOk(QString Str, TQCallerOrigin Sender)
{
	SafeErrorMessageWaitOk(Str, Sender);
}
// Print an error message and wait for user approve
TQErrCode CQMonitor::SafeErrorMessageWaitOk(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

  if(MonForm && IsMessageSafe(Str,mtError,static_cast<TQCallerOrigin>(CallerId)))
    MonForm->ErrorMessage(Str,true);

  return Q_NO_ERROR;
}
// Print a warning message and wait for user approve
void CQMonitor::WarningMessageWaitOk(QString Str, TQCallerOrigin Sender)
{
	SafeWarningMessageWaitOk(Str, Sender);
}

// Print a warning message and wait for user approve
TQErrCode CQMonitor::SafeWarningMessageWaitOk(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

  if(MonForm && IsMessageSafe(Str,mtWarning,static_cast<TQCallerOrigin>(CallerId)))
    MonForm->WarningMessage(Str,true);

  return Q_NO_ERROR;
}

void CQMonitor::NotificationMessageWait(QString Str, TQCallerOrigin Sender)
{
	SafeNotificationMessageWait(Str, Sender);
}
TQErrCode CQMonitor::SafeNotificationMessageWait(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

  if(MonForm && IsMessageSafe(Str,mtInformation,static_cast<TQCallerOrigin>(CallerId)))
	MonForm->NotificationMessageNoButtons(Str);
	
  return Q_NO_ERROR;
}

// Print a notification message and wait for user approve
void CQMonitor::NotificationMessageWaitOk(QString Str, TQCallerOrigin Sender)
{
	SafeNotificationMessageWaitOk(Str, Sender);
}
TQErrCode CQMonitor::SafeNotificationMessageWaitOk(QString Str, int CallerId)
{
  TQMonitorForm *MonForm = TQMonitorForm::Instance();

  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Str);

  if(MonForm && IsMessageSafe(Str,mtInformation,static_cast<TQCallerOrigin>(CallerId)))
	MonForm->NotificationMessage(Str,true);
	
  return Q_NO_ERROR;
}

// This wrapper resolves logic-blocked-by-gui cases between an unexpected popped up msg
// and the might-be criticial logic (e.g. turning pumps on) operating within the Wizards' GUI.
// A blocking popup may interfere with the logic as it is interactive (ALM defect #602).
bool CQMonitor::IsMessageSafe(QString Msg, int MsgType, TQCallerOrigin CallerId) const
{
	//if no wizard is currently running - allow all messages (no risk).
	//or if the caller is indeed a wizard - the message is safe by definition.
	if (ORIGIN_WIZARD_PAGE == CallerId) return true;
	if ((!m_IsWizardRunning) || (!m_NotifyWizardUnsafe))
		return true;
	if (!m_IsWizardRunning()) return true;

	//else - distinguish between intended messages and unexpected ones
	switch (MsgType) {
		case mtInformation: //filter out this type
		case mtConfirmation:
			return false;
		case mtWarning:
		case mtError:
			//notify the wizard that something unexpected has happened
			m_NotifyWizardUnsafe(Msg);
			return false;
		default: //mtCustom
			return true;
	}
}

// Get a single character from the monitor
char CQMonitor::GetChar(void)
{
  return 0;
}

// Request a number from the user (integer version)
bool CQMonitor::GetNumber(const char *Prompt,int& Number)
{
  return TQMonitorForm::Instance()->GetNumber(Prompt,Number);
}

// Request a number from the user (float version)
bool CQMonitor::GetNumber(const char *Prompt,double& Number)
{
  return TQMonitorForm::Instance()->GetNumber(Prompt,Number);
}

// Request a string from the user
QString CQMonitor::GetString(QString Prompt)
{
  return TQMonitorForm::Instance()->GetString(Prompt);
}

// Request a Non-empty string from the user
QString CQMonitor::GetNonEmptyString(QString Prompt)
{
  return TQMonitorForm::Instance()->GetNonEmptyString(Prompt);
}

// Ask the user a ok/cancel question (retrun true if yes)
bool CQMonitor::AskOKCancel(QString Prompt)
{
  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Prompt);

  return TQMonitorForm::Instance()->AskOKCancel(Prompt);
}

// Ask the user a yes/no question (retrun true if yes)
bool CQMonitor::AskYesNo(QString Prompt)
{
  if(m_MonitorErrorLoggingCallback)
	(*m_MonitorErrorLoggingCallback)(Prompt);

  return TQMonitorForm::Instance()->AskYesNo(Prompt);
}

#ifdef OS_WINDOWS
// Set the visibility of the monitor form
void CQMonitor::SetVisible(bool Visible)
{
  if(!Visible)
    TQMonitorForm::Instance()->SetVisible(false);

  m_Visible = Visible;
}
#endif

void CQMonitor::SetMonitorErrorLoggingCallback(TMonitorErrorLoggingCallback MonitorErrorLoggingCallback)
{
	m_MonitorErrorLoggingCallback = MonitorErrorLoggingCallback;
}

