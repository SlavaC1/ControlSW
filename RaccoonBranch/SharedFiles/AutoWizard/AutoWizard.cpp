/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Base class implementation for automatic      *
 *                     wizards.                                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/01/2003                                           *
 * Last upate: 05/01/2003                                           *
 ********************************************************************/

#ifdef OS_WINDOWS
// Required for the "Application" instance
#include <forms.hpp>
#endif

#include "QMonitor.h"
#include "AutoWizard.h"
#include "WizardPages.h"
#include "QPythonIntegrator.h"


// Show on the monitor a QException error
static void ShowQException(EQException ExceptionObject,const QString& WizardName);

// Show on the monitor VCL exceptions
static void ShowVCLException(Exception& VCLException,const QString& WizardName);

// Show on the monitor a generic exception error
static void ShowUnexpectedException(const QString& WizardName);


// AutoWizard class implementation
// ****************************************************************************

int CAutoWizard::m_AppDefaultImageID = -1;
bool CAutoWizard::s_Cancelled = false;

// Constructor
CAutoWizard::CAutoWizard(const QString Title,bool HelpVisible,int DefaultImageID,
                         bool UserButton1Visible,bool UserButton2Visible,
                         QString UserButton1Caption,QString UserButton2Caption) : Pages(m_PageList)
{
  m_Title = Title;
  m_WizardUICallback = NULL;
  m_WizardUICockie = 0;
  m_DoneOk = false;
  m_NextPage = 0;
  m_CurrentPage = -1;
  m_HelpVisible = HelpVisible;

  // Choose defualt image ID
  if(DefaultImageID == -1)
    m_DefaultImageID = m_AppDefaultImageID;
  else
    m_DefaultImageID = DefaultImageID;

  m_UserButton1Visible = UserButton1Visible;
  m_UserButton2Visible = UserButton2Visible;
  m_UserButton1Caption = UserButton1Caption;
  m_UserButton2Caption = UserButton2Caption;

}

// Destructor
CAutoWizard::~CAutoWizard(void)
{
  // Delete all registered pages
  for(TPageList::iterator i = m_PageList.begin(); i != m_PageList.end(); ++i)
    delete (*i);
}

// Add a page to the wizard
void CAutoWizard::AddPage(CWizardPage *NewPage)
{
  // If the page hasn't been assigned an event handler, assign the default handler
  if(NewPage->m_EventHandler == NULL)
    NewPage->SetPageEventsCallback(PageEventsHandlerCallback,reinterpret_cast<TGenericCockie>(this));

  if(NewPage->m_PageEnterEvent == NULL)
    NewPage->SetPageEnterCallback(PageEnterCallback,reinterpret_cast<TGenericCockie>(this));

  if(NewPage->m_PageLeaveEvent == NULL)
    NewPage->SetPageLeaveCallback(PageLeaveCallback,reinterpret_cast<TGenericCockie>(this));

  if(NewPage->m_PageResetEvent == NULL)
    NewPage->SetPageResetCallback(PageResetCallback, reinterpret_cast<TGenericCockie>(this));

  NewPage->m_PageNumber = m_PageList.size();
  m_PageList.push_back(NewPage);
}

bool CAutoWizard::IsDonePage(CWizardPage *Page)
{
  return ((Page->GetAttributes() & wpDonePage) || (Page->GetPageNumber() == GetNumberOfPages() - 1));
}

void CAutoWizard::SetDefaultTimeout(int Timeout)
{
  for(TPageList::iterator i = m_PageList.begin(); i != m_PageList.end(); ++i)
    (*i)->SetDefaultTimeout(Timeout);
}

// Generic callback for page events
void CAutoWizard::PageEventsHandlerCallback(CWizardPage *WizardPage,int Param1,int Param2,TGenericCockie Cockie)
{
  CAutoWizard *InstancePtr = reinterpret_cast<CAutoWizard *>(Cockie);

  // Call the virtual event handler function
  InstancePtr->PageEventsHandler(WizardPage,Param1,Param2);
}

