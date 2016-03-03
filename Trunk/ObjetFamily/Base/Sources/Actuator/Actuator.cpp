/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Actuator ON 8051 h/w                                     *
 * Module Description: This class implement services related to the *
 *                     Actuator on 0\8051 h/w.                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 1/May/02                                             *
 ********************************************************************/

#include "Actuator.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "AppParams.h"
#include "AppLogFile.h"
#include "FrontEnd.h"
#include "BackEndInterface.h"
#include "MaintenanceCountersDefs.h"

// Constants:

const int DELAY_BETWEEN_ACTUATOR_STATUS_VERIFICATION = 1000; //1 seconds.
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

#define DEFINE_ACTUATOR(id, descStr, formPartStr) (descStr),
#define DEFINE_ACTUATOR_SAME_VALUE(destID, srcID, descStr, formPartStr)
#define DEFINE_SENSOR(id, formPartStr)
char* ACTUATORS_NAME_TABLE [MAX_ACTUATOR_ID+1] =
{
#include "ActuatorDefs.h"
};
#undef DEFINE_ACTUATOR
#undef DEFINE_ACTUATOR_SAME_VALUE
#undef DEFINE_SENSOR

struct TActDesc
{
	int   ActuatorID;
	char* Description;
};
#define DEFINE_ACTUATOR(id, descStr, formPartStr)
#define DEFINE_ACTUATOR_SAME_VALUE(destID, srcID, descStr, formPartStr) {ACTUATOR_ID_##destID, descStr}, 
#define DEFINE_SENSOR(id, formPartStr)
TActDesc ACTUATORS_NAME_TABLE_SAME_VALUE [MAX_ACTUATOR_ID+1] =
{
#include "ActuatorDefs.h"
	{-1,""}
};
#undef DEFINE_ACTUATOR
#undef DEFINE_ACTUATOR_SAME_VALUE
#undef DEFINE_SENSOR

// Functions prototipes for test.
// ----------------------------------

// Class CActuator implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CActuatorBase::CActuatorBase(const QString& Name)
: CQComponent(Name), m_PurgeBathSensorStabilizer((CAppParams::Instance())->PurgeBathStabilizationWidth, true) /* 30 sec / actuators refresh rate */

{
   int i;
   ResetStatistics();
   INIT_METHOD(CActuatorBase,TimedSetOnOff);
   INIT_METHOD(CActuatorBase,ActivateRollerSuctionSystem);//RSS, itamar added
   INIT_METHOD(CActuatorBase,SetOnOff);
   INIT_METHOD(CActuatorBase,GetActuatorStatus);
   INIT_METHOD(CActuatorBase,GetInputStatus);
   INIT_METHOD(CActuatorBase,Test);
   INIT_METHOD(CActuatorBase,SetHoodFanOnOff);
   INIT_METHOD(CActuatorBase,SetAmbientHeaterOnOff);
   INIT_METHOD(CActuatorBase,SetHoodFanParamOnOff);
   INIT_METHOD(CActuatorBase,GetIfHoodFanIsOn);
   //INIT_METHOD(CActuatorBase,SetRSSValveOnOff);//RSS, itamar added
   //INIT_METHOD(CActuatorBase,SetRSSPumpOnOff);//RSS, itamar added
   INIT_METHOD(CActuatorBase,GetRollerSuctionSystemStatus);//RSS, itamar added
   INIT_METHOD(CActuatorBase,GetActuatorOnOff);
   INIT_METHOD(CActuatorBase,GetStabilizedPurgeBathSensorStatus);

   for(i=0;i<MAX_ACTUATOR_ID;i++)
      m_ActuatorTable[i] = false;
   for(i=0;i<MAX_SENSOR_ID;i++)
      m_SensorTable[i] = false;

   m_OCBClient = COCBProtocolClient::Instance();
   m_ParamsMgr = CAppParams::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();

   m_FlagCanSendGetStatusMsg = false;
   m_PollingControlFlag = false;

   //RSS, itamar
   m_IsRSSActivated = false;
   m_RSSOnValue = 1;
   m_RSSOffValue = 0;
   if(!m_ParamsMgr->IsSuctionSystemExist)
   {
        ACTUATORS_NAME_TABLE[2] = "Actuator: 2";
        ACTUATORS_NAME_TABLE[3] = "Actuator: 3";
   }
}

// Destructor
// -------------------------------------------------------
CActuatorBase::~CActuatorBase(void)
{
}


//-----------------------------------------------------------------------
// Constructor - Actuator 
// ------------------------------------------------------
CActuator::CActuator(const QString& Name):CActuatorBase(Name)
{
   m_Waiting=false;
   m_CancelFlag=false;
   
   m_FlagTimedCommandUnderUse = false;
   m_FlagRSSCommandUnderUse   = false;
   m_FlagGetRSSStatusUnderUse = false; 
   m_FlagSetOnOffUnderUse     = false;
   m_FlagGetStatusUnderUse    = false;
   m_FlagInputStatusUnderUse  = false;
   m_FlagGetModelSensor       = false;
   m_FlagGetModelStatus       = false;
   m_FlagResetModelSensor     = false;
   m_OdourFanInUse            = false;
   m_AmbientHeaterInUse       = false;
   
	  //Flag ack ok
   OnOff_AckOk=false;
   TimedOnOff_AckOk=false;
   RSS_AckOk=false; //RSS, itamar
   ResetModel_AckOk=false;
}

