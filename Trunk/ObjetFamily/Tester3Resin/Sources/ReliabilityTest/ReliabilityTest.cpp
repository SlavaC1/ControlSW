//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ReliabilityTest.h"
#include "QMonitor.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const int RED_LED_INDEX = 0;
const int GREEN_LED_INDEX = 1;

const int NUM_OF_HEATERS = 12;
const int NUM_OF_HEAD_HEATERS = 8;

const int HEATERS_ON = 1;
const int HEATERS_OFF = 0;
const int TEMPERATURE_TOLERANCE = 50; // ADu

const int SUPPORT_TYPE = 0;
const int MODEL_TYPE = 1;

const int FAN_ACTUATOR_NUM = 30;
const int PURGE_TIME = 1000;
const int PURGE_MODEL_AND_SUPPORT = 3;
const int PURGE_IDLE = 0;
const int PURGE_ACTIVE = 1;

const int MODEL_LOW_THRESHOLD = 1000;
const int MODEL_HIGH_THRESHOLD = 1020;
const int SUPPORT_LOW_THRESHOLD = 1120;
const int SUPPORT_HIGH_THRESHOLD = 1140;
const int PUMP_ON_TIME = 1;
const int PUMP_OFF_TIME = 5;
const int FILLING_TIMEOUT = 30;

const int PURGE_STATUS_CHECK_INTERVAL = 500;

TReliabilityTestForm *ReliabilityTestForm;
//---------------------------------------------------------------------------
__fastcall TReliabilityTestForm::TReliabilityTestForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TReliabilityTestForm::FormCreate(TObject *Sender)
{
  // load a red led glyph to the buttons
  Graphics::TBitmap *Glyph = new Graphics::TBitmap;
  ImageList1->GetBitmap(RED_LED_INDEX,Glyph);
  PurgeTestButton->Glyph = Glyph;
  TemperatueTestButton->Glyph = Glyph;

  // zero the heaters values (4095 ADu)
  for (int i = 0; i < NUM_OF_HEATERS; i++)
  {
    m_UpperLimitTempMsg.SetElements.HeaterTable[i] = 4095;
    m_LowerLimitTempMsg.SetElements.HeaterTable[i] = 4095;
  }
  m_UpperLimitTempMsg.MessageID = OHDB_SET_HEATERS_TEMPERATURE;
  m_LowerLimitTempMsg.MessageID = OHDB_SET_HEATERS_TEMPERATURE;

  m_LowLimitNow = false;
  m_TemperatureCycle = 0;
  m_PurgeCycle = 0;
  m_PurgeStatus = PURGE_ACTIVE;
}
//---------------------------------------------------------------------------


void TReliabilityTestForm::Open(CEdenProtocolEngine *OHDBProtocolEngine, CEdenProtocolEngine *OCBProtocolEngine)
{
  if (m_OHDBProtocolClient == NULL)
  {
    COHDBProtocolClient::Init(OHDBProtocolEngine);
    m_OHDBProtocolClient = COHDBProtocolClient::Instance();
  }

  if (m_OCBProtocolClient == NULL)
  {
    COCBProtocolClient::Init(OCBProtocolEngine);
    m_OCBProtocolClient = COCBProtocolClient::Instance();
    m_OCBProtocolClient->InstallMessageHandler(OCB_PURGE_ERROR, OCBHandler, reinterpret_cast<TGenericCockie>(this));
  }

  Show();
}

//---------------------------------------------------------------------------

