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
 * Author: Gedalia Trejger                                          *
 * Start date: 14/08/2001                                           *
 * Last upate: 02/09/2001                                           *
 ********************************************************************/

#include "HeadHeaters.h"
#include "QMonitor.h"
#include "QUtils.h"
#include "QThreadUtils.h"
#include "OHDBCommDefs.h"
#include "Q2RTErrors.h"
#include "QApplication.h"
#include "Q2RTApplication.h"
#include "QTimer.h"
#include "AppParams.h"
#include "Q2RTErrors.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include <math.h>
#include "MachineManager.h"
#include "FEResources.h"
#include "values.h" // needed for MAXFLOAT

// Constants:
//
const int ACK_STATUS_SUCCESS = 0;
const int ACK_STATUS_FAILURE = 1;

const bool TEMPERATURE_OK = true;
const bool TEMPERATURE_NOT_OK = false;

const int THERMISTOR_SHORT_VALUE = 375;
const int THERMISTOR_OPEN_VALUE = 4050;

const float HEAD_VOLTAGE_TOLERANCE             = 1.0;
const int   HEAD_VOLTAGES_STABILIZATION_TIME   = 50;
const int   WRITE_PARAMETERS_TO_E2PROM_TIMEOUT = QMsToTicks(2500);

const int VOLTAGE_STABILIZATION_TIME = 60; // Sec
const int CALIBRATION_FAIL_RETRIES = 3; // number of retries for calibrating head voltage.
const int FORCED_RETRIES           = 1; // number of times we force the calibration process to "cross" the calibration point.

//const float Rpot = 1.2;   // Potentiometer   - Need to change to 1.2 - Moshe Levi
const float Vref = 1.21;  // V reference

#define CONVERT_VPP_A2D_TO_VOLT(x) (21 * 2.4 * (x) / 4096)
#define CONVERT_24V_A2D_TO_VOLT(x) (11 * 2.4 * (x) / 4096)
#define CONVERT_VCC_A2D_TO_VOLT(x) (2.5 * 2.4 * (x) / 4096)
#define CONVERT_VDD_A2D_TO_VOLT(x) (5.75 * 2.4 * (x) / 4096)
#define CONVERT_HEAD_VPP_A2D_TO_VOLT(x) (16 * 2.4 * (x) / 1024)


//Functions For Class CHeadTemperature
//------------------------------------
// Static members
CLinearInterpolator<int> CHeadTemperature::m_A2DToCelcius;
CLinearInterpolator<int> CHeadTemperature::m_CelciusToA2D;

EHeadHeater::EHeadHeater(const QString& ErrMsg,const TQErrCode ErrCode) : EQException(ErrMsg,ErrCode) {}
EHeadHeater::EHeadHeater(const TQErrCode ErrCode) : EQException(PrintErrorMessage(ErrCode),ErrCode) {}

void CHeadTemperature::LoadTemperatureTable(const QString& FileName)
{
   m_A2DToCelcius.LoadFromFile(FileName);
   m_CelciusToA2D.LoadInverseFromFile(FileName);
}

int CHeadTemperature::ConvertCelciusToA2D(int value) {
   return (m_CelciusToA2D.Interpolate(value));
}

int CHeadTemperature::ConvertA2DToCelcius(int value){
   return (m_A2DToCelcius.Interpolate(value));
}

// Class CQSingleHeadHeater implementation
// ------------------------------------------------------------------

// Constructor
CQSingleHeadHeater::CQSingleHeadHeater(const QString& Name,int Index) : CQComponent(Name)
{
   m_Index         = Index;
   m_LastStatus    = HEAD_COLD;
   m_TemperatureOk = HEAD_COLD;
   m_WorkingHead   = false;
   m_ParamsMgr     = CAppParams::Instance();
   UpdateCurrentTemperatureA2D(GetParameter());
}

// Destructor
CQSingleHeadHeater::~CQSingleHeadHeater(void)
{}

int CQSingleHeadHeater::GetParameter(void)
{
   return m_ParamsMgr->HeatersTemperatureArray[m_Index];
}

void CQSingleHeadHeater::SaveParameter(int Value)
{
   m_ParamsMgr->HeatersTemperatureArray[m_Index]=Value;
   m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->HeatersTemperatureArray);
   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Save Head Heater %s, Value =%d",
                                     GetHeaterStr(m_Index).c_str(),
                                     Value);
}

int CQSingleHeadHeater::GetStandbyParameter(void)
{
   return m_ParamsMgr->StandbyTemperatureArray[m_Index];
}

void CQSingleHeadHeater::SaveStandbyParameter(int Value)
{
   m_ParamsMgr->StandbyTemperatureArray[m_Index]=Value;
   m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->StandbyTemperatureArray);
   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Save standby head heater values %s, Value =%d",
                                     GetHeaterStr(m_Index).c_str(),
                                     Value);
}


bool CQSingleHeadHeater::GetIfHeadWorking(void)
{
   bool IsWorking;

   m_MutexSingleHead.WaitFor();
      IsWorking = m_WorkingHead;
   m_MutexSingleHead.Release();
   return IsWorking;
}

void CQSingleHeadHeater::UpdateIfWorkingHead(bool IsWorking)
{
   m_MutexSingleHead.WaitFor();
      m_WorkingHead=IsWorking;
   m_MutexSingleHead.Release();
}

int CQSingleHeadHeater::GetIfTemperatureOk(void)
{
   int TemperatureOk;

   m_MutexSingleHead.WaitFor();
      TemperatureOk = m_TemperatureOk;
   m_MutexSingleHead.Release();
   return TemperatureOk;
}

int CQSingleHeadHeater::GetCurrentTemperatureInCelcius(void)
{
   int Temperature;

   m_MutexSingleHead.WaitFor();
      Temperature = m_CurrentTemperature.CelciusValue();
   m_MutexSingleHead.Release();
   return (Temperature);
}

int CQSingleHeadHeater::GetCurrentTemperatureInA2D(void)
{

#ifdef _DEBUG
   if(FindWindow(0, "HeaterThermistorShort.txt - Notepad"))
      return THERMISTOR_SHORT_VALUE - 1;
   if(FindWindow(0, "HeaterThermistorOpen.txt - Notepad"))
      return THERMISTOR_OPEN_VALUE + 1;
#endif


   int Temperature;

   m_MutexSingleHead.WaitFor();
      Temperature = m_CurrentTemperature.A2DValue();
   m_MutexSingleHead.Release();
   return (Temperature);
}

void CQSingleHeadHeater::UpdateCurrentTemperatureA2D(int TA2D)
{
  static CAppParams *ParamsMgr=CAppParams::Instance();
  int LowThreshold  = ParamsMgr->HeadsTemperatureLowThershold;
  int HighThreshold = ParamsMgr->HeadsTemperatureHighThershold;;

  // Histheresis - while reaching the set temperature
  if (m_LastStatus != HEAD_TEMPERATURE_OK)
  {
    LowThreshold  -= 50;
    HighThreshold -= 50;
  }

  m_MutexSingleHead.WaitFor();
  m_CurrentTemperature.AssignFromA2D(TA2D);

  if (m_CurrentTemperature < (m_SetTemperature.A2DValue() - HighThreshold))
     m_TemperatureOk = HEAD_HOT;
  else if(m_CurrentTemperature > (m_SetTemperature.A2DValue() + LowThreshold))
     m_TemperatureOk = HEAD_COLD;
  else
     m_TemperatureOk = HEAD_TEMPERATURE_OK;
  m_LastStatus = m_TemperatureOk;
  m_MutexSingleHead.Release();
}

int CQSingleHeadHeater::GetSetTemperatureInCelcius(void)
{
   int Temperature;

   m_MutexSingleHead.WaitFor();
      Temperature = m_SetTemperature.CelciusValue();
   m_MutexSingleHead.Release();
   return Temperature;
}

int CQSingleHeadHeater::GetSetTemperatureInA2D(void)
{
   int Temperature;

   m_MutexSingleHead.WaitFor();
      Temperature = m_SetTemperature.A2DValue();
   m_MutexSingleHead.Release();
   return Temperature;
}

void CQSingleHeadHeater::UpdateSetTemperatureA2D(int TA2D)
{
   static CAppParams *ParamsMgr=CAppParams::Instance();

   m_MutexSingleHead.WaitFor();
      m_SetTemperature.AssignFromA2D(TA2D);
   m_MutexSingleHead.Release();

   //Refresh temperature status
   UpdateCurrentTemperatureA2D(m_CurrentTemperature.A2DValue());
}

int CQSingleHeadHeater::GetStandbyTemperatureInCelcius(void)
{
   int Temperature;

   m_MutexSingleHead.WaitFor();
      Temperature = m_SetStandbyTempearture.CelciusValue();
   m_MutexSingleHead.Release();
   return (Temperature);
}

int CQSingleHeadHeater::GetStandbyTemperatureInA2D(void)
{
   int Temperature;

   m_MutexSingleHead.WaitFor();
      Temperature = m_SetStandbyTempearture.A2DValue();
   m_MutexSingleHead.Release();
   return (Temperature);
}

void CQSingleHeadHeater::UpdateStandbyTemperatureA2D(int TA2D)
{
   m_MutexSingleHead.WaitFor();
      m_SetStandbyTempearture.AssignFromA2D(TA2D);
   m_MutexSingleHead.Release();
}

void CQSingleHeadHeater::SetHeadParameters(THeadParameters *HeadParameters)
{
   m_MutexSingleHead.WaitFor();
   memcpy(&m_HeadParameters, HeadParameters, sizeof(THeadParameters));
   m_MutexSingleHead.Release();
}

void CQSingleHeadHeater::GetHeadParameters(THeadParameters *HeadParameters)
{
   m_MutexSingleHead.WaitFor();
   memcpy(HeadParameters, &m_HeadParameters, sizeof(THeadParameters));
   m_MutexSingleHead.Release();
}



// Class CHeadHeaters implementation
// ------------------------------------------------------------------

// Constructor - create the thread in suspend mode
CHeadHeatersBase::CHeadHeatersBase(const QString& Name) : CQComponent(Name)
{
  int i;

  // Create and assign unique name for each head-heater component
  for(i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
     m_Heaters[i] = new CQSingleHeadHeater(QReplaceSubStr(GetHeaterStr(i), " ", "_"),i);
                                       
  // Initialize linear interpolation
  try
  {
    CHeadTemperature::LoadTemperatureTable(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONFIGS_DIRNAME) + "HeadHeater.txt");
  }
  // Catch file loading related errors
  catch(EQStringList& Err)
  {
    QMonitor.WarningMessage(Err.GetErrorMsg());
  }

  m_Waiting=false;
  m_ParamsMgr = CAppParams::Instance();

  m_LastError=0;

  INIT_VAR_PROPERTY(HeaterIsOn,false);
  INIT_METHOD(CHeadHeatersBase,SetDefaultOnOff);
  INIT_METHOD(CHeadHeatersBase,WaitForHeadsTemperatureOK);
  INIT_METHOD(CHeadHeatersBase,SetDefaultHeateresTemperature);
  INIT_METHOD(CHeadHeatersBase,SetStandby2HeateresTemperature);
  INIT_METHOD(CHeadHeatersBase,GetHeatersStatus);
  INIT_METHOD(CHeadHeatersBase,GetPreHeaterStatus);
  INIT_METHOD(CHeadHeatersBase,IsHeadTemperatureOk);
  INIT_METHOD(CHeadHeatersBase,SetDefaultStandbyTemperature);
  INIT_METHOD(CHeadHeatersBase,GetStandbyTemperatures);
  INIT_METHOD(CHeadHeatersBase,DisplayHeadStatus);
  INIT_METHOD(CHeadHeatersBase,DebugDisplayHeadStatus);

  INIT_METHOD(CHeadHeatersBase,GetLastError);
  INIT_METHOD(CHeadHeatersBase,Test);
  INIT_METHOD(CHeadHeatersBase,SetHeatersTemperature);
  INIT_METHOD(CHeadHeatersBase,SetHeaterTemperature);
  INIT_METHOD(CHeadHeatersBase,SetUnitsAsA2D);
  INIT_METHOD(CHeadHeatersBase,IsHeaterOn);
  INIT_METHOD(CHeadHeatersBase,SetDefaultPrintingHeadsVoltages);
  INIT_METHOD(CHeadHeatersBase,GetPrintingHeadsVoltages);
  INIT_METHOD(CHeadHeatersBase,GetPowerSuppliesVoltages);
  INIT_METHOD(CHeadHeatersBase,GetCurrentVoltageTable);
  INIT_METHOD(CHeadHeatersBase,SetPrintingHeadsVoltage);
  INIT_METHOD(CHeadHeatersBase,SetPrintingHeadVoltage);
  INIT_METHOD(CHeadHeatersBase,GetVppPowerSupply);
  INIT_METHOD(CHeadHeatersBase,IsVppPowerSupplyLegal);
  INIT_METHOD(CHeadHeatersBase,IsVDDPowerSupplyLegal);
  INIT_METHOD(CHeadHeatersBase,Get24VPowerSupply);
  INIT_METHOD(CHeadHeatersBase,GetVDDPowerSupply);
  INIT_METHOD(CHeadHeatersBase,GetVCCPowerSupply);
  INIT_METHOD(CHeadHeatersBase,GetHeadTemperature);
  INIT_METHOD(CHeadHeatersBase,WriteToE2PROM);
  INIT_METHOD(CHeadHeatersBase,ReadFromE2PROM);
  INIT_METHOD(CHeadHeatersBase,CheckThermistorsStatus);
  INIT_METHOD(CHeadHeatersBase,GetHeadHeaterParameter);
  INIT_METHOD(CHeadHeatersBase,SaveHeadHeaterParameter);
  INIT_METHOD(CHeadHeatersBase,GetCurrentHeadVoltage);
  INIT_METHOD(CHeadHeatersBase,GetSetVoltageTable);
  INIT_METHOD(CHeadHeatersBase,AreHeadsTemperaturesOk);
  INIT_METHOD(CHeadHeatersBase,IsHeaterTemperatureOk);
  INIT_METHOD(CHeadHeatersBase,SetAndMeasureHeadVoltage);
  INIT_METHOD(CHeadHeatersBase,IsItNewEEProm);

  m_RepPot = m_ParamsMgr->RequestedPotentiometerValue;

  m_OHDBClient = COHDBProtocolClient::Instance();

  //Instance to error handler
  m_ErrorHandlerClient = CErrorHandler::Instance();

  for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
  {
     m_CurrentPotentiometerTable[i]=0;
     m_SetPotentiometerTable[i]=0;
  }
  memset(&m_ReqPotentiometerValues,0,NUMBER_OF_POTENTIOMETERS * sizeof(int));
  m_VppPowerSupply=0;
  m_Heater24VPowerSupply=0;
  m_VDDPowerSupply=0;
  m_VCCPowerSupply=0;
  m_TemperatureOkInOCB = true;
  SetHeaterState(HEAD_HEATER_OFF);
  ResetStatistics();
}

