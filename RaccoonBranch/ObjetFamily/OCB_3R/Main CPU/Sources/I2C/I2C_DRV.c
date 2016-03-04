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
   
#include "c8051F020.h"
#include "Define.h" 
#include "I2C_DRV.h" 
#include "MiniScheduler.h"
#include "TimerDrv.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"  
#include "string.h"   
//-----------------------------------------------------------------------------   
// Global VARIABLES   
//-----------------------------------------------------------------------------      
 
   
BYTE xdata SMB_DATA_IN[NUM_INCOMING_BYTES]={0}; // Global holder for SMBus data All receive data is written here     
BYTE xdata SMB_DATA_OUT[NUM_BYTES_WR]={0};      //Global holder for SMBus data. All transmit data is read from here  

BYTE code ByteShiftLeft = 8;
WORD xdata I2CA2DFirstSampleBuffer[BYTES_TO_WORDS] = {0};
WORD xdata I2CA2DSecondSampleBuffer[BYTES_TO_WORDS]= {0};
         
BYTE xdata TARGET = 0;                         // Target SMBus slave address    
bit SMB_BUSY;                        // Software flag to indicate when the SMB_Read() or SMB_Write() functions   
                                     // have claimed the SMBus      
//volatile BYTE data_count=0;         // SMB_DATA_IN and SMB_DATA_OUT counter in future if needed to write more then one 
							       //Byte to MSC  should use this counter   


bit SMB_RW;                        // Software flag to indicate the direction of the current transfer  

BYTE xdata FirstSecondMSC = 0x01;   	 //Indicates which of the two MSC cards is during process.
BYTE xdata WriteMSC1Pending  = FALSE;     //An indication Flag - indicates if there is a writing request during reading request 
BYTE xdata WriteMSC2Pending  = FALSE;
BYTE xdata WritePendingData[2];	 		 //Temp buffer- will bu used in a case of Writing during reading.

// 16-bit SFR declarations   
sfr16    TMR3RL   = 0x92;          // Timer3 reload registers   
sfr16    TMR3     = 0x94;          // Timer3 counter registers   
sbit SDA = P0^6;                   // SMBus on P0.0   
sbit SCL = P0^7;                   // and P0.1   

BYTE xdata I2C_Error_Notifier = 0;
BYTE xdata CheckMSC1HWError = 0;
BYTE xdata CheckMSC2HWError = 0;
BYTE xdata ErrorFlag = 0;

BYTE xdata TempSMB_DATA_OUT = 0;
BYTE xdata MSC1DataOut = 0; 
BYTE xdata MSC2DataOut = 0;

BYTE xdata SecondSlaveOccurrence = FALSE; 
BYTE xdata FirstSlaveOccurrence = FALSE;  

BYTE xdata I2CInitialize  = FALSE;
//BYTE xdata StopI2C = FALSE; //unused
BYTE xdata CanSendErrorMsg = TRUE; //if true, can send error msg
BYTE xdata InitDelay = 0;

BYTE xdata ISRKeepAliveCheck = 0;

TI2CNotificationMsg xdata Msg;

#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC
BYTE xdata I2CDebugIndex = 0;	
TI2CDebugMsg xdata DMsg;
#endif

TTaskHandle xdata I2CReadWriteMSCTaskHandle;
TTaskHandle xdata I2C_Status_TaskHandle;

/*****************************************************************************
 *																			 *
 *  NAME        : I2CWriteByte											     *
 *																			 *
 *  INPUT       : MSC card address, data to be written .					 *    
 *																			 *												 
 *																			 *
 *  DESCRIPTION : The function recieves the card address and the data to be	 *
 *                be written on the bus (Pumps activation/diactivation),	 *
 *                and lanches the writing operation.						 *
 *                This function supports a "write pending" case as well.     *         
 *				  								 							 *
 ****************************************************************************/
