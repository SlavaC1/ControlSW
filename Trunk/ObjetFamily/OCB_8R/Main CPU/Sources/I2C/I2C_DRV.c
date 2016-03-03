/*************************************************************************   
* FILENAME       : I2C {I2C_DRV.c}  									 *
**************************************************************************
*  																	     *
*  PURPOSE:   															 *
*    																	 *
*   																	 *
*  Interrupt-driven SMBus implementation in Master mode.  			     *
*  Only master states defined (no slave or arbitration)   			     *
*  multiple-byte SMBus data holders used for each transmit and receive   *  
*  Timer3 used by SMBus for SCL low timeout detection   				 *
*  SCL frequency defined by <SMB_FREQUENCY> constant   				     *
*  ARBLOST support included   										     *
*  supports multiple-byte writes and multiple-byte reads   			     *
*  Pinout:   															 *
*   P0.6 -> SDA (SMBus)   											     *
*   P0.7 -> SCL (SMBus)          										 *
**************************************************************************   
* PROGRAMER: Elad Hazan  												 *
**************************************************************************/   

#include "c8051F120.h"

#include "Define.h" 
#include "I2C_DRV.h" 
#include "MiniScheduler.h"
#include "TimerDrv.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"  
#include "string.h" 
#include "MSCInterface.h"  
//-----------------------------------------------------------------------------   
// Global VARIABLES   
//-----------------------------------------------------------------------------      

BYTE data TARGET = 0;         // Target SMBus slave address    
bit SMB_BUSY;                 // Software flag to indicate when the SMB_Read() or SMB_Write() functions have claimed the SMBus  

bit SMB_RW;                  // Software flag to indicate the direction of the current transfer  
// 16-bit SFR declarations   
sfr16    TMR3RL   = 0x92;   // Timer3 reload registers   
sfr16    TMR3     = 0x94;  // Timer3 counter registers   
sbit SDA = P0^6;           // SMBus on P0.0   
sbit SCL = P0^7;           // and P0.1   
BYTE data i=0; 
BYTE data j=0;
BYTE data InitDelay = 0; 


TFuncPtr ReadCompletedCallback;
TErrorFuncPtr OnFailCallback;


PBYTE SMB_DATA_OUT;
PBYTE SMB_DATA_IN;
BYTE data SMB_DATA_IN_SIZE = 0;
BYTE data SMB_DATA_OUT_SIZE = 0;


#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC 
BYTE data I2CDebugIndex = 0; 
TI2CDebugMsg data DMsg;
#endif   

/****************************************************************************
*																			*
*  NAME        : I2C_SetOnOff												*
*																			*
*  DESCRIPTION : Set on or off the  I2C driver, depends on the 			*
*                24V switch, which activates the I2C driver along with		*
*                the OHDB.													*
*                This function is the second part of the SMBus_Init()		*
*                but actualy the main part of activating the driver        *            
*																			*
****************************************************************************/  
void I2C_SetOnOff(BOOL OnOff)
{
	// For: 	SMB0CN, SMB0CR and all SMBus registers.
	// 		Note that all Interrupt registers (EIE1...) are accessible from all pages. 
	SFRPAGE = SMB0_PAGE;

	if (OnOff)
	{	
		SMB0CN = 0x07;          // Assert Acknowledge low (AA bit = 1b)
		SMB0CN |= 0x40;         // Enable SMBus;
		SMB0CR = 0xF4;			// Configure SMBug bitrate to 100 kHz
		// 		T_LOW  = 4 x (256 - SMB0CR) / SYSCLK
		// 		T_HIGH = 4 x (258 - SMB0CR) / SYSCLK + 625ns
		
		EIP1 |= 0x02;			// Set the I2C priority level to high
		EIE1 |= ESMB0;			// enable SMBus interrupts
		SI    = 0;              // Clear SMBUS interrupt Flag.
		MSCStartStop(OnOff);    //Activating the MSC Reading Task.	
	}
	else 
	{	
		MSCStartStop(OnOff);    //Activating the MSC Reading Task.	
		EIE1   &= ~ESMB0;
		
		SMB0CN  &= ~0x40; // Reset communication   
		SMB0CN  |= 0x40;   
		STA      = 0;   
		STO      = 0;   
		AA       = 0;   
		SMB_BUSY = 0;  // Free SMBus		
		SI       =0;  // Clear SMBUS interrupt Flag.
	}
}

void SMBus_Init (void)   
{                 
	SFRPAGE = CONFIG_PAGE;

	XBR0  |= 0x01;          // SMBus0 SDA and SCL routed to 2 Port pins.		 
}   


BYTE I2CWrite(BYTE ChipAddress, BYTE *Data, BYTE Size)
{
	SFRPAGE           = SMB0_PAGE;
	SMB_DATA_OUT      = Data; // Define next outgoing byte 
	SMB_DATA_OUT_SIZE = Size;
	TARGET            = ChipAddress;

	//Write operation
	SMB_BUSY = BUS_BUSY;                       // Claim SMBus (set to busy)   
	SMB_RW   = I2C_WRITE_OP;                     // Mark this transfer as a WRITE   
	STA      = 1;
	
	return TRUE;	
}

