/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Head vacuum sensor class                                 *
 * Module Description: This class implement services related to the *
 *                     vacuum                                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 14/08/2001                                           *
 * Last upate: 02/09/2001                                           *
 ********************************************************************/

#ifndef _HEAD_VACUUM_H_
#define _HEAD_VACUUM_H_

#include <assert.h>
#include "QMutex.h"
#include "OHDBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"
#include "LinearInterpolator.h"
#include "Vacuum.h"

const int VACUUM_STATUS_OK = 0;
const int VACUUM_STATUS_WRONG = 1;

const int MAX_VACUUM_HISTORY_VALUES = 20;

// Exception class for all the QLib RTTI elements
class EVacuum : public EQException {
  public:
    EVacuum(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};          

class CSensorVacuum : public CVacuum {
protected:
   static CLinearInterpolator <float> m_A2DToPressure;
   static CLinearInterpolator <float> m_PressureToA2D;
public:
   static void LoadTemperatureTable(const QString& FileName);
   float ConvertVacuumA2DToPressure(int);
   int ConvertVaccumPressureToA2D(float);
};

// Forward declaration
class CHeadVacuumBase;
class CHeadVacuum;
class CHeadVacuumDummy;
class CPurgeBase;
      
// Heads heater control class
class CHeadVacuumBase : public CQComponent {
  private:
    int m_CurrentVacuumIndex;
    CSensorVacuum m_VacuumSensorValue[MAX_VACUUM_HISTORY_VALUES];  //Current Values
    float m_VacuumAverage;
    int m_VacuumLastNValues;
    int m_AverageIndex;
    bool m_VaccumLastStatus;
    int m_VacuumSamplesToAverage;

    CPurgeBase *m_PurgeInstance;

  protected:
    COHDBProtocolClient *m_OHDBClient;

    CErrorHandler *m_ErrorHandlerClient;
    CAppParams    *m_ParamsMgr;

    // Protection mutex for the cancel flag
    CQMutex m_CancelFlagMutex;

    // Flags to control the waiting and cancel operation
    bool m_Waiting;

    // Protection mutex for datyabase
    CQMutex m_MutexDataBase;

     // is temperature is returned in A/D units or in celsius degrees
    bool m_A2DUnits;

    bool m_EnableErrorHandling;

    void UpdateA2DVacuumSensorValue(int);
    int m_VacuumOutOfRangeCounter;
    int m_VacuumTotalOutOfRangeCounter;
    int m_VacuumAverageOutOfRange;

  public:
    // Constructor
    CHeadVacuumBase(const QString& Name,CPurgeBase *PurgeInstance);

    // Destructor
    ~CHeadVacuumBase(void);

    // Cancel wait operations
    virtual void Cancel(void)=0;

    // Command to set vacuum parameters message
    DEFINE_METHOD(CHeadVacuumBase,TQErrCode,SetDefaultVacuumParams);

    // Command to set vacuum parameters message
    virtual TQErrCode SetVacuumParams (int,int)=0;

    // Command to get if vacuum is ok
    DEFINE_V_METHOD(CHeadVacuumBase,TQErrCode,IsVacuumOk)=0;

    // Command to get vacuum status
    DEFINE_V_METHOD(CHeadVacuumBase,TQErrCode,GetVacuumSensorStatus)=0;

    // Disable vacuum error handling, return previous state
    DEFINE_METHOD_1(CHeadVacuumBase,bool,EnableDisableErrorHandling,bool /*Enable*/);

    DEFINE_METHOD_1(CHeadVacuumBase,float,ConvertA2DToCmH20,int /* A2D Value */);

    DEFINE_METHOD(CHeadVacuumBase,float,GetVacuumAverageSensorValue_CmH2O);
    DEFINE_METHOD(CHeadVacuumBase,int,GetVacuumAverageSensorValue_A2D);
    float GetLastVacuumCmH20Value(void);
    int GetLastVacuumA2DValue(void);
    bool CheckVacuumSensorValue(void);

    //Tray heater Statistcs
    void DisplayStatistics(void);
    void ResetStatistics(void);

    //Test procedure
    DEFINE_METHOD(CHeadVacuumBase,TQErrCode,Test);
};     


class CHeadVacuum : public CHeadVacuumBase {

 private:
   bool m_CancelFlag;

   // Protection mutex for the cancel flag
   //CQMutex m_CancelFlagMutex;

   //Flag Head heater during command performance.
   bool m_FlagSetVacuumComUnderUse;
   bool m_FlagIsVacuumOkComUnderUse;
   bool m_FlagGetVacuumStatusComUnderUse;

   //Time between sent vacuum report
   time_t m_LastVacuumReport;
   
   //Ack Ok
   bool m_SetVacuum_AckOk;
   bool m_IsVacuumOk_AckOk;

   //SetVacuum ack response
   static void SetVacuumAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   //IsVacuumOk ack response
   static void IsVacuumOkAckResponse(int TransactionId,PVOID Data,
                                     unsigned DataLength,TGenericCockie Cockie);

   //GetVacuumStatus ack response
   static void GetVacuumSensorStatusAckResponse(int TransactionId,PVOID Data,
                                                unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to Ohdb
   void AckToOhdbNotification (int MessageID,
                               int TransactionID,
                               int AckStatus,
                               TGenericCockie Cockie);

   // head heater recovery procedure.
   void Recovery (void);

public:
    // Constructor
    CHeadVacuum(const QString& Name,CPurgeBase *PurgeInstance);

    // Destructor
    ~CHeadVacuum(void);

    // Command to set vacuum parameters message
    TQErrCode SetVacuumParams(int,int);

    // Command to get if vacuum is ok 
    TQErrCode IsVacuumOk(void);

    // Command to get vacuum status
    TQErrCode GetVacuumSensorStatus(void);

    void Cancel(void);
};


class CHeadVacuumDummy : public CHeadVacuumBase {
public:
   // Constructor
   CHeadVacuumDummy(const QString& Name,CPurgeBase *PurgeInstance);

   // Destructor
   ~CHeadVacuumDummy(void);

   // Command to set vacuum parameters message
   TQErrCode SetVacuumParams(int,int);

   // Command to get if vacuum is ok 
   TQErrCode IsVacuumOk(void);

   // Command to get vacuum status  
   TQErrCode GetVacuumSensorStatus(void);

   DEFINE_METHOD_1(CHeadVacuumDummy,TQErrCode,PutNewVacuumValue,int);
   void Cancel(void);
};


#endif

