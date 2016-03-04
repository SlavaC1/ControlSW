/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT front-end interface controls IDs.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 04/02/2002                                           *
 * Last upate: 25/09/2003                                           *
 ********************************************************************/

#ifndef _FRONT_END_CONTROL_IDS_H_
#define _FRONT_END_CONTROL_IDS_H_

#include "GlobalDefs.h"


  #define CASE_MESSAGE_ALL_TANKS(_base_) \
     case _base_##_MODEL_TANK1:\
     case _base_##_MODEL_TANK2:\
     case _base_##_MODEL_TANK3:\
     case _base_##_MODEL_TANK4:\
     case _base_##_MODEL_TANK5:\
     case _base_##_MODEL_TANK6:\
     case _base_##_SUPPORT_TANK1:\
	 case _base_##_SUPPORT_TANK2:


#define DEFINE_FE_LAMP_SET(_start_, _base_) \
   const int _base_##_BASE           = _start_; \
   const int _base_##_RIGHT_UV_LAMP  = _base_##_BASE + RIGHT_UV_LAMP;\
   const int _base_##_LEFT_UV_LAMP   = _base_##_BASE + LEFT_UV_LAMP;

#define CASE_MESSAGE_ALL_LAMPS(_base_) \
   case _base_##_RIGHT_UV_LAMP:\
   case _base_##_LEFT_UV_LAMP:



#define DEFINE_FE_TANK_SET(_start_, _base_) \
   const int _base_##_BASE             = _start_; \
   const int _base_##_SUPPORT_TANK1   = _base_##_BASE + TYPE_TANK_SUPPORT1;\
   const int _base_##_SUPPORT_TANK2   = _base_##_BASE + TYPE_TANK_SUPPORT2;\
   const int _base_##_MODEL_TANK1     = _base_##_BASE + TYPE_TANK_MODEL1;\
   const int _base_##_MODEL_TANK2     = _base_##_BASE + TYPE_TANK_MODEL2;\
   const int _base_##_MODEL_TANK3     = _base_##_BASE + TYPE_TANK_MODEL3;\
   const int _base_##_MODEL_TANK4     = _base_##_BASE + TYPE_TANK_MODEL4;\
   const int _base_##_MODEL_TANK5     = _base_##_BASE + TYPE_TANK_MODEL5;\
   const int _base_##_MODEL_TANK6     = _base_##_BASE + TYPE_TANK_MODEL6;



#define DEFINE_FE_TANK_WSET(start, _base_) \
   const int _base_##_BASE            = start; \
   const int _base_##_SUPPORT_TANK1   = _base_##_BASE + TYPE_TANK_SUPPORT1;\
   const int _base_##_SUPPORT_TANK2   = _base_##_BASE + TYPE_TANK_SUPPORT2;\
   const int _base_##_MODEL_TANK1     = _base_##_BASE + TYPE_TANK_MODEL1;\
   const int _base_##_MODEL_TANK2     = _base_##_BASE + TYPE_TANK_MODEL2;\
   const int _base_##_MODEL_TANK3     = _base_##_BASE + TYPE_TANK_MODEL3;\
   const int _base_##_MODEL_TANK4     = _base_##_BASE + TYPE_TANK_MODEL4;\
   const int _base_##_MODEL_TANK5     = _base_##_BASE + TYPE_TANK_MODEL5;\
   const int _base_##_MODEL_TANK6     = _base_##_BASE + TYPE_TANK_MODEL6;\
   const int _base_##_WASTE_TANK      = _base_##_BASE + TYPE_TANK_WASTE;



#define DEFINE_FE_CHAMBER_SET(start, _base_) \
   const int _base_##_BASE      = start; \
   const int _base_##_SUPPORT   = _base_##_BASE + TYPE_CHAMBER_SUPPORT;\
   const int _base_##_MODEL1    = _base_##_BASE + TYPE_CHAMBER_MODEL1;\
   const int _base_##_MODEL2    = _base_##_BASE + TYPE_CHAMBER_MODEL2;\
   const int _base_##_MODEL3    = _base_##_BASE + TYPE_CHAMBER_MODEL3;


