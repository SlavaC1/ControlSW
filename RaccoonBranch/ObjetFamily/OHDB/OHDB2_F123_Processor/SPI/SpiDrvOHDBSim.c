/*===========================================================================
 *   FILENAME       : SPI DRIVER Sim{SPIDRVOHDBSim.C}  
 *   PURPOSE        : Serial Peripheral Interface driver  
 *   DATE CREATED   : 06/12/2011
 *   PROGRAMMER     : Luda Margolis 
 *===========================================================================*/
#include "SpiDrv.h"
#include "..\ExtMemAddSim\ExtMemAddSim.h"
#include "..\EXTMemSim\EXTMem.h" 

// Constants
// =========
const BYTE SelectSlaveArrayPort1[NUM_OF_SPI_SLAVES] = {
									SPI_SELECT_E2PROM_1_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_2_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_3_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_4_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_5_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_6_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_7_VALUE_FOR_P1,
									SPI_SELECT_E2PROM_8_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_1_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_2_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_3_VALUE_FOR_P1,
                  					SPI_SELECT_POTENTIOMETER_4_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_5_VALUE_FOR_P1,
                  					SPI_SELECT_POTENTIOMETER_6_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_7_VALUE_FOR_P1,
									SPI_SELECT_POTENTIOMETER_8_VALUE_FOR_P1,
									SPI_SELECT_ON_BOARD_E2PROM_VALUE_FOR_P1,
									SPI_SELECT_A2D_PROCESSOR_VALUE_FOR_P1,
									SPI_SELECT_XILINX_VALUE_FOR_P1
	                };


const BYTE SelectSlaveArrayPort2[NUM_OF_SPI_SLAVES] = {
									SPI_SELECT_E2PROM_1_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_2_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_3_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_4_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_5_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_6_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_7_VALUE_FOR_P2,
									SPI_SELECT_E2PROM_8_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_1_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_2_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_3_VALUE_FOR_P2,
                 					SPI_SELECT_POTENTIOMETER_4_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_5_VALUE_FOR_P2,
                  					SPI_SELECT_POTENTIOMETER_6_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_7_VALUE_FOR_P2,
									SPI_SELECT_POTENTIOMETER_8_VALUE_FOR_P2,
									SPI_SELECT_ON_BOARD_E2PROM_VALUE_FOR_P2,
									SPI_SELECT_A2D_PROCESSOR_VALUE_FOR_P2,
									SPI_SELECT_XILINX_VALUE_FOR_P2
	                };

#define SPI_MAX_MSG_LEN 	100

#define ADDR_INDEX  0
#define DATA_MSB_INDEX  1
#define DATA_LSB_INDEX  2
#define ENABLE_PRINTING_COMMAND						0x01

// Encoder data (from OHDB) defines
#define OHDB_XILINX_DATA__ENCODER_ADDR			0x80
#define ROLLER_GET_SPEED_ADDRESS				0x41
#define DIAGNOSTIC_REG_ADDRESS					0x04
#define GO_REG_ADDRESS							0x03
// Type definitions
// ================
 

// Local routines
// ==============


// Module variables
// ================

// pointer to the Tx buffer
// ------------------------
	BYTE xdata SpiTxBuffer[SPI_MAX_MSG_LEN];

// pointer to the Rx buffer
// ------------------------
	BYTE xdata *SpiRxBuffer;

// Number of bytes to transmit/receive
// -----------------------------------
	BYTE xdata SpiDataLen;

// currnet index in the Tx buffer
// ------------------------------
	BYTE xdata SpiIndex;

// Is the spi in use
// -----------------
	bit SpiInUse;

// Is the current session is with the A2D processor
// ------------------------------------------------
	bit CommWithA2D;

// pointer to the call back function
// ---------------------------------
	TCallback EndSpiTransactionCallBack;

// delay before setting the NSS to low
// ----------------------------------- 
	BYTE xdata Delay;

//to remmeber where to read
BYTE SlaveId; 

/****************************************************************************
 *
 *  NAME        : SpiInit
 *
 *  DESCRIPTION : Initialize the SPI driver                                        
 *
 ****************************************************************************/
void SpiInit(void)
{
	SpiInUse = FALSE;
	CommWithA2D = FALSE;
	SlaveId = 0;

//DUMMY
	  
}