// Destructor
// -------------------------------------------------------
CActuator::~CActuator(void)
{
}

// Dummy constructor
CActuatorDummy::CActuatorDummy(const QString& Name):CActuatorBase(Name)
{
}


// Destructor
// -------------------------------------------------------
CActuatorDummy::~CActuatorDummy(void)
{
}
 
char* CActuatorBase::GetActuatorName(int ActuatorID)
{
	if(m_ParamsMgr->DualWasteEnabled)
	{
		for(int i = 0; i < MAX_ACTUATOR_ID; i++)
		{
			if (-1 == ACTUATORS_NAME_TABLE_SAME_VALUE[i].ActuatorID)
				break;

			if(ACTUATORS_NAME_TABLE_SAME_VALUE[i].ActuatorID == ActuatorID)
				return ACTUATORS_NAME_TABLE_SAME_VALUE[i].Description;
		}
	}

	return ACTUATORS_NAME_TABLE[ActuatorID];
}

//---------------------------------------------
//This procedure update the value of Actuator on off of OCB(8051).
void CActuatorBase::UpdateActuatorOnOff (int ID, bool OnOff)
{
   m_MutexDataBase.WaitFor();
      m_ActuatorTable[ID] = OnOff;
   m_MutexDataBase.Release();
}

//This procedure update the value of Actuator on off of OCB(8051).
void CActuatorBase::UpdateActuatorTable(T_ActuatorTable ActuatorTable)
{
   int i;
   m_MutexDataBase.WaitFor();
   for(i=0;i<MAX_ACTUATOR_ID;i++)
      m_ActuatorTable[i] = ActuatorTable[i];
   m_MutexDataBase.Release();
}

//This procedure get the value of Actuator on off of OCB(8051).
bool CActuatorBase::GetActuatorOnOff (int ID)
{
   bool OnOff;

   m_MutexDataBase.WaitFor();
      OnOff = m_ActuatorTable[ID];
   m_MutexDataBase.Release();
   return OnOff;
}

void CActuatorBase::DisplayActuatorTable(void)
{
   T_ActuatorTable DisplayTable;
   int i;

   m_MutexDataBase.WaitFor();
   for(i=0;i<MAX_ACTUATOR_ID;i++)
   {
     DisplayTable[i] = m_ActuatorTable[i];
   }
   m_MutexDataBase.Release();

   for(i=0;i<MAX_ACTUATOR_ID;i++)
   {
     QMonitor.Printf("ACTUATOR[%d] = %b ",i,DisplayTable[i]);
   }
}


//This procedure update the value of Sensor on off of OCB(8051).
void CActuatorBase::UpdateSensorOnOff (int ID, bool OnOff)
{
   m_MutexDataBase.WaitFor();
      m_SensorTable[ID] = OnOff;
   m_MutexDataBase.Release();
}

//This procedure update the value of Actuator on off of OCB(8051).
void CActuatorBase::UpdateInputTable(T_SensorTable SensorTable)
{
   int i;
   m_MutexDataBase.WaitFor();
   for(i=0;i<MAX_SENSOR_ID;i++)
      m_SensorTable[i] = SensorTable[i];
   m_MutexDataBase.Release();
}

//This procedure get the value of Sensor on off of OCB(8051).
bool CActuatorBase::GetSensorOnOff (int ID)
{
   bool OnOff;

   m_MutexDataBase.WaitFor();
      OnOff = m_SensorTable[ID];
   m_MutexDataBase.Release();
   return OnOff;
}

void CActuatorBase::DisplaySensorTable(void)
{
   T_SensorTable DisplayTable;
   int i;

   m_MutexDataBase.WaitFor();
   for(i=0;i<MAX_ACTUATOR_ID;i++)
   {
     DisplayTable[i] = m_SensorTable[i];
   }
   m_MutexDataBase.Release();

   for(i=0;i<MAX_ACTUATOR_ID;i++)
   {
     QMonitor.Printf("ACTUATOR[%d] = %b ",i,DisplayTable[i]);
   }
}

//-----------------------------------------------
//------------------------------------------------
//Actuator polling Control 
bool CActuatorBase::GetIfPollingControlIsOn(void)
{
   bool Flag;

   m_MutexDataBase.WaitFor();
      Flag = m_PollingControlFlag;
   m_MutexDataBase.Release();
   return Flag;
}

TQErrCode CActuatorBase::MarkIfPollingControlIsOn(bool OnOff)
{
   m_MutexDataBase.WaitFor();
      m_PollingControlFlag = OnOff;
   m_MutexDataBase.Release();

   return Q_NO_ERROR;
}