// Page enter callback
void CAutoWizard::PageEnterCallback(CWizardPage *WizardPage,TGenericCockie Cockie)
{
  CAutoWizard *InstancePtr = reinterpret_cast<CAutoWizard *>(Cockie);
  // Call the virtual event handler function
  InstancePtr->PageEnterWrapper(WizardPage);
}

void CAutoWizard::PageEnterWrapper(CWizardPage* WizardPage)
{
  PageEnter(WizardPage);
}

void CAutoWizard::PageLeaveWrapper(CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  PageLeave(WizardPage,LeaveReason);
}

// Page enter callback
void CAutoWizard::PageResetCallback(CWizardPage *WizardPage,TGenericCockie Cockie)
{
  CAutoWizard *InstancePtr = reinterpret_cast<CAutoWizard *>(Cockie);

  // Call the virtual event handler function
  InstancePtr->PageReset(WizardPage);
}

// Page leave callback
void CAutoWizard::PageLeaveCallback(CWizardPage *WizardPage,TGenericCockie Cockie,TWizardPageLeaveReason LeaveReason)
{
  CAutoWizard *InstancePtr = reinterpret_cast<CAutoWizard *>(Cockie);

  // Call the virtual event handler function
  InstancePtr->PageLeaveWrapper(WizardPage,LeaveReason);
}

// set a page refresh callback in all the registered pages
void CAutoWizard::SetPageRefreshCallback(TGenericWizardPageEvent PageRefreshCallback,TGenericCockie PageRefreshCockie)
{
  for(TPageList::iterator i = m_PageList.begin(); i != m_PageList.end(); ++i)
    (*i)->SetPageRefreshCallback(PageRefreshCallback,PageRefreshCockie);
}

void CAutoWizard::ResetAllPages(void)
{
  for(TPageList::iterator i = m_PageList.begin(); i != m_PageList.end(); ++i)
    (*i)->Reset();
}


// Start a wizard session
bool CAutoWizard::Start(void)
{
  s_Cancelled = false;
  m_DoneOk = false;
  m_Stop = false;
  m_CurrentPage = -1;
  m_NextPage = 0;

  ClearHistory();

  try
  {
    StartEvent();
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
    return false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,m_Title);
    return false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(m_Title);
    return false;
  }

  return true;
}

bool CAutoWizard::Close(void)
{
  m_DoneOk = true;

  try
  {
    CloseEvent();
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
    return false;
  }

  return true;

}

// End a wizard session
bool CAutoWizard::End()
{
   return End(true);
}

bool CAutoWizard::End(bool EnableNext)
{
  try
  {
  EnableDisableNext(false);
  EnableDisablePrevious(false);
  EnableDisableCancel(false);
  EnableDisableUserButton1(false);
  EnableDisableUserButton2(false);

  try
  {
    EndEvent();
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
    return false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,m_Title);
    return false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(m_Title);
    return false;
  }
  }
  __finally
  {
    EnableDisableNext(EnableNext);
  }
  m_Stop = true;
  return true;
}

bool CAutoWizard::Error(void)
{
   bool ret = Cancel();
   Close();
   return ret;
}

bool CAutoWizard::Cancel(void)
{
  s_Cancelled = true;

  try
  {
    if(m_CurrentPage >= 0)
      CancelEvent(m_PageList[m_CurrentPage]);
  }
  catch(EQPython &e)
  {
    return false;
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
    return false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,m_Title);
    return false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(m_Title);
    return false;
  }
  m_Stop = true;
  return true;
}

// Activate help for the current wizard page
void CAutoWizard::Help(void)
{
  try
  {
    if(m_CurrentPage >= 0)
      HelpEvent(m_PageList[m_CurrentPage]);
    else
      HelpEvent(NULL);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,m_Title);
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(m_Title);
  }
}

