/*===========================================================================
 *   FILENAME       : A/D  {A2D.C}  
 *   PURPOSE        : A/D driver module  
 *   DATE CREATED   : 11/07/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include <string.h>
#include "Define.h"
#include "A2D.h"
#include "TimerDrv.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"  
#include "Spi_A2D.h"

// Constants
// =========
static const BYTE xdata A2DSelectAnalogInputArray[NO_OF_CPU_ANALOG_INPUTS] = {
									 SELECT_CPU_ANALOG_IN1,			
									 SELECT_CPU_ANALOG_IN2,			
									 SELECT_CPU_ANALOG_IN3,			
									 SELECT_CPU_ANALOG_IN4,			
									 SELECT_CPU_ANALOG_IN5,			
									 SELECT_CPU_ANALOG_IN6,			
									 SELECT_CPU_ANALOG_IN7,			
									 SELECT_CPU_ANALOG_IN8,			
									 SELECT_CPU_TEMP_SENSOR,				
								   };


// Type definitions
// ================


// Local variables
// ===============
static BYTE xdata A2DCurrentAnalogPin;
static WORD xdata A2DSamplesBuffer[NO_OF_CPU_ANALOG_INPUTS];
	


// Local routines
// ==============
// Select the next analog input to be sampled 
//void A2D_SelectNextInput();  - no need in simulation mode


/****************************************************************************
 *
 *  NAME        : A2D_Init
 *
 *  DESCRIPTION : 
 *
 ****************************************************************************/
void A2D_Init (void)
{
//DUMMY


}//End of ADCInit


/****************************************************************************
 *
 *  NAME        : A2D_Isr
 *
 *  DESCRIPTION : Interrupt service routine for the A2D
 *
 ****************************************************************************/
void A2D_Isr() interrupt 15 using 1
{

//DUMMY
}


/****************************************************************************
 *
 *  NAME        : A2D_GetReading
 *
 *  INPUT       : Analog input number
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : Return The last digital reading for this analog input.
 *
 ****************************************************************************/
WORD A2D_GetReading(BYTE AnalogInNum)
{
	WORD xdata retVal = 5;


	switch (AnalogInNum){
		case CPU_TEMPERATURE_SENSOR + SIM_OHDB_OFFSET:	 	
			EXTMem_ReadAnalog12(AMBIENT_TEMP_ADD_OHDB, &retVal);	
			break;
		
		}
	return retVal;

}



/****************************************************************************
 *
 *  NAME        : A2D_GetBlockReadings
 *
 *  INPUT       : Analog input range (first, last), Buffer for the readings
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : Return The last digital reading for this analog input.
 *
 ****************************************************************************/
void A2D_GetBlockReadings(BYTE FirstAnalogIn, BYTE LastAnalogIn, WORD *Buffer)
{
	WORD xdata w[4] ;
	int sizeOfWord;
	

	sizeOfWord =  sizeof(WORD);
	if (FirstAnalogIn == SELECT_CPU_ANALOG_IN1 && LastAnalogIn == SELECT_CPU_ANALOG_IN4 ){
	//	EXTMem_ReadAnalog12(VS_SENSOR_ADD,&w);
		EXTMem_ReadWord(VS_SENSOR_ADD,  &w[0]);
		EXTMem_ReadWord(VCC_SENSOR_ADD, &w[1]);
		EXTMem_ReadWord(V_12_SENSOR_ADD,&w[2]);
		EXTMem_ReadWord(V_24_SENSOR_ADD,&w[3]);
/*	w[0] = 0x05 ;
	w[1] = 0x06 ;
	w[2] = 0x07 ;
	w[3] = 0x08 ; */
	memcpy(Buffer,&w,(LastAnalogIn-FirstAnalogIn+1)*sizeof(WORD));
//	memcpy(Buffer,&(w[0]), (LastAnalogIn-FirstAnalogIn+1)*sizeof(WORD));	
		}
	return;
}

/****************************************************************************
 *
 *  NAME        : A2D_SelectNextInput
 *
 *  INPUT       : None.
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Select the next analog input to be sampled 
 *
 ****************************************************************************/
/*
void A2D_SelectNextInput() using 1
{
// increment the current analog input variable
// ------------------------------------------- 	
	if (++A2DCurrentAnalogPin == NO_OF_CPU_ANALOG_INPUTS)
		A2DCurrentAnalogPin = 0;	

// selct the this analog input by writing to the MUX register
// -----------------------------------------------------------
	AMX0SL = A2DSelectAnalogInputArray[A2DCurrentAnalogPin];

}
*/
/****************************************************************************
 *
 *  NAME        : A2D_GetBuffer
 *
 *  DESCRIPTION : returns the  a pointer to the samples buffer
 ****************************************************************************/
#ifdef DEBUG
WORD *A2D_GetBuffer()
{
	return A2DSamplesBuffer;
}
#endif

