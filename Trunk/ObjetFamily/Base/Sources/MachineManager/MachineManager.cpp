/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Machine main management class                            *
 * Module Description: This is the implementation of the central    *
 *                     controlling and management object.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 05/06/2003                                           *
 ********************************************************************/

#include "MachineManager.h"
#include "MachineSequencer.h"
#include "Q2RTErrors.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"
#include "LayerProcess.h"
#include "FrontEnd.h"
#include "HostComm.h"
#include "AppLogFile.h"
#include "BackEndInterface.h"
#include "ModesManager.h"
#include "Motor.h"
#include "PrintControl.h"
#include "BITManager.h"
#include "HeadHeaters.h"
#include "Roller.h"
#include "ModesDefs.h"
#include "MaintenanceCountersDefs.h"
#include "HeadFilling.h"
#include "SignalTower.h"

const QString StatesLookup[] =
{
    "Power-Up",
    "Idle",
    "Pre-Print",
    "Printing",
    "Stopping",
    "Pausing",
    "Paused",
    "Stopped",
    "Terminating",
    "Purge",
    "Fire All",
    "Test Pattern",
    "GoTo Purge",
    "Wipe",
	"Standby 1",
    "Standby 2",
//#if defined CONNEX_260 || defined OBJET_260  //runtime objet
    "Tray Removal Position",
    "Head Inspection Position",
//#else
//"",
//#endif
    "Initializing...",
    "PowerFail Shutdown",
    "Tray Lower Position"
};

const TQWaitTime TIME_TO_GET_TEST_NUMBER_MS = 3000;

const int SECONDS_IN_HOUR                = 60 * 60; //itamar, Super purge

static QString FormatNames(const TFileNamesArray FlexFileNames, bool AsMode)
{
	QString ret;
	int LineNum = 1;
    for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
    {
        TBMPFileName FileName = (TBMPFileName)FlexFileNames + i * MAX_FILE_PATH;
		if(0 != strcmp(FileName, ""))
		{
			ret += ChamberToStr((TChamberIndex)i, AsMode) + ": \"" + QExtractFileName(FileName) + "\" ";

			if((float)ret.size() / 75 > LineNum)
			{
				ret += "\n";
				LineNum++;
			}
		}
    }
    return ret;
};

EMachineManager::EMachineManager(const QString ErrMsg,const TQErrCode ErrCode) : EQException(ErrMsg,ErrCode) {}
EMachineManager::EMachineManager(const TQErrCode ErrCode) : EQException(PrintErrorMessage(ErrCode),ErrCode) {}

// Constructor
CMachineManager::CMachineManager(void) : CQThread(true,"MachineManager")
{
    // Initialize dynamic methods
    INIT_METHOD(CMachineManager,StartPrint);
	INIT_METHOD(CMachineManager,StopPrint);
	INIT_METHOD(CMachineManager,AddPFFPrintJob);
    INIT_METHOD(CMachineManager,LoadLocalPrintJob);
//OBJET_MACHINE //config check!!!
    INIT_METHOD(CMachineManager,Load3LocalPrintJob);
    INIT_METHOD(CMachineManager,Load3LocalDoublePrintJob);
	INIT_METHOD(CMachineManager,Load4LocalPrintJob);
    //INIT_METHOD(CMachineManager,Load4LocalDoublePrintJob);
	INIT_METHOD(CMachineManager,Load8LocalPrintJob);
    INIT_METHOD(CMachineManager,LoadLocalDoublePrintJob);
    INIT_METHOD(CMachineManager,GetCurrentState);
    INIT_METHOD(CMachineManager,SetOnlineOffline);
	INIT_METHOD(CMachineManager,OffsetZStart);
    INIT_METHOD(CMachineManager,DisableStandbyPhase);
    INIT_METHOD(CMachineManager,EnableStandbyPhase);
    INIT_METHOD(CMachineManager,GetStandbyPhaseValue);
    INIT_METHOD(CMachineManager,TestPattern);
    INIT_METHOD(CMachineManager,Wipe);
    INIT_METHOD(CMachineManager,Purge);
    INIT_METHOD(CMachineManager,FireAll);
    INIT_METHOD(CMachineManager,DoFireAll);
    INIT_METHOD(CMachineManager,GoToPurgePosition);
    INIT_METHOD(CMachineManager,EnterStandbyState);
    INIT_METHOD(CMachineManager,ExitStandbyState);
    INIT_METHOD(CMachineManager,GotoTrayRemovalPosition);
    INIT_METHOD(CMachineManager,GotoTrayLowerPosition);
    INIT_METHOD(CMachineManager,GotoHeadInspectionPosition);
    INIT_METHOD(CMachineManager,PowerFailShutdown);
    INIT_METHOD(CMachineManager,WaitForEndOfPrinting);

    m_CurrentState                 = msPowerUp;
    m_LastState                    = msPowerUp;;
    m_CurrentJob                   = NULL;
    m_Online                       = false;
    m_StandbyPhaseOn               = true;
    m_JobOrigin                    = LocalJob;
    m_QualityMode                  = HQ_INDEX;
    m_PrintCompleted               = true;
    m_PerformSinglePurgeInSequence = true;
    m_EnableErrorMessageDuringStop= true;

	// Create the machine sequencer instance
    m_MachineSequencer = new CMachineSequencer("MachineSequencer");

    // Create a message queue for machine manager commands (only one command can be at the queue at a time)
    m_CommandsQueue    = new TCommandsQueue   (1,"",false);
    m_TestQueue        = new TTestQueue       (1,"",false);
    m_TestPatternQueue = new TTestPatternQueue(1,"",false);

    // Install the error notification callback
    CErrorHandler::Instance()->AddObserver(ErrorHandlerObserver,reinterpret_cast<TGenericCockie>(this));

    //callback for turning off everything on print exception
    SetExceptionCallbacks(ThreadExceptionHandlerCallback,reinterpret_cast<TGenericCockie>(this),ThreadUnExpectedExceptionHandlerCallback,reinterpret_cast<TGenericCockie>(this)  );


    m_ParamsMgr = CAppParams::Instance();

    m_PrintingWasStopped = false;

    m_IsWizardRunning       = false;
    m_IsPythonWizardRunning = false;
    m_IsPrintingWizardRunning = false;
    m_CanAddJobFromHost = false;
    m_runHCW = false;
	m_RunMRW = false;

    m_IsBITRunning = false;

    // Create a file print job
	m_LocalFilePrintJob  = new CFilePrintJob();
	m_DoubleFilePrintJob = new CDoubleFilePrintJob();

	m_pPFFSliceQueueThread = new CPFFSliceQueueThread();
	m_PFFPrintJob = new CPFFPrintJob();

//OBJET_MACHINE feature
    m_batchNoTable = CBatchNoTable::GetInstance();

    InitFlexibleParams();
}

// Destructor
CMachineManager::~CMachineManager(void)
{
	StopPrint(Q2RT_EMBEDDED_RESPONSE);


    Terminate();
    m_CommandsQueue->Release();
    WaitFor();
    Q_SAFE_DELETE(m_CommandsQueue);

    m_TestQueue->Release();
    WaitFor();
    Q_SAFE_DELETE(m_TestQueue);

    m_TestPatternQueue->Release();
    WaitFor();
	Q_SAFE_DELETE(m_TestPatternQueue);
	Q_SAFE_DELETE(m_MachineSequencer);
	// If the current job  has not been deleted yet, delete it now
	Q_SAFE_DELETE(m_CurrentJob);
    Q_SAFE_DELETE(m_LocalFilePrintJob);
	Q_SAFE_DELETE(m_DoubleFilePrintJob);
	Q_SAFE_DELETE(m_pPFFSliceQueueThread);
    Q_SAFE_DELETE(m_PFFPrintJob);
}

// Add a new printing job
void CMachineManager::AddJob(CPrintJob *Job, TJobOrigin JobOrigin /* = LocalJob by default*/ /*, bool DeletePrevJob*/)
{
    // A job change is possible only in idle mode
    if((m_CurrentState != msIdle) &&
            (m_CurrentState != msStandby1) &&
            (m_CurrentState != msStandby2))
    {
        if(m_CurrentJob != NULL)
            throw EMachineManager("Can not add a new job before previous job is done");
	}

    if( !IsPrintingModeAllowed(Job) )
	{
	   QMonitor.ErrorMessage("After replacing the Support material with a different type, you must run the Head Optimization Wizard before printing in this mode is enabled");
	   throw EMachineManager(Q2RT_PRINTING_MODE_NOT_ALLOWED);
    }

    if (JobOrigin == JobFromHost)
    {
        if (GetCanAddJobFromHost())
        {
			CQLog::Write(LOG_TAG_PRINT,"m_IsWizardRunning: %d, m_CanAddJobFromHost:%d",m_IsWizardRunning,m_CanAddJobFromHost);
            throw EMachineManager(Q2RT_EMBEDDED_RESPONSE);
        }
        Q2RTApplication->GetHostComm()->SetOnlineOffline(m_Online);
    }
    else
        SetCanAddJobFromHost(m_IsWizardRunning);


    CQLog::Write(LOG_TAG_PRINT,"New print job name: %s, job id: %d",Job->GetJobName().c_str(), Job->GetJobId());

    // Marking whether this is a local job or job from the host...
    m_JobOrigin          = JobOrigin;


    // Set the new job as the current job
    m_CurrentJob = Job;

    // Prepare to print in the required resolution
    m_QualityMode = Job->GetQualityMode();
    m_OperationMode = Job->GetOperationMode();
    CQLog::Write(LOG_TAG_PRINT,"AddJob Operation Mode:%d, Quality Mode:%d",m_OperationMode,m_QualityMode);


    // set the correct Y resolution according to host parameters:
    if (Job->GetYResolutionFactor() == 1)
        m_ParamsMgr->Shift_600DPI = false;
    else if (Job->GetYResolutionFactor() == 2)
        m_ParamsMgr->Shift_600DPI = true;

    m_MachineSequencer->ResetLastPrintedSliceNum();

    if(!Job->IsContinue())//bug 6912
    {
        m_MachineSequencer->MotorInitBeforeFirstLayer();
        m_MachineSequencer->UpdateLastSliceZPosition();
    }

    m_TotalNumberOfSlices = Job->GetTotalNumberOfSlices();

    // Update front-end
    FrontEndInterface->UpdateStatus(FE_TOTAL_NUM_OF_SLICES,Job->GetTotalNumberOfSlices());
    FrontEndInterface->UpdateStatus(FE_TOTAL_MODEL_HEIGHT,Job->GetTotalHeightInMM());
    FrontEndInterface->UpdateStatus(FE_CURRENT_SLICE,-1);
    FrontEndInterface->UpdateStatus(FE_CURRENT_MODEL_NAME,Job->GetJobName());

    // If we are already online, start printing immediately
    if(IsOnline())
        StartPrint();
}

void CMachineManager::SetFlexibleParams(CFlexibleParameters& FlexParamsMsg)
{
	m_flexibleParameters.m_numOfUVOn = FlexParamsMsg.m_numOfUVOn;
	m_flexibleParameters.m_delayBetweenLayers = FlexParamsMsg.m_delayBetweenLayers;
	m_flexibleParameters.m_rollerSpeed = FlexParamsMsg.m_rollerSpeed;
}
void CMachineManager::SetRollerSpeedAccordingToFlexibleParams(CFlexibleParameters& FlexParamsMsg)
{
CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();
CQLog::Write(LOG_TAG_PRINT,"SetRollerSpeedAccordingToFlexibleParams , rollerSpeed :%s",FlexParamsMsg.m_rollerSpeed.c_str());
  if(FlexParamsMsg.m_rollerSpeed == "FLX" )
	{
		CQLog::Write(LOG_TAG_PRINT,"SetRollerSpeedAccordingToFlexibleParams FLX");
		// save original parameter
		m_MachineSequencer->SetPrevRollerSpeed(m_ParamsMgr->RollerVelocity);
		// update parameter
		 m_ParamsMgr->RollerVelocity = m_ParamsMgr->RollerSpeedFlex;
		 m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->RollerVelocity);
		 CQLog::Write(LOG_TAG_PRINT,"SetRollerSpeedAccordingToFlexibleParams =  %d",m_ParamsMgr->RollerVelocity.Value());
	  // update OHDB
	  Roller->SetRollerParms(m_ParamsMgr->RollerVelocity);
	}
	else if(FlexParamsMsg.m_rollerSpeed == "RGD" )//"RGD"
   {
	 CQLog::Write(LOG_TAG_PRINT,"SetRollerSpeedAccordingToFlexibleParams RIGID");
	 // save original parameter
	 m_MachineSequencer->SetPrevRollerSpeed(m_ParamsMgr->RollerVelocity);
		// update parameter
	 m_ParamsMgr->RollerVelocity = m_ParamsMgr->RollerSpeedRigid;
	 m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->RollerVelocity);
	 CQLog::Write(LOG_TAG_PRINT,"SetRollerSpeedAccordingToFlexibleParams =  %d",m_ParamsMgr->RollerVelocity.Value());
	  // update OHDB
	 Roller->SetRollerParms(m_ParamsMgr->RollerVelocity);
   }
}

