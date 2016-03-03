/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Door control                                             *
 * Module Description: This class implement services related to the *
 *                     Door                                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/12/2001                                           *
 ********************************************************************/

#include "Door.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OCBCommDefs.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "AppParams.h"


// Constants:
// When the DOOR is Inside the first bit true,
//When the DOOR is locked the second bit is true.
const BYTE DOOR_OutsideAndUnlocked = 0x00;
const BYTE DOOR_InsideAndUnlocked = 0x01;
const BYTE DOOR_OutsideAndLocked = 0x02; 
const BYTE DOOR_InsideAndLocked = 0x03;

const int DELAY_BETWEEN_DOOR_STATUS_VERIFICATION = 30000; //30 seconds.
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;


// Functions prototipes for test.
// ----------------------------------

// Class CDoor implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
// ------------------------------------------------------
CDoorBase::CDoorBase(const QString& Name) : CQComponent(Name)
{
   m_Waiting=false;
   m_CloseWaiting=false;
   m_AbortPrint=false;
   m_Cancelled = false;
   m_SafetySystem = NULL;
   m_ResetNormallyOpenAckOk = false;
   CAppParams   *ParamsMgr  = CAppParams::Instance();
   if(ParamsMgr != NULL)
   {
	   m_IsSafetySystemExist =   ParamsMgr->SafetySystemExist;
	   m_bIsNormallyOpenInterLockFeedBack = ParamsMgr->NormallyOpenInterLockFeedback;
   }

   m_NumOfDoors = MAX_NUM_OF_DOORS;
   m_FlagResetNormallyOpen= false;

   //INIT_VAR_PROPERTY(DoorClosed,false);
  for(int i = 0 ; i<MAX_NUM_OF_DOORS;++i)
  {
	   DoorClosed[i] = false;
  }
   INIT_VAR_PROPERTY(DoorLocked,false);

   INIT_METHOD(CDoorBase,LockDoor);
   INIT_METHOD(CDoorBase,UnlockDoor);
   INIT_METHOD(CDoorBase,WaitForDoorLockOrUnlock);
   INIT_METHOD(CDoorBase,GetStatus);
   INIT_METHOD(CDoorBase,Enable);
   INIT_METHOD(CDoorBase,Disable);
   INIT_METHOD(CDoorBase,IsDoorReady);
   INIT_METHOD(CDoorBase,CheckIfDoorIsClosed);
   INIT_METHOD(CDoorBase,OpenDoor);
   INIT_METHOD(CDoorBase,CancelLockWait);
   INIT_METHOD(CDoorBase,GetDoorMessage);
   INIT_METHOD(CDoorBase,GetTrayCoverMessage);

   INIT_METHOD(CDoorBase,GetDoorStatusMessage);
   INIT_METHOD(CDoorBase,IsDoorClosed);
   
   m_OCBClient = COCBProtocolClient::Instance();

   //Instance to error handler
   m_ErrorHandlerClient = CErrorHandler::Instance();

  if(CHECK_EMULATION(ParamsMgr->OCB_Emulation))
  {
		m_SafetySystem = new CSafetySystemDummy("SafetySystemDummy");
  }
  else
  {
	 if(m_IsSafetySystemExist)
	 {
		m_SafetySystem = new CSafetySystemBase("SafetySystem");
	 }
	 else
	 {
		m_SafetySystem = new CSafetySystemDummy("SafetySystemDummy");
	 }
  }
}

// Destructor
// -------------------------------------------------------
CDoorBase::~CDoorBase(void)
{
}



TQErrCode CDoorBase::CancelLockWait(void)
{
  m_Cancelled = true;
  return Q_NO_ERROR;
}


bool CDoorBase::IsSafetySystemExist()
{
   return  m_IsSafetySystemExist;
}

//-----------------------------------------------------------------------
// Constructor - Door 
// ------------------------------------------------------
CDoor::CDoor(const QString& Name):CDoorBase(Name)
{
   m_CancelFlag=false;

   m_FlagDoorLockCommandUnderUse=false;
   m_FlagDoorUnlockCommandUnderUse=false;
   m_FlagGetStatusUnderUse=false;

   Locked_AckOk=false;  
   UnLocked_AckOk=false;

   INIT_METHOD(CDoor,Test);
   
   // Install a receive handler for a specific message ID
   m_OCBClient->InstallMessageHandler(OCB_DOOR_LOCK_NOTIFICATION,
									  &DoorLockNotification,
									  reinterpret_cast<TGenericCockie>(this));


}

// Destructor
// -------------------------------------------------------
CDoor::~CDoor(void)
{
}

// BackEnd use this method (in order to use it from the GUI)
BYTE CDoorBase::GetNumOfDoors()
{
	 return m_NumOfDoors;
}
 
