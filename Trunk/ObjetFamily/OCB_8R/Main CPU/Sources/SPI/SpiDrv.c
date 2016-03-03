/*===========================================================================
 *   FILENAME       : SPI DRIVER {SPIDRV.C}  
 *   PURPOSE        : Serial Peripheral Interface driver  
 *   DATE CREATED   : 10/9/2003
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "SpiDrv.h"


// Constants
// =========

const BYTE SelectSlaveArray[NUM_OF_SPI_SLAVES] = {
  SPI_SELECT_A2D_PROCESSOR,
  SPI_SELECT_SPI_1,
  SPI_SELECT_SPI_2,
  SPI_SELECT_SPI_3,
  SPI_SELECT_ODOR_FAN
	};

#define SPI_MAX_MSG_LEN 	100


// Type definitions
// ================
 

// Local routines
// ==============


// Module variables
// ================

// Pointer to the Tx buffer
	BYTE xdata SpiTxBuffer[SPI_MAX_MSG_LEN];

// Pointer to the Rx buffer
	BYTE xdata *SpiRxBuffer;

// Number of bytes to transmit/receive
	BYTE xdata SpiDataLen;

// Currnet index in the Tx buffer
	BYTE xdata SpiIndex;

// Is the spi in use
	bit SpiInUse;

// Is the current session is with the A2D processor
	bit CommWithA2D;

// Pointer to the call back function
	TCallback EndSpiTransactionCallBack;

// Delay before setting the NSS to low
	BYTE xdata Delay;



/****************************************************************************
 *
 *  NAME        : SpiInit
 *
 *  DESCRIPTION : Initialize the SPI driver                                        
 *
 ****************************************************************************/
void SpiInit(void)
{
	SpiInUse = FALSE;
	CommWithA2D = FALSE;

#ifdef OCB2
    SFRPAGE   = SPI0_PAGE;
    SPI0CFG   = 0x40;
    SPI0CN    = 0x09;
    SPI0CKR   = 0xFF; // Configure the spi clock to 21600 Hz
#else
// Set the SPI for sampling data on first edge of SCK period,
// SCK line low on idle state, 8 bit frame size
  SPI0CFG        = 0x07;

// Configure the spi clock to 21600 Hz
  SPI0CKR        = 0xff;             

// Set the spi to master mode, enable the spi device
  SPI0CN         = 0x03;
#endif


#ifdef OCB2
	SFRPAGE = CONFIG_PAGE;
#endif


// Configure P2.0, P2.1, P2.2 (address lines) as push pull 
  P2MDOUT |= 0x07;

// Enable the spi port pins
	XBR0 |= 0x02;

// Enable the crossbar
	XBR2 |= 0x40;

// Set the SPI priority level to high
	EIP1 |= 0x01;
	
// Enable SPI interrupt
  EIE1 |= ENABLE_SPI_INTERRUPT; 
	  
}


/****************************************************************************
 *
 *  NAME        : SpiSend
 *
 *  DESCRIPTION : Send a buffer using the SPI                                        
 *
 ****************************************************************************/
SPI_STATUS SpiSend(unsigned int DataLength ,BYTE *DataIn, TCallback EndTransactionCallBack) 
{
#ifdef OCB2
    SFRPAGE   = SPI0_PAGE;
#endif
// Disable spi interrupts
	EIE1 &= ~ENABLE_SPI_INTERRUPT;

// Copy the function arguments to the module variables for use in the ISR
	SpiRxBuffer = DataIn;
	SpiDataLen = DataLength;
	EndSpiTransactionCallBack = EndTransactionCallBack;

	SpiIndex = 0;

// Put the first byte in the spi data register to initiate the interrupt
	SPI0DAT = SpiTxBuffer[SpiIndex++];

// Enable SPI interrupt
  EIE1 |= ENABLE_SPI_INTERRUPT;

	return SPI_NO_ERROR;
}


