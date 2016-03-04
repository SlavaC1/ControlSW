/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Monitor)                               *
 * Module Description: Monitor form implementation for windows/BCB  *
 *                                                                  *
 * Compilation: Win32, BCB                                          *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 25/06/2003                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "QMonitorDlg.h"
#include "QMonitor.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

// Monitor command options (passed as WParam in the WM_QMONITOR_COMMAND message)
const int Q_CMD_ADD_LINE             = 0;
const int Q_CMD_CLEAR_ALL            = 1;
const int Q_CMD_GET_INT_NUMBER       = 2;
const int Q_CMD_GET_FLOAT_NUMBER     = 3;
const int Q_CMD_GET_STRING           = 4;
const int Q_CMD_ASK_YES_NO           = 5;
const int Q_CMD_ERROR_MESSAGE        = 6;
const int Q_CMD_WARNING_MESSAGE      = 7;
const int Q_CMD_NOTIFICATION_MESSAGE = 8;
const int Q_CMD_VISIBLE              = 9;
const int Q_CMD_ASK_OK_CANCEL        = 10;
const int Q_CMD_NOTIFICATION_MESSAGE_NO_BUTTONS        = 11;
const int Q_CMD_GET_NON_EMPTY_STRING = 12;

const int INITIAL_STRINGS_QUEUE_SIZE = 32;
const int MAX_STRINGS_QUEUE_SIZE = 256;

const int MAX_LINES_NUM_IN_MONITOR = 10000;

// Pointer to the singleton form object
TQMonitorForm *TQMonitorForm::m_QMonitorForm = NULL;

// Return an instance to the singleton form object
TQMonitorForm *TQMonitorForm::Instance(void)
{
  // Check if we are not trying to access non-initilaized instance
  if(m_QMonitorForm == NULL)
    throw EQMonitor("QMonitor is not initiailized");

  return m_QMonitorForm;
}

// Initialize the singleton object
// Note: Because of the ownership transfer to the Application object, no DeInit function is defined.
//       The OnDestroy event set the global singleton objecr to NULL.
void TQMonitorForm::Init(void)
{
  m_QMonitorForm = new TQMonitorForm(Application);
}


__fastcall TQMonitorForm::TQMonitorForm(TComponent* Owner)
        : TForm(Owner)
{
}

// Add a line to the monitor (thread safe)
void TQMonitorForm::WriteToMonitor(const QString Str)
{ 
  // If the current thread the GUI thread add the text directly to the window
  if(GetCurrentThreadId() == m_GUIMainThread)
    FormatAndAddTextToMonitor(Str);
  else
  {
    // Add to output queue
    if (AddToOutputQueue(Str))
    {
    // Synchronize with the GUI thread
    PostMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_ADD_LINE,0);
    }
  }
}

// Clear the entire monitor
void TQMonitorForm::ClearAll(void)
{
  m_CurrentLine = 0;
  PostMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_CLEAR_ALL,0);
}

