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

// Wee read all odd inputs, switch with FPGA mux, and read all even inputs and temp sensor
// The conversion back is done in a HeadsVoltageReader. See implementation in HeadsVoltageReaderTask
static const BYTE A2D0SelectAnalogInputArray[NO_OF_CPU_ANALOG_INPUTS] = 
{
	SELECT_CPU_AD0_ANALOG_IN1, 			
	SELECT_CPU_AD0_ANALOG_IN3, 			
	SELECT_CPU_AD0_ANALOG_IN5, 			
	SELECT_CPU_AD0_ANALOG_IN7, 			
	SELECT_CPU_AD0_ANALOG_IN2, 			
	SELECT_CPU_AD0_ANALOG_IN4, 			
	SELECT_CPU_AD0_ANALOG_IN6, 			
	SELECT_CPU_AD0_ANALOG_IN8, 			
	SELECT_CPU_AD0_TEMP_SENSOR 						
};

static const BYTE A2D2SelectAnalogInputArray[NO_OF_P1_ANALOG_INPUTS] = 
{
	SELECT_CPU_AD2_ANALOG_IN1,			
	SELECT_CPU_AD2_ANALOG_IN2,			
	SELECT_CPU_AD2_ANALOG_IN3,			
	SELECT_CPU_AD2_ANALOG_IN4,			
	SELECT_CPU_AD2_ANALOG_IN5,			
	SELECT_CPU_AD2_ANALOG_IN6,			
	SELECT_CPU_AD2_ANALOG_IN7,			
	SELECT_CPU_AD2_ANALOG_IN8	
};				   

// Local variables
// ===============
static BYTE xdata A2D0CurrentAnalogPin;
static WORD xdata A2D0SamplesBuffer[NO_OF_CPU_ANALOG_INPUTS];

static BYTE xdata A2D2CurrentAnalogPin;
static BYTE xdata A2D2SamplesBuffer[NO_OF_P1_ANALOG_INPUTS];

bit A2D0BufferReady;

void A2D_Init()
{
	// Use timer 2 overflow for initiating an A2D2 sample
	Timer_2_Init();
	
	A2D0_Init();
	A2D2_Init();	
}


/****************************************************************************
 *
 *  NAME        : A2D0_Init
 *
 *  DESCRIPTION : 
 *
 ****************************************************************************/
void A2D0_Init (void)
{
	SFRPAGE = ADC0_PAGE;

	// Define all the analog inputs as singled-ended inputs
	AMX0CF = 0;

	// Define the conversion clock period and the intenal amplifier gain
	// SAR conversion clock = 8 system clocks, gain = 1
    ADC0CF = 0x18;  

	// Define the first input to be sampled and select it
	A2D0CurrentAnalogPin = CPU_ANALOG_IN_1;
	AMX0SL               = SELECT_CPU_AD0_ANALOG_IN1;
 
	// Select internal refernce, enables the bias for the ADC, enable the temperature sensor
	REF0CN = 0x07;

	// Zero the samples buffer
	memset(A2D0SamplesBuffer, 0, NO_OF_CPU_ANALOG_INPUTS * sizeof(WORD));

	// Conversion initiated on every write of '1' to AD0BUSY
	ADC0CN = 0x00;	

	// Enable the A2D converter
	ADC0CN |= A2D_ENABLE;
	EIE2   |= A2D0_INT_ENABLE;

	// Enable interrputs
	EA |= ENABLE;

	A2D0BufferReady = FALSE;

	SFRPAGE = LEGACY_PAGE;
}


void A2D2_Init()
{
	SFRPAGE = ADC2_PAGE;

	// Define all the analog inputs as singled-ended inputs
	AMX2CF = 0x00;

	// Define the conversion clock period and the intenal amplifier gain
    ADC2CF = 0xF9; 

	// Define the first input to be sampled and select it
	A2D2CurrentAnalogPin = CPU_ANALOG_IN_1;
	AMX2SL               = SELECT_CPU_AD2_ANALOG_IN1;

	// Zero the samples buffer
	memset(A2D2SamplesBuffer, 0, NO_OF_P1_ANALOG_INPUTS * sizeof(BYTE));

	// Select the A2D for timer 2 overflow work mode
	ADC2CN = 0x06;	

	// Enable the A2D converter
	ADC2CN |= A2D_ENABLE;
	EIE2   |= A2D2_INT_ENABLE;

	// Enable interrputs
	EA |= ENABLE;

	SFRPAGE = LEGACY_PAGE;
}


