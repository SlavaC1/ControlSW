/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Roller                                                   *
 * Module Description: This class implement services related to the *
 *                     Roller on 0\8051 h/w.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/
 
 
#ifndef _ROLLER_H_
#define _ROLLER_H_

#include "OHDBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "QTimer.h"
#include "QMutex.h"
#include "QMessageQueue.h"
#include "Errorhandler.h"
#include "Q2RTErrors.h"

const bool ROLLER_ON  = true;
const bool ROLLER_OFF = false;
const int OHDB_ROLLER_ON_TIMEOUT_IN_SEC = 150;

typedef unsigned int UINT;

// Exception class for all the QLib RTTI elements
class ERoller : public EQException {
  public:
    ERoller(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// C Roller control class
class CRollerBase : public CQComponent{
 private:
    DEFINE_VAR_PROPERTY(bool,RollerOnOff);
    UINT m_Velocity;
    UINT m_RetriesCounter;
    UINT m_BumperSensitivity;
    bool m_BumperImpact;

 protected:
    COHDBProtocolClient *m_OHDBClient;
    
    // Flags to control the waiting and cancel operation
    bool m_Waiting;

    CQMutex m_MutexDataBase;

    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr; 

    //Special procedure to update Roller Database
    void UpdateRollerVelocity(UINT);
    void UpdateRetriesCounter(UINT);
    void UpdateBumperSensitivity(UINT);  
    void UpdateBumperImpact (bool);
                 
  public:
    // Constructor
    CRollerBase(const QString& Name);

    // Destructor
    ~CRollerBase(void);

    // Command to turn on/off the 8051 h/w.
    DEFINE_V_METHOD_1(CRollerBase,TQErrCode,SetRollerOnOff,bool) = 0;
                                                
    //This procedure send Get status message to OHDB(8051).
    DEFINE_V_METHOD(CRollerBase,TQErrCode,GetStatus)=0;

    //This procedure send Set parameters message to OHDB(8051).
    DEFINE_V_METHOD_1(CRollerBase,TQErrCode,SetRollerParms,int)=0;

    //This procedure send Set parameters message to OHDB(8051).
    DEFINE_METHOD(CRollerBase,TQErrCode,SetDefaultRollerParms);
    
    // Enable/Disable the bumper
    DEFINE_V_METHOD_1(CRollerBase,TQErrCode,BumperEnableDisable,bool)=0;

    //This procedure send Set parameters message to OHDB(8051).
    DEFINE_V_METHOD_3(CRollerBase,TQErrCode,SetBumperParams,int /*Sensitivity*/,int /*ResetTime*/,int /*ImpactCounter*/)=0;
    //This procedure send Set parameters message to OHDB(8051).
    DEFINE_METHOD(CRollerBase,TQErrCode,SetDefaultBumperParams);

    // Enable/Disable the bumper PEG mechanism
    DEFINE_V_METHOD_1(CRollerBase,TQErrCode,EnableDisableBumperPeg,bool)=0;

    DEFINE_METHOD(CRollerBase,TQErrCode,ResetBumperImpact);

    //This procedure update the value of Roller on off of OHDB(8051).
    DEFINE_METHOD_1(CRollerBase,TQErrCode,UpdateRollerOnOffValue,bool);

    //This procedure get the values status of Roller/Bumper of OHDB(8051).
    DEFINE_METHOD(CRollerBase,bool,GetRollerOnOffValue);   
    DEFINE_METHOD(CRollerBase,UINT,GetRollerVelocity);
    DEFINE_METHOD(CRollerBase,UINT,GetRetriesCounter);
    DEFINE_METHOD(CRollerBase,UINT,GetBumperSensivity);
    DEFINE_METHOD(CRollerBase,bool,GetBumperImpact);

    virtual void Cancel(void) {}
};


class CRoller : public CRollerBase {

 private:
   int m_RollerTimeStamp;
   int m_RollerStatusTimeStamp;   
   int m_RollerErrorTimeStamp;

   UINT m_LastRollerRetriesCount;
   UINT m_LastRollerRetryCheck;
   UINT m_TimeBetweenRollerRetryChecks;
   UINT m_MaxRollerRetries;


   //Flag Roller during command performance.
    bool m_FlagRollerOnOffUnderUse;
    bool m_FlagSetCommandUnderUse;
    bool m_FlagGetStatusUnderUse;
    bool m_FlagSetBumperCommandUnderUse;  

    bool m_CancelFlag;

    //Ack Ok
    bool m_SetOnOff_AckOk;
    bool m_SetParm_AckOk;
    bool m_SetBumper_AckOk;
    bool m_BumperEnableAckOk;
    bool m_BumperPegOnOffAckOk;
    CQMutex m_SetBumperPegOnOffMutex;

    // Protection mutex for the cancel flag
    CQMutex m_CancelFlagMutex;

   //This procedure is the callback for Set Roller OnOff OHDB
   static void OnOffAckResponse (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Set params message
   static void SetParmsAckResponse(int TransactionId,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);

   //Get status ack response
   static void GetStatusAckResponse(int TransactionId,PVOID Data,
                             unsigned DataLength,TGenericCockie Cockie);

   static void NotificationErrorMessage (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   static void NotificationBumperMessage (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);
   //This procedure is the callback for Set bumper sensitivity params message
   static void SetBumperAckResponse(int TransactionId,PVOID Data,
                             unsigned DataLength,TGenericCockie Cockie);
   static void BumperAckCallback(int TransactionID,PVOID Data,
                             unsigned DataLength,TGenericCockie Cockie);

   static void BumperPegAckCallback(int TransactionID,PVOID Data,
                             unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OHDB
   void AckToOHDBNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);

   // Tray Heater recovery procedure.
   void Recovery (void);

   TQErrCode CheckRollerStatus();

public:
    // Constructor
    CRoller(const QString& Name);

    // Destructor
    ~CRoller(void);   
    
    // Command to turn on/off the 8051 h/w.
    TQErrCode SetRollerOnOff (bool);
        
    //This procedure send Get status message to OHDB(8051).
    TQErrCode GetStatus(void);

    //This procedure send Set parameters message to OHDB(8051).
    TQErrCode SetRollerParms (int);

    //This procedure set bumper parameters to OHDB.
    TQErrCode SetBumperParams (int,int,int);

    //Test procedure
    DEFINE_METHOD(CRoller,TQErrCode,Test);
    // Enable/Disable the bumper
    TQErrCode BumperEnableDisable(bool Enable);

    // Enable/Disable the bumper PEG mechanism
    TQErrCode EnableDisableBumperPeg(bool Enable);

    void Cancel(void);
};


class CRollerDummy : public CRollerBase {
public:
    // Constructor
    CRollerDummy(const QString& Name);

    // Destructor
    ~CRollerDummy(void);

    // Command to turn on/off the 8051 h/w.
    TQErrCode SetRollerOnOff (bool);
          
    //This procedure send Get status message to OHDB(8051).
    TQErrCode GetStatus(void);

    //This procedure send Set parameters message to OHDB(8051).
    TQErrCode SetRollerParms (int);
    
    // Enable/Disable the bumper
    TQErrCode BumperEnableDisable(bool Enable);

    // Enable/Disable the bumper PEG mechanism
    TQErrCode EnableDisableBumperPeg(bool Enable);
    //This procedure set bumper parameters to OHDB.
    TQErrCode SetBumperParams (int,int,int);
};


#endif

