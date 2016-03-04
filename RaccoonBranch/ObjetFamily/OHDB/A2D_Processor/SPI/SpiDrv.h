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

#include "c8051F200.h"
#include "Define.h"


// Spi Errors
// ----------
typedef enum {
	SPI_NO_ERROR = 0,	
	SPI_BUSY,					 			// spi is in use
	SPI_WRITE_COLLISION,		 			// attempt to write while data transfer was in progress
	SPI_MODE_FAULT,							// master mode collision
	SPI_RECIEVE_OVERRUN					// receive overrun
	}SPI_STATUS;

// Spi Slave's id
// --------------
enum SPI_SLAVES_ID{
	E2PROM1_SLAVE_ID = 0,				
	E2PROM2_SLAVE_ID,				
	E2PROM3_SLAVE_ID,				
	E2PROM4_SLAVE_ID,				
	E2PROM5_SLAVE_ID,				
	E2PROM6_SLAVE_ID,				
	E2PROM7_SLAVE_ID,				
	E2PROM8_SLAVE_ID,
	XILINX_SLAVE_ID,
	POTENTIOMETER_SLAVE_ID,
	NUM_OF_SPI_SLAVES
	};				

 
#define SPI_SELECT_E2PROM_1				0x80
#define SPI_SELECT_E2PROM_2				0x90
#define SPI_SELECT_E2PROM_3				0xa0
#define SPI_SELECT_E2PROM_4				0xb0
#define	SPI_SELECT_E2PROM_5				0xc0
#define SPI_SELECT_E2PROM_6				0xd0
#define SPI_SELECT_E2PROM_7				0xe0
#define SPI_SELECT_E2PROM_8				0xf0
#define SPI_SELECT_XILINX					0x00
#define SPI_SELECT_POTENTIOMETER	0x10
#define SPI_SELECT_NONE						0x30

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
void SpiInit(BYTE *TxBufferToUse, int TxBufferSizeToUse);

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


BYTE SpiGetTxByteCounter();

#endif	








