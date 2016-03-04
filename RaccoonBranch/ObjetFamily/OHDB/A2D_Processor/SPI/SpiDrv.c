/*===========================================================================
 *   FILENAME       : SPI DRIVER {SPIDRV.C}  
 *   PURPOSE        : Hold's All SPI Comunication  routine  
 *   DATE CREATED   : 7/Oct/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   PROCEDURES     : SpiInit(),SpiSend(),SpiReceive()  
 *   Procedure Name : Procedure for controlling transmitting and receiving 
 *                    data via SPI port   
 *===========================================================================*/
//#pragma ROM(COMPACT)
#include "spidrv.h"
#include "addrv.h"
#include "version.h"  

// Compiler Definitions
// Done automaticly Writen in the prg file 
// ====================


// Procedure Specifications 
// ========================


// Local declerations
// ==================

// Constants
// =========
#define SPI_GET_A2D_READINGS_CMD	0x33
#define SPI_GET_A2D_SW_VERSION_CMD	0x34

// Structure definitions
// =====================

// Type definitions
// ================

// Array definitions
// =================
	
 
// Macros
// ======

// Local variables
// ===============
	static BYTE xdata *TxBuffer;
	static BYTE xdata *TxA2DBuffer; 
    static BYTE xdata TxBufferA2D_SW_Version[2];
	static int xdata TxBufferSize;

// Local routines
// ==============


// Module variables
// ================================

// the received byte
// ----------------- 
	BYTE xdata RxByte;

// the A2D reading to transmit
// --------------------
	WORD xdata A2DReading;

// currnet index in of byte to transmit
// ------------------------------------
	BYTE xdata TxCounter;


// Exported routines
// =================


/****************************************************************************
 *
 *  NAME        : SpiInit
 *
 *  DESCRIPTION : Initializiation of the SPI driver
 *
 ****************************************************************************/
void SpiInit(BYTE *TxBufferToUse, int TxBufferSizeToUse)
{
    TxBufferA2D_SW_Version[0] =  EXTERNAL_SOFTWARE_VERSION;  // ??? Check byte order
    TxBufferA2D_SW_Version[1] =  INTERNAL_SOFTWARE_VERSION;

	TxA2DBuffer = TxBufferToUse;
	TxBufferSize = TxBufferSizeToUse; // ??? is this needed ?

// set the tx byte counter to the buffer size
// -------------------------
	TxCounter = TxBufferSize ; // ??? is this needed ?

// set the SPI for sampling data on first edge of SCK period,
// SCK line low on idle state, 8 bit frame size
// -------------------------------------------- 
  SPI0CFG        = 0x07;

// Configure the spi clock to 21600 Hz
// -------------------------------
  SPI0CKR        = 0xff;             

// Set the spi to slave mode, enable the spi device
// ------------------------------------------------
  SPI0CN         = 0x01;


	PRT2CF |= 0x02; // set miso to push pull

// route P2.0, P2.1. P2.2, P2.3 to the SPI
// ---------------------------------------
	PRT2MX |= 0x01;

// Enable SPI interrupt
// --------------------
  EIE1          |= ENABLE_SPI_INTERRUPT;  

// enable interrputs
// -----------------
	EA |= ENABLE;	

}


/****************************************************************************
 *
 *  NAME        : SpiIsr
 *
 *  DESCRIPTION : Interrupt service routine for the SPI
 *
 ****************************************************************************/
void SpiIsr()interrupt 6 using 3
{	
  if (SPIF)
	{
	// enable interrputs
	// -----------------
		EA &= DISABLE;	

	// Get the receive byte and decode it
 	// ----------------------------------
		RxByte = SPI0DAT;

	// if the byte is a "get A2D readings command" reset the counter
	// -------------------------------------------------------------

    	if (RxByte == SPI_GET_A2D_READINGS_CMD || RxByte == SPI_GET_A2D_SW_VERSION_CMD)
		{
		  // code common to both cases:
		  TxCounter = 0;

		  if (RxByte == SPI_GET_A2D_READINGS_CMD)
		  {
		    TxBuffer = TxA2DBuffer;
		  }
		  else if (RxByte == SPI_GET_A2D_SW_VERSION_CMD)
		  {
		    TxBuffer = TxBufferA2D_SW_Version;
		  }
		}



	// get an A2D reading by the TxCounter (the counter counts in bytes, the readings are words)
	// -----------------------------------
		
  	SPI0DAT = TxBuffer[TxCounter];

		TxCounter++;

	// clear the interrupt flag
	// ------------------------
		SPIF = 0;

	// enable interrputs
	// -----------------
		EA |= ENABLE;
	} // if (SPIF)

	if(WCOL)	// write collision
	{
		WCOL = 0;
	}

	if(MODF)	// mode fault
	{
		MODF = 0;
	}

	if(RXOVRN)	// receive overrun
	{
		RXOVRN = 0;
	}

}  

 
BYTE SpiGetTxByteCounter() using 1
{
	return TxCounter;
}

