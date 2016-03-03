/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Safety System                                           *
 * Module Description: This class implement services related to the *
 *                     Safety System                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Guy Ezra                                                 *
 * Start date: 2/11/2012                                            *
 ********************************************************************/


#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "SafetySystem.h"
#include "QMonitor.h"


/*********************************
 *                               *
 *    CSafetySystemBase class   *
 *                               *
 *********************************/
CSafetySystemBase::CSafetySystemBase(const QString& Name) : CQComponent(Name)
{
   m_OCBClient = COCBProtocolClient::Instance();
   m_ErrorHandlerClient = CErrorHandler::Instance();
   for(int i = 0 ; i < MAX_NUM_OF_EMERGENCY_STOP_BUTTONS ; ++i )
   {
	  m_EmergencyStopButtonsStatus[ i ] =  UN_PRESSED;
   }
   m_FlagGetStatusUnderUse = false;
   m_ServiceKeyStatus = 0;
   m_bNeedToUpdate = false;

   INIT_METHOD(CSafetySystemBase,IsEmegencyStopButtonUnpressed);
}

CSafetySystemBase::~CSafetySystemBase()
{
}




// Get Safety System Status
//-------------------------------
TQErrCode CSafetySystemBase::GetStatus(void)
{

   // Verify if we are not performing other command
   if( m_FlagGetStatusUnderUse )
   {
	 CQLog::Write(LOG_TAG_GENERAL,"Safety System \"Get Status\" re-entry problem");
   }

   m_FlagGetStatusUnderUse = true;

   TOCBGetSafetySystemStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_SAFETY_SYSTEM_STATUS_MSG);

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,
										 sizeof(TOCBGetSafetySystemStatusMessage),
										 GetStatusAckResponse,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)


	  {
		  CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"GetStatus\" message");
	  }

   m_FlagGetStatusUnderUse=false;
   return Q_NO_ERROR;

}


#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CSafetySystemBase::GetStatusAckResponse(int TransactionId,PVOID Data,
								 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the SafetySystem turn on message
   TOCBSafetySystemStatusResponse *StatusMsg = static_cast<TOCBSafetySystemStatusResponse *>(Data);
   // Get a pointer to the instance
   CSafetySystemBase *InstancePtr = reinterpret_cast<CSafetySystemBase *>(Cockie);



   //Verify size of message
   if(DataLength != sizeof(TOCBSafetySystemStatusResponse))
	  {
	  FrontEndInterface->NotificationMessage("SafetySystem \"GetStatusAckResponse\" length error");
	  CQLog::Write(LOG_TAG_HEAD_FILLING,"SafetySystem \"GetStatusAckResponse\" length error");
	  return;
	  }

   if (static_cast<int>(StatusMsg->MessageID) != OCB_SEND_SAFETY_SYSTEM_STATUS_MSG)
	  {
	  FrontEndInterface->NotificationMessage("SafetySystem \"Notification Safety System Opened\" message id error");
	  CQLog::Write(LOG_TAG_HEAD_FILLING,"SafetySystem \"Notification Safety System\" message id error (0x%X)",(int)StatusMsg->MessageID);
	  return;
	  }

	  InstancePtr->UpdateSafetySystemStatus( StatusMsg->EmergencyStopButtonsStatus[0],StatusMsg->EmergencyStopButtonsStatus[1], StatusMsg->ServiceKeyStatus );
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"


 void CSafetySystemBase::UpdateSafetySystemStatus(BYTE emergencyStopRear, BYTE emergencyStopFront, BYTE serviceKey)
 {
	  m_MutexDataBase.WaitFor();

	  m_EmergencyStopButtonsStatus[0] = emergencyStopRear;
	  m_EmergencyStopButtonsStatus[1] = emergencyStopFront;
	  if(m_ServiceKeyStatus != serviceKey)
	  {
		m_ServiceKeyStatus = serviceKey;
		m_Maintenance.WaitFor();
		m_bNeedToUpdate = true;
		m_Maintenance.Release();
	  }

   m_MutexDataBase.Release();

 }

 bool CSafetySystemBase::IsServiceKeyInserted()
 {
		bool bWasInserted;
		m_MutexDataBase.WaitFor();
		 bWasInserted =  m_ServiceKeyStatus ;
	   m_MutexDataBase.Release();
	   return bWasInserted;
 }

bool CSafetySystemBase::IsEmegencyStopButtonUnpressed (BYTE buttonIndex)
{
	  bool bUnPressed;

	  try
	{
	   m_MutexDataBase.WaitFor();
			 bUnPressed = m_EmergencyStopButtonsStatus[buttonIndex];
	   m_MutexDataBase.Release();
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage("ERORR: Emergency Stop index is out of range");
		m_MutexDataBase.Release();
	}

	   return bUnPressed ;
}
 bool CSafetySystemBase::GetUpdateState()
{
	bool bNeedToUpdate;
	m_Maintenance.WaitFor();
	bNeedToUpdate = m_bNeedToUpdate ;
	m_Maintenance.Release();

	return bNeedToUpdate ;
}
void CSafetySystemBase::UpdatePopUpMessage()
{
	if(GetUpdateState())
	{
		if(IsServiceKeyInserted())
		{
			FrontEndInterface->EnableDisableControl(FE_CTRL_SERVICE_KEY_DLG,true,true);    //show message box
		}
		else
		{
			 FrontEndInterface->EnableDisableControl(FE_CTRL_SERVICE_KEY_DLG,false,true);    //
		}
		m_Maintenance.WaitFor();
		m_bNeedToUpdate = false;
		m_Maintenance.Release();
	}
}

 /*********************************************************************************************************************************************


/***********************************
 *                                 *
 *    CSafetySystemDummy class    *
 *                                 *
 ***********************************/

 CSafetySystemDummy::CSafetySystemDummy(const QString& Name) : CSafetySystemBase(Name)
{
 m_OCBClient = COCBProtocolClient::Instance();
   m_ErrorHandlerClient = CErrorHandler::Instance();
   for(int i = 0 ; i < MAX_NUM_OF_EMERGENCY_STOP_BUTTONS ; ++i )
   {
	  m_EmergencyStopButtonsStatus[ i ] =  PRESSED;
   }

   m_ServiceKeyStatus = SERVICE_KEY_WAS_NOT_INSERTED;
}

CSafetySystemDummy::~CSafetySystemDummy()
{
}
TQErrCode CSafetySystemDummy::GetStatus(void)
{
	return Q_NO_ERROR;
}

bool CSafetySystemDummy::IsAllEmegencyStopButtonsUnpressed (void)
{
	 return true;
}
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
bool CSafetySystemDummy::IsEmegencyStopButtonUnpressed (BYTE buttonIndex)
{
     return true;
}
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"

