/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: UV Replacement.                                          *
 * Module Description: UV Replacement wizard.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 ********************************************************************/

#include "AppLogFile.h"
#include "AppParams.h"
#include "BackEndInterface.h"
#include "HeatersDefs.h"
#include "MotorDefs.h"
#include "Q2RTApplication.h"
#include "QErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "UVLampsStatusFrame.h"
#include "UVReplacementWizard.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "MaintenanceCountersDefs.h"
#include "WizardImages.h"

const int  PRIMARY_HQ_LEVEL  = 2;
const int  PRIMARY_HS_LEVEL  = 3;
const int  PRIMARY_MM_LEVEL  = 2;
const int  PRIMARY_DJ1_LEVEL = 3;

const int  LAMP_REPLACEMENT_OPTION = 0;
const int  CALIBRATION_ONLY_OPTION = 1;
const int  POWER_TEST_MODE_OPTION  = 2;

const int  PERCENTAGE_THRESHOLD = 10;
const int  UV_LAMPS_POLL_TIME = 100;

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);
extern QString YesNoAnswerStrings[];
extern const int UVLampCounterID[NUMBER_OF_UV_LAMPS];

enum {uvrWelcomePage,                   //  1
      uvrInsertTray1Page,               //  2
      uvrVerifyCleanTrayPage,           //  3
      uvrCloseDoor1Page,                //  4
      uvrHomeAxisPage,                  //  5
      uvrChooseModePage,                //  6
      uvrChooseReplacedLampPage,        //  7
      uvrMoveBlockToReplacementPosPage, //  8
      uvrReplaceLampPage,               //  9
      uvrLampReplacementDonePage,       // 10
      uvrInsertTray2Page,               // 11
      uvrCloseDoor2Page,                // 12
      uvrLampIgnition1Page,             // 13
      uvrIgnitionProblemPage,           // 14
      uvrMoveBlockToSensorPos1Page,     // 15
      uvrAdjustUVLampsPage,             // 16
      uvrStatusPage,                    // 17
      uvrResetUVCalibrationPage,        // 18
      uvrCompletionPage,                // 19
	  uvrLastPage,                      // 20

      uvrPagesCount};

namespace UVReplacementWiz
{
	enum {
		END_WIZARD,
		RERUN_WIZARD,
		NUM_OF_OPTIONS
	};
};

CUVReplacementWizard::~CUVReplacementWizard()
{
  delete m_UVONCompletionPageTimer;
}

