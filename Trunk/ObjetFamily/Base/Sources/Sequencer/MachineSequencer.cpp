/********************************************************************
*                        Objet Geometries LTD.                     *
*                        ---------------------                     *
* Project: Q2RT.                                                   *
* Module: Machine sequencer.                                       *
* Module Description: Implementation of the specific Q2RT          *
*                     sequences.                                   *
*                                                                  *
* Compilation: Standard C++.                                       *
*                                                                  *
* Author: Ran Peleg/Gedalia Trejger.                               *
* Start date: 30/08/2001                                           *
* Last upate: 05/08/2002                                           *
********************************************************************/

#include "Actuator.h"
#include "AmbientTemperature.h"
#include "EvacuationAirFlow.h"
#include "AppLogFile.h"
#include "BackEndInterface.h"
#include "configuration.h"
#include "Container.h"
#include "Door.h"
#include "ExcelSequenceAnalyzer.h"
#include "FIFOPci.h"
#include "FrontEnd.h"
#include "HeadFilling.h"
#include "HeadHeaters.h"
#include "HeadStatus.h"
#include "Layer.h"
#include "LayerProcess.h"
#include "MachineSequencer.h"
#include "MaintenanceCounters.h"
#include "ModesDefs.h"
#include "ModesManager.h"
#include "Motor.h"
#include "OCBStatus.h"
#include "Power.h"
#include "PrintControl.h"
#include "Purge.h"
#include "Q2RTApplication.h"
#include "Q2RTErrors.h"
#include "QVersionInfo.h"
#include "Roller.h"
#include "Tester.h"
#include "TrayHeater.h"
#include "UVLamps.h"
#include "rfid.h"
#include "PowerFailHandler.h"
#include "SignalTower.h"
#include "Q2RTWizard.h"
#include "Fans.h"
#include "ConfigBackupFile.h"
#ifndef EDEN_CL
#include "About.h"
#endif

#include "FrontEndControlIDs.h"
#include "MotorMcb.h"
const char  CONFIG_BACKUP_FILE_NAME[] = "ConfigBackup\\ConfigBackupForHost.zip";

const float MOTOR_PURGE_DELTA_MOVE_MM    = 103.718; // Purge Y relative end position in mm (128.412 - 24.694)
const int X_SCATTER_RANGE                = 150;
const int X_SCATTER_MIN_POSITION         = 10;
const int Z_MOVEMENT_AFTER_BUMPER_IMPACT = 20; // mm
const int ELECTRIC_HOME_REG_ADDR         = 0x32;

const int PS_LEVEL_STABILIZATION_TIME    = 3;  // In Seconds...
const int SECONDS_IN_HOUR                = 60 * 60;
const int UV_LAMPS_POLL_TIME             = 100;

const bool IGNORE_BUMPER_IMPACT          = true;

const int HASP_CHECK_EACH_N_SLICES       = 10;

EMachineSequencer::EMachineSequencer(const QString& ErrMsg,const TQErrCode ErrCode) : EQException(ErrMsg,ErrCode) {}
EMachineSequencer::EMachineSequencer(const TQErrCode ErrCode) : EQException(PrintErrorMessage(ErrCode),ErrCode) {}

// Constructor
CMachineSequencer::CMachineSequencer(const QString& Name) : CQSequencer(Name), m_IgnorePowerByPass(false)
{
    INIT_METHOD(CMachineSequencer,HandleCartridgeDlgClose);
    INIT_METHOD(CMachineSequencer,ErrorInsertion);
    INIT_METHOD(CMachineSequencer,GoToPurge);
    INIT_METHOD(CMachineSequencer,FireAll);
    INIT_METHOD(CMachineSequencer,TestPattern);
    INIT_METHOD(CMachineSequencer,FireAllSequence);
    INIT_METHOD(CMachineSequencer,PurgeSequence);
    INIT_METHOD(CMachineSequencer,GetLastPrintedSliceNum);
    INIT_METHOD(CMachineSequencer,GetCurrentModelHeight);
    INIT_METHOD(CMachineSequencer,GetLastError);
    INIT_METHOD(CMachineSequencer,RemotePerformWipe);
    INIT_METHOD(CMachineSequencer,WriteToUVHistoryFile);
    INIT_METHOD(CMachineSequencer,OpenUVLampsHistoryFile);
    INIT_METHOD(CMachineSequencer,CloseUVLampsHistoryFile);
    INIT_METHOD(CMachineSequencer,GetUVValueSingleIteration);
    INIT_METHOD(CMachineSequencer,UVReset);
    INIT_METHOD(CMachineSequencer,GetUVMaxValueRead);
	INIT_METHOD(CMachineSequencer,GetUVMaxDeltaRead);
    INIT_METHOD(CMachineSequencer,GetUVSumRead);
    INIT_METHOD(CMachineSequencer,GetUVNumOfReadings);
    INIT_METHOD(CMachineSequencer,IsUVWizardLimited);
    INIT_METHOD(CMachineSequencer,SaveAllUVParams);
    INIT_METHOD(CMachineSequencer,DispatchHelp);
	

    m_ParamsMgr = CAppParams::Instance();

    m_SequencerBlock.KeepZLocation=0;
    m_SequencerBlock.StartZLocation=0;
    m_SequencerBlock.StartYPosition=0;
    m_SequencerBlock.XLeftSideSpace=0;
    m_SequencerBlock.XRightSideSpace=0;
    m_SequencerBlock.KeepCookingData.XStartPosition=0;
    m_SequencerBlock.KeepCookingData.XStopPosition=0;
    m_SequencerBlock.KeepCookingData.YPasses=0;
    m_SequencerBlock.KeepCookingData.YStartPosition=0;
    m_SequencerBlock.ZBacklash = 0;
    m_SequencerBlock.PrintingWasPaused = false;
    m_SequencerBlock.DoZHomeBeforePrint = true;
    m_SequencerBlock.CurrentModelHeight = 0.0f;
    m_SequencerBlock.LayerHeightDeltaInUM = 0;
    m_SequencerBlock.SingleLayerHeightInMM = 0;
    m_SequencerBlock.SingleLayerHeightInUM = 0;
    m_SequencerBlock.SingleHalfLayerHeightInUM = 0;
    m_SequencerBlock.CurrentHalfLayerHeightInUM = 0;
    m_SequencerBlock.HalfLayerHeightDeltaInUM = 0;
    m_SequencerBlock.DoublePassSpace = m_ParamsMgr->YSecondaryInterlaceNoOfPixels;
    m_SequencerBlock.LastPrintedSlice = 0;
    m_SequencerBlock.ZMultiplicator1StepIn_um = 0;

    m_CartridgesCheckState = CHECK_CARTRTIDGE_EXISTENCE;
    m_ContainerReplacementPromptTime = 0;
	m_BufferNumber = 0;
	m_PurgeIntervalsToPerformBrushWipe = 0;

    m_Motors             = NULL;
    m_UVLamps            = NULL;
    m_TrayHeater         = NULL;
    m_TrayPlacer         = NULL;
    m_Power              = NULL;
    m_Purge              = NULL;
    m_Door               = NULL;
    m_Container          = NULL;
    m_Roller             = NULL;
    m_HeadFilling        = NULL;
    m_HeadHeaters        = NULL;
    m_Vacuum             = NULL;
    m_HeadStatusSender   = NULL;
    m_PrintControl       = NULL;
    m_AmbientTemperature = NULL;
    m_AirFlow  		   = NULL;
    m_OCBStatusSender    = NULL;
    m_Actuator           = NULL;
	m_UVHistoryFile      = NULL;
	m_HOWHistoryFile     = NULL;
    m_PowerFail          = NULL;
	m_SignalTower        = NULL;
	m_Fans				 = NULL;

    m_CurrentMachineState = msIdle;
    m_DuringPurge         = false;
    m_UVWizardLimited     = true;
    m_PurgeStopped        = false;
    m_FireAllStopped      = false;
    m_TestPatternStopped  = false;

    m_IsSuctionSystemExist = m_ParamsMgr->IsSuctionSystemExist; //RSS, itamar added

    try
    {
        QMonitor.SetMonitorErrorLoggingCallback(LogMonitorMessage);

#if defined EMULATION_ONLY
        FIFOPCI_Init(true);

        //Initialize Configuration package
        CONFIG_Init();

        m_UVLamps = new CUvLampsDummy("UVLamps");
        m_TrayHeater = new CTrayHeaterDummy("TrayHeater");
        m_TrayPlacer = new CTrayPlacerDummy("TrayPlacer");
        m_Power = new CPowerDummy("Power");
        m_Door = new CDoorDummy("Door");
        m_AirFlow = new CEvacAirFlowDummy("AirFlow");
        m_Container = new CContainerDummy("Container");
        m_Purge = new CPurgeDummy("Purge");
        m_HeadFilling = new CHeadFillingDummy("HeadFilling");
        m_Actuator = new CActuatorDummy ("Actuator");
        m_OCBStatusSender = new COCBStatusSenderDummy("OCBStatusSender",
                m_Purge,
                m_UVLamps,
                m_Actuator,
                m_Door,
                m_Container,
                m_Power,
                m_TrayPlacer,
                m_AirFlow,
                m_HeadFilling,
                m_Container,
                m_TrayHeater);

		m_Roller             = new CRollerDummy             ("Roller");
        m_HeadHeaters        = new CHeadHeatersDummy        ("HeadHeaters");
        m_AmbientTemperature = new CAmbientTemperatureDummy ("AmbientTemperature");
        m_Vacuum             = new CHeadVacuumDummy         ("Vacuum",m_Purge);
        m_HeadStatusSender   = new CHeadStatusSenderDummy    ("HeadStatusSender",
                m_Roller,
                m_HeadFilling,
                m_HeadHeaters,
                m_Power,
                m_AmbientTemperature,
                m_Vacuum);
		m_PrintControl       = new CPrintControlDummy ("PrintControl");
		m_Fans				 = new CFans("Fans");

        m_PowerFail = new CPowerFail;   // Move to Q2RTApplication?
        m_PowerFail->Resume();

        m_Motors = new CMotorsDummy("Motors",m_Actuator,m_Door);
        m_LicenseManager = new CLicenseManagerDummy("LicenseManagerEmulation",LOG_TAG_HASP);
        m_SignalTower = new CSignalTowerDummy("SignalTower");

#else
        FIFOPCI_Init(m_ParamsMgr->DataCard_Emulation);

        //Initialize Configuration package
        CONFIG_Init();

        // Initialize control components
        if(!m_ParamsMgr->OCB_Emulation)
        {
            m_UVLamps = new CUvLamps("UVLamps");

            m_TrayHeater = new CTrayHeater("TrayHeater");
            m_TrayPlacer = new CTrayPlacer("TrayPlacer");

            m_Power = new CPower("Power");
            m_Door = new CDoor("Door");

            m_AirFlow = new CEvacAirFlow("AirFlow");

            //if (!m_ParamsMgr->WeightSensorsBypass)
            //  m_Container = new CContainer("Container");
            //else
            //  m_Container = new CContainerDummy("Container");

            m_Container = new CContainer("Container");
            m_Purge = new CPurge("Purge");
            m_HeadFilling = new CHeadFilling("HeadFilling");
            m_Actuator = new CActuator ("Actuator");
            m_OCBStatusSender = new COCBStatusSender("OCBStatusSender",
                    m_Purge,
                    m_UVLamps,
                    m_Actuator,
                    m_Door,
                    m_Container,
                    m_Power,
                    m_TrayPlacer,
                    m_AirFlow,
                    m_HeadFilling,
                    m_Container,
                    m_TrayHeater  );

            m_SignalTower = new CSignalTower("SignalTower");
        }
        else
        {
            m_UVLamps = new CUvLampsDummy("UVLamps");
            m_TrayHeater = new CTrayHeaterDummy("TrayHeater");
            m_TrayPlacer = new CTrayPlacerDummy("TrayPlacer");
            m_Power = new CPowerDummy("Power");
            m_Door = new CDoorDummy("Door");
            m_AirFlow = new CEvacAirFlowDummy("AirFlow");
            m_Container = new CContainerDummy("Container");
            m_Purge = new CPurgeDummy("Purge");
            m_HeadFilling = new CHeadFillingDummy("HeadFilling");
            m_Actuator = new CActuatorDummy ("Actuator");
            m_OCBStatusSender = new COCBStatusSenderDummy("OCBStatusSender",
                    m_Purge,
                    m_UVLamps,
                    m_Actuator,
                    m_Door,
                    m_Container,
                    m_Power,
                    m_TrayPlacer,
                    m_AirFlow,
                    m_HeadFilling,
                    m_Container,
                    m_TrayHeater);
            m_SignalTower = new CSignalTowerDummy("SignalTower");
        }
        if(!m_ParamsMgr->OHDB_Emulation)
        {
            m_Roller = new CRoller("Roller");
            m_HeadHeaters = new CHeadHeaters("HeadHeaters");
            m_AmbientTemperature = new CAmbientTemperature ("AmbientTemperature");
			m_Vacuum = new CHeadVacuum ("Vacuum",m_Purge);
            m_HeadStatusSender = new CHeadStatusSender("HeadStatusSender",
					m_Roller,
                    m_HeadFilling,
                    m_HeadHeaters,
                    m_Power,
					m_AmbientTemperature,
                    m_Vacuum);
			m_PrintControl = new CPrintControl ("PrintControl");
			m_Fans	= new CFans("Fans");
        }
        else
        {
            m_Roller             = new CRollerDummy             ("Roller");
            m_HeadHeaters        = new CHeadHeatersDummy        ("HeadHeaters");
            m_AmbientTemperature = new CAmbientTemperatureDummy ("AmbientTemperature");
			m_Vacuum             = new CHeadVacuumDummy         ("Vacuum",m_Purge);
            m_HeadStatusSender   = new CHeadStatusSenderDummy    ("HeadStatusSender",
                    m_Roller,
                    m_HeadFilling,
                    m_HeadHeaters,
                    m_Power,
                    m_AmbientTemperature,
					m_Vacuum);
			m_PrintControl       = new CPrintControlDummy ("PrintControl");
			m_Fans				 = new CFans("Fans");
        }

        m_PowerFail = new CPowerFail;   // Move to Q2RTApplication?
        m_PowerFail->Resume();

        //Because Model on tray sensor Motors should be create after Actuator
        if(!m_ParamsMgr->MCB_Emulation)
            m_Motors = new CMotors("Motors",m_Actuator,m_Door);
        else
            m_Motors = new CMotorsDummy("Motors",m_Actuator,m_Door);

        m_LicenseManager = new CLicenseManager("LicenseManager","ProfessionalFamily",LOG_TAG_HASP);

#endif
    }
    catch(...)
    {
        // In a case of exception, clean up and throw again
        DeInitControlComponents();
        throw;
    }
}

// Destructor
CMachineSequencer::~CMachineSequencer(void)
{
    Shutdown();
    DeInitControlComponents();
}

void CMachineSequencer::IgnorePowerByPass(bool Ignore)
{
    m_IgnorePowerByPass = Ignore;
}

// Start the control threads owned by the sequencer
void CMachineSequencer::StartControlThreads(void)
{
    //m_OCBStatusSender ->Resume(); Are not initialized to true
    //m_HeadStatusSender->Resume();  are not initialized to true
}

// Implement the sequence cancel action (override)
void CMachineSequencer::OnCancel(void)
{
    m_Motors->Cancel();
    m_UVLamps->Cancel();
    m_TrayHeater->Cancel();
    m_Power->Cancel();
    m_Purge->Cancel();
    m_Door->Cancel();
    m_Container->Cancel();
    m_Roller->Cancel();
    m_HeadFilling->Cancel();
    m_HeadHeaters->Cancel();
    m_Vacuum->Cancel();
    m_HeadStatusSender->Cancel();
    m_AmbientTemperature->Cancel();
    m_Actuator->Cancel();
    m_OCBStatusSender->Cancel();
}


// If the Canceled flag is set throw the EQSequenceCanceled exception
void CMachineSequencer::CancelCheck(void)
{
    if(Canceled)
        throw ESequenceCanceled("Cancel check");
}

void CMachineSequencer::DeInitControlComponents(void)
{
    if(m_HeadStatusSender != NULL)
    {
        delete m_HeadStatusSender;
        m_HeadStatusSender = NULL;
    }

    if(m_OCBStatusSender != NULL)
    {
        delete m_OCBStatusSender;
        m_OCBStatusSender = NULL;
    }

    if (m_PowerFail != NULL)
    {
        m_PowerFail->Shutdown();
        m_PowerFail = NULL;
    }

    if(m_UVLamps != NULL)
    {
        delete m_UVLamps;
        m_UVLamps = NULL;
    }

    if(m_TrayHeater != NULL)
    {
        delete m_TrayHeater;
        m_TrayHeater = NULL;
    }

    if(m_Motors != NULL)
    {
        delete m_Motors;
        m_Motors = NULL;
    }

    if(m_Roller != NULL)
    {
        delete m_Roller;
        m_Roller = NULL;
    }

    if(m_HeadHeaters != NULL)
    {
        delete m_HeadHeaters;
        m_HeadHeaters = NULL;
    }

    if(m_Vacuum != NULL)
    {
        delete m_Vacuum;
        m_Vacuum = NULL;
    }

    if(m_HeadFilling != NULL)
    {
        delete m_HeadFilling;
        m_HeadFilling = NULL;
    }

    if(m_Power != NULL)
    {
        delete m_Power;
        m_Power = NULL;
    }

    if(m_Purge != NULL)
    {
        delete m_Purge;
        m_Purge = NULL;
    }

    if(m_Door != NULL)
    {
        delete m_Door;
        m_Door = NULL;
    }

    if(m_Container != NULL)
    {
        delete m_Container;
        m_Container = NULL;
    }

    if(m_PrintControl != NULL)
    {
        delete m_PrintControl;
        m_PrintControl = NULL;
    }

    if(m_AmbientTemperature != NULL)
    {
        delete m_AmbientTemperature;
        m_AmbientTemperature = NULL;
    }

    if(m_AirFlow != NULL)
    {
        delete m_AirFlow;
        m_AirFlow = NULL;
    }

    if(m_Actuator != NULL)
    {
        delete m_Actuator;
        m_Actuator = NULL;
    }

    //We have to process awaiting messages because TCartridgeErrorForm::InformStopToDlg
    //sets a message (WM_INFORM_PRINTING_STOP) using PostMessage (i.e. async),
    //and m_SignalTower might already be deallocated by then.
    Application->ProcessMessages();

    Q_SAFE_DELETE(m_LicenseManager);
    Q_SAFE_DELETE(m_SignalTower);
}

TQErrCode CMachineSequencer::PowerUpSequence(void)
{
	TQErrCode Err;
	CBackEndInterface::Instance()->EnableDisableMaintenanceCounter(RESTART_COMPUTER_COUNTER_ID, true);
    SaveBackupFiles();
	m_MachineType = APPLICATION_NAME;
#ifndef EDEN_CL
	CQLog::Write(LOG_TAG_PRINT,"%s - Power Up time:%s (Version %s - %s %s)",
				 m_MachineType.c_str(),
				 QDateTimeToStr(QGetCurrentTime()).c_str(),
				 QGetAppVersionStr().c_str(),
				 GetVersionDate().c_str(),
				 GetVersionTime().c_str());
#endif
	try
	{
		FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Power Up");
		UpdateLastError(Q_NO_ERROR);

        // The package that defines machine type is the last in the list - highest in the hierarchy
        if(m_ParamsMgr->ShowPackageName && ! m_LicenseManager->GetLicensedPackages().empty())
			FrontEndInterface->UpdateStatus(FE_SET_MACHINE_LOGO, m_LicenseManager->GetLicensedPackages().back().PackageName);
        else
            FrontEndInterface->UpdateStatus(FE_SET_MACHINE_LOGO, "");

        //send ping to OCB
        if(CHECK_NOT_EMULATION(m_ParamsMgr->OCB_Emulation))
        {
            if (!COCBProtocolClient::Instance()->Ping())
            {
                CQLog::Write(LOG_TAG_GENERAL,"There is no communication with OCB");
                FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Power Up failed");
                throw EMachineSequencer("There is no communication with OCB");
            }
        }

        try
        {
            m_OCBStatusSender->DisplayOCBVersion();
        }
        catch(EQException& E)
        {
            QMonitor.ErrorMessage(E.GetErrorMsg());
        }

        // Check Signal Tower by leaving it ON during PowerUp sequence
        ActivateSignalTower(ST_LIGHT_ON, ST_LIGHT_ON, ST_LIGHT_ON, false);

        if((Err = m_Power->SetPowerOnOff(POWER_ON) ) != Q_NO_ERROR)
            throw EMachineSequencer("Set power on error");

        if((Err = m_Power->WaitForPowerOnOff() ) != Q_NO_ERROR)
            throw EMachineSequencer("Power is not turned ON - timeout");

        // if RFID instance was never created then we assume working
        // without RFID.
        if (CRFID::Instance())
        {
            try
            {
                CRFID::Instance()->Start();
            }
            catch(EQException& E)
            {
                QMonitor.ErrorMessage(E.GetErrorMsg());
            }
        }

        if(CBackEndInterface::Instance()->GetLicensedMaterialsList().size() == 0)
            QMonitor.WarningMessage(LM_ERR_NO_PACKAGE.c_str());

        // Send default parameters of UV Lamps to OCB (active lamps, timeouts...)
        if((Err = m_UVLamps->SetDefaultParmIgnitionTimeout() ) != Q_NO_ERROR)
            throw EMachineSequencer("Set default Uv lamps params error");

        // Send a message that resets the UV safety trigger in the OCB (immobility detected trigger)
        // THIS MESSAGE HAVE TO BE SENT ONLY ONCE, HERE IN POWER-UP SEQUENCE
        if((Err = m_UVLamps->ResetUVSafety() ) != Q_NO_ERROR)
            throw EMachineSequencer("Reset Uv Safety error");

        // Send a message "init the normally open variable in the OCB (relevant for doors and emergency stop buttons)
        // THIS MESSAGE HAVE TO BE SENT ONLY ONCE, HERE IN POWER-UP SEQUENCE

        if((Err = m_Door->ResetNormallyOpenInetLockFeedBack() ) != Q_NO_ERROR)
            throw EMachineSequencer("Reset door normally open error");

        // Set D/A Value to the UV Lamps (otherwise, if someone tries to turn on the UV lamps via the actuators
        //(before these functions are called for the first time) the power suppliers will not supply the correct voltage
        //and the lamps will go ON and OFF all the time!
        for(BYTE i = 0; i < NUMBER_OF_UV_LAMPS; i++)
            if((Err = m_UVLamps->SetUVD2AValue(i, m_ParamsMgr->UVLampPSValueArray[i]) ) != Q_NO_ERROR)
                throw EMachineSequencer("Set UV D2A value error");

        //	Turn UV lamps OFF
        if((Err = m_UVLamps->TurnOnOff(false) ) != Q_NO_ERROR)
            throw EMachineSequencer("Turn off Uv lamp error");

        //	Turn Tray heating OFF
        if((Err = m_TrayHeater->SetDefaultTrayStartTemperature()) != Q_NO_ERROR)
            throw EMachineSequencer("Tray Heater set temperature error.");
        if((Err = m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF) ) != Q_NO_ERROR)
            throw EMachineSequencer("Tray Heater turn off error.");

        if((Err = m_Container->GetLiquidTankInsertedStatus()) != Q_NO_ERROR)
            throw EMachineSequencer("'GetLiquidTankInsertedStatus' failed");

        if ((Err = m_Actuator->SetHoodFanIdle()) != Q_NO_ERROR)
            throw EMachineSequencer("'SetHoodFanIdle' failed");

        if((Err = m_Actuator->SetAmbientHeaterOnOff(false, AMBIENT_HEATER_1 | AMBIENT_HEATER_2)) != Q_NO_ERROR)
            throw EMachineSequencer("'SetAmbientHeaterOnOff' failed");

        //send ping to OHDB
        if(CHECK_NOT_EMULATION(m_ParamsMgr->OHDB_Emulation))
        {
            if (!COHDBProtocolClient::Instance()->Ping())
            {
                CQLog::Write(LOG_TAG_GENERAL,"There is no PING from OHDB");
                FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Power Up OHDB failed");
                throw EMachineSequencer("There is no communication with OHDB");
            }
            // now wait for it to load the FPGA
            if (Q_NO_ERROR != CBackEndInterface::Instance()->WaitForOhdbHwDrvToLoad())
            {
                throw EMachineSequencer("OHDB FPGA problem");
            }

            m_PrintControl->ResetDriverCircuit(); // it throws on error
        }

        try
        {
            m_HeadStatusSender->DisplayOHDBVersion();
        }
        catch(EQException& E)
        {
            QMonitor.ErrorMessage(E.GetErrorMsg());
        }

        // Write the Computer name to the Log
        QString ComputerName = Q2RTApplication->ComputerName;
        CQLog::Write(LOG_TAG_PRINT,"Computer name: %s", ComputerName.c_str());

        // Note: Setting HeadFillingOnOff OFF *before* SetDefaultHeatersTemperature reason:
        //       in case system is up while OCB head filling monitoring was still ON -
        //       sending SetDefaultHeatersTemperature() with IDLE temps will cause "Temperature is too high." error from OCB.
        // Turn head Liquids monitoring OFF
        if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
            throw EMachineSequencer("Head filling set param error");
        if((Err = m_HeadFilling->HeadFillingOnOff(false) ) != Q_NO_ERROR)
            throw EMachineSequencer("Head filling monitoring off error");

        m_AirFlow->SetupAirFlowParams();

//  Turn Heads heating OFF
		m_AmbientTemperature->SetOHDBAmbientParams();
        if((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
            throw EMachineSequencer("Set heater temperature error");
		if((Err = m_HeadHeaters->SetDefaultStandbyTemperature()) != Q_NO_ERROR)
            throw EMachineSequencer("Head Heater set default standby temperature error.");
        if((Err = m_HeadHeaters->SetDefaultPrintingHeadsVoltages()) != Q_NO_ERROR)
            throw EMachineSequencer("Set heater temperature error");
        if((Err = m_HeadHeaters->GetPrintingHeadsVoltages()) != Q_NO_ERROR)
            throw EMachineSequencer("Set heater temperature error");

        if(m_ParamsMgr->KeepHeadsHeated)
        {
            if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
                throw EMachineSequencer("Power UP:Set heater off fail");
        }
        else
        {
            if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF)) != Q_NO_ERROR)
                throw EMachineSequencer("Power UP:Set heater off fail");
        }

        //	Set default Vacuum valve parameters
        if((Err = m_Vacuum->SetDefaultVacuumParams() ) != Q_NO_ERROR)
            throw EMachineSequencer("Set vacuum parameters error.");

        //	set Support/Model valve parameters
        //May be one actuator should be turned here
		//Turn Vacuum ON and gifa OFF
		if ((Err = m_Actuator->SetOnOff(ACTUATOR_ID_VACUUM_VALVE,1) ) != Q_NO_ERROR)
			throw EMachineSequencer("Actuator message error.");

        try
        {
            m_Container->ActivateRollerAndPurgeWaste(false);
        }
        catch(EContainer &e)
        {
            throw EMachineSequencer(e.GetErrorMsg());
        }

        //turn RSS off, itamar added
        if(m_IsSuctionSystemExist)
        {
            if (m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                    m_ParamsMgr->RollerSuctionValveOffTime,false)!=Q_NO_ERROR)
                throw EMachineSequencer("Power up: Activate RSS message failed.");
        }
        // Turn Roller OFF
        if ((Err = m_Roller->SetRollerOnOff(ROLLER_OFF)) != Q_NO_ERROR)
            throw EMachineSequencer("set roller off error.");

        m_Roller->BumperEnableDisable(false);
        m_Roller->SetDefaultBumperParams();

        // Turn off material cooling fan to off
        if ((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF)) != Q_NO_ERROR)
            throw EMachineSequencer("set roller off error.");

        //Start to send periodic status message: Start the task
        m_OCBStatusSender->Resume();
        m_HeadStatusSender->Resume();

        //Start OCB and OHDB in debug/non-debug mode:
        if (CHECK_NOT_EMULATION(m_ParamsMgr->OCB_Emulation))
            if (IsDebuggerPresent())
                m_OCBStatusSender->EnterOCBDebugMode();
            else
                m_OCBStatusSender->ExitOCBDebugMode();

        if(CHECK_NOT_EMULATION(m_ParamsMgr->OHDB_Emulation))
            if (IsDebuggerPresent())
                m_HeadStatusSender->EnterOHDBDebugMode();
            else
                m_HeadStatusSender->ExitOHDBDebugMode();

        try
        {
            m_Motors->CheckMotorCommunication();

            // InitMotorParameters() Generates MCB sw requirement if error encountered.
            if((Err=m_Motors->InitMotorParameters() ) != Q_NO_ERROR)
                return Err;

            // IsMCBResetRequired() checks for MCB-reset-required condition.
            if (m_Motors->IsMCBResetRequired())
            {
                m_Motors->SWReset();

                // After MCB was reset, init motor's parameters again:
                if((Err=m_Motors->InitMotorParameters() ) != Q_NO_ERROR)
                    return Err;
            }

            // Disable Motors
            if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_DISABLE)) != Q_NO_ERROR)
                throw EMachineSequencer("Power UP:Set motor mode disable fail (" + QIntToStr(Err) + ")");

            //Turn interlock OFF
            if ((Err = m_Door->UnlockDoor() ) != Q_NO_ERROR)
                throw EMachineSequencer("Unlock door error");

            m_Container->UpdateChambers();

            CHaspInterfaceWrapper::Instance()->SetWriteToLog(true);
            CHaspInterfaceWrapper::Instance()->Resume();
        }
        catch(EQException& E)
        {
            FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Power Up Motor failed");
            QMonitor.ErrorMessage(E.GetErrorMsg());
        }

        // Power up sequence is done, turn off signal tower
        ActivateSignalTower(ST_LIGHT_OFF, ST_LIGHT_OFF, ST_LIGHT_OFF, false);

        for (int i=35; i; --i)
        {
            if (m_Container->AreAllTanksStable())
            {
                break;
            }
            FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Power Up (Please wait, " + QIntToStr(i) + ")");
            QSleep (1000);
        }

        //Update log
        FrontEndInterface->UpdateStatus(FE_PREVIOUS_JOB_LAST_SLICE,m_ParamsMgr->SliceNumber.Value());
		FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
	}
    catch(EQException& E)
    {
        QMonitor.ErrorMessage(E.GetErrorMsg());
    }
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::IdleSequence(void)
{
    TQErrCode Err;

    try
    {

        CQLog::Write(LOG_TAG_PRINT,"State:Idle");
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

		// Turn Off register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
		CQLog::Write(LOG_TAG_GENERAL,"Disabling XILINX_REG_PRINT_VOLTAGE");		
		Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, 0x0000);

		//Turns the interlock off
		if ((Err = m_Door->Disable() ) != Q_NO_ERROR)
            return Err;

    }
    catch(ESequenceCanceled)
    {

        // Filter cancel error and convert it to return code
        return Q2RT_SEQUENCE_CANCELED;
    }

    return Q_NO_ERROR;
}


