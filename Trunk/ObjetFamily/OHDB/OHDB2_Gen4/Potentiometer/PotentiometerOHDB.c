/*===========================================================================
 *   FILENAME       : Potentiometer Interface {Potentiometer.c}  
 *   PURPOSE        : Interface to the Potentiometer device  
 *   DATE CREATED   : 18/7/2002
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/

#include "PotentiometerOHDB.h"
#include "spidrv.h"

#define OHDB_POTENMTR_0_ADDR		0x00
#define OHDB_POTENMTR_1_ADDR		0x01
#define OHDB_POTENMTR_BUFFER_LENGTH	3
	
enum OHDB_POTENMTR_MESSAGE 
{
	OHDB_POTENMTR_ADDR_INDEX = 0,
	OHDB_POTENMTR_DATA_INDEX,
	OHDB_POTENMTR_INST_LENGTH
};

BYTE xdata OHDBPotenmtrTransactionStatus;
BYTE xdata OHDBPotenmtrDataIn[OHDB_POTENMTR_BUFFER_LENGTH];

// Different address for each half of Gen4 head
BYTE xdata OHDBPotenmtrAddress[] = {OHDB_POTENMTR_0_ADDR, OHDB_POTENMTR_1_ADDR};


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
void OHDBPotenmtrInit()
{
	OHDBPotenmtrTransactionStatus = OHDB_POTENMTR_SEND_FAILED;
}


/****************************************************************************
 *
 *  NAME        : PotenmtrWriteValue
 *
 *
 *  DESCRIPTION : Write a new value to one of the potentiometer devices.                                       
 *
 ****************************************************************************/
OHDB_POTENMTR_STATUS OHDBPotenmtrWriteValue(BYTE DeviceNum, BYTE Value)
{
	BYTE xdata *Message;

	// Try to lock the SPI device
	if (SpiLock() == SPI_BUSY)
		return OHDB_POTENMTR_SEND_FAILED;

	// Set the device status to busy
	OHDBPotenmtrTransactionStatus = OHDB_POTENMTR_BUSY;
 
	// Select the device (CS)
	SpiSelectSlave(POTENTIOMETER_BASE_SLAVE_ID + DeviceNum, CHIP_SELECT);

	// Prepare the message to be send using the spi
	Message                           = SpiGetTxBuffer();
	Message[OHDB_POTENMTR_ADDR_INDEX] = OHDBPotenmtrAddress[DeviceNum % 2];
	Message[OHDB_POTENMTR_DATA_INDEX] = Value;

	// Send the message using the spi
	SpiSend(OHDB_POTENMTR_INST_LENGTH ,OHDBPotenmtrDataIn, OHDBPotenmtrCallBack);
			 
	return OHDB_POTENMTR_NO_ERROR;   	
}


/****************************************************************************
 *
 *  NAME        : OHDBPotenmtrCallBack
 *
 *  INPUT       : The operation status.
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Callback function for the SPI ISR.                                       
 *
 ****************************************************************************/
void OHDBPotenmtrCallBack(BYTE Status) using 3
{
	if (Status == SPI_NO_ERROR)
  		OHDBPotenmtrTransactionStatus = OHDB_POTENMTR_SEND_SUCCESS;
	else
		OHDBPotenmtrTransactionStatus = OHDB_POTENMTR_SEND_FAILED;
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
OHDB_POTENMTR_STATUS OHDBPotenmtrGetWriteStatus(BYTE DeviceNum)
{
// if the spi operation is not done yet
// ------------------------------------
	if (OHDBPotenmtrTransactionStatus == OHDB_POTENMTR_BUSY)	
		return OHDB_POTENMTR_BUSY; 

// deselect the device (CS)
// ------------------------
	SpiSelectSlave(POTENTIOMETER_BASE_SLAVE_ID + DeviceNum, CHIP_DESELECT);


// unlock the spi since the operation is done
// ------------------------------------------
	SpiUnLock();

	return OHDBPotenmtrTransactionStatus;
} 
