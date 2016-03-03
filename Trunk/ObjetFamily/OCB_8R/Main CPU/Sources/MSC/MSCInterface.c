/*************************************************************************   
* FILENAME       : MSCInterface.c  									     *
**************************************************************************
*  																	     *
*  PURPOSE: Handeling the MSC cards (Cordilia) communication   			 *												 
*    		facing I2C													 *
*   																	 *		
**************************************************************************   
* PROGRAMER: Elad Hazan  												 *
**************************************************************************/ 

#include "MSCInterface.h"

#ifdef OCB_SIMULATOR
#include "c8051F120.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h"
#else
#include "c8051F020.h"
#endif

#include "Define.h" 
#include "I2C_DRV.h" 
#include "MiniScheduler.h"
#include "TimerDrv.h"
#include "MsgDecode.h"
#include "EdenProtocol.h"  
#include "string.h" 


BYTE xdata MSCSelectedCard                    = MSCCARD_1; //Counter, which indicates which of the MSC cards is during process.
BYTE xdata MSCWritePending [NUM_OF_MSC_CARDS] = {FALSE,};  //An indication Flag - indicates if there is a writing request during reading request 
BYTE xdata WritePendingData[NUM_OF_MSC_CARDS] = {0};  	   //Temp buffer- will bu used in a case of Writing during reading.
BYTE data  CheckMSCHWError [NUM_OF_MSC_CARDS] = {0};
BYTE data  PumpsActDataOut [NUM_OF_MSC_CARDS] = {0};
BYTE data  PumpsActDataIn  [NUM_OF_MSC_CARDS] = {0};
BYTE data  DataWordIndex                      = 0;
BYTE data  ErrorMsgSent                       = TRUE;
BYTE data  CheckForErrorFlag                  = MSC_NO_ERROR;
BYTE data  MSC_Error_Notifier                 = MSC_NO_ERROR;     
BYTE data  ByteShiftLeft                      = 8;

BYTE xdata NoSlaveErrorOccurrence[NUM_OF_MSC_CARDS]                 = {FALSE,};
BYTE xdata MSCReadData           [NUM_INCOMING_BYTES]               = {0}; // Global holder for SMBus data All receive data is written here
WORD xdata MSCA2DSampleBuffer    [NUM_OF_MSC_CARDS][BYTES_TO_WORDS] = {{0,},};

TTaskHandle data MSCReadWriteTaskHandle;
TTaskHandle data MSC_Status_TaskHandle;
TMSCNotificationMsg data Msg;

const BYTE code MSC_ChipSelectLookup[] = {CHIP_SELECT_MSC_1, CHIP_SELECT_MSC_2,	CHIP_SELECT_MSC_3, CHIP_SELECT_MSC_4};
const BYTE code MSC_CardsLookup[]      = {MSCCARD_1, 	     MSCCARD_2,     	MSCCARD_3,         MSCCARD_4};

const BYTE code MSC_Err_NoSlaveLookup[]  = {MSC_1_NO_SLAVE,  MSC_2_NO_SLAVE,  MSC_3_NO_SLAVE,  MSC_4_NO_SLAVE};
const BYTE code MSC_Err_HwErrorLookup[]  = {MSC_1_HW_ERROR,  MSC_2_HW_ERROR,  MSC_3_HW_ERROR,  MSC_4_HW_ERROR};
const BYTE code MSC_Err_ComErrorLookup[] = {MSC_1_COM_ERROR, MSC_2_COM_ERROR, MSC_3_COM_ERROR, MSC_4_COM_ERROR};


void MSC_Init (void)   
{ 	  
	MSCReadWriteTaskHandle = SchedulerInstallTask(MSCReadWriteTask);
	MSC_Status_TaskHandle  = SchedulerInstallTask(MSC_Status_Task);
}   


