/*===========================================================================
 *   FILENAME       : Print Driver {PrintDriver.c}  
 *   PURPOSE        : Configuration and control function for the printing mechanism  
 *   DATE CREATED   : 10/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/
#include "PrintDriver.h"
#include "XilinxInterface.h"
#include "..\..\..\OHDB\OHDB_F023_Processor\Potentiometer\PotentiometerOHDB.h"
#include "MsgDecodeOHDB.h"
#ifdef OCB_SIMULATOR
#include "EdenProtocol.h"
#else
#include "EdenProtocolOHDB.h"
#endif
#include "MiniScheduler.h"
#include "A2D.h"


// Constants
// =========
#define START_PEG_REG_ADDRESS							0x01
#define END_PEG_REG_ADDRESS		  					0x02
#define ACTUAL_POSITION_REG_ADDRESS				0x01
#define NUM_OF_FIRES_REG_ADDRESS					0x40
#define GO_REG_ADDRESS										0x03
#define DIAGNOSTIC_REG_ADDRESS						0x04
#define CONFIG_REG_ADDRESS								0x05
#define POLARITY_REG_ADDRESS							0x07
#define PULSE_WIDTH_AND_DELAY_REG_ADDRESS	0x08
#define BUMPER_START_PEG_REG_ADDRESS			0x0C
#define BUMPER_END_PEG_REG_ADDRESS				0x0B
#define BUMPER_IMPACT_REG_ADDRESS					0x30
#define WR_HD_ON                            0x31
#define SIMULATOR_FIRE_FREQ_REG_ADDR			0x35
#define SIMULATOR_NUM_OF_FIRES_REG_ADDR		0x36
#define CONTROL_REG_ADDRESS								0x0E							
#define RESET_ALL_REG_ADDR								0x10
#define RST_STT_REG_ADDRESS								0x11
#define SPARE_COMM_DIRECTION_REG_ADDR			0x70
#define PRE_PULSER_DELAY_RED_ADDR					0x50
#define POST_PULSER_DELAY_RED_ADDR				0x51
#define HEAD_1_DELAY_REG_ADDR							0x20
#define HEAD_2_DELAY_REG_ADDR							0x21
#define HEAD_3_DELAY_REG_ADDR							0x22
#define HEAD_4_DELAY_REG_ADDR							0x23
#define HEAD_5_DELAY_REG_ADDR							0x24
#define HEAD_6_DELAY_REG_ADDR							0x25
#define HEAD_7_DELAY_REG_ADDR							0x26
#define HEAD_8_DELAY_REG_ADDR							0x27
#define DIRECTION_OF_PLOT_REG_ADDR				0x75
#define BUMPER_PEG_ENABLE_ADDR            0x76


#define ENABLE_PRINTING_COMMAND						0x01
#define DISABLE_PRINTING_COMMAND					0x00
#define POLARITY_VALUE										0xFF

#define NUM_OF_PRINTING_HEADS							8

#define FIRE_ALL_IDLE                     0
#define FIRE_ALL_ACTIVE                   1

#define DISABLE_BUMPER_PEG                0x03

// Mapping of the sensors inputs to the analog inputs
// --------------------------------------------------
#define HEAD_1_VOLTAGE_SENSOR		CPU_ANALOG_IN_1
#define HEAD_2_VOLTAGE_SENSOR		CPU_ANALOG_IN_2
#define HEAD_3_VOLTAGE_SENSOR		CPU_ANALOG_IN_3
#define HEAD_4_VOLTAGE_SENSOR		CPU_ANALOG_IN_4	
#define HEAD_5_VOLTAGE_SENSOR		CPU_ANALOG_IN_5
#define HEAD_6_VOLTAGE_SENSOR		CPU_ANALOG_IN_6
#define HEAD_7_VOLTAGE_SENSOR		CPU_ANALOG_IN_7
#define HEAD_8_VOLTAGE_SENSOR		CPU_ANALOG_IN_8


// Type definitions
// ================
typedef struct {
	WORD FireAllFrequency;
	WORD FireAllTime;
	WORD Head;
	WORD NumOfFires;
	BYTE DestId;
	BYTE TransactionId;
	}TFireAllTaskParams;

	
 
// Local routines
// ==============


// Module variables
// ================
const BYTE HeadDelaysRegArray[NUM_OF_PRINTING_HEADS] = {
									HEAD_1_DELAY_REG_ADDR,
									HEAD_2_DELAY_REG_ADDR,
									HEAD_3_DELAY_REG_ADDR,
									HEAD_4_DELAY_REG_ADDR,
									HEAD_5_DELAY_REG_ADDR,
									HEAD_6_DELAY_REG_ADDR,
									HEAD_7_DELAY_REG_ADDR,
									HEAD_8_DELAY_REG_ADDR
	};

TFireAllTaskParams xdata FireAllTaskParams;
BYTE FireAllCounter;
BYTE FireAllStatus;


/****************************************************************************
 *
 *  NAME        : PrintDrv_Init 
 *
 *  DESCRIPTION : Init the xilinx registers and state machine.
 *								This function need to be executed once at power up.
 *								Note: this function is not written as a task and therefore
 *								will block the cpu until it is done.                                       
 *
 ****************************************************************************/
