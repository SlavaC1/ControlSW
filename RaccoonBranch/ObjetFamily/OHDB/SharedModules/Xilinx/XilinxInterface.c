/*===========================================================================
 *   FILENAME       : Xilinx Interface {XilinxInterface.c}  
 *   PURPOSE        : Interface to the xilinx device  
 *   DATE CREATED   : 17/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
//#pragma ROM(COMPACT)

#include "XilinxInterface.h"

#include "spidrv.h"


// Constants
// =========
#define XILINX_BUFFER_LENGTH	5


// Type definitions
// ================

enum XILINX_MESSAGE {
	XILINX_ADDR_INDEX = 0,
	XILINX_DATA_MSB_INDEX,
	XILINX_DATA_LSB_INDEX,
	XILINX_MESSAGE_LENGTH
	};

 
// Local routines
// ==============


// Module variables
// ================================

BYTE xdata XilinxDataIn[XILINX_BUFFER_LENGTH];
BYTE xdata XilinxTransactionStatus;

// Exported routines
// =================

/****************************************************************************
 *
 *  NAME        : XilinxInit
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None
 *
 *  DESCRIPTION : Initialization of the xilinx interface                                        
 *
 ****************************************************************************/
void XilinxInit()
{
	XilinxTransactionStatus = XILINX_SEND_FAILED;
}

/****************************************************************************
 *
 *  NAME        : XilinxRead
 *
 *  INPUT       : The address
 *
 *  OUTPUT      : XILINX_STATUS.
 *
 *  DESCRIPTION : Read from the xilinx device                                        
 *
 ****************************************************************************/
XILINX_STATUS XilinxRead(BYTE Address)
{
	BYTE xdata *Message;

// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return XILINX_SEND_FAILED;
	
// set the device status to busy
// -----------------------------
	XilinxTransactionStatus = XILINX_BUSY;
 
// select the xilinx device 
// ------------------------
	SpiSelectSlave(XILINX_SLAVE_ID,CHIP_SELECT);

// set the R/W pin to read (low)
// -----------------------------
	XILINX_R_W = 0;

// prepare the message
// -------------------
	Message = SpiGetTxBuffer();
	Message[XILINX_ADDR_INDEX] = Address;
	Message[XILINX_DATA_MSB_INDEX] = 0;
	Message[XILINX_DATA_LSB_INDEX] = 0;


// send the message using the spi
// ------------------------------
	SpiSend(XILINX_MESSAGE_LENGTH,XilinxDataIn,XilinxCallBack);
			 
	return XILINX_NO_ERROR;
	
}


/****************************************************************************
 *
 *  NAME        : XilinxWrite
 *
 *  INPUT       : TXilinxMessage struct (address, data)
 *
 *  OUTPUT      : XILINX_STATUS.
 *
 *  DESCRIPTION : Write to the xilinx device                                        
 *
 ****************************************************************************/
XILINX_STATUS XilinxWrite(TXilinxMessage *Msg)
{
	BYTE xdata *Message;

// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return XILINX_SEND_FAILED;

// set the device status to busy
// -----------------------------
	XilinxTransactionStatus = XILINX_BUSY;

// select the xilinx device 
// ------------------------
	SpiSelectSlave(XILINX_SLAVE_ID,CHIP_SELECT);

// set the R/W pin to write (high)
// -------------------------------
	XILINX_R_W = 1;

// prepare the message
// -------------------
	Message = SpiGetTxBuffer();
	Message[XILINX_ADDR_INDEX] = Msg->Address;
	Message[XILINX_DATA_MSB_INDEX] = (BYTE)(((Msg->Data) & MSB_MASK) >> 8);
	Message[XILINX_DATA_LSB_INDEX] = (BYTE)((Msg->Data) & LSB_MASK);
	
// send the message using the spi
// ------------------------------
	SpiSend(XILINX_MESSAGE_LENGTH,XilinxDataIn,XilinxCallBack);
			 
	return XILINX_NO_ERROR;
}


/****************************************************************************
 *
 *  NAME        : XilinxCallBack
 *
 *  INPUT       : Transaction status
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Called from the spi ISR, updates the transaction status.                                        
 *
 ****************************************************************************/
void XilinxCallBack(BYTE Status) using 3
{
	if (Status == SPI_NO_ERROR)
  	XilinxTransactionStatus = XILINX_SEND_SUCCESS;

	else
		XilinxTransactionStatus = XILINX_SEND_FAILED;
}


/****************************************************************************
 *
 *  NAME        : XilinxGetReceivedData
 *
 *  INPUT       : Data buffer
 *
 *  OUTPUT      : XILINX_STATUS.
 *
 *  DESCRIPTION : Get the data received after calling XilinxRead().                                        
 *
 ****************************************************************************/
XILINX_STATUS XilinxGetReceivedData(BYTE *Data)
{
// if the spi operation is not done yet
// ------------------------------------
	if (XilinxTransactionStatus == XILINX_BUSY)	
		return XILINX_BUSY; 

	if (XilinxTransactionStatus == XILINX_SEND_SUCCESS)
	{
		Data[1] = XilinxDataIn[XILINX_DATA_LSB_INDEX];
		Data[0] = XilinxDataIn[XILINX_DATA_MSB_INDEX];
	}

// deselect the device (CS)
// ------------------------
	SpiSelectSlave(XILINX_SLAVE_ID,CHIP_DESELECT);


// unlock the spi since the operation is done
// ------------------------------------------
	SpiUnLock();

	return XilinxTransactionStatus; 
}


/****************************************************************************
 *
 *  NAME        : XilinxGetWriteActionStatus
 *
 *  INPUT       : None
 *
 *  OUTPUT      : XILINX_STATUS.
 *
 *  DESCRIPTION : Get the status of the last write operation.                                        
 *
 ****************************************************************************/
XILINX_STATUS XilinxGetWriteActionStatus()
{
// if the spi operation is not done yet
// ------------------------------------
	if (XilinxTransactionStatus == XILINX_BUSY)	
		return XILINX_BUSY; 

// deselect the device (CS)
// ------------------------
	SpiSelectSlave(XILINX_SLAVE_ID,CHIP_DESELECT);

// set the R/W pin to read (low)
// -------------------------------
	XILINX_R_W = 0;

// unlock the spi since the operation is done
// ------------------------------------------
	SpiUnLock();

	return XilinxTransactionStatus;
}
