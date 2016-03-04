/*===========================================================================
 *   FILENAME       : Actuators {Actuators.h}  
 *   PURPOSE        : Interface to some general actuators
 *   DATE CREATED   : 21/Nov/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _ACTUATORS_OHDB_H_
#define _ACTUATORS_OHDB_H_

#include "Define.h"

void InitCoolingFans();

void FansActivationTask(BYTE Arg);

// Turn on/off the material cooling fans
void SetMaterialCoolingFansOnOff(BOOL OnOff, BYTE OnPeriod);

#endif