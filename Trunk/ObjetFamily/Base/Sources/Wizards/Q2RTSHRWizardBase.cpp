/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Base for Single head replacement.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Bojena Gleizer.                                          *
 * Last upate: 03/11/2003                                           *
 ********************************************************************/
#include "Q2RTSHRWizardBase.h"
#include "WizardImages.h"
#include "WizardPages.h"
#include "BackEndInterface.h"
#include "QTimer.h"
#include "FrontEnd.h"
#include "HeatersDefs.h"
#include "HeadReplacementDefs.h"
#include "QFileWithCheckSum.h"
#include "QMonitor.h"
#include "rfid.h"
#include "QScopeRestorePoint.h"
#include "Q2RTApplication.h"
#include "LinearInterpolator.h"
#include "MaintenanceCountersDefs.h"
#include "TankIdentificationNotice.h"

const int   A2D_ROOM_TEMPERATURE_FOR_TRAY            = 630;
const int   VACUUM_STABILIZATION_TIME                = 20; // sec
const int   NUM_OF_PURGES_AFTER_HEAD_REPLACMENT      = 5;
const int   WAIT_TIME_BETWEEN_PURGES_IN_SEC          = 10;
const int   WAIT_AFTER_PURGE                         = 2 * 60; // sec

#define WIZARD_HISTORY_FULL_DIR_NAME  (Q2RTApplication->AppFilePath.Value() + "\\" + LOAD_STRING(IDS_SERVICETOOLS_DIRNAME) + GetTitle() + "\\")

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CQ2RTSHRWizardBase::CQ2RTSHRWizardBase(int NameResourceID, QString UserButton1Caption) :
CQ2RTAutoWizard(NameResourceID,true,IN_PROCESS_IMAGE_ID,false,false,UserButton1Caption,"View Movie")
{
 m_VacuumLeakagePageIndex = 0;
     m_PurgePageIndex = 0;
     m_HeadReplacementFile = NULL;
	 m_RunHeadAlignmentWizard = false;
	 m_VacuumLeakage = false;
     m_TempFile = NULL;
     m_LastStateParam= NULL;
     memset( m_HeadsInsertedWrong, 0, sizeof(m_HeadsInsertedWrong));
     memset( m_HeadsToBeReplaced, 0, sizeof(m_HeadsToBeReplaced));
}

CQ2RTSHRWizardBase::~CQ2RTSHRWizardBase()
{
  Q_SAFE_DELETE( m_TempFile );
}