//---------------------------------------------

//This procedure get/update the value of Door Closed
bool CDoorBase::IsDoorClosed(void)
{
#ifdef _DEBUG
   if(FindWindow(0, "DoorIsOpen.txt - Notepad"))
		return false;
#endif


  if( m_IsSafetySystemExist)
  {
	 if(( m_IsSafetySystemExist) &&
		( m_SafetySystem!=NULL ) &&
		( m_SafetySystem->IsServiceKeyInserted()))
	 {
       return true;
	 }
  }
    bool Closed; 
   for( int i = 0; i< MAX_NUM_OF_DOORS; ++i)
   {
	   m_MutexDataBase.WaitFor();
			 Closed = DoorClosed[i];
	   m_MutexDataBase.Release();
	   if(Closed == false)
	   {
		return false ; //one of the doors is open
	   }
   }
   return true; //all the are close
}

bool CDoorBase::IsDoorClosed(BYTE index)
{
#ifdef _DEBUG
   if(FindWindow(0, "DoorIsOpen.txt - Notepad"))
		return false;
#endif
	  if( m_IsSafetySystemExist)
	  {
		 if(
		   (m_IsSafetySystemExist) &&
		   ( m_SafetySystem!=NULL ) &&
		   ( m_SafetySystem->IsServiceKeyInserted())
		  )
		 {
		   return true;
		 }
	  }

	bool Closed;
	try
	{
	   m_MutexDataBase.WaitFor();
			 Closed = DoorClosed[index];
	   m_MutexDataBase.Release();
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage("ERORR: Door index is out of range");
		m_MutexDataBase.Release();
	}

	 
		return Closed ; 

}

void CDoorBase::UpdateIfDoorIsClosed(bool Closed, BYTE index)
{
   m_MutexDataBase.WaitFor();
	  DoorClosed[index] = Closed;
   m_MutexDataBase.Release();
}

//This procedure get/update the value of Door locked.
bool CDoorBase::IsDoorLocked (void)
{
   bool Locked;

   m_MutexDataBase.WaitFor();
	  Locked = DoorLocked;
   m_MutexDataBase.Release();
   return Locked;
}  

void CDoorBase::UpdateIfDoorIsLocked(bool Locked)
{
   m_MutexDataBase.WaitFor();
      DoorLocked = Locked;
   m_MutexDataBase.Release();
}




// Get Status of the door
int CDoorBase::GetDoorStatus()
{
  bool IsClosed = IsDoorClosed();
  bool IsLocked = IsDoorLocked();

  if (IsClosed)
  {
    if (IsLocked)
      return DoorCloseLock;
    else
      return DoorCloseUnlock;
  } else
  {
    if (IsLocked)
      return DoorCloseUnlock;
    else
      return DoorOpenUnlock;
  }
}

void CDoorBase::CheckDoorReceivedMessage(int DoorStatus, BYTE index)
{
   switch(DoorStatus)
   {
      case DOOR_OutsideAndUnlocked: //0x00;
      {
         UpdateIfDoorIsClosed(false, index);
         UpdateIfDoorIsLocked(false);
         FrontEndInterface->UpdateStatus(FE_CURRENT_DOOR_STATUS, 0);
         break;
      }
      case DOOR_InsideAndUnlocked:  //0x01;
       {
         UpdateIfDoorIsClosed(true, index);
         UpdateIfDoorIsLocked(false);
         FrontEndInterface->UpdateStatus(FE_CURRENT_DOOR_STATUS, 0);
         break;
       }
      case DOOR_OutsideAndLocked:   //0x02;
      {
      	  UpdateIfDoorIsClosed(false, index);
          UpdateIfDoorIsLocked(true);
	  FrontEndInterface->UpdateStatus(FE_CURRENT_DOOR_STATUS, 0);
	  break;
      }
      case DOOR_InsideAndLocked:    //0x03;
      {
         UpdateIfDoorIsClosed(true, index);
         UpdateIfDoorIsLocked(true);
         FrontEndInterface->UpdateStatus(FE_CURRENT_DOOR_STATUS, 1);
         break;
      }
      default:
      {
      	break;
      }
   }
}


bool CDoorBase::IsDoorReady(void)
{
   return (IsDoorClosed() && IsDoorLocked());

}

TQErrCode CDoorBase::OpenDoor()
{
   for(int i= 0 ; i< MAX_NUM_OF_DOORS ;+i)
   {
	 UpdateIfDoorIsClosed(false, i);
   }
   return Q_NO_ERROR;
}

