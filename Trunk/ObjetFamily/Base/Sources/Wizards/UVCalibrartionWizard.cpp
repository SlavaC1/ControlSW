/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: UV Calibration.                                          *
 * Module Description: UV Calibration wizard.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Oved Ben Zeev                                            *
 * Start date: 3/3/2004                                             *
 * Last upate: 3/3/2004                                             *
 ********************************************************************/

#include "UVCalibrartionWizard.h"
#include "QTimer.h"
#include "AppParams.h"
#include "MotorDefs.h"
#include "HeatersDefs.h"
#include "QErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "AppLogFile.h"
#include "UVLampsStatusFrame.h"
#include "BackEndInterface.h"
#include "Q2RTApplication.h"
#include "AgreementPage.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "MaintenanceCountersDefs.h"
#include "WizardImages.h"

const bool LAMP_OK     = true;
const bool LAMP_NOT_OK = false;

const int TOO_WEAK     = 0;
const int TOO_STRONG   = 1;

const char HELP_FILE_NAME[]        = "\\Help\\UV_Wizard_Help_Files\\UVM Wizard.chm";

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);


// Some constants (machine dependent)
#if defined OBJET_500

  const float Y_POSITION    = 30;


  const int UV_P1[NUMBER_OF_UV_LAMPS]     = {0,180};
  const int UV_P2[NUMBER_OF_UV_LAMPS]     = {150,290};
#elif defined OBJET_350

  const int Y_POSITION       = 50.8;
  const int UV_P1[NUMBER_OF_UV_LAMPS]     = {78,262};
  const int UV_P2[NUMBER_OF_UV_LAMPS]     = {188,372};
#elif defined OBJET_260

  const float Y_POSITION    = 33;
  const int UV_P1[NUMBER_OF_UV_LAMPS]     = {0,130};
  const int UV_P2[NUMBER_OF_UV_LAMPS]     = {130,320};

#elif defined OBJET_1000
  const float Y_POSITION    = 30;
  const int UV_P1[NUMBER_OF_UV_LAMPS]     = {140,420};
  const int UV_P2[NUMBER_OF_UV_LAMPS]     = {250,530};

#endif



const int POWER_TEST_MODE_OPTION  = 0;
const int CALIBRATION_MODE_OPTION = 1;


const int X_LOOP_CYCLES_NUM              = 25;
const int PERCENTAGE_THRESHOLD           = 7;
const int PERCENTAGE_PER_STEP            = 3;
const float NORMAL_UV_POWER              = 11.5;
const int SECONDS_IN_MINUTES             = 60;
const int SECONDS_IN_HOUR                = 60 * SECONDS_IN_MINUTES;
const int UV_LAMPS_POLL_TIME             = 100;
const int DELAY_PER_STEP                 = 5;  // Seconds
const int STABILIZATION_DELAY            = 30;  // Seconds


enum {uvWelcomePage,                 
      uvUserAgreementPage,
      uvChooseWizardModePage,        
      uvVerifyCleanTrayPage,         
      uvPreparingPage,               
      uvPlaceSensorPage,             
      uvLampsOnPage,                 
      uvResetSensorPage,             
      uvLampScanPage,                
      uvInsertLampValuePage,         
      uvDisplayLampCapacityPage,     
      uvStabilizationPage,           
      uvHomeAxis,                    
      uvInterModeStabilizationPage,      
      uvTurnOffSensor,               
      uvStatusPage,
      uvCompletionPage};

