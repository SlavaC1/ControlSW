//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHDBTesterDlg.h"
#include "QMonitor.h"
#include "OHDBCommDefs.h"
#include "TesterApplication.h"
#include "EdenPCISys.h"
#include "FIFOPCI.h"
#include "ArrayParamsEditorDlg.h"
#include "GenFourTesterDefs.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TOHDBTesterForm *OHDBTesterForm;


const int XILINX_ID                   = 0;
const int POTENIOMETER_ID             = 1;
const int E2PROM1_ID                  = 2;
const int E2PROM2_ID                  = 3;
const int E2PROM3_ID                  = 4;
const int E2PROM4_ID                  = 5;
const int E2PROM5_ID                  = 6;
const int E2PROM6_ID                  = 7;
const int E2PROM7_ID                  = 8;
const int E2PROM8_ID                  = 9;


const int A2D_MSG_ID                  = 0x50;

const int START_LOOPBACK_TEST_MSG_ID  = 0x8E;
const int LOOPBACK_TEST_MSG_ID        = 0xEE;

const int SERIAL_NUMBER_LEN = 9;

const double V_REF      = 2.4;
const double A2D_RES    = 1024;
const double CONST_NUM1 = 0.776;
const double CONST_NUM2 = 0.00286;

// Constants for the Bumper Image List
const int BUMPER_DISABLED       = 0;
const int BUMPER_ENABLED_BUMPED = 1;
const int BUMPER_ENABLED        = 2;
const int BUMPER_UNKNOWN        = 3;
//

// Constants for the Heads Data Verify function
const int MIN_A2D_VALUE_FOR_60C = 850;
const int MAX_A2D_VALUE_FOR_60C = 1800;
const int MIN_A2D_VALUE_FOR_80C = 450;
const int MAX_A2D_VALUE_FOR_80C = 1300;

const float MAX_10KHZ_GAIN = 1.8;
const float MIN_10KHZ_GAIN = 0.3;

const float NOT_VALID = 0;
//




//---------------------------------------------------------------------------
__fastcall TOHDBTesterForm::TOHDBTesterForm(TComponent* Owner)
    : TForm(Owner)
{

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::FormCreate(TObject *Sender)
{
    E2PROMComboBox->ItemIndex  = 0;
    HeadNumComboBox->ItemIndex = 0;
    m_LoopbackFile             = new TMemoryStream;
    m_OHDBDisplayThread        = NULL;
    m_PCIStatus                = false;

    // Loading the "Unbumped" Bitmap to the Bumper Tab.
    BumperLedsImageList->GetBitmap (BUMPER_UNKNOWN,BumperOnOffLedImage->Picture->Bitmap);
    BumperOnOffLedImage->Refresh();
    BumperStatusLabel->Caption = "Status: Unknown";

    // Initiallize the heaters to be all on
    m_HeatersMask = 0x1FFF;

    m_InternalA2DVersion = 0;
    m_ExternalA2DVersion = 0;


}
//---------------------------------------------------------------------------

void TOHDBTesterForm::Open(CEdenProtocolEngine *ProtocolEngine)
{
    if (m_ProtocolClient == NULL)
    {
        m_ProtocolEngine = ProtocolEngine;
        COHDBProtocolClient::Init(m_ProtocolEngine);
        m_ProtocolClient = COHDBProtocolClient::Instance();

        if ((CAppParams::GetInstance())->OHDBA2DReadingsDisplay)
            m_OHDBDisplayThread = new COHDBDisplayThread(m_ProtocolClient, reinterpret_cast<TGenericCockie>(this), &A2DReceiveHandler, &StatusHandler, &StatusHandler);

        m_ProtocolClient->InstallMessageHandler(LOOPBACK_TEST_MSG_ID, 		 LoopBackTestRcvHandler,  reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_BUMPER_IMPACT_DETECTED, BumperRcvHandler,        reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_WAKE_UP_NOTIFICATION, 	 NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_ROLLER_ERROR, 			 NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_HEAD_HEATER_ERROR, 	 NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_HEAD_HEATER_ARE_ON_OFF, NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_FIRE_ALL_DONE, 	     NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_HEADS_DATA_NOT_VALID,   NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));
        m_ProtocolClient->InstallMessageHandler(OHDB_HEADS_DATA_WRITE_ERROR, NotificationsRcvHandler, reinterpret_cast<TGenericCockie>(this));

        if ((CAppParams::GetInstance())->PCICardExist)
        {
            if (EdenPCISystem_Init() == 0)
            {
                FIFOPCI_Init ();
                FIFOStatusTimer->Enabled = true;
                m_PCIStatus = true;
                LoopbackLoadButton->Enabled = true;
            }
        }
    }

    Show();

    if(! m_OHDBDisplayThread->Running)
        m_OHDBDisplayThread->Resume();
}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::FormDestroy(TObject *Sender)
{
    if (m_OHDBDisplayThread)
    {
        m_OHDBDisplayThread->Terminate();
        m_OHDBDisplayThread->WaitFor();
        Q_SAFE_DELETE(m_OHDBDisplayThread);
    }

    if (m_PCIStatus)
    {
        EdenPCI_DisableInterrupt();
        EdenPCI_DeInit();
    }
}
//---------------------------------------------------------------------------


