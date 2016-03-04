//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "OCBSimulatorDlg.h"
#include "EdenProtocolClient.h"
#include "EdenProtocolClientIDs.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "OCBCommDefs.h"
#include "TesterApplication.h"
#include "QTimer.h"
#include "QThreadUtils.h"
#include <IniFiles.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


const char NOTIFICATIONS_INI_FILE[] = "OCBNotifications.ini";

TOCBSimulatorForm *OCBSimulatorForm;
//---------------------------------------------------------------------------
__fastcall TOCBSimulatorForm::TOCBSimulatorForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TOCBSimulatorForm::Open(CEdenProtocolEngine *ProtocolEngine)
{
  if (m_ProtocolClient == NULL)
  {
    m_ProtocolEngine = ProtocolEngine;
    COCBSimProtocolClient::Init(m_ProtocolEngine);
    m_ProtocolClient = COCBSimProtocolClient::Instance();
    m_ProtocolClient->InstallReceiveHandler(0,255,AllMessageCallBack,(TGenericCockie)this);
    m_NotificationsThread = new CNotificationsThread(&m_NotificationList, m_ProtocolClient);
  }


  Show();
}
//---------------------------------------------------------------------------


void TOCBSimulatorForm::AllMessageCallBack(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie InstancePtr)
{
  TOCBSimulatorForm *Dlg = reinterpret_cast<TOCBSimulatorForm *>(InstancePtr);
  TOCBGenericMessage *Message = static_cast<TOCBGenericMessage *>(Data);

  // if the message is a ping message send a ping back
  if (Message->MessageID == OCB_PING)
  {
    TOCBPingMessage Ping;
    Ping.MessageID = OCB_PING;
    Dlg->m_ProtocolClient->SendNotificationAck(TransactionID,&Ping, sizeof(TOCBPingMessage));
    return;
  }

  Dlg->HandleMessage(Data, DataLength, TransactionID);

}
//---------------------------------------------------------------------------


void __fastcall TOCBSimulatorForm::FormDestroy(TObject *Sender)
{
  if (m_ProtocolClient)
    COCBSimProtocolClient::DeInit();
    

  if (m_MessageQueue)
		Q_SAFE_DELETE(m_MessageQueue);

  if (m_MsgLogQueue)
		Q_SAFE_DELETE(m_MsgLogQueue);

  if (m_OCBSimulator)
		Q_SAFE_DELETE(m_OCBSimulator);

  if (m_NotificationsThread)
		Q_SAFE_DELETE(m_NotificationsThread);
}
//---------------------------------------------------------------------------


void TOCBSimulatorForm::HandleMessage(PVOID Data,unsigned DataLength, int TransactionID)
{
  // insert the message to the message queue
  m_MessageQueue->Send(Data, DataLength);

  // post a message to inform the dialog to handle the message
  PostMessage(this->Handle,WM_OCB_SIM_MESSAGE_IN_Q, reinterpret_cast<unsigned>(DataLength), reinterpret_cast<int> (TransactionID));
}
//---------------------------------------------------------------------------

void __fastcall TOCBSimulatorForm::FormCreate(TObject *Sender)
{
  m_MessageQueue = new CQSimpleQueue(MAX_NUM_0F_MESSAGES,MAX_MSG_LENGTH);
  m_MsgLogQueue = new CQSimpleQueue(MAX_NUM_0F_MESSAGES,MAX_MSG_LENGTH);
  m_OCBSimulator = new COCBSimulator;

  InitializeNotificationMessageInfo();

}
//---------------------------------------------------------------------------


void TOCBSimulatorForm::DoHandleMessage(TMessage& Message)
{
  unsigned DataLength = Message.WParam;
  int TransactionID = Message.LParam;
  BYTE *Data = new BYTE[DataLength];

  m_MessageQueue->Receive(Data, DataLength);

  // display the message in the message log
  DisplayMessageInMessageLog(Data, DataLength, RECEIVED);

  // decode the message and display it in the right edit box
  DecodeAndDisplayMessage(Data);

  // reply to the message with the default reply for this message or
  // with the reply the user choosed
  ReplyToMessage(Data, TransactionID);

	delete []Data;

}
//---------------------------------------------------------------------------

