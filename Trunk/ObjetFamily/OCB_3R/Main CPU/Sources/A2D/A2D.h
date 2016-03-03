/*===========================================================================
 *   FILENAME       : A2D {A2D.h}  
 *   PURPOSE        : A/D driver (header file) 
 *   DATE CREATED   : 29/4/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#ifndef _A2D_H_
#define _A2D_H_
#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#elif defined OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
#include "Define.h"

// Analog inputs
// =============
#define CPU_ANALOG_IN_1						0
#define CPU_ANALOG_IN_2						1
#define CPU_ANALOG_IN_3						2
#define CPU_ANALOG_IN_4						3
#define CPU_ANALOG_IN_5						4
#define CPU_ANALOG_IN_6						5
#define CPU_ANALOG_IN_7						6
#define CPU_ANALOG_IN_8						7
#define CPU_TEMPERATURE_SENSOR		8


// Analog input select values to be written to AMX0SL
// ==================================================
#define SELECT_CPU_ANALOG_IN1				0x00
#define SELECT_CPU_ANALOG_IN2				0x01
#define SELECT_CPU_ANALOG_IN3				0x02
#define SELECT_CPU_ANALOG_IN4				0x03
#define SELECT_CPU_ANALOG_IN5				0x04
#define SELECT_CPU_ANALOG_IN6				0x05
#define SELECT_CPU_ANALOG_IN7				0x06
#define SELECT_CPU_ANALOG_IN8				0x07
#define SELECT_CPU_TEMP_SENSOR			0x08

	

#define NO_OF_CPU_ANALOG_INPUTS		9  
#define A2D_ENABLE		0x80
#define A2D_INT_ENABLE	0x02
#define A2D_INT_DISABLE	0xfd

// Function Prototype 
// ====================
void A2D_Init (void);



void A2D_GetBlockReadings(BYTE FirstAnalogIn, BYTE LastAnalogIn, WORD *Buffer);
#if defined DEBUG   || defined OCB_SIMULATOR
WORD A2D_GetReading(BYTE AnalogInNum);
WORD *A2D_GetBuffer();
#endif

#endif	
