/*===========================================================================
 *   FILENAME       : OhdbMain {OhdbMain.C}  
 *   PURPOSE        : Hold's All Analog to digital conversation routine  
 *   DATE CREATED   : 1/Oct/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : Main 
 *   Procedure Name : Main(),
 *   Description    : This file hold's the main function  
 *===========================================================================*/

#include "addrv.h"
#include "TimerDrv.h"
#include "SpiDrv.h"


// Compiler Definitions
// Done automaticly Writen in the prg file 
// ====================


// Procedure Specifications 
// ========================


// Local declerations
// ==================

// Constants
// =========

// Structure definitions
// =====================

// Type definitions
// ================


// Array definitions
// =================
	

// Pointer definitions
// =================

// Macros
// ======

// Local variables
// ===============
	WORD xdata *A2DBuffer;

// Local routines proto 
// ======================
 

// Exported routines & Variables
// =============================


// Debug definitions
// =================


// Hardware definitions
// =================
 	

void main (void)
{
// disable watchdog timer
// ----------------------
	WDTCN = 0xde;
  WDTCN = 0xad;

// set the processor for working with external oscillator 
// ------------------------------------------------------
	SysClkInit();

// initilize the A2D driver
// ------------------------
	A2DInit();

// initilize the SPI driver
// ------------------------
	A2DBuffer = A2DGetBuffer();
	SpiInit((BYTE*)A2DBuffer, NO_OF_ANALOG_INPUTS*2);
  
  while (1)
  {
 	
		
  }

}






void dummy_int_3() interrupt 3 using 1
{
}
 
void dummy_int_5() interrupt 5 using 1
{
}
 
void dummy_int_7() interrupt 7 using 1
{
}

void dummy_int_8() interrupt 8 using 1
{
}

void dummy_int_9() interrupt 9 using 1
{
}


