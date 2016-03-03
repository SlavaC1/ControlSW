/*===========================================================================
 *   FILENAME       : Uart driver {UartDrv.c}  
 *   PURPOSE        : Driver to the internal uart of the processor  
 *   DATE CREATED   : 11/Nov/2001
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
#include "Uartdrv.h"
#ifdef OCB2
#include "c8051F120.h"
#else
#include "c8051F020.h"
#endif
#include "RingBuff.h"
#include "TimerDrv.h"



// Constants
// =========
#define UART_TX_BUFFER_SIZE		 256
#define UART_1_RI_MASK           0x01
#define UART_1_TI_MASK           0x02
#define UART_1_RESET_RI_MASK     0xFE
#define UART_1_RESET_TI_MASK     0xFD
#define DISABLE_UART_1_INT_MASK  0xBF
#define ENABLE_UART_1_INT_MASK   0x40
#define UART_1_INT_MASK          0x40

// Type definitions
// ================
typedef struct{
  BOOL TxBusy;
  BYTE TxBuffer[UART_TX_BUFFER_SIZE];  
  BYTE TxBufferHead;
  BYTE TxBufferTail;
  BYTE TxBufferFreeSpace;
  RBUF_RingBuffer RcvRingBuffer;
}TUartStatus;

	
 
// Local routines
// ==============
UART_STATUS Uart0Send(BYTE *Data, BYTE Length);
UART_STATUS Uart1Send(BYTE *Data, BYTE Length);


// Module variables
// ================
TUartStatus xdata UartsStatus[2];
 



/****************************************************************************
 *
 *  NAME        : Uart0Init - Implementation for OCB2 (F120 CPU)
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Initialization of UART 0.                                        
 *
 ****************************************************************************/
#ifdef OCB2
void Uart0Init(void)
{
  bit SaveEnableAll = EA;
  	   
  EA             = DISABLE;         // Disable Interrupts

  SFRPAGE = UART0_PAGE;
  SSTA0         |= 0x10;            // Do not devide baud rate by two.  
  SSTA0         |= 0x0F;            // Set Timer 4 Overflow to generate UART0 RX & TX baud rates
  SCON0          = 0x50;            // SCON0.Bits7�6:  Mode 1: 8-Bit UART, Variable Baud Rate
  									// SCON0.Bit4:     REN0: Receive Enable

  Timer_4_Init();					// Initialize timer 4 for the baud rate clock

  TI0            = CLEAR;           // TI:   (SCOND0.Bit1) set TI to send first char of UART
  RI0            = CLEAR;           // RI:   (SCOND0.Bit0) clear the receive interrupt
  ES0			 = SET;             // Enable serial interrupt
  PS             = SET;             // interrupt priority to the serial port
  EA             = SaveEnableAll;

  SFRPAGE = CONFIG_PAGE;  // Switch to configuration page, for regs: XBR0..XBR2
  
// enable UART 0 port pins
// -------------------------
	XBR0 |= 0x04;

// enable the crossbar
// -------------------
	XBR2 |= 0x40;

	UartsStatus[UART_0].TxBusy = FALSE;
	UartsStatus[UART_0].TxBufferHead = 0;
	UartsStatus[UART_0].TxBufferTail = 0;
	UartsStatus[UART_0].TxBufferFreeSpace = UART_TX_BUFFER_SIZE - 1; 
}
#endif

/****************************************************************************
 *
 *  NAME        : Uart0Init - Implementation for OCB1 (F020 CPU)
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : Initialization of UART 0.                                        
 *
 ****************************************************************************/
#ifndef OCB2
void Uart0Init(void)
{
  bit SaveEnableAll = EA; 
  EA             = DISABLE;                  /* Disable Interrupts */
  SCON0          = 0x50;                     // SCON: mode 1, 8-bit UART, enable rcvr

// Initialize timer 1, which is used for the baud rate clock generator for UART 1
  Timer_1_Init();

  TI0            = CLEAR;                    // TI:   set TI to send first char of UART
  RI0            = CLEAR;                    // RI:  clear the receive interrupt
  REN0           = SET;                      // Enable reception
  ES0		     = 1;                        // Enable serial interrupt
  PCON          |= 0x80;                     // Set Double baud rate bit
  PCON          |= 0x40;                     
  PS             = SET;                      // interrupt priority to the serial port
  EA             = SaveEnableAll;
 
// enable UART 0 port pins
// -------------------------
	XBR0 |= 0x04;

// enable the crossbar
// -------------------
	XBR2 |= 0x40;

	UartsStatus[UART_0].TxBusy = FALSE;
	UartsStatus[UART_0].TxBufferHead = 0;
	UartsStatus[UART_0].TxBufferTail = 0;
	UartsStatus[UART_0].TxBufferFreeSpace = UART_TX_BUFFER_SIZE - 1; 
}
#endif

