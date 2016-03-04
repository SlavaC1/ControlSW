/*===========================================================================
 *   FILENAME       : A/D Driver {ADDRV.H}  
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     :  
 *   Procedure Name :
 *   Description    :
 *   Include File's :  Define.h,HardWareIO.h
 *===========================================================================*/

#ifndef _ADDRV_H_
#define _ADDRV_H_

#include "c8051F200.h"
#include "Define.h"

// Analog inputs
// =============
#define ANALOG_IN_1			0
#define ANALOG_IN_2			1
#define ANALOG_IN_3			2
#define ANALOG_IN_4			3
#define ANALOG_IN_5			4
#define ANALOG_IN_6			5
#define ANALOG_IN_7			6
#define ANALOG_IN_8			7
#define ANALOG_IN_9			8
#define ANALOG_IN_10		9
#define ANALOG_IN_11		10
#define ANALOG_IN_12		11
#define ANALOG_IN_13		12
#define ANALOG_IN_14		13
#define ANALOG_IN_15		14
#define ANALOG_IN_16		15
#define ANALOG_IN_17		16
#define ANALOG_IN_18		17
#define ANALOG_IN_19		18
#define ANALOG_IN_20		19
#define ANALOG_IN_21		20
#define ANALOG_IN_22		21
#define ANALOG_IN_23		22
#define ANALOG_IN_24		23


// Analog input select values to be written to AMX0SL
// ==================================================
#define SELECT_ANALOG_IN1				0x38
#define SELECT_ANALOG_IN2				0x39
#define SELECT_ANALOG_IN3				0x3a
#define SELECT_ANALOG_IN4		 		0x3b
#define SELECT_ANALOG_IN5				0x3c
#define SELECT_ANALOG_IN6				0x3d
#define SELECT_ANALOG_IN7		 		0x3e
#define SELECT_ANALOG_IN8				0x3f
#define SELECT_ANALOG_IN9				0x28
#define SELECT_ANALOG_IN10		 	0x29
#define SELECT_ANALOG_IN11			0x2a
#define SELECT_ANALOG_IN12			0x2b
#define SELECT_ANALOG_IN13			0x2c
#define SELECT_ANALOG_IN14			0x2d
#define SELECT_ANALOG_IN15			0x2e
#define SELECT_ANALOG_IN16			0x2f
#define SELECT_ANALOG_IN17			0x34
#define SELECT_ANALOG_IN18			0x35
#define SELECT_ANALOG_IN19			0x36
#define SELECT_ANALOG_IN20			0x37
#define SELECT_ANALOG_IN21			0x24
#define SELECT_ANALOG_IN22			0x25
#define SELECT_ANALOG_IN23			0x26
#define SELECT_ANALOG_IN24			0x27

#define NO_OF_ANALOG_INPUTS		24


// Values to be written to PxMODE registers for selecting pins for analog use
// ==========================================================================
#define SELECT_ANALOG_PINS_OF_PORT0		0x0f
#define SELECT_ANALOG_PINS_OF_PORT1		0x00
#define SELECT_ANALOG_PINS_OF_PORT2		0x0f
#define SELECT_ANALOG_PINS_OF_PORT3		0x00
	



#define A2D_ENABLE		0x80
#define A2D_INT_ENABLE	0x02
#define A2D_INT_DISABLE	0xfd
#define INTERNAL_VREF_SRC	0x03

// Function Prototype 
// ====================
void A2DInit (void);

WORD A2DGetReading(BYTE AnalogInNum);

WORD A2DGetReading_U3(BYTE AnalogInNum);

void A2DGetBlockReadings(BYTE FirstAnalogIn, LastAnalogIn, WORD *Buffer);

WORD *A2DGetBuffer();

unsigned int LittelBigConvert(unsigned int val);

#endif	