BYTE I2CRead(BYTE ChipAddress, BYTE *Data, BYTE Size, TFuncPtr callback, TFuncPtr OnMSCFailCallback)
{
	SFRPAGE               = SMB0_PAGE;
	SMB_DATA_IN	          = (BYTE*)Data;
	SMB_DATA_IN_SIZE      = Size;
	ReadCompletedCallback = callback;
	OnFailCallback        = OnMSCFailCallback; 
	TARGET                = ChipAddress;
	
	//Read operation
	SMB_BUSY = BUS_BUSY;                     // Claim SMBus (set to busy)   
	SMB_RW   = I2C_READ_OP;                  // Mark this transfer as a READ   
	STA      = 1;

	return TRUE;
}

BYTE I2CBusBusy()
{
	if(SMB_BUSY == BUS_BUSY)
		return TRUE;
	
	return FALSE;
}

/*****************************************************************************************
*																						 *
*  NAME        : SMBUS_ISR																 *
*																						 *
*  DESCRIPTION : SMBUS interrupt service routine.										 *
*                Master only implementation - no slave or arbitration states defined.   *                                     
*				  All incoming data is written to global array <SMB_DATA_IN>.   		 *
*				  All outgoing data is read from global array <SMB_DATA_OUT>.   		 *
*																						 *
****************************************************************************************/   