void CMachineSequencer::PrePrintSequence(bool IsContinueJob/*CPrintJob *Job*/) //itamarDeleteJob
{
	//PrepareOHDBModelConfig();
    if(!IsContinueJob)
		ZMotorParamsInitBeforeFirstLayer();

    CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence started");
	FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Start Pre Print");

    UpdateLastError(Q_NO_ERROR);
	m_CurrentMachineState = msPrePrint;

	//Reset scanning values
	InitScanningCookingData();
	TQErrCode Err = Q_NO_ERROR;

	// Activate Green light in the Signal Tower
	ActivateSignalTower(ST_LIGHT_OFF, ST_LIGHT_ON, ST_LIGHT_OFF);

	m_BufferNumber = 0;
	
	if ( Q_NO_ERROR != (Err = m_Fans->IsError()) )
	{
		throw EMachineSequencer(Err);
	}

    //Verify if parameters to print is OK.
	if ((Err = CheckParameters()) != Q_NO_ERROR)
		throw EMachineSequencer(Err);

    //Verify if purge bath is OK.
    if ((Err = CheckPurgeBath()) != Q_NO_ERROR)
    {
//        FrontEndInterface->NotificationMessage("Purge bath overflow, printing aborted. Printer maintenance required.");
        throw EMachineSequencer(Err);
    }

    if((Err = m_TrayPlacer->VerifyTrayInserted() ) != Q_NO_ERROR)
    {
		if(Err==Q2RT_TRAY_IS_NOT_INSERTED)
        {
            FrontEndInterface->NotificationMessage("TRAY IS OUT. Stop printing");
            throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
        }
        else
            throw EMachineSequencer(Err);
    }

    //Turns the interlock on  // todo -oNobody -cNone: turning the lock on here is not good enough. If it is locked here and not earlier - the user can open the cover (door) when the machine manager is already in prePrint phase. It causes undesireable side effects. Example: in XOffsetWizard, it causes the wizard to stay in printing transparency page (counting time forever) without making any progress.
	if((Err=m_Door->Enable())!= Q_NO_ERROR)
		throw EMachineSequencer(Err);

    //Choose which printer type will be used
    ChoosePrinterMotorSequencer ();

    // Set the Tank with the less resin to be active.
    m_Container->SetActiveContainersBeforePrinting(true);

    if(IsContinueJob == true)
    {
        CQLog::Write(LOG_TAG_PRINT,"Resume Sequence: lastSliceZPosition %d",(int)m_ParamsMgr->LastSliceZPosition);
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_Z,MOTOR_ENABLE)) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
        if((Err = m_Motors->InitMotorParameters()) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
        if((Err = m_Motors->MoveToAbsolutePosition(AXIS_Z,m_ParamsMgr->MaxPositionStep[AXIS_Z] - m_ParamsMgr->ZMaxPositionMargin,muSteps)) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
        if(m_Motors->IsMotorMoving(AXIS_Z))
            m_Motors->WaitForEndOfMovement(AXIS_Z,60);
    }

    // Check the status of the model support and waste cartridges
	if ((Err = CheckCartridgesStatusBeforePrint(msPrePrint)) != Q_NO_ERROR)
		throw EMachineSequencer(Err);

    // Check the current ambient temperature
    if (m_AmbientTemperature->GetCurrentTemperatureStatus() == AMBIENT_TEMPERATURE_COLD)
        throw EMachineSequencer(Q2RT_AMBIENT_ERROR_TEMPERATURE_COLD);
    if (m_AmbientTemperature->GetCurrentTemperatureStatus() == AMBIENT_TEMPERATURE_HOT)
        throw EMachineSequencer(Q2RT_AMBIENT_ERROR_TEMPERATURE_HOT);

    // Check the evacuation air-flow
    if (m_AirFlow->GetGlidingEvacAirFlowStatus() == AIR_FLOW_LOW_CRITICAL)
        throw EMachineSequencer(Q2RT_EVAC_AIRFLOW_SPEED_CRITICAL);
    m_AirFlow->ResetErrorState();

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    // InitMotorParameters() Generates MCB sw requirement if error encountered.
    if((Err=m_Motors->InitMotorParameters() ) != Q_NO_ERROR)
        throw EMachineSequencer(Err);

    // IsMCBResetRequired() checks for MCB-reset-required condition.
    if (m_Motors->IsMCBResetRequired())
    {
        m_Motors->SWReset();

        // After MCB was reset, init motor's parameters again:
        if((Err=m_Motors->InitMotorParameters() ) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
    }
    m_HeadStatusSender->ClearOHDBResetCount();

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

	//Prepare tray heater for printing
	if((Err=m_TrayHeater->Enable(START_PRINT_TEMPERATURE))!= Q_NO_ERROR)
        throw EMachineSequencer(Err);

    //Verify temperature later.

    if((Err = m_HeadHeaters->SetDefaultStandbyTemperature()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if((Err = m_HeadHeaters->SetDefaultPrintingHeadsVoltages()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if((Err = m_HeadHeaters->GetPrintingHeadsVoltages()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);

    m_HeadHeaters->ResetStatistics();
    m_Vacuum->ResetStatistics();
    m_Actuator->ResetStatistics();

    if((Err = m_Vacuum->SetDefaultVacuumParams()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);

    // Dirty Trick: Warming heads, but lowering the Threshold by 300 ADu, in order to
    //              'Reuse' some of the time to turn on th UV Lamps too...
    //              When reaching the 300 ADu line - turnning on the UV Lamps while
    //              the heads still warming up.

    // Saving the original Threshold values
    int PrevLowThreshold  = m_ParamsMgr->HeadsTemperatureLowThershold;
    int PrevHighThreshold = m_ParamsMgr->HeadsTemperatureHighThershold;

    // Assigning 'Dirty' Threshold values
    m_ParamsMgr->HeadsTemperatureLowThershold  += m_ParamsMgr->HeadsTemperatureThersholdOffset;
    m_ParamsMgr->HeadsTemperatureHighThershold += m_ParamsMgr->HeadsTemperatureThersholdOffset;

    //Turn Heads heating on
    if ((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
    {
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

        throw EMachineSequencer(Err);
    }

    // Warming heads...
    CQLog::Write(LOG_TAG_GENERAL,"PrePrintSequence() Calls SetDefaultOnOff() 3");
    if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
    {
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;
        throw EMachineSequencer(Err);
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming Heads");
    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT); //bug 6029
    }

    // Waiting for the heads to warm up to 300 ADu form the set point
    CQLog::Write(LOG_TAG_GENERAL,"PrePrintSequence() Calls WaitForHeadsTemperatureOK() 2");
    if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
    {
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

        if(Err == Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED)
            throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
        throw EMachineSequencer(Err);
    }

    // Restoring the 'Real' Thresholds
    m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
    m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

    // Apply the thresholds
    if((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"UV Lamps Ignition");

    // Set D/A Value to the UV Lamps
    for(BYTE i = 0; i < NUMBER_OF_UV_LAMPS; i++)
        m_UVLamps->SetUVD2AValue(i, m_ParamsMgr->UVLampPSValueArray[i]);

    // Turn UV lamps ON
    if((Err = m_UVLamps->Enable()) != Q_NO_ERROR)
    {
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
        throw EMachineSequencer(Err);
    }

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming Heads");

    // Waiting for the heads to reach the actual Threshold
    CQLog::Write(LOG_TAG_GENERAL,"PrePrintSequence() Calls WaitForHeadsTemperatureOK() 3");
    if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
    {
        if(Err == Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED)
            throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
        throw EMachineSequencer(Err);
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    //Turn Heads liquid monitoring ON
    if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if((Err = m_HeadFilling->GetIfThermistorsAreWorking()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if((Err = m_HeadFilling->HeadFillingOnOff(true)) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
	FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Filling Heads");

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }
    if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    try
    {
        m_Container->ActivateRollerAndPurgeWaste(true);
    }
    catch(EContainer &e)
    {
        throw EMachineSequencer(e.GetErrorMsg());
    }

    //RSS, itamar added, turn RSS on
    if(m_IsSuctionSystemExist)
    {
        if (m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                m_ParamsMgr->RollerSuctionValveOffTime,true)!= Q_NO_ERROR)
            throw EMachineSequencer("Activate RSS message failed.");
    }
    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }


    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming Tray");
    //Verify if tray temperature is ok.
    if((Err = m_TrayHeater->WaitForTrayInTemperature() ) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    if(!IsContinueJob)
    {
        if ((Err = MotorInitBeforeFirstLayer()) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
    }
    else
    {
        if ((Err = MotorInitAfterRecovery()) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
    }

    if ((Err = MotorPrePrintSequencer()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);

    //Before purge verify if sequence was not stopped
    if (m_CurrentMachineState != msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

	// Turn On register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
	CQLog::Write(LOG_TAG_GENERAL,"Enabling XILINX_REG_PRINT_VOLTAGE");
	Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, XILINX_PRINTING_ENABLED);

    //Activate Purge and wipe sequence with home and fire all
    if (m_ParamsMgr->DoPurgeBeforePrint)
    {
        if((Err = PurgeSequence(DONT_PERFORN_HOME, true)) != Q_NO_ERROR)
            throw EMachineSequencer(Err);
        //Fire All after purge
	 /*	if (m_ParamsMgr->PerformFireAllAfterPurge)
			if((Err = FireAllSequence()) != Q_NO_ERROR)
				throw EMachineSequencer(Err);
				*/
	}

    if (m_CurrentMachineState!=msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Moving Axis");
    if ((Err = MotorGoToZStartPosition()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Check OHDB and OCB");

    if ((Err = PrepareOHDBModelConfig ()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);

    if ((Err = PrepareOCBToPrintModel ()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    if (m_CurrentMachineState != msPrePrint)
    {
        CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
        throw EMachineSequencer(Q2RT_STOP_DURING_PRE_PRINT);
    }

    //Initiate statistics
    ResetPCIStatistics();

    // Clear the 'KeepHeadsHeated' parameter
    m_ParamsMgr->KeepHeadsHeated = false;
	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->KeepHeadsHeated);
}

//This Procedure
TQErrCode CMachineSequencer::ContinueSequence(/*CPrintJob *Job*/)
{
    TQErrCode Err;

    m_CurrentMachineState=msPrinting;
    UpdateLastError(Q_NO_ERROR);

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Recovery");

    CQLog::Write(LOG_TAG_GENERAL, "Continue printing after pause.");

    //Reset scanning values
	InitScanningCookingData();

	if ( Q_NO_ERROR != (Err = m_Fans->IsError()) )
	{
		return Err;
	}

    if ((Err = CheckParameters()) != Q_NO_ERROR)
        return Err;

    //Choose which printer type will be used
    ChoosePrinterMotorSequencer ();

    // Check the status of the model support and waste cartridges
    if ((Err = CheckCartridgesStatusBeforePrint(msPrinting)) != Q_NO_ERROR)
        return Err;

    //Verify Liquid ID
    if ((Err=m_Container->GetIfAllLiquidTankAreInserted()) != Q_NO_ERROR)
        return Err;

    // InitMotorParameters() Generates MCB sw requirement if error encountered.
    if((Err=m_Motors->InitMotorParameters() ) != Q_NO_ERROR)
        return Err;

    // IsMCBResetRequired() checks for MCB-reset-required condition.
    if (m_Motors->IsMCBResetRequired())
    {
        m_Motors->SWReset();

        // After MCB was reset, init motor's parameters again:
        if((Err=m_Motors->InitMotorParameters() ) != Q_NO_ERROR)
            return Err;
    }



    //Set tray to printing termperature
    if((Err = m_TrayHeater->SetDefaultTrayPrintingTemperature() ) != Q_NO_ERROR)
        return Err;

    if((Err = m_TrayPlacer->VerifyTrayInserted() ) != Q_NO_ERROR)
    {
        if(Err==Q2RT_TRAY_IS_NOT_INSERTED)
        {
            FrontEndInterface->NotificationMessage("TRAY IS OUT. Stop printing");
            return Q2RT_STOP_DURING_CONTINUE;
        }
        else
            return Err;
    }

    if(m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    //Turns the interlock on
    if((Err=m_Door->Enable())!= Q_NO_ERROR)
        return Err;


    //Resume Sequence
    CQLog::Write(LOG_TAG_PRINT,"Resume Sequence: lastSliceZPosition %d",m_ParamsMgr->LastSliceZPosition);
    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_Z,MOTOR_ENABLE)) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if((Err = m_Motors->InitMotorParameters()) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if((Err = m_Motors->MoveToAbsolutePosition(AXIS_Z,m_ParamsMgr->MaxPositionStep[AXIS_Z]- m_ParamsMgr->ZMaxPositionMargin,muSteps)) != Q_NO_ERROR)
        throw EMachineSequencer(Err);
    if(m_Motors->IsMotorMoving(AXIS_Z))
        m_Motors->WaitForEndOfMovement(AXIS_Z,60);

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    //Prepare tray heater for printing
    if((Err=m_TrayHeater->Enable(PRINTING_TEMPERATURE))!= Q_NO_ERROR)
        return Err;

    //Verify tray temperature later.

    if((Err = m_HeadHeaters->SetDefaultStandbyTemperature()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadHeaters->SetDefaultPrintingHeadsVoltages()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadHeaters->GetPrintingHeadsVoltages()) != Q_NO_ERROR)
        return Err;

    m_HeadHeaters->ResetStatistics();
    m_Vacuum->ResetStatistics();
    m_Actuator->ResetStatistics();

    if((Err = m_Vacuum->SetDefaultVacuumParams()) != Q_NO_ERROR)
        return Err;

    // Dirty Trick: Warming heads, but lowering the Threshold by 300 ADu, in order to
    //              'Reuse' some of the time to turn on th UV Lamps too...
    //              When reaching the 300 ADu line - turnning on the UV Lamps while
    //              the heads still warming up.

    // Saving the original Threshold values
    int PrevLowThreshold  = m_ParamsMgr->HeadsTemperatureLowThershold;
    int PrevHighThreshold = m_ParamsMgr->HeadsTemperatureHighThershold;

    // Assigning 'Dirty' Threshold values
    m_ParamsMgr->HeadsTemperatureLowThershold  += m_ParamsMgr->HeadsTemperatureThersholdOffset;
    m_ParamsMgr->HeadsTemperatureHighThershold += m_ParamsMgr->HeadsTemperatureThersholdOffset;

    //Turn Heads heating on
    if ((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
    {
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

        return Err;
    }

    // Warming heads...
    CQLog::Write(LOG_TAG_GENERAL,"ContinueSequence() calls SetDefaultOnOff() 4");
    if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
    {
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;
        return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming Heads");

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

        return Q2RT_STOP_DURING_CONTINUE;
    }

    CQLog::Write(LOG_TAG_GENERAL,"ContinueSequence() calls WaitForHeadsTemperatureOK() 4");
    // Waiting for the heads to warm up to 300 ADu form the set point
	if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(true)) != Q_NO_ERROR)
    {
        // Restoring the 'Real' Thresholds
        m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
        m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

        if(Err == Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED)
            return Q2RT_STOP_DURING_CONTINUE;
        return Err;
    }

    // Restoring the 'Real' Thresholds
    m_ParamsMgr->HeadsTemperatureLowThershold  = PrevLowThreshold;
    m_ParamsMgr->HeadsTemperatureHighThershold = PrevHighThreshold;

    // Apply the thresholds
    if((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"UV Lamps Ignition");

    // Turn UV lamps ON
    if((Err = m_UVLamps->Enable()) != Q_NO_ERROR)
    {
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
        return Err;
    }

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming Heads");

    // Waiting for the heads to reach the actual Threshold
    CQLog::Write(LOG_TAG_GENERAL,"ContinueSequence() calls WaitForHeadsTemperatureOK() 5");
    if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(true)) != Q_NO_ERROR)
    {
        if(Err == Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED)
            return Q2RT_STOP_DURING_PRE_PRINT;
        return Err;
    }

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    //Turn Heads liquid monitoring ON
    if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadFilling->GetIfThermistorsAreWorking()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadFilling->HeadFillingOnOff(true)) != Q_NO_ERROR)
        return Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Filling Heads");
    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Warming Tray");

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }
    //Verify if tray temperature is ok.
    if((Err = m_TrayHeater->WaitForTrayInTemperature() ) != Q_NO_ERROR)
        throw EMachineSequencer("Pre-print:Tray Heater Not under temperature");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    try
    {
        m_Container->ActivateRollerAndPurgeWaste(true);
    }
    catch(EContainer &e)
    {
        return Q2RT_CONTAINERS_WASTE_FULL;
    }

    //RSS, itamar added, turn RSS on
    if(m_IsSuctionSystemExist)
    {
        if(m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                m_ParamsMgr->RollerSuctionValveOffTime,true)!= Q_NO_ERROR)
            throw EMachineSequencer("Pre-print: Activate RSS message failed.");
    }
    if ((Err = MotorInitAfterRecovery()) != Q_NO_ERROR)
        return Err;
    if ((Err = MotorPrePrintSequencer()) != Q_NO_ERROR) //ItamarDeleteJob
        return Err;

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }
    //Activate Purge and wipe sequence with home and fire all
    if (m_ParamsMgr->DoPurgeBeforePrint)
    {
        if((Err = PurgeSequence(DONT_PERFORN_HOME, true)) != Q_NO_ERROR)
            return Err;
        //Fire All after purge
	/*	if (m_ParamsMgr->PerformFireAllAfterPurge)
        {
            if((Err = FireAllSequence()) != Q_NO_ERROR)
                return Err;
        }  */
    }

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Moving Axis");
    if ((Err = MotorGoToZStartPosition()) != Q_NO_ERROR)
        return Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    if ((Err = PrepareOHDBModelConfig ()) != Q_NO_ERROR)
        return Err;

    if ((Err = PrepareOCBToPrintModel ()) != Q_NO_ERROR)
        return Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    if (m_CurrentMachineState!=msPrinting)
    {
        CQLog::Write(LOG_TAG_PRINT,"Continue-Printing sequence stopped");
        return Q2RT_STOP_DURING_CONTINUE;
    }

    m_SequencerBlock.PrintingWasPaused = false;

    return Q_NO_ERROR;
}


void CMachineSequencer::ChoosePrinterMotorSequencer(void)
{
    //Initiate the distance of the double pass
    // - used only in 600 dpi print and in four heads print

    m_SequencerBlock.DoublePassSpace = 0;
    if (m_ParamsMgr->NozzleTest)
    {
        if(SINGLE_MATERIAL_OPERATION_MODE == m_ParamsMgr->PrintingOperationMode)
        {
            m_PrintMotorSequencer = &CMachineSequencer::NozzlesTestSequencer;
            return;
        }
        else if (DIGITAL_MATERIAL_OPERATION_MODE == m_ParamsMgr->PrintingOperationMode)
        {
            m_PrintMotorSequencer = &CMachineSequencer::NozzlesTestFourHeadSequencer;
            m_SequencerBlock.DoublePassSpace = m_ParamsMgr->YSecondaryInterlaceNoOfPixels;
            return;
        }
        else
            throw  EMachineSequencer("Nozzle test not defined for current number of Heads Printing",
                                     Q2RT_ERROR_IN_PARAMETER_MANAGER);
    }

    if(SINGLE_MATERIAL_OPERATION_MODE == m_ParamsMgr->PrintingOperationMode)
    {
        if(BIDIRECTION_PRINT_MODE != m_ParamsMgr->PrintDirection)
        {
            CQLog::Write(LOG_TAG_PRINT,"Unsupported sequencer type choosed");
            QMonitor.WarningMessage("Unsupported sequencer type choosed"+
                                    QIntToStr(static_cast<int>(m_ParamsMgr->PrintDirection)));
            throw  EMachineSequencer("Unsupported type of print sequencer");
        }
        if(m_ParamsMgr->DPI_InYAxis == 600)
        {
            m_PrintMotorSequencer = &CMachineSequencer::DoubleInterlacePrintSequencer;
            m_SequencerBlock.DoublePassSpace = m_ParamsMgr->YSecondaryInterlaceNoOfPixels;
            return;
        }
        //Verify which type of motor sequence we want to print
        switch(m_ParamsMgr->PrintSequenceVersion)
        {
        case ALTERNATE_SEQUENCER:
            m_PrintMotorSequencer = &CMachineSequencer::Eden260AlternatePrintSequencer;
            break;
        case ALTERNATE_600_DPI_SEQUENCER:
            m_PrintMotorSequencer = &CMachineSequencer::Double300DpiAlternate600PrintSequencer;
            break;
        case EIGHT_HEADS_300_X_2_SEQUENCER:
            m_PrintMotorSequencer = &CMachineSequencer::Eden260AlternatePrintSequencer;
            break;
        default:
            m_PrintMotorSequencer = &CMachineSequencer::Eden260AlternatePrintSequencer;
            break;
        }
    }
    else // 4 heads
    {
        m_SequencerBlock.DoublePassSpace = m_ParamsMgr->YSecondaryInterlaceNoOfPixels;
        m_PrintMotorSequencer = &CMachineSequencer::DoubleInterlacePrintSequencer;
    }
}//ChoosePrinterMotorSequencer


TQErrCode CMachineSequencer::PrintingSequence()
{
    TQErrCode Err;

    m_CurrentMachineState = msPrinting;

    CQLog::Write(LOG_TAG_PRINT,"Printing sequence started");

    CBackEndInterface::Instance()->ResetBumperImpactCounter();

    if (m_ParamsMgr->GenerateExcelAnalyzer)
        CExcelSequenceAnalyzer::Init((Q2RTApplication->AppFilePath.Value() + QString("SequencerAnalyzer.csv")).c_str(), 10, 10);
    else
        CExcelSequenceAnalyzerDummy::Init();

    CExcelSequenceAnalyzerBase *csv = CExcelSequenceAnalyzer::Instance();

    csv->WriteParameters();

    // Flag for electrical power interruption
    m_ParamsMgr->HomeAllIsNeeded = true;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->HomeAllIsNeeded);

    // Get a pointer to the layer process
    CLayerProcess *LayerProcess = Q2RTApplication->GetLayerProcess();

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Wait for first slice");

    for(;;)
    {
        //Luda - for QA testing purposes ONLY
        if((FindWindow(0, "ThrowExceptionUnExpected.txt - Notepad")||
                FindWindow(0, "ThrowExceptionExpected.txt - Notepad") )&& Q2RTApplication->GetQATestsAllowed() )
            throw EMachineManager("for QA purpose exception");

        if ((Err = CheckCartridgesStatus()) != Q_NO_ERROR)
        {
            UpdateLastError(Err);
            throw EMachineSequencer(Err);
        }

        if ((Err = CheckPurgeBath()) != Q_NO_ERROR)
        {
            UpdateLastError(Err);
            throw EMachineSequencer(Err);
        }

        // Get next layer
        TGetLayerResult GetLayerResult;
        CLayer *CurrentLayer = LayerProcess->GetLayer(GetLayerResult);

        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

        // If we shoulg go to pause mode, we expect toget a NULL layer. If the current layer is not
        // NULL, complete printing this layer and check next layer (should be NULL).
        if(m_CurrentMachineState == msPausing)
        {
            if(CurrentLayer == NULL)
                break;
        }
        else
        {
            // Break if done or if we are no longer in printing state
            if(m_CurrentMachineState != msPrinting)
            {
                CQLog::Write(LOG_TAG_PRINT,"Printing Stopped. Last Slice (Machine Sequencer): %d", m_SequencerBlock.LastPrintedSlice);
                csv->WriteDiffCol();
                csv->GenerateCsv();
                break;
            }
        }

        m_LastPrintedSliceMutex.WaitFor();

        if(CurrentLayer == (CLayer *)LAST_SLICE_MARKER)
        {
            FrontEndInterface->UpdateStatus(FE_CURRENT_SLICE,m_SequencerBlock.LastPrintedSlice + 1);
            FrontEndInterface->UpdateStatus(FE_PREVIOUS_JOB_LAST_SLICE,m_SequencerBlock.LastPrintedSlice + 1);

            CQLog::Write(LOG_TAG_PRINT,"Got last slice marker");

            csv->WriteDiffCol();
            csv->GenerateCsv();

            m_LastPrintedSliceMutex.Release();
            return Q2RT_PRINT_COMPLETED;
        }


        // If timeout or unknown error , leave print sequence
        if((GetLayerResult == glrTimeout) || (GetLayerResult == glrUnknownError))
        {
            CQLog::Write(LOG_TAG_PRINT,"Timeout while waiting for layer");
            FrontEndInterface->ErrorMessage("Timeout while waiting for layer");

            m_LastPrintedSliceMutex.Release();
            break;
        }

        // Ignore NULL layers
        if(CurrentLayer == NULL)
        {
            m_LastPrintedSliceMutex.Release();
            continue;
        }

        /* HASP "Leasing" feature check */
        if ((Q2RTApplication->GetMachineType()) != (mtObjet1000))
        {
			try
            {
            // Checking the HASP license directly - not through HaspInterface thread
            if(CurrentLayer->GetSliceNumber() % HASP_CHECK_EACH_N_SLICES == 0)
               CHaspInterfaceWrapper::Instance()->CheckPlugValidityDirectly();
            }
            catch(EHaspInterfaceException &err)
            {
            m_LastPrintedSliceMutex.Release();
            QString str = "Printing was stopped: " + err.GetErrorMsg();
            throw EHaspInterfaceException(str.c_str());
            }
            catch(...)
            {
            m_LastPrintedSliceMutex.Release();
			CQLog::Write(LOG_TAG_PRINT,"Printing sequence: Exception was thrown from HASP thread");
            }
        }

        // Update front-end
        FrontEndInterface->UpdateStatus(FE_CURRENT_SLICE,CurrentLayer->GetSliceNumber());
        FrontEndInterface->UpdateStatus(FE_PREVIOUS_JOB_LAST_SLICE,CurrentLayer->GetSliceNumber());
        FrontEndInterface->UpdateStatus(FE_CURRENT_LAYER,CurrentLayer->GetTotalNumCounter());
        FrontEndInterface->UpdateStatus(FE_CURRENT_MODEL_HEIGHT,m_SequencerBlock.CurrentModelHeight);

        if(dynamic_cast<CEmptyLayer*>(CurrentLayer) != NULL)
        {
            CQLog::Write(LOG_TAG_PRINT,"Got empty slice");
            m_SequencerBlock.LastPrintedSlice = CurrentLayer->GetSliceNumber();
            m_LastPrintedSliceMutex.Release();
            continue;
        }

        // In the first pass we need to start timers
        bool FirstTime = ((CurrentLayer->GetTotalNumCounter() == 0) ||
                          m_SequencerBlock.PrintingWasPaused);


        if((Err = IsPurgeOrFireAllNeeded(FirstTime)) != Q_NO_ERROR)
        {
            UpdateLastError(Err);
            m_LastPrintedSliceMutex.Release();
            throw EMachineSequencer(Err);
        }

        if(m_SequencerBlock.PrintingWasPaused)
        {
            FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Recovery printing");

            // perform continue sequence only when printing was paused
            // from the Embedded and not from the host
            if ((Err = ContinueSequence()) != Q_NO_ERROR)
            {
                m_LastPrintedSliceMutex.Release();
                throw EMachineSequencer(Err);
            }
        }

        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Printing new slice");

        unsigned long LayerStartTime = QGetTicks();

        if ((Err = (this->*m_PrintMotorSequencer)(CurrentLayer)) != Q_NO_ERROR)
        {
            if(Err!= Q2RT_JUMP_CURRENT_LAYER)
            {
				UpdateLastError(Err);
                FrontEndInterface->UpdateStatus(FE_PREVIOUS_JOB_LAST_SLICE,CurrentLayer->GetSliceNumber());
                m_LastPrintedSliceMutex.Release();
                throw EMachineSequencer(Err);
            }
        }

        // User may control the scanning through parameters: NumberOfScanningPasses and ZMoveDistanceBeforeScanning_um
        if (0 != m_ParamsMgr->LayersCountUntilCooling) // '0' indicates that we don't wish to operate between-layers cooling.
            if (0 == ((CurrentLayer->GetSliceNumber()+1) % m_ParamsMgr->LayersCountUntilCooling))
            {
                FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Performing cooling scans");

                if (m_ParamsMgr->UseUVDuringCoolingScanningPasses == false)
                    m_UVLamps->TurnOnOff(false);

                PerformCoolingScanningPhase();

                if (m_ParamsMgr->UseUVDuringCoolingScanningPasses == false)
                    m_UVLamps->TurnOnOff(true);
            }

        // Specific for this Chemistry Branch.
        while ((QGetTicks() - LayerStartTime) < QSecondsToTicks(m_ParamsMgr->AtLeastDelayTimeBetweenLayers))
        {
            //FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Delay between layers (" + QIntToStr((QGetTicks()-LayerStartTime)/1000) + " Sec.)");
            Q2RTApplication->YieldMainThread();
            QSleep (1000);
            if(m_CurrentMachineState != msPrinting)
                break;
        }

        m_SequencerBlock.KeepCookingData.IsPrintedStarted = 1;
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

        // Update total model height
        m_SequencerBlock.CurrentModelHeight += m_SequencerBlock.SingleLayerHeightInMM;
        m_SequencerBlock.LastPrintedSlice = CurrentLayer->GetSliceNumber();

        m_LastPrintedSliceMutex.Release();

        LayerProcess->DoneWithLayer(CurrentLayer);

        CQLog::Write(LOG_TAG_PRINT,"Wait for next Slice");
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Wait for next slice");

    }
    return Q_NO_ERROR;
}

//Sequencer procedures
//--------------------------------------------------------
TQErrCode CMachineSequencer::PausingSequence()
{
    CQLog::Write(LOG_TAG_PRINT,"Pausing sequence started");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Start Pausing");

    TQErrCode Err;
    m_CurrentMachineState = msPausing;
    // Turn Roller OFF
    if ((Err = m_Roller->SetRollerOnOff(ROLLER_OFF)) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_GENERAL,"Set roller off - error");
        return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Scanning");
    if ((Err = PerformScanningCookingPhase()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_MOTORS,"Error during scanning cooking phase:%d",Err);
        return Err;
    }

    // Turn off material cooling fan to off
    if ((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF)) != Q_NO_ERROR)
        return Err;

    m_SequencerBlock.PrintingWasPaused = true;

    if ((Err = MotorPausedSequencer()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_MOTORS,"Error during motor pausing sequence: %d",Err);
        return Err;
    }

    m_Roller->BumperEnableDisable(false);

    // Since we're going to set Head heaters OFF, we should first set head filling Off,
    // Or, we'll get a 'Temp too Low' error.
    if ((Err = m_HeadFilling->HeadFillingOnOff(false)) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    return Q_NO_ERROR;
}


TQErrCode CMachineSequencer::StoppingSequence()
{
    CQLog::Write(LOG_TAG_PRINT,"Stopping sequence started");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Start Stopping");

    TQErrCode Err;
    m_CurrentMachineState = msStopping;

    //Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",
                 m_AmbientTemperature->GetCurrentTemperatureCelcius());

    // Turn Roller OFF
    if ((Err = m_Roller->SetRollerOnOff(ROLLER_OFF)) != Q_NO_ERROR)
    {
        QMonitor.WarningMessage("Set roller on error");
        return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Scanning");
    if ((Err = PerformScanningCookingPhase(false)) != Q_NO_ERROR)
    {
        QMonitor.WarningMessage("Error during scanning cooking phase:"+
                                QIntToStr(static_cast<int>(Err)));

        m_UVLamps->TurnOnOff(false);

        return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    /*   FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Calibrating UV Lamps");

       OpenUVLampsHistoryFile();

       // Calibrate the UV Lamps
       CalibrateUVLampsSequence();

       CloseUVLampsHistoryFile();

       FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    */
    // Turning off UV Lamps
    if((Err = m_UVLamps->TurnOnOff(false)) != Q_NO_ERROR)
        return Err;


    // Turn off material cooling fan to off
    if ((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF)) != Q_NO_ERROR)
        return Err;
    if((Err = m_Actuator->SetHoodFanIdle()) != Q_NO_ERROR)
        return Err;

    // Now purging according to parameter manager
    if (m_ParamsMgr->DoPurgeAfterPrint || m_ParamsMgr->PerformSuperPurgeAfterPrint)
    {
        if((Err = PurgeSequence(PERFORM_HOME, true)) != Q_NO_ERROR)
            return Err;
    }

    if ((Err = m_HeadFilling->HeadFillingOnOff(false)) != Q_NO_ERROR)
        return Err;

    if ((Err = MotorStoppedSequencer()) != Q_NO_ERROR)
    {
        QMonitor.WarningMessage("Error during motor stopped sequencer");
        CQLog::Write(LOG_TAG_MOTORS,"Error during motor stopped sequencer. Error=%d",Err);
        return Err;
    }

    m_Roller->BumperEnableDisable(false);

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::PausedSequence()
{
    TQErrCode Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Start Paused");

    m_CurrentMachineState = msPaused;
    m_HeadStatusSender->DisplayOHDBResetCounter();
    CQLog::Write(LOG_TAG_PRINT,"Paused sequence started");

    //Turns the interlock off
    if((Err = m_Door->Disable())!= Q_NO_ERROR)
        return Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::PausedTimeoutSequence()
{
    TQErrCode Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Timeout Paused");

    m_CurrentMachineState = msPaused;

    CQLog::Write(LOG_TAG_PRINT,"Paused Timeout sequence started");

    if((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF))!= Q_NO_ERROR)
        return Err;

    //Turns the interlock off
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    return Q_NO_ERROR;
}


TQErrCode CMachineSequencer::StoppedSequence()
{
    TQErrCode Err;

    m_SequencerBlock.PrintingWasPaused = false;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Stopped");
    m_CurrentMachineState = msStopped;
    CQLog::Write(LOG_TAG_PRINT,"Stopped sequence started");

    //Verify if lamps are on
    if(m_UVLamps->GetIfTurnIsOn())
		if ((Err = m_UVLamps->TurnOnOff(false)) != Q_NO_ERROR)
            CQLog::Write(LOG_TAG_PRINT,"Turn off Uv Lamps fail");

    FrontEndInterface->UpdateStatus(FE_PREVIOUS_JOB_LAST_SLICE,m_ParamsMgr->SliceNumber.Value());

    CQLog::Write(LOG_TAG_PRINT,"END Print Model");
    CQLog::WriteUnformatted(LOG_TAG_PRINT,"---------------------------------------------------------------------");
    CQLog::Write(LOG_TAG_PRINT,"Statistics");
    m_HeadStatusSender->DisplayOHDBResetCounter();
    m_UVLamps->DisplayStatistics();
    m_TrayHeater->DisplayStatistics();
    m_Actuator->DisplayStatistics();
    m_HeadHeaters->DisplayStatistics();
    m_Vacuum->DisplayStatistics();
    Q2RTApplication->GetLayerProcess()->DisplayStatistics();
    CQLog::WriteUnformatted(LOG_TAG_PRINT,"---------------------------------------------------------------------");

    //Turn interlock OFF
    if ((Err = m_Door->Disable() ) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    return Err;
}


TQErrCode CMachineSequencer::Standby1Sequence(void)
{
    TQErrCode Err;

	m_CurrentMachineState = msStandby1;
	CQLog::Write(LOG_TAG_PRINT,"State:Standby 1");

    // Flag for electrical power interruption
    m_ParamsMgr->HomeAllIsNeeded = false;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->HomeAllIsNeeded);

    // Turn Tray heating OFF
    /*if((Err = m_TrayHeater->SetDefaultTrayStartTemperature()) != Q_NO_ERROR)
      throw EMachineSequencer("Tray Heater set temperature error.");
    if((Err = m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF) ) != Q_NO_ERROR)
      throw EMachineSequencer("Tray Heater turn off error.");
	 */

	// Turn Off register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
	CQLog::Write(LOG_TAG_GENERAL,"Disabling XILINX_REG_PRINT_VOLTAGE");			
	Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, 0x0000);

    if((Err = m_TrayHeater->SetTrayOnOff(TRAY_HEATER_ON) ) != Q_NO_ERROR)
		throw EMachineSequencer("Tray Heater turn ON error.");

	if((Err = m_HeadHeaters->SetDefaultPrintingHeadsVoltages()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadHeaters->GetPrintingHeadsVoltages()) != Q_NO_ERROR)
        return Err;


	if((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_TRAY_HEATERS,"Set heater temperature error - during enter standby");
		return Err;
	}

	if((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
	{
		CQLog::Write(LOG_TAG_HEAD_HEATERS, "Head Heater Switch on error - during enter standby");
		return Err;
	}

    //Turns the interlock off
    if((Err=m_Door->Disable())!= Q_NO_ERROR)
        return Err;

    return Err;
}


TQErrCode CMachineSequencer::Standby2Sequence(void)
{
    TQErrCode Err;

    m_CurrentMachineState = msStandby2;

    //Turns the interlock off
    if((Err=m_Door->Disable())!= Q_NO_ERROR)
        return Err;

    if(m_Container->IsWasteActive())
    {
        //Turn pump gifa OFF
        CQLog::Write(LOG_TAG_PRINT,"Switching off Dirt pumps");
		try
        {
            m_Container->ActivateRollerAndPurgeWaste(false);
        }
        catch(EContainer &e)
        {
            throw EMachineSequencer(e.GetErrorMsg());
        }
    }
    //RSS, itamar added, turn RSS off
    if(m_IsSuctionSystemExist)
    {
        if (m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                m_ParamsMgr->RollerSuctionValveOffTime,false)!=Q_NO_ERROR)
            throw EMachineSequencer("Standby2Sequence: Deactivate RSS message failed.");
    }

    // Turn Tray heating OFF
    if((Err = m_TrayHeater->SetDefaultTrayStartTemperature()) != Q_NO_ERROR)
        throw EMachineSequencer("Tray Heater set temperature error.");
    if((Err = m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF) ) != Q_NO_ERROR)
        throw EMachineSequencer("Tray Heater turn off error.");

    if((Err = m_HeadHeaters->SetStandby2HeateresTemperature()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_HEATERS, "Set heater temperature error - during enter standby 2");
        return Err;
	}

	if((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
	{
		CQLog::Write(LOG_TAG_HEAD_HEATERS, "Head Heater Switch on error - during enter standby 2");
		return Err;
	}

    CQLog::Write(LOG_TAG_PRINT,"State: Standby 2");
    return Err;
}

TQErrCode CMachineSequencer::Standby2ToIdleSequence(void)
{
    TQErrCode Err;

    m_CurrentMachineState = msIdle;
    if(m_Container->IsWasteActive())
    {
        //Turn pump gifa OFF
        CQLog::Write(LOG_TAG_PRINT,"Switching off Dirt pumps");
        try
        {
            m_Container->ActivateRollerAndPurgeWaste(false);
        }
        catch(EContainer &e)
        {
            throw EMachineSequencer(e.GetErrorMsg());
        }
    }
    //RSS, itamar added, turn RSS off
    if(m_IsSuctionSystemExist)
    {
        if (m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                m_ParamsMgr->RollerSuctionValveOffTime,false)!=Q_NO_ERROR)
            throw EMachineSequencer("Deactivate RSS message failed.");
    }

    if(!m_ParamsMgr->KeepHeadsHeated)
        if(m_HeadHeaters->IsHeaterOn())
        {
            CQLog::Write(LOG_TAG_HEAD_HEATERS,"Switching off Head Heaters");
            if((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF)) != Q_NO_ERROR)
                throw EMachineSequencer("Head Heater Switch off error.");
        }

    if(m_TrayHeater->IsTrayOn())
    {
        CQLog::Write(LOG_TAG_TRAY_HEATERS,"Switching off Tray Heater");
        if ((Err = m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF) ) != Q_NO_ERROR)
            throw EMachineSequencer("Set tray heater off message error.");
    }

    //Turns the interlock off
    if((Err=m_Door->Disable())!= Q_NO_ERROR)
        return Err;

    CQLog::Write(LOG_TAG_PRINT,"State: Exit Standby 2");
    return Err;
}


void CMachineSequencer::InitScanningCookingData(void)
{
    m_SequencerBlock.KeepCookingData.XStartPosition     = 0;
    m_SequencerBlock.KeepCookingData.XStopPosition      = 0;
    m_SequencerBlock.KeepCookingData.YPasses            = 0;
    m_SequencerBlock.KeepCookingData.YStartPosition     = 0;
    m_SequencerBlock.KeepCookingData.PrintScanDirection = FORWARD_PRINT_SCAN;
    m_SequencerBlock.KeepCookingData.IsPrintedStarted   = 0;
}

//Keep the parameters used in the end of the print model
// the cooking phase (In this phaze we need to pass more times on
// the model to be sure that all model and support is already solid.
void CMachineSequencer::KeepScanningCookingData(long YStart,
        long XStart,
        long XStop,
        int NoOfPasses,
        int Direction)
{
    if(NoOfPasses > m_SequencerBlock.KeepCookingData.YPasses)
    {
        m_SequencerBlock.KeepCookingData.XStartPosition = XStart;
        m_SequencerBlock.KeepCookingData.XStopPosition = XStop;
        m_SequencerBlock.KeepCookingData.YPasses = NoOfPasses;
        m_SequencerBlock.KeepCookingData.YStartPosition = YStart;
        m_SequencerBlock.KeepCookingData.PrintScanDirection = Direction;
    }
}

TQErrCode CMachineSequencer::PerformScanningCookingPhase (bool TurnUVOff /* = true by default */)
{

    TQErrCode Err;
    int       i;

    if(m_ParamsMgr->NumberOfScanningPasses               == 0 ||
            m_SequencerBlock.KeepCookingData.YPasses          == 0 ||
            m_SequencerBlock.KeepCookingData.IsPrintedStarted == 0)
    {
        //Don't perform Scanning
        CQLog::Write(LOG_TAG_PRINT,"Stopping without scanning");
        if((Err = m_UVLamps->TurnOnOff(false)) != Q_NO_ERROR)
            return Err;

        return Q_NO_ERROR;
    }

    //If Uv lamp is off don't perform scanning
    if(!m_UVLamps->TurnedOn)
        return Q_NO_ERROR;

    //If Uv is marked off don't perform scanning
    if(!m_ParamsMgr->UVLampsEnabled)
        return Q_NO_ERROR;
    CQLog::Write(LOG_TAG_PRINT,"Starting scanning. (%d Times)",m_ParamsMgr->NumberOfScanningPasses.Value());

    //Move down the tray before scanning cooking
    if ((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Z,
               CONFIG_ConvertZumToStep(m_ParamsMgr->ZMoveDistanceBeforeScanning_um))) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,
               m_SequencerBlock.KeepCookingData.XStartPosition)) != Q_NO_ERROR)
        return Err;

    if(m_SequencerBlock.KeepCookingData.PrintScanDirection == BACKWARD_PRINT_SCAN)
        m_SequencerBlock.KeepCookingData.YStartPosition -=
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                         (static_cast<float>(CONFIG_GetHeadsPrintWidht()*(m_SequencerBlock.KeepCookingData.YPasses-1))));

    long      PassesInLayer;  // passes in the same layer
    for(i=0; i < m_ParamsMgr->NumberOfScanningPasses; i++)
    {
        PassesInLayer=0;
        while(PassesInLayer < m_SequencerBlock.KeepCookingData.YPasses)
        {
            long      Location = m_SequencerBlock.KeepCookingData.YStartPosition +
                                 QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                                              static_cast<float>(CONFIG_GetHeadsPrintWidht()*PassesInLayer++));

            if ((Err = m_Motors->PerformBacklashIfNeeded(Location))  != Q_NO_ERROR)
                return Err;
            if ((Err = m_Motors->GoWaitAbsolutePositionSequence (AXIS_Y,Location)) != Q_NO_ERROR)
                return Err;

            if ((Err = m_Motors->JogToAbsolutePositionSequence (AXIS_X,
                       m_SequencerBlock.KeepCookingData.XStopPosition,
                       1,muSteps)) != Q_NO_ERROR)
                return Err;
        }
    }

    if(TurnUVOff)
        if((Err = m_UVLamps->TurnOnOff(false)) != Q_NO_ERROR)
            return Err;

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::PerformCoolingScanningPhase(void)
{

    TQErrCode Err;
    long Location;
    int i;

    if(m_ParamsMgr->NumberOfCoolingScanningPasses == 0 ||
            m_SequencerBlock.KeepCookingData.YPasses          == 0 ||
            m_SequencerBlock.KeepCookingData.IsPrintedStarted == 0)
    {
        //Don't perform Scanning
        CQLog::Write(LOG_TAG_PRINT,"Stopping without scanning");
        return Q_NO_ERROR;
    }

    CQLog::Write(LOG_TAG_PRINT,"Starting cooling scanning. (%d Times)",m_ParamsMgr->NumberOfCoolingScanningPasses.Value());

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,
               m_SequencerBlock.KeepCookingData.XStartPosition)) != Q_NO_ERROR)
        return Err;

    if(m_SequencerBlock.KeepCookingData.PrintScanDirection == BACKWARD_PRINT_SCAN)
        m_SequencerBlock.KeepCookingData.YStartPosition -=
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                         (static_cast<float>(CONFIG_GetHeadsPrintWidht()*(m_SequencerBlock.KeepCookingData.YPasses-1))));
    long PassesInLayer;  // passes in the same layer
    for(i = 0 ; i < m_ParamsMgr->NumberOfCoolingScanningPasses; i++)
    {
        PassesInLayer=0;
        while(PassesInLayer < m_SequencerBlock.KeepCookingData.YPasses)
        {
            Location = m_SequencerBlock.KeepCookingData.YStartPosition +
                       QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                                    static_cast<float>(CONFIG_GetHeadsPrintWidht()*PassesInLayer++));

            if ((Err = m_Motors->PerformBacklashIfNeeded(Location))  != Q_NO_ERROR)
                return Err;
            if ((Err = m_Motors->GoWaitAbsolutePositionSequence (AXIS_Y,Location)) != Q_NO_ERROR)
                return Err;

            if ((Err = m_Motors->JogToAbsolutePositionSequence (AXIS_X,
                       m_SequencerBlock.KeepCookingData.XStopPosition,
                       1,muSteps)) != Q_NO_ERROR)
                return Err;
        }
    }

    return Q_NO_ERROR;
}

// The following procedures are part of the sequencer
// but only have motor commands
//-------------------------------------------------------------
TQErrCode CMachineSequencer::MotorSendTYXHome(void)
{
// This procedures send Motor to home
    TQErrCode Err;

    //Enable All axis
    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;

// Take T,Y,X axis to Home position
    if ((Err = m_Motors->GoHomeSequence(AXIS_T)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_T,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
        return Err;
    //Disable T axis
    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoHomeSequence(AXIS_Y)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoHomeSequence(AXIS_X)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

// The following procedures are part of the sequencer
// but only have motor commands
//-------------------------------------------------------------
TQErrCode CMachineSequencer::MotorSendTYXZHome(void)
{
// This procedures send Motor to home
    TQErrCode Err;

    // First move T, Y & X home...
    if ((Err = MotorSendTYXHome()) != Q_NO_ERROR)
        return Err;

    // Take Z axis to Home position
    if ((Err = m_Motors->GoHomeSequence(AXIS_Z)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

// The following procedures are part of the sequencer
// but only have motor commands - Go home and Calculate and go to Z start position
//-------------------------------------------------------------
TQErrCode CMachineSequencer::MotorPrePrintSequencer() //itamarDeleteJob
{
// This procedures performs the set commands need by the Motor in the start
// of the print model
    TQErrCode Err;

    // Send all axis home
    if ((Err = MotorSendTYXHome()) != Q_NO_ERROR)
        return Err;

    // Step Z Start Position
    if(m_ParamsMgr->StartFromCurrentZPosition)
    {
        CQLog::Write(LOG_TAG_PRINT,"Print from current position");
        m_SequencerBlock.KeepZLocation = m_SequencerBlock.StartZLocation;
    }
    else
    {
        if(m_SequencerBlock.DoZHomeBeforePrint)
        {
            if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
                return Err;
        }

        // Step Prepare Z To Start Printing:  Moving down and up to fix position
        if(m_ParamsMgr->NozzleTest)
            m_SequencerBlock.KeepZLocation = CONFIG_ConvertZumToStep(m_ParamsMgr->NozzleTestHighPrint_um) +
                                             m_SequencerBlock.StartZLocation;
        else
            m_SequencerBlock.KeepZLocation = m_SequencerBlock.StartZLocation;

        // Lower the tray before doing purge
        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
                   m_SequencerBlock.KeepZLocation + m_SequencerBlock.ZBacklash,
                   MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
            return Err;
        CQLog::Write(LOG_TAG_PRINT,"Moving axis Z  to print start position: %d",m_SequencerBlock.KeepZLocation);
        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
                   m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
            return Err;
    }

    m_ParamsMgr->LastSliceZPosition = m_SequencerBlock.KeepZLocation;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->LastSliceZPosition);

    return Q_NO_ERROR;
}

//Update LastSliceZPosition parameter
TQErrCode CMachineSequencer::UpdateLastSliceZPosition()
{
    if(m_ParamsMgr->StartFromCurrentZPosition)
    {
        CQLog::Write(LOG_TAG_PRINT,"Print from current position");
        m_SequencerBlock.KeepZLocation = m_SequencerBlock.StartZLocation;
    }
    else
    {
        // Step Prepare Z To Start Printing:  Moving down and up to fix position
        if(m_ParamsMgr->NozzleTest)
            m_SequencerBlock.KeepZLocation = CONFIG_ConvertZumToStep(m_ParamsMgr->NozzleTestHighPrint_um) +
                                             m_SequencerBlock.StartZLocation;
        else
            m_SequencerBlock.KeepZLocation = m_SequencerBlock.StartZLocation;
    }

    m_ParamsMgr->LastSliceZPosition = m_SequencerBlock.KeepZLocation;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->LastSliceZPosition);

    return Q_NO_ERROR;
}

// The following procedures are part of the sequencer
// but only have motor commands Perform home and go to Z start position
//-----------------------------------------------------------------------
TQErrCode CMachineSequencer::MotorGoToZStartPosition(void)
{
// This procedures performs the set commands need by the Motor in the start
// of the print model
    TQErrCode Err;
    long ZPosition;

    ZPosition = m_SequencerBlock.KeepZLocation + m_SequencerBlock.ZBacklash;
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
               ZPosition,
               MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
               m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}


// This procedure perform some calculation for the current model
// and perform some movememts that should be prepare only before
// the first layer.
//----------------------------------------------------------------
void CMachineSequencer::ZMotorParamsInitBeforeFirstLayer(void)
{
    m_ParamsMgr->SliceNumber = 0;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->SliceNumber);
    m_ParamsMgr->LastSliceZPosition = 0;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->LastSliceZPosition);
    m_ParamsMgr->ModelHeight = 0.0f;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->ModelHeight);
}

TQErrCode CMachineSequencer::MotorInitBeforeFirstLayer(void)
{

    TQErrCode Err;

    ZMotorParamsInitBeforeFirstLayer();
    m_SequencerBlock.LayerHeightDeltaInUM      = 0;
	m_SequencerBlock.HalfLayerHeightDeltaInUM  = 0;
	m_SequencerBlock.CurrentModelHeight        = 0.0f;
	m_SequencerBlock.SingleLayerHeightInUM     = m_ParamsMgr->GetLayerHeight_um();
	m_SequencerBlock.SingleLayerHeightInMM     = m_SequencerBlock.SingleLayerHeightInUM / 1000.0f;
	m_SequencerBlock.SingleHalfLayerHeightInUM = m_ParamsMgr->LayerHeightDPI_um / 2;
	m_SequencerBlock.ZMultiplicator1StepIn_um  = m_ParamsMgr->StepsPerMM[AXIS_Z] / 1000.0f;

    m_SequencerBlock.XLeftSideSpace  =
        max(CONFIG_ConvertXmmToStep(m_ParamsMgr->LeftSideSpace_mm),CONFIG_GetXAccelerationSpace ());
    m_SequencerBlock.XRightSideSpace =
        max(CONFIG_ConvertXmmToStep(m_ParamsMgr->RightSideSpace_mm),CONFIG_GetXDecelerationSpace ());

    m_SequencerBlock.StartYPosition = CONFIG_GetPrintYStartPosition_step (/*m_SequencerBlock.DoublePassSpace*/);
    m_SequencerBlock.ZBacklash = CONFIG_ConvertZumToStep(m_ParamsMgr->Z_BacklashMove_um);

    if(CONFIG_ConvertXmmToStep(m_ParamsMgr->LeftSideSpace_mm) < CONFIG_GetXAccelerationSpace ())
    {
        CQLog::Write(LOG_TAG_GENERAL,"Acceleration Space is bigger");
    }

    if(CONFIG_ConvertXmmToStep(m_ParamsMgr->RightSideSpace_mm) < CONFIG_GetXDecelerationSpace ())
    {
        CQLog::Write(LOG_TAG_GENERAL,"Acceleration Space is bigger ");
    }

    if(m_ParamsMgr->StartFromCurrentZPosition)
    {
        m_SequencerBlock.DoZHomeBeforePrint = false;
        long Position;
        if((Err=m_Motors->SetZPosition(Position)) != Q_NO_ERROR)
        {
            CQLog::Write(LOG_TAG_PRINT,"Start From Current Position:Failed to Set Z Position");
            return Err;
        }

        CQLog::Write(LOG_TAG_PRINT,"Set Z Position");
        m_SequencerBlock.StartZLocation = Position;

        if((Err=m_Motors->GetAxisPosition()) != Q_NO_ERROR)
        {
            FrontEndInterface->ErrorMessage("Error in Z axis position. Z axis must be homed before printing");
            return Err;
        }
    }
    else
    {
        //Set the Z to perform home before Start print
        m_SequencerBlock.StartZLocation = CONFIG_ConvertMMToStep(AXIS_Z,m_ParamsMgr->Z_StartPrintPosition);
        m_SequencerBlock.DoZHomeBeforePrint = true;
    }

    return Q_NO_ERROR;
}

// This procedure perform some calculation for the current model
// and perform some movememts that should be prepare after recovery.
//----------------------------------------------------------------
TQErrCode CMachineSequencer::MotorInitAfterRecovery(void)
{

    TQErrCode Err;

    m_SequencerBlock.LayerHeightDeltaInUM      = 0;
    m_SequencerBlock.HalfLayerHeightDeltaInUM  = 0;
    m_SequencerBlock.SingleLayerHeightInUM     = m_ParamsMgr->GetLayerHeight_um();
    m_SequencerBlock.SingleLayerHeightInMM     = m_SequencerBlock.SingleLayerHeightInUM / 1000.0f;
    m_SequencerBlock.SingleHalfLayerHeightInUM = m_ParamsMgr->LayerHeightDPI_um / 2;
    m_SequencerBlock.ZMultiplicator1StepIn_um  = m_ParamsMgr->StepsPerMM[AXIS_Z] / 1000.0f;

    m_SequencerBlock.CurrentModelHeight = m_ParamsMgr->ModelHeight;

    m_SequencerBlock.XLeftSideSpace  =
        max(CONFIG_ConvertXmmToStep(m_ParamsMgr->LeftSideSpace_mm),CONFIG_GetXAccelerationSpace ());
    m_SequencerBlock.XRightSideSpace =
        max(CONFIG_ConvertXmmToStep(m_ParamsMgr->RightSideSpace_mm),CONFIG_GetXDecelerationSpace ());

    m_SequencerBlock.StartYPosition = CONFIG_GetPrintYStartPosition_step (/*m_SequencerBlock.DoublePassSpace*/);
    m_SequencerBlock.ZBacklash = CONFIG_ConvertZumToStep(m_ParamsMgr->Z_BacklashMove_um);

    if(CONFIG_ConvertXmmToStep(m_ParamsMgr->LeftSideSpace_mm) < CONFIG_GetXAccelerationSpace ())
    {
        CQLog::Write(LOG_TAG_GENERAL,"Acceleration Space is bigger");
    }

    if(CONFIG_ConvertXmmToStep(m_ParamsMgr->RightSideSpace_mm) < CONFIG_GetXDecelerationSpace ())
    {
        CQLog::Write(LOG_TAG_GENERAL,"Acceleration Space is bigger ");
    }

    //Set the start Z print position after recovery
    //We can be here because two reason 1) Pause (Normal - embedded or Job Manager)
    //                                  2) recovery.

    if(m_ParamsMgr->StartFromCurrentZPosition)
    {
        m_SequencerBlock.DoZHomeBeforePrint = false;
        long Position;
        if((Err=m_Motors->SetZPosition(Position)) != Q_NO_ERROR)
        {
            CQLog::Write(LOG_TAG_PRINT,"Start From Current Position:Failed to Set Z Position");
            return Err;
        }

        CQLog::Write(LOG_TAG_PRINT,"Set Z Position");
        m_SequencerBlock.StartZLocation = Position;

        if((Err=m_Motors->GetAxisPosition()) != Q_NO_ERROR)
        {
            FrontEndInterface->ErrorMessage("Error in Z axis position. Z axis must be homed before printing");
            return Err;
        }
    }
    else
    {
        m_SequencerBlock.StartZLocation = m_ParamsMgr->LastSliceZPosition;
        m_SequencerBlock.DoZHomeBeforePrint = true;
    }

    CQLog::Write(LOG_TAG_GENERAL,"Print from Recovery state");

    return Q_NO_ERROR;
}


// Calculate the X start position.
long CMachineSequencer::CalculateXStartPosition(TDPCPCLayerParams *LayerParms)
{
    long XStartPrintPosition = CONFIG_GetPrintXStartPosition_step() +
                               // EndOfPlot positon is in 1200 DPI units. We need to adjust to match to the XStepsPerPixel parametere
                               // which is in 600 DPI units.
                               static_cast<long>(m_ParamsMgr->XStepsPerPixel * static_cast<float>(LayerParms->StartOfPlot)) -
                               m_SequencerBlock.XLeftSideSpace;

    if(XStartPrintPosition<0)
    {
        CQLog::Write(LOG_TAG_PRINT,"XStart position out of range");
        throw EMachineSequencer("X start position is Out of range - probably problem with parameters" \
                                "XStartPrintPosition and LeftSideSpace_mm");
    }

    // Verify which will be the XStart position the calculated above  or
    // the value decided by Mechanic limitation.
    // There was another limitation:Eletronic Home Sensor
    // but at the moment n/a. :
    //   if(m_ParamsMgr->XEletricHomeInStep < XStartPrintPosition)
    //     XStartPrintPosition = m_ParamsMgr->XEletricHomeInStep;

    if (XStartPrintPosition > m_ParamsMgr->MaxXStartPosition)
        XStartPrintPosition = m_ParamsMgr->MaxXStartPosition;

    if(!m_ParamsMgr->XScatterBypass)
    {
        XStartPrintPosition -= rand() % (m_ParamsMgr->MaxXStartPosition - X_SCATTER_MIN_POSITION);
        if(XStartPrintPosition < X_SCATTER_MIN_POSITION)
            XStartPrintPosition = X_SCATTER_MIN_POSITION;
    }

    return XStartPrintPosition;
}

// Calculate the X stop position.
long CMachineSequencer::CalculateXStopPosition(TDPCPCLayerParams *LayerParms, CLayer *Layer)
{

    long XStopPrintPosition =
        CONFIG_GetPrintXStartPosition_step() + m_SequencerBlock.XRightSideSpace +

        // EndOfPlot positon is in 1200 DPI units. We need to adjust to match to the XStepsPerPixel parametere
        // which is in 600 DPI units.
        static_cast<long>(m_ParamsMgr->XStepsPerPixel * static_cast<float>(LayerParms->EndOfPlot));

    if(!m_ParamsMgr->XScatterBypass)
        XStopPrintPosition += rand() % X_SCATTER_RANGE;

    // Correction for working with X relocation test:
    if(m_ParamsMgr->XPegRelocationTest)
    {
        int Relocation = (Layer->GetSliceNumber()/m_ParamsMgr->XPegCounter)*
                         m_ParamsMgr->XPegOffset;

        // convert X pixels to steps:
        Relocation *= m_ParamsMgr->XStepsPerPixel;
        XStopPrintPosition += Relocation;
    }

    // printing area is bigger then tray (in X)
    if (XStopPrintPosition - m_SequencerBlock.XRightSideSpace > m_ParamsMgr->MaxPositionStep[AXIS_X])
    {
        CQLog::Write(LOG_TAG_PRINT,"XStopPrintPosition position out of range.");
        throw EMachineSequencer("XStopPrintPosition is Out of range - probably problem with parameters" \
                                "XStartPrintPosition and LeftSideSpace_mm, or trying printing a too large bitmap.");
    }

    if (XStopPrintPosition > m_ParamsMgr->MaxPositionStep[AXIS_X])
        XStopPrintPosition = m_ParamsMgr->MaxPositionStep[AXIS_X];

    return XStopPrintPosition;
}

// Calculate the current height for the layer
float CMachineSequencer::CalculateLayerHeight(void)
{
    float TmpValue = (m_SequencerBlock.SingleLayerHeightInUM + m_SequencerBlock.LayerHeightDeltaInUM) *
                     m_SequencerBlock.ZMultiplicator1StepIn_um;

    float TmpValue2 = static_cast<int>(TmpValue);

    m_SequencerBlock.LayerHeightDeltaInUM = (TmpValue - TmpValue2) / m_SequencerBlock.ZMultiplicator1StepIn_um;

    return (TmpValue2 / m_SequencerBlock.ZMultiplicator1StepIn_um);
}

// Calculate the current half height for the layer
float CMachineSequencer::CalculateHalfLayerHeight(void)
{
    float TmpValue = (m_SequencerBlock.SingleHalfLayerHeightInUM + m_SequencerBlock.HalfLayerHeightDeltaInUM) *
                     m_SequencerBlock.ZMultiplicator1StepIn_um;

    float TmpValue2 = static_cast<int>(TmpValue);

    m_SequencerBlock.HalfLayerHeightDeltaInUM = (TmpValue - TmpValue2) / m_SequencerBlock.ZMultiplicator1StepIn_um;

    return (TmpValue2 / m_SequencerBlock.ZMultiplicator1StepIn_um);
}

// Print a single layer
TQErrCode CMachineSequencer::PrepareTrayToPrintCurrentLayer(CLayer *Layer, bool LayerSeparation)
{
    long Location;
    TQErrCode Err;
    long SliceZPosition;

    //Calculate layer current height
    m_SequencerBlock.CurrentLayerHeightInUM = CalculateLayerHeight();

    CQLog::Write(LOG_TAG_MOTORS,"Prepare Motor to print current layer");

    // Prepare the tray to the next layer
    // -----------------------------------
    SliceZPosition = m_ParamsMgr->LastSliceZPosition;
    m_SequencerBlock.KeepZLocation = SliceZPosition +
                                     CONFIG_ConvertZumToStep(m_ParamsMgr->Z_MoveToStartNewLayer_um);

    // We need to move down the space of Parameter Z_MoveToStartNewLayer but
    // we need to move more (and fix back) because Mechanism limitation.
    Location = m_SequencerBlock.KeepZLocation + m_SequencerBlock.ZBacklash;
    if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
        return Err;

    // Now return back
    if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
              m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
        return Err;

    //keep for Recovery
    m_ParamsMgr->SliceNumber = (Layer->GetSliceNumber());
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->SliceNumber);
    if (LayerSeparation == false)
    {
        m_ParamsMgr->LastSliceZPosition = SliceZPosition + CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM);
        if (IsCurrSliceAccumulating())
        {
            m_ParamsMgr->LastSliceZPosition += CONFIG_ConvertZumToStep(m_ParamsMgr->SmartRollerWasteThickness);
        }
        m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->LastSliceZPosition);
    }
    m_ParamsMgr->ModelHeight = m_SequencerBlock.CurrentModelHeight;
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->ModelHeight);

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::IsPurgeOrFireAllNeeded (bool StartPrinting)
{
// The bool StartPrinting meaning that stopper for next purge
// should be initiated-or-reinitiated.
// The boolean is true in the start printing (first layer)
// or when the stopper for next purge should be re-started
// After each periodic time we re-initiate the time counting.
    static time_t PurgingTime;
    static time_t FireAllTime;
    time_t CurrentTime;
    TQErrCode Err;

    CurrentTime = QGetCurrentTime();
    if (StartPrinting)
    {
        PurgingTime = CurrentTime + (time_t)m_ParamsMgr->TimeBetweenPurgesInSec;
        FireAllTime = CurrentTime + (time_t)m_ParamsMgr->TimeBetweenFireAllInSec;
        return Q_NO_ERROR;
    }

    if (CurrentTime >= PurgingTime )
    {
        // Now purging and perform home only for MCB and not for Quadra
        if ((Err = PurgeSequence(PERFORM_HOME, true)) != Q_NO_ERROR)
            return Err;

        // Now fire all
        if (m_ParamsMgr->PerformFireAllAfterPurge)
        {
		 /*   if ((Err = FireAllSequence()) != Q_NO_ERROR)
                return Err;*/
            // mark the time for the next purge
            FireAllTime = CurrentTime + m_ParamsMgr->TimeBetweenFireAllInSec;
        }

        // mark the time for the next purge
        PurgingTime = CurrentTime + m_ParamsMgr->TimeBetweenPurgesInSec;

        return Q_NO_ERROR;
    }

    if (CurrentTime >= FireAllTime)
    {
        // Now fire all
        if ((Err = FireAllSequence()) != Q_NO_ERROR)
            return Err;

        // mark the time for the next purge
        FireAllTime = CurrentTime + m_ParamsMgr->TimeBetweenFireAllInSec;
    }

    return Q_NO_ERROR;
}