#ifdef OBJET_MACHINE
#define DEFINE_FE_THERMISTORS_SET(start, _base_) \
   const int _base_##_BASE      = start; \
   const int _base_##_SUPPORT   = _base_##_BASE + SUPPORT_CHAMBER_THERMISTOR;\
   const int _base_##_M1        = _base_##_BASE + M1_CHAMBER_THERMISTOR;\
   const int _base_##_M2        = _base_##_BASE + M2_CHAMBER_THERMISTOR;\
   const int _base_##_M3        = _base_##_BASE + M3_CHAMBER_THERMISTOR;\
   const int _base_##_SUPPORT_M3 = _base_##_BASE + SUPPORT_M3_CHAMBER_THERMISTOR;\
   const int _base_##_M1_M2     = _base_##_BASE + M1_M2_CHAMBER_THERMISTOR;
#endif


#define DEFINE_FE_CHAMBER_WSET(start, _base_) \
   const int _base_##_BASE      = start; \
   const int _base_##_SUPPORT   = _base_##_BASE + TYPE_CHAMBER_SUPPORT;\
   const int _base_##_MODEL1    = _base_##_BASE + TYPE_CHAMBER_MODEL1;\
   const int _base_##_MODEL2    = _base_##_BASE + TYPE_CHAMBER_MODEL2;\
   const int _base_##_MODEL3    = _base_##_BASE + TYPE_CHAMBER_MODEL3;\
   const int _base_##_WASTE     = _base_##_BASE + TYPE_CHAMBER_WASTE;


// General controls IDs
const int FE_TOTAL_NUM_OF_SLICES                        = 0;
const int FE_TOTAL_MODEL_HEIGHT                         = 1;
const int FE_CURRENT_SLICE                              = 2;
const int FE_CURRENT_LAYER                              = 3;
const int FE_CURRENT_MACHINE_STATE                      = 4;
const int FE_CURRENT_MODEL_HEIGHT                       = 5;

const int FE_HEAD_HEATER_CELSIUS_BASE       = 6;
const int FE_HEAD_HEATER_CELSIUS_HEAD_0     = FE_HEAD_HEATER_CELSIUS_BASE; //=6
const int FE_HEAD_HEATER_CELSIUS_HEAD_1     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_1;
const int FE_HEAD_HEATER_CELSIUS_HEAD_2     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_2;
const int FE_HEAD_HEATER_CELSIUS_HEAD_3     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_3;
#ifndef EDEN_250
const int FE_HEAD_HEATER_CELSIUS_HEAD_4     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_4;
const int FE_HEAD_HEATER_CELSIUS_HEAD_5     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_5;
const int FE_HEAD_HEATER_CELSIUS_HEAD_6     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_6;
const int FE_HEAD_HEATER_CELSIUS_HEAD_7     = FE_HEAD_HEATER_CELSIUS_BASE + HEAD_7;
#endif
const int FE_HEAD_HEATER_CELSIUS_BLOCK_0    = FE_HEAD_HEATER_CELSIUS_BASE + BLOCK_0;
const int FE_HEAD_HEATER_CELSIUS_BLOCK_1    = FE_HEAD_HEATER_CELSIUS_BASE + BLOCK_1;
const int FE_HEAD_HEATER_CELSIUS_BLOCK_2    = FE_HEAD_HEATER_CELSIUS_BASE + BLOCK_2;
const int FE_HEAD_HEATER_CELSIUS_BLOCK_3    = FE_HEAD_HEATER_CELSIUS_BASE + BLOCK_3;
#ifndef OBJET_MACHINE
const int FE_HEAD_HEATER_CELSIUS_PRE_HEATER = FE_HEAD_HEATER_CELSIUS_BASE + PRE_HEATER; //=18
#endif

