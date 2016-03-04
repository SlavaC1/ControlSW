/*===========================================================================
 *   FILENAME       : LiquidTanks {LiquidTanks.h}  
 *   PURPOSE        : Liquid tanks wieght and existence monitoring (header file)
 *   DATE CREATED   : 25/07/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _LIQUID_TANKS_H_
#define _LIQUID_TANKS_H_

#include "define.h"


typedef enum{
  MODEL_TANK_1 = 0,
  MODEL_TANK_2,
  MODEL_TANK_3,

  MODEL_TANK_4,
  MODEL_TANK_5,
  MODEL_TANK_6,

  SUPPORT_TANK_1,
  SUPPORT_TANK_2,
#ifndef OBJET_MACHINE
  SUPPORT_TANK_3,
#endif
  WASTE_TANK,

  // Keep this one last
  NUM_OF_LIQUID_TANKS
}TLiquidTanksID;


// Initialization of the liquid tanks
void LiquidTanksInit();


// Get the average weight of the liquid tanks
void LiquidTanksGetAverageWeights(float *TanksWeights);

// Get the status (tank inserted/out) of the tanks (not including the waste tank)
void LiquidTanksGetStatus(BOOL *TanksInserted);

#endif
