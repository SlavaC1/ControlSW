/*===========================================================================
 *   FILENAME       : Uart driver {UartDrv.c}  
 *   PURPOSE        : Driver to the internal uart of the processor  
 *   DATE CREATED   : 11/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
#include "Uartdrv.h"
#include "c8051F020.h"
#include "RingBuff.h"


// Constants
// =========
#define UART_TX_BUFFER_SIZE		100
#define SYSCLK        		   	11059200           // Oscillator Frequency 
#define BAUDRATE        			9600               // baudrate init value 



// Type definitions
// ================

	
 
// Local routines
// ==============


// Module variables
// ================
bit UartTxBusy;
BYTE xdata UartTxBuffer[UART_TX_BUFFER_SIZE];  
BYTE xdata UartTxBufferHead;
BYTE xdata UartTxBufferTail;
BYTE xdata UartTxBufferFreeSpace;
RBUF_RingBuffer xdata RcvRingBuffer;





/****************************************************************************
 *
 *  NAME        : Uart0Init
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Initialization of UART 0.                                        
 *
 ****************************************************************************/
void Uart0Init(void)
{
  bit SaveEnableAll = EA; 
  EA             = DISABLE;                                      /* Disable Interrupts */
  SCON0           = 0x50;                                  // SCON: mode 1, 8-bit UART, enable rcvr
  TMOD          |= 0x20;                                  // TMOD: timer 1, mode 2, 8-bit reload
  TH1            = -(SYSCLK/BAUDRATE/16);                 // set Timer1 reload value for baudrate
  CKCON         |= 0x10;                                  // Timer1 uses SYSCLK as time base
  TR1            = SET;                                   // TR1:  timer 1 run
  TI0             = CLEAR;                                   // TI:   set TI to send first char of UART
  RI0             = CLEAR;                                   // RI:  clear the receive interrupt
  REN0            = SET;                                   // Enable reception
  ES0						 = 1;                                   // Enable serial interrupt
  TCON          |= 0x40;                                  // Set timer 1 ON
  PCON          |= 0x80;                                  // Set Double baud rate bit
  PS             = SET;                                   // interrupt priority to the serial port
  EA             = SaveEnableAll;
  
// enable UART 0 port pins
// -------------------------
	XBR0 |= 0x04;

// enable the crossbar
// -------------------
	XBR2 |= 0x40;

	UartTxBusy = FALSE;
	UartTxBufferHead = 0;
	UartTxBufferTail = 0;
	UartTxBufferFreeSpace = UART_TX_BUFFER_SIZE - 1; 

}

/****************************************************************************
 *
 *  NAME        : Uart1Init
 *
 *  DESCRIPTION : Initialization of UART 1.                                        
 *
 ****************************************************************************/
void Uart1Init(void)
{

// Disable serial interrupt
// ------------------------
  EIE2 &= 0xBF;

// Disable recieve enable of UART 1
// -------------------------------
  SCON1 &= 0xEF;

// Enable UART 0 port pins
// -------------------------
  XBR2 |= 0x04;

// Enable the crossbar
// -------------------
	XBR2 |= 0x40;

}

 /****************************************************************************
 *
 *  NAME        : Uart0Isr
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : The UART Interrupt Service Routine.                                        
 *
 ****************************************************************************/
void Uart0Isr() interrupt 4 using 2
{
	
// Disable UART interrupts
// ------------------
	ES0 = DISABLE;	 


 if(RI0)                                                    
 {
   RI0 = 0;
   RingBuffInsert(&RcvRingBuffer); 
 } 

 if(TI0)
 {
	 TI0 = 0;
	 if (UartTxBufferHead != UartTxBufferTail)
	 {
	 	 SBUF0 = UartTxBuffer[UartTxBufferTail];
		 UartTxBufferTail = (UartTxBufferTail+1) % UART_TX_BUFFER_SIZE;
		 UartTxBufferFreeSpace++;
   }
	 else
	 	UartTxBusy	 = FALSE;
	   
 } 
// Enable UART interrupts
// -----------------
	ES0 = ENABLE;
}


 /****************************************************************************
 *
 *  NAME        : Uart1Isr
 *
 *  DESCRIPTION : UART 1 Interrupt Service Routine.                                        
 *
 ****************************************************************************/
void Uart1Isr() interrupt 20 using 2
{
}

/****************************************************************************
 *
 *  NAME        : UartSend
 *
 *  INPUT       : Data buffer, data length.
 *
 *  OUTPUT      : UART_STATUS.
 *
 *  DESCRIPTION : send a data buffer using the UART.                                        
 *
 ****************************************************************************/
UART_STATUS UartSend(BYTE *Data, BYTE Length)
{
	BYTE xdata i;
	bit PrevIntStatus; 
 
// check if there is enough room in the transmit buffer
// ----------------------------------------------------
	if (UartTxBufferFreeSpace < Length)
	{
		return UART_TX_BUFFER_FULL;
	}

	PrevIntStatus = ES0;
	ES0 = DISABLE;

// copy the data buffer to the Tx buffer
// -------------------------------------
	for (i = 0; i < Length; i++)
	{
		UartTxBuffer[UartTxBufferHead] = Data[i];
		// incerement the head index
		// -------------------------
    UartTxBufferHead = (UartTxBufferHead + 1) % UART_TX_BUFFER_SIZE;
	}

// update the free buffer space 
// ----------------------------
		UartTxBufferFreeSpace -= Length;
 

// if the uart is free transmit the first byte - initiate an interrupt
// -------------------------------------------------------------------
	if (!UartTxBusy)
	{
		UartTxBusy = TRUE;
		SBUF0 = UartTxBuffer[UartTxBufferTail];
		UartTxBufferTail = (UartTxBufferTail+1) % UART_TX_BUFFER_SIZE;
		UartTxBufferFreeSpace++;
  }

	ES0 = PrevIntStatus;
  return UART_NO_ERROR;
}


/****************************************************************************
 *
 *  NAME        : UartGetReceiveBuffer
 *
 *  INPUT       : None.
 *
 *  OUTPUT      : RBUF_RingBuffer.
 *
 *  DESCRIPTION : Returns a pointer to the receive data buffer.                                        
 *
 ****************************************************************************/
RBUF_RingBuffer *UartGetReceiveBuffer(void)
{
  return &RcvRingBuffer;
}