// Destructor
CHeadHeatersBase::~CHeadHeatersBase(void)
{
  // Free each head-heater component
  for(unsigned i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    Q_SAFE_DELETE(m_Heaters[i]);
}


// Set the units to be A/D units
TQErrCode CHeadHeatersBase::SetUnitsAsA2D(bool ADu)
{
  m_A2DUnits = ADu;
  return Q_NO_ERROR;
}

void CHeadHeatersBase::SetHeaterState(bool On)
{

  m_MutexDataBase.WaitFor();
  HeaterIsOn = On;
  m_MutexDataBase.Release();
}

bool CHeadHeatersBase::IsHeaterOn()
{
  bool RetVal;

  m_MutexDataBase.WaitFor();
  RetVal = HeaterIsOn;
  m_MutexDataBase.Release();

  return RetVal;
}

void CHeadHeatersBase::UpdateIfTemperatureOkInOCB(bool Value)
{
  m_MutexDataBase.WaitFor();
  m_TemperatureOkInOCB = Value;
  m_MutexDataBase.Release();
}

bool CHeadHeatersBase::GetIfTemperatureOkInOCB(void)
{
  bool RetVal;

  m_MutexDataBase.WaitFor();
  RetVal = m_TemperatureOkInOCB;
  m_MutexDataBase.Release();

  return RetVal;
}

//Constructor
CHeadHeaters::CHeadHeaters(const QString& Name): CHeadHeatersBase(Name)
{
   m_CancelFlag=false;

   m_FlagSetHeaterOnOffComUnderUse=0;
   m_FlagWaitComUnderUse=0;
   m_FlagSetHeaterTemperatureComUnderUse=0;
   m_FlagGetHeaterStatusComUnderUse=0;
   m_FlagIsTemperatureOkComUnderUse=0;
   m_FlagSetHeaterStandbyComUnderUse=0;
   m_FlagGetHeaterStandbyComUnderUse=0;
   m_FlagSetVoltageComUnderUse=0;
   m_FlagGetVoltageComUnderUse=0;
   m_FlagGetPowerSuppliesComUnderUse=0;

   m_SetHeaterOnOff_AckOk=false;
   m_SetHeaterTemperature_AckOk=false;
   m_IsTemperatureOk_AckOk=false;
   m_SetHeaterStandby_AckOk=false;
   m_GetHeaterStandby_AckOk=false;
   m_SetVoltage_AckOk=false;
   m_GetVoltage_AckOk=false;
   m_GetPowerSupplies_AckOk=false;

   // Install a receive handler for a specific message ID
   m_OHDBClient->InstallMessageHandler(OHDB_HEADS_TEMPERATURE_ERROR,
                                       &NotificationHeadsTemperatureError,
                                       reinterpret_cast<TGenericCockie>(this));

   m_OHDBClient->InstallMessageHandler(OHDB_HEAD_HEATER_ERROR,
                                       &NotificationHeadHeaterErrorResponse,
                                       reinterpret_cast<TGenericCockie>(this));

   m_OHDBClient->InstallMessageHandler(OHDB_HEADS_DATA_WRITE_ERROR,
                                       &SaveHeadDataErrorNotification,
                                       reinterpret_cast<TGenericCockie>(this));


   //m_OHDBClient->InstallMessageHandler(OHDB_HEAD_HEATER_ARE_ON_OFF,
   //                                    &NOT_IMPLEMENTED_YET,
   //                                    reinterpret_cast<TGenericCockie>(this));

   // shahar: line replaced
   // memset(m_HeatersSetTemperatures, 0xFF, sizeof(THeaterElementsTable));
   for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
     m_HeatersOutOfLimits[i] = false;
}    

// Destructor
CHeadHeaters::~CHeadHeaters(void)
{
}

void CHeadHeatersBase::UpdateLastError(int LastError)
{
   m_MutexDataBase.WaitFor();
      m_LastError = LastError;
   m_MutexDataBase.Release();
}
    
int CHeadHeatersBase::GetLastError(void)
{
   int LastError;

   m_MutexDataBase.WaitFor();
      LastError = m_LastError;
   m_MutexDataBase.Release();
      
   return (LastError);
}

//-------------------------------------------------------------------------
void CHeadHeatersBase::UpdateCurrentVoltageTable(TPotentiometerTable PotentiometerTable)
{
   int i;
   m_MutexDataBase.WaitFor();
   for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
      m_CurrentPotentiometerTable[i] = PotentiometerTable[i];
   m_MutexDataBase.Release();
}

int CHeadHeatersBase::GetCurrentVoltageTable(int Head)
{
   int Voltage;

   m_MutexDataBase.WaitFor();
   Voltage = m_CurrentPotentiometerTable[Head];
   m_MutexDataBase.Release();

   return Voltage;
                  
  /* //This is one solution in case we want to read all data
     //before a change occurs in m_CurrentPotentiometerTable.
     // The data is updated only when the first Head(=0) is read.
  static bool FirstTime=true;
  static TPotentiometerTable PotentiometerTable;

  if(Head==0)
     {
     m_MutexDataBase.WaitFor();
     for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
        PotentiometerTable[i] = m_CurrentPotentiometerTable[i];
     m_MutexDataBase.Release();
     }
  else
     {
     if(FirstTime)
        {
        m_MutexDataBase.WaitFor();
        for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
           PotentiometerTable[i] = m_CurrentPotentiometerTable[i];
        m_MutexDataBase.Release();
        }
     }

  return PotentiometerTable[i]; 
  */
}


float CHeadHeatersBase::GetCurrentHeadVoltage(int Head)
{
   int Voltage;

   m_MutexDataBase.WaitFor();
   Voltage = m_CurrentPotentiometerTable[Head];
   m_MutexDataBase.Release();

   return CONVERT_HEAD_VPP_A2D_TO_VOLT(Voltage);
}


void CHeadHeatersBase::UpdateSetVoltageTable(TPotentiometerTable PotentiometerTable)
{
   int i;

   m_MutexDataBase.WaitFor();
   for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
      m_SetPotentiometerTable[i] = PotentiometerTable[i];
   m_MutexDataBase.Release();
}

int CHeadHeatersBase::GetSetVoltageTable(int Head)
{
   int Voltage;
   
   m_MutexDataBase.WaitFor();
   Voltage = m_SetPotentiometerTable[Head];
   m_MutexDataBase.Release();

   return Voltage;
}

void CHeadHeatersBase::UpdatePowerSupply(float Vpp,float V24,float VDD,float VCC)
{
   m_MutexDataBase.WaitFor();
   //  m_VppPowerSupply = CONVERT_VPP_A2D_TO_VOLT(A2D_Vpp);
   //  m_Heater24VPowerSupply = CONVERT_24V_A2D_TO_VOLT(A2D_V24);
   //  m_VDDPowerSupply = CONVERT_VCC_A2D_TO_VOLT(A2D_VDD);
   //  m_VCCPowerSupply = CONVERT_VDD_A2D_TO_VOLT(A2D_VCC);
   m_VppPowerSupply =  Vpp;
   m_Heater24VPowerSupply = V24;
   m_VDDPowerSupply = VDD;
   m_VCCPowerSupply = VCC;
   m_MutexDataBase.Release();
}
    
float CHeadHeatersBase::GetVppPowerSupply(void)
{
   float Vpp;
   
   m_MutexDataBase.WaitFor();
     Vpp = m_VppPowerSupply;
   m_MutexDataBase.Release();
      
   return Vpp;
}
bool CHeadHeatersBase::IsVppPowerSupplyLegal(void)
{
 bool legal = true;
 float value = GetVppPowerSupply();
 if( value < VPP_MIN_VALUE || value > VPP_MAX_VALUE )
	legal = false;
 return legal;
}
float CHeadHeatersBase::Get24VPowerSupply(void)
{
   float V24;
   
   m_MutexDataBase.WaitFor();
    V24 = m_Heater24VPowerSupply;
   m_MutexDataBase.Release();
      
   return V24;
}

float CHeadHeatersBase::GetVDDPowerSupply(void)
{
   float VDD;
   
   m_MutexDataBase.WaitFor();
     VDD = m_VDDPowerSupply;
   m_MutexDataBase.Release();
      
   return VDD;
}
bool CHeadHeatersBase::IsVDDPowerSupplyLegal(void)
{
 bool legal = true;
 float value = GetVDDPowerSupply();
 if( value < VDD_MIN_VALUE || value > VDD_MAX_VALUE )
	legal = false;
 return legal;
}
float CHeadHeatersBase::GetVCCPowerSupply(void)
{
   float VCC;
   
   m_MutexDataBase.WaitFor();
     VCC = m_VCCPowerSupply;
   m_MutexDataBase.Release();
      
   return VCC;
}


TQErrCode CHeadHeatersBase::DebugDisplayHeadStatus(void)
{
   QString Output = "Head Temperatures (c): ";

   for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
   {
      Output += QIntToStr (m_Heaters[i]->GetCurrentTemperatureInCelcius());
      Output += " ";
   }

   CQLog::Write(LOG_TAG_HEAD_HEATERS, Output);

   Output = "Head Temperatures (A/D): ";

   for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
   {
      Output += QIntToStr (m_Heaters[i]->GetCurrentTemperatureInA2D());
      Output += " ";
   }

   CQLog::Write(LOG_TAG_HEAD_HEATERS, Output);

   return Q_NO_ERROR;

}


//-------------------------------------------------------------
TQErrCode CHeadHeatersBase::DisplayHeadStatus(void)
{
   int i;
   CQLog::Write(LOG_TAG_HEAD_HEATERS, "Head Temperatures/Status and Potentiometer of working heads:");
   CQLog::Write(LOG_TAG_HEAD_HEATERS, "Element:       Current:   Set:   Standby");
   for(i=0;i < TOTAL_NUMBER_OF_HEATERS; i++)
      {
      if (!m_Heaters[i]->GetIfHeadWorking())
         continue;

      CQLog::Write(LOG_TAG_HEAD_HEATERS, "%s =   %d      %d    %d     ",
                      GetHeaterStr(i).c_str(),
                      m_Heaters[i]->GetCurrentTemperatureInA2D(),
                      m_Heaters[i]->GetSetTemperatureInA2D(),
                      m_Heaters[i]->GetStandbyTemperatureInA2D());
      }

   TPotentiometerTable CurrentPotentiometerTable;
   TPotentiometerTable SetPotentiometerTable;

   m_MutexDataBase.WaitFor();
      for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
         {
         CurrentPotentiometerTable[i] = m_CurrentPotentiometerTable[i];
         SetPotentiometerTable[i] = m_SetPotentiometerTable[i];
         }
   m_MutexDataBase.Release();

   for(i=0;i < NUMBER_OF_POTENTIOMETERS; i++)
      {
      if (!m_Heaters[i]->GetIfHeadWorking())
         continue;

      CQLog::Write(LOG_TAG_HEAD_HEATERS, "Potentiometer %s=  Set %d   Current %d   ",
                      GetHeaterStr(i).c_str(),
                      SetPotentiometerTable[i],
                      CurrentPotentiometerTable[i]);
      }

   CQLog::Write(LOG_TAG_HEAD_HEATERS, "Power Supply");
   CQLog::Write(LOG_TAG_HEAD_HEATERS, " Vpp= %.2f",GetVppPowerSupply());
   CQLog::Write(LOG_TAG_HEAD_HEATERS, " 24V= %.2f",Get24VPowerSupply());
   CQLog::Write(LOG_TAG_HEAD_HEATERS, " VDD= %.2f",GetVDDPowerSupply());
   CQLog::Write(LOG_TAG_HEAD_HEATERS, " VCC= %.2f",GetVCCPowerSupply());

   return Q_NO_ERROR;
}

TQErrCode CHeadHeatersBase::DisplayVoltageTable(void)
{
   int i;

   TPotentiometerTable CurrentPotentiometerTable;

   m_MutexDataBase.WaitFor();
      for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
         {
         CurrentPotentiometerTable[i] = m_CurrentPotentiometerTable[i];
         }
   m_MutexDataBase.Release();

   if (!m_Heaters[i]->GetIfHeadWorking())
      {
      QMonitor.Print("Head did not working");
      return Q_NO_ERROR;
      }

   CQLog::Write(LOG_TAG_HEAD_HEATERS,"%d %d %d %d -- %d %d %d %d",
                   CurrentPotentiometerTable[0],CurrentPotentiometerTable[1],
                   CurrentPotentiometerTable[2],CurrentPotentiometerTable[3],
                   CurrentPotentiometerTable[4],CurrentPotentiometerTable[5],
                   CurrentPotentiometerTable[6],CurrentPotentiometerTable[7]);

   return Q_NO_ERROR;
}


USHORT CHeadHeatersBase::ConvertMaskTableToWord(THeatersMaskTable HeatersMaskTable)
{
   WORD j,HeaterMask = 0;
   int i;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      {
      j= (WORD)(1 << i);
      if(HeatersMaskTable[i])
         HeaterMask += j;
      }
   return HeaterMask;
}


void CHeadHeatersBase::GetHeadParameters(int HeadNumber, THeadParameters* HeadParameters)
{
  m_Heaters[HeadNumber]->GetHeadParameters(HeadParameters);
}


void CHeadHeatersBase::SetHeadParameters(int HeadNumber, THeadParameters* HeadParameters)
{
  m_Heaters[HeadNumber]->SetHeadParameters(HeadParameters);
}


//Get and Save parameters in App Params
int CHeadHeatersBase::GetHeadHeaterParameter(int HeadNumber)
{
  if(HeadNumber>=TOTAL_NUMBER_OF_HEADS_HEATERS)
     {
     CQLog::Write(LOG_TAG_HEAD_HEATERS," \"GetHeadHeaterParameter\" wrong value received:%d",HeadNumber);
     return 0;
     }
  return m_Heaters[HeadNumber]->GetParameter();
}

TQErrCode CHeadHeatersBase::SaveHeadHeaterParameter(int HeadNumber, int Value)
{
  if(!VALIDATE_HEATER_INCLUDING_PREHEATER(HeadNumber))
  {
     CQLog::Write(LOG_TAG_HEAD_HEATERS," \"SaveHeadHeaterParameter\" wrong value received:%d",HeadNumber);
     return Q_NO_ERROR;
  }
  m_Heaters[HeadNumber]->SaveParameter(Value);
  return Q_NO_ERROR;
}


//-------------------------------------------
// Command procedures
//--------------------------------------------

TQErrCode CHeadHeatersBase::SetDefaultOnOff(bool OnOff)
{
   int i;
   WORD j,HeatersMask = 0;
   
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      {
      j= (WORD)(1 << i);
      if(m_ParamsMgr->HeatersMaskTable[i])
         HeatersMask += j;
      }

   return (SetHeaterOnOff(OnOff,HeatersMask));
}

// Command to turn on/off the Head heater
TQErrCode CHeadHeaters::SetHeaterOnOff(bool OnOff,WORD HeatersMask)
{
   WORD j;

   // Verify if we are already not performing this command
   if (m_FlagSetHeaterOnOffComUnderUse)
       throw EHeadHeater(Q2RT_HEADHEATER_SEND_2MSGS_ERROR);

   // Reset the internal Database, to force realtime update.
   ResetHeatersStatusDatabase();

   m_FlagSetHeaterOnOffComUnderUse=true;

   m_CancelFlagMutex.WaitFor();
      m_CancelFlag = false;
   m_CancelFlagMutex.Release();

   // Build the Head Heater turn on message
   TOHDBSetHeadHeaterOnOffMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OHDB_SET_HEAD_HEATER_ON_OFF);
   SendMsg.OnOff = static_cast<BYTE>(OnOff);


   // shahar: convert the WORD to 250 HW addresses:
   TOnOffTable ConversionTable;  // lenth of WORD
   WORD HeatersMaskPhysical = HeatersMask;

   for (int i=0; i < 16 /*num of bits in WORD*/; i++)
     ConversionTable[i] = (int) ( ( HeatersMaskPhysical & ((WORD)1 << i) ) != 0 );


   HeatersMaskPhysical = 0;

   // construct the converted HeatersMask:
   for (int i=0; i < 16 /*num of bits in WORD*/; i++)
     HeatersMaskPhysical += (ConversionTable.Lookup(i) << i);

   SendMsg.HeatersMask = HeatersMaskPhysical;

   m_SetHeaterOnOff_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&SendMsg,
                                  sizeof(TOHDBSetHeadHeaterOnOffMessage),
                                  SetHeaterOnOffAckResponse,
                                  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagSetHeaterOnOffComUnderUse=false;
      throw EHeadHeater(Q2RT_HEADHEATER_DIDNT_GET_ACK_ERROR);
      }

   if(!m_SetHeaterOnOff_AckOk)
      {
      m_FlagSetHeaterOnOffComUnderUse=false;
      throw EHeadHeater(Q2RT_HEADHEATER_INVALID_REPLY_ERROR);
      }

   bool HeadWorking;
   for(int i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      {
      j= (WORD)(1 << i);
      HeadWorking = (HeatersMask & j);
      m_Heaters[i]->UpdateIfWorkingHead(HeadWorking);
      }

   SetHeaterState(OnOff);
   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heaters %s",OnOff ? "On" : "Off");
   m_FlagSetHeaterOnOffComUnderUse=false;
   return Q_NO_ERROR;
}

