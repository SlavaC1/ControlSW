/*===========================================================================
 *   FILENAME       : Head Data {HeadData.c}  
 *   PURPOSE        : 
 *   DATE CREATED   : 21/Aug/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/

#include "HeadData.h"
#include "E2PROMInterface.h"
#include "Utils.h"
#ifdef OCB_SIMULATOR
#include "EdenProtocol.h"
#else
#include "EdenProtocolOHDB.h"
#endif
#include "MsgDecodeOHDB.h"
#include <string.h>
#include "TimerDrv.h"


#define BASE_ADDRESS                            0x00
#define SERIAL_NUMBER_ADR                       BASE_ADDRESS
#define MODEL_POTEN_VALUES_ADR                  BASE_ADDRESS+4
#define SUPPORT_POTEN_VALUES_ADR                BASE_ADDRESS+19
#define TEMPERATURE_COMPENSATION_ADDR           BASE_ADDRESS+34
#define DISTANCE_FROM_REFERENCE_ADDR            BASE_ADDRESS+36
#define CHECK_SUM_ADDR                          BASE_ADDRESS+40


#define BACKUP_BASE_ADDRESS                     0x100
#define BACKUP_SERIAL_NUMBER_ADR                BACKUP_BASE_ADDRESS
#define BACKUP_MODEL_POTEN_VALUES_ADR           BACKUP_BASE_ADDRESS+4
#define BACKUP_SUPPORT_POTEN_VALUES_ADR         BACKUP_BASE_ADDRESS+19
#define BACKUP_TEMPERATURE_COMPENSATION_ADDR    BACKUP_BASE_ADDRESS+34
#define BACKUP_DISTANCE_FROM_REFERENCE_ADDR     BACKUP_BASE_ADDRESS+36
#define BACKUP_CHECK_SUM_ADDR                   BACKUP_BASE_ADDRESS+40


#define NUM_OF_HEADS                            8
#define ALL_HEADS_DATA_ARE_VALID                0xFF
#define MAX_NUM_OF_LAYER_THICKNESS_VALUES       15
#define DATA_AREA_ERROR                         0x01
#define BACKUP_DATA_AREA_ERROR                  0x02

typedef struct{
  WORD A2DValueFor60C;
  WORD A2DValueFor80C;
  float Model10KHzLineGain;
  float Model10KHzLineOffset;
  float Model20KHzLineGain;
  float Model20KHzLineOffset;
  float Support10KHzLineGain;
  float Support10KHzLineOffset;
  float Support20KHzLineGain;
  float Support20KHzLineOffset;
  float XOffset;
  unsigned long ProductionDate;
  char SerialNumber[9];
	WORD CheckSum;
	}THeadData; 

typedef struct{
  BYTE DestId;
  BYTE TransactionId ;
	BYTE HeadNum;
  THeadData HeadData;
  }TSetHeadDataTaskParams;

THeadData xdata HeadsData[NUM_OF_HEADS];
TSetHeadDataTaskParams SetHeadDataTaskParams;
BYTE xdata SIZE_OF_HEAD_DATA;
BYTE xdata SIZE_OF_HEAD_DATA_WITHOUT_CS;
BOOL xdata SecondWrite;
BOOL xdata BackupSecondWrite;
TTaskHandle xdata SetHeadDataTaskHandle;
BYTE xdata SetHeadDataError;
BYTE xdata DataLength;

// Every bit is the valid bit of an head data
BYTE xdata HeadsValidData;



/****************************************************************************
 *
 *  NAME        : HeadData_Init 
 *
 *  DESCRIPTION : Initilaization of the 'HeadData' module
 *
 ****************************************************************************/
void HeadData_Init()
{
  SIZE_OF_HEAD_DATA = sizeof(THeadData);
	SIZE_OF_HEAD_DATA_WITHOUT_CS = SIZE_OF_HEAD_DATA - sizeof(WORD);
  SetHeadDataTaskHandle = SchedulerInstallTask(HeadData_SetHeadDataTask);

	HeadsValidData = 0;
}


/****************************************************************************
 *
 *  NAME        : HeadData_ReadDataFromAllE2PROMs 
 *
 *  DESCRIPTION : Read the head data from all the heads, check if the data is valid,
 *                if not do the same for the backup data                                         
 *
 ****************************************************************************/
