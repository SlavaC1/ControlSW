/*===========================================================================
 *   FILENAME       : Actuators {Actuators.h}  
 *   PURPOSE        : Interface to the digital outputs
 *   DATE CREATED   : 14/06/2004
 *   PROGRAMMER     : Nir Sade 
 *===========================================================================*/


#ifndef _ACTUATORS_H_
#define _ACTUATORS_H_

#include "Define.h"

#ifdef OBJET_MACHINE
#define OUT_SPARE_4     	  0	//p4.0 //left
#define OUT_SPARE_3  	  1	//p4.1
#define ROLLER_SUCTION_PUMP_ACTUATOR_ID   2 //RSS, itamar	  		
#define ROLLER_SUCTION_VALVE_ACTUATOR_ID  3 //RSS, itamar
#else
#define SUPPORT_DRAIN_PUMP_1_ACTUATOR_ID  1
#define SUPPORT_DRAIN_PUMP_2_ACTUATOR_ID  0
#define MODEL_DRAIN_PUMP_1_ACTUATOR_ID    3	  		
#define MODEL_DRAIN_PUMP_2_ACTUATOR_ID    2
#endif

#define DIMMER_ACTUATOR_ID                4
#define PC_ON_ACTUATOR_ID                 6
#define BUZZER_ACTUATOR_ID                7
#define PORT_ENABLE_ACTUATOR_ID           8	 //p5.0

#define SPARE_1                           9
#define SPARE_2                           10 
#define SPARE_3                           11
#define SPARE_4                           12
#define SPARE_5                           13 
#define SPARE_6                           14

#define PUMPS_ACTUATOR_ID                 15 //p5.7
#define ODOUR_FAN_ACTUATOR_ID             16
#define LOCK_ACTUATOR_ID                  17
#define ROLLER_PUMP_ACTUATOR_ID           18
#define WASTE_PUMP_ACTUATOR_ID            19
#define VACUUM_VALVE_ACTUATOR_ID          20
#define AIR_VALVE_ACTUATOR_ID             21
#define MODEL_PINCH_LEFT_ACTUATOR_ID      22
#define MODEL_PINCH_RIGHT_ACTUATOR_ID     23
#define MODEL_PINCH_MIDDLE_ACTUATOR_ID     5
#define Y_Z_T_ON_ACTUATOR_ID              24
#define LEFT_LAMP_ACTUATOR_ID             25
#define RIGHT_LAMP_ACTUATOR_ID            26
#define TRAY_HEATER_ACTUATOR_ID           27
#define AC_CONTROL_ACTUATOR_ID            28
#define VPP_ON_ACTUATOR_ID                29
#define PS_24V_ON_ACTUATOR_ID             30
#define PS_SPARE_ACTUATOR_ID              31 // Not a Spare!
#ifndef LIMIT_TO_DM2
#define SUPPORT_PUMP_1_ACTUATOR_ID     	  32	//p4.0 //left
#define SUPPORT_PUMP_2_ACTUATOR_ID  	  33	//p4.1
#define MODEL_PUMP_1_ACTUATOR_ID          34	 //p5.1	//left
#define MODEL_PUMP_2_ACTUATOR_ID          35 //p5.4	//right
#else
#define MODEL_PUMP_1_ACTUATOR_ID          32	 //p5.1	//left
#define MODEL_PUMP_2_ACTUATOR_ID          33 //p5.4	//right
#define SUPPORT_PUMP_1_ACTUATOR_ID     	  40	//p4.0 //left
#define SUPPORT_PUMP_2_ACTUATOR_ID  	  41	//p4.1
#endif
#define SPARE_7                           36 
#define SPARE_8                           37
#define SPARE_9                           38 
#define SPARE_10                          39
#ifndef LIMIT_TO_DM2
#define MODEL_PUMP_3_ACTUATOR_ID          40 //p5.2	//left
#define MODEL_PUMP_4_ACTUATOR_ID          41 //p5.5
#else
#define MODEL_PUMP_3_ACTUATOR_ID          34 //p5.2	//left
#define MODEL_PUMP_4_ACTUATOR_ID          35 //p5.5
#endif
#define MODEL_PUMP_5_ACTUATOR_ID          42 //p5.3	//left
#define MODEL_PUMP_6_ACTUATOR_ID          43 //p5.6





void ActuatorsInit();

void ActuatorsSetOnOff(BYTE ActuatorNum, BOOL OnOff);
#ifdef DEBUG
void ActuatorsTimedSetOnOff(BYTE ActuatorNum, BOOL OnOff, DWORD Time);
#endif
BOOL ActuatorsGetState(BYTE ActustorNum);

void InitiateSuctionSystemActivation(WORD OnTime, WORD OffTime, BYTE OnOff);	//RSS, itamar

void SuctionSystemTaskFunc(BYTE Arg); //RSS, itamar

void ReadRollerSuctionPumpValue(WORD * ReadingValue);//RSS, itamar

void GetRollerSuctionPumpReadings(WORD * OnReadingValue, WORD * OffReadingValue); //RSS, itamar



void ActuatorsSetAllOff();

void ActuatorsGetAllActuatorsState(BYTE *ActuatorsState);


#endif