TQErrCode CDoorBase::CheckIfDoorIsClosed(void)
{
  TQErrCode Err;

  if((Err = GetStatus() ) != Q_NO_ERROR)
     return Err;

  if(IsDoorClosed())
	 return Q_NO_ERROR;

  return Q2RT_DOOR_ERROR;
}
TQErrCode CDoorBase::Enable()
{
  Enable(true);
  return Q_NO_ERROR;
}

CSafetySystemBase* CDoorBase::GetSafetySystemInstance()
{
	return  m_SafetySystem;
}
bool CDoorBase::HandleSafetySystemEmergencyStopButtons(bool showDialog)
{
	if(m_IsSafetySystemExist)   //check only if emergency stop is exist
	{
		int emergencyButtonIndex = 0 ;
		for( int i = 0; i<MAX_NUM_OF_EMERGENCY_STOP_BUTTONS; ++i)
		{
		   if (!m_SafetySystem->IsEmegencyStopButtonUnpressed(i))
		   {
			 if(showDialog)
			 {
				 emergencyButtonIndex = REAR_EMERGENCY_STOP_BUTTON + i ;   // go to the emergencyStop list
				 FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, emergencyButtonIndex , true);
				 FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,true,true);
			 }
			 return false;    // there is no need to check the doors status - there is pop up message
		   }
		}
	  }

	  return true;      // need to check the doors status
}



bool CDoorBase::CheckDoorsAndSafetySystem(bool showDialog, bool& servieKey)
{

   if(HandleSafetySystemEmergencyStopButtons(showDialog))
   {
	  if( (m_IsSafetySystemExist)&&( m_SafetySystem->IsServiceKeyInserted() ))
	  {
		 FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);
		 return true;       // there is no need to check the doors status
	  }
	  else
	  {
		 return   HandleDoorWindow(showDialog, servieKey);
	  }
   }
   else
   {
	return false;
   }
}



TQErrCode CDoorBase::Enable(bool showDialog)
{
  TQErrCode Err;

  
   if (CAppParams::Instance()->KeepDoorLocked)
	  return Q_NO_ERROR;
  time_t CloseDoorEndTime = QGetCurrentTime() + WAIT_FOR_DOOR_CLOSED_MIN*60;
  time_t LockDoorEndTime =  QGetCurrentTime() + OCB_DOOR_ON_TIMEOUT_IN_SEC;

  if((Err = GetStatus() ) != Q_NO_ERROR)
	 return Err;


  if((m_IsSafetySystemExist)&&((Err = m_SafetySystem->GetStatus() ) != Q_NO_ERROR))
	 return Err;


  bool serviceKey = false;
  m_Cancelled = false;
  CheckDoorsAndSafetySystem( showDialog, serviceKey);
  m_AbortPrint = false;



  serviceKey = false;
 while(!CheckDoorsAndSafetySystem( showDialog, serviceKey))
 {
	 if (QGetCurrentTime() > CloseDoorEndTime)
	 {
		Err = Q2RT_DOOR_FAIL_TO_CLOSE_DOOR;
		break;
	 }

     if(m_Cancelled)
     {
       Err = Q2RT_DOOR_CANCEL;
       break;
     }

	 if(
		((Err = GetStatus()) != Q_NO_ERROR)||
		((Err = m_SafetySystem->GetStatus()) != Q_NO_ERROR)
		 )
       break;

     if(m_AbortPrint)
	 {
          Err =Q2RT_SEQUENCE_CANCELED;
          break;
	 }

	 FrontEndInterface->YieldUIThread();
     QSleep(QMsToTicks(100));
 }
   //	FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);

   // If error...
   if(Err != Q_NO_ERROR)
   {
	 // Close door dialog
	 FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, PRINTER_COVER_IS_OPEN_, true);
	 FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);
	 return Err;
   }

   if(IsDoorLocked())
      {
	  // Close door dialog
	  FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, PRINTER_COVER_IS_OPEN_, true);
	  FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);

	  return Q_NO_ERROR;
      }

	  if((Err = LockDoor()) != Q_NO_ERROR)
	  {

		return Err;
	  }


  while(!IsDoorLocked())
	 {

     if (QGetCurrentTime() > LockDoorEndTime)
        {
        CQLog::Write(LOG_TAG_GENERAL,"Failed to lock door");

		// Close door dialog
		FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, PRINTER_COVER_IS_OPEN_, true);
		FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);
		return Q2RT_DOOR_FAIL_TO_LOCK_DOOR;
        }

	 if((Err = GetStatus() ) != Q_NO_ERROR)
     {
		// Close door dialog
		FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, PRINTER_COVER_IS_OPEN_, true);
		FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);
		return Err;
	 }
	 if (  (m_IsSafetySystemExist)&&( m_SafetySystem->IsServiceKeyInserted()) )
	{
		break;     //Service key was inserted, it's technician state, so there is no need to check if the doors are closed
	}

	 QSleep(QMsToTicks(150));
     }

   // Close door dialog
   FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, PRINTER_COVER_IS_OPEN_, true);
   FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,false,true);

   CQLog::Write(LOG_TAG_GENERAL,"Door locked");
   return Q_NO_ERROR;
}

