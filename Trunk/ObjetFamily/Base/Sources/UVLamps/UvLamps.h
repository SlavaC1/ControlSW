/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Uv lamp class                                            *
 * Module Description: This class implement services related to the *
 *                     Uv lamps. The are two lamps:                 *
 *                     LeftUvLamp(0) and RightUvlamp(1).            *
 *                     Uv lamps don't have a task.                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 22/08/2001                                           *
 * Last upate: 30/08/2001                                           *
 ********************************************************************/

#ifndef _UV_LAMPS_H_
#define _UV_LAMPS_H_

#include <assert.h>
#include "QMutex.h"
#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"

typedef unsigned int UINT;

// Exception class for all the QLib RTTI elements
class EUvLamps : public EQException {
  public:
    EUvLamps(const QString& ErrMsg,const TQErrCode ErrCode=0);
    EUvLamps(const TQErrCode ErrCode);
};

// Forward declaration
class CUvLamps;
class CUvLampsDummy;

// Single Uv lamps class
// Note: This class can be created and destroyed only by the CUvLamps class
class CQSingleUvLamp : public CQComponent
{
    friend CUvLamps;
    friend CUvLampsDummy;
    
  private:
    // Constructor
    bool m_UvLampOn;
    bool m_Enabled;    
    bool m_UvLampsLastRequest;
    CQMutex m_MutexUvOnOff;

private:
    CQSingleUvLamp(const QString& Name); // Constructor
    ~CQSingleUvLamp(void); // Destructor

    void TurnOnOff (bool OnOff);
    bool GetSingleStatus(void);
    void SetSingleTurnedOn(bool);

    bool GetSingleEnabled(void);
    void SetSingleEnabled(bool);

  public:
     DEFINE_PROPERTY(CQSingleUvLamp,bool,TurnedOn);
     DEFINE_PROPERTY(CQSingleUvLamp,bool,Enabled);
};

// C Ultra Violet lamps control class
class CUvLamps : public CQComponent // C Ultra Violet lamps control class
{
  protected:
    COCBProtocolClient *m_OCBClient;

    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;     

    // Each head heater is represented by CQSingleUvLamp class
    CQSingleUvLamp *m_UvLamps[NUMBER_OF_UV_LAMPS];

    //The current parameter of Uv lamp ignition timeout
    DEFINE_VAR_PROPERTY(int,IgnitionTimeoutParam);
    DEFINE_VAR_PROPERTY(int,PostIgnitionTimeoutParam);

    // Flag that permit to start send get status message each period time
    bool m_FlagCanSendGetStatusMsg;

    //Flag Uv lamps during command performance.
    bool m_FlagTurnCommandUnderUse;
    bool m_FlagSetA2DValueCommandUnderUse;
    bool m_FlagGetUVValueCommandUnderUse;
    bool m_FlagSetCommandUnderUse;
    bool m_FlagGetStatusUnderUse;
    bool m_UvLampsLastRequest;
    bool m_FlagSetUVSamplingParamsUnderUse;
	bool m_FlagResetUVSafetyUnderUse;
    
    bool m_Turn_AckOk;
    bool m_Set_AckOk;
    bool m_GetValue_AckOk;
    bool m_UvSamplingParams_AckOk;
	bool m_ResetUVSafetyAckOk;

    // Semaphore That verify if Ack is received or if Notify response
    // was received (Lamps are ON).
    CQEvent m_SyncEventTurnedOnOff;

    //The parameters: UV Lamps time life is kept in PARAMETER
    // manager (Status & statistic).
    int m_UvErrorCount[NUMBER_OF_UV_LAMPS];
    int m_UvNoiseCount[NUMBER_OF_UV_LAMPS];

    USHORT m_UVValueINT;
    USHORT m_UVValueEXT;
    USHORT m_UVMaxValueINT;
    USHORT m_UVMaxValueEXT;
    USHORT m_UVMaxDeltaINT;
    USHORT m_UVMaxDeltaEXT;
    ULONG  m_UVSumINT;
    ULONG  m_UVSumEXT;
    USHORT m_UVNumOfReadings;



    // UV lamps recovery procedure.
    void Recovery(void);

    //Mark what was the last command performed Turn Uv On or Off
    bool m_UvLampIsOn;

    //The above procedure mark if we turned on (not status)
    void SetTurnIsOn(bool OnOff);

    //set the status for Uv lamps
    void SetTurnStatusOnOff(bool OnOff);

    // Protection mutex for the cancel flag
    CQMutex m_CancelFlagMutex;

    // Protection mutex for the UV Sensor reading
    CQMutex m_MutexUVSensor;

    // Flags to control the waiting and cancel operation
    bool m_CancelFlag;
    bool m_Waiting;

    // Current ignition timeout value
    int m_IgnitionTimeout;