// decode the message and display it in the right edit box
void TOCBSimulatorForm::DecodeAndDisplayMessage(PVOID Data)
{
  TEdit *EditBox = NULL;
  AnsiString MessageString;
  TOCBGenericMessage *Message = static_cast<TOCBGenericMessage *>(Data);

  // find the component with tag value equals to the message id
  EditBox = dynamic_cast<TEdit *>(FindComponent(EDIT_BOX, Message->MessageID, RECIEVED_SUB_TAG));

  if (EditBox != NULL)
  {
    switch(Message->MessageID)
    {
      case OCB_SET_TRAY_ON_OFF:
      {
        TOCBSetTrayOnOffMessage *Message = static_cast<TOCBSetTrayOnOffMessage *>(Data);
        if (Message->TrayHeatingOnOff == OFF)
          MessageString = "Off";
        else if (Message->TrayHeatingOnOff == ON)
          MessageString = "On";
        break;
      }
      case OCB_SET_TRAY_TEMPERATURE:
      {
        TOCBSetTrayTemperatureMessage *Message = static_cast<TOCBSetTrayTemperatureMessage *>(Data);
        m_OCBSimulator->SetTrayTemperature(*Message);
        MessageString = IntToStr(Message->RequiredTrayTemperature) + " " + IntToStr(Message->ActiveMargine) + " " + IntToStr(Message->TraySetTimeout);
        break;
      }
      case OCB_GET_TRAY_STATUS:
        MessageString = "No data";
        break;

      case OCB_IS_TRAY_INSERTED:
        MessageString = "No data";
        break;

      case OCB_SET_POWER_ON_OFF:
      {
        TOCBSetPowerOnOffMessage *Message = static_cast<TOCBSetPowerOnOffMessage *>(Data);
        m_OCBSimulator->SetPowerOnOff(*Message);
        if (Message->PowerOnOff == OFF)
          MessageString = "Power Off";
        else if (Message->PowerOnOff == ON)
          MessageString = "Power On";
        break;
      }
      case OCB_GET_POWER_STATUS:
        MessageString = "No data";
        break;
      case OCB_SET_POWER_PARMS:
      {
        TOCBSetPowerParamsMessage *Message = static_cast<TOCBSetPowerParamsMessage *>(Data);
        MessageString = IntToStr(Message->OnDelay) + " " + IntToStr(Message->OffDelay);
        break;
      }
      case OCB_SET_UV_LAMPS_ON_OFF:
      {
        TOCBUvLampOnOffMessage *Message = static_cast<TOCBUvLampOnOffMessage *>(Data);
        m_OCBSimulator->SetUvLampsOnOff(*Message);
        if (Message->OnOff == OFF)
          MessageString = "Lamps Off";
        else if (Message->OnOff == ON)
          MessageString = "Lamps On";
        break;
      }
      case OCB_SET_UV_LAMPS_PARAMS:
      {
        TOCBSetUvParamsMessage *Message = static_cast<TOCBSetUvParamsMessage *>(Data);
        MessageString = IntToStr(Message->IgnitionTimeout);
        break;
      }
      case OCB_GET_UV_LAMPS_STATUS:
        MessageString = "No data";
        break;
      case OCB_PERFORM_PURGE:
      {
        TOCBPerformPurgeMessage *Message = static_cast<TOCBPerformPurgeMessage *>(Data);
//        MessageString = IntToStr(Message->PurgeTime) + " " + IntToStr(Message->PressureBuildTime);
        break;
      }
      case OCB_GET_PURGE_STATUS:
        MessageString = "No data";
        break;
      case OCB_AMBIENT_TEMPERATURE_ON_OFF:
      {
        TOCBAmbientTemperatureOnOffMessage *Message = static_cast<TOCBAmbientTemperatureOnOffMessage *>(Data);
        if (Message->AmbientTemperatureOnOff == OFF)
          MessageString = "Off";
        else if (Message->AmbientTemperatureOnOff == ON)
          MessageString = "On";
        break;
      }
      case OCB_SET_AMBIENT_TEMPERATURE_LEVEL:
      {
        TOCBSetAmbientTemperatureParamsMessage *Message = static_cast<TOCBSetAmbientTemperatureParamsMessage *>(Data);
        m_OCBSimulator->SetAmbientTemperatureLevel(*Message);
        MessageString = IntToStr(Message->AmbientTemperature) + " " + IntToStr(Message->LowAmbientTemperature) + " " + IntToStr(Message->HighAmbientTemperature);
        break;
      }
      case OCB_GET_AMBIENT_TEMPERATURE_STATUS:
        MessageString = "No data";
        break;
      case OCB_TIMED_SET_ACTUATOR_ON_OFF:
      {
        TOCBTimedSetActuatorOnOffMessage *Message = static_cast<TOCBTimedSetActuatorOnOffMessage *>(Data);
        MessageString = IntToStr(Message->ActuatorNumber);
        if (Message->ActuatorLevel == OFF)
          MessageString += " Off";
        else if (Message->ActuatorLevel == ON)
          MessageString += " On";
        MessageString += IntToStr(Message->Time);
        break;

      }
      case OCB_ACTIVATE_SUCTION_SYSTEM:  //RSS, itamar
      {
        TOCBActivateRollerSuctionSystemMessage *Message = static_cast<TOCBActivateRollerSuctionSystemMessage *>(Data);
        MessageString = IntToStr(Message->OnOff);
        MessageString += IntToStr(Message->ValveOnTime);
        MessageString += IntToStr(Message->ValveOffTime);
        break;
      }
      case OCB_SET_ACTUATOR_ON_OFF:
      {
        TOCBSetActuatorOnOffMessage *Message = static_cast<TOCBSetActuatorOnOffMessage *>(Data);
        MessageString = IntToStr(Message->ActuatorID);
        if (Message->OnOff == OFF)
          MessageString += " Off";
        else if (Message->OnOff == ON)
          MessageString += " On";
        break;
      }
      case OCB_GET_ACTUATOR_STATUS:
        MessageString = "No data";
        break;
      case OCB_GET_INPUTS_STATUS:
        MessageString = "No data";
        break;
      case OCB_SET_EOL_ON_OFF:
      {
        TOCBSetEOLOnOffMessage *Message = static_cast<TOCBSetEOLOnOffMessage *>(Data);
        m_OCBSimulator->SetEolOnOff(*Message);
        if (Message->EOLOnOff == OFF)
          MessageString = " Off";
        else if (Message->EOLOnOff == ON)
          MessageString = " On";
        break;
      }
      case OCB_SET_EOL_PARAMS:
      {
/*
        TOCBSetEOLParamsMessage *Message = static_cast<TOCBSetEOLParamsMessage *>(Data);
        m_OCBSimulator->SetEolParams(*Message);
        MessageString = IntToStr(Message->ModelEndLowLimit) + " " + IntToStr(Message->ModelStartHighLimit) + " " +
                        IntToStr(Message->ModelDeltaOk) + " " + IntToStr(Message->ModelDeltaCritical) + " " +
                        IntToStr(Message->ModelSuspendCycles) + " " + IntToStr(Message->SupportEndLowLimit) + " " +
                        IntToStr(Message->SupportStartHighLimit) + " " + IntToStr(Message->SupportDeltaOk) + " " +
                        IntToStr(Message->SupportDeltaCritical) + " " + IntToStr(Message->SupportSuspendCycles);
        break;
*/        
      }
      case OCB_GET_EOL_STATUS:
        MessageString = "No data";
        break;
      case OCB_HEADS_FILLING_CONTROL_ON_OFF:
      {
        TOCBHeadFillingControlOnOffMessage *Message = static_cast<TOCBHeadFillingControlOnOffMessage *>(Data);
        m_OCBSimulator->SetHeadsFillingControlOnOff(*Message);
        if (Message->HeadFillingControlOnOff == OFF)
          MessageString = " Off";
        else if (Message->HeadFillingControlOnOff == ON)
          MessageString = " On";
        break;
      }
      case OCB_SET_HEADS_FILLING_PARAMS:
      {
        TOCBSetHeadFillingParamsMessage *Message = static_cast<TOCBSetHeadFillingParamsMessage *>(Data);
        m_OCBSimulator->SetHeadsFillingParams(*Message);
/*        if (Message->MaterialType == MODEL)
          MessageString = "Model ";
        else if (Message->MaterialType == SUPPORT)
          MessageString = "Support ";
        MessageString += IntToStr(Message->ThermistorFullLowThreshold) + " " + IntToStr(Message->ThermistorFullHighThreshold)+ " " +
                         IntToStr(Message->ThermistorHalfFullLowThreshold) + " " + IntToStr(Message->ThermistorHalfFullHighThreshold);
*/
        break;
      }
      case OCB_GET_HEADS_FILLING_STATUS:
      {
        TOCBGetHeadFillingStatusMessage *Message = static_cast<TOCBGetHeadFillingStatusMessage *>(Data);
/*        if (Message->MaterialType == MODEL)
          MessageString = "Model";
        else if (Message->MaterialType == SUPPORT)
          MessageString = "Support";
*/
        break;
      }
      case OCB_LOCK_DOOR:
        m_OCBSimulator->SetDoorStatus(OCB_DOOR_LOCK,OCB_DOOR_CLOSE_LOCKED);
        MessageString = "No data";
        break;

      case OCB_UNLOCK_DOOR:
        m_OCBSimulator->SetDoorStatus(OCB_DOOR_UNLOCK,OCB_DOOR_CLOSE_UNLOCKED);
        MessageString = "No data";
        break;

      case OCB_GET_DOOR_STATUS:
        MessageString = "No data";
        break;

      default:
        // error
        break;
    }
  EditBox->Text = MessageString;
  }
}
//---------------------------------------------------------------------------