void __fastcall TReliabilityTestForm::TemperatureStartActionExecute(
      TObject *Sender)
{
  int TransactionHandle;
  QLib::TQWaitResult WaitResult;
  try
  {
    // get the lower and upper limits
    try
    {
      m_UpperLimitTemp = StrToInt(HighTempEdit->Text);
      m_LowerLimitTemp = StrToInt(LowTempEdit->Text);
    }
    catch (Exception &exception)
    {
      throw Exception("The value entered is not valid");
    }

    for (int i = 0; i < NUM_OF_HEAD_HEATERS; i++)
    {
      m_UpperLimitTempMsg.SetElements.HeaterTable[i] = m_UpperLimitTemp;
      m_LowerLimitTempMsg.SetElements.HeaterTable[i] = m_LowerLimitTemp;
    }

    // send 'SetHeatersTemp' msg for the lower limit (high temperature)
    // (zohar) TransactionHandle = m_OHDBProtocolClient->Send(&m_LowerLimitTempMsg,sizeof(TOHDBSetHeatersTemperatureMessage));

    // (zohar) m_OHDBProtocolClient->InstallReceiveHandler(TransactionHandle,OHDBHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) WaitResult = m_SetHeatersEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_OHDBProtocolClient->SendInstallWaitReply(&m_LowerLimitTempMsg,
                           sizeof(TOHDBSetHeatersTemperatureMessage),
                           OHDBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
      // (zohar) m_OHDBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      throw Exception("Tester did not recieve Ack for 'Set Heaters temperatures' message");
    }
    m_LowLimitNow = true;
    
    // turn on the heaters
    TOHDBSetHeadHeaterOnOffMessage Msg;
    Msg.MessageID = OHDB_SET_HEAD_HEATER_ON_OFF;
    Msg.OnOff = HEATERS_ON;

    // (zohar) TransactionHandle = m_OHDBProtocolClient->Send(&Msg,sizeof(TOHDBSetHeadHeaterOnOffMessage));

    // (zohar) m_OHDBProtocolClient->InstallReceiveHandler(TransactionHandle,OHDBHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) WaitResult = m_HeatersOnOffEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg,
                           sizeof(TOHDBSetHeadHeaterOnOffMessage),
                           OHDBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
      // (zohar) m_OHDBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      throw Exception("Tester did not recieve Ack for 'Turn heaters on' message");
    }

    m_TemperatureCycle = 1;
    TemperatureCyclesPanel->Caption = StrToInt(m_TemperatureCycle);

    // enable the timer for the test
    TemperatureTimer->Enabled = true;

    TemperatueTestButton->Glyph = NULL;
    TemperatueTestButton->Action = TemperatureStopAction;
  }
  catch (Exception &exception)
  {
    MessageDlg(exception.Message,mtError,TMsgDlgButtons() << mbOK,0);
    TemperatueTestButton->Down = false;
  }

}
//---------------------------------------------------------------------------
void __fastcall TReliabilityTestForm::TemperatureStopActionExecute(
      TObject *Sender)
{
 // disable the timer for the test
  TemperatureTimer->Enabled = false;

  // turn off the heaters
  TOHDBSetHeadHeaterOnOffMessage Msg;
  Msg.MessageID = OHDB_SET_HEAD_HEATER_ON_OFF;
  Msg.OnOff = HEATERS_OFF;

  // send 'SetHeatersTemp' msg
  // (zohar) int TransactionHandle = m_OHDBProtocolClient->Send(&Msg,sizeof(TOHDBSetHeadHeaterOnOffMessage));

  // (zohar) m_OHDBProtocolClient->InstallReceiveHandler(TransactionHandle,OHDBHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  // (zohar) TQWaitResult WaitResult = m_HeatersOnOffEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg,
                           sizeof(TOHDBSetHeadHeaterOnOffMessage),
                           OHDBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

  {
    // (zohar) m_OHDBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
    MessageDlg("Tester did not recieve Ack for 'Turn heaters off' message",mtError,TMsgDlgButtons() << mbOK,0);
  }

  TemperatueTestButton->Glyph = NULL;
  TemperatueTestButton->Action = TemperatureStartAction;
}
//---------------------------------------------------------------------------
void __fastcall TReliabilityTestForm::TemperatureTimerTimer(
      TObject *Sender)
{
  int TransactionHandle, Limit;
  QLib::TQWaitResult WaitResult;
  bool FansOn;

  // send 'GetHeatersStatus' msg
  TOHDBGetHeatersStatusMessage Msg;
  Msg.MessageID = OHDB_GET_HEATERS_STATUS;

  // (zohar) TransactionHandle = m_OHDBProtocolClient->Send(&Msg,sizeof(TOHDBGetHeatersStatusMessage));

  // (zohar) m_OHDBProtocolClient->InstallReceiveHandler(TransactionHandle,OHDBHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  // (zohar) WaitResult = m_HeatersStatusEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_OHDBProtocolClient->SendInstallWaitReply(&Msg,
                           sizeof(TOHDBGetHeatersStatusMessage),
                           OHDBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

  {
    // (zohar) m_OHDBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
    MessageDlg("Tester did not recieve heaters status",mtError,TMsgDlgButtons() << mbOK,0);
    return;
  }

  // check if the temperature reached the upper/lower limit
  if (m_LowLimitNow)
    Limit = m_LowerLimitTemp;
  else
    Limit = m_UpperLimitTemp;

  for (int i = 0; i < NUM_OF_HEAD_HEATERS; i++)
  {
    // check if one of the heads did not reached the desired temperature
    if (!(m_CurrentTemp.HeaterTable[i] <= (Limit + TEMPERATURE_TOLERANCE) && m_CurrentTemp.HeaterTable[i] >= (Limit - TEMPERATURE_TOLERANCE)))
      return;
  }

  if (m_LowLimitNow)
    m_LowLimitNow = false;
  else
    m_LowLimitNow = true;

  // send 'SetHeatersTemp' msg
/*
  if (m_LowLimitNow)
    TransactionHandle = m_OHDBProtocolClient->Send(&m_LowerLimitTempMsg,sizeof(TOHDBSetHeatersTemperatureMessage));
  else
    TransactionHandle = m_OHDBProtocolClient->Send(&m_UpperLimitTempMsg,sizeof(TOHDBSetHeatersTemperatureMessage));

  m_OHDBProtocolClient->InstallReceiveHandler(TransactionHandle,OHDBHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  WaitResult = m_SetHeatersEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));
*/

  if(m_LowLimitNow) {

    if (m_OHDBProtocolClient->SendInstallWaitReply(&m_LowerLimitTempMsg,
                           sizeof(TOHDBSetHeatersTemperatureMessage),
                           OHDBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
      // (zohar) m_OHDBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      MessageDlg("Tester did not recieve Ack for 'Set Heaters temperatures' message",mtError,TMsgDlgButtons() << mbOK,0);
      return;
    }
  }
  else {
    if (m_OHDBProtocolClient->SendInstallWaitReply(&m_UpperLimitTempMsg,
                           sizeof(TOHDBSetHeatersTemperatureMessage),
                           OHDBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
      // (zohar) m_OHDBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      MessageDlg("Tester did not recieve Ack for 'Set Heaters temperatures' message",mtError,TMsgDlgButtons() << mbOK,0);
      return;

    }
  }

  // turn the fan on/off
  if (!m_LowLimitNow)
    FansOn = true;
  else
    FansOn = false;

  TOCBSetActuatorOnOffMessage FansMsg;
  FansMsg.MessageID = OCB_SET_ACTUATOR_ON_OFF;
  FansMsg.ActuatorID = FAN_ACTUATOR_NUM;
  FansMsg.OnOff = FansOn;
  // (zohar) TransactionHandle = m_OCBProtocolClient->Send(&FansMsg,sizeof(TOCBSetActuatorOnOffMessage));

  // (zohar) m_OCBProtocolClient->InstallReceiveHandler(TransactionHandle,OCBHandler,reinterpret_cast<TGenericCockie>(this));

  // Wait for reply
  // (zohar) WaitResult = m_FansEvent.WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_OCBProtocolClient->SendInstallWaitReply(&FansMsg,
                           sizeof(TOCBSetActuatorOnOffMessage),
                           OCBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

  {
    // (zohar) m_OCBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
    MessageDlg("Tester did not recieve Ack for 'Set fans on' message",mtError,TMsgDlgButtons() << mbOK,0);
    return;
  }


  m_TemperatureCycle++;
  TemperatureCyclesPanel->Caption = StrToInt(m_TemperatureCycle);




}
//---------------------------------------------------------------------------


void TReliabilityTestForm::OHDBHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   TReliabilityTestForm *InstancePtr = reinterpret_cast<TReliabilityTestForm *>(Cockie);

   // Build the read data message
   TOHDBGenericMessage *Response = static_cast<TOHDBGenericMessage *>(Data);

   switch (Response->MessageID)
   {
      case OHDB_ACK:
      {
        TOHDBAck *Ack = static_cast<TOHDBAck *>(Data);
        switch(Ack->RespondedMessageID)
        {
          case OHDB_SET_HEATERS_TEMPERATURE:
            if (Ack->AckStatus == OHDB_Ack_Success)
              InstancePtr->m_SetHeatersEvent.SetEvent();
            break;

          case OHDB_SET_HEAD_HEATER_ON_OFF:
            if (Ack->AckStatus == OHDB_Ack_Success)
              InstancePtr->m_HeatersOnOffEvent.SetEvent();
            break;

          default:
            return;
        }
      }

      case OHDB_HEADS_TEMPERATURE_STATUS:
      {
        TOHDBHeadsTemperatureStatusResponse *HeadsStatus = static_cast<TOHDBHeadsTemperatureStatusResponse *>(Data);

        memcpy(&(InstancePtr->m_CurrentTemp),&(HeadsStatus->CurrentElements),sizeof(THeaterElements));

        // Trigger the event
        InstancePtr->m_HeatersStatusEvent.SetEvent();
        break;
      }

      default:
        return;
   }
}
//---------------------------------------------------------------------------

void TReliabilityTestForm::OCBHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   TReliabilityTestForm *InstancePtr = reinterpret_cast<TReliabilityTestForm *>(Cockie);

   // Build the read data message
   TOCBGenericMessage *Response = static_cast<TOCBGenericMessage *>(Data);

   switch (Response->MessageID)
   {
      case OCB_ACK:
      {
        TOCBAck *Ack = static_cast<TOCBAck *>(Data);
        switch(Ack->RespondedMessageID)
        {
          case OCB_SET_ACTUATOR_ON_OFF:
            if (Ack->AckStatus == OCB_Ack_Success)
              InstancePtr->m_FansEvent.SetEvent();
            break;

          case OCB_PERFORM_PURGE:
            if (Ack->AckStatus == OCB_Ack_Success)
              InstancePtr->m_PerformPurgeEvent.SetEvent();
            break;

          case OCB_SET_HEADS_FILLING_PARAMS:
            if (Ack->AckStatus == OCB_Ack_Success)
              InstancePtr->m_HeadFillingEvent.SetEvent();
            break;

          default:
            return;
        }
      }

      case OCB_PURGE_STATUS:
      {
        TOCBPurgeStatusResponse *PurgeMsg = static_cast<TOCBPurgeStatusResponse *>(Data);

        InstancePtr->m_PurgeStatus = PurgeMsg->PurgeStatus;
        // Trigger the event
        InstancePtr->m_PurgeStatusEvent.SetEvent();
        break;
      }

      case OCB_PURGE_ERROR:
      {
        TOCBPurgeErrorResponse *PurgeError = static_cast<TOCBPurgeErrorResponse *>(Data);
        QMonitor.ErrorMessage("Purge error (" + QIntToStr(PurgeError->PurgeError) + ")");
      }
      default:
        return;
   }
}
//---------------------------------------------------------------------------


