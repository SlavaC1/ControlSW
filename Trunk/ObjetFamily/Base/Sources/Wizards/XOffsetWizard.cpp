/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: X-Offset.                                                *
 * Module Description: X-Offset wizard.                             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Oved Ben Zeev                                            *
 * Start date: 17/11/2004                                           *
 * Last upate: 17/11/2004                                           *
 ********************************************************************/

#include "XOffsetWizard.h"
#include "QTimer.h"
#include "AppParams.h"
#include "MotorDefs.h"
#include "HeatersDefs.h"
#include "QErrors.h"
#include "XOffsetChooseBestPrintFrame.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "MachineManagerDefs.h"
#include "AppLogFile.h"
#include "BackEndInterface.h"
#include "WizardImages.h"

#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "MaintenanceCountersDefs.h" //itamar

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

extern QString YesNoAnswerStrings[];

enum {xoWelcomePage,
      xoIsTrayInserted1,
      xoCloseDoor1,
      xoHomeAxis,
      xoPlaceTransparency,
      xoIsTrayInserted2,
      xoCloseDoor2,            //Until this page - the wizard assumes responsibility over the door locking/unlocking
      xoPrintTransparency,     //Starting with this page - the wizard assumes that the machine sequencer is responsible for the door lock state.
      xoRemoveTransparency,
      xoChooseBestPrint,
      xoConfirmUpdates,
      xoNoUpdates,
      xoPrintTransparencyAgain,
      xoWizardCompleted,
      xoPagesCount // this must element must be the last in this enum
      };      
	  
namespace XOffsetWizard
{
	enum
	{
		ACTION_EXECUTE  = 0
	   ,ACTION_DONT_EXECUTE
	};
}

const int NO_MORE_HEADS   = -1;
const int REFERENCE_VALUE = 4;

CXOffsetWizard::CXOffsetWizard(void) : CQ2RTAutoWizard(IDS_XO_WIZARD,true,IN_PROCESS_IMAGE_ID)
{
  memset(m_BestPrint, 0, sizeof(m_BestPrint));
   m_CurrentHead = 0;
  TPagePointers pagesVector(xoPagesCount, 0);

  {
  	CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(), START_IMAGE_ID, wpHelpNotVisible);
    pPage->SubTitle = LOAD_STRING(IDS_XO_SUBTITLE);
  	pagesVector[xoWelcomePage] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[xoIsTrayInserted1] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,CAUTION_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[xoCloseDoor1] = pPage;
  }
  {
    CStatusWizardPage *pPage = new CStatusWizardPage("",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pPage->StatusMessage = LOAD_STRING(IDS_MOVING_AXIS);
  	pagesVector[xoHomeAxis] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new COneCheckBoxWizardPage(this,IDS_PLACE_TRANSPARENCY,IDS_PLACED_TRANSPARENCY,
                                                            PREPARATIONS_IMAGE_ID,wpPreviousDisabled);
    pPage->Title = "Check Printer";
    pagesVector[xoPlaceTransparency] = pPage;
  }
  {
    CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[xoIsTrayInserted2] = pPage;
  }
  {
    CCloseDoorPage *pPage = new CCloseDoorPage(this,CAUTION_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[xoCloseDoor2] = pPage;
  }
  { 
    CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Printing Transparency",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
	pPage->SubTitle = "Printing may take up to 10 minutes.";
  	pagesVector[xoPrintTransparency] = pPage;
  }
  {
    CCheckBoxWizardPage *pPage = new COneCheckBoxWizardPage(this,IDS_REMOVE_TRANSPARENCY,IDS_REMOVED_TRANSPARENCY,
                                                            PREPARATIONS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
    pPage->Title = "Check Printer";
    pagesVector[xoRemoveTransparency] = pPage;
  }
  {
    CXOffsetWizardPage* pPage = new CXOffsetWizardPage("Head Num",CHOOSE_BEST_LINE_IMAGE_ID);
  	pagesVector[xoChooseBestPrint] = pPage;
  }
  {
		CRadioGroupWizardPage* pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID);
	pPage->Strings.Add("Update system with new parameters.");
	pPage->Strings.Add("Keep previous parameters.");
    //pPage->SubTitle = LOAD_STRING(IDS_UPDATE_QUESTION);
	pPage->DefaultOption = XOffsetWizard::ACTION_EXECUTE;
	pagesVector[xoConfirmUpdates] = pPage;
  }
  {
    CMessageWizardPage* pPage = new CMessageWizardPage(LOAD_STRING(IDS_SYSTEM_PARAMS),QUESTION_IMAGE_ID, wpHelpNotVisible);
    pPage->SubTitle = "You did not change the head alignment.";
  	pagesVector[xoNoUpdates] = pPage;
  }
  {
		CRadioGroupWizardPage* pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID);
	pPage->Strings.Add("Repeat the test and print another transparency.");
	pPage->Strings.Add("End this wizard.");
	pPage->DefaultOption = XOffsetWizard::ACTION_EXECUTE;
	pagesVector[xoPrintTransparencyAgain] = pPage;
  }
  {
    CMessageWizardPage* pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
  	pagesVector[xoWizardCompleted] = pPage;
  }

  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
   	AddPage(*p);

};