void TOHDBTesterForm::StatusHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    TOHDBTesterForm *InstancePtr = reinterpret_cast<TOHDBTesterForm *>(Cockie);

    // Build the read data message
    TOHDBGenericMessage *Response = static_cast<TOHDBGenericMessage *>(Data);

    switch (Response->MessageID)
    {
    case OHDB_E2PROM_DATA:
    {
        TOHDBE2PROMResponse *E2PROMResponse = static_cast<TOHDBE2PROMResponse *>(Data);

        InstancePtr->m_E2PROMAddress = E2PROMResponse->Address;
        InstancePtr->m_E2PROMData    = E2PROMResponse->Data;

        // Trigger the event
        InstancePtr->m_E2PROMEvent.SetEvent();
        break;
    }

    case OHDB_XILINX_DATA:
    {
        TOHDBXilinxResponse *XilinxResponse = static_cast<TOHDBXilinxResponse *>(Data);

        InstancePtr->m_XilinxAddress = XilinxResponse->Address;
        InstancePtr->m_XilinxData    = XilinxResponse->Data;

        // Trigger the event
        InstancePtr->m_XilinxEvent.SetEvent();
        break;
    }

    case OHDB_HEADS_TEMPERATURE_OK:
    {
        TOHDBHeadsTemperatureOkResponse *HeadsTempStatus = static_cast<TOHDBHeadsTemperatureOkResponse *>(Data);
        InstancePtr->m_HeadTempraturesStatus = HeadsTempStatus->TemperatureOk;

        // Trigger the event
        InstancePtr->m_HeadTemperatureEvent.SetEvent();
        break;
    }

    case OHDB_ROLLER_STATUS:
    {
        TOHDBRollerStatusResponse *RollerStatus = static_cast<TOHDBRollerStatusResponse *>(Data);
        InstancePtr->m_RollerStatus = RollerStatus->RollerStatus;
        //InstancePtr->m_RollerSpeed  = RollerStatus->Velocity;

        // Trigger the event
        InstancePtr->m_GetRollerStatusEvent.SetEvent();
        break;
    }

    case OHDB_HEADS_DRIVER_SW_VERSION:
    {
        TOHDBHeadsDriverSoftwareVersionResponse *SoftwareVersion = static_cast<TOHDBHeadsDriverSoftwareVersionResponse *>(Data);
        InstancePtr->m_ExternalSoftwareVersion = SoftwareVersion->ExternalVersion;
        InstancePtr->m_InternalSoftwareVersion = SoftwareVersion->InternalVersion;
        // Trigger the event
        InstancePtr->m_GetSoftwareVersionEvent.SetEvent();
        break;

    }

    case OHDB_A2D_SW_VERSION:
    {
        TOHDBA2DDriverSoftwareVersionResponse *A2DVersion = static_cast<TOHDBA2DDriverSoftwareVersionResponse *>(Data);
        InstancePtr->m_ExternalA2DVersion = A2DVersion->ExternalVersion;
        InstancePtr->m_InternalA2DVersion = A2DVersion->InternalVersion;

        break;
    }

    case OHDB_HEADS_DRIVER_HW_VERSION:
    {
        TOHDBHeadsDriverHardwareVersionResponse *HardwareVersion = static_cast<TOHDBHeadsDriverHardwareVersionResponse *>(Data);
        InstancePtr->m_HardwareVersion = HardwareVersion->Version;

        // Trigger the event
        InstancePtr->m_GetHardwareVersionEvent.SetEvent();
        break;
    }

    case OHDB_PRINTING_HEADS_VOLTAGES:
    {
        TOHDBPrintingHeadsVoltagesResponse *HeadsVoltages = static_cast<TOHDBPrintingHeadsVoltagesResponse *>(Data);

        // Trigger the event
        InstancePtr->m_GetHeadsVoltagesEvent.SetEvent();
        memcpy(InstancePtr->m_HeadsVoltages,HeadsVoltages->HeadsVoltages,NUMBER_OF_HEADS * sizeof(WORD));

        // post a message to the dialog to display the current heads volategs
        PostMessage(InstancePtr->Handle,WM_OHDB_HEADS_VOLTAGES,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
        break;
    }

    case OHDB_HEADS_TEMPERATURE_STATUS:
    {
        TOHDBHeadsTemperatureStatusResponse *HeadsHeaters = static_cast<TOHDBHeadsTemperatureStatusResponse *>(Data);

        // Trigger the event
        InstancePtr->m_A2DHeatersDisplayEvent.SetEvent();
        memcpy(InstancePtr->m_HeadsHeaters, HeadsHeaters->CurrentElements.HeaterTable, NUMBER_OF_HEATERS * sizeof(WORD));

        // post a message to the dialog to display the current heads volategs
        PostMessage(InstancePtr->Handle,WM_OHDB_HEADS_HEATERS,reinterpret_cast<int> (0), reinterpret_cast<int> (0));
        break;
    }

    case OHDB_HEAD_DATA:
    {
        TOHDBHeadDataMsg *HeadData = static_cast<TOHDBHeadDataMsg *>(Data);

        // Trigger the event
        InstancePtr->m_GetHeadDataEvent.SetEvent();
        memcpy(&InstancePtr->m_HeadData,&HeadData->A2DValueFor60C,sizeof(THeadParameters));
        InstancePtr->m_HeadNum          = HeadData->HeadNum;
        InstancePtr->m_HeadDataCheckSum = HeadData->CheckSum;
        InstancePtr->m_HeadDataValidity = HeadData->Validity;

        break;
    }

    case OHDB_MATERIAL_SENSOR_STATUS:
    {
//        TOHDBMaterialLevelSensorStatusResponse *MaterialLevelSensorStatus = static_cast<TOHDBMaterialLevelSensorStatusResponse *>(Data);

//        InstancePtr->m_ModelFullLevel = MaterialLevelSensorStatus->ModelFullLevel;
//        InstancePtr->m_ModelHalfFullLevel = MaterialLevelSensorStatus->ModelHalfFullLevel;
//        InstancePtr->m_SupportFullLevel = MaterialLevelSensorStatus->SupportFullLevel;
//        InstancePtr->m_SupportlHalfFullLevel = MaterialLevelSensorStatus->SupportlHalfFullLevel;

        // Trigger the event
        InstancePtr->m_GetMaterialLevelEvent.SetEvent();
        break;
    }

    case OHDB_VACUUM_OK:
    {
        TOHDBHeadsVacuumOkResponse *HeadsVacuumOkResponse = static_cast<TOHDBHeadsVacuumOkResponse *>(Data);

        InstancePtr->m_HeadsVacuumOK = HeadsVacuumOkResponse->VacuumOk;

        // Trigger the event
        InstancePtr->m_GetVacuumStatusEvent.SetEvent();
        break;
    }

    case OHDB_VACUUM_STATUS:
    {
        TOHDBHeadsVacuumStatusResponse *HeadsVacuumStatusResponse = static_cast<TOHDBHeadsVacuumStatusResponse *>(Data);

        InstancePtr->m_ModelVacuumSensorValue = HeadsVacuumStatusResponse->ModelVacuumSensorValue;

        // Trigger the event
        InstancePtr->m_GetVacuumSensorStatusEvent.SetEvent();
        break;
    }

    case OHDB_AMBIENT_TEMPERATURE_STATUS:
    {
        TOHDBAmbientTemperatureStatusResponse *AmbientTemperatureResponse = static_cast<TOHDBAmbientTemperatureStatusResponse *>(Data);

        InstancePtr->m_AmbientTemperatureValue = AmbientTemperatureResponse->TemperatureValue;

        // Trigger the event
        InstancePtr->m_GetAmbientTemperatureEvent.SetEvent();
        break;
    }

    case OHDB_POWER_SUPPLIES_VOLTAGES:
    {
        TOHDBPowerSuppliesVoltagesMessage *PowerSuppliesVoltagesResponse = static_cast<TOHDBPowerSuppliesVoltagesMessage *>(Data);

        InstancePtr->m_VppPowerSupplyValue       = PowerSuppliesVoltagesResponse->VppPowerSupply;
        InstancePtr->m_Heater24VPowerSupplyValue = PowerSuppliesVoltagesResponse->V_24Voltage;
        InstancePtr->m_VDDPowerSupplyValue       = PowerSuppliesVoltagesResponse->VDDPowerSupply;
        InstancePtr->m_VCCPowerSupplyValue       = PowerSuppliesVoltagesResponse->VCCPowerSupply;
        InstancePtr->m_V_3_3_Voltage             = PowerSuppliesVoltagesResponse->V_3_3_Voltage;
        InstancePtr->m_V_1_2_Voltage             = PowerSuppliesVoltagesResponse->V_1_2_Voltage;

        // Trigger the event
        InstancePtr->m_GetPowerSuppliesVoltagesEvent.SetEvent();
        break;
    }

    case OHDB_FIRE_ALL_STATUS:
    {
        TOHDBFireAllStatusResponse *FireAllStatusResponse = static_cast<TOHDBFireAllStatusResponse *>(Data);

        InstancePtr->m_FireAllStatus = FireAllStatusResponse->Status;
        InstancePtr->m_FireAllCounter = FireAllStatusResponse->FireAllCounter;

        // Trigger the event
        InstancePtr->m_FireAllStatusEvent.SetEvent();
        break;
    }

    default:
        return;
    }

    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
}
//-------------------------------------------------------------------------

void TOHDBTesterForm::AckHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    TOHDBTesterForm *InstancePtr = reinterpret_cast<TOHDBTesterForm *>(Cockie);

    // Build the read data message
    TOHDBAck *Msg = static_cast<TOHDBAck *>(Data);

    if(Msg->MessageID != OHDB_ACK)
        return;

    switch (Msg->RespondedMessageID)
    {
    case OHDB_WRITE_DATA_TO_XILINX:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_XilinxWriteEvent.SetEvent();
        break;

    case OHDB_SET_POTENTIOMETER_VALUE:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_PotentiometerEvent.SetEvent();
        break;

    case OHDB_WRITE_DATA_TO_E2PROM:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_E2PROMWriteEvent.SetEvent();
        break;

    case OHDB_SET_ROLLER_ON_OFF:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_RollerOnOffEvent.SetEvent();
        break;

    case OHDB_SET_ROLLER_SPEED:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_RollerSpeedEvent.SetEvent();
        break;

    case START_LOOPBACK_TEST_MSG_ID:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_StartLoopBackEvent.SetEvent();
        break;

    case OHDB_SET_HEAD_HEATER_ON_OFF:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetHeatersOnOffEvent.SetEvent();
        break;

    case OHDB_SET_PRINTING_HEADS_VOLTAGES:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetHeadsVoltagesEvent.SetEvent();
        break;

    case OHDB_SET_HEAD_DATA:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetHeadDataEvent.SetEvent();
        break;

    case OHDB_SET_MATERIAL_COOLING_FAN_ON_OFF:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_MaterialCoolingFanEvent.SetEvent();
        break;

    case OHDB_SET_HEADS_VACUUM_PARAMS:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetVacuumValuesEvent.SetEvent();
        break;

    case OHDB_SW_RESET:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_ResetSoftwareEvent.SetEvent();
        break;

    case OHDB_RESET_DRIVER_CIRCUIT:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_ResetDriverCircuitEvent.SetEvent();
        break;

    case OHDB_SET_PRINTING_PASS_PARAMS:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetPrintingPassParamsEvent.SetEvent();
        break;

    case OHDB_ENABLE_PRINTING_CIRCUIT:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_EnablePrintingCircuitEvent.SetEvent();
        break;

    case OHDB_DISABLE_PRINTING_CIRCUIT:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_DisablePrintingCircuitEvent.SetEvent();
        break;

    case OHDB_RESET_DRIVER_STATE_MACHINE:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_ResetDriverStateMachineEvent.SetEvent();
        break;

    case OHDB_APPLY_DEFAULT_PRINT_PARAMS:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_ApplyDefaultPrintingParamsEvent.SetEvent();
        break;

    case OHDB_FIRE_ALL:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_FireAllEvent.SetEvent();
        break;

    case OHDB_SET_BUMPER_PARAMS:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetBumperSensitivityEvent.SetEvent();
        break;

    case OHDB_SET_BUMPER_IMPACT_ON_OFF:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetBumperOnOffEvent.SetEvent();
        break;

    case OHDB_SET_BUMPER_PEG_ON_OFF:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetBumperPegOnOffEvent.SetEvent();
        break;

    case OHDB_SET_HEATERS_TEMPERATURE:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_SetHeatersTempEvent.SetEvent();
        break;

    default:
        return;
    }

    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
}
//-------------------------------------------------------------------------

void TOHDBTesterForm::A2DReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{

    TOHDBA2DReadingsResponse *A2DMsg = static_cast<TOHDBA2DReadingsResponse *>(Data);
    TOHDBTesterForm *InstancePtr = reinterpret_cast<TOHDBTesterForm *>(Cockie);

    if (A2DMsg->MessageID != OHDB_A2D_READINGS)
        return;

    InstancePtr->m_A2DDisplayEvent.SetEvent();
    InstancePtr->m_OHDBDisplayThread->EnableErrorMessageDisplay(true);
    // (zohar) InstancePtr->m_ProtocolClient->UnInstallReceiveHandler(TransactionId);
    memcpy(InstancePtr->m_A2DValues,A2DMsg->Readings,NUM_OF_ANALOG_INPUTS * sizeof(WORD));

    // post a message to the dialog to display the current received reading
    PostMessage(InstancePtr->Handle,WM_OHDB_A2D_READINGS_DISPLAY,reinterpret_cast<int> (0), reinterpret_cast<int> (0));

}
//-------------------------------------------------------------------------