// reply to the message with the default reply for this message or
// with the reply the user choosed
void TOCBSimulatorForm::ReplyToMessage(PVOID Data, int TransactionID)
{
  TComboBox *ComboBox = NULL;
  TCheckBox *CheckBox = NULL;
  TOCBGenericMessage *Message = static_cast<TOCBGenericMessage *>(Data);

  ComboBox = dynamic_cast<TComboBox *>(FindComponent(COMBO_BOX, Message->MessageID, TRANSMIT_SUB_TAG));

  CheckBox = dynamic_cast<TCheckBox *>(FindComponent(CHECK_BOX, Message->MessageID, CHANGE_SUB_TAG));

  if (ComboBox != NULL && CheckBox != NULL) // found the components
  {
    if(CheckBox->Checked) // if the 'change value' option is on
      SendReplyMessage(Message->MessageID,Data,ComboBox->Text, TransactionID);

    else
      SendReplyMessage(Message->MessageID,Data,ComboBox->Items->Strings[0], TransactionID); // if the 'change value' option is off
  }                                                                         // send the default reply, which is the first item in the combo box
}

//---------------------------------------------------------------------------

// send a reply according to the message id of the recieved message, and the reply the user choosed
void TOCBSimulatorForm::SendReplyMessage(BYTE MessageId, PVOID Data, AnsiString Reply, int TransactionID)
{
  switch(MessageId)
  {
    case OCB_SET_TRAY_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_TRAY_TEMPERATURE:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_TRAY_STATUS:
    {
      if (Reply == "TrayStatus")
      {
        TOCBTrayStatusResponse TrayStatus = m_OCBSimulator->GetTrayStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&TrayStatus, sizeof(TOCBTrayStatusResponse));
        DisplayMessageInMessageLog(&TrayStatus, sizeof(TOCBTrayStatusResponse), TRANSMITED);
      }
      break;
    }
    case OCB_IS_TRAY_INSERTED:
    {
      TOCBTrayInStatusResponse TrayInStatus;
      TrayInStatus.MessageID = OCB_TRAY_IN_STATUS;
      if (Reply == "TrayInStatus - In")
        TrayInStatus.TrayExistense = 1;
      else if (Reply == "TrayInStatus - Out")
        TrayInStatus.TrayExistense = 0;

      m_ProtocolClient->SendNotificationAck(TransactionID,&TrayInStatus, sizeof(TOCBTrayInStatusResponse));
      DisplayMessageInMessageLog(&TrayInStatus, sizeof(TOCBTrayInStatusResponse), TRANSMITED);
    }
    
    case OCB_SET_POWER_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
       {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_POWER_STATUS:
    {
      if (Reply == "PowerStatus")
      {
        TOCBPowerStatusResponse PowerStatus = m_OCBSimulator->GetPowerStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&PowerStatus, sizeof(TOCBPowerStatusResponse));
        DisplayMessageInMessageLog(&PowerStatus, sizeof(TOCBPowerStatusResponse), TRANSMITED);

      }
      break;
    }
    case OCB_SET_POWER_PARMS:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
       {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_UV_LAMPS_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
       {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_UV_LAMPS_PARAMS:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_UV_LAMPS_STATUS:
    {
      if (Reply == "UvLampsStatus")
      {
        TOCBUvLampStatusResponse LampsStatus = m_OCBSimulator->GetUvLampsStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&LampsStatus, sizeof(TOCBUvLampStatusResponse));
        DisplayMessageInMessageLog(&LampsStatus, sizeof(TOCBUvLampStatusResponse), TRANSMITED);
      }
      break;
    }
    case OCB_PERFORM_PURGE:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_PURGE_STATUS:
    {
      if (Reply == "PurgeStatus - Idle")
      {
        TOCBPurgeStatusResponse PurgeStatus = m_OCBSimulator->GetPurgeStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&PurgeStatus, sizeof(TOCBPurgeStatusResponse));
        DisplayMessageInMessageLog(&PurgeStatus, sizeof(TOCBPurgeStatusResponse), TRANSMITED);
      }
      else if (Reply == "PurgeStatus - Active")
      {
        TOCBPurgeStatusResponse PurgeStatus = m_OCBSimulator->GetPurgeStatus();
        PurgeStatus.PurgeStatus = 1;  // active
        m_ProtocolClient->SendNotificationAck(TransactionID,&PurgeStatus, sizeof(TOCBPurgeStatusResponse));
        DisplayMessageInMessageLog(&PurgeStatus, sizeof(TOCBPurgeStatusResponse), TRANSMITED);
      }

      break;
    }
    case OCB_AMBIENT_TEMPERATURE_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_AMBIENT_TEMPERATURE_LEVEL:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_AMBIENT_TEMPERATURE_STATUS:
    {
      if (Reply == "AmbientTemperatureStatus")
      {
        TOCBAmbientTemperatureStatusResponse AmbientTemperature = m_OCBSimulator->GetAmbientTemperatureStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&AmbientTemperature, sizeof(TOCBAmbientTemperatureStatusResponse));
        DisplayMessageInMessageLog(&AmbientTemperature, sizeof(TOCBAmbientTemperatureStatusResponse), TRANSMITED);
      }
      break;
    }
    case OCB_TIMED_SET_ACTUATOR_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_ACTUATOR_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_ACTIVATE_SUCTION_SYSTEM:  //RSS, itamar
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_ACTUATOR_STATUS:
    {
      if (Reply == "ActuatorsStatus")
      {
        TOCBActuatorsStatusResponse ActuatorsStatus = m_OCBSimulator->GetActuatorsStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&ActuatorsStatus, sizeof(TOCBActuatorsStatusResponse));
        DisplayMessageInMessageLog(&ActuatorsStatus, sizeof(TOCBActuatorsStatusResponse), TRANSMITED);
      }
      break;
    }
    case OCB_GET_INPUTS_STATUS:
    {
      if (Reply == "InputsStatus")
      {
        TOCBInputsStatusResponse InputsStatus = m_OCBSimulator->GetInputsStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&InputsStatus, sizeof(TOCBInputsStatusResponse));
        DisplayMessageInMessageLog(&InputsStatus, sizeof(TOCBInputsStatusResponse), TRANSMITED);
      }
      break;
    }
    case OCB_SET_EOL_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_EOL_PARAMS:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_EOL_STATUS:
    {
      if (Reply == "EolStatus")
      {
/*
        TOCBEolStatusResponse EolStatus = m_OCBSimulator->GetEolStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&EolStatus, sizeof(TOCBEolStatusResponse));
        DisplayMessageInMessageLog(&EolStatus, sizeof(TOCBEolStatusResponse), TRANSMITED);
*/        
      }
      break;
    }
    case OCB_HEADS_FILLING_CONTROL_ON_OFF:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_SET_HEADS_FILLING_PARAMS:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_HEADS_FILLING_STATUS:
    {
      if (Reply == "HeadsFillingStatus")
      {
        TOCBSetHeadFillingParamsMessage *Message = static_cast<TOCBSetHeadFillingParamsMessage *>(Data);
//        TOCBHeadFillingStatusResponse HeadsFillingStatus = m_OCBSimulator->GetHeadsFillingStatus(Message->MaterialType);
//        m_ProtocolClient->SendNotificationAck(TransactionID,&HeadsFillingStatus, sizeof(TOCBHeadFillingStatusResponse));
//        DisplayMessageInMessageLog(&HeadsFillingStatus, sizeof(TOCBHeadFillingStatusResponse), TRANSMITED);
      }
      break;
    }
    case OCB_LOCK_DOOR:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_UNLOCK_DOOR:
    {
      TOCBAck Ack = {OCB_ACK, MessageId, OCB_Ack_Success};
      if (Reply == "Ack - Success")
      {
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
        SendNotification(MessageId);
      }
      else if (Reply == "Ack - Failure")
      {
        Ack.AckStatus = OCB_Ack_Failure;
        m_ProtocolClient->SendNotificationAck(TransactionID,&Ack, sizeof(TOCBAck));
        DisplayMessageInMessageLog(&Ack, sizeof(TOCBAck), TRANSMITED);
      }
      break;
    }
    case OCB_GET_DOOR_STATUS:
    {
      if (Reply == "DoorStatus")
      {
        TOCBDoorStatusResponse DoorStatus = m_OCBSimulator->GetDoorStatus();
        m_ProtocolClient->SendNotificationAck(TransactionID,&DoorStatus, sizeof(TOCBDoorStatusResponse));
        DisplayMessageInMessageLog(&DoorStatus, sizeof(TOCBDoorStatusResponse), TRANSMITED);
      }
      break;
    }
    default:
      // error
      break;
  }
}
//---------------------------------------------------------------------------


