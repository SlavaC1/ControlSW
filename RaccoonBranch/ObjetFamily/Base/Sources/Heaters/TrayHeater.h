/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Tray Heater class                                        *
 * Module Description: This class implement services related to the *
 *                     tray heater.                                 *
 *                                                                  *
 *                                                                  *
 * Compilation: Standard  C++.                                      *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 02/09/2001                                           *
 ********************************************************************/

#ifndef _TRAY_HEATER_H_
#define _TRAY_HEATER_H_

#include <assert.h>
#include "OCBProtocolClient.h"
#include "LinearInterpolator.h"
#include "Temperature.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"
#include "HeatersDefs.h"
                         
// Constants:
//
const bool TRAY_HEATER_ON  = true;
const bool TRAY_HEATER_OFF = false;
                        
const unsigned int OCB_TRAY_IN_TEMPERATURE_TIMEOUT_IN_SEC = 210;

const unsigned int TRAY_ERROR_THERMISTOR_OPEN         = 0;
const unsigned int TRAY_ERROR_THERMISTOR_SHORT        = 1;
const unsigned int TRAY_ERROR_THERMISTOR_OUT_OF_RANGE = 2;
const unsigned int TRAY_HEATER_NO_ERROR               = 3;

const bool CHECK_WITH_MORE_MARGINE = true;
const bool CHECK_WITHOUT_MORE_MARGINE = false;

const bool START_PRINT_TEMPERATURE = true;
const bool PRINTING_TEMPERATURE = false;


