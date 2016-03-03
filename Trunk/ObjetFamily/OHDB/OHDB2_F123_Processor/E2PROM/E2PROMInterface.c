/*===========================================================================
 *   FILENAME       : E2PROM Interface {E2PROMInterface.c}  
 *   PURPOSE        : Interface to the E2PROM device  
 *   DATE CREATED   : 17/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/

#ifdef OCB_SIMULATOR
	#include "c8051F120.h"
#else	
	#include "c8051F120.h" // Keep it here for consistency with OCB_SIMULATOR define
#endif

#include "E2PROMInterface.h"
#include "spidrv.h"
#include <string.h>


// Constants
// =======================
#define E2PROM_READ_INST		 0x03
#define E2PROM_WRITE_INST		 0x02
#define E2PROM_WRITE_ENABLE_INST 0x06
#define E2PROM_BUFFER_LENGTH	 100

enum E2PROM_READ 
{
	E2PROM_READ_INST_INDEX = 0,
	E2PROM_READ_ADDR_INDEX,
	E2PROM_READ_DUMMY_INDEX,
	E2PROM_READ_INST_LENGTH
};

enum E2PROM_WRITE 
{
	E2PROM_WRITE_INST_INDEX = 0,
	E2PROM_WRITE_ADDR_INDEX,
	E2PROM_WRITE_DATA_INDEX,
	E2PROM_WRITE_INST_LENGTH
};

enum E2PROM_WRITE_ENABLE 
{
	E2PROM_WRITE_ENABLE_INST_INDEX = 0,
	E2PROM_WRITE_ENABLE_INST_LENGTH
};

BYTE xdata E2PROMTransactionStatus;
BYTE xdata E2PROMDataIn[E2PROM_BUFFER_LENGTH];

// This flag distinguish between wriet enable operation to write operation
// ----------------------------------------------------------------------- 
BOOL xdata WriteEnable;

sbit WRITE_PROTECT = P2^5;


/****************************************************************************
 *
 *  NAME        : E2PROMInit
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None
 *
 *  DESCRIPTION : Initialization of the E2PROM interface                                        
 *
 ****************************************************************************/
void E2PROMInit()
{
	E2PROMTransactionStatus = E2PROM_SEND_FAILED;
	WriteEnable = FALSE;
}


/****************************************************************************
 *
 *  NAME        : E2PROMReadByte
 *
 *  INPUT       : E2PROM device number,
 *								Address,
 *								Pointer to the data buffer.
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : Read one byte from an E2PROM device.                                        
 *
 ****************************************************************************/
E2PROM_STATUS E2PROMReadByte(BYTE DeviceNum, WORD Address)
{
	BYTE xdata Instruction, Addr8bit, Addr;
	BYTE xdata *Message;
 
// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return E2PROM_SEND_FAILED;

// set the device status to busy
// -----------------------------
	E2PROMTransactionStatus = E2PROM_BUSY;

// prepare the address and the instruction byte
// --------------------------------------------
	Addr = (BYTE) Address; 	// the lower 8 bit address
	Addr8bit  = (Address & 0x0100) << 3; // take only the 9th bit of the address and shift to be the 3th bit
	Instruction = E2PROM_READ_INST | Addr8bit; //build the instruction byte from the read instruction  and the 9th bit address

// prepare the message to be send using the spi
// --------------------------------------------
	Message = SpiGetTxBuffer();
	Message[E2PROM_READ_INST_INDEX] = Instruction;
	Message[E2PROM_READ_ADDR_INDEX] = Addr;
  Message[E2PROM_READ_DUMMY_INDEX] = 0;

// Turn on the write protection (low)
// --------------------------------
  WRITE_PROTECT = 0;

// select the device (CS)
// ----------------------
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_SELECT);

// send the message using the spi
// ------------------------------
	SpiSend(E2PROM_READ_INST_LENGTH ,E2PROMDataIn, E2PROMCallBack);
			 
	return E2PROM_NO_ERROR;   	
}


/****************************************************************************
 *
 *  NAME        : E2PROMReadBlock
 *
 *  INPUT       : E2PROM device number,
 *								Address,
 *								Pointer to the data buffer,
 *								Number of bytes to read
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : Read Length of bytes from an E2PROM device.                                        
 *
 ****************************************************************************/

