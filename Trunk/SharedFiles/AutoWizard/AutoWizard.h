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

#ifndef _AUTO_WIZARD_H_
#define _AUTO_WIZARD_H_

#include <vector>
#include <stack>
#include "QObject.h"

#define PAGE_TIMEOUT_DISABLE -1
#define DEFAULT_CANCEL_PAGE_MSG "Are you sure you want to cancel the wizard?"

class CWizardPage;

// Wizard page refresh event type
typedef void (*TGenericWizardPageEvent)(CWizardPage *,TGenericCockie);

// Wizard UI control type and callback
typedef enum {wuiEnableNext,wuiEnablePrevious,wuiEnableCancel,wuiEnableHelp,
              wuiEnableUserButton1,wuiEnableUserButton2,wuiGotoNext,wuiGotoPrevious,wuiCancelNext,wuiGotoNextAsync} TWizardUIControlType;
              
typedef void (*TWizardUICallback)(TWizardUIControlType,bool /*Enabled*/,TGenericCockie);

typedef enum {lrGoNext,lrGoPrevious,lrCanceled} TWizardPageLeaveReason;

typedef enum {PROGRESSIVE_DISCLOSURE_EXPANDED = 0, PROGRESSIVE_DISCLOSURE_COLLAPSED} TProgressiveDisclosureState;

// Event type for page leave
typedef void (*TPageLeaveEvent)(CWizardPage *,TGenericCockie,TWizardPageLeaveReason);
typedef std::vector<CWizardPage *> TPagePointers;
typedef TPagePointers::const_iterator TPagePointersIterator;

// Base class for automatic wizards
class CAutoWizard : public CQObject
{
  private:

    typedef std::vector<CWizardPage *> TPageList;
    TPageList m_PageList;

    // Current page number
    int m_CurrentPage;

    // Next page number
    int m_NextPage;

    bool m_Stop;
    bool m_DoneOk;

    std::stack<CWizardPage *> m_History; // todo -oNobody -cNone: prefer typedef to prevent code duplication in precompilation and to be able to change the type for all its users.

    // Callback for user interface updates
    TWizardUICallback m_WizardUICallback;
    TGenericCockie m_WizardUICockie;

    // Application wide default image ID
    static int m_AppDefaultImageID;

    // Generic callback for page events
    static void PageEventsHandlerCallback(CWizardPage *WizardPage,int Param1,int Param2,TGenericCockie Cockie);

    // Page enter callback
    static void PageEnterCallback(CWizardPage *WizardPage,TGenericCockie Cockie);

    // Page exit callback
    static void PageLeaveCallback(CWizardPage *WizardPage,TGenericCockie Cockie,TWizardPageLeaveReason LeaveReason);


    // Page Reset callback
    static void PageResetCallback(CWizardPage *WizardPage, TGenericCockie Cockie);

  protected:
    // Wizard title
    QString m_Title;
    static bool s_Cancelled;

    // True if the help button should be visible
    bool m_HelpVisible;

    int m_DefaultImageID;

    bool m_UserButton1Visible;
    bool m_UserButton2Visible;
    QString m_UserButton1Caption;
    QString m_UserButton2Caption;

    // Read only access to the wizard pages
    const TPageList& Pages;

    // Add a page to the wizard
    virtual void AddPage(CWizardPage *NewPage);

    // Generic wizard page events handler
    virtual void PageEventsHandler(CWizardPage *WizardPage,int Param1,int Param2) {}

    // Start a wizard session event
    virtual void StartEvent(void) {}

    // End a wizard session event
    virtual void EndEvent(void) {}

    // Close a wizard session event
    virtual void CloseEvent(void) {}

    // Cancel a wizard session event
    virtual void CancelEvent(CWizardPage *WizardPage) {}

    // Help button pressed event
    virtual void HelpEvent(CWizardPage *WizardPage) {}

    // Page enter and exit events
    virtual void PageEnterWrapper(CWizardPage *WizardPage);
    virtual void PageLeaveWrapper(CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);

    virtual void PageEnter(CWizardPage *WizardPage) {}
    virtual void PageLeave(CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason) {}

	// Called when the wizard resets the page.
    virtual void PageReset(CWizardPage *WizardPage) {}
	
    // User buttons events
    virtual void UserButton1Event(CWizardPage *WizardPage) {}
    virtual void UserButton2Event(CWizardPage *WizardPage) {}

    //Stopped means Done or Cancelled
    bool IsStopped(void);
    bool End(bool);

  public:
    // Constructor
    CAutoWizard(const QString Title="Wizard",bool HelpVisible = false,int DefaultImageID = -1,
                bool UserButton1Visible = false,bool UserButton2Visible = false,
                QString UserButton1Caption = "",QString UserButton2Caption = "");

    // Destructor
    virtual ~CAutoWizard(void);
    virtual void SetDefaultTimeout(int Timeout);