CUVCalibrationWizard::CUVCalibrationWizard(void) : CQ2RTAutoWizard(IDS_UVC_WIZARD,true,IN_PROCESS_IMAGE_ID)
{

   m_UVLampIndex = 0;
   m_QualityModeIndex = 0;
   m_OperationModeIndex = 0;
   m_UVLampIndex;
   m_CleanUpRequired = false;
   m_PSValue = 0;
   m_CurPSIndex = 0 ;
   m_DelayTime = 0;
   memset (&m_WizardMode, 0 ,sizeof(m_WizardMode));
   memset (m_ValuePercentage, 0 ,sizeof(m_ValuePercentage));
   memset (m_LastValue, 0 ,sizeof(m_LastValue));
   memset (m_LampStatus, 0 ,sizeof(m_LampStatus));
   memset (m_AdditionalStatus, 0 ,sizeof(m_AdditionalStatus));
   memset (m_LampTotalStatus, 0 ,sizeof(m_LampTotalStatus)); 
  m_BackEnd = CBackEndInterface::Instance();

  CMessageWizardPage* WelcomePage = new CMessageWizardPage(GetTitle(),UV_WELCOME_PAGE_IMAGE_ID,wpPreviousDisabled);
  WelcomePage->SubTitle           = LOAD_STRING(IDS_UVC_GUIDE_THROUGH);
  AddPage(WelcomePage);

  CAgreementWizardPage* UserAgreementPage = new CAgreementWizardPage("UV Calibration Conditions",UV_WELCOME_PAGE_IMAGE_ID, wpHelpNotVisible);
  UserAgreementPage->Label1  = "BY SELECTING 'I AGREE' YOU ARE ACCEPTING THE FOLLOWING CONDITIONS FOR RUNNING THE UV-INTENSITY WIZARD.\
IF YOU DO NOT AGREE TO THESE CONDITIONS, CLICK 'CANCEL' AND CONTACT CUSTOMER SUPPORT. YOU MUST ACCEPT THE CONDITIONS TO CONTINUE.";
  UserAgreementPage->Label3  = "1. This wizard should only be used to adjust the intensity of the UV lamp(s) in this printer.\
The wizard is not designed for fixing or troubleshooting the printer.\
In case of any failure, or suspected failure, of your printer, request service from Customer Support.";

  UserAgreementPage->Label4  = "2. Refer to the instructions and the tools described in the user documentation before running this wizard.";

  UserAgreementPage->Label5  = "By checking the 'I agree', you indicate your acceptance of the conditions and compliance with the guidelines and instructions, specified in the User Guide.";
  AddPage(UserAgreementPage);

  CRadioGroupWizardPage* ChooseModePage =
			  new CRadioGroupWizardPage(LOAD_STRING(IDS_UVC_CHOOSE_MODE),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected);
  ChooseModePage->Strings.Add(LOAD_STRING(IDS_POWER_TEST));
  ChooseModePage->Strings.Add(LOAD_STRING(IDS_UVC_WIZARD));
  ChooseModePage->DefaultOption = POWER_TEST_MODE_OPTION;
  AddPage(ChooseModePage);

  CCheckBoxWizardPage* VerifyCleanTrayPage = new CCheckBoxWizardPage("",QUESTION_IMAGE_ID,wpNextWhenSelected | wpHelpNotVisible);
  VerifyCleanTrayPage->Strings.Add(LOAD_STRING(IDS_HOOD_IS_CLOSED));
  if(m_ParamMgr->RemovableTray) //runtime objet
  {
//#if defined EDEN_250 || defined EDEN_260 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
        VerifyCleanTrayPage->Strings.Add(LOAD_STRING(IDS_TRAY_IS_INSERTED));
//#endif
  }
  VerifyCleanTrayPage->Strings.Add(LOAD_STRING(IDS_TRAY_IS_EMPTY));
  AddPage(VerifyCleanTrayPage);

  CStatusWizardPage* PreparingPage = new CStatusWizardPage(LOAD_STRING(IDS_PREPARING),QUESTION_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
  AddPage(PreparingPage);

  CCheckBoxWizardPage* PlaceSensorPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SENSOR_SETUP),UV_PLACE_SENSOR_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected);
  PlaceSensorPage->Strings.Add(m_BackEnd->GetDoorMessage());
  PlaceSensorPage->Strings.Add(m_BackEnd->GetDoorMessage());
  PlaceSensorPage->Strings.Add(LOAD_STRING(IDS_TURN_ON_UV_METER));
  AddPage(PlaceSensorPage);

  CElapsingTimeWizardPage* LampsOnPage = new CElapsingTimeWizardPage("",UV_LAMPS_WARMING_IMAGE_ID,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
  AddPage(LampsOnPage);

  CCheckBoxWizardPage* ResetSensorPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_RESET_UV_SENSOR),UV_RESET_SENSOR_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected | wpHelpNotVisible);
  ResetSensorPage->Strings.Add(LOAD_STRING(IDS_DONE));
  AddPage(ResetSensorPage);

  CProgressWizardPage* LampScanPage = new CProgressWizardPage("",UV_LEFT_SCANNING_IMAGE_ID,wpCancelDisabled | wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
  AddPage(LampScanPage);

  CDataEntryWizardPage* InsertLampValuePage = new CDataEntryWizardPage(LOAD_STRING(IDS_ENTER_UV_READOUT),UV_ENTER_VLAUE_IMAGE_ID,wpPreviousDisabled);
  InsertLampValuePage->Strings.Add("");
  AddPage(InsertLampValuePage);
  InsertLampValuePage->FieldsTypes[0] = ftFloat;

  CStatusWizardPage* DisplayLampCapacityPage = new CStatusWizardPage("",IN_PROCESS_IMAGE_ID,wpPreviousDisabled);
  AddPage(DisplayLampCapacityPage);

  CElapsingTimeWizardPage* StabilizationPage = new CElapsingTimeWizardPage(LOAD_STRING(IDS_STABILIZATION),-1,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
  AddPage(StabilizationPage);

  CStatusWizardPage* HomeAxis = new CStatusWizardPage(LOAD_STRING(IDS_HOME_AXIS),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
  AddPage(HomeAxis);

  CElapsingTimeWizardPage* InterModeStabilizationPage = new CElapsingTimeWizardPage(LOAD_STRING(IDS_STABILIZATION),-1,wpPreviousDisabled | wpNextDisabled | wpHelpNotVisible);
  AddPage(InterModeStabilizationPage);

  CCheckBoxWizardPage* TurnOffSensorPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_TURN_OFF_SENSOR),UV_TURN_OFF_SENSOR_IMAGE_ID,wpPreviousDisabled | wpNextWhenSelected | wpHelpNotVisible);
  TurnOffSensorPage->Strings.Add(LOAD_STRING(IDS_DONE));
  AddPage(TurnOffSensorPage);

  CUVLampsStatusWizardPage* StatusPage = new CUVLampsStatusWizardPage("",UV_WELCOME_PAGE_IMAGE_ID,wpCancelDisabled | wpPreviousDisabled);
  AddPage(StatusPage);

  CMessageWizardPage* CompletionPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpCancelDisabled | wpPreviousDisabled | wpHelpNotVisible);
  AddPage(CompletionPage);
};

