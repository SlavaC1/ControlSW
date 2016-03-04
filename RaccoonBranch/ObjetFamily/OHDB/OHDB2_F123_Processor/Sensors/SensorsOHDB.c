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
#include "MsgDecodeOHDB.h"


// Constants
// =========
// Mapping of the sensors inputs to the analog inputs
// --------------------------------------------------

#define M1_THERMISTOR_LEVEL_SENSOR		   ANALOG_IN_13           + SIM_OHDB_OFFSET //M1 thermistor
#define M2_THERMISTOR_LEVEL_SENSOR		   ANALOG_IN_14           + SIM_OHDB_OFFSET //M2 thermistor
#define M3_THERMISTOR_LEVEL_SENSOR		   ANALOG_IN_15           + SIM_OHDB_OFFSET //M3 thermistor
#define SUPPORT_THERMISTOR_LEVEL_SENSOR	   ANALOG_IN_16           + SIM_OHDB_OFFSET //Support thermistor
#define M1_M2_THERMISTOR_LEVEL_SENSOR      ANALOG_IN_18           + SIM_OHDB_OFFSET //M1_M2 thermistor
#define SUPPORT_M3_THERMISTOR_LEVEL_SENSOR ANALOG_IN_17           + SIM_OHDB_OFFSET //Support_M3 thermistor 	
#define VACUUM_SENSOR				       ANALOG_IN_21           + SIM_OHDB_OFFSET						   		
#define AMBIENT_TEMP_SENSOR				   CPU_TEMPERATURE_SENSOR + SIM_OHDB_OFFSET

#define V_40_VPP_SENSOR CPU_ANALOG_IN_1
#define V_24_SENSOR     CPU_ANALOG_IN_2
#define V_12_VDD_SENSOR CPU_ANALOG_IN_3
#define V_5_VCC_SENSOR  CPU_ANALOG_IN_4
#define V_3_3_SENSOR    CPU_ANALOG_IN_7
#define V_1_2_SENSOR    CPU_ANALOG_IN_8



const BYTE xdata ThermistorLevelTable[NUM_OF_THERMISTORS] = 
{
	SUPPORT_THERMISTOR_LEVEL_SENSOR,			
	M1_THERMISTOR_LEVEL_SENSOR,
	M2_THERMISTOR_LEVEL_SENSOR,		
	M3_THERMISTOR_LEVEL_SENSOR,			
	SUPPORT_M3_THERMISTOR_LEVEL_SENSOR,											 			
	M1_M2_THERMISTOR_LEVEL_SENSOR																		 																		 											 
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
/*void Sensors_GetMaterialLevelSensors(WORD *MaterialLevels)
{
	SpiA2D_GetBlockReadings(MODEL_FULL_SENSOR, SUPPORT_H_FULL_SENSOR, MaterialLevels);
} */

void Sensors_GetMaterialLevelSensors(WORD ThermistorLevelArr[])
{
   	int i=0;
	for(;i<NUM_OF_THERMISTORS;i++)
		ThermistorLevelArr[i] = SpiA2D_GetReading(ThermistorLevelTable[i]);
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
	return A2D0_GetReading(AMBIENT_TEMP_SENSOR);
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
}