void __fastcall TReliabilityTestForm::PurgeStartActionExecute(TObject *Sender)
{
  int TransactionHandle;
  QLib::TQWaitResult WaitResult;

  try
  {
    try
    {
      m_PurgeInterval = StrToInt(PurgeFrequencyEdit->Text) * 1000;
    }
    catch (Exception &exception)
    {
      throw Exception("The value entered is not valid");
    }

    PurgeTimer->Interval = PURGE_STATUS_CHECK_INTERVAL;
    m_PurgeCycle = 0;
    PurgeCyclePanel->Caption = m_PurgeCycle;
    PurgeTimer->Enabled = true;
    PurgeTestButton->Glyph = NULL;
    PurgeTestButton->Action = PurgeStopAction;
  }
  catch (Exception &exception)
  {
    MessageDlg(exception.Message,mtError,TMsgDlgButtons() << mbOK,0);
    PurgeTestButton->Down = false;
  }

}
//---------------------------------------------------------------------------

void __fastcall TReliabilityTestForm::PurgeStopActionExecute(TObject *Sender)
{

  PurgeTestButton->Down = false; // this is done automatically when the button is clicked, but not when calling
                                 // the action explicitly
  PurgeTimer->Enabled = false;
  PurgeTestButton->Glyph = NULL;
  PurgeTestButton->Action = PurgeStartAction;
}
//---------------------------------------------------------------------------

