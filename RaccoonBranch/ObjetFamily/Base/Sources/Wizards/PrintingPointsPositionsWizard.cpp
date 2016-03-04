/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Printing Points Positions                                *
 * Module Description: Printing Points Positions wizard.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Oved Ben Zeev                                            *
 * Start date: 21/9/2005                                            *
 * Last upate: 21/9/2005                                            *
 ********************************************************************/

#include "PrintingPointsPositionsWizard.h"
#include "Q2RTApplication.h"
#include "AppParams.h"
#include "MotorDefs.h"
#include "QMonitor.h"
#include "AppLogFile.h"
#include "BackEndInterface.h"
#include "WizardImages.h"
#include "QScopeRestorePoint.h"

enum {ppWelcomePage,       
      ppIsTrayInserted,
      ppCleanTray,
      ppCloseDoor,
      ppPrintPatternTest,
      ppIsResultGood,
      ppPositionCorrection,
      ppWizardCompleted,

      ppPagesCount // this must element must be the last in this enum
      }; 

const int HORIZONTAL = 0;
const int VERTICAL   = 1;

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

CPrintingPositionWizard::CPrintingPositionWizard(void) : CQ2RTAutoWizard(IDS_PP_WIZARD,true,IN_PROCESS_IMAGE_ID)
{
  TPagePointers pagesVector(ppPagesCount, 0);
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Printing Position Adjustment",-1,wpHelpNotVisible);
	pPage->SubTitle = "Use this wizard to adjust the printing pargins on the build tray.\nThis procedure takes approximately 5 minutes.\nBefore begining:\n* Prepare a ruler with millimeter markings.\n* Make sure the build tray is empty and clean.\n\nWhen you click 'Next', the wizard prints a pattern test in the left-rear corner of the build tray.";
	pagesVector[ppWelcomePage] = pPage;
  }
  {
	CInsertTrayPage *pPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[ppIsTrayInserted] = pPage;
  }
  {
	CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this, PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[ppCleanTray] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[ppCloseDoor] = pPage;
  }
  {
	CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage("Warming Heads/Printing",-1,wpNextDisabled | wpPreviousDisabled | wpHelpNotVisible);
    pagesVector[ppPrintPatternTest] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage("Select Action",QUESTION_IMAGE_ID, wpPreviousDisabled);
	pPage->Strings.Add("Enter correction values");
    pPage->Strings.Add("End the wizard");
    pPage->DefaultOption = ANSWER_YES;
    pagesVector[ppIsResultGood] = pPage;
  }
  {
	CDataEntryWizardPage *pPage = new CDataEntryWizardPage("Enter Position Correction Values (mm)",DIRECTION_IMAGE_ID);
	pPage->SubTitle = LOAD_STRING(IDS_CORRECTION_INSTRUCTIONS);
    pPage->Strings.Add(LOAD_STRING(IDS_HORIZONTAL_CORRECTION));
    pPage->Strings.Add(LOAD_STRING(IDS_VERTICAL_CORRECTION));
    pPage->FieldsTypes[HORIZONTAL] = ftFloat;
    pPage->FieldsTypes[VERTICAL] = ftFloat;
    pagesVector[ppPositionCorrection] = pPage;
  }
  {
    CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
    pagesVector[ppWizardCompleted] = pPage;
  }
  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
    AddPage(*p);
};


// Start the wizard session event
void CPrintingPositionWizard::StartEvent()
{
  float TrayStartPositionX = m_ParamMgr->TrayStartPositionX;
  float TrayStartPositionY = m_ParamMgr->TrayStartPositionY;

  m_ParamMgr->AutoPrintCurrentZLocation = true;
  m_RepeatTest = false;
  m_PrevTestPatternXPos = m_ParamMgr->TestPatternXPosition;
  m_PrevTestPatternYPos = m_ParamMgr->TestPatternYPosition;

  m_ParamMgr->TestPatternXPosition = TrayStartPositionX;
  m_ParamMgr->TestPatternYPosition = TrayStartPositionY;
}


// Start the wizard session event
void CPrintingPositionWizard::EndEvent()
{
  float TestPatternXPosition = m_ParamMgr->TestPatternXPosition;
  float TestPatternYPosition = m_ParamMgr->TestPatternYPosition;

  // Saving the printing locations...
  m_ParamMgr->TrayStartPositionX = TestPatternXPosition;
  m_ParamMgr->TrayStartPositionY = TestPatternYPosition;

  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TrayStartPositionX);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->TrayStartPositionY);

  CleanUp();
}


// Cancel the wizard session event
void CPrintingPositionWizard::CancelEvent(CWizardPage *WizardPage)
{
  CleanUp();
}

