/*===========================================================================
 *   FILENAME       : Comparator  {Comparator.C}  
 *   PURPOSE        : Comparator driver module  
 *   DATE CREATED   : 14/08/2003
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "Comparator.h"
#ifdef OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif

/****************************************************************************
 *
 *  NAME        : Comparator0_Init
 *
 *  DESCRIPTION : Initialization of the comparator (0) driver
 *
 ****************************************************************************/
void Comparator0_Init()
{
#ifdef OCB2
	SFRPAGE = CPT0_PAGE;
#endif

  // Set the negative hysteresis to 2 mv
  CPT0CN &= 0xFC;
  CPT0CN |= 0x01;

  // Set the positive hysteresis to 2 mv
  CPT0CN &= 0xF3;
  CPT0CN |= 0x04;

	// enable comparator 0
	CPT0CN |= 0x80;

#ifdef OCB2
	SFRPAGE = CONFIG_PAGE;
#endif

  // Enable comparator 0 port pins
  XBR0 |= 0x80; 


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
#ifdef OCB2
	SFRPAGE = CPT1_PAGE;
#endif

  // Set the negative hysteresis to 2 mv
  CPT1CN &= 0xFC;
  CPT1CN |= 0x01;

  // Set the positive hysteresis to 2 mv
  CPT1CN &= 0xF3;
  CPT1CN |= 0x04;

	// enable comparator 1
	CPT1CN |= 0x80;

#ifdef OCB2
	SFRPAGE = CONFIG_PAGE;
#endif

  // Enable comparator 1 port pins
	XBR1 |= 0x01; 

}