//This procedure is the callback for SetHeaterOnOff
void CHeadHeaters::SetHeaterOnOffAckResponse (int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("Set HeatersOnOff length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Set HeatersOnOff length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("SetHeaterOnOffAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterOnOffAckResponse MSGID(0x%X) error message ",
                                        static_cast<int>(ResponseMsg->MessageID));
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OHDB_SET_HEAD_HEATER_ON_OFF)
      {
      FrontEndInterface->NotificationMessage("SetHeaterOnOffAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OnOffAckResponse:Responded Message ID (0x%X) Error",
                                        static_cast<int>(ResponseMsg->RespondedMessageID));
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("SetHeaterOnOffAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OnOffAckResponse Ack status Error (%d)",
                                        static_cast<int>(ResponseMsg->AckStatus));
      //InstancePtr->m_ErrorHandlerClient->ReportError("OnOffAckResponse Ack status Error",
      //                                  0,static_cast<int>(ResponseMsg->AckStatus));
      return;
      }

   InstancePtr->m_SetHeaterOnOff_AckOk=true;
}

void CHeadHeatersBase::SetHeadMinVoltage()
{
         int   CurrentVoltageA2D = 0;

         TPotentiometerTable i_PotentiometerValues;

         //int i_PotentiometerValues[TOTAL_NUMBER_OF_HEADS_HEATERS];
        // reset the potentiometer values to read the min heads voltages
        for(BYTE i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
        {
                i_PotentiometerValues[i] = 0;
        }

        SetPrintingHeadsVoltage(i_PotentiometerValues);

        // Wait for the stabilization of the voltages
        QSleep(HEAD_VOLTAGES_STABILIZATION_TIME*2.5);

        // Get the current voltage
        GetPrintingHeadsVoltages();

        // set the min heads voltages to the parameter manager - HSW_MinVoltageArray
        for(BYTE i = 0; i < TOTAL_NUMBER_OF_HEADS_HEATERS; i++)
        {
             CurrentVoltageA2D = GetCurrentVoltageTable(i);
             m_ParamsMgr->HSW_MinVoltageArray[i] = CONVERT_HEAD_VPP_A2D_TO_VOLT(CurrentVoltageA2D);

             CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM info: The minimal value for head number %d is %.2f Volt.",i,m_ParamsMgr->HSW_MinVoltageArray[i].Value());
        }
        m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->HSW_MinVoltageArray);
}


//The method distinguish between new EEprom to old EEprom.
// the resistors between new EEprom to old EEprom - are different!!
bool CHeadHeatersBase::IsItNewEEProm(const int headNum)
{

      THeadParameters HeadParameters[TOTAL_NUMBER_OF_HEADS_HEATERS];
      try
      {
       CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Head number %d, Trying to read head production Year", headNum);
        ReadHeadParametersFromE2PROM(headNum, &HeadParameters[headNum]);

        //ConvertStringToDate
      }
      catch(EQException& err)
      {
          throw EHeadHeater( "EEPROM Info: Recognizing head operation was failed because of a communication problem while trying to receive data from the heads.");
      }
       int iheadProductionDate =  (int)(HeadParameters[headNum].ProductionDate);

       double headProductionDate = (double)(iheadProductionDate);
      if(m_ParamsMgr->Production_Year_For_New_Heads == 0) //first time
      {
		   // CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Production_Year_For_New_Heads parameter is equeal to zero, EEprom number is %d is old",headNum);
           //This parameter wasn't exist in old machines.
           return false; //it is old eeprom
      }
      /*Getting from parameter manager the production year for new EEprom*/
      int newHeadProductionYear = m_ParamsMgr->Production_Year_For_New_Heads;
	  //CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Production_Year_For_New_Heads parameter is %d", newHeadProductionYear);
      /*Getting the production year from the EEPROM*/
      TDateTime ProductionHeadDate = headProductionDate;
      unsigned short usYear, Month, Day;
      ProductionHeadDate.DecodeDate(&usYear, &Month, &Day);
      int iCurrentHeadProductionYear = (int)usYear;
	  //CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Head number %d was created in %d year", headNum, iCurrentHeadProductionYear);
      if(iCurrentHeadProductionYear <  newHeadProductionYear)
      {
		  //CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Head number %d, has old resistors because it was produced in ( %d) year. The head is older than %d", headNum, iCurrentHeadProductionYear, newHeadProductionYear);
          return false;    // old eeprom
      }
      //CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Head number %d, has new resistors because it was produced in ( %d) year. The head is equal or newer than %d", headNum, iCurrentHeadProductionYear, newHeadProductionYear);
      return true; //new eeprom
}
void  CHeadHeatersBase::FindResistorValues(int HeadNum, float &ResistorHigh , float& ResistorLow)
{
       int index =0; // index for old eeprom
       //need to determinate the relevant resistors for the current eeprom (old eeprom or new eeprom)
       if(IsItNewEEProm(HeadNum))
       {
             index = 1;    //index for new eeprom
       }
       ResistorHigh = m_ParamsMgr->ResistorHighValue[index];
       ResistorLow  = m_ParamsMgr->ResistorLowValue[index];
       CQLog::Write(LOG_TAG_HEAD_HEATERS, "EEPROM Info: Head number %d, use high resistor value %f KOHM, and low resistor value %f KOHM", HeadNum, ResistorHigh, ResistorLow);
}

int CHeadHeatersBase::CalculatePotValue(int HeadNum,float ReqVoltage)
{
	float Rh, Rl; // Resistor High
        FindResistorValues(HeadNum, Rh , Rl) ;


	const float RV   = ReqVoltage;

	int potVal = ((((RV - Vref) / Vref) * (Rl + m_RepPot) - Rh) / ((m_RepPot * RV) / Vref)) * 255.0;

	if (potVal < 0)
	{
		potVal = 0;
	}

	return potVal;
}
//---------------------------------------------------------------------------

float CHeadHeatersBase::EstimateHeadVoltage(int HeadNum, int PotVal)
{
       	float Rh, Rl; // Resistor High
        FindResistorValues(HeadNum,  Rh , Rl) ;

	const float Rph  = m_RepPot * (PotVal / 255.0);
	const float Rpl  = m_RepPot - Rph;

	float estVolt = (Vref / (Rl + Rpl)) * (Rh + Rph) + Vref;

	return estVolt;
}

TQErrCode CHeadHeatersBase::CheckThermistorsIntegrity(int Head,int A2DValue)
{
   TQErrCode Err = Q_NO_ERROR;
   if(A2DValue < THERMISTOR_SHORT_VALUE)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads Heater %s, Thermistor short (%d)",
                                         GetHeaterStr(Head).c_str(),
                                         A2DValue);
      Err = Q2RT_HEAD_HEATER_THERMISTOR_SHORT;
      }

   if(A2DValue > THERMISTOR_OPEN_VALUE)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads Heater %s, Thermistor open (%d)",
                                         GetHeaterStr(Head).c_str(),
                                         A2DValue);
      Err=Q2RT_HEAD_HEATER_THERMISTOR_OPEN;
      }

   return Err;
}


