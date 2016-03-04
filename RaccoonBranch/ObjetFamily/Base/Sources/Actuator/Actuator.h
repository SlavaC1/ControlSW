/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Actuator ON 8051 h/w                                     *
 * Module Description: This class implement services related to the *
 *                     actuator on 0\8051 h/w.                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 1/May/2002                                           *
 *                                                                  *
 ********************************************************************/

#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"
#include "Q2RTErrors.h"


typedef bool T_ActuatorTable[MAX_ACTUATOR_ID];
typedef bool T_SensorTable[MAX_SENSOR_ID];

//Model on Tray Status
const int SENSOR_NOT_DETECT_MODEL_ON_TRAY = 0;
const int SENSOR_1_DETECT_MODEL_ON_TRAY = 1;
const int SENSOR_2_DETECT_MODEL_ON_TRAY = 2;
const int SENSOR_1_AND_2_DETECT_MODEL_ON_TRAY = 3;
const int FIRST_CHEAP_SELECT =   0xA4; //First MSC card address
const int SECOND_CHEAP_SELECT =  0xAA; //Second MSC card address
const int PUMPS_STATUS = 1;

// Exception class for all the QLib RTTI elements
class EActuator : public EQException {
  public:
    EActuator(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// C Actuator control class
class CActuatorBase : public CQComponent{
  private:
     T_ActuatorTable m_ActuatorTable;
     T_SensorTable m_SensorTable;
     bool m_PollingControlFlag;

     int m_CollisionStatistics_Sensor1;
     int m_CollisionStatistics_Sensor2;
     int m_CollisionStatistics_BothSensors;
  protected:
    //Update the values of actuators and sensors
    void UpdateActuatorOnOff (int ID, bool OnOff);
    void UpdateActuatorTable(T_ActuatorTable ActuatorTable);
    void UpdateSensorOnOff (int ID, bool OnOff);
    void UpdateInputTable(T_SensorTable SensorTable);
    void UpdateRSSStatus(int RSSOnValue, int RSSOffValue); //RSS, itamar
    int GetRSSOnValue(void); //RSS, itamar
    int GetRSSOffValue(void);//RSS, itamar

    COCBProtocolClient *m_OCBClient;

    // Protection mutex for database
    CQMutex m_MutexDataBase;
    
    // Flag that permit to start send get status message each period time
    bool m_FlagCanSendGetStatusMsg;
    
    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;

    bool m_IsRSSActivated; //RSS, itamar
    int m_RSSOnValue; //RSS, itamar
    int m_RSSOffValue; //RSS, itamar

  public:
    // Constructor
    CActuatorBase(const QString& Name);

    // Destructor
    ~CActuatorBase(void);

	//This procedure get the value of Actuator on off of OCB(8051).
	DEFINE_V_METHOD_1(CActuatorBase,bool,GetActuatorOnOff,int);
//    bool GetActuatorOnOff (int ID);
    void DisplayActuatorTable(void);
    bool GetSensorOnOff (int ID);
    void DisplaySensorTable(void); 

    // Command to timed set actuator to the 8051 h/w.
    DEFINE_V_METHOD_3(CActuatorBase,TQErrCode,TimedSetOnOff,int,bool,int) = 0;

    //Activate roller suction system
    DEFINE_V_METHOD_3(CActuatorBase,TQErrCode,ActivateRollerSuctionSystem,int,int,bool) = 0; //RSS, itamar

    // Command to set on off actuator.
    DEFINE_V_METHOD_2(CActuatorBase,TQErrCode,SetOnOff,int,bool)=0;

    //This procedure send Get status message to actuator-OCB(8051).
    DEFINE_V_METHOD(CActuatorBase,TQErrCode,GetActuatorStatus)=0;

    //This procedure send Get status message to actuator-OCB(8051).
    DEFINE_V_METHOD(CActuatorBase,TQErrCode,GetRollerSuctionSystemStatus)=0;  //RSS, itamar

    //This procedure send Get status message to input status-OCB(8051).
    DEFINE_V_METHOD(CActuatorBase,TQErrCode,GetInputStatus)=0;

    //Test procedure
    DEFINE_METHOD(CActuatorBase,TQErrCode,Test);

    //Verify if Actuator Control is on or off
    DEFINE_METHOD_1(CActuatorBase,TQErrCode,MarkIfPollingControlIsOn,bool);
    //Get if Actuator Control is on or off
    bool GetIfPollingControlIsOn(void);

    // recovery procedure.
    void Recovery (void);

    virtual void Cancel(void)=0;
	TQErrCode SetDirtPumpOnOff(bool WasteOnOff, bool RollerOnOff);
	TQErrCode SetWastePumpOnOff(bool OnOff);
	TQErrCode SetRollerPumpOnOff(bool OnOff);

    // Turn the dirt pump on/off
    DEFINE_METHOD_1(CActuatorBase,TQErrCode,SetDirtPumpOnOff,bool /*OnOff*/);
    //DEFINE_METHOD_1(CActuatorBase,TQErrCode,SetRSSValveOnOff,bool /*OnOff*/); //RSS, itamar added
    //DEFINE_METHOD_1(CActuatorBase,TQErrCode,SetRSSPumpOnOff,bool /*OnOff*/); //RSS, itamar added

    // Turn the hood fan on/off
    DEFINE_V_METHOD  (CActuatorBase,TQErrCode,SetHoodFanIdle)=0;
    DEFINE_V_METHOD_1(CActuatorBase,TQErrCode,SetHoodFanOnOff,bool)=0;
    DEFINE_V_METHOD_2(CActuatorBase,TQErrCode,SetHoodFanParamOnOff,bool,int)=0;
    DEFINE_METHOD(CActuatorBase,bool,GetIfHoodFanIsOn);

    //Actuator Statistcs
    void DisplayStatistics(void);
    void ResetStatistics(void);

};


class CActuator : public CActuatorBase {

 private:
   // Flags to control the waiting and cancel operation
   bool m_Waiting;
   bool m_CancelFlag;

   // Protection mutex for the cancel flag
   CQMutex m_CancelFlagMutex;
   CQMutex m_ActuatorStatusMutex;
   
   //Flag Actuator during command performance.
   bool m_FlagTimedCommandUnderUse;
   bool m_FlagRSSCommandUnderUse; //RSS, itamar
   bool m_FlagSetOnOffUnderUse;
   bool m_FlagGetStatusUnderUse;
   bool m_FlagGetRSSStatusUnderUse;//RSS, itamar
   bool m_FlagInputStatusUnderUse;
   bool m_FlagGetModelSensor;
   bool m_FlagGetModelStatus;
   bool m_FlagResetModelSensor;
   bool m_OdourFanInUse;

   //Flag ack ok
   bool OnOff_AckOk;
   bool TimedOnOff_AckOk;
   bool RSS_AckOk; //RSS, itamar
   bool ResetModel_AckOk;

   //This procedure is the callback for Set Actuator OnOff OCB
   static void OnOffAckResponse (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for timed Set Actuator OnOff OCB
   static void TimedOnOffAckResponse (int TransactionId,PVOID Data,
                                      unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Roller Suction System Activation OCB
   static void ActivateRollerSuctionSystemAckResponse (int TransactionId,PVOID Data,
                                      unsigned DataLength,TGenericCockie Cockie); //RSS, itamar

   //Get status ack response
   static void GetStatusAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   static void GetRSSStatusAckResponse(int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie); //RSS, itamar

   //Input status ack response
   static void InputStatusAckResponse(int TransactionId,PVOID Data,
                                      unsigned DataLength,TGenericCockie Cockie);

   //Input status ack response
   static void HoodFanOnOffResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);


public:
    // Constructor
    CActuator(const QString& Name);

    // Destructor
    ~CActuator(void);
    
    // Command to perform actuatorto the 8051 h/w.
    TQErrCode TimedSetOnOff (int,bool,int);

    // Command to perform actuatorto the 8051 h/w.
    TQErrCode ActivateRollerSuctionSystem (int,int,bool);  //RSS, itamar

    //This procedure is called in order to wait for 8051 to actuator end.
    TQErrCode SetOnOff(int,bool);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetActuatorStatus(void);

    TQErrCode GetRollerSuctionSystemStatus(void); //RSS, itamar

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetInputStatus(void);
    
    void Cancel(void);

    // Set Hood fan On, with low velocity
    TQErrCode SetHoodFanIdle();

    // Turn the hood fan on/off
    TQErrCode SetHoodFanOnOff(bool OnOff);

    // Turn the hood fan on/off
    TQErrCode SetHoodFanParamOnOff(bool OnOff, int Voltage);
};

class CActuatorDummy : public CActuatorBase {
public:
    // Constructor
    CActuatorDummy(const QString& Name);

    // Destructor
    ~CActuatorDummy(void);

    // Command to perform actuatorto the 8051 h/w.
    TQErrCode TimedSetOnOff (int,bool,int);

    // Command to perform actuatorto the 8051 h/w.
    TQErrCode ActivateRollerSuctionSystem (int,int,bool);  //RSS, itamar


    //This procedure is called in order to wait for 8051 to actuator end.
    TQErrCode SetOnOff(int,bool);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetActuatorStatus(void);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetRollerSuctionSystemStatus(void); //RSS, itamar

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetInputStatus(void);

    void Cancel(void) {}

    // Set Hood fan On, with low velocity
    TQErrCode SetHoodFanIdle();
    
    // Turn the hood fan on/off
    TQErrCode SetHoodFanOnOff(bool OnOff);

    // Turn the hood fan on/off
    TQErrCode SetHoodFanParamOnOff(bool OnOff, int Voltage);
};


#endif

