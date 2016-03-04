/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Door 8051 h/w                                            *
 * Module Description: This class implement services related to the *
 *                     Eden Door Open/Close and Locked              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/

#ifndef _DOOR_H_
#define _DOOR_H_

#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"

enum {DoorOpenUnlock  = 0,
      DoorCloseUnlock = 1,
      DoorCloseLock   = 2,
      DoorOpenLock    = 3};


const int OCB_DOOR_ON_TIMEOUT_IN_SEC = 1000;
const int OCB_DOOR_OFF_TIMEOUT_IN_SEC = 2;
const int WAIT_FOR_DOOR_CLOSED_MIN = 30;

const bool LOCK   = true;
const bool UNLOCK = false;

typedef unsigned int UINT;

// Exception class for all the QLib RTTI elements
class EDoor : public EQException {
  public:
    EDoor(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};  


// C Door control class
class CDoorBase : public CQComponent{    
private:    
   DEFINE_VAR_PROPERTY(bool,DoorClosed);   //Inside=Closed (true)-- Outside=Open
   DEFINE_VAR_PROPERTY(bool,DoorLocked);

   bool m_Cancelled;

protected:
    COCBProtocolClient *m_OCBClient;
    
    // Flags to control the waiting and cancel operation
    bool m_Waiting;
    bool m_CloseWaiting;
    bool m_AbortPrint;
    
    // Protection mutex for database
    CQMutex m_MutexDataBase;
    
    // Flag that permit to start send get status message each period time
    //bool m_FlagCanSendGetStatusMsg;

    //Special procedure to update Door Database
    void UpdateIfDoorIsClosed(bool);
    void UpdateIfDoorIsLocked(bool);
    void CheckDoorReceivedMessage(int);

    CErrorHandler *m_ErrorHandlerClient;

  public:
    // Constructor
    CDoorBase(const QString& Name);

    // Destructor
    ~CDoorBase(void);

    // Cancel the door lock waiting 
    DEFINE_METHOD(CDoorBase,TQErrCode,CancelLockWait);

    //Special procedure to check Door Database
    bool IsDoorClosed (void);
    bool IsDoorLocked(void);

    // Get Status of the door
    int GetDoorStatus();

    // Command to lock door.
    DEFINE_V_METHOD(CDoorBase,TQErrCode,LockDoor) = 0;

    // Command to unlock door.
    DEFINE_V_METHOD(CDoorBase,TQErrCode,UnlockDoor) = 0;

    // Command to wait for answer from 8051 for Door notification.
    DEFINE_V_METHOD_1(CDoorBase,TQErrCode,WaitForDoorLockOrUnlock,bool) = 0;
    DEFINE_V_METHOD(CDoorBase,TQErrCode,WaitForDoorClosed)=0;


    //This procedure send Get status message from Door-OCB(8051).
    DEFINE_V_METHOD(CDoorBase,TQErrCode,GetStatus)=0;

    virtual void Cancel(void)=0;

   //	DEFINE_METHOD(CDoorBase,TQErrCode,Enable);
    DEFINE_METHOD(CDoorBase,TQErrCode,Disable);
    DEFINE_METHOD(CDoorBase,TQErrCode,CheckIfDoorIsClosed);
    DEFINE_METHOD(CDoorBase,bool,IsDoorReady);
	DEFINE_METHOD(CDoorBase,TQErrCode,OpenDoor);
	DEFINE_METHOD(CDoorBase,TQErrCode,Enable);
    TQErrCode Enable(bool showDialog);
    void DisplayStatus(void);
};


class CDoor : public CDoorBase {

 private:
   // Semaphore That verify if Ack is received or if Notify response
   // was received.
   CQEvent m_SyncEventWaitDoorMessage;
   CQEvent m_SyncEventWaitForDoorClosed;

   bool m_CancelFlag;

   // Protection mutex for the cancel flag
   CQMutex m_CancelFlagMutex;

   //Flag Door during command performance.
   bool m_FlagDoorLockCommandUnderUse;
   bool m_FlagDoorUnlockCommandUnderUse;
   bool m_FlagGetStatusUnderUse;

   //Ack flag
   bool Locked_AckOk;
   bool UnLocked_AckOk;

   //This procedure is the callback for Set Door Lcked
   static void LockedAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Set Door UNLcked
   static void UnlockedAckResponse(int TransactionId,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);

   //Get status ack response
   static void GetStatusAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   // Notification Call Backs
   static void DoorLockNotification(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OCb
   void AckToOcbNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);

   // Door recovery procedure.
   void Recovery (void);

public:
    // Constructor
    CDoor(const QString& Name);

    // Destructor
    ~CDoor(void);
       
    //This procedure send Door Get status message.
    TQErrCode GetStatus(void);
    
    // Command to lovk door.
    TQErrCode LockDoor(void);
    
    // Command to unluck door.
    TQErrCode UnlockDoor(void);
                                                    
    //This procedure is called in order to wait for door to be locked/unlucked
    TQErrCode WaitForDoorLockOrUnlock(bool);
    TQErrCode WaitForDoorClosed(void);

    //Test procedure
    DEFINE_METHOD(CDoor,TQErrCode,Test);

    void Cancel(void);
};


class CDoorDummy : public CDoorBase {
public:
    // Constructor
    CDoorDummy(const QString& Name);

    // Destructor
    ~CDoorDummy(void);

    //This procedure send Door Get status message.
    TQErrCode GetStatus(void);
    
    // Command to lovk door.
    TQErrCode LockDoor(void);

    //Command to unluck door.
    TQErrCode UnlockDoor(void);

    //This procedure is called in order to wait for door to be locked/unlocked
    TQErrCode WaitForDoorLockOrUnlock(bool);
    TQErrCode WaitForDoorClosed(void);
    void Cancel(void){}
};        

#endif

