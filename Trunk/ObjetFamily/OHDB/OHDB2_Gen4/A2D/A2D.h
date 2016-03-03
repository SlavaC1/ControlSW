/*===========================================================================
 *   FILENAME       : A2D {A2D.h}  
 *   PURPOSE        : A/D driver (header file) 
 *   DATE CREATED   : 29/4/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#ifndef _A2D_H_
#define _A2D_H_

#include "c8051F120.h"
#include "Define.h"

// Analog inputs
// =============
#define CPU_ANALOG_IN_1		   0
#define CPU_ANALOG_IN_2		   1
#define CPU_ANALOG_IN_3		   2
#define CPU_ANALOG_IN_4		   3
#define CPU_ANALOG_IN_5		   4
#define CPU_ANALOG_IN_6		   5
#define CPU_ANALOG_IN_7		   6
#define CPU_ANALOG_IN_8		   7
#define CPU_TEMPERATURE_SENSOR 8


// Analog input select values to be written to AMX0SL
// ==================================================
#define SELECT_CPU_AD0_ANALOG_IN1  0x01
#define SELECT_CPU_AD0_ANALOG_IN2  0x01
#define SELECT_CPU_AD0_ANALOG_IN3  0x03
#define SELECT_CPU_AD0_ANALOG_IN4  0x03
#define SELECT_CPU_AD0_ANALOG_IN5  0x05
#define SELECT_CPU_AD0_ANALOG_IN6  0x05
#define SELECT_CPU_AD0_ANALOG_IN7  0x07
#define SELECT_CPU_AD0_ANALOG_IN8  0x07
#define SELECT_CPU_AD0_TEMP_SENSOR 0x08	


// Analog input select values to be written to AMX2SL
// ==================================================
#define SELECT_CPU_AD2_ANALOG_IN1  0x00
#define SELECT_CPU_AD2_ANALOG_IN2  0x01
#define SELECT_CPU_AD2_ANALOG_IN3  0x02
#define SELECT_CPU_AD2_ANALOG_IN4  0x03
#define SELECT_CPU_AD2_ANALOG_IN5  0x04
#define SELECT_CPU_AD2_ANALOG_IN6  0x05
#define SELECT_CPU_AD2_ANALOG_IN7  0x06
#define SELECT_CPU_AD2_ANALOG_IN8  0x07

#define NO_OF_CPU_ANALOG_INPUTS	9
#define NO_OF_P1_ANALOG_INPUTS  8 
  
#define A2D_ENABLE		 0x80
#define A2D0_INT_ENABLE	 0x02
#define A2D0_INT_DISABLE 0xFD

#define A2D2_INT_ENABLE	 0x10
#define A2D2_INT_DISABLE 0xEF

// Function Prototype 
// ====================
void A2D_Init(void);

void A2D0_Init();
void A2D2_Init();

WORD A2D0_GetReading(BYTE AnalogInNum);
WORD A2D2_GetReading(BYTE AnalogInNum);

// Select the next analog input to be sampled 
void A2D0_SelectNextInput();
void A2D2_SelectNextInput();

bit  IsA2D0BufferReady();
void SetA2D0BufferReadiness(bit Status);
BYTE GetA2D0SelectedInput();

#endif	
