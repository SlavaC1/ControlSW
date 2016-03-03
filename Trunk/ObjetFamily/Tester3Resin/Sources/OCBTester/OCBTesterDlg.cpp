//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include <functional>
#include "QTimer.h"
#include "QThreadUtils.h"
#include "OCBTesterDlg.h"
#include "OCBCommDefs.h"
#include "QMonitor.h"
#include "TesterApplication.h"
#include <IniFiles.hpp>
#include "GlobalDefs.h"
#include "AppLogFile.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOCBTesterForm *OCBTesterForm;

//#define SHOW_MESSAGE_BOXES

const int MODEL_ON_TRAY_SENSOR_1_MASK = 0x01;
const int MODEL_ON_TRAY_SENSOR_2_MASK = 0x02;

const int GREEN_OFF_LED = 0;
const int GREEN_ON_LED  = 1;
const int RED_OFF_LED   = 2;
const int RED_ON_LED    = 3;

//const int OFF = 0;
const int ON  = 1;

const int SENSOR_OFF = 0;
const int SENSOR_ON  = 1;

const int TRAY_INSERTED = 1;

const int NUMBER_OF_ANALOGS_IN_CPU     = 8;
const int NUMBER_OF_ANALOGS_IN_A2D_CPU = 24;

const int OCB_ACTUATOR_IN_BYTES = 6;
const int OCB_INPUTS_IN_BYTES   = 3;

const int PORT_ENABLE_ACTUATOR_ID = 8;
const int INTERLOCK_SENSOR_ID     = 10;

const int DOOR_OUTSIDE_UNLOCKED = 0;
const int DOOR_INSIDE_UNLOCKED  = 1;
const int DOOR_OUTSIDE_LOCKED   = 2;
const int DOOR_INSIDE_LOCKED    = 3;

const int LIQUID_TANK_IS_EMPTY = 0;

const char OCB_TESTER_INI_FILE[] = "OCBTester.ini";

