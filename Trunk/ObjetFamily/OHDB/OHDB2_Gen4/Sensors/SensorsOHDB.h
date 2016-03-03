/*===========================================================================
 *   FILENAME       : Sensors  {Sensors.H}  
 *   PURPOSE        : Sensors handler header file
 *   DATE CREATED   : 24/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _SENSORS_OHDB_H_
#define _SENSORS_OHDB_H_

#include "Define.h"

#define NUM_OF_THERMISTORS       12
#define NUM_OF_UPPER_THERMISTORS 4
#define NUM_OF_LOWER_THERMISTORS 8


// Function Prototype 
// ====================

// Get the current readings of the material level sensors
// ------------------------------------------------------
void Sensors_GetMaterialLevelSensors(WORD *MaterialLevels);

// Set the high and low limits for the heads vacuum
// ------------------------------------------------
void Sensors_SetHeadsVacuumParams(WORD *VacuumParams);

// Check if the current readings of the vacuum sensors are
// in the requested range that was	set by 'Sensors_SetHeadsVacuumParams'
// ----------------------------------------------------------------------
BOOL Sensors_IsHeadsVacuumOk();

// Get the current readings of the heads vacuum sensor
// ---------------------------------------------------
void Sensors_GetVacuumSensor(WORD *VacuumSensors);

// Get the current readings of the ambient temperature sensor
// ----------------------------------------------------------
WORD Sensors_GetAmbientTempSensor();

// Get the current power supplies voltages (in A/D units)                                         
// ------------------------------------------------------
void Sensors_GetPowerSuppliesVoltages(WORD* Voltages);


#endif	

