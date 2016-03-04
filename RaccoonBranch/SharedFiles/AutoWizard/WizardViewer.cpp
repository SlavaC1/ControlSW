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
 * Last upate: 15/01/2003                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include <assert.h>
#include "WizardViewer.h"
#include "WizardPages.h"
#include "QMonitor.h"
#include "QErrors.h"
#include "AutoWizardDlg.h"
#include "WizardImages.h"
#include "FEResources.h"
#include "TimeElapsingFrame.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Gauges"
#pragma resource "*.dfm"


// Pages indices
const int MESSAGE_PAGE_INDEX         = 0;
const int STATUS_PAGE_INDEX          = 1;
const int PROGRESS_PAGE_INDEX        = 2;
const int CHECKBOXES_PAGE_INDEX      = 3;
const int RADIOGROUP_PAGE_INDEX      = 4;
const int PROGRESS_STATUS_PAGE_INDEX = 5;
const int CUSTOM_PAGE_INDEX          = 6;
const int DATA_ENTRY_PAGE_INDEX      = 7;

// Help function to perform a logical XOR operation
bool LogicXor(bool A,bool B)
{
  return ((!A && B) || (A && !B));
}

// Proxy object for accessing wizard UI elements
class CWizardProxy : public CQComponent
{
  private:
    // Pointer to the current wizard instance
    CAutoWizard *m_CurrentWizard;

    TRunWizardCallback m_RunWizardEvent;

  public:
    // Constructor
    CWizardProxy(void);

    ~CWizardProxy()
    {
      m_CurrentWizard = NULL;
    }

    void SetCurrentWizard(CAutoWizard *CurrentWizard)
    {
      m_CurrentWizard = CurrentWizard;
    }

    // Set a callback for the "run wizard event"
    void SetRunWizardEvent(TRunWizardCallback RunWizardEvent) {
      m_RunWizardEvent = RunWizardEvent;
    }

    DEFINE_METHOD_1(CWizardProxy,TQErrCode,EnableDisableNext,bool);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,EnableDisablePrevious,bool);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,EnableDisableCancel,bool);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,EnableDisableHelp,bool);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,EnableDisableUserButton1,bool);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,EnableDisableUserButton2,bool);
    DEFINE_METHOD(CWizardProxy,TQErrCode,YieldWizardThread);
    DEFINE_METHOD(CWizardProxy,TQErrCode,YieldAndSleepWizardThread);
    DEFINE_METHOD(CWizardProxy,bool,IsCancelled);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,SetNextPage,int);
	DEFINE_METHOD_1(CWizardProxy,TQErrCode,RefreshPage,int);
		DEFINE_METHOD_2(CWizardProxy,TQErrCode,SetImagePage,int,int);
    DEFINE_METHOD(CWizardProxy,TQErrCode,GotoNextPage);
    DEFINE_METHOD(CWizardProxy,TQErrCode,GotoPreviousPage);
    DEFINE_METHOD_1(CWizardProxy,TQErrCode,GotoPage,int);
    DEFINE_METHOD(CWizardProxy,TQErrCode,Cancel);    

    DEFINE_METHOD(CWizardProxy,bool,RunWizard);
};

CWizardProxy::CWizardProxy(void) : CQComponent("WizardProxy")
{
  m_CurrentWizard = NULL;
  m_RunWizardEvent = NULL;

  INIT_METHOD(CWizardProxy,EnableDisableNext);
  INIT_METHOD(CWizardProxy,EnableDisablePrevious);
  INIT_METHOD(CWizardProxy,EnableDisableCancel);
  INIT_METHOD(CWizardProxy,EnableDisableHelp);
  INIT_METHOD(CWizardProxy,EnableDisableUserButton1);
  INIT_METHOD(CWizardProxy,EnableDisableUserButton2);
  INIT_METHOD(CWizardProxy,YieldWizardThread);
  INIT_METHOD(CWizardProxy,YieldAndSleepWizardThread);
  INIT_METHOD(CWizardProxy,RunWizard);
  INIT_METHOD(CWizardProxy,SetNextPage);
  INIT_METHOD(CWizardProxy,RefreshPage);
  INIT_METHOD(CWizardProxy,SetImagePage);
  INIT_METHOD(CWizardProxy,IsCancelled);
  INIT_METHOD(CWizardProxy,GotoNextPage);
  INIT_METHOD(CWizardProxy,GotoPreviousPage);
  INIT_METHOD(CWizardProxy,GotoPage);
  INIT_METHOD(CWizardProxy,Cancel);
}

