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
 * Last upate: 12/08/2001                                           *
 ********************************************************************/

#ifndef QMonitorDlgH
#define QMonitorDlgH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>

#include "QSemaphore.h"
#include "QMessageQueue.h"
#include "QMutex.h"
#include <Dialogs.hpp>
#include <ActnList.hpp>

#define WM_QMONITOR_COMMAND WM_USER

class TQMonitorForm : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu;
        TMenuItem *N1;
        TMenuItem *Close1;
        TMemo *MonitorText;
        TMenuItem *ClearAllMenuItem;
        TMenuItem *OptionsMenuItem;
        TMenuItem *AlwaysOnTopMenuItem;
        TMenuItem *N2;
        TMenuItem *SaveAsMenuItem;
        TSaveDialog *SaveDialog;
        TActionList *ActionList1;
        TAction *ClearAllAction;
        TAction *SaveAsAction;
        TPopupMenu *PopupMenu1;
        TMenuItem *ClearAll1;
        TMenuItem *SaveAs1;
        void __fastcall AlwaysOnTopMenuItemClick(TObject *Sender);
        void __fastcall Close1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall ClearAllActionExecute(TObject *Sender);
        void __fastcall SaveAsActionExecute(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations

    typedef CQMessageQueue<QString> TStringMessagQeueu;

    // Message queue if incoming strings
    TStringMessagQeueu *m_StringMessagQueue;

    int m_CurrentLine;

    DWORD m_GUIMainThread;

    // Message handler for the WM_UPDATE_CAPTION message
    MESSAGE void HandleQMonitorCommand(TMessage &Message);

    void FormatAndAddTextToMonitor(const QString& Str);

    // Implementations of the prompt functions (GetNumber,AskYesOrNo)
    // -------------------------------------------------------------

    AnsiString m_Prompt;
    int m_IntegerNumber;
    double m_FloatNumber;
    bool m_RetFlag;

    // Mark that the queue was resized to its maximum size and a "queue is full" message was displayed
    bool m_QueueIsFull;
    CQMutex m_PromptMutex;
    CQMutex m_MsgQueueMutex;

    // Request a number from the user (integer version)
    bool DoGetNumber(const AnsiString& Prompt,int& Number);

    // Request a number from the user (float version)
    bool DoGetNumber(const AnsiString& Prompt,double& Number);

    // Request a string from the user
    AnsiString DoGetString(const AnsiString& Prompt);
	
	// Request a non-empty string from the user
	AnsiString DoGetNonEmptyString(const AnsiString& Prompt);

    // Ask the user a yes/no question (retrun true if yes)
    bool DoAskYesNo(const AnsiString& Prompt);
    bool DoAskOKCancel(const AnsiString& Prompt);

    // Show an error message
    void DoErrorMessage(const QString& Prompt);

    // Show a warning message
    void DoWarningMessage(const QString& Prompt);

    // Show a notification message
    void DoNotificationMessageNoButtons(const QString& Prompt);
    void DoNotificationMessage(const QString& Prompt);

    //Add string to output queue. If the queue is full enlarge it till the maximum
    // size, when this limitaion is reached insert a sepcial message to the queue
    // ('Queue is full') throw the next messages until the queue is not full and return false
    bool AddToOutputQueue(const QString Str);
    static TQMonitorForm *m_QMonitorForm;

public:		// User declarations
        __fastcall TQMonitorForm(TComponent* Owner);

  // Add a line to the monitor (thread safe)
  void WriteToMonitor(const QString Line);

  // Clear the entire monitor
  void ClearAll(void);

  // Request a number from the user (integer version)
  bool GetNumber(const QString Prompt,int& Number);

  // Request a number from the user (float version)
  bool GetNumber(const QString Prompt,double& Number);

  // Request a string from the user
  QString GetString(const QString Prompt);
  
  // Request a non-empty string from the user
  QString GetNonEmptyString(const QString Prompt);

  // Ask the user a yes/no question (retrun true if yes)
  bool AskYesNo(const QString Prompt);
  bool AskOKCancel(const QString Prompt);

  // Show an error message
  void ErrorMessage(const QString Prompt,bool WaitForUser);

  // Show a warning message
  void WarningMessage(const QString Prompt,bool WaitForUser);

  // Show a notification message
  void NotificationMessageNoButtons(const QString Prompt);
  void NotificationMessage(const QString Prompt,bool WaitForUser);

  // Add QMenus user options, return pointer to the base menu item for attachment
  TMenuItem *AddQMenuUserOptions(void);

  // Set the form visibility (thread safe)
  void SetVisible(bool VisibleFlag);

  // Return an instance to the singleton form object
  static TQMonitorForm *Instance(void);

// Initialize the singleton object
  static void Init(void);

// Disable warnings about inline functions
#pragma option push -w-inl

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_QMONITOR_COMMAND,TMessage,HandleQMonitorCommand);
END_MESSAGE_MAP(TForm);

// Restore previous options
#pragma option pop

};

#endif