/*****************************************************************************
*																			 *
*  NAME        : MSCWriteByte											     *
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
BYTE MSCWriteByte(BYTE SelectedCard, BYTE dat )
{
	MSCWritePending[SelectedCard]  = TRUE;
	WritePendingData[SelectedCard] = dat;	

	return TRUE;
}


/****************************************************************************
*																			* 
*  NAME        : MSCReadWriteTask										    *	     
*																			* 
*  INPUT       : Arg.					     								*
*																			* 												 
*																			* 
*  DESCRIPTION : A routing task that used as a state machine for			*
*                1. Writing												*
*				  2. Reading												*
*				  3. BUS Busy        										*
*				  This is the core Function(Task) of the communication 		*
*				  handeling between MSC and I2C.							*
****************************************************************************/  

void MSCReadWriteTask( BYTE Arg )
{      
	BYTE idata i;
	BYTE idata WritePending = FALSE;

	enum
	{
		CHECK_WRITE_PENDING,
		READ_MSC,
		WAIT_FOR_BUS_NOT_BUSY,
	};

	switch (Arg)
	{		
		case CHECK_WRITE_PENDING:
		{
			for(i = 0; i < NUM_OF_MSC_CARDS; i++)
			{
				if(TRUE == MSCWritePending[i])
				{					
					I2CWrite(MSC_ChipSelectLookup[i], &WritePendingData[i], 1);																								
					PumpsActDataOut[i] = WritePendingData[i]; // keep the sent data for comparison with the read data later.
					MSCWritePending[i] = FALSE;
					WritePending       = TRUE;
					break; // We can use I2C bus only once at each task iteration. 
				}	
			}
			
			if(WritePending == FALSE) // No pending write operations.
			{
				SchedulerLeaveTask(READ_MSC);
				break;
			}			
	
			SchedulerLeaveTask(WAIT_FOR_BUS_NOT_BUSY);
		}
		break;

		case READ_MSC: 
		{			
			MSCSelectedCard = ++MSCSelectedCard % NUM_OF_MSC_CARDS;
			I2CRead(MSC_ChipSelectLookup[MSCSelectedCard], &MSCReadData[0], NUM_INCOMING_BYTES, Incoming_Bytes_To_Words_callback, OnMSCFailCallback);														

			SchedulerLeaveTask(WAIT_FOR_BUS_NOT_BUSY);
		}				      
		break;

		case WAIT_FOR_BUS_NOT_BUSY:
		{
			if(TRUE == I2CBusBusy())
				SchedulerLeaveTask(WAIT_FOR_BUS_NOT_BUSY);
			else
				SchedulerLeaveTask(CHECK_WRITE_PENDING);
		}
		break;
	}		  
} 

/****************************************************************************
*																			*
*  NAME        : MSCStartStop												*
*																			*
*  DESCRIPTION : Set on or off the I2CReadWriteMSCTaskHandle which 		*
*                depends on the I2C_SetOnOff() Function 					*
*                															*
****************************************************************************/
void MSCStartStop(BOOL OnOff )
{
	BYTE idata i;

	if(OnOff)
	{
		SchedulerResumeTask(MSCReadWriteTaskHandle,0);
		SchedulerTaskSleep(MSCReadWriteTaskHandle,500);

		for(i = 0; i < NUM_OF_MSC_CARDS; i++)
			MSCWriteByte(MSC_CardsLookup[i], 0x00);         
	}
	else	
		SchedulerSuspendTask(MSCReadWriteTaskHandle);	
}




/*****************************************************************************
*																			 *
*  NAME        : Incoming_Bytes_To_Words_callback							 *
*																			 *
*  INPUT       : MSC card number.											 *
*																			 *
*  DESCRIPTION : Turns the incoming bytes from SMBUS into Words, because	 *
*                all the data except the first two and the last byte is 	 *
*                12 but represented.              							 *
*				  															 *
****************************************************************************/   