TQErrCode CWizardProxy::EnableDisableNext(bool Enabled)
{
  if(m_CurrentWizard)
    m_CurrentWizard->EnableDisableNext(Enabled);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::EnableDisablePrevious(bool Enabled)
{
  if(m_CurrentWizard)
    m_CurrentWizard->EnableDisablePrevious(Enabled);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::EnableDisableCancel(bool Enabled)
{
  if(m_CurrentWizard)
    m_CurrentWizard->EnableDisableCancel(Enabled);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::EnableDisableHelp(bool Enabled)
{
  if(m_CurrentWizard)
    m_CurrentWizard->EnableDisableHelp(Enabled);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::EnableDisableUserButton1(bool Enabled)
{
  if(m_CurrentWizard)
    m_CurrentWizard->EnableDisableUserButton1(Enabled);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::EnableDisableUserButton2(bool Enabled)
{
  if(m_CurrentWizard)
    m_CurrentWizard->EnableDisableUserButton2(Enabled);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::YieldWizardThread(void)
{
  if(m_CurrentWizard)
    m_CurrentWizard->YieldWizardThread();

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::YieldAndSleepWizardThread(void)
{
  if(m_CurrentWizard)
    m_CurrentWizard->YieldAndSleepWizardThread();

  return Q_NO_ERROR;
}

bool CWizardProxy::IsCancelled(void)
{
  if(m_CurrentWizard)
    return m_CurrentWizard->IsCancelled();

  return false;
}

TQErrCode CWizardProxy::RefreshPage(int PageNumber)
{
  if(m_CurrentWizard)
    m_CurrentWizard->RefreshPage(PageNumber);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::SetImagePage(int PageNumber, int _ImageID)
{
  if(m_CurrentWizard)
	m_CurrentWizard->SetImagePage(PageNumber, _ImageID);

  return Q_NO_ERROR;
}

bool CWizardProxy::RunWizard(void)
{
  bool Ret    = false;

  try
  {
    // Fire the run wizard event
    if(m_RunWizardEvent)
      Ret = (*m_RunWizardEvent)();
    
  } catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg(),ORIGIN_WIZARD_PAGE);
    }

  return Ret;
}

TQErrCode CWizardProxy::SetNextPage(int PageNum)
{
  if(m_CurrentWizard)
    m_CurrentWizard->SetNextPage(PageNum);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::GotoNextPage(void)
{
  if(m_CurrentWizard)
    m_CurrentWizard->GotoNextPage();

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::GotoPreviousPage(void)
{
  if(m_CurrentWizard)
    m_CurrentWizard->GotoPreviousPage();

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::GotoPage(int PageNumber)
{
  if(m_CurrentWizard)
    m_CurrentWizard->GotoPage(PageNumber);

  return Q_NO_ERROR;
}

TQErrCode CWizardProxy::Cancel(void)
{
  if(m_CurrentWizard)
    m_CurrentWizard->Cancel();

  return Q_NO_ERROR;
}

CWizardProxy *TWizardViewerFrame::m_WizardProxy              = NULL;
bool          TWizardViewerFrame::m_IsTimeOutMechanismEnable = false;

void TWizardViewerFrame::SetTimeOutMechanismEnable(bool Enable)
{
  m_IsTimeOutMechanismEnable = Enable;
}

// Initialize the wizard proxy object
void TWizardViewerFrame::Init(void)
{
  if(m_WizardProxy == NULL)
    m_WizardProxy = new CWizardProxy;
}

void TWizardViewerFrame::DeInit(void)
{
  if(m_WizardProxy != NULL)
    Q_SAFE_DELETE(m_WizardProxy);
}

void TWizardViewerFrame::SetRunWizardEvent(TRunWizardCallback RunWizardEvent)
{
  // Sanity check
  if(m_WizardProxy != NULL)
    m_WizardProxy->SetRunWizardEvent(RunWizardEvent);
}

//---------------------------------------------------------------------------
__fastcall TWizardViewerFrame::TWizardViewerFrame(TComponent* Owner)
        : TFrame(Owner),m_AutoWizard(NULL),isNextCancelPage(false)
{
	NextButton->Anchors   = TAnchors() << akRight << akBottom;
	DoneButton->Anchors   = TAnchors() << akRight << akBottom;
	PrevButton->Anchors   = TAnchors() << akRight << akBottom;
	CancelButton->Anchors = TAnchors() << akRight << akBottom;
	HelpButton->Anchors   = TAnchors() << akLeft  << akBottom;
	UserButton1->Anchors  = TAnchors() << akLeft  << akBottom;
	UserButton2->Anchors  = TAnchors() << akLeft  << akBottom;
}

//---------------------------------------------------------------------------

// Destructor
__fastcall TWizardViewerFrame::~TWizardViewerFrame(void)
{
	UnRegisterCustomPageViewers();
}

// Help function that deletes all the controls from a given win control
void TWizardViewerFrame::CleanUpWinControl(TWinControl *WinControl)
{
  while(WinControl->ControlCount > 0)
  {
    TControl *Con = WinControl->Controls[0];

    // If the control is a win control, call for recursive cleanup
    TWinControl *WinCon = dynamic_cast<TWinControl *>(Con);

    if(WinCon != NULL)
      CleanUpWinControl(WinCon);

    WinControl->RemoveControl(Con);

    delete Con;
  }
}

// Register a custom wizard page viwer
void TWizardViewerFrame::RegisterCustomPageViewer(TWizardPageType PageType,CCustomWizardPageViewer *WizardPageViewer)
{
  m_CustomPageViewers[PageType] = WizardPageViewer;
}

// Unregister all custom wizard page viewers
void TWizardViewerFrame::UnRegisterCustomPageViewers(void)
{
  // Delete registered page viewers
  for(TCustomPageViewersList::iterator i = m_CustomPageViewers.begin(); i != m_CustomPageViewers.end(); ++i)
  {
    CCustomWizardPageViewer *PageViewer = (*i).second;

	Q_SAFE_DELETE(PageViewer);
  }

  m_CustomPageViewers.clear();
}

bool TWizardViewerFrame::RunWizard(CAutoWizard *AutoWizard,int StartingPage)
{
  m_AutoWizard = AutoWizard;

  m_WizardProxy->SetCurrentWizard(AutoWizard);
  
  RegisterCustomPageViewer(wptElapsingTime, new CTimeElapsingPageViewer);

  // Set all custom page parent property
  for(TCustomPageViewersList::iterator i = m_CustomPageViewers.begin(); i != m_CustomPageViewers.end(); ++i)
    ((*i).second)->SetParentWizard(AutoWizard);

  UserButton1->Caption = AutoWizard->GetUserButton1Caption().c_str();
  UserButton2->Caption = AutoWizard->GetUserButton2Caption().c_str();

  m_CurrentPage = NULL;

  // Attach a page refresh callback to the wizard
  m_AutoWizard->SetPageRefreshCallback(PageRefreshCallback,reinterpret_cast<TGenericCockie>(this));

  // Attach a UI handling callback
  m_AutoWizard->SetUIControlCallback(UIControlCallback,reinterpret_cast<TGenericCockie>(this));

  if (0 == StartingPage) // starting new wizard session
  	m_AutoWizard->ResetAllPages();

  // Start session
  if(!m_AutoWizard->Start())
    return false;

  return Restart(StartingPage);
}

void TWizardViewerFrame::SetTitle(AnsiString Title)
{
  // Use the hidden label (with an AutoSize property set to true) in order to measure the
  // required size of the title label.
  HiddenTitleLabel->Caption = Title;

  // If the width is wider then the label width, go to two lines mode
  if(HiddenTitleLabel->Width > TitleLabel->Width)
  {
    TitleLabel->Height = 52;

    if(HiddenTitleLabel->Width > TitleLabel->Width * 2)
      TitleLabel->Font->Size = 14;
    else
      TitleLabel->Font->Size = 18;
  } else
    {
      TitleLabel->Height = 26;
      TitleLabel->Font->Size = 18;
    }

  WizardPageControl->Height = (HiddenTitleLabel->Width > TitleLabel->Width)? 164 : 190;

  TitleLabel->Caption = Title;
}

bool TWizardViewerFrame::SetSubTitle(TLabel *SubTitleLabel,AnsiString SubTitle,bool GrowDown)
{
  bool Grow = false;

  // Use the hidden label (with an AutoSize property set to true) in order to measure the
  // required size of the subtitle label.
  HiddenSubTitleLabel->Caption = SubTitle;

  // If the width is wider then the label width, go to two lines mode
  if(HiddenSubTitleLabel->Width > SubTitleLabel->Width)
  {
    SubTitleLabel->Height = 32;
    SubTitleLabel->Top = SubTitleLabel->Tag + (GrowDown ? 0 : -16);
    Grow = true;
  } else
    {
      SubTitleLabel->Height = 16;
      SubTitleLabel->Top = SubTitleLabel->Tag;
    }

  SubTitleLabel->Caption = SubTitle;
  return Grow;
}

// Show a specific wizard page
bool TWizardViewerFrame::UpdateCurrentPageNoShow(CWizardPage *WizardPage)
{
  //if the page is done page show it anyway
  if (m_AutoWizard->IsDonePage(WizardPage))
     ShowPage(WizardPage);

  m_CurrentPage = WizardPage;
  // Call the "page enter" events
  if(!WizardPage->Enter())
    m_AutoWizard->Error();

  // Check if operation was cancelled
  if(m_AutoWizard->IsCancelled())
  {
    if(OnCancel)
      OnCancel(this);
    return false;
  }
  return true;
}
bool TWizardViewerFrame::UpdateImage(CWizardPage *WizardPage)
{
  int     ImageID;
  m_CurrentPage = WizardPage;
  if((ImageID = WizardPage->GetImageID()) == -1)
    ImageID = m_AutoWizard->GetDefaultImageID();
  QString ImageName = GetPageImage(ImageID);
 
  if(FileExists(ImageName.c_str()))
  {
	try
	{
	  PageImage->Picture->LoadFromFile(ImageName.c_str());
	  PageImage->Visible = true;
	  PageImage->Repaint();
	  return true;
	}
	catch(...)
	{
	  PageImage->Visible = false;
	}
  }
  else
  {
	PageImage->Visible = false;
  }

  return false;
}
bool TWizardViewerFrame::ShowPage(CWizardPage *WizardPage)
{
  PageWizardTimer->Interval = WizardPage->GetTimeout();
  EnableWizardPageTimeout(WizardPage->GetTimeoutEnable());

  int     ImageID;
  m_CurrentPage = WizardPage;
  SetTitle(WizardPage->Title.c_str());
  // If no image is assigned to the page, use the default image ID
  if((ImageID = WizardPage->GetImageID()) == -1)
    ImageID = m_AutoWizard->GetDefaultImageID();
  QString ImageName = GetPageImage(ImageID);

  if(FileExists(ImageName.c_str()))
  {
	try
	{
	  PageImage->Picture->LoadFromFile(ImageName.c_str());
	  PageImage->Visible = true;
	  PageImage->Repaint();
	}
	catch(...)
	{
	  PageImage->Visible = false;
	}
  }
  else
  {
	PageImage->Visible = false;
  }

  // Set the page "hint"
  PageImage->Hint = (m_AutoWizard->GetTitle() + "\n\rPage #" + QIntToStr(WizardPage->GetPageNumber())).c_str();

  // Enable/disable cancel button according to the page attributes

  DoneButton->Visible = m_AutoWizard->IsDonePage(WizardPage);
  DoneButton->Enabled = true;

  NextButton->Visible = !(DoneButton->Visible);
  NextButton->Enabled = !((WizardPage->GetAttributes() & wpNextDisabled) ||
                         (WizardPage->GetAttributes() & wpNextWhenSelected) ||
                         (WizardPage->GetAttributes() & wpNextOneChecked));

  HelpButton->Visible = (WizardPage->IsHelpVisible() && m_AutoWizard->IsHelpVisible());
  HelpAction->Enabled = true;

  //No use in cancel if we're on the done page
  CancelButton->Visible = true;
  CancelButton->Enabled = ((DoneButton->Visible) ? false : !(WizardPage->GetAttributes() & wpCancelDisabled));

  PrevButton->Visible = true;
  PrevButton->Enabled = ((WizardPage->GetPageNumber() > 0) ? !(WizardPage->GetAttributes() & wpPreviousDisabled) : false);

  // Set user buttons visibility and enabled state
  UserButton1->Visible = LogicXor(m_AutoWizard->IsUserButton1Visible(),
                                  (WizardPage->GetAttributes() & wpFlipButton1Visible));
  UserButton1->Enabled = !(WizardPage->GetAttributes() & wpUserButton1Disabled);

  UserButton2->Visible = LogicXor(m_AutoWizard->IsUserButton2Visible(),
                                  (WizardPage->GetAttributes() & wpFlipButton2Visible));
  UserButton2->Enabled = !(WizardPage->GetAttributes() & wpUserButton2Disabled);

  // Prepare the page according to its type
  bool NoRefreshOnShow = (WizardPage->GetAttributes() & wpNoRefreshOnShow);

  switch(WizardPage->GetPageType())    // TODO -cC++ -oArcady.Volman@objet.com : if polymorphic PrepareXXXPage is not possible, at least,dynamic_cast should be used instead of this check
  {
    case wptMessage:
      PrepareMessagePage(dynamic_cast<CMessageWizardPage *>(WizardPage));
      break;

    case wptStatus:
      PrepareStatusPage(dynamic_cast<CStatusWizardPage *>(WizardPage));
      break;

    case wptProgress:
      PrepareProgressPage(dynamic_cast<CProgressWizardPage *>(WizardPage));
      break;

    case wptCheckbox:
      PrepareCheckboxesPage(dynamic_cast<CCheckBoxWizardPage *>(WizardPage));
      break;

    case wptRadioGroup:
      PrepareRadioGroupPage(dynamic_cast<CRadioGroupWizardPage *>(WizardPage));
      break;

    case wptProgressStatus:
      PrepareProgressStatusPage(dynamic_cast<CProgressStatusWizardPage *>(WizardPage));
      break;

    case wptDataEntry:
      PrepareDataEntryPage(dynamic_cast<CDataEntryWizardPage *>(WizardPage));
      break;

 	case wptElapsingTime:
      PrepareCustomPage(WizardPage->GetPageType(),WizardPage);
      break;

    default:
      PrepareCustomPage(WizardPage->GetPageType(),WizardPage);
      NoRefreshOnShow = true;
  }

  if (!NoRefreshOnShow)
     WizardPage->Refresh();

  // Call the "page enter" events
  if(!WizardPage->Enter())
    m_AutoWizard->Error();

  // Check if operation was cancelled
  if(m_AutoWizard->IsCancelled())
  {
    if(OnCancel)
      OnCancel(this);

    return false;      
  }
  // Canceled page
  if((WizardPage->GetAttributes() & wpCancelPage))
  {
    PleaseWaitNotificationOn(true);
    m_AutoWizard->Cancel();
    PleaseWaitNotificationOn(false);
  }
  // End session
  else if(m_AutoWizard->IsDonePage(WizardPage))
  {
    PleaseWaitNotificationOn(true);
    bool isOnCancel = ((!m_AutoWizard->End()) && OnCancel);
    PleaseWaitNotificationOn(false);
    if (isOnCancel)
      OnCancel(this);
  }
  if((WizardPage->GetAttributes() & wpSelfClosingPage))
    DoneButtonClick(DoneButton);
  return true;
}

void TWizardViewerFrame::PleaseWaitNotificationOn(bool OnOff)
{
   PleaseWaitLabel->Visible = OnOff;
   DoneButton->Enabled = !OnOff;
   MainPanel->Visible = !OnOff;
   Refresh();
}

void TWizardViewerFrame::SetDefaultFrameLayout()
{
	// All values are as designed in DFM

    // Frame size
	this->Height = 294;
	this->Width  = 517;

	this->Parent->Height = 326;
	this->Parent->Width  = 523;

    // Image at the left side of the dialog
	this->PageImage->Height  = 230;
	this->PageImage->Width   = 120;
	this->PageImage->Top     = 8;
	this->PageImage->Left    = 11;
	this->PageImage->Stretch = false;

    // Panel that holds page controls
	this->MainPanel->BevelOuter = bvRaised;
	this->MainPanel->Left       = 144;
	this->MainPanel->Top        = 8;
	this->MainPanel->Height     = 231;
	this->MainPanel->Width      = 365;

	// Page control
	this->WizardPageControl->Left   = 1;
	this->WizardPageControl->Top    = 40;
	this->WizardPageControl->Height = (HiddenTitleLabel->Width > TitleLabel->Width) ? 164 : 190;
	this->WizardPageControl->Width  = 363;

    // Line above buttons
	this->Bevel1->Left   = 6;
	this->Bevel1->Top    = 248;
	this->Bevel1->Height = 9;
	this->Bevel1->Width  = 503;
}

void TWizardViewerFrame::PrepareMessagePage(CMessageWizardPage *PagePtr)
{  
  MessageLabel->Caption = PagePtr->SubTitle.c_str();

  // Select message page
  WizardPageControl->ActivePageIndex = MESSAGE_PAGE_INDEX;

  EnableDisableNext(!(PagePtr->GetAttributes() & wpNextDisabled));

  SetDefaultFrameLayout();
}

void TWizardViewerFrame::PrepareStatusPage(CStatusWizardPage *PagePtr)
{
  //StatusPanel->Caption = PagePtr->StatusMessage.c_str();
  StatusLabel->Caption = PagePtr->StatusMessage.c_str();

  SetSubTitle(StatusPageSubTitle,PagePtr->SubTitle.c_str());

  // Select status page
  WizardPageControl->ActivePageIndex = STATUS_PAGE_INDEX;

  EnableDisableNext(!(PagePtr->GetAttributes() & wpNextDisabled));

  SetDefaultFrameLayout();
}

void TWizardViewerFrame::PrepareProgressPage(CProgressWizardPage *PagePtr)
{
  ProgressIndicator->MinValue = PagePtr->Min;
  ProgressIndicator->MaxValue = PagePtr->Max;
  ProgressIndicator->Progress = PagePtr->Min;

  SetSubTitle(ProgressPageSubTitle,PagePtr->SubTitle.c_str());

  // Select progress page
  WizardPageControl->ActivePageIndex = PROGRESS_PAGE_INDEX;

  EnableDisableNext(!(PagePtr->GetAttributes() & wpNextDisabled));

  SetDefaultFrameLayout();
}

void TWizardViewerFrame::PrepareCheckboxesPage(CCheckBoxWizardPage *PagePtr)
{
	m_ProgressiveDisclosureControl = NULL;
	CleanUpWinControl(CheckBoxesContainer);
	m_FirstDisclosedCheckBoxIndex = (PagePtr->Strings.Count() - PagePtr->GetNumOfDisclosedElements());
	if (PagePtr->GetAttributes() & wpClearOnShow)
		PagePtr->SetChecksMask(0);

	for(unsigned i = 0; i < PagePtr->Strings.Count(); i++)
	{
		int boldIndex =  PagePtr->GetBoldIndex();
		// Create a checkbox and initialize properties
		TCheckBox *CheckBox = new TCheckBox((TComponent *)NULL);  // todo -oNobody -cNone: possible memory leak
		CheckBox->Caption = PagePtr->Strings[i].c_str();
		CheckBox->Tag     = i;
		CheckBox->Left    = 10;
		CheckBox->Width   = 264;
		CheckBox->Parent  = CheckBoxesContainer;

		CheckBox->Checked = ((PagePtr->GetChecksMask() & (1 << i)) != 0);

		CheckBox->OnClick = CheckBoxClickHandler;
		if(boldIndex != 0)
		{
			if((boldIndex & (1 << i)) != 0)
				CheckBox->Font->Style = TFontStyles()<< fsBold;
		}
		if (i >= m_FirstDisclosedCheckBoxIndex)
			CheckBox->Top = CheckBox->Height * 2 + i * (CheckBox->Height + 5);
		else
			CheckBox->Top = 10 + i * (CheckBox->Height + 5);

		if (i == m_FirstDisclosedCheckBoxIndex)
		{
			// Insert a separator line, to get a cleaner look
			TPanel *SepLine       = new TPanel((TComponent *)NULL);
			SepLine->Top          = CheckBox->Top - CheckBox->Height;
			SepLine->Height       = 3;
			SepLine->Left         = 8;
			SepLine->Width        = 220;
			SepLine->Parent       = CheckBoxesContainer;

			// Inject the Progressive disclosure control:
			m_ProgressiveDisclosureControl          = new TBitBtn((TComponent *)NULL);
			m_ProgressiveDisclosureControl->Enabled = true;
			m_ProgressiveDisclosureControl->Visible = true;
			m_ProgressiveDisclosureControl->Parent  = CheckBoxesContainer;
			m_ProgressiveDisclosureControl->Top     = (SepLine->Top + SepLine->Height / 2) - m_ProgressiveDisclosureControl->Height / 2;

			if ((PagePtr->GetDisclosureDefaultState() == PROGRESSIVE_DISCLOSURE_EXPANDED))
			{
				m_ProgressiveDisclosureControl->Action  = ProgressiveDisclosureExpandAction;
				m_ProgressiveDisclosureControl->OnClick = ProgressiveDisclosureExpandActionExecute;
			}
			else
			{
				m_ProgressiveDisclosureControl->Action  = ProgressiveDisclosureCollapseAction;
				m_ProgressiveDisclosureControl->OnClick = ProgressiveDisclosureCollapseActionExecute;
			}

			m_ProgressiveDisclosureControl->Tag     = -1;
			m_ProgressiveDisclosureControl->Left    = SepLine->Width + 18;
			m_ProgressiveDisclosureControl->Width   = 27;
			m_ProgressiveDisclosureControl->Height  = 25;
		}

	// If at least one Checkbox that belongs to the disclosed part is checked, then show the page in the 'Expanded' view.
		if ((i >= m_FirstDisclosedCheckBoxIndex) && (CheckBox->Checked))
	{
				m_ProgressiveDisclosureControl->Action  = ProgressiveDisclosureExpandAction;
				m_ProgressiveDisclosureControl->OnClick = ProgressiveDisclosureExpandActionExecute;
	}
	}

	if (m_ProgressiveDisclosureControl)
	m_ProgressiveDisclosureControl->Click();

	if(SetSubTitle(CheckBoxPageSubTitle,PagePtr->SubTitle.c_str(),true))
		CheckBoxesContainer->Top = CheckBoxesContainer->Tag + 16;
	else
		CheckBoxesContainer->Top = CheckBoxesContainer->Tag;

	CheckBoxesContainer->Height = WizardPageControl->Height - CheckBoxesContainer->Top - 16;

	// Select checkboxes page
	WizardPageControl->ActivePageIndex = CHECKBOXES_PAGE_INDEX;

	// If the page has an automatic next mode...
	if(PagePtr->GetAttributes() & wpNextWhenSelected) // todo -oNobody -cNone: rename to  wpNextWhenAllSelected
	{
		EnableDisableNext(~(0xFFFFFFFF << PagePtr->Strings.Count()) == PagePtr->GetChecksMask());
	}
	else
	if(PagePtr->GetAttributes() & wpNextOneChecked)
		EnableDisableNext(0 != PagePtr->GetChecksMask());
	else
	if(PagePtr->GetAttributes() & wpNextDisabled)
		EnableDisableNext(false);
	else
		EnableDisableNext(true);

	if (PagePtr->Strings.Count() && CheckBoxesContainer->Controls[0])
		dynamic_cast<TWinControl &>(*CheckBoxesContainer->Controls[0]).SetFocus(); // todo -oNobody -cNone: see why throws exceptions.

	SetDefaultFrameLayout();
}

void TWizardViewerFrame::PrepareRadioGroupPage(CRadioGroupWizardPage *PagePtr)
{
  // Clear current set of radio items
  RadioGroup->Items->Clear();

  const int NumberOfItems  = PagePtr->Strings.Count();

  const int MaxRadioGroupHeight = 0.85f * (float)RadioGroupPage->Height;

  // Set the height of the radio group according to the number of elements
  RadioGroup->Height = min(MaxRadioGroupHeight,10 + (30 * NumberOfItems));

  RadioGroup->OnClick = RadioGroupClickHandler;

  for(int i = 0; i < NumberOfItems; i++)
  {
    RadioGroup->Items->Add(PagePtr->Strings[i].c_str());
    dynamic_cast<TWinControl &>(*RadioGroup->Controls[i]).Enabled = !((PagePtr->GetDisabledMask() & (1 << i)) != 0); //disable needed rows
  }

  RadioGroup->ItemIndex = PagePtr->DefaultOption;

  if(SetSubTitle(RadioGroupPageSubTitle,PagePtr->SubTitle.c_str(),true))
    RadioGroup->Top = RadioGroup->Tag + 32;
  else
    RadioGroup->Top = RadioGroup->Tag;

  // Select radiogroup page
  WizardPageControl->ActivePageIndex = RADIOGROUP_PAGE_INDEX;

  // If the page has an automatic next mode...
  if(PagePtr->GetAttributes() & wpNextWhenSelected)
  {
    EnableDisableNext(PagePtr->DefaultOption != -1);

  } else
      EnableDisableNext(!(PagePtr->GetAttributes() & wpNextDisabled));

  if (RadioGroup->ItemIndex != -1)
    if (NumberOfItems && RadioGroup->Controls[RadioGroup->ItemIndex])
		dynamic_cast<TWinControl &>(*RadioGroup->Controls[RadioGroup->ItemIndex]).SetFocus();

  SetDefaultFrameLayout();
}

void TWizardViewerFrame::PrepareProgressStatusPage(CProgressStatusWizardPage *PagePtr)
{
  ProgressStatusProgressIndicator->MinValue = PagePtr->Min;
  ProgressStatusProgressIndicator->MaxValue = PagePtr->Max;
  ProgressStatusProgressIndicator->Progress = PagePtr->Min;

  SetSubTitle(ProgressStatusPageSubTitle,PagePtr->SubTitle.c_str());

  ProgressStatusStatusPanel->Caption = PagePtr->StatusMessage.c_str();

  PagePtr->CurrentMode = PagePtr->DefaultMode; 

  if(PagePtr->CurrentMode == CProgressStatusWizardPage::psmProgress)
  {
    ProgressStatusProgressIndicator->Visible = true;
    ProgressStatusStatusPanel->Visible = false;
  } else
    {
      ProgressStatusProgressIndicator->Visible = false;
      ProgressStatusStatusPanel->Visible = true;
    }

  // Select progress page
  WizardPageControl->ActivePageIndex = PROGRESS_STATUS_PAGE_INDEX;

  EnableDisableNext(!(PagePtr->GetAttributes() & wpNextDisabled));

  SetDefaultFrameLayout();
}

void TWizardViewerFrame::PrepareCustomPage(TWizardPageType PageType,CWizardPage *WizardPage)
{
  // Search for a registered wizard page viewer
   TCustomPageViewersList::iterator i = m_CustomPageViewers.find(PageType);

  // If found
  if(i != m_CustomPageViewers.end())
  {
    // Clean current tab sheet content
    CleanUpWinControl(CustomTabSheet);

    (*i).second->SetActivePage(WizardPage);
    (*i).second->Prepare(CustomTabSheet,WizardPage);

    if (WizardPage->GetAttributes() & wpNextDisabled)
       EnableDisableNext(false);
  }
  else
      MessageDlg("Custom wizard page could not be found",mtError,TMsgDlgButtons() << mbOK,0);

  // Select custom page
  WizardPageControl->ActivePageIndex = CUSTOM_PAGE_INDEX;
}

void TWizardViewerFrame::PrepareDataEntryPage(CDataEntryWizardPage *PagePtr)
{
  CleanUpWinControl(DataEntryFieldsContainer);
  bool ButtonEnable = true;
  for(unsigned i = 0; i < PagePtr->Strings.Count(); i++)
  {
    TLabeledEdit *Field       = new TLabeledEdit((TComponent *)NULL);
    Field->Tag                = i;
    Field->EditLabel->Caption = PagePtr->Strings[i].c_str();
    Field->Left               = 10;
    Field->Top                = 24 + i * 42;
    Field->Width              = 250;
    Field->Parent             = DataEntryFieldsContainer;
    Field->OnChange           = DataEntryChangeHandler;
    Field->Text               = PagePtr->FieldsValues[i].c_str();
    ButtonEnable             &= Field->Text.AnsiCompare("");
  }

  if(SetSubTitle(DataEntryPageSubTitle,PagePtr->SubTitle.c_str(),true))
    DataEntryFieldsContainer->Top = DataEntryFieldsContainer->Tag + 16;
  else
    DataEntryFieldsContainer->Top = DataEntryFieldsContainer->Tag;

  DataEntryFieldsContainer->Height = WizardPageControl->Height - DataEntryFieldsContainer->Top - 16;

  // Select data entry page
  WizardPageControl->ActivePageIndex = DATA_ENTRY_PAGE_INDEX;
  EnableDisableNext(!(PagePtr->GetAttributes() & wpNextDisabled) & ButtonEnable);

  SetDefaultFrameLayout();
}

// Refresh a wizard page
void TWizardViewerFrame::RefreshPage(CWizardPage *WizardPage)
{
	// Refresh the title
	SetTitle(WizardPage->Title.c_str());

	// Process the page according to its type
	switch(WizardPage->GetPageType())
	{
	case wptMessage:
		RefreshMessagePage(dynamic_cast<CMessageWizardPage *>(WizardPage));
		break;

	case wptStatus:
		RefreshStatusPage(dynamic_cast<CStatusWizardPage *>(WizardPage));
		break;

	case wptProgress:
		RefreshProgressPage(dynamic_cast<CProgressWizardPage *>(WizardPage));
		break;

	case wptCheckbox:
		RefreshCheckboxesPage(dynamic_cast<CCheckBoxWizardPage *>(WizardPage));
		break;

	case wptRadioGroup:
		RefreshRadioGroupPage(dynamic_cast<CRadioGroupWizardPage *>(WizardPage));
		break;

	case wptProgressStatus:
		RefreshProgressStatusPage(dynamic_cast<CProgressStatusWizardPage *>(WizardPage));
		break;

	case wptDataEntry:
		RefreshDataEntryPage(dynamic_cast<CDataEntryWizardPage *>(WizardPage));
		break;	

	default:
		RefreshCustomPage(WizardPage->GetPageType(),WizardPage);
	}

	// Make sure that the refresh will be visible
	Application->ProcessMessages();
}

void TWizardViewerFrame::RefreshMessagePage(CMessageWizardPage *PagePtr)
{
  MessageLabel->Caption = PagePtr->SubTitle.c_str();
}

void TWizardViewerFrame::RefreshStatusPage(CStatusWizardPage *PagePtr)
{
  //StatusPanel->Caption = PagePtr->StatusMessage.c_str();
  StatusLabel->Caption = PagePtr->StatusMessage.c_str();

  StatusPageSubTitle->Caption = PagePtr->SubTitle.c_str();
}

void TWizardViewerFrame::RefreshProgressPage(CProgressWizardPage *PagePtr)
{
  ProgressPageSubTitle->Caption = PagePtr->SubTitle.c_str();
  ProgressIndicator->Progress = PagePtr->Progress;
}

void TWizardViewerFrame::RefreshCheckboxesPage(CCheckBoxWizardPage *PagePtr)
{
  PrepareCheckboxesPage(PagePtr);
}

void TWizardViewerFrame::RefreshRadioGroupPage(CRadioGroupWizardPage *PagePtr)
{
  PrepareRadioGroupPage(PagePtr);
}

void TWizardViewerFrame::RefreshProgressStatusPage(CProgressStatusWizardPage *PagePtr)
{
  ProgressStatusPageSubTitle->Caption = PagePtr->SubTitle.c_str();
  ProgressStatusProgressIndicator->Progress = PagePtr->Progress;
  ProgressStatusStatusPanel->Caption = PagePtr->StatusMessage.c_str();

  if(PagePtr->CurrentMode == CProgressStatusWizardPage::psmProgress)
  {
    ProgressStatusProgressIndicator->Visible = true;
    ProgressStatusStatusPanel->Visible = false;
  } else
    {
      ProgressStatusProgressIndicator->Visible = false;
      ProgressStatusStatusPanel->Visible = true;
    }
}

void TWizardViewerFrame::RefreshCustomPage(TWizardPageType PageType,CWizardPage *WizardPage)
{
  // Search for a registered wizard page viewer
  TCustomPageViewersList::iterator i = m_CustomPageViewers.find(PageType);
  // If found
  if(i != m_CustomPageViewers.end())
  {
	(*i).second->SetActivePage(WizardPage);
	(*i).second->Refresh(CustomTabSheet,WizardPage);
  }
  else
    MessageDlg("Custom wizard page could not be found",mtError,TMsgDlgButtons() << mbOK,0);
}

void TWizardViewerFrame::RefreshDataEntryPage(CDataEntryWizardPage *PagePtr)
{
  TLabeledEdit *FirstLEdit = NULL;
  DataEntryPageSubTitle->Caption = PagePtr->SubTitle.c_str();
  bool ButtonEnable = true;

  for(int i = 0; i < (int)PagePtr->Strings.Count(); i++)
  {
    // Find the labeled edit component according to its tag
    for(int j = 0; j < DataEntryFieldsContainer->ControlCount; j++)
    {
      TLabeledEdit *LEdit = dynamic_cast<TLabeledEdit *>(DataEntryFieldsContainer->Controls[j]);

      if(LEdit)
      {
        // Saving the handle to the first data field
        if(LEdit->Tag == 0)
          FirstLEdit = LEdit;

        if(LEdit->Tag == i)
        {
          LEdit->Text = PagePtr->FieldsValues[i].c_str();
          ButtonEnable &= LEdit->Text.AnsiCompare("");
          LEdit->EditLabel->Caption = PagePtr->Strings[i].c_str();
          break;
        }
      }
    }

    if (FirstLEdit->CanFocus())
      FirstLEdit->SetFocus();
  }

  EnableDisableNext(ButtonEnable);
}

void __fastcall TWizardViewerFrame::NextButtonClick(TObject *Sender)
{
  TWizardPageLeaveReason LeaveReason = (isNextCancelPage ? lrCanceled : lrGoNext);
  isNextCancelPage = false;
  if(!LeaveCurrentPage(LeaveReason)) // this calls the leave event of the page
  {
    if (!m_AutoWizard->IsCancelled())
      m_AutoWizard->Cancel();
    if(OnCancel)
      OnCancel(this);
    return;
  }
  if(m_AutoWizard->IsCancelled())
  {
    if(OnCancel)
      OnCancel(this);
  }
  else
  {
      CWizardPage *Page = m_AutoWizard->GotoNextPageNoEvents();
      if(Page != NULL)
      { //If page is allows to enter, show
        if (Page->PreEnter())
           ShowPage(Page); // this also calls the enter event of the page.
        else //otherwise skip the page
        {
           UpdateCurrentPageNoShow(Page);
           if (!m_AutoWizard->IsStop())
              NextButtonClick(Sender);
        }
      }
  }
}

void __fastcall TWizardViewerFrame::PrevButtonClick(TObject *Sender)
{
  if(!LeaveCurrentPage(lrGoPrevious))
  {
    m_AutoWizard->Error();

    if(OnCancel)
      OnCancel(this);
  }

  // Check if the wizard was cancelled
  if(m_AutoWizard->IsCancelled())
  {
    if(OnCancel)
      OnCancel(this);
  }
  else
  {
      CWizardPage *Page = m_AutoWizard->GotoPreviousPageNoEvents();

      if(Page != NULL)
      { //If page is allows to enter, show
        if (Page->PreEnter())
           ShowPage(Page); // this also calls the enter event of the page.
        else //otherwise skip the page
        {
           UpdateCurrentPageNoShow(Page);
           PrevButtonClick(Sender);
        }
      }
  }
}
//---------------------------------------------------------------------------

void TWizardViewerFrame::PageRefreshCallback(CWizardPage *WizardPage,TGenericCockie Cockie)
{
  TWizardViewerFrame *FramePtr = reinterpret_cast<TWizardViewerFrame *>(Cockie);
  FramePtr->UpdateImage(WizardPage);
  FramePtr->RefreshPage(WizardPage);
}

void TWizardViewerFrame::UIControlCallback(TWizardUIControlType ControlType,bool Enabled,TGenericCockie Cockie)
{
  TWizardViewerFrame *FramePtr = reinterpret_cast<TWizardViewerFrame *>(Cockie);

  switch(ControlType)
  {
    case wuiEnableNext:
      FramePtr->EnableDisableNext(Enabled);
      break;

    case wuiEnablePrevious:
      FramePtr->PrevButton->Enabled = Enabled;
      break;

    case wuiEnableCancel:
      FramePtr->CancelButton->Enabled = Enabled;
      break;

    case wuiEnableHelp:
      FramePtr->HelpAction->Enabled = Enabled;
      break;

    case wuiEnableUserButton1:
      FramePtr->UserButton1->Enabled = Enabled;
      break;

    case wuiEnableUserButton2:
      FramePtr->UserButton2->Enabled = Enabled;
      break;
      
    case wuiCancelNext:
      FramePtr->isNextCancelPage = true;
    case wuiGotoNext:
      if(FramePtr->NextButton->Visible)
        FramePtr->NextButton->Click();
      else
        FramePtr->DoneButton->Click();
      break;

    case wuiGotoPrevious:
      FramePtr->PrevButton->Click();
      break;

    case wuiGotoNextAsync:
      FramePtr->GotoNextPageAsync();
  // When we post an Async message we don't want to ProcessMessages(), which will actually make it equivalent to a Synchronous call.
      return;
  }

  Application->ProcessMessages();
}

void __fastcall TWizardViewerFrame::DoneButtonClick(TObject *Sender)
{
  DoneButton->Enabled = false;
  //No LeaveCurrentPage from wpDonePage (there is CloseEvent for that purpose)
  //TNotifyEvent NotifyEvent = (LeaveCurrentPage(lrGoNext) ? OnDone : OnCancel);
  if(OnDone)
    OnDone(this);

  m_AutoWizard->Close();
}
//---------------------------------------------------------------------------

void __fastcall TWizardViewerFrame::CheckBoxClickHandler(TObject *Sender)
{
  assert(m_CurrentPage != NULL);

  TCheckBox *ChkBox = dynamic_cast<TCheckBox *>(Sender);

  // Current page should be a checkboxes page
  CCheckBoxWizardPage *ChkPage = dynamic_cast<CCheckBoxWizardPage *>(m_CurrentPage);

  if(ChkPage)
  {
    // Notify the checkbox event
    if(!m_CurrentPage->NotifyEvent(ChkBox->Tag,ChkBox->Checked ? 1 : 0))
    {
      // Cancel wizard on error
      m_AutoWizard->Error();

      if(OnCancel)
        OnCancel(this);

    } else
      {
        // If the page is in automatic next enable mode, compare the number of checks with the total number
        // of checkboxes.
        if(ChkPage->GetAttributes() & wpNextWhenSelected)
            EnableDisableNext(~(0xFFFFFFFF << ChkPage->Strings.Count()) == ChkPage->GetChecksMask());
        else
          // If the page is in automatic next when at least one checked, check if the number of checks
          // is bigger then zero.
          if(ChkPage->GetAttributes() & wpNextOneChecked)
            EnableDisableNext(ChkPage->GetChecksMask() > 0);
      }
  }
}

bool TWizardViewerFrame::IsNegativeNumberPossible(TDataEntryFieldType DataEntryFieldType)
{
 return ((DataEntryFieldType == ftInt) || (DataEntryFieldType == ftFloat));
}

void __fastcall TWizardViewerFrame::DataEntryChangeHandler(TObject *Sender)
{
  TLabeledEdit* LabeledEdit = dynamic_cast<TLabeledEdit *>(Sender);
  int           FieldNum    = LabeledEdit->Tag;

  //Current page should be a data entry page
  CDataEntryWizardPage* Page = dynamic_cast<CDataEntryWizardPage *>(m_CurrentPage);

  if(Page && !m_AutoWizard->IsCancelled())
  {
    // Verify field according to it's type
    if((LabeledEdit->Text.AnsiCompare("-") == 0) && IsNegativeNumberPossible(Page->FieldsTypes[FieldNum]))
    {
       EnableDisableNext(false);
       return;
    }
    else if((LabeledEdit->Text.AnsiCompare("") == 0) ||
        CheckString(LabeledEdit->Text.c_str(),Page->FieldsTypes[FieldNum]))
    {
       Page->FieldsValues[FieldNum] = LabeledEdit->Text.c_str();
       Page->NotifyEvent(FieldNum,0);
    }
    else
       LabeledEdit->Text = Page->FieldsValues[FieldNum].c_str();

    bool ButtonEnable = true;

    for(unsigned i = 0; i < Page->Strings.Count(); i++)
    {
        ButtonEnable = ButtonEnable && (Page->FieldsValues[i] != "");
        ButtonEnable = ButtonEnable && (Page->FieldsValues[i] != "-");
    }

    EnableDisableNext(ButtonEnable);
  }
}

void __fastcall TWizardViewerFrame::RadioGroupClickHandler(TObject *Sender)
{
  assert(m_CurrentPage != NULL);

  // Current page should be a radio group page
  CRadioGroupWizardPage *RadioGroupPage = dynamic_cast<CRadioGroupWizardPage *>(m_CurrentPage);

  if(RadioGroupPage)
  {
    // Notify the checkbox event
    if(!m_CurrentPage->NotifyEvent(RadioGroup->ItemIndex,0))
    {
      // Cancel wizard on error
      m_AutoWizard->Error();

      if(OnCancel)
        OnCancel(this);
    }
    else
    {
        // If the page is in automatic next enable mode, compare the number of checks with the total number
        // of checkboxes.
        if(RadioGroupPage->GetAttributes() & wpNextWhenSelected)
          if(RadioGroup->ItemIndex != -1)
            EnableDisableNext(true);
    }
  }
}

void __fastcall TWizardViewerFrame::CancelButtonClick(TObject *Sender)
{
  if (MessageDlg(m_CurrentPage->CancelMessage.c_str(),mtConfirmation,
                 TMsgDlgButtons() << mbYes << mbNo,0) == mrYes)
  {
    PleaseWaitNotificationOn(true);
    LeaveCurrentPage(lrCanceled); 
    m_AutoWizard->Cancel();
    m_AutoWizard->Close();
    PleaseWaitNotificationOn(false);
    if(OnCancel)
      OnCancel(this);
  }
}
//---------------------------------------------------------------------------

// Close current session (The OnCancel event will be (might be) called)
void TWizardViewerFrame::CloseSession(void)
{
  if(!m_AutoWizard->IsCancelled() && !m_AutoWizard->IsDoneOk())
  {
    LeaveCurrentPage(lrCanceled);
    m_AutoWizard->Cancel();
  }
  PleaseWaitNotificationOn(false);
}

// Return true if a cancel operation is curently allowed
bool TWizardViewerFrame::IsCancelAllowed(void)
{
  if(m_AutoWizard)
    if(m_AutoWizard->IsCancelled())
      return true;

  // If no page is currently assigned
  if(m_CurrentPage == NULL)
    return true;

  return !(m_CurrentPage->GetAttributes() & wpCancelDisabled);
}

void TWizardViewerFrame::EnableDisableButtons(bool Enabled)
{
   EnableDisableNext(Enabled);
   PrevButton->Enabled = Enabled;
   CancelButton->Enabled = Enabled;
   HelpAction->Enabled = Enabled;
   UserButton1->Enabled = Enabled;
   UserButton2->Enabled = Enabled;
}

// Enable or disable the next/done buttons
void TWizardViewerFrame::EnableDisableNext(bool Enabled)
{
  NextButton->Enabled = Enabled;
  DoneButton->Enabled = Enabled;
}

// Return true if the wizard has finished sucessfully
bool TWizardViewerFrame::IsDoneOk(void)
{
  return m_AutoWizard->IsDoneOk();
}

void __fastcall TWizardViewerFrame::HelpActionExecute(TObject *Sender)
{
  m_AutoWizard->Help();
}
//---------------------------------------------------------------------------

// Restart the wizard sesssion
bool TWizardViewerFrame::Restart(int StartingPage)
{
  PrevButton->Enabled = false;
  NextButton->Visible = true;
  DoneButton->Visible = false;
  PleaseWaitLabel->Visible = false;
  
  m_AutoWizard->ClearHistory();  // todo -oNobody -cNone: maybe shouldn't clear on resume
  m_AutoWizard->SetNextPage(StartingPage);

  // Extract the first page
  CWizardPage *FirstPage = m_AutoWizard->GotoNextPageNoEvents();

  // If a page has been found
  if(FirstPage != NULL)
  {
    if(!ShowPage(FirstPage))
      return false;
  }
  else
  {
    // Show message page with "Empty wizard warning"
    TitleLabel->Caption = "Empty wizard error";
    MessageLabel->Caption = "";
    WizardPageControl->ActivePageIndex = MESSAGE_PAGE_INDEX;
    NextButton->Enabled = false;
  }

  return true;
}

void __fastcall TWizardViewerFrame::UserButton1Click(TObject *Sender)
{
  m_AutoWizard->FireUserButton1Event();
}
//---------------------------------------------------------------------------

void __fastcall TWizardViewerFrame::UserButton2Click(TObject *Sender)
{
  m_AutoWizard->FireUserButton2Event();
}
//---------------------------------------------------------------------------

// Leave the current page (return true if canceled)
bool TWizardViewerFrame::LeaveCurrentPage(TWizardPageLeaveReason LeaveReason)
{
  bool Ret = true;
  EnableWizardPageTimeout(false);
  EnableDisableButtons(false);

  try
  {
    if(m_CurrentPage)
    {
      // Is it a custom page?
      if(m_CurrentPage->GetPageType() >= wptElapsingTime)
      {
        // Search for a registered wizard page viewer
        TCustomPageViewersList::iterator i = m_CustomPageViewers.find(m_CurrentPage->GetPageType());

        // If found, call the page viewer leave function
        if(i != m_CustomPageViewers.end())
          (*i).second->Leave(CustomTabSheet,m_CurrentPage,LeaveReason);
      }

      // Call the "page leave" event
      Ret = m_CurrentPage->Leave(LeaveReason);
    }
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    QMonitor.ErrorMessage(QFormatStr("Custom wizard page throw exception: '%s' , (Code %d)",
                                     QException.GetErrorMsg().c_str(),QException.GetErrorCode()),ORIGIN_WIZARD_PAGE);

    Ret = false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    QMonitor.ErrorMessage("Custom wizard page throw unexpected exception",ORIGIN_WIZARD_PAGE);
    Ret = false;
  }

  return Ret;
}

bool TWizardViewerFrame::CheckString(const QString Str,TDataEntryFieldType UserValuesType)
{
  switch(UserValuesType)
  {
    case ftInt:
      return TypedStringChecker<int>(Str);

    case ftFloat:
      return TypedStringChecker<float>(Str);

    case ftUnsigned:
      return TypedStringChecker<unsigned>(Str);

    case ftString:
      return true;
  }
  
  return false;
}

template<class T>
bool TWizardViewerFrame::TypedStringChecker(const QString Str)
{
  if(Str.length() == 0)
    return false;

  try
  {
    QStrToValue<T>(Str);
    return true;

  } catch(...)
    {
      return false;
    }
}

void TWizardViewerFrame::PostCancelMessage()
{
  PostMessage(Handle,WM_WIZARD_MESSAGE,0/*Always the "Wizard_Control"*/,reinterpret_cast<int>(FE_WIZARD_CANCEL));
}

void TWizardViewerFrame::GotoNextPageAsync()
{
  PostMessage(Handle,WM_WIZARD_MESSAGE,0 /*unused*/ , reinterpret_cast<int>(FE_WIZARD_GOTOPAGEASYNC));
}


// Message handler for the WM_WIZARD_MESSAGE message
void TWizardViewerFrame::HandleStatusUpdateMessage(TMessage &Message)
{
   switch(Message.LParam)
   {
      case FE_WIZARD_CANCEL:
      {
        if(m_WizardProxy != NULL)
            m_WizardProxy->Cancel();
        break;
      }
      case FE_WIZARD_GOTOPAGEASYNC:
            m_WizardProxy->GotoNextPage();
      default:
      break;
   }
}

void __fastcall TWizardViewerFrame::PageWizardTimerTimer(TObject *Sender)
{
   EnableWizardPageTimeout(false);
   LeaveCurrentPage(lrCanceled);
   PostCancelMessage();
   ShowMessage((m_AutoWizard->GetTitle() + " has been canceled because of inactivity.\nAll related printer processes have been terminated.").c_str());
   while(!m_AutoWizard->IsCancelled())
     m_AutoWizard->YieldAndSleepWizardThread();
   m_AutoWizard->Close();
   if(OnCancel)
     OnCancel(this);

}
//---------------------------------------------------------------------------

void TWizardViewerFrame::EnableWizardPageTimeout(bool Enable)
{
  if (Enable == false)
  {
     PageWizardTimer->Enabled = Enable;
     return;
  }
  
  if (m_IsTimeOutMechanismEnable == false)
     return;

  if(!m_AutoWizard || m_AutoWizard->IsCancelled())
     return;

  PageWizardTimer->Enabled = Enable;
}

void __fastcall TWizardViewerFrame::ProgressiveDisclosureClick(TObject *Sender)
{
  TComponent * MyComponent;
  TCheckBox  * CheckBox;

  TBitBtn  * ProgDiscBitBtn = dynamic_cast<TBitBtn *>(Sender);
  if (!ProgDiscBitBtn)
    return;

  for (int i = 0; i < CheckBoxesContainer->ControlCount; i++)
  {
    MyComponent = CheckBoxesContainer->Controls[i];
    CheckBox    = dynamic_cast<TCheckBox *>(MyComponent);

    if (CheckBox == NULL)
      continue;

    if (static_cast<unsigned int>(CheckBox->Tag) >= m_FirstDisclosedCheckBoxIndex)
    {
      if (ProgDiscBitBtn->Action == ProgressiveDisclosureExpandAction)
      {
        CheckBox->Visible = true;
        CheckBox->Enabled = true;
      } else
      {
        CheckBox->Visible = false;
        CheckBox->Enabled = false;
      }
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TWizardViewerFrame::ProgressiveDisclosureCollapseActionExecute(
      TObject *Sender)
{
  if (!m_ProgressiveDisclosureControl)
    return;

  ProgressiveDisclosureClick(m_ProgressiveDisclosureControl);
  m_ProgressiveDisclosureControl->Action = ProgressiveDisclosureExpandAction;
  LOAD_BITMAP(m_ProgressiveDisclosureControl->Glyph, IDB_CHEVRONS_DOWN);
}
//---------------------------------------------------------------------------

void __fastcall TWizardViewerFrame::ProgressiveDisclosureExpandActionExecute(
      TObject *Sender)
{
  if (!m_ProgressiveDisclosureControl)
    return;

  ProgressiveDisclosureClick(m_ProgressiveDisclosureControl);
  m_ProgressiveDisclosureControl->Action = ProgressiveDisclosureCollapseAction;
  LOAD_BITMAP(m_ProgressiveDisclosureControl->Glyph, IDB_CHEVRONS_UP);
}
//---------------------------------------------------------------------------

void TWizardViewerFrame::ReplaceViewerFrame(TFrame* CustomizedFrame, bool ShowImage)
{
    CustomizedFrame->Parent = MainPanel;

	//There exist a minimum border width for the panel's left & top sides.
	//This margin is the invisible border's width for the custom frame's right & bottom sides.
	const int PANEL_MARGIN = 10; //[pixels]

	//update entire canvas' size
	this->Height = CustomizedFrame->Height + NextButton->Height + Bevel1->Height + PANEL_MARGIN * 2;
	this->Width  = CustomizedFrame->Width + ((ShowImage) ? this->PageImage->Width + PANEL_MARGIN : 0) + PANEL_MARGIN;

	//MainPanel occupies entire user-defined frame, If user wants to see a border - he should add it manually.
	this->MainPanel->BevelOuter = bvNone; //disable panel's border. originally: bvRaised

	int LeftPos = PANEL_MARGIN / 2;

	if(ShowImage)
	{
		LeftPos += this->PageImage->Left + this->PageImage->Width;
		this->PageImage->Height  = CustomizedFrame->Height - PANEL_MARGIN * 2;
		this->PageImage->Stretch = true;
		this->PageImage->Top     = PANEL_MARGIN; 
	}
	else
	{
		this->PageImage->Top += this->Height; //hide the fixed image;
	}
	
	this->MainPanel->SetBounds(LeftPos, 0, this->Width, CustomizedFrame->Height);

	//update gui controls' positions & size
	this->Bevel1->Top   = CustomizedFrame->Height;
	this->Bevel1->Width = CustomizedFrame->Width + ((ShowImage) ? this->PageImage->Width + PANEL_MARGIN : 0);

	dynamic_cast<TForm *>(this->Parent)->Position = poMainFormCenter;
}

CAutoWizard* TWizardViewerFrame::GetCurrentWizard()
{
	return m_AutoWizard;
}