void CMachineManager::RevertRollerSpeed()
{
   CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();
   int prevRollerSpeed = m_MachineSequencer->GetPrevRollerSpeed();
   if( prevRollerSpeed != -1)
   {
	m_ParamsMgr->RollerVelocity =  m_MachineSequencer->GetPrevRollerSpeed();
	 CQLog::Write(LOG_TAG_PRINT,"RollerSpeed after revert =  %d",m_ParamsMgr->RollerVelocity.Value());
	 m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->RollerVelocity);
	 // update OHDB
	Roller->SetRollerParms(m_ParamsMgr->RollerVelocity);
	m_MachineSequencer->SetPrevRollerSpeed(-1);
   }
} 
void CMachineManager::SetHeadsAmbientTempAccordingToFlexibleParams(CFlexibleParameters& FlexParamsMsg)
{
  if(FlexParamsMsg.m_numOfUVOn==1)
  {
	   CQLog::Write(LOG_TAG_PRINT,"SetHeadsAmbientTempAccordingToFlexibleParams , num of UV's is :%d",FlexParamsMsg.m_numOfUVOn);
	  // save original parameter
	  m_MachineSequencer->SetPrevHighLimit(m_ParamsMgr->HeadAmbientTemperatureHighLimit);
	  // update parameter
	  m_ParamsMgr->HeadAmbientTemperatureHighLimit = m_ParamsMgr->HeadAmbientTemperatureHighLimitOneUVLamp;
	  m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->HeadAmbientTemperatureHighLimit);
	  CQLog::Write(LOG_TAG_PRINT,"HeadAmbientTemperatureHighLimit =  %d",m_ParamsMgr->HeadAmbientTemperatureHighLimit.Value());
	  // update OHDB
	  m_MachineSequencer->SetOHDBAmbientParams();
  }
}
void CMachineManager::RevertHeadsAmbientTemp()
{
   int prevTemp = m_MachineSequencer->GetPrevHighLimit();
   if( prevTemp != -1)
   {
	m_ParamsMgr->HeadAmbientTemperatureHighLimit =  m_MachineSequencer->GetPrevHighLimit();
	 CQLog::Write(LOG_TAG_PRINT,"HeadAmbientTemperatureHighLimit after revert =  %d",m_ParamsMgr->HeadAmbientTemperatureHighLimit.Value());
	 m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->HeadAmbientTemperatureHighLimit);
	 // update OHDB
	m_MachineSequencer->SetOHDBAmbientParams();
	m_MachineSequencer->SetPrevHighLimit(-1);
   }

}
//-1 tells to use the values from the modes and not the from the host values
void CMachineManager::InitFlexibleParams()
{
	m_flexibleParameters.m_numOfUVOn = -1;
	m_flexibleParameters.m_delayBetweenLayers = -1;
	m_flexibleParameters.m_rollerSpeed = "RGD";
}

// Thread execute function (override)
void CMachineManager::Execute(void)
{
    TMachineState OldState = msPowerUp;
    Q2RTApplication->RegisterToPythonExceptionThrowers(GetCurrentThreadId());

    try {
    for(;;)
    {
        // Main state machine
        switch(m_CurrentState)
        {
        case msPowerUp:
            FrontEndInterface->UpdateStatus(FE_CURRENT_MACHINE_STATE,(int)msInitializing);
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = PowerUpPhase();
            SetOnlineOffline(false);
            break;

        case msIdle:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE);
            FrontEndInterface->UpdateStatus(FE_NEW_JOB,FE_UI_NO_MODE);
            m_CurrentState = IdlePhase();
            break;

        case msPrePrint:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = PrePrintPhase();
            break;

        case msPrinting:
            m_CurrentState = PrintingPhase();
            break;

		case msStopping:
			m_CurrentState = StoppingPhase();
            break;

        case msPausing:
            m_CurrentState = PausingPhase();
            break;

        case msPaused:
            FrontEndInterface->EnableDisableControl(FE_PAUSE,false);
            m_CurrentState = PausedPhase();
            break;

        case msStopped:
        {
            // Delete current print job.
            m_CurrentJob = NULL;

            // Set the machine in offline state
			SetOnlineOffline(false);

            FrontEndInterface->EnableDisableControl(FE_PAUSE,false);
            FrontEndInterface->EnableDisableControl(FE_STOP,false);

            THaspConnectionStatus HaspStatus = CHaspInterfaceWrapper::Instance()->GetConnectionStatus();
            if(HaspStatus == hsConnected)
                FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);

            m_CurrentState = StoppedPhase();
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE);
            FrontEndInterface->UpdateStatus(FE_NEW_JOB,FE_UI_NO_MODE);
            break;
        }

        case msTerminate:
            // Leave the thread
            return;

        case msPowerFailShutdown:
            m_CurrentState = PowerFailShutdownPhase();
            break;

        case msPurge:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = PurgePhase();
            break;

        case msFireAll:
			FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = FireAllPhase();
            break;

        case msTestPattern:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
			m_CurrentState = TestPatternPhase();
            break;

		case msGoToPurge:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = GoToPurgePositionPhase();
            break;

        case msRemovalTray:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = GotoTrayRemovalPositionPhase();
            break;

        case msLowerTray:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = GotoTrayLowerPositionPhase();
            break;

        case msHeadInspection:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = GotoHeadInspectionPositionPhase();
            break;

		case msWipe:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);
            m_CurrentState = WipePhase();
            break;

        case msStandby1:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE);
            FrontEndInterface->UpdateStatus(FE_NEW_JOB,FE_UI_NO_MODE);
            m_CurrentState = Standby1Phase();
            break;

        case msStandby2:
            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE);
            FrontEndInterface->UpdateStatus(FE_NEW_JOB,FE_UI_NO_MODE);
            m_CurrentState = Standby2Phase();
            break;
        }

        // If state change, update the front end
        if(m_CurrentState != OldState)
        {
            FrontEndInterface->UpdateStatus(FE_CURRENT_MACHINE_STATE,(int)m_CurrentState);
            OldState = m_CurrentState;
        }
    }
    } catch(...) {
        CQLog::Write(LOG_TAG_GENERAL, "CMachineManager::Execute - unexpected error");
        if (!Terminated)
          throw EQException("CMachineManager::Execute - unexpected error");
	}
}

//itamar, Super purge
bool CMachineManager::HCWMessagesAlertCheck()
{
    bool stopPrint = false;
    m_runHCW = false;
    int TimeoutBetweenAlertMsgs = m_ParamsMgr->HCW_TimeoutBetweenAlertMsgs; //in hours
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    int LastHCWActivationTime = BackEnd->GetMaintenanceCounterElapsedSeconds(LAST_HCW_ACTIVATION_TIME_COUNTER_ID) / SECONDS_IN_HOUR;
    if(TimeoutBetweenAlertMsgs<LastHCWActivationTime && LastHCWActivationTime<2*TimeoutBetweenAlertMsgs)
    {
        if(QMonitor.AskYesNo("To ensure quality printing, you should clean the print heads.\n\rDo you want to run the Head Cleaning wizard now?"))
        {
            stopPrint = true;
            m_runHCW = true;
        }
        else
            CQLog::Write(LOG_TAG_GENERAL,"Super Purge: 1st message canceled, last HCW activation time = %d",LastHCWActivationTime);
    }
    else if(2*TimeoutBetweenAlertMsgs<=LastHCWActivationTime && LastHCWActivationTime<3*TimeoutBetweenAlertMsgs)
    {
        if(QMonitor.AskYesNo("To ensure quality printing, you should clean the print heads now!\n\rRun the Head Cleaning wizard?"))
        {
            stopPrint = true;
            m_runHCW = true;
        }
        else
			CQLog::Write(LOG_TAG_GENERAL,"Super Purge: 2nd message canceled, last HCW activation time = %d",LastHCWActivationTime);
    }
    else if(LastHCWActivationTime>=3*TimeoutBetweenAlertMsgs)
    {
        if (QMonitor.AskYesNo("To prevent printing problems, you must clean the print heads before continuing.\n\rIf you click Yes, the Head Cleaning wizard will open.\n\rIf you click No, the print job will be cancelled."))
        {
            m_runHCW = true;
        }
        else
        {
            CQLog::Write(LOG_TAG_GENERAL,"Super Purge: print job was cancelled, last HCW activation time = %d",LastHCWActivationTime);
            m_runHCW = false;
            m_Online = false;
            FrontEndInterface->EnableDisableControl(FE_ONLINE_MODE,m_Online);
        }
        stopPrint = true;
    }
    return stopPrint;
}

//Check if the user wants to run MRW after at least one channel "slept" without resin within enough time and might harm the pre-heater
bool CMachineManager::RunMRWAfterSleepingChannelsAlert()
{
	m_RunMRW = false;
	bool stopPrint = false;
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	int ChannelsSleepingTime = BackEnd->GetMaintenanceCounterElapsedSeconds(CHANNEL_SLEEPING_MAX_TIME_COUNTER_ID) / SECONDS_IN_HOUR;//FindMaxSleepingChannelTime();

	if (ChannelsSleepingTime > m_ParamsMgr->MRW_ChannelSleepingTimeBeforeAlert)
	{
		if(QMonitor.AskYesNo(QFormatStr("It is recommended after more than %d hours to run the Material Replacement Wizard and switch from '3 Materials' to '6 Materials' and then back.\nThis prevents the materials in the unused tubes from damaging the print block.\n\rDo you want to run the Material Replacement wizard now?",m_ParamsMgr->MRW_ChannelSleepingTimeBeforeAlert.Value())))
		{
			m_RunMRW = true;
			stopPrint = true;	
		}
	}

	return stopPrint;
}

//Check all MODEL channels and find the maximum sleeping time
int CMachineManager::FindMaxSleepingChannelTime()
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	UINT Max = 0;
	UINT CurrentCounter = 0;

	for (int i = 0; i < MAX_NUMBER_OF_MODEL_CHAMBERS-1; i++) //size of (MAX_NUMBER_OF_MODEL_CHAMBERS-1) since MODEL7 is also support in DM6 and shouldn't be taken into account here
	{
		CurrentCounter = BackEnd->GetMaintenanceCounterElapsedSeconds(SleepingChannelsCounterID[i]); //Update the counter with the max value of all the other channels counters
		if (CurrentCounter > Max)
		{
			Max = CurrentCounter;
		}
	}

	return Max;
}

// Start the printing of the current job
TQErrCode CMachineManager::StartPrint(void)
{
    // Check if a job is assigned
    if(m_CurrentJob == NULL)
        throw EMachineManager("No job is assigned for printing");

    // We can start a print from one of the following states:
    if((m_CurrentState != msIdle) &&
			(m_CurrentState != msStopped) &&
            (m_CurrentState != msPaused) &&
            (m_CurrentState != msPausing) &&
            (m_CurrentState != msStandby1) &&
            (m_CurrentState != msStandby2))
        throw EMachineManager("Can not start print from current state (" + StatesLookup[m_CurrentState] + ")",Q2RT_CANT_PRINT_FROM_THIS_STATE);

    if(BeforePrintBIT())
    {
        ShowMaterialWarning(SHOW_BEFORE_PRINTING);

        // Enable the print maintenance counters
        EnableDisablePrintTimeCounters(true);

        // If it is a continue after pause, restart the slice processing
        if((m_CurrentState == msPaused) || (m_CurrentState == msPausing))
            Q2RTApplication->GetLayerProcess()->ResumeProcessing();

        // Start printing
        m_CommandsQueue->Send(cmdStart);
    }

    return Q_NO_ERROR;
}

// Pause current printing
TQErrCode CMachineManager::PausePrint(bool PausedByHost)
{
    // We can pause only from printing state
	if(((m_CurrentState == msIdle)      ||
            (m_CurrentState == msStandby1)  ||
            (m_CurrentState == msStandby2)) &&
            (m_CurrentJob != NULL))
        StopPrint(Q2RT_EMBEDDED_RESPONSE);

    if(m_CurrentState != msPrinting)
        throw EMachineManager("Can not pause print from current state (" + StatesLookup[m_CurrentState] + ")");

    FrontEndInterface->EnableDisableControl(FE_PAUSE,false);
    FrontEndInterface->EnableDisableControl(FE_STOP, false);
    m_CurrentJob->UpdatePrintPausedByHost(PausedByHost);
    m_CurrentState = msPausing;
	m_MachineSequencer->MarkStateChange(msPausing);

    // Cancel current sequence and set start command
    m_CommandsQueue->Send(cmdPause);
    Q2RTApplication->GetLayerProcess()->PauseProcessing();

    // Disable the print counters
    EnableDisablePrintTimeCounters(false);

    return Q_NO_ERROR;
}

TQErrCode CMachineManager::PowerFailShutdown()
{
    StopPrint(Q2RT_POWERFAIL);
    m_CommandsQueue->Send(cmdPowerFailShutdown);

    return Q_NO_ERROR;
}

TQErrCode CMachineManager::WaitForEndOfPrinting()
{
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    TMachineState State = BackEnd->GetCurrentMachineState();
    if (State == msPrinting || State == msPrePrint || State == msPurge || State == msTestPattern)
    {
        BackEnd->EnableErrorMessageDuringStop(false);
        do
        {
            State = BackEnd->GetCurrentMachineState();
            Q2RTApplication->YieldMainThread();
        }
        while (!( State == msIdle || State == msStandby1 ));
        BackEnd->EnableErrorMessageDuringStop(true);
    }


    return Q_NO_ERROR;
}

TMachineState CMachineManager::PowerFailShutdownPhase()
{
    // Set the Shutdown wizard to run unattended, and launch it.
    m_ParamsMgr->SHD_RunUnattended = true;
    FrontEndInterface->UpdateStatus(FEOP_RUN_SHD_WIZARD, 1, true);

    return msIdle;
}

