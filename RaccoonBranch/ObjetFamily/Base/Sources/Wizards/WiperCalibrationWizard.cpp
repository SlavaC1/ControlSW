/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Wiper Calibration.                                       *
 * Module Description: 'Wiper Calibration' wizard.                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Bojena Gleizer                                           *
 * Start date: 13/1/2009                                            *
 * Last upate: 13/1/2009                                            *
 ********************************************************************/

#include "WiperCalibrationWizard.h"
#include "WizardImages.h"
#include "CustomWiperCalibrationPage.h"
#include "BackEndInterface.h"
#include "WizardPages.h"
#include "HostMessages.h"
#include "configuration.h"
#include "Motor.h"
#include "Q2RTApplication.h"

#define Y_AXIS_SAVE_DELTA 8  /*mm*/
//#define Y_AXIS_MOVE_DELTA 13 /*mm*/
#define MCB_PARAM_MAX_VALUE 100
#define MCB_PARAM_DEFAULT_VALUE 35
#define MCB_DELTA_PARAMETER 55
#define UNITIALIZED_VALUE -5000
#define X_AXIS_SPEED 10000
#define Y_AXIS_SPEED 9000

T_AxesTable AxesTable = {true, true, false, true};
const char WIPER_HELP_FILE_NAME[] = "\\Help\\Wiper Calibration Wizard\\Wiper Calibration Wizard.chm";

enum {PURGE_SELECTION = 0,
      WIPE_SELECTION};

typedef enum
{     ///////////////////////////////////////////////
      wcWelcomePage,
      wcCheckTray,
      wcCloseDoor,
      wcCalibrateTMaxPosition,
	  wcHomeAxisPage,
	  wcPositionAxisPage,
      wcCalibratePosition,
      wcInformation,
      wcPurgeWipe,
      wcPurging,
      wcWiping,
	  wcResultCheckPosition,
      wcIsResultGood,
	  wcCloseDoorBeforeCalibratingBasket,
	  wcMoveAxisBasket,
	  wcCalibrateBasket,
	  wcCloseDoorBeforeTestingBasket,
	  wcPurgeWipeBasket,
	  wcGoToPurgePosition,
	  wcResultBasketheckPosition,
	  wcIsBasketCalibrated,
      wcWizardCompleted,
      ///////////////////////////////////////////////
      wcPagesCount,
      wcNeedHomingFromHere = wcCalibrateTMaxPosition,
      wcNeedEnableDoorFromHere = wcCloseDoor,
      wcNeedParameterClearTillHere = wcIsResultGood
      ///////////////////////////////////////////////
} TWizardPagesIndex;

enum
{
	ANSWER_CALIBRATE_WIPER  = 0
   ,ANSWER_CALIBRATE_BASKET
   ,NUMBER_FINISH_WIZARD
};