const int FE_HEAD_HEATER_COLD_VALUE                     = 19;
const int FE_HEAD_HEATER_HOT_VALUE                      = 20;
const int FE_CURRENT_TRAY_TEMPERATURE                   = 21;
const int FE_CURRENT_TRAY_STATUS                        = 22;
const int FE_CURRENT_AMBIENT_TEMPERATURE                = 27;
const int FE_CURRENT_AMBIENT_STATUS                     = 28;
const int FE_CURRENT_POWER_STATUS                       = 29;
const int FE_CURRENT_DOOR_STATUS                        = 30;

DEFINE_FE_LAMP_SET (31, FE_CURRENT_UV_LAMP_STATUS)

const int FE_CURRENT_ROLLER_STATUS                      = 33;
const int FE_CURRENT_ROLLER_SPEED                       = 34;
const int FE_CURRENT_ACTUATOR_STATUS_START              = 35; // (35 - 67)

#ifndef PARAMS_EXP_IMP /*{*/
#define DEFINE_ACTUATOR(id, descStr, formPartStr) const int FE_ACTUATOR_##id = FE_CURRENT_ACTUATOR_STATUS_START + ACTUATOR_ID_##id;
#define DEFINE_SENSOR(id, formPartStr)
#include "ActuatorDefs.h"
#undef DEFINE_ACTUATOR
#undef DEFINE_SENSOR
#endif /*}*/


const int FE_CURRENT_OCB_SENSOR_STATUS_START    = 89; // (89 - 104)
const int FE_PREVIOUS_JOB_LAST_SLICE            = 113;
const int FE_MODEL_ON_TRAY_STATUS               = 114;
const int FE_CURRENT_VACUUM_STATUS              = 115;
const int FE_CURRENT_VACUUM_AVERAGE_VALUE       = 116;
const int FE_CURRENT_VACUUM_AVERAGE_CMH2O_VALUE = 117;
const int FE_CURRENT_UV_LAMP_SENSOR_VALUE       = 118;
const int FE_CURRENT_UV_LAMP_OCB_COMMAND        = 119;
#ifdef ALLOW_OPENING_MRW_FROM_HOST
const int FE_HOST_RENDING_RESIN_REPLACEMENT     = 120;
#endif

const int FE_HEAD_HEATER_A2D_BASE       = 132;
const int FE_HEAD_HEATER_A2D_HEAD_0     = FE_HEAD_HEATER_A2D_BASE; //=117
const int FE_HEAD_HEATER_A2D_HEAD_1     = FE_HEAD_HEATER_A2D_BASE + HEAD_1;
const int FE_HEAD_HEATER_A2D_HEAD_2     = FE_HEAD_HEATER_A2D_BASE + HEAD_2;
const int FE_HEAD_HEATER_A2D_HEAD_3     = FE_HEAD_HEATER_A2D_BASE + HEAD_3;
#ifndef EDEN_250
const int FE_HEAD_HEATER_A2D_HEAD_4     = FE_HEAD_HEATER_A2D_BASE + HEAD_4;
const int FE_HEAD_HEATER_A2D_HEAD_5     = FE_HEAD_HEATER_A2D_BASE + HEAD_5;
const int FE_HEAD_HEATER_A2D_HEAD_6     = FE_HEAD_HEATER_A2D_BASE + HEAD_6;
const int FE_HEAD_HEATER_A2D_HEAD_7     = FE_HEAD_HEATER_A2D_BASE + HEAD_7;
#endif
const int FE_HEAD_HEATER_A2D_BLOCK_0    = FE_HEAD_HEATER_A2D_BASE + BLOCK_0;
const int FE_HEAD_HEATER_A2D_BLOCK_1    = FE_HEAD_HEATER_A2D_BASE + BLOCK_1;
const int FE_HEAD_HEATER_A2D_BLOCK_2    = FE_HEAD_HEATER_A2D_BASE + BLOCK_2;
const int FE_HEAD_HEATER_A2D_BLOCK_3    = FE_HEAD_HEATER_A2D_BASE + BLOCK_3;
#ifndef OBJET_MACHINE
const int FE_HEAD_HEATER_A2D_PRE_HEATER = FE_HEAD_HEATER_A2D_BASE + PRE_HEATER; //=129
#endif