// Stop printing
TQErrCode CMachineManager::StopPrint(TQErrCode ErrCode)
{
    // In case received stop from host while local job - ignore it
	if ((m_JobOrigin == LocalJob) || (m_JobOrigin == PFFJob))
	{
        if((ErrCode == Q2RT_HOST_TCP_ERROR) || (ErrCode == Q2RT_HOST_RESPONSE))
		{
            CQLog::Write(LOG_TAG_PRINT,"Receive stop print from host during local job - ignoring (StopReason = %d)",ErrCode);
            return Q_NO_ERROR;
		}
	}
	else
	{
		RevertHeadsAmbientTemp();
		RevertRollerSpeed();
	}

    FrontEndInterface->InformStopToCartridgeDlg();
    // We can stop the print from one of the following states:
    switch(m_CurrentState)
    {
    case msPrinting:
    case msPaused:

        // Currently printing
		m_CurrentState = msStopping;

        CQLog::Write(LOG_TAG_PRINT,"Print stop reason = %d",ErrCode);
		m_MachineSequencer->MarkStateChange(msStopping);
		m_CommandsQueue->Send(cmdStop);
        Q2RTApplication->GetLayerProcess()->StopProcessing();
		break;

    case msPrePrint:
        // Currently in pre-printing
        m_CurrentState = msStopped;

        CQLog::Write(LOG_TAG_PRINT,"Pre-Print stop reason = %d",ErrCode);
        m_MachineSequencer->MarkStateChange(msStopped);

        try
        {
            // Execute stopped sequence, continue to idle if finished normally
            m_MachineSequencer->CancelPrePrintSequence();
        }
        catch(EQException& err)
        {
            QMonitor.ErrorMessage(err.GetErrorMsg());
        }

        m_CommandsQueue->Send(cmdStop);
        InformStop(ErrCode);
		break;

    case msStandby1:
    case msStandby2:
        CQLog::Write(LOG_TAG_PRINT,"Receive stop print during standby, StopReason=%d",ErrCode);
        FrontEndInterface->EnableDisableControl(FE_STOP,false);
        InformStop(ErrCode);

        // Free memory of pending Job
        if(m_CurrentJob != NULL)
            m_CurrentJob = NULL;

        return Q_NO_ERROR;

    case msTestPattern:
    case msPurge:
    case msFireAll:
        FrontEndInterface->EnableDisableControl(FE_STOP,false);
        m_MachineSequencer->StopSequence(m_CurrentState);
        return Q_NO_ERROR;

    case msIdle:
    default :
        InformStop(ErrCode);
        // Free memory of pending Job
        if(m_CurrentJob != NULL)
            m_CurrentJob = NULL;

        break;
	}

    // Marking there was a Stop (+ reason of stop)
    m_PrintingWasStopped = true;
    m_PrintErrCode       = ErrCode;

    // Set the machine in offline state
    SetOnlineOffline(false);

    FrontEndInterface->EnableDisableControl(FE_PAUSE,false);
    FrontEndInterface->EnableDisableControl(FE_STOP,false);
    FrontEndInterface->EnableDisableControl(FE_RESUME_VISIBLE,false);


    // Disable the print counters
    EnableDisablePrintTimeCounters(false);

    return Q_NO_ERROR;
}

// Stop printing
TQErrCode CMachineManager::StopPrintCommunication(TQErrCode ErrCode)
{
    InformStop(ErrCode);
    // Set the machine in offline state
    SetOnlineOffline(false);

    FrontEndInterface->EnableDisableControl(FE_PAUSE,false);
    FrontEndInterface->EnableDisableControl(FE_STOP,false);

    // Disable the print counters
    EnableDisablePrintTimeCounters(false);

    return Q_NO_ERROR;
}

// Manager kill sequencer
TQErrCode CMachineManager::KillSequencer(TQErrCode ErrCode)
{
    StopPrintCommunication(ErrCode);
    m_MachineSequencer->KillSequencer();
    ModesManagerHandlingOnStop();
    m_CurrentJob = NULL;
    return Q_NO_ERROR;
}

// State machine phase functions
// *********************************************************************************

TMachineState CMachineManager::PowerUpPhase(void)
{
    // Execute power up sequence
    m_MachineSequencer->PowerUpSequence();

    return msIdle;
}

TMachineState CMachineManager::IdlePhase(void)
{
    TMachineManagerCommands Cmd;

    FrontEndInterface->EnableDisableControl(FE_STOP,false);

    try
    {
		m_MachineSequencer->IdleSequence();
    }
    catch(EQException& Err)
    {
        HandleError(Err);
        return msTerminate;
    }

    for(;;)
    {
        // Get the new command
        if(m_CommandsQueue->Receive(Cmd) == QLib::wrReleased)
            return msTerminate;

        switch (Cmd)
        {
        case cmdStart:
            return msPrePrint;
        case cmdTestPattern:
            return msTestPattern;
        case cmdPurge:
            return msPurge;
		case cmdFireAll:
            return msFireAll;
        case cmdGoToPurge:
            return msGoToPurge;
        case cmdWipe:
            return msWipe;
        case cmdStandby1:
            return msStandby1;
        case cmdStandby2:
            return msStandby2;
        case cmdRemovalTray:
            return msRemovalTray;
        case cmdLowerTray:
            return msLowerTray;
        case cmdHeadInspection:
            return msHeadInspection;
        case cmdPowerFailShutdown:
            return msPowerFailShutdown;
#ifdef ALLOW_OPENING_MRW_FROM_HOST
        case cmdPendingHostRR:
            StartResinReplacementFromHost();
#endif
        default :
            break;
        }
    }
}



TMachineState CMachineManager::PrePrintPhase()
{
    TMachineState ret = msPrinting;
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    // Resetting flags
    m_PrintingWasStopped = false;
    m_PrintCompleted     = false;
	CQLog::Write(LOG_TAG_PRINT,"HeadAmbientTemperatureHighLimit before change mode =  %d",m_ParamsMgr->HeadAmbientTemperatureHighLimit.Value());

    // Entering mode according to the print resolution
	SetNewSystemMode(m_QualityMode,m_OperationMode);
	CQLog::Write(LOG_TAG_PRINT,"HeadAmbientTemperatureHighLimit after change mode =  %d",m_ParamsMgr->HeadAmbientTemperatureHighLimit.Value());
	SetHeadsAmbientTempAccordingToFlexibleParams(m_flexibleParameters);
	SetRollerSpeedAccordingToFlexibleParams(m_flexibleParameters);
	// Informing the FrontEnd of print from the host ...
    int Tag;
    MODE_TO_TAG(Tag, m_QualityMode, m_OperationMode);
    FrontEndInterface->UpdateStatus(FE_NEW_JOB, Tag, true);

    Q2RTApplication->GetLayerProcess()->SetNewPrintJob(m_CurrentJob);

    // Set user interface for print mode
    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,false);
    FrontEndInterface->EnableDisableControl(FE_STOP,true);
    FrontEndInterface->EnableDisableControl(FE_RESUME_VISIBLE,false);
	BackEnd->ResetBumperImpactCounter();
    try
	{
		BackEnd->SetMaintenanceCounter(CHANNEL_SLEEPING_MAX_TIME_COUNTER_ID,FindMaxSleepingChannelTime());

		if ((m_ParamsMgr->MRW_EnableSleepChannelAlerts) && (!m_IsWizardRunning))
		{
			if(RunMRWAfterSleepingChannelsAlert()) //If MRW is to be executed the pre-print is stopped
			{
				throw EQException(Q2RT_STOP_DURING_PRE_PRINT);
			}
		}

		if(m_ParamsMgr->HCW_EnableAlertMsgs)
        {
            //Need to run HCW, can't continue to printing phase
			if(HCWMessagesAlertCheck())
                throw EQException(Q2RT_STOP_DURING_PRE_PRINT);
        }
        // Log 'Start print' parameters
        CQLog::Write(LOG_TAG_PRINT,"Creating Print Start Params.cfg");
        m_ParamsMgr->LogParameters(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "Print Start Params.cfg");

        // Execute pre-print sequence, if canceled go back to idle state
        m_MachineSequencer->PrePrintSequence(m_CurrentJob->IsContinue()); //itamarDeleteJob
    }
    catch(EQException& err)
    {
        try
        {
            TQErrCode Err = err.GetErrorCode();
            CQLog::Write(LOG_TAG_GENERAL,"Pre Print error:%s (%d)" ,err.GetErrorMsg().c_str(), Err);
			FrontEndInterface->EnableDisableControl(FE_PAUSE,false);
            FrontEndInterface->EnableDisableControl(FE_STOP,false);
            FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);

            if (Err == Q2RT_STOP_DURING_PRE_PRINT ||
                    Err == Q2RT_SEQUENCE_CANCELED)
            {
                m_CurrentJob = NULL;
                StopPrintCommunication(Q2RT_EMBEDDED_RESPONSE);
                ModesManagerHandlingOnStop();
                ret = StoppedPrePrintPhase();
            }
			else
			{
                FrontEndInterface->ErrorMessage(QFormatStr("PrePrint error:%s",err.GetErrorMsg().c_str()));
                m_MachineSequencer->ActivateSignalTower(ST_LIGHT_ON, ST_LIGHT_OFF, ST_LIGHT_NC);
				KillSequencer(Err);
                ret = msIdle;
            }
			m_MachineSequencer->PrePrintSequenceCleanUp();

			if(m_RunMRW)
				FrontEndInterface->UpdateStatus(FEOP_RUN_MRW_WIZARD);
			else if(m_runHCW)
                FrontEndInterface->UpdateStatus(FE_HEAD_CLEANING_WIZARD);
        }
        catch(EQException& err)
        {
            QMonitor.ErrorMessage(err.GetErrorMsg());
            ret = msIdle;
        }
    }
    if(m_PrintingWasStopped) //ItamarDeleteJob
        ret = m_CurrentState;
    return ret;
}

TMachineState CMachineManager::PrintingPhase()
{


    TMachineState ret = msStopping;
	TQErrCode Err;
    // Set user interface for print mode
    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,false);
	FrontEndInterface->EnableDisableControl(FE_PAUSE,true);
    FrontEndInterface->EnableDisableControl(FE_RESUME_VISIBLE,false);
    try
    {
        try
        {
            //OBJET_MACHINE feature
            m_batchNoTable->Init(true,QDateTimeToStr(QGetCurrentTime()),m_CurrentJob->GetJobName(),OperationModeToStr(m_OperationMode));
            m_MachineSequencer->GetContainerInstance()->SetActiveTanksBatchNo();
            CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: SetIsPrinting(true)");

            Err = m_MachineSequencer->PrintingSequence();
            if(Err == Q2RT_PRINT_COMPLETED)
            {
                m_PrintCompleted = true;
                Err = Q_NO_ERROR;
            }
        }
        catch(EHaspInterfaceException& err)
        {
            CQLog::Write(LOG_TAG_GENERAL,"Printing error:%s",err.GetErrorMsg().c_str());
            FrontEndInterface->ErrorMessage(err.GetErrorMsg());
            FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);

            StopPrintCommunication(Q2RT_HASP_LICENSE_NOT_VALID);
            ModesManagerHandlingOnStop();
            m_CurrentJob = NULL;

            FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE);
			FrontEndInterface->UpdateStatus(FE_NEW_JOB,FE_UI_NO_MODE);

            ret = msStopping;
        }
        catch(EQException& err)
        {
            //Luda - for QA testing purposes ONLY
            if(FindWindow(0, "ThrowExceptionUnExpected.txt - Notepad") && Q2RTApplication->GetQATestsAllowed() )
                throw "for QA purpose un-expected exception" ; //un-expected

            else if (FindWindow(0, "ThrowExceptionExpected.txt - Notepad") && Q2RTApplication->GetQATestsAllowed() )
                throw EMachineManager("for QA purpose expected exception");  //expected
            try
            {
                Err = err.GetErrorCode();
                // If it's a non-fatal error, try to do normal stop
                if (Err == Q2RT_SEQUENCE_PAUSED)
                {
                    // Disable the print counters
                    EnableDisablePrintTimeCounters(false);
                    ret = msPausing;
                }
                else
                {
                    CQLog::Write(LOG_TAG_GENERAL,"Printing error:%s",err.GetErrorMsg().c_str());
                    FrontEndInterface->ErrorMessage(err.GetErrorMsg());
                    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);
					if (Err != Q2RT_CARTRIDGE_ERR_DLG_STOP)
						KillSequencer(Err);
					FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_IDLE_MODE);
                    FrontEndInterface->UpdateStatus(FE_NEW_JOB,FE_UI_NO_MODE);
                    if(Err == Q2RT_FATAL_ERROR)   // Fatal Error - Go to Idle state
                        ret = msIdle;
                    else
                    {
                        m_MachineSequencer->StopErrorDevice(Err);
                        ret = msStopping;
                    }
                }
            }
            catch(EQException& err)
            {
                QMonitor.ErrorMessage(err.GetErrorMsg());
            }
        }
    }
    __finally
    {
        // Log 'End print' parameters
        CQLog::Write(LOG_TAG_PRINT,"Creating Print End Params.cfg");
        m_ParamsMgr->LogParameters(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "Print End Params.cfg");
        // Disable the print counters
        EnableDisablePrintTimeCounters(false);

//OBJET_MACHINE feature
        CBackEndInterface* BackEnd = CBackEndInterface::Instance();
        int TotalPrintingHours = BackEnd->GetMaintenanceCounterElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / SECONDS_IN_HOUR;
        CQLog::Write(LOG_TAG_PRINT,"Total printing time: %d hours since %s",TotalPrintingHours,m_ParamsMgr->MachineSinceDate.c_str());

        m_batchNoTable->SetPrintEnd(QDateTimeToStr(QGetCurrentTime()));
        CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: SetIsPrinting(false)");

    }

    // Check if we got a new command
    if(m_CommandsQueue->ItemsCount > 0)
    {
        TMachineManagerCommands Cmd;

        if(m_CommandsQueue->Receive(Cmd) != QLib::wrReleased)
            // If it is pause, continue to pausing state
            if(Cmd == cmdPause)
                ret = msPausing;
    }

    return ret;
}