// display an A2D reading
MESSAGE void TOHDBTesterForm::DisplayA2DReading(TMessage& Message)
{
    for (int InputNum = 1; InputNum <= NUM_OF_ANALOG_INPUTS; InputNum++)
    {
        // find the panel with a tag that is equal to the analog input number
        TPanel* Panel = NULL;
        for (int i = 0; i < this->ComponentCount; i++)
        {
            if(this->Components[i]->Tag == InputNum)
            {
                Panel = dynamic_cast<TPanel *>(this->Components[i]);
                if (Panel != NULL)
                {
                    Panel->Caption = m_A2DValues[InputNum-1];
                    break;
                }
            }
        }

    }

}
//---------------------------------------------------------------------------


// Display the heads voltages
MESSAGE void TOHDBTesterForm::HeadsVoltagesDisplay(TMessage& Message)
{
    // Update the heads voltages display
    TPanel *Panel = NULL;
    for (int i = 0; i < HeadsVoltagesGroupBox->ControlCount; i++)
    {
        Panel = dynamic_cast<TPanel *>(HeadsVoltagesGroupBox->Controls[i]);
        if (Panel != NULL)
        {
            if (Panel->Tag < NUMBER_OF_HEADS)
            {
                Panel->Caption = StrToInt(m_HeadsVoltages[Panel->Tag]);
            }
            else
            {
                float A2DVoltage = m_HeadsVoltages[Panel->Tag - NUMBER_OF_HEADS];
                float HeadVoltage = CONVERT_HEAD_VPP_A2D_TO_VOLT(A2DVoltage);

                Panel->Caption = FloatToStrF (HeadVoltage, ffFixed, 4, 2);
            }
        }
    }
}
//---------------------------------------------------------------------------

