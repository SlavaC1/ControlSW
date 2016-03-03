/*===========================================================================
 *   FILENAME       : Xilinx Interface  {XilinxInterface.H}  
 *   PURPOSE        : Xilinx interface header file
 *   DATE CREATED   : 17/Oct/2001
 *   PROGRAMMER     : Nir Saadon 
 *   Include        :    
 *===========================================================================*/

#ifndef _XILINX_INTERFACE_H_
#define _XILINX_INTERFACE_H_

#ifdef OCB_SIMULATOR
   #include "c8051F120.h"
#elif defined (C8051F206_CPU)
	#include "c8051F200.h"
#elif defined(C8051F005_CPU)
	#include "c8051F000.h"
#elif defined(C8051F023_CPU)
	#include "c8051F020.h"
#elif defined(C8051F123_CPU)
	#include "c8051F120.h"
#endif

#include "Define.h"


// Xilinx Errors
// -------------
typedef enum {
	XILINX_NO_ERROR = 0,
	XILINX_SEND_SUCCESS,
	XILINX_SEND_FAILED,
	XILINX_BUSY
	}XILINX_STATUS;




typedef struct {
	BYTE Address;
	WORD Data;
	}TXilinxMessage;


// I/O ports
// ---------
#if defined (C8051F206_CPU)
	sbit XILINX_R_W = P0^3;
#elif defined (C8051F005_CPU)
	sbit XILINX_R_W = P3^3;
#elif defined (C8051F023_CPU)
	sbit XILINX_R_W = P1^6;
#elif defined (C8051F123_CPU)
	sbit XILINX_R_W = P3^0;
#endif

#ifdef OCB_SIMULATOR
sbit XILINX_R_W = 0x90^6;  //DUMMY port
#endif

// Function Prototype 
// ====================

// initialization of the xilinx interface
// --------------------------------------
void XilinxInit();

// read from the xilinx device
// ---------------------------
XILINX_STATUS XilinxRead(BYTE Address);

// write to the xilinx device
// --------------------------
XILINX_STATUS XilinxWrite(TXilinxMessage *Msg);

// call back function for the SPI ISR
// ----------------------------------
void XilinxCallBack(BYTE Status);

// Get the data received after calling XilinxRead()
// ------------------------------------------------
XILINX_STATUS XilinxGetReceivedData(BYTE *Data);

// Get the status of the last write operation
// ------------------------------------------                                        
XILINX_STATUS XilinxGetWriteActionStatus();


#endif	