void PrintDrv_Init()
{
	TXilinxMessage xdata Msg;

	FireAllTaskParams.FireAllTime = 0;
	FireAllCounter = 0;
	FireAllStatus = FIRE_ALL_IDLE;

	while(PrintDrv_ResetDriver() != PRINT_DRV_NO_ERROR); 
	while(PrintDrv_IsSetOperationDone()!= PRINT_DRV_NO_ERROR);

	while(PrintDrv_SetPolarity() != PRINT_DRV_NO_ERROR);
	while(PrintDrv_IsSetOperationDone()!= PRINT_DRV_NO_ERROR);

	Msg.Address = DIAGNOSTIC_REG_ADDRESS;
	Msg.Data = 0;	
	while(XilinxWrite(&Msg) != XILINX_NO_ERROR);
	while(XilinxGetWriteActionStatus() != XILINX_SEND_SUCCESS);

	Msg.Address = SPARE_COMM_DIRECTION_REG_ADDR;
	Msg.Data = 0;	
	while(XilinxWrite(&Msg) != XILINX_NO_ERROR);
	while(XilinxGetWriteActionStatus() != XILINX_SEND_SUCCESS);

	Msg.Address = CONTROL_REG_ADDRESS;
	Msg.Data = 0;	
	while(XilinxWrite(&Msg) != XILINX_NO_ERROR);
	while(XilinxGetWriteActionStatus() != XILINX_SEND_SUCCESS);

}

