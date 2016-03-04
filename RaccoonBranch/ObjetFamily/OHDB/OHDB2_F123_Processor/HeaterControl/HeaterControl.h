/*===========================================================================
 *   FILENAME       : Heater control unit  {HeaterControl.H}  
 *   PURPOSE        : Heater control unit header file
 *   DATE CREATED   : 4/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _HEATER_CONTROL_H_
#define _HEATER_CONTROL_H_


#include "Spi_A2D.h"
#include "MiniScheduler.h"

// Mapping of the heater control inputs to the analog inputs
// ---------------------------------------------------------
#define HEAD_1_TEMP		  ANALOG_IN_1  + SIM_OHDB_OFFSET
#define HEAD_2_TEMP		  ANALOG_IN_2  + SIM_OHDB_OFFSET
#define HEAD_3_TEMP		  ANALOG_IN_3  + SIM_OHDB_OFFSET
#define HEAD_4_TEMP		  ANALOG_IN_4  + SIM_OHDB_OFFSET
#define HEAD_5_TEMP		  ANALOG_IN_5  + SIM_OHDB_OFFSET
#define HEAD_6_TEMP		  ANALOG_IN_6  + SIM_OHDB_OFFSET
#define HEAD_7_TEMP		  ANALOG_IN_7  + SIM_OHDB_OFFSET
#define HEAD_8_TEMP		  ANALOG_IN_8  + SIM_OHDB_OFFSET
#define HEAD_BLOCK_1_TEMP ANALOG_IN_9  + SIM_OHDB_OFFSET
#define HEAD_BLOCK_2_TEMP ANALOG_IN_10 + SIM_OHDB_OFFSET
#define HEAD_BLOCK_3_TEMP ANALOG_IN_11 + SIM_OHDB_OFFSET
#define HEAD_BLOCK_4_TEMP ANALOG_IN_12 + SIM_OHDB_OFFSET

#ifndef OBJET_MACHINE
	#define EXTERNAL_LIQUID_TEMP ANALOG_IN_22
#endif

// Head heaters numbers
// --------------------
#define MODEL_HEAD_1_HEATER		0		
#define MODEL_HEAD_2_HEATER		1
#define MODEL_HEAD_3_HEATER		2
#define MODEL_HEAD_4_HEATER		3
#define SUPPORT_HEAD_1_HEATER	4
#define SUPPORT_HEAD_2_HEATER	5
#define SUPPORT_HEAD_3_HEATER	6
#define SUPPORT_HEAD_4_HEATER	7
#define FRONT_PRE_HEATER		8
#define REAR_PRE_HEATER			9
#define REAR_BLOCK_HEATER		10
#define FRONT_BLOCK_HEATER		11


typedef enum{
  TEMPERATURE_TOO_HIGH = 1,
  TEMPERATURE_TOO_LOW = 2
}TTemperatureErrDescription;

// Function Prototype 
// ====================

// Initializing the heater control unit
// ------------------------------------
void HeaterControlInit();

// Set the heater control to on/off
// --------------------------------
void HeaterControlSetOnOff(BOOL OnOff, WORD HeatersMask);

// Turn all head and perimeter heaters off
// ---------------------------------------
void HeaterTurnAllHeadHeaterOff();

// Set the state of an heater to on or off. (This function does not turn the 
// heater on/off it only set a bit in the heaters status word)
// -----------------------------------------------------------
void HeaterSetHeaterState(BYTE HeaterNum,BOOL On);


// Turn the heaters on or off according to their representation
// in the heaters status word
// --------------------------
void TurnHeatersOnOff();

// Set a heater control requested temperature
// -----------------------------------------
void HeaterSetTemperature(BYTE HeaterNum, WORD Temperature);

// Get the current temperatures
// ----------------------------
void HeaterGetCurrentTemperatures(WORD *Temperatures);

// Set heater control requested temperatures for the requested heaters
// -------------------------------------------------------------------
void HeaterSetTemperatures(BYTE FirstHeaterNum, BYTE LastHeaterNum, WORD *Temperatures, WORD LowThreshold, WORD HighThreshold);

// Set heater control requested standby temperatures for the requested heaters
// ---------------------------------------------------------------------------
void HeaterSetStandbyTemperatures(BYTE FirstHeaterNum, BYTE LastHeaterNum, WORD *Temperatures);

// Get the standby temperatures
// ----------------------------
void HeaterGetStandbyTemperatures(WORD *Temperatures);

// This task checks if the status of the heater control has 
//	changed to false, and if so turn off all the heaters 
// -----------------------------------------------------
void HeaterStatusTask(BYTE Arg);

// This task controls the heating of the heads and the 
// perimeter heaters according to their recent temperatures
// --------------------------------------------------------
void HeaterControlTask(BYTE Arg);


// Set the handles to the modul tasks
// ----------------------------------
void HeaterSetTasksHandles(TTaskHandle StatusTask, TTaskHandle ControlTask);


// Returns true if the tempratures of all the heaters are as requested 
// -------------------------------------------------------------------
BOOL HeaterIsTempraturesOK(BYTE* HeadNum, WORD* Temperature, TTemperatureErrDescription* ErrDescription);



#endif	
