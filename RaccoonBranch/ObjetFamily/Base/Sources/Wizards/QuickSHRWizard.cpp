/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Quick Single head replacement.                           *
 * Module Description: Quick Single head replacement wizard.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Sade.                                                *
 * Start date: 03/11/2003                                           *
 * Last upate: 03/11/2003                                           *
 ********************************************************************/
#include <limits>
#include <math.h>
#include "WizardPages.h"
#include "QuickSHRWizard.h"
#include "AppParams.h"
#include "QTimer.h"
#include "BackEndInterface.h"
#include "MotorDefs.h"
#include "LinearInterpolator.h"
#include "MaintenanceCounters.h"
#include "AppLogFile.h"
#include "QFileWithCheckSum.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "FrontEndControlIDs.h"
#include "FrontEnd.h"
#include "UserValuesEntryPage.h"
#include "HeadReplacementDefs.h"
#include "AgreementPage.h"
#include "HeadsCheckBoxesFrame.h"
#include "Q2RTApplication.h"
#include "WizardImages.h" 
#include "HostMessages.h"

extern QString YesNoAnswerStrings[];

#define HELP_MOVIE_FILE_NAME LOAD_STRING(IDS_QR_HELP_MOVIE_FILE_NAME)
using namespace std;

#define SHR_HELP_FILE_1_NAME LOAD_STRING(IDS_QR_WIZARD_HELP_FILE1)
#define SHR_HELP_FILE_2_NAME LOAD_STRING(IDS_QR_WIZARD_HELP_FILE2)
#define SHR_HELP_FILE_3_NAME LOAD_STRING(IDS_QR_WIZARD_HELP_FILE3)
#define SHR_HELP_FILE_4_NAME LOAD_STRING(IDS_QR_WIZARD_HELP_FILE4)
#define SHR_HELP_FILE_5_NAME LOAD_STRING(IDS_QR_WIZARD_HELP_FILE5)

#define WIZARD_DURATION(_duration_) QFormatStr("This wizard will take approximately##_duration_##minutes.\n%s",LOAD_STRING(IDS_WIZARD_NEXT_OR_CANCEL))

// Launch an help file in an external viewer
void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

const int   THRESHOLD_DELTA                          = 100;
const int   VACUUM_LEAKAGE_TEST_TIME                 = 30; // sec
const int   MAX_OPERATING_VOLTAGE                    = 37;
const int   MIN_A2D_VALUE_FOR_60C                    = 850;
const int   MAX_A2D_VALUE_FOR_60C                    = 1800;
const int   MIN_A2D_VALUE_FOR_80C                    = 450;
const int   MAX_A2D_VALUE_FOR_80C                    = 1100;
const float MAX_10KHZ_GAIN                           = 1.8;
const float MIN_10KHZ_GAIN                           = 0.3;
const float MIN_WEIGHT                               = 0.1;
const float MAX_WEIGHT                               = 5.0;
const float ROUND_QUANTIZATION_FACTOR                = 2.5; // Minutes


enum {qrWelcomePage,                //  0 ( 0 - Eden250)
      qrAgreementPage,              //  1 ( 1 - Eden250)
      qrInsertTray,               //    ( 2 - Eden250)
      qrCloseDoor1,               //  2 ( 3 - Eden250)
      qrLiquidsShortage,          //  3 ( 4 - Eden250)
      qrSelectHeads,              //  4 ( 5 - Eden250)
      // Head replacement
      qrHeadsHeating1,            //  5 ( 6 - Eden250)
      qrEmptyBlock,               //  6 ( 7 - Eden250)
      qrCoolingDown,              //  7 ( 8 - Eden250)
      qrReplacementPosition,      //  8 ( 9 - Eden250)
      qrReplacementWarning,       //  9 (10 - Eden250)
      qrReplaceHead,              // 10 (11 - Eden250)
      qrReInsertHead,             // 11 (12 - Eden250)

      // Installation check
      qrIntallationCompleteQuery, // 12 (13 - Eden250)
      qrRemoveTools,              // 13 (14 - Eden250)
      qrCloseDoor2,               // 14 (15 - Eden250)
      qrCheckInsertion,           // 15 (16 - Eden250)
      qrNoCharacterizationData,   // 16 (17 - Eden250)
      qrHeadsHeating2,            // 17 (18 - Eden250)
      qrFillBlock,                // 18 (19 - Eden250)
      qrVacuumLeakage,            // 19 (20 - Eden250)
      qrPurge,                    // 20 (21 - Eden250)

      // Weight test
      qrLastPhaseMessage,         // 21 (22 - Eden250)
      qrWeightTest,               // 22 (23 - Eden250)
      qrEnterWeights,             // 23 (24 - Eden250)
      qrAdjustPotentiometer,      // 24 (25 - Eden250)
      qrCalibratingFor,           // 25 (26 - Eden250)

      qrResumeWizard,             // 27 (28 - Eden250)
      qrWizardCompleted,          // 26 (27 - Eden250)
};