TQErrCode CDoorBase::Disable(void)
{
   TQErrCode Err;

   if (CAppParams::Instance()->KeepDoorLocked)
      return Q_NO_ERROR;
   // todo -oNobody -cNone: make this function clearer and remove redundant communication. Beware of race conditions with other responces/notifications about the door. Arcady. 
   if((Err = GetStatus() ) != Q_NO_ERROR)
      return Err;

   if(!IsDoorLocked())
   {
      CQLog::Write(LOG_TAG_GENERAL,"Door was already unlocked");
      return Q_NO_ERROR;
   }

   if((Err = UnlockDoor()) != Q_NO_ERROR)
      return Err;

   if((Err = WaitForDoorLockOrUnlock(UNLOCK)) != Q_NO_ERROR)
      return Err;

   if((Err = GetStatus() ) != Q_NO_ERROR)
      return Err;

   if(IsDoorLocked())
   {
      CQLog::Write(LOG_TAG_GENERAL,"Fail to unlock door");
      return Q_NO_ERROR;
   }

   CQLog::Write(LOG_TAG_GENERAL,"Door unlocked");
   return Q_NO_ERROR;
}








//OCB_LOCK_DOOR
// Command to lock Door
// ----------------------------------------------------------

TQErrCode CDoor::LockDoor(void)
{

   // Verify if we are not performing other command
   if (m_FlagDoorLockCommandUnderUse)
       throw EDoor("Door:Door LockDoor:two message send in the same time");

   m_FlagDoorLockCommandUnderUse=true;

   m_CancelFlagMutex.WaitFor();
      m_CancelFlag = false;
   m_CancelFlagMutex.Release();

   // Build the Door turn on message
   TOCBLockDoorMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OCB_LOCK_DOOR);

   Locked_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&SendMsg,
                                         sizeof(TOCBLockDoorMessage),
										 LockedAckResponse,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagDoorLockCommandUnderUse=false;
      throw EDoor("OCB didn't get ack for \"LockDoor\" message");
      }

   if(!Locked_AckOk)
      {
      m_FlagDoorLockCommandUnderUse=false;
      throw EDoor("OCB ack failure for \"LockDoor\" message");
      }

   CQLog::Write(LOG_TAG_GENERAL,"Lock Door message sent");
   m_FlagDoorLockCommandUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CDoor::LockedAckResponse(int TransactionId,PVOID Data,
                              unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Door  turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CDoor *InstancePtr = reinterpret_cast<CDoor *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("Door \"LockedAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"LockedAckResponse\" length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("Door \"LockedAckResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"LockedAckResponse\" message id error (0x%X)",
                                       (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_LOCK_DOOR)
      {
      FrontEndInterface->NotificationMessage("Door \"LockedAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"LockedAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Door \"LockedAckResponse\" ack status error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"LockedAckResponse\" ack status error (%d)",
                                       (int)ResponseMsg->AckStatus);
      return; 
      }

   InstancePtr->Locked_AckOk=true;
}


//OCB_UNLOCK_DOOR
// Command to unlock Door 
// ----------------------------------------------------------
TQErrCode CDoor::UnlockDoor(void)
{
   if (CAppParams::Instance()->KeepDoorLocked)
      return Q_NO_ERROR;

   // Verify if we are not performing other command
   if (m_FlagDoorLockCommandUnderUse)
       throw EDoor("Door:Door UNLockDoor:two message send in the same time");

   m_FlagDoorUnlockCommandUnderUse=true;

   m_CancelFlagMutex.WaitFor();
      m_CancelFlag = false;
   m_CancelFlagMutex.Release();

   // Mark start of wait
   m_Waiting = true;
   
   // Build the Door turn on message
   TOCBLockDoorMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OCB_UNLOCK_DOOR);

   UnLocked_AckOk=false;

   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&SendMsg,
                                         sizeof(TOCBLockDoorMessage),
                                         UnlockedAckResponse,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
   {
      m_FlagDoorUnlockCommandUnderUse=false;
      throw EDoor("OCB didn't get ack for \"UnlockDoor\" message");
   }

   if(!UnLocked_AckOk)
   {
      m_FlagDoorUnlockCommandUnderUse=false;
      throw EDoor("OCB ack failure for \"LockDoor\" message");
   }

   m_FlagDoorUnlockCommandUnderUse=false;
   CQLog::Write(LOG_TAG_GENERAL,"Unlock Door message sent");
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CDoor::UnlockedAckResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Door  turn on message
   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   // Get a pointer to the instance
   CDoor *InstancePtr = reinterpret_cast<CDoor *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
      {
      FrontEndInterface->NotificationMessage("Door \"UnlockedAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"UnlockedAckResponse\" length error"); // todo -oNobody -cNone: why LOG_TAG_HEAD_FILLING ? 
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
      {
      FrontEndInterface->NotificationMessage("Door \"UnlockedAckResponse\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"UnlockedAckResponse\" message id error (0x%X)", // todo -oNobody -cNone: why LOG_TAG_HEAD_FILLING ? 
                                       (int)ResponseMsg->MessageID);
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_UNLOCK_DOOR)
      {
      FrontEndInterface->NotificationMessage("Door \"UnlockedAckResponse\" responded message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"UnlockedAckResponse\" responded message id error (0x%X)",
                                       (int)ResponseMsg->RespondedMessageID);
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Door \"UnlockedAckResponse\" ack status error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"UnlockedAckResponse\" ack status error (%d)", // todo -oNobody -cNone: why LOG_TAG_HEAD_FILLING ?
                                       (int)ResponseMsg->AckStatus);
      return;
      }

   InstancePtr->UnLocked_AckOk=true;
}