void SMBUS_ISR (void) interrupt 7   
{   
	bit FAIL = 0;                                                                
	static BYTE idata Master_Transmitter_Address_Arbitration_Lost = 0;
	static BYTE idata Master_Transmitter_Data_Arbitration_Lost    = 0;
	static BYTE idata Connection_Lost                             = 0;
	static BYTE idata Arbitration_Lost                            = 0;
	static BYTE idata sent_byte_counter;   
	static BYTE idata rec_byte_counter;   
	static BYTE idata TargetToCheck;

	SFRPAGE = SMB0_PAGE;

	// Status code for the SMBus (SMB0STA register)   
	switch(SMB0STA)   
	{   
		// Master Transmitter/Receiver: START condition transmitted.   
		// Load SMB0DAT with slave device address.   
		case SMB_START:
		{		  
			SMB0DAT  = TARGET;      // Load address of the slave.   
			SMB0DAT &= 0xFE;        // Clear the LSB of the address for the   
			// R/W bit   
			SMB0DAT |= SMB_RW;      // Load R/W bit   
			STA = 0;                // Manually clear STA bit   

			rec_byte_counter  = 1;  // Reset the counter   
			sent_byte_counter = 1;  // Reset the counter
			TargetToCheck     = TARGET;
			
			// Debug - Add to log
			if(SMB_RW == I2C_WRITE_OP )
			{	
				BeginDebugMessage();
				AppendToDebugMessage(SMB0DAT);
			}
		}
		break;
		
		// Master Transmitter/Receiver: repeated START condition transmitted.   
		// Load SMB0DAT with slave device address   
		case SMB_RP_START:
		{			
			SMB0DAT  = TARGET;     // Load address of the slave.   
			SMB0DAT &= 0xFE;       // Clear the LSB of the address for the   
			// R/W bit   
			SMB0DAT |= SMB_RW;     // Load R/W bit		
			
			STA = 0;               // Manually clear STA bit   

			rec_byte_counter  = 1; // Reset the counter   
			sent_byte_counter = 1; // Reset the counter   			
		}
		break;   

		// Master Transmitter: Slave address + WRITE transmitted.  ACK received.   
		// For a READ: N/A   
		//   
		// For a WRITE: Send the first data byte to the slave.   
		case SMB_MTADDACK:
		{			
			SMB0DAT = SMB_DATA_OUT[sent_byte_counter - 1];   

			// Debug - Add to log
			AppendToDebugMessage(SMB0DAT);

			sent_byte_counter++; 
		}
		break;
		
		// Master Transmitter: Slave address + WRITE transmitted.  NACK received.   
		// Restart the transfer.   
		case SMB_MTADDNACK:
		{			
			if(Master_Transmitter_Address_Arbitration_Lost++) //Added in order to prevent connection lost  
			{  					                               // between OCB & Embedded While trying to reach un-existing Address
				
				Master_Transmitter_Address_Arbitration_Lost = 0;
				FAIL                                        = 1; 

				break;  
			}
			
			STA = 1; // Restart transfer
		}			
		break;   

		// Master Transmitter: Data byte transmitted.  ACK received.   
		// For a READ: N/A   
		//   
		// For a WRITE: Send all data.  After the last data byte, send the stop   
		//  bit.   
		case SMB_MTDBACK:   
		{
			if(sent_byte_counter <= SMB_DATA_OUT_SIZE)   
			{   
				// send data byte   
				SMB0DAT = SMB_DATA_OUT[sent_byte_counter - 1];   
				sent_byte_counter++;			
			}   
			else   
			{  
				STO      = 1;   // Set STO to terminate transfer   
				SMB_BUSY = 0;   // And free SMBus interface
				
				// Debug - Add to log
				EndDebugMessage(); // don't check if Read or Write OP. This must be Write.           
			}
		}			
		break;   

		// Master Transmitter: Data byte transmitted.  NACK received.   
		// Restart the transfer.   
		case SMB_MTDBNACK:   
		{
			if(Master_Transmitter_Data_Arbitration_Lost++) //Added in order to prevent connection lost  between OCB & Embedded
			{  					                           // While trying to reach un-existing Address.
				Master_Transmitter_Data_Arbitration_Lost = 0;
				FAIL                                     = 1; 

				break;  
			}
			/*******************************************************/
			STA = 1; // Restart transfer   
		}
		break;   

		//  Master Receiver: Slave address + READ transmitted.  ACK received.   
		//  For a READ: check if this is a one-byte transfer. if so, set the   
		//  NACK after the data byte is received to end the transfer. if not,   
		//  set the ACK and receive the other data bytes.   
		//   
		// For a WRITE: N/A   
		case SMB_MRADDACK:   
		{
			if(rec_byte_counter == SMB_DATA_IN_SIZE)   
			{   
				AA = 0; // Only one byte in this transfer, send NACK after byte is received   
			}   
			else   
			{   
				AA = 1; // More than one byte in this transfer, send ACK after byte is received   
			}   
		}
		break;   

		// Master Receiver: Slave address + READ transmitted.  NACK received.   
		// Restart the transfer.   
		case SMB_MRADDNACK:   
		{
			if(Connection_Lost++ > ON_OFF_DELAY) //Added in order to prevent connection lost  between OCB & Embedded
			{  					                 // While trying to reach un-existing  or Disconnected	Address.		 		   		  
				
				if(InitDelay++ > 10) //Adding inner delay in order to prevent pop-up error message during application initialization
					OnFailCallback(TargetToCheck); 				

				Connection_Lost = 0;
				FAIL            = 1; 
				break;  
			}
			
			STA = 1; // Restart transfer
		}			
		break;   

		// Master Receiver: Data byte received.  ACK transmitted.   
		// For a READ: receive each byte from the slave.  if this is the last   
		//  byte, send a NACK and set the STOP bit.   
		//   
		// For a WRITE: N/A   
		case SMB_MRDBACK:   
		{
			if(rec_byte_counter <= NUM_INCOMING_BYTES - 1)   
			{   
				SMB_DATA_IN[rec_byte_counter - 1] = SMB0DAT; // Store received byte   
				
				// Send ACK to indicate byte received   
				if(rec_byte_counter == (NUM_INCOMING_BYTES - 1)) 
					AA = 0;
				else 
					AA = 1;
				
				rec_byte_counter++;  // Increment the byte counter 					  
			}
		}			
		break;   

		// Master Receiver: Data byte received.  NACK transmitted.   
		// For a READ: Read operation has completed.  Read data register and   
		//  send STOP.   
		//   
		// For a WRITE: N/A   
		case SMB_MRDBNACK:   
		{
			SMB_DATA_IN[rec_byte_counter - 1] = SMB0DAT; // Store received byte   
			STO      = 1;   
			SMB_BUSY = 0;			
			AA       = 1; // Set AA for next transfer 
			
			// On completion of read operation, call a callback to notify the user.
			ReadCompletedCallback();
			InitDelay       = 0;
			Connection_Lost = 0;	 
		}
		break;   

		// Master Transmitter: Arbitration lost.   
		case SMB_MTARBLOST:   
		{
			if(Arbitration_Lost++) //Added in order to prevent connection lost  between OCB & Embedded
			{  					   // While trying to reach un-existing Address.
				Arbitration_Lost = 0;
				FAIL             = 1; 

				break;  
			}

			STA = 1; // Restart transfer   
		}
		break;   

		// All other status codes invalid.  Reset communication.   
		default:
		{			
			FAIL = 1;      
		}
		break;   
	}   

	if(FAIL) // If the transfer failed,   
	{ 		
		SMB0CN  &= ~0x40; // Reset communication   
		SMB0CN  |= 0x40;   
		STA      = 0;   
		STO      = 0;   
		AA       = 0;      
		SMB_BUSY = 0;    // Free SMBus   	     
		FAIL     = 0;   
	}

	SI = 0; // Clear interrupt flag  
}   



#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC  
void BeginDebugMessage()
{
	I2CDebugIndex = 0;
	memset(DMsg.I2CWriteOpReport, 0, 2);

}

void EndDebugMessage()
{
	DMsg.MsgId = I2C_DEBUG_MSG;

	EdenProtocolSend((BYTE*)&DMsg,sizeof(TI2CDebugMsg),EDEN_ID,OCB_ID,
	0,FALSE,EDEN_OCB_PROTOCOL);

}

void AppendToDebugMessage(BYTE DataReg)
{
	
	DMsg.I2CWriteOpReport[I2CDebugIndex++]= DataReg;
	
}
#else
void BeginDebugMessage()
{ 
}

void EndDebugMessage()
{
}

void AppendToDebugMessage(BYTE DataReg)
{
	BYTE ii = DataReg; // just to avoid a compiler warning.
}
#endif



