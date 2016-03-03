/*************************************************************************   
* FILENAME       : I2C {I2C_DRV.c}  									 *
**************************************************************************
*  																	     *
*  PURPOSE:   															 *
*    																	 *
*  usage: only for simulator!!  																	 *
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
* PROGRAMER: Luda Margolis 												 *
**************************************************************************/   
   
#include "c8051f120.h"
#include "Define.h" 
#include "I2C_DRV.h" 
#include "MiniScheduler.h"
#include "..\TimerSim\TimerDrv.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"  
#include "string.h" 
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"  
//-----------------------------------------------------------------------------   
// Global VARIABLES   
//-----------------------------------------------------------------------------      
 
   
BYTE idata SMB_DATA_IN[NUM_INCOMING_BYTES]={0,}; // Global holder for SMBus data All receive data is written here     
BYTE idata SMB_DATA_OUT[NUM_BYTES_WR]={0};      //Global holder for SMBus data. All transmit data is read from here  
BYTE idata I2CIndex = 0;
BYTE idata ByteShiftLeft = 8;
WORD idata I2CA2DFirstSampleBuffer[BYTES_TO_WORDS] = {0,};
WORD idata I2CA2DSecondSampleBuffer[BYTES_TO_WORDS]= {0,};
         
BYTE xdata TARGET = 0;                         // Target SMBus slave address    
bit SMB_BUSY;                        // Software flag to indicate when the SMB_Read() or SMB_Write() functions   
                                     // have claimed the SMBus      
volatile BYTE data_count=0;         // SMB_DATA_IN and SMB_DATA_OUT counter in future if needed to write more then one 
							       //Byte to MSC  should use this counter   


bit SMB_RW;                        // Software flag to indicate the direction of the current transfer  

BYTE idata FirstSecondMSC = 0x01;   	 //Indicates which of the two MSC cards is during process.
BYTE idata WriteMSC1Pending  = FALSE;     //An indication Flag - indicates if there is a writing request during reading request 
BYTE idata WriteMSC2Pending  = FALSE;
BYTE idata WritePendingData[2];	 		 //Temp buffer- will bu used in a case of Writing during reading.

// 16-bit SFR declarations   
sfr16    TMR3RL   = 0x92;          // Timer3 reload registers   
sfr16    TMR3     = 0x94;          // Timer3 counter registers   
sbit SDA = P0^6;                   // SMBus on P0.0   
sbit SCL = P0^7;                   // and P0.1   
BYTE idata i=0; 
BYTE idata j=0;
BYTE idata MSC1_Com_Error = FALSE;
BYTE idata MSC2_Com_Error = FALSE;
BYTE idata I2C_Error_Notifier = 0;
BYTE idata CheckMSC1HWError = 0;
BYTE idata CheckMSC2HWError = 0;
BYTE idata ErrorFlag = 0;
BYTE idata IndexMSC1;
BYTE idata IndexMSC2;
BYTE idata TempSMB_DATA_OUT = 0;
BYTE idata MSC1DataOut = 0; 
BYTE idata MSC2DataOut = 0;
BYTE idata SecondSlaveOccurrence = FALSE; 
BYTE idata FirstSlaveOccurrence = FALSE;  
BYTE idata OCBInitialize1 = FALSE; 
BYTE idata OCBInitialize2 = FALSE; 
BYTE idata I2CInitialize  = FALSE;
BYTE idata StopI2C = FALSE;
BYTE idata CanSendErrorMsg = TRUE;


BYTE idata I2CDebugIndex = 0; 

TI2CNotificationMsg idata Msg;
#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC
TI2CDebugMsg idata DMsg;
#endif

TTaskHandle idata I2CReadWriteMSCTaskHandle;
TTaskHandle idata I2C_Status_TaskHandle;