/* Free: 130 - 132 */
const int FE_MOTOR_X_AXIS_HOME_DONE             = 145;
const int FE_MOTOR_Y_AXIS_HOME_DONE             = 146;
const int FE_MOTOR_Z_AXIS_HOME_DONE             = 147;
const int FE_MOTOR_T_AXIS_HOME_DONE             = 148;
const int FE_MOTOR_X_AXIS_ENABLE                = 149;
const int FE_MOTOR_Y_AXIS_ENABLE                = 150;
const int FE_MOTOR_Z_AXIS_ENABLE                = 151;
const int FE_MOTOR_T_AXIS_ENABLE                = 152;
const int FE_HEAD_FILLING_MONITOR_STATUS        = 153;
const int FE_SHOW_BIT_RESULT                    = 154;
const int FE_MACHINE_SUB_STATUS                 = 155;
const int FE_UPDATE_UI_MODE                     = 156;
const int FE_REFRESH_MODES_DISPLAY              = 157;
const int FE_SERVICE_ALERT                      = 158;
const int FE_CLEAR_SERVICE_ALERT                = 159;
const int FE_CURRENT_TRAY_IN_A2D                = 160;
const int FE_CURRENT_AMBIENT_IN_A2D             = 161;
const int FE_CURRENT_MODEL_NAME                 = 162;
const int FE_NEW_JOB                            = 163;


// Use for UI modes
const int FE_UI_IDLE_MODE    = 0;
const int FE_UI_BUSY_MODE    = 1;

const int FE_SILENT_MRW_DONT_UPDATE_SEGMENTS 	= 0;
const int FE_SILENT_MRW_UPDATE_SEGMENTS 		= 1;

const int FE_UI_NO_MODE      = -1;

// Enable/Disable IDs
const int FE_ONLINE_MODE                 = 0;
const int FE_ONLINE_ENABLE               = 1;
const int FE_RESUME_VISIBLE              = 2;
const int FE_PAUSE                       = 3;
const int FE_STOP                        = 4;
// const int FE_RESUME_BY_CARTRIDGE_ERR_DLG ; // moved to TFENotRefreshable enum. (fix of bug# 6496)
const int FE_CTRL_DOOR_DLG               = 6;

// Cartridge error dialog IDs
const int FE_REPLACE_CARTRIDGE = 0;
const int FE_INSERT_CARTRIDGE  = 1;

const int FE_CARTRIDGE_DLG_RESULT_NONE   = 0;
const int FE_CARTRIDGE_DLG_RESULT_CANCEL = 1;
const int FE_CARTRIDGE_DLG_RESULT_OK     = 2;

