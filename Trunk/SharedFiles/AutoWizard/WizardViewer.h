/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Wizard viewing frame.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/01/2003                                           *
 * Last upate: 07/03/2003                                           *
 ********************************************************************/

#ifndef WizardViewerH
#define WizardViewerH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "AutoWizard.h"
#include "WizardPages.h"
#include <ActnList.hpp>
#include <Gauges.hpp>
#include <map>

class CMessageWizardPage;
class CStatusWizardPage;
class CProgressWizardPage;
class CCheckBoxWizardPage;
class CRadioGroupWizardPage;
class CProgressStatusWizardPage;
class TWizardViewerFrame;
class CDataEntryWizardPage;

class CWizardProxy;

typedef bool (*TRunWizardCallback)(void);

// Base class for custom wizard page viewers
class CCustomWizardPageViewer : CQObject {
  friend class TWizardViewerFrame;

  private:
    CWizardPage *m_ActiveWizardPage;
    CAutoWizard *m_ParentWizard;

    // Set the wizard parent
    void SetParentWizard(CAutoWizard *ParentWizard) {
      m_ParentWizard = ParentWizard;
    }

    // Set the current active page
    void SetActivePage(CWizardPage *WizardPage) {
      m_ActiveWizardPage = WizardPage;
    }

  protected:
    CWizardPage *GetActivePage(void) {
      return m_ActiveWizardPage;
    }

    // Return a pointer to the parent AutoWizard
    CAutoWizard *GetParentWizard(void) {
      return m_ParentWizard;
    }

  public:
    virtual void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage) {}
    virtual void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage) {}
    virtual void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason) {}
};


// Type for a list of custom page viewers
typedef std::map<TWizardPageType,CCustomWizardPageViewer *> TCustomPageViewersList;

// Define private windows message for status updates
#define WM_WIZARD_MESSAGE WM_USER
const int FE_WIZARD_GOTOPAGEASYNC = 1;
const int FE_WIZARD_CANCEL = 0;


class TWizardViewerFrame : public TFrame
{
__published:	// IDE-managed Components
        TBitBtn *NextButton;
        TBitBtn *PrevButton;
        TBitBtn *CancelButton;
        TBevel *Bevel1;
        TPageControl *WizardPageControl;
        TTabSheet *MessagePage;
        TLabel *MessageLabel;
        TTabSheet *StatusPage;
        TPanel *StatusPanel;
        TTabSheet *ProgressPage;
        TGauge *ProgressIndicator;
        TTabSheet *CheckboxesPage;
        TTabSheet *RadioGroupPage;
        TRadioGroup *RadioGroup;
        TLabel *TitleLabel;
        TBitBtn *DoneButton;
        TImage *PageImage;
        TBitBtn *HelpButton;
        TActionList *ActionList1;
        TAction *HelpAction;
        TTabSheet *CustomTabSheet;
        TBitBtn *UserButton2;
        TBitBtn *UserButton1;
        TLabel *HiddenTitleLabel;
    TPanel *MainPanel;
    TLabel *CheckBoxPageSubTitle;
    TLabel *RadioGroupPageSubTitle;
        TLabel *ProgressPageSubTitle;
        TLabel *StatusPageSubTitle;
    TTabSheet *ProgressStatusTabSheet;
    TGauge *ProgressStatusProgressIndicator;
    TLabel *ProgressStatusPageSubTitle;
    TPanel *ProgressStatusStatusPanel;
        TLabel *HiddenSubTitleLabel;
        TScrollBox *CheckBoxesContainer;
    TTabSheet *DataEntryPage;
    TLabel *DataEntryPageSubTitle;
    TScrollBox *DataEntryFieldsContainer;
    TLabel *StatusLabel;
        TTimer *PageWizardTimer;
  TAction *ProgressiveDisclosureExpandAction;
  TAction *ProgressiveDisclosureCollapseAction;
    TLabel *PleaseWaitLabel;
        void __fastcall NextButtonClick(TObject *Sender);
        void __fastcall PrevButtonClick(TObject *Sender);
        void __fastcall DoneButtonClick(TObject *Sender);
        void __fastcall CancelButtonClick(TObject *Sender);
        void __fastcall HelpActionExecute(TObject *Sender);
        void __fastcall UserButton1Click(TObject *Sender);
        void __fastcall UserButton2Click(TObject *Sender);
        void __fastcall PageWizardTimerTimer(TObject *Sender);
        void __fastcall ProgressiveDisclosureClick(TObject *Sender);
  void __fastcall ProgressiveDisclosureCollapseActionExecute(TObject *Sender);
  void __fastcall ProgressiveDisclosureExpandActionExecute(TObject *Sender);

private:	// User declarations
  CAutoWizard *m_AutoWizard;

  CWizardPage *m_CurrentPage;

  TCustomPageViewersList m_CustomPageViewers;

  static bool m_IsTimeOutMechanismEnable;

  unsigned int m_FirstDisclosedCheckBoxIndex;

  bool CheckString(const QString Str,TDataEntryFieldType UserValuesType);

  template<class T>
  bool TypedStringChecker(const QString Str);