/****************************************************************************
 *
 *  NAME        : A2D0_Isr
 *
 *  DESCRIPTION : Interrupt service routine for the A2D0
 *
 ****************************************************************************/
void A2D0_Isr() interrupt 15 using 1
{
	// Disable the ADC interrupt
	EIE2 &= A2D0_INT_DISABLE;

	// Clear the ADC conversion complete flag
	AD0INT = 0;	

	// Read the ADC value into the samples buffer
	A2D0SamplesBuffer[A2D0CurrentAnalogPin]  = ADC0H;
	A2D0SamplesBuffer[A2D0CurrentAnalogPin]  = A2D0SamplesBuffer[A2D0CurrentAnalogPin] << 8;
	A2D0SamplesBuffer[A2D0CurrentAnalogPin] |= ADC0L;	
		
	// Enable the ADC interrupt
	EIE2 |= A2D0_INT_ENABLE;
	
	A2D0BufferReady = TRUE;
}

/****************************************************************************
 *
 *  NAME        : A2D2_Isr
 *
 *  DESCRIPTION : Interrupt service routine for the A2D2
 *
 ****************************************************************************/
void A2D2_Isr() interrupt 18 using 1
{
	// Disable the ADC interrupt 	
	EIE2 &= A2D2_INT_DISABLE;		

	// Clear the ADC conversion complete flag
	AD2INT = 0;

	// Read the ADC value into the samples buffer
	A2D2SamplesBuffer[A2D2CurrentAnalogPin] = ADC2;	

	// Select the next analog input for the next conversion
	A2D2_SelectNextInput();

	// Enable the ADC interrupt
	EIE2 |= A2D2_INT_ENABLE;	
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
WORD A2D0_GetReading(BYTE AnalogInNum)
{
	WORD RetVal;
	
	// Disable the ADC interrupt
	EIE2 &= A2D0_INT_DISABLE;

	// Get the reading
	RetVal = A2D0SamplesBuffer[AnalogInNum];

	// Enable the ADC interrupt
	EIE2 |= A2D0_INT_ENABLE;

	return RetVal;
}

WORD A2D2_GetReading(BYTE AnalogInNum)
{
	WORD RetVal;

	EIE2 &= A2D2_INT_DISABLE;

	RetVal = A2D2SamplesBuffer[AnalogInNum];

	EIE2 |= A2D2_INT_ENABLE;

	// Shifting left 4 bits to normalize readings to 12 bits A2D at application level
	return (RetVal << 4);
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
void A2D0_SelectNextInput() 
{
	// Increment the current analog input variable	
	if (++A2D0CurrentAnalogPin == NO_OF_CPU_ANALOG_INPUTS)
		A2D0CurrentAnalogPin = 0;	

	// Selct the this analog input by writing to the MUX register
	AMX0SL = A2D0SelectAnalogInputArray[A2D0CurrentAnalogPin];
}

void A2D2_SelectNextInput() using 1
{
	// Increment the current analog input variable
	if (++A2D2CurrentAnalogPin == NO_OF_P1_ANALOG_INPUTS)
		A2D2CurrentAnalogPin = 0;	

	// Selct the this analog input by writing to the MUX register
	AMX2SL = A2D2SelectAnalogInputArray[A2D2CurrentAnalogPin];
}

bit IsA2D0BufferReady()
{
	return A2D0BufferReady;
}

void SetA2D0BufferReadiness(bit Status)
{
	A2D0BufferReady = Status;
}

BYTE GetA2D0SelectedInput()
{
	return A2D0CurrentAnalogPin;
}