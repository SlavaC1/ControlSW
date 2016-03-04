/*===========================================================================
 *   FILENAME       : SPI DRIVER {SPIDRV.C}  
 *   PURPOSE        : Serial Peripheral Interface driver  
 *   DATE CREATED   : 15/7/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include "SpiDrv.h"


// Constants
// =========
const BYTE SelectSlaveArrayPort1[NUM_OF_SPI_SLAVES] = {
									SPI_SELECT_E2PROM_1_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_2_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_3_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_4_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_5_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_6_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_7_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_8_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_1_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_2_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_3_VALUE_FOR_P1,
                  SPI_SELECT_POTENTIOMETER_4_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_5_VALUE_FOR_P1,
                  SPI_SELECT_POTENTIOMETER_6_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_7_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_8_VALUE_FOR_P1,
									SPI_SELECT_ON_BOARD_E2PROM_VALUE_FOR_P1,
									SPI_SELECT_A2D_PROCESSOR_VALUE_FOR_P1,
									SPI_SELECT_XILINX_VALUE_FOR_P1
	                };


const BYTE SelectSlaveArrayPort2[NUM_OF_SPI_SLAVES] = {
									SPI_SELECT_E2PROM_1_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_2_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_3_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_4_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_5_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_6_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_7_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_8_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_1_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_2_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_3_VALUE_FOR_P2,
                  SPI_SELECT_POTENTIOMETER_4_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_5_VALUE_FOR_P2,
                  SPI_SELECT_POTENTIOMETER_6_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_7_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_8_VALUE_FOR_P2,
									SPI_SELECT_ON_BOARD_E2PROM_VALUE_FOR_P2,
									SPI_SELECT_A2D_PROCESSOR_VALUE_FOR_P2,
									SPI_SELECT_XILINX_VALUE_FOR_P2
	                };

#define SPI_MAX_MSG_LEN 	100


// Type definitions
// ================
 

// Local routines
// ==============


// Module variables
// ================

// pointer to the Tx buffer
// ------------------------
	BYTE xdata SpiTxBuffer[SPI_MAX_MSG_LEN];

// pointer to the Rx buffer
// ------------------------
	BYTE xdata *SpiRxBuffer;

// Number of bytes to transmit/receive
// -----------------------------------
	BYTE xdata SpiDataLen;

// currnet index in the Tx buffer
// ------------------------------
	BYTE xdata SpiIndex;

// Is the spi in use
// -----------------
	bit SpiInUse;

// Is the current session is with the A2D processor
// ------------------------------------------------
	bit CommWithA2D;

// pointer to the call back function
// ---------------------------------
	TCallback EndSpiTransactionCallBack;

// delay before setting the NSS to low
// ----------------------------------- 
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

// set the SPI for sampling data on first edge of SCK period,
// SCK line low on idle state, 8 bit frame size
// -------------------------------------------- 
  SPI0CFG        = 0x07;

// Configure the spi clock to 21600 Hz
// -------------------------------
  SPI0CKR        = 0xff;             

// Set the spi to master mode, enable the spi device
// ------------------------------------------------
  SPI0CN         = 0x03;

// Configure P1.7, P2.0, P2.1, P2.2, P2.3 (address lines) as push pull 
// -------------------------------------------------------------------
  P1MDOUT |= 0x80;
  P2MDOUT |= 0x0f;

// Configure P1.6 (write enable for xilinx and E2PROM) as push pull
// ----------------------------------------------------------------
	P1MDOUT |= 0x40;

// enable the spi port pins
// ------------------------
	XBR0 |= 0x02;

// enable the crossbar
// -------------------
	XBR2 |= 0x40;

// set the SPI priority level to high
// ----------------------------------
	EIP1 |= 0x01;
	
// Enable SPI interrupt
// --------------------						 
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
	// ------------------------
		SPIF = 0;
	
		if(CommWithA2D)
		{
			// at the end of a data transfer set the NSS to high
			// -------------------------------------------------
		  P1 = (P1 & 0x7f)  | SPI_SELECT_NONE_VALUE_FOR_P1;
		  P2 = (P2 & 0xf0)  | SPI_SELECT_NONE_VALUE_FOR_P2;	  
		}
	
		
		// insert the recieved data to the Rx buffer
 		// -----------------------------------------
		SpiRxBuffer[SpiIndex-1] = SPI0DAT;
		
		
		if (SpiIndex < SpiDataLen)
		{
			if(CommWithA2D)
			{
				// delay before setting the NSS to low
				// -----------------------------------
				for (Delay = 0; Delay < 20; Delay++)
				 	;

				// set the NSS to low
				// ------------------
    		P1 = (P1 & 0x7f)  | SPI_SELECT_A2D_PROCESSOR_VALUE_FOR_P1;
		    P2 = (P2 & 0xf0)  | SPI_SELECT_A2D_PROCESSOR_VALUE_FOR_P2;
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
// --------------------
	if (SpiInUse  || TXBSY)
		return SPI_BUSY;

// lock the spi
// ------------
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
// modify only the 4 lower bits of port 2 and the upper bit of port 1
// ------------------------------------------------------------------ 
	if (Select == CHIP_SELECT)
	{
	  P1 = (P1 & 0x7f) | SelectSlaveArrayPort1[SlaveID];
		P2 = (P2 & 0xf0) | SelectSlaveArrayPort2[SlaveID];
	}
	else
	{
		P1 = (P1 & 0x7f)  | SPI_SELECT_NONE_VALUE_FOR_P1;
		P2 = (P2 & 0xf0)  | SPI_SELECT_NONE_VALUE_FOR_P2;
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