// Tank Identification Dialog Types:
const int FE_TANK_ID_NOTICE_DLG_RECENT_DLG_TYPE             = 100;
const int FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH               = 0;
const int FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH            = 1;
const int FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_MSPRINTING    = 2;
const int FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_MSPRINTING = 3;
const int FE_TANK_ID_NOTICE_DLG_UNSUPPORTED_RESIN           = 4;
const int FE_TANK_ID_NOTICE_DLG_NO_TAG                      = 5;
const int FE_TANK_ID_NOTICE_DLG_MODEL_IN_SUPPORT_MISPLACE   = 6;
const int FE_TANK_ID_NOTICE_DLG_SUPPORT_IN_MODEL_MISPLACE   = 7;
const int FE_TANK_ID_NOTICE_DLG_RESIN_EXPIRED               = 8;
const int FE_TANK_ID_NOTICE_DLG_HIDE                        = 9;  // Hides the dialog.
const int FE_TANK_ID_NOTICE_DLG_SERVICE_IN_REGULAR          = 15;
const int FE_TANK_ID_NOTICE_DLG_REGULAR_IN_SERVICE          = 16;
#ifdef OBJET_MACHINE
const int FE_TANK_ID_NOTICE_DLG_CHECK_CONTAINER_REFILL      = 17;
const int FE_TANK_ID_NOTICE_DLG_CALIBRATION_PROBLEM         = 18;
const int FE_TANK_ID_NOTICE_DLG_INVALID_TAG_VERSION         = 20; //not in use
const int FE_TANK_ID_MATERIAL_CONSUMPTION_PROBLEM		    = 21;
const int FE_TANK_ID_NOTICE_DLG_READING_TOO_LOW  		    = 22; //not in use
const int FE_TANK_ID_NOTICE_DLG_MACHINE_NO_MATCH_SUPPORT  	= 24;
const int FE_TANK_ID_NOTICE_DLG_PIPE_NO_MATCH_SUPPORT       = 25;
#endif

const int FE_TANK_ID_NOTICE_DLG_NOT_LICENSED_CARTRIDGE      = 19;
const int FE_TANK_NOT_ASSOCIATED_WITH_CHAMBER				= 23;
const int FE_TANK_ID_NOTICE_DLG_AUTHENTICATION_FAILED       = 26;
const int FE_TANK_ID_NOTICE_DLG_OVERCONSUMPTION             = 27; 

// Tank Identification Dialog Result IDs:
const int FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_SINGLEMODE = 10;
const int FE_TANK_ID_NOTICE_DLG_RESULT_DISABLE_TANK           = 11;
const int FE_TANK_ID_NOTICE_DLG_RESULT_RUN_RR_WIZARD          = 12;
const int FE_TANK_ID_NOTICE_DLG_RESULT_OK                     = 13;
const int FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_DMMODE     = 14;
const int FE_TANK_ID_NOTICE_DLG_RESULT_ENABLE_TANK_NO_TAG     = 15;

const int FE_TANK_ENABLED_STATUS  = 1;
const int FE_TANK_DISABLED_STATUS = 0;

//itamar objet
#ifdef OBJET_MACHINE
DEFINE_FE_THERMISTORS_SET (170, FE_CURRENT_HEAD_FILLING_IS_FILLED)
DEFINE_FE_TANK_WSET   (180, FE_CURRENT_TANK_STATUS)
DEFINE_FE_TANK_WSET   (190, FE_CURRENT_TANK_WEIGHT)
DEFINE_FE_TANK_SET    (200, FE_CURRENT_TANK_RELATIVE_WEIGHT)
DEFINE_FE_TANK_WSET   (210, FE_TANK_EXISTENCE_STATUS)
DEFINE_FE_TANK_SET    (220, FE_SET_ACTIVE_TANK)
DEFINE_FE_TANK_SET    (230, FE_UPDATE_RESIN_TYPE)
DEFINE_FE_CHAMBER_SET (240, FE_SET_DRAIN_PUMPS)
DEFINE_FE_CHAMBER_SET (250, FE_ACTIVE_PUMP)
DEFINE_FE_THERMISTORS_SET (260, FE_CURRENT_HEAD_FILLING)
DEFINE_FE_TANK_SET    (270, FE_TANK_ENABLING_STATUS)
DEFINE_FE_TANK_SET    (280, FE_TANK_ID_NOTICE_DLG)
DEFINE_FE_TANK_SET    (290, FE_TANK_DISABLING_HINT)
#else//////////
DEFINE_FE_CHAMBER_SET (165, FE_CURRENT_HEAD_FILLING_IS_FILLED)
DEFINE_FE_TANK_WSET   (170, FE_CURRENT_TANK_STATUS)
DEFINE_FE_TANK_WSET   (175, FE_CURRENT_TANK_WEIGHT)
DEFINE_FE_TANK_SET    (180, FE_CURRENT_TANK_RELATIVE_WEIGHT)
DEFINE_FE_TANK_WSET   (185, FE_TANK_EXISTENCE_STATUS)
DEFINE_FE_TANK_SET    (190, FE_SET_ACTIVE_TANK)
DEFINE_FE_TANK_SET    (195, FE_UPDATE_RESIN_TYPE)
DEFINE_FE_CHAMBER_SET (200, FE_SET_DRAIN_PUMPS)
DEFINE_FE_CHAMBER_SET (205, FE_ACTIVE_PUMP)
DEFINE_FE_CHAMBER_SET (210, FE_CURRENT_HEAD_FILLING)
DEFINE_FE_TANK_SET    (215, FE_TANK_ENABLING_STATUS)
DEFINE_FE_TANK_SET    (220, FE_TANK_ID_NOTICE_DLG)
DEFINE_FE_TANK_SET    (225, FE_TANK_DISABLING_HINT)
#endif////////////////////////////////////////////
//Non Refreshable
//////////////////////////////////////////////////////