int CAutoWizard::GetLastRelevantPageIndex(void)
{
  CWizardPage* Page = GetCurrentPage();
  if (Page->GetAttributes() & wpCancelPage)
     return GetPreviousPage()->GetPageNumber();
  return m_CurrentPage;
}

CWizardPage* CAutoWizard::GetCurrentPage(void)
{
  return m_PageList[m_CurrentPage];
}

int CAutoWizard::GetCurrentPageIndex(void)
{
  return m_CurrentPage;
}

// Get a pointer to the next page in the wizard
CWizardPage *CAutoWizard::GetNextPage(void)
{
  if((unsigned)m_NextPage < m_PageList.size())
  {
    return m_PageList[m_NextPage];

  }

  return NULL;
}

CWizardPage *CAutoWizard::GotoNextPageNoEvents(void)
{
	if((unsigned)m_NextPage < m_PageList.size()) // todo -oNobody -cNone: prefer c++ casts
	{
        CWizardPage* NextPage = m_PageList[m_NextPage];
        if (!(NextPage->GetAttributes() & wpIgnoreOnPrevious))
   		   if (m_NextPage != m_CurrentPage)
		      // Add page pointer to the history stack
   			  m_History.push(NextPage);

		m_CurrentPage = m_NextPage;
		m_NextPage++;

		return m_PageList[m_CurrentPage];
	}

	return NULL;
}

// Get a pointer to the page by the Id
CWizardPage *CAutoWizard::GetPageById(int PageId)
{
	 return m_PageList[PageId];
}


// Get a pointer to the previous page in the wizard
CWizardPage *CAutoWizard::GetPreviousPage(void)
{
  CWizardPage *prevpage = NULL;
  bool IsCurrentIgnored = (m_PageList[m_CurrentPage]->GetAttributes() & wpIgnoreOnPrevious);
  if(m_History.size() > (IsCurrentIgnored ? 0 : 1))
  {
    CWizardPage *page = m_History.top();
    if (!IsCurrentIgnored)
    {
       m_History.pop();
       prevpage = m_History.top();
       m_History.push(page);
    }
    else
       prevpage = page;
  }
  return prevpage;
}

CWizardPage *CAutoWizard::GotoPreviousPageNoEvents(void)
{
  CWizardPage *prevpage = GetPreviousPage();
  bool IsCurrentIgnored = (m_PageList[m_CurrentPage]->GetAttributes() & wpIgnoreOnPrevious);
  //If there is no previous return
  if (!prevpage)
      return prevpage;
  //If current page isn't ignored, remove it from history
  if (!IsCurrentIgnored)
     m_History.pop();
  //Update pages indexes
  m_CurrentPage = prevpage->GetPageNumber();
  m_NextPage = m_CurrentPage + 1;
  return prevpage;
}

void CAutoWizard::SetUIControlCallback(TWizardUICallback WizardUICallback,TGenericCockie WizardUICockie)
{
  m_WizardUICallback = WizardUICallback;
  m_WizardUICockie = WizardUICockie;
}

QString CAutoWizard::GetTitle(void)
{
  return m_Title;
}

bool CAutoWizard::IsCancelled(void)
{
  return s_Cancelled;
}

bool CAutoWizard::IsDoneOk(void)
{
  return m_DoneOk;
}
bool CAutoWizard::IsStop(void)
{
  return m_Stop;
}
bool CAutoWizard::IsHelpVisible(void)
{
  return m_HelpVisible;
}

int CAutoWizard::GetDefaultImageID(void)
{
  return m_DefaultImageID;
}

int CAutoWizard::GetNumberOfPages(void)
{
  return m_PageList.size();
}

QString CAutoWizard::GetUserButton1Caption(void)
{
  return m_UserButton1Caption;
}

QString CAutoWizard::GetUserButton2Caption(void)
{
  return m_UserButton2Caption;
}

bool CAutoWizard::IsUserButton1Visible(void)
{
  return m_UserButton1Visible;
}

bool CAutoWizard::IsUserButton2Visible(void)
{
  return m_UserButton2Visible;
}

