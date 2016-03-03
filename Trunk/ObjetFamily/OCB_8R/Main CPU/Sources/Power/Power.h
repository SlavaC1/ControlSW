/*===========================================================================
 *   FILENAME       : Power {Power.h}  
 *   PURPOSE        : 
 *   DATE CREATED   : 22/07/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _POWER_H_
#define _POWER_H_

#include "Define.h"



void PowerInit();

// Set the power parameters (currently not used)
void PowerSetParameters(BYTE PowerOnDelay, BYTE PowerOffDelay);

// Turn the power on/off
BOOL PowerTurnOnOff(BOOL OnOff);

// Get the power status
BOOL PowerGetStatus();


#endif