// Message handler for the WM_UPDATE_CAPTION message
void TQMonitorForm::HandleQMonitorCommand(TMessage &Message)
{
  QString Str;

  switch(Message.WParam)
  {
    case Q_CMD_ADD_LINE:
    {
	  QLib::TQWaitResult WaitResult = m_StringMessagQueue->Receive(Str,0);
	  if (WaitResult == QLib::wrSignaled)
        FormatAndAddTextToMonitor(Str);
      break;
    }

    case Q_CMD_CLEAR_ALL:
      MonitorText->Lines->Clear();
      break;

    case Q_CMD_GET_INT_NUMBER:
      m_RetFlag = DoGetNumber(m_Prompt,m_IntegerNumber);
      break;

    case Q_CMD_GET_FLOAT_NUMBER:
      m_RetFlag = DoGetNumber(m_Prompt,m_FloatNumber);
      break;

    case Q_CMD_GET_STRING:
      m_Prompt = DoGetString(m_Prompt);
      break;
	
	case Q_CMD_GET_NON_EMPTY_STRING:
	  m_Prompt = DoGetNonEmptyString(m_Prompt);
	  break;

    case Q_CMD_ASK_OK_CANCEL:
      m_RetFlag = DoAskOKCancel(m_Prompt);
      break;

    case Q_CMD_ASK_YES_NO:
      m_RetFlag = DoAskYesNo(m_Prompt);
      break;

    case Q_CMD_ERROR_MESSAGE:
    {
	  QLib::TQWaitResult WaitResult = m_StringMessagQueue->Receive(Str,0);
	  if (WaitResult == QLib::wrSignaled)
        DoErrorMessage(Str);
      break;
    }

    case Q_CMD_WARNING_MESSAGE:
    {
	  QLib::TQWaitResult WaitResult = m_StringMessagQueue->Receive(Str,0);
	  if (WaitResult == QLib::wrSignaled)
        DoWarningMessage(Str);
      break;
    }

    case Q_CMD_NOTIFICATION_MESSAGE_NO_BUTTONS:
    {
	  QLib::TQWaitResult WaitResult = m_StringMessagQueue->Receive(Str,0);
	  if (WaitResult == QLib::wrSignaled)
        DoNotificationMessageNoButtons(Str);
      break;
    }

    case Q_CMD_NOTIFICATION_MESSAGE:
    {
	  QLib::TQWaitResult WaitResult = m_StringMessagQueue->Receive(Str,0);
	  if (WaitResult == QLib::wrSignaled)
        DoNotificationMessage(Str);
      break;
    }

    case Q_CMD_VISIBLE:
      // Set the form visibility
      Visible = (Message.LParam != 0);
  }
}

void __fastcall TQMonitorForm::AlwaysOnTopMenuItemClick(TObject *Sender)
{
  AlwaysOnTopMenuItem->Checked = !AlwaysOnTopMenuItem->Checked;
  FormStyle = AlwaysOnTopMenuItem->Checked ? fsStayOnTop : fsNormal;
}

void __fastcall TQMonitorForm::Close1Click(TObject *Sender)
{
  Close();
}

void TQMonitorForm::FormatAndAddTextToMonitor(const QString& Str)
{
  // Check current content if we got to the line number limit
  if(MonitorText->Lines->Count >= MAX_LINES_NUM_IN_MONITOR)
    MonitorText->Lines->Clear();

  MonitorText->Lines->Add(Str.c_str());    
}

void __fastcall TQMonitorForm::FormCreate(TObject *Sender)
{
  m_CurrentLine = 0;
  m_StringMessagQueue = new TStringMessagQeueu(INITIAL_STRINGS_QUEUE_SIZE,"QMonitorMessageQueue",false);
  m_QueueIsFull = false;

  // Remember the GUI main thread
  m_GUIMainThread = GetCurrentThreadId();
}

// Request a number from the user (integer version)
bool TQMonitorForm::DoGetNumber(const AnsiString& Prompt,int& Number)
{
  bool Ret = true;
  AnsiString s = InputBox("Monitor",Prompt,"");

  try
  {
    Number = StrToInt(s);
  } catch(...)
    {
      // On error return false
      Ret = false;
    }

  return Ret;
}

// Request a number from the user (float version)
bool TQMonitorForm::DoGetNumber(const AnsiString& Prompt,double& Number)
{
  bool Ret = true;
  AnsiString s = InputBox("Monitor",Prompt,"");

  try
  {
    Number = StrToFloat(s);
  } catch(...)
    {
      // On error return false
      Ret = false;
    }

  return Ret;
}

// Request a string from the user
AnsiString TQMonitorForm::DoGetString(const AnsiString& Prompt)
{
  return InputBox("Monitor",Prompt,"");
}

// Request a non-empty string from the user
AnsiString TQMonitorForm::DoGetNonEmptyString(const AnsiString& Prompt)
{
  AnsiString Value = "Enter value here...";

  bool PressedOK = InputQuery("Enter Value",Prompt,Value);

  if (!PressedOK) 
  {
	return "DefaultValue";
  }
  else
  {
	return Value;	  
  }
}

// Ask the user a yes/no question (retrun true if yes)
bool TQMonitorForm::DoAskYesNo(const AnsiString& Prompt)
{
  return (MessageDlg(Prompt,mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,0) == mrYes);
}

