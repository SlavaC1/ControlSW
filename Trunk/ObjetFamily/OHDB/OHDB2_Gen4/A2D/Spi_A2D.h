/*===========================================================================
 *   FILENAME       : SPI A2D {Spi_A2D.h}  
 *   PURPOSE        : Gets the A/D values from the A2D processor by the SPI (header file) 
 *   DATE CREATED   : 29/4/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#ifndef _SPI_A2D_H_
#define _SPI_A2D_H_


#include "c8051F120.h"

// E2PROM Errors
// -------------
typedef enum 
{
	A2D_NO_ERROR = 0,
	A2D_SEND_SUCCESS,
	A2D_SEND_FAILED,
	A2D_WRITE_BLOCK_ERORR,
	A2D_BUSY
}SPI_A2D_STATUS;

#define SIM_OHDB_OFFSET 0

// Analog inputs
// =============
#define ANALOG_IN_1	 0
#define ANALOG_IN_2	 1
#define ANALOG_IN_3	 2
#define ANALOG_IN_4	 3
#define ANALOG_IN_5	 4
#define ANALOG_IN_6	 5
#define ANALOG_IN_7	 6
#define ANALOG_IN_8	 7
#define ANALOG_IN_9	 8
#define ANALOG_IN_10 9
#define ANALOG_IN_11 10
#define ANALOG_IN_12 11
#define ANALOG_IN_13 12
#define ANALOG_IN_14 13
#define ANALOG_IN_15 14
#define ANALOG_IN_16 15
#define ANALOG_IN_17 16
#define ANALOG_IN_18 17
#define ANALOG_IN_19 18
#define ANALOG_IN_20 19
#define ANALOG_IN_21 20
#define ANALOG_IN_22 21
#define ANALOG_IN_23 22
#define ANALOG_IN_24 23


#define NO_OF_ANALOG_INPUTS	24


// Function Prototype 
// ====================
void SpiA2D_Init (void);

WORD SpiA2D_GetReading(BYTE AnalogInNum);

void SpiA2D_GetBlockReadings(BYTE FirstAnalogIn, LastAnalogIn, WORD *Buffer);

SPI_A2D_STATUS SpiA2D_GetValuesBySpi();

SPI_A2D_STATUS SpiA2D_GetValuesCMDStatus();
SPI_A2D_STATUS SpiA2D_GetVersionCMDStatus();

void SpiA2D_GetSW_Version(BYTE* ExternalNum, BYTE* InternalNum);

// The A2D task for getting the current A2D reading from the A2D processor
void SpiA2D_Task(BYTE Arg);

BOOL SpiA2D_IsDataValid();
BOOL AreRetriesNeeded_GetSwVer();

#endif	