void CActuatorBase::PurgeBathSensorCalc(bool CurrRead)
{
    if (!(m_ParamsMgr->PurgeBathEnabled))
    { // not supported
        return;
    }

    // here true == OK
    bool stateBefore = m_PurgeBathSensorStabilizer.GetHistoryLastStatus();
    bool stateAfter  = m_PurgeBathSensorStabilizer.UpdateHistory(CurrRead);

    if ((stateBefore != stateAfter))
    {
        CQLog::Write(LOG_TAG_PRINT, "CActuatorBase::PurgeBathSensorCalc - purge bath OK: %d->%d", stateBefore, stateAfter);
        if (stateBefore)
        {
            QString overflowMsg("Purge bath overflow detected.\r\nPrinter maintenance required.");
            FrontEndInterface->NotificationMessage(overflowMsg);
            overflowMsg.replace(overflowMsg.find("\r"), 2, "  ");
            CQLog::Write(LOG_TAG_PRINT, overflowMsg);
        }
    }
}


//! returns "is full" as the stabilized status of the purge bath sensor
bool CActuatorBase::GetStabilizedPurgeBathSensorStatus()
{
    if (!(m_ParamsMgr->PurgeBathEnabled))
    { // not supported
        return false;
    }

    return !(m_PurgeBathSensorStabilizer.GetHistoryLastStatus());
}