CQuickSHRWizard::CQuickSHRWizard(void) : CQ2RTSHRWizardBase(IDS_QR_WIZARD)
{
  SetIndexes(qrVacuumLeakage, qrPurge);

  CMessageWizardPage *WelcomePage = new CMessageWizardPage(GetTitle(),HSW_HEADS_OUT_PICTURE_ID);
  WelcomePage->SubTitle = WIZARD_DURATION(75);
  AddPage(WelcomePage);

  CAgreementWizardPage *AgreementPage = new CAgreementWizardPage(LOAD_STRING(IDS_WIZARD_CONDITIONS),HSW_HEADS_OUT_PICTURE_ID);
  AddPage(AgreementPage);

  CInsertTrayPage *InsertTrayPage = new CInsertTrayPage(this, PREPARATIONS_IMAGE_ID);
  AddPage(InsertTrayPage);

  CCloseDoorPage *CloseDoorPage = new CCloseDoorPage(this, PREPARATIONS_IMAGE_ID);
  AddPage(CloseDoorPage);

  CCheckBoxWizardPage *LiquidShortPage = new CLiquidsShortagePage(this,-1,wpNextWhenSelected);
  AddPage(LiquidShortPage);

  CHeadsCheckboxesWizardPage *SelectHeadsPage = new CHeadsCheckboxesWizardPage(LOAD_STRING(IDS_SELECT_HEADS),HEAD_ORDER_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  SelectHeadsPage->SubTitle = "";
  SelectHeadsPage->CheckedArray = m_HeadsToBeReplaced;
  AddPage(SelectHeadsPage);

  // Head replacement
  CElapsingTimeWizardPage* HeadsHeating1Page = new CElapsingTimeWizardPage(LOAD_STRING(IDS_HEADS_HEATING),-1,wpNextDisabled | wpPreviousDisabled);
  HeadsHeating1Page->SubTitle = PROCESS_DURATION(IDN_HEADS_HEATING);
  AddPage(HeadsHeating1Page);

  CProgressStatusWizardPage *EmptyBlock1Page = new CProgressStatusWizardPage("Emptying block...",HEADS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  EmptyBlock1Page->DefaultMode = CProgressStatusWizardPage::psmProgress;
  AddPage(EmptyBlock1Page);

  CElapsingTimeWizardPage* CoolingDown1Page = new CElapsingTimeWizardPage(LOAD_STRING(IDS_COOLING_DOWN),IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  CoolingDown1Page->SubTitle = PROCESS_DURATION(IDN_COOLING_DOWN);
  AddPage(CoolingDown1Page);

  CStatusWizardPage *ReplacementPositionPage = new CStatusWizardPage("",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  ReplacementPositionPage->StatusMessage = "Moving to replacement position";
  AddPage(ReplacementPositionPage);

  CMessageWizardPage *ReplacementWarningPage = new CMessageWizardPage("CAUTION!!!",HSW_HEADS_OUT_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled);
  ReplacementWarningPage->SubTitle = LOAD_STRING(IDS_IMMEDIATE_REPLACEMENT);
  AddPage(ReplacementWarningPage);

  CCheckBoxWizardPage *ReplaceHeadPage = new CCheckBoxWizardPage("Head(s) replacement",HEAD_ORDER_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled | wpFlipButton2Visible);
  ReplaceHeadPage->SubTitle = "Please refer to the instructions included in the head replacement kit";
  AddPage(ReplaceHeadPage);

  CCheckBoxWizardPage *ReInsertHeadPage = new CCheckBoxWizardPage("Reinsert the heads",HSW_HEADS_OUT_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled | wpFlipButton2Visible);
  ReInsertHeadPage->SubTitle = "Head insertion test failed. The following heads should be removed and re-inserted";
  AddPage(ReInsertHeadPage);

  // Installation check
  CCheckBoxWizardPage *InstallationCheckPage = new CCheckBoxWizardPage("Head(s) installation checked?",LEVEL_HEADS_IMAGE_ID,wpNextWhenSelected);
  InstallationCheckPage->Strings.Add(LOAD_STRING(IDS_DONE));
  AddPage(InstallationCheckPage);

  CVerifyCleanTrayPage *VerifyCleanTrayPage = new CVerifyCleanTrayPage(this,PREPARATIONS_IMAGE_ID, wpPreviousDisabled);
  AddPage(VerifyCleanTrayPage);

  CMessageWizardPage *CloseDoor2Page = new CMessageWizardPage("Close door",-1,wpNextDisabled | wpPreviousDisabled);
  AddPage(CloseDoor2Page);

  CCheckBoxWizardPage *CheckInsertionPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_NOT_INSERTED_CORRECTLY),HSW_HEADS_IN_PICTURE_ID,wpNextWhenSelected | wpPreviousDisabled);
  CheckInsertionPage->SubTitle = LOAD_STRING(IDS_REINSERT_HEAD_DRIVER);
  CheckInsertionPage->Strings.Add(LOAD_STRING(IDS_DONE));
  AddPage(CheckInsertionPage);

  CCheckBoxWizardPage *NoCharacterizationDataPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_NO_HEAD_FACTORY_DATA),IN_PROCESS_IMAGE_ID,wpNextWhenSelected | wpPreviousDisabled);
  NoCharacterizationDataPage->SubTitle = LOAD_STRING(IDS_INSERT_OTHER_HEAD);
  NoCharacterizationDataPage->Strings.Add(LOAD_STRING(IDS_OTHER_HEAD_INSERTED));
  AddPage(NoCharacterizationDataPage);

  CElapsingTimeWizardPage* HeadsHeating2Page = new CElapsingTimeWizardPage(LOAD_STRING(IDS_HEADS_HEATING),-1,wpNextDisabled | wpPreviousDisabled);
  HeadsHeating2Page->SubTitle = PROCESS_DURATION(IDN_HEADS_HEATING);
  AddPage(HeadsHeating2Page);

  CStatusWizardPage *FillBlockPage = new CStatusWizardPage("",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  FillBlockPage->StatusMessage = LOAD_STRING(IDS_HEAD_FILLING) + (QString)"(~5 min.)";
  AddPage(FillBlockPage);

  CMessageWizardPage *VacuumLeakagePage = new CMessageWizardPage(LOAD_QSTRING(IDS_VACUUM_LEAKAGE_DETECTED),VACUUM_TEST_IMAGE_ID,wpPreviousDisabled);
  VacuumLeakagePage->SubTitle = LOAD_STRING(IDS_WRONG_HEAD_INSERTION);
  AddPage(VacuumLeakagePage);

  CStatusWizardPage *PurgePage = new CStatusWizardPage("",IN_PROCESS_IMAGE_ID,wpNextDisabled | wpPreviousDisabled);
  PurgePage->StatusMessage = LOAD_STRING(IDS_HEAD_FILLING) + (QString)"(~5 min.)";
  AddPage(PurgePage);

  CMessageWizardPage *LastPhasePage = new CMessageWizardPage(LOAD_STRING(IDS_PREPARING_FOR_WT), IN_PROCESS_IMAGE_ID,wpPreviousDisabled);
  LastPhasePage->SubTitle = LOAD_STRING(IDS_NO_INTERVENTION);
  AddPage(LastPhasePage);

  CStatusWizardPage *WeightTestPage = new CStatusWizardPage("",HSW_WEIGHT_TEST_PICTURE_ID,wpNextDisabled | wpPreviousDisabled);
  WeightTestPage->StatusMessage = LOAD_STRING(IDS_PRINTING);
  AddPage(WeightTestPage);

  CUserValuesEntryWizardPage *EnterWeightsPage = new CUserValuesEntryWizardPage("",HSW_WEIGHT_TEST_PICTURE_ID,wpPreviousDisabled);
  EnterWeightsPage->SetUserEntryDescriptions(GetHeadName);
  EnterWeightsPage->ValuesType = evFloat;
  EnterWeightsPage->ClearUserEntryValues();
  EnterWeightsPage->ClearUserEntryColors();
  AddPage(EnterWeightsPage);

  CStatusWizardPage *AdjustPotentiometerPage = new CStatusWizardPage(LOAD_STRING(IDS_ADJUST_POTENTIONETER_VALUES),-1,wpNextDisabled | wpPreviousDisabled  | wpCancelDisabled);
  AdjustPotentiometerPage->StatusMessage = LOAD_STRING(IDS_ACTION_TAKES_TIME);
  AddPage(AdjustPotentiometerPage);

  CMessageWizardPage *CalibratingDPIPage = new CMessageWizardPage("",-1,wpPreviousDisabled);
  AddPage(CalibratingDPIPage);

  CRadioGroupWizardPage *WizardCompletedPage = new CRadioGroupWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID, wpDonePage | wpPreviousDisabled);
  for(int i = 0; i < NUMBER_OF_ANSWERS; i++)
      WizardCompletedPage->Strings.Add(YesNoAnswerStrings[i]);
  WizardCompletedPage->DefaultOption = ANSWER_YES;
  WizardCompletedPage->SubTitle = QFormatStr(LOAD_STRING(IDS_RUN_ANOTHER_WIZARD_NOW),WIZARD_TITLE(IDS_XO_WIZARD));
  AddPage(WizardCompletedPage);

  CMessageWizardPage *ResumeWizardPage = new CMessageWizardPage(LOAD_STRING(IDS_RESUME_WIZARD),-1, wpPreviousDisabled );
  ResumeWizardPage->SubTitle = LOAD_STRING(IDS_PRESS_NEXT_TO_CONTINUE);
  AddPage(ResumeWizardPage);

  CMessageWizardPage *WizardCanceledPage = new CMessageWizardPage(WIZARD_CANCELED_STR,CANCEL_PAGE_IMAGE_ID, wpPreviousDisabled | wpCancelDisabled | wpDonePage);
  AddPage(WizardCanceledPage);

};

