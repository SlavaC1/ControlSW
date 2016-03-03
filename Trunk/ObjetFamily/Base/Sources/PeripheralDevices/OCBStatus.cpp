/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: OCB send status message class                            *
 * Module Description: This class implement services related to     *
 *                     send get status message to OCB.             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 28/01/2002                                           *
 ********************************************************************/

#include "OCBStatus.h"
#include "QUtils.h"
#include "QTimer.h"
#include "Q2RTErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "QThread.h"
#include "AppLogFile.h"
#include "OCBCommDefs.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "Q2RTApplication.h"
#include "CheckPoints.h"
#include "SignalTower.h"
#include "BackEndInterface.h"

#define SENSOR_ID_SERVICE_DOOR			2
#define SENSOR_ID_FRONT_RIGHT_DOOR		3
#define SENSOR_ID_FRONT_LEFT_DOOR		4
#define SENSOR_ID_SERVICE_KEY           5
#define SENSOR_ID_EMERGENCY_STOP_REAR   6
#define SENSOR_ID_EMERGENCY_STOP_FRONT  7
#define SENSOR_ID_IS_TRAY_INSERTED      8


// Constants:
const int DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL = 500;
//Delay : How much time to ask time TIME * DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL
/*const int TIME_BETWEEN_FILLING_STATUS = 2;   //
const int TIME_BETWEEN_POWER_STATUS = 4;
const int TIME_BETWEEN_TRAY_HEATER_STATUS = 8;
const int TIME_BETWEEN_UV_LAMPS_STATUS = 1;
const int TIME_BETWEEN_ACTUATOR_STATUS = 1;
const int TIME_BETWEEN_RSS_STATUS = 1; //RSS, itamar
const int TIME_BETWEEN_MODEL_SENSOR = 2;
const int TIME_BETWEEN_WEIGHT_STATUS = 1;*/

const int ONE_SECOND_PERIOD = MS_IN_SEC/DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL;

const int ACK_WAIT_TIMEOUT = QMsToTicks(1000);
const int WAIT_FOR_WAKE_UP = QMsToTicks(5000); // 5 seconds

const int COLD_RESET = 0;
const int WARM_RESET = 1;

class CheckPoints; // remove !!!
extern CheckPoints g_cp; // remove !!!

// Class COCBStatusSender implementation
// ------------------------------------------------------------------

// Constructor
// -------------------------------------------------------
COCBStatusSender::COCBStatusSender(const QString& Name,
                                   CPurgeBase *PurgePtr,
                                   CUvLamps *UvLampsPtr,
                                   CActuatorBase *ActuatorPtr,
                                   CDoorBase *DoorPtr,
                                   CContainerBase *ContainerPtr,
                                   CPowerBase *PowerPtr,
								   CTrayBase *TrayPlacePtr,
								   CEvacAirFlowBase *AirFlowPtr,
								   CHeadFillingBase *HeadFillingPtr,
								   CContainerBase *Containers,
								   CTrayBase *TrayHeater):
             CQThread(true,Name)
{
   m_ActuatorPtr = ActuatorPtr;            
   m_DoorPtr = DoorPtr;
   m_ContainerPtr = ContainerPtr;
   m_PowerPtr = PowerPtr;                  
   m_PurgePtr = PurgePtr;                  
   m_TrayPlacerPtr = TrayPlacePtr;
   m_AirFlowPtr = AirFlowPtr;
   m_UvLampsPtr = UvLampsPtr;
   m_HeadFillingPtr = HeadFillingPtr;
   m_ResetMessageSent=false;
   m_Containers = Containers;
   m_TrayHeaterPtr = TrayHeater;

   m_OCB_SW_ExternalVersion=0;
   m_OCB_SW_InternalVersion=0;
   m_OCB_A2D_SW_ExternalVersion=0;
   m_OCB_A2D_SW_InternalVersion=0;
   //Elad added
   for(int i=0;i<NUM_OF_MSC_CARDS;++i)
   {
	   m_MSC_HW_Version[i]=0;
	   m_MSC_FW_Version[i]=0;
   }
   ////////////////////
   m_OCB_HW_Version=0;

   m_DebugMode=false;

   m_ResetAckOk=false;
   m_SWVersionSent=false;
   m_A2D_SWVersionSent=false;
   m_HWVersionSent=false;
   m_DebugModeMessageSent=false;
   m_SWVersionAckOk=false;
   m_A2D_SWVersionAckOk=false;
   m_HWVersionAckOk=false;
   m_SWDebugModeAckOk=false;

   m_OCBClient = COCBProtocolClient::Instance();
   m_ParamsMgr = CAppParams::Instance();

   // Install a receive handler for a specific message ID
   m_OCBClient->InstallMessageHandler(OCB_RESET_WAKE_UP,
                                      &WakeUpNotification,
                                      reinterpret_cast<TGenericCockie>(this));

   INIT_METHOD(COCBStatusSender,Reset);
   INIT_METHOD(COCBStatusSender,DisplayOCBVersion);
   INIT_METHOD(COCBStatusSender,GetOCBSWVersionMessage);
   INIT_METHOD(COCBStatusSender,GetOCB_A2D_SWVersionMessage);
   INIT_METHOD(COCBStatusSender,GetOCBHWVersionMessage);
   INIT_METHOD(COCBStatusSender,EnterOCBDebugMode);
   INIT_METHOD(COCBStatusSender,ExitOCBDebugMode);
   INIT_METHOD(COCBStatusSender,GetOCBDebugModeValue);
}