TMachineState CMachineManager::StoppingPhase()
{
    try
    {
		// Blink Green light on a Signal Tower if printing was completed
		if( m_PrintCompleted == true )
		{
            m_MachineSequencer->ActivateSignalTower(ST_LIGHT_NC, ST_LIGHT_BLINK, ST_LIGHT_NC);
        }
        else  //printing was not completed
        {
            m_MachineSequencer->ActivateSignalTower(ST_LIGHT_NC, ST_LIGHT_OFF, ST_LIGHT_NC);
        }
		RevertHeadsAmbientTemp();
		RevertRollerSpeed();
        // Execute stopping sequence, if stopped canceled go back to idle state (this can be bad)
        if(m_MachineSequencer->GetLastError() == Q_NO_ERROR)
        {
            if(m_MachineSequencer->StoppingSequence() == Q2RT_FATAL_ERROR)
                throw EMachineManager("StoppingPhase: Fatal Error on StoppingSequence");
        }
        else
        {
            m_MachineSequencer->ActivateSignalTower(ST_LIGHT_ON, ST_LIGHT_NC, ST_LIGHT_NC);
            ModesManagerHandlingOnStop();
            if (m_MachineSequencer->ErrorStoppingSequence() == Q2RT_FATAL_ERROR)
                throw EMachineManager("StoppingPhase: unexpected error");
        }

    }
    catch(EQException& err)
    {
        if (m_PrintingWasStopped)
            InformStop(m_PrintErrCode,false);

        if (m_PrintCompleted && (m_JobOrigin == JobFromHost))
			// Informing the host of Job Completion...
            Q2RTApplication->GetHostComm()->ReportEndOfModelPrint();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    return msStopped;
}

TMachineState CMachineManager::PausingPhase()
{
    TQErrCode Err;
    FrontEndInterface->EnableDisableControl(FE_STOP, false);
    //If job was stopped before button disable return
    if (! m_CurrentJob)
        return m_CurrentState;

    try
    {
        Err = m_MachineSequencer->PausingSequence();
    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Execute pre-print sequence, if stopped canceled go back to idle state (this can be bad)
	if(Err == Q2RT_FATAL_ERROR)
        return msIdle;

    if(Err != Q_NO_ERROR)
        return msStopped;

    return msPaused;
}

TMachineState CMachineManager::PausedPhase()
{
    try
    {
        ShowMaterialWarning(SHOW_AFTER_PRINTING);
        m_MachineSequencer->PausedSequence();
    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    FrontEndInterface->EnableDisableControl(FE_STOP, true);
    if(! m_CurrentJob->IsPrintPausedByHost())
        FrontEndInterface->EnableDisableControl(FE_RESUME_VISIBLE,true);

    m_CurrentJob->UpdatePrintPausedByHost(false);

    TMachineManagerCommands Cmd;

    QLib::TQWaitResult WaitResult = m_CommandsQueue->Receive(Cmd,
                                    QMinutesToTicks(m_ParamsMgr->PausedTime_min.Value()));

    if(WaitResult == QLib::wrReleased)
        return msTerminate;

    //If PausedTime_min expired without any command accepted.
    if(WaitResult != QLib::wrSignaled)
    {
        try
        {
            // Turn head heaters OFF through a Machine Sequence:
            m_MachineSequencer->PausedTimeoutSequence();
        }
        catch(EQException& err)
        {
            QMonitor.ErrorMessage(err.GetErrorMsg());
            return msIdle;
        }
    }
    // A new command was accepted within timeout:
    else
    {
        switch(Cmd)
        {
        case cmdStart:
            return msPrinting;

		case cmdStop:
            return msStopped;
        }
    }

    // With headHeaters Off, wait for a new command forever:
    if(m_CommandsQueue->Receive(Cmd) == QLib::wrReleased)
        return msTerminate;

    switch(Cmd)
    {
    case cmdStart:
        return msPrinting;

    case cmdStop:
        return msStopped;
    }

    // This is an error
    return msIdle;
}

TMachineState CMachineManager::StoppedPhase()
{
    try
    {
        ShowMaterialWarning(SHOW_AFTER_PRINTING);
		if (m_PrintCompleted && (m_JobOrigin == JobFromHost))
            // Informing the host of Job Completion...
			Q2RTApplication->GetHostComm()->ReportEndOfModelPrint();
		else if (m_JobOrigin == PFFJob)
		{
        	m_pPFFSliceQueueThread->Cancel();	
		}

        if (m_PrintingWasStopped)
            InformStop(m_PrintErrCode,false);

        // Execute stopped sequence, continue to idle if finished normally
        if(m_MachineSequencer->StoppedSequence() == Q2RT_FATAL_ERROR)
        {
            ModesManagerHandlingOnStop();
            return msIdle;
        }

        ModesManagerHandlingOnStop();

    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    if(GetStandbyPhaseValue())
        return msStandby1;

    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
	return msIdle;
}

TMachineState CMachineManager::StoppedPrePrintPhase()
{
    try
    {
        m_MachineSequencer->ActivateSignalTower(ST_LIGHT_NC, ST_LIGHT_OFF, ST_LIGHT_NC);

        if(m_MachineSequencer->GetLastError() == Q_NO_ERROR)
        {
            if(m_MachineSequencer->StopDuringPrePrintSequence() == Q2RT_FATAL_ERROR)
                return msIdle;
        }
        else
        {
            if (m_MachineSequencer->PrePrintErrorStoppingSequence() == Q2RT_FATAL_ERROR)
                return msIdle;
        }

    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    if(GetStandbyPhaseValue())
        return msStandby1;

    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}


TMachineState CMachineManager::Standby1Phase()
{
    TMachineManagerCommands Cmd;

    FrontEndInterface->EnableDisableControl(FE_STOP,false);
    try
	{
		if (m_MachineSequencer->Standby1Sequence() != Q_NO_ERROR)
            return msIdle;
    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

#ifdef ALLOW_OPENING_MRW_FROM_HOST
    StartResinReplacementFromHost();
#endif

    for(;;)
    {
        // Get the new command
		QLib::TQWaitResult WaitResult = m_CommandsQueue->Receive(Cmd,
                                        QMinutesToTicks(m_ParamsMgr->Standby1Time_min.Value()));

        if(WaitResult == QLib::wrReleased)
            return msTerminate;

        if(WaitResult != QLib::wrSignaled)
            return msStandby2;

        // If start command, continue to pre-print mode
        switch (Cmd)
        {
        case cmdIdle:
        {
            // Go to idle state through the relavant standby states (standby1-> standby2 -> idle
            CQLog::Write(LOG_TAG_PRINT,"Going to idle through standby states");

            m_MachineSequencer->Standby2Sequence();
            m_LastState = msStandby2;
            m_MachineSequencer->Standby2ToIdleSequence();

            return msIdle;
        }

        case cmdStart:
            return msPrePrint;
        case cmdTestPattern:
            return msTestPattern;
        case cmdPurge:
			return msPurge;
        case cmdFireAll:
            return msFireAll;
        case cmdGoToPurge:
            return msGoToPurge;
        case cmdWipe:
            return msWipe;
        case cmdStandby2:
            return msStandby2;
        case cmdRemovalTray:
            return msRemovalTray;
        case cmdLowerTray:
            return msLowerTray;
        case cmdHeadInspection:
            return msHeadInspection;
        case cmdPowerFailShutdown:
            return msPowerFailShutdown;
#ifdef ALLOW_OPENING_MRW_FROM_HOST
        case cmdPendingHostRR:
            StartResinReplacementFromHost();
#endif
        default :
            break;
        }
    }
}

TMachineState CMachineManager::Standby2Phase()
{
	TMachineManagerCommands Cmd;

    try
    {
        if (m_MachineSequencer->Standby2Sequence() != Q_NO_ERROR)
        {
            m_MachineSequencer->Standby2ToIdleSequence();
            return msIdle;
        }

#ifdef ALLOW_OPENING_MRW_FROM_HOST
        StartResinReplacementFromHost();
#endif

        for(;;)
        {
            // Get the new command
            QLib::TQWaitResult WaitResult = m_CommandsQueue->Receive(Cmd,
                                            QMinutesToTicks(m_ParamsMgr->Standby2Time_min.Value()));

            if(WaitResult == QLib::wrReleased)
                return msTerminate;

            if(WaitResult != QLib::wrSignaled)
            {
                //it is no need to check status because anyway we will send it ot idle
                m_MachineSequencer->Standby2ToIdleSequence();
                return msIdle;
            }

            // If start command, continue to pre-print mode
            switch (Cmd)
            {
            case cmdStart:
                return msPrePrint;
            case cmdTestPattern:
                return msTestPattern;
            case cmdPurge:
                return msPurge;
            case cmdFireAll:
                return msFireAll;
            case cmdGoToPurge:
                return msGoToPurge;
            case cmdWipe:
                return msWipe;
            case cmdIdle:
            {
                // Go to idle state through the relavant standby states (standby2 -> idle)
                m_LastState = msStandby2;
                m_MachineSequencer->Standby2ToIdleSequence();
                return msIdle;
            }
            case  cmdRemovalTray:
                return msRemovalTray;
            case cmdLowerTray:
                return msLowerTray;
            case cmdHeadInspection:
                return msHeadInspection;
			case cmdPowerFailShutdown:
                return msPowerFailShutdown;
#ifdef ALLOW_OPENING_MRW_FROM_HOST
            case cmdPendingHostRR:
                StartResinReplacementFromHost();
#endif
            default :
                break; //continue on waiting
            }
        }
    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        m_MachineSequencer->Standby2ToIdleSequence();
        return msIdle;
    }
}


TMachineState CMachineManager::PurgePhase(void)
{
    TQErrCode Err;

    FrontEndInterface->EnableDisableControl(FE_STOP,true);
    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,false);
    try
    {
        // Close the door
		m_MachineSequencer->GetDoorInstance()->Enable();

        if((Err=m_MachineSequencer->PurgeSequence(PERFORM_HOME, m_PerformSinglePurgeInSequence )) != Q_NO_ERROR)
        {
            m_PerformSinglePurgeInSequence = true;

            if(Err==Q2RT_STOP_DURING_PURGE)
            {
                m_MachineSequencer->GetMotorsInstance()->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE);
                if (m_EnableErrorMessageDuringStop)
                    FrontEndInterface->NotificationMessage(PrintErrorMessage(Err).c_str());
                CQLog::Write(LOG_TAG_GENERAL,PrintErrorMessage(Err).c_str());
            }
            else
            {
                throw EMachineManager("Error during Purge");
            }
        }
        else
        {
            m_PerformSinglePurgeInSequence = true;

            m_MachineSequencer->GetMotorsInstance()->GoWaitHomeSequence(AXIS_Y);
            m_MachineSequencer->GetMotorsInstance()->GoWaitHomeSequence(AXIS_X);

            FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Filling Block");
            m_MachineSequencer->FillBlock();
            FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
        }
	}
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();
    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);
    if(GetStandbyPhaseValue())
        return msStandby1;

    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}


TMachineState CMachineManager::FireAllPhase(void)
{
    TQErrCode Err;

    FrontEndInterface->EnableDisableControl(FE_STOP,true);

    try
    {
        // Close the door
        m_MachineSequencer->GetDoorInstance()->Enable();

        if((Err = m_MachineSequencer->FireAllSequence()) != Q_NO_ERROR)
		{
            if(Err==Q2RT_STOP_DURING_FIRE_ALL)
            {
                m_MachineSequencer->GetMotorsInstance()->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE);
                m_MachineSequencer->FireAllSequenceCleanUp();
                if (m_EnableErrorMessageDuringStop)
                    FrontEndInterface->NotificationMessage(PrintErrorMessage(Err).c_str());
                CQLog::Write(LOG_TAG_GENERAL,PrintErrorMessage(Err).c_str());
            }
            else
                throw EMachineManager("Error during fire all");
        }

    }
    catch(EQException& err)
    {
        // Open the door
        m_MachineSequencer->GetDoorInstance()->Disable();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();
    if(GetStandbyPhaseValue())
        return ((m_LastState == msStandby2) ? msStandby1 : m_LastState);
    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

TMachineState CMachineManager::GotoHeadInspectionPositionPhase(void)
{
    try
    {
        if (m_MachineSequencer->GotoHeadInspectionPositionSequencer() != Q_NO_ERROR)
            throw EMachineManager("Error during Go to Head Inspection Position");

    }
    catch(EQException& err)
    {
        // Open the door
        m_MachineSequencer->GetDoorInstance()->Disable();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();
    if(GetStandbyPhaseValue())
        return ((m_LastState == msStandby2) ? msStandby1 : m_LastState);
    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

TMachineState CMachineManager::GotoTrayRemovalPositionPhase(void)
{
	try
    {
        if (m_MachineSequencer->GotoTrayRemovalPositionSequencer() != Q_NO_ERROR)
            throw EMachineManager("Error during Go to Tray Removal Position");

    }
    catch(EQException& err)
    {
        // Open the door
        m_MachineSequencer->GetDoorInstance()->Disable();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();
    if(GetStandbyPhaseValue())
        return ((m_LastState == msStandby2) ? msStandby1 : m_LastState);
    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

TMachineState CMachineManager::GotoTrayLowerPositionPhase(void)
{
    try
    {


		if (m_MachineSequencer->GotoTrayLowerPositionSequencer() != Q_NO_ERROR)
            throw EMachineManager("Error during Go to Tray Lower Position");

    }
    catch(EQException& err)
    {
        // Open the door
        m_MachineSequencer->GetDoorInstance()->Disable();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();
    if(GetStandbyPhaseValue())
        return ((m_LastState == msStandby2) ? msStandby1 : m_LastState);
    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

TMachineState CMachineManager::GoToPurgePositionPhase(void)
{
    try
    {
        // Close the door
        m_MachineSequencer->GetDoorInstance()->Enable();

        if (m_MachineSequencer->GoToPurge(PERFORM_HOME,true) != Q_NO_ERROR)
			throw EMachineManager("Error during go to purge position");

    }
    catch(EQException& err)
    {
        // Open the door
        m_MachineSequencer->GetDoorInstance()->Disable();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();
    if(GetStandbyPhaseValue())
        return ((m_LastState == msStandby2) ? msStandby1 : m_LastState);
    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

TMachineState CMachineManager::WipePhase(void)
{
    try
    {
        // Close the door
        m_MachineSequencer->GetDoorInstance()->Enable();

        if (m_MachineSequencer->PerformWipe(true, true) != Q_NO_ERROR)
            throw EMachineManager("Error during wipe");

    }
    catch(EQException& err)
    {
        // Open the door
        m_MachineSequencer->GetDoorInstance()->Disable();

        QMonitor.ErrorMessage(err.GetErrorMsg());
        return msIdle;
    }

    // Open the door
    m_MachineSequencer->GetDoorInstance()->Disable();

    if(GetStandbyPhaseValue())
        return ((m_LastState == msStandby2) ? msStandby1 : m_LastState);
    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

TMachineState CMachineManager::TestPatternPhase(void)
{
    TQErrCode Err;
    // Set user interface for print mode
    FrontEndInterface->EnableDisableControl(FE_STOP,true);
    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,false);
    CModesManager *ModesManager = CModesManager::Instance();
    if (ModesManager)
        ModesManager->GotoDefaultMode();

	TTestPatternMessage Msg;
	
    try
    {
        // Get the new command
		QLib::TQWaitResult WaitResult = m_TestPatternQueue->Receive(Msg, QMsToTicks(TIME_TO_GET_TEST_NUMBER_MS));

		if(WaitResult != QLib::wrSignaled)
			throw EMachineManager("Test pattern: error getting parameters");
			
        if((Err = m_MachineSequencer->TestPattern(Msg.AskToPrintInCurrentPosition, Msg.MoveAxisZInit)) != Q_NO_ERROR)
        { 
			if(Err == Q2RT_STOP_DURING_TESTPATTERN)
            {
                m_MachineSequencer->GetMotorsInstance()->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE);
                if (m_EnableErrorMessageDuringStop)
					FrontEndInterface->NotificationMessage(PrintErrorMessage(Err).c_str());
                CQLog::Write(LOG_TAG_GENERAL,PrintErrorMessage(Err).c_str());
            }
			else
			{
			    FrontEndInterface->EnableDisableControl(FE_STOP,false);
				FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);
				throw EMachineManager("Error during test pattern: " + PrintErrorMessageWithoutErrID(Err));
			}
		}
	}
    catch(EQException& err)
	{
		QMonitor.ErrorMessage(err.GetErrorMsg());

		if(GetStandbyPhaseValue())
			return msStandby1;

        CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
        return msIdle;
	}

    FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);
//OBJET_MACHINE feature
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	BackEnd->SetMachineActionsDate("Pattern Test");

    // Goto Standby1 Phase after TestPattern
    if(GetStandbyPhaseValue())
        return msStandby1;

    CQLog::Write(LOG_TAG_GENERAL,"Move to Idle State");
    return msIdle;
}

//------------------------------------------------------
TQErrCode CMachineManager::TestPattern(bool AskToPrintInCurrentPosition, bool MoveAxisZInit)
{
    TTestPatternMessage Msg = {AskToPrintInCurrentPosition,MoveAxisZInit};
    // We can perform monitor test pattern under these commands
    if((m_CurrentState == msIdle) ||
			(m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState = m_CurrentState;
        m_CurrentState = msTestPattern;
        m_TestPatternQueue->Release();
        m_TestPatternQueue->Send(Msg);
        m_CommandsQueue->Send(cmdTestPattern);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not perform Test Pattern from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::Wipe()
{
    // We can perform monitor wipe under these commands
    if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState = m_CurrentState;
        m_CurrentState = msWipe;
        m_CommandsQueue->Send(cmdWipe);
        return Q_NO_ERROR;
	}

    throw EMachineManager("Can not Wipe from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::Purge(bool PerformSinglePurgeInSequence)
{
    m_PerformSinglePurgeInSequence = PerformSinglePurgeInSequence;
    // We can perform monitor test under these commands
    if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState = m_CurrentState;
        m_CurrentState = msPurge;
        m_CommandsQueue->Send(cmdPurge);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not Purge from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::FireAll(void)
{
    // We can perform monitor fire all under these commands
    if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
			(m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState = m_CurrentState;
        m_CurrentState = msFireAll;
        m_CommandsQueue->Send(cmdFireAll);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not Fire All from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::DoFireAll(void)
{
    // We can perform monitor fire all under these commands
    if(!((m_CurrentState == msIdle)     ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped) ))
        throw EMachineManager("Can not Fire All from current state (" + StatesLookup[m_CurrentState] + ")");

    return m_MachineSequencer->FireAll();
}


TQErrCode CMachineManager::GoToPurgePosition(void)
{
    // We can perform monitor fire all under these commands
	if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState = m_CurrentState;
        m_CurrentState = msGoToPurge;
        m_CommandsQueue->Send(cmdGoToPurge);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not GoToPurge from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::GotoTrayRemovalPosition(void)
{
    // We can perform monitor go to tray removal position
    if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState    = m_CurrentState;
        m_CurrentState = msRemovalTray;
        m_CommandsQueue->Send(cmdRemovalTray);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not perform Removal Tray commands from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::GotoTrayLowerPosition(void)
{
    // We can perform monitor go to tray removal position
    if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
            (m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState    = m_CurrentState;
        m_CurrentState = msLowerTray;
        m_CommandsQueue->Send(cmdLowerTray);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not perform removal lower Tray commands from current state (" + StatesLookup[m_CurrentState] + ")");
}


TQErrCode CMachineManager::GotoHeadInspectionPosition(void)
{
    // We can perform monitor go to tray removal position
    if((m_CurrentState == msIdle) ||
            (m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2) ||
			(m_CurrentState == msStopped))
    {
        //Mark and send command
        m_LastState    = m_CurrentState;
        m_CurrentState = msHeadInspection;
        m_CommandsQueue->Send(cmdHeadInspection);
        return Q_NO_ERROR;
    }

    throw EMachineManager("Can not perform Head Inspection commands from current state (" + StatesLookup[m_CurrentState] + ")");
}

TQErrCode CMachineManager::EnterStandbyState(void)
{
    if(m_CurrentState != msIdle)
        throw EMachineManager("standby state can be manualy entry only from idle state");

    //Mark and send command
    m_LastState = m_CurrentState;
    m_CurrentState = msStandby1;
    m_CommandsQueue->Send(cmdStandby1);

    return Q_NO_ERROR;
}

TQErrCode CMachineManager::ExitStandbyState(void)
{
    if((m_CurrentState == msStandby1) ||
            (m_CurrentState == msStandby2))
	{
        //Mark and send command
        m_LastState = m_CurrentState;
        m_CommandsQueue->Send(cmdIdle);
        return Q_NO_ERROR;
    }

    throw EMachineManager("standby state can be exited manually only from standby state");
}

void CMachineManager::Start(void)
{
    m_MachineSequencer->StartControlThreads();
    Resume();
}

// Handle an error (response to qexception)
void CMachineManager::HandleError(EQException Err)
{
    // If the error occured while terminating, don't show error message
    if(!Terminated)
        QMonitor.ErrorMessage(Err.GetErrorMsg());
}

// Observer function for the error handler
void CMachineManager::ErrorHandlerObserver(TErrorInfoBlock& ErrorInfo,TGenericCockie Cockie)
{
    try
    {
		// Get a pointer to the machine manager instance
        CMachineManager *InstancePtr = reinterpret_cast<CMachineManager *>(Cockie);
        TQErrCode Err = ErrorInfo.GetErrorCode();
        QString   ErrDescription = "";


        switch(Err)
        {
        case Q2RT_BUMPER_IMPACT:
			CQLog::Write(LOG_TAG_PRINT,"ERROR HANDLER: %s",ErrorInfo.GetErrorMsg().c_str());
            InstancePtr->m_MachineSequencer->HandleBumperImpactError();
            break;

        case Q2RT_HEAD_HEATER_VOLTAGE_DROP:
        case Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT :
        case Q2RT_HEADS_FILLING_TIMEOUT :
        case Q2RT_HEADS_FILLING_MONITORING_ERROR :
        case Q2RT_HEADS_FILLING_THERMISTOR_OPEN:
        case Q2RT_HEADS_FILLING_THERMISTOR_SHORT:
        case Q2RT_CONTAINER_FILLING_SHORT:
        case Q2RT_CONTAINER_FILLING_OPEN:
        case Q2RT_UV_LAMP_IGNITION_TIMEOUT:
        case Q2RT_UV_LAMP_RIGHT_LAMP_ERROR:
        case Q2RT_UV_LAMP_LEFT_LAMP_ERROR:
        case Q2RT_TRAY_HEATER_ERROR:
        case Q2RT_TRAY_HEATER_THERMISTOR_OPEN:
        case Q2RT_TRAY_HEATER_THERMISTOR_SHORT:
        case Q2RT_TRAY_HEATER_THERMISTOR_OUT_OF_RANGE:
        case Q2RT_HEAD_VACUUM_OUT_OF_RANGE:
		case Q2RT_HEAD_VACUUM_AVERAGE_OUT_OF_RANGE:
        case Q2RT_AMBIENT_ERROR_TEMPERATURE_COLD:
        case Q2RT_AMBIENT_ERROR_TEMPERATURE_HOT:
            CQLog::Write(LOG_TAG_PRINT,"ERROR HANDLER:%s Error =%d  Data=%d %d",
                         ErrorInfo.GetErrorMsg().c_str(),
                         Err,ErrorInfo.GetParam1(),ErrorInfo.GetParam2());
            FrontEndInterface->ErrorMessage(QFormatStr("Error Handler:%s Data=%d %d",
                                            ErrorInfo.GetErrorMsg().c_str(),
                                            ErrorInfo.GetParam1(),ErrorInfo.GetParam2()));

            if(InstancePtr->m_CurrentState == msPrePrint ||
                    InstancePtr->m_CurrentState == msPrinting ||
                    InstancePtr->m_CurrentState == msPausing  ||
                    InstancePtr->m_CurrentState == msPaused   ||
                    InstancePtr->m_CurrentState == msStopping ||
                    InstancePtr->m_CurrentState == msStopped)
            {
                InstancePtr->m_MachineSequencer->UpdateLastError(Err);
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
                InstancePtr->StopPrint(Err);
            }
            else
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
            break;

        case Q2RT_EVAC_AIRFLOW_SPEED_CRITICAL:
        case Q2RT_EVAC_AIRFLOW_SPEED_WARNING:
        {
            FrontEndInterface->ErrorMessage(QFormatStr("Error Handler: %s",
											ErrorInfo.GetErrorMsg().c_str()));
            TMachineState currState = InstancePtr->m_CurrentState;
            if (msPrePrint == currState || msPrinting == currState ||
                    msPausing  == currState || msPaused   == currState ||
                    msStopping == currState || msStopped  == currState)
            {
                InstancePtr->m_MachineSequencer->UpdateLastError(Err);
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
                if (Q2RT_EVAC_AIRFLOW_SPEED_CRITICAL == Err)
                    InstancePtr->StopPrint(Err);
            }
            else
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
        }
        break;

        case Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_HIGH :
        case Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_LOW :

            InstancePtr->m_MachineSequencer->GetHeadHeatersInstance()->GetHeatersStatus();
            InstancePtr->m_MachineSequencer->GetHeadHeatersInstance()->DisplayHeadStatus();
            ErrDescription = "Error Handler:" + ErrorInfo.GetErrorMsg() + " ";
            ErrDescription+= InstancePtr->m_MachineSequencer->GetHeadHeatersInstance()->GetHeaterErrorDescription();

            FrontEndInterface->ErrorMessage(ErrDescription);
            CQLog::Write(LOG_TAG_PRINT, ErrDescription);

            if(InstancePtr->m_CurrentState == msPrePrint ||
                    InstancePtr->m_CurrentState == msPrinting ||
					InstancePtr->m_CurrentState == msPausing  ||
                    InstancePtr->m_CurrentState == msPaused   ||
                    InstancePtr->m_CurrentState == msStopping ||
                    InstancePtr->m_CurrentState == msStopped)
            {
                InstancePtr->m_MachineSequencer->UpdateLastError(Err);
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
                InstancePtr->StopPrint(Err);
            }
            else
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
            break;


        case Q2RT_ROLLER_FAILURE_ERROR:
            if(InstancePtr->m_CurrentState == msPrinting ||
                    InstancePtr->m_CurrentState == msPausing  ||
                    InstancePtr->m_CurrentState == msPaused)
            {
                if(InstancePtr->m_MachineSequencer->GetCurrentModelHeight() >= InstancePtr->m_ParamsMgr->MinModelHeightForRollerCheck)
                {
                    CQLog::Write(LOG_TAG_PRINT,"ERROR HANDLER:%s Error =%d  Data=%d %d",
                                 ErrorInfo.GetErrorMsg().c_str(),
                                 Err,ErrorInfo.GetParam1(),ErrorInfo.GetParam2());
                    FrontEndInterface->ErrorMessage(QFormatStr("Error Handler:%s Data=%d %d",
                                                    ErrorInfo.GetErrorMsg().c_str(),
                                                    ErrorInfo.GetParam1(),ErrorInfo.GetParam2()));

                    InstancePtr->m_MachineSequencer->UpdateLastError(Err);
					InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
                    InstancePtr->StopPrint(Err);
                }
            }
            else if (InstancePtr->m_CurrentState == msIdle     ||
                     InstancePtr->m_CurrentState == msStandby1 ||
                     InstancePtr->m_CurrentState == msStandby2)

            {
                CQLog::Write(LOG_TAG_PRINT,"ERROR HANDLER:%s Error =%d  Data=%d %d",
                             ErrorInfo.GetErrorMsg().c_str(),
                             Err,ErrorInfo.GetParam1(),ErrorInfo.GetParam2());
                FrontEndInterface->ErrorMessage(QFormatStr("Error Handler:%s Data=%d %d",
                                                ErrorInfo.GetErrorMsg().c_str(),
                                                ErrorInfo.GetParam1(),ErrorInfo.GetParam2()));

                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
            }
            break;

        case Q2RT_INVALID_HEATING_RATE:
		case Q2RT_HEAD_HEATER_THERMISTOR_SHORT:
		case Q2RT_HEAD_HEATER_THERMISTOR_OPEN:
		case Q2RT_HEAD_MAIN_FAN_SPEED_CRITICAL:
            CQLog::Write(LOG_TAG_PRINT,"ERROR HANDLER: %s Error =%d  Data=%d %d",
                         ErrorInfo.GetErrorMsg().c_str(),
                         Err,ErrorInfo.GetParam1(),ErrorInfo.GetParam2());
            FrontEndInterface->ErrorMessage(QFormatStr("Error Handler: %s",
                                            ErrorInfo.GetErrorMsg().c_str()));

            if(InstancePtr->m_CurrentState == msPrePrint ||
                    InstancePtr->m_CurrentState == msPrinting ||
                    InstancePtr->m_CurrentState == msPausing  ||
                    InstancePtr->m_CurrentState == msPaused   ||
                    InstancePtr->m_CurrentState == msStopping ||
                    InstancePtr->m_CurrentState == msStopped)
            {
                InstancePtr->m_MachineSequencer->UpdateLastError(Err);
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
                InstancePtr->m_MachineSequencer->CancelHeadHeating();
                InstancePtr->StopPrint(Err);
                InstancePtr->DisableStandbyPhase();
            }
            else
                InstancePtr->m_MachineSequencer->StopErrorDevice(Err);
            break;

        case Q2RT_LIQUID_TANKS_DISABLED_ERROR:
            switch(InstancePtr->m_CurrentState)
            {
            case msPrePrint:
            case msPrinting:
            case msStopping:
            case msPausing:
                //This machine states are handled by prining mechanism
                break;

            case msPurge:
				InstancePtr->m_MachineSequencer->GetHeadFillingInstance()->AllowHeadFilling(false);
                InstancePtr->m_MachineSequencer->CancelHeadHeating();
                InstancePtr->StopPrint(Err);
                FrontEndInterface->EnableDisableControl(FE_ONLINE_ENABLE,true);
                break;

            default:
                InstancePtr->m_MachineSequencer->GetHeadFillingInstance()->AllowHeadFilling(false);
            }
            break;

        default:
            CQLog::Write(LOG_TAG_PRINT,"ERROR HANDLER:%s Error=%d  Data1=%d %d",
                         ErrorInfo.GetErrorMsg().c_str(),
                         Err,ErrorInfo.GetParam1(),ErrorInfo.GetParam2() );
            FrontEndInterface->ErrorMessage(QFormatStr("Error Handler:%s Data=%d %d",
                                            ErrorInfo.GetErrorMsg().c_str(),
                                            ErrorInfo.GetParam1(),ErrorInfo.GetParam2()));
            break;
        }
    }
    catch (EQException& ExceptionErr)
    {
        CQLog::Write(LOG_TAG_PRINT,"Error during exception handle (%d:%s)",
                     ExceptionErr.GetErrorCode(),ErrorInfo.GetErrorMsg().c_str());
        FrontEndInterface->ErrorMessage(QFormatStr("Error during exception handle (%d:%s)",
                                        ExceptionErr.GetErrorCode(),ErrorInfo.GetErrorMsg().c_str()));
    }
}

// Load a local file print job (return a pointer to the file print job object)
TQErrCode CMachineManager::LoadLocalPrintJob(QString ModelFileName,QString SupportFileName,
        int BitmapResolution,QString ModeStr,int SliceNum)
{
    int QualityMode,OperationMode;
    if (GetModeFromShortStr(ModeStr, QualityMode, OperationMode) == false)
        throw EMachineManager("LoadLocalPrintJob::Undefined mode");

    TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    memset(FileNames, 0, TFileNamesArraySIZE);
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, SupportFileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , ModelFileName.c_str());
    TQErrCode ret = LoadLocalPrintJob(FileNames, BitmapResolution, QualityMode, OperationMode, SliceNum);
    free (FileNames);
    return ret;
}

TQErrCode CMachineManager::LoadLocalDoublePrintJob(QString FirstModelFileName, QString FirstSupportFileName,
        QString SecondModelFileName,QString SecondSupportFileName,
        int BitmapResolution,QString ModeStr,int FirstSliceNum,int SecondSliceNum)
{
    int QualityMode,OperationMode;
    if (GetModeFromShortStr(ModeStr, QualityMode, OperationMode) == false)
        throw EMachineManager("LoadLocalPrintJob::Undefined mode");

    TFileNamesArray FileNames1 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    TFileNamesArray FileNames2 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    memset(FileNames1, 0, TFileNamesArraySIZE);
	memset(FileNames2, 0, TFileNamesArraySIZE);
    strcpy((TBMPFileName)FileNames1 + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, FirstSupportFileName.c_str());
    strcpy((TBMPFileName)FileNames1 + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , FirstModelFileName.c_str());
    strcpy((TBMPFileName)FileNames2 + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, SecondSupportFileName.c_str());
    strcpy((TBMPFileName)FileNames2 + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , SecondModelFileName.c_str());
    TQErrCode ret = LoadLocalDoublePrintJob(FileNames1, FileNames2, BitmapResolution, QualityMode, OperationMode, FirstSliceNum, SecondSliceNum);
    free (FileNames1);
    free (FileNames2);
    return ret;
}

//OBJET_MACHINE config//check!!! Is this DM with two materials (add for 3 materials)
// Load a local file print job (return a pointer to the file print job object)
TQErrCode CMachineManager::Load3LocalDoublePrintJob(QString FirstModelFileName, QString FirstSupportFileName,QString FirstModel2FileName,
        QString SecondModelFileName,QString SecondSupportFileName,QString SecondModel2FileName,
        int BitmapResolution,QString ModeStr,int FirstSliceNum,int SecondSliceNum)
{
    int QualityMode,OperationMode;
    if (GetModeFromShortStr(ModeStr, QualityMode, OperationMode) == false)
        throw EMachineManager("LoadLocalPrintJob::Undefined mode");

    TFileNamesArray FileNames1 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    TFileNamesArray FileNames2 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    memset(FileNames1, 0, TFileNamesArraySIZE);
    memset(FileNames2, 0, TFileNamesArraySIZE);
    strcpy((TBMPFileName)FileNames1 + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, FirstSupportFileName.c_str());
    strcpy((TBMPFileName)FileNames1 + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , FirstModelFileName.c_str());
    strcpy((TBMPFileName)FileNames1 + MAX_FILE_PATH*TYPE_CHAMBER_MODEL2 , FirstModel2FileName.c_str());
    strcpy((TBMPFileName)FileNames2 + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, SecondSupportFileName.c_str());
	strcpy((TBMPFileName)FileNames2 + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , SecondModelFileName.c_str());
    strcpy((TBMPFileName)FileNames2 + MAX_FILE_PATH*TYPE_CHAMBER_MODEL2 , SecondModel2FileName.c_str());
    TQErrCode ret = LoadLocalDoublePrintJob(FileNames1, FileNames2, BitmapResolution, QualityMode, OperationMode, FirstSliceNum, SecondSliceNum);
    free (FileNames1);
    free (FileNames2);
    return ret;
}

TQErrCode CMachineManager::Load3LocalPrintJob(QString ModelFileName,QString SupportFileName,QString Model2FileName,
        int BitmapResolution,QString ModeStr,int SliceNum)
{
    int QualityMode,OperationMode;
    if (GetModeFromShortStr(ModeStr, QualityMode, OperationMode) == false)
        throw EMachineManager("LoadLocalPrintJob::Undefined mode");

    TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    memset(FileNames, 0, TFileNamesArraySIZE);
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, SupportFileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , ModelFileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL2 , Model2FileName.c_str());
    TQErrCode ret = LoadLocalPrintJob(FileNames, BitmapResolution, QualityMode, OperationMode, SliceNum);
    free (FileNames);
    return ret;
}

//OBJET_MACHINE config
TQErrCode CMachineManager::Load4LocalPrintJob(QString ModelFileName,QString SupportFileName,QString Model2FileName,
        QString Model3FileName,int BitmapResolution,QString ModeStr,int SliceNum)
{
	int QualityMode,OperationMode;
    if (GetModeFromShortStr(ModeStr, QualityMode, OperationMode) == false)
        throw EMachineManager("LoadLocalPrintJob::Undefined mode");

    TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    memset(FileNames, 0, TFileNamesArraySIZE);
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, SupportFileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , ModelFileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL2 , Model2FileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL3 , Model3FileName.c_str());
    TQErrCode ret = LoadLocalPrintJob(FileNames, BitmapResolution, QualityMode, OperationMode, SliceNum);
    free (FileNames);
    return ret;
}
TQErrCode CMachineManager::Load8LocalPrintJob(QString ModelFileName,QString SupportFileName,QString Model2FileName,
		QString Model3FileName,
		QString Model4FileName,
		QString Model5FileName,
		QString Model6FileName,
		QString Model7FileName,int BitmapResolution,QString ModeStr)
{
	int QualityMode,OperationMode;
	int SliceNum = 1;
    if (GetModeFromShortStr(ModeStr, QualityMode, OperationMode) == false)
        throw EMachineManager("LoadLocalPrintJob::Undefined mode");

    TFileNamesArray FileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
    memset(FileNames, 0, TFileNamesArraySIZE);
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_SUPPORT, SupportFileName.c_str());
	strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL1 , ModelFileName.c_str());
    strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL2 , Model2FileName.c_str());
	strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL3 , Model3FileName.c_str());
	strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL4 , Model4FileName.c_str());
	strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL5 , Model5FileName.c_str());
	strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL6 , Model6FileName.c_str());
	strcpy((TBMPFileName)FileNames + MAX_FILE_PATH*TYPE_CHAMBER_MODEL7 , Model7FileName.c_str());
	TQErrCode ret = LoadLocalPrintJob(FileNames, BitmapResolution, QualityMode, OperationMode, SliceNum);

    free (FileNames);
    return ret;
}
TQErrCode CMachineManager::LoadLocalDoublePrintJob(TFileNamesArray   FileNames1,
        TFileNamesArray   FileNames2,
        int               BitmapResolution,
        int               QualityMode,
        int               OperationMode,
        int               SliceNum1,
        int               SliceNum2)
{

    // Create a file print job

    TFileNamesArray FlexFileNames1 = NULL, FlexFileNames2 = NULL;
    try
    {
        // Making Sure there is no collision with another print...
        // A job change is possible only in idle mode
        if((m_CurrentState == msIdle)     ||
				(m_CurrentState == msStandby1) ||
                (m_CurrentState == msStandby2))
        {
            InformStop(Q2RT_EMBEDDED_RESPONSE,false);
        }
        else
            throw EMachineManager("Can not add a new local job before previous job is done");

        TFileNamesArray FlexFileNames1 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
        TFileNamesArray FlexFileNames2 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
        for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
        {
            strcpy((TBMPFileName)FlexFileNames1 + i*MAX_FILE_PATH,
                   Q2RTApplication->FlexibleFileName((TBMPFileName)FileNames1 + i*MAX_FILE_PATH).c_str() );
            strcpy((TBMPFileName)FlexFileNames2 + i*MAX_FILE_PATH,
				   Q2RTApplication->FlexibleFileName((TBMPFileName)FileNames2 + i*MAX_FILE_PATH).c_str() );
        }

        bool ModeChangeOnPrintStart = m_ParamsMgr->ModeChangeOnPrintStart;
        CModesManager *ModesManager = CModesManager::Instance();

		SetNewSystemMode(QualityMode,OperationMode);

        m_DoubleFilePrintJob->DeInit();
        m_DoubleFilePrintJob->Init(FormatNames(FlexFileNames1, OperationMode==SINGLE_MATERIAL_OPERATION_MODE),
                                   FlexFileNames1,
                                   FlexFileNames2,
                                   SliceNum1,
                                   SliceNum2,
								   m_ParamsMgr->GetLayerHeight_um() / 1000.0f,
                                   BitmapResolution,
                                   QualityMode, OperationMode,
                                   m_ParamsMgr->DPI_InYAxis);

        // Returning to default mode
        if (ModeChangeOnPrintStart == true)
            ModesManager->GotoDefaultMode();

        // Start processing new print job
        AddJob(m_DoubleFilePrintJob);
        free(FlexFileNames1);
        free(FlexFileNames2);
    }
    catch(...)
    {

        if(FlexFileNames1)
            free(FlexFileNames1);
        if(FlexFileNames2)
            free(FlexFileNames2);

        throw;
    }
    return Q_NO_ERROR;
}

// Load a local file print job (return a pointer to the file print job object)
TQErrCode CMachineManager::LoadLocalPrintJob(TFileNamesArray   FileNames,
		int               BitmapResolution,
        int               QualityMode,
        int               OperationMode,
        int               SliceNum)
{
    TFileNamesArray FlexFileNames = NULL;
	try



	
    {
        // Making Sure there is no collision with another print...
        // A job change is possible only in idle mode
        if((m_CurrentState == msIdle)     ||
                (m_CurrentState == msStandby1) ||
				(m_CurrentState == msStandby2))
            InformStop(Q2RT_EMBEDDED_RESPONSE,false);
        else
            throw EMachineManager("Can not add a new local job before previous job is done");

        TFileNamesArray FlexFileNames = (TFileNamesArray)malloc(TFileNamesArraySIZE);
        for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
            strcpy((TBMPFileName)FlexFileNames + i * MAX_FILE_PATH, Q2RTApplication->FlexibleFileName((TBMPFileName)FileNames + i * MAX_FILE_PATH).c_str());

        bool ModeChangeOnPrintStart = m_ParamsMgr->ModeChangeOnPrintStart;
        CModesManager *ModesManager = CModesManager::Instance();

		SetNewSystemMode(QualityMode,OperationMode);

		m_LocalFilePrintJob->DeInit();
        m_LocalFilePrintJob->Init(FormatNames(FlexFileNames, OperationMode==SINGLE_MATERIAL_OPERATION_MODE),
                                  FlexFileNames,
                                  SliceNum,
                                  m_ParamsMgr->GetLayerHeight_um() / 1000.0f,
                                  BitmapResolution,
                                  QualityMode,
                                  OperationMode,
                                  m_ParamsMgr->DPI_InYAxis);

        // Returning to default mode
        if (ModeChangeOnPrintStart == true)
            ModesManager->GotoDefaultMode();

        // Start processing new print job
        AddJob(m_LocalFilePrintJob);
		free(FlexFileNames);
    }
    catch(...)
    {

        if(FlexFileNames)
            free(FlexFileNames);

        throw;
    }
    return Q_NO_ERROR;
}

TQErrCode CMachineManager::ExecutePFFPrintJob(char* FolderPath)
{
	TQErrCode ret = AddPFFPrintJob(FolderPath);
	return ret;
}

TQErrCode CMachineManager::AddPFFPrintJob(QString FolderPath)
{
	m_pPFFSliceQueueThread->Init(FolderPath);
	if (m_pPFFSliceQueueThread->ValidatePFFJobPramas())
	{
		m_pPFFSliceQueueThread->Resume();

		m_PFFPrintJob->Init(m_pPFFSliceQueueThread);

		AddJob(m_PFFPrintJob, PFFJob);
	}

	return Q_NO_ERROR;
}

void CMachineManager::SetNewSystemMode(  int QualityMode,int OperationMode)
{
    CModesManager* ModesManager       = CModesManager::Instance();

    if (m_ParamsMgr->ModeChangeOnPrintStart == false)
        return;

	ModesManager->GotoDefaultMode();
    ModesManager->EnterMode(PRINT_MODE[QualityMode][OperationMode],
                            MACHINE_QUALITY_MODES_DIR(QualityMode, OperationMode));
    m_ParamsMgr->PrintingOperationMode = OperationMode;
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->PrintingOperationMode);

	HandleFlexibleParameters();
}

void CMachineManager::HandleFlexibleParameters()
{
	if(m_flexibleParameters.m_numOfUVOn!=-1)
	{
		if(m_flexibleParameters.m_numOfUVOn==2)
		{
			m_ParamsMgr->UVSingleLampEnabledArray[0] = 1;
			m_ParamsMgr->UVSingleLampEnabledArray[1] = 1;
        }
		else if(m_flexibleParameters.m_numOfUVOn==1)
        {
            m_ParamsMgr->UVSingleLampEnabledArray[0] = 1;
            m_ParamsMgr->UVSingleLampEnabledArray[1] = 0;
        }
    }

    if(m_flexibleParameters.m_delayBetweenLayers>=0)
        m_ParamsMgr->AtLeastDelayTimeBetweenLayers = m_flexibleParameters.m_delayBetweenLayers;
}

TMachineState CMachineManager::GetCurrentState(void)
{
    return m_CurrentState;
}

void CMachineManager::IgnorePowerByPass(bool Ignore)
{
    m_MachineSequencer->IgnorePowerByPass(Ignore);
}

void CMachineManager::MarkPythonWizardRunning(bool IsRunning)
{
    CQLog::Write(LOG_TAG_OBJET_MACHINE,"MarkPythonWizardRunning: %d",IsRunning);
    MarkWizardRunning(IsRunning);
    m_IsPythonWizardRunning = IsRunning;
}

void CMachineManager::MarkWizardRunning(bool IsRunning)
{
    CQLog::Write(LOG_TAG_OBJET_MACHINE,"MarkWizardRunning: %d",IsRunning);
    m_IsWizardRunning = IsRunning;
    if (!m_IsWizardRunning)
    {
        SetCanAddJobFromHost(m_IsWizardRunning);

#ifdef ALLOW_OPENING_MRW_FROM_HOST  // I place it within a dedicated #ifdef section because it might not be safe for all puposes (e.g. it is not thread safe).
        m_CommandsQueue->Send(cmdPendingHostRR);  // This is used to trigger the processing of a pending Material Replacement Wizard (MRW) request from host after current MRW was interrupted and cancelled by the request from the host while the MRW was on the welcome page.
#endif
    }
}

void CMachineManager::MarkPrintingWizardRunning(bool IsRunning)
{
    m_IsPrintingWizardRunning = IsRunning;
}

bool CMachineManager::IsPrintingWizardRunning(void)
{
    return m_IsPrintingWizardRunning;
}

bool CMachineManager::IsWizardRunning(void)
{
    return m_IsWizardRunning;
}

bool CMachineManager::IsPythonWizardRunning(void)
{
    return m_IsPythonWizardRunning;
}

void CMachineManager::MarkBITRunning(bool IsRunning)
{
    m_IsBITRunning = IsRunning;
}

bool CMachineManager::IsBITRunning()
{
    return m_IsBITRunning;
}

// Change system state to online or offline
TQErrCode CMachineManager::SetOnlineOffline(bool Online)
{
    try
    {
        // If different then current state
        if(Online != m_Online)
        {
            // If we should go online
            if(Online)
            {
                // If a job is currently loaded, start print
                if(m_CurrentJob != NULL)
                    StartPrint();
            }
            m_Online = Online;

            FrontEndInterface->EnableDisableControl(FE_ONLINE_MODE,m_Online);

            if(!Online ||
                    ((m_CurrentJob != NULL) && (m_JobOrigin == JobFromHost)))
                Q2RTApplication->GetHostComm()->SetOnlineOffline(Online);
        }

    }
    catch(EQException& err)
    {
        QMonitor.ErrorMessage(err.GetErrorMsg());
        m_Online = false;
        FrontEndInterface->EnableDisableControl(FE_ONLINE_MODE,m_Online);
    }

    return Q_NO_ERROR;
}

TQErrCode CMachineManager::OffsetZStart(float OffsetInMM)
{
    m_ParamsMgr->Z_StartPrintPosition.DisableObservers();

    // Assigning new value
    m_ParamsMgr->Z_StartPrintPosition += OffsetInMM;

    m_ParamsMgr->Z_StartPrintPosition.EnableObservers();

    return Q_NO_ERROR;
}

// Handle issues related to modes manager when stopping printing
void CMachineManager::ModesManagerHandlingOnStop(void)
{
    if(m_ParamsMgr->DefaultModeAfterPrint)
        CModesManager::Instance()->GotoDefaultMode();
}

TQErrCode CMachineManager::DisableStandbyPhase(void)
{
    m_StandbyPhaseOn = false;
    return Q_NO_ERROR;
}

TQErrCode CMachineManager::EnableStandbyPhase(void)
{
    m_StandbyPhaseOn = true;
    return Q_NO_ERROR;
}

bool CMachineManager::GetStandbyPhaseValue(void)
{
    return m_StandbyPhaseOn;
}

void CMachineManager::EnableDisablePrintTimeCounters(bool Enable)
{
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	FOR_ALL_HEADS(i)
	BackEnd->EnableDisableMaintenanceCounter(HeadsCounterID[i],Enable);

	//end of print
	if( false == Enable)
	{
		//override support head 0 counter
		ULONG h0Counter =  BackEnd->GetMaintenanceCounterElapsedSeconds(HEAD_S1_COUNTER_ID);
		BackEnd->SetMaintenanceCounter(HEAD_S0_COUNTER_ID, h0Counter);

		//Convert drop counter to grams, then advance counter
		for( int i = 0; i < TOTAL_NUMBER_OF_HEADS; i++)
		{
			unsigned long weight = m_ParamsMgr->HeadDropCounterDuringPrint[i] * m_ParamsMgr->DropWeight_NG / m_ParamsMgr->DropCounterNormaliser;
			BackEnd->AdvanceCounterBySeconds(HeadsCounter_DC_ID[i], weight * 36);
        }
	}
	else
	{
		for( int i = 0; i < TOTAL_NUMBER_OF_HEADS; i++)
			m_ParamsMgr->HeadDropCounterDuringPrint[i] = 0;
	}

    BackEnd->EnableDisableMaintenanceCounter(TOTAL_PRINTING_TIME_COUNTER_ID, Enable);
    BackEnd->EnableDisableMaintenanceCounter(WIPER_BLADE_COUNTER_ID, Enable);
    BackEnd->EnableDisableMaintenanceCounter(ROLLER_BLADE_COUNTER_ID, Enable);
    BackEnd->EnableDisableMaintenanceCounter(SERVICE_COUNTER_ID, Enable);
    BackEnd->EnableDisableMaintenanceCounter(LAST_HCW_ACTIVATION_TIME_COUNTER_ID, Enable); //itamar, Super purge
	BackEnd->EnableDisableMaintenanceCounter(HOW_HS_HM_COUNTER_ID, Enable);
	BackEnd->EnableDisableMaintenanceCounter(HOW_HQ_COUNTER_ID, Enable);
	BackEnd->EnableDisableMaintenanceCounter(WASTE_PUMP_LEFT_COUNTER_ID, Enable);

	BackEnd->EnableDisableMaintenanceCounter(PATTERN_TEST_COUNTER_ID, Enable);
	BackEnd->EnableDisableMaintenanceCounter(HEAD_ALIGNMENT_COUNTER_ID, Enable);
	BackEnd->EnableDisableMaintenanceCounter(LOAD_CELL_CALIBRATION_COUNTER_ID, Enable);
	BackEnd->EnableDisableMaintenanceCounter(ROLLER_BATH_COUNTER_ID, Enable);

	for (int i = 0; i < MAX_NUMBER_OF_MODEL_CHAMBERS-1; i++) //size of (MAX_NUMBER_OF_MODEL_CHAMBERS-1) since MODEL7 is also support in DM6 and shouldn't be taken into account here
	{
		if (m_ParamsMgr->MRW_IsChannelAsleep[i])
		{
        	BackEnd->EnableDisableMaintenanceCounter(SleepingChannelsCounterID[i], Enable);
		}
	}
}

// Do before print BIT sequence, return true if Ok
bool CMachineManager::BeforePrintBIT(void)
{
    bool AllowPrint = false;

    if(m_ParamsMgr->DoBeforePrintBIT)
    {
        // Run the "before print" tests
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Self Test...");
        Q2RTApplication->YieldMainThread();

        TTestResult TestResult = Q2RTApplication->GetBITManager()->RunBITSet("Before Print");

        // If error, dont't allow printing
        switch(TestResult)
        {
        case trGo:
        case trUnknown:
            AllowPrint = true;
            break;

        case trWarning:
            AllowPrint = QMonitor.AskYesNo("At least one built-in-test issued a warning.\r\nStart print anyway?");
            break;

        case trNoGo:
            QMonitor.ErrorMessageWaitOk("Can not start print - at least one built-in-test has failed");
        }
    }
    else
        AllowPrint = true;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    Q2RTApplication->YieldMainThread();

    if(!AllowPrint)
        FrontEndInterface->UpdateStatus(FE_SHOW_BIT_RESULT,0,true);

    return AllowPrint;
}

// Get a pointer to the current active job
CPrintJob* CMachineManager::GetCurrentJob(void)
{
    return m_CurrentJob;
}

int CMachineManager::GetTotalNumberOfSlices(void)
{
    return m_TotalNumberOfSlices;
}

bool CMachineManager::IsPrintCompleted(void)
{
    // Although the implementation that uses setting/clearing m_PrintCompleted is unreliable in theory
    // (because prePrinting phase clears m_PrintCompleted and theoretically, a new prePrinting phase can start before
    // the client of this function calls it)
    // it is suitable for most of our practical uses:
    // we just need to make sure that no new printing starts before we call IsPrintCompleted.
	return m_PrintCompleted;
}

// Instance to the actual machine sequencer
CMachineSequencer* CMachineManager::GetMachineSequencer(void)
{
    return m_MachineSequencer;
}

bool CMachineManager::IsOnline(void)
{
    return m_Online;
}

void CMachineManager::AcceptPendingResinReplacementRequest(ResinReplacementRequestType* Msg)
{

    m_MutexHostResinRequest.WaitFor();

    //Communication with the Host is sequential(Don't need mutex for this)

    m_ParamsMgr->MRW_Host_NewResinArraySize = Msg->NumberOfModels;
    m_ParamsMgr->MRW_Host_IsPending         = true;

#ifdef ALLOW_OPENING_MRW_FROM_HOST
    m_ParamsMgr->MRW_HostWarnPendingRequest = true;

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    m_ParamsMgr->MRW_Host_RequestTime = QFormatStr("This wizard was launched by host request accepted at %s",strtok(asctime (timeinfo), "\n\0"));
#endif

    m_ParamsMgr->MRW_Host_MachineState      = Msg->MaintenanceMode;

    for(int i = 0; i < Msg->NumberOfModels; i++)
        m_ParamsMgr->MRW_Host_NewResinArray[i] = Msg->Materials[i];

    m_MutexHostResinRequest.Release();

#ifdef ALLOW_OPENING_MRW_FROM_HOST
    m_CommandsQueue->Send(cmdPendingHostRR);
#endif

}

#ifdef ALLOW_OPENING_MRW_FROM_HOST
void CMachineManager::StartResinReplacementFromHost(void)
{

    if (false == m_ParamsMgr->MRW_Host_IsPending)
        return;
    if (true  == m_IsWizardRunning) // todo -oNobody -cNone: this is not thread safe.
        return;


    m_MutexHostResinRequest.WaitFor();
    FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_BUSY_MODE);

    m_ParamsMgr->MRW_LastState             = 0;
    m_ParamsMgr->MRW_SelectedOperationMode = m_ParamsMgr->MRW_Host_MachineState;
    ChooseTanksToReplace();

    m_ParamsMgr->MRW_Host_IsPending = false;


    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_LastState);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_SelectedOperationMode);

    FrontEndInterface->UpdateStatus(FE_UPDATE_UI_MODE,FE_UI_NO_MODE);
    m_MutexHostResinRequest.Release();

    SendHostPendingMessage();
}

void CMachineManager::SendHostPendingMessage(void)
{
    FrontEndInterface->UpdateStatus(FE_HOST_RENDING_RESIN_REPLACEMENT);
}
#else

void CMachineManager::SetParametersForResinReplacement(void)
{
    m_MutexHostResinRequest.WaitFor();

    // If there was an job request from the host that required resin replacement(MRW) and has not been 'attended' yet
    if (m_ParamsMgr->MRW_Host_IsPending)
    {
        // Set operation mode according to the last resin replacement requested received from the host
        m_ParamsMgr->MRW_SelectedOperationMode = m_ParamsMgr->MRW_Host_MachineState;
        m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_SelectedOperationMode);

        // Set tanks and materials selection according to the last resin replacement requested received from the host
        ChooseTanksToReplace();

        // Mark the host's resin replacment request as 'attended'
        m_ParamsMgr->MRW_Host_IsPending = false;
        m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_Host_IsPending);
    }

    m_MutexHostResinRequest.Release();
}
#endif

void CMachineManager::ChooseTanksToReplace(void)
{
    map<QString, int>            RequiredMaterials;
    TMaterialToTankVectorMap     AvailableMaterials;
    TMaterialToTankVectorMapIter MaterialIter;
    TTankIndexVector             CurTankVector;

    //Calculate current tanks materials state
    for(int i = FIRST_MODEL_TANK_TYPE; i < LAST_MODEL_TANK_TYPE; i++)
    {
        AvailableMaterials[QStrToLower(m_ParamsMgr->TypesArrayPerTank[i])].push_back((TTankIndex)i);
        m_ParamsMgr->MRW_TanksOptionArray[i] = false;
    }

    //Calculate missing resins and remove resins that should stay from available
    for(int i = 0; i < m_ParamsMgr->MRW_Host_NewResinArraySize; i++)
    {
        m_ParamsMgr->MRW_Host_NewResinArray[i] = QStrToLower(m_ParamsMgr->MRW_Host_NewResinArray[i]);
        if ((MaterialIter = AvailableMaterials.find(m_ParamsMgr->MRW_Host_NewResinArray[i]))
                != AvailableMaterials.end())
            HandleCurrent(AvailableMaterials, MaterialIter, m_ParamsMgr->MRW_Host_NewResinArray[i], (m_ParamsMgr->MRW_Host_MachineState == m_ParamsMgr->MRW_SelectedOperationMode));
        else
            RequiredMaterials[m_ParamsMgr->MRW_Host_NewResinArray[i]]++;

    }

    //Find resin to replace and replace
    for(map<QString, int>::iterator Iter = RequiredMaterials.begin(); Iter != RequiredMaterials.end(); ++Iter) // todo -oNobody -cNone: define map<QString, int>::iterator;
    {
        for(int i = Iter->second; i > 0; i--)
        {
            if(AvailableMaterials.empty())
                throw EQException("Illegal: Can not find appropriate tank to replace");

            MaterialIter = AvailableMaterials.begin();
            HandleCurrent(AvailableMaterials, MaterialIter, Iter->first, true);
        }
    }
    for(MaterialIter =  AvailableMaterials.begin();
            MaterialIter != AvailableMaterials.end(); ++MaterialIter)
    {
        CurTankVector = MaterialIter->second;
        for(TTankIndexVectorIter Iter = CurTankVector.begin(); Iter != CurTankVector.end(); ++Iter)
            m_ParamsMgr->MRW_NewResinArray[*Iter] = MaterialIter->first;
    }
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_NewResinArray);

    for(int i = FIRST_MODEL_TANK_TYPE; i < LAST_MODEL_TANK_TYPE; i++)
        if (m_ParamsMgr->MRW_TanksOptionArray[i] == true)
            return;

    m_ParamsMgr->MRW_TanksOptionArray[FIRST_MODEL_TANK_TYPE] = true;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_TanksOptionArray);


}