// sets the combo boxes and check boxes on a tab sheet to their default value
void __fastcall TOCBSimulatorForm::DefaultsBitBtnClick(TObject *Sender)
{
  TComboBox *ComboBox = NULL;
  TCheckBox *CheckBox = NULL;
  TBitBtn *DefaultsButton = dynamic_cast<TBitBtn *>(Sender);

  // get the parent of this button in order to find to which tab sheet it belongs
  TTabSheet *TabSheet = dynamic_cast<TTabSheet *> (DefaultsButton->Parent);

  for (int i = 0; i < this->ComponentCount; i++)  // find all the combo boxes and check boxes on the form
  {
    ComboBox = dynamic_cast<TComboBox *>(this->Components[i]);

    // check if this is a combo box and if it is on the same tab sheet as the button
    if (ComboBox != NULL && ComboBox->Parent == TabSheet)
    {
      ComboBox->ItemIndex = 0;  // set the item index for the first item, which is the default
      ComboBox = NULL;         // set to NULL for the next search
    }

    CheckBox = dynamic_cast<TCheckBox *>(this->Components[i]);

    // check if this is a check box and if it is on the same tab sheet as the button
    if (CheckBox != NULL && CheckBox->Parent == TabSheet)
    {
      CheckBox->Checked = false;  // set the check box to not checked, which is the defalt state
      CheckBox = NULL;            // set to NULL for the next search
    }

  }
}
//---------------------------------------------------------------------------