void CAutoWizard::SetAppDefaultImageID(int ImageID)
{
  m_AppDefaultImageID = ImageID;
}

// Set next page using page number
void CAutoWizard::SetNextPage(int PageNumber)
{
  if((PageNumber >= 0) && ((unsigned)PageNumber < m_PageList.size()))
    m_NextPage = PageNumber;
}

// Set next page using page instance pointer
void CAutoWizard::SetNextPage(CWizardPage *Page)
{
  SetNextPage(Page->GetPageNumber());
}

// Dialog UI controls
void CAutoWizard::EnableDisableNext(bool Enabled)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiEnableNext,Enabled,m_WizardUICockie);
}

void CAutoWizard::EnableDisablePrevious(bool Enabled)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiEnablePrevious,Enabled,m_WizardUICockie);
}

void CAutoWizard::EnableDisableCancel(bool Enabled)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiEnableCancel,Enabled,m_WizardUICockie);
}

void CAutoWizard::EnableDisableHelp(bool Enabled)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiEnableHelp,Enabled,m_WizardUICockie);
}

void CAutoWizard::EnableDisableUserButton1(bool Enabled)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiEnableUserButton1,Enabled,m_WizardUICockie);
}

void CAutoWizard::EnableDisableUserButton2(bool Enabled)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiEnableUserButton2,Enabled,m_WizardUICockie);
}

// Yield thread (on windows: "process messages")
void CAutoWizard::YieldWizardThread()
{
#ifdef OS_WINDOWS
  Application->ProcessMessages();
#else
  Sleep(0);
#endif
}

// Yield & Sleep thread (on windows: "process messages")
void CAutoWizard::YieldAndSleepWizardThread(int SleepTime /* = 100 ms by default */)
{
#ifdef OS_WINDOWS
  Application->ProcessMessages();
  Sleep(SleepTime);
#else
  QSleep(SleepTime);
#endif
}


// Return the stop flag status
bool CAutoWizard::IsStopped(void)
{
#ifdef OS_WINDOWS
  if(Application->Terminated)
    return true;
#endif
  return (m_DoneOk || s_Cancelled);
}


// Clear the user navigation history
void CAutoWizard::ClearHistory(void)
{
  // Clear history stack
  while(m_History.size() > 0)
    m_History.pop();
}

// Trigger the user button events
void CAutoWizard::FireUserButton1Event(void)
{
  try
  {
    UserButton1Event(m_PageList[m_CurrentPage]);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,m_Title);
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(m_Title);
  }
}

void CAutoWizard::FireUserButton2Event(void)
{
  try
  {
    UserButton2Event(m_PageList[m_CurrentPage]);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,m_Title);
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,m_Title);
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(m_Title);
  }
}

// Change one page next
void CAutoWizard::GotoNextPage(void)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiGotoNext,0,m_WizardUICockie);
}

// Change one page previous
void CAutoWizard::GotoPreviousPage(void)
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiGotoPrevious,0,m_WizardUICockie);
}

// Change to an arbitrary page (version 1)
void CAutoWizard::GotoPage(int PageNumber)
{
  SetNextPage(PageNumber);
  TWizardPageLeaveReason LeaveReason = ((GetNextPage()->GetAttributes() & wpCancelPage) ? wuiCancelNext : wuiGotoNext);
  if(m_WizardUICallback)
    (*m_WizardUICallback)(LeaveReason,0,m_WizardUICockie);
}

// Change to an arbitrary page (version 2)
void CAutoWizard::GotoPage(CWizardPage *Page)
{
  SetNextPage(Page);
  TWizardPageLeaveReason LeaveReason = ((Page->GetAttributes() & wpCancelPage) ? wuiCancelNext : wuiGotoNext);
  if(m_WizardUICallback)
    (*m_WizardUICallback)(LeaveReason,0,m_WizardUICockie);
}