void CPrintingPositionWizard::IsResultGoodPageLeave(CWizardPage *WizardPage)
{
  CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
  int Results  = Page->GetSelectedOption();
  m_RepeatTest = (Results == ANSWER_NO);
  if (!m_RepeatTest)
     SetNextPage(ppWizardCompleted);
}

void CPrintingPositionWizard::PositionCorrectionPageLeave(CWizardPage *WizardPage)
{
  CDataEntryWizardPage* Page         = dynamic_cast<CDataEntryWizardPage *>(WizardPage);
  float                 HCorrecrtion = QStrToValue<float>(Page->FieldsValues[HORIZONTAL]);
  float                 VCorrecrtion = QStrToValue<float>(Page->FieldsValues[VERTICAL]);

  m_ParamMgr->TestPatternXPosition += HCorrecrtion;
  m_ParamMgr->TestPatternYPosition += VCorrecrtion;
  SetNextPage(ppCleanTray);
}

void CPrintingPositionWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  if (LeaveReason != lrGoNext)
    return;

  switch(WizardPage->GetPageNumber())
  {
    case ppIsResultGood:
         IsResultGoodPageLeave(WizardPage);
         break;

    case ppPositionCorrection:
         PositionCorrectionPageLeave(WizardPage);
         break;

  }
}

void CPrintingPositionWizard::PrintPatternTestPageEnter(CWizardPage *WizardPage)
{
  if(m_ParamMgr->OfficeProfessionalType==OFFICE_MACHINE) //runtime objet
  {
//#if defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
        CScopeRestorePoint<bool> scKeepDoorLocked(m_ParamMgr->KeepDoorLocked);
        m_ParamMgr->KeepDoorLocked = true;
//#endif
  }

  DoPatternTest();

  if (IsCancelled())
     throw CQ2RTAutoWizardCancelledException();
  if(m_ParamMgr->OfficeProfessionalType==OFFICE_MACHINE) //runtime objet
//#if defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260  || defined OBJET_260
  // Print another Pattern Test in the middle of the tray...
  {
  CScopeRestorePoint<float> scTestPatternXPosition(m_ParamMgr->TestPatternXPosition);
  m_ParamMgr->TestPatternXPosition += (m_ParamMgr->TrayXSize / 2);
  m_ParamMgr->KeepDoorLocked = false;
  DoPatternTest();
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }
//#endif

  GotoNextPage();
}

void CPrintingPositionWizard::PageEnter(CWizardPage *WizardPage)
{
  switch(WizardPage->GetPageNumber())
  {
    case ppPositionCorrection:
    {
      CDataEntryWizardPage *Page = dynamic_cast<CDataEntryWizardPage *>(WizardPage);
      Page->FieldsValues[HORIZONTAL] = "0";
      Page->FieldsValues[VERTICAL]   = "0";
      Page->Refresh();
      break;
    }

    case ppPrintPatternTest:
         PrintPatternTestPageEnter(WizardPage);
         break;
  }
}

void CPrintingPositionWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();
	switch(WizardPage->GetPageNumber())
	{
		case ppWelcomePage: Application->HelpContext(10); break;
		case ppPrintPatternTest: Application->HelpContext(20); break;
		case ppIsResultGood: Application->HelpContext(30); break;
		case ppPositionCorrection: Application->HelpContext(40); break;
        default: break;
	}
}


void CPrintingPositionWizard::CleanUp()
{

  // Returning the previous values...
  m_ParamMgr->TestPatternXPosition = m_PrevTestPatternXPos;
  m_ParamMgr->TestPatternYPosition = m_PrevTestPatternYPos;

  if (m_BackEnd->IsDoorLocked()) 
     EnableAllAxesAndHome(NULL,BLOCKING,FORCE_HOME);

  // Disabling 'T' Axis
  m_BackEnd->EnableMotor(false, AXIS_T);
  m_ParamMgr->AutoPrintCurrentZLocation = false;

  // Unlock the Door
  while(m_BackEnd->IsDoorLocked()) 
    m_BackEnd->LockDoor(false);
}


bool CPrintingPositionWizard::DoPatternTest()
{
  m_BackEnd->EnableMotor(true, AXIS_Z);
  m_BackEnd->MoveMotorToZStartPosition();
  m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME);
  m_BackEnd->DoTestPattern();
  
  m_BackEnd->SetOnlineOffline(true);
  if (WaitForStateToDiff(msTestPattern) == msStopped)
     return false;

  TMachineState State;
  do
  {
    State = m_BackEnd->GetCurrentMachineState();
    YieldAndSleepWizardThread();
    if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }
  while(State == msTestPattern || State == msStopping || State == msStopped);

  return true;
}