TQErrCode CMachineSequencer::MotorPausedSequencer(void)
{
    TQErrCode Err;
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Motor Stopping sequence");

    long PosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,muMM);
    if ((Err = m_Motors->PerformBacklashIfNeeded(PosInSteps)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,m_ParamsMgr->MotorsPurgeXStartPosition,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
               m_SequencerBlock.KeepZLocation))!= Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::MotorStoppedSequencer(void)
{
    TQErrCode Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Motor Stopped sequence");

    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
        return Err;

    //If it already in home position then there is no need to go to position=0.
    if(m_ParamsMgr->TAxisEndPrintPosition.Value() != m_Motors->GetAxisLocation(AXIS_T))
    {
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T, MOTOR_ENABLE)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_T,
                   m_ParamsMgr->TAxisEndPrintPosition)) != Q_NO_ERROR)
            return Err;
    }

    if(m_ParamsMgr->StartFromCurrentZPosition)
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
               CONFIG_ConvertMMToStep(AXIS_Z,m_ParamsMgr->ZAxisEndPrintPosition),
               MORE_TIME_TO_FINISH_MOVEMENT))!= Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;


    return Q_NO_ERROR;
}


TQErrCode CMachineSequencer::CancelPrePrintSequence(void)
{
    CQLog::Write(LOG_TAG_PRINT,"Cancel PrePrint sequence");

    m_HeadHeaters->Cancel();
    m_HeadFilling->Cancel();
    m_TrayHeater->Cancel();
    m_UVLamps->Cancel();
    m_Door->Cancel();

    return Q_NO_ERROR;
}


