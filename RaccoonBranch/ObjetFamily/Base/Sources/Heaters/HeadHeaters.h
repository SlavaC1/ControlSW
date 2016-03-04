/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Head heaters class                                       *
 * Module Description: This class implement services related to the *
 *                     print heads heaters.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 14/08/2001                                           *
 * Last upate: 02/09/2001                                           *
 ********************************************************************/

#ifndef _HEAD_HEATERS_H_
#define _HEAD_HEATERS_H_

#include <assert.h>
#include "QMutex.h"
#include "OHDBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"
#include "LinearInterpolator.h"
#include "Temperature.h"
#include "HeatersDefs.h"
#include "HeadParameters.h"
#include "HeadsMapper.h"
#include "Power.h"                                 
const int NUMBER_OF_POTENTIOMETERS = TOTAL_NUMBER_OF_HEADS_HEATERS;
const unsigned int OHDB_HEAD_TEMPERATURE_TURN_ON_TIMEOUT_IN_SEC = 210;

const int HEAD_NO_ERROR = 0;
const int HEAD_ERROR_VOLTAGE_DROP = 1;
const int HEAD_ERROR_HEATING_TIMEOUT = 2;
const int HEAD_ERROR_HEADS_HEATING_TIMEOUT = 3;
const int HEAD_ERROR_BLOCKS_HEATING_TIMEOUT = 4;
const int HEAD_ERROR_HEAD0_HEATING_TIMEOUT = 11;
const int HEAD_ERROR_HEAD1_HEATING_TIMEOUT = 12;
const int HEAD_ERROR_HEAD2_HEATING_TIMEOUT = 13;
const int HEAD_ERROR_HEAD3_HEATING_TIMEOUT = 14;
const int HEAD_ERROR_HEAD4_HEATING_TIMEOUT = 15;
const int HEAD_ERROR_HEAD5_HEATING_TIMEOUT = 16;
const int HEAD_ERROR_HEAD6_HEATING_TIMEOUT = 17;
const int HEAD_ERROR_HEAD7_HEATING_TIMEOUT = 18;
const int HEAD_ERROR_BLOCK0_HEATING_TIMEOUT = 21;
const int HEAD_ERROR_BLOCK1_HEATING_TIMEOUT = 22;
const int HEAD_ERROR_BLOCK2_HEATING_TIMEOUT = 23;
const int HEAD_ERROR_BLOCK3_HEATING_TIMEOUT = 24;

const int HEAD_HEATER_ON  = 1;
const int HEAD_HEATER_OFF = 0;

const int HEAD_COLD = 0;
const int HEAD_TEMPERATURE_OK = 1;
const int HEAD_HOT = 2;


class CQSingleHeadHeater;

const char *NameOf_THeaterElementsTable = "TOTAL_NUMBER_OF_HEATERS int";
const char *NameOf_TPotentiometerTable = "NUMBER_OF_POTENTIOMETERS int";
const char *NameOf_TOnOffTable = "16 int (for num of bits in a WORD)";
const char *NameOf_TQHeadHeaterTable = "TOTAL_NUMBER_OF_HEATERS CQSingleHeadHeater*";

// THeatersMaskTable is never used in conjunction to the communication, so i left old typedef:
typedef bool THeatersMaskTable[TOTAL_NUMBER_OF_HEATERS];
typedef CQHeadsMapper <int, (int)TOTAL_NUMBER_OF_HEATERS, NameOf_THeaterElementsTable> THeaterElementsTable;
typedef CQHeadsMapper <int, (int)NUMBER_OF_POTENTIOMETERS, NameOf_TPotentiometerTable> TPotentiometerTable;
typedef CQHeadsMapper <int, (int)(16), NameOf_TOnOffTable> TOnOffTable;
typedef CQHeadsMapper <CQSingleHeadHeater*, (int)TOTAL_NUMBER_OF_HEATERS, NameOf_TQHeadHeaterTable> TQHeadHeaterTable;


// Exception class for all the QLib RTTI elements
class EHeadHeater : public EQException {
  public:
    EHeadHeater(const QString& ErrMsg,const TQErrCode ErrCode=0);
    EHeadHeater(const TQErrCode ErrCode);
};

class CHeadTemperature : public CTemperature {
protected:
   static CLinearInterpolator <int> m_A2DToCelcius;
   static CLinearInterpolator <int> m_CelciusToA2D;
public:
   static void LoadTemperatureTable(const QString& FileName);
   int ConvertCelciusToA2D(int);
   int ConvertA2DToCelcius(int);
};

// Forward declaration
class CHeadHeatersBase;
class CHeadHeaters;
class CHeadHeatersDummy;

