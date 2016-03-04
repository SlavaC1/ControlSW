//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EdenGeneralDeviceFrame.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "FrontEndControlIDs.h"
#include "MotorDefs.h"
#include "MainUnit.h"
#include "AppParams.h"
#include "UvLamps.h"
#include "QThreadUtils.h"
#include "AppLogFile.h"
#include "FEResources.h"
#include "Raccoon.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const int UV_SAMPLING_WINDOW_SIZE_FOR_AVERAGE = 1000;
const int UV_SAMPLING_WINDOW_SIZE_FOR_MAX = 5;
const int UV_SAMPLING_THRESHOLD = 10; // %


//---------------------------------------------------------------------------
__fastcall TGeneralDeviceFrame::TGeneralDeviceFrame(TComponent* Owner)
	: TFrame(Owner),m_UvLampsMask(0)
{
	for(short l = 0; l < NUMBER_OF_UV_LAMPS; l++)
	{
		m_UVGroupBox[l]                   = new TGroupBox(this);
		m_UVGroupBox[l]->Parent           = UVDimmingGroupBox;
		m_UVGroupBox[l]->Caption          = GetUVLampStr(l).c_str();
		m_UVGroupBox[l]->TabOrder         = l;
		m_UVGroupBox[l]->Left             = 5;
		m_UVGroupBox[l]->Width            = 217;
		m_UVGroupBox[l]->Height           = 49;
		m_UVGroupBox[l]->Top              = 17+(5+m_UVGroupBox[l]->Height)*l;

		m_UVPanelArray[l]                 = new TPanel(this);
		m_UVPanelArray[l]->Parent         = m_UVGroupBox[l];
		m_UVPanelArray[l]->Left           = 186;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_LEFT);
		m_UVPanelArray[l]->Width          = 23;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_WIDTH);
		m_UVPanelArray[l]->Height         = 20;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_HEIGHT);
		m_UVPanelArray[l]->Top            = 19;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_TOP_PANEL);
		m_UVPanelArray[l]->BevelOuter     = bvLowered;
		m_UVPanelArray[l]->Caption        = "???";
		m_UVPanelArray[l]->ParentBackground = false;

		m_UVTrackBarArray[l]              = new TTrackBar(this);
		m_UVTrackBarArray[l]->Parent      = m_UVGroupBox[l];
		m_UVTrackBarArray[l]->Max         = SIZE_OF_UV_POWER_SUPPLY_ARRAY_30 - 1;
		m_UVTrackBarArray[l]->Tag         = l;
		m_UVTrackBarArray[l]->Left        = 3;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_LEFT);
		m_UVTrackBarArray[l]->Width       = 182;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_WIDTH);
		m_UVTrackBarArray[l]->Height      = 25;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_HEIGHT);
		m_UVTrackBarArray[l]->Top         = 16;//INT_FROM_RESOURCE_ARRAY(l,IDN_GENERAL_LAMP_TOP_PANEL);
		m_UVTrackBarArray[l]->OnChange    = UVTrackBarChange;


		m_UVLampLabelArray[l]             = new TLabel(this);
		m_UVLampLabelArray[l]->Parent     = UVOnOffGroupBox;
		m_UVLampLabelArray[l]->Left       = 5+5+16;
		m_UVLampLabelArray[l]->Top        = m_UVGroupBox[l]->Top;
		m_UVLampLabelArray[l]->Caption    = GetUVLampStr(l).c_str();
		m_UVLampLabelArray[l]->Alignment  = taLeftJustify;

		m_UVLampsChecks[l]                = new TCheckBox(this);
		m_UVLampsChecks[l]->Parent        = UVOnOffGroupBox;
		m_UVLampsChecks[l]->Tag           = l;
		m_UVLampsChecks[l]->TabOrder      = l;
		m_UVLampsChecks[l]->Left          = 8;
		m_UVLampsChecks[l]->Width         = 16;
		m_UVLampsChecks[l]->OnClick       = UVLampsCheckBoxClick;
		m_UVLampsChecks[l]->Top           = m_UVLampLabelArray[l]->Top+14;
		m_UVLampsChecks[l]->Checked       = false;

		m_UVLampPanelArray[l]             = new TPanel(this);
		m_UVLampPanelArray[l]->Parent     = UVOnOffGroupBox;
		m_UVLampPanelArray[l]->Left       = m_UVLampLabelArray[l]->Left;
		m_UVLampPanelArray[l]->Width      = UVSensor->Width;
		m_UVLampPanelArray[l]->Height     = UVSensor->Height;
		m_UVLampPanelArray[l]->Top        = m_UVLampsChecks[l]->Top;
		m_UVLampPanelArray[l]->BevelOuter = bvLowered;
		m_UVLampPanelArray[l]->Color      = clRed;
		m_UVLampPanelArray[l]->Caption    = "Off";
		m_UVLampPanelArray[l]->ParentBackground = false;
	}

	SetUVDimmingButton->TabOrder = NUMBER_OF_UV_LAMPS;

	LOAD_BITMAP(PowerButton->Glyph,            IDB_RED_LED);
	LOAD_BITMAP(OdourFanButton->Glyph,         IDB_RED_LED);
	LOAD_BITMAP(TurnRollerButton->Glyph,       IDB_RED_LED);
	LOAD_BITMAP(CoolingFansSpeedButton->Glyph, IDB_RED_LED);
	LOAD_BITMAP(TurnUVLampsButton->Glyph,      IDB_RED_LED);
	LOAD_BITMAP(LockDoorButton->Glyph,         IDB_UNLOCK_SMALL);

	//RSS, itamar
	CAppParams *ParamsMgr = CAppParams::Instance();
	if(!ParamsMgr->IsSuctionSystemExist)
	{
		RSSPumpOnValuePanel->Visible = false;
		RSSPumpOffValuePanel->Visible = false;
		SuctionValveOnLabel->Visible = false;
		SuctionValveOffLabel->Visible = false;
	}

}
//---------------------------------------------------------------------------