TQErrCode CMachineSequencer::StopDuringPrePrintSequence(void)
{
    CQLog::Write(LOG_TAG_PRINT,"Stop PrePrint sequence");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Stop during Pre-Print");

    TQErrCode Err;

    if ((Err = m_UVLamps->TurnOnOff(false) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_HeadFilling->HeadFillingOnOff(false) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Roller->SetRollerOnOff(ROLLER_OFF) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Roller->BumperEnableDisable(false) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF) ) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE) ) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::CancelHeadHeating(void)
{
    m_HeadHeaters->Cancel();
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::CancelPrint(void)
{
    CQLog::Write(LOG_TAG_PRINT,"Cancel print");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Cancel Print");

    m_Door->Cancel();
    m_HeadHeaters->Cancel();
    m_UVLamps->Cancel();
    m_TrayHeater->Cancel();
    m_HeadFilling->Cancel();

    m_UVLamps->TurnOnOff(false);
    m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
    m_HeadFilling->HeadFillingOnOff(false);
    m_Roller->SetRollerOnOff(ROLLER_OFF);
    m_Roller->BumperEnableDisable(false);
    m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
    m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF);

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::KillSequencer(void)
{
    CQLog::Write(LOG_TAG_PRINT,"Kill sequencer");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Kill sequencer");

    m_UVLamps->TurnOnOff(false);
    m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
    m_HeadFilling->HeadFillingOnOff(false);
    m_Roller->SetRollerOnOff(ROLLER_OFF);
    m_Roller->BumperEnableDisable(false);
    m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
    m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF);
    m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE);
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::Shutdown(void)
{
    //TQErrCode Err = Q_NO_ERROR;

	FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Shutdown");
	CBackEndInterface::Instance()->EnableDisableMaintenanceCounter(RESTART_COMPUTER_COUNTER_ID, false);
    try
    {
        TTankIndex DrainPumps[NUMBER_OF_CHAMBERS];
        CQLog::Write(LOG_TAG_PRINT,"Shutdown - Switch off devices");
        m_Door->UnlockDoor();
        m_UVLamps->TurnOnOff(false);
        if(!m_ParamsMgr->KeepHeadsHeated)                         // todo -oNobody -cNone: keep heads heated when shutting down ?  where else ?
            m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);

        m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
        m_HeadFilling->HeadFillingOnOff(false);

        m_Roller->SetRollerOnOff(ROLLER_OFF);
        m_Roller->BumperEnableDisable(false);
        m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF);
		
		// Turn Off register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
		CQLog::Write(LOG_TAG_GENERAL,"Disabling XILINX_REG_PRINT_VOLTAGE");		
		Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, 0x0000);
		
        try
        {
            m_Container->ActivateRollerAndPurgeWaste(false);
        }
        catch(EContainer &e)
        {
        }
        //RSS, itamar added, turn RSS off
        if(m_IsSuctionSystemExist)
        {
            m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                                                    m_ParamsMgr->RollerSuctionValveOffTime,false);
        }
        else
        {
            if (!m_ParamsMgr->DrainBypass)
            {
                for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
                    DrainPumps[i] = NO_TANK;
                m_Container->SetDrainTanks(DrainPumps, false);
            }
        }
        m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE);

        if(!m_ParamsMgr->PowerOffBypass || m_IgnorePowerByPass)
        {
            CQLog::Write(LOG_TAG_PRINT,"Shutdown - Power off");
            m_Power->SetPowerOnOff(POWER_OFF);
            m_Power->WaitForPowerOnOff();
        }
    }
    catch(...)
    {
        CQLog::Write(LOG_TAG_PRINT,"Error during Shutdown");
    }

    CQLog::Write(LOG_TAG_PRINT,"Shutdown");

    return Q_NO_ERROR;
}

void CMachineSequencer::StopErrorDevice(TQErrCode LastError)
{
    CQLog::Write(LOG_TAG_PRINT,"Handle error:%s",PrintErrorMessage(LastError).c_str());
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Stop error");
    switch(LastError)
    {
    case Q2RT_HEAD_HEATER_THERMISTOR_SHORT:
	case Q2RT_HEAD_HEATER_THERMISTOR_OPEN:
	case Q2RT_HEAD_MAIN_FAN_SPEED_CRITICAL:
        m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
        break;

    case Q2RT_HEADS_FILLING_TIMEOUT :
    case Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_HIGH:
    case Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_LOW:
    case Q2RT_HEADS_FILLING_MONITORING_ERROR :
    case Q2RT_HEADS_FILLING_THERMISTOR_OPEN:
    case Q2RT_HEADS_FILLING_THERMISTOR_SHORT:
        FrontEndInterface->UpdateStatus(FE_HEAD_FILLING_MONITOR_STATUS, 0, true);
        m_HeadFilling->HeadFillingOnOff(false);
        break;

    case Q2RT_TRAY_HEATER_ERROR:
    case Q2RT_TRAY_HEATER_THERMISTOR_OPEN:
    case Q2RT_TRAY_HEATER_THERMISTOR_SHORT:
    case Q2RT_TRAY_HEATER_THERMISTOR_OUT_OF_RANGE:
        m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
        break;

    case Q2RT_ROLLER_FAILURE_ERROR:
        m_Roller->SetRollerOnOff(ROLLER_OFF);
        break;

    default:
        break;
    }
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
}
void CMachineSequencer::LogOHDB_FIFOStatus()
{
	WORD Status = CBackEndInterface::Instance()->ReadDataFromOHDBXilinx(0x05);

	enum
	{
		AlmostEmpty,
		AlmostFull,
		Empty,
		Full
	};

	QString StatusStr = "";

	if((Status >> AlmostEmpty) & 1)
		StatusStr += " Almost empty";
	if((Status >> AlmostFull) & 1)
		StatusStr += " Almost full";
	if((Status >> Empty) & 1)
		StatusStr += " Empty";
	if((Status >> Full) & 1)
		StatusStr += " Full";
	if(Status == 5)
	{
	CQLog::Write(LOG_TAG_PRINT, QFormatStr("OHDB FPGA FIFO status: %s (%d)", StatusStr.c_str(), Status).c_str());
	}
	if((Status >> Full) & 1)
		throw EMachineSequencer(QFormatStr("OHDB FPGA FIFO status: %s (%d)", StatusStr.c_str(), Status).c_str());
}