void HeadData_ReadDataFromAllE2PROMs()
{
  BYTE xdata i, Length, CurrLength;
	WORD Address;
	BYTE xdata *Data;
	WORD CheckSum;

//	#ifdef OCB_SIMULATOR
//	return ;
//	#else
	TimerDelay(10);
//	#endif
  for(i = 0; i < NUM_OF_HEADS; i++)
	{
	  // Read the head data
	  while(E2PROMReadBlock(i, BASE_ADDRESS, SIZE_OF_HEAD_DATA) != E2PROM_NO_ERROR);
    while(E2PROMGetReceivedData(i, (BYTE *)&HeadsData[i], SIZE_OF_HEAD_DATA) == E2PROM_BUSY);

		CheckSum = Utils_CalculateCheckSum((BYTE *)&HeadsData[i], SIZE_OF_HEAD_DATA_WITHOUT_CS);
		if (CheckSum != HeadsData[i].CheckSum || CheckSum == 0)
		{
      // Read the backup head data
	    while(E2PROMReadBlock(i, BACKUP_BASE_ADDRESS, SIZE_OF_HEAD_DATA) != E2PROM_NO_ERROR);
      while(E2PROMGetReceivedData(i, (BYTE *)&HeadsData[i], SIZE_OF_HEAD_DATA) == E2PROM_BUSY);
      
			CheckSum = Utils_CalculateCheckSum((BYTE *)&HeadsData[i], SIZE_OF_HEAD_DATA_WITHOUT_CS); 
			if (CheckSum == HeadsData[i].CheckSum && CheckSum != 0)
			{
        // Write the backup data to the non-backup area  
				Length = SIZE_OF_HEAD_DATA;
				Address = BASE_ADDRESS;
 			  Data = (BYTE *)&HeadsData[i]; 
        while (Length > 0)
				{
				  while (E2PROMWriteEnable(i) != E2PROM_NO_ERROR);
          while (E2PROMGetWriteStatus(SetHeadDataTaskParams.HeadNum) == E2PROM_BUSY);

          CurrLength = E2PR0M_PAGE_SIZE - (Address % E2PR0M_PAGE_SIZE); 

          while(E2PROMWriteBlock(i, Address, Data, CurrLength)!= E2PROM_NO_ERROR);
          while(E2PROMGetWriteStatus(SetHeadDataTaskParams.HeadNum) == E2PROM_BUSY);

					Length -= CurrLength;
					Address += CurrLength;
					Data += CurrLength;
				}

				// Mark that this head has valid data
				HeadsValidData |= (1 << i);
			}
    }
	  else
		{
		  // Mark that this head has valid data
			HeadsValidData |= (1 << i);
		}

	}

	if (HeadsValidData != ALL_HEADS_DATA_ARE_VALID)
	{
	  // Send notification to Eden
		THeadsDataNotValidMsg Msg;
		Msg.MsgId = HEADS_DATA_NOT_VALID_MSG;
		Msg.HeadsValidData = HeadsValidData; 
    OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsDataNotValidMsg),EDEN_DEST_ID,0,FALSE);
	}

}


/****************************************************************************
 *
 *  NAME        : HeadData_GetHeadData 
 *
 *  DESCRIPTION : Returns the head data of a specific head                                            
 *
 ****************************************************************************/
BOOL HeadData_GetHeadData(int HeadNum, BYTE* Data)
{
  // Copy the head data for this head without the check sum 
  memcpy(Data, &HeadsData[HeadNum], SIZE_OF_HEAD_DATA/*SIZE_OF_HEAD_DATA_WITHOUT_CS*/);

	return (HeadsValidData & (1 << HeadNum));
} 


/****************************************************************************
 *
 *  NAME        : HeadData_SetHeadDataTaskParams 
 *
 *  DESCRIPTION : Set the parameters for the 'SetHeadDataTask'                                           
 *
 ****************************************************************************/
void HeadData_SetHeadDataTaskParams(BYTE DestId, BYTE TransactionId, BYTE HeadNum, BYTE* HeadData)
{
  SetHeadDataTaskParams.DestId = DestId;
	SetHeadDataTaskParams.TransactionId = TransactionId;
	SetHeadDataTaskParams.HeadNum = HeadNum;
	memcpy(&SetHeadDataTaskParams.HeadData, HeadData, SIZE_OF_HEAD_DATA_WITHOUT_CS);

	// Calculate the checksum
	SetHeadDataTaskParams.HeadData.CheckSum = Utils_CalculateCheckSum(HeadData, SIZE_OF_HEAD_DATA_WITHOUT_CS);

	SecondWrite = FALSE;
	BackupSecondWrite = FALSE;
	SetHeadDataError = 0;
	DataLength = SIZE_OF_HEAD_DATA;

}


