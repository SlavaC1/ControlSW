/*===========================================================================
 *   FILENAME       : Bumper {Bumper.h}  
 *   PURPOSE        : Bumper module header file
 *   DATE CREATED   : 17/8/2003
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifndef _BUMPER_H_
#define _BUMPER_H_

#include "Define.h"

// Initialization of the bumper
void Bumper_Init();

// Set the bumper parameters (sensitivity, reset time, impact count)
void Bumper_SetParameters(WORD Sensitivity, WORD ResetTime, BYTE ImpactCount);  

// Set the bumper impact detection mechanism on/off
void Bumper_SetOnOff(BOOL OnOff);



#endif													