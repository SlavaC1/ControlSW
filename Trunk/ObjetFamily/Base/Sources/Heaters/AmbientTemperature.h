/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Ambient Temperature ON 8051 h/w                          *
 * Module Description: This class implement services related to the *
 *                     Fan that control the internal temperature of *
 *                     Eden. It is controlled via 8051 h/w.         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 3/03/2002                                            *
 * Date : 30/Dec/2002                                               *
 *                                                                  *
 ********************************************************************/
 
#ifndef _AMBIENT_H_
#define _AMBIENT_H_

#include "ErrorHandler.h"
#include "LinearInterpolator.h"
#include "OCBCommDefs.h"
#include "OCBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "OHDBProtocolClient.h"
#include "Q2RTErrors.h"
#include "QMessageQueue.h"
#include "QTimer.h"
#include "Temperature.h"

const int FAN_ON  = 1;
const int FAN_OFF = 0;

const int AMBIENT_ERROR = -1;
const int OVER_HEAT     = 0;
const int UNDER_HEAT    = 1;
const int IN_LIMIT      = 2;

const int AMBIENT_UNDER_LIMIT      = -1;
const int AMBIENT_TEMPERATURE_COLD = 0;
const int AMBIENT_TEMPERATURE_OK   = 1;
const int AMBIENT_TEMPERATURE_HOT  = 2;
const int AMBIENT_OVER_LIMIT       = 3;

class CBackEndInterface;