    bool IsDonePage(CWizardPage *Page);

    void SetPageRefreshCallback(TGenericWizardPageEvent PageRefreshCallback,TGenericCockie PageRefreshCockie);

    // Set the callback for UI events (e.g. EnableDisableNext)
    void SetUIControlCallback(TWizardUICallback WizardUICallback,TGenericCockie WizardUICockie);

    // Return the wizard's title
    QString GetTitle(void);

    // Start a wizard session
    virtual bool Start(void);

    // End a wizard session
    virtual bool End(void);
    virtual bool Close(void);

    // Internal error(exception) a wizard session
    virtual bool Error(void);
    
    // Cancel a wizard session
    virtual bool Cancel(void);

    // Activate help for the current wizard page
    virtual void Help(void);

    // Clear the user navigation history
    virtual void ClearHistory(void);

    // Return true if the wizard was cancelled
    virtual bool IsCancelled(void);

    // Return true if the wizard was completed Ok
    bool IsDoneOk(void);
    // Returns true if the wizard finished the end() or cancelled function OK
    bool IsStop(void);
    // Return true if the help button is enabled (visible)
    bool IsHelpVisible(void);

    int GetDefaultImageID(void);

		// Get a pointer to the next page in the wizard
		CWizardPage *GetNextPage(void);
		
		// Get a pointer to the page by the Id
		CWizardPage *GetPageById(int PageId);

    CWizardPage *GetCurrentPage(void);

    int GetCurrentPageIndex(void);
    int GetLastRelevantPageIndex(void);

    // Get a pointer to the previous page in the wizard
    CWizardPage *GetPreviousPage(void);

	// Make the wizard go to previous/next page without trigerring the events (but keeping track of pages history)
	CWizardPage *GotoNextPageNoEvents(void);
   	CWizardPage *GotoPreviousPageNoEvents(void);

    // Set what would be the next page (two versions)
    void SetNextPage(int PageNumber);
    void SetNextPage(CWizardPage *Page);

    // Return the total number of pages installed
    int GetNumberOfPages(void);

    // Trigger the user button events
    void FireUserButton1Event(void);
    void FireUserButton2Event(void);

    QString GetUserButton1Caption(void) ;

    QString GetUserButton2Caption(void);

    bool IsUserButton1Visible(void);

	bool IsUserButton2Visible(void);

    // Resets all wizard pages
    virtual void ResetAllPages(void);

    // Change one page next
    void GotoNextPage(void);

    // Change one page previous
    void GotoPreviousPage(void);

    // Change to an arbitrary page (version 1)
    void GotoPage(int PageNumber);

    // Change to an arbitrary page (version 2)
    void GotoPage(CWizardPage *Page);

    void GotoNextPageAsync();

    // Dialog UI control
    void EnableDisableNext(bool Enabled);
    void EnableDisablePrevious(bool Enabled);
    void EnableDisableCancel(bool Enabled);
    void EnableDisableHelp(bool Enabled);
    void EnableDisableUserButton1(bool Enabled);
    void EnableDisableUserButton2(bool Enabled);

    // Yield thread (on windows: "process messages")
    void YieldWizardThread();

    // Yield & Sleep thread (on windows: "process messages")
    static void YieldAndSleepWizardThread(int SleepTime = 100);

    // Refresh a specific page
    void RefreshPage(int PageNumber);

		// Sets new image for specific page
	void SetImagePage(int PageNumber, int _ImageID);
	
    // Application wide default image ID
    static void SetAppDefaultImageID(int ImageID);
};

// Wizard page attributes
typedef unsigned int TWizardPageAttributes;
const unsigned int wpNextDisabled        = 1 << 0;   // Next button is disabled when page starts
const unsigned int wpPreviousDisabled    = 1 << 1;   // Previous button is disabled when page starts
const unsigned int wpCancelDisabled      = 1 << 2;   // Cancel button is disabled when page starts
const unsigned int wpUserButton1Disabled = 1 << 3;   // User button 1 is disabled when page starts
const unsigned int wpUserButton2Disabled = 1 << 4;   // User button 2 is disabled when page starts
const unsigned int wpNextWhenSelected    = 1 << 5;   // Highlight next only when all the checkboxes/radio items are selected
const unsigned int wpNextOneChecked      = 1 << 6;   // Highlight next when at least one checkbox is selected
const unsigned int wpDonePage            = 1 << 7;   // The page is a finishing ("Done") page
const unsigned int wpFlipButton1Visible  = 1 << 8;   // Flip user button 1 visiblity (as set in the AutoWizard class constructor)
const unsigned int wpFlipButton2Visible  = 1 << 9;   // Flip user button 2 visiblity (as set in the AutoWizard class constructor)
const unsigned int wpClearOnShow         = 1 << 10;  // Always clear all checkboxes on enter
const unsigned int wpCancelPage          = 1 << 11;  // The page is a finishing ("Done") page with error
const unsigned int wpIgnoreOnPrevious    = 1 << 12;  // Don't put this page on prevous pages history list 
const unsigned int wpHelpNotVisible      = 1 << 13;  // Don't show 'Help' button on the page  
const unsigned int wpNoRefreshOnShow     = 1 << 14;  // Don't refresh in ShowPage
const unsigned int wpNoTimeout           = 1 << 15;  
const unsigned int wpSelfClosingPage     = 1 << 16;  // The page is a finishing ("Done") page with error

