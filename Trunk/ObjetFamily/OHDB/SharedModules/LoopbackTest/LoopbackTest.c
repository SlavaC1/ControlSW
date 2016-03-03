/*===========================================================================
 *   FILENAME       : LoopBackTest {LoopBackTest.c}  
 *   PURPOSE        : A test to check the data path between the Eden to the OHDB  
 *   DATE CREATED   : 13/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
#include <string.h>
#include "LoopBackTest.h"
#include "XilinxInterface.h"
#include "TimerDrv.h"
#include "EdenProtocol.h"
#include "MiniScheduler.h"
#include "MsgDecodeOHDB.h"


// Constants
// =========
#define FIFO_DIRECTION_REG_ADDRESS	0x70 
#define FIFO_STAUTS_REG_ADDRESS			0x72 
#define FIFO_LSB_WORD_REG_ADDRESS		0x70
#define FIFO_MSB_WORD_REG_ADDRESS		0x71


#define SIZE_OF_FIFO_DATA_BUFFER		32

#define FIFO_EMPTY_BIT_MASK					0x0004
#define FIFO_EMPTY									0x04	


// Type definitions
// ================

typedef struct {		
		BYTE MsgId;
		BYTE Length;
		BYTE Data[SIZE_OF_FIFO_DATA_BUFFER];
	}TLoopBackTestMsg;

// Task states
// -----------
enum{
	CHECK_IF_FIFO_NOT_EMPTY = 0,
	WAIT_FOR_CHECK_OP_DONE,
	READ_LSB_WORD_FROM_FIFO,
	WAIT_FOR_LSB_READ_OP_DONE,
	READ_MSB_WORD_FROM_FIFO,
	WAIT_FOR_MSB_READ_OP_DONE,
	SEND_MESSAGE
	};

 
// Local routines
// ==============


// Module variables
// ================
	BYTE xdata FifoDataBuffer[SIZE_OF_FIFO_DATA_BUFFER];
	BYTE xdata FifoBufferIndex = 0;
	bit FifoTimeout = FALSE;
	TIMER_struct xdata FifoTimeoutTimer;


/****************************************************************************
 *
 *  NAME        : LoopBackTestInit 
 *
 *
 *  DESCRIPTION :                                         
 *
 ****************************************************************************/
BOOL LoopBackTestInit()
{
	TXilinxMessage xdata LoopBackInit;

// build the msg to be sent to the xilinx
// --------------------------------------
	LoopBackInit.Address = FIFO_DIRECTION_REG_ADDRESS;
	LoopBackInit.Data = 0;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&LoopBackInit) == XILINX_SEND_FAILED)
		return FALSE;
 
	return TRUE;
	
}

/****************************************************************************
 *
 *  NAME        : IsLoopBackTestInitDone 
 *
 *
 *  DESCRIPTION :                                         
 *
 ****************************************************************************/
BOOL IsLoopBackTestInitDone()
{	
	XILINX_STATUS Status;

// check if the write to the xilinx command is done
// -------------------------------------------------
	Status = XilinxGetWriteActionStatus();
	if (Status != XILINX_BUSY)
		return TRUE;
	
	return FALSE;

}

/****************************************************************************
 *
 *  NAME        : LoopBackTestTask 
 *
 *
 *  DESCRIPTION :                                         
 *
 ****************************************************************************/