#define POS   23          // position at which to insert data
#define MASK  ((1UL<<9)-1)  // mask of nine 1s

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
		 
    	
		if(address == FIRST_CHIP_SELECT)
		{
		  EXTMem_Write(FIRSTMSCCARD_ADD, dat);
		}
		
		else
		
		if(address == SECOND_CHIP_SELECT)
		{
			EXTMem_Write(SECONDMSCCARD_ADD, dat);		
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
/*
void I2CReadWriteTask( BYTE Arg )
{      
	 //DUMMY init
 	  Arg = 0;

} 
*/
   
   
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
		//Dummy init
  	  OnOff = 0;
 
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
   
/*  
BYTE Incoming_Bytes_To_Words(BYTE FirstSecondMSC)
{
   	//Dummy init
	FirstSecondMSC	  = 0 ;
		
	return TRUE;
}   
*/
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

	BYTE val = 0;
	WORD xdata retVal = 0;
	//implement
		if (A2DInNum == PUMPS_STATUS)
		{
			if(FirstSecondMSC == FIRSTMSCCARD)
			{
		  		EXTMem_Read(FIRSTMSCCARD_ADD, &val);
			}
			else 
			
			if(FirstSecondMSC == SECONDMSCCARD)
			{
				EXTMem_Read(SECONDMSCCARD_ADD, &val);		
			} 
			retVal = val;	
		}

		else  if ( A2DInNum == SWITCHES )
		{	
			if(FirstSecondMSC == FIRSTMSCCARD)
			{
		  		EXTMem_Read(FIRSTMSCCARD_SENS_ADD, &val);
			}
		
			else
			
			if(FirstSecondMSC == SECONDMSCCARD)
			{
				EXTMem_Read(SECONDMSCCARD_SENS_ADD, &val);		
			} 
			retVal = val;
		}

		//implementation for weight sensors
		else if (A2DInNum == LOAD_CELL_1_STATUS) //for weight sensor:
		{
			if(FirstSecondMSC == FIRSTMSCCARD)//MODEL_1_WEIGHT
			{
		  		EXTMem_ReadAnalog12(FIRSTMSCCARD_LOAD_CELL1_ADD, &retVal); //  ReadAnalog12 reads 12 bits!!
			}
		
			else
			
			if(FirstSecondMSC == SECONDMSCCARD)	//MODEL_3_WEIGHT
			{
				EXTMem_ReadAnalog12(SECONDMSCCARD_LOAD_CELL1_ADD, &retVal);		
			} 

		}
			
		else if (A2DInNum == LOAD_CELL_2_STATUS) //for weight sensor: 
		{
		  	if(FirstSecondMSC == FIRSTMSCCARD) ////MODEL_2_WEIGHT
			{
		  		EXTMem_ReadAnalog12(FIRSTMSCCARD_LOAD_CELL2_ADD, &retVal);
			}
		
			else
			
			if(FirstSecondMSC == SECONDMSCCARD)	 //MODEL_4_WEIGHT
			{
				EXTMem_ReadAnalog12(SECONDMSCCARD_LOAD_CELL2_ADD, &retVal);		
			} 
		}

		else if (A2DInNum == LOAD_CELL_3_STATUS) //for weight sensor
		{
			if(FirstSecondMSC == FIRSTMSCCARD)	 //SUPPORT_1_WEIGHT
			{
		  		EXTMem_ReadAnalog12(FIRSTMSCCARD_LOAD_CELL3_ADD, &retVal);
			}
		
			else
			
			if(FirstSecondMSC == SECONDMSCCARD)//MODEL_5_WEIGHT
			{
				EXTMem_ReadAnalog12(SECONDMSCCARD_LOAD_CELL3_ADD, &retVal);		
			} 

		}

		else if (A2DInNum == LOAD_CELL_4_STATUS) //for weight sensor
		{
			if(FirstSecondMSC == FIRSTMSCCARD) //SUPPORT_2_WEIGHT
			{
		  		EXTMem_ReadAnalog12(FIRSTMSCCARD_LOAD_CELL4_ADD, &retVal);
			}
		
			else
			
			if(FirstSecondMSC == SECONDMSCCARD)//MODEL_6_WEIGHT
			{
				EXTMem_ReadAnalog12(SECONDMSCCARD_LOAD_CELL4_ADD, &retVal);		
			} 

		}

	return retVal;
}  

/******************************************************************************
 *																			  *
 *  NAME        : I2CA2D_GetBlockReading									  *
 *																			  *
 *  INPUT       : Temp buffer, MSC card number.								  *
 *																			  *
 *  OUTPUT      : Entire MSC data buffer(11 WORDS).							  *
 *																			  *
 *  DESCRIPTION : Returns the relevant data buffer according to the requested *              
 *				  card number( MSC1 or MSC2).								  *
 *****************************************************************************/

void I2CA2D_GetBlockReading( WORD *state, BYTE FirstSecondMSC)
{
 //BYTE val; 
 WORD xdata w[ _3_3V_STATUS -_24V_STATUS + 1 ];
 switch (FirstSecondMSC)
 {
 case FIRSTMSCCARD:
  //	EXTMem_Read(FIRSTMSCCARD_SENS_ADD,&val );
  	EXTMem_ReadWord(FIRSTMSCCARD_24V_STATUS_ADD, &w[0]);
	EXTMem_ReadWord(FIRSTMSCCARD_7V_STATUS_ADD,  &w[1]);
	EXTMem_ReadWord(FIRSTMSCCARD_5V_STATUS_ADD,  &w[2]);
	EXTMem_ReadWord(FIRSTMSCCARD_3_3V_STATUS_ADD,&w[3]);
  	break;
 case SECONDMSCCARD:
//	EXTMem_Read(SECONDMSCCARD_SENS_ADD,&val);
  	EXTMem_ReadWord(SECONDMSCCARD_24V_STATUS_ADD, &w[0]);
	EXTMem_ReadWord(SECONDMSCCARD_7V_STATUS_ADD,  &w[1]);
	EXTMem_ReadWord(SECONDMSCCARD_5V_STATUS_ADD,  &w[2]);
	EXTMem_ReadWord(SECONDMSCCARD_3_3V_STATUS_ADD,&w[3]);
	break;

 default: state = 0;

  }
  //w = (w & ~(MASK<<POS)) | ((val & MASK) << POS);
 // memcpy(state, &w, BYTES_TO_WORDS);
  memcpy(state,&w,(_3_3V_STATUS -_24V_STATUS + 1)*sizeof(WORD));
}
/*
void I2CPrepareStatusMsg()
{
    
  
}
*/
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
/*																			 
void I2C_Status_Task(BYTE Arg)
{
	//Dummy init
  Arg = 0 ;

}*/
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
/*
BYTE CheckForError(BYTE TargetToCheck)
{
	BYTE temp  = TargetToCheck;
 // TargetToCheck = 0 ; //Dummy init
  BYTE Retval = I2C_NO_ERROR;
  	 //Verifing the Pumps status: Write = read, and if it's un-equal it launches an Error.

	   return Retval;

}
*/

#ifdef DEBUG_SEND_DEBUG_MESSAGE_TO_PC  
void BeginDebugMessage()
{
  
  
}

void EndDebugMessage()
{
 

} 
void AppendToDebugMessage(BYTE DataReg)
{
    
	
}  
#else
/*
void BeginDebugMessage()
{ 
}

void EndDebugMessage()
{
}
   
void AppendToDebugMessage(BYTE DataReg)
{
	DataReg = 0; //Dummy init
}
*/
#endif



