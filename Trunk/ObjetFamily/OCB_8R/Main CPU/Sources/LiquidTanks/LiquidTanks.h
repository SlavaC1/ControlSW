/*===========================================================================
 *   FILENAME       : LiquidTanks {LiquidTanks.h}  
 *   PURPOSE        : Liquid tanks wieght and existence monitoring (header file)
 *   DATE CREATED   : 25/07/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _LIQUID_TANKS_H_
#define _LIQUID_TANKS_H_

#include "define.h"


typedef enum
{
	MODEL_TANK_1 = 0,
	MODEL_TANK_2,
	MODEL_TANK_3,
	MODEL_TANK_4,
	MODEL_TANK_5,
	MODEL_TANK_6,
	MODEL_TANK_7,
	MODEL_TANK_8,
	MODEL_TANK_9,
	MODEL_TANK_10,
	MODEL_TANK_11,
	MODEL_TANK_12,
	MODEL_TANK_13,
	MODEL_TANK_14,
	SUPPORT_TANK_1,
	SUPPORT_TANK_2,
	WASTE_TANK_1,
	WASTE_TANK_2,	 	
	// Keep this one last
	NUM_OF_LIQUID_TANKS,
	NUM_OF_TANK_SENSORS = WASTE_TANK_1
}TLiquidTanksID;


// Initialization of the liquid tanks
void LiquidTanksInit();


// Get the average weight of the liquid tanks
void LiquidTanksGetAverageWeights(float *TanksWeights);

// Get the status (tank inserted/out) of the tanks (not including the waste tank)
void LiquidTanksGetStatus(BOOL *TanksInserted);

#endif