// Start the wizard session event
void CQuickSHRWizard::StartEvent()
{
  m_LastStateParam = &m_ParamMgr->QR_LastState;
  m_RunHeadAlignmentWizard = false;
  m_ResetValuesNeeded      = true;
  m_VacuumLeakage          = false;

  for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
  {
       m_HeadsToBeReplaced  [i] = false;
       m_HeadsInsertedWrong [i] = false;
       m_EnteredWeightValues[i] = 0.0;
  }

  m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,true);

  FOR_ALL_QUALITY_MODES(qm)
    	FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om))      
              m_BackEnd->EnableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  // Get the current heads voltages and save a backup of them
  for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
  {
	if(m_ParamMgr->IsModelHead(i))
		m_PrevHeadsVoltages[i]           = m_ParamMgr->RequestedHeadVoltagesModel[i];
	else
		m_PrevHeadsVoltages[i]         = m_ParamMgr->RequestedHeadVoltagesSupport[i];
		
    m_PrevHeadPotentiometerValues[i] = m_ParamMgr->PotentiometerValues[i];
  }
  
  // Disable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(false);

  m_QualityModeIndex   = 0;
  m_OperationModeIndex = 0;
  m_BackEnd->GotoDefaultMode();
  m_BackEnd->EnterMode(PRINT_MODE[m_QualityModeIndex][m_OperationModeIndex],
                       MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));
}


// End the wizard session event
void CQuickSHRWizard::EndEvent()
{
  CleanUp();
  m_ParamMgr->QR_LastState = qrWelcomePage;
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->QR_LastState);
}


// Cancel the wizard session event
void CQuickSHRWizard::CancelEvent(CWizardPage *WizardPage)
{
  TurnPowerOn();

  TMachineState State = m_BackEnd->GetCurrentMachineState();
  if(State == msPrinting || State == msPrePrint)
  {
	m_BackEnd->StopPrint(Q2RT_EMBEDDED_RESPONSE);
    do
    {
      State = m_BackEnd->GetCurrentMachineState();
      YieldAndSleepWizardThread();
    }while( State != msStandby1 && State != msStandby2 && State != msStopped  && State != msIdle );
  }

  if (WizardPage->GetPageNumber() == qrEmptyBlock)
  {
     m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE,false);
     m_BackEnd->WipeHeads(false);
     YieldWizardThread();
     m_BackEnd->WipeHeads(false);
  }

  for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
  {
	m_ParamMgr->PotentiometerValues[i]   = m_PrevHeadPotentiometerValues[i];
	if(m_ParamMgr->IsModelHead(i))
		m_ParamMgr->RequestedHeadVoltagesModel[i] = m_PrevHeadsVoltages[i];
	else
		m_ParamMgr->RequestedHeadVoltagesSupport[i] = m_PrevHeadsVoltages[i];
  }

  CleanUp();
}


void CQuickSHRWizard::HelpEvent(CWizardPage *WizardPage)
{
  QString FileName = Q2RTApplication->AppFilePath.Value();
  switch(WizardPage->GetPageNumber())
  {
    case qrWelcomePage:
    case qrAgreementPage:
//runtime objet    
//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
    case qrInsertTray:
//#endif
    case qrCloseDoor1:
    case qrLiquidsShortage:
    case qrResumeWizard:
    case qrSelectHeads:
    case qrHeadsHeating1:
    case qrEmptyBlock:
      FileName += SHR_HELP_FILE_2_NAME;
      break;

    case qrCoolingDown:
    case qrReplacementPosition:
    case qrReplacementWarning:
    case qrReplaceHead:
      FileName += SHR_HELP_FILE_3_NAME;
      break;

    case qrReInsertHead:
    case qrIntallationCompleteQuery:
    case qrRemoveTools:
    case qrCloseDoor2:
    case qrCheckInsertion:
    case qrNoCharacterizationData:
    case qrHeadsHeating2:
    case qrFillBlock:
    case qrVacuumLeakage:
    case qrPurge:
      FileName += SHR_HELP_FILE_4_NAME;
      break;

    case qrLastPhaseMessage:
    case qrWeightTest:
    case qrEnterWeights:
    case qrWizardCompleted:
    case qrAdjustPotentiometer:
    case qrCalibratingFor:
      FileName += SHR_HELP_FILE_5_NAME;
      break;

  }
  ExternalAppExecute(FileName.c_str(),"");
}