// clear the message log
void __fastcall TOCBSimulatorForm::ResetLogBitBtnClick(TObject *Sender)
{
  MessageLogMemo->Clear();
}
//---------------------------------------------------------------------------

void TOCBSimulatorForm::DisplayMessageInMessageLog(PVOID Message, unsigned Length, int Direction)
{
  // insert the message to the message log queue
  m_MsgLogQueue->Send(Message, Length);

  // post a message to display the message in the message log
  PostMessage(this->Handle,WM_OCB_SIM_MSG_LOG, Length, Direction);

}
//---------------------------------------------------------------------------

void TOCBSimulatorForm::DoDisplayMessageInMessageLog(TMessage& Message)
{
  unsigned Length = Message.WParam;
  int Direction = Message.LParam;
  BYTE *Data = new BYTE[Length];

  m_MsgLogQueue->Receive(Data, Length);

  QString Str;
  if ( Direction == RECEIVED)
    Str = "Recieved: ";
  else if ( Direction == TRANSMITED)
    Str = "Transmited: ";


  for(unsigned i = 0; i < Length; i++)
    Str += QFormatStr("%.2X ",((PBYTE)Data)[i]);

  MessageLogMemo->Lines->Add(Str.c_str());

	delete []Data;


}
//---------------------------------------------------------------------------

