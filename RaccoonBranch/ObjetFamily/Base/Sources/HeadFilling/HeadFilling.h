/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Heads filling 8051 h/w                                   *
 * Module Description: This class implement services related to     *
 *                     Heads filling verifications (OCB)            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/01/2002                                           *
 ********************************************************************/

#ifndef _HEADFILLING_H_
#define _HEADFILLING_H_

#include <iostream>
#include "OCBProtocolClient.h"
#include "OHDBProtocolClient.h"
#include "OCBCommDefs.h"
#include "OHDBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "Errorhandler.h"
#include "Q2RTErrors.h"
#include "Purge.h"
#include "Container.h"
//#include "HysteresisValue.h"

const int OCB_HEAD_FILLING_ON_TIMEOUT_IN_SEC = 1000;
const BYTE HEADS_FILLING_ON  = 0x01;
const BYTE HEADS_FILLING_OFF = 0x0;
typedef enum
{
HEAD_FILLING_NO_ERROR          = 0,
ERROR_HeadsTemperatureTooHigh,
ERROR_HeadsTemperatureTooLow,
ERROR_SupportEOL,   // this is filling Timeout
ERROR_ModelEOL,     // this is filling Timeout
ERROR_Model2EOL, // this is filling Timeout
ERROR_Model3EOL, // this is filling Timeout
ERROR_S_M3_EOL,
ERROR_M1_M2_EOL,
ERROR_ContainerNotAllowed
} TOCBError;

typedef enum{
  I2C_NO_ERROR,
  MSC1_NO_SLAVE,
  MSC1_HW_ERROR,
  MSC1_COM_ERROR,
  MSC2_NO_SLAVE,
  MSC2_HW_ERROR,
  MSC2_COM_ERROR,
  I2C_ERROR,
  NUM_OF_ERROR_TYPES = MSC1_COM_ERROR+1, //including I2C_ERROR which is common for both MSCs
  NUM_OF_ERRORS
  }I2C_ERROR_MSG;

using namespace std;

const int TANK_ACTIVATED = 1;

// Exception class for all the QLib RTTI elements
class EHeadFilling : public EQException {
  public:
    EHeadFilling(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};             

// C Heads filling base class
class CHeadFillingBase : public CQComponent
{
private:
   DEFINE_VAR_PROPERTY(bool,HeadFillingSensorOnOff);   //On=true---Off=false
   //bool HeadFillingSensorOnOff;   //On=true---Off=false
         
   int m_FillingTimeout;                             // set param
   int m_TimePumpOn;                                 // set param
   int m_TimePumpOff;                                // set param

   int m_LastError;

   int m_CurrentFullLevel[NUM_OF_CHAMBERS_THERMISTORS];  //Current value of Heads filling sensor //OBJET NEW BLOCK
   BYTE m_TanksID        [NUMBER_OF_CHAMBERS];  // received from OCB

   bool m_ReportTemperatureError;
   bool m_HeadFillingAllowed;

protected:
   COCBProtocolClient*  m_OCBClient;
   COHDBProtocolClient* m_OHDBClient;
   //OBJET NEW BLOCK
   int m_lastThermistorIndex;
   int m_startThermistorIndex;

   int m_activeThermistors[NUM_OF_CHAMBERS_THERMISTORS];    
   int m_ThermistorFullLow[NUM_OF_CHAMBERS_THERMISTORS];   
   int m_ThermistorFullHigh[NUM_OF_CHAMBERS_THERMISTORS];    
   
   // Flags to control the waiting and cancel operation
   bool m_Waiting;

   // Protection mutex for database
   CQMutex m_MutexDataBase;
   CQEvent m_SyncEventWaitForFilled;

   CErrorHandler *m_ErrorHandlerClient;
   CAppParams    *m_ParamsMgr;

   //Special procedure to update Head Fillings Database
   void UpdateOnOff(bool);
   void UpdateCurrentThermistorFull (int,int);
   void UpdateSetThermistorFullLow (int,int);
   void UpdateSetThermistorFullHigh (int,int);