// Ask the user a ok/cancel question (retrun true if yes)
bool TQMonitorForm::DoAskOKCancel(const AnsiString& Prompt)
{
  return (MessageDlg(Prompt,mtConfirmation,TMsgDlgButtons() << mbOK << mbCancel ,0) == mrOk);
}

// Show an error message
void TQMonitorForm::DoErrorMessage(const QString& Prompt)
{
  MessageDlg(Prompt.c_str(),mtError,TMsgDlgButtons() << mbOK,0);
}

// Show a warning message
void TQMonitorForm::DoWarningMessage(const QString& Prompt)
{
  MessageDlg(Prompt.c_str(),mtWarning,TMsgDlgButtons() << mbOK,0);
}

void TQMonitorForm::DoNotificationMessageNoButtons(const QString& Prompt)
{
  MessageDlg(Prompt.c_str(),mtInformation,TMsgDlgButtons(),0);
}

// Show a notification message
void TQMonitorForm::DoNotificationMessage(const QString& Prompt)
{
  MessageDlg(Prompt.c_str(),mtInformation,TMsgDlgButtons() << mbOK,0);
}

// Request a number from the user (integer version)
bool TQMonitorForm::GetNumber(const QString Prompt,int& Number)
{
  // Make sure that only we can access the internal data
  m_PromptMutex.WaitFor();

  // Remember for later
  m_Prompt = Prompt.c_str();

  // Synchronize with the GUI thread
  SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_GET_INT_NUMBER,0);

  m_PromptMutex.Release();

  if(m_RetFlag)
    Number = m_IntegerNumber;

  return m_RetFlag;
}

// Request a non-empty string from the user
QString TQMonitorForm::GetNonEmptyString(const QString Prompt)
{
  // Make sure that only we can access the internal data
  m_PromptMutex.WaitFor();

  // Remember for later
  m_Prompt = Prompt.c_str();

  // Synchronize with the GUI thread
  SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_GET_NON_EMPTY_STRING,0);

  m_PromptMutex.Release();

  // The return string is in the prompt
  return m_Prompt.c_str();
}

// Request a number from the user (float version)
bool TQMonitorForm::GetNumber(const QString Prompt,double& Number)
{
  // Make sure that only we can access the internal data
  m_PromptMutex.WaitFor();

  // Remember for later
  m_Prompt = Prompt.c_str();

  // Synchronize with the GUI thread
  SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_GET_FLOAT_NUMBER,0);

  m_PromptMutex.Release();

  if(m_RetFlag)
    Number = m_FloatNumber;

  return m_RetFlag;
}

// Request a string from the user
QString TQMonitorForm::GetString(const QString Prompt)
{
  // Make sure that only we can access the internal data
  m_PromptMutex.WaitFor();

  // Remember for later
  m_Prompt = Prompt.c_str();

  // Synchronize with the GUI thread
  SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_GET_STRING,0);

  m_PromptMutex.Release();

  // The return string is in the prompt
  return m_Prompt.c_str();
}

// Ask the user a yes/no question (retrun true if yes)
bool TQMonitorForm::AskYesNo(const QString Prompt)
{
  // Make sure that only we can access the internal data
  m_PromptMutex.WaitFor();

  // Remember for later
  m_Prompt = Prompt.c_str();

  // Synchronize with the GUI thread
  SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_ASK_YES_NO,0);

  m_PromptMutex.Release();

  return m_RetFlag;
}

// Ask the user a yes/no question (retrun true if yes)
bool TQMonitorForm::AskOKCancel(const QString Prompt)
{
  // Make sure that only we can access the internal data
  m_PromptMutex.WaitFor();

  // Remember for later
  m_Prompt = Prompt.c_str();

  // Synchronize with the GUI thread
  SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_ASK_OK_CANCEL,0);

  m_PromptMutex.Release();

  return m_RetFlag;
}