// Destructor
// -------------------------------------------------------
COCBStatusSender::~COCBStatusSender(void)
{
}


// --------------------------------------------------------------------
// This task procedure is responsable to send status message to OCB
//---------------------------------------------------------------------
void COCBStatusSender::Execute(void)
{
	// Constants:
	const int DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL = 500;
	//Delay : How much time to ask time TIME * DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL

   m_timeCounter = 0;

   //The first time the message is sent immediatly after that according to 
   //each status message specif time. 
   while(!Terminated)
      {
      try
		 {
		 if((m_timeCounter%(ONE_SECOND_PERIOD))==0)
         {
		   m_UvLampsPtr->GetStatus();
           // enable following line to allow constant update of UV Sensor value.
           m_UvLampsPtr->GetUVValue();		  
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD))==0)
		 {
		   m_ActuatorPtr->GetActuatorStatus();
		   m_ActuatorPtr->GetInputStatus();	//Sensors

				if(! m_ParamsMgr->RemovableTray)
				{
				m_TrayPlacerPtr->UpdateTrayInserted(true); // Consider as "always in"
				}
				else
					m_TrayPlacerPtr->UpdateTrayInserted(m_ActuatorPtr->GetSensorOnOff(SENSOR_ID_IS_TRAY_INSERTED));   //update the stay status - in or out


			 if( m_DoorPtr->IsSafetySystemExist())
			 {
			   m_DoorPtr->GetSafetySystemInstance()->UpdateSafetySystemStatus( m_ActuatorPtr->GetSensorOnOff(SENSOR_ID_EMERGENCY_STOP_REAR),m_ActuatorPtr->GetSensorOnOff(SENSOR_ID_EMERGENCY_STOP_FRONT), m_ActuatorPtr->GetSensorOnOff(SENSOR_ID_SERVICE_KEY));
			   m_DoorPtr->GetSafetySystemInstance()->UpdatePopUpMessage();
			 }

			 if( CBackEndInterface::Instance()->GetSignalTowerLightState(GREEN_LIGHT) == ST_LIGHT_BLINK )
			 {
			 	 //if a door is opened, turn off Green light on signal tower
			 	 if(  ( m_ActuatorPtr->GetSensorOnOff( SENSOR_ID_SERVICE_DOOR ) &&
					m_ActuatorPtr->GetSensorOnOff( SENSOR_ID_FRONT_RIGHT_DOOR ) &&
					m_ActuatorPtr->GetSensorOnOff( SENSOR_ID_FRONT_LEFT_DOOR ) ) == false  )
				 {					
					 Q2RTApplication->GetMachineManager()->GetMachineSequencer()->ActivateSignalTower(ST_LIGHT_NC, ST_LIGHT_OFF, ST_LIGHT_NC);
	             }
			 }
			
		 }




		 if((m_timeCounter%(ONE_SECOND_PERIOD))==1)
         {
           m_Containers->GetLiquidTanksWeight(); // the exception causing call !!		  
		   m_Containers->CheckWasteAndActivateAlertIfNeeded();
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD))==1)
         {
           m_ActuatorPtr->GetRollerSuctionSystemStatus();          
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD*2))==3)
		 {
		   m_HeadFillingPtr->GetAllHeadsFillingsStatus();
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD*4))==6)
		 {
		   m_PowerPtr->GetStatus();

		   m_PowerPtr->GetPowerSuppliersStatus();

		   if(Q2RTApplication->GetMachineManager()->GetCurrentState() == msPrinting &&
			  CHECK_NOT_EMULATION(m_ParamsMgr->OCB_Emulation))
			  CQLog::Write(LOG_TAG_POWER,"Get power status");
		 }

		 if((m_timeCounter%(ONE_SECOND_PERIOD*8))==13)
		 {
		   m_TrayHeaterPtr->GetTrayStatus(); //Find out the current tray temperature
		 }
		 if((m_timeCounter%(ONE_SECOND_PERIOD*5))==8)
		 {
			//if (!m_AirFlowPtr->IsBoundaryCheckSuspended())
			if (m_ParamsMgr->EvacuationIndicatorEnabled)
				m_AirFlowPtr->GetEvacuationSensorStatusResponse();
		 }
         //m_DoorPtr->GetStatus();
         //m_ContainerPtr

         //QSleep(DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL);
		 PausePoint(DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL);
		 m_timeCounter = (m_timeCounter+1)%100;

         }

      // Catch QException classes
      catch(EQException& QException)
         {
         QMonitor.Print("OCB sender task: " +
                        QException.GetErrorMsg()+ " Error Code: " +
                        QIntToStr(QException.GetErrorCode()));
         } //end of catch
      catch(...)
        {
          unsigned int i=g_cp.IncCheckPoint1();
          QSleep(5000);
          CQLog::Write(LOG_TAG_GENERAL,"(shahar) OCB Status sender. unknown exception caught. IncCheckPoint1 = %d", i);
        }
      } // end of while
}


