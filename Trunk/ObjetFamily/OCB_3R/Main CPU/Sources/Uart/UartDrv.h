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


typedef enum{UART_0, UART_1}TUartNum;

// UART Errors
typedef enum {
	UART_NO_ERROR = 0,
	UART_TX_BUFFER_FULL
	}UART_STATUS;



// Function Prototype 
// ====================

//Initialization of UART 0
void Uart0Init(void);

//Initialization of UART 1
void Uart1Init(void);

//send a data buffer using UART 0 or 1
UART_STATUS UartSend(BYTE *Data, BYTE Length, TUartNum UartNum);

// Returns a pointer to the receive data buffer
RBUF_RingBuffer *UartGetReceiveBuffer(TUartNum UartNum);

// Disable UART 0 interrupt and return the previuos interrupt state                                        
BYTE Uart0DisableInterrupt();

// Disable UART 1 interrupt and return the previuos interrupt state                                        
BYTE Uart1DisableInterrupt();

// Disable UART 0 interrupt and return the previuos interrupt state                                        
BYTE Uart0EnableInterrupt();

// Disable UART 1 interrupt and return the previuos interrupt state                                        
BYTE Uart1EnableInterrupt();




#endif	