CUVReplacementWizard::CUVReplacementWizard(void) : CQ2RTAutoWizard(IDS_UVR_WIZARD,true,IN_PROCESS_IMAGE_ID)
{
    memset(&m_WizardMode, 0, sizeof(TUVAdjustMode));
    memset(m_IsLampReplaced, 0, sizeof(m_IsLampReplaced));
    m_CleanUpRequired =  false;
    m_QualityModeIndex = 0;
    m_OperationModeIndex = 0;
    m_RunUVCalibrationWizard = false;
  TPagePointers pagesVector(uvrPagesCount, 0);
  {
     CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(),FULL_BLOCK_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
     pPage->SubTitle = LOAD_STRING(IDS_UVR_WIZARD_SUBTITLE);
     pagesVector[uvrWelcomePage] = pPage;
  }
  {
     CInsertTrayPage *pPage = new CInsertTrayPage(this, FULL_BLOCK_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
     pagesVector[uvrInsertTray1Page] = pPage;
  }
  {
     CVerifyCleanTrayPage *pPage = new CVerifyCleanTrayPage(this, FULL_BLOCK_IMAGE_ID, wpPreviousDisabled | wpHelpNotVisible);
     pagesVector[uvrVerifyCleanTrayPage] = pPage;
  }
  {
     CCloseDoorPage *pPage = new CCloseDoorPage(this,FULL_BLOCK_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
     pagesVector[uvrCloseDoor1Page] = pPage;
  }
  {
     CHomingAxisPage *pPage = new CHomingAxisPage(this,"",FULL_BLOCK_IMAGE_ID, wpHelpNotVisible);
  	 pagesVector[uvrHomeAxisPage] = pPage;
  }
  {
     CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SELECT_UVR_MODE),FULL_BLOCK_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
     pPage->Strings.Add(LOAD_STRING(IDS_UVR_REPLACEMENT_AND_ADJUSTMENT));
     pPage->Strings.Add(LOAD_STRING(IDS_UVR_ADJUSTMENT_ONLY));
     pPage->Strings.Add(LOAD_STRING(IDS_UVR_EVALUATION_ONLY));
     pPage->DefaultOption = LAMP_REPLACEMENT_OPTION;
     pagesVector[uvrChooseModePage] = pPage;
  }
  {
     CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID,wpPreviousDisabled | wpNextOneChecked | wpHelpNotVisible);
	 for(int i = 0; i < NUMBER_OF_UV_LAMPS; i++)
	 {
		 QString Ret;
		 if(VALIDATE_UV_LAMP(i))
			Ret = LOAD_QSTRING_ARRAY(i,IDS_UVLAMPS_REPLACING_STRINGS);
		 pPage->Strings.Add(Ret.c_str());
	 }
        pagesVector[uvrChooseReplacedLampPage] = pPage;
  }
  {
     CStatusWizardPage *pPage = new CStatusWizardPage(LOAD_STRING(IDS_MOVING_AXIS),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
     pagesVector[uvrMoveBlockToReplacementPosPage] = pPage;
  }
  {
     CMessageWizardPage *pPage = new CMessageWizardPage(LOAD_STRING(IDS_REPLACE_LAMP_CAPTION),PREPARATIONS_IMAGE_ID,wpPreviousDisabled);
     pPage->SubTitle = LOAD_STRING(IDS_REPLACE_LAMP);
     pagesVector[uvrReplaceLampPage] = pPage;
  }
  {
     CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("Restore Printer",SHADOW_BRACKET_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected);
	 pPage->SubTitle = LOAD_STRING(IDS_SYSTEM_CHECKLIST_CONFIRM);
     pPage->Strings.Add(LOAD_STRING(IDS_REPLACED_LAMP));
     pPage->Strings.Add(LOAD_STRING(IDS_CORRECT_SHADOW_LOCATION));
     pagesVector[uvrLampReplacementDonePage] = pPage;
  }
  {
     CInsertTrayPage *pPage = new CInsertTrayPage(this, FULL_BLOCK_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
     pagesVector[uvrInsertTray2Page] = pPage;
  }
  {
     CCloseDoorPage *pPage = new CCloseDoorPage(this,PREPARATIONS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
     pagesVector[uvrCloseDoor2Page] = pPage;
  }
  {
     CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage(LOAD_STRING(IDS_UVR_WARMING_LAMPS_CAPTION),-1,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
     pPage->SubTitle = QFormatStr(LOAD_STRING(IDS_UV_WARMING_DURATION_STR), 4); // QFormatStr(LOAD_STRING(IDS_DURATION_STR), LOAD_STRING(IDS_UV_LAMPS_WARMING),"4");
     pagesVector[uvrLampIgnition1Page] = pPage;
  }
  {
     CMessageWizardPage *pPage = new CMessageWizardPage(LOAD_STRING(IDS_IGNITION_MALFUNCTION),-1,wpPreviousDisabled | wpHelpNotVisible);
     pPage->SubTitle = LOAD_STRING(IDS_CHECK_UV_CABLE);
     pagesVector[uvrIgnitionProblemPage] = pPage;
  }
  {
     CStatusWizardPage *pPage = new CStatusWizardPage(LOAD_STRING(IDS_MOVING_AXIS),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
     pagesVector[uvrMoveBlockToSensorPos1Page] = pPage;
  }
  {
     CElapsingTimeWizardPage *pPage = new CElapsingTimeWizardPage(LOAD_STRING(IDS_CALIBRATING_UV),-1,wpPreviousDisabled | wpNextDisabled | wpCancelDisabled | wpHelpNotVisible);
     pPage->SubTitle = "(Up to 15 min)";
     pagesVector[uvrAdjustUVLampsPage] = pPage;
  }
  {
     CUVLampsStatusWizardPage *pPage = new CUVLampsStatusWizardPage(LOAD_STRING(IDS_INTENSITY_RESULTS),LAMPS_STATUS_IMAGE_ID,wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible);
     pagesVector[uvrStatusPage] = pPage;
  }
  {
     CRadioGroupWizardPage* pPage = new CRadioGroupWizardPage(GetTitle(),-1, wpPreviousDisabled | wpHelpNotVisible);
     pPage->SubTitle = QFormatStr("Would you like to reset the '%s' counter?", CounterNameLookup[UV_LAMPS_CALIBRATION_COUNTER_ID]);
     for(int i = 0; i < NUMBER_OF_ANSWERS; i++)
        pPage->Strings.Add(YesNoAnswerStrings[i]);
     pPage->DefaultOption = ANSWER_YES;
     pagesVector[uvrResetUVCalibrationPage] = pPage;
  }
  {
     CRadioGroupWizardPage *pPage = new CRadioGroupWizardPage(LOAD_STRING(IDS_SYSTEM_ACTION_SELECT),QUESTION_IMAGE_ID, wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
	 pPage->Strings.Add("End the wizard.");
     pPage->Strings.Add("Run the UV Calibration Wizard now.");
     pPage->DefaultOption = UVReplacementWiz::END_WIZARD;
	// pPage->SubTitle = QFormatStr(LOAD_STRING(IDS_RUN_ANOTHER_WIZARD_NOW),"UV calibration wizard");
     pagesVector[uvrCompletionPage] = pPage;
  }
  {
     CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible | wpDonePage);
     pagesVector[uvrLastPage] = pPage;  
  }

  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
   	AddPage(*p);

  m_UVONCompletionPageTimer           = new TTimer(NULL);
  m_UVONCompletionPageTimer->Enabled  = false;
  m_UVONCompletionPageTimer->OnTimer  = UVONCompletionPageTimerEvent;
  m_UVONCompletionPageTimer->Interval = USER_ENTRY_RESPONSE_TIMEOUT * 1000 /*2 minutes*/;
};


// Start the wizard session event
void CUVReplacementWizard::StartEvent()
{
  m_WizardMode = IntensityEvaluationMode;
  m_RunUVCalibrationWizard = false;

  m_BackEnd->OpenUVLampsHistoryFile();
  m_BackEnd->WriteToUVHistoryFile("\n");
  m_BackEnd->WriteToUVHistoryFile("Line");
  m_BackEnd->WriteToUVHistoryFile(GetTitle());

  // Resettig the mode
  m_BackEnd->GotoDefaultMode();

  m_CleanUpRequired        = false;

  // Enable Changes in modes without asking the user to aprove
     FOR_ALL_QUALITY_MODES(qm)
	 FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om) == true)
              m_BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
}

// End the wizard session event
void CUVReplacementWizard::EndEvent()
{
  m_BackEnd->WriteToUVHistoryFile(WIZARD_COMPLETED_STR);
  m_BackEnd->WriteToUVHistoryFile("Line");
  CleanUp();
}

// Cancel the wizard session event
void CUVReplacementWizard::CancelEvent(CWizardPage *WizardPage)
{
  m_BackEnd->WriteToUVHistoryFile(WIZARD_CANCELED_STR);
  m_BackEnd->WriteToUVHistoryFile("Line");
  CleanUp();
}

void CUVReplacementWizard::ResetUVCalibrationPageLeave(CWizardPage *WizardPage)
{
  if (dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption() == ANSWER_YES)
     for(int i = 0; i < NUMBER_OF_UV_LAMPS; i++)
        if (m_IsLampReplaced[i])
           m_BackEnd->ResetMaintenanceCounter(UVLampCounterID[i]);
}

void CUVReplacementWizard::ChooseModePageLeave(CWizardPage *WizardPage)
{
  // Updating the Mode according to the user click....
  m_WizardMode = (TUVAdjustMode) dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption();
  switch (m_WizardMode)
  {
    case ReplacementAndAdjustmentMode:
       #pragma warn -8066 // Disable "Unreachable code" warning
       #pragma warn -8008 // Disable "Condition is always true" warning
      if (NUMBER_OF_UV_LAMPS <= 1)
      {
          m_IsLampReplaced[0] = true;
          SetNextPage(uvrMoveBlockToReplacementPosPage);
      }
      #pragma warn .8066 // Enable "Unreachable code" warning
      #pragma warn .8008 // Enable "Condition is always true" warning
      break;

    case UVAdjustmentOnlyMode:
      m_BackEnd->WriteToUVHistoryFile("UV lamp adjustment without replacement mode");
      // There is no need for Lamp replacement - go directly to ignition page.
      SetNextPage(uvrLampIgnition1Page);
      break;

    case IntensityEvaluationMode:
      m_BackEnd->WriteToUVHistoryFile("UV lamp intensity evaluation mode");
      // There is no need for Lamp replacement - go directly to ignition page.
      SetNextPage(uvrLampIgnition1Page);
      break;
  }
}

void CUVReplacementWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  // Respond only to 'next' events
  if (LeaveReason != lrGoNext)
    return;

  switch(WizardPage->GetPageNumber())
  {
    case uvrResetUVCalibrationPage:
         ResetUVCalibrationPageLeave(WizardPage);
         break;
       
    case uvrCloseDoor1Page:
         m_CleanUpRequired = true;
         break;

    case uvrChooseModePage:
         ChooseModePageLeave(WizardPage);
         break;

    case uvrChooseReplacedLampPage:
    {
      // Updating the Mode according to the user click....
      CCheckBoxWizardPage* Page       = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
      unsigned             ChecksMask = Page->GetChecksMask();
      
      for(int i = 0; i < NUMBER_OF_UV_LAMPS; i++)
      {
          m_IsLampReplaced[i] = (ChecksMask  & (1 << i));
          if (m_IsLampReplaced[i])
             m_BackEnd->WriteToUVHistoryFile("Replacing " + GetUVLampStr(i));
      }
      break;
    }

    case uvrLampReplacementDonePage:
    {
      USHORT PrimaryValue[NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES] =
                      {{m_ParamMgr->UVPowerSupplyValuesArray30[PRIMARY_HS_LEVEL],
                        m_ParamMgr->UVPowerSupplyValuesArray30[PRIMARY_HQ_LEVEL]},
                       {m_ParamMgr->UVPowerSupplyValuesArray30[PRIMARY_MM_LEVEL],
                        m_ParamMgr->UVPowerSupplyValuesArray30[PRIMARY_DJ1_LEVEL]}};

      FOR_ALL_QUALITY_MODES(qm)
      {
		  FOR_ALL_OPERATION_MODES(om)
          {
              if (GetModeAccessibility(qm, om) == false)
                  continue;
              m_BackEnd->GotoDefaultMode();
              m_BackEnd->EnterMode(PRINT_MODE[qm][om],MACHINE_QUALITY_MODES_DIR(qm,om));
              FOR_ALL_UV_LAMPS(l)
                if (m_IsLampReplaced[l])
                  m_ParamMgr->UVLampPSValueArray[l] = PrimaryValue[qm][om];
                  
              m_ParamMgr->SaveSingleParameter(&m_ParamMgr->UVLampPSValueArray);
          }
      }
      // Returning to default mode
      m_BackEnd->GotoDefaultMode();

      // Disable Help Button
      EnableDisableHelp(false);

      break;
    }

    case uvrAdjustUVLampsPage:
         // Turn off UV Lamps
         TurnUVLamps(false);
         break;

    case uvrCompletionPage:
         m_RunUVCalibrationWizard = (dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption() == UVReplacementWiz::RERUN_WIZARD);
         break;
  }
}

void CUVReplacementWizard::ChooseModePageEnter(CWizardPage *WizardPage)
{
  // In case there is no internal UV sensor in the machine.
  m_BackEnd->WriteToUVHistoryFile("Date");
  m_BackEnd->WriteToUVHistoryFile("Replacing UV lamp");
  m_WizardMode = ReplacementAndAdjustmentMode;
  GotoNextPage();
}

void CUVReplacementWizard::MoveBlockToReplacementPosPageEnter(CWizardPage *WizardPage)
{
  CStatusWizardPage *Page = dynamic_cast<CStatusWizardPage *>(WizardPage);
  EnableDisableNext(false);
  Page->StatusMessage = "X Axis";
  Page->Refresh();
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->XCleanHeadsPosition, true, muMM);
  Page->StatusMessage = "Y Axis";
  Page->Refresh();
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->YCleanHeadsPosition, true, muMM);
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  GotoNextPage();
}