// Single head heater class
// Note: This class can be created and destroyed only by the CHeadHeaters class
class CQSingleHeadHeater : public CQComponent {
  friend CHeadHeatersBase;
  friend CHeadHeatersDummy;
  friend CHeadHeaters;

  private:
    // Constructor
    CQSingleHeadHeater(const QString& Name,int Index);

    // Destructor
    ~CQSingleHeadHeater(void);

    int m_Index;
    int GetParameter(void);
    void SaveParameter(int);
    int GetStandbyParameter(void);
    void SaveStandbyParameter(int);

    CAppParams *m_ParamsMgr;

    int m_LastStatus;
    int m_TemperatureOk;
    CHeadTemperature m_CurrentTemperature;
    CHeadTemperature m_SetTemperature;
    CHeadTemperature m_SetStandbyTempearture;
    bool m_WorkingHead;

    CQMutex m_MutexSingleHead;

    THeadParameters m_HeadParameters;

    //void UpdateTemperatureOk(bool);
    void UpdateCurrentTemperatureA2D(int);
    void UpdateSetTemperatureA2D(int);
    void UpdateStandbyTemperatureA2D(int);
    void UpdateIfWorkingHead(bool);

    //bool GetTemperatureOk(void);
    int GetCurrentTemperatureInCelcius(void);
    int GetCurrentTemperatureInA2D(void);
    int GetSetTemperatureInA2D(void);
    int GetSetTemperatureInCelcius(void);
    int GetStandbyTemperatureInA2D(void);
    int GetStandbyTemperatureInCelcius(void);
    bool GetIfHeadWorking(void);
    int GetIfTemperatureOk(void);

    void SetHeadParameters(THeadParameters *HeadParameters);
    void GetHeadParameters(THeadParameters *HeadParameters);

  public:
};

// Heads heater control class
class CHeadHeatersBase : public CQComponent
{
private:
    int m_LastError;

    DEFINE_VAR_PROPERTY(bool,HeaterIsOn);

    float m_VppPowerSupply;
    float m_Heater24VPowerSupply;
    float m_VDDPowerSupply;
    float m_VCCPowerSupply;
    float m_RepPot ;

    bool m_TemperatureOkInOCB;

    TPotentiometerTable m_CurrentPotentiometerTable;
    TPotentiometerTable m_SetPotentiometerTable;


  protected:
    // Each head heater is represented by CQSingleHeadHeater class
    // The first 4 are Model and the following 4 are support
    // following 2 blocks model and finally 2 blocks support.

    TQHeadHeaterTable m_Heaters;

    TPotentiometerTable m_ReqPotentiometerValues;

    // the requested head heaters temperatures
    THeaterElementsTable m_HeatersSetTemperatures;


    // Thread execute function (override)
    //void Execute(void);

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

    void UpdateLastError(int);

    void SetHeaterState(bool On);
    void UpdateCurrentVoltageTable(TPotentiometerTable);
    void UpdateSetVoltageTable(TPotentiometerTable);
    void UpdatePowerSupply(float Vpp,float V24,float VDD,float VCC);

    void UpdateIfTemperatureOkInOCB(bool);
    bool GetIfTemperatureOkInOCB(void);

    int m_TemperatureOutOfMargine;
    int m_TemperatureOutOfMargineTotalCount;
  public:
    // Constructor
    CHeadHeatersBase(const QString& Name);

    // Destructor
    ~CHeadHeatersBase(void);

    // Get access to a single head heater
    CQSingleHeadHeater& operator [](unsigned Index) {
      // Range check
      assert(Index < TOTAL_NUMBER_OF_HEATERS);

      return *m_Heaters[Index];
    }

    // Return the number of items (heads)
    unsigned Count(void) {
      return TOTAL_NUMBER_OF_HEATERS;
    }

    // Cancel wait operations
    virtual void Cancel(void)=0;

    USHORT ConvertMaskTableToWord (THeatersMaskTable);

    // Command to turn on/off the Head heater
    DEFINE_METHOD_1(CHeadHeatersBase,TQErrCode,SetDefaultOnOff,bool);
    // Command to turn on/off the Head heater
    virtual TQErrCode SetHeaterOnOff(bool,WORD)=0;

    // Is the heater control on
    DEFINE_METHOD(CHeadHeatersBase,bool,IsHeaterOn);


    // Command to wait to head heater to be on off
    DEFINE_V_METHOD_1(CHeadHeatersBase,TQErrCode,WaitForHeadsTemperatureOK,bool /* ContinueJob */)=0; //(int Timeout)=0;