CWiperCalibrationWizard::CWiperCalibrationWizard() : CQ2RTAutoWizard(IDS_WC_WIZARD,true,IN_PROCESS_IMAGE_ID,false,false,"Run")
{
  TPagePointers pagesVector(wcPagesCount, 0);
  {
	CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(),WC_WELCOME_IID,wpPreviousDisabled|wpHelpNotVisible);
	pPage->SubTitle = "This wizard guides you through the procedures necessary for checking and adjusting the wiper blades and purge basket.\
					  \nIf you're installing a new purge basket please make sure all the screws are tightened all the way down.";
	pagesVector[wcWelcomePage] = pPage;
  }
  {
    CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	#if defined EDEN_250 || defined EDEN_260 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
		pPage->Strings.Add("The build tray is inserted.");
	#endif
	pagesVector[wcCheckTray] = pPage;
  }
  {
    CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
  	pagesVector[wcCloseDoor] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage(LOAD_STRING(IDS_PREPARING_FOR_CALIBRATION),WC_T_HEIGHT_IID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = LOAD_STRING(IDS_T_ADJUST);
  	pagesVector[wcCalibrateTMaxPosition] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage(LOAD_STRING(IDS_PREPARING_FOR_CALIBRATION),WC_HOMING_IID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = LOAD_STRING(IDS_HOME_AXIS);
  	pagesVector[wcHomeAxisPage] = pPage;
  }
  {
    CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage(LOAD_STRING(IDS_PREPARING_FOR_CALIBRATION),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpIgnoreOnPrevious | wpHelpNotVisible);
    pPage->PointsStatusMessage = LOAD_STRING(IDS_MOVING_TO_POSITION);
  	pagesVector[wcPositionAxisPage] = pPage;
  }
  {
    CCustomWiperCalibrationPage* pPage = new CCustomWiperCalibrationPage("Wiper Calibration Wizard", WC_CALIBRATION_IID, wpPreviousDisabled | wpNextDisabled | wpIgnoreOnPrevious);
  	pagesVector[wcCalibratePosition] = pPage;
  }
  {
	CMessageWizardPage *pPage = new CMessageWizardPage("Approve New T-Height Value",QUESTION_IMAGE_ID, wpIgnoreOnPrevious | wpHelpNotVisible);
	pagesVector[wcInformation] = pPage;
  }
  {
    //Previous disable needed for continuous entering this page
	CRadioGroupWizardPage* pPage = new CRadioGroupWizardPage("Select Action",QUESTION_IMAGE_ID, wpIgnoreOnPrevious | wpHelpNotVisible);
    pPage->SubTitle  = "To test the new T-height calibration, select an option and click Next.";
    pPage->Strings.Add("Purge and wipe");
    pPage->Strings.Add("Wipe only");
    pPage->Strings.Add("Proceed to Head Inspection.");
    pPage->SetDisabledMask(4);
    pPage->DefaultOption = 1;
  	pagesVector[wcPurgeWipe] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Purging Print Heads",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpIgnoreOnPrevious | wpHelpNotVisible);
    pPage->PointsStatusMessage = "Purging";
  	pagesVector[wcPurging] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Wiping Heads",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpIgnoreOnPrevious | wpHelpNotVisible);
    pPage->PointsStatusMessage = "Wiping";
  	pagesVector[wcWiping] = pPage;
  }
  {
    CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Wiper Inspection",GENERAL_PURPOSE_1_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
    pPage->PointsStatusMessage = "Moving to inspection position";
  	pagesVector[wcResultCheckPosition] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage("Check Printer",PREPARATIONS_IMAGE_ID, wpPreviousDisabled );
	pPage->SubTitle  = "Inspect the print heads to make sure they are clean and dry. If not, repeat the wiper calibration.";
	pPage->Strings.Add("Repeat Wiper Calibration Wizard");
	pPage->Strings.Add("Save parameters and continue to Basket Calibration");
	pPage->Strings.Add("Save wiper parametersand and End Wizard");
	pPage->DefaultOption = ANSWER_CALIBRATE_BASKET;
  	pagesVector[wcIsResultGood] = pPage;
  }
  {
    CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID, wpPreviousDisabled | wpDonePage | wpHelpNotVisible);
    pPage->SubTitle  = "The new wiper parameters were saved.";
  	pagesVector[wcWizardCompleted] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[wcCloseDoorBeforeCalibratingBasket] = pPage;
  }
  {
	CStatusWizardPage *pPage = new CStatusWizardPage(LOAD_STRING(IDS_MOVING_AXIS),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
	pPage->StatusMessage = LOAD_STRING(IDS_MOVING_AXIS);
	pagesVector[wcMoveAxisBasket] = pPage;
  }
  {
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Check Wiper Basket Height",PREPARATIONS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow );
	pPage->SubTitle  = "Adjust the wiper basket height if necessary.";
	pPage->Strings.Add("The wiper basket height is adjusted");
	pagesVector[wcCalibrateBasket] = pPage;
  }
  {
	CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
	pagesVector[wcCloseDoorBeforeTestingBasket] = pPage;
  }
  {
	//Previous disable needed for continuous entering this page
	CRadioGroupWizardPage* pPage = new CRadioGroupWizardPage("Select Action",QUESTION_IMAGE_ID, wpIgnoreOnPrevious | wpHelpNotVisible);
	pPage->SubTitle  = "To test the new basket calibration, select an option and click Next.";
	pPage->Strings.Add("Go to Purge Position");
	pPage->Strings.Add("Proceed to Head Inspection.");
	pPage->DefaultOption = 1;
	pagesVector[wcPurgeWipeBasket] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Moving to purge position",IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
	pPage->PointsStatusMessage = "Moving to purge position";
	pagesVector[wcGoToPurgePosition] = pPage;
  }
  {
	CSuspensionPointsStatusPage *pPage = new CSuspensionPointsStatusPage("Wiper Inspection",GENERAL_PURPOSE_1_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
	pPage->PointsStatusMessage = "Moving to inspection position";
	pagesVector[wcResultBasketheckPosition] = pPage;
  }
  {
	CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage("Check Printer",PREPARATIONS_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible );
	pPage->SubTitle  = "Inspect the Purge Unit area to make sure basket is calibrated. If not, repeat the basket calibration.";
	pPage->Strings.Add("Repeat Basket Calibration");
	pPage->Strings.Add("Finish Basket Calibration");
    pPage->DefaultOption = ANSWER_YES;
	pagesVector[wcIsBasketCalibrated] = pPage;
  }
  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
   	AddPage(*p);

}


void CWiperCalibrationWizard::StartEvent()
{
  PrevMotorsPurgeXStartPosition = m_ParamMgr->MotorsPurgeXStartPosition;
  PrevMotorPurgeYActPosition = m_ParamMgr->MotorPurgeYActPosition;
  PrevMaxTPositionStep = m_ParamMgr->MaxPositionStep[AXIS_T];
  m_ParamMgr->MotorPurgeYActPosition = m_ParamMgr->MotorPurgeYActPosition - Y_AXIS_SAVE_DELTA;
  FirstMotorWipeTActDelta = PrevMotorWipeTActDelta = UNITIALIZED_VALUE;
  m_PreviousXSpeed = m_BackEnd->SetMotorSpeed(AXIS_X,X_AXIS_SPEED/2);
  m_PreviousYSpeed = m_BackEnd->SetMotorSpeed(AXIS_X,Y_AXIS_SPEED/2);
  m_IsCalibratingBasket = false;
}

void CWiperCalibrationWizard::EndEvent()
{
  CleanUp();
}

void CWiperCalibrationWizard::CancelEvent(CWizardPage *WizardPages)
{
  int PageIndex = GetLastRelevantPageIndex();
  if (wcNeedParameterClearTillHere >= PageIndex && !m_IsCalibratingBasket)
  {
     m_ParamMgr->MotorsPurgeXStartPosition = PrevMotorsPurgeXStartPosition;
     m_ParamMgr->MotorPurgeYActPosition = PrevMotorPurgeYActPosition;
     m_ParamMgr->MaxPositionStep[AXIS_T] = PrevMaxTPositionStep;
  }
  CleanUp();
  if (FirstMotorWipeTActDelta != UNITIALIZED_VALUE)
     if (PrevMotorWipeTActDelta != FirstMotorWipeTActDelta)
        MCB55DiagWrite(FirstMotorWipeTActDelta);
}

void CWiperCalibrationWizard::HelpEvent(CWizardPage *WizardPage)
{
	switch(WizardPage->GetPageNumber())
	{
		case  wcWelcomePage:		m_BackEnd->DispatchHelp(10, WIPER_HELP_FILE_NAME); break;
		case wcCalibratePosition:	m_BackEnd->DispatchHelp(20, WIPER_HELP_FILE_NAME); break;
		case wcInformation:			m_BackEnd->DispatchHelp(30, WIPER_HELP_FILE_NAME); break;
		case wcIsResultGood:		m_BackEnd->DispatchHelp(40, WIPER_HELP_FILE_NAME); break;
		case wcCalibrateBasket:		m_BackEnd->DispatchHelp(15, WIPER_HELP_FILE_NAME); break;

	}
}

void CWiperCalibrationWizard::CleanUp()
{
  int PageIndex = GetLastRelevantPageIndex();

  

  if (wcNeedHomingFromHere <= PageIndex)
  {
     m_BackEnd->EnableMotor(false, AXIS_ALL);
     EnableAllAxesAndHome(NULL, true, true);
     m_BackEnd->EnableMotor(false, AXIS_ALL);
  }
  //return the motors velocity to 'before wizard' value
  m_BackEnd->SetMotorSpeed(AXIS_X,m_PreviousXSpeed);
  m_BackEnd->SetMotorSpeed(AXIS_X,m_PreviousYSpeed);

  if (wcNeedEnableDoorFromHere <= PageIndex)
	 m_BackEnd->EnableDoor(false);
	 //disable the done option for next run of the wizard
  CWizardPage* pPage = GetPageById(wcPurgeWipe);
  dynamic_cast<CRadioGroupWizardPage *>(pPage)->SetDisabledMask(4);
  pPage = GetPageById(wcPurgeWipeBasket);
  dynamic_cast<CRadioGroupWizardPage *>(pPage)->SetDisabledMask(4);
}

void CWiperCalibrationWizard::PageLeave(CWizardPage *Page, TWizardPageLeaveReason LeaveReason)
{
   TWizardPagesIndex PageIndex = (TWizardPagesIndex)Page->GetPageNumber();
   if (LeaveReason != lrGoNext)
      return;

   switch(PageIndex)
   {
     case wcCheckTray:
          if( ! m_BackEnd->IsTrayInserted() )
          {
            dynamic_cast<CCheckBoxWizardPage *>(Page)->SetChecksMask(0);
            Page->Refresh();
            SetNextPage(wcCheckTray);
          }
          break;
     case wcIsResultGood:
          IsResultGoodPageLeave(Page);
          break;

     case wcCalibratePosition:
          CalibratePositionPageLeave(Page);
          break;
	 case wcPurgeWipe: //set done radio option enable after the first nextclick
			dynamic_cast<CRadioGroupWizardPage *>(Page)->SetDisabledMask(0);
			SetNextPage( (dynamic_cast<CRadioGroupWizardPage *>(Page)->GetSelectedOption() == PURGE_SELECTION) ?																	wcPurging :( dynamic_cast<CRadioGroupWizardPage *>(Page)->GetSelectedOption() == WIPE_SELECTION ? wcWiping: wcResultCheckPosition  ) );
		  break;
	 case wcPurgeWipeBasket:
	 {
			if( dynamic_cast<CRadioGroupWizardPage *>(Page)->GetSelectedOption() == 0 )
				SetNextPage( wcGoToPurgePosition );
			else
			   SetNextPage(	wcResultBasketheckPosition );
			break;
	 }
	 case wcIsBasketCalibrated:		  IsIsBasketCalibratedPageLeave(Page);
		  break;   }
}

void CWiperCalibrationWizard::PageEnter(CWizardPage *Page)
{
   TWizardPagesIndex PageIndex = (TWizardPagesIndex)Page->GetPageNumber();
   switch(PageIndex)
   {
     case wcCheckTray:
          m_BackEnd->EnableDoor(false);
          break;

     case wcCalibratePosition:
          Page->Refresh();
          break;

     case wcCalibrateTMaxPosition:
          CalibrateTMaxPositionPageEnter(Page);
          break;
           
     case wcHomeAxisPage:
		  HomeAxisPageEnter(Page);
          break;

     case wcPositionAxisPage:
		  PositionAxisPageEnter(Page);
          break;

	case wcPurgeWipe:
         SetNextPage(wcResultCheckPosition);
         break;

	case wcPurging:
		 if(m_IsCalibratingBasket)
			SetNextPage(wcPurgeWipeBasket);
		 else
			SetNextPage(wcPurgeWipe);
         PurgeWipePageEnter(Page, msPurge, msStandby1, DoPurgeSequence);
         break;

	case wcWiping:
		 if(m_IsCalibratingBasket)
			SetNextPage(wcPurgeWipeBasket);
		 else
			SetNextPage(wcPurgeWipe);
		 PurgeWipePageEnter(Page, msWipe, m_BackEnd->GetCurrentMachineState(), DoWipe);
         break;

	case wcResultCheckPosition:
	case wcResultBasketheckPosition:
         ResultCheckPositionPageEnter(Page);
		 break;
	case wcMoveAxisBasket:
		 MoveAxisBasketPageEnter(Page);
		 break;
	case wcPurgeWipeBasket:
         SetNextPage(wcResultBasketheckPosition);
		 break;
	case wcGoToPurgePosition:
         m_BackEnd->GotoPurgePosition(true, false);
		 SetNextPage(wcIsBasketCalibrated);
		 GotoNextPage();
		 break;
	case wcIsBasketCalibrated:
         m_BackEnd->EnableDoor(false);
	break;

   }
}

bool CWiperCalibrationWizard::ResultCheckPositionPageEnter(CWizardPage *WizardPage)
{                       
  EnableAllAxesAndHome(AxesTable);
  TQErrCode Err = Q_NO_ERROR;
  
  TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->XCleanHeadsPosition, NO_BLOCKING, muMM));
  TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->YCleanHeadsPosition, NO_BLOCKING, muMM));
  TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
  TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
  if (m_BackEnd->GetMotorsPosition(AXIS_Z) != m_ParamMgr->MaxPositionStep[AXIS_Z])
  {
	EnableAxeAndHome(AXIS_Z);
	TRY_FUNCTION(Err, m_BackEnd->MoveZAxisDown());
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME));
  }
  GotoNextPage();
  return true;
}

