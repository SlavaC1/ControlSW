/*===========================================================================
 *   FILENAME       : Uart driver {UartDrv.h}  
 *   PURPOSE        : Uart driver header file  
 *   DATE CREATED   : 11/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/

#ifndef _UART_DRV_H_
#define _UART_DRV_H_

#include "Define.h"
#include "RingBuff.h"

// UART Errors
// -----------
typedef enum {
	UART_NO_ERROR = 0,
	UART_TX_BUFFER_FULL
	}UART_STATUS;



// Function Prototype 
// ====================

//Initialization of UART 0
// -------------------------
void Uart0Init(void);

//send a data buffer using the UART
// --------------------------------
UART_STATUS UartSend(BYTE *Data, BYTE Length);

// Returns a pointer to the receive data buffer
// --------------------------------------------
RBUF_RingBuffer *UartGetReceiveBuffer(void);


#endif	