TComponent* TOCBSimulatorForm::FindComponent(TComponentsType Type, int Tag, TSubTagType SubTag)
{
  TComboBox* ComboBox = NULL;
  TCheckBox* CheckBox = NULL;
  TUpDown* UpDown = NULL;
  TEdit* Edit = NULL;

  Tag = Tag | (SubTag << 8);  // combine the tag and the subtag

  for (int i = 0; i < this->ComponentCount; i++)
  {
    if(this->Components[i]->Tag == Tag)
    {
      switch(Type)
      {
        case COMBO_BOX:
        {
          ComboBox = dynamic_cast<TComboBox *>(this->Components[i]);
          if (ComboBox != NULL)
            return ComboBox;
          break;
        }
        case CHECK_BOX:
        {
          CheckBox = dynamic_cast<TCheckBox *>(this->Components[i]);
          if (CheckBox != NULL)
            return CheckBox;
          break;
        }
        case EDIT_BOX:
        {
          Edit = dynamic_cast<TEdit *>(this->Components[i]);
          if (Edit != NULL)
            return Edit;
          break;
        }
        case UP_DOWN:
        {
          UpDown = dynamic_cast<TUpDown *>(this->Components[i]);
          if (UpDown != NULL)
            return UpDown;
          break;
        }
      }
    }
  }
  return NULL;
}
//---------------------------------------------------------------------------