void CAutoWizard::GotoNextPageAsync()
{
  if(m_WizardUICallback)
    (*m_WizardUICallback)(wuiGotoNextAsync,0,m_WizardUICockie);
}

// Refresh a specific page
void CAutoWizard::RefreshPage(int PageNumber)
{
  if(PageNumber >= 0 && PageNumber < (int)m_PageList.size())
  m_PageList[PageNumber]->Refresh();
}

// Sets new image for specific page
void CAutoWizard::SetImagePage(int PageNumber, int _ImageID)
{
  m_PageList[PageNumber]->m_ImageID = _ImageID;
}

// Show on the monitor a QException error
static void ShowQException(EQException ExceptionObject,const QString& WizardName)
{
  QMonitor.ErrorMessageWaitOk(QFormatStr("Wizard '%s' throw exception: '%s' , (Code %d)",WizardName.c_str(),
                        ExceptionObject.GetErrorMsg().c_str(),ExceptionObject.GetErrorCode()),ORIGIN_WIZARD_PAGE);
}

// Show on the monitor VCL exceptions
static void ShowVCLException(Exception& VCLException,const QString& WizardName)
{
  QMonitor.ErrorMessageWaitOk(QFormatStr("Wizard '%s' throw exception: %s",WizardName.c_str(),VCLException.Message.c_str()),ORIGIN_WIZARD_PAGE);
}

// Show on the monitor a generic exception error
static void ShowUnexpectedException(const QString& WizardName)
{
  QMonitor.ErrorMessageWaitOk(QFormatStr("Wizard '%s' throw unexpected exception",WizardName.c_str()),ORIGIN_WIZARD_PAGE);
}

// CWizardPage class implementation
// ****************************************************************************

// Constructor
CWizardPage::CWizardPage(const QString Title_,int ImageID,TWizardPageAttributes PageAttributes)
{
  Title                 = Title_;
  m_ImageID             = ImageID;
  m_PageAttributes      = PageAttributes;
  m_EventHandler        = NULL;
  m_EventHandlerCockie  = 0;
  m_PageRefreshCallback = NULL;
  m_PageRefreshCockie   = 0;
  m_PageEnterEvent      = NULL;
  m_PageEnterCockie     = 0;
  m_PageLeaveEvent      = NULL;
  m_PageLeaveCockie     = 0;
  m_PageResetEvent      = NULL;
  m_PageResetCockie     = 0;
  m_DefaultTimeout      = m_Timeout = PAGE_TIMEOUT_DISABLE;
  m_PageNumber          = -1;
  CancelMessage         = DEFAULT_CANCEL_PAGE_MSG;

  if (m_PageAttributes & wpCancelPage)
     m_PageAttributes |= wpDonePage;
}

// Refresh the page on the screen
void CWizardPage::Refresh(void)
{
  if(m_PageRefreshCallback)
    (*m_PageRefreshCallback)(this,m_PageRefreshCockie);
}

// Reset the page's internal data
void CWizardPage::Reset(void)
{
  try
  {
	  if(m_PageResetEvent)
	    (*m_PageResetEvent)(this, m_PageResetCockie);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,Title);
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,Title);
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(Title);
  }

}

// Notify event (generic version)
bool CWizardPage::NotifyEvent(int Param1,int Param2)
{
  try
  {
    if(m_EventHandler)
      (*m_EventHandler)(this,Param1,Param2,m_EventHandlerCockie);
  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,Title);
    return false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,Title);
    return false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(Title);
    return false;
  }

  return true;
}

// Enter the page
bool CWizardPage::PreEnter(void){return true;}

bool CWizardPage::Enter(void)
{
  try
  {
    if(m_PageEnterEvent)
      (*m_PageEnterEvent)(this,m_PageEnterCockie);

  }
  // todo -oNobody -cNone: !!!!!! these handlers block the on cancel event !!! (stuff may be left turned on and cause damage)
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,Title);
    return false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,Title);
    return false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(Title);
    return false;
  }

  return true;
}

