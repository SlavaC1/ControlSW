/*===========================================================================
 *   FILENAME       : HeadsFilling {HeadsFilling.h}  
 *   PURPOSE        : Control of the filling of the model and support liquids 
 *   DATE CREATED   : 03/08/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _HEADS_FILLING_H_
#define _HEADS_FILLING_H_

#include "Define.h"
#include "TimerDrv.h"

#ifdef OCB_SIMULATOR //for TTemperatureErrDescription definition collapse
	#include "HeaterControl.h"
#endif

typedef enum
{
	NO_ERROR,
	HEADS_TEMPERATURE_TOO_HIGH,
	HEADS_TEMPERATURE_TOO_LOW,
	SUPPORT_FILLING_TIMEOUT,
	M1_FILLING_TIMEOUT,
	M2_FILLING_TIMEOUT,
	M3_FILLING_TIMEOUT,
	M4_FILLING_TIMEOUT,
	M5_FILLING_TIMEOUT,
	M6_FILLING_TIMEOUT,
	M7_FILLING_TIMEOUT,
	SUPPORT_M7_FILLING_TIMEOUT,
	M1_M2_FILLING_TIMEOUT,
	M3_M4_FILLING_TIMEOUT,
	M5_M6_FILLING_TIMEOUT
}THeadsFillingError;



#ifndef OCB_SIMULATOR
typedef enum
{
  TEMPERATURE_TOO_HIGH = 1,
  TEMPERATURE_TOO_LOW  = 2
}TTemperatureErrDescription;
#endif

// Constants
typedef enum
{
	MATERIAL_LEVEL_LOW,
	MATERIAL_LEVEL_IN_RANGE,
	MATERIAL_LEVEL_HIGH
}TMaterialLevel;

typedef struct
{
  BYTE 		    	 PumpID;
  BYTE          	 ChamberType; 
  BYTE          	 RelatedTank; // The Tank associated with this Chamber.
  WORD 		    	 PumpOnTime;
  WORD 		    	 PumpOffTime;
  WORD 		    	 FillingTimeout;
  WORD 		    	 LowThreshold;
  WORD 		    	 HighThreshold;
  WORD 		    	 LiquidLevel;
  BOOL 		    	 CheckTimeout;
  TIMER_struct  	 PumpTimer;
  TIMER_struct   	 FillingTimer;
  BOOL  			 FillingOn;
  BOOL  			 FillingTaskStartStop;
  BOOL  			 IsInStandbyState;
  THeadsFillingError FillingTimeoutErrCode;
  BYTE  		     ActiveThermistor;
}ChamberDescriptor;


void HeadsFillingInit();

void HeadsFillingSetParameters  (WORD MaterialLowTheresholdArr[], WORD MaterialHighTheresholdArr[], WORD TimePumpOn, WORD TimePumpOff, WORD Timeout, BYTE ActiveThermistorsArr[]);
void HeadsFillingSetLiquidsLevel(WORD ThermistorLevelArr[]);
void HeadsFillingGetLiquidsLevel(WORD ThermistorLevelArr[]);
void HeadFillingGetChamberTank  (BYTE ChambersTankArr[]);
BOOL HeadFillingSetChamberTank  (BYTE ChambersTankArr[]);

void HeadsFillingSetOnOff(BOOL On);

BOOL HeadsFillingGetStatus();

THeadsFillingError HeadsFillingGetLastError();

BOOL HeadsFillingAreHeadsFilled();								  

void HeadsFillingSetHeadsTemperatureStatus(BOOL Status,TTemperatureErrDescription ErrDescription);
BOOL HeadsFillingGetHeadsTemperatureStatus();					  

TMaterialLevel GetLiquidLevelStatus(ChamberDescriptor* ChamberDesc);

void FillingStateMachine(BYTE Arg, ChamberDescriptor* ChamberDesc);

#endif