void CUVReplacementWizard::ReplaceLampPageEnter(CWizardPage *WizardPage)
{
  m_BackEnd->EnableDoor(false);
  EnableDisableHelp(true);
}

void CUVReplacementWizard::LampIgnition1PageEnter(CWizardPage *WizardPage)
{
  EnableDisableHelp(false);
  if (m_BackEnd->EnableDoor(true) != Q_NO_ERROR)
    throw EQException("Unable to lock door");

  EnableAllAxesAndHome();
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  if (UVLampIgnition() == true)
  {
    // Check if the UV lamps are on
    if (m_BackEnd->AreUVLampsTurnedOn())
    {
      SetNextPage(uvrMoveBlockToSensorPos1Page);
      // There is no internal UV sensor - finish wizard.
      TurnUVLamps(false);
      SetNextPage(uvrResetUVCalibrationPage);
    }
  }
  GotoNextPage();
}                               

void CUVReplacementWizard::MoveBlockToSensorPos1PageEnter(CWizardPage *WizardPage)
{
  CStatusWizardPage *Page = dynamic_cast<CStatusWizardPage *>(WizardPage);
  EnableDisableNext(false);
  Page->StatusMessage = LOAD_STRING(IDS_HOME_AXIS);
  Page->Refresh();
  EnableAllAxesAndHome();
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

  Page->StatusMessage = "Move Axis X to sensor location";
  Page->Refresh();
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->UVSensorLampXPositionArray[RIGHT_UV_LAMP], BLOCKING, muMM);

  Page->StatusMessage = "Move Axis Y to sensor location";
  Page->Refresh();
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->UVSensorLampYPositionArray[RIGHT_UV_LAMP], BLOCKING, muMM);

  GotoNextPage();
}