/****************************************************************************
 *
 *  NAME        : PrintDrv_ResetDriver 
 *
 *  DESCRIPTION : Reset the print driver                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_ResetDriver()
{
	TXilinxMessage xdata ResetMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	ResetMsg.Address = RESET_ALL_REG_ADDR;
	ResetMsg.Data = 1;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&ResetMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}

/****************************************************************************
 *
 *  NAME        : PrintDrv_ResetStateMachine 
 *
 *  DESCRIPTION : Reset the print driver state machine                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_ResetStateMachine()
{
	TXilinxMessage xdata ResetStateMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	ResetStateMsg.Address = RST_STT_REG_ADDRESS;
	ResetStateMsg.Data = 1;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&ResetStateMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}

/****************************************************************************
 *
 *  NAME        : PrintDrv_SetStartPeg 
 *
 *  DESCRIPTION : Set the 'start peg' register value                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetStartPeg(WORD StartPeg)
{	
	TXilinxMessage xdata StartPegMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	StartPegMsg.Address = START_PEG_REG_ADDRESS;
	StartPegMsg.Data = StartPeg;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&StartPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetEndPeg 
 *
 *  DESCRIPTION : Set the 'end peg' register value                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetEndPeg(WORD EndPeg)
{	
	TXilinxMessage xdata EndPegMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	EndPegMsg.Address = END_PEG_REG_ADDRESS;
	EndPegMsg.Data = EndPeg;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&EndPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetNumOfFires 
 *
 *  DESCRIPTION : Set the 'number of fires' register value                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetNumOfFires(WORD NumOfFires)
{	
	TXilinxMessage xdata NumOfFiresMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	NumOfFiresMsg.Address = NUM_OF_FIRES_REG_ADDRESS;
	NumOfFiresMsg.Data = NumOfFires;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&NumOfFiresMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_GetHeadPosition 
 *
 *  DESCRIPTION : Get the printing head actual position                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_GetHeadPosition()
{
	if (XilinxRead(ACTUAL_POSITION_REG_ADDRESS) == XILINX_NO_ERROR)
		return PRINT_DRV_NO_ERROR;
 
	return PRINT_DRV_OP_FAIL;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_EnablePrintingCircuitry 
 *
 *  DESCRIPTION : Enable the printing driver (Go command)                                        
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_EnablePrintingCircuitry()
{
	TXilinxMessage xdata EnablePrintingMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	EnablePrintingMsg.Address = GO_REG_ADDRESS;
	EnablePrintingMsg.Data = ENABLE_PRINTING_COMMAND;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&EnablePrintingMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_DisblePrintingCircuitry 
 *
 *  DESCRIPTION : Disble the printing driver                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_DisblePrintingCircuitry()
{
	TXilinxMessage xdata DisablePrintingMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	DisablePrintingMsg.Address = GO_REG_ADDRESS;
	DisablePrintingMsg.Data = DISABLE_PRINTING_COMMAND;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&DisablePrintingMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetResulotion 
 *
 *  DESCRIPTION : Set the printing resulotion                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetResulotion(BYTE Resulotion)
{
	TXilinxMessage xdata SetResulotionMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	SetResulotionMsg.Address = CONFIG_REG_ADDRESS;
	SetResulotionMsg.Data = Resulotion;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&SetResulotionMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetPolarity 
 *
 *  DESCRIPTION : Set the polarity                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetPolarity()
{
	TXilinxMessage xdata SetPolarityMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	SetPolarityMsg.Address = POLARITY_REG_ADDRESS;
	SetPolarityMsg.Data = POLARITY_VALUE;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&SetPolarityMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetPulseWidthAndDelay 
 *
 *  DESCRIPTION : Set the pulse width and the pulse delay                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetPulseWidthAndDelay(BYTE Width, BYTE Delay)
{
	TXilinxMessage xdata SetPulseMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	SetPulseMsg.Address = PULSE_WIDTH_AND_DELAY_REG_ADDRESS;
	SetPulseMsg.Data = Delay;
	SetPulseMsg.Data = SetPulseMsg.Data << 8;
	SetPulseMsg.Data |= Width;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&SetPulseMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetPrintDirection 
 *
 *  DESCRIPTION : Set the print direction      
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetPrintDirection(BYTE Direction)
{
	TXilinxMessage xdata PrintDirectionMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	PrintDirectionMsg.Address = DIRECTION_OF_PLOT_REG_ADDR;
	PrintDirectionMsg.Data = Direction;

	// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&PrintDirectionMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetDiagnostics 
 *
 *  DESCRIPTION : Set the diagnostics register               
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetDiagnostics(WORD RegValue)
{
	TXilinxMessage xdata Msg;

// build the msg to be sent to the xilinx
// --------------------------------------
	Msg.Address = DIAGNOSTIC_REG_ADDRESS;
	Msg.Data = RegValue;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&Msg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetCommDirection
 *
 *  DESCRIPTION : Set the communication direction register 
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetCommDirection(BYTE Direction)
{
	TXilinxMessage xdata Msg;

// build the msg to be sent to the xilinx
// --------------------------------------
	Msg.Address = SPARE_COMM_DIRECTION_REG_ADDR;
	Msg.Data = Direction;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&Msg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetControl
 *
 *  DESCRIPTION : Set the control register
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetControl(BYTE RegValue)
{
	TXilinxMessage xdata Msg;

// build the msg to be sent to the xilinx
// --------------------------------------
	Msg.Address = CONTROL_REG_ADDRESS;
	Msg.Data = RegValue;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&Msg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;
}



/****************************************************************************
 *
 *  NAME        : PrintDrv_SetHeadDelay 
 *
 *  DESCRIPTION : Set a head delay                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetHeadDelay(BYTE HeadNum, BYTE Delay)
{
	TXilinxMessage xdata HeadDelayMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	HeadDelayMsg.Address = HeadDelaysRegArray[HeadNum];
	HeadDelayMsg.Data = Delay;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&HeadDelayMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetBumperEndPeg 
 *
 *  DESCRIPTION : Set the bumper end peg                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetBumperEndPeg(WORD EndPeg)
{
	TXilinxMessage xdata EndPegMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	EndPegMsg.Address = BUMPER_END_PEG_REG_ADDRESS;
	EndPegMsg.Data = EndPeg;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&EndPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetBumperStartPeg 
 *
 *  DESCRIPTION : Set the bumper start peg                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetBumperStartPeg(WORD StartPeg)
{
	TXilinxMessage xdata StartPegMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	StartPegMsg.Address = BUMPER_START_PEG_REG_ADDRESS;
	StartPegMsg.Data = StartPeg;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&StartPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_GetBumperImpact 
 *
 *  DESCRIPTION : Get the bumper impact status                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_GetBumperImpact()
{
// TBD
}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetPrePulserDelay 
 *
 *  DESCRIPTION : Set the pre pulser delay register                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetPrePulserDelay(WORD Delay)
{
	TXilinxMessage xdata DelayMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	DelayMsg.Address = PRE_PULSER_DELAY_RED_ADDR;
	DelayMsg.Data = Delay;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&DelayMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}

/****************************************************************************
 *
 *  NAME        : PrintDrv_SetPostPulserDelay 
 *
 *  DESCRIPTION : Set the post pulser delay register                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetPostPulserDelay(WORD Delay)
{
	TXilinxMessage xdata DelayMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	DelayMsg.Address = POST_PULSER_DELAY_RED_ADDR;
	DelayMsg.Data = Delay;	
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&DelayMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}

/****************************************************************************
 *
 *  NAME        : PrintDrv_IsSetOperationDone 
 *
 *  DESCRIPTION : Is the 'Set' operation done (for all the 'set' operations)?                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_IsSetOperationDone()
{
	XILINX_STATUS Status;

// check if the write to the xilinx command is done
// -------------------------------------------------
	Status = XilinxGetWriteActionStatus();
	if (Status != XILINX_BUSY)
	{
		if (Status == XILINX_SEND_SUCCESS)
			return PRINT_DRV_NO_ERROR;

		return PRINT_DRV_OP_FAIL;
	}
  
	return PRINT_DRV_OP_NOT_DONE;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_IsGetOperationDone 
 *
 *  DESCRIPTION : Is the 'Get'operation done?                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_IsGetOperationDone(WORD *Data)
{
	WORD XilinxData;
	if (XilinxGetReceivedData((BYTE*)&XilinxData) != XILINX_BUSY)
	{
		// only the lower 8 bit of the roller speed are meaningful
		*Data = LSB_MASK & XilinxData;
		return PRINT_DRV_NO_ERROR;
	}

	return PRINT_DRV_OP_NOT_DONE;

}


/****************************************************************************
 *
 *  NAME        : PrintDrv_SetFireAllParams 
 *
 *  DESCRIPTION : Set the parameters for the fire all task                                         
 *
 ****************************************************************************/