BYTE I2CWriteByte( BYTE address, BYTE dat )
{
       // Mark that we have a pending write operation.
	   // 1. If we are in the middle of a *read* operation, this write will be launched immediatly after the read is completed.
	   // 2. If we are in the middle of a *write* operation, this write will be launched immediatly after the write is completed.
	    if(address == FIRST_CHIP_SELECT)
		{
		  WriteMSC1Pending = TRUE;
		  WritePendingData[FIRSTMSCCARD] = dat;
		}		
		else if(address == SECOND_CHIP_SELECT)
		{
		  WriteMSC2Pending = TRUE;
		  WritePendingData[SECONDMSCCARD] = dat;		
		}  		       	 
   return TRUE ;
}



/****************************************************************************
 *																			* 
 *  NAME        : I2CReadWriteTask										    *	     
 *																			* 
 *  INPUT       : Arg.					     								*
 *																			* 												 
 *																			* 
 *  DESCRIPTION : A routing task that reads alternately the incoming data  	*
 *                from the MSC cards.										*
 *                This function supports a "writing during reading" 		*
 *                case as well.           									*
 *				  								 							*
 ****************************************************************************/  

void I2CReadWriteTask( BYTE Arg )
{      
	enum
	{
	  CHECK_WRITE_PENDING,
	  READ_MSC,
	  WAIT_FOR_BUS_NOT_BUSY
	};

      // todo: move the following block into the SetOnOff() function.
	if(I2CInitialize == FALSE)
	{
	  I2CInitialize = TRUE;
	  I2CWriteByte(FIRST_CHIP_SELECT,0x00);
	  I2CWriteByte(SECOND_CHIP_SELECT,0x00);         
	  SchedulerLeaveTask(Arg);
	  return;
	}	   
	switch (Arg)
	{		
		case CHECK_WRITE_PENDING:
		///////////////////////////
			if (TRUE == WriteMSC1Pending)
			{
				WriteMSC1Pending = FALSE;
				SMB_DATA_OUT[0] = WritePendingData[FIRSTMSCCARD]; // Define next outgoing byte 
				TARGET = FIRST_CHIP_SELECT;             // Target the Slave for next SMBus transfer
			}
			else if (TRUE == WriteMSC2Pending)
			{
				WriteMSC2Pending = FALSE;
				SMB_DATA_OUT[0] = WritePendingData[SECONDMSCCARD]; // Define next outgoing byte 
				TARGET = SECOND_CHIP_SELECT;             // Target the Slave for next SMBus transfer
			}
			else // no pending write operations.
			{
				SchedulerLeaveTask(READ_MSC);
				break;
			}
			//Write operation
			SMB_BUSY = BUS_BUSY;                       // Claim SMBus (set to busy)   
			SMB_RW = I2C_WRITE_OP;                         // Mark this transfer as a WRITE   
			STA = 1; 
			SchedulerLeaveTask(WAIT_FOR_BUS_NOT_BUSY);
			break;

		case READ_MSC:		
			if (FirstSecondMSC == FIRSTMSCCARD) 
			{
				FirstSecondMSC = SECONDMSCCARD;									
				TARGET = SECOND_CHIP_SELECT;             // Target the Slave for next SMBus transfer  					              
			}
			else  
			{
				FirstSecondMSC = FIRSTMSCCARD;							
				TARGET = FIRST_CHIP_SELECT;             // Target the Slave for next SMBus transfer  					      
			}

			SMB_BUSY = BUS_BUSY;                     // Claim SMBus (set to busy)   
			SMB_RW   = I2C_READ_OP;                  // Mark this transfer as a READ   
			STA      = 1;
				
			SchedulerLeaveTask(WAIT_FOR_BUS_NOT_BUSY);				      
   			break;

		case 	WAIT_FOR_BUS_NOT_BUSY:
		/////////////////////////////
			if (SMB_BUSY == BUS_BUSY)
				SchedulerLeaveTask(WAIT_FOR_BUS_NOT_BUSY);
			else
				SchedulerLeaveTask(CHECK_WRITE_PENDING);
			break;

		default:
		  break;
	}
	//if ISR doesn't reset ISRKeepAliveCheck for 400 cycles (~8sec), send an error to EM 
	if(ISRKeepAliveCheck>400)
	{		      
	    I2C_Error_Notifier = I2C_ERROR;   		         		   
		CanSendErrorMsg = FALSE;
		I2CPrepareStatusMsg();
		SchedulerResumeTask(I2C_Status_TaskHandle,0);
		ISRKeepAliveCheck = 0;				   						 	 		          
	}	
	ISRKeepAliveCheck++;
} 

   
   
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
  if (OnOff)
  {  
  		SMB0CN = 0x07;          // Assert Acknowledge low (AA bit = 1b)
		SMB0CN |= 0x40;         // Enable SMBus;
		SMB0CR = 0xCC;			// 100 kHz
  		EIE1 |= ESMB0;			// enable SMBus interrupts
  		SI=0;                   // Clear SMBUS interrupt Flag.
		
		SchedulerResumeTask(I2CReadWriteMSCTaskHandle,0);
		SchedulerTaskSleep(I2CReadWriteMSCTaskHandle,500);		                  
  }
  else 
  { 	
		I2CInitialize = FALSE;	
		SchedulerSuspendTask(I2CReadWriteMSCTaskHandle);
		
	  	EIE1   &= ~ESMB0;
	   
	  	SMB0CN &= ~0x40; // Reset communication   
      	SMB0CN |= 0x40;   
      	STA = 0;   
      	STO = 0;   
      	AA = 0;   
   
      	SMB_BUSY = 0;  // Free SMBus
	
	  	SI=0;         // Clear SMBUS interrupt Flag.		
	}
}
   