//------------------------------------------------------------
//                            OHDB
//-------------------------------------------------------------
// Commands preformed by OHDB during printing and pre-printing
TQErrCode CMachineSequencer::PrepareOHDBModelConfig (void)
{
    TQErrCode Err;

    m_Roller->EnableDisableBumperPeg(true);
    m_Roller->SetDefaultBumperParams();
    // Stop fire mechanism in OHDB
    m_PrintControl->Stop();

    // Stop fire mechanism in PCI
    FIFOPCI_NoGo();

    m_AmbientTemperature->SetOHDBAmbientParams();

	m_PrintControl->ResetDriverStateMachine();
    m_PrintControl->ApplyDefaultPrintParams();

    // Set parameters for job configuration (should be called once per job)
    m_PrintControl->SetDefaultConfigParams ();

    // Turn Roller ON
    if ((Err = m_Roller->SetDefaultRollerParms()) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Roller->SetRollerOnOff(ROLLER_ON)) != Q_NO_ERROR)
        return Err;

    m_AmbientTemperature->SetMaterialCoolingFansDutyCycle(m_ParamsMgr->MaterialCoolingFans_PWM);
    if((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_ON)) != Q_NO_ERROR)
        return Err;

    if(m_ParamsMgr->AmbientControlByPass)
        if((Err = m_Actuator->SetHoodFanOnOff(true)) != Q_NO_ERROR)
            return Err;

    m_Roller->BumperEnableDisable(true);

    return Q_NO_ERROR;
}

//Check if pass is empty - in order to skip pass without data
void CMachineSequencer::CalcIsPassNotEmpty ()
{
    if(!m_ParamsMgr->PassMask)
        return;

    unsigned YPassMask = m_Layer->GetPassMask();
    unsigned TempMask  = (YPassMask >> m_PassInLayer);

    m_PassIsNotEmpty   = TempMask & 1;
    if(m_PassInLayer==0 && m_PassIsNotEmpty==false)
    {
        CQLog::Write(LOG_TAG_PRINT,"Error: PassInLayer 0 is empty In Slice:%d",m_Layer->GetSliceNumber());
        m_PassIsNotEmpty = true;
    }
}

//Check if pass is empty - in order to skip pass without data
bool CMachineSequencer::CheckIfPassMaskIsNotEmpty (CLayer *Layer,
        int PassNumber)
{
    if(!m_ParamsMgr->PassMask)
        return true;

    unsigned YPassMask = Layer->GetPassMask();
    unsigned TempMask = (YPassMask >> PassNumber);
    bool PassIsNotEmpty = TempMask & 1;
    if(PassNumber==0 && PassIsNotEmpty==false)
    {
        CQLog::Write(LOG_TAG_PRINT,"Error: PassNumber 0 is empty In Slice:%d",Layer->GetSliceNumber());
        PassIsNotEmpty = true;
    }

    return PassIsNotEmpty;
}


// Commads preformed by OHDB during printing and pre-printing
TQErrCode CMachineSequencer::PrepareOHDBLayerConfig (CLayer *Layer,
        TDPCPCLayerParams *LayerParms,
        int PEGOffset)
{
    // Three things: 1- call to print control
    //               2- call to FIFO
    //               3- GO

    long StartPEG_step;
    int StartPEG,EndPEG; //,NoOfFires;

    CQLog::Write(LOG_TAG_PRINT,"Prepare OHDB layer configuration.");

    StartPEG_step = (CONFIG_GetPrintXStartPosition_step() - m_ParamsMgr->XEncoderHomeInStep);
    StartPEG = static_cast<int>(static_cast<float>(StartPEG_step)/m_ParamsMgr->XStepsPerPixel) +
               LayerParms->StartOfPlot;
    EndPEG        = StartPEG + (LayerParms->EndOfPlot - LayerParms->StartOfPlot + PEGOffset);

    if(m_ParamsMgr->XPegRelocationTest)
    {
        int Relocation = (Layer->GetSliceNumber()/m_ParamsMgr->XPegCounter)*
                         m_ParamsMgr->XPegOffset;
        StartPEG += Relocation;
        EndPEG   += Relocation;

        CQLog::Write(LOG_TAG_POWER,"Start Peg = %d End Peg =%d",StartPEG,EndPEG);
    }

	CQLog::Write(LOG_TAG_PROCESS,"Start Peg = %d End Peg =%d Num of Fires =%d",StartPEG,EndPEG,LayerParms->NoOfFires);
	m_PrintControl->SetLayerParams(StartPEG, EndPEG, LayerParms->NoOfFires);

    return Q_NO_ERROR;
}

// Commads preformed by OHDB before each pass
TQErrCode CMachineSequencer::PrepareOHDBPassConfig (CLayer *Layer,
        TDPCPCLayerParams *LayerParms,
        int PassNumber,
		int SubPassNumber)
{
	TQErrCode Err;
    LogOHDB_FIFOStatus();
    //Advance fire test.
    if(m_ParamsMgr->AdvanceFireTest && PassNumber)
		if((Err = PrepareOHDBLayerConfig(Layer, LayerParms, PassNumber)) != Q_NO_ERROR)
            return Err;

    FIFOPCI_ResetBufferAddress();
    FIFOPCI_NoGo();

    //Reset state machine
//    m_PrintControl->ResetDriverStateMachine(); // resetting before each pass is too much (iddan.kalo)

    int BufferLength;

    // NUM_OF_BYTES_SINGLE_HEAD is the amount of bytes required for a single head
    if(m_ParamsMgr->PrintDirection == BIDIRECTION_PRINT_MODE)
		BufferLength = LayerParms->NoOfFires * 2 * 2 * NUM_OF_BYTES_SINGLE_HEAD;
    else
		BufferLength = LayerParms->NoOfFires * 2 * NUM_OF_BYTES_SINGLE_HEAD;


	FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Write Layer Data");

	int BufferIndex = 0;

    // 300/150 DPI in Y printing
	if((SINGLE_MATERIAL_OPERATION_MODE == m_ParamsMgr->PrintingOperationMode) && (m_ParamsMgr->DPI_InYAxis != 600))
    {
		BufferIndex = BufferLength * 4 * PassNumber;

		if (Q_NO_ERROR != (Err=FIFOPCI_WriteAsync(m_SequencerBlock.BufferLayerPrintPtr + BufferIndex, static_cast<unsigned>(BufferLength))))
			return Err;

	}
	else
	{
		BufferIndex = BufferLength * 4 * (PassNumber * 2 + SubPassNumber);

		// 600 DPI in Y printing
		if (Q_NO_ERROR != (Err=FIFOPCI_WriteAsync(m_SequencerBlock.BufferLayerPrintPtr + BufferIndex, static_cast<unsigned>(BufferLength))))
			return Err;
	}

	if(m_ParamsMgr->DumpPassToFile || FindWindow(0, "DumpPassToFile.txt - Notepad"))
    {
        QString FileName;
        FILE *out = NULL;
		
		
		QString Path = m_ParamsMgr->DumpPassToFilePath;
		ForceDirectories(Path.c_str());
		FileName = Path + "\\" + "Hardware_Pass_" + QIntToStr(PassNumber + 1) + "_Dump_" + QIntToStr(m_BufferNumber);
        out = fopen(FileName.c_str(), "wb");

		fwrite(m_SequencerBlock.BufferLayerPrintPtr + BufferIndex, sizeof(BYTE), BufferLength * sizeof(DWORD), out);

        m_BufferNumber++;

        if (out)
            fclose(out);
	}    

	FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Printing Pass " + QIntToStr(PassNumber + 1));

    //GO
    m_PrintControl->Go();

    return Q_NO_ERROR;
}

// Perform end of pass status check
TQErrCode CMachineSequencer::CheckOCBEndOfPassStatus (void)
{
    TQErrCode Err;

    //Verify if there are Uv lamps problem
    if(!(m_UVLamps->GetTurnStatus()))
    {
        CQLog::Write(LOG_TAG_UV_LAMPS,"Uv lamps is OFF");
        if((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;
        if((Err = m_UVLamps->EnableDuringPrint()) != Q_NO_ERROR)
            return Err;

        if ((Err = MotorSendTYXHome()) != Q_NO_ERROR)
            return Err;

        if(!m_ParamsMgr->StartFromCurrentZPosition)
        {
            if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
                return Err;

            if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
                       m_ParamsMgr->LastSliceZPosition,
                       MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
                return Err;
        }

        return Q2RT_JUMP_CURRENT_LAYER;
    }

    return Q_NO_ERROR;
}

// Check if data was transferred ok to OHDB and Perform Status check
TQErrCode CMachineSequencer::CheckEndOfPass (void)
{
    TQErrCode Err;

    bool FIFONotEmpty = CheckForFIFOEmpty();

    // Stop fire mechanism in PCI
    FIFOPCI_NoGo();

    //Stop OHDB
    m_PrintControl->Stop();

    // Check if X axis pass through the index, if not send X axis home
    if(CHECK_NOT_EMULATION(m_ParamsMgr->OHDB_Emulation))
    {
        WORD IndexTest = (WORD)(Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(ELECTRIC_HOME_REG_ADDR) & 0x0001);
        CQLog::Write(LOG_TAG_GENERAL,"Index test result = %d",IndexTest);
        if (!IndexTest)
        {
			m_PrintControl->ResetDriverStateMachine();
            if((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
                return Err;
        }
    }
    if(FIFONotEmpty)
        return Q2RT_FIFO_NOT_EMPTY_ERROR;

    if((Err=CheckOCBEndOfPassStatus()) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

void CMachineSequencer::CheckEndOfPassAndNotifyError(void)
{
    TQErrCode Err = CheckEndOfPass();

    if (Err != Q_NO_ERROR)
    {
        if (Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
        {
            CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice%d.",
                         m_Layer->GetSliceNumber());
            throw EMachineSequencer("CheckEndOfPass Error",Err);
        }
    }

}

// Check if data was transferred ok to OHDB
TQErrCode CMachineSequencer::CheckEndOfLayer (void)
{
    TQErrCode Err;

    CheckForFIFOEmpty();

    //Stop OHDB
    m_PrintControl->Stop();

    // Stop fire mechanism in PCI
    FIFOPCI_NoGo();

    // Check if X axis pass through the index, if not send X axis home
    if(CHECK_NOT_EMULATION(m_ParamsMgr->OHDB_Emulation))
    {
        WORD IndexTest = (WORD)(Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(ELECTRIC_HOME_REG_ADDR) & 0x0001);
        CQLog::Write(LOG_TAG_GENERAL,"Index test result = %d",IndexTest);
        if (!IndexTest)
        {
			m_PrintControl->ResetDriverStateMachine();
            if((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
                return Err;
        }
    }
    if((Err=CheckOCBEndOfPassStatus()) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

//------------------------------------------------------------
//                            OCB
//------------------------------------------------------------
// Commands preformed by OCB during pre-printing
TQErrCode CMachineSequencer::PrepareOCBToPrintModel(void)
{
    m_AirFlow->SetupAirFlowParams();

    //Set tray to printing termperature
    return m_TrayHeater->SetDefaultTrayPrintingTemperature();
}

// Commands preformed by OCB during printing
TQErrCode CMachineSequencer::CheckStatusDuringPrinting(void)
{
    TQErrCode Err;

    CQLog::Write(LOG_TAG_PRINT,"Check Status during print.");

    // Verify the heads temperatures are ok.
    if((Err = m_HeadHeaters->CheckThermistorsStatus(true)) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_HEATERS,"CheckStatusDuringPrinting detect heads temperature error");
        return Err;
    }

    //Check if Head filling monitor is active
    if (!(m_HeadFilling->GetOnOff()))
    {
        //Activate it
        CQLog::Write(LOG_TAG_HEAD_FILLING,"CheckOCBDuringPrinting:Heads filling monitor was not active - activate it");
        if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
            return Err;
        if((Err = m_HeadFilling->HeadFillingOnOff(true)) != Q_NO_ERROR)
            return Err;
    }

    if((Err = m_HeadFilling->GetIfThermistorsAreWorking()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"CheckOCBDuringPrinting sequence detect error in Head fillings - not filled");
        return Err;
    }

    return Q_NO_ERROR;
}


// -------------------------------------------------------
TQErrCode CMachineSequencer::CheckPurgeBath(void)
{
    if (!(m_ParamsMgr->PurgeBathEnabled))
    {
        return Q_NO_ERROR;
    }

    if (m_Actuator->GetStabilizedPurgeBathSensorStatus())
    {
        CQLog::Write(LOG_TAG_PRINT,"CMachineSequencer::CheckPurgeBath - purge bath overflow detected");
        return Q2RT_PURGE_BATH_OVERFLOW;
    }

    return Q_NO_ERROR;
}


// -------------------------------------------------------
TQErrCode CMachineSequencer::CheckParameters(void)
{
    //Verify Nozzles Tests parameters
    if(m_ParamsMgr->NozzleTest)
    {
        if(m_ParamsMgr->PrintDirection != BIDIRECTION_PRINT_MODE)
        {
            CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Print direction is not marked as BIDERECTION_PRINT_MODE");
            throw EMachineSequencer("Nozzles test: Print direction is not marked as BIDERECTION_PRINT_MODE ",
                                    Q2RT_ERROR_IN_PARAMETER_MANAGER);
        }
        if(m_ParamsMgr->ScatterEnabled)
        {
            CQLog::Write(LOG_TAG_PRINT,"Nozzles test don't accept Scatter enabled option");
            throw EMachineSequencer("Nozzles test don't accept Scatter enabled option",
                                    Q2RT_ERROR_IN_PARAMETER_MANAGER);
        }
    }

    //Verify if parameters to print is OK
    if(m_ParamsMgr->TrayStartPositionX < m_ParamsMgr->LeftSideSpace_mm)
    {
        throw EMachineSequencer("TrayStartPositionX mm must be greather" \
                                " then LeftSideSpace_mm",Q2RT_ERROR_IN_PARAMETER_MANAGER);
    }

    if(m_ParamsMgr->StartFromCurrentZPosition)
        if(!QMonitor.AskYesNo("Eden is configured to start print from current Z Position. " \
                              "Do you want to continue (Yes), or do you want to continue print without this option(No)."))
        {
            CQLog::Write(LOG_TAG_GENERAL,"Disable option: StartFromCurrentZPosition");
            m_ParamsMgr->StartFromCurrentZPosition = false;
            m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->StartFromCurrentZPosition);
        }
        else
            CQLog::Write(LOG_TAG_GENERAL,"Print with option: StartFromCurrentZPosition");

    QString Msg;
    if (m_ParamsMgr->IsPrintingAllowed(Msg) == false)
        throw EResinReplacementNotCompleted(Msg,Q2RT_MRW_ISNT_COMPLETED);

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::GotoHeadInspectionPositionSequencer(void)
{
    TQErrCode Err;
    bool bGoToLowerPosition = false;

    CQLog::Write(LOG_TAG_GENERAL,"Go to head inspection position");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");


    if(QMonitor.AskYesNo("Is there any model on the tray?"))
    {
        //GoToLowerPosition
        bGoToLowerPosition = true;
    }
    if(m_ParamsMgr->RemovableTray)
    {
        if((Err = m_TrayPlacer->VerifyTrayInserted(true)) != Q_NO_ERROR)
        {
            return Err;
        }
    }
    //Open Door - OCB
    if((Err=m_Door->Enable())!= Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;


    long PosInSteps;
    if(bGoToLowerPosition)
    {
        // Z axis - go to lower position
        CQLog::Write(LOG_TAG_GENERAL,"Go to Tray Lower Position Sequencer during Head Inspection Position Sequencer");
        PosInSteps = CAppParams::Instance()->MaxPositionStep[AXIS_Z] -  CAppParams::Instance()->ZMaxPositionMargin;
        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,PosInSteps,MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        {
            return Err;
        }
    }

    //Home X, Y and Z
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
        return Err;

    if(!bGoToLowerPosition)
    {
        if(!m_Motors->GetAxisIfHomeWasPerformed(AXIS_Z))
        {
            if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
            {
                return Err;
            }
        }
        CQLog::Write(LOG_TAG_GENERAL,"Go to ZCleanHeadsPosition during Head Inspection Position Sequencer");
        PosInSteps = CONFIG_ConvertZmmToStep(CAppParams::Instance()->ZCleanHeadsPosition);
        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,PosInSteps,MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        {
            return Err;
        }
    }


    PosInSteps = CONFIG_ConvertXmmToStep(CAppParams::Instance()->XCleanHeadsPosition);
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,PosInSteps,MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        return Err;

    PosInSteps = CONFIG_ConvertYmmToStep(CAppParams::Instance()->YCleanHeadsPosition);
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,PosInSteps,MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    CQLog::Write(LOG_TAG_GENERAL,"Go to head inspection position done");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    return Q_NO_ERROR;
}

//-------------------------------------------------
TQErrCode CMachineSequencer::GotoTrayRemovalPositionSequencer(void)
{
    TQErrCode Err;

    CQLog::Write(LOG_TAG_GENERAL,"Go to tray upper position");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Tray upper position");
    if(m_ParamsMgr->RemovableTray)
    {
        if((Err = m_TrayPlacer->VerifyTrayInserted(true)) != Q_NO_ERROR)
        {
            return Err;
        }
    }
    //Open Door - OCB
    if((Err=m_Door->Enable())!= Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
        return Err;

    if(!m_Motors->GetAxisIfHomeWasPerformed(AXIS_Z))
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
               CONFIG_ConvertMMToStep(AXIS_Z,m_ParamsMgr->Z_StartPrintPosition), MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    CQLog::Write(LOG_TAG_GENERAL,"Go to tray upper position done");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    return Q_NO_ERROR;
}
//-------------------------------------------------
TQErrCode CMachineSequencer::GotoTrayLowerPositionSequencer(void)
{
    TQErrCode Err;

    CQLog::Write(LOG_TAG_GENERAL,"Go to tray Lower position");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Tray lower position");
    if(m_ParamsMgr->RemovableTray)
    {
        if((Err = m_TrayPlacer->VerifyTrayInserted(true)) != Q_NO_ERROR)
        {
            return Err;
        }
    }
    //Open Door - OCB
    if((Err=m_Door->Enable())!= Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;


    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
        return Err;

    if(!m_Motors->GetAxisIfHomeWasPerformed(AXIS_Z))
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;

    CBackEndInterface::Instance()->MoveZAxisDownToLowestPosition();
    CBackEndInterface::Instance()->WaitForEndOfMovement(AXIS_Z, Z_DOWN_WAIT_TIME);

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    CQLog::Write(LOG_TAG_GENERAL,"Go to tray lower position done");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    return Q_NO_ERROR;
}

// --------------------------------------------------------------------------------
TQErrCode CMachineSequencer::GoToPurgeEx(bool PerformHome,bool DisableAxisT, bool AvoidContactWithBasket) // 'AvoidContactWithBasket' -  send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads
{
    TQErrCode Err;
    bool PerformYHome = true;

    CQLog::Write(LOG_TAG_GENERAL,"GoTo Purge position");

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;

    if (m_CurrentMachineState == msPrinting ||
            m_CurrentMachineState == msPausing ||
            m_CurrentMachineState == msStopping)
    {
        if((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF)) != Q_NO_ERROR)
            return Err;
        if(m_CurrentMachineState == msPrinting)
            PerformYHome = m_Motors->GetIfCanPerformHomeDuringPrint();
    }

    // Home for T,Y,X
    //if home was already peformed on Axis T, don't perform it again
    if( m_Motors->GetAxisIfHomeWasPerformed(AXIS_T) == false )
    {
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
            return Err;
    }

    if(PerformHome)
    {
        if(PerformYHome)
            if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
                return Err;
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;
    }

    // see if a backlash movement is needed due to a small movement (and perform it)
    long PosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,muMM);
    if ((Err = m_Motors->PerformBacklashIfNeeded(PosInSteps)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,m_ParamsMgr->MotorsPurgeXStartPosition,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->OpenWasteTankEx(AvoidContactWithBasket)) != Q_NO_ERROR)
        return Err;

    if (PerformHome && DisableAxisT)
    {
        //we are not in the sequencer therefore disable T axes
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
            return Err;
    }

    CQLog::Write(LOG_TAG_GENERAL,"GoTo Purge done");

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::GoToPurge(bool PerformHome,bool DisableAxisT)
{
    return GoToPurgeEx(PerformHome, DisableAxisT, false); // 'false' - do not avoid contact of new purge unit Basket with heads (the Basket needs to touch the heads to prevent messy purges)
}

TQErrCode CMachineSequencer::RemotePerformWipe(bool PerformStartCommands,bool GoHomeAfterWipe)
{
    return PerformWipe(PerformStartCommands, GoHomeAfterWipe);
}

TQErrCode CMachineSequencer::PerformWipe(bool PerformStartCommands, bool GoHomeAfterWipe)
{
    TQErrCode Err;

    CQLog::Write(LOG_TAG_GENERAL,"Performing Wipe");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Perform Wipe");

    //Enable Axis if it is not enable
    if (!m_Motors->IsMotorEnabled(AXIS_T))
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_ENABLE)) != Q_NO_ERROR)
            return Err;

    if (!m_Motors->IsMotorEnabled(AXIS_Y))
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_Y,MOTOR_ENABLE)) != Q_NO_ERROR)
            return Err;

    if (!m_Motors->IsMotorEnabled(AXIS_X))
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_X,MOTOR_ENABLE)) != Q_NO_ERROR)
            return Err;

    if (PerformStartCommands)
    {
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->GoHomeSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->GoHomeSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
            return Err;

        if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,m_ParamsMgr->MotorsPurgeXStartPosition,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
            return Err;
    }

    if ((Err = m_Motors->GoToPreWipePosition()) != Q_NO_ERROR) //In order that the basket won't touch the heads before moving Y Axis (this used to cause failed nozzles) we move it to pre-wipe position.
        return Err;

    //Now move Y to wiper start position:
    long YPosInSteps = CONFIG_ConvertYmmToStep(m_ParamsMgr->MotorPurgeYActPosition);

    // see if a backlash movement is needed due to a small movement (and perform it)
    if ((Err = m_Motors->PerformBacklashIfNeeded(YPosInSteps)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosInSteps)) != Q_NO_ERROR)
        return Err;

    //Put Y in purging-wipe velocity
    if((Err=m_Motors->SetVelocity(AXIS_Y,m_ParamsMgr->MotorPurgeYVelocity,muMM))!= Q_NO_ERROR)
        return  Err;

    //Put T in act position WIPER
    if ((Err = m_Motors->WipeWasteTank()) != Q_NO_ERROR)
        return Err;

    //Now move Y to wiper clean the head from giffa.
    YPosInSteps = CONFIG_ConvertYmmToStep(m_ParamsMgr->MotorPurgeYActPosition + MOTOR_PURGE_DELTA_MOVE_MM);

    // Note: we don't check for Backlash move here, since we may assume that MOTOR_PURGE_DELTA_MOVE_MM is large enough
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosInSteps,MORE_TIME_TO_FINISH_MOVEMENT)) != Q_NO_ERROR)
        return Err;

    //Close T axis
    // if ((Err = m_Motors->CloseWasteTank()) != Q_NO_ERROR)
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
        return Err;

    if (PerformStartCommands)
        if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
            return Err;

    //Return Y to printing velocity
    if((Err=m_Motors->SetVelocity(AXIS_Y,m_ParamsMgr->MotorsVelocity[AXIS_Y],muMM))!= Q_NO_ERROR)
        return  Err;

    if (GoHomeAfterWipe)
    {
        if ((Err = m_Motors->GoHomeSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->GoHomeSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
            return Err;
        if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y,WAIT_MOTOR_HOME_MORE_TIME_SEC)) != Q_NO_ERROR)
            return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");
    CQLog::Write(LOG_TAG_GENERAL,"Wipe done");
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::PerformBrushWipe()
{
	TQErrCode Err;

	CQLog::Write(LOG_TAG_GENERAL,"Perform Brush Wipe");
	FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Perform Brush Wipe");

	//Enable Axis if it is not enable
	if (!m_Motors->IsMotorEnabled(AXIS_Y))
	  if ((Err = m_Motors->SetMotorEnableDisable(AXIS_Y,MOTOR_ENABLE)) != Q_NO_ERROR)
		 return Err;

	if (!m_Motors->IsMotorEnabled(AXIS_X))
	  if ((Err = m_Motors->SetMotorEnableDisable(AXIS_X,MOTOR_ENABLE)) != Q_NO_ERROR)
		 return Err;

	//Now move X, Y to special wiper start position:
	long XPosInSteps   = /*CONFIG_ConvertXmmToStep*/(m_ParamsMgr->BrushWipeXStartPositionMM);
	long YPosInSteps   = /*CONFIG_ConvertYmmToStep*/(m_ParamsMgr->BrushWipeYStartPositionMM);
	long DeltaYInSteps = /*CONFIG_ConvertYmmToStep*/(m_ParamsMgr->BrushWipeDeltaYMoveMM);
	long WipeTimes     = m_ParamsMgr->BrushWipeWipeTimes;

	if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosInSteps,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
			return Err;

	if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XPosInSteps,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
			return Err;

	//Put Y in purging-wipe velocity
	if((Err=m_Motors->SetVelocity(AXIS_Y,m_ParamsMgr->MotorPurgeYVelocity,muMM))!= Q_NO_ERROR)
	  return  Err;

	for (int i = 0; i < WipeTimes; i++)
	{
		if ( i%2 == 0 )
		{
			if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosInSteps+DeltaYInSteps,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
			return Err;
		}
		else
		{
        	if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,YPosInSteps,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
			return Err;
        }
	}

	//Return Y to printing velocity
	if((Err=m_Motors->SetVelocity(AXIS_Y,m_ParamsMgr->MotorsVelocity[AXIS_Y],muMM))!= Q_NO_ERROR)
	  return  Err;

	return Q_NO_ERROR;
}