// Start the wizard session event
void CUVCalibrationWizard::StartEvent()
{
  int TotalPrintingHours = 0;
  m_WizardMode           = PowerTestMode;
  m_WizardModeStr        = LOAD_STRING(IDS_POWER_TEST);
  
  // write to the histroy file now because this is a power-test-only version
  m_BackEnd->WriteToUVHistoryFile("UV Power Test Wizard:");
  m_BackEnd->WriteToUVHistoryFile("----------------------");

  // Enable Changes in modes without asking the user to aprove
  for(int i = 0; i < NUMBER_OF_UV_LAMPS; i++)
      m_LampStatus[i] = LAMP_OK;

  FOR_ALL_QUALITY_MODES(qm)
  {
	  FOR_ALL_OPERATION_MODES(om)
      {
          if (GetModeAccessibility(qm, om) == false)
              continue;
              
   for(int l=0;l<NUMBER_OF_UV_LAMPS;l++)
          {
              m_UVStatusLabel[l][qm][om]   = "";
              m_LampTotalStatus[l][qm][om] = LAMP_OK;
          }
          m_BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
      }
  }

  // Resettig the mode
  m_BackEnd->GotoDefaultMode();

  m_CleanUpRequired        = false;
  m_DelayTime              = 0;
  m_UVLampIndex            = 0;
  m_QualityModeIndex       = 0;
  m_OperationModeIndex     = 0;

  m_BackEnd->OpenUVLampsHistoryFile();
  m_BackEnd->WriteToUVHistoryFile("\n");
  m_BackEnd->WriteToUVHistoryFile("Date");

  TotalPrintingHours = (m_BackEnd->GetMaintenanceCounterElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / SECONDS_IN_HOUR);
  m_BackEnd->WriteToUVHistoryFile("Total Printing Hours: " + QIntToStr(TotalPrintingHours));
}

// End the wizard session event
void CUVCalibrationWizard::EndEvent()
{
  int TotalPrintingHours = (m_BackEnd->GetMaintenanceCounterElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / SECONDS_IN_HOUR);
  m_BackEnd->WriteToUVHistoryFile(WIZARD_COMPLETED_STR);

  if (m_WizardMode == CalibrationMode)
  {
     m_ParamMgr->LastUVCalibration = TotalPrintingHours;
     m_BackEnd->ResetMaintenanceCounter(UV_LAMPS_CALIBRATION_COUNTER_ID);
  }
  CleanUp();
}