/****************************************************************************
 *																			*
 *  NAME        : SMBus_Init												*
 *																			*
 *  DESCRIPTION : Initialize the I2C driver                                 *       
 *																			*
 ****************************************************************************/   
void SMBus_Init (void)   
{ 	   
    XBR0  |= 0x01;          // SMBus0 SDA and SCL routed to 2 Port pins. 

	I2CReadWriteMSCTaskHandle  = SchedulerInstallTask(I2CReadWriteTask);
	I2C_Status_TaskHandle = SchedulerInstallTask(I2C_Status_Task);
 }   

/*****************************************************************************
 *																			 *
 *  NAME        : Timer3_Init												 *
 *																			 *
 *  DESCRIPTION :Timer3 configured for use by the SMBus low timeout detect   *
 *				 feature as follows:   										 *
 *				 - SYSCLK/12 as Timer3 clock source   						 *
 *               - Timer3 reload registers loaded for a 25ms overflow period *                           
 *				 - Timer3 pre-loaded to overflow after 25ms   				 *
 *				 - Timer3 enabled   										 *
 ****************************************************************************/    
/*

Currently we are not using this.

void Timer3_Init (void)   
{   
   TMR3CN = 0x01;                      // Timer3 uses SYSCLK/12   
   
   TMR3RL = -(SYSCLK/8/40);           // Timer3 configured to overflow after   
   TMR3 = TMR3RL;                      // ~25ms (for SMBus low timeout detect)   
   
   EIE2 |= 0x01;                       // Timer3 interrupt enable   
   TMR3CN |= 0x04;                     // Start Timer3   
}
*/
   
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
   static BYTE Master_Transmitter_Address_Arbitration_Lost = 0;
   static BYTE Master_Transmitter_Data_Arbitration_Lost = 0;
   static BYTE Connection_Lost = 0;
   static BYTE Arbitration_Lost = 0;
   static BYTE sent_byte_counter;   
   static BYTE rec_byte_counter;   
   static BYTE CheckForErrorFlag = I2C_NO_ERROR;
   static BYTE TargetToCheck;

   // Status code for the SMBus (SMB0STA register)   
   switch (SMB0STA)   
   {   
      // Master Transmitter/Receiver: START condition transmitted.   
      // Load SMB0DAT with slave device address.   
      case SMB_START:   
   	     SMB0DAT = TARGET;             // Load address of the slave.   
         SMB0DAT &= 0xFE;              // Clear the LSB of the address for the R/W bit   
         SMB0DAT |= SMB_RW;            // Load R/W bit   
         STA = 0;                      // Manually clear STA bit   
   
         rec_byte_counter = 1;         // Reset the counter   
         sent_byte_counter = 1;        // Reset the counter
		 TargetToCheck = TARGET;

#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC  // Debug - Add to log		  
		 if (SMB_RW == I2C_WRITE_OP )
		 {	
		     BeginDebugMessage();
			 AppendToDebugMessage(SMB0DAT);
		 }
#endif
		 break;
      // Master Transmitter/Receiver: repeated START condition transmitted.   
      // Load SMB0DAT with slave device address   
      case SMB_RP_START:   
         SMB0DAT = TARGET;             // Load address of the slave.   
         SMB0DAT &= 0xFE;              // Clear the LSB of the address for the   
                                       // R/W bit   
         SMB0DAT |= SMB_RW;            // Load R/W bit   
         		 
		 STA = 0;                      // Manually clear STA bit   
   
         rec_byte_counter = 1;         // Reset the counter   
         sent_byte_counter = 1;        // Reset the counter   
#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC		 // Debug - Add to log
		 AppendToDebugMessage(SMB0DAT);
#endif        
		 break;     
      // Master Transmitter: Slave address + WRITE transmitted.  ACK received.   
      // For a READ: N/A            
      // For a WRITE: Send the first data byte to the slave.   
      case SMB_MTADDACK:   

		 TempSMB_DATA_OUT = SMB_DATA_OUT[sent_byte_counter-1];
         SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];   
		 SMB_DATA_OUT[sent_byte_counter-1] = 0;

