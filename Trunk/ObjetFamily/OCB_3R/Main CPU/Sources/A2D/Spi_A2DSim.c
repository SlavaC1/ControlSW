/*===========================================================================
 *   FILENAME       : SPI A2D Sim {Spi_A2DSim.c}  
 *   PURPOSE        : for SIMUALTOR use only!!! Gets the A/D values from the A2D processor by the SPI 
 *   DATE CREATED   : 28/02/2012
 *   PROGRAMMER     : Luda Margolis 
 *===========================================================================*/
#include <string.h>
#include "Define.h"
#include "Spi_A2D.h"
#include "spiDrv.h"
#include "ByteOrder.h"
#include "MiniScheduler.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h" 
// Local declerations
// ==================

// Constants
// =========
#define A2D_GET_VALUES_INST_INDEX  	0
#define A2D_GET_VALUES_INST_LENGTH	49 // ??? should be: (2 * NO_OF_ANALOG_INPUTS) + 1
#define A2D_GET_SW_VERSION_INST_LENGTH 3 // Command, SW Ver Low Byte, SW Ver High Byte
#define	A2D_BUFFERS_NUM							2
#define SPI_GET_A2D_READINGS_CMD		0x33
#define SPI_GET_A2D_SW_VERSION_CMD	    0x34

// States for Task's State-Machine 
#define SEND_GET_SW_VERSION_CMD			0
#define WAIT_FOR_SW_VERSION_READINGS	1
#define SEND_A2D_CMD					2
#define WAIT_FOR_READINGS				3

#ifdef OCB_SIMULATOR
#define A2D_TASK_DELAY_TIME					1000   // note: original value for non-sim is 190
#else
#define A2D_TASK_DELAY_TIME					190
#endif

	



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
	WORD xdata *A2D_SW_VersionBuffer;

	BYTE xdata A2DTransactionStatus;
	
	BYTE xdata RxBufferNum;

	BYTE xdata TaskCycles;

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
	memset(A2DRxBuffer,0,A2D_BUFFERS_NUM * A2D_GET_VALUES_INST_LENGTH);
	memset(A2DRxBufferSW_Version, 0, A2D_GET_SW_VERSION_INST_LENGTH); 

// the pointer to the samples buffer points to the second byte of the 
// Rx buffer (the first byte is junk received after sending the command)
	A2DSamplesBuffer = (WORD*)(A2DRxBuffer[0] + 1);

// the SW Version buffer (= A2DRxBufferSW_Version) without the first byte
	A2D_SW_VersionBuffer = (WORD*)(A2DRxBufferSW_Version[0] + 1);

	RxBufferNum = 0;

	TaskCycles = 0;

// Route vref to a port pin (for use of the A2D CPU)
  REF0CN |= 0x01;
  
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
    // Get SW Version data on Nth cycle. Let A2D stabilize. 
	// (See if this causes the bug)
    if (TaskCycles >= 10 && TaskCycles < 20 && Arg == SEND_A2D_CMD) 
	{
	  Arg = SEND_GET_SW_VERSION_CMD;
	  TaskCycles = 30;
	}
	else if (TaskCycles < 10)
	  TaskCycles++;

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
				SchedulerLeaveTask(SEND_A2D_CMD);
				SchedulerTaskSleep(-1, A2D_TASK_DELAY_TIME);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_READINGS);
			break;

		case SEND_GET_SW_VERSION_CMD: // done once, at startup, and goes on to A2D sampling.
			if (SpiA2D_GetSwVersionBySpi() == A2D_NO_ERROR)
 		  	    SchedulerLeaveTask(WAIT_FOR_SW_VERSION_READINGS);
			else
				SchedulerLeaveTask(SEND_GET_SW_VERSION_CMD);
			break;

		case WAIT_FOR_SW_VERSION_READINGS:
			if (SpiA2D_GetValuesCMDStatus() != A2D_BUSY)
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
void SpiA2D_CallBack(BYTE Status) // using 3 // For SIM this is not needed. 
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


