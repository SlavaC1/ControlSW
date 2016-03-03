/*===========================================================================
 *   FILENAME       : SPI A2D {Spi_A2D.c}  
 *   PURPOSE        : Gets the A/D values from the A2D processor by the SPI 
 *   DATE CREATED   : 29/4/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#include <string.h>
#include "Define.h"
#include "Spi_A2D.h"
#include "spidrv.h"
#include "MiniScheduler.h"

// Local declerations
// ==================

// Constants
// =========
#define A2D_GET_VALUES_INST_INDEX  	    0
#define A2D_GET_VALUES_INST_LENGTH	    49 // ??? should be: (2 * NO_OF_ANALOG_INPUTS) + 1
#define A2D_GET_SW_VERSION_INST_LENGTH  3 // Command, SW Ver Low Byte, SW Ver High Byte
#define	A2D_BUFFERS_NUM							2
#define SPI_GET_A2D_READINGS_CMD		0x33
#define SPI_GET_A2D_SW_VERSION_CMD	    0x34
#define A2D_TASK_DELAY_TIME				190
#define A2D_GET_SW_VERSION_RETRIES      5	



// Local variables
// ===============

// buffers for the samples recieved by the spi. The size is the number of 
// analog input * 2 (the samples are words) + 1 for the first byte sent - the command
	BYTE xdata A2DRxBuffer[A2D_BUFFERS_NUM][A2D_GET_VALUES_INST_LENGTH];
    
// Command + SW Ver Low Byte + SW Ver High Byte 
	BYTE xdata A2DRxBufferSW_Version[A2D_GET_SW_VERSION_INST_LENGTH];  

// the samples buffer (= A2DRxBuffer without the first byte)
	WORD xdata *A2DSamplesBuffer;

// the SW Version buffer (= A2DRxBufferSW_Version) without the first byte
	BYTE xdata A2D_SW_VersionBuffer[2];

	BYTE xdata A2DTransactionStatus;
	
	BYTE xdata RxBufferNum;

    BOOL xdata DataIsValid;

    BYTE xdata GetVersionRetries;

// Local routines
// ==============
	void SpiA2D_CallBack(BYTE Status);

	BYTE* SpiA2D_GetFreeBuffer();

	SPI_A2D_STATUS SpiA2D_GetSwVersionBySpi();


/****************************************************************************
 *
 *  NAME        : SpiA2D_Init
 *
 *  DESCRIPTION : Initialization of the modoule
 *
 ****************************************************************************/
void SpiA2D_Init (void)
{
// zero the Rx Buffers:
	memset(A2DRxBuffer,           0, A2D_BUFFERS_NUM * A2D_GET_VALUES_INST_LENGTH);
	memset(A2DRxBufferSW_Version, 0, A2D_GET_SW_VERSION_INST_LENGTH);
	memset(A2D_SW_VersionBuffer,  0, 2); 	

// the pointer to the samples buffer points to the second byte of the 
// Rx buffer (the first byte is junk received after sending the command)
	A2DSamplesBuffer = (WORD*)(A2DRxBuffer[0] + 1);

	RxBufferNum = 0;

    GetVersionRetries = 0;

	SFRPAGE = SPI0_PAGE; 

// Route vref to a port pin (for use of the A2D CPU)
  REF0CN |= 0x01;
  
  DataIsValid = FALSE;

  // A2D SW Ver. is updated only once - here:
  // SpiA2D_GetSwVersionBySpi();    
}
/****************************************************************************
 *
 *  NAME        : SpiA2D_Task
 *
 *  DESCRIPTION : The A2D task for getting the current A2D reading from the 
 *								A2D processor
 *
 ****************************************************************************/
