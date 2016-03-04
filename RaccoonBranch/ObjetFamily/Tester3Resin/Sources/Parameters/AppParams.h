/********************************************************************
 *                Quadra1 Application parameters set                *
 *                ----------------------------------                *
 * Module description: This module define the set of parameters     *
 *                     required for the application.                *
 *                                                                  *
 * Compilation: Standard C++ / BCB.                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 7/05/2000                                            *
 * Last upate: 16/08/2000                                           *
 ********************************************************************/

#ifndef _APP_PARAMS_H_
#define _APP_PARAMS_H_

#include "QParamsContainer.h"
#include "QFileParamsStream.h"

// This class is implemented as a singleton with Init/DeInit factory static functions
class CAppParams : public CQParamsContainer {
  private:
    // Pointer to the singleton instance
    static CAppParams *m_AppParamsInstance;

    // Private constructor prevents un-authorized initialization
    CAppParams(const QString& IniFileName);

    CAppParams::~CAppParams();

    CQParamsFileStream *m_ParamsStream;

  public:
    // Factory functions
    static void Init(const QString& IniFileName="");
    static void DeInit(void);

    // Get an instance pointer
    static CAppParams *GetInstance(void);

    static CAppParams *Instance(void);
    // Parameters defintion start here
    // -------------------------------------
    DEFINE_PARAM(QString,LastWorkingDirectory);
    DEFINE_PARAM(bool,OCBCommCheck);
    DEFINE_PARAM(bool,OHDBA2DReadingsDisplay);
    DEFINE_PARAM(bool,PCICardExist);
    DEFINE_PARAM(int,FireFrquency);
    DEFINE_PARAM(float,PulseWidth);
    DEFINE_PARAM(float,PulseDelay);
    DEFINE_PARAM(int,PostPulseDelay);
    DEFINE_PARAM(int,NumOfFires);
    DEFINE_PARAM(int,FireTime);
    DEFINE_PARAM(int,CycleOnTime);
    DEFINE_PARAM(int,CycleOffTime);
    DEFINE_PARAM(int,CycleTotalTime);
    DEFINE_PARAM(int,ModelHead0Temp);
    DEFINE_PARAM(int,ModelHead1Temp);
    DEFINE_PARAM(int,ModelHead2Temp);
    DEFINE_PARAM(int,ModelHead3Temp);
    DEFINE_PARAM(int,SupportHead0Temp);
    DEFINE_PARAM(int,SupportHead1Temp);
    DEFINE_PARAM(int,SupportHead2Temp);
    DEFINE_PARAM(int,SupportHead3Temp);
    DEFINE_PARAM(int,ModelBlockFrontTemp);
    DEFINE_PARAM(int,ModelBlockRearTemp);
    DEFINE_PARAM(int,SupportBlockFrontTemp);
    DEFINE_PARAM(int,SupportBlockRearTemp);
    DEFINE_PARAM(int,ExternalLiquidTemp);
    DEFINE_PARAM(int,LowThresholdTemp);
    DEFINE_PARAM(int,HighThresholdTemp);

    DEFINE_PARAM(int,BumperSensitivity);
    DEFINE_PARAM(int,BumperResetTime);
    DEFINE_PARAM(int,BumperImpactCounter);

    DEFINE_PARAM(unsigned long,SerialNumber);
    DEFINE_PARAM(int,TemperatureCompensation);
    DEFINE_PARAM(float,RefPointDistance);

    DEFINE_ARRAY_PARAM(int,15,ModelPotValuesByLayerThick);
    DEFINE_ARRAY_PARAM(int,15,SupportPotValuesByLayerThick);
    DEFINE_ARRAY_PARAM(int,8,PotentiometerSetValue);
    DEFINE_ARRAY_PARAM(int,96,NozzlePatternValue);


    // OCB Parameters
	// --------------
	DEFINE_ARRAY_PARAM(int, 6, ActiveThermistors); //S,M1,M2,M3,S_M3,M1_M2 , each thermistor receives 0/1 (not active)/(active)
	DEFINE_PARAM(int,Model1LevelThermistorLow);
	DEFINE_PARAM(int,Model1LevelThermistorHigh);
	DEFINE_PARAM(int,Model2LevelThermistorLow);
	DEFINE_PARAM(int,Model2LevelThermistorHigh);
	DEFINE_PARAM(int,Model3LevelThermistorLow);
	DEFINE_PARAM(int,Model3LevelThermistorHigh);
	DEFINE_PARAM(int,ModelLevelThermistorLow);
	DEFINE_PARAM(int,ModelLevelThermistorHigh);
    DEFINE_PARAM(int,TimePumpON);
    DEFINE_PARAM(int,TimePumpOFF);
    DEFINE_PARAM(int,TimeoutFilling);

	DEFINE_PARAM(int,SupportLevelThermistorLow);
	DEFINE_PARAM(int,SupportLevelThermistorHigh);
	DEFINE_PARAM(int,SLevelThermistorLow);
	DEFINE_PARAM(int,SLevelThermistorHigh);

    DEFINE_PARAM(int,PurgeTime);
    DEFINE_ARRAY_PARAM(int,2,RFReadersConnection);
	DEFINE_ARRAY_PARAM(int,2,RFReadersChannels);

	//SIMULATORS - parameters
    DEFINE_PARAM(bool,SimulatorMode);
    DEFINE_PARAM(int,FactorTimeBetween);
    DEFINE_PARAM(int,PingReplayInMS);
	DEFINE_PARAM(int,OCBWaitAckTimeout);
	DEFINE_PARAM(int,OHDBWaitAckTimeout);
};
#endif