// Show an error message
void TQMonitorForm::ErrorMessage(const QString Prompt,bool WaitForUser)
{
   // Add to output queue
  if (AddToOutputQueue(Prompt))
  {

    // Synchronize with the GUI thread
    if(WaitForUser)
      SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_ERROR_MESSAGE,0);
    else
      PostMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_ERROR_MESSAGE,0);

  }
}

// Show a warning message
void TQMonitorForm::WarningMessage(const QString Prompt,bool WaitForUser)
{
   // Add to output queue
  if (AddToOutputQueue(Prompt))
  {
    // Synchronize with the GUI thread
    if(WaitForUser)
      SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_WARNING_MESSAGE,0);
    else
      PostMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_WARNING_MESSAGE,0);

  }
}

void TQMonitorForm::NotificationMessageNoButtons(const QString Prompt)
{
   // Add to output queue
  if (AddToOutputQueue(Prompt))
    // Synchronize with the GUI thread
    SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_NOTIFICATION_MESSAGE_NO_BUTTONS,0);
}

// Show a notification message
void TQMonitorForm::NotificationMessage(const QString Prompt,bool WaitForUser)
{
   // Add to output queue
  if (AddToOutputQueue(Prompt))
  {
    // Synchronize with the GUI thread
    if(WaitForUser)
      SendMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_NOTIFICATION_MESSAGE,0);
    else
      PostMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_NOTIFICATION_MESSAGE,0);

  }
}

void __fastcall TQMonitorForm::ClearAllActionExecute(TObject *Sender)
{
  ClearAll();
}

void __fastcall TQMonitorForm::SaveAsActionExecute(TObject *Sender)
{
  if(SaveDialog->Execute())
    MonitorText->Lines->SaveToFile(SaveDialog->FileName);
}

// Add QMenus user options, return pointer to the base menu item for attachment
TMenuItem *TQMonitorForm::AddQMenuUserOptions(void)
{
  // Create a new menu item
  TMenuItem *UserOptionsBaseMenu = new TMenuItem(this);

  UserOptionsBaseMenu->Caption = "Monitor";

  MainMenu->Items->Add(UserOptionsBaseMenu);

  return UserOptionsBaseMenu;
}

// Set the form visibility (thread safe)
void TQMonitorForm::SetVisible(bool VisibleFlag)
{
  PostMessage(Handle,WM_QMONITOR_COMMAND,Q_CMD_VISIBLE,VisibleFlag ? 1 : 0);
}

void __fastcall TQMonitorForm::FormDestroy(TObject *Sender)
{
  Q_SAFE_DELETE(m_StringMessagQueue);

  // Make sure that the singleton pointer is null when the form is destroying
  m_QMonitorForm = NULL;
}

// Add string to output queue. If the queue is full enlarge it till the maximum
// size, when this limitaion is reached insert a sepcial message to the queue
// ('Queue is full') throw the next messages until the queue is not full and return false
bool TQMonitorForm::AddToOutputQueue(const QString Str)
{
  // Make sure that only we can access the internal data
  m_MsgQueueMutex.WaitFor();

  // If there is place in the message queue, add current item
  if(m_StringMessagQueue->ItemsCount < m_StringMessagQueue->QueueSize)
  {
    m_StringMessagQueue->Send(Str);
    m_QueueIsFull = false;
  }
  else
  {
    // If the queue size is less than the maximum, resize it and insert the string
    if ((m_StringMessagQueue->QueueSize * 2) <= MAX_STRINGS_QUEUE_SIZE)
    {
      m_StringMessagQueue->QueueSize = m_StringMessagQueue->QueueSize * 2;
      m_StringMessagQueue->Send(Str);
      m_QueueIsFull = false;
    }
    else
    {
      // If the queue is at its maximum size, insert a 'Queue is full' message
      if (!m_QueueIsFull)
      {
        m_QueueIsFull = true;
        QString RemovedString;
        m_StringMessagQueue->SendUrgentAndBreakInto("QMonitor queue is full, the next messages will not be displayed",RemovedString);

        m_MsgQueueMutex.Release();
        return false;
        
      }
      else
      {
        m_MsgQueueMutex.Release();
        return false;
      }
    }
  }

  m_MsgQueueMutex.Release();
  return true;

}