void CQuickSHRWizard::UserButton2Event(CWizardPage *WizardPage)
{
  switch(WizardPage->GetPageNumber())
  {
    case qrReplaceHead:
    case qrReInsertHead:
    {
      QString FileName = Q2RTApplication->AppFilePath.Value() + HELP_MOVIE_FILE_NAME;
      ExternalAppExecute(FileName.c_str(),"");
      break;
    }
  }
}

bool CQuickSHRWizard::CheckRequiredConditions()
{
  CAppParams *ParamsMgr = CAppParams::Instance();
//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
   if(ParamsMgr->OfficeProfessionalType==OFFICE_MACHINE) //runtime objet
   {
        if (!m_BackEnd->IsTrayInserted())
        {
        SetNextPage(qrInsertTray);
        return false;
        }
   }
//#endif
   if (m_BackEnd->CheckIfDoorIsClosed() != Q_NO_ERROR)
   {
      SetNextPage(qrCloseDoor1);
      return false;
   }
   return true;
}

void CQuickSHRWizard::EnterWeightsPageLeave(CWizardPage* WizardPage)
{
   CUserValuesEntryWizardPage* Page   = dynamic_cast<CUserValuesEntryWizardPage *>(WizardPage);
   float                       MaterialFactor;
   EnableDisableNext(false);

   Page->ClearUserEntryColors();

   // Check the validity of the weight
   for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
   {
      m_EnteredWeightValues[i] = QStrToValue<float>(Page->Data[i]);
      if (m_EnteredWeightValues[i] < MIN_WEIGHT || m_EnteredWeightValues[i] > MAX_WEIGHT)
      {
         Page->LabelsColor[i] = INT_FROM_RESOURCE(IDN_ERROR_COLOR);
         QMonitor.NotificationMessage(QFormatStr(
            "The weights entered are not valid. Should be between %d(gr) and %d(gr).",
                                              (int)MIN_WEIGHT,(int)MAX_WEIGHT),ORIGIN_WIZARD_PAGE);
         SetNextPage(qrEnterWeights);
         return;
	  }
	  MaterialFactor   = GET_MATERIAL_FACTOR(i,m_OperationModeIndex);
	  m_EnteredWeightValues[i] *= MaterialFactor;
   }
}

void CQuickSHRWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
   int PageIndex = WizardPage->GetPageNumber();
   if ((PageIndex != qrResumeWizard) && (LeaveReason != lrGoNext))
      return;
      
   switch(PageIndex)
   { 
       case qrAgreementPage:
            m_QualityModeIndex   = 0;
            m_OperationModeIndex = 0;
            EnableDisableNext(false);
            m_BackEnd->TurnUVLamps(false);
            m_BackEnd->TurnTrayHeater(false);
////runtime objet
//#if defined EDEN_260 || defined EDEN_250 || defined EDEN_260_V || defined CONNEX_260 || defined OBJET_260
       case qrInsertTray:
//#endif       
       case qrCloseDoor1:
            if (CheckRequiredConditions())
               EnableAllAxesAndHome();
            break;

       case qrCheckInsertion:
            SetNextPage(qrCloseDoor2);
            break;

       case qrNoCharacterizationData:
            SetNextPage(qrRemoveTools);
            break;

       case qrEnterWeights:
            EnterWeightsPageLeave(WizardPage);
            break;

       case qrResumeWizard:
            switch (m_ParamMgr->QR_LastState)
            {
                case qrReplaceHead:
                case qrReInsertHead:
                case qrIntallationCompleteQuery:
                case qrRemoveTools:
                case qrCloseDoor2:
                case qrCheckInsertion:
                case qrNoCharacterizationData:
                     break;
                default:
                     EnableDisableNext(false);
                     EnableAllAxesAndHome();
                     EnableDisableNext(true);
                     break;
            }
            break;

        case qrWizardCompleted:
		{
			m_BackEnd->SetOnlineOffline(false);
             // Mark whether to run 'Head Alignment Wizard'
             m_RunHeadAlignmentWizard =
             	(ANSWER_YES == dynamic_cast<CRadioGroupWizardPage *>(WizardPage)->GetSelectedOption());
             break;
        }
        case qrReplaceHead:
             ReplaceHeadPageLeave(WizardPage);

  }
}


    // Saving previous heads parameters
            // Read the heads paramters after the replacement