// unlock the spi since the operation is done
// ------------------------------------------
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
	WORD xdata retVal = 0;
	int test = 0;
	switch (AnalogInNum){	
	case ANALOG_IN_8:
		EXTMem_ReadAnalog12(ANALOG_IN_8_ADD, &retVal);
		break;
	case ANALOG_IN_9:  //tray heater
		EXTMem_ReadAnalog12(ANALOG_IN_9_ADD, &retVal);
		break;
	case ANALOG_IN_18:
		test = test +1;
		break;
	case ANALOG_IN_20:
		EXTMem_ReadAnalog12(ANALOG_IN_20_ADD, &retVal);
	  	break;
	case ANALOG_IN_21:
		EXTMem_ReadAnalog12(ANALOG_IN_21_ADD, &retVal);
	  	break;
/*	case ANALOG_IN_6 + SIM_OHDB_OFFSET:	
	case ANALOG_IN_7 + SIM_OHDB_OFFSET:	
	case ANALOG_IN_8 + SIM_OHDB_OFFSET:	
	case ANALOG_IN_9 + SIM_OHDB_OFFSET:	
	case ANALOG_IN_10 + SIM_OHDB_OFFSET:	
	case ANALOG_IN_11 + SIM_OHDB_OFFSET:	
	case ANALOG_IN_12 + SIM_OHDB_OFFSET:	
		test = test +1;
		break;
		*/
	case (ANALOG_IN_13+SIM_OHDB_OFFSET): //M1_THERMISTOR_LEVEL_SENSOR
		EXTMem_ReadAnalog12(ANALOG_IN_13_ADD_OHDB, &retVal);
		break;
	case ANALOG_IN_14+SIM_OHDB_OFFSET: //M2_THERMISTOR_LEVEL_SENSOR
		EXTMem_ReadAnalog12(ANALOG_IN_14_ADD_OHDB, &retVal);
		break;
	case ANALOG_IN_15+SIM_OHDB_OFFSET: //M3_THERMISTOR_LEVEL_SENSOR
		EXTMem_ReadAnalog12(ANALOG_IN_15_ADD_OHDB, &retVal);
		break;
	case ANALOG_IN_16+SIM_OHDB_OFFSET: //SUPPORT_THERMISTOR_LEVEL_SENSOR
		EXTMem_ReadAnalog12(ANALOG_IN_16_ADD_OHDB, &retVal);
		break;
//	case ANALOG_IN_17 + SIM_OHDB_OFFSET:	
//		test = test +1;
//		break;
	case ANALOG_IN_18 + SIM_OHDB_OFFSET:	//VACUUM
		EXTMem_ReadAnalog12(ANALOG_IN_18_ADD_OHDB, &retVal);	
		break;
	case ANALOG_IN_21+SIM_OHDB_OFFSET: //Voltages OHDB - VPP
		EXTMem_ReadAnalog12(ANALOG_IN_21_ADD_OHDB, &retVal);
		break;
	case ANALOG_IN_22+SIM_OHDB_OFFSET: //SUPPORT_M3_THERMISTOR_LEVEL_SENSOR
		EXTMem_ReadAnalog12(ANALOG_IN_22_ADD_OHDB, &retVal);
		break;
	case ANALOG_IN_23+SIM_OHDB_OFFSET: //Voltages OHDB - VDD
		EXTMem_ReadAnalog12(ANALOG_IN_23_ADD_OHDB, &retVal);
		break;
	case ANALOG_IN_24+SIM_OHDB_OFFSET:	//M1_M2_THERMISTOR_LEVEL_SENSOR
		EXTMem_ReadAnalog12(ANALOG_IN_24_ADD_OHDB, &retVal);
	   break;
	   }	//end switch case

	return retVal;
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
void SpiA2D_GetBlockReadings(BYTE FirstAnalogIn, BYTE LastAnalogIn, WORD *Buffer)
{
		WORD tempBuff[ANALOG_IN_12_ADD_OHDB+1 ];
	int i;
	if (FirstAnalogIn == (ANALOG_IN_1 + SIM_OHDB_OFFSET) && LastAnalogIn == (ANALOG_IN_12 + SIM_OHDB_OFFSET)) {		 //heater temp reading from Ext.mem

	for (i = FirstAnalogIn; i<=LastAnalogIn ;i++ ){
	switch (i){
	case ANALOG_IN_1+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_1_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_2+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_2_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_3+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_3_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_4+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_4_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_5+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_5_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_6+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_6_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_7+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_7_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_8+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_8_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_9+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_9_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_10+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_10_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_11+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_11_ADD_OHDB, &tempBuff[i]);
		break;
	case ANALOG_IN_12+ SIM_OHDB_OFFSET:
		EXTMem_ReadAnalog12(ANALOG_IN_12_ADD_OHDB, &tempBuff[i]);
		break;
	default: ;

	} //end switch	
	} //end for
	memcpy(Buffer,&(tempBuff[FirstAnalogIn]), (LastAnalogIn-FirstAnalogIn+1)*sizeof(WORD));
	}//if for heaters temperature simulation


	
	return;
}

void SpiA2D_GetSW_Version(BYTE* ExternalVer, BYTE* InternalVer)
{
// We need not disable the ISRs, since the version buffer is updated only *once* and hence safe.
	*ExternalVer = A2DRxBufferSW_Version[1];
	*InternalVer = A2DRxBufferSW_Version[2];
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



/*Taken from OHDB spi_A2D*/
BOOL SpiA2D_IsDataValid()
{
  return TRUE;
}