void PrintDrv_SetFireAllParams(WORD Frequency, WORD Time, WORD Head, WORD NumOfFires, BYTE DestId, BYTE TransactionId)
{
	FireAllTaskParams.FireAllFrequency = Frequency;
	FireAllTaskParams.FireAllTime = Time;
    FireAllTaskParams.Head = Head;
	FireAllTaskParams.NumOfFires = NumOfFires;
	FireAllTaskParams.DestId = DestId;
	FireAllTaskParams.TransactionId = TransactionId;
}



/****************************************************************************
 *
 *  NAME        : PrintDrv_FireAllTask 
 *
 *  DESCRIPTION : This task performs the fire all operation. first it sends ACK
 *								to EDEN, then it set all the XILINIX registers required for the 
 *                fire all operation, then it waits for the fire all time, sets
 *                the xilinx reghisters for terminating the fire all and send 
 *                notification to EDEN
 ****************************************************************************/
void PrintDrv_FireAllTask(BYTE Arg)
{
	enum{
	SEND_ACK,
	RESET_ALL_1,
	WAIT_FOR_RESET_1_OP_DONE,
	NO_GO_1,
	WAIT_FOR_NO_GO_1_DONE,
	RESET_DIAGNOSTICS_1,
	WAIT_FOR_RESET_DIAG_1_DONE,
	SET_FREQUENCY,
	WAIT_FOR_FREQUENCY_DONE,
	SET_HEAD,
	WAIT_FOR_HEAD_DONE,
	SET_NUM_OF_FIRES,
	WAIT_FOR_NUM_OF_FIRES_DONE,
	SET_DIAGNOSTICS,
	WAIT_FOR_SET_DIAG_DONE,
	GO,
	WAIT_FOR_GO_OP_DONE,
	NO_GO_2,
	WAIT_FOR_NO_GO_2_DONE,
	RESET_DIAGNOSTICS_2,
	WAIT_FOR_RESET_DIAG_2_DONE,
	READ_DIAGNOSTICS,
  WAIT_FOR_READ_DIAG_DONE,
	RESET_ALL_2,
	WAIT_FOR_RESET_2_OP_DONE,
	SEND_NOTIFICATION
	};

	PRINT_DRV_STATUS Status;
	XILINX_STATUS XilinxStatus;
	TXilinxMessage xdata Msg;
	WORD xdata XilinxData;


	switch (Arg)
	{
		case SEND_ACK:
		{	
			TOHDBAckMsg xdata Msg;

			Msg.MsgId = ACK_MSG;
			Msg.RespondedMsgId = FIRE_ALL_MSG;
			Msg.AckStatus = ACK_SUCCESS;
			if (OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),FireAllTaskParams.DestId,
 					FireAllTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
			  FireAllStatus = FIRE_ALL_ACTIVE;
				SchedulerLeaveTask(RESET_ALL_1);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}
	
		case RESET_ALL_1:
			if (PrintDrv_ResetDriver() == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_RESET_1_OP_DONE);
			else
				SchedulerLeaveTask(RESET_ALL_1);
			break;

		case WAIT_FOR_RESET_1_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR)
 		 			SchedulerLeaveTask(NO_GO_1);
				else
					SchedulerLeaveTask(RESET_ALL_1);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_1_OP_DONE);
			break;

		case NO_GO_1:
			if (PrintDrv_DisblePrintingCircuitry() == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_NO_GO_1_DONE);
			else
				SchedulerLeaveTask(NO_GO_1);
			break;

		case WAIT_FOR_NO_GO_1_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(RESET_DIAGNOSTICS_1);
				else
					SchedulerLeaveTask(NO_GO_1);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_NO_GO_1_DONE);
			break;

		case RESET_DIAGNOSTICS_1:
			Msg.Address = DIAGNOSTIC_REG_ADDRESS;
			Msg.Data = 0;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_RESET_DIAG_1_DONE);
			else
				SchedulerLeaveTask(RESET_DIAGNOSTICS_1);
			break;
	
		case WAIT_FOR_RESET_DIAG_1_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_NUM_OF_FIRES);
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_DIAG_1_DONE);	
			break;

		case SET_FREQUENCY:
			Msg.Address = SIMULATOR_FIRE_FREQ_REG_ADDR;
			Msg.Data = 33333333/(16L * (FireAllTaskParams.FireAllFrequency));
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_FREQUENCY_DONE);
			else
				SchedulerLeaveTask(SET_FREQUENCY);
			break;
	
		case WAIT_FOR_FREQUENCY_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_HEAD);
			else
				SchedulerLeaveTask(WAIT_FOR_FREQUENCY_DONE);
			break;
	   case SET_HEAD:
			Msg.Address = WR_HD_ON;
			Msg.Data = FireAllTaskParams.Head;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_HEAD_DONE);
			else
				SchedulerLeaveTask(SET_HEAD);
			break;
	
		case WAIT_FOR_HEAD_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_DIAGNOSTICS);
			else
				SchedulerLeaveTask(WAIT_FOR_HEAD_DONE);
			break;
		case SET_NUM_OF_FIRES: 		
			Msg.Address = SIMULATOR_NUM_OF_FIRES_REG_ADDR;
			Msg.Data = FireAllTaskParams.NumOfFires;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_NUM_OF_FIRES_DONE);
			else
				SchedulerLeaveTask(SET_NUM_OF_FIRES);
			break;
	
		case WAIT_FOR_NUM_OF_FIRES_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_FREQUENCY);
			else
				SchedulerLeaveTask(WAIT_FOR_NUM_OF_FIRES_DONE);
			break;

		case SET_DIAGNOSTICS:
			Msg.Address = DIAGNOSTIC_REG_ADDRESS;
			Msg.Data = 0x33B;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_SET_DIAG_DONE);
			else
				SchedulerLeaveTask(SET_DIAGNOSTICS);
			break;
	
		case WAIT_FOR_SET_DIAG_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(GO);
			else
				SchedulerLeaveTask(WAIT_FOR_SET_DIAG_DONE);	
			break;

		case GO:
			if (PrintDrv_EnablePrintingCircuitry() == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_GO_OP_DONE);
			else
				SchedulerLeaveTask(GO);
			break;

		case WAIT_FOR_GO_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 		{
			 		SchedulerLeaveTask(NO_GO_2);
					SchedulerTaskSleep(-1, FireAllTaskParams.FireAllTime);
				}
				else
					SchedulerLeaveTask(GO);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_GO_OP_DONE);
			break;

	 	case NO_GO_2:
			if (PrintDrv_DisblePrintingCircuitry() == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_NO_GO_2_DONE);
			else
				SchedulerLeaveTask(NO_GO_2);
			break;

		case WAIT_FOR_NO_GO_2_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(RESET_DIAGNOSTICS_2);
				else
					SchedulerLeaveTask(NO_GO_2);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_NO_GO_2_DONE);
			break;

		case RESET_DIAGNOSTICS_2:
			Msg.Address = DIAGNOSTIC_REG_ADDRESS;
			Msg.Data = 0;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_RESET_DIAG_2_DONE);
			else
				SchedulerLeaveTask(RESET_DIAGNOSTICS_2);
			break;

		case WAIT_FOR_RESET_DIAG_2_DONE:
		  XilinxStatus = XilinxGetWriteActionStatus();
			if (XilinxStatus != XILINX_BUSY)
			{
			 
				if (XilinxStatus == XILINX_SEND_SUCCESS)
					SchedulerLeaveTask(READ_DIAGNOSTICS);
			  else
				 	SchedulerLeaveTask(RESET_DIAGNOSTICS_2);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_DIAG_2_DONE);	
			break;

	  case READ_DIAGNOSTICS:
			if (XilinxRead(DIAGNOSTIC_REG_ADDRESS) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_READ_DIAG_DONE);
			else
  			SchedulerLeaveTask(READ_DIAGNOSTICS);		
			break;

		case WAIT_FOR_READ_DIAG_DONE:
			XilinxStatus = XilinxGetReceivedData((BYTE*)&XilinxData);
			if (XilinxStatus  != XILINX_BUSY)
			{	
				if (XilinxStatus == XILINX_SEND_SUCCESS)
				{
				  // check if the content of the diagnostics register is 0
				  if (XilinxData == 0)
					  SchedulerLeaveTask(RESET_ALL_2);
					else
					{
					  SchedulerLeaveTask(RESET_DIAGNOSTICS_2);
					}
				}
				else
					SchedulerLeaveTask(READ_DIAGNOSTICS);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_READ_DIAG_DONE);
			break;

		case RESET_ALL_2:
			if (PrintDrv_ResetDriver() == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_RESET_2_OP_DONE);
			else
				SchedulerLeaveTask(RESET_ALL_2);
			break;

		case WAIT_FOR_RESET_2_OP_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR)
 			  {
				  FireAllCounter++;
 		 			SchedulerLeaveTask(SEND_NOTIFICATION);
				}
				else
					SchedulerLeaveTask(RESET_ALL_2);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_2_OP_DONE);
			break;

		case SEND_NOTIFICATION:
		{
			TFireAllDoneMsg Notification;
			Notification.MsgId = FIRE_ALL_DONE_MSG;
      Notification.Counter = FireAllCounter; 
			if (OHDBEdenProtocolSend((BYTE*)&Notification,sizeof(TFireAllDoneMsg),FireAllTaskParams.DestId,
 				0,FALSE) == EDEN_PROTOCOL_NO_ERROR)
			{
			  FireAllStatus = FIRE_ALL_IDLE;
				SchedulerLeaveTask(SEND_ACK);
				SchedulerSuspendTask(-1);
			}
			else
				SchedulerLeaveTask(SEND_NOTIFICATION);
		 	break;

		}

		default:
			SchedulerLeaveTask(SEND_ACK);
			break;

	}
	return;
}


