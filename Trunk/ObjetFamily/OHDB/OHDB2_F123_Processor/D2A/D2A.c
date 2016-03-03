/*===========================================================================
 *   FILENAME       : D/A  {D2A.C}  
 *   PURPOSE        : D/A driver module  
 *   DATE CREATED   : 28/07/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "D2A.h"
#include "c8051F120.h"

/****************************************************************************
 *
 *  NAME        : D2A0_Init
 *
 *  DESCRIPTION : Initialization of the D/A (0) driver
 *
 ****************************************************************************/
void D2A0_Init()
{
	SFRPAGE = DAC0_PAGE;

	// Set the data format
	DAC0CN &= 0xF8;
	
	// Set the D2A to 'On Demand' mode
	DAC0CN &= 0xE7;

	// Enable the D2A converter
	DAC0CN |= 0x80;

	SFRPAGE = LEGACY_PAGE;
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
	SFRPAGE = DAC0_PAGE;

	// First, writing the lower byte to DAC0L 
	DAC0L = NewValue & LSB_MASK;
	
	// Followed by a write to DAC0H that intiate the conversion
	DAC0H = (NewValue & MSB_MASK) >> 8;

	SFRPAGE = LEGACY_PAGE;
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
	SFRPAGE = DAC1_PAGE;

	// Set the data format
	DAC1CN &= 0xF8;
	
	// Set the D2A to 'On Demand' mode
	DAC1CN &= 0xE7;
	
	// Enable the D2A converter
	DAC1CN |= 0x80;

	SFRPAGE = LEGACY_PAGE;
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
	SFRPAGE = DAC1_PAGE;

	// First, writing the lower byte to DAC1L 
	DAC1L = NewValue & LSB_MASK;
	
	// Followed by a write to DAC1H that intiate the conversion
	DAC1H = (NewValue & MSB_MASK) >> 8;

	SFRPAGE = LEGACY_PAGE;
}

