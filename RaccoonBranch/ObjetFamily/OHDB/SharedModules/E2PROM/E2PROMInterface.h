/*===========================================================================
 *   FILENAME       : E2PROM Interface  {E2PROMInterface.H}  
 *   PURPOSE        : E2PROM interface header file
 *   DATE CREATED   : 17/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _E2PROM_INTERFACE_H_
#define _E2PROM_INTERFACE_H_

#include "Define.h"


// E2PROM Errors
// -------------
typedef enum {
	E2PROM_NO_ERROR = 0,
	E2PROM_SEND_SUCCESS,
	E2PROM_SEND_FAILED,
	E2PROM_WRITE_BLOCK_ERORR,
	E2PROM_BUSY
	}E2PROM_STATUS;


typedef struct {
	BYTE DeviceNum;
  WORD Address;
  BYTE Data;
	}TE2PROMMessage;


// Function Prototype 
// ====================

// read one byte from an E2PROM device
// -----------------------------------
E2PROM_STATUS E2PROMReadByte(BYTE DeviceNum, WORD Address);

// read Length of bytes from an E2PROM device
// ------------------------------------------
E2PROM_STATUS E2PROMReadBlock(BYTE DeviceNum, WORD Address, BYTE Length);

// Sends a Write enable instruction to an E2PROM device
// ----------------------------------------------------
E2PROM_STATUS E2PROMWriteEnable(BYTE DeviceNum);

// write one byte to an E2PROM device
// ----------------------------------
E2PROM_STATUS E2PROMWrite(BYTE DeviceNum, WORD Address, BYTE Data);

// write Length of bytes (up to 16, on the same page) to an E2PROM device
// ----------------------------------------------------------------------
E2PROM_STATUS E2PROMWriteBlock(BYTE DeviceNum, WORD Address,BYTE *Data, BYTE Length);


// test if an E2PROM device exist
// ------------------------------
E2PROM_STATUS TestE2PROMExistence(BYTE DeviceNum);

// protect a partition of an E2PROM device array
// ---------------------------------------------
void E2PROMProtectPartition(BYTE DeviceNum, BYTE Partition);

// call back function for the E2PROM  SPI operations
// -------------------------------------------------
void E2PROMCallBack(BYTE Status);

// get the data received bythe read operation
// ------------------------------------------
E2PROM_STATUS E2PROMGetReceivedData(BYTE DeviceNum, BYTE *Data, BYTE Length);

// get the status of the last write operation
// ------------------------------------------
E2PROM_STATUS E2PROMGetWriteStatus(BYTE DeviceNum);

// Initialization of the E2PROM interface
// --------------------------------------
void E2PROMInit(); 

#endif	