void CMachineManager::HandleCurrent(TMaterialToTankVectorMap&     AvailableMaterials,
                                    TMaterialToTankVectorMapIter& MaterialIter,
                                    const QString&                NewResinName,
                                    const bool&                   IsTankSelected)
{
    TTankIndexVector& CurTankVector      = MaterialIter->second;
    int               NewResinArrayIndex = *(CurTankVector.begin());
    m_ParamsMgr->MRW_TanksOptionArray[NewResinArrayIndex] = IsTankSelected;
    m_ParamsMgr->MRW_NewResinArray   [NewResinArrayIndex] = NewResinName;
    CurTankVector.erase(CurTankVector.begin());
    if (CurTankVector.empty())
        AvailableMaterials.erase(MaterialIter);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_NewResinArray);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_TanksOptionArray);
}

void CMachineManager::ShowMaterialWarning(int Value)
{
    QString MaterialWarningBefore = m_ParamsMgr->MaterialWarningBeforeText;
    QString MaterialWarningAfter  = m_ParamsMgr->MaterialWarningAfterText;

    if ((Value == SHOW_BEFORE_PRINTING && MaterialWarningBefore != "") ||
            (Value == SHOW_AFTER_PRINTING  && MaterialWarningAfter != ""))
        if (FrontEndInterface->ShowMaterialWarning(Value) != Q_NO_ERROR)
            CQLog::Write(LOG_TAG_GENERAL,"Material warning error");
}