void CWiperCalibrationWizard::CalibratePositionPageLeave(CWizardPage *WizardPage)
{
  m_ParamMgr->MotorsPurgeXStartPosition = CONFIG_ConvertStepToUnits(AXIS_X,m_BackEnd->GetMotorsPosition(AXIS_X),muMM);
  m_ParamMgr->MotorPurgeYActPosition = CONFIG_ConvertStepToUnits(AXIS_Y,m_BackEnd->GetMotorsPosition(AXIS_Y),muMM) + Y_AXIS_SAVE_DELTA;
  int MotorWipeTActPositionSteps = m_BackEnd->GetMotorsPosition(AXIS_T);

  if (MotorWipeTActPositionSteps + MCB_DELTA_PARAMETER_DELTA > m_ParamMgr->MaxPositionStep[AXIS_T])
	 throw EQException("New calibrated value exceeds T max position.");

  int NewValue = PrevMotorWipeTActDelta + (MotorWipeTActPositionSteps - PrevMotorWipeTActPositionSteps) + MCB_DELTA_PARAMETER_DELTA;
  CMessageWizardPage* InfoPage = dynamic_cast<CMessageWizardPage *>(GetPageById(wcInformation));
  QString str =  QFormatStr("Current value (before last adjustments): %d\nNew calculated value: %d",FirstMotorWipeTActDelta,NewValue);
  str += "\n\nNote: The new value has not been saved to MCB yet.\n- If you want to repeat the adjustment procesures before continuing, click 'Previous'.\n- If you do not want to save the new T-height value, click 'Cancel'.\n- To test the effectiveness of the new T-height value, click 'Next'.";
  InfoPage->SubTitle = str;
  if(NewValue>=0)
  {
	PrevMotorWipeTActDelta = NewValue;
	MCB55DiagWrite(NewValue);
	EnableAxeAndHome(AXIS_T);
	InfoPage->RemoveAttribute(wpNextDisabled);
	WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("T enter: %d, T exit: %d", PrevMotorWipeTActPositionSteps, MotorWipeTActPositionSteps));
  }
  else
  {
    WriteToLogFile(LOG_TAG_GENERAL,"The new value is negative,please calibrate again" );
    TQErrCode Err = Q_NO_ERROR;
	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_T, PrevMotorWipeTActPositionSteps, true));

	InfoPage->SubTitle += QFormatStr("\n\n\nThe ‘New value’ cannot be a negative value.\nRecalibrate the wiper until the ‘New value’ is positive.");
	InfoPage->SetAttributes(InfoPage->GetAttributes() | wpNextDisabled);

  }

}