void CQ2RTSHRWizardBase::SetIndexes(int VacuumLeakagePageIndex, int PurgePageIndex)
{
   m_VacuumLeakagePageIndex = VacuumLeakagePageIndex;
   m_PurgePageIndex = PurgePageIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CQ2RTSHRWizardBase::CoolingDownPageEnter(CWizardPage* WizardPage)
{
    // Clear the last machine sub state
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

	int HeadsTemperatures[TOTAL_NUMBER_OF_HEATERS];
    // Set the temperatures for the heads
    for(int i = FIRST_HEAD; i < LAST_HEAD; i++)
		HeadsTemperatures[i] = m_ParamMgr->HeadsA2DRoomTemperature;
    // Set the temperatures for the block heaters
    for (int i = FIRST_BLOCK; i < LAST_BLOCK; i++)
		HeadsTemperatures[i] = m_ParamMgr->BlockA2DRoomTemperature;
//OBJET_MACHINE, PreHeater
	//HeadsTemperatures[PRE_HEATER] = A2D_ROOM_TEMPERATURE_FOR_BLOCK;

    // Turn the fans on for faster cooling
    m_BackEnd->SetHoodFanOnOff(true);
    m_BackEnd->TurnCoolingFansOnOff(true, 100);
    m_BackEnd->HeadHeatersSetIgnoreError(true);
	SelectedHeadsHeatingCycle(m_ParamMgr->HeadsCoolingTimeoutSec, HeadsTemperatures, true);
    m_BackEnd->HeadHeatersSetIgnoreError(false);
    m_BackEnd->TurnTrayHeater(true, A2D_ROOM_TEMPERATURE_FOR_TRAY, true);
    
    while ( m_BackEnd->GetTrayTemperatureStatus() == TRAY_HOT )
    {
       YieldAndSleepWizardThread();
       if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
    }

    m_BackEnd->TurnHeadHeaters(false);
    m_BackEnd->TurnTrayHeater(false);
    m_BackEnd->SetHoodFanIdle();
    m_BackEnd->TurnCoolingFansOnOff(false);
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CQ2RTSHRWizardBase::ReplacementPositionPageEnter(CWizardPage* WizardPage, int PageIndex)
{
    YieldWizardThread();
    TQErrCode Err = Q_NO_ERROR;

    TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_T, 0, BLOCKING, muMM));
    TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_X, m_ParamMgr->XCleanHeadsPosition, NO_BLOCKING, muMM));
    TRY_FUNCTION(Err, m_BackEnd->MoveMotorToAbsolutePosition(AXIS_Y, m_ParamMgr->YCleanHeadsPosition, NO_BLOCKING, muMM));
    TRY_FUNCTION(Err, m_BackEnd->MoveZAxisDown());

    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_X));
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Y));
    TRY_FUNCTION(Err, m_BackEnd->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME));

    if (m_VacuumLeakage)
       SetNextPage(PageIndex);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CQ2RTSHRWizardBase::ReplaceHeadPageEnter(CWizardPage* WizardPage, int PageIndex)
{
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
   Page->Strings.Clear();
   for(int i = TOTAL_NUMBER_OF_HEADS/2 - 1 ; i >= 0; i--)
	   if (m_HeadsToBeReplaced[i])
		  Page->Strings.Add(GetDoubleHeadName(i) + " replaced");
   Page->Refresh();
   SetNextPage(PageIndex);                      
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CQ2RTSHRWizardBase::ReplaceHeadPageLeave(CWizardPage* WizardPage)
{
    bool firstEnter = true;
   FOR_ALL_DOUBLE_HEADS(i)
   {  if (m_HeadsToBeReplaced[i])
	  {
		m_BackEnd->ResetMaintenanceCounter(HeadsCounterID[i*2]);
		m_BackEnd->ResetMaintenanceCounter(HeadsCounterID[i*2+1]);
		m_BackEnd->ResetMaintenanceCounter(HeadsCounter_DC_ID[i*2]);
		m_BackEnd->ResetMaintenanceCounter(HeadsCounter_DC_ID[i*2+1]);

		//check if support head was replaced
		if( firstEnter && !(m_ParamMgr->IsModelHead(i*2))  )
		{
			firstEnter = false;
		}
			if( firstEnter && !(m_ParamMgr->IsModelHead(i*2+1))  )
		{
			firstEnter = false;
		}
	  }
   }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CQ2RTSHRWizardBase::ReInsertHeadPageEnter(CWizardPage* WizardPage)
{
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
   Page->Strings.Clear();
   for(int i = TOTAL_NUMBER_OF_HEADS_HEATERS/2 - 1 ; i >= 0; i--)
      if (m_HeadsToBeReplaced[i])
          Page->Strings.Add("Head " + GetDoubleHeadName(i) + " was re-inserted");
   Page->Refresh();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CQ2RTSHRWizardBase::CheckInsertionPageEnter(CWizardPage* WizardPage)
{
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
   Page->SubTitle = "Please reinsert heads: ";
   for(int i = TOTAL_NUMBER_OF_HEADS_HEATERS/2 - 1; i >= 0; i--)
   {
      if(m_HeadsInsertedWrong[i] == true)
      {
         Page->SubTitle += GetDoubleHeadName(i) + " ";
         // Reset the flag...
         m_HeadsInsertedWrong[i] = false;
      }
   }
   Page->Refresh();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CQ2RTSHRWizardBase::NoCharacterizationDataPageEnter(CWizardPage* WizardPage)
{
   CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
   Page->SubTitle += m_NoCharacterizationDescription;
   Page->Refresh();

   // Turn the power off and open the door
   m_BackEnd->EnableDoor(false);
      TurnPowerOff();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CQ2RTSHRWizardBase::FillBlockPageEnter(CWizardPage* WizardPage, T_AxesTable a_AxesTable)
{
   YieldWizardThread();
   EnableAllAxesAndHome(a_AxesTable);

   TQErrCode Err = Q_NO_ERROR;
   // Go to purge position for case of vacuum leakage
   TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));
   unsigned Timeout   = m_ParamMgr->FillingTimeout;
   // Ignore 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(false);
   CBEMonitorActivator MonitorActivator; //meaning m_BackEnd->TurnHeadFilling(true);
   HeadsFillingCycle(Timeout);
   // Read vacuum sensor
   int VacuumReadingA2D = m_BackEnd->GetAverageVacuumValue_A2D();
   // Wait until vacuum stabilize ,checking the vacuum in both cases:when the heads are filled or not.

   unsigned StartTime = QGetTicks();
   unsigned CurrTime  = QGetTicks();
   while (VacuumReadingA2D > m_ParamMgr->VacuumHighLimit || VacuumReadingA2D < m_ParamMgr->VacuumLowLimit)
   {
   	  if ((CurrTime > StartTime + QSecondsToTicks(VACUUM_STABILIZATION_TIME)) && CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
	  {
		 GotoPage(m_VacuumLeakagePageIndex);
		 return false;
	  }
         if (IsCancelled())
         {
            WriteToLogFile(LOG_TAG_GENERAL,"Fill Block Page Enter was cancelled during vacuum sequnce");
            throw CQ2RTAutoWizardCancelledException();
         }

  	 YieldAndSleepWizardThread();
	 CurrTime         = QGetTicks();
	 VacuumReadingA2D = m_BackEnd->GetAverageVacuumValue_A2D();
  }//end while

  //In this step there is no Vacuum error.
  //But there is need to check if there was timeout while filling the heads
  if ( CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
  {
        Timeout = QSecondsToTicks((Timeout>0) ? Timeout : 0);
    	if (CurrTime >= (StartTime + Timeout))
        {
                WriteToLogFile(LOG_TAG_GENERAL,"Error: Fill Block Page Enter was cancelled because head filling time out, Current time is:%d start time is:%d, and time out is: %d", QIntToStr(CurrTime), QIntToStr(StartTime),QIntToStr(Timeout) );
                m_VacuumLeakage = false;
        	throw EQException(TIMEOUT_REASON(IDS_HEAD_FILLING));
        }
  }      
  m_VacuumLeakage = false;
  GotoPage(m_PurgePageIndex);
  WriteToLogFile(LOG_TAG_GENERAL,"Fill Block Page Enter was ompleted successfully" );
  return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CQ2RTSHRWizardBase::PurgePageEnter(CWizardPage* WizardPage, int qm, int om, T_AxesTable a_AxesTable)
{
   bool ret = true;
   // Loading the right Mode
   m_BackEnd->GotoDefaultMode();
   m_BackEnd->EnterMode(PRINT_MODE[qm][om],
                        MACHINE_QUALITY_MODES_DIR(qm,om));

   EnableAllAxesAndHome(a_AxesTable);
   HeadsHeatingCycle();

   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   // Ignore 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(false);

   CBEMonitorActivator MonitorActivator; //meaning m_BackEnd->TurnHeadFilling(true);
   HeadsFillingCycle(m_ParamMgr->FillingTimeout);
   // Goto Purge Position
   TQErrCode Err = Q_NO_ERROR;
   TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, true));
   for(int i = 0; i < NUM_OF_PURGES_AFTER_HEAD_REPLACMENT; i++)
   {
      m_BackEnd->Purge(false, false);
      WriteToLogFile(LOG_TAG_GENERAL,"Purging");
      if ((ret = Q2RTWizardSleep(m_ParamMgr->MotorPostPurgeTimeSec + WAIT_TIME_BETWEEN_PURGES_IN_SEC)) == false)
         break;
   }
   YieldWizardThread();
   m_BackEnd->WipeHeads(false);
   YieldWizardThread();
   m_BackEnd->WipeHeads(false);
   YieldWizardThread();

   if (!ret) return ret;
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   TRY_FUNCTION(Err, m_BackEnd->GotoPurgePosition(true, false));
   YieldWizardThread();

   if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation))
      if (Q2RTWizardSleep(WAIT_AFTER_PURGE) == false)
         return false;

   // Stop ignoring 'Temperature' errors at the filling stage...
   m_BackEnd->SetReportTemperatureError(true);
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CQ2RTSHRWizardBase::ResumeWizardPageEnter(CWizardPage* WizardPage)
{
   YieldWizardThread();
      try
      {
        // Read the needed wizard data from the temporay file
        if (m_TempFile->BeginRead() == Q_OPEN_FILE_AND_BACKUP_FILE_ERR)
        {
            QMonitor.ErrorMessage("Error while reading the wizard data. Resumption of the wizard could not be possible",ORIGIN_WIZARD_PAGE);
			 SetNextPage(0);
        }
        else
        {
          ResumeWizardSpecificData();
          m_TempFile->EndRead();

          CMessageWizardPage *ResumePage = dynamic_cast<CMessageWizardPage *>(WizardPage);

          ResumePage->SubTitle = GetResumeWarningString();
          ResumePage->Refresh();
          YieldWizardThread();
          SetResumingPage();
        }
      }
      catch(EQFileWithCheckSum &e)
      {
		  QMonitor.ErrorMessage("Error while reading the wizard data. Resumption of the wizard could not be possible",ORIGIN_WIZARD_PAGE);
         SetNextPage(0);
      }
}

void CQ2RTSHRWizardBase::HeadsHeatingCycle(const int TimeOut,int* HeadsTemperatures, bool IncludePreHeater, const bool& bKeepHeatingEvenOpenOrShortCircuit)
{
  CQ2RTAutoWizard::HeadsHeatingCycle(TimeOut,TOTAL_NUMBER_OF_HEATERS,IncludePreHeater,HeadsTemperatures, bKeepHeatingEvenOpenOrShortCircuit);
}

void CQ2RTSHRWizardBase::HeadsOnlyHeatingCycle(const int TimeOut,int* HeadsTemperatures, const bool& bKeepHeatingEvenOpenOrShortCircuit)
{
  CParamScopeRestorePointArray<bool,TOTAL_NUMBER_OF_HEATERS> Mask(m_ParamMgr->HeatersMaskTable);
  for(int i = TOTAL_NUMBER_OF_HEADS_HEATERS ; i < TOTAL_NUMBER_OF_HEATERS; i++)
         m_ParamMgr->HeatersMaskTable[i] = false;
  HeadsHeatingCycle(TimeOut,HeadsTemperatures, false, bKeepHeatingEvenOpenOrShortCircuit);
}

void CQ2RTSHRWizardBase::SelectedHeadsHeatingCycle(const int TimeOut,int* HeadsTemperatures, const bool& bKeepHeatingEvenOpenOrShortCircuit)
{
  CParamScopeRestorePointArray<bool,TOTAL_NUMBER_OF_HEATERS> Mask(m_ParamMgr->HeatersMaskTable);
  for(int i = 0 ; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
  {
     if (i < TOTAL_NUMBER_OF_HEADS_HEATERS && m_HeadsToBeReplaced[i/2])
         m_ParamMgr->HeatersMaskTable[i] = false;
  }
  HeadsHeatingCycle(TimeOut,HeadsTemperatures,true, bKeepHeatingEvenOpenOrShortCircuit);
}

void CQ2RTSHRWizardBase::SaveWizardDataToTemporaryFile()
{
  // Write the needed wizard data (for case of resuming the wizard) to a temporay file
  m_TempFile->BeginWrite();

  SaveWizardSpecificDataToTemporaryFile();
  if(m_TempFile->EndWrite() == Q_OPEN_FILE_AND_BACKUP_FILE_ERR)
        QMonitor.ErrorMessage("Error while writing the wizard data. Resumption of the wizard will not be possible",ORIGIN_WIZARD_PAGE);

}
void CQ2RTSHRWizardBase::TurnPowerOff()
{
  if (!m_BackEnd->IsPowerTurnedOn()) // Check if power is already OFF
    return;

  WriteToLogFile(LOG_TAG_GENERAL,"Turning power OFF");

  // First Pause following threads:
  m_BackEnd->PauseOCBStatusThread();  // the OCB status sender repeatedly calls GetLiquidTanksWeight(), which eventualy enables the RFID Sampling. This is undesired, after ReaderStopAndClear() is called. (see below)
  m_BackEnd->PauseHeadStatusThread(); // don't send messages to the turned off OHDB. (avoid communiaction errors)		

  // Send tank removal signals, and block any further signals from being processed.
  m_BackEnd->SendTanksRemovalSignals();
  m_BackEnd->IgnoreTankInsertionAndRemovalSignals();
  CRFIDBase* RFIDInstance = CRFID::Instance();
  if(RFIDInstance!=NULL)
  {
	CRFID::Instance()->ReaderStopAndClear(0/*for reader num: 0*/);
    CRFID::Instance()->ReaderStopAndClear(1/*for reader num: 1*/);
  }
	
 for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
  {
	 m_BackEnd->SkipMachineResinTypeChecking(static_cast<TTankIndex>(i), true);
	 TTankIDNotice::HideDialog(static_cast<TTankIndex>(i));
  }

  // Turn Off power supply
  if (m_BackEnd->IsPowerTurnedOn() == true)
  {
    m_BackEnd->TurnPower(false);
    m_BackEnd->WaitForPowerOnOff();
  }
}

void CQ2RTSHRWizardBase::TurnPowerOn()
{
  if (m_BackEnd->IsPowerTurnedOn()) // Check if power is already ON
    return;

  WriteToLogFile(LOG_TAG_GENERAL,"Turning power ON");

  m_BackEnd->TurnPower(true);

  if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation) && (CRFID::Instance()->WaitForReaderPowerUp(0) != true)
  && (CRFID::Instance()->WaitForReaderPowerUp(1) != true))		
    CQLog::Write(LOG_TAG_RFID,"Error waiting to RF Reader PowerUp.");

  // Since OHDB power supply is used also for the RFID reader, and was turned OFF,
  // we need to restart tank identification process:
  m_BackEnd->AllowTankInsertionAndRemovalSignals();  
  m_BackEnd->GetLiquidTankInsertedStatus();

  m_BackEnd->ResumeOCBStatusThread();
  m_BackEnd->ResumeHeadStatusThread();

  // Following Sleep loop solved the slow-response of the RFID mechanism.
  // I don't understand why the RFID should become slower while Wizard is running.
  if (CHECK_NOT_EMULATION(m_ParamMgr->OHDB_Emulation))
     for (int i=0; i<1000; i++)
	   YieldAndSleepWizardThread(10);
  /* Refresh the front-end display*/
	FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY,0,true);
	for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	{
		m_BackEnd->SkipMachineResinTypeChecking(i, false);
	}
 /* for (int Tank = FIRST_TANK_TYPE; Tank < LAST_TANK_TYPE; ++Tank)
	m_BackEnd->UpdateTanks(static_cast<TTankIndex>(Tank));
  for (int Tank = FIRST_TANK_TYPE; Tank < LAST_TANK_TYPE; ++Tank)
			while (false == m_BackEnd->WaitForIdentificationCompletion(
				static_cast<TTankIndex>(Tank),
				true, //true = DontWaitIfTankIsOut
				100)) //time to wait
			{
				YieldWizardThread();
			}
   m_BackEnd->UpdateChambers(false);
 */
 for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
	 TTankIDNotice::HideDialog(static_cast<TTankIndex>(i));
}

bool CQ2RTSHRWizardBase::RunHeadAlignment()
{
   return m_RunHeadAlignmentWizard;
}

void CQ2RTSHRWizardBase::HeadsHeating1PageEnter(CWizardPage* WizardPage)
{
    YieldWizardThread();
     m_BackEnd->HeadHeatersSetIgnoreError(true);
    SelectedHeadsHeatingCycle(DEFAULT_TIMEOUT, NULL,true);
     m_BackEnd->HeadHeatersSetIgnoreError(false);
}

bool CQ2RTSHRWizardBase::Start(void)
{
  m_TempFile = new CQFileWithCheckSum(Q2RTApplication->AppFilePath.Value()+ LOAD_STRING(IDS_CONFIGS_DIRNAME) + GetTitle() +".tmp");

  EnableHistoryFile();
  
  FOR_ALL_MODES(qm, om)
     if (GetModeAccessibility(qm, om))
        m_BackEnd->EnableConfirmationBypass(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
  return CQ2RTAutoWizard::Start();
}

void CQ2RTSHRWizardBase::Q2RTCleanUp(void)
{
  try
  {
     FOR_ALL_MODES(qm, om)
        if (GetModeAccessibility(qm, om))
           m_BackEnd->DisableConfirmationBypass(PRINT_MODE[qm][om], MACHINE_QUALITY_MODES_DIR(qm,om));
     m_BackEnd->HeadHeatersSetIgnoreError(false);
     CQ2RTAutoWizard::Q2RTCleanUp();
  }
  __finally
  {
     Q_SAFE_DELETE(m_TempFile);
  }
}
void CQ2RTSHRWizardBase::SaveLastState()
{
  m_LastStateParam->AssignFromString(QIntToStr(GetCurrentPageIndex()));
  m_ParamMgr->SaveSingleParameter(m_LastStateParam);
  SaveWizardDataToTemporaryFile();
}

void CQ2RTSHRWizardBase::HeadsHeating2PageEnter(CWizardPage* WizardPage)
{
/*   FOR_ALL_MODES(qm, om)
   {
		   YieldWizardThread();
		   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
		   if (GetModeAccessibility(qm, om) == false)
			  continue;

		   // Setting temeperature and voltage to the head according to the data in it
		   m_BackEnd->GotoDefaultMode();
		   m_BackEnd->EnterMode(PRINT_MODE[qm][om],MACHINE_QUALITY_MODES_DIR(qm,om));
		   /*New Request : no need to change temperature after Head Replacement*/
		   /*  if (!SetHeadsTemperature())
			  return;*/
//   }

   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();

   // This is a patch that saves heating/cooling time (heating to the first lowest weight-test-temperature first):
   m_BackEnd->GotoDefaultMode();
   m_BackEnd->EnterMode(PRINT_MODE[LOW_TEMPATURE_INDEX][0],
                        MACHINE_QUALITY_MODES_DIR(LOW_TEMPATURE_INDEX,0));
   HeadsHeatingCycle();
   
   // This one is a patch to fix bug 6114
   EnableAxeAndHome(AXIS_Y, true, true);
   EnableAxeAndHome(AXIS_X, true, true);
   
   HeadsDrainingCycle(WizardPage,m_ParamMgr->HF_StabilizationTime, false);
   if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation))
      Q2RTWizardSleep(1*60);
   YieldWizardThread();
   if (IsCancelled()) throw CQ2RTAutoWizardCancelledException();
   if (CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && !m_BackEnd->AreHeadsEmpty())
      throw EQException("Wrong thermistor value after emptying");
   WriteToLogFile(LOG_TAG_GENERAL,"Heads are empty");
   GotoNextPage();
}

