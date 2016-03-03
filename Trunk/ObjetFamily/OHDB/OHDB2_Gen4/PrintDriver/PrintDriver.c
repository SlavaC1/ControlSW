/*===========================================================================
 *   FILENAME       : Print Driver {PrintDriver.c}  
 *   PURPOSE        : Configuration and control function for the printing mechanism  
 *   DATE CREATED   : 10/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *===========================================================================*/
#include "PrintDriver.h"
#include "XilinxInterface.h"

#include "MsgDecodeOHDB.h"
#ifdef OCB_SIMULATOR
#include "..\..\..\OHDB\OHDB2_Gen4\Potentiometer\PotentiometerOHDB.h"
#include "EdenProtocol.h"
#else
#include "PotentiometerOHDB.h"
#include "EdenProtocolOHDB.h"
#endif
#include "MiniScheduler.h"

#define ENABLE_PRINTING_COMMAND	  0x01
#define DISABLE_PRINTING_COMMAND  0x00

#define FIRE_ALL_IDLE             0
#define FIRE_ALL_ACTIVE           1

#define DISABLE_BUMPER_PEG        0x03

// Type definitions
// ================
typedef struct 
{
	WORD FireAllFrequency;
	WORD FireAllTime;
	WORD Head;
	DWORD NumOfFires;
	BYTE DestId;
	BYTE TransactionId;
}TFireAllTaskParams;


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
	FireAllCounter                = 0;
	FireAllStatus                 = FIRE_ALL_IDLE;

	while(PrintDrv_ResetDriver()       != PRINT_DRV_NO_ERROR); 
	while(PrintDrv_IsSetOperationDone()!= PRINT_DRV_NO_ERROR);	

	Msg.Address = FPGA_REG_DIAG;
	Msg.Data    = 0;	
	while(XilinxWrite(&Msg)            != XILINX_NO_ERROR);
	while(XilinxGetWriteActionStatus() != XILINX_SEND_SUCCESS);

	Msg.Address = FPGA_REG_SPARE_COMM_DIRECTION;
	Msg.Data    = 0;	
	while(XilinxWrite(&Msg)            != XILINX_NO_ERROR);
	while(XilinxGetWriteActionStatus() != XILINX_SEND_SUCCESS);	
	
	Msg.Address = FPGA_REG_DATA_SOURCE;
	Msg.Data    = 0; // Data PCI as data source	
	while(XilinxWrite(&Msg)            != XILINX_NO_ERROR);
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

    // Build the msg to be sent to the xilinx
	ResetMsg.Address = FPGA_REG_RESET_ALL;
	ResetMsg.Data    = 1;	
 	
	// Send the mesage to the xilinx 
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

	// Build the msg to be sent to the xilinx
	ResetStateMsg.Address = FPGA_REG_RESET_STATE_MACHINE;
	ResetStateMsg.Data    = 1;	
 	
	// Send the mesage to the xilinx 
	if(XilinxWrite(&ResetStateMsg) == XILINX_SEND_FAILED)
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

	// Build the msg to be sent to the xilinx
	StartPegMsg.Address = FPGA_REG_START_PEG;
	StartPegMsg.Data    = StartPeg;	
 	
	// Send the mesage to the xilinx 
	if(XilinxWrite(&StartPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;
}


PRINT_DRV_STATUS PrintDrv_WriteToRegister(BYTE Address, WORD Data)
{	
	TXilinxMessage xdata Msg;

	// Build the msg to be sent to the xilinx
	Msg.Address = Address;
	Msg.Data    = Data;	
 	
	// Send the mesage to the xilinx 
	if(XilinxWrite(&Msg) == XILINX_SEND_FAILED)
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

	// Build the msg to be sent to the xilinx
	EndPegMsg.Address = FPGA_REG_END_PEG;
	EndPegMsg.Data = EndPeg;	
 	
	// Send the mesage to the xilinx 
	if(XilinxWrite(&EndPegMsg) == XILINX_SEND_FAILED)
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
PRINT_DRV_STATUS PrintDrv_SetNumOfFires(DWORD NumOfFires)
{	
	TXilinxMessage xdata NumOfFiresMsg;

	// Build the msg to be sent to the xilinx

	NumOfFiresMsg.Address = FPGA_REG_NUMBER_OF_FIRES;
	NumOfFiresMsg.Data    = NumOfFires;
 	
	// Send the mesage to the xilinx 
	if(XilinxWrite(&NumOfFiresMsg) == XILINX_SEND_FAILED)
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
	if(XilinxRead(FPGA_REG_ACTUAL_POSITION) == XILINX_NO_ERROR)
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

	// Build the msg to be sent to the xilinx
	EnablePrintingMsg.Address = FPGA_REG_GO;
	EnablePrintingMsg.Data    = ENABLE_PRINTING_COMMAND;
 	
	// Send the mesage to the xilinx 
	if(XilinxWrite(&EnablePrintingMsg) == XILINX_SEND_FAILED)
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

	// Build the msg to be sent to the xilinx
	DisablePrintingMsg.Address = FPGA_REG_GO;
	DisablePrintingMsg.Data    = DISABLE_PRINTING_COMMAND;
 	
	// Send the mesage to the xilinx 
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

	// Build the msg to be sent to the xilinx
	SetResulotionMsg.Address = FPGA_REG_CONFIG;
	SetResulotionMsg.Data    = Resulotion;
 	
	// Send the mesage to the xilinx 
	if (XilinxWrite(&SetResulotionMsg) == XILINX_SEND_FAILED)
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

	// Build the msg to be sent to the xilinx
	Msg.Address = FPGA_REG_DIAG;
	Msg.Data    = RegValue;
 	
	// Send the mesage to the xilinx 
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
	Msg.Address = FPGA_REG_SPARE_COMM_DIRECTION;
	Msg.Data    = Direction;
 	
	// send the mesage to the xilinx 
	if (XilinxWrite(&Msg) == XILINX_SEND_FAILED)
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
	EndPegMsg.Address = FPGA_REG_BUMPER_END_PEG;
	EndPegMsg.Data    = EndPeg;	
 	
	// send the mesage to the xilinx 
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
	StartPegMsg.Address = FPGA_REG_BUMPER_START_PEG;
	StartPegMsg.Data    = StartPeg;	
 	
	// send the mesage to the xilinx 
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
void PrintDrv_SetFireAllParams(WORD Frequency, WORD Time, WORD Head, DWORD NumOfFires, BYTE DestId, BYTE TransactionId)
{
	FireAllTaskParams.FireAllFrequency = Frequency;
	FireAllTaskParams.FireAllTime      = Time;
    FireAllTaskParams.Head             = Head;
	FireAllTaskParams.NumOfFires       = NumOfFires;
	FireAllTaskParams.DestId           = DestId;
	FireAllTaskParams.TransactionId    = TransactionId;
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
	enum
	{
		SEND_ACK,
		RESET_STATE_MACHINE,
		WAIT_FOR_STATE_MACHINE_DONE,
		NO_GO_1,
		WAIT_FOR_NO_GO_1_DONE,
		SELECT_DATA_SOURCE,
		WAIT_FOR_DATA_SOURCE_SELECTION_DONE,
		RESET_DIAGNOSTICS_1,
		WAIT_FOR_RESET_DIAG_1_DONE,
		SET_FREQUENCY,
		WAIT_FOR_FREQUENCY_DONE,
		SET_HEAD,
		WAIT_FOR_HEAD_DONE,
		SET_NUM_OF_FIRES_LOW,
		WAIT_FOR_NUM_OF_FIRES_LOW_DONE,
		SET_NUM_OF_FIRES_HIGH,
		WAIT_FOR_NUM_OF_FIRES_HIGH_DONE,
		SET_NUM_OF_FIRES_LOW_DUTY_CYCLE,
		WAIT_NUM_OF_FIRES_LOW_DUTY_CYCLE,
		SET_NUM_OF_FIRES_HIGH_DUTY_CYCLE,
		WAIT_NUM_OF_FIRES_HIGH_DUTY_CYCLE,
		SET_NUM_OF_CYCLES,
        WAIT_NUM_OF_CYCLES,
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
		RESET_DATA_SOURCE,
		WAIT_FOR_RESET_DATA_SOURCE_DONE,
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

			Msg.MsgId          = ACK_MSG;
			Msg.RespondedMsgId = FIRE_ALL_MSG;
			Msg.AckStatus      = ACK_SUCCESS;
			
			if(OHDBEdenProtocolSend((BYTE*)&Msg,sizeof(TOHDBAckMsg),FireAllTaskParams.DestId, FireAllTaskParams.TransactionId,TRUE) == EDEN_PROTOCOL_NO_ERROR)
			{
				FireAllStatus = FIRE_ALL_ACTIVE;
				SchedulerLeaveTask(RESET_STATE_MACHINE);
			}
			else
				SchedulerLeaveTask(SEND_ACK);
		 	break;
		}
	
		case RESET_STATE_MACHINE:
			if (PrintDrv_ResetStateMachine() == PRINT_DRV_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_STATE_MACHINE_DONE);
			else
				SchedulerLeaveTask(RESET_STATE_MACHINE);
			break;

		case WAIT_FOR_STATE_MACHINE_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR)
 		 			SchedulerLeaveTask(NO_GO_1);
				else
					SchedulerLeaveTask(RESET_STATE_MACHINE);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_STATE_MACHINE_DONE);
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
 		 			SchedulerLeaveTask(SELECT_DATA_SOURCE);
				else
					SchedulerLeaveTask(NO_GO_1);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_NO_GO_1_DONE);
			break;
			
		case SELECT_DATA_SOURCE:
			Msg.Address = FPGA_REG_DATA_SOURCE;
			Msg.Data    = 1; // RS232 as data source
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_DATA_SOURCE_SELECTION_DONE);
			else
				SchedulerLeaveTask(SELECT_DATA_SOURCE);
			break;

		case WAIT_FOR_DATA_SOURCE_SELECTION_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(RESET_DIAGNOSTICS_1);
				else
					SchedulerLeaveTask(SELECT_DATA_SOURCE);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_DATA_SOURCE_SELECTION_DONE);
			break;

		case RESET_DIAGNOSTICS_1:
			Msg.Address = FPGA_REG_DIAG;
			Msg.Data = 0;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_RESET_DIAG_1_DONE);
			else
				SchedulerLeaveTask(RESET_DIAGNOSTICS_1);
			break;
	
		case WAIT_FOR_RESET_DIAG_1_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_NUM_OF_FIRES_LOW);
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_DIAG_1_DONE);	
			break;

		case SET_FREQUENCY:
			Msg.Address = FPGA_REG_SIM_FIRE_FREQ;
			Msg.Data = 33333333 / (16L * (FireAllTaskParams.FireAllFrequency));
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
			Msg.Address = FPGA_REG_WR_HEAD_ON;
			Msg.Data    = FireAllTaskParams.Head;
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
			
		case SET_NUM_OF_FIRES_LOW: 		
			Msg.Address = FPGA_REG_FIRES_ON_L;
			Msg.Data    = FireAllTaskParams.NumOfFires;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_NUM_OF_FIRES_LOW_DONE);
			else
				SchedulerLeaveTask(SET_NUM_OF_FIRES_LOW);
			break;
	
		case WAIT_FOR_NUM_OF_FIRES_LOW_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_NUM_OF_FIRES_HIGH);
			else
				SchedulerLeaveTask(WAIT_FOR_NUM_OF_FIRES_LOW_DONE);
			break;
			
		case SET_NUM_OF_FIRES_HIGH: 		
			Msg.Address = FPGA_REG_FIRES_ON_H;
			Msg.Data    = (FireAllTaskParams.NumOfFires & 0xFFFF0000) >> 16;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_NUM_OF_FIRES_HIGH_DONE);
			else
				SchedulerLeaveTask(SET_NUM_OF_FIRES_HIGH);
			break;
	
		case WAIT_FOR_NUM_OF_FIRES_HIGH_DONE:
			if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_NUM_OF_FIRES_LOW_DUTY_CYCLE);
			else
				SchedulerLeaveTask(WAIT_FOR_NUM_OF_FIRES_HIGH_DONE);
			break;

		case SET_NUM_OF_FIRES_LOW_DUTY_CYCLE:
			Msg.Address = FPGA_REG_FIRES_ON_DUTY_CYCLE_L;
			Msg.Data    = 0;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_NUM_OF_FIRES_LOW_DUTY_CYCLE);
			else
				SchedulerLeaveTask(SET_NUM_OF_FIRES_LOW_DUTY_CYCLE);
			break;
		case WAIT_NUM_OF_FIRES_LOW_DUTY_CYCLE:
		   if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_NUM_OF_FIRES_HIGH_DUTY_CYCLE);
			else
				SchedulerLeaveTask(WAIT_NUM_OF_FIRES_LOW_DUTY_CYCLE);
			break;
		case SET_NUM_OF_FIRES_HIGH_DUTY_CYCLE :
			Msg.Address = FPGA_REG_FIRES_ON_DUTY_CYCLE_H;
			Msg.Data    = 0;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_NUM_OF_FIRES_HIGH_DUTY_CYCLE);
			else
				SchedulerLeaveTask(SET_NUM_OF_FIRES_HIGH_DUTY_CYCLE);
			break;
		case WAIT_NUM_OF_FIRES_HIGH_DUTY_CYCLE:
		   if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_NUM_OF_CYCLES);
			else
				SchedulerLeaveTask(WAIT_NUM_OF_FIRES_HIGH_DUTY_CYCLE);
			break;
		case SET_NUM_OF_CYCLES :
			Msg.Address = FPGA_REG_FIRES_NUM_OF_CYCLES;
			Msg.Data    = 1;
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_NUM_OF_CYCLES);
			else
				SchedulerLeaveTask(SET_NUM_OF_CYCLES);
			break;
		case WAIT_NUM_OF_CYCLES:
		   if (XilinxGetWriteActionStatus() == XILINX_SEND_SUCCESS)
				SchedulerLeaveTask(SET_FREQUENCY);
			else
				SchedulerLeaveTask(WAIT_NUM_OF_CYCLES);
			break;

		case SET_DIAGNOSTICS:
			Msg.Address = FPGA_REG_DIAG;
			if(FireAllTaskParams.Head == 0) /*all heads , fireAll in application*/
				Msg.Data = 0x33B;
			else
				Msg.Data = 0x31B;    /*bit 5 need to be 0  for FireALL per head*/
				
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
			Msg.Address = FPGA_REG_DIAG;
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
			if (XilinxRead(FPGA_REG_DIAG) == XILINX_NO_ERROR)
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
					  SchedulerLeaveTask(RESET_DATA_SOURCE);
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
		
		case RESET_DATA_SOURCE:
			Msg.Address = FPGA_REG_DATA_SOURCE;
			Msg.Data    = 0; // DataPCI as data source
			if (XilinxWrite(&Msg) == XILINX_NO_ERROR)
				SchedulerLeaveTask(WAIT_FOR_RESET_DATA_SOURCE_DONE);
			else
				SchedulerLeaveTask(RESET_DATA_SOURCE);
			break;

		case WAIT_FOR_RESET_DATA_SOURCE_DONE:
			Status = PrintDrv_IsSetOperationDone();
			if (Status != PRINT_DRV_OP_NOT_DONE)
			{	
				if (Status == PRINT_DRV_NO_ERROR) 
 		 			SchedulerLeaveTask(SEND_NOTIFICATION);
				else
					SchedulerLeaveTask(RESET_DATA_SOURCE);
			}
			else
				SchedulerLeaveTask(WAIT_FOR_RESET_DATA_SOURCE_DONE);
			break;

		case SEND_NOTIFICATION:
		{
			TFireAllDoneMsg Notification;
			Notification.MsgId   = FIRE_ALL_DONE_MSG;
			Notification.Counter = FireAllCounter;
			
			if (OHDBEdenProtocolSend((BYTE*)&Notification, sizeof(TFireAllDoneMsg), FireAllTaskParams.DestId,	0,FALSE) == EDEN_PROTOCOL_NO_ERROR)
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
	Status = OHDBPotenmtrGetWriteStatus(HeadNum);
	if (Status != OHDB_POTENMTR_BUSY)
	{
		if (Status == OHDB_POTENMTR_SEND_SUCCESS)
			return PRINT_DRV_NO_ERROR;

		return PRINT_DRV_OP_FAIL;
	}
  
	return PRINT_DRV_OP_NOT_DONE;
}

// Enable\disable the bumper PEG mechanism
PRINT_DRV_STATUS PrintDrv_EnableDisableBumperPeg(BOOL Enable)
{
	TXilinxMessage xdata BumperPegMsg;

	// build the msg to be sent to the xilinx
	BumperPegMsg.Address = FPGA_REG_BUMPER_PEG_ENABLE;
	if (Enable)
		BumperPegMsg.Data = 0;
	else
		BumperPegMsg.Data = DISABLE_BUMPER_PEG;
 	
	// send the mesage to the xilinx 
	if (XilinxWrite(&BumperPegMsg) == XILINX_SEND_FAILED)
		return PRINT_DRV_OP_FAIL;
 
	return PRINT_DRV_NO_ERROR;
}

