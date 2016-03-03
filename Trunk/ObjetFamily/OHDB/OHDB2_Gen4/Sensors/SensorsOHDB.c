/*===========================================================================
 *   FILENAME       : Sensors {Sensors.c}  
 *   PURPOSE        : Handles the material level sensors, the vacuum sensor,
 *										and the ambient temperature sensor 
 *   DATE CREATED   : 24/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
#include "SensorsOHDB.h"
#include "Spi_A2D.h"
#include "A2D.h"
#include "HeadsVoltageReader.h"
#include "MsgDecodeOHDB.h"
#ifndef SIM_GEN4
#include "SpiExtA2D.h"
#endif


// Constants
// =========
// Mapping of the sensors inputs to the analog inputs
// --------------------------------------------------

// Starting with a left most chamber (support)
#define M1_THERM_LEVEL_SENSOR      ANALOG_IN_13 + SIM_OHDB_OFFSET //M1 thermistor
#define M2_THERM_LEVEL_SENSOR      ANALOG_IN_14	+ SIM_OHDB_OFFSET //M2 thermistor
#define M3_THERM_LEVEL_SENSOR      ANALOG_IN_15	+ SIM_OHDB_OFFSET //M3 thermistor
#define M4_THERM_LEVEL_SENSOR      ANALOG_IN_16	+ SIM_OHDB_OFFSET //support thermistor
#define M5_THERM_LEVEL_SENSOR      ANALOG_IN_17 + SIM_OHDB_OFFSET
#define M6_THERM_LEVEL_SENSOR      ANALOG_IN_18 + SIM_OHDB_OFFSET
#define M7_THERM_LEVEL_SENSOR      ANALOG_IN_19	+ SIM_OHDB_OFFSET
#define SUPPORT_THERM_LEVEL_SENSOR ANALOG_IN_20 + SIM_OHDB_OFFSET 

#ifdef SIM_GEN4
	#define S_M7_THERM_LEVEL_SENSOR    ANALOG_IN_26 + SIM_OHDB_OFFSET 
	#define M1_M2_THERM_LEVEL_SENSOR   ANALOG_IN_27	+ SIM_OHDB_OFFSET 
	#define M3_M4_THERM_LEVEL_SENSOR   ANALOG_IN_28	+ SIM_OHDB_OFFSET 
	#define M5_M6_THERM_LEVEL_SENSOR   ANALOG_IN_29 + SIM_OHDB_OFFSET 
#else
#define S_M7_THERM_LEVEL_SENSOR    EXT_A2D_CH4 + SIM_OHDB_OFFSET // It reads from LTC1863 unit through SPI in OHDB2 rev C (SpiExtA2D module)
#define M1_M2_THERM_LEVEL_SENSOR   EXT_A2D_CH7 + SIM_OHDB_OFFSET 
#define M3_M4_THERM_LEVEL_SENSOR   EXT_A2D_CH6 + SIM_OHDB_OFFSET 
#define M5_M6_THERM_LEVEL_SENSOR   EXT_A2D_CH5 + SIM_OHDB_OFFSET 		
#endif
 	
#define VACUUM_SENSOR		 ANALOG_IN_21           + SIM_OHDB_OFFSET						   		
#define AMBIENT_TEMP_SENSOR	 CPU_TEMPERATURE_SENSOR /*+ SIM_OHDB_OFFSET   - because of the manipulations in HeadsVoltagesReader*/

#define V_40_VPP_SENSOR CPU_ANALOG_IN_1
#define V_24_SENSOR     CPU_ANALOG_IN_2
#define V_12_VDD_SENSOR CPU_ANALOG_IN_3
#define V_5_VCC_SENSOR  CPU_ANALOG_IN_4
#define V_3_3_SENSOR    CPU_ANALOG_IN_5
#define V_1_2_SENSOR    CPU_ANALOG_IN_6

// Thermistors mapping for Gen4 block
const BYTE xdata ThermistorLevelTable[NUM_OF_THERMISTORS] = 
{
	SUPPORT_THERM_LEVEL_SENSOR,			
	M1_THERM_LEVEL_SENSOR,
	M2_THERM_LEVEL_SENSOR,		
	M3_THERM_LEVEL_SENSOR,			
	M4_THERM_LEVEL_SENSOR,											 			
	M5_THERM_LEVEL_SENSOR,
	M6_THERM_LEVEL_SENSOR,
	M7_THERM_LEVEL_SENSOR,
	S_M7_THERM_LEVEL_SENSOR,
	M1_M2_THERM_LEVEL_SENSOR,
	M3_M4_THERM_LEVEL_SENSOR,
	M5_M6_THERM_LEVEL_SENSOR																		 																		 											 
};