// Cancel wait operations
void COCBStatusSender::Cancel(void)
{
    Suspend();
}


//    
// Notification OCB WAKE UP
void COCBStatusSender::WakeUpNotification(int TransactionId,
                                           PVOID Data,
                                           unsigned DataLength,
                                           TGenericCockie Cockie)
{
  COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);
  TOCBResetWakeUpResponse *Message = static_cast<TOCBResetWakeUpResponse *>(Data);
  
  if(Message->MessageID != OCB_RESET_WAKE_UP)
     {
     CQLog::Write(LOG_TAG_GENERAL,"OCB WakeUpNotification unknown message id");
     return;
     }

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength != sizeof(TOCBResetWakeUpResponse))
     {
     CQLog::Write(LOG_TAG_GENERAL,"OCB WakeUpNotification data length error");
     return;
     }

  // Check if the ack is sucessful
  switch (Message->ResetType)
     {
     case COLD_RESET:
        CQLog::Write(LOG_TAG_GENERAL,"RESET: OCB cold reset");
        break;
     case WARM_RESET:
        CQLog::Write(LOG_TAG_GENERAL,"RESET: OCB warm reset");
        break;
     default:
        CQLog::Write(LOG_TAG_GENERAL,"OCB reset unknown");
        break;
     }

  InstancePtr->AckToOCBNotification(OCB_RESET_WAKE_UP,
                                     TransactionId,
                                     OCB_Ack_Success,
                                     Cockie);
}

// Send OCB reset message to OCb
TQErrCode COCBStatusSender::Reset(void)
{
  TOCBGenericMessage Message;

  if(m_ResetMessageSent)
     throw EOCBStatus("Two OCB Reset message sent in the same time");

  m_ResetMessageSent=true;

  Message.MessageID = OCB_SW_RESET;

  m_ResetAckOk = false;

  //Send the messahe
  if (m_OCBClient->SendInstallWaitReply(&Message,sizeof(TOCBGenericMessage),
                      ResetAck,
					  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
	 {
     m_ResetMessageSent=false;
     throw EOCBStatus("OCB:Reset - Error while waiting for ACK");
     }

  // Check if we really got ack
  if(!m_ResetAckOk)
    {
    m_ResetMessageSent=false;
    throw EOCBStatus("OCB reset acknolodge fail");
    }

  m_ResetMessageSent=false;
  return Q_NO_ERROR;
}

void COCBStatusSender::ResetAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBAck))
     {
     // Check if the ping is sucessful
     TOCBAck *Message = static_cast<TOCBAck *>(Data);

     if(Message->MessageID == OCB_ACK)
        {
        if (Message->RespondedMessageID == OCB_SW_RESET)
           InstancePtr->m_ResetAckOk = (Message->AckStatus == OCB_Ack_Success);
        else
           InstancePtr->m_ResetAckOk = false;
        }
     else
        InstancePtr->m_ResetAckOk = false;
     }
  else
     InstancePtr->m_ResetAckOk = false;
}