typedef enum
{
   FEOP_NOT_REFRESHABLE_START = 10000,
   FEOP_RUN_MRW_WIZARD,
   FEOP_RUN_SHD_WIZARD,
   FEOP_RUN_MRW_WIZARD_SILENTLY,
   FEOP_RUN_BLOCKING_MRW_WIZARD_SILENTLY,
   FEOP_RESUME_BY_CARTRIDGE_ERR_DLG,
   FEOP_CANCEL_WIZARD,
   FEOP_NOT_REFRESHABLE_END
}TFENotRefreshable;

const int FE_HASP_UPDATE_STATUS_LABEL  = 300;
const int FE_HASP_UI_CONTROLS_STATUS   = 301;
const int FE_HASP_STATUS_LABEL_ENABLED = 302;

const int FE_RSS_PUMP_ON_VALUE = 303; //RSS, itamar
const int FE_RSS_PUMP_OFF_VALUE = 304;//RSS, itamar

const int FE_HASP_INCOMING_SLICES_PROTECTION = 305; 

const int FE_HEAD_CLEANING_WIZARD     = 306; //itamar, Super purge

const int FE_CURRENT_VACUUM_PUMP_VOLT_VALUE = 307; //Elad, Vacuum Pump Voltage.
const int FE_CURRENT_A2D_VACUUM_PUMP_VOLT_VALUE = 308;//Elad, Vacuum Pump Voltage A2D show.
//Elad -Four Power Supply
const int FE_CURRENT_VS_POWER_SUPPLIERS_STATUS       = 309;
const int FE_CURRENT_A2D_VS_POWER_SUPPLIERS_STATUS   = 310;
const int FE_CURRENT_VCC_POWER_SUPPLIERS_STATUS      = 311;
const int FE_CURRENT_A2D_VCC_POWER_SUPPLIERS_STATUS  = 312;
const int FE_CURRENT_12V_POWER_SUPPLIERS_STATUS      = 313;
const int FE_CURRENT_A2D_12V_POWER_SUPPLIERS_STATUS  = 314;
const int FE_CURRENT_24V_POWER_SUPPLIERS_STATUS      = 315;
const int FE_CURRENT_A2D_24V_POWER_SUPPLIERS_STATUS  = 316;
//Elad -Four OHDB Power Supply
const int FE_CURRENT_OHDB_VPP_POWER_SUPPLIERS_STATUS     = 317;
const int FE_CURRENT_OHDB_A2D_VPP_POWER_SUPPLIERS_STATUS = 318;
const int FE_CURRENT_OHDB_V24_POWER_SUPPLIERS_STATUS     = 319;
const int FE_CURRENT_OHDB_A2D_V24_POWER_SUPPLIERS_STATUS = 320;
const int FE_CURRENT_OHDB_VDD_POWER_SUPPLIERS_STATUS     = 321;
const int FE_CURRENT_OHDB_A2D_VDD_POWER_SUPPLIERS_STATUS = 322;
const int FE_CURRENT_OHDB_VCC_POWER_SUPPLIERS_STATUS     = 323;
const int FE_CURRENT_OHDB_A2D_VCC_POWER_SUPPLIERS_STATUS = 324;