// Initialization for the array of NotificationMessageInfo from the ini file
void TOCBSimulatorForm::InitializeNotificationMessageInfo()
{
  TComboBox* ComboBox = NULL;
  TUpDown* UpDown = NULL;


  TMemIniFile *IniFile = new TMemIniFile(QString(CTesterApplication::GetInstance()->AppFilePath.Value() + NOTIFICATIONS_INI_FILE).c_str());

  try
  {
    TStringList *Notifications = new TStringList();

    for (int i = MIN_OCB_MSG_NUMBER; i < MAX_OCB_MSG_NUMBER; i++)
    {
      IniFile->ReadSectionValues(i, Notifications);
      if (Notifications->Count == 0)
        continue;
      ComboBox = dynamic_cast<TComboBox *>(FindComponent(COMBO_BOX, i, NOTIFICATION_SUB_TAG));
      UpDown = dynamic_cast<TUpDown *>(FindComponent(UP_DOWN, i, NOTIFICATION_TIME_SUB_TAG));
      if (ComboBox == NULL || UpDown == NULL)
        continue;
      ComboBox->Items->Add("No Notification");
      ComboBox->ItemIndex = 0;
      for (int j = 0; j < Notifications->Count; j++)
      {
        ComboBox->Items->Add(Notifications->Names[j]);
        m_NotificationMessageInfo[i-100].MessageList.push_back(ConvertMessageToVector(Notifications->Values[Notifications->Names[j]].c_str()));
      }
      m_NotificationMessageInfo[i-100].ComboBox = ComboBox;
      m_NotificationMessageInfo[i-100].UpDown = UpDown;
      Notifications->Clear();
    }
  } __finally {
      Q_SAFE_DELETE(IniFile);
    }
}
//---------------------------------------------------------------------------