#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC		 // Debug - Add to log
		 AppendToDebugMessage(SMB0DAT);
#endif

         sent_byte_counter++; 		        
		 //In order to identify which Card is damaged.
		 if(TARGET == FIRST_CHIP_SELECT) 
		   MSC1DataOut = TempSMB_DATA_OUT;
		 else if(TARGET == SECOND_CHIP_SELECT) 
		   MSC2DataOut = TempSMB_DATA_OUT;
         break;   
      // Master Transmitter: Slave address + WRITE transmitted.  NACK received.   
      // Restart the transfer.   
      case SMB_MTADDNACK: 
         if (Master_Transmitter_Address_Arbitration_Lost++) //Added in order to prevent connection lost  
		 {  					                           // between OCB & Embedded While trying to reach un-existing Address		    			
			Master_Transmitter_Address_Arbitration_Lost= 0;
            FAIL = 1; 
			break;  
		 }
		 /*******************************************************/
		 STA = 1;                      // Restart transfer   
         break;   
   
      // Master Transmitter: Data byte transmitted.  ACK received.   
      // For a READ: N/A           
      // For a WRITE: Send all data.  After the last data byte, send the stop bit.   
      case SMB_MTDBACK:      
         if (sent_byte_counter <= NUM_BYTES_WR)   
         {   
			// send data byte   
            SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];   
            sent_byte_counter++;
			
#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC		 // Debug - Add to log
		 AppendToDebugMessage(SMB0DAT);