// Cancel the wizard session event
void CUVCalibrationWizard::CancelEvent(CWizardPage *WizardPage)
{
  m_BackEnd->WriteToUVHistoryFile(WIZARD_CANCELED_STR);
  CleanUp();
}

void CUVCalibrationWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  // Respond only to 'next' events 
  if (LeaveReason != lrGoNext)
    return;

  switch(WizardPage->GetPageNumber())
  {
    case uvChooseWizardModePage:
      // Updating the Mode according to the user click....
      m_WizardMode    = (TUVWizardMode) dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption();
      m_WizardModeStr = LOAD_STRING((m_WizardMode == PowerTestMode) ? IDS_POWER_TEST : IDS_UVC_WIZARD);

      m_BackEnd->WriteToUVHistoryFile(m_WizardModeStr);
      m_BackEnd->WriteToUVHistoryFile("----------------------");
      break;

    case uvInsertLampValuePage:
    {
      CDataEntryWizardPage *Page = dynamic_cast<CDataEntryWizardPage *>(WizardPage);
      m_LastValue[m_UVLampIndex] = QStrToValue<float>(Page->FieldsValues[0]);
      m_ValuePercentage[m_UVLampIndex] = ((m_LastValue[m_UVLampIndex] / (X_LOOP_CYCLES_NUM * 2)) / NORMAL_UV_POWER) * 100;
      break;
    }

    case uvDisplayLampCapacityPage:
	{

      int     DesiredPercent = m_ParamMgr->UVDesiredPercentageLampValueArray[m_UVLampIndex];
      int     PercentDiff    = DesiredPercent - m_ValuePercentage[m_UVLampIndex];
      QString OutputStr;

      CommentLampStatusToUVHistoryFile(m_UVLampIndex);
      
      // If we're in Power Test Mode - Do nothing...
      if (m_WizardMode == PowerTestMode)
      {
         // if the UV Lamp is out of it's limits ...
         if (abs(PercentDiff) > PERCENTAGE_THRESHOLD)
         {
             m_LampStatus[m_UVLampIndex]       = LAMP_NOT_OK;
             m_AdditionalStatus[m_UVLampIndex] = ((m_ValuePercentage[m_UVLampIndex] > DesiredPercent) ? TOO_STRONG : TOO_WEAK);
         }
      }
      // if we're not within threshold
      else
      {
         if (abs(PercentDiff) > PERCENTAGE_THRESHOLD)
         {
			bool NeedMoreCapacity = (PercentDiff > 0);
			bool ImprovePossible  = GetNextPSValue(NeedMoreCapacity, PercentDiff, m_UVLampIndex);

            // Can't get to the desired value - leave current one.
            if (ImprovePossible == true)
            {
               m_BackEnd->SetUVD2AValue(static_cast<BYTE>(m_UVLampIndex), m_PSValue);
               SetNextPage(uvStabilizationPage);
               m_BackEnd->WriteToUVHistoryFile("Improve needed");
               break;
            }
         }
         else
            // Power Supply reached its range.... Finish with right UV lamp
            m_LampStatus[m_UVLampIndex] = LAMP_OK;

         m_ParamMgr->UVLampPSValueArray[m_UVLampIndex] = m_PSValue;
         m_ParamMgr->SaveSingleParameter(&m_ParamMgr->UVLampPSValueArray);
      }

      CommentLampStatusToStatusPage(m_UVLampIndex);
      m_UVLampIndex++;

      if (!VALIDATE_UV_LAMP(m_UVLampIndex))
      {
         m_UVLampIndex = 0;
         SetNextPage(uvHomeAxis);
      }
      else
      {
         m_PSValue = m_ParamMgr->UVLampPSValueArray[m_UVLampIndex];
         InitIndices();
         // Can't get to the desired value - leave current one.
         SetNextPage(uvResetSensorPage);
         if (m_WizardMode == PowerTestMode)
             break;
         m_BackEnd->EnableMotor(true, AXIS_X);
         m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X,UV_P1[m_UVLampIndex],BLOCKING,muMM);
      }
      break;
    }
  }
}


