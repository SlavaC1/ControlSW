

#ifndef _MSCINTERFACE_H_
#define _MSCINTERFACE_H_


#include "Define.h"

// Device addresses (7 bits, lsb is R/W)
/*
#define  CHIP_SELECT_MSC_1  0xA4 // First MSC card address
#define  CHIP_SELECT_MSC_2  0xA6 // Second MSC card address
#define  CHIP_SELECT_MSC_3  0xAA // Third MSC card address
#define  CHIP_SELECT_MSC_4  0xAC // Fourth MSC card address
*/
/* MSC2*/
#define  CHIP_SELECT_MSC_1  0xA4 // First MSC card address
#define  CHIP_SELECT_MSC_2  0xAA // Second MSC card address
#define  CHIP_SELECT_MSC_3  0xA0 // Third MSC card address
#define  CHIP_SELECT_MSC_4  0xA8 // Fourth MSC card address


#define SWITCHES			0
#define	PUMPS_STATUS		1
#define	LOAD_CELL_1_STATUS	2
#define	LOAD_CELL_2_STATUS	3
#define	LOAD_CELL_3_STATUS	4
#define	LOAD_CELL_4_STATUS	5
#define	_24V_STATUS			6
#define	_7V_STATUS			7
#define	_5V_STATUS			8
#define	_3_3V_STATUS		9 

enum
{
	MSC_NO_ERROR,
	MSC_1_NO_SLAVE,
	MSC_2_NO_SLAVE,
	MSC_3_NO_SLAVE,
	MSC_4_NO_SLAVE,
	MSC_1_HW_ERROR,
	MSC_2_HW_ERROR,
	MSC_3_HW_ERROR,
	MSC_4_HW_ERROR,
	MSC_1_COM_ERROR,
	MSC_2_COM_ERROR,
	MSC_3_COM_ERROR,
	MSC_4_COM_ERROR	
};
  
enum  
{
	MSCCARD_1, 
	MSCCARD_2,
	MSCCARD_3,
	MSCCARD_4,
	NUM_OF_MSC_CARDS	
};  

BYTE MSCWriteByte          (BYTE SelectedCard, BYTE dat);
WORD MSCA2D_GetReading     (BYTE AnalogInNum,  BYTE SelectedCard);
void MSCA2D_GetBlockReading(WORD *state,       BYTE SelectedCard);
void MSC_Status_Task       (BYTE Arg);
BYTE CheckForError         (BYTE CardToCheck);
void MSCPrepareStatusMsg   ();
void MSCStartStop          (BOOL OnOff);
void OnMSCFailCallback     ();
void MSC_Init              ();
void MSCReadWriteTask      (BYTE Arg);
void Incoming_Bytes_To_Words_callback();
#endif