void CQuickSHRWizard::CloseDoor2PageEnter(CWizardPage* WizardPage)
{
   // Initiating the error description
   m_NoCharacterizationDescription = "";

   TurnPowerOn();

   m_BackEnd->LockDoor(true);
   while(!m_BackEnd->IsDoorLocked())
   {
      YieldAndSleepWizardThread();
      if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   }

   // Read E2PROMs and head thermistor in order to check that the heads are inserted correctly
   BYTE Data = E2PROM_TEST_DATA;
   bool TestResult = true;
   
   for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
   {
      // Write the date to the SHR History file
      WriteToHistoryFile("Date");
      
      // Check only heads which are in the HeatersMask Table
      if (m_ParamMgr->HeatersMaskTable[i] == true)
      {
         // Write to a certain data to a certain address, read and compare
         m_BackEnd->WriteToE2PROM(i, E2PROM_TEST_ADDRESS, E2PROM_TEST_DATA);
         if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
            Data = m_BackEnd->ReadFromE2PROM(i, E2PROM_TEST_ADDRESS);

         if(Data != E2PROM_TEST_DATA)
         {
            TestResult = false;
            m_HeadsInsertedWrong[i] = true;
         }
      }
   }
   
  if (TestResult)
     TestResult = m_BackEnd->AreHeadHeatersThermistorsValid(false);
     
  if (TestResult)
  {
     for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
     {
         if (m_HeadsToBeReplaced[i])
         {
            // Read the heads paramters after the replacement
            if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
            {
                m_BackEnd->ReadHeadParametersFromE2PROM(i,&m_CurrHeadsParams[i]);
                // Check if the head contains the chracterization data
                if (!m_CurrHeadsParams[i].Validity)
                {
                    TestResult = false;
                    m_NoCharacterizationDescription += "\n" + GetHeadName(i) + ": checksum error";
                    continue;
                }
            }

            float  Gain = ((IS_SUPPORT_HEAD(i)) ?
                           m_CurrHeadsParams[i].Support10KHzLineGain :
                           m_CurrHeadsParams[i].Model10KHzLineGain);

            // Check that the chracterization data is valid
            if (((m_CurrHeadsParams[i].A2DValueFor60C < MIN_A2D_VALUE_FOR_60C) ||
                 (m_CurrHeadsParams[i].A2DValueFor60C > MAX_A2D_VALUE_FOR_60C) ||
                 (m_CurrHeadsParams[i].A2DValueFor80C < MIN_A2D_VALUE_FOR_80C) ||
                 (m_CurrHeadsParams[i].A2DValueFor80C > MAX_A2D_VALUE_FOR_80C) ||
                 (Gain < MIN_10KHZ_GAIN) || (Gain > MAX_10KHZ_GAIN))           &&
                (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation)))
            {
              TestResult = false;
              m_NoCharacterizationDescription +=  "\n" + GetHeadName(i) + ": invalid head data";
              continue;
            }
            else
            {
              QString HeadReplacementLog = QFormatStr("Head %s was replaced: old head's S/N: 0x%x, new head's S/N: 0x%x",
                                                      GetHeadName(i).c_str(),m_PreviousHeadsParams[i].SerialNumber,m_CurrHeadsParams[i].SerialNumber);
              // Write the head S/N to the log file
              WriteToLogFile(LOG_TAG_GENERAL,HeadReplacementLog);
              WriteToHistoryFile(HeadReplacementLog);
            }
         }
     }
     if (TestResult)
     {
          // Everything is ok - continue wizard
          WriteToHistoryFile("\n");
          GotoPage(qrHeadsHeating2);
          return;
     }
     else
     {
          // Some error found - report it....
          GotoPage(qrNoCharacterizationData);
          return;
     }
  }
  else
  {
     // Turn the power off and open the door, for insertion check
     m_BackEnd->EnableDoor(false);
        TurnPowerOff();
     GotoNextPage();
  }
}

bool CQuickSHRWizard::CalibratingForPageEnter(CWizardPage* WizardPage)
{
  m_ResetValuesNeeded      = true;
  SaveLastState();
  do
  {
     m_OperationModeIndex++;
     if (!VALIDATE_OPERATION_MODE(m_OperationModeIndex))
     {
        m_OperationModeIndex = 0;
        m_QualityModeIndex++;
        if (!VALIDATE_QUALITY_MODE(m_QualityModeIndex))
        {
           m_QualityModeIndex = 0;
           GotoPage(qrWizardCompleted);
           return false;
        }
     }
  }while(GetModeAccessibility(m_QualityModeIndex, m_OperationModeIndex) == false);
  SetNextPage(qrPurge);
  WizardPage->Title = "Calibrating for " + GetModeStr(m_QualityModeIndex, m_OperationModeIndex);
  WizardPage->Refresh();
  return true;
}

bool CQuickSHRWizard::WeightTestPageEnter(CWizardPage* WizardPage)
{
   CStatusWizardPage* Page             = dynamic_cast<CStatusWizardPage *>(WizardPage);
   bool                     IsPrintCompleted = false;

   Page->Title = LOAD_STRING(IDS_QR_WEIGHT_TEST) + GetModeStr(m_QualityModeIndex, m_OperationModeIndex);
   Page->Refresh();

   Page->SubTitle = LOAD_STRING(IDS_HEADS_HEATING);
   Page->Refresh();

   HeadsHeatingCycle();
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

   Page->SubTitle = LOAD_STRING(IDS_PRINTING);
   Page->Refresh();

   IsPrintCompleted = DoWeightTest(Page);
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

   if (IsPrintCompleted == false)
      throw EQException(LOAD_STRING(IDS_HR_PRINTING_ERROR));

  GotoNextPage();
  return true;
}

void CQuickSHRWizard::EnterWeightsPageEnter(CWizardPage* WizardPage)
{
   CUserValuesEntryWizardPage *Page = dynamic_cast<CUserValuesEntryWizardPage *>(WizardPage);
   if(m_ResetValuesNeeded)
   {
       // Resetting previous values in the Data Entry boxes
       Page->ClearUserEntryValues();
       m_ResetValuesNeeded = false;  
   }

   Page->Title    = QFormatStr("%s %s", LOAD_STRING(IDS_QR_WEIGHT_TEST), GetModeStr(m_QualityModeIndex, m_OperationModeIndex).c_str());
   Page->SubTitle = QFormatStr(LOAD_STRING(IDS_HR_ENTER_WEIGHTS), QFloatToStr(m_ParamMgr->QR_DefaultHeadWeight).c_str()).c_str();
   Page->Refresh();

   SaveWizardDataToTemporaryFile();
}

