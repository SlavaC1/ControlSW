/*===========================================================================
 *   FILENAME       : Potentiometer Interface {Potentiometer.c}  
 *   PURPOSE        : Interface to the Potentiometer device  
 *   DATE CREATED   : 22/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
//#pragma ROM(COMPACT)

#include "Potentiometer.h"
#include "spidrv.h"



// Constants
// =========
#define POTENMTR_WRITE_INST					0x11
#define POTENMTR_BUFFER_LENGTH			5

// Type definitions
// ================
	
	enum OHDB_POTENMTR_MESSAGE {
	OHDB_POTENMTR_WRITE_INST_INDEX = 0,
	OHDB_POTENMTR_DATA_INDEX,
	OHDB_POTENMTR_INST_LENGTH
	};
// Local routines
// ==============


// Module variables
// ================================
	BYTE xdata PotenmtrTransactionStatus;
	BYTE xdata PotenmtrDataIn[POTENMTR_BUFFER_LENGTH];


// Exported routines
// =================




/****************************************************************************
 *
 *  NAME        : PotenmtrInit
 *
 *  INPUT       : NONE
 *
 *  OUTPUT      : NONE.
 *
 *  DESCRIPTION : Initialization of the poteniometer interface.                                       
 *
 ****************************************************************************/
void PotenmtrInit()
{
	PotenmtrTransactionStatus = POTENMTR_SEND_FAILED;
}



/****************************************************************************
 *
 *  NAME        : PotenmtrWriteValue
 *
 *  INPUT       : The new value.
 *
 *  OUTPUT      : POTENMTR_STATUS.
 *
 *  DESCRIPTION : Write a new value to the poteniometer device.                                       
 *
 ****************************************************************************/
POTENMTR_STATUS PotenmtrWriteValue(BYTE Value)
{
	BYTE xdata *Message;

// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return POTENMTR_SEND_FAILED;

// set the device status to busy
// -----------------------------
	PotenmtrTransactionStatus = POTENMTR_BUSY;
 
// select the device (CS)
// ----------------------
	SpiSelectSlave(POTENTIOMETER_SLAVE_ID,CHIP_SELECT);

// prepare the message to be send using the spi
// --------------------------------------------
	Message = SpiGetTxBuffer();
	Message[POTENMTR_WRITE_INST_INDEX] = POTENMTR_WRITE_INST;
	Message[POTENMTR_DATA_INDEX] = Value;

// send the message using the spi
// ------------------------------
	SpiSend(OHDB_POTENMTR_INST_LENGTH ,OHDBPotenmtrDataIn, OHDBPotenmtrCallBack);
			 
	return POTENMTR_NO_ERROR;   	

}


/****************************************************************************
 *
 *  NAME        : PotenmtrCallBack
 *
 *  INPUT       : The operation status.
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Callback function for the SPI ISR.                                       
 *
 ****************************************************************************/
void PotenmtrCallBack(BYTE Status) using 3
{
	if (Status == SPI_NO_ERROR)
  	PotenmtrTransactionStatus = POTENMTR_SEND_SUCCESS;

	else
		PotenmtrTransactionStatus = POTENMTR_SEND_FAILED;
}


/****************************************************************************
 *
 *  NAME        : PotenmtrGetWriteStatus
 *
 *  INPUT       : None.
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Returns the status of the last write operation.
 *								(also deselect the potentiometer device and unlocks the SPI)                                      
 *
 ****************************************************************************/
POTENMTR_STATUS PotenmtrGetWriteStatus()
{
// if the spi operation is not done yet
// ------------------------------------
	if (PotenmtrTransactionStatus == POTENMTR_BUSY)	
		return POTENMTR_BUSY; 

// deselect the device (CS)
// ------------------------
	SpiSelectSlave(POTENTIOMETER_SLAVE_ID,CHIP_DESELECT);


// unlock the spi since the operation is done
// ------------------------------------------
	SpiUnLock();

	return PotenmtrTransactionStatus;
} 