bool CMachineManager::EnableErrorMessageDuringStop(bool Enable)
{
    m_EnableErrorMessageDuringStop = Enable;
    return m_EnableErrorMessageDuringStop;
}

void CMachineManager::SetCanAddJobFromHost(bool CanAddJobFromHost)
{
    m_CanAddJobFromHost = CanAddJobFromHost;
}

bool CMachineManager::GetCanAddJobFromHost(void)
{
    return m_CanAddJobFromHost;
}

/*
void CMachineManager::QExceptionCallback(CQThread *ThreadPtr,EQException ExceptionObject,TGenericCockie Cockie)
{
  CQThread::QExceptionCallback(ThreadPtr,);
  QMonitor.PrintfNonMaskable("Thread '%s' throw exception: '%s' , (Code %d)",ThreadPtr->Name().c_str(),
                             ExceptionObject.GetErrorMsg().c_str(),ExceptionObject.GetErrorCode());
}

void CMachineManager::UnexpectedExceptionCallback(CQThread *ThreadPtr,TGenericCockie Cockie)
{
  QMonitor.PrintfNonMaskable("Thread '%s' threw an unexpected exception",ThreadPtr->Name().c_str());
}
*/

void CMachineManager::InformStop(TQErrCode ErrCode, bool InformLayerProcess)
{
    if (InformLayerProcess)
        Q2RTApplication->GetLayerProcess()->StopProcessing();

    // Informing the host of the stop...
    //if(m_CurrentJob != NULL)
    if(m_JobOrigin == JobFromHost)
    {
        Q2RTApplication->GetHostComm()->StopPrint(ErrCode);
        //Q2RTApplication->GetHostComm()->CloseConnection();
    }
}