// Wait procedure to Door Lock and Unlock - notification
//---------------------------------------------------------
TQErrCode CDoor::WaitForDoorClosed(void)
{
  // Mark start of wait
   m_CloseWaiting = true;

   // Wait for Notify message that Door are ON.
   QLib::TQWaitResult WaitResult = m_SyncEventWaitForDoorClosed.
           WaitFor(QMinutesToTicks(WAIT_FOR_DOOR_CLOSED_MIN));

   // Mark that we no longer waiting
   m_CloseWaiting = false;

   m_CancelFlagMutex.WaitFor();
   if(m_CancelFlag)
      {
      m_CancelFlag = false;
      m_CancelFlagMutex.Release();
      throw ESequenceCanceled("Door Notification message canceled");
      }
   m_CancelFlagMutex.Release();

   if(WaitResult != QLib::wrSignaled)
      {
      //throw EDoor("Timeout while waiting for Door closed",Q2RT_DOOR_FAIL_TO_CLOSE_DOOR);
      return Q2RT_DOOR_FAIL_TO_CLOSE_DOOR;
      }

   return Q_NO_ERROR;
}


// Wait procedure to Door Lock and Unlock - notification
//---------------------------------------------------------
TQErrCode CDoor::WaitForDoorLockOrUnlock(bool WaitForLock)
{
   QLib::TQWaitResult WaitResult;
   
   if (WaitForLock == true)
   {
	 // Wait for Notify message that Door are ON.
	 WaitResult = m_SyncEventWaitDoorMessage.WaitFor(QSecondsToTicks(OCB_DOOR_ON_TIMEOUT_IN_SEC)); // todo -oNobody -cNone: DANGER: a thread waiting for lock may receive this event from an unlock operation.
   } else
   {
     // Wait for Notify message that Door are OFF.
     WaitResult = m_SyncEventWaitDoorMessage.WaitFor(QSecondsToTicks(OCB_DOOR_OFF_TIMEOUT_IN_SEC));
   }

   // Mark that we no longer waiting
   m_Waiting = false;

   m_CancelFlagMutex.WaitFor();
   if(m_CancelFlag)
   {
     m_CancelFlag = false;
     m_CancelFlagMutex.Release();
     throw ESequenceCanceled("Door Notification message canceled");
   }
   m_CancelFlagMutex.Release();

   if(WaitResult != QLib::wrSignaled)
   {
     CQLog::Write(LOG_TAG_HEAD_FILLING,"Oved: Timeout while waiting for m_SyncEventWaitDoorMessage"); // todo -oNobody -cNone: why LOG_TAG_HEAD_FILLING ?
     return Q2RT_DOOR_FAIL_TO_LOCK_DOOR;
   }

   return Q_NO_ERROR;
}