#endif   
         }   
         else   
         {  			
			STO = 1;                   // Set STO to terminate transfer   
	        SMB_BUSY = 0;              // And free SMBus interface
			
#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC	
			// Debug - Add to log
		 	EndDebugMessage(); // don't check if Read or Write OP. This must be Write.   
#endif         
		 }    
         break;   
   
      // Master Transmitter: Data byte transmitted.  NACK received.   
      // Restart the transfer.   
      case SMB_MTDBNACK:   
         if (Master_Transmitter_Data_Arbitration_Lost++) //Added in order to prevent connection lost  between OCB & Embedded
		 {  					                         // While trying to reach un-existing Address.
			Master_Transmitter_Data_Arbitration_Lost= 0;
            FAIL = 1; 
			break;  
		 }
		 /*******************************************************/
		 STA = 1;                      // Restart transfer      
         break;      
      // Master Receiver: Slave address + READ transmitted.  ACK received.   
      // For a READ: check if this is a one-byte transfer. if so, set the   
      //  NACK after the data byte is received to end the transfer. if not,   
      //  set the ACK and receive the other data bytes.    
      // For a WRITE: N/A   
      case SMB_MRADDACK:      
         if (rec_byte_counter == NUM_INCOMING_BYTES)   
         {   
            AA = 0;    // send NACK after last byte is received                                             
         }   
         else   
         {   
            AA = 1;    //transfer is not completed, send ACK after each received byte                                           
         }      
         break;   
   
      // Master Receiver: Slave address + READ transmitted.  NACK received.   
      // Restart the transfer.   
      case SMB_MRADDNACK: 	   
         if (Connection_Lost++ >ON_OFF_DELAY) //Added in order to prevent connection lost  between OCB & Embedded
		 { 		   
		   if(InitDelay++ >ON_OFF_DELAY) //Additional delay to prevent error messages at startup
		   { 					              // While trying to reach un-existing  or Disconnected	Address.		  
		    if(TargetToCheck == SECOND_CHIP_SELECT)
		    { 		     
			 I2C_Error_Notifier = MSC2_NO_SLAVE;
			 I2CA2DSecondSampleBuffer[_24V_STATUS]  = 0;			 

			 if(SecondSlaveOccurrence == FALSE && CanSendErrorMsg == TRUE)
		   	 {
				CanSendErrorMsg = FALSE;
				I2CPrepareStatusMsg();
				SchedulerResumeTask(I2C_Status_TaskHandle,0);
			 }			 
			 SecondSlaveOccurrence = TRUE;				
		    }
		    else if (TargetToCheck == FIRST_CHIP_SELECT ) 
		    {		     
		   	 I2C_Error_Notifier = MSC1_NO_SLAVE;
			 I2CA2DFirstSampleBuffer[_24V_STATUS]  = 0;
	
			 if(FirstSlaveOccurrence == FALSE && CanSendErrorMsg == TRUE)
		   	 {
				CanSendErrorMsg = FALSE;
				I2CPrepareStatusMsg();
				SchedulerResumeTask(I2C_Status_TaskHandle,0);
			 }
				FirstSlaveOccurrence = TRUE;						
		    }
			InitDelay = ON_OFF_DELAY+1;
		   }		   
		   Connection_Lost= 0;
           FAIL = 1; 
		   break;  
		 }
		 /*******************************************************/
		 STA = 1;                      // Restart transfer      
         break;      
      // Master Receiver: Data byte received.  ACK transmitted.   
      // For a READ: receive each byte from the slave.  if this is the last   
      //  byte, send a NACK and set the STOP bit.   
      //   
      // For a WRITE: N/A   
      case SMB_MRDBACK:   
 		 if (rec_byte_counter <= NUM_INCOMING_BYTES-1)   
         {   
            SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received byte   
                                                      // Send ACK to indicate byte received   
            if (rec_byte_counter == (NUM_INCOMING_BYTES-1)) AA = 0;
			else AA = 1;
			rec_byte_counter++;        // Increment the byte counter 
         }      
         break;      
      // Master Receiver: Data byte received.  NACK transmitted.   
      // For a READ: Read operation has completed.  Read data register and send STOP.      
      // For a WRITE: N/A   
      case SMB_MRDBNACK:   
   
         SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received byte   
         STO = 1;   
         SMB_BUSY = 0;   
		 
         AA = 1;                       // Set AA for next transfer   

		 if(TargetToCheck == FIRST_CHIP_SELECT) 
		 	FirstSlaveOccurrence = FALSE;
		 else if(TargetToCheck == SECOND_CHIP_SELECT) 
		 	SecondSlaveOccurrence = FALSE;

		 Incoming_Bytes_To_Words(FirstSecondMSC);		 
				 
		 if((CheckForErrorFlag = CheckForError(TargetToCheck))!= I2C_NO_ERROR && CanSendErrorMsg == TRUE) 
		 {
		   CanSendErrorMsg = FALSE;
		   I2CPrepareStatusMsg();
		   SchedulerResumeTask(I2C_Status_TaskHandle,0);		   
		 }
		 		
		 if(CheckForErrorFlag == MSC1_HW_ERROR || CheckForErrorFlag == MSC2_HW_ERROR)
		 { 
		 	CheckForErrorFlag = I2C_NO_ERROR;
			I2CWriteByte(FIRST_CHIP_SELECT,0x00); //Turn-off all pumps in case of HW Error. 
			I2CWriteByte(SECOND_CHIP_SELECT,0x00);//Turn-off all pumps in case of HW Error.			           			
		 	break;
		 }		
	     //Check if we have a pending Writing request at the end of the current Reading operation.                             	   
	     Connection_Lost = 0;
	     CheckForErrorFlag = I2C_NO_ERROR;	 	
		 //EndDebugMessage(); 
         break;      
      // Master Transmitter: Arbitration lost.   
      case SMB_MTARBLOST:	        
         if (Arbitration_Lost++) //Added in order to prevent connection lost  between OCB & Embedded
		 {  					// While trying to reach un-existing Address.
		   Arbitration_Lost= 0;
           FAIL = 1; 
		   break;  
		 }
		 /*******************************************************/
		 STA = 1;                      // Restart transfer      
         break;      
      // All other status codes invalid.  Reset communication.   
      default:   
         FAIL = 1;      
         break;   
   }      

   if(FAIL)                           // If the transfer failed,   
   {         		  
	  SMB0CN &= ~0x40;                 // Reset communication   
      SMB0CN |= 0x40;   
      STA = 0;   
      STO = 0;   
      AA = 0;      
      SMB_BUSY = 0;                    // Free SMBus   	     
      FAIL = 0;   
   }         
   ISRKeepAliveCheck = 0; 
   SI = 0;                             // Clear interrupt flag  
}   
   
 
/****************************************************************************
 *																			*			 
 *  NAME        : Timer3_ISR												*		     
 *																			*			 
 *  DESCRIPTION : A Timer3 interrupt indicates an SMBus SCL low timeout.   	*             
 *                The SMBus is disabled and re-enabled if a timeout occurs. *                                                  
 *				  	                                                        *             																			*			 
 ****************************************************************************/   
