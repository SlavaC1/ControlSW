
#include "Fans.h"

#include <stdio.h>
#include <stdlib.h>
int seed = 10;

/****************************************************************************
 *
 *  NAME        : FansReadDataTask
 *
 *  DESCRIPTION : 2 seconds periodic task that reads the fans data from 
 *                the Xilinx device and sends the data.
 ****************************************************************************/
 
 static int DataMsgIndex = 0;	// Go over the fans registers, read the register data and fill the entry in the message sent to Eden.
 TFansDataMsg xdata FansMsg;
 
 
void FansReadDataTask(BYTE Arg)
{
	enum{
	SEND_READ_CMD,
	WAIT_FOR_OP_DONE,
	SEND_DATA
	};
	
	switch (Arg)
	{
		case SEND_READ_CMD:
		{
			if( XILINX_NO_ERROR == FansReadData() )
			{
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			}			  
			else
			{
				SchedulerTaskSleep(-1,FANS_READ_DATA_TASK_PERIODIC_TIME);
				SchedulerLeaveTask(SEND_READ_CMD);
			}
			break;
		}
		case WAIT_FOR_OP_DONE:
		{
			if ( XILINX_SEND_SUCCESS == FansGetData() )
			{
				if ( FANS_DATA_LENGTH > DataMsgIndex )
				{
					DataMsgIndex++;
					SchedulerLeaveTask(SEND_READ_CMD);
				}
				if ( FANS_DATA_LENGTH <= DataMsgIndex )
				{
					DataMsgIndex = 0;
					SchedulerLeaveTask(SEND_DATA);
				}
			}	
			else
			{
				SchedulerTaskSleep(-1,FANS_READ_DATA_TASK_PERIODIC_TIME);
				SchedulerLeaveTask(WAIT_FOR_OP_DONE);
			}
			break;
		}
		case SEND_DATA:
		{				
			if ( EDEN_PROTOCOL_NO_ERROR == SendFansData() )
			{
				SchedulerLeaveTask(SEND_READ_CMD);				
				SchedulerTaskSleep(-1,FANS_READ_DATA_TASK_PERIODIC_TIME);
			}
			else
			{
				SchedulerLeaveTask(SEND_DATA);
			}
		 	break;
		}
 
	 	default:
			SchedulerLeaveTask(SEND_READ_CMD);
			break;

	 }
	 return;
}


XILINX_STATUS FansReadData()
{
		BYTE address;
		XILINX_STATUS ReturnCode;
	
		switch (DataMsgIndex)
		{
			case LEFT_UV_FAN_1:
			{
				address = LEFT_UV_FAN_1_ADDRESS; 
				break;
			}
			case LEFT_UV_FAN_2:
			{
				address = LEFT_UV_FAN_2_ADDRESS;
				break;
			}
			case LEFT_MATERIAL_FAN_1:
			{
				address = LEFT_MATERIAL_FAN_1_ADDRESS;
				break;
			}
			case LEFT_MATERIAL_FAN_2:
			{
				address = LEFT_MATERIAL_FAN_2_ADDRESS;
				break;
			}
			case LEFT_MATERIAL_FAN_3:
			{
				address = LEFT_MATERIAL_FAN_3_ADDRESS;
				break;
			}
			case RIGHT_UV_FAN_1:
			{
				address = RIGHT_UV_FAN_1_ADDRESS;
				break;
			}
			case RIGHT_UV_FAN_2:
			{
				address = RIGHT_UV_FAN_2_ADDRESS;
				break;
			}
			case RIGHT_MATERIAL_FAN_1:
			{
				address = RIGHT_MATERIAL_FAN_1_ADDRESS;
				break;
			}
			case RIGHT_MATERIAL_FAN_2:
			{
				address = RIGHT_MATERIAL_FAN_2_ADDRESS;
				break;
			}
			case MIDDLE_FAN:
			{
				address = MIDDLE_FAN_ADDRESS;
				break;
			}
			case MATERIAL_FANS_STATE:
			{
				address = MATERIAL_FANS_STATE_ADDRESS;
				break;
			}
			case MATERIAL_FANS_DUTY_CYCLE:
			{
				address = MATERIAL_FANS_DUTY_CYCLE_ADDRESS;
				break;
			}
			default:
			{
				address = MIDDLE_FAN_ADDRESS;
				break;
			}
		}		
				
		ReturnCode = XilinxRead(address/*FansRegisterAddresses[DataMsgIndex]*/);
		
		return ReturnCode;
}


XILINX_STATUS FansGetData()
{
		XILINX_STATUS ReturnCode;
		WORD					Data;
	
		if ( FANS_DATA_LENGTH <= DataMsgIndex )
		{
			return XILINX_SEND_FAILED;
		}
		
		ReturnCode = XilinxGetReceivedData( (BYTE*)&Data ); 
		
		if ( XILINX_SEND_SUCCESS == ReturnCode )
		{
				if ( NUM_OF_FANS > DataMsgIndex )
				{
//					Data = 90;//DataMsgIndex;
					FansMsg.FansData[DataMsgIndex] = Raw16bitsToSpeedData8bits(Data);
				}
				else if ( MATERIAL_FANS_DUTY_CYCLE == DataMsgIndex )
				{
//					Data = 0xff;
					FansMsg.FansData[DataMsgIndex] = (BYTE)Data;
				}
				else if ( MATERIAL_FANS_STATE == DataMsgIndex )
				{
//					Data = 0x0700;
					FansMsg.FansData[DataMsgIndex] = (BYTE)((Data >> 8) & 0x07);	// keeping only bits 8, 9, 10
				}
		}
		
		return ReturnCode; 
}

EDEN_PROTOCOL_STATUS SendFansData()
{
		EDEN_PROTOCOL_STATUS ReturnCode;
	
		FansMsg.MsgId = 	FANS_SEND_DATA_MSG;
	
//	SwapUnsignedShortArray(FansMsg.FansData,FansMsg.FansData,FANS_DATA_LENGTH);
	
		ReturnCode = OHDBEdenProtocolSend(	(BYTE*)&FansMsg,
																				sizeof(TFansDataMsg),
																				EDEN_DEST_ID,					
																				0,
																				FALSE 
																			);
																				
		return ReturnCode; 
}


BYTE Raw16bitsToSpeedData8bits(WORD Data)
{
	Data = Data/2;
	if ( 0xff < Data )
	{
		Data = 0xff;
	}
	return ( (BYTE)Data );
	
//	srand(seed++);
//	Data = Data/2;
//	return 200 - rand()%200;
}