E2PROM_STATUS E2PROMReadBlock(BYTE DeviceNum, WORD Address, BYTE Length)
{
	BYTE xdata Instruction, Addr8bit, Addr, index;
	BYTE xdata *Message;
	
// try to lock the SPI device
	if (SpiLock() == SPI_BUSY)
		return E2PROM_SEND_FAILED;

// set the device status to busy
	E2PROMTransactionStatus = E2PROM_BUSY;

// prepare the address and the instruction byte
	Addr = (BYTE) Address; 	// the lower 8 bit address
	Addr8bit  = (Address & 0x0100) << 3; // take only the 9th bit of the address and shift to be the 3th bit
	Instruction = E2PROM_READ_INST | Addr8bit; //build the instruction byte from the read instruction  and the 9th bit address

// prepare the message to be send using the spi
	Message = SpiGetTxBuffer();
	Message[E2PROM_READ_INST_INDEX] = Instruction;
	Message[E2PROM_READ_ADDR_INDEX] = Addr;
	
	for (index = 0; index < Length; index++) 
  	Message[E2PROM_READ_DUMMY_INDEX+index] = 0;

// Turn on the write protection (low)
// --------------------------------
  WRITE_PROTECT = 0;

// select the device (CS)
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_SELECT);

// send the message using the spi
	SpiSend(E2PROM_READ_INST_LENGTH + Length - 1, E2PROMDataIn, E2PROMCallBack);
			 
	return E2PROM_NO_ERROR; 
}


/****************************************************************************
 *
 *  NAME        : E2PROMWrite
 *
 *  INPUT       : E2PROM device number,
 *								Address,
 *								Data to be written.
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : Write one byte to an E2PROM device.
 *								Call E2PROMWriteEnable first.                                        
 *
 ****************************************************************************/
E2PROM_STATUS E2PROMWrite(BYTE DeviceNum, WORD Address, BYTE Data)
{
	BYTE xdata Instruction, Addr8bit, Addr;
	BYTE xdata *Message;

// deselect the device after the write enable inst
// -----------------------------------------------
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_DESELECT);

// set the device status to busy
// -----------------------------
	E2PROMTransactionStatus = E2PROM_BUSY;

// prepare the address and the instruction byte
// --------------------------------------------
	Addr = (BYTE) Address; 	// the lower 8 bit address
	Addr8bit  = (Address & 0x0100) << 3; // take only the 9th bit of the address and shift to be the 3th bit
	Instruction = E2PROM_WRITE_INST | Addr8bit; //build the instruction byte from the write instruction  and the 9th bit address

// prepare the message to be send using the spi
// --------------------------------------------
	Message = SpiGetTxBuffer();
	Message[E2PROM_WRITE_INST_INDEX] = Instruction;
	Message[E2PROM_WRITE_ADDR_INDEX] = Addr;
  Message[E2PROM_WRITE_DATA_INDEX] = Data;


  WriteEnable = FALSE;

// select the device (CS)
// ----------------------
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_SELECT);

// send the message using the spi
// ------------------------------
	SpiSend(E2PROM_WRITE_INST_LENGTH ,E2PROMDataIn, E2PROMCallBack);
			 
	return E2PROM_NO_ERROR;


}


/****************************************************************************
 *
 *  NAME        : E2PROMWriteBlock
 *
 *  INPUT       : E2PROM device number,
 *								Address,
 *								Pointer to the data buffer,
 *								Number of bytes to write
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : Write Length of bytes (up to 16, on the same page) to an E2PROM device.
 *								Call E2PROMWriteEnable first.                                        
 *
 ****************************************************************************/

E2PROM_STATUS E2PROMWriteBlock(BYTE DeviceNum, WORD Address, BYTE *Data, BYTE Length)
{
	BYTE xdata Instruction, Addr8bit, Addr, index;
	BYTE xdata *Message;


// deselect the device after the write enable inst.
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_DESELECT);

// set the device status to busy
// -----------------------------
	E2PROMTransactionStatus = E2PROM_BUSY;

// check if all the bytes are on the same page
	if (((Address + Length - 1)/16) > (Address/16))
		return E2PROM_WRITE_BLOCK_ERORR;

// prepare the address and the instruction byte
	Addr = (BYTE) Address; 	// the lower 8 bit address
	Addr8bit  = (Address & 0x0100) << 3; // take only the 9th bit of the address and shift to be the 3th bit
	Instruction = E2PROM_WRITE_INST | Addr8bit; //build the instruction byte from the write instruction  and the 9th bit address