void CXOffsetWizard::EndEvent()
{
  m_BackEnd->GotoDefaultMode();
}

void CXOffsetWizard::CancelEvent(CWizardPage *WizardPage)
{

  if (WizardPage->GetPageNumber() < xoPrintTransparency) // starting with xoPrintTransparency, we assume that the machine sequencer is responsible for the door lock state.
  {
      //Unlock the Door
      m_BackEnd->LockDoor(false);
      while(m_BackEnd->IsDoorLocked())
        YieldAndSleepWizardThread();
  }

  //Go back to the default mode
  m_BackEnd->GotoDefaultMode();
  //Disabling T Axis
  m_BackEnd->EnableMotor(false, AXIS_T);
}

void CXOffsetWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  int PageIndex = WizardPage->GetPageNumber();

  if (LeaveReason == lrCanceled)
     return;
      
  if (LeaveReason != lrGoNext)
     if (PageIndex != xoChooseBestPrint)
            return;

  switch(PageIndex)
  {
	case xoWelcomePage:
         // Start heating the block to save time
	     StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true); //6483
		 break;
    case xoRemoveTransparency:
         for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
             m_BestPrint[i] = REFERENCE_VALUE;
         //Setting 'm_CurrentHead' to the first head to update. (which is: LAST_HEAD)
         m_CurrentHead = LAST_HEAD-2;//MO is not participating
         break;

    case xoChooseBestPrint:
    {
         CXOffsetWizardPage* Page        = dynamic_cast<CXOffsetWizardPage *>(WizardPage);
         // Traversing heads in the opposite order: from 'LAST_HEAD' to 'FIRST_HEAD', to be "compatible" with the Eden.
         int CurrentHead = ((LeaveReason == lrGoPrevious) ? GetNextHead(): GetPreviousHead());
         m_BestPrint[m_CurrentHead] = Page->ChosenPrintLine;
         if (VALIDATE_HEAD(CurrentHead))
         {
            m_CurrentHead = CurrentHead;
			if(LeaveReason == lrGoPrevious)
			{
				SetNextPage(xoChooseBestPrint+1);
				GotoNextPageNoEvents();
            }
			else
			  SetNextPage(xoChooseBestPrint);
         }
         break;
    }

	case xoConfirmUpdates:
    {
         CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
		 if (Page->GetSelectedOption() == XOffsetWizard::ACTION_EXECUTE)
		 {
            UpdateParameters();
            m_WizardSummary = "Alignment parameter(s) updated.";
            SetNextPage(xoPrintTransparencyAgain);
         }
         else
            m_WizardSummary = "Alignment parameter(s) NOT updated.";
         break;
    }

    case xoPrintTransparencyAgain:
    {
         CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
         //'Update the parameters' was chosen
         if (Page->GetSelectedOption() == XOffsetWizard::ACTION_EXECUTE /*Repeat the test*/)
           SetNextPage(xoIsTrayInserted1);
         break;
    }
  }
}

bool CXOffsetWizard::PrintTransparencyPageEnter(CWizardPage *WizardPage)
{
  TMachineState State;
  m_BackEnd->GotoDefaultMode();
  m_BackEnd->EnterMode(HEAD_ALIGNMENT_MODE, GENERAL_MODES_DIR); // this mode includes RunScript instructions that enter modes and initiate a print job
  m_BackEnd->SetOnlineOffline(true);
  WaitForEndOfPrint();
  GotoNextPage();
  return true;
}