// Notification Of Door lock status
// --------------------------------------------------------------------
void CDoor::DoorLockNotification(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   TOCBDoorLockNotificationResponse *DoorMsg =
       static_cast<TOCBDoorLockNotificationResponse *>(Data);

   // Get a pointer to the instance
   CDoor *InstancePtr = reinterpret_cast<CDoor *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBDoorLockNotificationResponse))
   {
      FrontEndInterface->NotificationMessage("Door \"DoorLockNotification\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"DoorLockNotification\" length error"); // todo -oNobody -cNone: why head filling
      return;
   }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(DoorMsg->MessageID) != OCB_DOOR_LOCK_NOTIFICATION)
   {
      FrontEndInterface->NotificationMessage("Door \"DoorLockNotification\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"DoorLockNotification\" message id error (0x%X)",
                                       (int)DoorMsg->MessageID);
      return;
   }
        
   InstancePtr->AckToOcbNotification(OCB_DOOR_LOCK_NOTIFICATION,
                                     TransactionId,
                                     ACK_STATUS_SUCCESS,
                                     Cockie);

   InstancePtr->m_CancelFlagMutex.WaitFor();
   if(!InstancePtr->m_CancelFlag)
   {
	  for (BYTE i = 0; i < MAX_NUM_OF_DOORS; ++i)
	  {   
		  InstancePtr->CheckDoorReceivedMessage( static_cast<int>(DoorMsg->DoorStatus), i);
		  InstancePtr->DisplayStatus();

		  if(InstancePtr->m_CloseWaiting)
			 InstancePtr->m_SyncEventWaitForDoorClosed.SetEvent();

		  if(InstancePtr->m_Waiting)
		  {
			InstancePtr->m_SyncEventWaitDoorMessage.SetEvent();
		  }
	  }
   }
   InstancePtr->m_CancelFlagMutex.Release();
}

// Get Door Status
//-------------------------------
TQErrCode CDoor::GetStatus(void)
{
   // Verify if we are not performing other command
   if(m_FlagGetStatusUnderUse)
     CQLog::Write(LOG_TAG_GENERAL,"Door \"GetStatus\" re-entry problem");

   m_FlagGetStatusUnderUse=true;

   TOCBGetDoorStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OCB_GET_DOOR_STATUS);
   
   // Send a Turn ON request
   if (m_OCBClient->SendInstallWaitReply(&GetMsg,
                                         sizeof(TOCBGetDoorStatusMessage),
                                         GetStatusAckResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_GENERAL,"OCB didn't get ack for \"GetStatus\" message");
      }

   m_FlagGetStatusUnderUse=false;
   return Q_NO_ERROR;
}


// Acknolodges for Command turn on/off
// -------------------------------------------------------
void CDoor::GetStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie)
{
   // Build the Door turn on message
   TOCBDoorStatusResponse *StatusMsg =
                   static_cast<TOCBDoorStatusResponse *>(Data);
   
   // Get a pointer to the instance
   CDoor *InstancePtr = reinterpret_cast<CDoor *>(Cockie);

   //Verify size of message
   if(DataLength != sizeof(TOCBDoorStatusResponse))
      {
      FrontEndInterface->NotificationMessage("Door \"GetStatusAckResponse\" length error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"GetStatusAckResponse\" length error");
      return;
      }

   if (static_cast<int>(StatusMsg->MessageID) != OCB_DOOR_STATUS)
      {
      FrontEndInterface->NotificationMessage("Door \"NotificationDoorOpened\" message id error");
      CQLog::Write(LOG_TAG_HEAD_FILLING,"Door \"NotificationDoorOpened\" message id error (0x%X)",
                                       (int)StatusMsg->MessageID);
      return;
      }
	  for (BYTE doorIndex = 0; doorIndex < MAX_NUM_OF_DOORS; ++doorIndex)
	  {
			  InstancePtr->CheckDoorReceivedMessage(static_cast<int>(StatusMsg->DoorStatus[doorIndex]), doorIndex);
	  }
}


//----------------------------------------------------------------
void CDoor::AckToOcbNotification (int MessageID,
                                  int TransactionID,
                                  int AckStatus,
                                  TGenericCockie Cockie)
{
   // Build the Door turn on message
   TOCBAck AckMsg;
   
   // Get a pointer to the instance
   CDoor *InstancePtr = reinterpret_cast<CDoor *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OCB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OCBClient->SendNotificationAck(TransactionID,
                                                 &AckMsg,
                                                 sizeof(TOCBAck));
}

//----------------------------------------------------------------
void CDoor::Cancel(void)
{
  m_AbortPrint=true;

  if(m_Waiting || m_CloseWaiting)
  {
    m_CancelFlagMutex.WaitFor();

    m_CancelFlag = true;

    if(m_Waiting)
       m_SyncEventWaitDoorMessage.SetEvent();
    if(m_CloseWaiting)
         m_SyncEventWaitForDoorClosed.SetEvent();

    m_CancelFlagMutex.Release();
  }
}