// prepare the message to be send using the spi
	Message = SpiGetTxBuffer();
	Message[E2PROM_WRITE_INST_INDEX] = Instruction;
	Message[E2PROM_WRITE_ADDR_INDEX] = Addr;
	for (index = 0; index < Length; index++)
	  Message[E2PROM_WRITE_DATA_INDEX+index] = Data[index];
 
	WriteEnable = FALSE;

	// select the device (CS)
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_SELECT);

	// send the message using the spi
	SpiSend(E2PROM_WRITE_INST_LENGTH + Length - 1, E2PROMDataIn,E2PROMCallBack);
			 
	return E2PROM_NO_ERROR;
}


/****************************************************************************
 *
 *  NAME        : E2PROMWriteEnable
 *
 *  INPUT       : E2PROM device number,
 *								Address,
 *								Data to be written.
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : Sends a Write enable instruction to an E2PROM device.
 *								Call this function before calling E2PROMWrite() or E2PROMWriteBlock()                                       
 *
 ****************************************************************************/
E2PROM_STATUS E2PROMWriteEnable(BYTE DeviceNum)
{
	BYTE xdata *Message;
 
// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return E2PROM_SEND_FAILED;

// set the device status to busy
// -----------------------------
	E2PROMTransactionStatus = E2PROM_BUSY;

// Turn off the write protection (high) (should stay high until the end of the writing)
// ------------------------------------------------------------------------------------
  WRITE_PROTECT = 1;

  WriteEnable = TRUE;


// prepare a write enable instruction
// ----------------------------------
	Message = SpiGetTxBuffer();
	Message[E2PROM_WRITE_ENABLE_INST_INDEX] = E2PROM_WRITE_ENABLE_INST;

// select the device (CS)
// ----------------------
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_SELECT);

// send the message using the spi
// ------------------------------
	SpiSend(E2PROM_WRITE_ENABLE_INST_LENGTH ,E2PROMDataIn, E2PROMCallBack);

	return E2PROM_NO_ERROR;

}

/****************************************************************************
 *
 *  NAME        : E2PROMCallBack
 *
 *  INPUT       : Transaction status
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Called from the spi ISR, updates the transaction status.                                        
 *
 ****************************************************************************/
void E2PROMCallBack(BYTE Status) using 3
{
	if (Status == SPI_NO_ERROR)
  	E2PROMTransactionStatus = E2PROM_SEND_SUCCESS;

	else
		E2PROMTransactionStatus = E2PROM_SEND_FAILED;
}

/****************************************************************************
 *
 *  NAME        : E2PROMGetReceivedData
 *
 *  INPUT       : Data buffer, Data length
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : .                                        
 *
 ****************************************************************************/
E2PROM_STATUS E2PROMGetReceivedData(BYTE DeviceNum, BYTE *Data, BYTE Length)
{
// if the spi operation is not done yet
// ------------------------------------
	if (E2PROMTransactionStatus == E2PROM_BUSY)	
		return E2PROM_BUSY; 

	if (E2PROMTransactionStatus == E2PROM_SEND_SUCCESS)
	{
		if (Length == 1)		// save the memcpy overhead
			*Data = E2PROMDataIn[2];	// the two first byte is junk received after th read insturction
		else
			memcpy (Data, &(E2PROMDataIn[2]), Length); // the tow first byte is junk received after th read insturction
	}

// deselect the device (CS)
// ------------------------
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_DESELECT);


// unlock the spi since the operation is done
// ------------------------------------------
	SpiUnLock();

	return E2PROMTransactionStatus; 
}


/****************************************************************************
 *
 *  NAME        : E2PROMGetWriteStatus
 *
 *  INPUT       : NONE
 *
 *  OUTPUT      : E2PROM_ERROR.
 *
 *  DESCRIPTION : Return the status of the last write operation.
 *								Call it after E2PROMWriteByte or E2PROMWriteBlock                                        
 *
 ****************************************************************************/
E2PROM_STATUS E2PROMGetWriteStatus(BYTE DeviceNum)
{
// if the spi operation is not done yet
// ------------------------------------
	if (E2PROMTransactionStatus == E2PROM_BUSY)	
		return E2PROM_BUSY; 



// deselect the device (CS)
// ------------------------
	SpiSelectSlave(E2PROM_BASE_SLAVE_ID + DeviceNum,CHIP_DESELECT);


// Turn on the write protection (low)
// --------------------------------
  if (!WriteEnable)
  {
    WRITE_PROTECT = 0;

  // unlock the spi since the operation is done
  // ------------------------------------------
	  SpiUnLock();
	}

	return E2PROMTransactionStatus;

}

