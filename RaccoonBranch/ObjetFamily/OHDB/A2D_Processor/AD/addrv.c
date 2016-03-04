/*===========================================================================
 *   FILENAME       : A/D Driver {ADDRV.C}  
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : ADInit (void) 
 *   Procedure Name :
 *   Description    :
 *   Include        :  
 *===========================================================================*/
#include <string.h>
#include "c8051F200.h"
#include "Define.h"
#include "addrv.h"
#include "TimerDrv.h"
#include "SpiDrv.h"


// Procedure Specifications 
// ========================


// Local declerations
// ==================

// Constants
// =========
	const BYTE A2DSelectAnalogInputArray[NO_OF_ANALOG_INPUTS] = {
									 SELECT_ANALOG_IN1,			
									 SELECT_ANALOG_IN2,			
									 SELECT_ANALOG_IN3,				
									 SELECT_ANALOG_IN4,		 		
									 SELECT_ANALOG_IN5,				
									 SELECT_ANALOG_IN6,			
									 SELECT_ANALOG_IN7,		 		
									 SELECT_ANALOG_IN8,				
									 SELECT_ANALOG_IN9,				
									 SELECT_ANALOG_IN10,		 	
									 SELECT_ANALOG_IN11,		
									 SELECT_ANALOG_IN12,		
									 SELECT_ANALOG_IN13,			
									 SELECT_ANALOG_IN14,		
									 SELECT_ANALOG_IN15,		
									 SELECT_ANALOG_IN16,		
									 SELECT_ANALOG_IN17,		
									 SELECT_ANALOG_IN18,		
									 SELECT_ANALOG_IN19,		
									 SELECT_ANALOG_IN20,
									 SELECT_ANALOG_IN21,		
									 SELECT_ANALOG_IN22,		
									 SELECT_ANALOG_IN23,		
									 SELECT_ANALOG_IN24
								 };

// Structure definitions
// =====================

// Type definitions
// ================

// Array definitions
// =================
	typedef union 
	{
 	int WordData;
 	unsigned char ByteData[1];
	}LittelBigEndian;


// Macros
// ======

// Local variables
// ===============

	BYTE xdata A2DCurrentAnalogPin;
	WORD xdata A2DSamplesBuffer[NO_OF_ANALOG_INPUTS];
	


// Local routines
// ==============

// Select the next analog input to be sampled 
	void A2DSelectNextInput();

// Exported routines
// =================

/*===========================================================================
 *   FILENAME       : A/D Driver   
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : A2DInit()
 *   Description    : Init The AD hardware registers 
 *   Include        : None 
 *===========================================================================*/
 void A2DInit (void)
{
// define the conversion clock period and the the intenal amplifier gain
// SAR conversion clock = 8 system clocks, gain = 1
// ------------------------------------------------
	ADC0CF = 0x60;

	
// define the input pins that are for analog use
// ---------------------------------------------
	P0MODE = SELECT_ANALOG_PINS_OF_PORT0;
	P1MODE = SELECT_ANALOG_PINS_OF_PORT1;
	P2MODE = SELECT_ANALOG_PINS_OF_PORT2;
	P3MODE = SELECT_ANALOG_PINS_OF_PORT3;

// define the first input to be sampled and select it
// --------------------------------------------------  
	A2DCurrentAnalogPin  = ANALOG_IN_1;
	AMX0SL = SELECT_ANALOG_IN1;
 
// Select external reference as the VREF source
// --------------------------------------------
	REF0CN = 0;

// zero the samples buffer
// -----------------------
	memset(A2DSamplesBuffer,0,NO_OF_ANALOG_INPUTS * sizeof(WORD));

// select the A2D for timer 2 overflow work mode
// ---------------------------------------------
	ADC0CN |= 0x0c;
	
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
 *  NAME        : A2DIsr
 *
 *  DESCRIPTION : Interrupt service routine for the A2D
 *
 ****************************************************************************/
void A2DIsr() interrupt 15 using 1
{

// disable the ADC interrupt
// -------------------------
	EIE2 &= A2D_INT_DISABLE;		

// clear the ADC conversion complete flag
// --------------------------------------
	ADCINT = 0;

  if ((SpiGetTxByteCounter() >> 1) != A2DCurrentAnalogPin)
  {
    // read the ADC value into the samples buffer
    // ------------------------------------------
	  A2DSamplesBuffer[A2DCurrentAnalogPin] = ADC0H;
	  A2DSamplesBuffer[A2DCurrentAnalogPin] = A2DSamplesBuffer[A2DCurrentAnalogPin] << 8;
	  A2DSamplesBuffer[A2DCurrentAnalogPin] |= ADC0L;
  }

// select the next analog input for the next conversion
// ----------------------------------------------------
	A2DSelectNextInput();

// enable the ADC interrupt
// ------------------------
	EIE2 |= A2D_INT_ENABLE;
}


/****************************************************************************
 *
 *  NAME        : A2DGetReading
 *
 *  INPUT       : Analog input number
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : Return The last digital reading for this analog input.
 *
 ****************************************************************************/
WORD A2DGetReading(BYTE AnalogInNum)
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
 *  NAME        : A2DGetReading_U3
 *
 *  INPUT       : Analog input number
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : The same as A2DGetReading but using register bank 3
 *
 ****************************************************************************/
WORD A2DGetReading_U3(BYTE AnalogInNum) using 3
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
 *  NAME        : A2DGetBlockReadings
 *
 *  INPUT       : Analog input range (first, last), Buffer for the readings
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : Return The last digital reading for this analog input.
 *
 ****************************************************************************/
void A2DGetBlockReadings(BYTE FirstAnalogIn, LastAnalogIn, WORD *Buffer)
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
 *  NAME        : A2DSelectNextInput
 *
 *  INPUT       : None.
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Select the next analog input to be sampled 
 *
 ****************************************************************************/
void A2DSelectNextInput() using 1
{
// increment the current analog input variable
// ------------------------------------------- 	
	if (++A2DCurrentAnalogPin == NO_OF_ANALOG_INPUTS)
		A2DCurrentAnalogPin = 0;	

// selct the this analog input by writing to the MUX register
// -----------------------------------------------------------
	AMX0SL = A2DSelectAnalogInputArray[A2DCurrentAnalogPin];

}

/****************************************************************************
 *
 *  NAME        : A2DGetBuffer
 *
 *  DESCRIPTION : returns the  a pointer to the samples buffer
 ****************************************************************************/
WORD *A2DGetBuffer()
{
	return A2DSamplesBuffer;
}
/*===========================================================================
 *   FILENAME       : A/D Driver   
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : LittelBigConvert()
 *   Description    : Init The AD hardware registers 
 *   Include        : None 
 *===========================================================================*/
unsigned int LittelBigConvert(unsigned int val)
{
  unsigned char data1,data2;             
	unsigned int  Cval;                    
	static        LittelBigEndian Conver; //Creat an instance to the 
	                                      //converstion union 
	Conver.WordData = val;                //Swep the data byte 
	data1 = Conver.ByteData[0]; 
	data2 = Conver.ByteData[1];
	Conver.ByteData[0]= data2;            //Transfer the data byte to the union 
	Conver.ByteData[1]= data1;
	Cval = Conver.WordData; 
	return (Cval);
}//LittelBigConvert