void CQuickSHRWizard::AdjustPotentiometerPageEnter(CWizardPage* WizardPage)
{
   float   Weight;
   float   a, b, v;
   float   LayerThicknesses[TOTAL_NUMBER_OF_HEADS_HEATERS];
   QString PrintingStr;
   float   NetLayerThickness;

   m_BackEnd->GotoDefaultMode();
   m_BackEnd->EnterMode(PRINT_MODE[m_QualityModeIndex][m_OperationModeIndex],
                        MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));

   float   ProductLT_m = m_ParamMgr->ProductLT_m;
   float   ProductLT_n = m_ParamMgr->ProductLT_n;

   float   Factor    = CM_PER_INCH / DPI[m_QualityModeIndex];
   float   BMPWidth  = INT_FROM_RESOURCE_ARRAY(m_QualityModeIndex,WEIGHT_TEST_X_NUM_OF_PIXELS) * Factor;
   float   BMPHeight = INT_FROM_RESOURCE_ARRAY(m_QualityModeIndex,WEIGHT_TEST_Y_NUM_OF_PIXELS) * Factor;
   QString ModeStr   = GetModeStr(m_QualityModeIndex, m_OperationModeIndex);
   EnableDisableNext(false);



   for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
   {
      Weight = m_EnteredWeightValues[i];
      LayerThicknesses[i] = (Weight * 10000) /
                            (BMPWidth * BMPHeight * m_TotalNumberOfSlices * FLOAT_FROM_RESOURCE(IDN_SPECIFIC_GRAVITY));

      float* LinesDataArray  = ((IS_MODEL_HEAD(i)) ?
                                &(m_CurrHeadsParams[i].Model20KHzLineGain) :
                                &(m_CurrHeadsParams[i].Support20KHzLineGain));
      LinesDataArray -= m_QualityModeIndex*2;
      a = *LinesDataArray; //Gain
      LinesDataArray++;
      b = *LinesDataArray; //Offset

       // If the layer thickness of the replaced heads is different than expected,
      // calculate b and the required voltage
      if (m_HeadsToBeReplaced[i])
      {
		 NetLayerThickness      = m_ParamMgr->GetLayerHeight_um(DPI[m_QualityModeIndex]);
		 m_TargetLayerThickness = (NetLayerThickness + ProductLT_m) * ProductLT_n;
		 bool isModel =m_ParamMgr->IsModelHead(i);
		 float CalcLayerThickness =0;
		 if(isModel)
			CalcLayerThickness = m_ParamMgr->RequestedHeadVoltagesModel[i] * a + b;
		 else
			CalcLayerThickness = m_ParamMgr->RequestedHeadVoltagesSupport[i] * a + b;
			
		 if (LayerThicknesses[i] != CalcLayerThickness)
		 {
			if(isModel)
				b = LayerThicknesses[i] - m_ParamMgr->RequestedHeadVoltagesModel[i] * a;
			else
				b = LayerThicknesses[i] - m_ParamMgr->RequestedHeadVoltagesSupport[i] * a;
            *LinesDataArray = b;
         }
         try
         {
             v = (m_TargetLayerThickness - b) / a;
			 m_BackEnd->SetAndMeasureHeadVoltage(i, v);
			 if(isModel)
				m_ParamMgr->RequestedHeadVoltagesModel[i] = v;
			 else
				m_ParamMgr->RequestedHeadVoltagesSupport[i] = v;
				
             m_ParamMgr->PotentiometerValues[i] = m_BackEnd->GetCurrHeadPotentiometerValue(i);
         }
         catch(EQException& err)
         {
            CUserValuesEntryWizardPage* Page   = dynamic_cast<CUserValuesEntryWizardPage *>(Pages[qrEnterWeights]);
            if(QMonitor.AskYesNo("Target Voltage out of range in head. Would you like to correct the entered values?"))
            {
               Page->ClearUserEntryColors();
               Page->LabelsColor[i] = INT_FROM_RESOURCE(IDN_ERROR_COLOR);
               GotoPage(qrEnterWeights);
            }
            else
              throw EQException("Target Voltage out of range in head" + GetHeadName(i) + ". \nCall Service for help.");
            QString Msg = "Target Voltage out of range in head " + GetHeadName(i);
            if (m_CancelReason == "")
               Msg += ". Going back to correct the values";
            else
               Msg += ". Cancelling the wizard";
            WriteToLogFile(LOG_TAG_GENERAL,Msg);
            return;
         }
         // Write the new head voltage to the log file
         WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s voltage was changed to: %2f", GetHeadName(i).c_str(), v));
         WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s layer thickness vs. voltage line equation: a = %2f, b = %2f",GetHeadName(i).c_str(), a , b));
      }
   }
   // Write the weights to the log file
   QString WeightsLog = "The user has entered the following weights(multiplied by material factor): ";
   for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
      WeightsLog += (QFloatToStr(m_EnteredWeightValues[i]) + " ");
   WriteToLogFile(LOG_TAG_GENERAL,WeightsLog);
   PrintingStr = ModeStr + ": Printing " + QIntToStr(m_TotalNumberOfSlices) + " slices";
   WriteToLogFile(LOG_TAG_GENERAL, PrintingStr);

   m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);
   m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
   m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
   EnableDisableNext(false);

   // Finished calibrating for 1200 & 600 DPI...
   if (!VALIDATE_QUALITY_MODE(m_QualityModeIndex))
      SetNextPage(qrWizardCompleted);

   GotoNextPage();
}