    // Command to set heater temperature
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,SetDefaultHeateresTemperature);
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,SetStandby2HeateresTemperature);

    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,SetHeatersTemperature)=0;

    // Set a specific head heater requested temperautre
    DEFINE_V_METHOD_2(CHeadHeatersBase,TQErrCode,SetHeaterTemperature,int,int);

    virtual TQErrCode SetHeatersTemperature(THeaterElementsTable)=0;

    // Command to get heaters status
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,GetHeatersStatus)=0;

    // Command to get pre heater status
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,GetPreHeaterStatus)=0;
    
    // Command to get if temperature are in range
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,IsHeadTemperatureOk)=0;

    //Procedure to wait and verify until temperature is ok in Eden and in OCB
    TQErrCode VerifyIfTemperatureOk(void);

    // Command to set heater standby temperature
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,SetDefaultStandbyTemperature);

    virtual TQErrCode SetStandbyTemperature(THeaterElementsTable)=0;

    // Command to get standby heaters tempearture values
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,GetStandbyTemperatures)=0;

    // Display All the Head Status to the monitor
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,DisplayHeadStatus);

    // Display All the Head Status to the Log
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,DebugDisplayHeadStatus);

    // Display All the Head Status to the monitor
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,DisplayVoltageTable);

    DEFINE_METHOD(CHeadHeatersBase,int,GetLastError);

    DEFINE_METHOD(CHeadHeatersBase,float,GetVppPowerSupply);
    DEFINE_METHOD(CHeadHeatersBase,float,Get24VPowerSupply);
    DEFINE_METHOD(CHeadHeatersBase,float,GetVDDPowerSupply);
    DEFINE_METHOD(CHeadHeatersBase,float,GetVCCPowerSupply);
	DEFINE_METHOD(CHeadHeatersBase,bool,IsVppPowerSupplyLegal);
    DEFINE_METHOD(CHeadHeatersBase,bool,IsVDDPowerSupplyLegal);

    DEFINE_METHOD_1(CHeadHeatersBase,int,GetCurrentVoltageTable,int);
    //TQErrCode GetCurrentVoltageTable(TPotentiometerTable &PotentiometerTable);
    DEFINE_METHOD_1(CHeadHeatersBase,int,GetSetVoltageTable,int);

    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,SetDefaultPrintingHeadsVoltages);
    virtual TQErrCode SetPrintingHeadsVoltage(TPotentiometerTable)=0;
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,SetPrintingHeadsVoltage)=0;
    DEFINE_V_METHOD_2(CHeadHeatersBase,TQErrCode,SetPrintingHeadVoltage,int,int)=0;
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,GetPrintingHeadsVoltages)=0;
    DEFINE_V_METHOD(CHeadHeatersBase,TQErrCode,GetPowerSuppliesVoltages)=0;

    //Check for thermistor integrity
    DEFINE_METHOD_1(CHeadHeatersBase,TQErrCode,CheckThermistorsStatus,bool);
    TQErrCode CheckThermistorsIntegrity(int,int);
    // Set the units to be A/D units
    DEFINE_METHOD_1(CHeadHeatersBase,TQErrCode,SetUnitsAsA2D,bool);

    //Tray heater Statistcs
    void DisplayStatistics(void);
    void ResetStatistics(void);

    //Test procedure
    DEFINE_METHOD_1(CHeadHeatersBase,int,GetHeadHeaterParameter,int);
    DEFINE_METHOD_2(CHeadHeatersBase,TQErrCode,SaveHeadHeaterParameter,int,int);


    //Test procedure
    DEFINE_METHOD(CHeadHeatersBase,TQErrCode,Test);

    DEFINE_V_METHOD(CHeadHeatersBase,bool,AreHeadsTemperaturesOk)=0;
    DEFINE_V_METHOD_1(CHeadHeatersBase,bool,IsHeaterTemperatureOk,int)=0;

    DEFINE_METHOD_1(CHeadHeatersBase,int,GetHeadTemperature,int);

    void GetHeadParameters(int,THeadParameters*);
    void SetHeadParameters(int,THeadParameters*);
    virtual void ReadHeadParametersFromE2PROM(int,THeadParameters*)=0;
    virtual void SaveHeadParametersOnE2PROM(int,THeadParameters*)=0;

    // Read/Write single byte from/to head card EEPROM
    DEFINE_V_METHOD_2(CHeadHeatersBase,int,ReadFromE2PROM,int /*HeadNumber*/,USHORT /*Address*/) = 0;
    DEFINE_V_METHOD_3(CHeadHeatersBase,TQErrCode,WriteToE2PROM,int /*HeadNumber*/,USHORT /*Address*/,int /*Data*/) = 0;

    DEFINE_V_METHOD_2(CHeadHeatersBase,TQErrCode,SetAndMeasureHeadVoltage,int,float)=0;
    DEFINE_METHOD_1(CHeadHeatersBase,float,GetCurrentHeadVoltage,int);

    virtual QString GetHeaterErrorDescription();

    void  FindResistorValues(int HeadNum, float &ResistorHigh , float& ResistorLow);
	void SetHeadMinVoltage();
	DEFINE_METHOD_1(CHeadHeatersBase,bool,IsItNewEEProm,int);
    int CalculatePotValue(int HeadNum, float ReqVoltage);

    float EstimateHeadVoltage(int HeadNum, int PotVal);
    
};