// Default page attributes
const int DEFAULT_WIZARD_PAGE_ATTRIBUTES = 0;

// Wizard page type
typedef int TWizardPageType;

const int wptMessage        = 0;
const int wptStatus         = 1;
const int wptProgress       = 2;
const int wptCheckbox       = 3;
const int wptRadioGroup     = 4;
const int wptProgressStatus = 5;
const int wptDataEntry      = 6;
const int wptElapsingTime   = 7;
const int wptCustom         = 8;


// Wizard event type
typedef void (*TWizardPageEvent)(CWizardPage *WizardPage,int Param1,int Param2,TGenericCockie Cockie);

// Base class for wiard pages
class CWizardPage : public CQObject
{
  friend class CAutoWizard;

  private:
    TWizardPageAttributes m_PageAttributes;
    int m_ImageID;
    int m_PageNumber;
    int m_Timeout,m_DefaultTimeout;
    
    QString m_ImageName;

    // Just a tag
    void *m_Tag1;

    // Page events callback
    TWizardPageEvent m_EventHandler;
    TGenericCockie m_EventHandlerCockie;

    // Refresh callback
    TGenericWizardPageEvent m_PageRefreshCallback;
    TGenericCockie m_PageRefreshCockie;

    // Page enter callback
    TGenericWizardPageEvent m_PageEnterEvent;
    TGenericCockie m_PageEnterCockie;

    // Page exit callback
    TPageLeaveEvent m_PageLeaveEvent;
    TGenericCockie m_PageLeaveCockie;

    // Page Reset callback
    TGenericWizardPageEvent m_PageResetEvent;
    TGenericCockie m_PageResetCockie; // todo -oNobody -cNone: what are these cookies for ?!?!!?

    // Set page refresh callback
    void SetPageRefreshCallback(TGenericWizardPageEvent PageRefreshCallback,TGenericCockie PageRefreshCockie);

    // Set page Reset callbacks
    void SetPageResetCallback(TGenericWizardPageEvent PageResetEvent, TGenericCockie PageResetCockie);

  public:
	QString Title;

	QString CancelMessage;

    CWizardPage()
	{
	  m_PageAttributes  = 0;
      m_ImageID = m_PageNumber = m_Timeout = m_DefaultTimeout = 0;
	  m_Tag1 = NULL;
	  m_EventHandler = NULL;
      m_PageRefreshCallback = NULL;
      m_PageEnterEvent = NULL;
      m_PageLeaveEvent = NULL;
      m_PageResetEvent = NULL;
	};

    // Constructor
    CWizardPage(const QString Title, int ImageID = -1,
                TWizardPageAttributes PageAttributes = DEFAULT_WIZARD_PAGE_ATTRIBUTES);
    bool IsHelpVisible(void);

    // Set page event callback
    void SetPageEventsCallback(TWizardPageEvent EventHandler,TGenericCockie EventHandlerCockie);
    // Set page enter callback
    void SetPageEnterCallback(TGenericWizardPageEvent PageEnterEvent,TGenericCockie PageEnterCockie);
    // Set page exit callbacks
    void SetPageLeaveCallback(TPageLeaveEvent PageLeaveEvent,TGenericCockie PageLeaveCockie);
    // Just a tag
    void SetTag1(void *Tag1);
    void *GetTag1(void);
    int GetTimeout(void);
    bool GetTimeoutEnable(void);
    void SetTimeout(int Timeout);
    void SetDefaultTimeout(int Timeout);
    // Notify an event (generic version)
    virtual bool NotifyEvent(int Param1,int Param2);
    // PreEnter the page
    virtual bool PreEnter(void);
    // Enter the page
    virtual bool Enter(void);
    // Leave from the page
    virtual bool Leave(TWizardPageLeaveReason LeaveReason);
	// Reset the page's internal data
    virtual void Reset(void);
    // Get page number
    int GetPageNumber(void);
    // Get the image ID
    int GetImageID(void);
    // Set the image ID
    void SetImageID(int ImageId);
    // Get attributes word
    TWizardPageAttributes GetAttributes(void);
    void SetAttributes(TWizardPageAttributes);
    void AddAttribute(TWizardPageAttributes Attribute);
    void RemoveAttribute(TWizardPageAttributes Attribute);

    // Get the polymorphic page type
    virtual TWizardPageType GetPageType(void) = 0;

    // Refresh the page on the screen
    virtual void Refresh(void);
};

#endif