/****************************************************************************
 *
 *  NAME        : PrintDrv_GetFireAllStatus 
 *
 *  DESCRIPTION : Returns the current status of the fire all task
 *
 ****************************************************************************/
BYTE PrintDrv_GetFireAllStatus()
{
  return FireAllStatus;
}


/****************************************************************************
 *
 *  NAME        : PrintDrv_GetFireAllCounter 
 *
 *  DESCRIPTION : Returns the current counter of the fire all task
 *
 ****************************************************************************/
BYTE PrintDrv_GetFireAllCounter()
{
  return FireAllCounter;
}

/****************************************************************************
 *
 *  NAME        : PrintDrv_SetHeadVoltage 
 *
 *  DESCRIPTION : Set the voltage of a printing head by setting its potentiometer value                                         
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_SetHeadVoltage(BYTE HeadNum, BYTE PotentiometerValue)
{
	if (OHDBPotenmtrWriteValue(HeadNum, PotentiometerValue) == OHDB_POTENMTR_NO_ERROR)
		return PRINT_DRV_NO_ERROR;
 	
	return PRINT_DRV_OP_FAIL;
}


/****************************************************************************
 *
 *  NAME        : PrintDrv_IsSetVoltageDone 
 *
 *  DESCRIPTION : Is the 'Set head voltage' action done?                                          
 *
 ****************************************************************************/
