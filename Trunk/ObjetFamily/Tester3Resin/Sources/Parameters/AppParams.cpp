/********************************************************************
 *                Quadra 1Application parameters set                *
 *                ----------------------------------                *
 * Module description: This module define the set of parameters     *
 *                     required for the application.                *
 *                                                                  *
 * Compilation: Standard C++ / BCB.                                 *
 *                                                                  *
 * Author: .                                                        *
 * Start date: 21/8/2000                                            *
 * Last upate: 21/08/2000                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "AppParams.h"


// Pointer to the singleton instance (static)
CAppParams *CAppParams::m_AppParamsInstance = NULL;


// Constructor - Initialize all the app parameters using the parameters manager
//               predefined macros (e.g. INIT_PARAM).
CAppParams::CAppParams(const QString& IniFileName) : CQParamsContainer(NULL, "")
{
  INIT_PARAM(LastWorkingDirectory,"","General");
  INIT_PARAM(OCBCommCheck,"true","OCB");
  INIT_PARAM(OHDBA2DReadingsDisplay,"true","OHDB");
  INIT_PARAM(PCICardExist,"false","OHDB");
  INIT_PARAM(FireFrquency,2000,"Heads Tester");
  INIT_PARAM(PulseWidth,5.5f,"Heads Tester");
  INIT_PARAM(PulseDelay,28.0f,"Heads Tester");
  INIT_PARAM(PostPulseDelay,8,"Heads Tester");
  INIT_PARAM(NumOfFires,1,"Heads Tester");
  INIT_PARAM(FireTime,100,"Heads Tester");
  INIT_PARAM(CycleOnTime,1,"Heads Tester");
  INIT_PARAM(CycleOffTime,1,"Heads Tester");
  INIT_PARAM(CycleTotalTime,1,"Heads Tester");
  INIT_PARAM(NumOfCycles,5,"Heads Tester");
  INIT_PARAM(NozzlePatternValue,"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
								"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
                                "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
                                "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0","Heads Tester");

  INIT_PARAM(ModelHead0Temp,1800,"OHDB");
  INIT_PARAM(ModelHead1Temp,1800,"OHDB");
  INIT_PARAM(ModelHead2Temp,1800,"OHDB");
  INIT_PARAM(ModelHead3Temp,1800,"OHDB");
  INIT_PARAM(SupportHead0Temp,1800,"OHDB");
  INIT_PARAM(SupportHead1Temp,1800,"OHDB");
  INIT_PARAM(SupportHead2Temp,1800,"OHDB");
  INIT_PARAM(SupportHead3Temp,1800,"OHDB");
  INIT_PARAM(ModelBlockFrontTemp,1800,"OHDB");
  INIT_PARAM(ModelBlockRearTemp,1800,"OHDB");
  INIT_PARAM(SupportBlockFrontTemp,1800,"OHDB");
  INIT_PARAM(SupportBlockRearTemp,1800,"OHDB");
  INIT_PARAM(ExternalLiquidTemp,1800,"OHDB");
  INIT_PARAM(LowThresholdTemp,100,"OHDB");
  INIT_PARAM(HighThresholdTemp,200,"OHDB");

  INIT_PARAM(SerialNumber,1234,"OHDB");

  INIT_PARAM(BumperSensitivity,1200,"Bumper");
  INIT_PARAM(BumperResetTime,700,"Bumper");
  INIT_PARAM(BumperImpactCounter,1,"Bumper");
  INIT_PARAM(ComLogEnabled,false,"Heads Tester");

  INIT_PARAM(ModelPotValuesByLayerThick,"128,128,128,128,128,128,128,128,128,128,128,128,128,128,128","OHDB");

  // OCB Parameters
  // --------------
  INIT_PARAM(ActiveThermistors,"1,1,1,1,0,0","OCB");//objet_machine
  INIT_PARAM(Model1LevelThermistorLow,1100,"OCB");
  INIT_PARAM(Model1LevelThermistorHigh,1120,"OCB");
  INIT_PARAM(Model2LevelThermistorLow,1100,"OCB");
  INIT_PARAM(Model2LevelThermistorHigh,1120,"OCB");
  INIT_PARAM(Model3LevelThermistorLow,1100,"OCB");
  INIT_PARAM(Model3LevelThermistorHigh,1120,"OCB");
  INIT_PARAM(ModelLevelThermistorLow,1100,"OCB");
  INIT_PARAM(ModelLevelThermistorHigh,1120,"OCB");
  INIT_PARAM(TimePumpON,1,"OCB");
  INIT_PARAM(TimePumpOFF,5,"OCB");
  INIT_PARAM(TimeoutFilling,240,"OCB");

  INIT_PARAM(SupportLevelThermistorLow,1150,"OCB");
  INIT_PARAM(SupportLevelThermistorHigh,1170,"OCB");
  INIT_PARAM(SLevelThermistorLow,1150,"OCB");
  INIT_PARAM(SLevelThermistorHigh,1170,"OCB");

  INIT_PARAM(PurgeTime,2000,"OCB");
  INIT_PARAM_WITH_LIMITS(RFReadersConnection,"3,4,6,7","0,0,0,0","8,8,8,8","RFReaders");
  RFReadersConnection.SetDescription("COM Port numbers for each RF reader. Zero means unused.");
  INIT_PARAM_WITH_LIMITS(RFReadersChannels,"4,4,4,4","0,0,0,0","8,8,8,8","RFReaders");
  RFReadersChannels.SetDescription("Number of channels to be used by each RF reader.");
  ////////////////////

  ModelPotValuesByLayerThick.SetCellDescription(0,"15u");
  ModelPotValuesByLayerThick.SetCellDescription(1,"16u");
  ModelPotValuesByLayerThick.SetCellDescription(2,"17u");
  ModelPotValuesByLayerThick.SetCellDescription(3,"18u");
  ModelPotValuesByLayerThick.SetCellDescription(4,"19u");
  ModelPotValuesByLayerThick.SetCellDescription(5,"20u");
  ModelPotValuesByLayerThick.SetCellDescription(6,"21u");
  ModelPotValuesByLayerThick.SetCellDescription(7,"22u");
  ModelPotValuesByLayerThick.SetCellDescription(8,"23u");
  ModelPotValuesByLayerThick.SetCellDescription(9,"24u");
  ModelPotValuesByLayerThick.SetCellDescription(10,"25u");
  ModelPotValuesByLayerThick.SetCellDescription(11,"26u");
  ModelPotValuesByLayerThick.SetCellDescription(12,"27u");
  ModelPotValuesByLayerThick.SetCellDescription(13,"28u");
  ModelPotValuesByLayerThick.SetCellDescription(14,"29u");

  INIT_PARAM(PotentiometerSetValue,"205,205,205,205,205,205,205,205","Heads Voltages");

  INIT_PARAM(SupportPotValuesByLayerThick,"128,128,128,128,128,128,128,128,128,128,128,128,128,128,128","OHDB");
  SupportPotValuesByLayerThick.SetCellDescription(0,"15u");
  SupportPotValuesByLayerThick.SetCellDescription(1,"16u");
  SupportPotValuesByLayerThick.SetCellDescription(2,"17u");
  SupportPotValuesByLayerThick.SetCellDescription(3,"18u");
  SupportPotValuesByLayerThick.SetCellDescription(4,"19u");
  SupportPotValuesByLayerThick.SetCellDescription(5,"20u");
  SupportPotValuesByLayerThick.SetCellDescription(6,"21u");
  SupportPotValuesByLayerThick.SetCellDescription(7,"22u");
  SupportPotValuesByLayerThick.SetCellDescription(8,"23u");
  SupportPotValuesByLayerThick.SetCellDescription(9,"24u");
  SupportPotValuesByLayerThick.SetCellDescription(10,"25u");
  SupportPotValuesByLayerThick.SetCellDescription(11,"26u");
  SupportPotValuesByLayerThick.SetCellDescription(12,"27u");
  SupportPotValuesByLayerThick.SetCellDescription(13,"28u");
  SupportPotValuesByLayerThick.SetCellDescription(14,"29u");

  INIT_PARAM(TemperatureCompensation,0,"OHDB");
  INIT_PARAM(RefPointDistance,0,"OHDB");

  //Simulator
  INIT_PARAM(SimulatorMode,false,"Simulator");
  SimulatorMode.SetDescription("Enter the Simulator mode");
  SimulatorMode.SetAttributes(paRestartRequired);
  INIT_PARAM_WITH_LIMITS(FactorTimeBetween,5,1,500,"Simulator");
  FactorTimeBetween.SetDescription("Factor to time_Between to OCB and Head parameters(in simulator mode). ");
  FactorTimeBetween.SetAttributes(paRestartRequired);

  INIT_PARAM_WITH_LIMITS(PingReplayInMS,5000,1000,60000,"Simulator");
  PingReplayInMS.SetDescription("Maximum time to wait for a ping replay(in simulator mode). ");
  PingReplayInMS.SetAttributes(paRestartRequired);

  INIT_PARAM_WITH_LIMITS(OCBWaitAckTimeout,5000,1000,60000,"Simulator");
  OCBWaitAckTimeout.SetDescription("Maximum time to wait for a single replay(in simulator mode). ");
  OCBWaitAckTimeout.SetAttributes(paRestartRequired);
 
  INIT_PARAM_WITH_LIMITS(OHDBWaitAckTimeout,5000,1000,60000,"Simulator");
  OHDBWaitAckTimeout.SetDescription("Maximum time to wait for a single replay(in simulator mode). ");
  OHDBWaitAckTimeout.SetAttributes(paRestartRequired);


  // Parameters for Gen4 heads
  INIT_PARAM(GenFourPotValues,              "0,0,0,0,0,0,0,0", "Gen4 Heads");
  INIT_PARAM(GenFourHeaters,                "1800,1800,1800,1800,1800,1800,1800,1800,1800,1800,1800,1800,1800", "Gen4 Heads");
  INIT_PARAM(GenFourBlockFillingThermsLow,  "1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100", "Gen4 Heads");
  INIT_PARAM(GenFourBlockFillingThermsHigh, "1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100", "Gen4 Heads");
  INIT_PARAM(GenFourActiveThermistors,      "1,1,1,1,1,1,1,1,0,0,0,0", "Gen4 Heads");
  INIT_PARAM(GenFourActiveTanksPerChamber,  "0,2,4,6,8,10,12,14", "Gen4 Heads");
  INIT_PARAM(GenFourNozzlePatternValues,    "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0," \
											"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0", "Gen4 Heads");
  INIT_PARAM(GenFourInterval,               3,   "Gen4 Heads");
  INIT_PARAM(GenFourPropagationDelay,       650, "Gen4 Heads");
  INIT_PARAM(GenFourSlewRate,               12,  "Gen4 Heads");
  INIT_PARAM(GenFourDoublePulse_Width_P1,   5.5, "Gen4 Heads");
  INIT_PARAM(GenFourDoublePulse_Width_P2,   5.5, "Gen4 Heads");
  INIT_PARAM(GenFourVoltagePercentage,      80,  "Gen4 Heads");
  INIT_PARAM(GenFourHeatingRate,            6,   "Gen4 Heads");
  INIT_PARAM(HeatingWatchdogSamplingInterval, 3, "Gen4 Heads");

  // Create parameters stream for the parameters file
  m_ParamsStream = new CQParamsFileStream(IniFileName);

  // Assign the stream to the container
  AssignParamsStream(m_ParamsStream);

}

// Factory functions
void CAppParams::Init(const QString& IniFileName)
{
  // Avoid allocating an instance if already allocated
  if(!m_AppParamsInstance)
  {
    m_AppParamsInstance = new CAppParams(IniFileName);
    m_AppParamsInstance->LoadAll();
  }
}

CAppParams::~CAppParams()
{
  m_AppParamsInstance->SaveAll();
	Q_SAFE_DELETE(m_ParamsStream);
}

void CAppParams::DeInit(void)
{
  // Deallocate instance
  if(m_AppParamsInstance)
  {

    Q_SAFE_DELETE(m_AppParamsInstance);
    m_AppParamsInstance = NULL;

  }
}

// Get a pointer to the instance
CAppParams *CAppParams::GetInstance(void)
{
  return m_AppParamsInstance;
}

CAppParams *CAppParams::Instance(void)
{
  return m_AppParamsInstance;
}
// End of "AppParams.cpp"