void Incoming_Bytes_To_Words_callback()	 
{
	BYTE data i,j;
	static WORD idata I2CA2DTempSampleBuffer[BYTES_TO_WORDS];	
	
	for(i = 0, j = 0; i < BYTES_TO_WORDS; i++)  //BYTES_TO_WORDS-> 19 Bytes turn into 11 Words 
	{											//The First two bytes and the last one 
												//stay as is and all the others gother together in pairs.
		if(i < FIRST_TWO_BYTES)
		{		 
			I2CA2DTempSampleBuffer[DataWordIndex++] = MSCReadData[j++];  		 
		}		
		else if(i < LAST_BYTE)
		{	     
			I2CA2DTempSampleBuffer[DataWordIndex]    = MSCReadData[j++] << ByteShiftLeft;
			I2CA2DTempSampleBuffer[DataWordIndex++] |= MSCReadData[j++];		
		}
		else
		{ 
			I2CA2DTempSampleBuffer[DataWordIndex] = MSCReadData[j];	      		 
			DataWordIndex = 0; 
		} 		 
	}

	memcpy(MSCA2DSampleBuffer[MSCSelectedCard], I2CA2DTempSampleBuffer, sizeof(I2CA2DTempSampleBuffer));
	
	// Storing the read data of pumps active state for the selected card
	PumpsActDataIn[MSCSelectedCard] = MSCReadData[PUMPS_STATUS];
	
	NoSlaveErrorOccurrence[MSC_CardsLookup[MSCSelectedCard]] = FALSE;	

	memset(I2CA2DTempSampleBuffer, 0, sizeof(I2CA2DTempSampleBuffer));	

	if((CheckForErrorFlag = CheckForError(MSC_CardsLookup[MSCSelectedCard])) != MSC_NO_ERROR && ErrorMsgSent == TRUE) 
	{
		ErrorMsgSent = FALSE;
		MSCPrepareStatusMsg();
		SchedulerResumeTask(MSC_Status_TaskHandle,0);		   
	}

	for(i = 0; i < NUM_OF_MSC_CARDS; i++)
	{
		if(CheckForErrorFlag == MSC_Err_HwErrorLookup[i])
		{
			CheckForErrorFlag = MSC_NO_ERROR;
			
			//Turn-off all pumps in case of HW Error
			for(j = 0; j < NUM_OF_MSC_CARDS; j++)
				MSCWriteByte(MSC_CardsLookup[j], 0x00);	

			break;
		}	
	} 
	
	CheckForErrorFlag = MSC_NO_ERROR;
}   

void OnMSCFailCallback(BYTE TargetToCheck)
{
	BYTE idata i;

	for(i = 0; i < NUM_OF_MSC_CARDS; i++)
	{
		if(MSC_ChipSelectLookup[i] == TargetToCheck)
		{
			if(NoSlaveErrorOccurrence[MSC_CardsLookup[i]] == FALSE && ErrorMsgSent == TRUE)
			{
				MSC_Error_Notifier = MSC_Err_NoSlaveLookup[i];
				ErrorMsgSent       = FALSE;
				NoSlaveErrorOccurrence[MSC_CardsLookup[i]] = TRUE;
	
				MSCPrepareStatusMsg();
				SchedulerResumeTask(MSC_Status_TaskHandle, 0);	
			}
		}
		
		MSCWritePending[i]  = FALSE;
		WritePendingData[i]	= 0;	
	}	
}


/*****************************************************************************
*																			 *
*  NAME        : MSCA2D_GetReading											 *
*																			 *
*  INPUT       : A2D buffer Cell, MSC card number.							 *
*																			 *
*  OUTPUT      : A2D Value.												 *
*																			 *
*  DESCRIPTION : Returns the relevant A2D value according to the requested  *             
*				  card number( MSC1 or MSC2).								 *
****************************************************************************/
WORD MSCA2D_GetReading(BYTE A2DInNum, BYTE SelectedCard)
{
	WORD xdata RetVal;
	EIE1 &= ~ESMB0;								  

	RetVal = MSCA2DSampleBuffer[SelectedCard][A2DInNum];	
	
	EIE1 |= ESMB0;

    //RetVal = 500;  /*LUDA: testing*/
	return RetVal;
}  

