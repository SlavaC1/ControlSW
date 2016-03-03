/*===========================================================================
 *   FILENAME       : Comparator  {Comparator.C}  
 *   PURPOSE        : Comparator driver module  
 *   DATE CREATED   : 14/08/2003
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "Comparator.h"
#include "c8051F120.h" 

/****************************************************************************
 *
 *  NAME        : Comparator0_Init
 *
 *  DESCRIPTION : Initialization of the comparator (0) driver
 *
 ****************************************************************************/
void Comparator0_Init()
{
	SFRPAGE = CPT0_PAGE;

  	// Set the negative hysteresis to 2 mv
  	CPT0CN &= 0xFC;
  	CPT0CN |= 0x01;

  	// Set the positive hysteresis to 2 mv
  	CPT0CN &= 0xF3;
  	CPT0CN |= 0x04;

	// enable comparator 0
	CPT0CN |= 0x80;
	
	SFRPAGE = LEGACY_PAGE;	 
}


/****************************************************************************
 *
 *  NAME        : Comparator0_Init
 *
 *  DESCRIPTION : Initialization of the comparator (1) driver
 *
 ****************************************************************************/
void Comparator1_Init()
{
	SFRPAGE = CPT1_PAGE;  
  
  	// Set the negative hysteresis to 2 mv
  	CPT1CN &= 0xFC;
  	CPT1CN |= 0x01;

  	// Set the positive hysteresis to 2 mv
  	CPT1CN &= 0xF3;
  	CPT1CN |= 0x04;

	// enable comparator 1
	CPT1CN |= 0x80;
	
	SFRPAGE = LEGACY_PAGE;	 	
}