/****************************************************************************
 *
 *  NAME        : SpiSend
 *
 *  DESCRIPTION : Send a buffer using the SPI                                        
 *
 ****************************************************************************/
 SPI_STATUS SpiSend(unsigned int DataLength ,BYTE *DataIn, TCallback EndTransactionCallBack) 
 {
	BYTE Address;
	WORD dataIn;
	unsigned int length =	DataLength ;
//DUMMY
	//SPI0DAT = SpiTxBuffer[SpiIndex++];
	//get DataIn
 		 DataIn[0] = 0 ;
		 DataIn[1] = 0 ;
   	
	switch(SlaveId){
	case XILINX_SLAVE_ID:
		Address = SpiTxBuffer[ADDR_INDEX];
		dataIn  = SpiTxBuffer[DATA_LSB_INDEX];
		if (Address == OHDB_XILINX_DATA__ENCODER_ADDR ){
			EXTMem_ReadWord(XILINX_ADD_OHDB, (WORD*)&DataIn[0]);
			} 
		if (Address == ROLLER_GET_SPEED_ADDRESS) { //dummy just for now...(before implementing in simulation)
			DataIn[1] = 0xFF;	
		}
		if (Address == DIAGNOSTIC_REG_ADDRESS){
		 DataIn[0] = 0 ;
		 DataIn[1] = 0 ;

		}
		if (Address == GO_REG_ADDRESS && dataIn == ENABLE_PRINTING_COMMAND )  //during printing
			EXTMem_WriteBitsNoLock(XILINX_PRINTING_ADD_OHDB, 1, 0x1);
		else  if (Address == GO_REG_ADDRESS && dataIn != ENABLE_PRINTING_COMMAND )
			EXTMem_WriteBitsNoLock(XILINX_PRINTING_ADD_OHDB, 0, 0x1);

		break;
	}

	EndTransactionCallBack(SPI_NO_ERROR);
	return SPI_NO_ERROR;
 }//End of SpiInit()


/****************************************************************************
 *
 *  NAME        : SpiIsr
 *
 *  DESCRIPTION : SPI interrupt service routine                                        
 *
 ****************************************************************************/
void SpiIsr()interrupt 6 using 3
{

	//DUMMY
	EndSpiTransactionCallBack(SPI_NO_ERROR);
}  // End of  SpiIsr

/****************************************************************************
 *
 *  NAME        : SpiLock
 *
 *  INPUT       : NONE.
 *
 *  OUTPUT      : SPI_STATUS.
 *
 *  DESCRIPTION : try to get a lock on the SPI device.                                        
 *
 ****************************************************************************/
SPI_STATUS SpiLock()
{

	// check if SPI is free
// --------------------
	if (SpiInUse)
		return SPI_BUSY;

// lock the spi
// ------------
	SpiInUse = TRUE;

	return SPI_NO_ERROR;
  
}


 
/****************************************************************************
 *
 *  NAME        : SpiUnLock
 *
 *  INPUT       : NONE.
 *
 *  OUTPUT      : NONE.
 *
 *  DESCRIPTION : unlock the SPI device.                                        
 *
 ****************************************************************************/
void SpiUnLock()
{
	SpiInUse = FALSE;
}


/****************************************************************************
 *
 *  NAME        : SpiSelectSlave
 *
 *  INPUT       : Slave Id, Select/Deselect.
 *
 *  OUTPUT      : NONE.
 *
 *  DESCRIPTION : select/deselect a slave device.                                        
 *
 ****************************************************************************/
void SpiSelectSlave(BYTE SlaveID, BYTE Select)
{

	if (Select == CHIP_SELECT && SlaveID == XILINX_SLAVE_ID)
	{
		//save that reading from xilinx
		SlaveId = XILINX_SLAVE_ID ;

	}
	else   //not impementing yet
	 	SlaveId = 0 ;

}


/****************************************************************************
 *
 *  NAME        : SpiGetTxBuffer
 *
 *  DESCRIPTION : Returns a pointer to the spi transmit buffer.                                        
 *
 ****************************************************************************/
BYTE* SpiGetTxBuffer()
{
	return SpiTxBuffer;
}


/****************************************************************************
 *
 *  NAME        : SpiSetCommWithA2D
 *
 *  DESCRIPTION : Used for setteing the current session as a session with the 
 *								A2D processor.                                       
 *
 ****************************************************************************/
void SpiSetCommWithA2D(BOOL State)
{
	CommWithA2D = State;
}