void CUVReplacementWizard::AdjustUVLampsPageEnter(CWizardPage *WizardPage)
{
/*
  CElapsingTimeWizardPage* Page = dynamic_cast<CElapsingTimeWizardPage *>(WizardPage);
  TMachineState            State;
  m_BackEnd->CalibrateUVLamps(m_WizardMode);
  Page->Title = ((m_WizardMode == IntensityEvaluationMode) ? "Evaluating the UV lamps" : "Calibrating the UV lamps");
  Page->Refresh();

  if (WaitForStateToDiff(msCalibrateUV) == msCalibrateUV)
     return;
  WaitForState(msCalibrateUV);  
*/
  GotoPage(uvrStatusPage);
}

void CUVReplacementWizard::StatusPageEnter(CWizardPage *WizardPage)
{
  CUVLampsStatusWizardPage* Page = dynamic_cast<CUVLampsStatusWizardPage *>(WizardPage);
  QString                   LampOutputRemark[NUMBER_OF_UV_LAMPS];
  bool                      LampOutputResult[NUMBER_OF_UV_LAMPS];

  for(m_QualityModeIndex = 0; m_QualityModeIndex < NUMBER_OF_QUALITY_MODES; m_QualityModeIndex++)
  {
	FOR_ALL_OPERATION_MODES(om)
    {
		m_OperationModeIndex = om;
      if (GetModeAccessibility(m_QualityModeIndex, m_OperationModeIndex) == false)
          continue;
          
      m_BackEnd->GotoDefaultMode();
      m_BackEnd->EnterMode(PRINT_MODE[m_QualityModeIndex][m_OperationModeIndex],
                           MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));
      FillInResults(LampOutputRemark,LampOutputResult);
      for (int l = 0; l < NUMBER_OF_UV_LAMPS; l++)
      {
          Page->UVStatusLabel[l][m_QualityModeIndex][m_OperationModeIndex] = LampOutputRemark[l];
          Page->LampStatus   [l][m_QualityModeIndex][m_OperationModeIndex] = LampOutputResult[l];
      }
    }
  }

  m_OperationModeIndex++;
  if (VALIDATE_OPERATION_MODE(m_OperationModeIndex))
     SetNextPage(uvrStatusPage);
  else
     m_OperationModeIndex = 0;
  Page->Refresh();
}