bool CUVCalibrationWizard::PreparingPageEnter(CWizardPage* WizardPage)
{
  CStatusWizardPage* Page      = dynamic_cast<CStatusWizardPage *>(WizardPage);
  int                ZPosition = m_ParamMgr->Z_StartPrintPosition + 14;
  
  EnableDisableNext(false);

  if (m_BackEnd->EnableDoor(true) != Q_NO_ERROR)
     throw ("Can not lock door");

  m_CleanUpRequired   = true;
  Page->StatusMessage = LOAD_STRING(IDS_HOME_AXIS);
  Page->Refresh();
  EnableAllAxesAndHome();
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  Page->StatusMessage = LOAD_STRING(IDS_MOVING_TRAY_TO_SENSOR_POSITION);
  Page->Refresh();
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Z, ZPosition, BLOCKING, muMM);
  // Unlock door
  if (m_BackEnd->EnableDoor(false) != Q_NO_ERROR)
     throw ("Can not unlock door");
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  GotoNextPage();
  return true;
}

bool CUVCalibrationWizard::LampsOnPageEnter(CWizardPage* WizardPage)
{

  CElapsingTimeWizardPage* Page = dynamic_cast<CElapsingTimeWizardPage *>(WizardPage);
  Page->Title    = m_WizardModeStr;

  m_BackEnd->EnterMode(PRINT_MODE[m_QualityModeIndex][m_OperationModeIndex],
                       MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));

  Page->SubTitle = QFormatStr(LOAD_STRING(IDS_UV_WARMING_DURATION_STR),
                                    (m_ParamMgr->UVLampIgnitionTime + m_ParamMgr->UVCalibrationPostIgnitionTime
                                     									+ (SECONDS_IN_MINUTES - 1) // to get ceiling value
                                    ) / SECONDS_IN_MINUTES);
  Page->Refresh();
  if (m_BackEnd->EnableDoor(true) != Q_NO_ERROR)
     throw ("Can not lock door");

  // Setting PowerSupply params according to the mode
  for(BYTE l = 0; l < NUMBER_OF_UV_LAMPS; l++)
      m_BackEnd->SetUVD2AValue(l, m_ParamMgr->UVLampPSValueArray[l]);

  m_PSValue = m_ParamMgr->UVLampPSValueArray[m_UVLampIndex];
  InitIndices();

  // Turn UV lamps on
  m_BackEnd->SetDefaultParmIgnitionTimeout();
  m_BackEnd->TurnUVLamps(true);

  unsigned EndTime = QGetTicks() + QSecondsToTicks(m_ParamMgr->UVLampIgnitionTime);

  // Wait for UV lamps to turn on
  while (IsCancelled() != true)
  {
    if (QGetTicks() > EndTime)
    {
	  QMonitor.ErrorMessage(TIMEOUT_REASON(IDS_UV_LAMPS_IGNITION),ORIGIN_WIZARD_PAGE);
	  m_BackEnd->WriteToUVHistoryFile(TIMEOUT_REASON(IDS_UV_LAMPS_IGNITION));
      throw (TIMEOUT_REASON(IDS_UV_LAMPS_IGNITION));
    }

    if (m_BackEnd->AreUVLampsTurnedOn())
      break;

    YieldAndSleepWizardThread();
    QSleep(UV_LAMPS_POLL_TIME);
  }

  if (Q2RTWizardSleep(m_ParamMgr->UVCalibrationPostIgnitionTime) == false)
     return false;

  QSleep(UV_LAMPS_POLL_TIME);
  if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  GotoNextPage();
  return true;
}

bool CUVCalibrationWizard::ResetSensorPageEnter(CWizardPage* WizardPage)
{
   //this page is entered several times
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
   Page->SetChecksMask(0);
   Page->Refresh();

   if (m_UVLampIndex == LEFT_UV_LAMP)
   {//fixme
      m_BackEnd->EnableMotor(true, AXIS_X);
      m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X,UV_P1[m_UVLampIndex],BLOCKING,muMM);
   }
  return true;
}

bool CUVCalibrationWizard::LampScanPageEnter(CWizardPage* WizardPage)
{
  CProgressWizardPage* Page  = dynamic_cast<CProgressWizardPage *>(WizardPage);
  Page->Title    = m_WizardModeStr + "-" + GetModeStr(m_QualityModeIndex,m_OperationModeIndex);
  Page->SubTitle = GetUVLampStr((TUVLampIndex)m_UVLampIndex);
  Page->Max      = X_LOOP_CYCLES_NUM;
  Page->Progress = 0;
  Page->Refresh();

  m_BackEnd->EnableMotor(true, AXIS_X);
  m_BackEnd->EnableMotor(true, AXIS_Y);
  
  if (m_UVLampIndex == RIGHT_UV_LAMP)
  {//fixme
      m_BackEnd->GoToMotorHomePosition(AXIS_Y,true);
      m_BackEnd->GoToMotorHomePosition(AXIS_X,true);
  }
  m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, Y_POSITION, BLOCKING, muMM);
  Page->Refresh();
  EnableDisableCancel(true);
  if (XLoop(Page, UV_P1[m_UVLampIndex], UV_P2[m_UVLampIndex], X_LOOP_CYCLES_NUM) != true)
    return false;
  GotoNextPage();
  return true;
}