TJobOrigin CMachineManager::GetJobOrigin()
{
	return m_JobOrigin;
}

//Set for each channel if it's asleep or active
void CMachineManager::SetSleepingChannels()
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	TTankOperationMode CurrentTanksOpMode = static_cast<TTankOperationMode>(m_ParamsMgr->TanksOperationMode.Value());

	switch (CurrentTanksOpMode)
	{
		case S_M1_M3_M5_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M1_M3_M6_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M1_M4_M5_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M1_M4_M6_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M2_M3_M5_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M2_M3_M6_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M2_M4_M5_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M2_M4_M6_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);

			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = true;
			break;

		case S_M1_M7_ACTIVE_TANKS_MODE:
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL1 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL2 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL3 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL4 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL5 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			m_ParamsMgr->MRW_IsChannelAsleep[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS] = false;
			BackEnd->ResetMaintenanceCounter(SleepingChannelsCounterID[TYPE_CHAMBER_MODEL6 - MAX_NUMBER_OF_SUPPORT_CHAMBERS]);
			break;

		default:
    		break;
	}

	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MRW_IsChannelAsleep);
}

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void CMachineManager::ThreadExceptionHandlerCallback(CQThread *ThreadPtr,EQException Exception, TGenericCockie Cockie)
{

    CMachineManager *InstancePtr = reinterpret_cast<CMachineManager *>(Cockie);

    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    CActuatorBase *Actuators   = InstancePtr->m_MachineSequencer->GetActuatorInstance();
    //CRollerBase *Roller        = InstancePtr->m_MachineSequencer->GetRollerInstance();


    //1. UV Turn off
    BackEnd->TurnUVLamps(false);

    //2. HeadFilling Turn off
    InstancePtr->m_MachineSequencer->GetHeadFillingInstance()->HeadFillingOnOff(false);

    //3. HeadHeaters Turn off
    //InstancePtr->m_MachineSequencer->CancelHeadHeating();
    CQLog::Write(LOG_TAG_GENERAL,"ThreadExceptionHandlerCallback() calls SetDefaultOnOff()");
    InstancePtr->m_MachineSequencer->GetHeadHeatersInstance()->SetDefaultOnOff(HEAD_HEATER_OFF);

    //4. Roller Turn off
    //Roller->SetRollerOnOff(false);
    BackEnd->TurnRoller(false);

    //5. Actuators Turn off
    //for(int i=0;i<MAX_ACTUATOR_ID;i++)
    //RIGHT_LAMP_ACTUATOR_ID            26
    Actuators->SetOnOff(26, false);
    //LEFT_LAMP_ACTUATOR_ID             25
    Actuators->SetOnOff(25, false);
    //6. Door Un-Lock
    InstancePtr->m_MachineSequencer->GetDoorInstance()->Disable();
    CQLog::Write(LOG_TAG_GENERAL,"Expected exception callBack (Machine Manager).");
    QMonitor.ErrorMessageWaitOk("An error has occurred.\nClose and restart the printer application.");
}