void Timer3_ISR (void) interrupt 14   
{   
   SMB0CN &= ~0x40;                    // Disable SMBus   
   SMB0CN |= 0x40;                     // Re-enable SMBus   
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending flag   
   SMB_BUSY = 0;                       // Free bus   
   
}   
   
 
/*****************************************************************************
 *																			 *
 *  NAME        : Incoming_Bytes_To_Words									 *
 *																			 *
 *  INPUT       : MSC card number.											 *
 *																			 *
 *  DESCRIPTION : Turns the incoming bytes from SMBUS into Words, because	 *
 *                all the data except the first two and the last byte is 	 *
 *                12 but represented.              							 *
 *				  															 *
 ****************************************************************************/   
   
  
BYTE Incoming_Bytes_To_Words(BYTE FirstSecondMSC)
{
    BYTE idata i;		
	WORD * xdata pI2CA2DSampleBuffer =  (FirstSecondMSC == FIRSTMSCCARD) ? &(I2CA2DFirstSampleBuffer[0]) :  &(I2CA2DSecondSampleBuffer[0]);

	for (i=0;i< BYTES_TO_WORDS;i++)            //BYTES_TO_WORDS-> 19 Bytes turn into 11 Words 
	{											  //The First two bytes and the last one 
	  											         //stay as is and all the others gother together in pairs.
		if (i<FIRST_TWO_BYTES)
		{		 
			pI2CA2DSampleBuffer[i] = SMB_DATA_IN[i];  		 		 
		} 
	   
		else if(i<LAST_BYTE)
		{	     					
			pI2CA2DSampleBuffer[i] = (SMB_DATA_IN[i*2-2] << ByteShiftLeft) | (SMB_DATA_IN[i*2-1]);		 				
 		}
		else
		{ 		
			pI2CA2DSampleBuffer[i] = SMB_DATA_IN[i*2-2];	      		 		  			
 		} 		 
	}	
    
	//memset(SMB_DATA_IN, 0, NUM_INCOMING_BYTES);			
	return TRUE;
}   