void CUVReplacementWizard::PageEnter(CWizardPage *WizardPage)
{
  RestartUVONTimer();
  switch(WizardPage->GetPageNumber())
  {
    case uvrChooseModePage:
         ChooseModePageEnter(WizardPage);
         break;

    case uvrMoveBlockToReplacementPosPage:
         MoveBlockToReplacementPosPageEnter(WizardPage);
         break;

    case uvrReplaceLampPage:
         ReplaceLampPageEnter(WizardPage);
         break;

    case uvrLampIgnition1Page:
         LampIgnition1PageEnter(WizardPage);
         break;

    case uvrIgnitionProblemPage:
         SetNextPage(uvrMoveBlockToReplacementPosPage);
         break;

    case uvrMoveBlockToSensorPos1Page:
         MoveBlockToSensorPos1PageEnter(WizardPage);
         break;

    case uvrAdjustUVLampsPage:
         AdjustUVLampsPageEnter(WizardPage);
         break;

    case uvrResetUVCalibrationPage:
         //always reset the counters
         GotoNextPage();
         break;

    case uvrStatusPage:
         StatusPageEnter(WizardPage);
         break;

    case uvrCompletionPage:
         CleanUp();
         break;
  }
}

void CUVReplacementWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();	
	switch(WizardPage->GetPageNumber())
	{
		case uvrReplaceLampPage:         Application->HelpContext(10); break;		
		case uvrLampReplacementDonePage: Application->HelpContext(20); break;	
		default: break;
	}
}