// Leave the page
bool CWizardPage::Leave(TWizardPageLeaveReason LeaveReason)
{
  try
  {
    if(m_PageLeaveEvent)
      (*m_PageLeaveEvent)(this,m_PageLeaveCockie,LeaveReason);

  }
  // Catch QException classes
  catch(EQException& QException)
  {
    ShowQException(QException,Title);
    return false;
  }
  // Catch VCL Exceptions
  catch(Exception& VCLException)
  {
    ShowVCLException(VCLException,Title);
    return false;
  }
  // Catch unexpected exceptions
  catch(...)
  {
    ShowUnexpectedException(Title);
    return false;
  }

  return true;
}


void CWizardPage::SetPageRefreshCallback(TGenericWizardPageEvent PageRefreshCallback,TGenericCockie PageRefreshCockie)
{
  m_PageRefreshCallback = PageRefreshCallback;
  m_PageRefreshCockie = PageRefreshCockie;
}

void CWizardPage::SetPageResetCallback(TGenericWizardPageEvent PageResetEvent, TGenericCockie PageResetCockie)
{
  m_PageResetEvent = PageResetEvent;
  m_PageResetCockie = PageResetCockie;
}

void CWizardPage::SetPageEventsCallback(TWizardPageEvent EventHandler,TGenericCockie EventHandlerCockie)
{
  m_EventHandler = EventHandler;
  m_EventHandlerCockie = EventHandlerCockie;
}

void CWizardPage::SetPageEnterCallback(TGenericWizardPageEvent PageEnterEvent,TGenericCockie PageEnterCockie)
{
  m_PageEnterEvent = PageEnterEvent;
  m_PageEnterCockie = PageEnterCockie;
}

void CWizardPage::SetPageLeaveCallback(TPageLeaveEvent PageLeaveEvent,TGenericCockie PageLeaveCockie)
{
  m_PageLeaveEvent = PageLeaveEvent;
  m_PageLeaveCockie = PageLeaveCockie;
}

void CWizardPage::SetTag1(void *Tag1)
{
  m_Tag1 = Tag1;
}

bool CWizardPage::IsHelpVisible(void)
{
   return (!(GetAttributes() & wpHelpNotVisible)/*invisible help button - old version*/
                                               /*future : invisible help button - new version*/);
}

void* CWizardPage::GetTag1(void)
{
  return m_Tag1;
}

int CWizardPage::GetTimeout(void)
{
  return ((m_Timeout == PAGE_TIMEOUT_DISABLE) ? m_DefaultTimeout : m_Timeout);
}

bool CWizardPage::GetTimeoutEnable(void)
{
  //No timeouts on donePages
  if (m_PageAttributes & wpDonePage)
     return false;
  if (m_PageAttributes & wpNoTimeout)
     return false;
  if (m_DefaultTimeout == PAGE_TIMEOUT_DISABLE)
     if (m_Timeout == PAGE_TIMEOUT_DISABLE)
       return false;
  return true;
}

void CWizardPage::SetTimeout(int Timeout)
{
  m_Timeout = Timeout;
}

void CWizardPage::SetDefaultTimeout(int Timeout)
{
  m_DefaultTimeout = Timeout;
}

int CWizardPage::GetPageNumber(void)
{
  return m_PageNumber;
}

int CWizardPage::GetImageID(void)
{
  return m_ImageID;
}

void CWizardPage::SetImageID(int ImageId)
{
  m_ImageID = ImageId ;
}

TWizardPageAttributes CWizardPage::GetAttributes(void)
{
  return m_PageAttributes;
}

void CWizardPage::SetAttributes(TWizardPageAttributes Attributes)
{
   m_PageAttributes = Attributes;
}

void CWizardPage::AddAttribute(TWizardPageAttributes Attribute)
{
  m_PageAttributes |= Attribute;
}

void CWizardPage::RemoveAttribute(TWizardPageAttributes Attribute)
{
  m_PageAttributes &= ~Attribute;
}