//
// reset the immobility detection mechanism trigger (for doors & safety system)
// THIS MESSAGE HAVE TO BE SENT ONLY ONCE, HERE IN POWER-UP SEQUENCE
// It is relevent for 3 doors + 2 emegency Stop
// -------------------------------------------------------------------
TQErrCode CDoorBase::ResetNormallyOpenInetLockFeedBack()
{
   if (m_FlagResetNormallyOpen)
	   CQLog::Write(LOG_TAG_GENERAL,"Door \"ResetNormallyOpen\" two message send in the same time");

   m_FlagResetNormallyOpen=true;

   m_ResetNormallyOpenAckOk=false;

  TOCBResetNormallyOpenMessage NormallyOpenMessage;
  NormallyOpenMessage.MessageID           = OCB_RESET_NORMALLY_OPEN;
  NormallyOpenMessage.NormallyOpenStatus  = static_cast<BYTE>(m_bIsNormallyOpenInterLockFeedBack);

  // Send a reset normally open message
  if (m_OCBClient->SendInstallWaitReply(&NormallyOpenMessage,
						  sizeof(TOCBResetNormallyOpenMessage),
						  ResetNormallyOpenAck,
						  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
	 CQLog::Write(LOG_TAG_GENERAL,"OCB did not ack for Doors \"Reset Doors Normally open\" message");
  }

   m_FlagResetNormallyOpen = false;
   return Q_NO_ERROR;
}

// -------------------------------------------------------
void CDoorBase::ResetNormallyOpenAck (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CDoor *InstancePtr = reinterpret_cast<CDoor *>(Cockie);

   TOCBAck *ResponseMsg = static_cast<TOCBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOCBAck))
	  {
	  FrontEndInterface->NotificationMessage("Doors & safety system\"ResetDoor&SafetySystemAck\" length error");
	  CQLog::Write(LOG_TAG_POWER,"Doors & safety system \"ResetDoor&SafetySystemAck\" length error");
	  return;
	  }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OCB_ACK)
	  {
	  FrontEndInterface->NotificationMessage("Doors & safety system\"ResetDoor&SafetySystemAck\"  MSGID error");
	  CQLog::Write(LOG_TAG_POWER,"Doors & safety system\"ResetDoor&SafetySystemAck\"  msg id error (0x%X)",
											  (int)ResponseMsg->MessageID);
	  return;

	  }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OCB_RESET_NORMALLY_OPEN)
	  {
	  FrontEndInterface->NotificationMessage("Doors & safety system\"ResetDoor&SafetySystemAck\"  Responded msg Id error");
	  CQLog::Write(LOG_TAG_POWER,"Doors & safety system\"ResetDoor&SafetySystemAck\"  Responded msg Id error(0x%X)",
									 (int)ResponseMsg->RespondedMessageID);
      return;

      }

   if (ResponseMsg->AckStatus)
	  {
	  FrontEndInterface->NotificationMessage("Doors & safety system\"ResetDoor&SafetySystemAck\" Ack error");
	  CQLog::Write(LOG_TAG_POWER,"Doors & safety system\"ResetDoor&SafetySystemAck\" Ack error(%d)",
									 (int)ResponseMsg->AckStatus);
	  return;
	  }

   InstancePtr->m_ResetNormallyOpenAckOk=true;

}

TQErrCode CDoor::Test (void)
{
   TQErrCode  ErrCode;
   int Closed,Locked;

   //ErrCode = GetStatus();

   Closed=(int)IsDoorClosed();
   Locked=(int)IsDoorLocked();
   QMonitor.Printf("Door:Closed %d -- Locked %d",Closed,Locked);

   if(UnlockDoor() == Q_NO_ERROR)
	  ErrCode = WaitForDoorLockOrUnlock(false);

   Closed=(int)IsDoorClosed();
   Locked=(int)IsDoorLocked();
   QMonitor.Printf("Door:Closed %d -- Locked %d",Closed,Locked);

   if (LockDoor()== Q_NO_ERROR)
	  ErrCode = WaitForDoorLockOrUnlock(true);

   Closed=(int)IsDoorClosed();
   Locked=(int)IsDoorLocked();
   QMonitor.Printf("Door:Closed %d -- Locked %d",Closed,Locked);

   //ErrCode = GetStatus();
   return ErrCode;
}
 
// Dummy Constructor
CDoorDummy::CDoorDummy(const QString& Name):CDoorBase(Name)
{
     m_IsSafetySystemExist = false;


}

// Destructor
// -------------------------------------------------------
CDoorDummy::~CDoorDummy(void)
{
}


// Dummy procedures for CDoorDummy
//-------------------------------------------------------------
TQErrCode CDoorDummy::WaitForDoorLockOrUnlock(bool)
{
   return Q_NO_ERROR;
}

TQErrCode CDoorDummy::WaitForDoorClosed(void)
{
   if(QMonitor.AskYesNo("Press Yes to close door or No to Abort"))
   {
	  	for (BYTE doorIndex = 0; doorIndex < MAX_NUM_OF_DOORS; ++doorIndex)
		{
	 		 UpdateIfDoorIsClosed(true, doorIndex);
		}
    	return Q_NO_ERROR;
   }
   
   for (BYTE doorIndex = 0; doorIndex < MAX_NUM_OF_DOORS; ++doorIndex)
   {
   	    UpdateIfDoorIsClosed(false, doorIndex);
   }
   return Q2RT_DOOR_FAIL_TO_CLOSE_DOOR;
}