// -------------------------------------------------------------
//OCB_PERFORM_ACTUATOR
TQErrCode CActuator::SetOnOff(int ActuatorId, bool OnOff)
{
   CQMutexHolder MutexHolder(&m_ActuatorStatusMutex); // sync actuator status between OCBStatus ans Actuator direct access

   // Verify if we are not performing other command
   if (m_FlagSetOnOffUnderUse)
       throw EActuator("Actuator:Set OnOff:two message send in the same time");

   //We start a new command enable to receive Notification message again
   //m_CancelFlagMutex.WaitFor();
   //   m_CancelFlag = false;
   //m_CancelFlagMutex.Release();

   m_FlagSetOnOffUnderUse=true;

   // Build the Actuator turn on message
   TOCBSetActuatorOnOffMessage ActuatorMsg;

   ActuatorMsg.MessageID = static_cast<BYTE>(OCB_SET_ACTUATOR_ON_OFF);
   ActuatorMsg.ActuatorID = static_cast<BYTE>(ActuatorId);
   ActuatorMsg.OnOff= static_cast<BYTE>(OnOff);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&ActuatorMsg,
                                         sizeof(TOCBSetActuatorOnOffMessage),
                                         OnOffAckResponse,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagSetOnOffUnderUse=false;
      throw EActuator("OCB didn't get ack for Actuator On-Off message:ID:" +
                      QIntToStr(ActuatorId) + "OnOFF=" + QIntToStr((int)OnOff));
   }

   //Keep parameters used
   UpdateActuatorOnOff(ActuatorId,OnOff);

   CQLog::Write(LOG_TAG_GENERAL,"%s - %s",GetActuatorName(ActuatorId),OnOff ? "On" : "Off");

   m_FlagSetOnOffUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CActuator::OnOffAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Actuator  turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CActuator *InstancePtr = reinterpret_cast<CActuator *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
      FrontEndInterface->NotificationMessage("Actuator \"OnOffAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"OnOffAckResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
      FrontEndInterface->NotificationMessage("Actuator \"OnOffAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"OnOffAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_ACTUATOR_ON_OFF)
   {
      FrontEndInterface->NotificationMessage("Actuator \"OnOffAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"OnOffAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("Actuator \"OnOffAckResponse\" ack error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"OnOffAckResponse\" ack error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
   }

   InstancePtr->OnOff_AckOk=true;
}


TQErrCode CActuator::TimedSetOnOff(int ActuatorId, bool OnOff,int Time)
{
   // Verify if we are not performing other command
   if (m_FlagTimedCommandUnderUse)
       throw EActuator("Actuator:Set timed OnOff:two message send in the same time");

   //We start a new command enable to receive Notification message again
   //m_CancelFlagMutex.WaitFor();
   //   m_CancelFlag = false;
   //m_CancelFlagMutex.Release();

   m_FlagTimedCommandUnderUse=true;

   // Build the Actuator turn on message
   TOCBTimedSetActuatorOnOffMessage ActuatorMsg;

   ActuatorMsg.MessageID = static_cast<BYTE>(OCB_TIMED_SET_ACTUATOR_ON_OFF);
   ActuatorMsg.ActuatorNumber = static_cast<BYTE>(ActuatorId);
   ActuatorMsg.ActuatorLevel= static_cast<BYTE>(OnOff);
   ActuatorMsg.Time= static_cast<USHORT>(OnOff);

   TimedOnOff_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&ActuatorMsg,
                         sizeof(TOCBTimedSetActuatorOnOffMessage),
                         TimedOnOffAckResponse,
						 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagTimedCommandUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"TimedSetOnOff\" message:ID=%d",
                                   ActuatorId);
      throw EActuator("OCB didn't get ack for \"TimedSetOnOff\":ID:" +
                      QIntToStr(ActuatorId) + "OnOFF=" + QIntToStr((int)OnOff));
   }

   if(!TimedOnOff_AckOk)
   {
      m_FlagTimedCommandUnderUse=false;
      CQLog::Write(LOG_TAG_GENERAL,"OCB ack failure for \"TimedSetOnOff\" ID=%d",
                                   ActuatorId);
      throw EActuator("OCB ack failure for \"TimedSetOnOff\":ID:" +
                      QIntToStr(ActuatorId) + "OnOFF=" + QIntToStr((int)OnOff));
   }

   //Keep parameters used
   UpdateActuatorOnOff(ActuatorId,OnOff);

   CQLog::Write(LOG_TAG_GENERAL,"%s - %s - Time %d",GetActuatorName(ActuatorId),OnOff ? "Timed On" : "Timed Off",Time);

   m_FlagTimedCommandUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command set timed on/off
// -------------------------------------------------------
void CActuator::TimedOnOffAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Actuator timed turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CActuator *InstancePtr = reinterpret_cast<CActuator *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
      FrontEndInterface->NotificationMessage("Actuator \"TimedOnOffAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"TimedOnOffAckResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
      FrontEndInterface->NotificationMessage("Actuator \"TimedOnOffAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"TimedOnOffAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_ACTUATOR_ON_OFF)
   {
      FrontEndInterface->NotificationMessage("Actuator \"TimedOnOffAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"TimedOnOffAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("Actuator \"TimedOnOffAckResponse\" ack error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"TimedOnOffAckResponse\" ack error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
   }

   InstancePtr->TimedOnOff_AckOk=true;
}

//RSS, itamar
TQErrCode CActuator::ActivateRollerSuctionSystem(int OnTime, int OffTime, bool OnOff)
{
   TQErrCode Err = Q_NO_ERROR;
   // Verify if we are not performing other command
   if (m_FlagRSSCommandUnderUse)
   {
       CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Roller Suction System: two messages were sent in the same time");
       throw EActuator("Roller Suction System: two messages were sent in the same time");
   }
   m_FlagRSSCommandUnderUse = true;

   // Build the Actuator turn on message
   TOCBActivateRollerSuctionSystemMessage RSSMsg;

   RSSMsg.MessageID = static_cast<BYTE>(OCB_ACTIVATE_SUCTION_SYSTEM);
   RSSMsg.ValveOnTime = static_cast<USHORT>(OnTime);
   RSSMsg.ValveOffTime= static_cast<USHORT>(OffTime);
   RSSMsg.OnOff= static_cast<BYTE>(OnOff);

   RSS_AckOk = false;

   // Send RSS activation request
   if (m_OCBClient->SendInstallWaitReply(&RSSMsg,
                         sizeof(TOCBActivateRollerSuctionSystemMessage),
                         ActivateRollerSuctionSystemAckResponse,
						 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagRSSCommandUnderUse=false;
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"OCB didn't get ack for \"Activate RSS\" message:ID=%d",
                                   RSSMsg.MessageID);
      throw EActuator("OCB didn't get ack for \"Activate RSS\" OnOFF=" + QIntToStr((int)OnOff));
   }

   if(!RSS_AckOk)
   {
      m_FlagRSSCommandUnderUse=false;
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"OCB ack failure for \"RSS\" ID=%d",
                                   RSSMsg.MessageID);
      throw EActuator("OCB didn't get ack for \"RSS\"OnOFF=" + QIntToStr((int)OnOff));
   }

   CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"RSS: OnTime: %d, OffTime: %d - %s",OnTime,OffTime,OnOff ? "On" : "Off");
   //CBackEndInterface* BackEnd = CBackEndInterface::Instance();
   //BackEnd->EnableDisableMaintenanceCounter(ROLLER_SUCTION_PUMP_COUNTER_ID, OnOff);

   m_IsRSSActivated = OnOff;
   m_FlagRSSCommandUnderUse=false;

   return Err;
}


// Acknolodges for Command set timed on/off
// -------------------------------------------------------
void CActuator::ActivateRollerSuctionSystemAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Actuator timed turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CActuator *InstancePtr = reinterpret_cast<CActuator *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
      FrontEndInterface->NotificationMessage("Actuator \"RSSAckResponse\" length error");
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"RSSAckResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
      FrontEndInterface->NotificationMessage("Actuator \"RSSAckResponse\" message id error");
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"RSSAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_ACTIVATE_SUCTION_SYSTEM)
   {
      FrontEndInterface->NotificationMessage("Actuator \"RSSAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"RSSAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("Actuator \"RSSAckResponse\" ack error");
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"RSSAckResponse\" ack error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
   }

   InstancePtr->RSS_AckOk=true;
}
//RSS, itamar


// Get Actuator Status is sent only during Actuator
//-------------------------------------------
TQErrCode CActuator::GetActuatorStatus(void)
{
   CQMutexHolder MutexHolder(&m_ActuatorStatusMutex); // sync actuator status between OCBStatus ans Actuator direct access

   // Verify if we are not performing other command
   if (m_FlagGetStatusUnderUse)
   {
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"GetStatus\" re-entry problem");
      return Q_NO_ERROR;
   }

   m_FlagGetStatusUnderUse=true;

   TOCBGetActuatorStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_ACTUATOR_STATUS);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,sizeof(TOCBGetActuatorStatusMessage),
                                  GetStatusAckResponse,
								  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"GetActuatorStatus\".");
   }

   m_FlagGetStatusUnderUse=false;
   return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CActuator::GetStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   T_ActuatorTable ActuatorTable;

   // Build the Actuator turn on message
   TOCBActuatorsStatusResponse *StatusMsg =
                   static_cast<TOCBActuatorsStatusResponse *>(Data);

   // Get a pointer to the instance
   CActuator *InstancePtr = reinterpret_cast<CActuator *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBActuatorsStatusResponse))
   {
      FrontEndInterface->NotificationMessage("Actuator \"GetStatusAckResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"GetStatusAckResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OCB_ACTUATOR_STATUS)
   {
      FrontEndInterface->NotificationMessage("Actuator \"GetStatusAckResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"Actuator \"GetStatusAckResponse\" message id error",
                                    (int)StatusMsg->MessageID);
      return;
   }


   for(int i=0, j=0; j < NUM_OF_ACTUATORS_PORTS; j++)
   {
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x01);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x02);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x04);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x08);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x10);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x20);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x40);
      ActuatorTable[i++] = ((int)StatusMsg->ActuatorsArray[j] & 0x80);
   }

   InstancePtr->UpdateActuatorTable(ActuatorTable);

   // Updating the Font-End with the Actuator status