   void UpdateSetFillingTimeout (int);
   void UpdateSetTimePumpOn (int);
   void UpdateSetTimePumpOff (int);

   void UpdateLastError(int);
   void ClearError(bool);

 public:
   // Constructor
   CHeadFillingBase(const QString& Name);

   // Destructor
   ~CHeadFillingBase(void);

   //Special procedure to check CHeadFillingBase Database
   DEFINE_METHOD(CHeadFillingBase,bool,GetOnOff);
   DEFINE_METHOD(CHeadFillingBase,TQErrCode,DisplayHeadFillingData);
   DEFINE_METHOD(CHeadFillingBase,int,GetLastError);
   
   DEFINE_METHOD_1(CHeadFillingBase,int,GetCurrentThermistorFull,int);
   DEFINE_METHOD_1(CHeadFillingBase,bool,GetIfCurrentThermistorIsFull,int);
   TQErrCode GetIfThermistorsAreWorking(void);

   DEFINE_METHOD(CHeadFillingBase,int,GetSetFillingTimeout);
   DEFINE_METHOD(CHeadFillingBase,int,GetSetTimePumpOn);
   DEFINE_METHOD(CHeadFillingBase,int,GetSetTimePumpOff);

   DEFINE_METHOD_2(CHeadFillingBase,TQErrCode,UpdateActiveTanks,BYTE,BYTE);
   DEFINE_METHOD_1(CHeadFillingBase,char,GetActiveTank,int);

   // Command to turn on CHeadFillingBase sensors. 
   DEFINE_V_METHOD_1(CHeadFillingBase,TQErrCode,HeadFillingOnOff,bool);
    
   // Command to set CHeadFillingBase parameters
   DEFINE_V_METHOD(CHeadFillingBase,TQErrCode,SetDefaultParms)=0;
   DEFINE_METHOD_1(CHeadFillingBase,TQErrCode,AllowHeadFilling,bool);
   DEFINE_METHOD(CHeadFillingBase,bool,IsHeadFillingAllowed);

   //virtual TQErrCode SetParms()=0;
    
   //This procedure send Get heads fillings status from OCB
   virtual TQErrCode GetHeadsFillingsStatus()=0;
   virtual TQErrCode GetHeadsFillingsActiveTanks()=0;
   //OBJET_MACHINE
   virtual TQErrCode UpdateActiveThermistors()=0;
   ////OBJET_MACHINE, update which thermistors should be used for MRW in HSW
   virtual TQErrCode SetActiveThermistorsAccordingToParamMngr()=0;

   DEFINE_METHOD(CHeadFillingBase,TQErrCode,GetAllHeadsFillingsStatus);

   //This procedure send Get status message from HeadFilling-from OHDB.
   DEFINE_V_METHOD(CHeadFillingBase,TQErrCode,GetMaterialSensorStatus)=0;

   // Command to wait to head container to be filled (full)
   DEFINE_V_METHOD(CHeadFillingBase,TQErrCode,WaitForFilledHeadContainer)=0;

   //Test procedure
    DEFINE_METHOD(CHeadFillingBase,TQErrCode,Test);
    DEFINE_METHOD_1(CHeadFillingBase,TQErrCode,TestUpdateThermistor,int);


   // Cancel wait operations
   virtual void Cancel(void)=0;

   void SetReportTemperatureError(bool Report)
   {
     m_ReportTemperatureError = Report;
   }

   bool ReportTemperatureError()
   {
     return m_ReportTemperatureError;
   }
};


class CHeadFilling : public CHeadFillingBase {

 private:
   //long m_HeadFillingStatusTimeStamp;
   long m_HeadFillingErrorTimeStamp;
   long m_HeadsFillingStatusTimeStamp;
   bool m_CancelFlag;
   
   // Protection mutex for the cancel flag
   CQMutex m_CancelFlagMutex;

