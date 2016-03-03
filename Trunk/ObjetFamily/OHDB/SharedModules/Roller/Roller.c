/*===========================================================================
 *   FILENAME       : Roller {Roller.c}  
 *   PURPOSE        : Interface to the roller device  
 *   DATE CREATED   : 8/Jan/2002
 *   PROGRAMMER     : Nir Saadon 
 *   PROCEDURES     : 
 *===========================================================================*/
#include "Roller.h"
#include "XilinxInterface.h"


// Constants
// =========
#define ROLLER_SET_ON_OFF_ADDRESS		0x09		
#define ROLLER_SET_SPEED_ADDRESS		0x0A
#define ROLLER_GET_SPEED_ADDRESS		0x41

#define ENABLE_ROLLER_DRV						0x02
#define DISABLE_AND_SET_OFF_ROLLER	0x00
#define ENABLE_AND_SET_ON_ROLLER		0x03

// Type definitions
// ================

	
 
// Local routines
// ==============


// Module variables
// ================
bit RollerIsOn;
bit RollerIsEnabled;

/****************************************************************************
 *
 *  NAME        : Roller_Init 
 *
 *
 *  DESCRIPTION : Initailization of the roller module                                        
 *
 ****************************************************************************/
void Roller_Init()
{	
	RollerIsOn = FALSE;
	RollerIsEnabled = FALSE;

}


/****************************************************************************
 *
 *  NAME        : Roller_EnableDriver 
 *
 *
 *  DESCRIPTION : Enable/disable the roller hardware driver.                                        
 *
 ****************************************************************************/
ROLLER_STATUS Roller_EnableDriver(BOOL Enable)
{
	TXilinxMessage xdata EnableMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	EnableMsg.Address = ROLLER_SET_ON_OFF_ADDRESS;
	if (Enable)
	{
		EnableMsg.Data = ENABLE_ROLLER_DRV;
		RollerIsEnabled = TRUE;
	}
	else
	{
		EnableMsg.Data = DISABLE_AND_SET_OFF_ROLLER;
		RollerIsEnabled = FALSE;
		RollerIsOn = FALSE;
	}
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&EnableMsg) == XILINX_SEND_FAILED)
		return ROLLER_OP_FAIL;
 
	return ROLLER_NO_ERROR;
	
}


/****************************************************************************
 *
 *  NAME        : Roller_SetOnOff 
 *
 *
 *  DESCRIPTION : Set the roller on or off.                                        
 *
 ****************************************************************************/
ROLLER_STATUS Roller_SetOnOff(BOOL On)
{
	TXilinxMessage xdata SetOnOffMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	SetOnOffMsg.Address = ROLLER_SET_ON_OFF_ADDRESS;
	if (On)
	{
		SetOnOffMsg.Data = ENABLE_AND_SET_ON_ROLLER;
		if (RollerIsEnabled)
			RollerIsOn = TRUE;
	}
	else
	{
		SetOnOffMsg.Data = DISABLE_AND_SET_OFF_ROLLER;
		RollerIsOn = FALSE;
		RollerIsEnabled = FALSE;
	}
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&SetOnOffMsg) == XILINX_SEND_FAILED)
		return ROLLER_OP_FAIL;
 
	return ROLLER_NO_ERROR;
	
}


/****************************************************************************
 *
 *  NAME        : Roller_SetSpeed 
 *
 *
 *  DESCRIPTION : Set the speed of the roller.                                        
 *
 ****************************************************************************/
ROLLER_STATUS Roller_SetSpeed(WORD Speed)
{
	TXilinxMessage xdata SetSpeedMsg;

// build the msg to be sent to the xilinx
// --------------------------------------
	SetSpeedMsg.Address = ROLLER_SET_SPEED_ADDRESS;
	SetSpeedMsg.Data = Speed;
 	
// send the mesage to the xilinx 
// ------------------------------
	if (XilinxWrite(&SetSpeedMsg) == XILINX_SEND_FAILED)
		return ROLLER_OP_FAIL;
 
	return ROLLER_NO_ERROR;

}


/****************************************************************************
 *
 *  NAME        : Roller_SetOnOff 
 *
 *
 *  DESCRIPTION : Is the roller on or off?                                        
 *
 ****************************************************************************/
BOOL Roller_IsOn()
{
	return RollerIsOn;
}


/****************************************************************************
 *
 *  NAME        : Roller_SetOnOff 
 *
 *
 *  DESCRIPTION : Get the roller speed                                        
 *
 ****************************************************************************/
ROLLER_STATUS Roller_GetSpeed()
{
	if (XilinxRead(ROLLER_GET_SPEED_ADDRESS) == XILINX_NO_ERROR)
		return ROLLER_NO_ERROR;
 
	return ROLLER_OP_FAIL;

}


/****************************************************************************
 *
 *  NAME        : Roller_IsSetOperationDone 
 *
 *
 *  DESCRIPTION : Is the 'SetOnOff' or the 'SetSpeed' operation done?                                        
 *
 ****************************************************************************/
ROLLER_STATUS Roller_IsSetOperationDone()
{
// check if the write to the xilinx command is done
// -------------------------------------------------

	if (XilinxGetWriteActionStatus() != XILINX_BUSY)
		return ROLLER_NO_ERROR;
  
	return ROLLER_OP_NOT_DONE;

}


/****************************************************************************
 *
 *  NAME        : Roller_IsGetOperationDone 
 *
 *
 *  DESCRIPTION : Is the 'IsOn' or the 'GetSpeed' operation done?                                        
 *
 ****************************************************************************/
ROLLER_STATUS Roller_IsGetOperationDone(WORD *Data)
{
	WORD XilinxData;
	if (XilinxGetReceivedData((BYTE*)&XilinxData) != XILINX_BUSY)
	{
		// only the lower 8 bit of the roller speed are meaningful
		*Data = LSB_MASK & XilinxData;
		return ROLLER_NO_ERROR;
	}

	return ROLLER_OP_NOT_DONE;

}