// Display the current parameters for the tray temperature, ambient temperature and roller speed
void TGeneralDeviceFrame::DisplayCurrentParameters()
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	int TrayA2DRequiredValue     = BackEnd->GetCurrentTrayTemperatureParameter();
	int TrayCelciusRequiredValue = BackEnd->TrayConvertA2DToCelcius(TrayA2DRequiredValue);

	A2DTrayTemperaturePanel->Caption = IntToStr(TrayA2DRequiredValue);

	ReqTrayTempUpDown->Position    = TrayCelciusRequiredValue;
	ReqRollerSpeedUpDown->Position = BackEnd->GetCurrentRollerSpeedParameter();
}
//---------------------------------------------------------------------------


void __fastcall TGeneralDeviceFrame::PowerOnActionExecute(TObject *Sender)
{
	CQLog::Write(LOG_TAG_GENERAL, "Turning power ON");

	CBackEndInterface *BackEnd = CBackEndInterface::Instance();

	try
	{
		BackEnd->TurnPower(true);

		QSleep(CAppParams::Instance()->HardwareInitDelay_mSec.Value());
		Raccoon::CRaccoon::Instance()->InitHardwareAndStart();

		BackEnd->ResumeOCBStatusThread();
		BackEnd->ResumeHeadStatusThread();

		BackEnd->GetLiquidTankInsertedStatus();   

		PowerButton->Glyph = NULL;
		PowerButton->Action = PowerOffAction;
		LOAD_BITMAP(PowerButton->Glyph,IDB_GREEN_LED);

		CQLog::Write(LOG_TAG_GENERAL, "Power turned ON");

	}
	catch(EQException& Exeception)
	{
		PowerButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::PowerOffActionExecute(TObject *Sender)
{
	CQLog::Write(LOG_TAG_GENERAL, "Turning power OFF");

	CBackEndInterface *BackEnd = CBackEndInterface::Instance();

	try
	{ 
		// First Pause following threads:
		BackEnd->PauseOCBStatusThread();  // the OCB status sender repeatedly calls GetLiquidTanksWeight(), which eventualy enables the RFID Sampling. This is undesired, after ReaderStopAndClear() is called. (see below)
		BackEnd->PauseHeadStatusThread(); // don't send messages to the turned off OHDB. (avoid communiaction errors)

		CQLog::Write(LOG_TAG_RACCOON,"SendTanksRemovalSignals.");
		BackEnd->SendTanksRemovalSignals();

		// Turn Off power supply:
		BackEnd->TurnPower(false);
		BackEnd->WaitForPowerOnOff();

		PowerButton->Glyph = NULL;
		PowerButton->Action = PowerOnAction;
		LOAD_BITMAP(PowerButton->Glyph,IDB_RED_LED);

		CQLog::Write(LOG_TAG_GENERAL, "Power turned OFF");

	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------


void __fastcall TGeneralDeviceFrame::LockDoorActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface::Instance()->LockDoor(true);

		LockDoorButton->Glyph = NULL;
		LockDoorButton->Action = UnlockDoorAction;
		LOAD_BITMAP(LockDoorButton->Glyph,IDB_LOCK_SMALL);

	}
	catch(EQException& Exeception)
	{
		LockDoorButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::UnlockDoorActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface* BackEnd = CBackEndInterface::Instance();

		BackEnd->LockDoor(false);

		LockDoorButton->Glyph = NULL;
		LockDoorButton->Action = LockDoorAction;
		LOAD_BITMAP(LockDoorButton->Glyph,IDB_UNLOCK_SMALL);


		BackEnd->EnableMotor(false,AXIS_ALL);
	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::MCBResetActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface::Instance()->MCBReset();
	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::TrayOnActionExecute(TObject *Sender)
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	try
	{
		// Get the requested temperature in Celcius
		int RequestedTemp = ReqTrayTempUpDown->Position;

		// Get the value in A2D units
		int A2DRequestedTemp = BackEnd->TrayConvertCelciusToA2D(RequestedTemp);

		// Update the A2D units Panel
		A2DTrayTemperaturePanel->Caption = IntToStr(A2DRequestedTemp);

		BackEnd->TurnTrayHeater(true, A2DRequestedTemp);
		TurnTrayButton->Glyph = NULL;
		TurnTrayButton->Action = TrayOffAction;
		LOAD_BITMAP(TurnTrayButton->Glyph,IDB_GREEN_LED);

	}
	catch(EQException& Exeception)
	{
		TurnTrayButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::TrayOffActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface::Instance()->TurnTrayHeater(false);

		TurnTrayButton->Glyph = NULL;
		TurnTrayButton->Action = TrayOnAction;
		LOAD_BITMAP(TurnTrayButton->Glyph,IDB_RED_LED);
	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------



void __fastcall TGeneralDeviceFrame::RollerOnActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface::Instance()->TurnRoller(true, ReqRollerSpeedUpDown->Position);

		TurnRollerButton->Glyph = NULL;
		TurnRollerButton->Action = RollerOffAction;
		LOAD_BITMAP(TurnRollerButton->Glyph,IDB_GREEN_LED);

	}
	catch(EQException& Exeception)
	{
		TurnRollerButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::RollerOffActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface::Instance()->TurnRoller(false);

		TurnRollerButton->Glyph = NULL;
		TurnRollerButton->Action = RollerOnAction;
		LOAD_BITMAP(TurnRollerButton->Glyph,IDB_RED_LED);

	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}

}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::UVLampsOnActionExecute(TObject *Sender)
{
	try
	{
		FOR_ALL_UV_LAMPS(l)
		m_UVLampsChecks[l]->Enabled = false;
		CBackEndInterface::Instance()->TurnSpecifiedUVLamps(true,m_UvLampsMask);

		TurnUVLampsButton->Glyph = NULL;
		TurnUVLampsButton->Action = UVLampsOffAction;
		LOAD_BITMAP(TurnUVLampsButton->Glyph,IDB_GREEN_LED);

	}
	catch(EQException& Exeception)
	{
		PowerButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::UVLampsOffActionExecute(TObject *Sender)
{
	try
	{
		FOR_ALL_UV_LAMPS(l)
		m_UVLampsChecks[l]->Enabled = true;
		CBackEndInterface::Instance()->TurnSpecifiedUVLamps(false,m_UvLampsMask);

		TurnUVLampsButton->Glyph = NULL;
		TurnUVLampsButton->Action = UVLampsOnAction;
		LOAD_BITMAP(TurnUVLampsButton->Glyph,IDB_RED_LED);

	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}

// Update status on the frame (integer version)
void TGeneralDeviceFrame::UpdateStatus(int ControlID,int Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}

// Update status on the frame (float version)
void TGeneralDeviceFrame::UpdateStatus(int ControlID,float Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}

// Update status on the frame (string version)
void TGeneralDeviceFrame::UpdateStatus(int ControlID,QString Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}


// Message handler for the WM_STATUS_UPDATE message
void TGeneralDeviceFrame::HandleStatusUpdateMessage(TMessage &Message)
{

	switch(Message.WParam)
	{
		case FE_CURRENT_VACUUM_LAST_CMH2O_VALUE:
		{
			float NewValue = *((float *)& Message.LParam);
			H2O_VacuumSensorLastValuePanel-> Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			break;
		}

		case FE_CURRENT_VACUUM_LAST_VALUE:
			A2D_VacuumSensorLastValuePanel->Caption = IntToStr(Message.LParam);
			break;

		case FE_CURRENT_VACUUM_AVERAGE_CMH2O_VALUE:
		{
			float NewValue = *((float *)& Message.LParam);
			H2O_VacuumSensorAveragePanel-> Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			break;
		}

		case FE_CURRENT_VACUUM_AVERAGE_VALUE:
			A2D_VacuumSensorAveragePanel->Caption = IntToStr(Message.LParam);
			break;

		case FE_CURRENT_TRAY_TEMPERATURE:
			C_TrayTemperaturePanel->Caption = IntToStr(Message.LParam);
			break;
		case FE_RSS_PUMP_ON_VALUE:  //RSS, itamar
			RSSPumpOnValuePanel->Caption = IntToStr(Message.LParam);
			break;
		case FE_RSS_PUMP_OFF_VALUE:  //RSS, itamar
			RSSPumpOffValuePanel->Caption = IntToStr(Message.LParam);
			break;
		case FE_CURRENT_TRAY_IN_A2D:
			A2D_TrayTemperaturePanel->Caption = IntToStr(Message.LParam);
			break;

		case FE_CURRENT_AMBIENT_TEMPERATURE:
			C_CurrAmbientTempPanel->Caption = IntToStr(Message.LParam);
			break;

		case FE_CURRENT_AMBIENT_IN_A2D:
			A2D_CurrAmbientTempPanel->Caption = IntToStr(Message.LParam);
			break;

		case FE_CURRENT_POWER_STATUS:
			if(Message.LParam)
			{
				PowerStatusPanel->Caption = "On";
				PowerStatusPanel->Color = clLime;
			}
			else
			{
				PowerStatusPanel->Caption = "Off";
				PowerStatusPanel->Color = clRed;
			}
			break;

		case FE_CURRENT_DOOR_STATUS:
			if(Message.LParam)
			{
				DoorStatusPanel->Caption = "Locked";
				DoorStatusPanel->Color = clLime;

				LockDoorButton->Glyph = NULL;
				LockDoorButton->Action = UnlockDoorAction;
				LOAD_BITMAP(LockDoorButton->Glyph,IDB_LOCK_SMALL);

				LockDoorButton->Down = true;
			}
			else
			{
				DoorStatusPanel->Caption = "Unlocked";
				DoorStatusPanel->Color = clRed;

				LockDoorButton->Glyph = NULL;
				LockDoorButton->Action = LockDoorAction;
				LOAD_BITMAP(LockDoorButton->Glyph,IDB_UNLOCK_SMALL);

				LockDoorButton->Down = false;
			}
			break;


		case FE_CURRENT_UV_LAMP_SENSOR_VALUE:
			UVSensor->Caption = IntToStr(Message.LParam);
			break;

		case FE_CURRENT_UV_LAMP_OCB_COMMAND:
			if(Message.LParam)
			{
				TurnUVLampsButton->Glyph = NULL;
				TurnUVLampsButton->Action = UVLampsOffAction;
				LOAD_BITMAP(TurnUVLampsButton->Glyph,IDB_GREEN_LED);
			}
			else
			{
				TurnUVLampsButton->Glyph = NULL;
				TurnUVLampsButton->Action = UVLampsOnAction;
				LOAD_BITMAP(TurnUVLampsButton->Glyph,IDB_RED_LED);
			}
			break;

			CASE_MESSAGE_ALL_LAMPS(FE_CURRENT_UV_LAMP_STATUS)
			if(Message.LParam)
			{
				m_UVLampPanelArray[Message.WParam - FE_CURRENT_UV_LAMP_STATUS_BASE]->Color = clLime;
				m_UVLampPanelArray[Message.WParam - FE_CURRENT_UV_LAMP_STATUS_BASE]->Caption = "On";
			}
			else
			{
				m_UVLampPanelArray[Message.WParam - FE_CURRENT_UV_LAMP_STATUS_BASE]->Color = clRed;
				m_UVLampPanelArray[Message.WParam - FE_CURRENT_UV_LAMP_STATUS_BASE]->Caption = "Off";
			}
			break;

		case FE_CURRENT_ROLLER_STATUS:
			if(Message.LParam)
			{
				RollerStatusPanel->Caption = "On";
				RollerStatusPanel->Color = clLime;
			}
			else
			{
				RollerStatusPanel->Caption = "Off";
            RollerStatusPanel->Color = clRed;
			}
         break;
		case FE_ACTUATOR_ODOR_FAN:
			if(Message.LParam)
			{
				FanStatusPanel->Caption = "On";
				FanStatusPanel->Color = clLime;
			}
			else
			{
				FanStatusPanel->Caption = "Off";
				FanStatusPanel->Color = clRed;
			}
			break;

		default:
			break;
	}
}
//---------------------------------------------------------------------------

// Update the current devices status
void TGeneralDeviceFrame::UpdateDevicesStatus()
{
	try
	{
		CBackEndInterface* BackEnd = CBackEndInterface::Instance();

		if(BackEnd->GetUVLampsLastCommand())
		{
			TurnUVLampsButton->Action = UVLampsOffAction;
			TurnUVLampsButton->Down   = true;
		}
		else
		{
			TurnUVLampsButton->Action = UVLampsOnAction;
			TurnUVLampsButton->Down = false;
		}

		TurnRollerButton->Glyph = NULL;
		if(BackEnd->IsRollerTurnedOn())
		{
			TurnRollerButton->Action = RollerOffAction;
			TurnRollerButton->Down   = true;
		}
		else
		{
			TurnRollerButton->Action = RollerOnAction;
			TurnRollerButton->Down   = false;
		}

		if(BackEnd->IsPowerTurnedOn())
		{
			PowerButton->Down   = true;
			PowerButton->Glyph = NULL;
			PowerButton->Action = PowerOffAction;
			LOAD_BITMAP(PowerButton->Glyph,IDB_GREEN_LED);
		}
		else
		{
			PowerButton->Down   = false;
			PowerButton->Glyph = NULL;
			PowerButton->Action = PowerOnAction;
			LOAD_BITMAP(PowerButton->Glyph,IDB_RED_LED);
		}
	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}

}

void __fastcall TGeneralDeviceFrame::CoolingFansOnActionExecute(TObject *Sender)
{
	try
	{
		int DutyCycle = CoolingFansDutyCycleUpDown->Position;
		CBackEndInterface::Instance()->TurnCoolingFansOnOff(true, DutyCycle);

		CoolingFansSpeedButton->Glyph = NULL;
		CoolingFansSpeedButton->Action = CoolingFansOffAction;
		LOAD_BITMAP(CoolingFansSpeedButton->Glyph,IDB_GREEN_LED);
	}
	catch(EQException& Exeception)
	{
		CoolingFansSpeedButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}

}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::CoolingFansOffActionExecute(TObject *Sender)
{
	try
	{
		CBackEndInterface::Instance()->TurnCoolingFansOnOff(false);

		CoolingFansSpeedButton->Glyph = NULL;
		CoolingFansSpeedButton->Action = CoolingFansOnAction;
		LOAD_BITMAP(CoolingFansSpeedButton->Glyph,IDB_RED_LED);

	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::AmbientOdourFanOffActionExecute(TObject *Sender)
{
	try
	{
		int RequestedVoltage = OdourFanValueUpDown->Position;

		CBackEndInterface::Instance()->SetHoodFanParamOnOff(false, RequestedVoltage) ;

		OdourFanButton->Glyph = NULL;
		OdourFanButton->Action = AmbientOdourFanOnAction;
		LOAD_BITMAP(OdourFanButton->Glyph,IDB_RED_LED);

	}
	catch(EQException& Exeception)
	{
		OdourFanButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::AmbientOdourFanOnActionExecute(TObject *Sender)
{
	try
	{
		int RequestedVoltage = OdourFanValueUpDown->Position;
		CBackEndInterface::Instance()->SetHoodFanParamOnOff(true, RequestedVoltage) ;

		OdourFanButton->Glyph = NULL;
		OdourFanButton->Action = AmbientOdourFanOffAction;
		LOAD_BITMAP(OdourFanButton->Glyph,IDB_GREEN_LED);
	}
	catch(EQException& Exeception)
	{
		OdourFanButton->Down = false;
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::DisplayValuesModeCheckBoxClick(TObject *Sender)
{
	MainForm->GetUIFrame()->GetMaintenanceStatusFrame()->SetDisplayMode(DisplayValuesModeCheckBox->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::RestartAveragingButtonClick(TObject *Sender)
{
	CBackEndInterface* BackEnd  = CBackEndInterface::Instance();

	try
	{
		if(BackEnd->SetUVSamplingParams(true) != Q_NO_ERROR)
			return;
	}
	catch(EUvLamps& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
		return;
	}

	SensorSyncAveragePanel->Caption = "0";
	Timer1->Interval = 1000;
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TGeneralDeviceFrame::Timer1Timer(TObject *Sender)
{
	CBackEndInterface* BackEnd  = CBackEndInterface::Instance();

	Timer1->Enabled = false;

	for(int i=0; i < 5; i++)
	{
		try
		{
			// Force UV sensor read from OCB:
			BackEnd->GetUVValue();
			break;
		}
		catch(EUvLamps& Exeception)
		{
			// Wait here since it could be we're having a conflict with another call to GetUVValue() by OCBStatusSender.
			// In that case we should let the other call finish.
			QSleep(5);
		}
	}

	// Get the UV Sensor value:
	SensorSyncAveragePanel->Caption = IntToStr(BackEnd->GetUVSensorValue(true));
}
//---------------------------------------------------------------------------


void __fastcall TGeneralDeviceFrame::SetUVDimmingButtonClick(TObject *Sender)
{
	try
	{
		CAppParams *ParamsMgr = CAppParams::Instance();
		int MsgRetVal;
		MsgRetVal = MessageDlg("Are you sure you want to update the UV settings?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0);
		if(MsgRetVal != mrYes)
			return;
		CBackEndInterface* BackEnd  = CBackEndInterface::Instance();
		FOR_ALL_UV_LAMPS(l)
		{
			ParamsMgr->UVLampPSValueArray[l] = ParamsMgr->UVPowerSupplyValuesArray30[m_SetUVDimming[l]];
			m_UVPanelArray[l]->Caption = IntToStr(m_SetUVDimming[l] + 1); // +1 to align the display to array with [1..30] instead of [0..29]
			// also set the D2A of the lamps
			BackEnd->SetUVD2AValue(l, ParamsMgr->UVLampPSValueArray[l]);
		}
		ParamsMgr->SaveSingleParameter(&ParamsMgr->UVLampPSValueArray);
	}
	catch(EQException& Exeception)
	{
		QMonitor.ErrorMessage(Exeception.GetErrorMsg());
	}

}
//---------------------------------------------------------------------------

void TGeneralDeviceFrame::UpdateUVDimming()
{
	CAppParams *ParamsMgr = CAppParams::Instance();

	for(int i = 0; i < SIZE_OF_UV_POWER_SUPPLY_ARRAY_30; i++)
	{
		FOR_ALL_UV_LAMPS(l)
		if(ParamsMgr->UVLampPSValueArray[l] >= ParamsMgr->UVPowerSupplyValuesArray30[i])
		{
			m_SetUVDimming[l]              = i;
			//m_UVRadioButton[l][i]->Checked = true;
		}

	}

	FOR_ALL_UV_LAMPS(l)
	{
		m_UVTrackBarArray[l]->Position = m_SetUVDimming[l];
		m_UVPanelArray[l]->Caption = IntToStr(m_SetUVDimming[l] + 1); // +1 to align the display to array with [1..30] instead of [0..29]
		m_UVLampsChecks[l]->Checked = ParamsMgr->UVSingleLampEnabledArray[l];
	}
}


void __fastcall TGeneralDeviceFrame::UVTrackBarChange(TObject *Sender)
{
	TTrackBar *MyTrackBar = dynamic_cast<TTrackBar *>(Sender);
	if(MyTrackBar != NULL)
	{
		// Keep wanted UV Dimming value
		m_SetUVDimming[MyTrackBar->Tag] = MyTrackBar->Position;
		m_UVPanelArray[MyTrackBar->Tag]->Caption = IntToStr(MyTrackBar->Position + 1) + "*"; // +1 to align the display to array with [1..30] instead of [0..29]
	}

}

void __fastcall TGeneralDeviceFrame::UVLampsCheckBoxClick(TObject *Sender)
{
	TCheckBox* CheckBox = dynamic_cast<TCheckBox*>(Sender);
	if(!CheckBox)
		return; //todo error/warning
	if(CheckBox->Checked)
		m_UvLampsMask |= (1<<CheckBox->Tag);
	else
		m_UvLampsMask &= ~(1<<CheckBox->Tag);
	TurnUVLampsButton->Enabled = m_UvLampsMask;
}