/******************************************************************************
*																			  *
*  NAME        : MSCA2D_GetBlockReading									  *
*																			  *
*  INPUT       : Temp buffer, MSC card number.								  *
*																			  *
*  OUTPUT      : Entire MSC data buffer(11 WORDS).							  *
*																			  *
*  DESCRIPTION : Returns the relevant data buffer according to the requested *              
*				  card number( MSC1 or MSC2).								  *
*****************************************************************************/

void MSCA2D_GetBlockReading(WORD *state, BYTE SelectedCard)
{																										
	EIE1 &= ~ESMB0;
	
	memcpy(state, &MSCA2DSampleBuffer[SelectedCard], sizeof(MSCA2DSampleBuffer[SelectedCard]));	
	
	EIE1 |= ESMB0;
}

/******************************************************************************
*																			  *
*  NAME        : MSCPrepareStatusMsg									      *
*																			  *								  																			  *
*  DESCRIPTION : Install the relevant data to be sent to the Embedded if	  *
*                 one off the three possible errors occured				  *
*				  (1). No_Slave_Error										  *
*				  (2). Communocation_Error									  *
*				  (3). HW_Error                                               *               
*				  															  *
*                 if the error is :(1), it will zero the relevant buffer	  *
*				  that is beeing sent to the embedded anyway 				  *
*****************************************************************************/

void MSCPrepareStatusMsg()
{    
	Msg.MsgId       = MSC_STATUS_MSG;
	Msg.MSCStatus   = MSC_Error_Notifier; 
	Msg.CardNum	    = MSCSelectedCard;
	Msg.LastWritten = WritePendingData[MSCSelectedCard];
	Msg.LastRead    = PumpsActDataIn[MSCSelectedCard];
	
	//Zero the voltages Buffers in case of Communication error.
	memset(MSCA2DSampleBuffer[MSCSelectedCard], 0, sizeof(MSCA2DSampleBuffer[MSCSelectedCard]));
}

/*****************************************************************************
*																			 *
*  NAME        : MSC_Status_Task											 *
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

void MSC_Status_Task(BYTE Arg)
{ 
	if (EdenProtocolSend((BYTE*)&Msg, sizeof(TMSCNotificationMsg), EDEN_ID,OCB_ID, 0, FALSE,EDEN_OCB_PROTOCOL) == EDEN_PROTOCOL_NO_ERROR)
	{		
		SchedulerSuspendTask(-1);
	}
	else
	{
		MSC_Error_Notifier = MSC_NO_ERROR;
		ErrorMsgSent       = TRUE;
		SchedulerLeaveTask(Arg);
	}	
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

BYTE CheckForError(BYTE CardToCheck)
{
	BYTE Retval = MSC_NO_ERROR;
	//Verifing the Pumps status: Write = read, and if it's un-equal it launches an Error.

	if(PumpsActDataOut[CardToCheck] != PumpsActDataIn[CardToCheck])
	{			
		++CheckMSCHWError[CardToCheck];
		MSC_Error_Notifier = MSC_Err_ComErrorLookup[CardToCheck];
		
		if(CheckMSCHWError[CardToCheck] == 3) 
		{
			MSC_Error_Notifier           = MSC_Err_HwErrorLookup[CardToCheck];
			CheckMSCHWError[CardToCheck] = 0;
			
			Retval = MSC_Err_HwErrorLookup[CardToCheck];
		}
		else
			Retval = MSC_NO_ERROR;
	}		
	else
	{ 
		CheckMSCHWError[CardToCheck] = 0;
	}	

	return Retval;
}