void CXOffsetWizard::PageEnter(CWizardPage *WizardPage)
{

	// Enable/Disable Help Button
	switch(WizardPage->GetPageNumber())
    {
        case xoWelcomePage:
        case xoCloseDoor1:
        case xoCloseDoor2:
        case xoConfirmUpdates:
        case xoPrintTransparencyAgain:
        case xoPlaceTransparency:
        case xoPrintTransparency:
        case xoRemoveTransparency:
        case xoChooseBestPrint:
        	EnableDisableHelp(true);			
            break;
        default:
        	EnableDisableHelp(false);
    }


  switch(WizardPage->GetPageNumber())
  {
    case xoHomeAxis:
         EnableAllAxesAndHome();
         m_BackEnd->MoveMotorToZStartPosition(); //Moving tray to Z_Start
     	 m_BackEnd->WaitForEndOfMovement(AXIS_Z);
         GotoNextPage();
         break;

    case xoPlaceTransparency:
         m_BackEnd->LockDoor(false); //Unlock the Door
         while(m_BackEnd->IsDoorLocked())
         {
            YieldAndSleepWizardThread();
            if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
         }
         break;

    case xoPrintTransparency:
		 if (PrintTransparencyPageEnter(WizardPage) == false)
            return;
         break;

    case xoChooseBestPrint:
    {
         EnableDisableNext(true);
		 CXOffsetWizardPage *Page = dynamic_cast<CXOffsetWizardPage *>(WizardPage);
         Page->Title = GetHeadName(m_CurrentHead); //bug 7988
         Page->ChosenPrintLine = m_BestPrint[m_CurrentHead];
         Page->Refresh();
         break;
    }

    case xoWizardCompleted:
    {
         CMessageWizardPage *Page = dynamic_cast<CMessageWizardPage *>(WizardPage);
         Page->SubTitle = m_WizardSummary;
         Page->Refresh();
		 m_WizardSummary = "";
		 m_BackEnd->ResetMaintenanceCounter(HEAD_ALIGNMENT_COUNTER_ID);
         break;
    }
  }
}

void CXOffsetWizard::UpdateParameters()
{
  #ifdef OBJET_MACHINE_KESHET
  //Updating the parameters in the parameter manager
  for (int i = TOTAL_NUMBER_OF_HEADS_HEATERS - 2, CorrectionValue = 0; i >= 0; i--)
  {
	//Every line on the left of the refernce line is equal '-2'
	//Every line on the right of the refernce line is equal '+2'
	CorrectionValue = 2 * (m_BestPrint[i] - REFERENCE_VALUE);
	m_ParamMgr->HeadMapArray[S0 + i] -= CorrectionValue;
  }
  #else

  //Updating the parameters in the parameter manager
  for (int i = TOTAL_NUMBER_OF_HEADS_HEATERS - 2, CorrectionValue = 0; i >= 0; i--)
  {
	//Every line on the left of the refernce line is equal '-2'
	//Every line on the right of the refernce line is equal '+2'
	CorrectionValue = 2 * (m_BestPrint[i] - REFERENCE_VALUE);
	m_ParamMgr->HeadMapArray[M0 - i] += CorrectionValue;
  }
  #endif
  //Saving the parameters to the file
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->HeadMapArray);
}

int CXOffsetWizard::GetNextHead()
{
  int ret = m_CurrentHead + 1;
  return ((VALIDATE_HEAD(ret) && (ret != M0)) ? ret : NO_MORE_HEADS); // we skip Head7/M0 because it is the "reference" head. The user should not be asked about it.
}

int CXOffsetWizard::GetPreviousHead()
{
  int ret = m_CurrentHead - 1;
  return (VALIDATE_HEAD(ret) ? ret : NO_MORE_HEADS);
}

void CXOffsetWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();
	switch(WizardPage->GetPageNumber())
	{
		case xoWelcomePage: Application->HelpContext(10); break;
		case xoPlaceTransparency: Application->HelpContext(20); break;
		case xoChooseBestPrint: Application->HelpContext(30); break;
		case xoConfirmUpdates: Application->HelpContext(40); break;
		case xoPrintTransparencyAgain: Application->HelpContext(50); break;
        default: break;
	}
}

