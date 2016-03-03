/*===========================================================================
 *   FILENAME       : SPI External A2D {SpiExtA2D.h}  
 *   PURPOSE        : Gets the A/D values from the external A2D LTC1863 unit by SPI
 *                    This unit has 8 analog inputs, but currently we use only 4 for
 *                    upper thermistors
 *   DATE CREATED   : 19/2/2014
 *   PROGRAMMER     : Slava Chuhovich 
 *===========================================================================*/
 
#include "SpiExtA2D.h"
#include "spidrv.h"
#include "MiniScheduler.h"
#include <string.h> // memset

#define EXT_A2D_TASK_DELAY_TIME 100
#define EXT_A2D_BASIC_CMD       0x84 // Basic control command: SD = 1 (MSB), OS, S1, S0 - for channel address configuration, COM = 0, UNI = 1, SLP = 0 (7 bit)

BYTE xdata  ExtA2DTransactionStatus;
BYTE xdata  SelectedChannel;
BYTE xdata  RxBufferNumber;

// This buffer holds the values for all channels
static WORD xdata ExtA2DSamplesBuffer[NUM_OF_EXT_A2D_CHANNELS];

// A buffer to receive data. We use a double buffer
BYTE xdata ExtA2DRxBuffer[2][EXT_A2D_BUFFER_SIZE];

// This order is according to LTC1863 unit data sheet
BYTE xdata ExtA2DChannels[NUM_OF_EXT_A2D_CHANNELS] = 
{
	EXT_A2D_CH0, 
	EXT_A2D_CH2, 
    EXT_A2D_CH4, 
	EXT_A2D_CH6, 
	EXT_A2D_CH1, 
	EXT_A2D_CH3, 
	EXT_A2D_CH5, 
	EXT_A2D_CH7
};

// To mark if the transaction was successfully performed 
void SpiExtA2D_CallBack(BYTE Status);

SPI_EXT_A2D_STATUS SpiExtA2D_SendRequest();
SPI_EXT_A2D_STATUS SpiExtA2D_GetValues();
BYTE*              SpiExtA2D_GetFreeBuffer();


void SpiExtA2D_Init()
{
	SelectedChannel         = 0;
	RxBufferNumber          = 0;
	ExtA2DTransactionStatus = EXT_A2D_NO_ERROR;
	
	memset(ExtA2DSamplesBuffer, 0, NUM_OF_EXT_A2D_CHANNELS * sizeof(WORD));
}

void SpiExtA2D_Task(BYTE Arg)
{
	enum
	{
		SEND_A2D_REQUEST,
		WAIT_FOR_RESULTS		
	};
	
	switch(Arg)
	{
		case SEND_A2D_REQUEST:
		{
			if(SpiExtA2D_SendRequest() == EXT_A2D_NO_ERROR)
 		  	    SchedulerLeaveTask(WAIT_FOR_RESULTS);
			else
				SchedulerLeaveTask(SEND_A2D_REQUEST);	
		}
		break;
		
		case WAIT_FOR_RESULTS:
		{
			if(SpiExtA2D_GetValues() != EXT_A2D_BUSY)
 		    {	
				SchedulerLeaveTask(SEND_A2D_REQUEST);
				SchedulerTaskSleep(-1, EXT_A2D_TASK_DELAY_TIME);
			}
			else
			{
				SchedulerLeaveTask(WAIT_FOR_RESULTS);	
			}
		}
		break;
		
		default:
			break;	
	}
}

// Sends the command to request the A2D values by SPI
SPI_EXT_A2D_STATUS SpiExtA2D_SendRequest()
{
	BYTE xdata *Message;

	// Try to lock the SPI device
	if(SpiLock() == SPI_BUSY)
		return EXT_A2D_SEND_FAILED;

	// Set the device status to busy
	ExtA2DTransactionStatus = EXT_A2D_BUSY;

	// Prepare the message to be send using the spi
	Message = SpiGetTxBuffer();
	Message[EXT_A2D_CMD_INDEX]  = EXT_A2D_BASIC_CMD | (ExtA2DChannels[SelectedChannel] << 4);  // The actual address is bits 4, 5, 6 
	Message[EXT_A2D_DATA_INDEX] = 0x00;                                                        // To receive the data here	
	
	// Select the device (CS)
	SpiSelectSlave(A2D_EXT_SLAVE_ID, CHIP_SELECT);

	// Send the message using the spi
	SpiSend(EXT_A2D_BUFFER_SIZE, SpiExtA2D_GetFreeBuffer(), SpiExtA2D_CallBack);
			 
	return EXT_A2D_NO_ERROR;
}

SPI_EXT_A2D_STATUS SpiExtA2D_GetValues()
{
	WORD idata InputData;
	BYTE idata PrevChannel;
	
	// If the SPI operation is not done yet
	if(ExtA2DTransactionStatus == EXT_A2D_BUSY)	
		return EXT_A2D_BUSY; 

	// Deselect the device (CS)
	SpiSelectSlave(A2D_EXT_SLAVE_ID, CHIP_DESELECT);	

	// Copy the read data for the selected channel
	InputData  = ExtA2DRxBuffer[RxBufferNumber][0];
	InputData  = InputData << 8;
	InputData |= ExtA2DRxBuffer[RxBufferNumber][1];
	
	// We get the values for the previous channel. 
	PrevChannel = (SelectedChannel == 0) ? (NUM_OF_EXT_A2D_CHANNELS - 1) : (SelectedChannel - 1);
	
	// Normalizing to 12bit A2D
	ExtA2DSamplesBuffer[PrevChannel] = InputData >> 4; 

	// Increment the counter for the next channel
	SelectedChannel	= ++SelectedChannel % NUM_OF_EXT_A2D_CHANNELS;    

	// Unlock the spi since the operation is done
	SpiUnLock();

	return ExtA2DTransactionStatus;
}

void SpiExtA2D_CallBack(BYTE Status) using 3
{
	if(Status == SPI_NO_ERROR)
		ExtA2DTransactionStatus = EXT_A2D_SEND_SUCCESS;
	else
		ExtA2DTransactionStatus = EXT_A2D_SEND_FAILED;
}

BYTE* SpiExtA2D_GetFreeBuffer()
{
	RxBufferNumber = 1 - RxBufferNumber;
	return ExtA2DRxBuffer[RxBufferNumber];		
}

WORD SpiExtA2D_GetReading(BYTE ChannelIndex)
{
	return ExtA2DSamplesBuffer[ChannelIndex];
}