void CMachineManager::ThreadUnExpectedExceptionHandlerCallback(CQThread *ThreadPtr, TGenericCockie Cockie)
{

    CMachineManager *InstancePtr = reinterpret_cast<CMachineManager *>(Cockie);

    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    CActuatorBase *Actuators   = InstancePtr->m_MachineSequencer->GetActuatorInstance();
    //CRollerBase *Roller        = InstancePtr->m_MachineSequencer->GetRollerInstance();

    //1. UV Turn off
    BackEnd->TurnUVLamps(false);
    //2. HeadFilling Turn off
    InstancePtr->m_MachineSequencer->GetHeadFillingInstance()->HeadFillingOnOff(false);

    //3. HeadHeaters Turn off
    //InstancePtr->m_MachineSequencer->CancelHeadHeating();
    CQLog::Write(LOG_TAG_GENERAL,"ThreadUnExpectedExceptionHandlerCallback() calls SetDefaultOnOff()");
    InstancePtr->m_MachineSequencer->GetHeadHeatersInstance()->SetDefaultOnOff(HEAD_HEATER_OFF);

    //4. Roller Turn off
    //Roller->SetRollerOnOff(false);
    BackEnd->TurnRoller(false);

    //5. Actuators Turn off
    //for(int i=0;i<MAX_ACTUATOR_ID;i++)
    //RIGHT_LAMP_ACTUATOR_ID            26
    Actuators->SetOnOff(26, false);
    //LEFT_LAMP_ACTUATOR_ID             25
    Actuators->SetOnOff(25, false);

    //6. Door Un-Lock
    InstancePtr->m_MachineSequencer->GetDoorInstance()->Disable();
    CQLog::Write(LOG_TAG_GENERAL,"Un-Expected exception callBack (Machine Manager).");
    QMonitor.ErrorMessageWaitOk("An error has occured.\nClose and restart the printer application.");
}

bool CMachineManager::IsPrintingModeAllowed(CPrintJob *Job)
{
	//Always allow printing in wizards
	if( CBackEndInterface::Instance()->IsWizardRunning() )
		return true;
	int index = 0;
	FOR_ALL_MODES(qm,om)
	{
		QString Mode = m_ParamsMgr->HSW_PrintingAllowedForMode.GetCellDescription(index);
		if ( 0 == Mode.compare( GetModeStr(Job->GetQualityMode(), Job->GetOperationMode())))
		{	bool flag = m_ParamsMgr->HSW_PrintingAllowedForMode[index];
			return flag;
		}
		index++;
	}
	return false;
}

/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