bool CQ2RTSHRWizardBase::SetHeadsTemperature()
{
  // Calculate the new temperature set for the replaced heads and their voltage
  // by their layer thickness vs. voltage line equation and the current layer thickness setting
  int   Tn, SVn;
  CLinearInterpolator<int> LinearInterpolator;

  for(int i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
  {
     YieldAndSleepWizardThread();
     if (m_HeadsToBeReplaced[i/2])
     {
        LinearInterpolator.Clear();
        LinearInterpolator.AddEntry(60,m_CurrHeadsParams[i].A2DValueFor60C);
        LinearInterpolator.AddEntry(80,m_CurrHeadsParams[i].A2DValueFor80C);
        Tn = (IS_MODEL_HEAD(i) ? m_ParamMgr->ModelWorkTemperature : m_ParamMgr->SupportWorkTemperature);

        // Retrieve the working temperature as ADu
        SVn = LinearInterpolator.Interpolate(Tn);

        // Set the A/D value to the 'PerMachine'
        m_BackEnd->SaveHeadHeaterParameter(i, SVn);
     }
  }
  return true;
}

 void CQ2RTSHRWizardBase::InitHeadsToBeReplacedArray()
 {
   FOR_ALL_DOUBLE_HEADS(i)
   {
      m_HeadsToBeReplaced[i] = false;
   }
 }