/****************************************************************************
 *
 *  NAME        : SpiIsr
 *
 *  DESCRIPTION : SPI interrupt service routine                                        
 *
 ****************************************************************************/
void SpiIsr()interrupt 6 using 3
{
	if (SPIF)
	{
	// Clear the interrupt flag
		SPIF = 0;
	
		if(CommWithA2D)
		{
			// At the end of a data transfer set the NSS to high
		  P2 = 0xf8  | SPI_SELECT_NONE;	  
		}
	
		
		// Insert the recieved data to the Rx buffer
		SpiRxBuffer[SpiIndex-1] = SPI0DAT;
		
		
		if (SpiIndex < SpiDataLen)
		{
			if(CommWithA2D)
			{
				// Delay before setting the NSS to low
				for (Delay = 0; Delay < 20; Delay++)
				 	;

				// Set the NSS to low
		    P2 = 0xf8  | SPI_SELECT_A2D_PROCESSOR;
			}
			
		  SPI0DAT = SpiTxBuffer[SpiIndex++];
			return;
		}
		else
		{
			if (SpiIndex == SpiDataLen)
			{
				EndSpiTransactionCallBack(SPI_NO_ERROR);
				return;
			}
		
		}	
	}

	if(WCOL)	// write collision
	{ 
		WCOL = 0;
		EndSpiTransactionCallBack(SPI_WRITE_COLLISION);
		return;
	}

	if(MODF)	// mode fault
	{
		MODF = 0;
		EndSpiTransactionCallBack(SPI_MODE_FAULT);
		return;
	}

	if(RXOVRN)	// receive overrun
	{
		RXOVRN = 0;
		EndSpiTransactionCallBack(SPI_RECIEVE_OVERRUN);
		return;
	}

} 

/****************************************************************************
 *
 *  NAME        : SpiLock
 *
 *  INPUT       : NONE.
 *
 *  OUTPUT      : SPI_STATUS.
 *
 *  DESCRIPTION : try to get a lock on the SPI device.                                        
 *
 ****************************************************************************/
SPI_STATUS SpiLock()
{

// Check if SPI is free
	if (SpiInUse /* || TXBSY*/)
		return SPI_BUSY;

// Lock the spi
	SpiInUse = TRUE;

	return SPI_NO_ERROR;
  
}


 
/****************************************************************************
 *
 *  NAME        : SpiUnLock
 *
 *  INPUT       : NONE.
 *
 *  OUTPUT      : NONE.
 *
 *  DESCRIPTION : unlock the SPI device.                                        
 *
 ****************************************************************************/
void SpiUnLock()
{
	SpiInUse = FALSE;
}


/****************************************************************************
 *
 *  NAME        : SpiSelectSlave
 *
 *  INPUT       : Slave Id, Select/Deselect.
 *
 *  OUTPUT      : NONE.
 *
 *  DESCRIPTION : select/deselect a slave device.                                        
 *
 ****************************************************************************/
void SpiSelectSlave(BYTE SlaveID, BYTE Select)
{
// Modify only the 3 lower bits of port 2
	if (Select == CHIP_SELECT)
		P2 = 0xf8 | SelectSlaveArray[SlaveID];
	else
		P2 = 0xf8 | SPI_SELECT_NONE;

}


/****************************************************************************
 *
 *  NAME        : SpiGetTxBuffer
 *
 *  DESCRIPTION : Returns a pointer to the spi transmit buffer.                                        
 *
 ****************************************************************************/
BYTE* SpiGetTxBuffer()
{
	return SpiTxBuffer;
}


/****************************************************************************
 *
 *  NAME        : SpiSetCommWithA2D
 *
 *  DESCRIPTION : Used for setteing the current session as a session with the 
 *								A2D processor.                                       
 *
 ****************************************************************************/
void SpiSetCommWithA2D(BOOL State)
{
	CommWithA2D = State;
}