TQErrCode CHeadHeatersBase::CheckThermistorsStatus(bool DuringPrint)
{
   int i;
   TQErrCode Err = Q_NO_ERROR;
   bool AllTemperaturesOk=true;

   //Check if one of the heaters is not working
   for(i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
      {
      if (!m_Heaters[i]->GetIfHeadWorking())
         continue;

      if (Err == Q_NO_ERROR)
        Err = CheckThermistorsIntegrity(i,m_Heaters[i]->GetCurrentTemperatureInA2D());

      //Statistics
      if(DuringPrint)
         if(m_Heaters[i]->GetIfTemperatureOk() != HEAD_TEMPERATURE_OK)
            AllTemperaturesOk=false;
      }

      m_TemperatureOutOfMargineTotalCount++;
      if (!AllTemperaturesOk)
         {
         //Mark Statistics
         m_TemperatureOutOfMargine++;
         }


   return Err;
}

// Command to wait to head heater to be on off
TQErrCode CHeadHeaters::WaitForHeadsTemperatureOK(bool ContinueJob)
{
   TQErrCode Err;
   int Timeout;

   // Verify if we are already not performing this command
   if (m_FlagWaitComUnderUse)
       throw EHeadHeater(Q2RT_HEADHEATER_SEND_2MSGS_ERROR);

   m_FlagWaitComUnderUse=true;

   if((Err=CheckThermistorsStatus(false)) != Q_NO_ERROR)
      {
      m_FlagWaitComUnderUse=false;
      return Err;
      }

   //Verify if the current temperature is OK in this case don't wait
   if (AreHeadsTemperaturesOk())
      {
      m_FlagWaitComUnderUse=false;
      return Q_NO_ERROR;
      }

   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Waiting for heads heated");

   //For debbugging
   for(int i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      {
      if(m_Heaters[i]->GetIfHeadWorking())
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadTemperature %s: Set(%d A/D = %d�C), Current(%d A/D = %d�C)",
            GetHeaterStr(i).c_str(),
            m_Heaters[i]->GetSetTemperatureInA2D(),
            m_Heaters[i]->GetSetTemperatureInCelcius(),
            m_Heaters[i]->GetCurrentTemperatureInA2D(),
            m_Heaters[i]->GetCurrentTemperatureInCelcius());
      }

   //Calculate timeout time
   if((Q2RTApplication->GetMachineManager()->GetCurrentState() == msPrinting) && (ContinueJob == false))
     // Printing and not after pause - short timeout
     Timeout = QSecondsToTicks(m_ParamsMgr->HeadsPrintingTimeoutSec);
   else
     // Start Printing - long timeout
     Timeout = QSecondsToTicks(m_ParamsMgr->HeadsHeatingTimeoutSec);

   //The temperature are not in range we will wait untill it is in range
   m_Waiting=true;

   // Wait for reply
   QLib::TQWaitResult WaitResult = m_SyncEventWaitForTemperature.WaitFor(Timeout);

   m_Waiting=false;

   if(m_CancelFlag)
      {
      m_CancelFlagMutex.WaitFor();
         m_CancelFlag = false;
      m_CancelFlagMutex.Release();

      //For debbugging
      for(int i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
         {
         if(m_Heaters[i]->GetIfHeadWorking())
            CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadTemperature %s: Set(%d A/D = %d�C), Current(%d A/D = %d�C)",
                      GetHeaterStr(i).c_str(),
                      m_Heaters[i]->GetSetTemperatureInA2D(),
                      m_Heaters[i]->GetSetTemperatureInCelcius(),
                      m_Heaters[i]->GetCurrentTemperatureInA2D(),
                      m_Heaters[i]->GetCurrentTemperatureInCelcius());
         }

      if((Err= CheckThermistorsStatus(false)) != Q_NO_ERROR)
         {
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads temperature out of range");
         m_FlagWaitComUnderUse=false;
         return Err;
         }

      m_FlagWaitComUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Wait for Head Temperature Ok:Canceled/Stopped:");
      return Q2RT_HEAD_HEATER_STOPPED_OR_CANCELED;
      }

   if(WaitResult != QLib::wrSignaled)
      {
      if(WaitResult == QLib::wrTimeout)
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heaters Timeout");

      if (AreHeadsTemperaturesOk())
         {
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads are warm now");
         FrontEndInterface->NotificationMessage("Heads are warm now - but one error occured ");
         m_FlagWaitComUnderUse=false;
         return Q_NO_ERROR;
         }

      //For debbugging
      for(int i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
         {
         if(m_Heaters[i]->GetIfHeadWorking())
            CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadTemperature %s: Set(%d A/D = %d�C), Current(%d A/D = %d�C)",
                      GetHeaterStr(i).c_str(),
                      m_Heaters[i]->GetSetTemperatureInA2D(),
                      m_Heaters[i]->GetSetTemperatureInCelcius(),
                      m_Heaters[i]->GetCurrentTemperatureInA2D(),
                      m_Heaters[i]->GetCurrentTemperatureInCelcius());
         }

      FrontEndInterface->NotificationMessage("Head Heater does not match required temperature");
      m_FlagWaitComUnderUse=false;
      return Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT;
      //throw EHeadHeater("Head Heater does not match required temperature");
      }

   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads are warm");
   m_FlagWaitComUnderUse=false;
   return Q_NO_ERROR;
}

// Command to set default heater temperature
TQErrCode CHeadHeatersBase::SetDefaultHeateresTemperature(void)
{
   THeaterElementsTable HeaterElementsTable;
   int i;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
   {
      HeaterElementsTable[i]= m_Heaters[i]->GetParameter();
   }

   return (SetHeatersTemperature(HeaterElementsTable));
}

// Command to set temperature of 25 celcius standby2 temperature
TQErrCode CHeadHeatersBase::SetStandby2HeateresTemperature(void)
{
   THeaterElementsTable HeaterElementsTable;
   int i;

   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Set temperature to standby 2");

   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      HeaterElementsTable[i]= m_Heaters[i]->GetStandbyParameter();

   return (SetHeatersTemperature(HeaterElementsTable));
}


TQErrCode CHeadHeatersBase::SetHeaterTemperature(int HeaterNum, int Temperature)
{
  m_HeatersSetTemperatures[HeaterNum] = Temperature;
  return Q_NO_ERROR;
}

TQErrCode CHeadHeaters::SetHeatersTemperature()
{
  SetHeatersTemperature(m_HeatersSetTemperatures);
  return Q_NO_ERROR;
}

TQErrCode CHeadHeaters::SetHeatersTemperature(THeaterElementsTable HeaterSetTable)
{
   // Verify if we are already not performing this command
   if (m_FlagSetHeaterTemperatureComUnderUse)
       throw EHeadHeater("HeadHeater:SetHeaterTemperature two message send in the same time");

   m_FlagSetHeaterTemperatureComUnderUse=true;
   
   // Build the Head Heater set message message
   TOHDBSetHeatersTemperatureMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OHDB_SET_HEATERS_TEMPERATURE);    

   int i;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS ;i++)
      SendMsg.SetElements.HeaterTable[i] = static_cast<USHORT>(HeaterSetTable.Lookup(i));

   SendMsg.LowThreshold = m_ParamsMgr->HeadsTemperatureLowThershold;
   SendMsg.HighThreshold = m_ParamsMgr->HeadsTemperatureHighThershold;
   m_SetHeaterTemperature_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&SendMsg,
                                  sizeof(TOHDBSetHeatersTemperatureMessage),
                                  SetHeaterTemperatureAckResponse,
								  reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagSetHeaterTemperatureComUnderUse=false;
      throw EHeadHeater(Q2RT_HEADHEATER_DIDNT_GET_ACK_ERROR);
      }

   if(!m_SetHeaterTemperature_AckOk)
      {
      m_FlagSetHeaterTemperatureComUnderUse=false;
      throw EHeadHeater(Q2RT_HEADHEATER_INVALID_REPLY_ERROR);
      }

   for(i=0; i<TOTAL_NUMBER_OF_HEATERS;i++)
      m_Heaters[i]->UpdateSetTemperatureA2D(HeaterSetTable[i]);

   m_FlagSetHeaterTemperatureComUnderUse=false;
   return (Q_NO_ERROR);
}

//This procedure is the callback for SetHeaterTemperature parameters
void CHeadHeaters::SetHeaterTemperatureAckResponse (int TransactionId,
                                                    PVOID Data,
                                                    unsigned DataLength,
                                                    TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   // Head Heater response message
   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("Set SetHeaterTemperatureAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Set SetHeaterTemperatureAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("SetHeaterTemperatureAckResponse MSGID error message ");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterTemperatureAckResponse MSGID error message (0x%X)",
                                        static_cast<int>(ResponseMsg->MessageID));
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OHDB_SET_HEATERS_TEMPERATURE)
      {
      FrontEndInterface->NotificationMessage("SetHeaterTemperatureAckResponse MSGID error message ");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterTemperatureAckResponse:Responded Message ID Error (0x%X)",
                                        static_cast<int>(ResponseMsg->RespondedMessageID));
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("Set SetHeaterTemperatureAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterTemperatureAckResponse Ack status Error (%d)",
                                        static_cast<int>(ResponseMsg->AckStatus));
      return;
      }

   InstancePtr->m_SetHeaterTemperature_AckOk=true;
}

// Command to get heaters status
TQErrCode CHeadHeaters::GetHeatersStatus(void)
{
   // Verify if we are already not performing this command
   if (m_FlagGetHeaterStatusComUnderUse)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadHeaters \"GetHeatersStatus\" re-entry problem");
      return Q_NO_ERROR;
      }

   m_FlagGetHeaterStatusComUnderUse=true;

   TOHDBGetHeatersStatusMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_GET_HEATERS_STATUS);

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetHeatersStatusMessage),
                            GetHeaterStatusAckResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"GetHeatersStatus\" message");
      } 

   m_FlagGetHeaterStatusComUnderUse=false;

   //Verify status of temperature
   int i,CurrentValue;
   TQErrCode Err;
   int HighThreshold = CAppParams::Instance()->HeadsTemperatureHighThershold;

   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
   {
      CurrentValue = m_Heaters[i]->GetCurrentTemperatureInA2D();

      // Even if heater is "masked out", we still update the display with its temp. reading.
      FrontEndInterface->UpdateStatus(FE_HEAD_HEATER_A2D_BASE+i,static_cast<int>(CurrentValue));
      FrontEndInterface->UpdateStatus(FE_HEAD_HEATER_CELSIUS_BASE+i,
               static_cast<int>(m_Heaters[i]->GetCurrentTemperatureInCelcius()));

      if(!m_Heaters[i]->GetIfHeadWorking())
            continue;

      Err = CheckThermistorsIntegrity(i,CurrentValue);
      if(Err != Q_NO_ERROR)
      {
         if(IsHeaterOn() && !m_HeatersOutOfLimits[i])
         {
           QString OpenShort = "";

           if(Err == Q2RT_HEAD_HEATER_THERMISTOR_SHORT)
             OpenShort = "short";

           if(Err == Q2RT_HEAD_HEATER_THERMISTOR_OPEN)
             OpenShort = "open";

            m_ErrorHandlerClient->ReportError(GetHeaterStr(i) + " thermistor is " + OpenShort,
                                              Err,
                                              CurrentValue);
            m_HeatersOutOfLimits[i] = true;
         }
      }
      else
      {
        m_HeatersOutOfLimits[i] = false;
      }

      if (m_Heaters[i]->GetIfTemperatureOk() == HEAD_HOT)
      {
        if(CurrentValue < (m_Heaters[i]->GetParameter() - HighThreshold))
          FrontEndInterface->UpdateStatus(FE_HEAD_HEATERS_STATUS_BASE+i, HEAD_HOT);
        else
          FrontEndInterface->UpdateStatus(FE_HEAD_HEATERS_STATUS_BASE+i, HEAD_TEMPERATURE_OK);
      }
      else
        FrontEndInterface->UpdateStatus(FE_HEAD_HEATERS_STATUS_BASE+i,
                                      static_cast<int>(m_Heaters[i]->GetIfTemperatureOk()));
   }

   if (m_Waiting)
      {
      m_MutexDataBase.WaitFor();
      if(m_Waiting && AreHeadsTemperaturesOk())
         {
         m_Waiting=false;
         m_SyncEventWaitForTemperature.SetEvent();
         }
      m_MutexDataBase.Release();
      }

   return Q_NO_ERROR;  
}

//Get Heater status ack response
void CHeadHeaters::GetHeaterStatusAckResponse(int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   // Head Heater status message
   TOHDBHeadsTemperatureStatusResponse *StatusMsg =
      static_cast<TOHDBHeadsTemperatureStatusResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBHeadsTemperatureStatusResponse))
      {
      FrontEndInterface->NotificationMessage("GetHeaterStatusAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetHeaterStatusAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OHDB_HEADS_TEMPERATURE_STATUS)
      {
      InstancePtr->m_ErrorHandlerClient->ReportError("GetHeaterStatus ack Error message id",
                                        0,static_cast<int>(StatusMsg->MessageID));
      return;
      }

   int i;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
   {
     InstancePtr->m_Heaters.Lookup(i)->UpdateCurrentTemperatureA2D(static_cast<int>(
                                   StatusMsg->CurrentElements.HeaterTable[i]));
   }
}

// Command to get pre heater status
TQErrCode CHeadHeaters::GetPreHeaterStatus(void)
{
  // Read current temperatures (all heads + preheater) from OHDB:
  GetHeatersStatus();
#ifndef OBJET_MACHINE
  if(!m_Heaters[PRE_HEATER]->GetIfHeadWorking())
  {
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"PreHeater isn't working");
  }
  else
  {
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"%s Temperature: Set(%d A/D = %d�C), Current(%d A/D = %d�C)",
        GetHeaterStr(PRE_HEATER).c_str(),
        m_Heaters.Lookup(PRE_HEATER)->GetSetTemperatureInA2D(),
        m_Heaters.Lookup(PRE_HEATER)->GetSetTemperatureInCelcius(),
        m_Heaters.Lookup(PRE_HEATER)->GetCurrentTemperatureInA2D(),
        m_Heaters.Lookup(PRE_HEATER)->GetCurrentTemperatureInCelcius());
  }
#endif  
  return Q_NO_ERROR;
}


// Command to get if temperature are in range
TQErrCode CHeadHeaters::IsHeadTemperatureOk(void)
{
   // Verify if we are already not performing this command
   if (m_FlagIsTemperatureOkComUnderUse)
      throw EHeadHeater("HeadHeater:IsHeadTempeartureOk:two message send in the same time");
      
   m_FlagIsTemperatureOkComUnderUse=true;

   TOHDBIsHeadsTempeartureOKMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_IS_HEADS_TEMPERATURE_OK);

   m_IsTemperatureOk_AckOk = false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBIsHeadsTempeartureOKMessage),
                            IsTemperatureOkAckResponse,
                            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"IsHeadTempeartureOk\" message");
      m_FlagIsTemperatureOkComUnderUse=false;
      throw EHeadHeater("OHDB didn't get ack for IsHeadTempeartureOk message");
      }

   if(!m_IsTemperatureOk_AckOk)
      {
      m_FlagIsTemperatureOkComUnderUse=false;
      throw EHeadHeater("Head Heater: IsHeadTemperatureOk: Invalid reply from OHDB");
      }

   m_FlagIsTemperatureOkComUnderUse=false;
   return Q_NO_ERROR;
}