class CHeadHeaters : public CHeadHeatersBase {

 private:
   // Semaphore That verify if Ack is received or if Notify response
   // was received.
   CQEvent m_SyncEventWaitForTemperature;
   //CQEvent m_SyncEventVoltage;

   bool m_CancelFlag;

   // Protection mutex for the cancel flag
   //CQMutex m_CancelFlagMutex;

   CQMutex m_ReadHeadParametersMutex;
   CQMutex m_SaveHeadParametersMutex;
   CQMutex m_ReadFromE2PROMMutex;
   CQMutex m_WriteToE2PROMMutex;

   //Flag Head heater during command performance.
   bool m_FlagSetHeaterOnOffComUnderUse;
   bool m_FlagWaitComUnderUse;
   bool m_FlagSetHeaterTemperatureComUnderUse;
   bool m_FlagGetHeaterStatusComUnderUse;
   bool m_FlagIsTemperatureOkComUnderUse;
   bool m_FlagSetHeaterStandbyComUnderUse;
   bool m_FlagGetHeaterStandbyComUnderUse;
   bool m_FlagSetVoltageComUnderUse;
   bool m_FlagGetVoltageComUnderUse;
   bool m_FlagGetPowerSuppliesComUnderUse;

   //Ack Ok
   bool m_SetHeaterOnOff_AckOk;
   bool m_SetHeaterTemperature_AckOk;
   bool m_IsTemperatureOk_AckOk;
   bool m_SetHeaterStandby_AckOk;
   bool m_GetHeaterStandby_AckOk;
   bool m_SetVoltage_AckOk;
   bool m_GetVoltage_AckOk;
   bool m_GetPowerSupplies_AckOk;
   bool m_GetHeadData_AckOk;
   bool m_SetHeadData_AckOk;
   bool m_ReadFromE2PROM_AckOk;
   bool m_WriteToE2PROM_AckOk;

   BYTE m_E2PROMData;

   bool m_HeatersOutOfLimits[TOTAL_NUMBER_OF_HEATERS];