// Enable all axes and home (if needed)
void CUVReplacementWizard::EnableAllAxesAndHome()
{
  CQ2RTAutoWizard::EnableAllAxesAndHome();
  m_BackEnd->EnableMotor(false, AXIS_T);
}

void CUVReplacementWizard::CleanUp()
{
  if (!m_CleanUpRequired)
     return;
     
  m_BackEnd->CloseUVLampsHistoryFile();
  m_BackEnd->GotoDefaultMode();

  // Turn off UV Lamps
  TurnUVLamps(false);

  // Home X & Y Axis
  m_BackEnd->GoToMotorHomePosition(AXIS_Y, true);
  m_BackEnd->GoToMotorHomePosition(AXIS_X, true);

  // Open the door
  m_BackEnd->EnableDoor(false);

			FOR_ALL_OPERATION_MODES(om)
  FOR_ALL_QUALITY_MODES(qm)
        if (GetModeAccessibility(qm, om) == true)
           m_BackEnd->DisableConfirmationBypass(PER_MACHINE_MODE[qm][om],
                                                MACHINE_QUALITY_MODES_DIR(qm, om));
}

bool CUVReplacementWizard::UVLampIgnition()
{
  unsigned EndTime;

  // Entering 'High Quality' Mode
  m_BackEnd->EnterMode(PRINT_MODE[0][0],MACHINE_QUALITY_MODES_DIR(0,0));

  // Setting PowerSupply params according to the mode
   FOR_ALL_UV_LAMPS(l)
      m_BackEnd->SetUVD2AValue(l, m_ParamMgr->UVLampPSValueArray[l]);

  // Turn UV lamps on
  m_BackEnd->SetDefaultParmIgnitionTimeout();
  m_BackEnd->TurnUVLamps(true);
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  EndTime = QGetTicks() + QSecondsToTicks(m_ParamMgr->UVLampIgnitionTime);
  // Wait for UV lamps to turn on
  while (IsCancelled() != true)
  {
    if (QGetTicks() > EndTime)
      return false;

    if (m_BackEnd->AreUVLampsTurnedOn())
      break;

    YieldAndSleepWizardThread();
    QSleep(UV_LAMPS_POLL_TIME);
  }

  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  EndTime = QGetTicks() + QSecondsToTicks(m_ParamMgr->UVCalibrationPostIgnitionTime);

  // Wait for UV lamps post ignition time
  while (IsCancelled() != true)
  {
    if (QGetTicks() > EndTime)
      break;

    YieldAndSleepWizardThread();
    QSleep(UV_LAMPS_POLL_TIME);
  }
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  m_UVONCompletionPageTimer->Enabled = true;
  return true;
}