__fastcall TOCBTesterForm::TOCBTesterForm(TComponent* Owner)
    : TForm(Owner)
{
    IsFirstInputsUpdate    = true;
    IsFirstActuatorsUpdate = true;

    for (int i=0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
    {
        m_ChamberCurrentPumpState[i] = 0;
    }

    for (int i=0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
    {
        m_ChamberLiquidLevel[i] = 0;
	}

	for(int i = 0; i < TanksExistenceGroupBox->ControlCount; i++)
	{
		TLabeledEdit *Edit  = dynamic_cast<TLabeledEdit *>(TanksExistenceGroupBox->Controls[i]);

		if(Edit)
			m_TankInplaces[Edit->Tag] = Edit;
	}

	for(int i = 0; i < TanksWeightGroupBox->ControlCount; i++)
	{
		TLabeledEdit *Edit  = dynamic_cast<TLabeledEdit *>(TanksWeightGroupBox->Controls[i]);

		if(Edit)
			m_TankWeights[Edit->Tag] = Edit;
	}
}
//---------------------------------------------------------------------------

void TOCBTesterForm::Open(CEdenProtocolEngine *ProtocolEngine)
{
    if (m_ProtocolClient == NULL)
    {
        m_ProtocolEngine = ProtocolEngine;
        COCBProtocolClient::Init(m_ProtocolEngine);
        m_ProtocolClient = COCBProtocolClient::Instance();
        InstallNotificationHandlers();
        m_TimeoutThread = new CTimeoutThread(&m_TimeoutList,m_ProtocolClient);
        if ((CAppParams::GetInstance())->OCBCommCheck)
            m_CommStatusThread = new CCommStatusThread(m_ProtocolClient);

		m_DisplayThread = new COCBDisplayThread(m_ProtocolClient,
												reinterpret_cast<TGenericCockie>(this),
												&ActuatorReceiveHandler,
												&A2DReceiveHandler,
												&InputsReceiveHandler,
												&DoorReceiveHandler,
												&PurgeStatusReceiveHandler,
												&FillingReceiveHandler,
												&GetUVLampsStatusReceiveHandler,
												&IsTrayInsertedReceiveHandler,
												&PowerStatusHandler,
												&LiquidTanksStatusHandler,
												&LiquidTanksWeightsHandler);

        m_IsSWResetInUse = false;
        m_IsPowerOnInUse = false;
        m_IsHeatingOnInUse = false;
        m_IsTrayParametersInUse = false;
        m_IsUVLampsInUse = false;
        m_IsUVLampsParameterInUse = false;
        m_IsPerformPurgeInUse = false;
        m_IsSWVersionInUse = false;
        m_IsLockDoorInUse = false;
        m_IsActivePumpsInUse = false;
        m_IsHeadParamsInUse = false;
        m_IsFillingInUse = false;
        m_IsOdourInUse = false;
        m_IsA2DDeviceInUse = false;
    }

	Show();

	if(! m_DisplayThread->Running)
		m_DisplayThread->Resume();
	
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::StatusBar1DrawPanel(TStatusBar *StatusBar,
        TStatusPanel *Panel, const TRect &Rect)
{
    StatusBar1->Canvas->Draw(Rect.Left,Rect.Top, m_LedBMP);
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::LedTimerTimer(TObject *Sender)
{
    if (GetCommStatus())
    {
        if (m_LedOn == true)
        {
            LEDsImageList->GetBitmap(GREEN_OFF_LED, m_LedBMP);
            m_LedOn = false;
        }
        else
        {
            LEDsImageList->GetBitmap(GREEN_ON_LED, m_LedBMP);
            m_LedOn = true;
        }
    }
    else    // the comm status is not ok
    {
        if (m_LedOn == true)
        {
            LEDsImageList->GetBitmap(RED_OFF_LED, m_LedBMP);
            m_LedOn = false;
        }
        else
        {
            LEDsImageList->GetBitmap(RED_ON_LED, m_LedBMP);
            m_LedOn = true;
        }
    }

    StatusBar1->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::FormCreate(TObject *Sender)
{
    m_ProtocolClient = NULL;
    m_TimeoutThread = NULL;
    m_CommStatusThread = NULL;
    m_DisplayThread = NULL;

    m_LedBMP = new Graphics::TBitmap();
    m_LedBMP->Transparent = true;
    m_LedOn = false;
    m_CommStatus = false;

    // Updating the Param Menager
    CAppParams *ParamsMgr = CAppParams::GetInstance();

    Model1LevelThermistorLowEdit->Text    = IntToStr((int)ParamsMgr->Model1LevelThermistorLow);
    Model2LevelThermistorLowEdit->Text    = IntToStr((int)ParamsMgr->Model2LevelThermistorLow);
    Model3LevelThermistorLowEdit->Text    = IntToStr((int)ParamsMgr->Model3LevelThermistorLow);
    Model1LevelThermistorHighEdit->Text   = IntToStr((int)ParamsMgr->Model1LevelThermistorHigh);
    Model2LevelThermistorHighEdit->Text   = IntToStr((int)ParamsMgr->Model2LevelThermistorHigh);
    Model3LevelThermistorHighEdit->Text   = IntToStr((int)ParamsMgr->Model3LevelThermistorHigh);
    SupportLevelThermistorLowEdit->Text   = IntToStr((int)ParamsMgr->SupportLevelThermistorLow);
    SupportLevelThermistorHighEdit->Text  = IntToStr((int)ParamsMgr->SupportLevelThermistorHigh);

    ModelLevelThermistorLowEdit->Text   = IntToStr((int)ParamsMgr->ModelLevelThermistorLow);
    ModelLevelThermistorHighEdit->Text  = IntToStr((int)ParamsMgr->ModelLevelThermistorHigh);
    SLevelThermistorLowEdit->Text   = IntToStr((int)ParamsMgr->SLevelThermistorLow);
    SLevelThermistorHighEdit->Text  = IntToStr((int)ParamsMgr->SLevelThermistorHigh);

    SLowThCheckBox->Checked = ParamsMgr->ActiveThermistors[SUPPORT_CHAMBER_THERMISTOR];
    M1LowThCheckBox->Checked = ParamsMgr->ActiveThermistors[M1_CHAMBER_THERMISTOR];
    M2LowThCheckBox->Checked = ParamsMgr->ActiveThermistors[M2_CHAMBER_THERMISTOR];
    M3LowThCheckBox->Checked = ParamsMgr->ActiveThermistors[M3_CHAMBER_THERMISTOR];
    MHighThCheckBox->Checked = ParamsMgr->ActiveThermistors[M1_M2_CHAMBER_THERMISTOR];
    SHighThCheckBox->Checked = ParamsMgr->ActiveThermistors[SUPPORT_MODEL_CHAMBER_THERMISTOR];

    PumpOnEdit->Text                     = IntToStr((int)ParamsMgr->TimePumpON);
    PumpOffEdit->Text                    = IntToStr((int)ParamsMgr->TimePumpOFF);
    TimeoutFillingEdit->Text             = IntToStr((int)ParamsMgr->TimeoutFilling);
}

//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::FormDestroy(TObject *Sender)
{
    if (m_TimeoutThread)
        Q_SAFE_DELETE(m_TimeoutThread);

    if(m_CommStatusThread)
        Q_SAFE_DELETE(m_CommStatusThread);

    if (m_DisplayThread)
    {
        m_DisplayThread->Terminate();
        m_DisplayThread->WaitFor();
        Q_SAFE_DELETE(m_DisplayThread);
    }

    if (m_LedBMP)
        Q_SAFE_DELETE(m_LedBMP);
}
// --------------------------------------------------------------------------

void TOCBTesterForm::InstallNotificationHandlers()
{
    m_ProtocolClient->InstallMessageHandler(OCB_TRAY_TEMPERATURE_IN_RANGE,  TrayTemperatureIsOKHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_POWER_ERROR,                PowerErrorHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_POWER_IS_ON_OFF,            PowerIsOnOffHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_UV_LAMPS_ARE_ON,            UVLampsAreOnOffHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_UV_LAMPS_ERROR,             UVLampsErrorHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_PURGE_END,                  PurgeEndHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_PURGE_ERROR,                PurgeErrorHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_HEADS_FILLING_ERROR,        HeadsFillingErrorHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_DOOR_LOCK_NOTIFICATION,     DoorIsLockedHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_RESET_WAKE_UP,              ResetWakeUpHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_LIQUID_TANK_STATUS_CHANGED, LiquidTanksStatusChangesHandler, reinterpret_cast<TGenericCockie>(this));
    m_ProtocolClient->InstallMessageHandler(OCB_HEADS_FILLING_NOTIFICATION, HeadsFillingHandler, reinterpret_cast<TGenericCockie>(this));
}
//---------------------------------------------------------------------------

void TOCBTesterForm::AckHandler(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    TOCBTesterForm *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    // Build the read data message
    TOCBAck *Msg = static_cast<TOCBAck *>(Data);

    if(Msg->MessageID != OCB_ACK)
        return;

    InstancePtr->UpdateTimeoutFlag(Msg->RespondedMessageID);

    // remove this transaction handle from the time out list
    InstancePtr->RemoveFromTimeOutList(TransactionID, Data, DataLength, Cockie);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void TOCBTesterForm::RemoveFromTimeOutList(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    TOCBTesterForm *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    // remove this transaction handle from the time out list
    InstancePtr->m_TimeoutList.StartProtect();

    InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionID);

    for (TTimeoutList::iterator i = InstancePtr->m_TimeoutList.begin(); i != InstancePtr->m_TimeoutList.end(); ++i)
    {
        if (i->TransactionID == TransactionID)
        {
            InstancePtr->m_TimeoutList.erase(i);
            break;
        }
    }

    InstancePtr->m_TimeoutList.StopProtect();
}
//---------------------------------------------------------------------------


void TOCBTesterForm::SetCommStatus(bool Status)
{
    m_CommStatus = Status;
}
//---------------------------------------------------------------------------

bool TOCBTesterForm::GetCommStatus()
{
    return m_CommStatus;
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::PowerOnActionExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsPowerOnInUse == false)
    {
        try
        {
            m_IsPowerOnInUse = true;

            // build the message
            TOCBSetPowerParamsMessage Message;
            Message.MessageID = OCB_SET_POWER_PARMS;
            Message.OnDelay = StrToInt(PowerOnDelayEdit->Text);
            Message.OffDelay = StrToInt(PowerOffDelayEdit->Text);


            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetPowerParamsMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SET_POWER_PARMS;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }


        // build the message
        TOCBSetPowerOnOffMessage Message;
        Message.MessageID  = OCB_SET_POWER_ON_OFF;
        Message.PowerOnOff = ON;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetPowerOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_POWER_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        // Changing the button....
        PowerButton->Action  = PowerOffAction;
    }
}
//---------------------------------------------------------------------------



void __fastcall TOCBTesterForm::PowerOffActionExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsPowerOnInUse == false)
    {
        m_IsPowerOnInUse = true;

        // build the message
        TOCBSetPowerOnOffMessage Message;
        Message.MessageID  = OCB_SET_POWER_ON_OFF;
        Message.PowerOnOff = OFF;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetPowerOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_POWER_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        // Changing the button....
        PowerButton->Action  = PowerOnAction;
    }
}

//---------------------------------------------------------------------------
void TOCBTesterForm::LiquidTanksWeightsHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBLiquidTanksWeightResponse *LiquidTanksWeightsMsg = static_cast<TOCBLiquidTanksWeightResponse *>(Data);
    TOCBTesterForm                *InstancePtr          = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (LiquidTanksWeightsMsg->MessageID != OCB_LIQUID_TANK_WEIGHT)
        return;

	InstancePtr->m_LiquidTanksWeightsDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);

	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
		InstancePtr->m_TankWeight[i] = LiquidTanksWeightsMsg->TankWeight[i];

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_LIQUID_TANKS_WEIGHTS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::LiquidTanksStatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBLiquidTankStatusMessage *LiquidTanksStatusMsg = static_cast<TOCBLiquidTankStatusMessage *>(Data);
    TOCBTesterForm              *InstancePtr          = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (LiquidTanksStatusMsg->MessageID != OCB_LIQUID_TANK_INSERTED_STATUS)
        return;

    InstancePtr->m_LiquidTanksDisplayEvent.SetEvent();
	InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);

	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
		InstancePtr->m_TankStatus[i] = LiquidTanksStatusMsg->TankStatus[i];
		
    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_LIQUID_TANKS_STATUS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::PowerStatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBPowerStatusResponse *PowerStatusMsg = static_cast<TOCBPowerStatusResponse *>(Data);
    TOCBTesterForm          *InstancePtr    = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (PowerStatusMsg->MessageID != OCB_POWER_STATUS)
        return;

    InstancePtr->m_PowerDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    InstancePtr->m_PowerStatus = PowerStatusMsg->PowerOnOff;

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_POWER_STATUS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::TrayStatusReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBTrayStatusResponse *TrayStatusMsg = static_cast<TOCBTrayStatusResponse *>(Data);
    TOCBTesterForm         *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (TrayStatusMsg->MessageID != OCB_TRAY_STATUS)
        return;

    InstancePtr->m_TrayStatusDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    InstancePtr->m_CurrentTrayTemp = TrayStatusMsg->CurrentTrayTemp;

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_CURRENT_TRAY_TEMP_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::LiquidTanksStatusChangesHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBLiquidTanksStatusChangedResponse *Msg         = static_cast<TOCBLiquidTanksStatusChangedResponse *>(Data);
    TOCBTesterForm                       *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_LIQUID_TANK_STATUS_CHANGED)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_LIQUID_TANK_STATUS_CHANGED);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_LIQUID_TANK_STATUS_CHANGED,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::ResetWakeUpHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBResetWakeUpResponse *Msg         = static_cast<TOCBResetWakeUpResponse *>(Data);
    TOCBTesterForm          *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_RESET_WAKE_UP)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_RESET_WAKE_UP);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_RESET_WAKE_UP,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::DoorIsLockedHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBDoorLockNotificationResponse *Msg         = static_cast<TOCBDoorLockNotificationResponse *>(Data);
    TOCBTesterForm                   *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_DOOR_LOCK_NOTIFICATION)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_DOOR_LOCK_NOTIFICATION);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_DOOR_LOCK_NOTIFICATION,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::HeadsFillingErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBHeadFillingErrorResponse *Msg         = static_cast<TOCBHeadFillingErrorResponse *>(Data);
    TOCBTesterForm               *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_HEADS_FILLING_ERROR)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_HEADS_FILLING_ERROR);

    InstancePtr->m_FillingErrorType = Msg->HeadFillingError;

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_HEADS_FILLING_ERROR,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::PurgeErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBPurgeErrorResponse *Msg         = static_cast<TOCBPurgeErrorResponse *>(Data);
    TOCBTesterForm         *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_PURGE_ERROR)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_PURGE_ERROR);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_PURGE_ERROR,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::PurgeEndHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBPurgeEndResponse *Msg         = static_cast<TOCBPurgeEndResponse *>(Data);
    TOCBTesterForm       *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_PURGE_END)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_PURGE_END);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_PURGE_END,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::UVLampsErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBUvLampErrorResponse *Msg         = static_cast<TOCBUvLampErrorResponse *>(Data);
    TOCBTesterForm          *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_UV_LAMPS_ERROR)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_UV_LAMPS_ERROR);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_UV_LAMPS_ERROR,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::UVLampsAreOnOffHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBUvLampsAreOnOffResponse *Msg         = static_cast<TOCBUvLampsAreOnOffResponse *>(Data);
    TOCBTesterForm              *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_UV_LAMPS_ARE_ON)
        return;

    InstancePtr->m_IsUVLampsAreOn = (Msg->LampState == ON ? true : false);

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_UV_LAMPS_ARE_ON);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_UV_LAMPS_ARE_ON_OFF,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::PowerIsOnOffHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBPowerAreOnOffResponse *Msg         = static_cast<TOCBPowerAreOnOffResponse *>(Data);
    TOCBTesterForm            *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_POWER_IS_ON_OFF)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_POWER_IS_ON_OFF);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_POWER_IS_ON_OFF,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::PowerErrorHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBPowerError *Msg         = static_cast<TOCBPowerError *>(Data);
    TOCBTesterForm *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (Msg->MessageID != OCB_POWER_ERROR)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_POWER_ERROR);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_POWER_ERROR,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::TrayTemperatureIsOKHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBTrayTemperatureInRangeResponse *IsTrayTempInRangeMsg = static_cast<TOCBTrayTemperatureInRangeResponse *>(Data);
    TOCBTesterForm                     *InstancePtr          = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (IsTrayTempInRangeMsg->MessageID != OCB_TRAY_TEMPERATURE_IN_RANGE)
        return;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_TRAY_TEMPERATURE_IN_RANGE);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_TRAY_TEMP_IN_RANGE_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::IsTrayInsertedReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBTrayInStatusResponse *IsTrayInMsg = static_cast<TOCBTrayInStatusResponse *>(Data);
    TOCBTesterForm           *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (IsTrayInMsg->MessageID != OCB_TRAY_IN_STATUS)
        return;

    InstancePtr->m_IsTrayInDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    InstancePtr->m_IsTrayIn = IsTrayInMsg->TrayExistense;

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_IS_TRAY_IN_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::GetUVLampsStatusReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBUvLampStatusResponse *UVLampsStatusMsg = static_cast<TOCBUvLampStatusResponse *>(Data);
    TOCBTesterForm           *InstancePtr      = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (UVLampsStatusMsg->MessageID != OCB_UV_LAMPS_STATUS)
        return;

    InstancePtr->m_UVLampsStatusDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    InstancePtr->m_RightUVLampValue = UVLampsStatusMsg->RightOnOff;
    InstancePtr->m_LeftUVLampValue  = UVLampsStatusMsg->LeftOnOff;

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_UV_LAMPS_STATUS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::FillingReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBHeadFillingStatusResponse *FillingMsg = static_cast<TOCBHeadFillingStatusResponse *>(Data);
    TOCBTesterForm                *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (FillingMsg->MessageID != OCB_HEADS_FILLING_STATUS)
        return;

    InstancePtr->m_FillingDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);

    InstancePtr->m_FillingOnOff           = FillingMsg->HeadFillingControlOnOff;
    InstancePtr->m_Model1ThermistorLevel  = FillingMsg->ResinLevel[M1_CHAMBER_THERMISTOR];
    InstancePtr->m_Model2ThermistorLevel  = FillingMsg->ResinLevel[M2_CHAMBER_THERMISTOR];
    InstancePtr->m_Model3ThermistorLevel  = FillingMsg->ResinLevel[M3_CHAMBER_THERMISTOR];
    InstancePtr->m_SupportThermistorLevel = FillingMsg->ResinLevel[SUPPORT_CHAMBER_THERMISTOR];

    InstancePtr->m_SThermistorLevel = FillingMsg->ResinLevel[SUPPORT_MODEL_CHAMBER_THERMISTOR];
    InstancePtr->m_ModelThermistorLevel = FillingMsg->ResinLevel[M1_M2_CHAMBER_THERMISTOR];

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_FILLING_READINGS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::HeadsFillingActivePumpsHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    /*
      TOCBHeadsFillingActivePumpsMsg *FillingPumpsMsg = static_cast<TOCBHeadsFillingActivePumpsMsg *>(Data);
      TOCBTesterForm                 *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

      if (FillingPumpsMsg->MessageID != OCB_HEADS_FILLING_ACTIVE_PUMPS)
        return;

      // remove this transaction handle from the time out list
      InstancePtr->RemoveFromTimeOutList(TransactionId, Data, DataLength, Cockie);

      InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
      InstancePtr->m_ModelPump = FillingPumpsMsg->ModelPumpID;
      InstancePtr->m_SupportPump = FillingPumpsMsg->SupportPumpID;

      // post a message to the dialog to display the current received reading
      PostMessage(InstancePtr->Handle,WM_OCB_HEADS_FILLING_PUMPS,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
    */
}

