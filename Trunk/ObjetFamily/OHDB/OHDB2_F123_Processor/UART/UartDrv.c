/*===========================================================================
 *   FILENAME       : Uart driver {UartDrv.c}  
 *   PURPOSE        : Driver to the internal uart of the processor  
 *   DATE CREATED   : 11/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/

#include "c8051F120.h"
#include "Uartdrv.h"
#include "RingBuff.h"
#include "TimerDrv.h"

#define UART_TX_BUFFER_SIZE	100

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
	
	EA = DISABLE;  // Disable Interrupts
	
	SFRPAGE = UART0_PAGE;
	SSTA0 |= 0x10;     // Do not devide baud rate by two.  
	SSTA0 |= 0x0F;     // Set Timer 4 Overflow to generate UART0 RX & TX baud rates
	SCON0  = 0x50;     // SCON0.Bits7–6:  Mode 1: 8-Bit UART, Variable Baud Rate
					   // SCON0.Bit4:     REN0: Receive Enable
	
	Timer_4_Init();	   // Initialize timer 4 for the baud rate clock
	
	TI0 = CLEAR;           // TI:   (SCOND0.Bit1) set TI to send first char of UART
	RI0 = CLEAR;           // RI:   (SCOND0.Bit0) clear the receive interrupt
	ES0	= SET;             // Enable serial interrupt
	PS  = SET;             // interrupt priority to the serial port
	EA  = SaveEnableAll;  	
	
	UartTxBusy = FALSE;
	UartTxBufferHead = 0;
	UartTxBufferTail = 0;
	UartTxBufferFreeSpace = UART_TX_BUFFER_SIZE - 1; 
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
	ES0 = DISABLE;
	
	// NOTE: SFRPAGE register will automatically be switched to the Uart0Isr Page When an interrupt occurs.  SFRPAGE will return to its previous setting on exit from this routine.  
	
	// Check if a frame error occured 
	if (SSTA0 & 0x80) // FE0 flag
		SSTA0 &= ~0x80;
	
	// Check if a receive overrun error occured 
	if (SSTA0 & 0x40) // RXOV0 flag
		SSTA0 &= ~0x40;	
	
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
			UartTxBusy = FALSE;	
	} 
	
	// Enable UART interrupts	
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

