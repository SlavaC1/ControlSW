/*===========================================================================
 *   FILENAME       : Fans {Fans.h}  
 *   PURPOSE        : Fans task to read fans data every 2 seconds and send to embedded to monitor
 *   DATE CREATED   : 05/10/2014
 *   PROGRAMMER     : Assaf Goldshtein 
 *===========================================================================*/
 
#ifndef _FANS_H_
#define _FANS_H_

#include "Define.h"
#include "MiniScheduler.h"
#include "MsgDecodeOHDB.h"
#include "ByteOrder.h"
#ifdef OCB_SIMULATOR
#include "EdenProtocol.h"
#else
#include "EdenProtocolOHDB.h"
#endif
#include "XilinxInterface.h"

// If updated, need to update also in RAD GlobalDefs.h
enum FANS{
	LEFT_UV_FAN_1,
	LEFT_UV_FAN_2,
	LEFT_MATERIAL_FAN_1,
	LEFT_MATERIAL_FAN_2,
	LEFT_MATERIAL_FAN_3,
	RIGHT_UV_FAN_1,
	RIGHT_UV_FAN_2,
	RIGHT_MATERIAL_FAN_1,
	RIGHT_MATERIAL_FAN_2,
	MIDDLE_FAN,
	NUM_OF_FANS,
	MATERIAL_FANS_STATE = NUM_OF_FANS,
	MATERIAL_FANS_DUTY_CYCLE,
	FANS_DATA_LENGTH
};


#define LEFT_UV_FAN_1_ADDRESS							0x91
#define	LEFT_UV_FAN_2_ADDRESS 						0x92
#define	LEFT_MATERIAL_FAN_1_ADDRESS 			0x93
#define	LEFT_MATERIAL_FAN_2_ADDRESS 			0x94
#define	LEFT_MATERIAL_FAN_3_ADDRESS 			0x95
#define	RIGHT_UV_FAN_1_ADDRESS						0x96
#define	RIGHT_UV_FAN_2_ADDRESS 						0x97
#define	RIGHT_MATERIAL_FAN_1_ADDRESS 			0x98
#define	RIGHT_MATERIAL_FAN_2_ADDRESS 			0x99
#define	MIDDLE_FAN_ADDRESS 								0x9A
#define	MATERIAL_FANS_STATE_ADDRESS				0x90
#define	MATERIAL_FANS_DUTY_CYCLE_ADDRESS 	0x9B


/*
const BYTE xdata FansRegisterAddresses[FANS_DATA_LENGTH] = {0x91,
	                                                                 0x92,
	                                                                 0x93,
	                                                                 0x94,
	                                                                 0x95,
	                                                                 0x96,
	                                                                 0x97,
	                                                                 0x98,
	                                                                 0x99,
	                                                                 0x9A,
	                                                                 0x90,
	                                                                 0x9B};
*/


#define FANS_READ_DATA_TASK_PERIODIC_TIME		2000


typedef struct
{
	BYTE MsgId;
	BYTE FansData[FANS_DATA_LENGTH];
	/*
	[0] - LEFT_UV_FAN_1,
	[1] - LEFT_UV_FAN_2,
	[2] - LEFT_MATERIAL_FAN_1,
	[3] - LEFT_MATERIAL_FAN_2,
	[4] - LEFT_MATERIAL_FAN_3,
	[5] - RIGHT_UV_FAN_1,
	[6] - RIGHT_UV_FAN_2,
	[7] - RIGHT_MATERIAL_FAN_1,
	[8] - RIGHT_MATERIAL_FAN_2,
	[9] - MIDDLE_FAN,
	[10] - MATERIAL_FANS_STATE,
	[11] - MATERIAL_FANS_DUTY_CYCLE,
	*/
}TFansDataMsg;


void FansReadDataTask(BYTE Arg);

XILINX_STATUS FansReadData();
XILINX_STATUS FansGetData();
EDEN_PROTOCOL_STATUS SendFansData();
BYTE Raw16bitsToSpeedData8bits(WORD Data);




#endif // _FANS_H_