// Purge sequence
TQErrCode CMachineSequencer::LongPurgeSequence(bool PerformHome, bool PerformSinglePurgeInSequence)
{
    TQErrCode Err;
    bool TurnOnFillingsMonitor=false;
    bool PurgeError = false;

    CQLog::Write(LOG_TAG_HEAD_FILLING,"Perform Purge");

    //Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",
                 m_AmbientTemperature->GetCurrentTemperatureCelcius());

    m_PurgeStopped = false;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Go To Purge");
	if ((Err = GoToPurge(PerformHome,DONT_DISABLE_AXIS_T)) != Q_NO_ERROR)
		return Err;
  /*
   	// First move T, Y & X home...
   if ((Err = MotorSendTYXHome()) != Q_NO_ERROR)
	  return Err;
   if ((m_ParamsMgr->BrushWipeBeforePurge) || (m_ParamsMgr->BrushWipeAfterPurge))
   {
	m_PurgeIntervalsToPerformBrushWipe++;
   }
   if (m_ParamsMgr->BrushWipeBeforePurge)
   {
	if ( m_PurgeIntervalsToPerformBrushWipe == m_ParamsMgr->BrushWipePerformEveryXPurgeTimes)
	{
		PerformBrushWipe();
	}
   }
   
    if ((Err = GoToPurge(PerformHome,DONT_DISABLE_AXIS_T)) != Q_NO_ERROR)
		return Err;
		*/
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Purge");

    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    if (m_CurrentMachineState == msPrinting ||
            m_CurrentMachineState == msPrePrint  ||
            m_CurrentMachineState == msStopping ||
            m_CurrentMachineState == msPausing)
    {
        //Check if Head filling monitor is active
        if (!(m_HeadFilling->GetOnOff()))
        {
            //Activate it
            TurnOnFillingsMonitor=true;
            CQLog::Write(LOG_TAG_HEAD_FILLING,"Heads filling monitor was not active - activate it");
            FrontEndInterface->NotificationMessage("Purge sequence detect Heads filling monitor was not active");
        }
    }
    else
    {
        if ((Err = m_HeadHeaters->SetDefaultHeateresTemperature()) != Q_NO_ERROR)
            return Err;

        if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
            return Err;
    }

    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Purge - Warming Heads");

    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    // Verify the heads temperatures are ok.
    if((Err = m_HeadHeaters->VerifyIfTemperatureOk()) != Q_NO_ERROR)
        return Err;

    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Purge");

    if ((m_CurrentMachineState != msPrinting &&
            m_CurrentMachineState != msPrePrint &&
            m_CurrentMachineState != msStopping &&
            m_CurrentMachineState != msPausing) ||
            TurnOnFillingsMonitor)
    {
        if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
            return Err;
        if((Err = m_HeadFilling->HeadFillingOnOff(true)) != Q_NO_ERROR)
            return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Purge - Filling Heads");

    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge sequence detect error in Head fillings - not filled");
        return Err;
    }
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Purge");

    //Turn pump gifa ON
    try
    {
        m_Container->ActivateRollerAndPurgeWaste(true);
    }
    catch(EContainer &e)
    {
        return Q2RT_CONTAINERS_WASTE_FULL;
    }

    //RSS, itamar added, turn RSS on
    if(m_IsSuctionSystemExist)
    {
        if ((Err = m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                   m_ParamsMgr->RollerSuctionValveOffTime,true))!= Q_NO_ERROR)
            return Err;
    }
    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    // Verify the heads temperatures are ok.
    if((Err = m_HeadHeaters->VerifyIfTemperatureOk()) != Q_NO_ERROR)
        return Err;

    if(m_PurgeStopped)
        return Q2RT_STOP_DURING_PURGE;

    m_DuringPurge = true;

    m_HeadStatusSender->m_CanSendIsHeadTemperatureOkMessage = true;

    //Head in the purge location start purging
    int purges = PerformSinglePurgeInSequence ? 1 : (m_ParamsMgr->NumOfPurgesInSequence) ;
	int WaitMS = m_ParamsMgr->WaitBetweenPurgesTimeMS;
    int wipes = m_ParamsMgr->WipesAfterPurge;

    /* Super Purge conditions */

    bool SuperPurge = false;
    if( 	m_ParamsMgr->EnableSuperPurge && // super purge enabled
            m_CurrentMachineState == msPrinting && // the machine is printing
            Q2RTApplication->GetMachineManager()->GetJobOrigin() == JobFromHost	) // this is a HOST print job
    {
        m_ParamsMgr->PurgeCyclicCounter++;

        CQLog::Write(LOG_TAG_GENERAL,"Purge Cyclic Counter = %d", m_ParamsMgr->PurgeCyclicCounter.Value());

        // check if this sequence should be a super purge sequence
        if( m_ParamsMgr->TimeBetweenSuperPurgesInMinutes <= (m_ParamsMgr->PurgeCyclicCounter * (m_ParamsMgr->TimeBetweenPurgesInSec/60)) )
        {
            SuperPurge = true;
        }
    }

    if( 	m_ParamsMgr->EnableSuperPurge && // super purge enabled
            m_CurrentMachineState == msStopping && // the machine is stopping a print job
            m_ParamsMgr->PerformSuperPurgeAfterPrint && // we need to perform a super purge after a print job
            Q2RTApplication->GetMachineManager()->GetJobOrigin() == JobFromHost	) // this is a HOST print job
    {
        SuperPurge = true;
    }

    if( SuperPurge )
    {
        purges = m_ParamsMgr->SuperPurge_NumOfPurges;
        wipes = m_ParamsMgr->SuperPurge_NumOfWipes;
        m_ParamsMgr->PurgeCyclicCounter = 0;
    }

	m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->PurgeCyclicCounter);

    /* end of Super Purge conditions */

    for (int i = 0; i < purges; i++)
    {

        if( i != 0 )   // Sleep for 'WaitBetweenPurgesTimeMS' only between Purges
        {
			QSleep(WaitMS);
            //CQLog::Write(LOG_TAG_GENERAL,"Waiting for %d ms", WaitMS);
		}

		CQLog::Write(LOG_TAG_GENERAL,"%sPerforming Purge [%d/%d]", (SuperPurge)?"Super Purge Sequence: ":"", i+1, purges);

		if ((Err = m_Purge->PerformDefaultPurge()) != Q_NO_ERROR)
		{
			PurgeError = true;
			break;
		}
	}
	if(m_ParamsMgr->DisableVacuumInPurge == true)
	{
	// Ignore vacuum errors
	CBackEndInterface::Instance()->EnableDisableVacuumErrorHandling(false);
	QSleep(m_ParamsMgr->WaitBeforeVacuumOffTimeMS);
		//Turn Vacuum OFF
	if ((Err = m_Actuator->SetOnOff(ACTUATOR_ID_VACUUM_VALVE,0) ) != Q_NO_ERROR)
		throw EMachineSequencer("Actuator message error.");
	}
    m_HeadStatusSender->m_CanSendIsHeadTemperatureOkMessage = false;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"");

    QSleepSec(m_ParamsMgr->MotorPostPurgeTimeSec);
    m_DuringPurge=false;

    for (int i = 0; i<wipes; i++)
    {
        CQLog::Write(LOG_TAG_GENERAL,"%sPerforming Wipe [%d/%d]", (SuperPurge)?"Super Purge Sequence: ":"", i+1, wipes);
        if((Err=PerformWipe(false))!= Q_NO_ERROR)
            return  Err;
	}
		if(m_ParamsMgr->DisableVacuumInPurge == true)
	{
    	if ((Err = m_Actuator->SetOnOff(ACTUATOR_ID_VACUUM_VALVE,1) ) != Q_NO_ERROR)
		throw EMachineSequencer("Actuator message error.");
		CBackEndInterface::Instance()->EnableDisableVacuumErrorHandling(true);
    }
		//Fire All after purge
	if (m_ParamsMgr->PerformFireAllAfterPurge)
		if((Err = FireAllSequence()) != Q_NO_ERROR)
				CQLog::Write(LOG_TAG_PRINT,"FireAll(after purge) problem ,Error = %d",Err);

     
    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
		return Err;

   if (m_ParamsMgr->BrushWipeAfterPurge)
   {
   	if ( m_PurgeIntervalsToPerformBrushWipe == m_ParamsMgr->BrushWipePerformEveryXPurgeTimes)
   	{
   		PerformBrushWipe();
   	}
   }

   if ((m_ParamsMgr->BrushWipeBeforePurge) || (m_ParamsMgr->BrushWipeAfterPurge))
   {
		if ( m_PurgeIntervalsToPerformBrushWipe == m_ParamsMgr->BrushWipePerformEveryXPurgeTimes)
		{
			m_PurgeIntervalsToPerformBrushWipe = 0;
		}
   }

    if(m_CurrentMachineState == msPrinting)
    {
        m_AmbientTemperature->SetMaterialCoolingFansDutyCycle(m_ParamsMgr->MaterialCoolingFans_PWM);
        if((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_ON)) != Q_NO_ERROR)
            return Err;
    }

    CQLog::Write(LOG_TAG_HEAD_FILLING,"Purge sequence done");

    if(PurgeError)
        return Q2RT_FATAL_ERROR; //Q2RT_PURGE_NOT_FINISHED;

    return Err;
}

TQErrCode CMachineSequencer::PurgeSequence(bool PerformHome, bool PerformSinglePurgeInSequence )
{
	if (!m_HeadFilling->IsHeadFillingAllowed())
	{
		CQLog::Write(LOG_TAG_HEAD_FILLING,"A request for PurgeSequence was aborted. Heads-Filling is currently not allowed due to Containers status.");
		throw EHeadFilling(QFormatStr(LOAD_STRING(IDS_HEAD_FILLING_IS_NOT_ALLOWED), LOAD_STRING(IDS_RR_WIZARD)));
    }
    TQErrCode Err = LongPurgeSequence(PerformHome, PerformSinglePurgeInSequence);

    if( Err == Q2RT_TEMPERATURE_IS_NOT_OK_IN_OCB ||
            Err == Q2RT_PURGE_ERROR)
	{
		CQLog::Write(LOG_TAG_PRINT,"Try to Perform Purge Again,Last Error = %d",Err);

        //Retry -> Tray again
		Err = LongPurgeSequence(PerformHome, PerformSinglePurgeInSequence);
    }
	m_AmbientTemperature->SetAmbientHeaterDuringPurge(false);

 /*
	//Fire All after purge
	if (m_ParamsMgr->PerformFireAllAfterPurge)
		if((Err = FireAllSequence()) != Q_NO_ERROR)
				CQLog::Write(LOG_TAG_PRINT,"FireAll(after purge) problem ,Error = %d",Err);
     */
	return Err;
}

// Print a single layer
TQErrCode CMachineSequencer::FireAll(void)
{
// Local variables
// ---------------
    TQErrCode Err;
	bool TurnOffFPGAPrintingBit;

// Code
// ----
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All");

    // Set parameters for job configuration (should be called once per job)
    m_PrintControl->SetDefaultConfigParams ();

	if(m_CurrentMachineState == msPrinting ||
			m_CurrentMachineState == msPrePrint ||
			m_CurrentMachineState == msStopping ||
			m_CurrentMachineState == msPausing)
	{
		TurnOffFPGAPrintingBit = false;
	}
	else
	{
		TurnOffFPGAPrintingBit = true;

		// Turn On register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
		CQLog::Write(LOG_TAG_GENERAL,"Enabling XILINX_REG_PRINT_VOLTAGE");
		Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, XILINX_PRINTING_ENABLED);
	}

// Send message fire all
    if ((Err = m_PrintControl->FireAll()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"Fire all sent message fail");
        QMonitor.WarningMessage("Fire all sent message fail");
        return Err;
    }

// Wait for fire all finish
    if((Err=m_PrintControl->WaitForFireAllFinished()) != Q_NO_ERROR)
        return Err;

    //Wait for end of fire all
    QSleep(m_ParamsMgr->PostFireAllTime_ms);

    if(CHECK_NOT_EMULATION(m_ParamsMgr->OHDB_Emulation))
    {
		if (TurnOffFPGAPrintingBit)
		{
			// Turn Off register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
			CQLog::Write(LOG_TAG_GENERAL,"Disabling XILINX_REG_PRINT_VOLTAGE");
			Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, 0x0000);
		}

        //Verify if OHDB - Xilinx diagnostic register is ok
        WORD DiagRegister = Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(4);
        bool SentMessageOnlyOnce = true;
        int Counter = 0;
        while(DiagRegister != 0)
        {
            CQLog::Write(LOG_TAG_PRINT,"Wrong value in Xilinx diagnostic register=%X",DiagRegister);

            Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(4,0);
            DiagRegister = Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(4);

            if(SentMessageOnlyOnce)
            {
                SentMessageOnlyOnce=false;
                FrontEndInterface->NotificationMessage("Wrong value in Xilinx diagnostic register");
            }

            if (Counter++ > 100)
            {
                FrontEndInterface->NotificationMessage("Xilinx error - Recovery, after close program and Reset OHDB");
                CQLog::Write(LOG_TAG_PRINT,"Xilinx error after Fire all");
                return Q2RT_FATAL_ERROR;
            }
        }
    }

    CQLog::Write(LOG_TAG_HEAD_FILLING,"Fire all done");
    return Q_NO_ERROR;
}
/*--------------------------------------------------------------------------*/
// Print a single layer
TQErrCode CMachineSequencer::FireAll(unsigned int head,int numOfFires,int qualityMode)
{
// Local variables
// ---------------
    TQErrCode Err;

// Code
// ----
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All with head mask");

    // Set parameters for job configuration (should be called once per job)
    m_PrintControl->SetDefaultConfigParams ();

// Send message fire all
    if ((Err = m_PrintControl->FireAll(head,numOfFires,qualityMode)) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"Fire all sent message fail");
        QMonitor.WarningMessage("Fire all sent message fail");
        return Err;
    }

// Wait for fire all finish
    if((Err=m_PrintControl->WaitForFireAllFinished()) != Q_NO_ERROR)
        return Err;

    //Wait for end of fire all
    QSleep(m_ParamsMgr->PostFireAllTime_ms);

    if(CHECK_NOT_EMULATION(m_ParamsMgr->OHDB_Emulation))
    {
        //Verify if OHDB - Xilinx diagnostic register is ok
        WORD DiagRegister = Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(4);
        bool SentMessageOnlyOnce = true;
        int Counter = 0;
		while(DiagRegister != 0)
        {
            CQLog::Write(LOG_TAG_PRINT,"Wrong value in Xilinx diagnostic register=%X",DiagRegister);

            Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(4,0);
            DiagRegister = Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(4);

            if(SentMessageOnlyOnce)
            {
                SentMessageOnlyOnce=false;
                FrontEndInterface->NotificationMessage("Wrong value in Xilinx diagnostic register");
            }

            if (Counter++ > 100)
            {
                FrontEndInterface->NotificationMessage("Xilinx error - Recovery, after close program and Reset OHDB");
                CQLog::Write(LOG_TAG_PRINT,"Xilinx error after Fire all");
                return Q2RT_FATAL_ERROR;
            }
        }
    }

    CQLog::Write(LOG_TAG_HEAD_FILLING,"Fire all done");
    return Q_NO_ERROR;
}

/*-----------------------------------------------------------------------------*/
// Print a single layer
// Print a single layer
TQErrCode CMachineSequencer::FireAllSequence(void)
{
    TQErrCode Err;
	bool PerformHome, TurnOffHeadHeater, TurnOffFPGAPrintingBit;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All");
    CQLog::Write(LOG_TAG_HEAD_FILLING,"Perform Fire all");

    m_FireAllStopped=false;

    //Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",
                 m_AmbientTemperature->GetCurrentTemperatureCelcius());

    if (m_CurrentMachineState == msPrinting ||
            m_CurrentMachineState == msPrePrint ||
            m_CurrentMachineState == msStopping ||
            m_CurrentMachineState == msPausing)
    {
        PerformHome = false;
        TurnOffHeadHeater = false;
		TurnOffFPGAPrintingBit = false;
    }
    else
    {
        //If not during printing perform home
        if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
            return Err;

		// Turn On register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
	    CQLog::Write(LOG_TAG_GENERAL,"Enabling XILINX_REG_PRINT_VOLTAGE");	
		Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, XILINX_PRINTING_ENABLED);

		//If not during printing perform home
		PerformHome            = true;
        TurnOffHeadHeater = true;
		TurnOffFPGAPrintingBit = true;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All-Warming Heads");

    if(m_FireAllStopped)
        return Q2RT_STOP_DURING_FIRE_ALL;

    // Verify the heads temperatures are ok.
    CQLog::Write(LOG_TAG_GENERAL,"FireAllSequence() calls WaitForHeadsTemperatureOK() 6");
    if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
        return Err;

    //Turn Heads liquid monitoring ON
    if (TurnOffHeadHeater)        //If not during printing
    {
        if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
            return Err;
        if((Err = m_HeadFilling->GetIfThermistorsAreWorking()) != Q_NO_ERROR)
            return Err;
        if((Err = m_HeadFilling->HeadFillingOnOff(true)) != Q_NO_ERROR)
            return Err;
    }

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All-Filling Heads");

    if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Go To Fire All position");
    if ((Err = GoToPurgeEx(PerformHome,DONT_DISABLE_AXIS_T, true)) != Q_NO_ERROR) // 'True' will send T axis to a lower point which the basket (of the new Purge unit) does not touch the heads
        return Err;

    if(m_FireAllStopped)
        return Q2RT_STOP_DURING_FIRE_ALL;
    if ((Err = FireAll()) != Q_NO_ERROR)
        return Err;

    //Close T axis
    //if ((Err = m_Motors->CloseWasteTank()) != Q_NO_ERROR)
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    if (m_CurrentMachineState == msPrinting)
    {
        m_AmbientTemperature->SetMaterialCoolingFansDutyCycle(m_ParamsMgr->MaterialCoolingFans_PWM);
        if((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_ON)) != Q_NO_ERROR)
            return Err;
    }
    if (TurnOffHeadHeater)
    {
        m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
        m_HeadFilling->HeadFillingOnOff(false);
    }

	if (TurnOffFPGAPrintingBit)
	{
		// Turn Off register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
		CQLog::Write(LOG_TAG_GENERAL,"Disabling XILINX_REG_PRINT_VOLTAGE");
		Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, 0x0000);
	}

    m_HeadFilling->HeadFillingOnOff(false);
    return Q_NO_ERROR;
}
/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
// Print a single layer
TQErrCode CMachineSequencer::FireAllSequence(unsigned int head,int numOfFires,int qualityMode)
{
    TQErrCode Err;
    bool /*PerformHome,*/ TurnOffHeadHeater;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All");
    CQLog::Write(LOG_TAG_HEAD_FILLING,"Perform Fire all");

    m_FireAllStopped=false;

    //Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",
                 m_AmbientTemperature->GetCurrentTemperatureCelcius());
    //If not during printing perform home
    // PerformHome = true;
    if ((Err = m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_ON)) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All-Warming Heads");

    if(m_FireAllStopped)
        return Q2RT_STOP_DURING_FIRE_ALL;

    // Verify the heads temperatures are ok.
    if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
        return Err;

    //Turn Heads liquid monitoring ON
    if((Err = m_HeadFilling->SetDefaultParms()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadFilling->GetIfThermistorsAreWorking()) != Q_NO_ERROR)
        return Err;
    if((Err = m_HeadFilling->HeadFillingOnOff(true)) != Q_NO_ERROR)
        return Err;

    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All-Filling Heads");

    if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
        return Err;

//  FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Go To Fire All position");
//  if ((Err = GoToPurge(PerformHome,DONT_DISABLE_AXIS_T)) != Q_NO_ERROR)
//     return Err;

    if(m_FireAllStopped)
        return Q2RT_STOP_DURING_FIRE_ALL;

	// Turn On register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
	CQLog::Write(LOG_TAG_GENERAL,"Enabling XILINX_REG_PRINT_VOLTAGE");
	Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, XILINX_PRINTING_ENABLED);

    if ((Err = FireAll(head,numOfFires,qualityMode)) != Q_NO_ERROR)
        return Err;

	// Turn Off register 0x07 bit 0.   Bit value: 0 = Idle; 1 = Printing
	CQLog::Write(LOG_TAG_GENERAL,"Disabling XILINX_REG_PRINT_VOLTAGE");
	Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(XILINX_REG_PRINT_VOLTAGE, 0x0000);

    //Close T axis
//  if ((Err = m_Motors->CloseWasteTank()) != Q_NO_ERROR)
    //    return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

        m_HeadFilling->HeadFillingOnOff(false);
    return Q_NO_ERROR;
}