void SpiA2D_Task(BYTE Arg)
{
	enum
	{
		SEND_GET_SW_VERSION_CMD,
		WAIT_FOR_SW_VERSION_READINGS,
		SEND_A2D_CMD,
		WAIT_FOR_READINGS
	};
	
	switch (Arg)
	{
		case SEND_A2D_CMD:		
			if (SpiA2D_GetValuesBySpi() == A2D_NO_ERROR)
 		  	    SchedulerLeaveTask(WAIT_FOR_READINGS);
			else
				SchedulerLeaveTask(SEND_A2D_CMD);
			break;

		case WAIT_FOR_READINGS:
			if (SpiA2D_GetValuesCMDStatus() != A2D_BUSY)
 		    {	
				if(TRUE == AreRetriesNeeded_GetSwVer())
					SchedulerLeaveTask(SEND_GET_SW_VERSION_CMD);
				else
					SchedulerLeaveTask(SEND_A2D_CMD);
				
				SchedulerTaskSleep(-1, A2D_TASK_DELAY_TIME);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_READINGS);
			break;

		case SEND_GET_SW_VERSION_CMD: 
			if (SpiA2D_GetSwVersionBySpi() == A2D_NO_ERROR)
 		  	    SchedulerLeaveTask(WAIT_FOR_SW_VERSION_READINGS);
			else
				SchedulerLeaveTask(SEND_GET_SW_VERSION_CMD);
			break;

		case WAIT_FOR_SW_VERSION_READINGS:
			if (SpiA2D_GetVersionCMDStatus() != A2D_BUSY)
 		    {	
				SchedulerLeaveTask(SEND_A2D_CMD);				
				SchedulerTaskSleep(-1, A2D_TASK_DELAY_TIME);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_SW_VERSION_READINGS);
			break;

		 default:
		    SchedulerLeaveTask(SEND_A2D_CMD);
			break;

	 }
	 return;
}


/****************************************************************************
 *
 *  NAME        : SpiA2D_GetValuesBySpi
 *
 *  DESCRIPTION : Sends "Get A2D values" to the A2D processor and recives the readings
 *
 ****************************************************************************/
SPI_A2D_STATUS SpiA2D_GetValuesBySpi()
{
	BYTE xdata *Message, i;

// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return A2D_SEND_FAILED;

// set the device status to busy
// -----------------------------
	A2DTransactionStatus = A2D_BUSY;

// prepare the message to be send using the spi
// --------------------------------------------
	Message = SpiGetTxBuffer();
	Message[A2D_GET_VALUES_INST_INDEX] = SPI_GET_A2D_READINGS_CMD;
	for (i = 1; i <= NO_OF_ANALOG_INPUTS*2; i++)
		Message[i] = 0;

	SpiSetCommWithA2D(TRUE);

// select the device (CS)
// ----------------------
	SpiSelectSlave(A2D_PROCESSOR_SLAVE_ID,CHIP_SELECT);

// send the message using the spi
// ------------------------------
	SpiSend(A2D_GET_VALUES_INST_LENGTH ,SpiA2D_GetFreeBuffer(), SpiA2D_CallBack);
			 
	return A2D_NO_ERROR;
} 

/****************************************************************************
 *
 *  NAME        : SpiA2D_GetSwVersionBySpi
 *
 *  DESCRIPTION : Sends "Get SW Version" to the A2D processor and recives Version two-byte value.
 *
 ****************************************************************************/
SPI_A2D_STATUS SpiA2D_GetSwVersionBySpi()
{
	BYTE xdata *Message;

// try to lock the SPI device
// --------------------------
	if (SpiLock() == SPI_BUSY)
		return A2D_SEND_FAILED;

// set the device status to busy
// -----------------------------
	A2DTransactionStatus = A2D_BUSY;

// prepare the message to be send using the spi
// --------------------------------------------
	Message = SpiGetTxBuffer(); // get the pointer to the transmit buffer used by the Spi_Driver.

	Message[A2D_GET_VALUES_INST_INDEX] = SPI_GET_A2D_SW_VERSION_CMD;

	// 0xFF is for debugging. change to zero.
	Message[1] = 0; //0xFF; // Sending 1st zero - for receiving Low SW Version BYTE.
	Message[2] = 0; //0xFF; // Sending 2nd zero - for receiving High SW Version BYTE.

	SpiSetCommWithA2D(TRUE);

// select the device (CS)
// ----------------------
	SpiSelectSlave(A2D_PROCESSOR_SLAVE_ID,CHIP_SELECT);

// send the message using the spi
// ------------------------------
	SpiSend(A2D_GET_SW_VERSION_INST_LENGTH, A2DRxBufferSW_Version, SpiA2D_CallBack);
			 
	return A2D_NO_ERROR;
} 


/****************************************************************************
 *
 *  NAME        : SpiA2D_CallBack
 *
 *  DESCRIPTION : Called from the spi ISR, updates the transaction status.                                        
 *
 ****************************************************************************/
void SpiA2D_CallBack(BYTE Status) using 3
{
	if (Status == SPI_NO_ERROR)
		A2DTransactionStatus = A2D_SEND_SUCCESS;
	else
		A2DTransactionStatus = A2D_SEND_FAILED;
}


