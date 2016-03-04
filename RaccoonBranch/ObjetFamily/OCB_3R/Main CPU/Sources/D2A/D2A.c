/*===========================================================================
 *   FILENAME       : D/A  {D2A.C}  
 *   PURPOSE        : D/A driver module  
 *   DATE CREATED   : 25/11/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "D2A.h"
#include "c8051F020.h"

/****************************************************************************
 *
 *  NAME        : D2A0_Init
 *
 *  DESCRIPTION : Initialization of the D/A (0) driver
 *
 ****************************************************************************/
void D2A0_Init()
{
  // Set the data format
  DAC0CN &= 0xF8;

	// Set the D2A to 'On Demand' mode
  DAC0CN &= 0xE7;

	// Enable the D2A converter
	DAC0CN |= 0x80;

}


/****************************************************************************
 *
 *  NAME        : D2A0_Write
 *
 *  DESCRIPTION : Set the output of the D2A (0)
 *
 ****************************************************************************/
void D2A0_Write(WORD NewValue)
{
  // First, writing the lower byte to DAC0L 
  DAC0L = NewValue & LSB_MASK;

	// Followed by a write to DAC0H that intiate the conversion
  DAC0H = (NewValue & MSB_MASK) >> 8;
}

/****************************************************************************
 *
 *  NAME        : D2A1_Init
 *
 *  DESCRIPTION : Initialization of the D/A (1) driver 
 *
 ****************************************************************************/
void D2A1_Init()
{
  // Set the data format
  DAC1CN &= 0xF8;

	// Set the D2A to 'On Demand' mode
  DAC1CN &= 0xE7;

	// Enable the D2A converter
	DAC1CN |= 0x80;

}


/****************************************************************************
 *
 *  NAME        : D2A1_Write
 *
 *  DESCRIPTION : Set the output of the D2A (1)
 *
 ****************************************************************************/
void D2A1_Write(WORD NewValue)
{
  // First, writing the lower byte to DAC1L 
  DAC1L = NewValue & LSB_MASK;

	// Followed by a write to DAC1H that intiate the conversion
  DAC1H = (NewValue & MSB_MASK) >> 8;
}


// Set the output of the one of the D2A's 
void D2A_Write(BYTE DeviceId, WORD Value)
{
  if (DeviceId == D2A_0)
    D2A0_Write(Value);

  else if (DeviceId == D2A_1)
    D2A1_Write(Value);
}