void CWiperCalibrationWizard::IsResultGoodPageLeave(CWizardPage *WizardPage)
{
  CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
  int Results  = Page->GetSelectedOption();
  if (Results == ANSWER_CALIBRATE_WIPER)
	 SetNextPage(wcHomeAxisPage);
  else
  {
     m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MotorsPurgeXStartPosition);
     m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MotorPurgeYActPosition);
	 m_ParamMgr->SaveSingleParameter(&m_ParamMgr->MaxPositionStep);

	 if (Results == NUMBER_FINISH_WIZARD)
		 SetNextPage(wcWizardCompleted);
	 else
		 m_IsCalibratingBasket = true;
  }
}

void CWiperCalibrationWizard::IsIsBasketCalibratedPageLeave(CWizardPage *WizardPage)
{
  CRadioGroupWizardPage *Page = dynamic_cast<CRadioGroupWizardPage *>(WizardPage);
  int Results  = Page->GetSelectedOption();
  if (Results == ANSWER_NO)
	SetNextPage(wcCloseDoorBeforeCalibratingBasket);
}

void CWiperCalibrationWizard::CalibrateTMaxPositionPageEnter(CWizardPage *WizardPage)
{
  TQErrCode Err = Q_NO_ERROR;

  StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true); //bug 6483

  EnableAllAxesAndHome(AxesTable);

  TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->XCleanHeadsPosition, true, muMM));

  int MinPosition = m_ParamMgr->MotorPurgeTActPosition-m_ParamMgr->MotorPurgeTTolerance;
  int MaxPosition = m_ParamMgr->MaxPositionStep.DefaultValue(AXIS_T);
  m_ParamMgr->MaxPositionStep[AXIS_T] = MaxPosition;
  
  int CurrentPosition = MinPosition + (MaxPosition-MinPosition)/2;
  while ((MaxPosition-MinPosition) >= 2)
  {
     if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
     CurrentPosition = ((CurrentPosition < MinPosition) ? MinPosition : (MinPosition + (MaxPosition-MinPosition)/2));
     if ((Err = m_BackEnd->MoveMotorToAbsolutePosition(AXIS_T, CurrentPosition, true, muSteps)) == Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT)
     {
        MaxPosition = CurrentPosition;
     }
     else if (Err == Q_NO_ERROR)
     {
        MinPosition = CurrentPosition;
     }
     else //Unexpected error
     { //Makes an exception in case there is an error
        TRY_FUNCTION(Err, Err);
     }
     EnableAxeAndHome(AXIS_T, BLOCKING, true);
	 YieldWizardThread();
  }
  m_ParamMgr->MaxPositionStep[AXIS_T] = MinPosition;
  m_BackEnd->EnableMotor(false, AXIS_T);
  FirstCalculatedMaxTPositionStep = m_ParamMgr->MaxPositionStep[AXIS_T];
  WriteToLogFile(LOG_TAG_GENERAL, QFormatStr("Changing MaxPositionStep[Axis_T] to %d", (int)m_ParamMgr->MaxPositionStep[AXIS_T]));
  GotoNextPage();
}