void LoopBackTestTask(BYTE Arg)
{	
	WORD Data;
	XILINX_STATUS Status;

	switch(Arg)
	{
		case CHECK_IF_FIFO_NOT_EMPTY:
			if (XilinxRead(FIFO_STAUTS_REG_ADDRESS) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_CHECK_OP_DONE);
			else
				SchedulerLeaveTask(CHECK_IF_FIFO_NOT_EMPTY);		
			break;

		case WAIT_FOR_CHECK_OP_DONE:
			Status = XilinxGetReceivedData((BYTE*)&Data);
			if ( Status != XILINX_BUSY)
			{	
				if (Status == XILINX_SEND_SUCCESS)
				{
					if ((Data & FIFO_EMPTY_BIT_MASK) != FIFO_EMPTY)
 		 				SchedulerLeaveTask(READ_LSB_WORD_FROM_FIFO);
					else
					{
						if (TimerTimeoutExpired(&FifoTimeoutTimer) == TIMEOUT_EXPIRED)
						{	
							FifoTimeout = TRUE;
							SchedulerLeaveTask(SEND_MESSAGE);
						}
						else
							SchedulerLeaveTask(CHECK_IF_FIFO_NOT_EMPTY);
					}
				}
				else
					SchedulerLeaveTask(CHECK_IF_FIFO_NOT_EMPTY);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_CHECK_OP_DONE);
			break;

		case READ_LSB_WORD_FROM_FIFO:
			if (XilinxRead(FIFO_LSB_WORD_REG_ADDRESS) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_LSB_READ_OP_DONE);
			else
				SchedulerLeaveTask(READ_LSB_WORD_FROM_FIFO);		
			break;

		case WAIT_FOR_LSB_READ_OP_DONE:
			Status = XilinxGetReceivedData((BYTE*)&Data);
			if (Status  != XILINX_BUSY)
			{	
				if (Status == XILINX_SEND_SUCCESS)
				{
					FifoDataBuffer[FifoBufferIndex++] = LSB_MASK & Data;
					FifoDataBuffer[FifoBufferIndex++] = (Data & MSB_MASK) >> 8;
					SchedulerLeaveTask(READ_MSB_WORD_FROM_FIFO);
				}
				else
					SchedulerLeaveTask(READ_LSB_WORD_FROM_FIFO);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_LSB_READ_OP_DONE);
			break;

		case READ_MSB_WORD_FROM_FIFO:
			if (XilinxRead(FIFO_MSB_WORD_REG_ADDRESS) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_MSB_READ_OP_DONE);
			else
				SchedulerLeaveTask(READ_MSB_WORD_FROM_FIFO);		
			break;

		case WAIT_FOR_MSB_READ_OP_DONE:
			Status = XilinxGetReceivedData((BYTE*)&Data);
			if (Status  != XILINX_BUSY)
			{	
				if (Status == XILINX_SEND_SUCCESS)
				{
					FifoDataBuffer[FifoBufferIndex++] = LSB_MASK & Data;
					FifoDataBuffer[FifoBufferIndex++] = (Data & MSB_MASK) >> 8;
					TimerSetTimeout(&FifoTimeoutTimer ,TIMER0_MS_TO_TICKS(500));
					SchedulerLeaveTask(SEND_MESSAGE);
				}
				else
					SchedulerLeaveTask(READ_MSB_WORD_FROM_FIFO);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_MSB_READ_OP_DONE);
			break;
	
		case SEND_MESSAGE:
		{
			TLoopBackTestMsg Msg;

			if (FifoBufferIndex == SIZE_OF_FIFO_DATA_BUFFER || FifoTimeout)
			{
				Msg.MsgId = LOOPBACK_TEST_MSG;
				Msg.Length = FifoBufferIndex;
				memcpy(Msg.Data,FifoDataBuffer,FifoBufferIndex);
				FifoBufferIndex = 0;
				if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TLoopBackTestMsg),
					EDEN_DEST_ID,0,FALSE) == EDEN_PROTOCOL_NO_ERROR)
				{
					SchedulerLeaveTask(CHECK_IF_FIFO_NOT_EMPTY);
					if(FifoTimeout)
 				  {
						FifoTimeout = FALSE;
						SchedulerSuspendTask(-1);
					}
				}
				else
					SchedulerLeaveTask(SEND_MESSAGE);
			}
			else
				SchedulerLeaveTask(CHECK_IF_FIFO_NOT_EMPTY);
		 	break;
		 }


		default:
			SchedulerLeaveTask(CHECK_IF_FIFO_NOT_EMPTY);
			break;

	}
}