//IsTempeartureOk ack response
void CHeadHeaters::IsTemperatureOkAckResponse(int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBHeadsTemperatureOkResponse *StatusMsg =
      static_cast<TOHDBHeadsTemperatureOkResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBHeadsTemperatureOkResponse))
      {
      FrontEndInterface->NotificationMessage("IsTempeartureOkAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"IsTempeartureOkAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OHDB_HEADS_TEMPERATURE_OK)
      {
      FrontEndInterface->NotificationMessage("IsTempeartureOkAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"TemperatureOk Ack Error message id (%d)",
                                        static_cast<int>(StatusMsg->MessageID));
      return;
      }

   InstancePtr->m_IsTemperatureOk_AckOk=true;

   bool TemperatureOk = static_cast<bool>(StatusMsg->TemperatureOk);

   InstancePtr->UpdateIfTemperatureOkInOCB(TemperatureOk);

   if (TemperatureOk == TEMPERATURE_OK)
   {
     CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB Heads and Block Temperature are OK");
   }
   else
   {
     // todo -oShahar.Behagen@objet.com -cNone: TOHDBHeadsTemperatureOkResponse has a head number paramter
     // in case temperatures are not OK. see if this has to be translated.
     CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB Heads and Block Temperature are NOT OK,");
     CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head %d, Temperature = %d, temperature is %s",
                                       (StatusMsg->HeadNum),
                                       (StatusMsg->Temperature),
                                       (StatusMsg->ErrDescription == 2)? "too low.":"too high");
   }
}


TQErrCode CHeadHeatersBase::VerifyIfTemperatureOk(void)
{
   const int RETRY_TEMPERATURE_IS_OK = 3;
   TQErrCode Err;

   int i = 0;
   Err = Q2RT_TEMPERATURE_IS_NOT_OK_IN_OCB;
   while(Err == Q2RT_TEMPERATURE_IS_NOT_OK_IN_OCB && i++< RETRY_TEMPERATURE_IS_OK)
      {
      // Verify the heads temperatures are ok.
      if((Err = WaitForHeadsTemperatureOK(false)) != Q_NO_ERROR)
         return Err;

      //Verify if temperature Ok
      if((Err = IsHeadTemperatureOk()) != Q_NO_ERROR)
         return Err;

      if(GetIfTemperatureOkInOCB())
         return Q_NO_ERROR;
      else
         Err = Q2RT_TEMPERATURE_IS_NOT_OK_IN_OCB;

      QSleepSec(2);
      }

   return Err;
}


// Command to set heater standby temperature 
TQErrCode CHeadHeatersBase::SetDefaultStandbyTemperature(void)
{
   THeaterElementsTable HeaterElementsTable;
   int i;

   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      HeaterElementsTable[i]= m_Heaters[i]->GetStandbyParameter();
   
   return (SetStandbyTemperature(HeaterElementsTable));
}

TQErrCode CHeadHeaters::SetStandbyTemperature(THeaterElementsTable HeaterTable)
{
   // Verify if we are already not performing this command
   if (m_FlagSetHeaterStandbyComUnderUse)
       throw EHeadHeater("HeadHeater:SetStandbyTemperature two message send in the same time");

   m_FlagSetHeaterStandbyComUnderUse=true;
   
   // Build the Head Heater set standy message
   TOHDBSetHeatersStandbyTemperatureMessage SendMsg;

   SendMsg.MessageID = static_cast<BYTE>(OHDB_SET_HEATERS_STANDBY_TEMPERATURE);    

   int i;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      SendMsg.StandbyElements.HeaterTable[i] = static_cast<USHORT>(HeaterTable.Lookup(i));

   m_SetHeaterStandby_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&SendMsg,
                            sizeof(TOHDBSetHeatersStandbyTemperatureMessage),
                            SetHeaterStandbyAckResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"SetStandbyTemperature\" message");
      m_FlagSetHeaterStandbyComUnderUse=false;
      throw EHeadHeater("OHDB didn't get ack for SetStandbyTemperature");
      }

   if(!m_SetHeaterStandby_AckOk)
      {
      m_FlagSetHeaterStandbyComUnderUse=false;
      throw EHeadHeater("Head Heater: SetStandbyTemperature: Invalid reply from OHDB");
      }

   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      m_Heaters[i]->UpdateStandbyTemperatureA2D(HeaterTable[i]);

   m_FlagSetHeaterStandbyComUnderUse=false;
   return (Q_NO_ERROR);
}

//SetHeaterStandby ack response
void CHeadHeaters::SetHeaterStandbyAckResponse(int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("SetHeaterStandbyAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterStandbyAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("SetHeaterStandbyAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterStandbyAckResponse MSGID error message (0x%X)",
                                        static_cast<int>(ResponseMsg->MessageID));
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OHDB_SET_HEATERS_STANDBY_TEMPERATURE)
      {
      FrontEndInterface->NotificationMessage("SetHeaterStandbyAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterStandbyAckResponse:Responded Message ID Error (0x%X)",
                                        static_cast<int>(ResponseMsg->RespondedMessageID));
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("SetHeaterStandbyAckResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetHeaterStandbyAckResponse Ack status Error (%d)",
                                        static_cast<int>(ResponseMsg->AckStatus));
      return;
      }

   InstancePtr->m_SetHeaterStandby_AckOk=true;
}

// Command to get standby heaters tempearture values 
TQErrCode CHeadHeaters::GetStandbyTemperatures(void)
{
   // Verify if we are already not performing this command
   if (m_FlagGetHeaterStandbyComUnderUse)
       throw EHeadHeater("HeadHeater:GetStandbytemperature:two message send in the same time");

   m_FlagGetHeaterStandbyComUnderUse=true;

   TOHDBGetHeatersStandbyTemperatureMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_GET_HEATERS_STANDBY_TEMPERATURE);

   m_GetHeaterStandby_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetHeatersStandbyTemperatureMessage),
                            GetHeaterStandbyAckResponse,
                            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"GetStandbyTemperature\" message");
      m_FlagGetHeaterStandbyComUnderUse=false;
      throw EHeadHeater("OHDB didn't get ack for GetStandbyTemperature message");
      }

   if(!m_GetHeaterStandby_AckOk)
      {
      m_FlagGetHeaterStandbyComUnderUse=false;
      throw EHeadHeater("Head Heater: GetStandbyTemperatures: Invalid reply from OHDB");
      }


   m_FlagGetHeaterStandbyComUnderUse=false;
   return (Q_NO_ERROR);
}

//GetHeaterStandby ack response
void CHeadHeaters::GetHeaterStandbyAckResponse(int TransactionId,
                                              PVOID Data,
                                              unsigned DataLength,
                                              TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBHeadsStandbyTemperatureResponse *StatusMsg =
      static_cast<TOHDBHeadsStandbyTemperatureResponse *>(Data);

      //Verify size of message
   if(DataLength != sizeof(TOHDBHeadsStandbyTemperatureResponse))
      {
      FrontEndInterface->NotificationMessage("GetHeaterStandbyAckResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetHeaterStandbyAckResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(StatusMsg->MessageID) != OHDB_HEADS_STANDBY_TEMPERATURE)
      {
      FrontEndInterface->NotificationMessage("GetHeaterStandby Error message id ");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetHeaterStandby Error message id (0x%X)",
                                        static_cast<int>(StatusMsg->MessageID));
      return;
      }
   
   int i;
   int Value;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      {
      Value = static_cast<int>(StatusMsg->StandbyElements.HeaterTable[i]);
      InstancePtr->m_Heaters.Lookup(i)->UpdateStandbyTemperatureA2D(Value);
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heater Standby temperature A2D: [%d]=%d",
                                        i,Value);
      }

   InstancePtr->m_GetHeaterStandby_AckOk=true;
}             


//Potentiometer
TQErrCode CHeadHeatersBase::SetDefaultPrintingHeadsVoltages(void)
{
  int i;
  TPotentiometerTable PotentiometerTable;

   for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
     PotentiometerTable[i] = m_ParamsMgr->PotentiometerValues[i];

   return SetPrintingHeadsVoltage(PotentiometerTable);
}

TQErrCode CHeadHeaters::SetPrintingHeadsVoltage()
{
  return SetPrintingHeadsVoltage(m_ReqPotentiometerValues);
}

TQErrCode CHeadHeaters::SetPrintingHeadVoltage(int HeadNum, int PotentiometerValue)
{
  m_ReqPotentiometerValues[HeadNum] = PotentiometerValue;
  return Q_NO_ERROR;
}

TQErrCode CHeadHeaters::SetPrintingHeadsVoltage(TPotentiometerTable PotentiometerTable)
{
   //Verify if we are already not performing this command
   if (m_FlagSetVoltageComUnderUse)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadHeaters \"GetAmbientTemperatureStatus\" re-entry problem");
      return Q_NO_ERROR;
      }

   m_FlagSetVoltageComUnderUse=true;

   TOHDBSetPrintingHeadsVoltagesMessage SetMsg;

   SetMsg.MessageID = static_cast<BYTE>(OHDB_SET_PRINTING_HEADS_VOLTAGES);
   int i;
   for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
      SetMsg.PotentiometerValues[i] = static_cast<USHORT>(PotentiometerTable.Lookup(i));

   m_SetVoltage_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&SetMsg,
                            sizeof(TOHDBSetPrintingHeadsVoltagesMessage),
                            SetPrintingHeadsVoltageResponse,
                            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {

      m_FlagSetVoltageComUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"SetPrintingHeadsVoltage\" message");
      throw EHeadHeater("OHDB didn't get ack for \"SetPrintingHeadsVoltage\" message");
      }

   if(!m_SetVoltage_AckOk)
      {
      m_FlagSetVoltageComUnderUse=false;
      throw EHeadHeater(Q2RT_HEADHEATER_INVALID_REPLY_ERROR);
      }

   UpdateSetVoltageTable(PotentiometerTable);
   m_FlagSetVoltageComUnderUse = false;
   return Q_NO_ERROR;
}

void CHeadHeaters::SetPrintingHeadsVoltageResponse(int TransactionId,
                                                   PVOID Data,
                                                   unsigned DataLength,
                                                   TGenericCockie Cockie)
{  
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBAck *ResponseMsg = static_cast<TOHDBAck *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBAck))
      {
      FrontEndInterface->NotificationMessage("SetPrintingHeadsVoltageResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetPrintingHeadsVoltageResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(ResponseMsg->MessageID) != OHDB_ACK)
      {
      FrontEndInterface->NotificationMessage("SetPrintingHeadsVoltageResponse MSGID error message");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetPrintingHeadsVoltageResponse MSGID error message (0x%X)",
                                        static_cast<int>(ResponseMsg->MessageID));
      return;
      }

   if (static_cast<int>(ResponseMsg->RespondedMessageID) != OHDB_SET_PRINTING_HEADS_VOLTAGES)
      {
      FrontEndInterface->NotificationMessage("SetPrintingHeadsVoltage:Responded Message ID Error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetPrintingHeadsVoltage:Responded Message ID Error (0x%X)",
                                        static_cast<int>(ResponseMsg->RespondedMessageID));
      return;
      }

   if (ResponseMsg->AckStatus)
      {
      FrontEndInterface->NotificationMessage("SetPrintingHeadsVoltage Ack status Error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"SetPrintingHeadsVoltage Ack status Error (%d)",
                                        static_cast<int>(ResponseMsg->AckStatus));
      return;
      }

   InstancePtr->m_SetVoltage_AckOk=true;
}     


TQErrCode CHeadHeaters::GetPrintingHeadsVoltages(void)
{
   //Verify if we are already not performing this command
   if (m_FlagGetVoltageComUnderUse)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadHeaters \"GetPrintingHeadsVoltages\" re-entry problem");
      return Q_NO_ERROR;
      }

   m_FlagGetVoltageComUnderUse=true;

   TOHDBGetPrintingHeadsVoltagesMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_GET_PRINTING_HEADS_VOLTAGES);

   m_GetVoltage_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetPrintingHeadsVoltagesMessage),
                            GetPrintingHeadsVoltagesResponse,
                            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagGetVoltageComUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"GetPrintingHeadsVoltages\" message");
      throw EHeadHeater("OHDB didn't get ack for \"GetPrintingHeadsVoltages\" message");
      }

   //m_SyncEventVoltage.WaitFor(OHDB_WAIT_ACK_TIMEOUT);

   if(!m_GetVoltage_AckOk)
      {
      m_FlagGetVoltageComUnderUse=false;
      throw EHeadHeater(Q2RT_HEADHEATER_INVALID_REPLY_ERROR);
      }

   m_FlagGetVoltageComUnderUse = false;
   return Q_NO_ERROR;    
}    


void CHeadHeaters::GetPrintingHeadsVoltagesResponse(int TransactionId,
                                                    PVOID Data,
                                                    unsigned DataLength,
                                                    TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBPrintingHeadsVoltagesResponse *VoltageMsg =
      static_cast<TOHDBPrintingHeadsVoltagesResponse *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBPrintingHeadsVoltagesResponse))
      {
      FrontEndInterface->NotificationMessage("GetPrintingHeadsVoltagesResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetPrintingHeadsVoltagesResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(VoltageMsg->MessageID) != OHDB_PRINTING_HEADS_VOLTAGES)
      {
      FrontEndInterface->NotificationMessage("GetHeaterStatus ack Error message id");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetHeaterStatus ack Error message id (0x%X)",
                                        static_cast<int>(VoltageMsg->MessageID));
      return;
      }

   int i;
   TPotentiometerTable PotentiometerTable;
   for(i=0;i<NUMBER_OF_POTENTIOMETERS;i++)
      {
      PotentiometerTable.Lookup(i) = static_cast<int>(VoltageMsg->HeadsVoltages[i]);
      }

   InstancePtr->UpdateCurrentVoltageTable(PotentiometerTable);
   InstancePtr->m_GetVoltage_AckOk=true;

   //InstancePtr->m_SyncEventVoltage.SetEvent();
}