#define DEFINE_ACTUATOR(id, descStr, formPartStr) FrontEndInterface->UpdateStatus(FE_ACTUATOR_##id,(ActuatorTable[ACTUATOR_ID_##id]?  1 : 0));
#define DEFINE_ACTUATOR_SAME_VALUE(destID, srcID, descStr, formPartStr) FrontEndInterface->UpdateStatus(FE_ACTUATOR_##destID,(ActuatorTable[ACTUATOR_ID_##srcID]?  1 : 0));
#define DEFINE_SENSOR(id, formPartStr)
#include "ActuatorDefs.h"
#undef DEFINE_ACTUATOR
#undef DEFINE_ACTUATOR_SAME_VALUE
#undef DEFINE_SENSOR

}

//RSS, itamar
TQErrCode CActuator::GetRollerSuctionSystemStatus(void)
{
   if(m_IsRSSActivated)
   {
        // Verify if we are not performing other command
        if (m_FlagGetRSSStatusUnderUse)
        {
                CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"GetRSSStatus\" re-entry problem");
                return Q_NO_ERROR;
        }

        m_FlagGetRSSStatusUnderUse=true;

        TOCBGetRSSStatusMessage GetMsg;

        GetMsg.MessageID = static_cast<BYTE>(GET_ROLLER_SUCTION_PUMP_VALUE);

        // Send a Turn ON request
        if (m_OCBClient->SendInstallWaitReply(&GetMsg,sizeof(TOCBGetRSSStatusMessage),
                                  GetRSSStatusAckResponse,
                                  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
        {
                CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"OCB didn't get ack for \"GetRSSStatusMessage\".");
        }

        FrontEndInterface->UpdateStatus(FE_RSS_PUMP_ON_VALUE, GetRSSOnValue());
        FrontEndInterface->UpdateStatus(FE_RSS_PUMP_OFF_VALUE, GetRSSOffValue());

        m_FlagGetRSSStatusUnderUse=false;
   }
   return Q_NO_ERROR;
}

// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CActuator::GetRSSStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Actuator turn on message
   TOCBRSSStatusResponse *StatusMsg = static_cast<TOCBRSSStatusResponse *>(Data);

   // Get a pointer to the instance
   CActuator *InstancePtr = reinterpret_cast<CActuator *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBRSSStatusResponse))
   {
      FrontEndInterface->NotificationMessage("Actuator \"GetRSSAckResponse\" length error");
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"GetRSSAckResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OCB_ROLLER_SUCTION_PUMP_VALUE)
   {
      FrontEndInterface->NotificationMessage("Actuator \"GetRSSAckResponse\" message id error");
      CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"Actuator \"GetRSSAckResponse\" message id error",
                                    (int)StatusMsg->MessageID);
      return;
   }

   InstancePtr->UpdateRSSStatus(StatusMsg->RollerSuctionPumpOnValue,StatusMsg->RollerSuctionPumpOffValue);
}

void CActuatorBase::UpdateRSSStatus(int RSSOnValue, int RSSOffValue) //RSS, itamar
{
   //should display these values in "General devices" screen
   m_RSSOnValue = RSSOnValue;
   m_RSSOffValue = RSSOffValue;
}

int CActuatorBase::GetRSSOnValue(void)
{
    return m_RSSOnValue;
}
int CActuatorBase::GetRSSOffValue(void)
{
    return m_RSSOffValue;
}