void __fastcall TReliabilityTestForm::PurgeTimerTimer(TObject *Sender)
{
  int TransactionHandle;
  QLib::TQWaitResult WaitResult;

  try
  {
    // send 'get purge status' msg
    TOCBGetPurgeStatusMessage Msg;
    Msg.MessageID = OCB_GET_PURGE_STATUS;

    // (zohar) TransactionHandle = m_OCBProtocolClient->Send(&Msg,sizeof(TOCBGetPurgeStatusMessage));

    // (zohar) m_OCBProtocolClient->InstallReceiveHandler(TransactionHandle,OCBHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) WaitResult = m_PurgeStatusEvent.WaitFor(QMsToTicks(OHDB_REPLY_TIME_IN_MS));

    if (m_OCBProtocolClient->SendInstallWaitReply(&Msg,
                           sizeof(TOCBGetPurgeStatusMessage),
                           OCBHandler,
						   reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
      // (zohar) m_OCBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      throw Exception("Tester did not recieve purge status");
    }

    // check if purge can be executed now
    if (m_PurgeStatus != PURGE_IDLE)
    {
      PurgeTimer->Interval = PURGE_STATUS_CHECK_INTERVAL;
      return;
    }

    // send 'Perform purge' msg
    TOCBPerformPurgeMessage PurgeMsg;
    PurgeMsg.MessageID = OCB_PERFORM_PURGE;
    PurgeMsg.PurgeTime = PURGE_TIME;
    // (zohar) TransactionHandle = m_OCBProtocolClient->Send(&PurgeMsg,sizeof(TOCBPerformPurgeMessage));

    // (zohar) m_OCBProtocolClient->InstallReceiveHandler(TransactionHandle,OCBHandler,reinterpret_cast<TGenericCockie>(this));

    // Wait for reply
    // (zohar) WaitResult = m_PerformPurgeEvent.WaitFor(QMsToTicks(OCB_REPLY_TIME_IN_MS));

    if (m_OCBProtocolClient->SendInstallWaitReply(&PurgeMsg,
                           sizeof(TOCBPerformPurgeMessage),
                           OCBHandler,
                           reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)

    {
      // (zohar) m_OCBProtocolClient->UnInstallReceiveHandler(TransactionHandle);
      throw Exception("Tester did not recieve Ack for 'Perform purge' message");
    }

    m_PurgeCycle++;
    PurgeCyclePanel->Caption = m_PurgeCycle;
    PurgeTimer->Interval = m_PurgeInterval;
  }
  catch (Exception &exception)
  {
    MessageDlg(exception.Message,mtError,TMsgDlgButtons() << mbOK,0);
    PurgeStopAction->Execute();
  }
}
//---------------------------------------------------------------------------