bool CUVCalibrationWizard::InsertLampValuePageEnter(CWizardPage* WizardPage)
{
  CDataEntryWizardPage* Page = dynamic_cast<CDataEntryWizardPage *>(WizardPage);
  Page->FieldsValues[0]      = "";
  Page->Strings[0]           = GetUVLampStr((TUVLampIndex)m_UVLampIndex) + " Power";
  Page->Refresh();
  return true;
}

bool CUVCalibrationWizard::DisplayLampCapacityPageEnter(CWizardPage* WizardPage)
{
  CStatusWizardPage *Page = dynamic_cast<CStatusWizardPage *>(WizardPage);
  Page->Title             = GetUVLampStr((TUVLampIndex)m_UVLampIndex) + " Current Capacity:";
  Page->StatusMessage     = QFormatStr("%d%% In Level %d (out of %d)",
                                        m_ValuePercentage[m_UVLampIndex],
                                        m_CurPSIndex + 1,
                                        SIZE_OF_UV_POWER_SUPPLY_ARRAY_30);
  Page->Refresh();
  return true;
}

bool CUVCalibrationWizard::StatusPageEnter(CWizardPage* WizardPage)
{
  CUVLampsStatusWizardPage* Page = dynamic_cast<CUVLampsStatusWizardPage *>(WizardPage);
  Page->Title = m_WizardModeStr + " " + LOAD_STRING(IDS_RESULTS);
  for(int l = 0; l < NUMBER_OF_UV_LAMPS; l++)
      FOR_ALL_QUALITY_MODES(qm)
    	FOR_ALL_OPERATION_MODES(om)
          {
              if(GetModeAccessibility(qm, om) == false)
                 continue;
              Page->UVStatusLabel[l][qm][om] = m_UVStatusLabel[l][qm][om];
              Page->LampStatus[l][qm][om]    = m_LampTotalStatus[l][qm][om];
          }
  Page->Refresh();
  return true;
}

bool CUVCalibrationWizard::HomeAxisPageEnter(CWizardPage* WizardPage)
{
  bool breakSwitch = false;
  do
  {
      m_QualityModeIndex++;
      if (!VALIDATE_QUALITY_MODE(m_QualityModeIndex))
      {
         m_QualityModeIndex = 0;
         m_OperationModeIndex++;
         if (!VALIDATE_OPERATION_MODE(m_OperationModeIndex))
         {
            m_OperationModeIndex = 0;
            SetNextPage(uvTurnOffSensor);
            GotoNextPage();
            breakSwitch = true;
         }
     }
  }while(GetModeAccessibility(m_QualityModeIndex, m_OperationModeIndex) == false);

  if (breakSwitch == true)
      return false;
               
  m_BackEnd->GotoDefaultMode();
  m_BackEnd->EnterMode(PRINT_MODE[m_QualityModeIndex][m_OperationModeIndex],
                       MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));

  // Setting PowerSupply params according to the mode
  for(BYTE l = 0; l < NUMBER_OF_UV_LAMPS; l++)
  {
      m_BackEnd->SetUVD2AValue(l, m_ParamMgr->UVLampPSValueArray[l]);
      m_LampStatus[l] = LAMP_OK;

  }
  m_PSValue = m_ParamMgr->UVLampPSValueArray[m_UVLampIndex];
  InitIndices();

  // Homing motors
  m_BackEnd->GoToMotorHomePosition(AXIS_Y,true);
  m_BackEnd->GoToMotorHomePosition(AXIS_X,true);

  GotoNextPage();
  return true;
}