// Type definitions
// ================
typedef struct
{
	WORD HighLimit;
	WORD LowLimit;
}TVacuumParams;
	
 
// Local routines
// ==============


// Module variables
// ================
TVacuumParams xdata VacuumParams;



/****************************************************************************
 *
 *  NAME        :  Sensors_GetMaterialLevelSensors
 *
 *
 *  DESCRIPTION :  Get the current readings of the material level sensors                                        
 *
 ****************************************************************************/
void Sensors_GetMaterialLevelSensors(WORD ThermistorLevelArr[])
{
   	BYTE idata i = 0;
#ifndef SIM_GEN4	
	for(; i < NUM_OF_THERMISTORS; i++)
	{
		if(i < NUM_OF_LOWER_THERMISTORS)
			ThermistorLevelArr[i] = SpiA2D_GetReading(ThermistorLevelTable[i]);
		else
			ThermistorLevelArr[i] = SpiExtA2D_GetReading(ThermistorLevelTable[i]);
	}
#else
	for(; i < NUM_OF_THERMISTORS; i++)
	{
		ThermistorLevelArr[i] = SpiA2D_GetReading(ThermistorLevelTable[i]);
	}
#endif
} 


/****************************************************************************
 *
 *  NAME        :  Sensors_SetHeadsVacuumParams
 *
 *
 *  DESCRIPTION :  Set the high and low limits for the heads vacuum                                         
 *
 ****************************************************************************/
void Sensors_SetHeadsVacuumParams(WORD *Params)
{
	TVacuumParams xdata *SetParams = (TVacuumParams *) Params;

	VacuumParams.HighLimit = SetParams->HighLimit;
	VacuumParams.LowLimit = SetParams->LowLimit;
}


/****************************************************************************
 *
 *  NAME        :  Sensors_IsHeadsVacuumOk
 *
 *
 *  DESCRIPTION : Check if the current readings of the vacuum sensors are
 *								in the requested range that was	set by 
 *								'Sensors_SetHeadsVacuumParams'                                        
 *
 ****************************************************************************/
BOOL Sensors_IsHeadsVacuumOk()
{
	WORD VacuumValue;

	VacuumValue = SpiA2D_GetReading(VACUUM_SENSOR);
	if (VacuumValue >= VacuumParams.LowLimit &&	VacuumValue <= VacuumParams.HighLimit)
  		return TRUE;
	
	return FALSE;
}


/****************************************************************************
 *
 *  NAME        : Sensors_GetVacuumSensor 
 *
 *
 *  DESCRIPTION : Get the current readings of the heads vacuum sensor                                        
 *
 ****************************************************************************/
void Sensors_GetVacuumSensor(WORD *VacuumSensors)
{
	*VacuumSensors = SpiA2D_GetReading(VACUUM_SENSOR); 
}


/****************************************************************************
 *
 *  NAME        : Sensors_GetAmbientTempSensor 
 *
 *
 *  DESCRIPTION : Get the current reading of the ambient temperature sensor                                         
 *
 ****************************************************************************/
WORD Sensors_GetAmbientTempSensor()
{
	return A2D0Reader_GetReading(AMBIENT_TEMP_SENSOR);
}


/****************************************************************************
 *
 *  NAME        : Sensors_GetPowerSuppliesVoltages 
 *
 *  DESCRIPTION : Get the current power supplies voltages (in A/D units)                                         
 *
 ****************************************************************************/
void Sensors_GetPowerSuppliesVoltages(WORD* Voltages)
{
	Voltages[0] = A2D2_GetReading(V_40_VPP_SENSOR);
	Voltages[1] = A2D2_GetReading(V_24_SENSOR);
	Voltages[2] = A2D2_GetReading(V_12_VDD_SENSOR);
	Voltages[3] = A2D2_GetReading(V_5_VCC_SENSOR);
	Voltages[4] = A2D2_GetReading(V_3_3_SENSOR);
	Voltages[5] = A2D2_GetReading(V_1_2_SENSOR);	
}