// Perform fire all after purge during printing
TQErrCode CMachineSequencer::FireAllAfterPurgeSequence(void)
{
    TQErrCode Err;

    CQLog::Write(LOG_TAG_HEAD_FILLING,"Perform Fire all after purge");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Fire All after purge");

    //Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",
                 m_AmbientTemperature->GetCurrentTemperatureCelcius());

    // Verify the heads temperatures are ok.
    if((Err = m_HeadHeaters->WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
        return Err;

    if((Err = m_HeadFilling->WaitForFilledHeadContainer()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"FireAll sequence:Head fillings:Wait to be filled error");
        return Err;
    }

    long PosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,muMM);
    if ((Err = m_Motors->PerformBacklashIfNeeded(PosInSteps)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,EXTRA_WAIT_TIME_FOR_END_OF_MOVE,muMM)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;
    //if this function is called, the tank will open without the basket (of the new purge unit) touching the heads
    if ((Err = m_Motors->OpenWasteTankEx(true)) != Q_NO_ERROR)
        return Err;

    if ((Err = FireAll()) != Q_NO_ERROR)
        return Err;

    //Close T axis
    //if ((Err = m_Motors->CloseWasteTank()) != Q_NO_ERROR)
    if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}


// Check if the data FIFO is empty (return true if empty and write to log file)
bool CMachineSequencer::CheckForFIFOEmpty(void)
{
    if(FindWindow(0, "FifoEmpty.txt - Notepad") && Q2RTApplication->GetQATestsAllowed() )
    {
        CQLog::Write(LOG_TAG_PRINT,"FIFO is not empty - for QA Test");
        return true;
    }
    if(CHECK_EMULATION(m_ParamsMgr->DataCard_Emulation))
        return false;

    if(!FIFOPCI_IsEmpty())
    {
        // fifo is not empty
        ULONG *BufferPtr = new unsigned long [9000];

        int NoOfRd=FIFOPCI_ReadLeftDWORDS(BufferPtr,9000);

        CQLog::Write(LOG_TAG_PRINT,"FIFO is not empty (%d DWORDS)",NoOfRd);

        delete  [] BufferPtr;

        FIFOPCI_CancelWriteAsync();
        m_PrintControl->ResetDriverStateMachine();

        return true;
    }

    return false;
}


// Return a pointer to the motors instance
CMotorsBase* CMachineSequencer::GetMotorsInstance()
{
    return m_Motors;
}

// Return a pointer to the UV Lamps instance
CUvLamps* CMachineSequencer::GetUVLampsInstance()
{
    return m_UVLamps;
}

// Return a pointer to the tray instance
CTrayBase* CMachineSequencer::GetTrayHeaterInstance()
{
    return m_TrayHeater;
}

CTrayBase* CMachineSequencer::GetTrayPlacerInstance()
{
    return m_TrayPlacer;
}

// Return a pointer to the power instance
CPowerBase* CMachineSequencer::GetPowerInstance()
{
    return m_Power;
}

// Return a pointer to the purge instance
CPurgeBase* CMachineSequencer::GetPurgeInstance()
{
    return m_Purge;
}

// Return a pointer to the door instance
CDoorBase* CMachineSequencer::GetDoorInstance()
{
    return m_Door;
}

CSignalTower* CMachineSequencer::GetSignalTower()
{
    return m_SignalTower;
}

// Return a pointer to the roller instance
CRollerBase* CMachineSequencer::GetRollerInstance()
{
    return m_Roller;
}

// Return a pointer to the head filling instance
CHeadFillingBase* CMachineSequencer::GetHeadFillingInstance()
{
    return m_HeadFilling;
}

// Return a pointer to the head heaters instance
CHeadHeatersBase* CMachineSequencer::GetHeadHeatersInstance()
{
    return m_HeadHeaters;
}

// Return a pointer to the License Manager instance
CLicenseManager* CMachineSequencer::GetLicenseManager()
{
    return m_LicenseManager;
}

// Return a pointer to the ambient temperature instance
CAmbientTemperatureBase* CMachineSequencer::GetAmbientTemperatureInstance()
{
    return m_AmbientTemperature;
}

// Return a pointer to the actuator instance
CActuatorBase* CMachineSequencer::GetActuatorInstance()
{
    return m_Actuator;
}


//Return a pointer to the EOL instance
CContainerBase* CMachineSequencer::GetContainerInstance()
{
    return m_Container;
}

// Return a pointer to the print control instance
CPrintControl* CMachineSequencer::GetPrintControlInstance()
{
    return m_PrintControl;
}

// Return a pointer to the Head Vacuum instance
CHeadVacuumBase * CMachineSequencer::GetHeadVacuumInstance()
{
    return m_Vacuum;
}

// Return a pointer to the Head Vacuum instance
CFansBase * CMachineSequencer::GetFansInstance()
{
    return m_Fans;
}

COCBStatusSender *CMachineSequencer::GetOCBStatusInstance()
{
    return m_OCBStatusSender;
}

CHeadStatusSender *CMachineSequencer::GetHeadStatusInstance()
{
    return m_HeadStatusSender;
}

// Return the number of the last printed slice
int CMachineSequencer::GetLastPrintedSliceNum(void)
{
    int LastPrintedSlice;

    m_LastPrintedSliceMutex.WaitFor();
    LastPrintedSlice = m_SequencerBlock.LastPrintedSlice;
    m_LastPrintedSliceMutex.Release();

    return LastPrintedSlice;
}


// Return the number of the last printed slice (might have a slight impreciseness)
int CMachineSequencer::GetLastSliceNum(void)
{
    return m_SequencerBlock.LastPrintedSlice;
}


void CMachineSequencer::ResetLastPrintedSliceNum(void)
{
    m_SequencerBlock.LastPrintedSlice = -1;
}

float CMachineSequencer::GetCurrentModelHeight(void)
{
    return m_SequencerBlock.CurrentModelHeight;
}

void CMachineSequencer::CheckCartridgeExistence(bool& CheckIsDone)
{
    // Reset the 'm_CartridgeErrorDlgCloseEvent' event
    m_CartridgeErrorDlgCloseEvent.WaitFor(0);
    for (int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; i++)
    {
        if (!m_Container->IsTankInAndEnabled( m_Container->GetActiveTankNum( (TChamberIndex) i) ) )
        {
            CQLog::Write(LOG_TAG_GENERAL,ChamberToStr((TChamberIndex)i, true, (TOperationModeIndex)((int)m_ParamsMgr->PipesOperationMode)) + " cartridge(s) is out");

            m_ContainerReplacementPromptTime = QGetTicks();
            FrontEndInterface->ShowCartridgeError(FE_INSERT_CARTRIDGE, (TChamberIndex)i, false); // Prompts the user to insert cartridge using non modal dialog

            // Cartridge absence  has just been detected; Move the cartridge status checking state machine to timeout detection state, mark the current status check as Done and continue printing.
            CheckIsDone = true;
            m_CartridgesCheckState = ((i == TYPE_CHAMBER_WASTE) ? CHECK_UV_LAMPS_TIME_OUT : CHECK_CARTRIDGE_DIALOG_TIMEOUT);


            m_Chamber = (TChamberIndex)i;
            return;
        }
    }
    // fall to the next state
    m_CartridgesCheckState = CHECK_LIQUIDS_WEIGHT;
    return;
}

void CMachineSequencer::CheckLiquidsWeight(bool& CheckIsDone, bool& TurnOnUvLamps)
{
    for (int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; i++)
    {
        if(i == TYPE_CHAMBER_WASTE)
        {
            if(! m_Container->GetWasteWeightStatusDuringPrinting())
            {
                CQLog::Write(LOG_TAG_GENERAL,"Waste weight is above the maximum level");
                m_ContainerReplacementPromptTime = QGetTicks();
                FrontEndInterface->ShowCartridgeError(FE_REPLACE_CARTRIDGE, static_cast<TChamberIndex>(i), false);
                m_CartridgesCheckState = CHECK_UV_LAMPS_TIME_OUT;
                m_Chamber = static_cast<TChamberIndex>(i);
                return;
            }
        }
        else
        {
            if (! m_Container->IsWeightOk(static_cast<TChamberIndex>(i)))
            {
                CQLog::Write(LOG_TAG_GENERAL,ChamberToStr((TChamberIndex)i) + " weight is below the minimum level");
                m_ContainerReplacementPromptTime = QGetTicks();
                FrontEndInterface->ShowCartridgeError(FE_REPLACE_CARTRIDGE,static_cast<TChamberIndex>(i), false);
                m_CartridgesCheckState = CHECK_CARTRIDGE_DIALOG_TIMEOUT;
                m_Chamber = static_cast<TChamberIndex>(i);
                return;
            }
        }
    }
    m_CartridgesCheckState = CHECK_CARTRTIDGE_EXISTENCE;
    CheckIsDone = true;
}

void CMachineSequencer::CheckCartridgeDialogTimeOut(TQErrCode& RetVal, bool& CheckIsDone, bool& TurnOnUvLamps)
{
    if (m_CartridgeErrorDlgResult == FE_CARTRIDGE_DLG_RESULT_NONE)
    {
        if (QGetTicks() >
                (m_ContainerReplacementPromptTime + QSecondsToTicks(CAppParams::Instance()->CartridgeErrorContinuePrintTime)))
        {
            CQLog::Write(LOG_TAG_GENERAL,"CartridgeErrorContinuePrintTime has expired");
            m_CartridgesCheckState = CHECK_UV_LAMPS_TIME_OUT;

        }
        else
        {
            CheckIsDone = true;
            return;
        }
    }
    else if (m_CartridgeErrorDlgResult == FE_CARTRIDGE_DLG_RESULT_CANCEL)
    {
        CQLog::Write(LOG_TAG_GENERAL,"The user clicked the cancel button of the cartridge error dialog");
        m_CartridgesCheckState     = CHECK_CARTRTIDGE_EXISTENCE;

        RetVal = Q2RT_CARTRIDGE_ERR_DLG_STOP;
        CheckIsDone = true;
        return;
    }
    else if (m_CartridgeErrorDlgResult == FE_CARTRIDGE_DLG_RESULT_OK)
        m_CartridgesCheckState = CHECK_CARTRTIDGE_EXISTENCE;
    return;

}


TQErrCode CMachineSequencer::CheckCartridgesStatus()
{
    TQErrCode RetVal        = Q_NO_ERROR;
    bool      CheckIsDone   = false;
    bool      TurnOnUvLamps = false;

    //This method checks whether printing can be continued with respect to current  m_CartridgesCheckState and determines the next m_CartridgesCheckState for the next cartridge status check.

    while (!Canceled && !CheckIsDone)
    {
        switch(m_CartridgesCheckState)
        {
        case CHECK_CARTRTIDGE_EXISTENCE:
            CheckCartridgeExistence(CheckIsDone); // If required cartridge is missing printing should continue until time out is expired.
            break;

        case CHECK_LIQUIDS_WEIGHT:
            CheckLiquidsWeight(CheckIsDone, TurnOnUvLamps); // If required cartridge is empty printing should continue until time out is expired.
            break;

        case CHECK_CARTRIDGE_DIALOG_TIMEOUT:
            CheckCartridgeDialogTimeOut(RetVal, CheckIsDone, TurnOnUvLamps);
            break;

        case ATTEMP_SWITCH_TANKS:
            AttempSwitchTanks(CheckIsDone);
            break;

        case CHECK_UV_LAMPS_TIME_OUT:
            CheckUVLampsTimeout(RetVal, CheckIsDone);
            break;

        case WAIT_FOR_CARTRIDGE_REPLACEMENT:
            WaitForCartridgeReplacement(RetVal, CheckIsDone, TurnOnUvLamps);
            break;

        }
    }

    if (TurnOnUvLamps)
    {
        // Turn the UV lamps on
        CQLog::Write(LOG_TAG_GENERAL,"Turning UV lamps on");
        if(m_UVLamps->EnableDuringPrint() != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_UV_LAMP_TURNING_ON);

        if (MotorSendTYXHome() != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_HOMING_AXES);

        if (m_Motors->GoWaitHomeSequence(AXIS_Z) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_HOMING_AXES);

        if (m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z, m_ParamsMgr->LastSliceZPosition, MORE_TIME_TO_FINISH_MOVEMENT) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_MOVING_AXES);
    }

    return RetVal;
}

TQErrCode CMachineSequencer::HandleCartridgeDlgClose(int Result)
{
    m_CartridgeErrorDlgResult = Result;
    if (m_CartridgeErrorDlgResult != FE_CARTRIDGE_DLG_RESULT_NONE)
        m_CartridgeErrorDlgCloseEvent.SetEvent();
    return Q_NO_ERROR;
}

//Error special procedures
void CMachineSequencer::UpdateLastError(TQErrCode ErrorCode)
{
    if(ErrorCode == Q_NO_ERROR)
    {
        m_KeepLastError.Error=ErrorCode;
        m_KeepLastError.ErrorCounter=0;
        return;
    }

    if(m_KeepLastError.ErrorCounter)
    {
        m_KeepLastError.ErrorCounter++;
        CQLog::Write(LOG_TAG_GENERAL,"Machine Sequence:Keep Error #%d = %s(%d)",
                     m_KeepLastError.ErrorCounter,
                     PrintErrorMessage(ErrorCode).c_str(),
                     ErrorCode);
    }
    else
    {
        m_KeepLastError.Error=ErrorCode;
        m_KeepLastError.ErrorCounter++;

        CQLog::Write(LOG_TAG_GENERAL,"Machine Sequence:Keep Error #%d = %s(%d)",
                     m_KeepLastError.ErrorCounter,
                     PrintErrorMessage(ErrorCode).c_str(),
                     m_KeepLastError.Error);
    }
}

//Error special procedures
TQErrCode CMachineSequencer::GetLastError(void)
{
    return m_KeepLastError.Error;
}


TQErrCode CMachineSequencer::ErrorStoppingSequence()
{
    bool CanMotorMove = true;
    bool CanPerformPurge = true;
    TQErrCode Err;
    m_CurrentMachineState = msStopping;

    TQErrCode LastError = GetLastError();
    if(LastError == Q_NO_ERROR)
        CQLog::Write(LOG_TAG_PRINT,"Error Stopping sequence (NO ERROR !!!) ");
    else
        CQLog::Write(LOG_TAG_PRINT,"Error Stopping sequence (%s)",
                     PrintErrorMessage(LastError).c_str());

    //Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",
                 m_AmbientTemperature->GetCurrentTemperatureCelcius());

    // Turn Roller OFF
    m_Roller->SetRollerOnOff(ROLLER_OFF);

    switch(LastError)
    {
    case Q2RT_HEAD_VACUUM_OUT_OF_RANGE:
    case Q2RT_HEAD_VACUUM_AVERAGE_OUT_OF_RANGE:
        return VacuumEmergencyStopping();

    case Q2RT_FATAL_ERROR:
        CanMotorMove = false;
        CanPerformPurge = false;
        break;

    case Q2RT_MCB_EVENT_ERROR_OVER_CURRENT:
    case Q2RT_MCB_EVENT_ERROR_HIGH_TEMPERATURE:
    case Q2RT_MCB_EVENT_ERROR_LOW_TEMPERATURE:
    case Q2RT_MCB_EVENT_ERROR_VOLTAGE_DROP:
    case Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT:
    case Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT:
    case Q2RT_MCB_EVENT_ERROR_HOME_SENSOR_NOT_DETECTED:
    case Q2RT_MCB_EVENT_ERROR_RIGHT_LIMIT_NOT_DETECTED:
    case Q2RT_MCB_EVENT_ERROR_LEFT_LIMIT_NOT_DETECTED:
    case Q2RT_MCB_EVENT_ERROR_STOPPPED_DUE_ERROR_ON_ANOTHER_AXIS:
    case Q2RT_MCB_EVENT_ERROR_KILL_MOTION_DUE_COLLISION_INT:
    case Q2RT_MCB_EVENT_ERROR_STOP_DUE_TO_A_KILL_COMMAND:
    case Q2RT_MCB_EVENT_ERROR_STOP_DUE_TO_A_STOP_COMMAND:
    case Q2RT_MCB_EVENT_ERROR_TOP_COVER_OPEN:
    case Q2RT_MCB_EVENT_ERROR_COLLISION:
    case Q2RT_MCB_EVENT_ERROR_TIME_OUT:
    case Q2RT_MCB_EVENT_ERROR_CANNOT_ESCAPE_FROM_HOME_POSITION:
    case Q2RT_MCB_EVENT_ERROR_Y_ENCODER_DETECT_ERROR:
    case Q2RT_MCB_EVENT_ERROR_OTHER_PROBLEM:
    case Q2RT_MCB_ACK_STATUS_FAILURE:
    case Q2RT_MCB_ACK_AXIS_IN_DISABLE_STATE:
    case Q2RT_MCB_ACK_MOTOR_IS_IN_HW_LIMIT:
    case Q2RT_MCB_ACK_ILLEGAL_COMMAND:
    case Q2RT_MCB_ACK_NOT_RELEVANT_COMMAND:
    case Q2RT_MCB_ACK_MOTOR_OVERCURRENT_STATE:
    case Q2RT_MCB_ACK_OTHER_ERROR:
    case Q2RT_MCB_ACK_VOLTAGE_DROP:
    case Q2RT_MCB_ACK_COLLISION:
    case Q2RT_MCB_ACK_DIAG_COMMAND_NOT_RELEVANT:
    case Q2RT_MCB_ACK_NO_EXIST_COMMAND:
    case Q2RT_MCB_ACK_Y_ENCODER_DETECT_ERROR:
    case Q2RT_MCB_ACK_TIMEOUT:
    case Q2RT_MCB_WRONG_POSITION_RX_FROM_MCB:
    case Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_X:
    case Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_Y:
    case Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_Z:
    case Q2RT_MCB_NOT_ARRIVED_TO_SOLICITED_POSITION_T:
    case Q2RT_ACS_FAIL_TO_DISABLE:
    case Q2RT_ACS_FAIL_TO_ENABLE:
    case Q2RT_ACS_ERROR_DURING_MOVE:
    case Q2RT_ACS_RESET_FAIL:
    case Q2RT_ACS_KILL_FAIL:
    case Q2RT_ACS_FAIL_TO_GET_LOCATION_X:
    case Q2RT_ACS_FAIL_TO_GET_LOCATION_Y:
    case Q2RT_ACS_FAIL_TO_GET_LOCATION_Z:
    case Q2RT_ACS_FAIL_TO_GET_LOCATION_T:
    case Q2RT_ACS_WRONG_POSITION_RX_X:
    case Q2RT_ACS_WRONG_POSITION_RX_Y:
    case Q2RT_ACS_WRONG_POSITION_RX_Z:
    case Q2RT_ACS_WRONG_POSITION_RX_T:
    case Q2RT_ACS_WRONG_POSITION_CALCULATED_X:
    case Q2RT_ACS_WRONG_POSITION_CALCULATED_Y:
    case Q2RT_ACS_WRONG_POSITION_CALCULATED_Z:
    case Q2RT_ACS_WRONG_POSITION_CALCULATED_T:
    case Q2RT_ACS_FAIL_GO_TO_POSITION_X:
    case Q2RT_ACS_FAIL_GO_TO_POSITION_Y:
    case Q2RT_ACS_FAIL_GO_TO_POSITION_Z:
    case Q2RT_ACS_FAIL_GO_TO_POSITION_T:
    case Q2RT_ACS_X_HOME_FAIL:
    case Q2RT_ACS_Y_HOME_FAIL:
    case Q2RT_ACS_Z_HOME_FAIL:
    case Q2RT_ACS_T_HOME_FAIL:
    case Q2RT_ACS_HOME_ALL_FAIL:
    case Q2RT_ACS_FAIL_TO_GET_IF_AXIS_IS_MOVING:
    case Q2RT_ACS_MOTOR_TIMEOUT_X:
    case Q2RT_ACS_MOTOR_TIMEOUT_Y:
    case Q2RT_ACS_MOTOR_TIMEOUT_Z:
    case Q2RT_ACS_MOTOR_TIMEOUT_T:
    case Q2RT_ACS_FAIL_MOVEY_X_FAIL:
    case Q2RT_ACS_FAIL_MOVEZ_X_FAIL:
    case Q2RT_ACS_FAIL_MOVEYZ_X_FAIL:
    case Q2RT_BUMPER_IMPACT:
        // Lower Z height after impact

        CanMotorMove = false;
        CanPerformPurge = false;
        break;
    case Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_HIGH:
    case Q2RT_HEADS_FILLING_TEMPERATURE_IS_TOO_LOW:
    case Q2RT_HEADS_FILLING_MONITORING_ERROR:
    case Q2RT_HEADS_FILLING_TIMEOUT:
    case Q2RT_HEADS_FILLING_THERMISTOR_OPEN:
    case Q2RT_HEADS_FILLING_THERMISTOR_SHORT:
        CanPerformPurge = false;
        break;
    case Q2RT_DOOR_FAIL_TO_LOCK_DOOR:
    case Q2RT_DOOR_FAIL_TO_CLOSE_DOOR:
    case Q2RT_DOOR_ERROR:
        CanMotorMove = false;
        CanPerformPurge = false;
        break;
    case Q2RT_HEAD_HEATER_VOLTAGE_DROP:
    case Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT:
    case Q2RT_HEAD_HEATER_TEMPERATURE_IS_TOO_HIGH:
    case Q2RT_HEAD_HEATER_THERMISTOR_OPEN:
    case Q2RT_HEAD_HEATER_THERMISTOR_SHORT:
	case Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED:
	case Q2RT_HEAD_MAIN_FAN_SPEED_CRITICAL:
        CanPerformPurge = false;
        break;
    case Q2RT_PURGE_END_MESSAGE_NOT_RECEIVED:
    case Q2RT_PURGE_NOT_FINISHED:
        CanPerformPurge = false;
        break;
    default:
        break;
    }

    if(CanMotorMove)
    {
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Error Stopping-Scanning");
        if ((Err = PerformScanningCookingPhase()) != Q_NO_ERROR)
        {
            QMonitor.WarningMessage("Error during scanning cooking phase:"+
                                    QIntToStr(static_cast<int>(Err)));
            return Err;
        }
        FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Error Stopping");

        // Turn off material cooling fan to off
        if ((Err = m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF)) != Q_NO_ERROR)
            return Err;

        if(CanPerformPurge)
            // Now purging according to parameter manager
            if (m_ParamsMgr->DoPurgeAfterPrint || m_ParamsMgr->PerformSuperPurgeAfterPrint)
            {
                FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Error Stopping-Purge");
                if((Err = PurgeSequence(PERFORM_HOME, true)) != Q_NO_ERROR)
                    return Err;
                FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Error Stopping");
            }

        if ((Err = MotorStoppedSequencer()) != Q_NO_ERROR)
        {
            QMonitor.WarningMessage("Error during motor stopped sequencer");
            CQLog::Write(LOG_TAG_PRINT,"Error during motor stopped sequencer. Error=%d",Err);
            return Err;
        }
    }
    else
    {
        m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE);
        m_UVLamps->TurnOnOff(false);
        m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF);
    }

    m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
    CQLog::Write(LOG_TAG_GENERAL,"ErrorStoppingSequence() calls SetDefaultOnOff() 5");
    m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
    m_Actuator->SetHoodFanIdle();
    m_HeadFilling->HeadFillingOnOff(false);
    m_Roller->BumperEnableDisable(false);

    return LastError;
}

TQErrCode CMachineSequencer::PrePrintErrorStoppingSequence()
{
    TQErrCode LastError = GetLastError();
    switch(LastError)
    {
    case Q2RT_HEAD_VACUUM_OUT_OF_RANGE:
    case Q2RT_HEAD_VACUUM_AVERAGE_OUT_OF_RANGE:
        return VacuumEmergencyStopping();

    default:
        m_UVLamps->TurnOnOff(false);
        m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
        m_HeadFilling->HeadFillingOnOff(false);
        m_Roller->SetRollerOnOff(ROLLER_OFF);
        m_Roller->BumperEnableDisable(false);
        m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
        m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF);
        break;
    }
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::VacuumEmergencyStopping(void)
{
    TQErrCode Err;

    CQLog::Write(LOG_TAG_GENERAL,"Vacuum emergency stopping - GoTo Purge");
    FrontEndInterface->UpdateStatus(FE_MACHINE_SUB_STATUS,"Vacuum - stopping");

    m_HeadFilling->HeadFillingOnOff(false);
    m_UVLamps->TurnOnOff(false);

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE)) != Q_NO_ERROR)
        return Err;

    // Go home if needed
    if (!m_Motors->GetAxisIfHomeWasPerformed(AXIS_Y))
    {
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
    }

    if (!m_Motors->GetAxisIfHomeWasPerformed(AXIS_X))
    {
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;
    }

    if (!m_Motors->GetAxisIfHomeWasPerformed(AXIS_T))
    {
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_T)) != Q_NO_ERROR)
            return Err;
    }

    if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,m_ParamsMgr->MotorsPurgeXStartPosition,muMM)) != Q_NO_ERROR)
        return Err;

    long PosInSteps = CONFIG_ConvertUnitsToStep(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,muMM);

    if ((Err = m_Motors->PerformBacklashIfNeeded(PosInSteps)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,m_ParamsMgr->MotorsPurgeYStartPosition,muMM)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->OpenWasteTank()) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
        return Err;
    if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
        return Err;

    QSleepSec(WAIT_VACUUM_ERROR_WAIT_TO_WIPE_SEC);

	if((Err=PerformWipe(false,true)) != Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->OpenWasteTank()) != Q_NO_ERROR)
        return Err;

    m_TrayHeater->SetTrayOnOff(TRAY_HEATER_OFF);
	CQLog::Write(LOG_TAG_GENERAL,"VacuumEmergencyStopping() calls SetDefaultOnOff() 6");
    m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
    m_Roller->SetRollerOnOff(ROLLER_OFF);
    m_AmbientTemperature->SetMaterialCoolingFanOnOff(FAN_OFF);
    m_Roller->BumperEnableDisable(false);

    if (!m_Motors->GetAxisIfHomeWasPerformed(AXIS_Z))
    {
        if ((Err = m_Motors->GoWaitHomeSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;
    }

    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
               m_ParamsMgr->ZAxisEndPrintPosition,
               MORE_TIME_TO_FINISH_MOVEMENT))!= Q_NO_ERROR)
        return Err;

    if ((Err = m_Motors->SetMotorEnableDisable(AXIS_ALL, MOTOR_DISABLE)) != Q_NO_ERROR)
        return Err;

    return Q_NO_ERROR;
}

//Verify OCB version
bool CMachineSequencer::CheckOCBForVersionHigherThen1_29(void)
{
    if(m_OCBStatusSender->GetOCBSWExternalVersion() > 1)
        return true;

    if(m_OCBStatusSender->GetOCBSWInternalVersion() > 28)
        return true;

    return false;
}

//Verify OCB version
bool CMachineSequencer::CheckOHDBForVersionHigherThen1_5(void)
{
    if(m_HeadStatusSender->GetOHDBSWExternalVersion() > 1)
        return true;

    if(m_HeadStatusSender->GetOHDBSWInternalVersion() > 5)
        return true;

    return false;
}

//Verify OCB version
bool CMachineSequencer::CheckOHDBForVersionHigherThen1_6(void)
{
    if(m_HeadStatusSender->GetOHDBSWExternalVersion() > 1)
        return true;

    if(m_HeadStatusSender->GetOHDBSWInternalVersion() > 6)
        return true;

    return false;
}

TQErrCode CMachineSequencer::ErrorInsertion(TQErrCode ErrorInserted,int Value)
{
    ErrorInserted*=-1;

    CQLog::Write(LOG_TAG_PRINT,"Error insertion %s Value = %d",
                 PrintErrorMessage(ErrorInserted).c_str(),
                 ErrorInserted);
    CErrorHandler::Instance()->ReportError(PrintErrorMessage(ErrorInserted).c_str(),
                                           ErrorInserted,
                                           Value);
    return Q_NO_ERROR;
}


TQErrCode CMachineSequencer::CheckCartridgesStatusBeforePrint(TMachineState ToCompare)
{

    TQErrCode RetValue;
//  bool      NotExitLoop;

    // Reset the event:
    m_CartridgeErrorDlgCloseEvent.WaitFor(0);

    bool      TankOk  = true;
    do
    {
        //Check if the all tanks(including waste) are in place
        m_Container->GetLiquidTankInsertedStatus();
        for (int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; i++)
        {
            if (m_CurrentMachineState != ToCompare)
            {
                CQLog::Write(LOG_TAG_PRINT,"PrePrint sequence stopped");
                return Q2RT_STOP_DURING_PRE_PRINT;
            }
            TankOk = m_Container->IsActiveLiquidTankInserted((TChamberIndex)i);
            if (!TankOk)
            {
                if ((RetValue = FrontEndInterface->ShowCartridgeError(FE_INSERT_CARTRIDGE, i, true)) != Q_NO_ERROR)
                    return RetValue;
            }

            TankOk = m_Container->IsActiveLiquidTankEnabled((TChamberIndex)i);
            if (!TankOk)
            {
                if ((RetValue = FrontEndInterface->ShowCartridgeError(FE_REPLACE_CARTRIDGE, i, true)) != Q_NO_ERROR)
                    return RetValue;
            }

            if (!m_Container->IsWeightOk((TChamberIndex)i))
            {
                if ((RetValue = FrontEndInterface->ShowCartridgeError(FE_REPLACE_CARTRIDGE, i, true)) != Q_NO_ERROR)
                    return RetValue;
            }
        }
    }
    while (!TankOk);

    return Q_NO_ERROR;
}//CheckCartridgesStatusBeforePrint

void CMachineSequencer::HandleBumperImpactError(void)
{
    bool ContinuePossible = true;

    m_Roller->BumperEnableDisable(false);
    m_PrintControl->Stop();
    m_Motors->WaitForHWAccess();
    m_Motors->UpdateBumperImpact();

    FIFOPCI_ResetBufferAddress();
    FIFOPCI_NoGo();

    if (m_Motors->IsMotorMoving(AXIS_X))
        m_Motors->KillMotion(AXIS_X);

    if (m_Motors->IsMotorMoving(AXIS_Z))
        m_Motors->KillMotion(AXIS_Z);

    m_Motors->Cancel();
    m_Motors->SetMotorEnableDisable(AXIS_ALL,MOTOR_ENABLE);
    m_Motors->GoWaitToRelativePosition(AXIS_Z, Z_MOVEMENT_AFTER_BUMPER_IMPACT, muMM, IGNORE_BUMPER_IMPACT);
    m_Motors->GoHomeSequence(AXIS_T, IGNORE_BUMPER_IMPACT);

    // Home X
    if (m_Motors->GoHomeSequence(AXIS_X, IGNORE_BUMPER_IMPACT) != Q_NO_ERROR)
        ContinuePossible = false;

    if (ContinuePossible)
        if (m_Motors->CheckForEndOfMovementSequence(AXIS_X,WAIT_MOTOR_HOME_MORE_TIME_SEC) != Q_NO_ERROR)
            ContinuePossible = false;

    // Home Y
    if (m_Motors->GoHomeSequence(AXIS_Y, IGNORE_BUMPER_IMPACT) != Q_NO_ERROR)
        ContinuePossible = false;

    if (ContinuePossible)
        if (m_Motors->CheckForEndOfMovementSequence(AXIS_Y,WAIT_MOTOR_HOME_MORE_TIME_SEC) != Q_NO_ERROR)
            ContinuePossible = false;

    // Move Z to Z_StartPrintPosition
    if (ContinuePossible)
        m_Motors->GoWaitToAbsolutePosition(AXIS_Z, m_ParamsMgr->Z_StartPrintPosition, muMM, IGNORE_BUMPER_IMPACT);

    // Disable T
    m_Motors->SetMotorEnableDisable(AXIS_T,MOTOR_DISABLE);
    m_Motors->ReleaseHWAccess();
}