/****************************************************************************
 *
 *  NAME        : Uart1Init - Implementation for OCB2 (F120 CPU)
 *
 *  DESCRIPTION : Initialization of UART 1.                                        
 *
 ****************************************************************************/
#ifdef OCB2
void Uart1Init(void)
{

// Disable UART 1 interrupt
  EIE2 &= DISABLE_UART_1_INT_MASK;

  SFRPAGE = UART1_PAGE;	

// Set UART 1 to mode 1 8-Bit UART, Variable Baud Rate, and Enable the UART receiver.
  SCON1 = 0x10;

// Reset the transmit and receive interrupt flags
  SCON1 &= 0xFC;

// Set the interrupt priority to high
  EIP2 |= 0x40; 

// Initialize timer 1, which is used for the baud rate clock generator for UART 1
  Timer_1_Init();

  SFRPAGE = CONFIG_PAGE;	

// Enable UART 1 port pins
  XBR2 |= 0x04;

// Enable the crossbar
  XBR2 |= 0x40;
  
  UartsStatus[UART_1].TxBusy = FALSE;
  UartsStatus[UART_1].TxBufferHead = 0;
  UartsStatus[UART_1].TxBufferTail = 0;
  UartsStatus[UART_1].TxBufferFreeSpace = UART_TX_BUFFER_SIZE - 1; 


// Enable UART 1 interrupt
  EIE2 |= ENABLE_UART_1_INT_MASK;
}
#endif

/****************************************************************************
 *
 *  NAME        : Uart1Init - Implementation for OCB1 (F020 CPU)
 *
 *  DESCRIPTION : Initialization of UART 1.                                        
 *
 ****************************************************************************/
#ifndef OCB2
void Uart1Init(void)
{

// Disable UART 1 interrupt
  EIE2 &= DISABLE_UART_1_INT_MASK;

// Reset SSAT1 bit on PCON reg to allow the change in the UART mode
  PCON &= 0xF7;
 
// Set UART 1 to mode 1 
  SCON1 &= 0x7F;
  SCON1 |= 0x40;

// Set SSAT1 bit on PCON reg to allow the use of the SCON bits 6,7 as status indicators
  PCON |= 0x08;

// Enable the UART receiver
  SCON1 |= 0x10;

// Reset the transmit and receive interrupt flag
  SCON1 &= 0xFC;

// Set the interrupt priority to high
  EIP2 |= 0x40; 

// Initialize timer 4 for the baud rate clock
  Timer_4_Init();

// Enable UART 0 port pins
  XBR2 |= 0x04;

// Enable the crossbar
	XBR2 |= 0x40;

	UartsStatus[UART_1].TxBusy = FALSE;
	UartsStatus[UART_1].TxBufferHead = 0;
	UartsStatus[UART_1].TxBufferTail = 0;
	UartsStatus[UART_1].TxBufferFreeSpace = UART_TX_BUFFER_SIZE - 1; 


// Enable UART 1 interrupt
  EIE2 |= ENABLE_UART_1_INT_MASK;

}
#endif

 /****************************************************************************
 *
 *  NAME        : Uart0Isr
 *
 *  INPUT       : None
 *
 *  OUTPUT      : None.
 *
 *  DESCRIPTION : UART 0 Interrupt Service Routine.                                        
 *
 ****************************************************************************/