/****************************************************************************
 *
 *  NAME        : SpiA2D_GetValuesCMDStatus
 *
 *  DESCRIPTION : Return the status of the last "Get A2D values" command
 *								Call it after A2DGetValuesBySpi                                        
 *
 ****************************************************************************/
SPI_A2D_STATUS SpiA2D_GetValuesCMDStatus()
{
// if the spi operation is not done yet
// ------------------------------------
	if (A2DTransactionStatus == A2D_BUSY)	
		return A2D_BUSY; 

// deselect the device (CS)
// ------------------------
	SpiSelectSlave(A2D_PROCESSOR_SLAVE_ID,CHIP_DESELECT);

	SpiSetCommWithA2D(FALSE);

	A2DSamplesBuffer = (WORD*)(A2DRxBuffer[RxBufferNum] + 1);

    DataIsValid = TRUE;

// unlock the spi since the operation is done
// ------------------------------------------
	SpiUnLock();

	return A2DTransactionStatus;

}

SPI_A2D_STATUS SpiA2D_GetVersionCMDStatus()
{
	if (A2DTransactionStatus == A2D_BUSY)	
		return A2D_BUSY;

	SpiSelectSlave(A2D_PROCESSOR_SLAVE_ID,CHIP_DESELECT);

	SpiSetCommWithA2D(FALSE);

	A2D_SW_VersionBuffer[0] = A2DRxBufferSW_Version[1];
	A2D_SW_VersionBuffer[1] = A2DRxBufferSW_Version[2];

    DataIsValid = TRUE;

	SpiUnLock();

	return A2DTransactionStatus;
}

/****************************************************************************
 *
 *  NAME        : SpiA2D_GetReading
 *
 *  INPUT       : Analog input number
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : Return The last digital reading for this analog input.
 *
 ****************************************************************************/
WORD SpiA2D_GetReading(BYTE AnalogInNum)
{
	WORD RetVal;
	
	EIE1 &= ~ENABLE_SPI_INTERRUPT;

	RetVal = A2DSamplesBuffer[AnalogInNum];
	
	// Enable SPI interrupt
	// --------------------						 
  	EIE1 |= ENABLE_SPI_INTERRUPT;

	return RetVal;
}


/****************************************************************************
 *
 *  NAME        : SpiA2D_GetBlockReadings
 *
 *  INPUT       : Analog input range (first, last), Buffer for the readings
 *
 *  OUTPUT      : Digital reading.
 *
 *  DESCRIPTION : Return The last digital reading for this analog input.
 *
 ****************************************************************************/
void SpiA2D_GetBlockReadings(BYTE FirstAnalogIn, LastAnalogIn, WORD *Buffer)
{

	EIE1 &= ~ENABLE_SPI_INTERRUPT;

// get the readings
// ----------------
	memcpy(Buffer,&(A2DSamplesBuffer[FirstAnalogIn]), (LastAnalogIn-FirstAnalogIn+1)*sizeof(WORD));

// Enable SPI interrupt
// --------------------						 
  EIE1 |= ENABLE_SPI_INTERRUPT;

	return;
}

void SpiA2D_GetSW_Version(BYTE* ExternalVer, BYTE* InternalVer)
{
	EIE1 &= ~ENABLE_SPI_INTERRUPT;

	*ExternalVer = A2D_SW_VersionBuffer[0];
	*InternalVer = A2D_SW_VersionBuffer[1];
	
	EIE1 |= ENABLE_SPI_INTERRUPT;
}


/****************************************************************************
 *
 *  NAME        : SpiA2D_GetFreeBuffer
 *
 *  DESCRIPTION : Returns a free buffer for receiving A2D values
 *
 ****************************************************************************/
BYTE* SpiA2D_GetFreeBuffer()
{
	RxBufferNum = 1 - RxBufferNum;
	return A2DRxBuffer[RxBufferNum];		
}

BOOL SpiA2D_IsDataValid()
{
  return DataIsValid;
}

BOOL AreRetriesNeeded_GetSwVer()
{
	BYTE ExternalVer, InternalVer;
	
	ExternalVer = 0;
	InternalVer = 0;
	
	SpiA2D_GetSW_Version(&ExternalVer, &InternalVer);
	
	if(0 == ExternalVer || GetVersionRetries > A2D_GET_SW_VERSION_RETRIES)
	{
		GetVersionRetries++;
		return TRUE; 
	}
	
	return FALSE;
}