   //Flag Heads filling during command performance.
   bool m_FlagOnOffCommandUnderUse;
   bool m_FlagSetCommandUnderUse;
   bool m_FlagWaitHeadFilledComUnderUse;
   bool m_FlagGetMaterialStatusComUnderUse;
   bool m_FlagGetFillingslStatusComUnderUse;
   bool m_FlagGetActiveTanksComUnderUse;

   //Ack bool
   bool OnOff_AckOk;
   bool Set_AckOk;

   //This procedure is the callback for Set Heads filling On Off sensor
   static void OnOffAckResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Set Heads filling parameters
   static void SetParmsAckResponse (int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie);

   // Material Sensor Status ack response
   static void MaterialSensorStatusAckResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie);

   // Get heads fillings status
   static void GetHeadsFillingsStatusAckResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie);

   // Get heads fillings active pumps response
   static void GetHeadsFillingsActiveTanksAckResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie);

   // Notification Call Backs
   static void NotificationErrorResponse(int TransactionId,PVOID Data,
                                unsigned DataLength,TGenericCockie Cockie);
   // I2C Notification Call Backs
   static void MSCNotificationMessage (int TransactionId,PVOID Data,
								 unsigned DataLength,TGenericCockie Cockie);

   static void I2CDebugMessage (int TransactionId,PVOID Data,
								 unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OCB
   void AckToOcbNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);

   // Heads Filling recovery procedure.
   void Recovery (void);

   // Call back for the 'HeadsFillingNotification' message
   static void HeadsFillingNotification(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
   static void HeadsFillingOnOffNotification(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

public:
    // Constructor
    CHeadFilling(const QString& Name);

    // Destructor
    ~CHeadFilling(void);
       
    // Command to turn on CHeadFillingBase sensors.
    TQErrCode HeadFillingOnOff(bool);

    // Command to set CHeadFillingBase parameters
	TQErrCode SetDefaultParms();

	//OBJET_MACHINE, update which thermistors should be used
	TQErrCode UpdateActiveThermistors();
	TQErrCode SetActiveThermistorsAccordingToParamMngr();
    //This procedure send Get status message from HeadFilling-OCB(8051).
	TQErrCode GetMaterialSensorStatus(void);

	//This procedure send Get Heads Fillings Status OCB.
	TQErrCode GetHeadsFillingsStatus();

    //This procedure send Get Active Pumps to OCB.
    TQErrCode GetHeadsFillingsActiveTanks();

    // Get last received values of active pumps:
    char GetActiveTank(TChamberIndex Chamber);

    // Command to wait to head container to be filled (full)
    TQErrCode WaitForFilledHeadContainer(void);
       
    void Cancel(void);
};


class CHeadFillingDummy : public CHeadFillingBase
{
public:
    // Constructor
    CHeadFillingDummy(const QString& Name);

    // Destructor
    ~CHeadFillingDummy(void);

    // Command to set CHeadFillingBase parameters
    TQErrCode SetDefaultParms (/*USHORT*,USHORT*,USHORT,USHORT,USHORT*/);

    // Command to wait to head container to be filled (full)
   TQErrCode WaitForFilledHeadContainer(void);
             
   // Command to get material level sensor status 
   TQErrCode GetMaterialSensorStatus(void);

   //This procedure send Get Heads Fillings Status OCB.
   TQErrCode GetHeadsFillingsStatus();

   char GetActiveTank(TChamberIndex Chamber);

   //This procedure send Get Heads Fillings Status OCB.
   TQErrCode GetHeadsFillingsActiveTanks();
   TQErrCode HeadFillingOnOff(bool);
   TQErrCode UpdateActiveThermistors();
   TQErrCode SetActiveThermistorsAccordingToParamMngr();
   void Cancel(void);
};

class CMonitorActivator : public CQObject
{
private:
  CHeadFillingBase* m_HeadFilling;
protected:

public:
  CMonitorActivator(CHeadFillingBase* HeadFilling = NULL);
  ~CMonitorActivator();

};


#endif

