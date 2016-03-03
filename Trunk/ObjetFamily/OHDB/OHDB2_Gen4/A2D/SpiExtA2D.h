/*===========================================================================
 *   FILENAME       : SPI External A2D {SpiExtA2D.h}  
 *   PURPOSE        : Gets the A/D values from the external A2D LTC1863 unit by SPI (header file) 
 *   DATE CREATED   : 19/2/2014
 *   PROGRAMMER     : Slava Chuhovich 
 *===========================================================================*/

#ifndef _SPI_EXT_A2D_H_
#define _SPI_EXT_A2D_H_

#include "Define.h"

typedef enum 
{
	EXT_A2D_NO_ERROR = 0,
	EXT_A2D_SEND_SUCCESS,
	EXT_A2D_SEND_FAILED,
	EXT_A2D_WRITE_BLOCK_ERORR,
	EXT_A2D_BUSY
}SPI_EXT_A2D_STATUS;

typedef enum 
{
	EXT_A2D_CMD_INDEX = 0,
	EXT_A2D_DATA_INDEX,	
	EXT_A2D_BUFFER_SIZE
}SPI_EXT_A2D_CMDS;

// This order is according to LTC1863 unit data sheet
typedef enum
{
	EXT_A2D_CH0,
	EXT_A2D_CH2,
	EXT_A2D_CH4,
	EXT_A2D_CH6,
	EXT_A2D_CH1,
	EXT_A2D_CH3,
	EXT_A2D_CH5,
	EXT_A2D_CH7,
	NUM_OF_EXT_A2D_CHANNELS
}TExtA2DChannelAddress;

void SpiExtA2D_Init();
void SpiExtA2D_Task(BYTE Arg);
WORD SpiExtA2D_GetReading(BYTE ChannelIndex);

#endif