void CUVReplacementWizard::FillInResults(QString* LampOutputRemarkArray,
                                         bool*    LampOutputResultArray)
{
  int LampA2DValue         [NUMBER_OF_UV_LAMPS];
  int LampDesiredPercentage[NUMBER_OF_UV_LAMPS];
  int LampActualPercentage [NUMBER_OF_UV_LAMPS];
  int LampPSLevel          [NUMBER_OF_UV_LAMPS];

  for (int i = 0; i < SIZE_OF_UV_POWER_SUPPLY_ARRAY_30; i++)
  {
   FOR_ALL_UV_LAMPS(l)
        if (m_ParamMgr->UVLampPSValueArray[l] >= m_ParamMgr->UVPowerSupplyValuesArray30[i])
           LampPSLevel[l] = i;
  }

  for(int i = 0; i < NUMBER_OF_UV_LAMPS; i++)
  {
      LampA2DValue         [i] = m_ParamMgr->UVLampPSValueArray[i];
      LampActualPercentage [i] = (LampA2DValue[i] - m_ParamMgr->UVSensorLampOffsetArray[i]) / m_ParamMgr->UVSensorLampGainArray[i];
      LampDesiredPercentage[i] = m_ParamMgr->UVDesiredPercentageLampValueArray[i];
      ProcessResult(LampDesiredPercentage[i], LampActualPercentage[i],  LampPSLevel[i],
                    LampOutputRemarkArray[i], LampOutputResultArray[i], i);
  }
}


void CUVReplacementWizard::ProcessResult(int      DesiredPercentage,
                                         int      ActualPercentage,
                                         int      CurrPSLevel,
                                         QString& OutputRemark,
                                         bool&    OutputResult,
                                         int      Lamp)
{
  OutputRemark        = "";
  OutputResult        = true;
  QString UVLogStatus = QFormatStr("%s (%s): Desired %d%, got %d%.",
                                   GetUVLampStr(Lamp),
                                   GetModeStr(m_QualityModeIndex, m_OperationModeIndex),
                                   DesiredPercentage,
                                   ActualPercentage);

  if(abs(DesiredPercentage - ActualPercentage) > PERCENTAGE_THRESHOLD)
  {
    // the lamps 'Actual Percentage' is out of the 'OK area' (~10%)
    if((DesiredPercentage - ActualPercentage) > PERCENTAGE_THRESHOLD)
    {
      // Lamp power is weaker than wanted
      OutputResult = false;
      OutputRemark = ((CurrPSLevel == (SIZE_OF_UV_POWER_SUPPLY_ARRAY_30 - 1)) ? "Should be replaced" : "Should be calibrated");
      UVLogStatus += OutputRemark;
    }
    else if(CurrPSLevel != 0)
    // Lamp power is stronger than wanted
    {
       // The lamp is in some other than the lowest level - calibration required
       OutputRemark = "Should be calibrated";
       UVLogStatus   += OutputRemark;
       OutputResult  = false;
    }
  }

  m_BackEnd->WriteToUVHistoryFile(UVLogStatus);
}

void __fastcall CUVReplacementWizard::UVONCompletionPageTimerEvent(TObject *Sender)
{
   CQLog::Write(LOG_TAG_GENERAL,"UV Lamp Replacement Wizard - User Entry response timeout");
   if (GetCurrentPage() == Pages[uvrCompletionPage])
      TurnUVLamps(false);
   else
      throw EQException("UV Lamp Replacement Wizard - User Entry response timeout");
}

void CUVReplacementWizard::TurnUVLamps(bool Enable)
{
   m_BackEnd->TurnUVLamps(Enable);
   m_UVONCompletionPageTimer->Enabled = Enable;
}

void CUVReplacementWizard::RestartUVONTimer()
{
   if (m_UVONCompletionPageTimer->Enabled == true)
   {
      m_UVONCompletionPageTimer->Enabled = false;
      m_UVONCompletionPageTimer->Enabled = true;
   }
}

bool CUVReplacementWizard::RunUVCalibrationWizard(void)
{
  return m_RunUVCalibrationWizard;
}