// Exception class for all the QLib RTTI elements
class EAmbient : public EQException {
  public:
    EAmbient (const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CATTemperature : public CProportionalTemperature {
protected:
   //The following two lines should be declared in the inherit CTemperature.
   static CLinearInterpolator<float> m_A2DToCelcius;
   static CLinearInterpolator<float> m_CelciusToA2D;
public:
   static void LoadTemperatureTable(const QString& FileName);
   int ConvertCelciusToA2D(int value);
   int ConvertA2DToCelcius(int value);
};

// C AmbientBase control class
class CAmbientTemperatureBase : public CQComponent {
  protected:

   CATTemperature m_OHDBCurrentAmbientTemperature;
   CATTemperature m_LowLimit;
   CATTemperature m_FanLimit;
   CATTemperature m_HighLimit;
   int m_OrgHighLimit;
   int m_AmbientTemperatureStatus;
   COHDBProtocolClient *m_OHDBClient;

   // Protection mutex for database
   CQMutex m_MutexDataBase;

   // Flag that permit to start send get status message each period time
   bool m_FlagCanSendGetStatusMsg;
   CQMutex m_MutexAmbientHeaterDuringPurge;

   CErrorHandler*     m_ErrorHandlerClient;
   CAppParams*        m_ParamsMgr;
   CBackEndInterface* m_BackEnd;

   void UpdateOHDBCurrentTemperatureA2D(int);
   void UpdateLowLimitA2D (int);
   void UpdateHighLimitA2D(int);
   void UpdateFanLimitA2D (int);

    // is temperature is returned in A/D units or in celsius degrees
    bool m_A2DUnits;

	bool m_AmbientError;

	CQMutex m_TempControlSuspendMutex;
    bool    m_TempControlSuspended;

  public:
   // Constructor
    CAmbientTemperatureBase(const QString& Name);

    // Destructor
    ~CAmbientTemperatureBase(void);

    int GetLowLimitA2D(void);
    int GetLowLimitCelcius(void);
    int GetHighLimitA2D(void);
    int GetHighLimitCelcius(void);
    int GetFanLimitA2D(void);
    int GetFanLimitCelcius(void);

    // Command to Turn on ambient temperature: Turn on/off FAN.
    DEFINE_METHOD_1(CAmbientTemperatureBase,TQErrCode,TurnAmbientTemperature,bool);

    // Command to set ambient temperature.
    DEFINE_METHOD_4(CAmbientTemperatureBase,TQErrCode,SetTemperature,int,int,int,int);
    DEFINE_METHOD_1(CAmbientTemperatureBase, TQErrCode, SetUserTemperature, int);

    // Converting units procedures
    DEFINE_METHOD_1(CAmbientTemperatureBase,int,ConvertA2DToCelcius,int);
    DEFINE_METHOD_1(CAmbientTemperatureBase,int,ConvertCelciusToA2D,int);

	//This procedure gets the value of Current ambient temperature
    DEFINE_METHOD(CAmbientTemperatureBase,int,GetCurrentTemperatureCelcius);

	//This procedure gets the value of Current ambient temperature in A/D units
	DEFINE_METHOD(CAmbientTemperatureBase,int,GetCurrentTemperatureA2D);

	//This procedure gets the value of Current ambient temperature in A/D units
    DEFINE_METHOD(CAmbientTemperatureBase,int,GetCurrentTemperatureStatus);

	//This procedure checks if temperature is in range
	void UpdateCurrentAmbientStatus();
	
    //Set on/off the material cooling Fan
    DEFINE_V_METHOD_1(CAmbientTemperatureBase,TQErrCode,SetMaterialCoolingFanOnOff,bool)=0;

    //Set the duty cycle of the material cooling fan
    DEFINE_V_METHOD_1(CAmbientTemperatureBase,TQErrCode,SetMaterialCoolingFansDutyCycle,BYTE)=0;

    DEFINE_METHOD(CAmbientTemperatureBase,TQErrCode,SetOHDBAmbientParams);

    // Command to get ambient tempearture sensor status
    DEFINE_V_METHOD(CAmbientTemperatureBase,TQErrCode,GetOHDBAmbientTemperatureStatus)=0;
	virtual void Cancel(void)=0;
	virtual void SetAmbientHeaterDuringPurge(bool bAmbientHeaterDuringPurge) = 0 ;

    //Test procedure
	DEFINE_METHOD(CAmbientTemperatureBase,TQErrCode,Test);

	void SetTempControlSuspended(bool Suspended);

	bool IsTempControlSuspended();
	int GetPrevHighLimit();
	void SetPrevHighLimit(int prevHighLimit);
};


class CAmbientTemperature : public CAmbientTemperatureBase {

 private:
   int m_StatusTimeStamp;
   int m_ErrorTimeStamp;

   BYTE m_CoolingFansDutyCycle;

   // Semaphore That verify if Ack is received or if Notify response
   // was received.
   CQEvent m_SyncEventWaitInLimit;

   // Flags to control the waiting and cancel operation
   bool m_Waiting;
   bool m_CancelFlag;
   bool m_IsAmbientHeaterDuringPurge;

   // Protection mutex for the cancel flag
   CQMutex m_CancelFlagMutex;

   //Flag Purge during command performance.
   bool m_FlagGetOHDBAmbientStatusComUnderUse;
   bool m_FlagSetMaterialCoolingUnderUse;
        
   //Flag ack ok
   bool SetCooling_AckOk;

   //GetAmbientStatus ack response
   static void GetOHDBAmbientStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   static void MaterialCoolingFanOnOffAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //static void NotificationErrorMessage (int TransactionId,PVOID Data,
   //                              unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OHDB
   void AckToOhdbNotification (int MessageID,
                               int TransactionID,
                               int AckStatus,
                               TGenericCockie Cockie);

   // Ambient recovery procedure.
   //void Recovery (void);

   // Turn the hood fan on/off according to the current ambient temperature
   void TemperatureControl();

public:
    // Constructor
    CAmbientTemperature(const QString& Name);

    // Destructor
    ~CAmbientTemperature(void);

    // Command to get ambient tempearture sensor status
    TQErrCode GetOHDBAmbientTemperatureStatus(void);

    //Set on/off the material cooling Fan
    TQErrCode SetMaterialCoolingFanOnOff(bool);

    //Set the duty cycle of the material cooling fan
	TQErrCode SetMaterialCoolingFansDutyCycle(BYTE);

	bool GetAmbientHeaterDuringPurge();

	 void SetAmbientHeaterDuringPurge(bool bAmbientHeaterDuringPurge);

	void Cancel(void);
};


class CAmbientTemperatureDummy : public CAmbientTemperatureBase {
public:
   // Constructor
   CAmbientTemperatureDummy(const QString& Name);

   // Destructor
   ~CAmbientTemperatureDummy(void);

   // Command to get ambient tempearture sensor status
   TQErrCode GetOHDBAmbientTemperatureStatus(void);

   //Set on/off the material cooling Fan
   TQErrCode SetMaterialCoolingFanOnOff(bool);

   //Set the duty cycle of the material cooling fan
   TQErrCode SetMaterialCoolingFansDutyCycle(BYTE);

   void Cancel(void){}
   void SetAmbientHeaterDuringPurge(bool bAmbientHeaterDuringPurge) {}



};



#endif