TQErrCode CHeadHeaters::GetPowerSuppliesVoltages(void)
{
   //Verify if we are already not performing this command
   if (m_FlagGetPowerSuppliesComUnderUse)
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadHeaters \"GetPowerSuppliesVoltages\" re-entry problem");
      return Q_NO_ERROR;
      }

   m_FlagGetPowerSuppliesComUnderUse=true;

   TOHDBGetPowerSuppliesVoltagesMessage GetMsg;

   GetMsg.MessageID = static_cast<BYTE>(OHDB_GET_POWER_SUPPLIES_VOLTAGES);

   m_GetPowerSupplies_AckOk=false;

   // Send a Turn ON request
   if (m_OHDBClient->SendInstallWaitReply(&GetMsg,
                            sizeof(TOHDBGetPowerSuppliesVoltagesMessage),
                            GetPowerSuppliesVoltagesResponse,
							reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
      {
      m_FlagGetPowerSuppliesComUnderUse=false;
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"GetPowerSuppliesVoltages\" message");
      throw EHeadHeater("OHDB didn't get ack for \"GetPowerSuppliesVoltages\" message");
      }

   if(!m_GetPowerSupplies_AckOk)
      {
      m_FlagGetPowerSuppliesComUnderUse = false;
      throw EHeadHeater("Head Heater: \"GetPowerSuppliesVoltages\" Invalid reply from OHDB");
      }

   m_FlagGetPowerSuppliesComUnderUse = false;
   return Q_NO_ERROR;
}

void CHeadHeaters::GetPowerSuppliesVoltagesResponse(int TransactionId,
                                                    PVOID Data,
                                                    unsigned DataLength,
                                                    TGenericCockie Cockie)
{
   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBPowerSuppliesVoltagesMessage *PSMsg =
      static_cast<TOHDBPowerSuppliesVoltagesMessage *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBPowerSuppliesVoltagesMessage))
      {
      FrontEndInterface->NotificationMessage("GetPowerSuppliesVoltagesResponse length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetPowerSuppliesVoltagesResponse length error");
      return;
      }

   // Update DataBase before Open the Semaphore/Set event.
   if (static_cast<int>(PSMsg->MessageID) != OHDB_POWER_SUPPLIES_VOLTAGES)
      {
      FrontEndInterface->NotificationMessage("GetPowerSuppliesVoltagesResponse ack Error message id");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"GetPowerSuppliesVoltagesResponse ack Error message id (0x%X)",
                                        static_cast<int>(PSMsg->MessageID));
      return;
      }

     float Vpp,Vcc,Vdd,_24V;
	  
	  Vpp = CONVERT_VPP_A2D_TO_VOLT((float)PSMsg->VppPowerSupply);
	 
	  Vcc = CONVERT_VCC_A2D_TO_VOLT((float)PSMsg->VCCPowerSupply);
	  
	  Vdd = CONVERT_VDD_A2D_TO_VOLT((float)PSMsg->VDDPowerSupply);
	  
	  _24V = CONVERT_24V_A2D_TO_VOLT((float)PSMsg->V_24Voltage);
	  
	  
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_A2D_VPP_POWER_SUPPLIERS_STATUS,PSMsg->VppPowerSupply);
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_A2D_V24_POWER_SUPPLIERS_STATUS,PSMsg->V_24Voltage);
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_A2D_VDD_POWER_SUPPLIERS_STATUS,PSMsg->VDDPowerSupply);
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_A2D_VCC_POWER_SUPPLIERS_STATUS,PSMsg->VCCPowerSupply);
   
   /*InstancePtr->UpdatePowerSupply((0xfff & static_cast<float>(PSMsg->VppPowerSupply)),
                     (0xfff & static_cast<float>( PSMsg->Heater24VPowerSupply)),
                     (0xfff & static_cast<float>(PSMsg->VDDPowerSupply)),
                     (0xfff & static_cast<float>(PSMsg->VCCPowerSupply)));*/
   InstancePtr->UpdatePowerSupply(Vpp,_24V,Vdd,Vcc);
   
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_VPP_POWER_SUPPLIERS_STATUS,Vpp);
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_V24_POWER_SUPPLIERS_STATUS,_24V);
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_VDD_POWER_SUPPLIERS_STATUS,Vdd);
   FrontEndInterface->UpdateStatus(FE_CURRENT_OHDB_VCC_POWER_SUPPLIERS_STATUS,Vcc);

   InstancePtr->m_GetPowerSupplies_AckOk=true;
}


int CHeadHeatersBase::GetHeadTemperature(int HeadNumber)
{
  return m_Heaters[HeadNumber]->GetCurrentTemperatureInA2D();
}


// Notification Call Backs
void CHeadHeaters::NotificationHeadHeaterErrorResponse(int TransactionId,
                                                       PVOID Data,
                                                       unsigned DataLength,
                                                       TGenericCockie Cockie)
{
   int HeadError,Q2RTError;
   QString StringError;

   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBHeadHeaterErrorNotification *ErrorMsg
      = static_cast<TOHDBHeadHeaterErrorNotification *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBHeadHeaterErrorNotification))
      {
      FrontEndInterface->NotificationMessage("Head Heater Error Response length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"\"Head Heater Error Response length error");
      return;
      }

   if(static_cast<int>(ErrorMsg->MessageID) != OHDB_HEAD_HEATER_ERROR)
      {
      // Delete Pointer of message buffer.
      InstancePtr->AckToOhdbNotification(OHDB_HEAD_HEATER_ERROR,
                                         TransactionId,
                                         ACK_STATUS_FAILURE,
                                         Cockie);
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification Error message:",
                                        0,(int)ErrorMsg->MessageID);
      return;
      }

   InstancePtr->AckToOhdbNotification(OHDB_HEAD_HEATER_ERROR,
                                      TransactionId,
                                      ACK_STATUS_SUCCESS,
                                      Cockie);

   HeadError = static_cast<int>(ErrorMsg->Error);
   InstancePtr->UpdateLastError(HeadError);

   switch (HeadError)
      {
      case HEAD_NO_ERROR:
         return;
      case HEAD_ERROR_VOLTAGE_DROP:
         Q2RTError = Q2RT_HEAD_HEATER_VOLTAGE_DROP;
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heater voltage drop");
         break;
      case HEAD_ERROR_HEATING_TIMEOUT :
         Q2RTError = Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT;
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads and block heating timeout");
         break;

      case HEAD_ERROR_HEADS_HEATING_TIMEOUT :
         Q2RTError = Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT;
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heater timeout error");
         break;

      case HEAD_ERROR_BLOCKS_HEATING_TIMEOUT:
         Q2RTError = Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT;
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Block heater timeout error");
         break;

      case HEAD_ERROR_HEAD0_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD1_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD2_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD3_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD4_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD5_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD6_HEATING_TIMEOUT :
      case HEAD_ERROR_HEAD7_HEATING_TIMEOUT :
         Q2RTError = Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT;
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heater timeout error: Number %s",
                  GetHeaterStr(HeadError - HEAD_ERROR_HEAD0_HEATING_TIMEOUT).c_str());
         break;

      case HEAD_ERROR_BLOCK0_HEATING_TIMEOUT :
      case HEAD_ERROR_BLOCK1_HEATING_TIMEOUT :
      case HEAD_ERROR_BLOCK2_HEATING_TIMEOUT :
      case HEAD_ERROR_BLOCK3_HEATING_TIMEOUT :
         Q2RTError = Q2RT_HEAD_HEATER_TEMPERATURE_TIMEOUT;
         CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heater timeout error: Number %s",
                  GetHeaterStr((HeadError - HEAD_ERROR_BLOCK0_HEATING_TIMEOUT) +
                                            TOTAL_NUMBER_OF_HEADS_HEATERS).c_str());
         break;
      default:
         break;
      }

   FrontEndInterface->NotificationMessage("HEAD HEATER ERROR");

   InstancePtr->m_ErrorHandlerClient->ReportError("HeadHeater Error Event is:",Q2RTError,HeadError);
}



void CHeadHeaters::NotificationHeadsTemperatureError(int TransactionId,
                                                     PVOID Data,
                                                     unsigned DataLength,
                                                     TGenericCockie Cockie)
{
   QString ErrorString;

   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   TOHDBHeadsTemperatureError *ErrorMsg = static_cast<TOHDBHeadsTemperatureError *>(Data);

   //Verify size of message
   if(DataLength != sizeof(TOHDBHeadsTemperatureError))
   {
      FrontEndInterface->NotificationMessage("Head Temperature Error Response length error");
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"\"Head Temperature Error Response length error");
      return;
   }

   if(static_cast<int>(ErrorMsg->MessageID) != OHDB_HEADS_TEMPERATURE_ERROR)
   {
      // Delete Pointer of message buffer.
      InstancePtr->AckToOhdbNotification(OHDB_HEADS_TEMPERATURE_ERROR,
                                         TransactionId,
                                         ACK_STATUS_FAILURE,
                                         Cockie);
      InstancePtr->m_ErrorHandlerClient->ReportError("Notification Error message:",
                                        0,(int)ErrorMsg->MessageID);
      return;
   }

   InstancePtr->AckToOhdbNotification(OHDB_HEADS_TEMPERATURE_ERROR,
                                      TransactionId,
                                      ACK_STATUS_SUCCESS,
                                      Cockie);

   // Assembling the error message to the Log
   ErrorString = "\"Heads Temperature Error: ";
   for (int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
   {
     ErrorString += QIntToStr (ErrorMsg->Temperatures[i]);
     ErrorString += " ";
   }

   CQLog::Write(LOG_TAG_HEAD_HEATERS, ErrorString);
}



void CHeadHeaters::AckToOhdbNotification (int MessageID,
                                  int TransactionID,
                                  int AckStatus,
                                  TGenericCockie Cockie)
{
   TOHDBAck AckMsg;

   // Get a pointer to the instance
   CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

   AckMsg.MessageID = static_cast<BYTE>(OHDB_EDEN_ACK);
   AckMsg.RespondedMessageID =  static_cast<BYTE>(MessageID);
   AckMsg.AckStatus =  static_cast<BYTE>(AckStatus);

   // Send a Turn ON request
   InstancePtr->m_OHDBClient->SendNotificationAck(TransactionID,
                                                  &AckMsg,
                                                  sizeof(TOHDBAck));
}

//\\=======================================================================

// head heater recovery procedure.
void CHeadHeaters::Recovery (void)
{
}

// Cancel wait operations
void CHeadHeaters::Cancel(void)
{
   bool Canceled = false;

   m_MutexDataBase.WaitFor();
   m_CancelFlag = false;
   if(m_Waiting)
      {
      m_Waiting=false;
      m_CancelFlag = true;
      Canceled = true;
      m_SyncEventWaitForTemperature.SetEvent();
      }
   m_MutexDataBase.Release();

   if(Canceled)
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Cancel head heating print");
}


void CHeadHeaters::ReadHeadParametersFromE2PROM(int HeadNumber, THeadParameters* HeadParameters)
{
  m_ReadHeadParametersMutex.WaitFor();

  THeaterElementsTable ConversionTable;  // lenth of WORD
  BYTE PhysicalHead = 0;

  for (int i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    ConversionTable[i] = 0;

  ConversionTable[HeadNumber] = 1;

  for (BYTE i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    if (ConversionTable.Lookup(i) == 1)
      PhysicalHead = i;

  TOHDBGetHeadDataMsg GetMsg;
  GetMsg.MessageID = OHDB_GET_HEAD_DATA;
  GetMsg.HeadNum =   PhysicalHead;

  m_GetHeadData_AckOk = false;

  // Send the meassage
  if (m_OHDBClient->SendInstallWaitReply(&GetMsg, sizeof(TOHDBGetHeadDataMsg), ReadHeadDataResponse,
										 reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
    m_ReadHeadParametersMutex.Release();
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"GetHeadData\" message");
    throw EHeadHeater("OHDB didn't get ack for \"GetHeadData\" message");
  }

  if(!m_GetHeadData_AckOk)
  {
    m_ReadHeadParametersMutex.Release();
    throw EHeadHeater("Head Heater: \"GetHeadData\" Invalid reply from OHDB");
  }

  m_ReadHeadParametersMutex.Release();
  GetHeadParameters(HeadNumber, HeadParameters);
}


void CHeadHeaters::ReadHeadDataResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
  // Get a pointer to the instance
  CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

  TOHDBHeadDataMsg *HeadDataMsg = static_cast<TOHDBHeadDataMsg *>(Data);

  //Verify size of message
  if(DataLength != sizeof(TOHDBHeadDataMsg))
  {
    FrontEndInterface->NotificationMessage("Recieved \"HeadData\" message with incorrect length");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved \"GetHeadData\" message with incorrect length");
    return;
  }

  if (static_cast<int>(HeadDataMsg->MsgId) != OHDB_HEAD_DATA)
  {
    FrontEndInterface->NotificationMessage("Recieved \"HeadData\" message with incorrect message ID");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved \"HeadData\" message with incorrect message ID (0x%X)", HeadDataMsg->MsgId);
    return;
  }


  // Get the logical head number:
  THeaterElementsTable ConversionTable;
  for (int i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    ConversionTable[i] = i;
  int LogicalHeadNum = ConversionTable.Lookup(HeadDataMsg->HeadNum);

  InstancePtr->SetHeadParameters(LogicalHeadNum, (THeadParameters*)&(HeadDataMsg->A2DValueFor60C));

  InstancePtr->m_GetHeadData_AckOk = true;
}