    //This procedure is the callback for Set parameter Uv lamps message
    static void SetParmAck (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
	
	static void ResetUVSafetyAck (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    //This procedure is the callback for get status message
    static void GetStatusResponse(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    static void GetUVValueAckResponseMsg (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Callback for ack for UvSetSamplingParamsMsg
    static void SetUvSamplingParamsAckResponseMsg (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Acknolodges for Command turn on/off
    static void TurnOnOffAckResponseMsg(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Acknolodges for Command SetUVD2AValue
    static void SetUVD2AValueAckResponseMsg(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Notification Of Lamps Turning
    static void NotificationLampsAreOnOffMessage (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    // Notification of lamps error
    static void NotificationErrorMessage (int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

    //This procedure Update Uv Lamp Internal Status
    void UpdateUvLampInternalStatus(bool*);

    // Update (from OCB) the last UV command sent to the OCB.
    void UpdateUvLampOCBLastRequest(bool lastRequest);

    // update internal UV Sensor value
    void UpdateUvLampValue(USHORT valueINT, USHORT valueEXT);
    void UpdateUvLampMaxValue(USHORT valueINT, USHORT valueEXT);
    void UpdateUvLampMaxDelta(USHORT valueINT, USHORT valueEXT);
    void UpdateUvLampSum(ULONG valueINT, ULONG valueEXT);
    void UpdateUvLampNumOfReadings(USHORT NumOfReadings);


    // Replay Ack to OCb
    void AckToOcbNotification (int MessageID,
                               int TranscationID,
                               int AckStatus,
                               TGenericCockie Cockie);
                               
    virtual TQErrCode SetMaskedDefaultParmIgnitionTimeout(BYTE);

public:
    // Constructor
    CUvLamps(const QString& Name);

    // Destructor
    ~CUvLamps(void);

    // Get access to a single Uv Lamps.
    CQSingleUvLamp& operator [](unsigned Index);
    // Return the number of items (Uv lamps)
    unsigned Count(void);

    //Get what was the left command sent to OCB turn on or turn off
    bool GetIfTurnIsOn(void);

    //Get current status of Uv lamps
    bool GetTurnStatus(void);


    // Return the last (On/Off) command that is known to the OCB.
    bool GetUVLampsLastCommand();

    // Command to turn on/off the Uv lamps
    DEFINE_V_METHOD_1(CUvLamps,TQErrCode,TurnOnOff,bool);
    DEFINE_V_METHOD_2(CUvLamps,TQErrCode,TurnSpecifiedOnOff,bool,BYTE);

    // Command to set D/A value to the Uv lamp
    DEFINE_V_METHOD_2(CUvLamps,TQErrCode,SetUVD2AValue,BYTE/*LampID*/,USHORT/*D2A Value*/);

    //Mark the current Uv lamps status
    DEFINE_PROPERTY(CUvLamps,bool,TurnedOn);

    //This procedure is called in order to wait for the lamps to turn on
    DEFINE_V_METHOD(CUvLamps,TQErrCode,WaitForTurnedOn);
    
    //This procedure send Set default parameter Uv lamps message to OCB.
    DEFINE_V_METHOD(CUvLamps,TQErrCode,SetDefaultParmIgnitionTimeout);

    //This procedure send Set parameter Uv lamps message to OCB.
    DEFINE_V_METHOD_5(CUvLamps,TQErrCode,SetParmIgnitionTimeout,USHORT,USHORT,bool,BYTE,UINT);
	
	// Send a message that resets the UV safety trigger in the OCB (immobility detected trigger)
	// THIS MESSAGE HAVE TO BE SENT ONLY ONCE, HERE IN POWER-UP SEQUENCE
	DEFINE_V_METHOD(CUvLamps,TQErrCode,ResetUVSafety);

    //This procedure send Get status message to OCB.
    DEFINE_V_METHOD(CUvLamps,TQErrCode,GetStatus);

	DEFINE_METHOD(CUvLamps,int,GetUVLampsStatus);//Get current status of Uv lamps

    DEFINE_V_METHOD(CUvLamps,TQErrCode,GetUVValue);
    DEFINE_V_METHOD_1(CUvLamps,int,GetUVSensorValue,bool/*InternalSensor*/);
    DEFINE_V_METHOD_1(CUvLamps,int,GetUVSensorMaxValue,bool/*InternalSensor*/);
    DEFINE_V_METHOD_1(CUvLamps,int,GetUVSensorMaxDelta,bool/*InternalSensor*/);
    DEFINE_V_METHOD_1(CUvLamps,int,GetUVSensorSum,bool/*InternalSensor*/);
    DEFINE_V_METHOD(CUvLamps,int,GetUVNumOfReadings);


    DEFINE_V_METHOD_1(CUvLamps,TQErrCode,SetUVSamplingParams,bool);

    //This procedure Test this package.
    DEFINE_METHOD(CUvLamps,TQErrCode,Test);

    bool m_InsertLampError;
    DEFINE_METHOD(CUvLamps,TQErrCode,InsertLampError);

    //UV Lamps Statistcs
    void DisplayStatistics(void);
    void ResetStatistics(void);

    //Prepare UV to print - turn on
    TQErrCode Enable(void);

    //Try to turn on Uv lamp after error
    TQErrCode EnableDuringPrint(void);

    void Cancel(void);
};


// C Ultra Violet lamps Dummy control class
class CUvLampsDummy : public CUvLamps {
public:
   //This procedure send Get status message to OCB.
   TQErrCode GetStatus(void);

   TQErrCode GetUVValue(void);
   int GetUVSensorValue(bool InternalSensor);
   int GetUVSensorMaxValue(bool InternalSensor);
   int GetUVSensorMaxDelta(bool InternalSensor);
   int GetUVSensorSum(bool InternalSensor);
   int GetUVNumOfReadings();

   TQErrCode SetUVSamplingParams(bool ResetAverage);

   bool GetUVLampsLastCommand();

    // Constructor
    CUvLampsDummy(const QString& Name);

    // Destructor
    ~CUvLampsDummy(void);    

    // Command to turn on/off the Uv lamps
    TQErrCode TurnOnOff(bool OnOff);

    // Command to set D/A value to the Uv lamp
    TQErrCode SetUVD2AValue(BYTE LampID, USHORT D2A_Value);

    TQErrCode WaitForTurnedOn(void);

     //This procedure send Set parameter Uv lamps message to OCB.
    TQErrCode SetParmIgnitionTimeout (USHORT,USHORT,bool,BYTE,UINT);
	
	TQErrCode ResetUVSafety();

    void Cancel(void){}
};


#endif