void CWiperCalibrationWizard::HomeAxisPageEnter(CWizardPage *WizardPage)
{
  if (PrevMotorWipeTActDelta == UNITIALIZED_VALUE)
  {
	 FirstMotorWipeTActDelta = MCB55DiagRead();

	 if (FirstMotorWipeTActDelta > MCB_PARAM_MAX_VALUE)
	 {
		 PrevMotorWipeTActDelta = MCB_PARAM_DEFAULT_VALUE;
		 MCB55DiagWrite(MCB_PARAM_DEFAULT_VALUE);
	 }
	 else
		 PrevMotorWipeTActDelta = FirstMotorWipeTActDelta;
  }

  EnableAllAxesAndHome(AxesTable);  //NoZHome
  GotoNextPage();
}


bool CWiperCalibrationWizard::PositionAxisPageEnter(CWizardPage *WizardPage)
{
    TQErrCode Err = Q_NO_ERROR;

    TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->MotorsPurgeXStartPosition, false, muMM));

    float YPos = (m_ParamMgr->MotorPurgeYActPosition  < CONFIG_ConvertStepToUnits(AXIS_Y,m_ParamMgr->MinPositionStep[AXIS_Y],muMM))?
				   CONFIG_ConvertStepToUnits(AXIS_Y,m_ParamMgr->MinPositionStep[AXIS_Y],muMM) : (m_ParamMgr->MotorPurgeYActPosition );

    TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, YPos, false, muMM));
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));

	TRY_FUNCTION(Err, m_BackEnd->MoveMotorToTWipePosition());
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_T));
	PrevMotorWipeTActPositionSteps = CONFIG_ConvertStepToUnits(AXIS_T,m_BackEnd->GetMotorsPosition(AXIS_T),muSteps);

	TRY_FUNCTION(Err, m_BackEnd->MoveMotorRelative(AXIS_T, (CHECK_EMULATION(m_ParamMgr->OCB_Emulation)) ? 0 : (-1*MCB_DELTA_PARAMETER_DELTA), true));
	TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_T));

  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

  StartAsyncHeatingCycle(TOTAL_NUMBER_OF_HEATERS, true);

  GotoNextPage();
  return true;
}

