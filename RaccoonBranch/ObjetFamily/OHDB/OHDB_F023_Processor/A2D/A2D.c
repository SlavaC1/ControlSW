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



// Constants
// =========
static const BYTE A2DSelectAnalogInputArray[NO_OF_CPU_ANALOG_INPUTS] = {
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
void A2D_SelectNextInput();


/****************************************************************************
 *
 *  NAME        : A2D_Init
 *
 *  DESCRIPTION : 
 *
 ****************************************************************************/
void A2D_Init (void)
{
// define all the analog inputs as singled-ended inputs
// ---------------------------------------------------- 
	AMX0CF = 0;

// define the conversion clock period and the the intenal amplifier gain
// SAR conversion clock = 8 system clocks, gain = 1
// ------------------------------------------------
	ADC0CF = 0x38;  

// define the first input to be sampled and select it
// --------------------------------------------------  
	A2DCurrentAnalogPin  = CPU_ANALOG_IN_1;
	AMX0SL = SELECT_CPU_ANALOG_IN1;
 
// select internal refernce, enables the bias for the ADC, enable the temperature sensor
// -------------------------------------------------------------------------------------
	REF0CN = 0x07;

// zero the samples buffer
// -----------------------
	memset(A2DSamplesBuffer,0,NO_OF_CPU_ANALOG_INPUTS * sizeof(WORD));

// select the A2D for timer 2 overflow work mode
// ---------------------------------------------
	ADC0CN = 0x0c;
	
// init timer 2 to use timer 2 overflow for initiating an A2D sample
// -----------------------------------------------------------------
	Timer_2_Init();

// enable the A2D converter
// ------------------------
	ADC0CN |= A2D_ENABLE;
	EIE2 |= A2D_INT_ENABLE;

// enable interrputs
// -----------------
	EA |= ENABLE;


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

// disable the ADC interrupt
// -------------------------
	EIE2 &= A2D_INT_DISABLE;		

// clear the ADC conversion complete flag
// --------------------------------------
	AD0INT = 0;

// read the ADC value into the samples buffer
// ------------------------------------------
	A2DSamplesBuffer[A2DCurrentAnalogPin] = ADC0H;
	A2DSamplesBuffer[A2DCurrentAnalogPin] = A2DSamplesBuffer[A2DCurrentAnalogPin] << 8;
	A2DSamplesBuffer[A2DCurrentAnalogPin] |= ADC0L;

// select the next analog input for the next conversion
// ----------------------------------------------------
	A2D_SelectNextInput();

// enable the ADC interrupt
// ------------------------
	EIE2 |= A2D_INT_ENABLE;
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
	WORD RetVal;
// disable the ADC interrupt
// -------------------------
	EIE2 &= A2D_INT_DISABLE;

// get the reading
// ---------------
	RetVal = A2DSamplesBuffer[AnalogInNum];

// enable the ADC interrupt
// ------------------------
	EIE2 |= A2D_INT_ENABLE;

	return RetVal;
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
		
// disable the ADC interrupt
// -------------------------
	EIE2 &= A2D_INT_DISABLE;

// get the readings
// ----------------
	memcpy(Buffer,&(A2DSamplesBuffer[FirstAnalogIn]), (LastAnalogIn-FirstAnalogIn+1)*sizeof(WORD));

// enable the ADC interrupt
// ------------------------
	EIE2 |= A2D_INT_ENABLE;

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

/****************************************************************************
 *
 *  NAME        : A2D_GetBuffer
 *
 *  DESCRIPTION : returns the  a pointer to the samples buffer
 ****************************************************************************/
WORD *A2D_GetBuffer()
{
	return A2DSamplesBuffer;
}

