/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Purge ON 8051 h/w                                        *
 * Module Description: This class implement services related to the *
 *                     purge on 0\8051 h/w.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/10/2001                                           *
 ********************************************************************/
 
#ifndef _PURGE_H_
#define _PURGE_H_

#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"
#include "Q2RTErrors.h"
#include "PurgeDefs.h"


const int PURGE_NO_ERROR = 0;
const int HEADS_TEMPERATURE_OUT_OF_RANGE = 1;
const int MODEL_EOL = 2;
const int SUPPORT_EOL = 3;
const int MODEL_FILL_ERROR= 4;
const int SUPPORT_FILL_ERROR = 5;
const int HEADS_THERMISTOR_SHORT = 6;
const int HEADS_THERMISTOR_OPEN = 7;
const int CONTAINER_THERMISTOR_SHORT = 8;
const int CONTAINER_THERMISTOR_OPEN = 9;
const int PURGE_CANCELED = 20;
const int PURGE_ADD_MORE_TIME = 21;

//delays
const int WAIT_UNTIL_NEXT_STATUS_CHECK = 3; // seconds

// Exception class for all the QLib RTTI elements
class EPurge : public EQException {
  public:
    EPurge(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


typedef struct {
   int ReturnValue;
} TEndPurgeMessage;


// C Purge control class
class CPurgeBase : public CQComponent{
  private:
    unsigned GetLastPurgeTime(void) {
      return m_LastPurgeTime;
    }

    void SetLastPurgeTime(unsigned) {}

  protected:
   DEFINE_VAR_PROPERTY(bool,UnderPurge);
   DEFINE_VAR_PROPERTY(UINT,TimeDurationParameter);

    COCBProtocolClient *m_OCBClient;

    // Protection mutex for database
    CQMutex m_MutexDataBase;
    
    // Flag that permit to start send get status message each period time
    bool m_FlagCanSendGetStatusMsg;

    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;

    unsigned m_LastPurgeTime;

    // Command to start purge to the 8051 h/w.
    virtual TQErrCode PerformPurge(USHORT Time) = 0;

    //This procedure is called in order to wait to purge end.
    virtual TQErrCode WaitForPurgeEnd(void) = 0;

  public:
    // Constructor
    CPurgeBase(const QString& Name);

    // Destructor
    ~CPurgeBase(void);

    // Last purge execution time in system ticks
    DEFINE_PROPERTY(CPurgeBase,unsigned,LastPurgeTime);

    // Command to start purge with default parameters (INIFILE):long purge
    DEFINE_V_METHOD(CPurgeBase,TQErrCode,PerformDefaultPurge);

    // Purge model, support or both
    DEFINE_METHOD_1(CPurgeBase,TQErrCode,Purge,int);

    //This procedure send Get status message to OCB(8051).
    DEFINE_V_METHOD(CPurgeBase,TQErrCode,GetStatus)=0;

    //This procedure update the value of UnderPurge.
    DEFINE_METHOD_1(CPurgeBase,TQErrCode,UpdateUnderPurgeValue,bool);

    //This procedure get the value of UnderPurge variable;
    DEFINE_METHOD(CPurgeBase,bool,GetUnderPurgeValue);

    //This procedure check during 1 minute if purge finish
    virtual bool CheckIfPurgeAlreadyFinish (void)=0;

    //Test procedure
    DEFINE_METHOD(CPurgeBase,TQErrCode,Test);

    //If head filling is working during purge may be it is a race condition
    //that after checking liquid the value of thermisthors change and OCB
    //start to filling.
    virtual void AddMoreTimeToWaitForPurgeEnd(void);

    virtual void Cancel(void)=0;
};


class CPurge : public CPurgeBase {

 private:
   bool m_Waiting;

   //Messsage Queue that verify if Ack if Notify end of purge response was received.
   typedef CQMessageQueue <TEndPurgeMessage> TEndPurgeQueue;
   TEndPurgeQueue *m_EndOfPurgeQueue;

   //Flag Purge during command performance.
   bool m_FlagPerformPurgeUnderUse;
   bool m_FlagGetStatusUnderUse;

   //Flag ack ok
   bool OnOff_AckOk;

    // Command to perform purgeto the 8051 h/w.
    TQErrCode PerformPurge (USHORT);
   
   //This procedure is the callback for Set Purge OnOff OCB
   static void OnOffAckResponse (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //Get status ack response
   static void GetStatusAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   // Notification Call Backs
   static void NotificationPurgeEnd(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockiee);

   static void NotificationErrorMessage(int TransactionId,PVOID Data,
                                        unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OCb
   void AckToOcbNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);

   // Tray Heater recovery procedure.
   void Recovery (void);

public:
    // Constructor
    CPurge(const QString& Name);

    // Destructor
    ~CPurge(void);

    //This procedure is called in order to wait for 8051 to purge end.
    TQErrCode WaitForPurgeEnd(void);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetStatus(void);

    //This procedure check during 1 minute if purge finish
    bool CheckIfPurgeAlreadyFinish (void);

    void AddMoreTimeToWaitForPurgeEnd(void);

    void Cancel(void);
};


class CPurgeDummy : public CPurgeBase
{
  private:
    // Command to perform purge to the 8051 h/w.
    virtual TQErrCode PerformPurge (USHORT);

  public:
    // Constructor
    CPurgeDummy(const QString& Name);

    // Destructor
    ~CPurgeDummy(void);

    //This procedure is called in order to wait for purge end.
    TQErrCode WaitForPurgeEnd (void);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetStatus(void);

    //This procedure check during 1 minute if purge finish
    bool CheckIfPurgeAlreadyFinish (void);

    void Cancel(void) {}
};


#endif

