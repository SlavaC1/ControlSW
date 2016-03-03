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

#if defined OCB_SIMULATOR & defined SIM_GEN4
 static const BYTE A2D0SelectAnalogInputArray[NO_OF_CPU_ANALOG_INPUTS] = 
{
	SELECT_CPU_ANALOG_IN1,			
	SELECT_CPU_ANALOG_IN2,			
	SELECT_CPU_ANALOG_IN3,			
	SELECT_CPU_ANALOG_IN4,			
	SELECT_CPU_ANALOG_IN5,			
	SELECT_CPU_ANALOG_IN6,			
	SELECT_CPU_ANALOG_IN7,			
	SELECT_CPU_ANALOG_IN8,			
	SELECT_CPU_TEMP_SENSOR				
};
#endif
// Type definitions
// ================


// Local variables
// ===============
static BYTE xdata A2DCurrentAnalogPin;
static WORD xdata A2DSamplesBuffer[NO_OF_CPU_ANALOG_INPUTS];

#if defined OCB_SIMULATOR & defined SIM_GEN4
static BYTE xdata A2D0CurrentAnalogPin;	
bit A2D0BufferReady;
#endif
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
void A2D_Isr() // interrupt 15 using 1
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
WORD A2D0_GetReading(BYTE AnalogInNum)
{
	WORD xdata retVal = 5;


	switch (AnalogInNum){
		//NEED FOR SIM_OHDB_OFFSET, because uses counter from zero to num of cpu(9) in A2D0_SelectNextInput (++A2D0CurrentAnalogPin == NO_OF_CPU_ANALOG_INPUTS)
		case CPU_TEMPERATURE_SENSOR /*+ SIM_OHDB_OFFSET*/ : 	 	
			EXTMem_ReadAnalog12(AMBIENT_TEMP_ADD_OHDB, &retVal);	
			break;
		
		}
	return retVal;
}  
WORD A2D2_GetReading(BYTE AnalogInNum)
{
/*	WORD RetVal;

	EIE2 &= A2D2_INT_DISABLE;

	RetVal = A2D2SamplesBuffer[AnalogInNum];

	EIE2 |= A2D2_INT_ENABLE;

	// Shifting left 4 bits to normalize readings to 12 bits A2D at application level
	return (RetVal << 4);
	*/
	BYTE temp = AnalogInNum;
	return 0;
}
#if defined OCB_SIMULATOR & defined SIM_GEN4
void SetA2D0BufferReadiness(bit Status)
{
	A2D0BufferReady = Status;
}
bit IsA2D0BufferReady()
{
	return TRUE;
}
BYTE GetA2D0SelectedInput()
{
	return A2D0CurrentAnalogPin;
}
void A2D0_SelectNextInput() //using 1
{
// increment the current analog input variable
// ------------------------------------------- 	
	if ((++A2D0CurrentAnalogPin ) == NO_OF_CPU_ANALOG_INPUTS)
		A2D0CurrentAnalogPin = 0;	

// selct the this analog input by writing to the MUX register
// -----------------------------------------------------------
	AMX0SL = A2D0SelectAnalogInputArray[A2D0CurrentAnalogPin];
}
#endif