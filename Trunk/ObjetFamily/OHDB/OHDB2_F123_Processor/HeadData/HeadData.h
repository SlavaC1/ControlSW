/*===========================================================================
 *   FILENAME       : Head Data {HeadData.h}  
 *   PURPOSE        : 
 *   DATE CREATED   : 21/Aug/2002
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/
#ifndef _HEAD_DATA_H_
#define _HEAD_DATA_H_

#include "Define.h"
#include "MiniScheduler.h"

// Initilaization of the 'HeadData' modoule
void HeadData_Init();

// Read the head data from all the heads, check if the data is valid,
// if not do the same for the backup data       
void HeadData_ReadDataFromAllE2PROMs();

// Returns the head data of a specific head
BOOL HeadData_GetHeadData(int HeadNum, BYTE* Data);

// Set the parameters for the 'SetHeadDataTask'
void HeadData_SetHeadDataTaskParams(BYTE DestId, BYTE TransactionId, BYTE HeadNum, BYTE* HeadData);

// This task writes the data received to a specific head (also 
// to the backup data area) and performs verification                                    
void HeadData_SetHeadDataTask(BYTE State);

// Returns the handle of the 'SetHeadDataTask' 
TTaskHandle HeadData_GetHeadDataTaskHandle();

#endif
