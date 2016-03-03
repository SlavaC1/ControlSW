/*===========================================================================
 *   FILENAME       : SPI DRIVER {SPIDRV.C}  
 *   PURPOSE        : Serial Peripheral Interface driver  
 *   DATE CREATED   : 15/7/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "SpiDrv.h"


// Constants
// =========

const BYTE SelectSlaveArray[NUM_OF_SPI_SLAVES] = 
{
	SPI_SELECT_E2PROM_1,
	SPI_SELECT_E2PROM_2,
	SPI_SELECT_E2PROM_3,
	SPI_SELECT_E2PROM_4,
	SPI_SELECT_E2PROM_5,
	SPI_SELECT_E2PROM_6,
	SPI_SELECT_E2PROM_7,
	SPI_SELECT_E2PROM_8,
	SPI_SELECT_POTENTIOMETER_1,
	SPI_SELECT_POTENTIOMETER_2,
	SPI_SELECT_POTENTIOMETER_3,
	SPI_SELECT_POTENTIOMETER_4,
	SPI_SELECT_POTENTIOMETER_5,
	SPI_SELECT_POTENTIOMETER_6,
	SPI_SELECT_POTENTIOMETER_7,
	SPI_SELECT_POTENTIOMETER_8,
	SPI_SELECT_ON_BOARD_E2PROM,
	SPI_SELECT_A2D_PROCESSOR,	
	SPI_SELECT_XILINX,
	SPI_SELECT_A2D_EXT
};

#define SPI_MAX_MSG_LEN 100

// pointer to the Tx buffer	
BYTE xdata SpiTxBuffer[SPI_MAX_MSG_LEN];

// pointer to the Rx buffer
BYTE xdata *SpiRxBuffer;

// Number of bytes to transmit/receive
BYTE xdata SpiDataLen;

// currnet index in the Tx buffer
BYTE xdata SpiIndex;

// Is the spi in use
bit SpiInUse;

// Is the current session is with the A2D processor
bit CommWithA2D;

// pointer to the call back function
TCallback EndSpiTransactionCallBack;

// delay before setting the NSS to low
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
	SpiInUse    = FALSE;
	CommWithA2D = FALSE;

    SFRPAGE = SPI0_PAGE;
    SPI0CFG = 0x40;
    SPI0CN  = 0x09;
    SPI0CKR = 0xFF; // Configure the spi clock to 21600 Hz

	// Configure P2.3 P2.4 P2.5 P2.6 P2.7 (address lines) as push pull. For SPI chip selects.
	P2MDOUT |= 0xF8;	
	
	// Configure P3.0 P3.1 (address lines) as push pull. For SPI_WR	
	P3MDOUT |= 0x03;

	// set the SPI priority level to high	
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
    SFRPAGE = SPI0_PAGE;

	// disable spi interrupts
	// ----------------------
	EIE1 &= ~ENABLE_SPI_INTERRUPT;

	// copy the function arguments to the module variables for use in the ISR
	// ---------------------------------------------------------------------- 	
	SpiRxBuffer = DataIn;
	SpiDataLen = DataLength;
	EndSpiTransactionCallBack = EndTransactionCallBack;

	SpiIndex = 0;

	// put the first byte in the spi data register to initiate the interrupt
	// ---------------------------------------------------------------------
	SPI0DAT = SpiTxBuffer[SpiIndex++];

	// Enable SPI interrupt
	// --------------------						 
  	EIE1 |= ENABLE_SPI_INTERRUPT;

	return SPI_NO_ERROR;
 }//End of SpiInit()


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
		// clear the interrupt flag		
		SPIF = 0;
	
		if(CommWithA2D)
		{
			// at the end of a data transfer set the NSS to high						
			P2 = (P2 & SPI_SELECT_NONE) | SPI_SELECT_NONE;			
		}	
		
		// insert the recieved data to the Rx buffer 	
		SpiRxBuffer[SpiIndex-1] = SPI0DAT;		
		
		if (SpiIndex < SpiDataLen)
		{
			if(CommWithA2D)
			{
				// delay before setting the NSS to low				
				for (Delay = 0; Delay < 20; Delay++)
				 	;

				// set the NSS to low					    	
			    P2 = (P2 & SPI_SELECT_NONE) | SPI_SELECT_A2D_PROCESSOR;				
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
}  // End of  SpiIsr

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
	// check if SPI is free	
	if (SpiInUse)
		return SPI_BUSY;

	// lock the spi
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
	// Modify only the bits 3, 4, 5, 6, 7 of P2
	if (Select == CHIP_SELECT)
	{	  
		P2 = (P2 & SPI_SELECT_NONE) | SelectSlaveArray[SlaveID];		
	}
	else
	{	
		P2 = (P2 & SPI_SELECT_NONE) | SPI_SELECT_NONE;
	}
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