/****************************************************************************
 *
 *  NAME        : HeadData_SetHeadDataTask 
 *
 *  DESCRIPTION : This task writes the data received to a specific head (also 
 *                to the backup data area) and performs verification                                          
 *
 ****************************************************************************/
void HeadData_SetHeadDataTask(BYTE State)
{
  enum{
	  WRITE_ENABLE,
		WAIT_FOR_ENABLE_OP_DONE,
    WRITE_BLOCK,
		WAIT_FOR_WRITE_OP_DONE,
		VERIFY,
		WAIT_FOR_VERIFY_OP_DONE,
		VERIFY_BACKUP,
		WAIT_FOR_VERIFY_BACKUP_DONE,
		SEND_ACK,
		SEND_WRITE_ERROR_NOTIFICATION
  };
  
  BYTE xdata CurrLength;
	static WORD xdata Address = BASE_ADDRESS;
	static BYTE xdata *Data = &SetHeadDataTaskParams.HeadData;
  THeadData xdata VerifyData;

  switch(State)
	{
	  case WRITE_ENABLE:
		  if (E2PROMWriteEnable(SetHeadDataTaskParams.HeadNum) == E2PROM_NO_ERROR)
	  	  SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
		  else
			  SchedulerLeaveTask(WRITE_ENABLE);
		  break;

	  case WAIT_FOR_ENABLE_OP_DONE:
		  if (E2PROMGetWriteStatus(SetHeadDataTaskParams.HeadNum) != E2PROM_BUSY)
	  	  SchedulerLeaveTask(WRITE_BLOCK);
		  else
			  SchedulerLeaveTask(WAIT_FOR_ENABLE_OP_DONE);
		  break;

	  case WRITE_BLOCK:
		  if (DataLength > E2PR0M_PAGE_SIZE)
			  CurrLength = E2PR0M_PAGE_SIZE - (Address % E2PR0M_PAGE_SIZE);
			else
        CurrLength = DataLength;

      // Write the data to the head E2PROM
      if(E2PROMWriteBlock(SetHeadDataTaskParams.HeadNum, Address, Data, CurrLength)== E2PROM_NO_ERROR)
			  SchedulerLeaveTask(WAIT_FOR_WRITE_OP_DONE);
		  else
			  SchedulerLeaveTask(WRITE_BLOCK);  
	    break;

		case WAIT_FOR_WRITE_OP_DONE:
		  if (E2PROMGetWriteStatus(SetHeadDataTaskParams.HeadNum) != E2PROM_BUSY)
			{
//			#ifdef OCB_SIMULATOR
//			;
//			#else
			TimerDelay(10);
//			#endif
  		  Address += CurrLength;
			  DataLength -= CurrLength; 
				Data += CurrLength;

			  if (DataLength > 0)
				{
 				  SchedulerLeaveTask(WRITE_ENABLE);
				}
				else
				{
				  // If we just finished writing the data, write it also to the backup area
				  if (Address - SIZE_OF_HEAD_DATA == BASE_ADDRESS)
          {
				    Address = BACKUP_BASE_ADDRESS;
            SchedulerLeaveTask(WRITE_ENABLE);
					}
					else
					{
            Address = BASE_ADDRESS;
						SchedulerLeaveTask(VERIFY);
					}
					DataLength = SIZE_OF_HEAD_DATA;
					Data = (BYTE*)&SetHeadDataTaskParams.HeadData;
				}
			}
		  else
			  SchedulerLeaveTask(WAIT_FOR_WRITE_OP_DONE);
		  break;

		case VERIFY:
		  if (E2PROMReadBlock(SetHeadDataTaskParams.HeadNum, BASE_ADDRESS, SIZE_OF_HEAD_DATA) == E2PROM_NO_ERROR)
        SchedulerLeaveTask(WAIT_FOR_VERIFY_OP_DONE); 
			else
        SchedulerLeaveTask(VERIFY);
		  break;

		case WAIT_FOR_VERIFY_OP_DONE:
		  if (E2PROMGetReceivedData(SetHeadDataTaskParams.HeadNum, (BYTE *)&VerifyData, SIZE_OF_HEAD_DATA) != E2PROM_BUSY)
			{			  
 			  if (Utils_CalculateCheckSum((BYTE *)&VerifyData, SIZE_OF_HEAD_DATA_WITHOUT_CS) == VerifyData.CheckSum)
          SchedulerLeaveTask(VERIFY_BACKUP);
				else
				{
				  if (SecondWrite)
					{
					  SetHeadDataError |= DATA_AREA_ERROR;
				    SchedulerLeaveTask(VERIFY_BACKUP);
  				}
					else
					{
					  SecondWrite = TRUE;
					  SchedulerLeaveTask(WRITE_ENABLE);
					}
				}
			}
      else
			  SchedulerLeaveTask(WAIT_FOR_VERIFY_OP_DONE);
		  break;

		case VERIFY_BACKUP:
  	  if (E2PROMReadBlock(SetHeadDataTaskParams.HeadNum, BACKUP_BASE_ADDRESS, SIZE_OF_HEAD_DATA) == E2PROM_NO_ERROR)
        SchedulerLeaveTask(WAIT_FOR_VERIFY_BACKUP_DONE); 
			else
        SchedulerLeaveTask(VERIFY_BACKUP);
      break;

		case WAIT_FOR_VERIFY_BACKUP_DONE:
		  if (E2PROMGetReceivedData(SetHeadDataTaskParams.HeadNum, (BYTE *)&VerifyData, SIZE_OF_HEAD_DATA) != E2PROM_BUSY)
			{  
			  if (Utils_CalculateCheckSum((BYTE*)&VerifyData, SIZE_OF_HEAD_DATA_WITHOUT_CS) == VerifyData.CheckSum)
				{
				  // Copy the data to the heads data array
          memcpy(&HeadsData[SetHeadDataTaskParams.HeadNum],&SetHeadDataTaskParams.HeadData, SIZE_OF_HEAD_DATA);
          HeadsData[SetHeadDataTaskParams.HeadNum].CheckSum = VerifyData.CheckSum;

          SchedulerLeaveTask(SEND_ACK);
				}
				else
				{
				  if (BackupSecondWrite)
					{
					  SetHeadDataError |= BACKUP_DATA_AREA_ERROR;
				    SchedulerLeaveTask(SEND_ACK);
  				}
					else
					{
					  Address = BACKUP_BASE_ADDRESS;
						BackupSecondWrite = TRUE;
				    SchedulerLeaveTask(WRITE_ENABLE);  
					}
				}
			}
      else
			  SchedulerLeaveTask(WAIT_FOR_VERIFY_BACKUP_DONE);
 	    break;

		case SEND_ACK:
		{
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = SET_HEAD_DATA_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),SetHeadDataTaskParams.DestId,
 					SetHeadDataTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
			  if (SetHeadDataError == 0)
				{
				  SchedulerLeaveTask(WRITE_ENABLE);
				  SchedulerSuspendTask(-1);
				}
				else
          SchedulerLeaveTask(SEND_WRITE_ERROR_NOTIFICATION);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}

    case SEND_WRITE_ERROR_NOTIFICATION:
		{
			THeadsDataWriteErrorMsg xdata Msg;

			Msg.MsgId = HEADS_DATA_WRITE_ERROR_MSG;
			Msg.HeadNum = SetHeadDataTaskParams.HeadNum;
			Msg.HeadsDataError = SetHeadDataError;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(THeadsDataWriteErrorMsg),EDEN_DEST_ID,0,FALSE) == EDEN_PROTOCOL_NO_ERROR)
			{
        SchedulerLeaveTask(WRITE_ENABLE);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_WRITE_ERROR_NOTIFICATION);
 
		  break;
    }

		default:
		  SchedulerLeaveTask(WRITE_ENABLE);
			break;
		
	}

}


/****************************************************************************
 *
 *  NAME        : HeadData_GetHeadDataTaskHandle 
 *
 *  DESCRIPTION : Returns the handle of the 'SetHeadDataTask' 
 *
 ****************************************************************************/
TTaskHandle HeadData_GetHeadDataTaskHandle()
{
  return SetHeadDataTaskHandle; 
}
