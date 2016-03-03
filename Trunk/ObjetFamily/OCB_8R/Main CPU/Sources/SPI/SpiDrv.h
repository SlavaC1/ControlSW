/*===========================================================================
 *   FILENAME       : SPI DRIVER {SPIDRV.H}  
 *   PURPOSE        : Hold's All SPI commuinication routine  
 *   DATE CREATED   : 02/Sep/2001
 *   PROGRAMMER     : Juval Izhaki 
 *   Description    : SPI H File 
 *   Include        : Define.h,HardWareIO.h   
 *===========================================================================*/

#ifndef _SPI_DRV_H_
#define _SPI_DRV_H_

#ifdef OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
#include "Define.h"


// Spi Errors
// ----------
typedef enum {
	SPI_NO_ERROR = 0,	
	SPI_BUSY,					 			// spi is in use
	SPI_WRITE_COLLISION,		// attempt to write while data transfer was in progress
	SPI_MODE_FAULT,					// master mode collision
	SPI_RECIEVE_OVERRUN			// receive overrun
	}SPI_STATUS;

// Spi Slave's id
// --------------
enum SPI_SLAVES_ID{
	A2D_PROCESSOR_SLAVE_ID = 0,
	SPI1_SLAVE_ID,
	SPI2_SLAVE_ID,
	SPI3_SLAVE_ID,
	ODOR_FAN_SLAVE_ID,
	NUM_OF_SPI_SLAVES
	};				


#define SPI_SELECT_A2D_PROCESSOR          			  0x00
#define SPI_SELECT_SPI_1                 				  0x04
#define SPI_SELECT_SPI_2                 				  0x02
#define SPI_SELECT_SPI_3                 				  0x06
#define	SPI_SELECT_ODOR_FAN             				  0x01
#define SPI_SELECT_NONE             						  0x05

#define CHIP_DESELECT					0	
#define CHIP_SELECT						1	

#define  ENABLE_SPI_INTERRUPT  0x01


// typdef for the end transaction callback
// ---------------------------------------
typedef void (*TCallback)(int);


// Function Prototype 
// ====================

// initialize the SPI device
// -------------------------
void SpiInit(void);

// send data using the SPI
// -----------------------
SPI_STATUS SpiSend(unsigned int DataLength ,BYTE *DataIn,TCallback EndTransactionCallBack);  

// try to get a lock on the SPI device
// -----------------------------------
SPI_STATUS SpiLock();

// unlock the SPI device
// ---------------------
void SpiUnLock();

// select/deselect a slave device
// ------------------------------
void SpiSelectSlave(BYTE SlaveID, BYTE Select);

// Returns a pointer to the spi transmit buffer
// --------------------------------------------
BYTE* SpiGetTxBuffer();

// Used for setteing the current session as a session with the A2D processor
// -------------------------------------------------------------------------
void SpiSetCommWithA2D(BOOL State);


#endif	