void CQuickSHRWizard::PageEnter(CWizardPage *WizardPage)
{
  int PageIndex = WizardPage->GetPageNumber();
  m_CleanUpRequired = (qrAgreementPage < PageIndex );
  switch(PageIndex)
  {
    case qrHeadsHeating1:
         SaveLastState();

         // Saving previous heads parameters
         if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
            for (int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
               // Read the heads paramters after the replacement
               m_BackEnd->ReadHeadParametersFromE2PROM(i,&m_PreviousHeadsParams[i]);
         HeadsHeating1PageEnter(WizardPage);
         GotoNextPage();
         break;

    case qrEmptyBlock:
         HeadsDrainingCycle(WizardPage,m_ParamMgr->QR_EmptyBlockTime);
         /* fixme: Do I need following 3 lines? */
         m_BackEnd->TurnHeadFilling(false);
         m_BackEnd->TurnTrayHeater(false);
         GotoNextPage();
         break;

    case qrCoolingDown:
         if (CoolingDownPageEnter(WizardPage) == false)
            return;
         GotoNextPage();
         break;

    case qrReplacementPosition:
         SaveLastState();
         ReplacementPositionPageEnter(WizardPage, qrReInsertHead);
         m_BackEnd->EnableDoor(false);
         TurnPowerOff();
         GotoNextPage();
         break;

    case qrReplaceHead:
         ReplaceHeadPageEnter(WizardPage, qrIntallationCompleteQuery);
         break;

    case qrReInsertHead:
         SaveLastState();
         ReInsertHeadPageEnter(WizardPage);
         break;

    case qrCloseDoor2:
         CloseDoor2PageEnter(WizardPage);
         break;

    case qrCheckInsertion:
         CheckInsertionPageEnter(WizardPage);
         break;

    case qrNoCharacterizationData:
         NoCharacterizationDataPageEnter(WizardPage);
         break;

    case qrHeadsHeating2:
         SaveLastState();
         HeadsHeating2PageEnter(WizardPage);
         break;

    case qrFillBlock:
         if (FillBlockPageEnter(WizardPage) == false)
            return;
         break;


    case qrPurge:
         if (PurgePageEnter(WizardPage, m_QualityModeIndex, m_OperationModeIndex) == false)
            return;
         GotoNextPage();
         break;

    case qrIntallationCompleteQuery:
    case qrLastPhaseMessage:
	case qrWelcomePage:
         SaveLastState();
         break;
         
    case qrCalibratingFor:
         if (CalibratingForPageEnter(WizardPage) == false)
            return;
         SetNextPage(qrWizardCompleted);   
         break;
         
    case qrVacuumLeakage:
         SaveWizardDataToTemporaryFile();
		 m_BackEnd->TurnHeadFilling(false);
         SetNextPage(qrHeadsHeating1);
         m_VacuumLeakage = true;
         break;

    case qrWeightTest:
         if (WeightTestPageEnter(WizardPage) == false)
            return;
         break;

    case qrEnterWeights:
         EnterWeightsPageEnter(WizardPage);
         break;

    case qrAdjustPotentiometer:
         AdjustPotentiometerPageEnter(WizardPage);
         break;

    case qrResumeWizard:
         ResumeWizardPageEnter(WizardPage);
         SetNextPage(m_ParamMgr->QR_LastState);
         break;
  }
}

void CQuickSHRWizard::ResumeWizardSpecificData()
{
   m_TempFile->Read((BYTE*)m_HeadsToBeReplaced,sizeof(m_HeadsToBeReplaced));
   m_TempFile->Read((BYTE*)m_CurrHeadsParams,sizeof(m_CurrHeadsParams));
   m_TempFile->Read((BYTE*)m_PreviousHeadsParams,sizeof(m_PreviousHeadsParams));
   m_TempFile->Read((BYTE*)&m_TargetLayerThickness,sizeof(m_TargetLayerThickness));
   m_TempFile->Read((BYTE*)&m_TotalNumberOfSlices,sizeof(m_TotalNumberOfSlices));
   m_TempFile->Read((BYTE*)&m_QualityModeIndex,sizeof(m_QualityModeIndex));
   m_TempFile->Read((BYTE*)&m_OperationModeIndex,sizeof(m_OperationModeIndex));
}//ResumeWizardPageEnter

bool CQuickSHRWizard::SetHeadsTemperature()
{
  if (!CQ2RTSHRWizardBase::SetHeadsTemperature())
	  return false;

  for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
  {
	bool isModel = m_ParamMgr->IsModelHead(i);
	
	 if (m_HeadsToBeReplaced[i])
	 {
		float v = (isModel ? m_ParamMgr->QR_ModelPrimaryVoltage : m_ParamMgr->QR_SupportPrimaryVoltage);

        try
        {
		   m_BackEnd->SetAndMeasureHeadVoltage(i, v);
		   if(isModel)
			m_ParamMgr->RequestedHeadVoltagesModel[i] = v;
		   else
			m_ParamMgr->RequestedHeadVoltagesSupport[i] = v;
			
           m_ParamMgr->PotentiometerValues[i] = m_BackEnd->GetCurrHeadPotentiometerValue(i);
        }
        catch(EQException& err)
        {
           CUserValuesEntryWizardPage* Page   = dynamic_cast<CUserValuesEntryWizardPage *>(Pages[qrEnterWeights]);
           if(QMonitor.AskYesNo("Target Voltage out of range in head. Would you like to correct the entered values?"))
           {
              Page->ClearUserEntryColors();
              Page->LabelsColor[i] = INT_FROM_RESOURCE(IDN_ERROR_COLOR);
              GotoPage(qrEnterWeights);
           }
            else
              throw EQException("Target Voltage out of range in head" + GetHeadName(i) + ". \nCall Service for help.");
           throw EQException("Unable to set tempeture and/or voltage");
        }
        // Write the head S/N to the log file
        WriteToLogFile(LOG_TAG_GENERAL,QFormatStr("Head %s was replaced. The S/N of the new head is %s", GetHeadName(i).c_str(), m_CurrHeadsParams[i].SerialNumber));

        // Reset the maintenance counter of the replaced head
     }
     YieldAndSleepWizardThread();
  }
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesModel);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->RequestedHeadVoltagesSupport);
  m_ParamMgr->SaveSingleParameter(&m_ParamMgr->PotentiometerValues);

  // Everything is ok - continue.
  return true;
}

int CQuickSHRWizard::GetContinuePageNumber()
{
  return qrResumeWizard;
}

bool CQuickSHRWizard::DoWeightTest(CWizardPage* WizardPage)
{
  CStatusWizardPage* Page             = dynamic_cast<CStatusWizardPage *>(WizardPage);
  bool                     IsPrintCompleted = false;
  int                      EstimatedTime;

  // Enable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(true);
  m_BackEnd->GotoDefaultMode();
  m_BackEnd->EnterMode(WEIGHT_TEST[m_QualityModeIndex][m_OperationModeIndex],
                       MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));
  m_BackEnd->SetOnlineOffline(true);
  m_TotalNumberOfSlices = m_BackEnd->GetTotalNumberOfSlices();

  // Calculating the printing time in 5 minutes Quants
  EstimatedTime = (m_TotalNumberOfSlices * PASS_TIME * PASS_PER_LAYER) / SECONDS_IN_MINUTE;
  EstimatedTime = (EstimatedTime + ROUND_QUANTIZATION_FACTOR) / TIME_ROUND_QUANTIZATION;
  EstimatedTime *= TIME_ROUND_QUANTIZATION;

  Page->SubTitle =  QFormatStr(LOAD_STRING(IDS_DURATION_STR), LOAD_STRING(IDS_PRINTING), QIntToStr(EstimatedTime).c_str());
  Page->Refresh();

  TMachineState State = WaitForStateToDiff(msPrinting);
  if (State == msStopped)
     throw EQException("Error while preparing to print the weight test");
  do
  {
     State = m_BackEnd->GetCurrentMachineState();
     YieldAndSleepWizardThread();
     if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
  }while(State == msPrinting || State == msStopping || State == msStopped);

  // Find out if the print was completed
  IsPrintCompleted = m_BackEnd->IsPrintCompleted();

  m_BackEnd->EnterMode(PRINT_MODE[m_QualityModeIndex][m_OperationModeIndex],
                       MACHINE_QUALITY_MODES_DIR(m_QualityModeIndex,m_OperationModeIndex));

  // Disable vacuum errors
  m_BackEnd->EnableDisableVacuumErrorHandling(false);
  return IsPrintCompleted;
}