// Get SW version
TQErrCode COCBStatusSender::GetOCBSWVersionMessage(void)
{
  TOCBGetDriverSWVersionMessage Message;

  if(m_SWVersionSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"GetOCBSWVersionMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_SWVersionSent=true;
  Message.MessageID = OCB_GET_OCB_SW_VERSION;

  m_SWVersionAckOk = false;

  //send the message
  if (m_OCBClient->SendInstallWaitReply(&Message,
                                sizeof(TOCBGetDriverSWVersionMessage),
                                GetOCBSWVersionAck,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_SWVersionSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OCB didn't get ack for \"GetOCBSWVersionMessage\" message");
     throw EOCBStatus("OCB didn't get ack for \"GetOCBSWVersionMessage\" message");
     }

  // Check if we really got ack
  if(!m_SWVersionAckOk)
    {
    m_SWVersionSent=false;
    throw EOCBStatus("OCB \"GetOCBSWVersionMessage\": Invalid reply from OCB");
    }

  m_SWVersionSent=false;
  return Q_NO_ERROR;
}

void COCBStatusSender::GetOCBSWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBDriverSoftwareVersionResponse  ))
     {
     // Check if the ping is sucessful
     TOCBDriverSoftwareVersionResponse  *Message =
             static_cast<TOCBDriverSoftwareVersionResponse  *>(Data);

     if(Message->MessageID == OCB_SW_VERSION)
        {
		InstancePtr->m_OCB_SW_ExternalVersion = Message->ExternalVersion;
		InstancePtr->m_OCB_SW_InternalVersion = Message->InternalVersion;
		for(int i=0;i<NUM_OF_MSC_CARDS;++i)
		{
			InstancePtr->m_MSC_HW_Version[i]  =  Message->MSCHardwareVersion[i];
			InstancePtr->m_MSC_FW_Version[i]  =  Message->MSCFirmwareVersion[i];
		}
        InstancePtr->m_SWVersionAckOk = true;
        }
     else
        InstancePtr->m_SWVersionAckOk = false;
     }
  else
     InstancePtr->m_SWVersionAckOk = false;
}

// Get OCB-A2D SW version
TQErrCode COCBStatusSender::GetOCB_A2D_SWVersionMessage(void)
{
  TOCBA2DGetDriverSWVersionMessage Message;

  if(m_A2D_SWVersionSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"GetOCB_A2D_SWVersionMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_A2D_SWVersionSent=true;
  Message.MessageID = OCB_GET_OCB_A2D_SW_VERSION;

  m_A2D_SWVersionAckOk = false;

  //send the message
  if (m_OCBClient->SendInstallWaitReply(&Message,
                                sizeof(TOCBA2DGetDriverSWVersionMessage),
                                GetOCB_A2D_SWVersionAck,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_A2D_SWVersionSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OCB didn't get ack for \"GetOCB_A2D_SWVersionMessage\" message");
     throw EOCBStatus("OCB didn't get ack for \"GetOCB_A2D_SWVersionMessage\" message");
     }

  // Check if we really got ack
  if(!m_A2D_SWVersionAckOk)
    {
    m_A2D_SWVersionAckOk=false;
    throw EOCBStatus("OCB \"GetOCB_A2D_SWVersionMessage\": Invalid reply from OCB");
    }

  m_A2D_SWVersionAckOk=false;
  return Q_NO_ERROR;
}

// Get OCB-A2D SW version
void COCBStatusSender::GetOCB_A2D_SWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBA2DDriverSoftwareVersionResponse  ))
     {
     // Check if the ping is sucessful
     TOCBA2DDriverSoftwareVersionResponse  *Message =
             static_cast<TOCBA2DDriverSoftwareVersionResponse  *>(Data);

     if(Message->MessageID == OCB_A2D_SW_VERSION)
        {
        InstancePtr->m_OCB_A2D_SW_ExternalVersion = Message->ExternalVersion;
        InstancePtr->m_OCB_A2D_SW_InternalVersion = Message->InternalVersion;
        InstancePtr->m_A2D_SWVersionAckOk = true;
        }
     else
        InstancePtr->m_A2D_SWVersionAckOk = false;
     }
  else
     InstancePtr->m_A2D_SWVersionAckOk = false;
}