// Exception class for all the QLib RTTI elements
class ETrayHeater : public EQException {
  public:
    ETrayHeater(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CTrayTemperature : public CTemperature {
protected:
   static CLinearInterpolator <int> m_A2DToCelcius;
   static CLinearInterpolator <int> m_CelciusToA2D;
public:
   static void LoadTemperatureTable(const QString& FileName);
   int ConvertCelciusToA2D(int);
   int ConvertA2DToCelcius(int);
};


// C tray base control class
class CTrayBase : public CQComponent {
 private:
    CTrayTemperature m_CurrentTrayTemperature; // Current Tray Temperature in A/D units
    int m_TrayStatus;

    // status parameters:
    DEFINE_VAR_PROPERTY(bool,TrayInserted);
    DEFINE_VAR_PROPERTY(bool,TrayOn);
    int m_OutOfRangeCounter;
    int m_TotalOutOfRangeCounter;

 protected:
    COCBProtocolClient *m_OCBClient;

    CTrayTemperature m_StartTrayTemperature;
    CTrayTemperature m_PrintingTrayTemperature;
    CTrayTemperature m_SetTrayTemperature; // last requested tray temperature in A/D units

    int m_ActiveMarginInPercent;  // Definition of the margin around the set point
    int m_TrayTimeoutTemperature;

    // Protection mutex for datyabase
    CQMutex m_MutexDataBase;

    // Flag that permit to start send get status message each period time
    bool m_FlagCanSendGetStatusMsg;

    // Calculate procedures:
    void UpdateTrayInserted(bool);
    void UpdateTrayIsOn(bool);

    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;

    // is temperature is returned in A/D units or in celsius degrees
    bool m_A2DUnits;

  public:
    // Constructor
    CTrayBase(const QString& Name);

    // Destructor
    ~CTrayBase(void);

    // Command to turn on/off the tray heater
    DEFINE_V_METHOD_1(CTrayBase,TQErrCode,SetTrayOnOff,bool) = 0;

    //This procedure is called in order to wait for the Tray in Temperature to turn on
    DEFINE_V_METHOD(CTrayBase,TQErrCode,WaitForTrayInTemperature) = 0;

    //This procedure send Set tray temperature message to OCB.
    DEFINE_V_METHOD_3(CTrayBase,TQErrCode,SetTrayTemperatureMsg,int,int,int) = 0;

    //This procedure send Set default tray temperature message to OCB.
    DEFINE_METHOD(CTrayBase,TQErrCode,SetDefaultTrayStartTemperature);
    DEFINE_METHOD(CTrayBase,TQErrCode,SetDefaultTrayPrintingTemperature);
    DEFINE_METHOD_1(CTrayBase, TQErrCode, SetUserTrayTemperature,int);

    //This procedure send Get status message to OCB.
    DEFINE_V_METHOD(CTrayBase,TQErrCode,GetTrayStatus) = 0;
    
    //This procedure send Get status message to OCB.
    DEFINE_V_METHOD(CTrayBase,TQErrCode,GetTrayInsertedStatus)=0;

    bool CheckTrayTemperatureStatus(void); // use error handler to display error
    TQErrCode IsTemperatureValid(void);

    DEFINE_METHOD(CTrayBase,bool,IsTrayInserted);
    DEFINE_METHOD(CTrayBase,bool,IsTrayOn);
    
    DEFINE_METHOD(CTrayBase,int,GetTrayTemperatureA2D);
    DEFINE_METHOD(CTrayBase,int,GetTrayTemperatureCelcius);
    DEFINE_METHOD_1(CTrayBase,TQErrCode,UpdateTrayTemperatureA2D,int);
    DEFINE_METHOD(CTrayBase,int,GetCurrentTrayInRangeStatus);

    DEFINE_METHOD(CTrayBase,TQErrCode,UpdateToPrintTemperature);

    DEFINE_METHOD(CTrayBase,TQErrCode,DisplayTrayTemperature);

    DEFINE_METHOD_1(CTrayBase,int,ConvertA2DToCelcius,int);
    DEFINE_METHOD_1(CTrayBase,int,ConvertCelciusToA2D,int);

    CTrayTemperature GetTrayTemperature (void);

    TQErrCode Enable(bool);
    void WaitUntilTrayInserted();
//    bool CheckContinueIfTrayIsOut(void);
    TQErrCode VerifyTrayInserted(bool showDialog=true);

    //Tray heater Statistcs
    void DisplayStatistics(void);
    void ResetStatistics(void);

    //Test procedure
    DEFINE_METHOD(CTrayBase,TQErrCode,Test);

    virtual void Cancel(void) =0;
};


class CTrayHeater : public CTrayBase {

private:

   // Semaphore That verify if Ack is received or if Notify response
   // was received (Tray Heater is in Range).
   CQEvent m_SyncEventTrayIsInRange;

   //Flag Tray heater during command performance.
   bool m_FlagTrayOnOffUnderUse;
   bool m_FlagSetCommandUnderUse;
   bool m_FlagGetStatusUnderUse;
   bool m_FlagTrayInCommandUnderUse;

   //Ack Flag.
   bool m_OnOff_AckOk;
   bool m_Set_AckOk;
   bool m_TrayIn_AckOk;

   // Protection mutex for the cancel flag
    CQMutex m_CancelFlagMutex;

   // Flags to control the waiting and cancel operation
   bool m_Waiting;
   bool m_CancelFlag;

   //This procedure is the callback procedure for Set OnOff Tray Heater
   static void OnOffAckResponse (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback procedure for Set temperature of Tray Heater
   static void SetTemperatureAckResponse (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback procedure for get status message
   static void TrayStatusResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure handles callback for Is Tray existense message
   static void TrayInsertedStatusResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   // Notification Call Backs procedures
   static void NotificationTemperatureInRangeResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   static void NotificationTrayHeatingTimeoutResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   static void NotificationErrorMessage (int TransactionId,PVOID Data,
                                         unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OCb
   void AckToOcbNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);
                   
   static void PeriodicProcess(TGenericCockie Cockie);

   // Tray Heater recovery procedure.
   void Recovery (void) {}


public:
    // Constructor
    CTrayHeater(const QString& Name);

    // Destructor
    ~CTrayHeater(void);

    // Command to turn on/off the Tray heater
    TQErrCode SetTrayOnOff(bool OnOff);

    //This procedure is called in order to wait for the Tray in Temperature to turn on
    TQErrCode WaitForTrayInTemperature(void);

     //This procedure send Set tray temperature message to OCB.
    TQErrCode SetTrayTemperatureMsg(int TemperatureInCelcius,int Margin,int Timeout);

    //This procedure send Get status message to OCB.
    TQErrCode GetTrayStatus(void);

    //This procedure send get status message about Tray existense
    TQErrCode GetTrayInsertedStatus(void);

    void Cancel(void);
};


class CTrayDummy : public CTrayBase {
public:
    // Constructor
    CTrayDummy(const QString& Name);

    // Destructor
    ~CTrayDummy(void);

    // Command to turn on/off the tray heater
    TQErrCode SetTrayOnOff(bool OnOff);

    //This procedure is called in order to wait for the Tray in Temperature to turn on
    TQErrCode WaitForTrayInTemperature(void);

     //This procedure send Set tray temperature message to OCB.
    TQErrCode SetTrayTemperatureMsg(int TemperatureInCelcius,int Margin,int Timeout);

    //This procedure send Get status message to OCB.
    TQErrCode GetTrayStatus(void);

    //This procedure send get status message about Tray existense
    TQErrCode GetTrayInsertedStatus(void);

    void Cancel(void){}
};


#endif

                  