//---------------------------------------------------------------------------
void TOCBTesterForm::SWVersionHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBDriverSoftwareVersionResponse *SWVersionMsg = static_cast<TOCBDriverSoftwareVersionResponse *>(Data);
    TOCBTesterForm                    *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (SWVersionMsg->MessageID != OCB_SW_VERSION)
        return;

    // remove this transaction handle from the time out list
    InstancePtr->RemoveFromTimeOutList(TransactionId, Data, DataLength, Cockie);

//  InstancePtr->m_SWVersionDisplayEvent.SetEvent();
//  InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
	InstancePtr->m_ExternalVersion = SWVersionMsg->ExternalVersion;
	InstancePtr->m_InternalVersion = SWVersionMsg->InternalVersion;

	for(int i = 0; i < NUM_OF_MSC_CARDS; i++)
	{
		InstancePtr->m_MSCFirmwareVersion[i] = SWVersionMsg->MSCFirmwareVersion[i];
		InstancePtr->m_MSCHardwareVersion[i] = SWVersionMsg->MSCHardwareVersion[i];
	}

	InstancePtr->m_IsSWVersionInUse = false;   

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_SW_VERSION_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::PurgeStatusReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBPurgeStatusResponse *PurgeMsg = static_cast<TOCBPurgeStatusResponse *>(Data);
    TOCBTesterForm          *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (PurgeMsg->MessageID != OCB_PURGE_STATUS)
        return;

    InstancePtr->m_PurgeDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    InstancePtr->m_PurgeStatus = PurgeMsg->PurgeStatus;

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_PURGE_STATUS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::DoorReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBDoorStatusResponse *DoorMsg      = static_cast<TOCBDoorStatusResponse *>(Data);
    TOCBTesterForm         *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (DoorMsg->MessageID != OCB_DOOR_STATUS)
        return;

    InstancePtr->m_DoorDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    for(int i=0; i<MAX_NUM_OF_DOORS; ++i)
    {
        InstancePtr->m_DoorStatus[i] = DoorMsg->DoorStatus[i];
    }

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_DOOR_STATUS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::InputsReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBInputsStatusResponse *InputsMsg = static_cast<TOCBInputsStatusResponse *>(Data);
    TOCBTesterForm           *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (InputsMsg->MessageID != OCB_INPUT_STATUS)
        return;

    InstancePtr->m_InputsDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    //memcpy(&(InstancePtr->m_InputsValues), InputsMsg->InputsArray, OCB_INPUTS_IN_BYTES * sizeof(BYTE));

    for(int i=0, j=0; j < 3; j++)
    {
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x01);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x02);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x04);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x08);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x10);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x20);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x40);
        InstancePtr->m_InputsValues[i++] = ((int)InputsMsg->InputsArray[j] & 0x80);
    }

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_INPUTS_READINGS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

//---------------------------------------------------------------------------
void TOCBTesterForm::ActuatorReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBActuatorsStatusResponse *ActuatorsMsg = static_cast<TOCBActuatorsStatusResponse *>(Data);
    TOCBTesterForm              *InstancePtr  = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (ActuatorsMsg->MessageID != OCB_ACTUATOR_STATUS)
        return;

    InstancePtr->m_ActuatorsDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    //memcpy(&(InstancePtr->m_ActuatorsValues), ActuatorsMsg->ActuatorsArray, OCB_ACTUATOR_IN_BYTES * sizeof(BYTE));

    for(int i=0, j=0; j < 6; j++)
    {
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x01);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x02);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x04);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x08);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x10);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x20);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x40);
        InstancePtr->m_ActuatorsValues[i++] = ((int)ActuatorsMsg->ActuatorsArray[j] & 0x80);
    }

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_ACTUATORS_READINGS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}
//-------------------------------------------------------------------------

void TOCBTesterForm::A2DReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{

    TOCDBA2DReadingsResponse *A2DMsg      = static_cast<TOCDBA2DReadingsResponse *>(Data);
    TOCBTesterForm           *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    if (A2DMsg->MessageID != OCB_A2D_READINGS_STATUS)
        return;

    InstancePtr->m_A2DDisplayEvent.SetEvent();
    InstancePtr->m_DisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    memcpy(InstancePtr->m_A2DValues,A2DMsg->Readings,OCB_NUM_OF_ANALOG_INPUTS * sizeof(WORD));

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_A2D_READINGS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}
//---------------------------------------------------------------------------