// convert a message string to a vector of bytes representing the message
TNotificationMessage TOCBSimulatorForm::ConvertMessageToVector(QString MessageString)
{
  TNotificationMessage Message;
  int StartIndex = 0,EndIndex = 0;
  QString Word;
  BYTE HexValue;

  do
  {
    StartIndex = MessageString.find_first_not_of(" \t\n",EndIndex);

    if(StartIndex == -1)
      break;

    EndIndex = MessageString.find_first_of(" \t\n",StartIndex);
    if(EndIndex == -1)
      EndIndex = MessageString.length();

    Word = MessageString.substr(StartIndex,EndIndex - StartIndex);

    HexValue = (BYTE)StrToInt(QString("$" + Word).c_str());
    Message.push_back(HexValue);

  } while((unsigned)EndIndex != MessageString.length());

  return Message;
}
//---------------------------------------------------------------------------

// send the notification the user selected at the selected time
void TOCBSimulatorForm::SendNotification(int MessageId)
{
  int ItemIndex = m_NotificationMessageInfo[MessageId-100].ComboBox->ItemIndex;
  if (ItemIndex == 0) // the user selected "no notification"
    return;
  ItemIndex--;  // item index 0 is "no notification"
  TSendNotificationType Notification;
  Notification.Data = (&(*((m_NotificationMessageInfo[MessageId-100].MessageList[ItemIndex]).begin())));
  Notification.DataLength = (m_NotificationMessageInfo[MessageId-100].MessageList[ItemIndex]).size();
  Notification.TimeToSend = m_NotificationMessageInfo[MessageId-100].UpDown->Position + QTicksToSeconds(QGetTicks());
  m_NotificationList.push_back(Notification);

}
//---------------------------------------------------------------------------


// send a notification without relevance to an incoming message
void __fastcall TOCBSimulatorForm::SendNotificationButtonClick(TObject *Sender)
{
  short UpDownValue;
  TButton *SendButton = dynamic_cast<TButton *>(Sender);
  TUpDown *UpDown = NULL;
  UpDown = dynamic_cast<TUpDown *>(FindComponent(UP_DOWN, SendButton->Tag, NOTIFICATION_TIME_SUB_TAG));

  if (UpDown == NULL)
    return;

  // send the notification immediately
  UpDownValue = UpDown->Position;
  UpDown->Position = 0;

  SendNotification(SendButton->Tag);

  // restore the value of the updown control
  UpDown->Position = UpDownValue;
}
//---------------------------------------------------------------------------