TQErrCode CDoorDummy::LockDoor(void)
{
	for (BYTE doorIndex = 0; doorIndex < MAX_NUM_OF_DOORS; ++doorIndex)
	{
	   UpdateIfDoorIsClosed(true, doorIndex);
    }
	   UpdateIfDoorIsLocked(true);

   return Q_NO_ERROR;
}

//Command to unluck door.
TQErrCode CDoorDummy::UnlockDoor(void)
{
   if (CAppParams::Instance()->KeepDoorLocked)
      return Q_NO_ERROR;

   UpdateIfDoorIsLocked(false);
   return Q_NO_ERROR;
}

//This procedure send Door Get status message.
TQErrCode CDoorDummy::GetStatus(void)
{
	for(BYTE doorIndex = 0 ; doorIndex<MAX_NUM_OF_DOORS; ++doorIndex)
	{
	  UpdateIfDoorIsClosed(true, doorIndex);
	}
	return Q_NO_ERROR;
}

TQErrCode CDoorDummy::Enable(bool showDialog)
{

return Q_NO_ERROR;

}


TQErrCode CDoorDummy::ResetNormallyOpenInetLockFeedBack()
{
   return Q_NO_ERROR;
}


//-------------------------------------------------------------
//-------------------------------------------------------------



/*******************************************************************************************************************/
#pragma warn -8008 // Disable "Condition is always true" warning (MAX_NUM_OF_DOORS)
/*******************************************************************************************************************/
/*******************************************************************************************************************/
#pragma warn -8066 // Disable ""Unreachable code" warning (MAX_NUM_OF_DOORS)
/*******************************************************************************************************************/
void CDoorBase::DisplayStatus(void)
{
	static const char *DoorArray[] ={"Door", "Side door", "Front right door", "Front left door", "Service door"};
	int i = 0, j=0;
	 if( MAX_NUM_OF_DOORS > 1)
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
}

void CDoorBase::PrintOpenDoors(void) //this method prints only open doors
{
	static const char *DoorArray[] ={"Door", "Side door", "Front right door", "Front left door", "Service door"};
	BYTE i = 0, j=0;

	 if( MAX_NUM_OF_DOORS > 1)
	 {
		++j;
	 }
	for( i; i<MAX_NUM_OF_DOORS; ++i, ++j)
	{
	   bool bIsDoorClose = IsDoorClosed(i);
	   if(!bIsDoorClose)
	   {
		CQLog::Write(LOG_TAG_GENERAL,"%s Status: %s  %s",
				  DoorArray[j],
				 bIsDoorClose ? "Closed" : "Not Closed",
				  IsDoorLocked() ? "Locked" : "Not Locked");
	   }
	}
}

bool CDoorBase::HandleDoorWindow(bool showDialog, bool& servieKey)
{


	int i =0;
	for(;i<MAX_NUM_OF_DOORS; i++)
	{
		 if (!IsDoorClosed(i))
		 {
			if(showDialog)
			{
				   if(MAX_NUM_OF_DOORS > 1)
				   {
						++i;       // The first door in the list describe only one door
				   }
				   FrontEndInterface->UpdateStatus(FE_SET_DOOR_DLG_LABEL, i, true);
					FrontEndInterface->EnableDisableControl(FE_CTRL_DOOR_DLG,true,true);
			}
			return false;
		 }
	}

   return true;
}


QString CDoorBase::GetDoorMessage()
{
  if(MAX_NUM_OF_DOORS > 1)
  {
	return "Doors are closed.";
  }
  return "Cover is closed.";
}

QString CDoorBase::GetTrayCoverMessage()
{
  if(MAX_NUM_OF_DOORS > 1)
  {
	return "Build tray is clear(empty) and the doors are closed.";
  }
  return "Build tray is clear(empty) and the cover is closed.";
}


QString CDoorBase::GetDoorStatusMessage()
{
   if(MAX_NUM_OF_DOORS > 1)
   {
	return "Please check if the doors are closed.";
   }
   return "Please check if the door is closed.";
}
/*******************************************************************************************************************/
#pragma warn .8066 // Enable "Unreachable code" warning (MAX_NUM_OF_DOORS)
/*******************************************************************************************************************/
/*******************************************************************************************************************/
#pragma warn .8008 // Enable "Condition is always true" warning (MAX_NUM_OF_DOORS)
/*******************************************************************************************************************/