// display an Actuators reading
MESSAGE void TOCBTesterForm::DisplayActuatorsReading(TMessage& Message)
{
    int ActuatorValue     = -1;
    int PrevActuatorValue = -1;
    int j = 0;

    // In case there were no changes - do nothing
    /*for (j = 0; j < NUMBER_OF_ACTUATORS; j++)
    {
     if((m_PrevActuatorsValues[j] != m_ActuatorsValues[j]) && (IsFirstActuatorsUpdate != true))
     {
         j=0;
       break;
     }
    }
    if(j==NUMBER_OF_ACTUATORS)
    return;*/

    for (int InputNum = 0; InputNum < NUMBER_OF_ACTUATORS; InputNum++)
    {
        if((m_PrevActuatorsValues[InputNum] == m_ActuatorsValues[InputNum]) && (IsFirstActuatorsUpdate != true))
            continue;
        m_PrevActuatorsValues[InputNum] = m_ActuatorsValues[InputNum];
        // find the panel with a tag that is equal to the analog input number
        TImage* Image = NULL;

        for (int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
        {
            if(ActuatorsGroupBox->Controls[i]->Tag == InputNum)
            {
                Image = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
                if (Image != NULL)
                {
                    //ActuatorValue     = 1 & (m_ActuatorsValues >> InputNum);
                    //PrevActuatorValue = 1 & (m_PrevActuatorsValues >> InputNum);

                    // In case there is no change in the Actuator state - skip it
                    //if ((PrevActuatorValue == ActuatorValue) && (IsFirstActuatorsUpdate != true))
                    //  continue;

                    if (m_ActuatorsValues[InputNum] != OFF)
                    {
                        LEDsImageList->GetBitmap (GREEN_ON_LED,Image->Picture->Bitmap);

                        // 'Port Enable' is inverted logic. Turn on the Red Led.
                        if (PORT_ENABLE_ACTUATOR_ID == ActuatorsGroupBox->Controls[i]->Tag)
                            LEDsImageList->GetBitmap (RED_ON_LED,Image->Picture->Bitmap);
                    }
                    else
                    {
                        LEDsImageList->GetBitmap (RED_ON_LED,Image->Picture->Bitmap);

                        // 'Port Enable' is inverted logic. Turn on the Green Led.
                        if (PORT_ENABLE_ACTUATOR_ID == ActuatorsGroupBox->Controls[i]->Tag)
                            LEDsImageList->GetBitmap (GREEN_ON_LED,Image->Picture->Bitmap);
                    }
                    Image->Refresh();

                    break;
                }
            }
        }
    }
    IsFirstActuatorsUpdate = false;
    //m_PrevActuatorsValues = m_ActuatorsValues;
}
//---------------------------------------------------------------------------


// display an Inputs reading
MESSAGE void TOCBTesterForm::DisplayInputsReading(TMessage& Message)
{
    //int InputValue     = -1;
    //int PrevInputValue = -1;

    // In case there were no changes - do nothing
    /*if ((m_PrevInputsValues == m_InputsValues) && (IsFirstInputsUpdate != true))
      return;*/

    for (int InputNum = 0; InputNum < NUMBER_OF_INPUTS; InputNum++)
    {

        if((m_PrevInputsValues[InputNum] == m_InputsValues[InputNum]) && (IsFirstInputsUpdate != true))
            continue;
        m_PrevInputsValues[InputNum] = m_InputsValues[InputNum];
        // find the panel with a tag that is equal to the analog input number
        TImage* Image = NULL;

        for (int i = 0; i < SensorsGroupBox->ControlCount; i++)
        {
            if(SensorsGroupBox->Controls[i]->Tag == InputNum)
            {
                Image = dynamic_cast<TImage *>(SensorsGroupBox->Controls[i]);
                if (Image != NULL)
                {
                    /*InputValue     = 1 & (m_InputsValues >> InputNum);
                    PrevInputValue = 1 & (m_PrevInputsValues >> InputNum);

                    // In case there is no change in the Input state - skip it
                    if ((PrevInputValue == InputValue) && (IsFirstInputsUpdate != true))
                    continue;*/

                    if (m_InputsValues[InputNum] != SENSOR_OFF)
                    {
                        LEDsImageList->GetBitmap (GREEN_ON_LED,Image->Picture->Bitmap);
                    }
                    else
                    {
                        LEDsImageList->GetBitmap (RED_ON_LED,Image->Picture->Bitmap);
                    }
                    Image->Refresh();

                    break;
                }
            }
        }
    }
    IsFirstInputsUpdate = false;
    //m_PrevInputsValues = m_InputsValues;
}
//---------------------------------------------------------------------------
// display the Power Status
MESSAGE void TOCBTesterForm::DisplayPowerStatus(TMessage& Message)
{
    if (m_PowerStatus == ON)
    {
        LEDsImageList->GetBitmap (GREEN_ON_LED,PowerImage->Picture->Bitmap);
    }
    else
    {
        LEDsImageList->GetBitmap (RED_ON_LED,PowerImage->Picture->Bitmap);
    }
    PowerImage->Refresh();
}

//---------------------------------------------------------------------------
// Notify that the status of the liquid tanks is changed
MESSAGE void TOCBTesterForm::NotifyLiquidTanksStatusChanged(TMessage& Message)
{
//  ShowMessage ("The status of the liquid tanks is changed.");
}

//---------------------------------------------------------------------------
// Notify that there is resest wake up
MESSAGE void TOCBTesterForm::NotifyResetWakeUp(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("There is a reset wake up");
#endif
}

//---------------------------------------------------------------------------
// Notify that there is a filling error
MESSAGE void TOCBTesterForm::NotifyFillingError(TMessage& Message)
{
    switch (m_FillingErrorType)
    {
    case TOO_HIGH:
#if SHOW_MESSAGE_BOXES
        ShowMessage ("Filling error: Head temperature is out of range.");
#endif
        break;

    case TOO_LOW:
#if SHOW_MESSAGE_BOXES
        ShowMessage ("Filling error: Head temperature is out of range.");
#endif
        break;

    case TIME_OUT:
#if SHOW_MESSAGE_BOXES
        ShowMessage ("Filling error: Filling timeout");
#endif
        break;

    default:
        break;
    }
}

//---------------------------------------------------------------------------
// Notify that there is a purge error
MESSAGE void TOCBTesterForm::NotifyPurgeError(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("There is a Purge error");
#endif
}

//---------------------------------------------------------------------------
// Notify that the purge is ended
MESSAGE void TOCBTesterForm::NotifyPurgeEnd(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("The purge was ended");
#endif
}

//---------------------------------------------------------------------------
// Notify that there is an error with the UV lamps
MESSAGE void TOCBTesterForm::NotifyUVLampsError(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("There is some error with the UV Lamps");
#endif
}

//---------------------------------------------------------------------------
// Notify that the door is locked
MESSAGE void TOCBTesterForm::NotifyDoorLock(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("The Door is locked");
#endif
}

//---------------------------------------------------------------------------
// Notify that there is a Power error
MESSAGE void TOCBTesterForm::NotifyPowerError(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("There is a Power error");
#endif
}

//---------------------------------------------------------------------------
// Notify that the Power status has changed
MESSAGE void TOCBTesterForm::NotifyPowerIsOnOff(TMessage& Message)
{
#if SHOW_MESSAGE_BOXES
    ShowMessage ("The Power status is changed");
#endif
}

//---------------------------------------------------------------------------
// Notify that the UV Lamps are on
MESSAGE void TOCBTesterForm::NotifyLampsAreOnOff(TMessage& Message)
{
    if (m_IsUVLampsAreOn)
    {
//    ShowMessage ("UV Lamps are on");
    }
    else
    {
//    ShowMessage ("UV Lamps are off");
    }
}

//---------------------------------------------------------------------------
// Display which pumps are active
MESSAGE void TOCBTesterForm::DisplayHeadsFillingPumps(TMessage& Message)
{
    /*
      TRadioButton *MyButton;
      TComponent   *MyComp;

      for(int i = 0; i < SupportActivePumpsGroupBox->ControlCount; i++)
      {
        MyComp = SupportActivePumpsGroupBox->Controls[i];

        if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
        {
          if (MyButton->Tag == (int)m_SupportPump)
          {
            MyButton->Checked = true;
          }
        }
      }

      for(int i = 0; i < ModelActivePumpsGroupBox->ControlCount; i++)
      {
        MyComp = ModelActivePumpsGroupBox->Controls[i];

        if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
        {
          if (MyButton->Tag == (int)m_ModelPump)
          {
            MyButton->Checked = true;
          }
        }
      }
      */
}

MESSAGE void TOCBTesterForm::DisplayHeadsFilling(TMessage& Message)
{
    Model1ThermistorLevelPanel->Caption  = IntToStr(m_ChamberLiquidLevel[M1_CHAMBER_THERMISTOR]);
    Model2ThermistorLevelPanel->Caption  = IntToStr(m_ChamberLiquidLevel[M2_CHAMBER_THERMISTOR]);
    Model3ThermistorLevelPanel->Caption  = IntToStr(m_ChamberLiquidLevel[M3_CHAMBER_THERMISTOR]);
    SupportThermistorLevelPanel->Caption = IntToStr(m_ChamberLiquidLevel[SUPPORT_CHAMBER_THERMISTOR]);
    ModelThermistorLevelPanel->Caption  = IntToStr(m_ChamberLiquidLevel[M1_M2_CHAMBER_THERMISTOR]);
	SThermistorLevelPanel->Caption = IntToStr(m_ChamberLiquidLevel[SUPPORT_MODEL_CHAMBER_THERMISTOR]);

    Model1ThermistorLevelPanel->Color    = ((m_ChamberCurrentPumpState[M1_CHAMBER_THERMISTOR]) ? clLime : clRed);
    Model2ThermistorLevelPanel->Color    = ((m_ChamberCurrentPumpState[M2_CHAMBER_THERMISTOR]) ? clLime : clRed);
    Model3ThermistorLevelPanel->Color    = ((m_ChamberCurrentPumpState[M3_CHAMBER_THERMISTOR]) ? clLime : clRed);
    SupportThermistorLevelPanel->Color   = ((m_ChamberCurrentPumpState[SUPPORT_CHAMBER_THERMISTOR]) ? clLime : clRed);
    ModelThermistorLevelPanel->Color    = ((m_ChamberCurrentPumpState[M1_M2_CHAMBER_THERMISTOR]) ? clLime : clRed);
    SThermistorLevelPanel->Color   =      ((m_ChamberCurrentPumpState[SUPPORT_MODEL_CHAMBER_THERMISTOR]) ? clLime : clRed);
}

//---------------------------------------------------------------------------
// Display the Liquid Tanks Weights
MESSAGE void TOCBTesterForm::DisplayLiquidTanksWeights(TMessage& Message)
{
	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
		m_TankWeights[i]->Text = FloatToStrF(m_TankWeight[i], ffFixed, 4, 2);
}

//---------------------------------------------------------------------------
// Display the Liquid Tanks Status
MESSAGE void TOCBTesterForm::DisplayLiquidTanksStatus(TMessage& Message)
{
	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
	{
    	if(LIQUID_TANK_IS_EMPTY != m_TankStatus[i])
		{
			m_TankInplaces[i]->Text = "In";
			m_TankInplaces[i]->Color = clLime;
		}
		else
		{
			m_TankInplaces[i]->Text = "Out";
			m_TankInplaces[i]->Color = clRed;
		}
    }
}

//---------------------------------------------------------------------------
// Display the Door Status
MESSAGE void TOCBTesterForm::DisplayDoorStatus(TMessage& Message)
{
    /*
    static const char *DoorArray[] ={"Door", "Side door", "Front right door", "Front left door", "Service door"};
    	int i = 0, j=0;

    	 if( CAppParams::Instance()->NumOfDoors > 1)
    	 {
    		++j;
    	 }
    	for( i; i<MAX_NUM_OF_DOORS; ++i, ++j)
    	{


    	   CQLog::Write(LOG_TAG_GENERAL,"%s Status: %s  %s",
    				  DoorArray[j],
    				  IsDoorClosed(i) ? "Closed" : "Not Closed",
    				  IsDoorLocked() ? "Locked" : "Not Locked");
    	}


    */

    const char *DoorArray[] = {"Door", "Side door", "Front right door", "Front left door", "Service door"};

    const char *DoorStatus[] = {" Outside and Unlocked", " Inside and Unlocked", "  Outside and Locked", " Inside and Locked"};
    int i = 0, j=0;

    if( MAX_NUM_OF_DOORS!= 1)
    {
        ++j;
    }

    for( i = 0 ; i < MAX_NUM_OF_DOORS; i++, j++)
    {
        switch (m_DoorStatus[i])
        {
        case DOOR_OUTSIDE_UNLOCKED:
            DoorStatusEdit->Text = DoorArray[j];
            DoorStatusEdit->Text+= DoorArray[i];
            break;

        case DOOR_INSIDE_UNLOCKED:
            DoorStatusEdit->Text = DoorArray[j];
            DoorStatusEdit->Text+= DoorArray[i];
            break;

        case DOOR_OUTSIDE_LOCKED:
            DoorStatusEdit->Text = DoorArray[j];
            DoorStatusEdit->Text+= DoorArray[i];
            break;

        case DOOR_INSIDE_LOCKED:
            DoorStatusEdit->Text = DoorArray[j];
            DoorStatusEdit->Text+= DoorArray[i];
            break;
        }
    }
}

//---------------------------------------------------------------------------
// Display the SW Version
MESSAGE void TOCBTesterForm::DisplaySWVersion(TMessage& Message)
{
	InternalVersionEdit->Text = QFormatStr("%02d", m_InternalVersion).c_str();
	ExternalVersionEdit->Text = QFormatStr("%d", m_ExternalVersion).c_str();

	MSC1HWEdit->Text = m_MSCHardwareVersion[MSC_CARD_1];
	MSC1FWEdit->Text = m_MSCFirmwareVersion[MSC_CARD_1];

	MSC2HWEdit->Text = m_MSCHardwareVersion[MSC_CARD_2];
	MSC2FWEdit->Text = m_MSCFirmwareVersion[MSC_CARD_2];

	MSC3HWEdit->Text = m_MSCHardwareVersion[MSC_CARD_3];
	MSC3FWEdit->Text = m_MSCFirmwareVersion[MSC_CARD_3];

	MSC4HWEdit->Text = m_MSCHardwareVersion[MSC_CARD_4];
	MSC4FWEdit->Text = m_MSCFirmwareVersion[MSC_CARD_4];
}

//---------------------------------------------------------------------------
// Display the Purge Status
MESSAGE void TOCBTesterForm::DisplayPurgeStatus(TMessage& Message)
{
    // Updating the Led
    if (ON == m_PurgeStatus)
    {
        LEDsImageList->GetBitmap (GREEN_ON_LED,PurgeStatusImage->Picture->Bitmap);
    }
    else
    {
        LEDsImageList->GetBitmap (RED_ON_LED,PurgeStatusImage->Picture->Bitmap);
    }
    PurgeStatusImage->Refresh();
}

//---------------------------------------------------------------------------
// Display the Heads Filling Status
MESSAGE void TOCBTesterForm::DisplayFillingStatus(TMessage& Message)
{
    // Updating the Led
    if (ON == m_FillingOnOff)
    {
        LEDsImageList->GetBitmap (GREEN_ON_LED,FillingImage->Picture->Bitmap);
    }
    else
    {
        LEDsImageList->GetBitmap (RED_ON_LED,FillingImage->Picture->Bitmap);
    }
    FillingImage->Refresh();

    SupportThermistorLevelPanel->Caption = IntToStr (m_SupportThermistorLevel);
    Model1ThermistorLevelPanel->Caption   = IntToStr (m_Model1ThermistorLevel);
    Model2ThermistorLevelPanel->Caption   = IntToStr (m_Model2ThermistorLevel);
    Model3ThermistorLevelPanel->Caption   = IntToStr (m_Model3ThermistorLevel);

    ModelThermistorLevelPanel->Caption   = IntToStr (m_ModelThermistorLevel);
    SThermistorLevelPanel->Caption   = IntToStr (m_SThermistorLevel);
}

//---------------------------------------------------------------------------
// Display the UV Lamps Status
MESSAGE void TOCBTesterForm::DisplayUVLampsStatus(TMessage& Message)
{
    if (ON == m_RightUVLampValue)
    {
        LEDsImageList->GetBitmap (GREEN_ON_LED,RightUVLampImage->Picture->Bitmap);
    }
    else
    {
        LEDsImageList->GetBitmap (RED_ON_LED,RightUVLampImage->Picture->Bitmap);
    }

    if (ON == m_LeftUVLampValue)
    {
        LEDsImageList->GetBitmap (GREEN_ON_LED,LeftUVLampImage->Picture->Bitmap);
    }
    else
    {
        LEDsImageList->GetBitmap (RED_ON_LED,LeftUVLampImage->Picture->Bitmap);
    }
    LeftUVLampImage->Refresh();
    RightUVLampImage->Refresh();
}

//---------------------------------------------------------------------------
// Display that the tray temperature is in range
MESSAGE void TOCBTesterForm::DisplayTrayTempInRange(TMessage& Message)
{
    TemperatureOKPanel->Caption = "Yes";
}

//---------------------------------------------------------------------------
// display the Current Tray Temperature
MESSAGE void TOCBTesterForm::DisplayIsTrayInserted(TMessage& Message)
{
    if (TRAY_INSERTED == m_IsTrayIn)
    {
        LEDsImageList->GetBitmap (GREEN_ON_LED,TrayInsertedImage->Picture->Bitmap);
    }
    else
    {
        LEDsImageList->GetBitmap (RED_ON_LED,TrayInsertedImage->Picture->Bitmap);
    }
    TrayInsertedImage->Refresh();
}

//---------------------------------------------------------------------------
// display the Current Tray Temperature
MESSAGE void TOCBTesterForm::DisplayCurrentTrayTemp(TMessage& Message)
{
    CurrentTemperaturePanel->Caption = IntToStr (m_CurrentTrayTemp);
}


//---------------------------------------------------------------------------
// display an A2D reading
MESSAGE void TOCBTesterForm::DisplayA2DReading(TMessage& Message)
{
    for (int InputNum = 1; InputNum <= OCB_NUM_OF_ANALOG_INPUTS; InputNum++)
    {
        // find the panel with a tag that is equal to the analog input number
        TPanel* Panel = NULL;

        if (InputNum <= NUMBER_OF_ANALOGS_IN_CPU)
        {
            for (int i = 0; i < A2D_CPU_GroupBox->ControlCount; i++)
            {
                if(A2D_CPU_GroupBox->Controls[i]->Tag == InputNum)
                {
                    Panel = dynamic_cast<TPanel *>(A2D_CPU_GroupBox->Controls[i]);
                    if (Panel != NULL)
                    {
                        Panel->Caption = m_A2DValues[InputNum-1];
                        break;
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < A2DGroupBox->ControlCount; i++)
            {
                if(A2DGroupBox->Controls[i]->Tag == InputNum)
                {
                    Panel = dynamic_cast<TPanel *>(A2DGroupBox->Controls[i]);
                    if (Panel != NULL)
                    {
                        Panel->Caption = m_A2DValues[InputNum-1];
                        break;
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::ActuatorOffClick(TObject *Sender)
{
    TButton *Actuator = dynamic_cast<TButton *>(Sender);
    if (Actuator == NULL)
        return;

    try
    {
        // build the message
        TOCBSetActuatorOnOffMessage Message;
        Message.MessageID  = OCB_SET_ACTUATOR_ON_OFF;
        Message.ActuatorID = Actuator->Tag;
        Message.OnOff      = OFF;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetActuatorOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_ACTUATOR_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);
    }
    catch(...)
    {
        QMonitor.ErrorMessage("Invalid arguments");
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::ActuatorOnClick(TObject *Sender)
{
    TButton *Actuator = dynamic_cast<TButton *>(Sender);
    if (Actuator == NULL)
        return;

    try
    {
        // build the message
        TOCBSetActuatorOnOffMessage Message;
        Message.MessageID  = OCB_SET_ACTUATOR_ON_OFF;
        Message.ActuatorID = Actuator->Tag;
        Message.OnOff      = ON;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetActuatorOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_ACTUATOR_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);
    }
    catch(...)
    {
        QMonitor.ErrorMessage("Invalid arguments");
    }
}
//---------------------------------------------------------------------------



void __fastcall TOCBTesterForm::SetTrayParametersButtonClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsTrayParametersInUse == false)
    {
        m_IsTrayParametersInUse = true;

        try
        {
            // build the message
            TOCBSetTrayTemperatureMessage Message;
            Message.MessageID = OCB_SET_TRAY_TEMPERATURE;
            Message.RequiredTrayTemperature = StrToInt(SetTrayTempEdit->Text);
            Message.ActiveMargine = StrToInt(SetTrayMarginEdit->Text);
            Message.TraySetTimeout = StrToInt(SetTrayTimeoutEdit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetTrayTemperatureMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SET_TRAY_TEMPERATURE;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments.");
        }

        TemperatureOKPanel->Caption = "No";
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::HeatingOnClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsHeatingOnInUse == false)
    {
        m_IsHeatingOnInUse = true;

        // build the message
        TOCBSetTrayOnOffMessage Message;
        Message.MessageID        = OCB_SET_TRAY_ON_OFF;
        Message.TrayHeatingOnOff = ON;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetTrayOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_TRAY_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        // Changing the button....
        HeatingBottun->Action  = HeatingOffAction;
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::HeatingOffClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsHeatingOnInUse == false)
    {
        m_IsHeatingOnInUse = true;

        // build the message
        TOCBSetTrayOnOffMessage Message;
        Message.MessageID        = OCB_SET_TRAY_ON_OFF;
        Message.TrayHeatingOnOff = OFF;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetTrayOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_TRAY_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        // Changing the button....
        HeatingBottun->Action  = HeatingOnAction;
    }
}
//---------------------------------------------------------------------------

void TOCBTesterForm::SendAck (int RespondedMessageID)
{
    // build the message
    TOCBAck Message;
    Message.MessageID          = OCB_EDEN_ACK;
    Message.RespondedMessageID = RespondedMessageID;
    Message.AckStatus          = SUCCESS;

    // send the message
    m_ProtocolClient->Send(&Message, sizeof(TOCBAck));
}

void __fastcall TOCBTesterForm::UVLampsOnExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsUVLampsInUse == false)
    {
        m_IsUVLampsInUse = true;

        // build the message
        TOCBUvLampOnOffMessage Message;
        Message.MessageID = OCB_SET_UV_LAMPS_ON_OFF;
        Message.OnOff     = ON;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBUvLampOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_UV_LAMPS_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        // Changing the button....
        UVLampsButton->Action = UVLampsOff;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::UVLampsOffExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsUVLampsInUse == false)
    {
        m_IsUVLampsInUse = true;

        // build the message
        TOCBUvLampOnOffMessage Message;
        Message.MessageID = OCB_SET_UV_LAMPS_ON_OFF;
        Message.OnOff     = OFF;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBUvLampOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_UV_LAMPS_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        // Changing the button....
        UVLampsButton->Action = UVLampsOn;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::SetUVLampsParameterButtonClick(TObject *Sender)
{
    int UVLampsMask;

    // Don't send the message if the same message was sent without
    if (m_IsUVLampsParameterInUse == false)
    {
        m_IsUVLampsParameterInUse = true;

        TRadioButton *MyButton;
        TComponent   *MyComp;

        // Retrieving the Active UV lamps MAsk
        for(int i = 0; i < ActiveLampsMaskGroupBox->ControlCount; i++)
        {
            MyComp = ActiveLampsMaskGroupBox->Controls[i];

            if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
            {
                if (MyButton->Checked)
                {
                    UVLampsMask = MyButton->Tag;
                }
            }
        }
        try
        {
            // build the message
            TOCBSetUvParamsMessage Message;
            Message.MessageID           = OCB_SET_UV_LAMPS_PARAMS;
            Message.IgnitionTimeout     = StrToInt(IgnitionTimeoutEdit->Text);
            Message.PostIgnitionTimeout = StrToInt(PostIgnitionTimeoutEdit->Text);
            Message.SensorBypass        = StrToInt((SensorBypassCheckBox->Checked ? 1 : 0));
            Message.ActiveLampsMask     = UVLampsMask;
            Message.MotorsImmobilityTimeoutSec = 1800;

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetUvParamsMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SET_UV_LAMPS_PARAMS;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::HeadsFillingOnExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsFillingInUse == false)
    {
        m_IsFillingInUse = true;

        // build the message
        TOCBHeadFillingControlOnOffMessage Message;
        Message.MessageID               = OCB_HEADS_FILLING_CONTROL_ON_OFF;
        Message.HeadFillingControlOnOff = ON;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBHeadFillingControlOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_HEADS_FILLING_CONTROL_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        HeadsFillingButton->Action = HeadsFillingOff;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::HeadsFillingOffExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsFillingInUse == false)
    {
        m_IsFillingInUse = true;

        // build the message
        TOCBHeadFillingControlOnOffMessage Message;
        Message.MessageID               = OCB_HEADS_FILLING_CONTROL_ON_OFF;
        Message.HeadFillingControlOnOff = OFF;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBHeadFillingControlOnOffMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_HEADS_FILLING_CONTROL_ON_OFF;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        HeadsFillingButton->Action = HeadsFillingOn;
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::SetHeadsParametersButtonClick(TObject *Sender)
{
    BYTE   MessageID;
    USHORT TimeTankOn;
    USHORT TimeTankOff;
    USHORT FillingTimeout;
    /*iMPORTANT: The Order is Support, Model, [Model2]*/
    USHORT ResinLowThereshold[NUM_OF_CHAMBERS_THERMISTORS];
    USHORT ResinHighThershold[NUM_OF_CHAMBERS_THERMISTORS];

    // Don't send the message if the same message was sent without
    if (m_IsHeadParamsInUse == false)
    {
        m_IsHeadParamsInUse = true;

        try
        {
            // build the message
            TOCBSetHeadFillingParamsMessage Message;
            Message.MessageID            = OCB_SET_HEADS_FILLING_PARAMS;
            Message.ResinLowThereshold[M1_CHAMBER_THERMISTOR]   = StrToInt(Model1LevelThermistorLowEdit->Text);
            Message.ResinLowThereshold[M2_CHAMBER_THERMISTOR]   = StrToInt(Model2LevelThermistorLowEdit->Text);
            Message.ResinLowThereshold[M3_CHAMBER_THERMISTOR]   = StrToInt(Model3LevelThermistorLowEdit->Text);
            Message.ResinLowThereshold[SUPPORT_CHAMBER_THERMISTOR]  = StrToInt(SupportLevelThermistorLowEdit->Text);
            Message.ResinLowThereshold[M1_M2_CHAMBER_THERMISTOR]   = StrToInt(ModelLevelThermistorLowEdit->Text);
			Message.ResinLowThereshold[SUPPORT_MODEL_CHAMBER_THERMISTOR]  = StrToInt(SLevelThermistorLowEdit->Text);
			Message.ResinHighThershold[M1_CHAMBER_THERMISTOR]   = StrToInt(Model1LevelThermistorHighEdit->Text);
			Message.ResinHighThershold[M2_CHAMBER_THERMISTOR]   = StrToInt(Model2LevelThermistorHighEdit->Text);
			Message.ResinHighThershold[M3_CHAMBER_THERMISTOR]   = StrToInt(Model3LevelThermistorHighEdit->Text);

			Message.ResinHighThershold[SUPPORT_CHAMBER_THERMISTOR]  = StrToInt(SupportLevelThermistorHighEdit->Text);
			Message.ResinHighThershold[M1_M2_CHAMBER_THERMISTOR]   = StrToInt(ModelLevelThermistorHighEdit->Text);
			Message.ResinHighThershold[SUPPORT_MODEL_CHAMBER_THERMISTOR]  = StrToInt(SLevelThermistorHighEdit->Text);


			Message.ActiveThermistors[SUPPORT_CHAMBER_THERMISTOR] = SLowThCheckBox->Checked;
			Message.ActiveThermistors[M1_CHAMBER_THERMISTOR] = M1LowThCheckBox->Checked;
			Message.ActiveThermistors[M2_CHAMBER_THERMISTOR] = M2LowThCheckBox->Checked;
			Message.ActiveThermistors[M3_CHAMBER_THERMISTOR] = M3LowThCheckBox->Checked;
			Message.ActiveThermistors[M1_M2_CHAMBER_THERMISTOR] = MHighThCheckBox->Checked;
            Message.ActiveThermistors[SUPPORT_MODEL_CHAMBER_THERMISTOR] = SHighThCheckBox->Checked;

            Message.TimePumpOn           = StrToInt(PumpOnEdit->Text);
            Message.TimePumpOff          = StrToInt(PumpOffEdit->Text);
            Message.FillingTimeout       = StrToInt(TimeoutFillingEdit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetHeadFillingParamsMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SET_HEADS_FILLING_PARAMS;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);


            // Updating the Param Menager
            CAppParams *ParamsMgr = CAppParams::GetInstance();

            ParamsMgr->Model1LevelThermistorLow    = StrToInt(Model1LevelThermistorLowEdit->Text);
            ParamsMgr->Model2LevelThermistorLow    = StrToInt(Model2LevelThermistorLowEdit->Text);
            ParamsMgr->Model3LevelThermistorLow    = StrToInt(Model3LevelThermistorLowEdit->Text);
            ParamsMgr->Model1LevelThermistorHigh   = StrToInt(Model1LevelThermistorHighEdit->Text);
            ParamsMgr->Model2LevelThermistorHigh   = StrToInt(Model2LevelThermistorHighEdit->Text);
            ParamsMgr->Model3LevelThermistorHigh   = StrToInt(Model3LevelThermistorHighEdit->Text);
            ParamsMgr->SupportLevelThermistorLow  = StrToInt(SupportLevelThermistorLowEdit->Text);
            ParamsMgr->SupportLevelThermistorHigh = StrToInt(SupportLevelThermistorHighEdit->Text);

            ParamsMgr->ModelLevelThermistorLow    = StrToInt(ModelLevelThermistorLowEdit->Text);
            ParamsMgr->ModelLevelThermistorHigh   = StrToInt(ModelLevelThermistorHighEdit->Text);
            ParamsMgr->SLevelThermistorLow  = StrToInt(SLevelThermistorLowEdit->Text);
            ParamsMgr->SLevelThermistorHigh = StrToInt(SLevelThermistorHighEdit->Text);

            ParamsMgr->ActiveThermistors[SUPPORT_CHAMBER_THERMISTOR] = SLowThCheckBox->Checked;
            ParamsMgr->ActiveThermistors[M1_CHAMBER_THERMISTOR] = M1LowThCheckBox->Checked;
            ParamsMgr->ActiveThermistors[M2_CHAMBER_THERMISTOR] = M2LowThCheckBox->Checked;
            ParamsMgr->ActiveThermistors[M3_CHAMBER_THERMISTOR] = M3LowThCheckBox->Checked;
            ParamsMgr->ActiveThermistors[M1_M2_CHAMBER_THERMISTOR] = MHighThCheckBox->Checked;
            ParamsMgr->ActiveThermistors[SUPPORT_MODEL_CHAMBER_THERMISTOR] = SHighThCheckBox->Checked;

            ParamsMgr->TimePumpON                  = StrToInt(PumpOnEdit->Text);
            ParamsMgr->TimePumpOFF                 = StrToInt(PumpOffEdit->Text);
            ParamsMgr->TimeoutFilling              = StrToInt(TimeoutFillingEdit->Text);

            // Saving parameters
            ParamsMgr->SaveAll();
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::PerformPurgeButtonClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsPerformPurgeInUse == false)
    {
        m_IsPerformPurgeInUse = true;

        try
        {
            // build the message
            TOCBPerformPurgeMessage Message;
            Message.MessageID = OCB_PERFORM_PURGE;
            Message.PurgeTime = StrToInt(PerformPurge_TimeEdit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBPerformPurgeMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_PERFORM_PURGE;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);


            // Updating the Param Menager
            CAppParams *ParamsMgr = CAppParams::GetInstance();

            ParamsMgr->PurgeTime          = StrToInt(PerformPurge_TimeEdit->Text);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::SWResetButtonClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsSWResetInUse == false)
    {
        try
        {
            m_IsSWResetInUse = true;

            // build the message
            TOCBGenericMessage Message;
            Message.MessageID  = OCB_SW_RESET;

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBGenericMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SW_RESET;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::GetOCBSoftwareVersionClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
   // if (m_IsSWVersionInUse == false)
	//{
		m_IsSWVersionInUse = true;

		try
		{
			// build the message
			TOCBGetDriverSWVersionMessage Message;
			Message.MessageID = OCB_GET_OCB_SW_VERSION;

			// send the message
			int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBGetDriverSWVersionMessage));

			// install message handler
			m_ProtocolClient->InstallReceiveHandler(TransactionHandle, SWVersionHandler, reinterpret_cast<TGenericCockie>(this));

			// mark the message as waiting for ack/status (insert it to timeout list)
			TSendTimeoutType Timeout;
			Timeout.MsgID = OCB_SW_VERSION;
			Timeout.TransactionID = TransactionHandle;
			Timeout.Timeout = QTicksToMs(QGetTicks() + OCB_REPLY_TIME_IN_MS);
			m_TimeoutList.push_back(Timeout);
		}
		catch(...)
		{
			QMonitor.ErrorMessage("Invalid arguments");
		}
   //	}
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::LockDoorExecute(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsLockDoorInUse == false)
    {
        m_IsLockDoorInUse = true;

        // build the message
        TOCBLockDoorMessage Message;
        Message.MessageID = OCB_LOCK_DOOR;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBLockDoorMessage));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_LOCK_DOOR;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);

        DoorButton->Action = UnLockDoorAction;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::UnLockDoorExecute(TObject *Sender)
{
    // build the message
    TOCBUnlockDoorMessage Message;
    Message.MessageID = OCB_UNLOCK_DOOR;

    // send the message
    int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBUnlockDoorMessage));

    // install message handler
    m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

    // mark the message as waiting for ack/status (insert it to timeout list)
    TSendTimeoutType Timeout;
    Timeout.MsgID = OCB_UNLOCK_DOOR;
    Timeout.TransactionID = TransactionHandle;
    Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
    m_TimeoutList.push_back(Timeout);

    DoorButton->Action = LockDoorAction;
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::Model1PumpSelectExecte(TObject *Sender)
{

    TRadioButton *MyButton;
    TComponent   *MyComp;
    TRadioButton *Button;
    TComponent   *Comp;

    for(int i = 0; i < Model1ActivePumpsGroupBox->ControlCount; i++)
    {
        MyComp = Model1ActivePumpsGroupBox->Controls[i];

        if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
        {
            if (MyButton->Checked == true)
            {
                m_Model1_Container = MyButton->Tag;
                /*
                		// Disabling the parallel radio button
                		for(int j = 0; j < SupportActivePumpsGroupBox->ControlCount; j++)
                		{
                		  Comp = SupportActivePumpsGroupBox->Controls[j];

                		  if((Button = dynamic_cast<TRadioButton *>(Comp)) != 0)
                          {
                            // Disable the right button - enable all others
                			if (Button->Tag == MyButton->Tag)
                            {
                              Button->Enabled = false;
                            } else {
                			  Button->Enabled = true;
                            }
                          }
                        }
                */
            }
        }
    }

}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::Model2PumpSelectExecte(TObject *Sender)
{

    TRadioButton *MyButton;
    TComponent   *MyComp;
    TRadioButton *Button;
    TComponent   *Comp;

    for(int i = 0; i < Model2ActivePumpsGroupBox->ControlCount; i++)
    {
        MyComp = Model2ActivePumpsGroupBox->Controls[i];

        if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
        {
            if (MyButton->Checked == true)
            {
                m_Model2_Container = MyButton->Tag;
                /*
                        // Disabling the parallel radio button
                        for(int j = 0; j < SupportActivePumpsGroupBox->ControlCount; j++)
                        {
                          Comp = SupportActivePumpsGroupBox->Controls[j];

                          if((Button = dynamic_cast<TRadioButton *>(Comp)) != 0)
                          {
                            // Disable the right button - enable all others
                            if (Button->Tag == MyButton->Tag)
                            {
                              Button->Enabled = false;
                            } else {
                              Button->Enabled = true;
                            }
                          }
                        }
                */
            }
        }
    }
}

void __fastcall TOCBTesterForm::Model3PumpSelectExecte(TObject *Sender)
{

    TRadioButton *MyButton;
    TComponent   *MyComp;
    TRadioButton *Button;
    TComponent   *Comp;

    for(int i = 0; i < Model3ActivePumpsGroupBox->ControlCount; i++)
    {
        MyComp = Model3ActivePumpsGroupBox->Controls[i];

        if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
        {
            if (MyButton->Checked == true)
            {
                m_Model3_Container = MyButton->Tag;
                /*
                		// Disabling the parallel radio button
                		for(int j = 0; j < SupportActivePumpsGroupBox->ControlCount; j++)
                		{
                		  Comp = SupportActivePumpsGroupBox->Controls[j];

                		  if((Button = dynamic_cast<TRadioButton *>(Comp)) != 0)
                          {
                            // Disable the right button - enable all others
                			if (Button->Tag == MyButton->Tag)
                            {
                              Button->Enabled = false;
                            } else {
                			  Button->Enabled = true;
                            }
                          }
                        }
                */
            }
        }
    }

}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::SupportPumpSelectExecute(TObject *Sender)
{

    TRadioButton *MyButton;
    TComponent   *MyComp;
    TRadioButton *Button;
    TComponent   *Comp;

    for(int i = 0; i < SupportActivePumpsGroupBox->ControlCount; i++)
    {
        MyComp = SupportActivePumpsGroupBox->Controls[i];

        if((MyButton = dynamic_cast<TRadioButton *>(MyComp)) != 0)
        {
            if (MyButton->Checked == true)
            {
                m_SupportContainer = MyButton->Tag;

            }
        }
    }

}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::SetActivePumpsButtonClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsActivePumpsInUse == false)
    {
        m_IsActivePumpsInUse = true;

        // build the message
        TOCBSetChamberTankMsg Message;
        Message.MessageID     = OCB_SET_CHAMBERS_TANK;

        Message.TankID[TYPE_CHAMBER_MODEL1]  = m_Model1_Container;
        Message.TankID[TYPE_CHAMBER_MODEL2]  = m_Model2_Container;
        Message.TankID[TYPE_CHAMBER_MODEL3]  = m_Model3_Container;
        Message.TankID[TYPE_CHAMBER_SUPPORT] = m_SupportContainer;

        // send the message
        int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetChamberTankMsg));

        // install message handler
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // mark the message as waiting for ack/status (insert it to timeout list)
        TSendTimeoutType Timeout;
        Timeout.MsgID = OCB_SET_CHAMBERS_TANK;
        Timeout.TransactionID = TransactionHandle;
        Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
        m_TimeoutList.push_back(Timeout);
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::HeadsTabExecute(TObject *Sender)
{
    /* Shahar

      // build the message
      TOCBGetHeadsFillingActivePumpsMsg Message;
      Message.MessageID = OCB_GET_HEAD_FILLING_ACTIVE_PUMPS;

      // send the message
      int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBGetHeadsFillingActivePumpsMsg));

      // install message handler
      m_ProtocolClient->InstallReceiveHandler(TransactionHandle,HeadsFillingActivePumpsHandler,reinterpret_cast<TGenericCockie>(this));

      // mark the message as waiting for ack/status (insert it to timeout list)
      TSendTimeoutType Timeout;
      Timeout.MsgID = OCB_HEADS_FILLING_ACTIVE_PUMPS;
      Timeout.TransactionID = TransactionHandle;
      Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
      m_TimeoutList.push_back(Timeout);

    */
}
//---------------------------------------------------------------------------


void TOCBTesterForm::UpdateTimeoutFlag (int MsgID)
{
    switch (MsgID)
    {
    case OCB_SW_RESET:
        m_IsSWResetInUse = false;
        break;

        // Intentional fall through
    case OCB_SET_POWER_PARMS:
    case OCB_SET_POWER_ON_OFF:
        m_IsPowerOnInUse = false;
        break;

    case OCB_SET_TRAY_ON_OFF:
        m_IsHeatingOnInUse = false;
        break;

    case OCB_SET_TRAY_TEMPERATURE:
        m_IsTrayParametersInUse = false;
        break;

    case OCB_SET_UV_LAMPS_ON_OFF:
        m_IsUVLampsInUse = false;
        break;

    case OCB_SET_UV_LAMPS_PARAMS:
        m_IsUVLampsParameterInUse = false;
        break;

    case OCB_PERFORM_PURGE:
        m_IsPerformPurgeInUse = false;
        break;

    case OCB_GET_OCB_SW_VERSION:
        m_IsSWVersionInUse = false;
        break;

    case OCB_LOCK_DOOR:
        m_IsLockDoorInUse = false;
        break;

    case OCB_SET_CHAMBERS_TANK:
        m_IsActivePumpsInUse = false;
        break;

    case OCB_SET_HEADS_FILLING_PARAMS:
        m_IsHeadParamsInUse = false;
        break;

    case OCB_HEADS_FILLING_CONTROL_ON_OFF:
        m_IsFillingInUse = false;
        break;

    case OCB_SET_ODOUR_FAN_ON_OFF:
        m_IsOdourInUse = false;
        break;

    case OCB_SET_D2A_VALUE:
        m_IsA2DDeviceInUse = false;
        break;
    }
}

void __fastcall TOCBTesterForm::OdourOffExecute(TObject *Sender)
{
    // Sending 'Off' message to the OCB
    // Don't send the message if the same message was sent without
    if (m_IsOdourInUse == false)
    {
        try
        {
            m_IsOdourInUse = true;

            // build the message
            TOCBSetOdourFanOnOffMessage Message;
            Message.MessageID  = OCB_SET_ODOUR_FAN_ON_OFF;
            Message.OnOff      = OFF;
            Message.FanVoltage = StrToInt(OdourValueEdit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetOdourFanOnOffMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SET_ODOUR_FAN_ON_OFF;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }

        // Changing the button....
        OdourOnOffButton->Action = OdourOn;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::OdourOnExecute(TObject *Sender)
{
    // Sending 'On' message to the OCB
    // Don't send the message if the same message was sent without
    if (m_IsOdourInUse == false)
    {
        try
        {
            m_IsOdourInUse = true;

            // build the message
            TOCBSetOdourFanOnOffMessage Message;
            Message.MessageID  = OCB_SET_ODOUR_FAN_ON_OFF;
            Message.OnOff      = ON;
            Message.FanVoltage = StrToInt(OdourValueEdit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetOdourFanOnOffMessage));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID = OCB_SET_ODOUR_FAN_ON_OFF;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;
            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }

        // Changing the button....
        OdourOnOffButton->Action = OdourOff;
    }
}
//---------------------------------------------------------------------------


void __fastcall TOCBTesterForm::SetD2AValueDevice1ButtonClick(
    TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsA2DDeviceInUse == false)
    {
        try
        {
            m_IsA2DDeviceInUse = true;

            // build the message
            TOCBSetD2AValue Message;
            Message.MessageID = OCB_SET_D2A_VALUE;
            Message.DeviceID  = 0;
            Message.Value     = StrToInt(SetD2AValueDevice1Edit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetD2AValue));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID         = OCB_SET_D2A_VALUE;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout       = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;

            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TOCBTesterForm::SetD2AValueDevice2ButtonClick(TObject *Sender)
{
    // Don't send the message if the same message was sent without
    if (m_IsA2DDeviceInUse == false)
    {
        try
        {
            m_IsA2DDeviceInUse = true;

            // build the message
            TOCBSetD2AValue Message;
            Message.MessageID = OCB_SET_D2A_VALUE;
            Message.DeviceID  = 1;
            Message.Value     = StrToInt(SetD2AValueDevice2Edit->Text);

            // send the message
            int TransactionHandle = m_ProtocolClient->Send(&Message, sizeof(TOCBSetD2AValue));

            // install message handler
            m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // mark the message as waiting for ack/status (insert it to timeout list)
            TSendTimeoutType Timeout;
            Timeout.MsgID         = OCB_SET_D2A_VALUE;
            Timeout.TransactionID = TransactionHandle;
            Timeout.Timeout       = QTicksToMs(QGetTicks()) + OCB_REPLY_TIME_IN_MS;

            m_TimeoutList.push_back(Timeout);
        }
        catch(...)
        {
            QMonitor.ErrorMessage("Invalid arguments");
        }
    }
}
//---------------------------------------------------------------------------

void TOCBTesterForm::HeadsFillingHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
    TOCBHeadsFillingNotification *Msg = static_cast<TOCBHeadsFillingNotification *>(Data);
    TOCBTesterForm               *InstancePtr = reinterpret_cast<TOCBTesterForm *>(Cockie);

    //Verify size of message
    if(DataLength != sizeof(TOCBHeadsFillingNotification))
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"HeadsFillingNotification\" length error");
        return;
    }

    if (static_cast<int>(Msg->MessageID) != OCB_HEADS_FILLING_NOTIFICATION)
    {
        CQLog::Write(LOG_TAG_HEAD_FILLING,"Head Filling \"HeadsFillingNotification\" message id error (0x%X)",Msg->MessageID);
        return;
    }

    InstancePtr->m_ChamberCurrentPumpState[Msg->ChamberId] = Msg->PumpStatus;
    InstancePtr->m_ChamberLiquidLevel[Msg->ChamberId]      = Msg->LiquidLevel;

    // Send ACK
    InstancePtr->SendNotificationAck (OCB_HEADS_FILLING_NOTIFICATION);

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OCB_HEADS_FILLING_NOTIFICATION,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
}

void TOCBTesterForm::SendNotificationAck (int RespondedMessageID)
{
    // build the message
    TOCBAck Message;
    Message.MessageID          = OCB_EDEN_ACK;
    Message.RespondedMessageID = RespondedMessageID;
    Message.AckStatus          = SUCCESS;

    // send the message
    m_ProtocolClient->SendNotificationAck(RespondedMessageID, &Message, sizeof(TOCBAck));
}
//---------------------------------------------------------------------------


