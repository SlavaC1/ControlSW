/*===========================================================================
 *   FILENAME       : Sensors {Sensors.h}  
 *   PURPOSE        : Interface to the digital inputs
 *   DATE CREATED   : 21/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "Define.h"

#define IN_SPARE_1_SENSOR_ID           0
#define IN_SPARE_2_SENSOR_ID           1

#define IS_FB_SENSOR_ID                2 // P2.4 - InterLock side door
#define IS_RB_SENSOR_ID                3 // P2.5 - InterLock front door right
#define IL_FB_SENSOR_ID                4 // P2.6 - InterLock front door left
#define SERVICE_KEY_SENSOR_ID          5 // P2.7 - Service key (Interlock bypass)

#define SSR_FB_SENSOR_ID               6 // P3.0 - Safety switch ( emergency stop) rear
#define SSF_FB_SENSOR_ID               7 // P3.1 - Safety switch ( emergency stop) front
#define TRAY_IN_PLACE_SENSOR_ID        8
#define LEFT_LAMP_SENSOR_ID            9
#define INTERLOCK_SENSOR_ID            10
#define RIGHT_LAMP_SENSOR_ID           11
#define MODEL_ON_TARY_1_SENSOR_ID      12
#define MODEL_ON_TARY_2_SENSOR_ID      13 
#define IN_SPARE_7_SENSOR_ID           14 //p0.6	//left
#define IN_SPARE_8_SENSOR_ID           15 //p0.7

#define SUPPORT_1_INSERTED_SENSOR_ID   16
#define SUPPORT_2_INSERTED_SENSOR_ID   17 
#define MODEL_1_INSERTED_SENSOR_ID     18
#define MODEL_2_INSERTED_SENSOR_ID     19
#define MODEL_3_INSERTED_SENSOR_ID     20
#define MODEL_4_INSERTED_SENSOR_ID     21		
#define MODEL_5_INSERTED_SENSOR_ID     22 
#define MODEL_6_INSERTED_SENSOR_ID     23
#define MODEL_7_INSERTED_SENSOR_ID     24
#define MODEL_8_INSERTED_SENSOR_ID     25
#define MODEL_9_INSERTED_SENSOR_ID     26
#define MODEL_10_INSERTED_SENSOR_ID    27
#define MODEL_11_INSERTED_SENSOR_ID    28
#define MODEL_12_INSERTED_SENSOR_ID    29
#define MODEL_13_INSERTED_SENSOR_ID    30
#define MODEL_14_INSERTED_SENSOR_ID    31 

//#define WASTE_1_INSERTED_SENSOR_ID     0 //for waste connected through MS
//#define WASTE_2_INSERTED_SENSOR_ID     0 //for waste connected through MS

#define NUM_OF_SENSORS                 32


void SensorsInit();

BOOL SensorsGetState(BYTE SensorNum);

void SensorsGetAllSensorsState(BYTE *SensorsState);

void SensorSetNormallyOpen(BYTE stauts);	 //set the normally open status

BYTE SensorGetNormallyOpenStatus(); //get the normally open status

#endif