PRINT_DRV_STATUS PrintDrv_IsSetVoltageDone(BYTE HeadNum)
{
	OHDB_POTENMTR_STATUS Status;

// check if the write to the potentiometer is done
// -----------------------------------------------
	Status = OHDBPotenmtrGetWriteStatus(HeadNum);
	if (Status != OHDB_POTENMTR_BUSY)
	{
		if (Status == OHDB_POTENMTR_SEND_SUCCESS)
			return PRINT_DRV_NO_ERROR;

		return PRINT_DRV_OP_FAIL;
	}
  
	return PRINT_DRV_OP_NOT_DONE;

}

/****************************************************************************
 *
 *  NAME        : PrintDrv_GetHeadsVoltages
 *
 *  DESCRIPTION : Get the current printing heads voltages              
 *
 ****************************************************************************/
void PrintDrv_GetHeadsVoltages(WORD* Voltages)
{
  A2D_GetBlockReadings(HEAD_1_VOLTAGE_SENSOR, HEAD_8_VOLTAGE_SENSOR, Voltages);
}


// Enable\disable the bumper PEG mechanism
PRINT_DRV_STATUS PrintDrv_EnableDisableBumperPeg(BOOL Enable)
{
	TXilinxMessage xdata BumperPegMsg;

// build the msg to be sent to the xilinx
	BumperPegMsg.Address = BUMPER_PEG_ENABLE_ADDR;
  if (Enable)
	  BumperPegMsg.Data = 0;
	else
    BumperPegMsg.Data = DISABLE_BUMPER_PEG;
 	
// send the mesage to the xilinx 
	if (XilinxWrite(&BumperPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;

}