void CUVCalibrationWizard::PageEnter(CWizardPage *WizardPage)
{
  switch(WizardPage->GetPageNumber())
  {
    case uvVerifyCleanTrayPage:
         WizardPage->Title = m_WizardModeStr;
         WizardPage->Refresh();
         if ((m_BackEnd->IsTrayInserted() != true) || (m_BackEnd->CheckIfDoorIsClosed() != Q_NO_ERROR))
            SetNextPage(uvVerifyCleanTrayPage);
         break;

////////////////////////////////////////////////////////////////////////////////
    case uvPreparingPage:
         if (PreparingPageEnter(WizardPage) == false)
            return;
         break;

////////////////////////////////////////////////////////////////////////////////
    case uvLampsOnPage:
         if (LampsOnPageEnter(WizardPage) == false)
            return;
         break;

////////////////////////////////////////////////////////////////////////////////
    case uvResetSensorPage:
         if (ResetSensorPageEnter(WizardPage) == false)
            return;
         break;
////////////////////////////////////////////////////////////////////////////////
    case uvLampScanPage:
         if (LampScanPageEnter(WizardPage) == false)
            return;
         break;

////////////////////////////////////////////////////////////////////////////////
    case uvInsertLampValuePage:
         if (InsertLampValuePageEnter(WizardPage) == false)
            return;
         break;

////////////////////////////////////////////////////////////////////////////////
    case uvDisplayLampCapacityPage:
         if (DisplayLampCapacityPageEnter(WizardPage) == false)
            return;
         break;
         
////////////////////////////////////////////////////////////////////////////////

    case uvStabilizationPage:
    case uvInterModeStabilizationPage:
         if (Q2RTWizardSleep((CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation)) ? STABILIZATION_DELAY : 1) == false)
             return;
         QSleep(UV_LAMPS_POLL_TIME);
         if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
         SetNextPage(uvResetSensorPage);
         GotoNextPage();
         break;
////////////////////////////////////////////////////////////////////////////////

    case uvTurnOffSensor:
         m_BackEnd->TurnUVLamps(false);
         break;
////////////////////////////////////////////////////////////////////////////////

    case uvStatusPage:
         if (StatusPageEnter(WizardPage) == false)
            return;
         break;
         
////////////////////////////////////////////////////////////////////////////////
    case uvHomeAxis:
         if (HomeAxisPageEnter(WizardPage) == false)
            return;
         break;
  }
}

void CUVCalibrationWizard::HelpEvent(CWizardPage *WizardPage)
{	
	switch(WizardPage->GetPageNumber())
	{
		case uvWelcomePage: 		    m_BackEnd->DispatchHelp(10, HELP_FILE_NAME); break;
		case uvChooseWizardModePage:    m_BackEnd->DispatchHelp(15, HELP_FILE_NAME); break;
		case uvPlaceSensorPage: 	    m_BackEnd->DispatchHelp(20, HELP_FILE_NAME); break;
		case uvInsertLampValuePage:	    m_BackEnd->DispatchHelp(27, HELP_FILE_NAME); break;
		case uvDisplayLampCapacityPage:	m_BackEnd->DispatchHelp(29, HELP_FILE_NAME); break;			
		case uvStatusPage:              m_BackEnd->DispatchHelp(30, HELP_FILE_NAME); break;
		
		default: break;		
	}
}

bool CUVCalibrationWizard::XLoop(CProgressWizardPage *WizardPage, int X1, int X2, int Cycles)
{
  for (int i = 1; i <= Cycles; i++)
  {
    WizardPage->Progress = i;
    WizardPage->Refresh();

    m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, X2, BLOCKING, muMM);
    m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, X1, BLOCKING, muMM);

    YieldAndSleepWizardThread();
    if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }
  return true;
}

bool CUVCalibrationWizard::GetNextPSValue(bool NeedMoreCapacity, int PercentDiff, int LampID)
{
  bool*   pLampStatus       = &m_LampStatus[LampID];
  int*    pAdditionalStatus = &m_AdditionalStatus[LampID];
  int     OrigIndex;
  QString OutputStr;

  // If we're already in the minimum value, and still need to lower it - leave it
  if ((m_CurPSIndex == 0) && (NeedMoreCapacity == false))
  {
     *pLampStatus = LAMP_OK;
     return false;
  }

  // If we're already in the maximum value, and still need to lower it produce an error message
  if ((m_CurPSIndex == SIZE_OF_UV_POWER_SUPPLY_ARRAY_30 - 1) && (NeedMoreCapacity == true))
  {
     *pLampStatus       = LAMP_NOT_OK;
     *pAdditionalStatus = TOO_WEAK;
     return false;
  }

  OrigIndex     = m_CurPSIndex;
  m_CurPSIndex += (PercentDiff / PERCENTAGE_PER_STEP);

  if (m_CurPSIndex < 0)
    m_CurPSIndex = 0;

  if (m_CurPSIndex >= SIZE_OF_UV_POWER_SUPPLY_ARRAY_30)
    m_CurPSIndex = SIZE_OF_UV_POWER_SUPPLY_ARRAY_30 - 1;

  m_PSValue   = m_ParamMgr->UVPowerSupplyValuesArray30[m_CurPSIndex];
  m_DelayTime = (abs(OrigIndex - m_CurPSIndex)) * DELAY_PER_STEP;

  return true;
}