// Display the heads voltages
MESSAGE void TOHDBTesterForm::HeadsHeatersDisplay(TMessage& Message)
{
    // Update the heads heaters display
    TEdit *Edit = NULL;
    for (int i = 0; i < SetHeadsHeatersGroupBox->ControlCount; i++)
    {
        Edit = dynamic_cast<TEdit *>(SetHeadsHeatersGroupBox->Controls[i]);
        if (Edit != NULL)
        {
            if (Edit->Tag != -1)
            {
                Edit->Text = StrToInt(m_HeadsHeaters[Edit->Tag]);
            }
        }

    }

}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::XilinxWriteButtonClick(TObject *Sender)
{

    try
    {
        BYTE Address = (BYTE) StrToInt("$"+XilinxAddressEdit->Text);
        WORD Data = (WORD) StrToInt("$"+XilinxDataEdit->Text);
        if (MacroRecordCheckBox->Checked)
            AddToMacroRecorder(Address, Data);

		WriteDataToXilinx(Address, Data);
    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::XilinxReadButtonClick(TObject *Sender)
{
	try
    {
        TXilinxReadMessage Msg;
        int Address = StrToInt("$" + XilinxAddressEdit->Text);

        Msg.MessageID = OHDB_READ_FROM_XILINX;
        Msg.Address = (BYTE) Address;
        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TXilinxReadMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_XilinxEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TXilinxReadMessage),
                StatusHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        else
        {
            if (m_XilinxAddress != Address)
            {
                MessageDlg("Tester recieved wrong address",mtError,TMsgDlgButtons() << mbOK,0);
                return;
            }
            else
            {
                XilinxDataEdit->Text = IntToHex(m_XilinxData,4);
                XilinxDataEdit->Font->Color = clBlue;
            }
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::E2PROMWriteButtonClick(TObject *Sender)
{
    try
    {
        TOHDBE2PROMWriteMessage Msg;

        Msg.MessageID = OHDB_WRITE_DATA_TO_E2PROM;
        Msg.HeadNum   = E2PROMComboBox->ItemIndex;
        Msg.Address   = (WORD) StrToInt("$"+E2PROMAddressEdit->Text);
        Msg.Data      = (WORD) StrToInt("$"+E2PROMDataEdit->Text);
        int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBE2PROMWriteMessage));

#ifdef WAIT_FOR_ACK
        m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        QLib::TQWaitResult WaitResult = m_E2PROMWriteEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (WaitResult != QLib::wrSignaled)
        {
            m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }
#endif
    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::E2PROMReadButtonClick(TObject *Sender)
{
    try
    {
        TOHDBE2PROMReadMessage Msg;
        int Address = StrToInt("$" + E2PROMAddressEdit->Text);

        Msg.MessageID = OHDB_READ_FROM_E2PROM;
        Msg.HeadNum   = E2PROMComboBox->ItemIndex;
        Msg.Address   = (WORD) Address;
        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBE2PROMReadMessage));


        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_E2PROMEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBE2PROMReadMessage),
                StatusHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
        else
        {
            if (m_E2PROMAddress != Address)
            {
                MessageDlg("Tester recieved wrong address",mtError,TMsgDlgButtons() << mbOK,0);
                return;
            }
            else
            {
                E2PROMDataEdit->Text = IntToHex(m_E2PROMData,2);
                E2PROMDataEdit->Font->Color = clBlue;
            }
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);
    }

}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::PotenmtrWriteButtonClick(TObject *Sender)
{

    try
    {
        TOHDBSetPotentiometerValueMessage Msg;
        Msg.MessageID          = OHDB_SET_POTENTIOMETER_VALUE;
        Msg.HeadNum            = PotentiometerComboBox->ItemIndex;
        Msg.PotentiometerValue = (BYTE) StrToInt("$"+PotenmtrDataEdit->Text);
        // (zohar) int TransactionHandle  = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetPotentiometerValueMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_PotentiometerEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetPotentiometerValueMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::DataEditClick(TObject *Sender)
{
    TEdit *Edit = dynamic_cast<TEdit *>(Sender);
    if (Edit == NULL)
        return;
    Edit->Font->Color = clBlack;
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetHeatersTempButtonClick(TObject *Sender)
{

    try
    {
        CAppParams *ParamsMgr = CAppParams::GetInstance();

        TOHDBSetHeatersTemperatureMessage Msg;
        Msg.MessageID = OHDB_SET_HEATERS_TEMPERATURE;

        ParamsMgr->SupportHead3Temp      = Msg.SetElements.HeaterTable[0]  = (WORD) StrToInt(SupportHead3TempEdit->Text);
        ParamsMgr->SupportHead2Temp      = Msg.SetElements.HeaterTable[1]  = (WORD) StrToInt(SupportHead2TempEdit->Text);
        ParamsMgr->SupportHead1Temp      = Msg.SetElements.HeaterTable[2]  = (WORD) StrToInt(SupportHead1TempEdit->Text);
        ParamsMgr->SupportHead0Temp      = Msg.SetElements.HeaterTable[3]  = (WORD) StrToInt(SupportHead0TempEdit->Text);
        ParamsMgr->ModelHead3Temp        = Msg.SetElements.HeaterTable[4]  = (WORD) StrToInt(ModelHead3TempEdit->Text);
        ParamsMgr->ModelHead2Temp        = Msg.SetElements.HeaterTable[5]  = (WORD) StrToInt(ModelHead2TempEdit->Text);
        ParamsMgr->ModelHead1Temp        = Msg.SetElements.HeaterTable[6]  = (WORD) StrToInt(ModelHead1TempEdit->Text);
        ParamsMgr->ModelHead0Temp        = Msg.SetElements.HeaterTable[7]  = (WORD) StrToInt(ModelHead0TempEdit->Text);
        ParamsMgr->SupportBlockFrontTemp = Msg.SetElements.HeaterTable[8]  = (WORD) StrToInt(SupportBlockFrontTempEdit->Text);
        ParamsMgr->SupportBlockRearTemp  = Msg.SetElements.HeaterTable[9]  = (WORD) StrToInt(SupportBlockRearTempEdit->Text);
        ParamsMgr->ModelBlockRearTemp    = Msg.SetElements.HeaterTable[10] = (WORD) StrToInt(ModelBlockRearTempEdit->Text);
        ParamsMgr->ModelBlockFrontTemp   = Msg.SetElements.HeaterTable[11] = (WORD) StrToInt(ModelBlockFrontTempEdit->Text);
        ParamsMgr->ExternalLiquidTemp    = Msg.SetElements.HeaterTable[12] = (WORD) StrToInt(ExternalLiquidTempEdit->Text);
        ParamsMgr->LowThresholdTemp      = Msg.LowThreshold                = (WORD) StrToInt(LowThresholdTempEdit->Text);
        ParamsMgr->HighThresholdTemp     = Msg.HighThreshold               = (WORD) StrToInt(HighThresholdTempEdit->Text);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetHeatersTemperatureMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetHeatersTempEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetHeatersTemperatureMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::IsHeadTempOkButtonClick(TObject *Sender)
{
    /*#pragma pack(push,1)
      struct TIsHeadTemperatureOkMsg{
        BYTE MessageID;
        };
    #pragma pack(pop)

        TIsHeadTemperatureOkMsg Msg;
        Msg.MessageID = OHDB_IS_HEADS_TEMPERATURE_OK;
        int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TIsHeadTemperatureOkMsg));
    */


    TOHDBIsHeadsTempeartureOKMessage Msg;
    Msg.MessageID = OHDB_IS_HEADS_TEMPERATURE_OK;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBIsHeadsTempeartureOKMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_HeadTemperatureEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBIsHeadsTempeartureOKMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        if (m_HeadTempraturesStatus == 0)
            TemperatureStatusPanel->Caption = "Not Ok";
        else if (m_HeadTempraturesStatus == 1)
            TemperatureStatusPanel->Caption = "Ok";
    }

}
//---------------------------------------------------------------------------


void TOHDBTesterForm::AddToMacroRecorder(BYTE Address, WORD Data)
{
    MacroMemo->Lines->Add(IntToHex(Address,2) + " " + IntToHex(Data,4));
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::MacroLoadButtonClick(TObject *Sender)
{
    if(MacroOpenDialog->Execute())
        MacroMemo->Lines->LoadFromFile(MacroOpenDialog->FileName);

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::MacroSaveButtonClick(TObject *Sender)
{
    if(MacroSaveDialog->Execute())
        MacroMemo->Lines->SaveToFile(MacroSaveDialog->FileName);

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::MacroExecuteButtonClick(TObject *Sender)
{
    DWORD Adr, Data;

    for(int i = 0; i < MacroMemo->Lines->Count; i++)
    {
        if(sscanf(MacroMemo->Lines->Strings[i].c_str(), "%x %x", &Adr, &Data) != 2)
        {
            MessageDlg("Invalid macro format",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }

        WriteDataToXilinx((BYTE)Adr, (WORD)Data);
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::TurnOffHeatersButtonClick(TObject *Sender)
{

    TOHDBSetHeadHeaterOnOffMessage Msg;
    Msg.MessageID   = OHDB_SET_HEAD_HEATER_ON_OFF;
    Msg.OnOff       = (SetHeatersOnRadioButton->Checked ? 1 : 0);
    Msg.HeatersMask = m_HeatersMask;

    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetHeadHeaterOnOffMessage));
    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_SetHeatersOnOffEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBSetHeadHeaterOnOffMessage),
            AckHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
    }

}
//---------------------------------------------------------------------------


void TOHDBTesterForm::WriteDataToXilinx(BYTE Address, WORD Data)
{
    TOHDBXilinxWriteMessage Msg;
    Msg.MessageID = OHDB_WRITE_DATA_TO_XILINX;
    Msg.Address = Address;
    Msg.Data = Data;

    int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBXilinxWriteMessage));
#ifdef WAIT_FOR_ACK
    m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    QLib::TQWaitResult WaitResult = m_XilinxWriteEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (WaitResult != QLib::wrSignaled)
    {
        m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
    }
#else
    Sleep(30);
#endif

}
//---------------------------------------------------------------------------



void __fastcall TOHDBTesterForm::SetRollerOnOffButtonClick(TObject *Sender)
{
    TOHDBSetRollerOnOffMessage Msg;
    Msg.MessageID = OHDB_SET_ROLLER_ON_OFF;
    Msg.OnOff = (RollerOnRadioButton->Checked ? 1 : 0);
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetRollerOnOffMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_RollerOnOffEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBSetRollerOnOffMessage),
            AckHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::setRollerSpeedButtonClick(TObject *Sender)
{
    try
    {
        TOHDBSetRollerSpeedMessage Msg;
        Msg.MessageID = OHDB_SET_ROLLER_SPEED;
        Msg.Velocity  = (WORD) StrToInt(RollerSpeedEdit->Text);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetRollerSpeedMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_RollerSpeedEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetRollerSpeedMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetRollerStatusButtonClick(TObject *Sender)
{
    TOHDBGetRollerStatusMessage Msg;
    Msg.MessageID = OHDB_GET_ROLLER_STATUS;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetRollerStatusMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetRollerStatusEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetRollerStatusMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        if (m_RollerStatus == 0)
            RollerStatusPanel->Caption = "Off";
        else if (m_RollerStatus == 1)
            RollerStatusPanel->Caption = "On";

        RollerSpeedPanel->Caption = m_RollerSpeed;
    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::LoopbackLoadButtonClick(TObject *Sender)
{
    if(LoopbackOpenDialog->Execute())
    {
        m_LoopbackFile->LoadFromFile(LoopbackOpenDialog->FileName);

        LoopbackGoBitBtn->Enabled = true;


    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::LoopbackGoBitBtnClick(TObject *Sender)
{
#pragma pack(push,1)
    struct TStartLoopBackTest
    {
        BYTE MessageID;
    };
#pragma pack(pop)

    BYTE Buffer[1000];

    // enable the stop button
    LoopbackStopBitBtn->Enabled = true;

    // disbale the go and the load buttons
    LoopbackLoadButton->Enabled = false;
    LoopbackGoBitBtn->Enabled = false;

    m_StopLoopbackTest = false;
    m_LoopBackCompareIndex = 0;
    m_LoopbackFile->Position = 0;

    if (EnableOHDBCheckBox->Checked)
    {
        // send 'Start loop back test' message
        TStartLoopBackTest Msg;

        Msg.MessageID = START_LOOPBACK_TEST_MSG_ID;
        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TStartLoopBackTest));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_StartLoopBackEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TStartLoopBackTest),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
            return;
        }
    }



    if (ContinuousCheckBox->Checked)
    {
        unsigned long CurrTickCount, PrevTickCount;
        PrevTickCount = GetTickCount();
        while(!m_StopLoopbackTest)
        {
            if (FIFOPCI_IsEmpty())
                FIFOPCI_WriteAsync((BYTE*)m_LoopbackFile->Memory,m_LoopbackFile->Size/sizeof(DWORD) + (m_LoopbackFile->Size%sizeof(DWORD) ? 1 : 0));

            // print the time every 10 seconds
            CurrTickCount = GetTickCount();
            if (CurrTickCount > (PrevTickCount+10000))
            {
                PrevTickCount = CurrTickCount;
                QMonitor.Print(TimeToStr(Time()).c_str());
            }

            Application->ProcessMessages();
        }
    }
    else
        FIFOPCI_WriteAsync((BYTE*)m_LoopbackFile->Memory,m_LoopbackFile->Size/sizeof(DWORD) + (m_LoopbackFile->Size%sizeof(DWORD) ? 1 : 0));

    // disable the stop button
    LoopbackStopBitBtn->Enabled = false;

    // enbale the go and the load buttons
    LoopbackLoadButton->Enabled = true;
    LoopbackGoBitBtn->Enabled = true;

}
//---------------------------------------------------------------------------


void TOHDBTesterForm::LoopBackTestRcvHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
#pragma pack(push,1)
    struct TLoopBackTest
    {
        BYTE MessageID;
        BYTE Length;
        BYTE Data[32];
    };
#pragma pack(pop)

    TOHDBTesterForm *InstancePtr = reinterpret_cast<TOHDBTesterForm *>(Cockie);
    TLoopBackTest *Msg = static_cast<TLoopBackTest *>(Data);

    if (InstancePtr->m_LoopBackCompareIndex > InstancePtr->m_LoopbackFile->Size)
    {
        QMonitor.Printf("Receive more bytes than file size");
        return;
    }

    for (int i = 0; i < Msg->Length; i++)
    {
        BYTE Expected = ((BYTE *)(InstancePtr->m_LoopbackFile->Memory))[InstancePtr->m_LoopBackCompareIndex];
        if (Msg->Data[i] != Expected)
            QMonitor.Printf("Loopback error: %d Expected %x, Received %x",InstancePtr->m_LoopBackCompareIndex,Expected, Msg->Data[i]);
        else
            QMonitor.Printf("%d Expected %x, Received %x",InstancePtr->m_LoopBackCompareIndex, Expected, Msg->Data[i]);

        InstancePtr->m_LoopBackCompareIndex++;

    }

    if (InstancePtr->m_LoopBackCompareIndex == (InstancePtr->m_LoopbackFile->Size - 120))
    {
        QMonitor.Printf("Loopback test done");
        return;
    }

}
//---------------------------------------------------------------------------
void __fastcall TOHDBTesterForm::FormShow(TObject *Sender)
{
    CAppParams *ParamsMgr = CAppParams::GetInstance();

    // Put the potentiometer values in all the edit boxes
    TEdit *EditBox = NULL;
    for (int i = 0; i < HeadsVoltagesGroupBox->ControlCount; i++)
    {
        EditBox = dynamic_cast<TEdit *>(HeadsVoltagesGroupBox->Controls[i]);
        if (EditBox != NULL)
            EditBox->Text              = IntToStr((int)ParamsMgr->PotentiometerSetValue[EditBox->Tag]);
    }

    ModelHead0TempEdit->Text         = IntToStr((int)ParamsMgr->ModelHead0Temp);
    ModelHead1TempEdit->Text         = IntToStr((int)ParamsMgr->ModelHead1Temp);
    ModelHead2TempEdit->Text         = IntToStr((int)ParamsMgr->ModelHead2Temp);
    ModelHead3TempEdit->Text         = IntToStr((int)ParamsMgr->ModelHead3Temp);
    SupportHead0TempEdit->Text       = IntToStr((int)ParamsMgr->SupportHead0Temp);
    SupportHead1TempEdit->Text       = IntToStr((int)ParamsMgr->SupportHead1Temp);
    SupportHead2TempEdit->Text       = IntToStr((int)ParamsMgr->SupportHead2Temp);
    SupportHead3TempEdit->Text       = IntToStr((int)ParamsMgr->SupportHead3Temp);
    ModelBlockFrontTempEdit->Text    = IntToStr((int)ParamsMgr->ModelBlockFrontTemp);
    ModelBlockRearTempEdit->Text     = IntToStr((int)ParamsMgr->ModelBlockRearTemp);
    SupportBlockFrontTempEdit->Text  = IntToStr((int)ParamsMgr->SupportBlockFrontTemp);
    SupportBlockRearTempEdit->Text   = IntToStr((int)ParamsMgr->SupportBlockRearTemp);
    ExternalLiquidTempEdit->Text     = IntToStr((int)ParamsMgr->ExternalLiquidTemp);
    LowThresholdTempEdit->Text       = IntToStr((int)ParamsMgr->LowThresholdTemp);
    HighThresholdTempEdit->Text      = IntToStr((int)ParamsMgr->HighThresholdTemp);

    SetBumperSensitivityEdit->Text = IntToStr((int)ParamsMgr->BumperSensitivity);
    SetBumperResetTimeEdit->Text = IntToStr((int)ParamsMgr->BumperResetTime);
    SetBumperImpactCounterEdit->Text = IntToStr((int)ParamsMgr->BumperImpactCounter);

#ifdef OHDB2
    for (int InputNum = 1; InputNum <= NUM_OF_ANALOG_INPUTS; InputNum++)
    {
        TLabel *Label = NULL;
        for (int i = 0; i < this->ComponentCount; i++)
        {
            Label = dynamic_cast<TLabel *>(this->Components[i]);
            if(Label)
            {
                switch(Label->Tag)
                {
                case 13:
                    Label->Caption = "In 13 (M1 Liquid Level)";
                    break;
                case 14:
                    Label->Caption = "In 14 (M2 Liquid Level)";
                    break;
                case 15:
                    Label->Caption = "In 15 (M3 Liquid Level)";
                    break;
                case 16:
                    Label->Caption = "In 16 (S Liquid Level)";
                    break;
                case 17:
                    Label->Caption = "In 17 (M1/M2 Liquid Level)";
                    break;
                case 18:
                    Label->Caption = "In 18 (S/M3 Liquid Level)";
                    break;
                case 19:
                    Label->Caption = "In 19 (Spare Therm1)";
                    break;
                case 20:
                    Label->Caption = "In 20 (Spare Therm2)";
                    break;
                case 21:
                    Label->Caption = "In 21 (Vacuum Sensor)";
                    break;
                case 22:
                    Label->Caption = "In 22 (Pre-Heater)";
                    break;
                case 23:
                    Label->Caption = "In 23 (Temp1)";
                    break;
                case 24:
                    Label->Caption = "In 24 (Temp2)";
                    break;
                }
            }
        }
    }
#endif
}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::LoopbackStopBitBtnClick(TObject *Sender)
{
    // disbale the stop button
    LoopbackStopBitBtn->Enabled = false;

    //  enable the go and the load buttons
    LoopbackLoadButton->Enabled = true;
    LoopbackGoBitBtn->Enabled = true;

    m_StopLoopbackTest = true;

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::FIFOStatusTimerTimer(TObject *Sender)
{
    if (!m_PCIStatus)
        return;

    switch (FIFOPCI_ReadStatus())
    {
    case FIFO_EMPTY:
        FIFOStatusProgressBar->Position = FIFOStatusProgressBar->Min;
        break;
    case FIFO_1_TO_AE:
        FIFOStatusProgressBar->Position = FIFOStatusProgressBar->Max / 5;
        break;
    case FIFO_AE_TO_HF:
        FIFOStatusProgressBar->Position = FIFOStatusProgressBar->Max / 5 * 2;
        break;
    case FIFO_HF_TO_AF:
        FIFOStatusProgressBar->Position = FIFOStatusProgressBar->Max / 5 * 3;
        break;
    case FIFO_AF_TO_FULL:
        FIFOStatusProgressBar->Position = FIFOStatusProgressBar->Max / 5 * 4;
        break;
    case FIFO_FULL:
        FIFOStatusProgressBar->Position = FIFOStatusProgressBar->Max;
        break;

    default:
        break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetSoftwareVersionButtonClick(TObject *Sender)
{
    TOHDBGetDriverSWVersionMessage Msg;
    Msg.MessageID = OHDB_GET_HEADS_DRIVER_SW_VERSION;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetDriverSWVersionMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetSoftwareVersionEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetDriverSWVersionMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        SoftwareVersionPanel->Caption = QFormatStr("%d.%02d", m_ExternalSoftwareVersion, m_InternalSoftwareVersion).c_str();
    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetHardwareVersionButtonClick(TObject *Sender)
{
    TOHDBGetDriverHWVersionMessage Msg;
    Msg.MessageID = OHDB_GET_HEADS_DRIVER_HW_VERSION;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetDriverHWVersionMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetHardwareVersionEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetDriverHWVersionMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        HardwareVersionPanel->Caption = IntToStr(m_HardwareVersion);
    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetPotnmtrValuesButtonClick(TObject *Sender)
{
    try
    {
        CAppParams *ParamsMgr = CAppParams::GetInstance();

        TOHDBSetPrintingHeadsVoltagesMessage Msg;

        // Get the potentiometer values from all the edit boxes
        TEdit *EditBox = NULL;
        for (int i = 0; i < HeadsVoltagesGroupBox->ControlCount; i++)
        {
            EditBox = dynamic_cast<TEdit *>(HeadsVoltagesGroupBox->Controls[i]);
            if (EditBox != NULL)
                ParamsMgr->PotentiometerSetValue[EditBox->Tag] = Msg.PotentiometerValues[EditBox->Tag] = StrToInt(EditBox->Text);
        }

        Msg.MessageID = OHDB_SET_PRINTING_HEADS_VOLTAGES;
        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetPrintingHeadsVoltagesMessage));
        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetHeadsVoltagesEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetPrintingHeadsVoltagesMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            throw Exception("Tester did not recieve Ack");
        }

    }
    catch (Exception &exception)
    {
        MessageDlg(exception.Message,mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------



void __fastcall TOHDBTesterForm::HeatersMaskCheckBoxClick(TObject *Sender)
{
    TCheckBox *CheckBox = dynamic_cast<TCheckBox *>(Sender);
    if (CheckBox == NULL)
        return;

    if (CheckBox->Checked)
        m_HeatersMask |= (1 << CheckBox->Tag);
    else
        m_HeatersMask &= ~(1 << CheckBox->Tag);

}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::SetHeadDataButtonClick(TObject *Sender)
{
    try
    {
        TOHDBSetHeadDataMsg Msg;

        Msg.MessageID              = OHDB_SET_HEAD_DATA;
        Msg.HeadNum                = HeadNumComboBox->ItemIndex;
        Msg.A2DValueFor60C         = StrToInt(A2DValuefor60CEdit->Text);
        Msg.A2DValueFor80C         = StrToInt(A2DValuefor80CEdit->Text);
        Msg.Model10KHzLineGain     = StrToFloat(Model10KhzAEdit->Text);
        Msg.Model10KHzLineOffset   = StrToFloat(Model10KhzBEdit->Text);
        Msg.Model20KHzLineGain     = StrToFloat(Model20KhzAEdit->Text);
        Msg.Model20KHzLineOffset   = StrToFloat(Model20KhzBEdit->Text);
        Msg.Support10KHzLineGain   = StrToFloat(Support10KhzAEdit->Text);
        Msg.Support10KHzLineOffset = StrToFloat(Support10KhzBEdit->Text);
        Msg.Support20KHzLineGain   = StrToFloat(Support20KhzAEdit->Text);
        Msg.Support20KHzLineOffset = StrToFloat(Support20KhzBEdit->Text);
        Msg.XOffset                = StrToFloat(XOffsetEdit->Text);
        Msg.ProductionDate         = (int)(ProductionDateTimePicker->Date);

        memset(&Msg.SerialNumber, 0, sizeof(Msg.SerialNumber));
        memcpy(Msg.SerialNumber, SerialNumberEdit->Text.c_str(), SerialNumberEdit->Text.Length());

        SerialNumberEdit->Font->Color   = clBlack;
        Support10KhzAEdit->Font->Color  = clBlack;
        Model20KhzAEdit->Font->Color    = clBlack;
        Support20KhzAEdit->Font->Color  = clBlack;
        Support20KhzBEdit->Font->Color  = clBlack;
        Support10KhzBEdit->Font->Color  = clBlack;
        Model10KhzAEdit->Font->Color    = clBlack;
        Model20KhzBEdit->Font->Color    = clBlack;
        Model10KhzBEdit->Font->Color    = clBlack;
        A2DValuefor80CEdit->Font->Color = clBlack;
        A2DValuefor60CEdit->Font->Color = clBlack;
		ProductionDateTimePicker->Color = clWhite;
        XOffsetEdit->Font->Color        = clBlack;


        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetHeadDataMsg));
        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetHeadDataEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBSetHeadDataMsg), AckHandler, reinterpret_cast<TGenericCockie>(this), 1500) != QLib::wrSignaled)
        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            throw Exception("Tester did not recieve Ack");
        }
    }
    catch (Exception &exception)
    {
        MessageDlg(exception.Message,mtError,TMsgDlgButtons() << mbOK,0);
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetMaterialLevelStatusButtonClick(
    TObject *Sender)
{
    TOHDBGetMaterialLevelSensorStatusMessage Msg;
    Msg.MessageID = OHDB_GET_MATERIAL_LEVEL_SENSORS_STATUS;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetMaterialLevelSensorStatusMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetMaterialLevelEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetMaterialLevelSensorStatusMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        GetSupportMaterialStatusLevelEdit->Text = IntToStr (m_SupportFullLevel);
        GetModelMaterialStatusLevelEdit->Text   = IntToStr (m_ModelFullLevel);
    }
}
//---------------------------------------------------------------------------





void __fastcall TOHDBTesterForm::SetMaterialCoolingFanOnBtnClick(
    TObject *Sender)
{
    SetMaterialCoolingFanEdit->Enabled = True;
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetMaterialCoolingFanOffBtnClick(
    TObject *Sender)
{
    SetMaterialCoolingFanEdit->Enabled = False;

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetMaterialCoolingFanButtonClick(TObject *Sender)
{
    try
    {
        bool OnOff = (SetMaterialCoolingFanOnBtn->Checked ? 1 : 0);
        TOHDBSetMaterialCollingMessage Msg;

        Msg.MessageID = OHDB_SET_MATERIAL_COOLING_FAN_ON_OFF;
        Msg.OnOff = static_cast<BYTE>(OnOff);
        Msg.Percent = (BYTE) StrToInt(SetMaterialCoolingFanUpDown->Position);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetMaterialCollingMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_MaterialCoolingFanEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetMaterialCollingMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetVacuumModelValueButtonClick(
    TObject *Sender)
{
    try
    {
        TOHDBSetHeadsVacuumParamsMessage Msg;

        Msg.MessageID = OHDB_SET_HEADS_VACUUM_PARAMS;
        Msg.ModelHighLimit = (WORD) StrToInt(SetVacuumModelHighValueEdit->Text);
        Msg.ModelLowLimit  = (WORD) StrToInt(SetVacuumModelLowValueEdit->Text);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetHeadsVacuumParamsMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetVacuumValuesEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetHeadsVacuumParamsMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetVacuumStatusButtonClick(
    TObject *Sender)
{
    TOHDBIsHeadsVacuumOKMessage Msg;
    Msg.MessageID = OHDB_IS_HEADS_VACUUM_OK;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBIsHeadsVacuumOKMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetVacuumStatusEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBIsHeadsVacuumOKMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        AnsiString IsHeadsVacuumOK = (m_HeadsVacuumOK ? "OK" : "Not OK");
        GetVacuumStatusEdit->Text = IsHeadsVacuumOK;
    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetVacuumSensorStatusButtonClick(
    TObject *Sender)
{
    TOHDBGetHeadsVacuumSensorStatusMessage Msg;
    Msg.MessageID = OHDB_GET_VACUUM_SENSOR_STATUS;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetHeadsVacuumSensorStatusMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetVacuumSensorStatusEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetHeadsVacuumSensorStatusMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        GetVacuumSensorStatusEdit->Text = IntToStr (m_ModelVacuumSensorValue);
    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetAmbientTemperatureButtonClick(
    TObject *Sender)
{
    TOHDBGetAmbientSensorStatusMessage Msg;
    Msg.MessageID = OHDB_GET_AMBIENT_TEMP_SENSOR_STATUS;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetAmbientSensorStatusMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetAmbientTemperatureEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetAmbientSensorStatusMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        float Temerature = ((((m_AmbientTemperatureValue * V_REF) / A2D_RES) - CONST_NUM1) / CONST_NUM2);

        GetAmbientTemperatureA2DEdit->Text = IntToStr (m_AmbientTemperatureValue);
        GetAmbientTemperatureEdit->Text    = FloatToStrF (Temerature, ffFixed, 4, 2);
    }
//         FloatToStrF(m_HeadData.Model10KHzLineGain,ffFixed,4,2);
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetPowerSuppliesVoltagesButtonClick(
    TObject *Sender)
{
    TOHDBGetPowerSuppliesVoltagesMessage Msg;
    Msg.MessageID = OHDB_GET_POWER_SUPPLIES_VOLTAGES;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetPowerSuppliesVoltagesMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_GetPowerSuppliesVoltagesEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetPowerSuppliesVoltagesMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        float VoltVpp  = CONVERT_VPP_A2D_TO_VOLT(m_VppPowerSupplyValue);
        float Volt24V  = CONVERT_24V_A2D_TO_VOLT(m_Heater24VPowerSupplyValue);
        float VoltVdd  = CONVERT_VDD_A2D_TO_VOLT(m_VDDPowerSupplyValue);
        float VoltVcc  = CONVERT_VCC_A2D_TO_VOLT(m_VCCPowerSupplyValue);
        float Volt_3_3 = CONVERT_3_3_A2D_TO_VOLT(m_V_3_3_Voltage);
        float Volt_1_2 = CONVERT_1_2_A2D_TO_VOLT(m_V_1_2_Voltage);

        VppPowerSupplyEdit->Text        = IntToStr (m_VppPowerSupplyValue);
        VppPowerSupplyCEdit->Text       = FloatToStrF (VoltVpp, ffFixed, 4, 2);

        Heater24VPowerSupplyEdit->Text  = IntToStr (m_Heater24VPowerSupplyValue);
        Heater24VPowerSupplyCEdit->Text = FloatToStrF (Volt24V, ffFixed, 4, 2);

        VDDPowerSupplyEdit->Text        = IntToStr (m_VDDPowerSupplyValue);
        VDDPowerSupplyCEdit->Text       = FloatToStrF (VoltVdd, ffFixed, 4, 2);

        VCCPowerSupplyEdit->Text        = IntToStr (m_VCCPowerSupplyValue);
        VCCPowerSupplyCEdit->Text       = FloatToStrF (VoltVcc, ffFixed, 4, 2);

        V_3_3_PowerSupplyEdit->Text     = IntToStr (m_V_3_3_Voltage);
        V_3_3_PowerSupplyCEdit->Text    = FloatToStrF (Volt_3_3, ffFixed, 4, 2);

        V_1_2_PowerSupplyEdit->Text     = IntToStr (m_V_1_2_Voltage);
        V_1_2_PowerSupplyCEdit->Text    = FloatToStrF (Volt_1_2, ffFixed, 4, 2);
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SoftwareResetButtonClick(TObject *Sender)
{
    try
    {
        TOHDBSWResetMessage Msg;

        Msg.MessageID = OHDB_SW_RESET;

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetHeadsVacuumParamsMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_ResetSoftwareEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetHeadsVacuumParamsMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::ResetDriverCircuitButtonClick(
    TObject *Sender)
{
    try
    {
        int TransactionHandle;
        QLib::TQWaitResult WaitResult;

        bool ResetDriverCircuit = (ResetDriverCircuitCheckBox->Checked ? 1 : 0);
        bool ResetDriverStateMachine = (ResetDriverStateMachineCheckBox->Checked ? 1 : 0);
        bool ApplyDefaultPrintParams = (DefaultPrintParamsCheckBox->Checked ? 1 : 0);

        if (ResetDriverCircuit)
        {
            TOHDBGenericMessage Msg;

            Msg.MessageID = OHDB_RESET_DRIVER_CIRCUIT;

            // (zohar) TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGenericMessage));

            // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // Wait for reply
            // (zohar) WaitResult = m_ResetDriverCircuitEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

            if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                    sizeof(TOHDBGenericMessage),
                    AckHandler,
                    reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

            {
                // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
                MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
            }
        }


        if (ResetDriverStateMachine)
        {
            TOHDBGenericMessage Msg;

            Msg.MessageID = OHDB_RESET_DRIVER_STATE_MACHINE;

            // (zohar) TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGenericMessage));

            // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // Wait for reply
            // (zohar) WaitResult = m_ResetDriverStateMachineEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

            if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                    sizeof(TOHDBGenericMessage),
                    AckHandler,
                    reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

            {
                // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
                MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
            }
        }

        if (ApplyDefaultPrintParams)
        {
            TOHDBGenericMessage Msg;

            Msg.MessageID = OHDB_APPLY_DEFAULT_PRINT_PARAMS;

            // (zohar) TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGenericMessage));

            // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // Wait for reply
            // (zohar) WaitResult = m_ApplyDefaultPrintingParamsEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

            if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                    sizeof(TOHDBGenericMessage),
                    AckHandler,
                    reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

            {
                // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
                MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
            }
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetPrintingPassParametersButtonClick(
    TObject *Sender)
{
    try
    {
        TOHDBSetPrintingPassParams Msg;

        Msg.MessageID       = OHDB_SET_PRINTING_PASS_PARAMS;
        Msg.StartPEG        = (WORD) StrToInt (StartPegParamEdit->Text);
        Msg.EndPEG          = (WORD) StrToInt (EndPegParamEdit->Text);
        Msg.NoOfFires       = (WORD) StrToInt (NumberOfFireParamEdit->Text);
        Msg.BumperStartPEG  = (WORD) StrToInt (BumperStartPegParamEdit->Text);
        Msg.BumperEndPEG    = (WORD) StrToInt (BumperEndPegParamEdit->Text);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetPrintingPassParams));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetPrintingPassParamsEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetPrintingPassParams),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::EnablePrintingCircuitButtonClick(
    TObject *Sender)
{
    try
    {
        QLib::TQWaitResult WaitResult;
        int TransactionHandle;

        bool OnOff = (EnablePrintingCircuitRadioButton->Checked ? 1 : 0);
        if (OnOff)
        {
            TOHDBGo Msg;

            Msg.MessageID = OHDB_ENABLE_PRINTING_CIRCUIT;

            // (zohar) TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGo));

            // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // Wait for reply
            // (zohar) WaitResult = m_EnablePrintingCircuitEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

            m_ProtocolClient->SendInstallWaitReply(&Msg,
                                                   sizeof(TOHDBGo),
                                                   AckHandler,
                                                   reinterpret_cast<TGenericCockie>(this));

        }
        else
        {
            TOHDBStop Msg;

            Msg.MessageID = OHDB_DISABLE_PRINTING_CIRCUIT;

            // (zohar) TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBStop));

            // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

            // Wait for reply
            // (zohar) WaitResult = m_DisablePrintingCircuitEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

            m_ProtocolClient->SendInstallWaitReply(&Msg,
                                                   sizeof(TOHDBStop),
                                                   AckHandler,
                                                   reinterpret_cast<TGenericCockie>(this));

        }

        if (WaitResult != QLib::wrSignaled)
        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }

}
//---------------------------------------------------------------------------


void __fastcall TOHDBTesterForm::FireAllButtonClick(TObject *Sender)
{
    try
    {
        TOHDBFireAllMessage Msg;

        Msg.MessageID = OHDB_FIRE_ALL;
        Msg.Frequency = (WORD) StrToInt(FireAllFrequencyEdit->Text);
        Msg.Time      = (WORD) StrToInt(FireAllTimeEdit->Text);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBFireAllMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_FireAllEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBFireAllMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }

}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::GetFireAllStatusButtonClick(
    TObject *Sender)
{
    TOHDBGetFireAllStatusMessage Msg;
    Msg.MessageID = OHDB_GET_FIRE_ALL_STATUS;
    // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBGetFireAllStatusMessage));

    // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,StatusHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) TQWaitResult WaitResult = m_FireAllStatusEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBGetFireAllStatusMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
        // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        FireAllStatusEdit->Text  = IntToStr (m_FireAllStatus);
        FireAllCounterEdit->Text = IntToStr (m_FireAllCounter);
    }


}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetBumperSensitivityButtonClick(
    TObject *Sender)
{
    try
    {
        TOHDBSetRollerBumperParamsMessage Msg;
        CAppParams *ParamsMgr = CAppParams::GetInstance();

        Msg.MessageID                                               = OHDB_SET_BUMPER_PARAMS;
        ParamsMgr->BumperSensitivity   = Msg.Sensitivity            = (WORD) StrToInt(SetBumperSensitivityEdit->Text);
        ParamsMgr->BumperResetTime     = Msg.ResetTime              = (WORD) StrToInt(SetBumperResetTimeEdit->Text);
        ParamsMgr->BumperImpactCounter = Msg.ImpactCountBeforeError = (BYTE) StrToInt(SetBumperImpactCounterEdit->Text);

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBSetRollerBumperParamsMessage));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetBumperSensitivityEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBSetRollerBumperParamsMessage),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }


}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetBumperOnOffButtonClick(TObject *Sender)
{
    try
    {
        bool OnOff = (SetBumperOnRadioButton->Checked ? 1 : 0);
        TOHDBImpactBumperOnOff Msg;

        if (OnOff)
        {
            // Loading the "Unbumped" Bitmap to the Bumper Tab.
            BumperLedsImageList->GetBitmap (BUMPER_ENABLED,BumperOnOffLedImage->Picture->Bitmap);
            BumperOnOffLedImage->Refresh();
            BumperStatusLabel->Caption = "Status: Enabled";
        }
        else
        {
            // Loading the "Unbumped" Bitmap to the Bumper Tab.
            BumperLedsImageList->GetBitmap (BUMPER_DISABLED,BumperOnOffLedImage->Picture->Bitmap);
            BumperOnOffLedImage->Refresh();
            BumperStatusLabel->Caption = "Status: Disabled";
        }

        Msg.MessageID = OHDB_SET_BUMPER_IMPACT_ON_OFF;
        Msg.Impact    = (BYTE) OnOff;

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(TOHDBImpactBumperOnOff));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetBumperOnOffEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(TOHDBImpactBumperOnOff),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }

    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);

    }


}
//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::BumperStatusResetButtonClick(
    TObject *Sender)
{
    // Loading the "Unbumped" Bitmap to the Bumper Tab.
    BumperLedsImageList->GetBitmap (BUMPER_ENABLED,BumperOnOffLedImage->Picture->Bitmap);
    BumperOnOffLedImage->Refresh();
    BumperStatusLabel->Caption = "Status: Enabled";

    // Enabling the BumperStatusResetButton button only after a bump.
    BumperStatusResetButton->Enabled = false;
}
//---------------------------------------------------------------------------


void TOHDBTesterForm::BumperRcvHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    TOHDBTesterForm *InstancePtr = reinterpret_cast<TOHDBTesterForm *>(Cockie);

    // post a message to the dialog to display the current heads volategs
    PostMessage(InstancePtr->Handle,WM_OHDB_BUMPER_NOTIFICATION,reinterpret_cast<int> (0), reinterpret_cast<int> (0));

}
//---------------------------------------------------------------------------

MESSAGE void TOHDBTesterForm::BumperNotificationDisplay(TMessage& Message)
{
    // Loading the "Unbumped" Bitmap to the Bumper Tab.
    BumperLedsImageList->GetBitmap (BUMPER_ENABLED_BUMPED,BumperOnOffLedImage->Picture->Bitmap);
    BumperOnOffLedImage->Refresh();
    BumperStatusLabel->Caption = "Status: Bumped";

    BumperStatusResetButton->Enabled = true;
}
//---------------------------------------------------------------------------


void TOHDBTesterForm::NotificationsRcvHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    TOHDBTesterForm *InstancePtr = reinterpret_cast<TOHDBTesterForm *>(Cockie);
    int MoreInfo = 0;

    // Build the read data message
    TOHDBGenericMessage *Msg = static_cast<TOHDBGenericMessage *>(Data);

    if (Msg->MessageID == OHDB_WAKE_UP_NOTIFICATION)
    {
        TOHDBWakeUpNotificationResponse *WakeUpNotificationResponse = static_cast<TOHDBWakeUpNotificationResponse *>(Data);

        MoreInfo = WakeUpNotificationResponse->WakeUpReason;
    }

    // post a message to the dialog to display the current heads volategs
    PostMessage(InstancePtr->Handle,WM_OHDB_OTHER_NOTIFICATION, Msg->MessageID, MoreInfo);
}

//---------------------------------------------------------------------------

void __fastcall TOHDBTesterForm::SetBumperPegOnOffButtonClick(
    TObject *Sender)
{
    try
    {
        bool OnOff = (SetBumperPegOnRadioButton->Checked ? 1 : 0);
        TOHDBSetBumperPegOnOff Msg;

        Msg.MessageID = OHDB_SET_BUMPER_PEG_ON_OFF;
        Msg.OnOff    = (BYTE) OnOff;

        // (zohar) int TransactionHandle = m_ProtocolClient->Send(&Msg,sizeof(OHDB_SET_BUMPER_PEG_ON_OFF));

        // (zohar) m_ProtocolClient->InstallReceiveHandler(TransactionHandle,AckHandler,reinterpret_cast<TGenericCockie>(this));

        // Wait for reply
        // (zohar) TQWaitResult WaitResult = m_SetBumperPegOnOffEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

        if (m_ProtocolClient->SendInstallWaitReply(&Msg,
                sizeof(OHDB_SET_BUMPER_PEG_ON_OFF),
                AckHandler,
                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

        {
            // (zohar) m_ProtocolClient->UnInstallReceiveHandler(TransactionHandle);
            MessageDlg("Tester did not recieve Ack",mtError,TMsgDlgButtons() << mbOK,0);
        }
    }
    catch (Exception &exception)
    {
        MessageDlg("The value entered is not valid",mtError,TMsgDlgButtons() << mbOK,0);
    }
}
//---------------------------------------------------------------------------

// Display Other notification
MESSAGE void TOHDBTesterForm::OtherNotificationDisplay(TMessage& Message)
{
    int MessageID = (int) Message.WParam;
    int MoreInfo  = (int) Message.LParam;

    AnsiString OutputString = "Init";
    AnsiString MoreString   = "Init";
    TMsgDlgType DlgType = mtConfirmation	;

    switch (MessageID)
    {
    case OHDB_WAKE_UP_NOTIFICATION:
    {
        if (MoreInfo)
        {
            MoreString = "Restart";
        }
        else
        {
            MoreString = "Power Up";
        }
        OutputString = "The OHDB Processor Software successfully reset (" + MoreString + ").";
        DlgType      = mtInformation;
        break;
    }
    case OHDB_ROLLER_ERROR:
    {
        OutputString = "Roller Error.";
        DlgType      = mtError;
        break;
    }
    case OHDB_HEAD_HEATER_ERROR:
    {
        OutputString = "Heater Error.";
        DlgType      = mtError;
        break;
    }
    case OHDB_HEAD_HEATER_ARE_ON_OFF:
    {
        OutputString = "The OHDB Processor Software successfully reset.";
        DlgType      = mtInformation;
        break;
    }
    case OHDB_FIRE_ALL_DONE:
    {
        OutputString = "Fire All: Done!";
        DlgType      = mtInformation;
        break;
    }
    case OHDB_HEADS_DATA_NOT_VALID:
    {
        OutputString = "Heads Data is not valid.";
        DlgType      = mtWarning;
        break;
    }
    case OHDB_HEADS_DATA_WRITE_ERROR:
    {
        OutputString = "Heads Data wite error.";
        DlgType      = mtError;
        break;
    }
    }

    MessageDlg(OutputString, DlgType, TMsgDlgButtons() << mbOK,0);

}

void __fastcall TOHDBTesterForm::VerifyHeadDataButtonClick(TObject *Sender)
{
    bool GetOK = GetAndDisplayHeadsData ();
    AnsiString ValueInfo;

    // If the GetAndDisplayHeadsData function succeeded getting head's data
    if (GetOK == true)
    {
        AnsiString  VerifyResult = "";
        TMsgDlgType DlgType      = mtInformation;


        // Check if the head contains the chracterization data
        if (m_HeadDataValidity == NOT_VALID)
        {
            VerifyResult += "The Data is not valid.\n";
            DlgType       = mtError;
        }

        // Check if the Support 10KHz Gain is between the normal values
        if (m_HeadData.Support10KHzLineGain < MIN_10KHZ_GAIN)
        {
            ValueInfo     = FloatToStrF (MIN_10KHZ_GAIN, ffFixed, 4, 2);
            VerifyResult += "Support 10KHz Line Gain value is below normal.";
            VerifyResult += " (Value should be above " + ValueInfo + ")\n";
            DlgType       = mtError;
        }
        else if (m_HeadData.Support10KHzLineGain > MAX_10KHZ_GAIN)
        {
            ValueInfo     = FloatToStrF (MAX_10KHZ_GAIN, ffFixed, 4, 2);
            VerifyResult += "Support 10KHz Line Gain value is above normal.";
            VerifyResult += " (Value should be below " + ValueInfo + ")\n";
            DlgType       = mtError;
        }


        // Check if the Model 10KHz Gain is between the normal values
        if (m_HeadData.Model10KHzLineGain < MIN_10KHZ_GAIN)
        {
            ValueInfo     = FloatToStrF (MIN_10KHZ_GAIN, ffFixed, 4, 2);
            VerifyResult += "Model 10KHz Line Gain value is below normal.";
            VerifyResult += " (Value should be above " + ValueInfo + ")\n";
            DlgType      = mtError;
        }
        else if (m_HeadData.Model10KHzLineGain > MAX_10KHZ_GAIN)
        {
            ValueInfo     = FloatToStrF (MAX_10KHZ_GAIN, ffFixed, 4, 2);
            VerifyResult += "Model 10KHz Line Gain value is above normal.";
            VerifyResult += " (Value should be below " + ValueInfo + ")\n";
            DlgType       = mtError;
        }


        // Check if the A/D value for 60°C is between the normal values
        if (m_HeadData.A2DValueFor60C < MIN_A2D_VALUE_FOR_60C)
        {
            VerifyResult += "A/D value for 60°C is below normal.";
            ValueInfo     = FloatToStrF (MIN_A2D_VALUE_FOR_60C, ffFixed, 5, 0);
            VerifyResult += " (Value should be above " + ValueInfo + ")\n";
            DlgType      = mtError;
        }
        else if (m_HeadData.A2DValueFor60C > MAX_A2D_VALUE_FOR_60C)
        {
            VerifyResult += "A/D value for 60°C is above normal.";
            ValueInfo     = FloatToStrF (MAX_A2D_VALUE_FOR_60C, ffFixed, 5, 0);
            VerifyResult += " (Value should be below " + ValueInfo + ")\n";
            DlgType       = mtError;
        }


        // Check if the A/D value for 80°C is between the normal values
        if (m_HeadData.A2DValueFor80C < MIN_A2D_VALUE_FOR_80C)
        {
            VerifyResult += "A/D value for 80°C is below normal.";
            ValueInfo     = FloatToStrF (MIN_A2D_VALUE_FOR_80C, ffFixed, 5, 0);
            VerifyResult += " (Value should be above " + ValueInfo + ")\n";
            DlgType      = mtError;
        }
        else if (m_HeadData.A2DValueFor80C > MAX_A2D_VALUE_FOR_80C)
        {
            VerifyResult += "A/D value for 80°C is above normal.";
            ValueInfo     = FloatToStrF (MAX_A2D_VALUE_FOR_80C, ffFixed, 5, 0);
            VerifyResult += " (Value should be below " + ValueInfo + ")\n";
            DlgType       = mtError;
        }

        // If verify ok - display an ok message.
        if (DlgType == mtInformation)
        {
            VerifyResult = "Head Data OK.";
        }

        // Displaying Verify results.
        MessageDlg(VerifyResult, DlgType, TMsgDlgButtons() << mbOK,0);

    }
}
//---------------------------------------------------------------------------

bool TOHDBTesterForm::GetAndDisplayHeadsData ()
{
    TOHDBGetHeadDataMsg Msg;

    Msg.MessageID = OHDB_GET_HEAD_DATA;
    Msg.HeadNum   = HeadNumComboBox->ItemIndex;

    if(m_ProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBGetHeadDataMsg), StatusHandler, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return false;
    }
    else
    {
        if(m_HeadNum != Msg.HeadNum)
        {
            MessageDlg("The head number received does not match the head number in the request",mtError,TMsgDlgButtons() << mbOK,0);
            return false;
        }

        SerialNumberEdit->Text         = ((const char *)(m_HeadData.SerialNumber));
        ProductionDateTimePicker->Date = (int)(m_HeadData.ProductionDate);
        XOffsetEdit->Text              = FloatToStrF(m_HeadData.XOffset,ffFixed,4,2);
        Model10KhzAEdit->Text          = FloatToStrF(m_HeadData.Model10KHzLineGain,ffFixed,4,2);

        if (Model10KhzAEdit->Text == "NAN")
            Model10KhzAEdit->Text = "N/A";

        Model10KhzBEdit->Text = FloatToStrF(m_HeadData.Model10KHzLineOffset,ffFixed,4,2);
        if (Model10KhzBEdit->Text == "NAN")
            Model10KhzBEdit->Text = "N/A";

        Model20KhzAEdit->Text = FloatToStrF(m_HeadData.Model20KHzLineGain,ffFixed,4,2);
        if (Model20KhzAEdit->Text == "NAN")
            Model20KhzAEdit->Text = "N/A";

        Model20KhzBEdit->Text = FloatToStrF(m_HeadData.Model20KHzLineOffset,ffFixed,4,2);
        if (Model20KhzBEdit->Text == "NAN")
            Model20KhzBEdit->Text = "N/A";

        Support10KhzAEdit->Text = FloatToStrF(m_HeadData.Support10KHzLineGain,ffFixed,4,2);
        if (Support10KhzAEdit->Text == "NAN")
            Support10KhzAEdit->Text = "N/A";

        Support10KhzBEdit->Text = FloatToStrF(m_HeadData.Support10KHzLineOffset,ffFixed,4,2);
        if (Support10KhzBEdit->Text == "NAN")
            Support10KhzBEdit->Text = "N/A";

        Support20KhzAEdit->Text = FloatToStrF(m_HeadData.Support20KHzLineGain,ffFixed,4,2);
        if (Support20KhzAEdit->Text == "NAN")
            Support20KhzAEdit->Text = "N/A";

        Support20KhzBEdit->Text = FloatToStrF(m_HeadData.Support20KHzLineOffset,ffFixed,4,2);
        if (Support20KhzBEdit->Text == "NAN")
            Support20KhzBEdit->Text = "N/A";

        A2DValuefor60CEdit->Text = IntToStr(m_HeadData.A2DValueFor60C);
        A2DValuefor80CEdit->Text = IntToStr(m_HeadData.A2DValueFor80C);


        SerialNumberEdit->Font->Color   = clBlue;
        Support10KhzAEdit->Font->Color  = clBlue;
        Model20KhzAEdit->Font->Color    = clBlue;
        Support20KhzAEdit->Font->Color  = clBlue;
        Support20KhzBEdit->Font->Color  = clBlue;
        Support10KhzBEdit->Font->Color  = clBlue;
        Model10KhzAEdit->Font->Color    = clBlue;
        Model20KhzBEdit->Font->Color    = clBlue;
        Model10KhzBEdit->Font->Color    = clBlue;
        A2DValuefor80CEdit->Font->Color = clBlue;
        A2DValuefor60CEdit->Font->Color = clBlue;
		ProductionDateTimePicker->Color = clBlue;
        XOffsetEdit->Font->Color        = clBlue;

        HeadsDataCheckSumEdit->Text = m_HeadDataCheckSum;
        HeadsDataValidityEdit->Text = m_HeadDataValidity;
    }

    return true;
}



void __fastcall TOHDBTesterForm::GetHeadDataButtonClick(TObject *Sender)
{
    GetAndDisplayHeadsData ();

}
//---------------------------------------------------------------------------




void __fastcall TOHDBTesterForm::GetADVersionButtonClick(TObject *Sender)
{
    TOHDBA2DGetDriverSWVersionMessage Msg;
    Msg.MessageID = OHDB_GET_OHDB_A2D_SW_VERSION;

    if(m_ProtocolClient->SendInstallWaitReply(&Msg,	sizeof(TOHDBA2DGetDriverSWVersionMessage),	StatusHandler, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        MessageDlg("Tester did not recieve the data",mtError,TMsgDlgButtons() << mbOK,0);
        return;
    }
    else
    {
        ADVersionPanel->Caption = QFormatStr("%d.%02d", m_ExternalA2DVersion, m_InternalA2DVersion).c_str();
    }
}
//---------------------------------------------------------------------------

void TOHDBTesterForm::ReadEncoderData()
{
    TOHDBXilinxReadMessage Msg;
	Msg.MessageID = OHDB_READ_FROM_XILINX;
	Msg.Address   = 0x80;              
    
	if(m_ProtocolClient->SendInstallWaitReply(&Msg, sizeof(TOHDBXilinxWriteMessage), ReadEncoderDataReceiveHandler, reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
		throw Exception("ReadEncoderData: Tester did not recieve Ack");                                                                                                        
}

void TOHDBTesterForm::ReadEncoderDataReceiveHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	TOHDBXilinxResponse *XilinxResponse = static_cast<TOHDBXilinxResponse *>(Data);
	TOHDBTesterForm     *InstancePtr    = reinterpret_cast<TOHDBTesterForm *>(Cockie);

	InstancePtr->m_EncoderData = XilinxResponse->Data;
}

void __fastcall TOHDBTesterForm::EncoderReadButtonClick(TObject *Sender)
{
	ReadEncoderData();
	EncoderValueEdit->Text = IntToHex(m_EncoderData, 4);
}
//---------------------------------------------------------------------------