void CQuickSHRWizard::SaveWizardSpecificDataToTemporaryFile()
{
  m_TempFile->Write((BYTE*)m_HeadsToBeReplaced,sizeof(m_HeadsToBeReplaced));
  m_TempFile->Write((BYTE*)m_CurrHeadsParams,sizeof(m_CurrHeadsParams));
  m_TempFile->Write((BYTE*)m_PreviousHeadsParams,sizeof(m_PreviousHeadsParams));
  m_TempFile->Write((BYTE*)&m_TargetLayerThickness,sizeof(m_TargetLayerThickness));
  m_TempFile->Write((BYTE*)&m_TotalNumberOfSlices,sizeof(m_TotalNumberOfSlices));
  m_TempFile->Write((BYTE*)&m_QualityModeIndex,sizeof(m_QualityModeIndex));
  m_TempFile->Write((BYTE*)&m_OperationModeIndex,sizeof(m_OperationModeIndex));
}

QString CQuickSHRWizard::GetResumeWarningString()
{
   return GetResumeWarningString(m_ParamMgr->QR_LastState);
}

QString CQuickSHRWizard::GetResumeWarningString(int PageNumber)
{
  QString ResumeString;
  switch (PageNumber)
  {
    case qrHeadsHeating1:
      ResumeString = LOAD_STRING(IDS_PRESS_NEXT_TO_CONTINUE);
      break;

    case qrCoolingDown:
      ResumeString = "Warning: It is assumed that the heads block is empty at this stage.\nIf this is not the case press 'Cancel' and start the wizard again.\nTo continue press 'Next'.";
      break;

    case qrReplacementPosition:
      ResumeString = "Warning: It is assumed that the heads block is empty and cool at this stage.\nIf this is not the case press 'Cancel' and start the wizard again.\nTo continue press 'Next'.";
      break;

    case qrReplaceHead:
    case qrReInsertHead:
      ResumeString = "Warning: It is assumed that at this stage, the heads block is empty, cool and at the replacement position.\nIf this is not the case press 'Cancel' and start the wizard again.\nTo continue press 'Next'.";
      break;

    case qrIntallationCompleteQuery:
    case qrHeadsHeating2:
      ResumeString = "Warning: It is assumed that at this stage, the new head(s) is(are) inserted.\nIf this is not the case press 'Cancel' and start the wizard again.\nTo continue press 'Next'.";
      break;

    case qrVacuumLeakage:
    case qrLastPhaseMessage:
    case qrEnterWeights:
    case qrCalibratingFor:
      ResumeString = LOAD_STRING(IDS_PRESS_NEXT_TO_CONTINUE);
      break;

    default:
      ResumeString = "This is not a legal resume stage. Press 'Cancel' and start the wizard again.";
      EnableDisableNext(false);
      break;

  }
  return ResumeString;
}


void CQuickSHRWizard::CleanUp()
{
  FOR_ALL_QUALITY_MODES(qm)
    	FOR_ALL_OPERATION_MODES(om)
          if (GetModeAccessibility(qm, om))      
              m_BackEnd->DisableConfirmationBypass(PER_MACHINE_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));

  m_BackEnd->GotoDefaultMode();
  m_BackEnd->SetActuatorState(ACTUATOR_ID_WASTE_PUMP,false);
  m_BackEnd->EnableDisableVacuumErrorHandling(true);

  if (!m_CleanUpRequired)
     return;

  m_BackEnd->SetActuatorState(ACTUATOR_ID_AIR_VALVE,false);
  m_BackEnd->SetActuatorState(ACTUATOR_ID_ROLLER_PUMP,false);
  m_BackEnd->TurnHeadFilling(false);

  m_BackEnd->TurnTrayHeater(false);
  m_BackEnd->SetHoodFanIdle();
  m_BackEnd->TurnCoolingFansOnOff(false);

  // Stop ignoring 'Temperature' errors at the filling stage...
  m_BackEnd->SetReportTemperatureError(true);

  EnableAllAxesAndHome();
  m_BackEnd->EnableMotor(false, AXIS_ALL);
  m_BackEnd->EnableDoor(false);

}
//---------------------------------------------------------------------------

void CQuickSHRWizard::WriteToHistoryFile(QString Str)
{
  // Prepare the date/time string
  char DateTimeStr[MAX_DATE_SIZE];
  time_t CurrentTime = time(NULL);
  struct tm *LocalTimeTM = localtime(&CurrentTime);
  strftime(DateTimeStr,MAX_DATE_SIZE,"%d/%m/%y %X",LocalTimeTM);

  // If the handle is still Ok
  if(m_HeadReplacementFile)
  {
    if (Str == "Line")
    {
      fputs("--------------------------------------------------------------------",m_HeadReplacementFile);
      fputs("--------------------------------------------------------",m_HeadReplacementFile);
      fputs("\n",m_HeadReplacementFile);
      return;
    }

    if (Str == "Date")
    {
      fputs(DateTimeStr,m_HeadReplacementFile);
      fputs("\n",m_HeadReplacementFile);
      return;
    }

    if(fputs(Str.c_str(),m_HeadReplacementFile) == EOF)
    {
      // If error, close the file
      fclose(m_HeadReplacementFile);
      m_HeadReplacementFile = NULL;
    } else
    {
      if(fputs("\n",m_HeadReplacementFile) == EOF)
      {
        // If error, close the file
        fclose(m_HeadReplacementFile);
        m_HeadReplacementFile = NULL;
      } else
      {
        // Make sure that the data is on the disk
        fflush(m_HeadReplacementFile);
      }
    }
  }
}

void CQuickSHRWizard::SetResumingPage()
{
  SetNextPage(m_ParamMgr->QR_LastState);
}