// Get Input Status is sent to read sensors status
//--------------------------------------------------
TQErrCode CActuator::GetInputStatus(void)
{
  // Verify if we are not performing other command
  if (m_FlagInputStatusUnderUse)
  {
    CQLog::Write(LOG_TAG_GENERAL,"Actuator \"GetInputStatus\" re-entry problem");
    return Q_NO_ERROR;
  }

  m_FlagInputStatusUnderUse=true;

  TOCBGetInputsStatusMessage GetMsg;

  GetMsg.MessageID = static_cast<BYTE>(OCB_GET_INPUTS_STATUS);

  // Send a Turn ON request
  if (m_OCBClient->SendInstallWaitReply(&GetMsg,sizeof(TOCBGetInputsStatusMessage),
                                 InputStatusAckResponse,
                                 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
    CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"GetInputStatus\".");
  }

  m_FlagInputStatusUnderUse=false;
  return Q_NO_ERROR;
}


// Acknowledges for Command turn on/off
// -------------------------------------------------------
void CActuator::InputStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
  T_SensorTable SensorTable;
  int i,j;

  // Build the Actuator turn on message
  TOCBInputsStatusResponse *StatusMsg = static_cast<TOCBInputsStatusResponse *>(Data);

  // Get a pointer to the instance
  CActuator *InstancePtr = reinterpret_cast<CActuator *>(Cockie);

  //Verify size of message
  if(DataLength != sizeof(TOCBInputsStatusResponse))
  {
    FrontEndInterface->NotificationMessage("Actuator \"InputStatusAckResponse\" length error");
    CQLog::Write(LOG_TAG_GENERAL,"Actuator \"InputStatusAckResponse\" length error");
    return;
  }

  // Update DataBase before Open the Semaphore/Set event.
  if (static_cast<int>(StatusMsg->MessageID) != OCB_INPUT_STATUS)
  {
    FrontEndInterface->NotificationMessage("Actuator \"InputStatusAckResponse\" message id error");
    CQLog::Write(LOG_TAG_GENERAL,"Actuator \"InputStatusAckResponse\" message id error",
                                  (int)StatusMsg->MessageID);
    return;
  }

  for(i=0, j=0; j < NUM_OF_BYTES_SENSORS; j++)
  {
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x01);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x02);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x04);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x08);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x10);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x20);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x40);
    SensorTable[i++] = ((int)StatusMsg->InputsArray[j] & 0x80);
  }

  InstancePtr->UpdateInputTable(SensorTable);

  for(int i=0; i<MAX_SENSOR_ID; i++)
    FrontEndInterface->UpdateStatus(FE_CURRENT_OCB_SENSOR_STATUS_START+i, (SensorTable[i]?  1 : 0));

//  CQLog::Write(LOG_TAG_GENERAL, QFormatStr("----------Sensors: %02x %02x %02x %02x", StatusMsg->InputsArray[0], StatusMsg->InputsArray[1], StatusMsg->InputsArray[2], StatusMsg->InputsArray[3]));

  // stabilize sensors (can expand this to 1 stabilizer per sensor)
  InstancePtr->PurgeBathSensorCalc(SensorTable[SENSOR_ID_0]);
}


//------------------------------------------------------------------

void CActuator::Cancel(void)
{}

/* The 2 following methods enable setting on/off the relevant actuators manually
//RSS, itamar added
// Turn the RSS pump on/off
TQErrCode CActuatorBase::SetRSSPumpOnOff(bool OnOff)
{
  TQErrCode RetValue = SetOnOff(ACTUATOR_ID_ROLLER_SUCTION_PUMP, OnOff);
  if(RetValue == Q_NO_ERROR)
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    BackEnd->EnableDisableMaintenanceCounter(ROLLER_SUCTION_PUMP_COUNTER_ID, OnOff);
    CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,QFormatStr("Roller Suction Pump %s", (OnOff ? "On" : "Off")));
  }
  return RetValue;
}

//RSS, itamar added
// Turn the RSS valve on/off
TQErrCode CActuatorBase::SetRSSValveOnOff(bool OnOff)
{
  TQErrCode RetValue = SetOnOff(ACTUATOR_ID_ROLLER_SUCTION_VALVE, OnOff);
  if(RetValue == Q_NO_ERROR)
  {
    CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,QFormatStr("Roller Suction Valve %s", (OnOff ? "On" : "Off")));
  }
  return RetValue;
}
*/

TQErrCode CActuator::SetHoodFanIdle()
{
  TQErrCode Ret;

  CQLog::Write(LOG_TAG_GENERAL,"Odour fan - Idle");

  Ret = SetHoodFanParamOnOff (true, m_ParamsMgr->OdourFanIdleVoltage);
  return Ret;
}

// Turn the hood fan on/off
TQErrCode CActuator::SetHoodFanOnOff(bool OnOff)
{
  TQErrCode Ret;

  CQLog::Write(LOG_TAG_GENERAL,"Odour fan - %s",OnOff ? "On" : "Off");
  Ret = SetHoodFanParamOnOff (OnOff, m_ParamsMgr->OdourFanVoltage);
  return Ret;
}