/*****************************************************************************
 *																			 *
 *  NAME        : I2CA2D_GetReading											 *
 *																			 *
 *  INPUT       : A2D buffer Cell, MSC card number.							 *
 *																			 *
 *  OUTPUT      : A2D Value.												 *
 *																			 *
 *  DESCRIPTION : Returns the relevant A2D value according to the requested  *             
 *				  card number( MSC1 or MSC2).								 *
 ****************************************************************************/
WORD I2CA2D_GetReading(BYTE A2DInNum, BYTE FirstSecondMSC)
{
	WORD RetVal;
	EIE1 &= ~ESMB0;
  	
	if(!FirstSecondMSC)
	{
		RetVal = I2CA2DFirstSampleBuffer[A2DInNum];		
    }
	else 
	{
	    RetVal = I2CA2DSecondSampleBuffer[A2DInNum];
	}
   	
	EIE1 |= ESMB0;

	return RetVal;
}  

/******************************************************************************
 *																			  *
 *  NAME        : I2CA2D_GetBlockReading									  *
 *																			  *
 *  INPUT       : Temp buffer, MSC card number.								  *
 *																			  *
 *  OUTPUT      : Card voltages							  					  *
 *																			  *
 *  DESCRIPTION : Returns the voltages according to the requested 			  *              
 *				  card number( MSC1 or MSC2).								  *
 *****************************************************************************/

void I2CA2D_GetBlockReading( WORD *state, BYTE FirstSecondMSC)
{

	EIE1 &= ~ESMB0;
    
	if(!FirstSecondMSC)
	{
	  state[0] = I2CA2DFirstSampleBuffer[_24V_STATUS];
	  state[1] = I2CA2DFirstSampleBuffer[_7V_STATUS];
	  state[2] = I2CA2DFirstSampleBuffer[_5V_STATUS];
	  state[3] = I2CA2DFirstSampleBuffer[_3_3V_STATUS];
	  //memset(I2CA2DFirstSampleBuffer, 0, sizeof(WORD)*4);			
	}
	else
	{
	  state[0] = I2CA2DSecondSampleBuffer[_24V_STATUS];
	  state[1] = I2CA2DSecondSampleBuffer[_7V_STATUS];
	  state[2] = I2CA2DSecondSampleBuffer[_5V_STATUS];
	  state[3] = I2CA2DSecondSampleBuffer[_3_3V_STATUS];
	  //memset(I2CA2DSecondSampleBuffer, 0, sizeof(WORD)*4);
	}
	
	EIE1 |= ESMB0;
}