void __fastcall CWiperCalibrationWizard::DoWipe(TObject *Sender)
{
  m_BackEnd->DoWipe();
}

void __fastcall CWiperCalibrationWizard::DoPurgeSequence(TObject *Sender)
{
  m_BackEnd->DoPurgeSequence( false /* PerformSinglePurgeInSequence */);
}

void CWiperCalibrationWizard::PurgeWipePageEnter(CWizardPage *WizardPage, TMachineState checkState, TMachineState doneState, TNotifyEvent FuncPtr)
{
    m_BackEnd->SetOnlineOffline(false);
	if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
    FuncPtr(NULL);
    WaitForStateToDiff(checkState);
	if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
    GotoNextPage();
}

void CWiperCalibrationWizard::MCB55DiagWrite(int Value)
{                                         //todo - do not ignore return value
  TQErrCode Err = Q_NO_ERROR;
  m_BackEnd->EnableMotor(false, AXIS_ALL);
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->MCBEnterDiag());
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->MCBDiagWrite(MCB_DELTA_PARAMETER, Value));
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->MCBReset());
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->WaitForEndOfSWReset(10));
  m_BackEnd->SetMotorSpeed(AXIS_X,X_AXIS_SPEED/2);
  m_BackEnd->SetMotorSpeed(AXIS_X,Y_AXIS_SPEED/2);

  m_ParamMgr->MaxPositionStep[AXIS_T] = FirstCalculatedMaxTPositionStep + PrevMotorWipeTActDelta - FirstMotorWipeTActDelta;
}

int CWiperCalibrationWizard::MCB55DiagRead()
{
  if (CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
     return 36;
  TQErrCode Err = Q_NO_ERROR;
  int Value;
  m_BackEnd->EnableMotor(false, AXIS_ALL);
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->MCBEnterDiag());
  Value = m_BackEnd->MCBDiagRead(MCB_DELTA_PARAMETER);
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->MCBReset());
  TRY_FUNCTION_IGNORE_CANCELLED(Err, m_BackEnd->WaitForEndOfSWReset(10));
  m_BackEnd->SetMotorSpeed(AXIS_X,X_AXIS_SPEED/2);
  m_BackEnd->SetMotorSpeed(AXIS_X,Y_AXIS_SPEED/2);
  return Value;
}


void CWiperCalibrationWizard::MoveAxisBasketPageEnter(CWizardPage *WizardPage)
{
	EnableAllAxesAndHome(NULL, true, true);
	m_BackEnd->MoveMotorToTPurgePosition();
	m_BackEnd->WaitForEndOfMovement(AXIS_T);
	m_BackEnd->EnableDoor(false);
	GotoNextPage();
}