void CHeadHeaters::SaveHeadParametersOnE2PROM(int HeadNumber, THeadParameters* HeadParameters)
{
  m_SaveHeadParametersMutex.WaitFor();

  THeaterElementsTable ConversionTable;  // lenth of WORD
  BYTE PhysicalHead = 0;

  for (int i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    ConversionTable[i] = 0;

  ConversionTable[HeadNumber] = 1;

  for (BYTE i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    if (ConversionTable.Lookup(i) == 1)
      PhysicalHead = i;

  TOHDBSetHeadDataMsg SetMsg;
  SetMsg.MessageID = OHDB_SET_HEAD_DATA;
  SetMsg.HeadNum = PhysicalHead;
  memcpy(&(SetMsg.A2DValueFor60C), HeadParameters, sizeof(THeadParameters));

  m_SetHeadData_AckOk = false;

  // Send the meassage
  if (m_OHDBClient->SendInstallWaitReply(&SetMsg, sizeof(TOHDBSetHeadDataMsg), SaveHeadDataResponse,
                                         reinterpret_cast<TGenericCockie>(this),WRITE_PARAMETERS_TO_E2PROM_TIMEOUT) != QLib::wrSignaled)
  {
    m_SaveHeadParametersMutex.Release();
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"SetHeadData\" message");
    throw EHeadHeater("OHDB didn't get ack for \"SetHeadData\" message");
  }

  if(!m_SetHeadData_AckOk)
  {
    m_ReadHeadParametersMutex.Release();
    throw EHeadHeater("Head Heater: \"SetHeadData\" Invalid reply from OHDB");
  }

  m_SaveHeadParametersMutex.Release();
  SetHeadParameters(HeadNumber, HeadParameters);
}

void CHeadHeaters::SaveHeadDataResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
  // Get a pointer to the instance
  CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

  TOHDBAck *AckMsg = static_cast<TOHDBAck *>(Data);

  //Verify size of message
  if(DataLength != sizeof(TOHDBAck))
  {
    FrontEndInterface->NotificationMessage("Recieved Ack (for \"SetHeadData\" message) with incorrect length");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved Ack (for \"SetHeadData\" message) with incorrect length");
    return;
  }

  if (static_cast<int>(AckMsg->MessageID) != OHDB_ACK)
  {
    FrontEndInterface->NotificationMessage("Recieved Ack (for \"SetHeadData\" message) with incorrect message ID");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved Ack (for \"SetHeadData\" message) with incorrect message ID (0x%X)", AckMsg->MessageID);
    return;
  }

  if (AckMsg->AckStatus)
  {
    FrontEndInterface->NotificationMessage("Recieved Nack for \"SetHeadData\" message");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved Nack for \"SetHeadData\" message", AckMsg->MessageID);
    return;
  }

  InstancePtr->m_SetHeadData_AckOk = true;
}



void CHeadHeaters::SaveHeadDataErrorNotification(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{

  TOHDBHeadsDataWriteErrorMsg *ErrorMsg = static_cast<TOHDBHeadsDataWriteErrorMsg *>(Data);

  //Verify size of message
  if(DataLength != sizeof(TOHDBHeadsDataWriteErrorMsg))
  {
    FrontEndInterface->NotificationMessage("Recieved \"Heads data write error\" message with incorrect length");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved \"Heads data write error\" message with incorrect length");
    return;
  }

  QString Message = "Error occoured while writing the head parameters of head #" + QIntToStr(ErrorMsg->HeadNum);
  FrontEndInterface->NotificationMessage(Message);
  CQLog::Write(LOG_TAG_HEAD_HEATERS,Message);
}

int CHeadHeaters::ReadFromE2PROM(int HeadNumber,USHORT Address)
{
  m_ReadFromE2PROMMutex.WaitFor();

  THeaterElementsTable ConversionTable;  // lenth of WORD
  int PhysicalHead = 0;

  for (int i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    ConversionTable[i] = 0;

  ConversionTable[HeadNumber] = 1;

  for (int i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    if (ConversionTable.Lookup(i) == 1)
      PhysicalHead = i;

  TOHDBE2PROMReadMessage ReadMsg;
  ReadMsg.MessageID = OHDB_READ_FROM_E2PROM;
  ReadMsg.HeadNum = PhysicalHead;
  ReadMsg.Address = Address;

  m_ReadFromE2PROM_AckOk = false;

  // Send the meassage
  if (m_OHDBClient->SendInstallWaitReply(&ReadMsg, sizeof(TOHDBE2PROMReadMessage), ReadFromE2PROMResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
    m_ReadFromE2PROMMutex.Release();
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"ReadFromE2PROM\" message");
    throw EHeadHeater("OHDB didn't get ack for \"ReadFromE2PROM\" message");
  }

  if(!m_ReadFromE2PROM_AckOk)
  {
    m_ReadFromE2PROMMutex.Release();
    throw EHeadHeater("Head Heater: \"ReadFromE2PROM\" Invalid reply from OHDB");
  }

  m_ReadFromE2PROMMutex.Release();
  return m_E2PROMData;
}

void CHeadHeaters::ReadFromE2PROMResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
  // Get a pointer to the instance
  CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

  TOHDBE2PROMResponse *E2PROMMsg = static_cast<TOHDBE2PROMResponse *>(Data);

  //Verify size of message
  if(DataLength != sizeof(TOHDBE2PROMResponse))
  {
    FrontEndInterface->NotificationMessage("Recieved \"E2PROMData\" message with incorrect length");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved \"E2PROMData\" message with incorrect length");
    return;
  }

  if (static_cast<int>(E2PROMMsg->MessageID) != OHDB_E2PROM_DATA)
  {
    FrontEndInterface->NotificationMessage("Recieved \"E2PROMData\" message with incorrect message ID");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved \"E2PROMData\" message with incorrect message ID (0x%X)", E2PROMMsg->MessageID);
    return;
  }

  InstancePtr->m_E2PROMData = E2PROMMsg->Data;

  InstancePtr->m_ReadFromE2PROM_AckOk = true;
}