void CMachineSequencer::FireAllSequenceCleanUp()
{
    m_HeadHeaters->SetDefaultOnOff(HEAD_HEATER_OFF);
}

void CMachineSequencer::PrePrintSequenceCleanUp()
{
    try
    {
        m_Container->ActivateRollerAndPurgeWaste(false);
    }
    catch (EContainer &e)
    {
    }
    //RSS, itamar added, turn RSS off
    if(m_IsSuctionSystemExist)
    {
        m_Actuator->ActivateRollerSuctionSystem(m_ParamsMgr->RollerSuctionValveOnTime,
                                                m_ParamsMgr->RollerSuctionValveOffTime,false);
    }

    // If the door is open - axes can't move - return.
    if (! m_Door->IsDoorReady())
        return;

    // Enable Y Axis (in case it's disabled)
    if (!m_Motors->IsMotorEnabled(AXIS_Y))
        if (m_Motors->SetMotorEnableDisable(AXIS_Y,MOTOR_ENABLE) != Q_NO_ERROR)
            return;

    m_Motors->GoHomeSequence(AXIS_Y);
    m_Motors->CheckForEndOfMovementSequence(AXIS_Y,WAIT_MOTOR_HOME_MORE_TIME_SEC);

    // Enable X Axis (in case it's disabled)
    if (!m_Motors->IsMotorEnabled(AXIS_X))
        if (m_Motors->SetMotorEnableDisable(AXIS_X,MOTOR_ENABLE) != Q_NO_ERROR)
            return;

    m_Motors->GoHomeSequence(AXIS_X);
    m_Motors->CheckForEndOfMovementSequence(AXIS_X,WAIT_MOTOR_HOME_MORE_TIME_SEC);
}

void CMachineSequencer::FillBlock()
{
    m_HeadFilling->HeadFillingOnOff(true);
    m_HeadFilling->WaitForFilledHeadContainer();
    m_HeadFilling->HeadFillingOnOff(false);
}

int CMachineSequencer::GetAvgUVValue(int Lamp, int PSLevel)
{

    long CurrentVelocity = 0;
    long SlowVelocity    = 0;

    // Decreseing X Axis velocity
    CurrentVelocity = m_Motors->GetAxisCurrentVelocity(AXIS_X);
    SlowVelocity = (CurrentVelocity / 8);

    // Slowing down Axis X
    m_Motors->SetVelocity(AXIS_X, SlowVelocity, muSteps);

    QString LogOutput = "";
    QString A2DValues = "";

    int InitLocation = 0;
    int UVNumberOfSamples = m_ParamsMgr->UVNumberOfSamples;

    // Static array. We should never use all of it...
    int UVValuesArr[50] = {0,};


    int UVTotalValue    = 0;
    int PeakValue = 0;
    int PeakIndex = -1;
    int LowIndex  = -1;
    int HighIndex = -1;
    int SamplesForAvarage = m_ParamsMgr->UVNumberOfSamplesForAverage;

    // The sampling position is the sensor position minus half of the samples number
    InitLocation = m_ParamsMgr->UVSensorLampXPositionArray[Lamp] - (UVNumberOfSamples / 2);
    unsigned EndTime;
    for (int i = 0, UVSensorReading = 0; i < UVNumberOfSamples; i++)
    {
        int CurrLocation = InitLocation + i;
        m_Motors->GoWaitToAbsolutePosition(AXIS_X, CurrLocation, muMM, false);

        // Waiting for sensor to stabilize
        EndTime = QGetTicks() + QSecondsToTicks(2);

        // Wait for UV lamps to stabilize
        while (QGetTicks() < EndTime)
        {
            Q2RTApplication->YieldMainThread();
            QSleep(UV_LAMPS_POLL_TIME);
        }

        // Read and save the A/D Value of the UV Sensor
        UVSensorReading = m_UVLamps->GetUVSensorValue(true);
        UVValuesArr[i]  = UVSensorReading;

        if(UVSensorReading > PeakValue)
        {
            PeakValue = UVSensorReading;
            PeakIndex = i;
        }
    }

    LowIndex  = (PeakIndex - SamplesForAvarage < 0 ? 0 : (PeakIndex - SamplesForAvarage));
    HighIndex = (PeakIndex + SamplesForAvarage >= UVNumberOfSamples ? UVNumberOfSamples - 1 : (PeakIndex + SamplesForAvarage));

    // Summing the UV values
    for(int i = LowIndex; i <= HighIndex; i++)
    {
        UVTotalValue += UVValuesArr[i];
        A2DValues    += QIntToStr(UVValuesArr[i]);

        if (i != HighIndex)
            A2DValues += ", ";
    }

    // Calculate the avarage value
    UVTotalValue /= (HighIndex - LowIndex + 1);

    if (m_ParamsMgr->LogUVExtendedData = true)
    {
        LogOutput  = "PS level = " + QIntToStr(PSLevel + 1) + ", Peak = " + QIntToStr(UVValuesArr[PeakIndex]);
        LogOutput += ", Peak index = " + QIntToStr(PeakIndex + 1) + ", Average value = "  + QIntToStr(UVTotalValue);
        LogOutput += ". UV Values (" +  QIntToStr(LowIndex + 1) + " - " + QIntToStr(HighIndex + 1) + ") - [";
        LogOutput += A2DValues + "]";

    }

    WriteToUVHistoryFile (LogOutput);

    // Restoring Axis X's speed
    m_Motors->SetVelocity(AXIS_X, CurrentVelocity, muSteps);

    return UVTotalValue;
}

//---------------------------------------------------------------------------

TQErrCode CMachineSequencer::WriteToHOWHistoryFile(QString Str)
{
    // Prepare the date/time string
    char DateTimeStr[MAX_DATE_SIZE];
    time_t CurrentTime = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr,MAX_DATE_SIZE,"%d/%m/%y %X",LocalTimeTM);

	QString DirName  = Q2RTApplication->AppFilePath.Value();
	DirName += "\\ServiceTools\\Head Optimization Wizard\\";
	QString FileName = DirName + "Head Optimization Wizard.txt";
	CBackEndInterface::Instance()->ForceFileCreation(FileName, "");
	m_HOWHistoryFile = fopen(FileName.c_str(), "at");

    // If the handle is still Ok
	if(m_HOWHistoryFile)
	{
	   fputs("\n",m_HOWHistoryFile);
	   fputs(DateTimeStr,m_HOWHistoryFile);
	   fputs("\n",m_HOWHistoryFile);

		if(fputs(Str.c_str(),m_HOWHistoryFile) == EOF)
        {
            // If error, close the file
			fclose(m_HOWHistoryFile);
			m_HOWHistoryFile = NULL;
        }
        else
        {
			if(fputs("\n",m_HOWHistoryFile) == EOF)
            {
                // If error, close the file
				fclose(m_HOWHistoryFile);
				m_HOWHistoryFile = NULL;
            }
            else
            {
                // Make sure that the data is on the disk
                fflush(m_HOWHistoryFile);
            }
        }
    }
    fclose(m_UVHistoryFile);
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::OpenUVLampsHistoryFile()
{
    QString DirName  = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_UV_LAMPS_HISTORY_DIR_NAME);
    QString FileName = DirName + LOAD_STRING(IDS_UV_LAMPS_HISTORY_FILE_NAME);
	CBackEndInterface::Instance()->ForceFileCreation(FileName, LOAD_STRING(IDS_UV_LAMPS_HISTORY_RESTRICTIONS));
    m_UVHistoryFile = fopen(FileName.c_str(), "at");
    return Q_NO_ERROR;
}
//---------------------------------------------------------------------------

TQErrCode CMachineSequencer::WriteToUVHistoryFile(QString Str)
{
    // Prepare the date/time string
    char DateTimeStr[MAX_DATE_SIZE];
    time_t CurrentTime = time(NULL);
    struct tm *LocalTimeTM = localtime(&CurrentTime);
    strftime(DateTimeStr,MAX_DATE_SIZE,"%d/%m/%y %X",LocalTimeTM);

    // If the handle is still Ok
    if(m_UVHistoryFile)
    {
        if (Str == "Line")
        {
            fputs("--------------------------------------------------------------------",m_UVHistoryFile);
            fputs("--------------------------------------------------------",m_UVHistoryFile);
            fputs("\n",m_UVHistoryFile);
            return Q_NO_ERROR;
        }

        if (Str == "Date")
        {
            fputs(DateTimeStr,m_UVHistoryFile);
            fputs("\n",m_UVHistoryFile);
            return Q_NO_ERROR;
        }

        if(fputs(Str.c_str(),m_UVHistoryFile) == EOF)
        {
            // If error, close the file
            fclose(m_UVHistoryFile);
            m_UVHistoryFile = NULL;
        }
        else
        {
            if(fputs("\n",m_UVHistoryFile) == EOF)
            {
                // If error, close the file
                fclose(m_UVHistoryFile);
                m_UVHistoryFile = NULL;
            }
            else
            {
                // Make sure that the data is on the disk
                fflush(m_UVHistoryFile);
            }
        }
    }

    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::CloseUVLampsHistoryFile()
{
    fclose(m_UVHistoryFile);
    return Q_NO_ERROR;
}

bool CMachineSequencer::IsUVWizardLimited(void)
{
    return m_UVWizardLimited;
}

void CMachineSequencer::SetUVWizardLimited(bool limited)
{
    m_UVWizardLimited = limited;
}

TQErrCode CMachineSequencer::SaveAllUVParams()
{
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->LastUVCalibration);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->UVNumberOfSamples);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->UVNumberOfSamplesForMax);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->UVNumberOfSamplesForAverage);

    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->UVLampPSValueArray);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->UVInternalSensorGainArray);
    m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->UVInternalSensorOffsetArray);
    return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::DispatchHelp(int HelpContext, QString Path)
{
    QString FileName      = Q2RTApplication->AppFilePath.Value() + Path;
    Application->HelpFile = FileName.c_str();

    Application->HelpContext(HelpContext);
    return Q_NO_ERROR;

}


void CMachineSequencer::AttempSwitchTanks(bool& CheckIsDone)
{
    CQLog::Write(LOG_TAG_GENERAL,"Trying to switch to new tank");
    // Trying to switch to another tank (if possible)
    if (Q_NO_ERROR == m_Container->SwitchToNewTank(m_Chamber))
    {
        CQLog::Write(LOG_TAG_GENERAL,"Tank switching succeeded");

        CheckIsDone = true;
        m_CartridgesCheckState = CHECK_CARTRTIDGE_EXISTENCE;
    }
    else
    {
        CQLog::Write(LOG_TAG_GENERAL,"Tank switching failed");
        m_CartridgesCheckState = CHECK_UV_LAMPS_TIME_OUT;
    }
}

void CMachineSequencer::CheckUVLampsTimeout(TQErrCode& RetVal, bool& CheckIsDone) // todo -oNobody -cNone: function name is misleading
{

    // Turn the head filling off
    CQLog::Write(LOG_TAG_GENERAL,"Turning head filling off due to cartridge error");
    if(m_HeadFilling->HeadFillingOnOff(false) != Q_NO_ERROR)
        throw EMachineSequencer(Q2RT_HEAD_FILLING_TURNING_OFF);

    // Turn Roller OFF
    CQLog::Write(LOG_TAG_GENERAL,"Turning roller off due to cartridge error");
    if (m_Roller->SetRollerOnOff(ROLLER_OFF) != Q_NO_ERROR)
        throw EMachineSequencer(Q2RT_ROLLER_TURNING_OFF);

    int waitedSincePrompt = QGetTicks() - m_ContainerReplacementPromptTime;
    int timeout =	QSecondsToTicks(CAppParams::Instance()->CartridgeErrorUVLampsOnTime) - waitedSincePrompt;
    if ((0 < timeout) &&
            (QLib::wrSignaled == m_CartridgeErrorDlgCloseEvent.WaitFor(timeout)))
    {
        if (m_CartridgeErrorDlgResult == FE_CARTRIDGE_DLG_RESULT_CANCEL)
        {
            CQLog::Write(LOG_TAG_GENERAL,"The user clicked on the cancel button of the cartridge error dialog");
            m_CartridgesCheckState = CHECK_CARTRTIDGE_EXISTENCE;
            RetVal                 = Q2RT_CARTRIDGE_ERR_DLG_STOP;
            CheckIsDone            = true;
            return;
        }

        // Turn the head filling on
        CQLog::Write(LOG_TAG_GENERAL,"Turning head filling on, cartridge error has been resolved");
        if(m_HeadFilling->HeadFillingOnOff(true) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_HEAD_FILLING_TURNING_ON);

        // Turn Roller ON
        CQLog::Write(LOG_TAG_GENERAL,"Turning roller on, cartridge error has been resolved");
        if (m_Roller->SetRollerOnOff(ROLLER_ON) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_ROLLER_TURNING_ON);
        m_CartridgesCheckState = CHECK_CARTRTIDGE_EXISTENCE;
        return;
    }
    else
    {
        CQLog::Write(LOG_TAG_GENERAL,"CartridgeErrorUVLampsOnTime has expired");
        // Turn the UV lamps off
        if(m_UVLamps->TurnOnOff(false) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_UV_LAMP_TURNING_OFF);

        // Fall to the next state
        m_CartridgesCheckState = WAIT_FOR_CARTRIDGE_REPLACEMENT;
    }
}

void CMachineSequencer::WaitForCartridgeReplacement(TQErrCode& RetVal, bool& CheckIsDone, bool& TurnOnUvLamps)
{
    m_CartridgesCheckState     = CHECK_CARTRTIDGE_EXISTENCE;

    int waitedSincePrompt = QGetTicks() - m_ContainerReplacementPromptTime;
    int timeout =	QSecondsToTicks(CAppParams::Instance()->CartridgeErrorBeforePauseTime) - waitedSincePrompt;

    if ((0 < timeout) &&
            (QLib::wrSignaled == m_CartridgeErrorDlgCloseEvent.WaitFor(timeout)))
    {
        CancelCheck();
        if (m_CartridgeErrorDlgResult == FE_CARTRIDGE_DLG_RESULT_CANCEL)
        {
            CQLog::Write(LOG_TAG_GENERAL,"The user clicked on the cancel button of the cartridge error dialog");
            RetVal = Q2RT_CARTRIDGE_ERR_DLG_STOP;
            CheckIsDone = true;
            return;
        }

        // Turn the head filling on
        CQLog::Write(LOG_TAG_GENERAL,"Turning head filling on, cartridge error has been resolved");
        if(m_HeadFilling->HeadFillingOnOff(true) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_HEAD_FILLING_TURNING_ON);

        // Turn Roller ON
        CQLog::Write(LOG_TAG_GENERAL,"Turning roller on, cartridge error has been resolved");
        if (m_Roller->SetRollerOnOff(ROLLER_ON) != Q_NO_ERROR)
            throw EMachineSequencer(Q2RT_ROLLER_TURNING_ON);
        TurnOnUvLamps = true;
//flag for CHECK_CARTRTIDGE_EXISTENCE state to check if update for batch number is needed
        //m_tankReplaced = true;
        return;
    }
    else
    {
        CQLog::Write(LOG_TAG_GENERAL,"CartridgeErrorBeforePauseTime has expired, pausing the current job");
        // The cartridge error dialog will handle the stop/resume printing
        FrontEndInterface->EnableDisableControl(FEOP_RESUME_BY_CARTRIDGE_ERR_DLG,true,true);

        // Pause the current job
        RetVal = Q2RT_SEQUENCE_PAUSED;
        CheckIsDone = true;
        return;
    }
}

TQErrCode CMachineSequencer::TestPrintSequencer(CLayer *Layer)
{
    MotorInitBeforeFirstLayer();
    return DoubleInterlacePrintSequencer(Layer);
}

int CMachineSequencer::UVReset()
{

    // reset the AVG
    m_UVLamps->SetUVSamplingParams(true);

    return 0;
}

int CMachineSequencer::GetUVMaxValueRead(bool InternalSensor, bool UpdateFromOCB)
{

    if( UpdateFromOCB )
    {
        // read the AVG from the OCB
        TQErrCode Err = m_UVLamps->GetUVValue();

        if( Err == Q_NO_ERROR )
        {
            return m_UVLamps->GetUVSensorMaxValue(InternalSensor);
        }
        else
        {
            return 0;
        }
    }

    // w/o update from the Lotus
    return m_UVLamps->GetUVSensorMaxValue(InternalSensor);
}

// call this function only right after GetUVMaxValueRead
int CMachineSequencer::GetUVMaxDeltaRead(bool InternalSensor, bool UpdateFromOCB)
{

    if( UpdateFromOCB )
    {
        // read the AVG from the OCB
        TQErrCode Err = m_UVLamps->GetUVValue();

        if( Err == Q_NO_ERROR )
        {
            return m_UVLamps->GetUVSensorMaxDelta(InternalSensor);
        }
        else
        {
            return 0;
        }
    }

    // w/o update from the OCB
    return m_UVLamps->GetUVSensorMaxDelta(InternalSensor);
}

int CMachineSequencer::GetUVSumRead(bool InternalSensor, bool UpdateFromOCB )
{

    if( UpdateFromOCB )
    {

        // read the AVG from the OCB
        TQErrCode Err = m_UVLamps->GetUVValue();

        if( Err == Q_NO_ERROR )
        {
            return m_UVLamps->GetUVSensorSum(InternalSensor);
        }
        else
        {
            return 0;
        }
    }

    // w/o update from the OCB
    return m_UVLamps->GetUVSensorSum(InternalSensor);
}

int CMachineSequencer::GetUVNumOfReadings(bool UpdateFromOCB)
{

    if( UpdateFromOCB )
    {

        // read the AVG from the OCB
        TQErrCode Err = m_UVLamps->GetUVValue();

        if( Err == Q_NO_ERROR )
        {
            return m_UVLamps->GetUVNumOfReadings();
        }
        else
        {
            return 0;
        }
    }

    // w/o update from the OCB
    return m_UVLamps->GetUVNumOfReadings();
}


// go to location, perform 1 second reading and return the result
int CMachineSequencer::GetUVValueSingleIteration(int Location, bool InternalSensor, bool read, int ms)
{

    unsigned EndTime;

    if( read )
    {
        // move the heads such that the lamp is over the sensor
        m_Motors->GoWaitToAbsolutePosition(AXIS_X, Location, muMM, false);  // move the lamp above the sensor

        // reset the AVG
        m_UVLamps->SetUVSamplingParams(true);

        // wait for 1 second
        EndTime = QGetTicks() + QSecondsToTicks(1);
        while (QGetTicks() < EndTime)
        {
            Q2RTApplication->YieldMainThread();
            QSleep( ms /*UV_LAMPS_POLL_TIME*/);
        }

        // read the AVG from the OCB
        TQErrCode Err = m_UVLamps->GetUVValue();

        if( Err == Q_NO_ERROR )
        {
            return m_UVLamps->GetUVSensorValue(InternalSensor);
            //UVValuesArr[i] = m_UVLamps->GetUVSensorValue();
        }
        else
        {
            return 0;
            //UVValuesArr[i] = 0;
        }
    }

    else
    {
        // return to init position (50 mm left to the center of the sensor)
        m_Motors->GoWaitToAbsolutePosition(AXIS_X, Location, muMM, false);  // move the lamp above the sensor


        // wait for 1 second (for the sensor to cool off
        EndTime = QGetTicks() + QSecondsToTicks(1);
        while (QGetTicks() < EndTime)
        {
            Q2RTApplication->YieldMainThread();
            QSleep( ms /*UV_LAMPS_POLL_TIME*/);
        }

        return 0;
    }
}

void CMachineSequencer::SetLiquidTanksEnabledStatus(bool status)
{
    // notify the relevant packages:
    m_HeadFilling->AllowHeadFilling(status);
}

void CMachineSequencer::HandleHaspStatus()
{
    THaspConnectionStatus HaspStatus = CHaspInterfaceWrapper::Instance()->GetConnectionStatus();

    switch(HaspStatus)
    {
    case hsConnected:
    {
        TMachineState    MachineState;
		CMachineManager *MachineManager = Q2RTApplication->GetMachineManager();
        if(MachineManager)
            MachineState = MachineManager->GetCurrentState();
        else
            return; // When closing the application Machine Manager may be unavailable

        // Don't update buttons state during printing  //bug 6028
		if((MachineState == msIdle) || (MachineState == msStandby1) || (MachineState == msStandby2))
            FrontEndInterface->EnableDisableControl(FE_HASP_UI_CONTROLS_STATUS, true, true /* force refresh */);

        THaspPlugInfo PlugInfo = CHaspInterfaceWrapper::Instance()->GetPlugInfo();

        if(PlugInfo.LicenseType == "trial" || PlugInfo.LicenseType == "expiration")
        {
            int DaysToExpiration = CHaspInterfaceWrapper::Instance()->GetDaysToExpiration();

            if ((DaysToExpiration < CAppParams::Instance()->HASP_DaysToExpirationNotification) && (DaysToExpiration != -1))
            {
                QString str;

                if(DaysToExpiration == 0)
                    str = QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_TODAY));
                else
                    str = QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_IN), DaysToExpiration);
                FrontEndInterface->UpdateStatus(FE_HASP_UPDATE_STATUS_LABEL, str);
                FrontEndInterface->EnableDisableControl(FE_HASP_STATUS_LABEL_ENABLED, true, true /* force refresh */);
            }
            else
            {
                FrontEndInterface->UpdateStatus(FE_HASP_UPDATE_STATUS_LABEL, LOAD_STRING(IDS_HASP_OK));
            }
        }
        else
        {
            FrontEndInterface->UpdateStatus(FE_HASP_UPDATE_STATUS_LABEL, LOAD_STRING(IDS_HASP_OK));
        }

        break;
    }

    case hsDisconnected:
	{
		FrontEndInterface->UpdateStatus(FE_HASP_UPDATE_STATUS_LABEL, LOAD_STRING(IDS_HASP_NOT_DETECTED));
		FrontEndInterface->EnableDisableControl(FE_HASP_UI_CONTROLS_STATUS, false, true /* force refresh */);
		break;
    }

    case hsExpired:
    {
        FrontEndInterface->UpdateStatus(FE_HASP_UPDATE_STATUS_LABEL, LOAD_STRING(IDS_HASP_LICENSE_EXPIRED));
        FrontEndInterface->EnableDisableControl(FE_HASP_UI_CONTROLS_STATUS, false, true /* force refresh */);
        break;
    }

    case hsInvalid:
    {
        FrontEndInterface->UpdateStatus(FE_HASP_UPDATE_STATUS_LABEL, LOAD_STRING(IDS_HASP_INVALID));
        FrontEndInterface->EnableDisableControl(FE_HASP_UI_CONTROLS_STATUS, false, true /* force refresh */);
        break;
    }
    }
}

void CMachineSequencer::HandleIncomingSlicesProtection(bool Show)
{
    FrontEndInterface->EnableDisableControl(FE_HASP_INCOMING_SLICES_PROTECTION, Show, true /* force refresh */);
}

void CMachineSequencer::LogMonitorMessage(QString Message)
{
    CQLog::Write(LOG_TAG_GENERAL,QFormatStr("Monitor: %s", Message.c_str()));
}

TQErrCode CMachineSequencer::ActivateSignalTower(BYTE Red, BYTE Green, BYTE Yellow, bool ValidateState)
//TQErrCode CMachineSequencer::ActivateSignalTower(TSignalTowerLightState Red, TSignalTowerLightState Green, TSignalTowerLightState Yellow, bool ValidateState)
{
    CAppParams *ParamsMgr = CAppParams::Instance();
    if(ParamsMgr->SignalTowerEnabled == false)
    {
        return Q_NO_ERROR;
    }

    //if signal tower state isn't changed
    if( ValidateState == true )
    {
        if(	( Red == ST_LIGHT_NC || Red == m_SignalTower->GetLightState( RED_LIGHT )) &&
                ( Green == ST_LIGHT_NC || Green == m_SignalTower->GetLightState( GREEN_LIGHT )) &&
                ( Yellow == ST_LIGHT_NC || Yellow == m_SignalTower->GetLightState( YELLOW_LIGHT )) )
        {
            return Q_NO_ERROR;
        }
    }

    int DutyOnTime  = ParamsMgr->LightDutyOnTime_ms;
    int DutyOffTime = ParamsMgr->LightDutyOffTime_ms;

    return m_SignalTower->ActivateSignalTower(Red, Green, Yellow, DutyOnTime, DutyOffTime);

}
int CMachineSequencer::GetCurrentTemperatureCelcius()
{
    int ambTemp =  m_AmbientTemperature->GetCurrentTemperatureCelcius();
//Display ambient temperature
    CQLog::Write(LOG_TAG_AMBIENT,"Ambient temperature = %d C",ambTemp);
//ambTemp = 20;
    return ambTemp;
}
void CMachineSequencer::SetOHDBAmbientParams()
{
	m_AmbientTemperature->SetOHDBAmbientParams();
}
int CMachineSequencer::GetPrevHighLimit()
{
   return m_AmbientTemperature->GetPrevHighLimit();
}
void CMachineSequencer::SetPrevHighLimit(int prevHighLimit)
{
   m_AmbientTemperature->SetPrevHighLimit(prevHighLimit);
}
int CMachineSequencer::GetPrevRollerSpeed()
{
   return m_Roller->GetPrevRollerSpeed();
}
void CMachineSequencer::SetPrevRollerSpeed(int prevRollerSpeed)
{
   m_Roller->SetPrevRollerSpeed(prevRollerSpeed);
}
void CMachineSequencer::SaveBackupFiles()
{
		CConfigBackup *ConfigBackup = new CConfigBackup();
	try
	{

		QString FileName = Q2RTApplication->AppFilePath.Value() + CONFIG_BACKUP_FILE_NAME;
		ConfigBackup->CreateConfigBackupFile();

	}
	__finally
	{
    	Q_SAFE_DELETE(ConfigBackup);
	}
}