   //This procedure is the callback for SetHeaterOnOff
   static void SetHeaterOnOffAckResponse (int TransactionId,PVOID Data,
                                          unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for SetHeaterTemperature parameters
   static void SetHeaterTemperatureAckResponse (int TransactionId,PVOID Data,
                                                unsigned DataLength,TGenericCockie Cockie);

   //Get Heater status ack response
   static void GetHeaterStatusAckResponse(int TransactionId,PVOID Data,
                                          unsigned DataLength,TGenericCockie Cockie);

   //IsTempeartureOk ack response
   static void IsTemperatureOkAckResponse(int TransactionId,PVOID Data,
                                          unsigned DataLength,TGenericCockie Cockie);

   //SetHeaterStandby ack response
   static void SetHeaterStandbyAckResponse(int TransactionId,PVOID Data,
                                           unsigned DataLength,TGenericCockie Cockie);
   
   //GetHeaterStandby ack response
   static void GetHeaterStandbyAckResponse(int TransactionId,PVOID Data,
                                           unsigned DataLength,TGenericCockie Cockie);

   // Notification Call Backs
   static void NotificationHeadHeaterErrorResponse(int TransactionId,PVOID Data,
                                                   unsigned DataLength,TGenericCockie Cockie);

   static void NotificationHeadsTemperatureError(int TransactionId,PVOID Data,
                                                 unsigned DataLength,TGenericCockie Cockie);

   //Potentiometer/Voltage responses
   static void SetPrintingHeadsVoltageResponse(int TransactionId,PVOID Data,
                                               unsigned DataLength,TGenericCockie Cockie);
   static void GetPrintingHeadsVoltagesResponse(int TransactionId,PVOID Data,
                                                unsigned DataLength,TGenericCockie Cockie);
   static void GetPowerSuppliesVoltagesResponse(int TransactionId,PVOID Data,
                                                unsigned DataLength,TGenericCockie Cockie);

   static void ReadHeadDataResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

   static void SaveHeadDataResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

   static void ReadFromE2PROMResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

   static void WriteToE2PROMResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

   static void SaveHeadDataErrorNotification(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);



   // Replay Ack to Ohdb
   void AckToOhdbNotification (int MessageID,
                               int TransactionID,
                               int AckStatus,
                               TGenericCockie Cockie);

   // head heater recovery procedure.
   void Recovery (void);

public:
    // Constructor
    CHeadHeaters(const QString& Name);

    // Destructor
    ~CHeadHeaters(void);

    // Command to turn on/off the Head heater 
    TQErrCode SetHeaterOnOff(bool,WORD);

    // Command to wait to head heater to be on off
    TQErrCode WaitForHeadsTemperatureOK(bool ContinueJob);

    TQErrCode SetHeatersTemperature();

    TQErrCode SetHeatersTemperature(THeaterElementsTable);
    
    // Command to get heaters status
    TQErrCode GetHeatersStatus(void);

    // Command to get pre heater status
    TQErrCode GetPreHeaterStatus(void);

    // Command to get if temperature are in range
    TQErrCode IsHeadTemperatureOk(void);
    
    TQErrCode SetStandbyTemperature(THeaterElementsTable);
    
    // Command to get standby heaters tempearture values 
    TQErrCode GetStandbyTemperatures(void);
    
    //Potentiometer
    TQErrCode SetPrintingHeadsVoltage(TPotentiometerTable);
    TQErrCode SetPrintingHeadsVoltage();
    TQErrCode SetPrintingHeadVoltage(int, int);
    TQErrCode GetPrintingHeadsVoltages(void);
    TQErrCode GetPowerSuppliesVoltages(void);

    void Cancel(void);

    bool AreHeadsTemperaturesOk();
    bool IsHeaterTemperatureOk (int HeaterNum);

    void ReadHeadParametersFromE2PROM(int HeadNumber, THeadParameters* HeadParameters);
    void SaveHeadParametersOnE2PROM(int HeadNumber, THeadParameters* HeadParameters);

    int ReadFromE2PROM(int HeadNumber,USHORT Address);
    TQErrCode WriteToE2PROM(int HeadNumber,USHORT Address,int Data);

    TQErrCode SetAndMeasureHeadVoltage(int HeadNumber, float ReqVoltage);
    void ResetHeatersStatusDatabase();
};


class CHeadHeatersDummy : public CHeadHeatersBase {
public:
   // Constructor
   CHeadHeatersDummy(const QString& Name);

   // Destructor
   ~CHeadHeatersDummy(void);

   // Command to turn on/off the Head heater
   TQErrCode SetHeaterOnOff(bool,WORD);

   // Command to wait to head heater to be on off
   TQErrCode WaitForHeadsTemperatureOK(bool ContinueJob);

   TQErrCode SetHeatersTemperature();

   TQErrCode SetHeatersTemperature(THeaterElementsTable);

   // Command to get heaters status
   TQErrCode GetHeatersStatus(void);

   // Command to get pre heater status
   TQErrCode GetPreHeaterStatus(void);

   // Command to get if temperature are in range
   TQErrCode IsHeadTemperatureOk(void);

   TQErrCode SetStandbyTemperature(THeaterElementsTable);

   // Command to get standby heaters tempearture values
   TQErrCode GetStandbyTemperatures(void);

   //Potentiometer
   TQErrCode SetPrintingHeadsVoltage(TPotentiometerTable);
   TQErrCode SetPrintingHeadsVoltage();
   TQErrCode SetPrintingHeadVoltage(int, int);
   TQErrCode GetPrintingHeadsVoltages(void);
   TQErrCode GetPowerSuppliesVoltages(void);

   void Cancel(void);

   bool AreHeadsTemperaturesOk();
   bool IsHeaterTemperatureOk (int HeaterNum);

   void ReadHeadParametersFromE2PROM(int HeadNumber, THeadParameters* HeadParameters);
   void SaveHeadParametersOnE2PROM(int HeadNumber, THeadParameters* HeadParameters);

   int ReadFromE2PROM(int HeadNumber,USHORT Address);
   TQErrCode WriteToE2PROM(int HeadNumber,USHORT Address,int Data);

   TQErrCode SetAndMeasureHeadVoltage(int HeadNumber, float ReqVoltage);
   QString GetHeaterErrorDescription();
	TQErrCode CheckThermistorsIntegrity(int Head,int A2DValue);
};

#endif