// Turn the hood fan on/off
TQErrCode CActuator::SetHoodFanParamOnOff(bool OnOff, int Voltage)
{
   // Verify if we are not performing other command
   if (m_OdourFanInUse)
	   throw EActuator("Actuator HoodFan:Set OnOff:two message send in the same time");


   m_OdourFanInUse = true;

   // Build the Actuator turn on message
   TOCBSetOdourFanOnOffMessage OdourMsg;

   OdourMsg.MessageID  = OCB_SET_ODOUR_FAN_ON_OFF;
   OdourMsg.OnOff      = OnOff ? 1 : 0;
   OdourMsg.FanVoltage = Voltage;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&OdourMsg,
                                         sizeof(TOCBSetOdourFanOnOffMessage),
                                         HoodFanOnOffResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_OdourFanInUse=false;
      throw EActuator("OCB didn't get ack for Odour Fan On-Off message. OnOFF = " + OnOff ? "On" : "Off");
   }

   m_OdourFanInUse = false;

   // Enable/disable the dirt pump maintenance counter
   CBackEndInterface::Instance()->EnableDisableMaintenanceCounter(CARBON_FILTER_COUNTER_ID, OnOff);
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CActuator::HoodFanOnOffResponse(int TransactionId,PVOID Data,
                             unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Actuator  turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
      FrontEndInterface->NotificationMessage("\"HoodFanOnOffResponse\" length error");
      CQLog::Write(LOG_TAG_GENERAL,"\"HoodFanOnOffResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
      FrontEndInterface->NotificationMessage("\"HoodFanOnOffResponse\" message id error");
      CQLog::Write(LOG_TAG_GENERAL,"\"HoodFanOnOffResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_ODOUR_FAN_ON_OFF)
   {
      FrontEndInterface->NotificationMessage("\"HoodFanOnOffResponse\" responded message id error");
      CQLog::Write(LOG_TAG_GENERAL,"\"HoodFanOnOffResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
      FrontEndInterface->NotificationMessage("\"HoodFanOnOffResponse\" ack error");
	  CQLog::Write(LOG_TAG_GENERAL,"\"HoodFanOnOffResponse\" ack error (%d)",
                                       (int)ResponseMsg->AckStatus);
	  return;
   }
}

TQErrCode CActuator::SetAmbientHeaterOnOff(bool OnOff, /* TAmbientHeaterSelector */ BYTE Heater)
{
   if(! CAppParams::Instance()->AmbientHeaterEnabled)
       return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_AmbientHeaterInUse)
	   throw EActuator("Actuator AmbientHeater:Set OnOff:two message send in the same time");


   m_AmbientHeaterInUse = true;

   // Build the Actuator turn on message
   TOCBAmbientHeaterOnOffMessage AmbientHeaterMsg;

   AmbientHeaterMsg.MessageID      = OCB_SET_AMBIENT_HEATER_ON_OFF;
   AmbientHeaterMsg.OnOff          = OnOff ? 1 : 0;
   AmbientHeaterMsg.HeaterSelector = Heater;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&AmbientHeaterMsg,
                                         sizeof(TOCBAmbientHeaterOnOffMessage),
                                         AmbientHeaterOnOffResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
	  m_AmbientHeaterInUse=false;
      throw EActuator("OCB didn't get ack for Ambient Heater On-Off message. OnOFF = " + OnOff ? "On" : "Off");
   }

   m_AmbientHeaterInUse = false;
   
   return Q_NO_ERROR;
}

void CActuator::AmbientHeaterOnOffResponse(int TransactionId, PVOID Data,
										   unsigned DataLength, TGenericCockie Cockie)
{
	// Build the Actuator  turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
   {
	  FrontEndInterface->NotificationMessage("\"AmbientHeaterOnOffResponse\" length error");
	  CQLog::Write(LOG_TAG_GENERAL,"\"AmbientHeaterOnOffResponse\" length error");
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
   {
	  FrontEndInterface->NotificationMessage("\"AmbientHeaterOnOffResponse\" message id error");
	  CQLog::Write(LOG_TAG_GENERAL,"\"AmbientHeaterOnOffResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
   }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_SET_AMBIENT_HEATER_ON_OFF)
   {
	  FrontEndInterface->NotificationMessage("\"AmbientHeaterOnOffResponse\" responded message id error");
	  CQLog::Write(LOG_TAG_GENERAL,"\"AmbientHeaterOnOffResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
   }

   if (ResponseMsg->AckStatus)
   {
	  FrontEndInterface->NotificationMessage("\"AmbientHeaterOnOffResponse\" ack error");
      CQLog::Write(LOG_TAG_GENERAL,"\"AmbientHeaterOnOffResponse\" ack error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return;
   }
}


bool CActuatorBase::GetIfHoodFanIsOn(void)
{
  return GetActuatorOnOff(ACTUATOR_ID_ODOR_FAN);
}

void CActuatorBase::Recovery(void)
{
}

// Dummy procedures for CActuatorDummy
//----------------------------------
// Get Actuator Status procedures
//---------------------------
// Wait procedure to Actuator On Off notification
//--------------------------------------------

//OCB_PERFORM_ACTUATOR
TQErrCode CActuatorDummy::SetOnOff(int ActuatorId, bool OnOff)
{
   //Keep parameters used
   UpdateActuatorOnOff(ActuatorId,OnOff);
   CQLog::Write(LOG_TAG_GENERAL,"%s - %s",GetActuatorName(ActuatorId),OnOff ? "On" : "Off");
   return Q_NO_ERROR;
}
TQErrCode CActuatorDummy::TimedSetOnOff(int ActuatorId, bool OnOff,int Time)
{
   //Keep parameters used
   UpdateActuatorOnOff(ActuatorId,OnOff);
   CQLog::Write(LOG_TAG_GENERAL,"%s - %s - Time %d",GetActuatorName(ActuatorId),OnOff ? "Timed On" : "Timed Off",Time);

   return Q_NO_ERROR;
}
TQErrCode CActuatorDummy::ActivateRollerSuctionSystem (int OnTime,int OffTime,bool OnOff)  //RSS, itamar
{
   //CBackEndInterface* BackEnd = CBackEndInterface::Instance();
   CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,"RSS: OnTime: %d, OffTime: %d - %s",OnTime,OffTime,OnOff ? "On" : "Off");
   //BackEnd->EnableDisableMaintenanceCounter(ROLLER_SUCTION_PUMP_COUNTER_ID, OnOff);
   m_IsRSSActivated = OnOff;
   return Q_NO_ERROR;
}

// Get Actuator Status is sent only during Actuator
//-------------------------------------------
TQErrCode CActuatorDummy::GetActuatorStatus(void)
{
   return Q_NO_ERROR;
}

//RSS, itamar
TQErrCode CActuatorDummy::GetRollerSuctionSystemStatus(void)
{
   if(m_IsRSSActivated)
   {
        FrontEndInterface->UpdateStatus(FE_RSS_PUMP_ON_VALUE, ++m_RSSOnValue);
        FrontEndInterface->UpdateStatus(FE_RSS_PUMP_OFF_VALUE,++m_RSSOffValue);
   }
   return Q_NO_ERROR;
}

// Get Input Status is sent to read sensors status
//--------------------------------------------------
TQErrCode CActuatorDummy::GetInputStatus(void)
{
   return Q_NO_ERROR;
}

// Turn the hood On with Low velocity
TQErrCode CActuatorDummy::SetHoodFanIdle()
{
   SetOnOff(ACTUATOR_ID_ODOR_FAN,true);

  return Q_NO_ERROR;
}

// Turn the hood fan on/off
TQErrCode CActuatorDummy::SetHoodFanOnOff(bool OnOff)
{
   SetOnOff(ACTUATOR_ID_ODOR_FAN,OnOff);

  return Q_NO_ERROR;
}

TQErrCode CActuatorDummy::SetAmbientHeaterOnOff(bool OnOff, /* TAmbientHeaterSelector */ BYTE Heater)
{
	switch(Heater)
	{
		case AMBIENT_HEATER_1:
			SetOnOff(ACTUATOR_ID_AMBIENT_HEATER_1,OnOff);
			break;

		case AMBIENT_HEATER_2:
			SetOnOff(ACTUATOR_ID_AMBIENT_HEATER_2,OnOff);
			break;

		case (AMBIENT_HEATER_1 | AMBIENT_HEATER_2):
			SetOnOff(ACTUATOR_ID_AMBIENT_HEATER_1,OnOff);
			SetOnOff(ACTUATOR_ID_AMBIENT_HEATER_2,OnOff);
			break;
	}

	return Q_NO_ERROR;
}

// Turn the hood fan on/off
TQErrCode CActuatorDummy::SetHoodFanParamOnOff(bool OnOff, int Voltage)
{
   SetOnOff(ACTUATOR_ID_ODOR_FAN,OnOff);

  return Q_NO_ERROR;
}

TQErrCode CActuatorBase::Test (void)
{
   return Q_NO_ERROR;
}

//UV Lamps Statistcs
void CActuatorBase::DisplayStatistics(void)
{
  if(m_CollisionStatistics_Sensor1 ||
     m_CollisionStatistics_Sensor2 ||
     m_CollisionStatistics_BothSensors)
  {
    CQLog::Write(LOG_TAG_UV_LAMPS,"Actuator: Collisions: Sensor1:%d Sensor2:%d Both:%d",
                                  m_CollisionStatistics_Sensor1,
                                  m_CollisionStatistics_Sensor2,
                                  m_CollisionStatistics_BothSensors);
  }
}

void CActuatorBase::ResetStatistics(void)
{
    m_CollisionStatistics_Sensor1=0;
    m_CollisionStatistics_Sensor2=0;
    m_CollisionStatistics_BothSensors=0;
}