  static CWizardProxy *m_WizardProxy;

  TBitBtn *m_ProgressiveDisclosureControl;

//  static CWizBackupSizes *m_CustomPageUIPositionsBackup; 
  							  
public:
  bool isNextCancelPage;
  bool UpdateImage(CWizardPage *WizardPage);

private:
  void PleaseWaitNotificationOn(bool OnOff);

  // Help function that deletes all the controls from a given win control
  void CleanUpWinControl(TWinControl *WinControl); 

  void __fastcall CheckBoxClickHandler(TObject *Sender);
  unsigned int CountChecked() const;
  void __fastcall RadioGroupClickHandler(TObject *Sender);
  void __fastcall DataEntryChangeHandler(TObject *Sender);

  void EnableWizardPageTimeout(bool Enable);

  void SetTitle(AnsiString Title);
  bool SetSubTitle(TLabel *SubTitleLabel,AnsiString SubTitle,bool GrowDown = false);

  bool ShowPage(CWizardPage *WizardPage);
  bool UpdateCurrentPageNoShow(CWizardPage *WizardPage);

  bool CheckString(const QString Str);

  // Prepare pages
  void PrepareMessagePage(CMessageWizardPage *PagePtr);
  void PrepareStatusPage(CStatusWizardPage *PagePtr);
  void PrepareProgressPage(CProgressWizardPage *PagePtr);
  void PrepareCheckboxesPage(CCheckBoxWizardPage *PagePtr);
  void PrepareRadioGroupPage(CRadioGroupWizardPage *PagePtr);
  void PrepareProgressStatusPage(CProgressStatusWizardPage *PagePtr);
  void PrepareCustomPage(TWizardPageType PageType,CWizardPage *WizardPage);
  void PrepareDataEntryPage(CDataEntryWizardPage *PagePtr);

  // Refresh a wizard page
  void RefreshPage(CWizardPage *WizardPage);

  // Refresh pages
  void RefreshMessagePage(CMessageWizardPage *PagePtr);
  void RefreshStatusPage(CStatusWizardPage *PagePtr);
  void RefreshProgressPage(CProgressWizardPage *PagePtr);
  void RefreshCheckboxesPage(CCheckBoxWizardPage *PagePtr);
  void RefreshRadioGroupPage(CRadioGroupWizardPage *PagePtr);
  void RefreshProgressStatusPage(CProgressStatusWizardPage *PagePtr);
  void RefreshCustomPage(TWizardPageType PageType,CWizardPage *WizardPage);
  void RefreshDataEntryPage(CDataEntryWizardPage *PagePtr);

  void EnableDisableNext    (bool Enabled);
  void EnableDisableButtons(bool Enabled);

  // Leave the current page
  bool LeaveCurrentPage(TWizardPageLeaveReason LeaveReason);

  static void PageRefreshCallback(CWizardPage *WizardPage,TGenericCockie Cockie);
  static void UIControlCallback(TWizardUIControlType ControlType,bool Enabled,TGenericCockie Cockie);

		bool IsNegativeNumberPossible(TDataEntryFieldType DataEntryFieldType);

	// Message handler for the WM_WIZARD_MESSAGE message
	void HandleStatusUpdateMessage(TMessage &Message);

public:		// User declarations
  __fastcall TWizardViewerFrame(TComponent* Owner);

  // Destructor
  __fastcall ~TWizardViewerFrame(void);

  // Register a custom wizard page viewer
  void RegisterCustomPageViewer(TWizardPageType PageType,CCustomWizardPageViewer *WizardPageViewer);

  // Unregister all custom wizard page viewers
  void UnRegisterCustomPageViewers(void);

  bool RunWizard(CAutoWizard *AutoWizard,int StartingPage = 0);

  // Return true if a cancel operation is cuurently allowed
  bool IsCancelAllowed(void);

  // Return true if the wizard has finished sucessfully
  bool IsDoneOk(void);

  // Close current session (The OnCancel event will be (might be) called)
  void CloseSession(void);

  // Restart the wizard sesssion
  bool Restart(int StartingPage = 0);

  // Event handler for the wizard "done" and "cancel" events
  TNotifyEvent OnDone;
  TNotifyEvent OnCancel;

  static void SetTimeOutMechanismEnable(bool Enable);
  static void Init(void);
  static void DeInit(void);
  static void SetRunWizardEvent(TRunWizardCallback RunWizardEvent);

  void PostCancelMessage(); // for cancelling the wizard without asking the user
  void GotoNextPageAsync();

  /* Expands the viewer's dimensions to accomodate a new customized frame,
	 replacing all GUI except basic navigation components.
	 Consecutive calls to ReplaceViewerFrame() will re-resize and re-replace
	 the viewer's canvas.
	 Effects are reversed only upon calling RestoreViewerFrame().
   */
  void ReplaceViewerFrame(TFrame* CustomizedFrame, bool ShowImage = false);

  void SetDefaultFrameLayout();

  CAutoWizard* GetCurrentWizard();

BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_WIZARD_MESSAGE,TMessage,HandleStatusUpdateMessage);
END_MESSAGE_MAP(TFrame);


};

#endif