TQErrCode CHeadHeaters::WriteToE2PROM(int HeadNumber,USHORT Address,int Data)
{
  m_WriteToE2PROMMutex.WaitFor();

  THeaterElementsTable ConversionTable;  // lenth of WORD
  BYTE PhysicalHead = 0;

  for (int i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    ConversionTable[i] = 0;

  ConversionTable[HeadNumber] = 1;

  for (BYTE i=0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    if (ConversionTable.Lookup(i) == 1)
      PhysicalHead = i;

  TOHDBE2PROMWriteMessage WriteMsg;
  WriteMsg.MessageID = OHDB_WRITE_DATA_TO_E2PROM;
  WriteMsg.HeadNum = PhysicalHead;
  WriteMsg.Address = Address;
  WriteMsg.Data = (BYTE)Data;

  m_WriteToE2PROM_AckOk = false;

  // Send the meassage
  if (m_OHDBClient->SendInstallWaitReply(&WriteMsg, sizeof(TOHDBE2PROMWriteMessage), WriteToE2PROMResponse,
                                         reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
  {
    m_WriteToE2PROMMutex.Release();
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"OHDB didn't get ack for \"WriteToE2PROM\" message");
    throw EHeadHeater("OHDB didn't get ack for \"WriteToE2PROM\" message");
  }

  if(!m_WriteToE2PROM_AckOk)
  {
    m_WriteToE2PROMMutex.Release();
    throw EHeadHeater("Head Heater: \"WriteToE2PROM\" Invalid reply from OHDB");
  }

  m_WriteToE2PROMMutex.Release();

  return Q_NO_ERROR;
}

void CHeadHeaters::WriteToE2PROMResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
  // Get a pointer to the instance
  CHeadHeaters *InstancePtr = reinterpret_cast<CHeadHeaters *>(Cockie);

  TOHDBAck *AckMsg = static_cast<TOHDBAck *>(Data);

  //Verify size of message
  if(DataLength != sizeof(TOHDBAck))
  {
    FrontEndInterface->NotificationMessage("Recieved Ack (for \"WriteToE2PROM\" message) with incorrect length");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved Ack (for \"WriteToE2PROM\" message) with incorrect length");
    return;
  }

  if (static_cast<int>(AckMsg->MessageID) != OHDB_ACK)
  {
    FrontEndInterface->NotificationMessage("Recieved Ack (for \"WriteToE2PROM\" message) with incorrect message ID");
    CQLog::Write(LOG_TAG_HEAD_HEATERS,"Recieved Ack (for \"WriteToE2PROM\" message) with incorrect message ID (0x%X)", AckMsg->MessageID);
    return;
  }

  InstancePtr->m_WriteToE2PROM_AckOk = true;

}

void CHeadHeaters::ResetHeatersStatusDatabase()
{
  for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
  {
    if(!m_Heaters[i]->GetIfHeadWorking())
      continue;
    m_Heaters[i]->m_TemperatureOk = HEAD_COLD;
  }
}

//-----------------------------------------------------
// Dummy procedures
//------------------------------------------------------

CHeadHeatersDummy::CHeadHeatersDummy(const QString& Name):
   CHeadHeatersBase(Name)
{
   UpdatePowerSupply( (40.0*4095.0/(21.0*2.4)),  // Vpp
                      (24.0*4095.0/(11.0*2.4)),  // 24V
                      (12.21*4095.0/(5.75*2.4)), // VDD
                      (5.04*4095.0/(2.5*2.4)) ); // VCC
}

// Destructor
CHeadHeatersDummy::~CHeadHeatersDummy(void)
{}

// Command to turn on/off the Head heater
TQErrCode CHeadHeatersDummy::SetHeaterOnOff(bool OnOff,WORD HeatersMask)
{
   SetHeaterState(OnOff);
   CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head heaters %s",OnOff ? "On" : "Off");
   return (Q_NO_ERROR);
}

// Command to wait to head heater to be on off 
TQErrCode CHeadHeatersDummy::WaitForHeadsTemperatureOK(bool ContinueJob)
{
   return Q_NO_ERROR;
}

TQErrCode CHeadHeatersDummy::SetHeatersTemperature()
{
  SetHeatersTemperature(m_HeatersSetTemperatures);
  return (Q_NO_ERROR);
}

TQErrCode CHeadHeatersDummy::SetHeatersTemperature(THeaterElementsTable HeaterElementsTable)
{
   int  i;
   WORD j, HeatersMask = 0;

   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
   {
      j= (WORD)(1 << i);
      if(m_ParamsMgr->HeatersMaskTable[i])
         HeatersMask += j;
   }

   bool HeadWorking;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
   {
      j           = (WORD)(1 << i);
      HeadWorking = (HeatersMask & j);
      m_Heaters[i]->UpdateIfWorkingHead(HeadWorking);
      if(HeadWorking)
      {
         m_Heaters[i]->UpdateSetTemperatureA2D(HeaterElementsTable[i]);
         m_Heaters[i]->UpdateCurrentTemperatureA2D(HeaterElementsTable[i]);
         FrontEndInterface->UpdateStatus(FE_HEAD_HEATERS_STATUS_BASE+i,HEAD_TEMPERATURE_OK);
         FrontEndInterface->UpdateStatus(FE_HEAD_HEATER_A2D_BASE+i,HeaterElementsTable[i]);
         FrontEndInterface->UpdateStatus(FE_HEAD_HEATER_CELSIUS_BASE+i,HeaterElementsTable[i]);
      }
   }
   return Q_NO_ERROR;
}


// Command to get heaters status
TQErrCode CHeadHeatersDummy::GetHeatersStatus(void)
{
   int i;
   TQErrCode Err;
   int CurrentA2DValue;
   //Test
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      {
      if(m_Heaters[i]->GetIfHeadWorking())
         {
         CurrentA2DValue = m_Heaters[i]->GetCurrentTemperatureInA2D();
         Err = CheckThermistorsIntegrity(i,CurrentA2DValue);
         if(Err != Q_NO_ERROR)
            {
            if(IsHeaterOn())
                {
                SetDefaultOnOff(HEAD_HEATER_OFF);
                CQLog::Write(LOG_TAG_HEAD_HEATERS,"Turn off head heaters");
                m_ErrorHandlerClient->ReportError("Head thermistor is out of limits ",
                                                  Err,
                                                  CurrentA2DValue);
               }
            }

          FrontEndInterface->UpdateStatus(FE_HEAD_HEATER_A2D_BASE+i,
                             CurrentA2DValue);
          FrontEndInterface->UpdateStatus(FE_HEAD_HEATER_CELSIUS_BASE+i,
                             m_Heaters[i]->GetCurrentTemperatureInCelcius() );
         FrontEndInterface->UpdateStatus(FE_HEAD_HEATERS_STATUS_BASE+i,
                             m_Heaters[i]->GetIfTemperatureOk());
         }
      }
      
   return Q_NO_ERROR;
}

TQErrCode CHeadHeatersDummy::GetPreHeaterStatus(void)
{
   return Q_NO_ERROR;
}


// Command to get if temperature are in range 
TQErrCode CHeadHeatersDummy::IsHeadTemperatureOk(void)
{
   return Q_NO_ERROR;
}

TQErrCode CHeadHeatersDummy::SetStandbyTemperature(THeaterElementsTable HeaterTable)
{
   int i;
   for(i=0;i<TOTAL_NUMBER_OF_HEATERS;i++)
      m_Heaters[i]->UpdateStandbyTemperatureA2D(HeaterTable[i]);

   return Q_NO_ERROR;
}

// Command to get standby heaters tempearture values
TQErrCode CHeadHeatersDummy::GetStandbyTemperatures(void)
{
   return Q_NO_ERROR;
}


TQErrCode CHeadHeatersDummy::SetPrintingHeadsVoltage(TPotentiometerTable)
{
   return Q_NO_ERROR;
}

TQErrCode CHeadHeatersDummy::GetPrintingHeadsVoltages(void)
{
   return Q_NO_ERROR;
}

TQErrCode CHeadHeatersDummy::GetPowerSuppliesVoltages(void)
{
   return Q_NO_ERROR;
}

void CHeadHeatersDummy::Cancel(void)
{
   bool Canceled = false;

   m_MutexDataBase.WaitFor();
   if(m_Waiting)
      {
      m_Waiting=false;
      Canceled = true;
      }
   m_MutexDataBase.Release();

   if(Canceled)
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Cancel head heating print");
}

TQErrCode CHeadHeatersDummy::SetPrintingHeadsVoltage()
{
  return Q_NO_ERROR;
}

TQErrCode CHeadHeatersDummy::SetPrintingHeadVoltage(int HeadNum, int PotentiometerValue)
{
  return Q_NO_ERROR;
}

//---------------------------------------
//Test procedure
TQErrCode CHeadHeatersBase::Test(void)
{
/*
   CHeadTemperature T,TR;
   int Celcius=0;

   T.AssignFromCelcius(600);
   TR = T;
   Celcius = TR.CelciusValue();
   Celcius = T.CelciusValue();
   Celcius = Celcius;
   Celcius = 0;

   T.AssignFromCelcius(100);
   TR = T;
   Celcius = TR.CelciusValue();
   Celcius = T.CelciusValue();
   Celcius = Celcius;
   Celcius = 0;

   T.AssignFromCelcius(10);
   TR = T;
   Celcius = TR.CelciusValue();
   Celcius = T.CelciusValue();
   Celcius = Celcius;
   Celcius = 0;

   T.AssignFromA2D(600);
   TR = T;
   Celcius = TR.CelciusValue();
   Celcius = T.CelciusValue();

   try
      {
      if (GetHeatersStatus())
        throw EHeadHeater("Heaters status fail");

      if (WaitForHeadsTemperatureOK())
         throw EHeadHeater("wait to heater on temperature error");

      if(SetDefaultHeateresTemperature())
         throw EHeadHeater("set heater temperature error");

      if(SetDefaultStandbyTemperature())
         throw EHeadHeater("set default heater temperature error");

      //Start commands
      if (SetDefaultOnOff(HEAD_HEATER_ON))
         throw EHeadHeater("set heater on error");

      if (GetHeatersStatus())
        throw EHeadHeater("Heaters status fail");

      //Can StartPrint
      //if (WaitForHeadsTemperatureOK())
      //   throw EHeadHeater("wait to heater on temperature error");

      //EndCommands
      if (SetDefaultOnOff(HEAD_HEATER_OFF))
         throw EHeadHeater("Heater off error");

      if (WaitForHeadsTemperatureOK())
         throw EHeadHeater("wait to heater on temperature error");

      } // Catch file loading related errors

   catch(...)
     {
       QMonitor.Print("Head Tester:Error catched Occurred ");
     }
  */
   return (Q_NO_ERROR);
}


//Statistics
//---------------------------------------------------
void CHeadHeatersBase::DisplayStatistics(void)
{
    if(m_TemperatureOutOfMargine)
       {
       CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head Heater was in range %6.2f percent of the time (%d/%d).",
                  (100*(float)(1-((float)m_TemperatureOutOfMargine/(float)m_TemperatureOutOfMargineTotalCount))),
                  m_TemperatureOutOfMargine,m_TemperatureOutOfMargineTotalCount);
       }
}

void CHeadHeatersBase::ResetStatistics(void)
{
   m_TemperatureOutOfMargine=0;
   m_TemperatureOutOfMargineTotalCount=0;
}

bool CHeadHeaters::AreHeadsTemperaturesOk()
{
  int i;
  for(i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
    {
    if(!m_Heaters[i]->GetIfHeadWorking())
      continue;

    if(m_Heaters[i]->GetIfTemperatureOk() != HEAD_TEMPERATURE_OK)
      return false;
    }
  return true;
}

bool CHeadHeaters::IsHeaterTemperatureOk (int HeaterNum)
{
  if (!VALIDATE_HEATER_INCLUDING_PREHEATER(HeaterNum))
    throw EHeadHeater("Invalid heater number (Range is 0-12)");

  // If heater doesn't work, return true
  if (m_Heaters[HeaterNum]->GetIfHeadWorking() != true)
     return true;
  if (m_Heaters[HeaterNum]->GetIfTemperatureOk() != HEAD_TEMPERATURE_OK)
     return false;

  return true;
}

bool CHeadHeatersDummy::IsHeaterTemperatureOk (int HeaterNum)
{
  return true;
}

bool CHeadHeatersDummy::AreHeadsTemperaturesOk()
{
  return (!FindWindow(0, "HeatingTimeout.txt - Notepad"));

///////////////////////////////////////////////////////////////////

// Use this code to generate heads heating delays specified in miliseconds
  // static int time = 0;
  // bool res = false;

  // if (0 == time || QGetTicks() - time > 10000)  // delay reset time
		// time = QGetTicks();

  // if (QGetTicks() - time > 6000)  // heating delay time
  // {
      // time = 0;
      // res = true;

  // }
  // return res;
  
///////////////////////////////////////////////////////////////////

//  return true;
}

void CHeadHeatersDummy::ReadHeadParametersFromE2PROM(int HeadNumber, THeadParameters* HeadParameters)
{
  GetHeadParameters(HeadNumber, HeadParameters);
}

void CHeadHeatersDummy::SaveHeadParametersOnE2PROM(int HeadNumber, THeadParameters* HeadParameters)
{
  SetHeadParameters(HeadNumber, HeadParameters);
}


int CHeadHeatersDummy::ReadFromE2PROM(int HeadNumber,USHORT Address)
{
  return 0x55;
}

TQErrCode CHeadHeatersDummy::WriteToE2PROM(int HeadNumber,USHORT Address,int Data)
{
  return Q_NO_ERROR;
}

TQErrCode CHeadHeaters::SetAndMeasureHeadVoltage(int HeadNumber, float ReqVoltage)
{
	float PrevDeviation     = MAXFLOAT;
	float PrevVoltage       = 0.0;
	int   CurrentVoltageA2D = 0;
	float CurrentVoltage    = 0.0;
	int   PotValue          = 0;
	int   PrevPotValue      = 0;
	unsigned EndTime;
	float deviation         = 0.0;
	float bestDeviation     = 0.0;
	
	int   CalibrationPointCrossingsCounter = 0;
	
	QString VoltageErrorStr = "Voltage error in ";

        float min_limit = m_ParamsMgr->HSW_MinVoltageArray[HeadNumber].Value();
	float max_limit = m_ParamsMgr->HSW_MaxVoltageArray[HeadNumber].Value();
	CHECK_REQUESTED_VOLTAGE_LIMITS(ReqVoltage, EHeadHeater,HeadNumber, min_limit,max_limit);


	// Calculate the potentiometer value
	PotValue = CalculatePotValue(HeadNumber, ReqVoltage);

	EndTime = QGetTicks() + QSecondsToTicks(VOLTAGE_STABILIZATION_TIME);

	CQLog::Write(LOG_TAG_HEAD_HEATERS,"Calibrating Head: %s", GetHeaterStr(HeadNumber).c_str());
	CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadNum %d - start calibrating. ReqVolt %.2f", HeadNumber, ReqVoltage);
	while (!m_CancelFlag)
	{
		if (QGetTicks() > EndTime)
		{
			// Timeout....
			VoltageErrorStr += GetHeaterStr(HeadNumber);
			throw EHeadHeater(VoltageErrorStr);
		}

		// Set the potentiometer value for the requested head
		SetPrintingHeadVoltage(HeadNumber, PotValue);
		SetPrintingHeadsVoltage();

		// Wait for the stabilization of the voltages
		QSleep(HEAD_VOLTAGES_STABILIZATION_TIME);

		// Get the current voltage
		GetPrintingHeadsVoltages();
		
		CurrentVoltageA2D = GetCurrentVoltageTable(HeadNumber);
		CurrentVoltage = CONVERT_HEAD_VPP_A2D_TO_VOLT(CurrentVoltageA2D);
		
		CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadNum %d SetPot %d CurrVolt %.2f (ReqVolt %.2f)", HeadNumber, PotValue, CurrentVoltage, ReqVoltage);		
		
		if (0.0 == PrevVoltage) // on first iteration initialize PrevVoltage:
			PrevVoltage = CurrentVoltage;

                float voltage_tolerance  = 2 * fabs((EstimateHeadVoltage(HeadNumber, PotValue) -  EstimateHeadVoltage(HeadNumber,PotValue + 1)));
                
		deviation = fabs(ReqVoltage - CurrentVoltage);

		
		if (((ReqVoltage - CurrentVoltage) * (ReqVoltage - PrevVoltage)) <= 0) // Stopping condition: if we have just passed the ReqVoltages point.
		{
			bestDeviation = deviation;

			if (PrevDeviation < deviation) // "rollback" values with prev values. (when previous deviation is the smallest one.)
				bestDeviation  = PrevDeviation;

			if (bestDeviation > voltage_tolerance) // Detect potential HW *minor* problems.
			{
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"Head Voltage Calibration Warning: Head: %s", GetHeaterStr(HeadNumber).c_str());
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"                                  Required Voltage: %.2f Deviation: %.2f Estim-Tolerance: %.2f", ReqVoltage, PrevDeviation, voltage_tolerance);
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"                                  Curr Point: Potentiometer: %d Voltage: %.2f", PotValue,   CurrentVoltage);
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"                                  Lase Point: Potentiometer: %d Voltage: %.2f", PrevPotValue,   PrevVoltage);				
				
				// Detect potential HW *major* problems: could not reach a voltage deviation below HEAD_VOLTAGE_TOLERANCE
				if (CalibrationPointCrossingsCounter > CALIBRATION_FAIL_RETRIES)
				{
					if (bestDeviation > HEAD_VOLTAGE_TOLERANCE)
					{						
						QString Output = QFormatStr("Retry number: %s. Could't calibrate the %s head: could't reach a voltage deviation below %s V",IntToStr(CalibrationPointCrossingsCounter).c_str(),
																																					GetHeaterStr(HeadNumber).c_str(),
																																					FloatToStr(HEAD_VOLTAGE_TOLERANCE).c_str());


						CQLog::Write(LOG_TAG_HEAD_HEATERS, Output);
						throw EQException(Output);
					}


					else
					{
						// following assertion should always hold true here: (CalibrationPointCrossingsCounter > FORCED_RETRIES)
						break; // "not so Successfully" finished calibrating this head's voltage, but still.
					}
				}				
			}
			else			
			{	
				if (CalibrationPointCrossingsCounter >= FORCED_RETRIES)
					break; // Successfully finished calibrating this head's voltage.
				
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadNum %d - crossed calibration point (with small voltage deviation). Still, going now to cross it again for verification.", HeadNumber);					
			}
			

			CalibrationPointCrossingsCounter++;
			
		} // if (((ReqVoltage - CurrentVoltage) * (ReqVoltage - PrevVoltage)) <= 0) // Stopping condition: if we have just passed the ReqVoltages point.	

			PrevDeviation = deviation;

		PrevPotValue = PotValue;
		PrevVoltage  = CurrentVoltage;

		if (ReqVoltage < CurrentVoltage)
			PotValue--;
		else
			PotValue++;

		if (PotValue < 0)
		{
			PotValue = 0;
		}
	} // while

	if (PrevDeviation < deviation) // "rollback" values with prev values, if previous deviation was the smallest one.
	{
		PotValue = PrevPotValue;
		CurrentVoltage = PrevVoltage;
		
		SetPrintingHeadVoltage(HeadNumber, PotValue);
		SetPrintingHeadsVoltage();
		
		QSleep(HEAD_VOLTAGES_STABILIZATION_TIME);
		GetPrintingHeadsVoltages();
		CQLog::Write(LOG_TAG_HEAD_HEATERS,"(rollback) HeadNum %d SetPot %d CurrVolt %.2f (ReqVolt %.2f)", HeadNumber, PotValue, CurrentVoltage, ReqVoltage);				
	}

	CQLog::Write(LOG_TAG_HEAD_HEATERS,"HeadNum %d - finished calibrating. ReqVolt %.2f BestDeviation %.2f PotVal %d", HeadNumber, ReqVoltage, bestDeviation, PotValue);

	return Q_NO_ERROR;
}


QString CHeadHeatersBase::GetHeaterErrorDescription()
{
  QString HeatersOutput    = "No Heating problem was found";
  QString TmpHeatersOutput = "Heating problem found in:\n";
  bool HeatingProblem      = false;

  // Checking for heating problem...
  for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
  {
    if (IsHeaterTemperatureOk(i) != true)
    {
      HeatingProblem = true;
      TmpHeatersOutput += GetHeaterStr(i);
      TmpHeatersOutput += "\n";
    }
  }
  if (HeatingProblem == true)
    HeatersOutput = TmpHeatersOutput;
    
  return HeatersOutput;
}


TQErrCode CHeadHeatersDummy::SetAndMeasureHeadVoltage(int HeadNumber, float ReqVoltage)
{
  float min_limit = m_ParamsMgr->HSW_MinVoltageArray[HeadNumber].Value();
  float max_limit = m_ParamsMgr->HSW_MaxVoltageArray[HeadNumber].Value();
  CHECK_REQUESTED_VOLTAGE_LIMITS(ReqVoltage, EHeadHeater,HeadNumber,min_limit,max_limit);
  return Q_NO_ERROR;
}

QString CHeadHeatersDummy::GetHeaterErrorDescription()
{
  return "We're in Emulation...\n" + CHeadHeatersBase::GetHeaterErrorDescription();
}

TQErrCode CHeadHeatersDummy::CheckThermistorsIntegrity(int Head,int A2DValue)
{
   TQErrCode Err = Q_NO_ERROR;

   if(FindWindow(0, "HeaterThermistorShort.txt - Notepad"))
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads Heater %s, Thermistor short (%d)",
                                         GetHeaterStr(Head).c_str(),
                                         A2DValue);
      Err = Q2RT_HEAD_HEATER_THERMISTOR_SHORT;
      }

   if(FindWindow(0, "HeaterThermistorOpen.txt - Notepad"))
      {
      CQLog::Write(LOG_TAG_HEAD_HEATERS,"Heads Heater %s, Thermistor open (%d)",
                                         GetHeaterStr(Head).c_str(),
                                         A2DValue);
      Err=Q2RT_HEAD_HEATER_THERMISTOR_OPEN;
      }

   return Err;
}