void Uart0Isr() interrupt 4 using 2
{
  BYTE xdata Data	;
// Disable UART interrupts
 ES0 = DISABLE;	 

#ifdef OCB2
// NOTE: SFRPAGE register will automatically be switched to the Uart0Isr Page When an interrupt occurs.  SFRPAGE will return to its previous setting on exit from this routine.  

 // Check if a frame error occured 
 if (SSTA0 & 0x80) // FE0 flag
   SSTA0 &= ~0x80;

 // Check if a receive overrun error occured 
 if (SSTA0 & 0x40) // RXOV0 flag
   SSTA0 &= ~0x40;

#else

 // Check if a frame error occured 
 if (SM00)
   SM00 = 0;

 // Check if a receive overrun error occured 
 if (SM10)
   SM10 = 0;

#endif

 if(RI0)                                                    
 {
   RI0 = 0;
   Data = SBUF0;
   RingBuffInsert(&(UartsStatus[UART_0].RcvRingBuffer),Data); 
 } 

 if(TI0)
 {
	 TI0 = 0;
	 if (UartsStatus[UART_0].TxBufferHead != UartsStatus[UART_0].TxBufferTail)
	 {
	 	 SBUF0 = UartsStatus[UART_0].TxBuffer[UartsStatus[UART_0].TxBufferTail];
		 UartsStatus[UART_0].TxBufferTail = (UartsStatus[UART_0].TxBufferTail+1) % UART_TX_BUFFER_SIZE;
		 UartsStatus[UART_0].TxBufferFreeSpace++;
   }
	 else
	 	UartsStatus[UART_0].TxBusy	 = FALSE;
	   
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
// Disable UART 1 interrupt
  EIE2 &= DISABLE_UART_1_INT_MASK;

// NOTE: The SFRPAGE register will automatically be switched to the Uart1Isr Page
// When an interrupt occurs.  SFRPAGE will return to its previous setting on exit
// from this routine.  

 if(SCON1 & UART_1_RI_MASK)                                                    
 {
   SCON1 &= UART_1_RESET_RI_MASK;
   RingBuffInsert(&(UartsStatus[UART_1].RcvRingBuffer),SBUF1); 
 } 

 if(SCON1 & UART_1_TI_MASK)
 {
   SCON1 &= UART_1_RESET_TI_MASK;
	 if (UartsStatus[UART_1].TxBufferHead != UartsStatus[UART_1].TxBufferTail)
	 {
	 	 SBUF1 = UartsStatus[UART_1].TxBuffer[UartsStatus[UART_1].TxBufferTail];
		 UartsStatus[UART_1].TxBufferTail = (UartsStatus[UART_1].TxBufferTail+1) % UART_TX_BUFFER_SIZE;
		 UartsStatus[UART_1].TxBufferFreeSpace++;
   }
	 else
	 	UartsStatus[UART_1].TxBusy	 = FALSE;
	   
 } 

// Enable UART 1 interrupt
  EIE2 |= ENABLE_UART_1_INT_MASK;
}

/****************************************************************************
 *
 *  NAME        : UartSend
 *
 *  INPUT       : Data buffer, data length, UART number.
 *
 *  OUTPUT      : UART_STATUS.
 *
 *  DESCRIPTION : send a data buffer using UART 0 or 1.                                        
 *
 ****************************************************************************/
UART_STATUS UartSend(BYTE *Data, BYTE Length, TUartNum UartNum)
{ 
	UART_STATUS xdata Status;
	
	if (UartNum == UART_0)
		Status = Uart0Send(Data, Length);
	
	if (UartNum == UART_1)
		Status = Uart1Send(Data, Length);
	
	return Status;
}


UART_STATUS Uart0Send(BYTE *Data, BYTE Length)
{
	BYTE xdata i;
	BYTE xdata PrevIntStatus; 
  TUartStatus *Uart0Status = &UartsStatus[UART_0];

#ifdef OCB2
  // Select the SFR for UART 0 Page.
    SFRPAGE = UART0_PAGE;
#endif

// check if there is enough room in the transmit buffer
	if (Uart0Status->TxBufferFreeSpace < Length)
	{
		return UART_TX_BUFFER_FULL;
	}

// copy the data buffer to the Tx buffer
	for (i = 0; i < Length; i++)
	{
    PrevIntStatus = ES0;
    ES0 = DISABLE;

		Uart0Status->TxBuffer[Uart0Status->TxBufferHead] = Data[i];

		// incerement the head index
    Uart0Status->TxBufferHead = (Uart0Status->TxBufferHead + 1) % UART_TX_BUFFER_SIZE;

    // update the free buffer space 
		Uart0Status->TxBufferFreeSpace--;

   	ES0 = PrevIntStatus;
	}

  PrevIntStatus = ES0;
  ES0 = DISABLE;
// if the uart is free transmit the first byte - initiate an interrupt
	if (!Uart0Status->TxBusy)
	{
		Uart0Status->TxBusy = TRUE;
		SBUF0 = Uart0Status->TxBuffer[Uart0Status->TxBufferTail];

		Uart0Status->TxBufferTail = (Uart0Status->TxBufferTail+1) % UART_TX_BUFFER_SIZE;
		Uart0Status->TxBufferFreeSpace++;
  }
 	ES0 = PrevIntStatus;

  return UART_NO_ERROR;
}


UART_STATUS Uart1Send(BYTE *Data, BYTE Length)
{
	BYTE xdata i;
	BYTE xdata PrevIntStatus; 
  TUartStatus *Uart1Status = &UartsStatus[UART_1];

#ifdef OCB2
  // Select the SFR for UART 1 Page.
    SFRPAGE = UART1_PAGE;
#endif

// check if there is enough room in the transmit buffer
	if (Uart1Status->TxBufferFreeSpace < Length)
	{
		return UART_TX_BUFFER_FULL;
	}

// copy the data buffer to the Tx buffer
	for (i = 0; i < Length; i++)
	{
    PrevIntStatus = EIE2 & UART_1_INT_MASK;
    EIE2 &= DISABLE_UART_1_INT_MASK;

		Uart1Status->TxBuffer[Uart1Status->TxBufferHead] = Data[i];

		// incerement the head index
    Uart1Status->TxBufferHead = (Uart1Status->TxBufferHead + 1) % UART_TX_BUFFER_SIZE;

    // update the free buffer space 
		Uart1Status->TxBufferFreeSpace--;

    EIE2 |= PrevIntStatus;
	}

   PrevIntStatus = EIE2 & UART_1_INT_MASK;
   EIE2 &= DISABLE_UART_1_INT_MASK;
// if the uart is free transmit the first byte - initiate an interrupt
	if (!Uart1Status->TxBusy)
	{
		Uart1Status->TxBusy = TRUE;
		SBUF1 = Uart1Status->TxBuffer[Uart1Status->TxBufferTail];

		Uart1Status->TxBufferTail = (Uart1Status->TxBufferTail+1) % UART_TX_BUFFER_SIZE;
		Uart1Status->TxBufferFreeSpace++;
  }
  EIE2 |= PrevIntStatus;

  return UART_NO_ERROR;
}

/****************************************************************************
 *
 *  NAME        : UartGetReceiveBuffer
 *
 *  INPUT       : UART number
 *
 *  OUTPUT      : RBUF_RingBuffer.
 *
 *  DESCRIPTION : Returns a pointer to the receive data buffer.                                        
 *
 ****************************************************************************/
RBUF_RingBuffer *UartGetReceiveBuffer(TUartNum UartNum)
{
  return &(UartsStatus[UartNum].RcvRingBuffer);
}


/****************************************************************************
 *
 *  NAME        : Uart0DisableInterrupt
 *
 *  DESCRIPTION : Disable UART 0 interrupt and return the previuos interrupt state                                        
 *
 ****************************************************************************/
BYTE Uart0DisableInterrupt()
{
  BYTE xdata PrevIntStatus = ES0;
	ES0 = DISABLE;
  return PrevIntStatus;
}


/****************************************************************************
 *
 *  NAME        : Uart1DisableInterrupt
 *
 *  DESCRIPTION : Disable UART 1 interrupt and return the previuos interrupt state                                        
 *
 ****************************************************************************/
BYTE Uart1DisableInterrupt()
{
  BYTE xdata PrevIntStatus = EIE2 & UART_1_INT_MASK;
  EIE2 &= DISABLE_UART_1_INT_MASK;
  return PrevIntStatus;
}


/****************************************************************************
 *
 *  NAME        : Uart0EnableInterrupt
 *
 *  DESCRIPTION : Disable UART 0 interrupt and return the previuos interrupt state                                        
 *
 ****************************************************************************/
BYTE Uart0EnableInterrupt()
{
  BYTE xdata PrevIntStatus = ES0;
	ES0 = ENABLE;
  return PrevIntStatus;
}


/****************************************************************************
 *
 *  NAME        : Uart1EnableInterrupt
 *
 *  DESCRIPTION : Disable UART 1 interrupt and return the previuos interrupt state                                        
 *
 ****************************************************************************/
BYTE Uart1EnableInterrupt()
{
  BYTE xdata PrevIntStatus = EIE2 & UART_1_INT_MASK;
  EIE2 |= ENABLE_UART_1_INT_MASK;
  return PrevIntStatus;
}
 