TQErrCode COCBStatusSender::GetOCBHWVersionMessage(void)
{

  TOCBGetDriverHWVersionMessage Message;

  if(m_HWVersionSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"GetOCBHWVersionMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_HWVersionSent=true;
  Message.MessageID = OCB_GET_OCB_HW_VERSION;

  m_HWVersionAckOk = false;

  //send the message
  if (m_OCBClient->SendInstallWaitReply(&Message,sizeof(TOCBGetDriverHWVersionMessage),
                                GetOCBHWVersionAck,
								reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_HWVersionSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OCB didn't get ack for \"GetOCBHWVersionMessage\" message");
     throw EOCBStatus("OCB didn't get ack for \"GetOCBHWVersionMessage\" message");
     }

  // Check if we really got ack
  if(!m_HWVersionAckOk)
    {
    m_HWVersionSent=false;
    throw EOCBStatus("OCB \"GetOCBHWVersionMessage\": Invalid reply from OCB");
    }

  m_HWVersionSent=false;
  return Q_NO_ERROR;
}

void COCBStatusSender::GetOCBHWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBDriverHardwareVersionResponse))
     {
     // Check if the ping is sucessful
     TOCBDriverHardwareVersionResponse  *Message =
             static_cast<TOCBDriverHardwareVersionResponse  *>(Data);

     if(Message->MessageID == OCB_HW_VERSION)
        {
        InstancePtr->m_OCB_HW_Version = Message->Version;
        InstancePtr->m_HWVersionAckOk = true;
        }
     else
        InstancePtr->m_HWVersionAckOk = false;
     }
  else
     InstancePtr->m_HWVersionAckOk = false;
}


TQErrCode COCBStatusSender::DisplayOCBVersion(void)
{
   GetOCBSWVersionMessage();
   GetOCB_A2D_SWVersionMessage();

   CQLog::Write(LOG_TAG_GENERAL,"OCB SW version = %d.%d",
								   m_OCB_SW_ExternalVersion,
								   m_OCB_SW_InternalVersion);
   for(int i=0;i<NUM_OF_MSC_CARDS;i++)
   {
	   CQLog::Write(LOG_TAG_GENERAL,"MSC%d HW version = %d",i+1,m_MSC_HW_Version[i]);
	   CQLog::Write(LOG_TAG_GENERAL,"MSC%d FW version = %d",i+1,m_MSC_FW_Version[i]);
   }

   CQLog::Write(LOG_TAG_GENERAL,"OCB A2D SW version = %d.%d",
                                   m_OCB_A2D_SW_ExternalVersion,
                                   m_OCB_A2D_SW_InternalVersion);
   return Q_NO_ERROR;
}

int COCBStatusSender::GetOCBSWExternalVersion(void)
{
   return m_OCB_SW_ExternalVersion;
}

int COCBStatusSender::GetOCBSWInternalVersion(void)
{
   return m_OCB_SW_InternalVersion;
}

int COCBStatusSender::GetOCB_A2D_SWExternalVersion(void)
{
   return m_OCB_A2D_SW_ExternalVersion;
}

int COCBStatusSender::GetOCB_A2D_SWInternalVersion(void)
{
   return m_OCB_A2D_SW_InternalVersion;
}


int COCBStatusSender::GetOCBHWVersion(void)
{
   return m_OCB_HW_Version;
}

//Elad added , I2C HW+FW Version.
int COCBStatusSender::GetMSCHWVersion(int index)
{
   return m_MSC_HW_Version[index];
}

int COCBStatusSender::GetMSCFWVersion(int index)
{
   return m_MSC_FW_Version[index];
}


//Debug Mode
bool COCBStatusSender::GetOCBDebugModeValue(void)
{
   return m_DebugMode;
}

TQErrCode COCBStatusSender::EnterOCBDebugMode(void)
{
   return EnterOCBModeMessage(true);
}

TQErrCode COCBStatusSender::ExitOCBDebugMode(void)
{
   return EnterOCBModeMessage(false);
}