void CUVCalibrationWizard::InitIndices()
{
  m_CurPSIndex  = -1;

  for (int i = 0; i < SIZE_OF_UV_POWER_SUPPLY_ARRAY_30; i++)
  {
    if (m_PSValue >= m_ParamMgr->UVPowerSupplyValuesArray30[i])
      m_CurPSIndex  = i;
  }

  // If the Power Supply value is lower than allowed...
  if (m_CurPSIndex == -1)
    throw EQException("UV Calibration wizard Error: Illeagal Power Supply Value");
  return;
}


void CUVCalibrationWizard::CleanUp()
{
  if (m_CleanUpRequired == false)
     return;
  m_BackEnd->WriteToUVHistoryFile("Line");
  m_BackEnd->CloseUVLampsHistoryFile();
  m_BackEnd->GotoDefaultMode();

  // Turn off UV Lamps
  m_BackEnd->TurnUVLamps(false);

  // Home X & Y Axis
  m_BackEnd->GoToMotorHomePosition(AXIS_Y, true);
  m_BackEnd->GoToMotorHomePosition(AXIS_X, true);
  m_BackEnd->EnableMotor(false,AXIS_ALL);

  // Unlock the door
  m_BackEnd->EnableDoor(false);

  FOR_ALL_OPERATION_MODES(om)
     FOR_ALL_QUALITY_MODES(qm)
         if (GetModeAccessibility(qm, om) == true)
            m_BackEnd->DisableConfirmationBypass(PER_MACHINE_MODE[qm][om],
                                              MACHINE_QUALITY_MODES_DIR(qm, om));
}

void CUVCalibrationWizard::CommentLampStatusToUVHistoryFile(int LampID)
{
  QString StatusOutput = QFormatStr("%s ('%s') - %d - %d%% (Desired %d%%), Level %d out of %d",
                                    GetUVLampStr((TUVLampIndex)LampID).c_str(),
                                    GetModeStr(m_QualityModeIndex, m_OperationModeIndex).c_str(),
                                    (int)m_LastValue[LampID],
                                    m_ValuePercentage[LampID],
                                    (int)(m_ParamMgr->UVDesiredPercentageLampValueArray[LampID]),
                                    m_CurPSIndex + 1,
                                    SIZE_OF_UV_POWER_SUPPLY_ARRAY_30);
  m_BackEnd->WriteToUVHistoryFile(StatusOutput);
}

void CUVCalibrationWizard::CommentLampStatusToStatusPage(int LampID)
{
  QString* pStatus        = &(m_UVStatusLabel   [LampID][m_QualityModeIndex][m_OperationModeIndex]);
  bool*    LampTotalRes   = &(m_LampTotalStatus [LampID][m_QualityModeIndex][m_OperationModeIndex]);
  int*     pAddStatus     = &(m_AdditionalStatus[LampID]);
  QString  AddStatusOutput, StatusOutput;

  if (m_LampStatus[LampID] == LAMP_NOT_OK)
  { 
     *LampTotalRes   = LAMP_NOT_OK;
     AddStatusOutput = LOAD_STRING((*pAddStatus == TOO_WEAK) ? IDS_UV_TOO_WEAK : IDS_UV_TOO_STRONG);
     *pStatus        = LOAD_STRING((m_WizardMode == PowerTestMode) ? IDS_HAS_RECALIBRATE : IDS_SHOULD_REPLACE);
     StatusOutput    = QFormatStr("%s ('%s') - Lamp is %s. %s",
                                   GetUVLampStr((TUVLampIndex)LampID).c_str(),
                                   GetModeStr(m_QualityModeIndex, m_OperationModeIndex).c_str(),
                                   AddStatusOutput.c_str(),
                                   pStatus->c_str());
     m_BackEnd->WriteToUVHistoryFile(StatusOutput);
  }
}


