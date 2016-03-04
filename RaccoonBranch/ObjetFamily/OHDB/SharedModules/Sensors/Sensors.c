/*===========================================================================
 *   FILENAME       : Sensors {Sensors.c}  
 *   PURPOSE        : Handles the material level sensors, the vacuum sensor,
 *										and the ambient temperature sensor 
 *   DATE CREATED   : 24/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
#include "Sensors.h"
#include "Spi_A2D.h"
#include "A2D.h"


// Constants
// =========
// Mapping of the sensors inputs to the analog inputs
// --------------------------------------------------
#define MODEL_FULL_SENSOR			  ANALOG_IN_13
#define MODEL_H_FULL_SENSOR			ANALOG_IN_14
#define SUPPORT_FULL_SENSOR			ANALOG_IN_15
#define SUPPORT_H_FULL_SENSOR		ANALOG_IN_16	
#define VACUUM_SENSOR				    ANALOG_IN_17		
#define AMBIENT_TEMP_SENSOR			CPU_TEMPERATURE_SENSOR



// Type definitions
// ================
typedef struct{
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
void Sensors_GetMaterialLevelSensors(WORD *MaterialLevels)
{
	SpiA2D_GetBlockReadings(MODEL_FULL_SENSOR, SUPPORT_H_FULL_SENSOR, MaterialLevels);
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
 *  DESCRIPTION : Get the current readings of the ambient temperature sensor                                         
 *
 ****************************************************************************/
WORD Sensors_GetAmbientTempSensor()
{
	return A2D_GetReading(AMBIENT_TEMP_SENSOR);
}