//Enter debug mode
TQErrCode COCBStatusSender::EnterOCBModeMessage(bool DebugMode)
{

  TOCBSetDebugModeMessage Message;

  if(m_DebugModeMessageSent)
     {
     CQLog::Write(LOG_TAG_PRINT,"Two \"EnterOCBModeMessage\" messages sent in the same time");
     return Q_NO_ERROR;
     }

  m_DebugModeMessageSent=true;
  Message.MessageID = OCB_SET_DEBUG_MODE;
  Message.DebugMode = DebugMode;

  m_SWDebugModeAckOk = false;

  //send the message
  if (m_OCBClient->SendInstallWaitReply(&Message,sizeof(TOCBSetDebugModeMessage),
                                GetOCBDebugMessageAck,
                                reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
     {
     m_DebugModeMessageSent=false;
     CQLog::Write(LOG_TAG_PRINT,"OCB didn't get ack for \"EnterOCBModeMessage\" message");
     throw EOCBStatus("OCB didn't get ack for \"EnterOCBModeMessage\" message");
     }

  // Check if we really got ack
  if(!m_SWDebugModeAckOk)
    {
    m_DebugModeMessageSent=false;
    throw EOCBStatus("\"EnterOCBModeMessage\": Invalid reply from OCB");
    }

  m_DebugMode = DebugMode;
  m_DebugModeMessageSent=false;
  return Q_NO_ERROR;
}

void COCBStatusSender::GetOCBDebugMessageAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
  COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);

  // Check if we got ack message
  // Ignore messages smaller the ack message size
  if(DataLength == sizeof(TOCBAck))
     {
     // Check if the ping is sucessful
     TOCBAck  *Message = static_cast<TOCBAck  *>(Data);

     if(Message->MessageID == OCB_ACK)
        if (Message->RespondedMessageID == OCB_SET_DEBUG_MODE)
           {
           InstancePtr->m_SWDebugModeAckOk = (Message->AckStatus == OCB_Ack_Success);
           return;
           }
     }

    InstancePtr->m_SWDebugModeAckOk = false;
}





void COCBStatusSender::AckToOCBNotification (int MessageID,
                                            int TransactionID,
                                            int AckStatus,
                                            TGenericCockie Cockie)
{
   // Build the ack message
   TOCBAck AckMsg;

   // Get a pointer to the instance
   COCBStatusSender *InstancePtr = reinterpret_cast<COCBStatusSender *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Ack message
   InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                 sizeof(TOCBAck));
}


//-----------------------------------------------------
// Dummy procedures
//------------------------------------------------------
COCBStatusSenderDummy::COCBStatusSenderDummy(const QString& Name, 
                                             CPurgeBase *PurgePtr,          
                                             CUvLamps *UvLampsPtr,          
                                             CActuatorBase *ActuatorPtr,    
                                             CDoorBase *DoorPtr,
                                             CContainerBase *ContainerPtr,
                                             CPowerBase *PowerPtr,          
											 CTrayBase *TrayPlacerPtr,
											 CEvacAirFlowBase *AirFlowPtr,
                                             CHeadFillingBase *HeadFillingPtr,
											 CContainerBase *Containers,
											 CTrayBase *TrayHeater):
   COCBStatusSender(Name,
                    PurgePtr,
                    UvLampsPtr,
                    ActuatorPtr,
                    DoorPtr,
                    ContainerPtr,
                    PowerPtr,
					TrayPlacerPtr,
					AirFlowPtr,
                    HeadFillingPtr,
					Containers,
					TrayHeater)
{
}        


// Destructor
COCBStatusSenderDummy::~COCBStatusSenderDummy(void)
{
}


void COCBStatusSenderDummy::Cancel(void)
{
   Suspend();
}

TQErrCode COCBStatusSenderDummy::Reset(void)
{
   CQLog::Write(LOG_TAG_GENERAL,"OCB DUMMY RESET");
   return Q_NO_ERROR;
}


TQErrCode COCBStatusSenderDummy::GetOCBSWVersionMessage(void)
{
   return Q_NO_ERROR;
}
TQErrCode COCBStatusSenderDummy::GetOCB_A2D_SWVersionMessage(void)
{
   return Q_NO_ERROR;
}
TQErrCode COCBStatusSenderDummy::GetOCBHWVersionMessage(void)
{
   return Q_NO_ERROR;
}

//Enter debug mode
TQErrCode COCBStatusSenderDummy::EnterOCBModeMessage(bool DebugMode)
{
    m_DebugMode = DebugMode;
    return Q_NO_ERROR;
}