void I2CPrepareStatusMsg()
{
    
  Msg.MsgId       = I2C_STATUS_MSG;
  Msg.I2CStatus   = I2C_Error_Notifier; 
  if(TARGET == FIRST_CHIP_SELECT) 
	Msg.CardNum = FIRSTMSCCARD;
  else if(TARGET == SECOND_CHIP_SELECT) 
	Msg.CardNum = SECONDMSCCARD;
  //Msg.CardNum	  = TARGET;
  Msg.LastWritten = TempSMB_DATA_OUT;
  Msg.LastRead    = SMB_DATA_IN[PUMPS_STATUS];

  //Zero the voltages Buffers in case of Communication error.
  switch(Msg.I2CStatus)
  {
    case MSC1_NO_SLAVE:
	     memset(I2CA2DFirstSampleBuffer, 0, sizeof(WORD)*BYTES_TO_WORDS);
	break;
	
	case MSC2_NO_SLAVE:
         memset(I2CA2DSecondSampleBuffer, 0, sizeof(WORD)*BYTES_TO_WORDS);	     
    break; 
  }
}

/*****************************************************************************
 *																			 *
 *  NAME        : I2C_Status_Task											 *
 *																			 *
 *  INPUT       : Arg.														 *
 *																			 *
 *  OUTPUT      : Error notification, type of the Error and relevant data	 *
 *				  about the Error that relavnt to the user.					 *
 *																			 *
 *  DESCRIPTION : Lanches within the SMBUS_ISR if an Error occurred,		 *
 *                and report to the user the Following:						 *
 *                1. Error type.											 *
 *                2. MSC Card which the Error occured on.					 *
 *                3. Last data that was written.							 *
 *                4. Last data to be read.									 *
 *****************************************************************************/
																			 
void I2C_Status_Task(BYTE Arg)
{
  
  if (EdenProtocolSend((BYTE*)&Msg,sizeof(TI2CNotificationMsg),EDEN_ID,OCB_ID,
   			  0,FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
		  
            SchedulerSuspendTask(-1);
		  else
		    SchedulerLeaveTask(Arg);

	 I2C_Error_Notifier = 0;
	 CanSendErrorMsg = TRUE;
}

/****************************************************************************
 *																			*
 *  NAME        : CheckForError												*
 *																			*
 *  INPUT       : NONE.														*
 *																			*
 *  OUTPUT      : ERROR_STATUS.												*
 *																			*
 *  DESCRIPTION : Checking if and which  Error occurred during I2C          *      
 *				  communication.											*
 ***************************************************************************/

BYTE  CheckForError(BYTE TargetToCheck)
{
  BYTE Retval = I2C_NO_ERROR;
  	 //Verifing the Pumps status: Write = read, and if it's un-equal it launches an Error.

	 switch(TargetToCheck)
	 {
	
	 	case FIRST_CHIP_SELECT:

			if( (MSC1DataOut != SMB_DATA_IN[PUMPS_STATUS]))
			{
			  	 
					++CheckMSC1HWError;
			  	 	I2C_Error_Notifier = MSC1_COM_ERROR;// I2C_COM1_ERROR
			  			 
				 	if(CheckMSC1HWError == 3) 
			  	     {
			 			I2C_Error_Notifier = MSC1_HW_ERROR;
						CheckMSC1HWError = 0;
						
						Retval = MSC1_HW_ERROR;
			  	  	 }
               		else
			    		Retval = MSC1_COM_ERROR;
			}
			
			else 
				CheckMSC1HWError = 0;
		break;
           
		case SECOND_CHIP_SELECT:
			if((MSC2DataOut != SMB_DATA_IN[PUMPS_STATUS]))
			{   
			 	
				++CheckMSC2HWError;
				I2C_Error_Notifier = MSC2_COM_ERROR;// I2C_COM2_ERROR
			 			
				if(CheckMSC2HWError == 3) 
			 	{
			 		I2C_Error_Notifier = MSC2_HW_ERROR;
					CheckMSC2HWError = 0;
					
					Retval = MSC2_HW_ERROR;
			 	}
				else 
					Retval = MSC2_COM_ERROR;
			}			 
			else 
				CheckMSC2HWError = 0; 
		break;			   		
     }
	 return Retval;
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
#endif