const int FE_CURRENT_MSC1_I2C_ANALOG_IN_7_STATUS    = 325;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_8_STATUS    = 326;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_9_STATUS    = 327;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_10_STATUS   = 328;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_7V_STATUS   = 329;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_8V_STATUS   = 330;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_9V_STATUS   = 331;
const int FE_CURRENT_MSC1_I2C_ANALOG_IN_10V_STATUS  = 332;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_7_STATUS    = 333;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_8_STATUS    = 334;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_9_STATUS    = 335;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_10_STATUS   = 336;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_7V_STATUS   = 337;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_8V_STATUS   = 338;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_9V_STATUS   = 339;
const int FE_CURRENT_MSC2_I2C_ANALOG_IN_10V_STATUS  = 340;

const int FE_CURRENT_PURGE_STATUS               = 341;
const int FE_CURRENT_X_AXIS_POSITION            = 342;
const int FE_CURRENT_Y_AXIS_POSITION            = 343;
const int FE_CURRENT_Z_AXIS_POSITION            = 344;
const int FE_CURRENT_T_AXIS_POSITION            = 345;

const int FE_HEAD_HEATERS_STATUS_BASE       = 346;
const int FE_HEAD_HEATERS_STATUS_HEAD_0     = FE_HEAD_HEATERS_STATUS_BASE; //=346
const int FE_HEAD_HEATERS_STATUS_HEAD_1     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_1;
const int FE_HEAD_HEATERS_STATUS_HEAD_2     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_2;
const int FE_HEAD_HEATERS_STATUS_HEAD_3     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_3;
#ifndef EDEN_250
const int FE_HEAD_HEATERS_STATUS_HEAD_4     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_4;
const int FE_HEAD_HEATERS_STATUS_HEAD_5     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_5;
const int FE_HEAD_HEATERS_STATUS_HEAD_6     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_6;
const int FE_HEAD_HEATERS_STATUS_HEAD_7     = FE_HEAD_HEATERS_STATUS_BASE + HEAD_7;
#endif
const int FE_HEAD_HEATERS_STATUS_BLOCK_0    = FE_HEAD_HEATERS_STATUS_BASE + BLOCK_0;
const int FE_HEAD_HEATERS_STATUS_BLOCK_1    = FE_HEAD_HEATERS_STATUS_BASE + BLOCK_1;
const int FE_HEAD_HEATERS_STATUS_BLOCK_2    = FE_HEAD_HEATERS_STATUS_BASE + BLOCK_2;
const int FE_HEAD_HEATERS_STATUS_BLOCK_3    = FE_HEAD_HEATERS_STATUS_BASE + BLOCK_3;//=357
#ifndef OBJET_MACHINE
const int FE_HEAD_HEATERS_STATUS_PRE_HEATER = FE_HEAD_HEATERS_STATUS_BASE + PRE_HEATER;
#endif

const int FE_HEADS_OPERATION_MODE 				= 363;
const int FE_ERROR_IN_MOTOR                     = 364;
const int FE_CURRENT_VACUUM_LAST_VALUE          = 365;
const int FE_CURRENT_VACUUM_LAST_CMH2O_VALUE    = 366;
const int FE_SET_MACHINE_LOGO                   = 367;

#define IS_REFRESHABLE_ID(_op_) ((FEOP_NOT_REFRESHABLE_START > (_op_)) || (FEOP_NOT_REFRESHABLE_END < (_op_)))


#endif

