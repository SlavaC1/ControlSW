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
#define IN_SPARE_3_SENSOR_ID           2
#define IN_SPARE_4_SENSOR_ID           3 
#define IN_SPARE_5_SENSOR_ID           4
#define IN_SPARE_6_SENSOR_ID           5
#define SPARE_1_INSERTED_SENSOR_ID     6
#define SPARE_2_INSERTED_SENSOR_ID     7
#define TRAY_IN_PLACE_SENSOR_ID        8
#define LEFT_LAMP_SENSOR_ID            9
#define INTERLOCK_SENSOR_ID            10
#define RIGHT_LAMP_SENSOR_ID           11
#define MODEL_ON_TARY_1_SENSOR_ID      12
#define MODEL_ON_TARY_2_SENSOR_ID      13

#ifdef OBJET_MACHINE
#define IN_SPARE_7_SENSOR_ID           14 //p0.6	//left
#define IN_SPARE_8_SENSOR_ID           15 //p0.7

#ifndef LIMIT_TO_DM2
#define SUPPORT_1_INSERTED_SENSOR_ID   16
#define SUPPORT_2_INSERTED_SENSOR_ID   17 
#define MODEL_1_INSERTED_SENSOR_ID     18
#define MODEL_2_INSERTED_SENSOR_ID     19
#define MODEL_3_INSERTED_SENSOR_ID     20
#define MODEL_4_INSERTED_SENSOR_ID     21
#else
#define SUPPORT_1_INSERTED_SENSOR_ID   20
#define SUPPORT_2_INSERTED_SENSOR_ID   21 
#define MODEL_3_INSERTED_SENSOR_ID     18
#define MODEL_4_INSERTED_SENSOR_ID     19
#define MODEL_1_INSERTED_SENSOR_ID     16
#define MODEL_2_INSERTED_SENSOR_ID     17
#endif
#define MODEL_5_INSERTED_SENSOR_ID     22
#define MODEL_6_INSERTED_SENSOR_ID     23 





#define NUM_OF_SENSORS                 24
#else
#define NUM_OF_SENSORS                 14
#endif

void SensorsInit();

BOOL SensorsGetState(BYTE SensorNum);

void SensorsGetAllSensorsState(BYTE *SensorsState);

#endif