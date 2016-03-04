/*===========================================================================
 *   FILENAME       : Roller  {Roller.H}  
 *   PURPOSE        : Roller interface header file
 *   DATE CREATED   : 8/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _ROLLER_H_
#define _ROLLER_H_

#include "Define.h"

typedef enum {
	ROLLER_NO_ERROR = 0,
	ROLLER_OP_NOT_DONE,
	ROLLER_OP_FAIL
	}ROLLER_STATUS;



// Function Prototype 
// ====================

// Initailization of the roller module
// -----------------------------------                                        
void Roller_Init();

// Enable/disable the roller hardware driver
// -----------------------------------------
ROLLER_STATUS Roller_EnableDriver(BOOL Enable);

// Set the roller on or off
// ------------------------
ROLLER_STATUS Roller_SetOnOff(BOOL OnOff);

// Set the speed of the roller
// ---------------------------
ROLLER_STATUS Roller_SetSpeed(WORD Speed);

// Is the roller on or off?
// ------------------------
BOOL Roller_IsOn();

// Get the roller speed
// --------------------
ROLLER_STATUS Roller_GetSpeed();

// Is the 'SetOnOff' or the 'SetSpeed' operation done?
// ---------------------------------------------------
ROLLER_STATUS Roller_IsSetOperationDone();

// Is the 'IsOn' or the 'GetSpeed' operation done?
// -----------------------------------------------
ROLLER_STATUS Roller_IsGetOperationDone(WORD *Data);

#endif	


