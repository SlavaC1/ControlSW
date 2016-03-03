/*===========================================================================
 *   FILENAME       : ThermOpMode.h
 *   PURPOSE        : Control of the thermistor modes (for thermistor health)
 *   DATE CREATED   : 08/09/2014
 *   PROGRAMMER     : Iddan Kalo
 *===========================================================================*/


#ifndef _THERM_OP_MODE_H_
#define _THERM_OP_MODE_H_

#include "Define.h"

/* FPGA defs */
/* reg values (masks) */
#define LO_THERMISTORS_ON	   0x01
#define HI_THERMISTORS_ON      0x02
#define UNUSED_THERMISTOR_MODE 0x00

/* reg addresses */
#define THERMISTORS_SET_OP_MODE_ADDRESS 0x50

BYTE ThermOpModeId2Mask(BYTE OpModeId) /* if this is used in more places, move it to a .c file */
{
    static BYTE ThermistorOpModeBitMaskTable[] =  /* see TThermistorsOperationMode in GlobalDefs.h (EM) */
    { /* 0 HIGH_THERMISTORS_MODE                   */ LO_THERMISTORS_ON|HI_THERMISTORS_ON, /* DM6v2 */
      /* 1 LOW_THERMISTORS_MODE                    */ LO_THERMISTORS_ON,   /* DM6v2 */
      /* 2 SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE */ UNUSED_THERMISTOR_MODE,
      /* 3 SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE */ UNUSED_THERMISTOR_MODE,
      /* 4 HIGH_AND_LOW_THERMISTORS_MODE           */ LO_THERMISTORS_ON|HI_THERMISTORS_ON }; /* DM6v2 */

    if (OpModeId >= sizeof(ThermistorOpModeBitMaskTable)/sizeof(ThermistorOpModeBitMaskTable[0]))
    {
        return UNUSED_THERMISTOR_MODE;
    }

    return ThermistorOpModeBitMaskTable[OpModeId];
}

#endif /*_THERM_OP_MODE_H_*/

