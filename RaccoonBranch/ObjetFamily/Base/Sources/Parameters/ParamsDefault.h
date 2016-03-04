/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Application parameters.                                  *
 * Module Description: Default values that differn from Eden 260 to *
 *                     Eden 330.                                    *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 28/05/2003                                           *
 * Last upate: 27/08/2003                                           *
 ********************************************************************/

#ifndef _PARAMS_DEFAULT_H_
#define _PARAMS_DEFAULT_H_

#ifdef OS_WINDOWS
  // The terminal com number can have the value 0 which means that the terminal port is not used
const int TERMINAL_COM_NUM = -1;
#elif defined(OS_VXWORKS)
const int TERMINAL_COM_NUM = 1;
#endif

const char *RF_READER_CONNECTION__CELL_DESCRIPTIONS[]={
       "Reader 0",
       "Reader 1"};

const char *RF_READER_CHANNELS__CELL_DESCRIPTIONS[]={
       "Reader 0",
       "Reader 1"};

const int  OCB_COM_NUM_DEFAULT  = 2;
const int  OHDB_COM_NUM_DEFAULT = 2;
const int  MCB_COM_NUM_DEFAULT  = 1;
const bool ENABLE_COM2_DEFAULT  = true;

const int       MOTOR_PURGE_T_OPEN_POSITION_DEFAULT             = 1500;
const int       MOTOR_PURGE_T_ACT_POSITION_DEFAULT              = 2100;
const char      MAX_MOTORS_DECELERATION[]                       = "30000,10000,10000,100000";
const char      TEST_PATTERN_DATA[]                             =
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF";

const float     X_REPLACE_WASTE_CONTAINER_POS_DEFAULT           = 450.0;
const bool      WEIGHT_SENSORS_BYPASS_DEFAULT                   = false;
const char      MOTORS_SMOOTH_FACTOR_DEFAULT[]                  = "100,10,0,0";
const int       NOZZLES_TEST_HIGH_PRINT_UM_DEFAULT              = 0;
const int       Y_SECONDARY_INTERLACE_NO_PIXELS                 = 0;
const int       VACUUM_LOW_LIMIT_DEFAULT                        = 1950;
const int       VACUUM_LOW_CRITICAL_LIMIT_DEFAULT               = 1750;
const int       VACUUM_HIGH_CRITICAL_LIMIT_DEFAULT              = 2950;
const char      STANDBY_TEMPERATURE_ARRAY_DEFAULT[]             = "2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2000";
const int       PRINTING_TRAY_TEMPERATURE_DEFAULT               = 4000;
const int       START_TRAY_TEMPERATURE_DEFAULT                  = 4000;

const char      HEAD_TEST_V_LOW[]                               = "26,28,28,28";
const char      HEAD_TEST_V_HIGH[]                              = "32,34,34,34";

const char HSW_FIRST_TIME_CALIBRATION_CELL_DEFAULTS[]="0,1";
const char *SUPPORT_MATERIAL_TYPE[] = {"FullCure705","SUP707"};


const float     HSW_BMP_WIDTH                                   = 5.9;
const int       TRAY_Y_SIZE_DEFAULT_MM                          = 397;
const int       TRAY_X_SIZE_DEFAULT_MM                          = 500;
const float     PRODUCT_LT_N  									= 1.125;
const char      HSW_NUM_OF_SLICES[]                             = "17,28"; 
const float DEFAULT_HEAD_WEIGHT                         		= 2.08;
const int  TP_X_RIGHT_SIDE_POINT_DEFAULT                		= 490;
const char TP_TRAY_POINTS_DEFAULT[]                     		= "265,40,480,380,50,380";
const float BUMPER_CALIBRATION_ROLLER_POSITION_Y_DEFAULT 		= 67;                                 
 const bool      TRAY_HEATER_BYPASS                              = true;
const float     Y_REPLACE_WASTE_CONTAINER_POS_DEFAULT           = 62.795;
const float     X_CLEAN_WIPER_POSITION_DEFAULT                  = 450;
const int       Z_CLEAN_HEADS_POSITION_DEFAULT                  = 170.0;
const int       SCATTER_FACTOR_MIN_DEFAULT                      = 100;
const int       SCATTER_FACTOR_MIN_MAX                          = 383;
const int       SCATTER_FACTOR_MAX_MAX                          = 383;
const int       SCATTER_FACTOR_MAX_DEFAULT                      = 383;
const float     X_CLEAN_HEADS_POSITION_DEFAULT                  = 360;
const int       X_ENCODER_HOME_IN_STEP_DEFAULT                  = 1090;
const float     Y_CLEAN_HEADS_POSITION_DEFAULT                  = 100;
const char      X_LAMPS_UV_SENSOR_POSITION[]                    = "557,738";
const char      Y_LAMPS_UV_SENSOR_POSITION[]                    = "315,315";
const char      UV_SENSOR_LAMPS_OFFSET[]                        = "0,0";
const char      UV_SENSOR_LAMPS_GAIN[]                          = "1.906,1.859";
const int       MAX_ALLOWED_MISSING_NOZZLES                     = 10;
const int       RIGHT_SIDE_SPACE_MM                             = 90;
const float     MOTOR_PURGE_Y_ACT_POSITION_DEFAULT              = 23;
const float     MOTOR_PURGE_Y_VELOCITY_DEFAULT                  = 20;
const float     Y_BACKLASH_MOVE_MM                              = 0;
const char      STEPS_PER_MM_DEFAULT[]                          = "24.242,47.2125,500,1";
const float     X_STEPS_PER_PIXEL_DEFAULT                       = 0.512;
const float     Y_STEPS_PER_PIXEL_DEFAULT                       = 4.02448;
const char      MIN_POSITION_STEP_DEFAULT[]                     = "0,-75,-5800,0";
const char      MAX_POSITION_STEP_DEFAULT[]                     = "17900,19000,96500,2192";
const float     TRAY_START_POSITION_X_DEFAULT                   = 130;
const float     TRAY_START_POSITION_Y_DEFAULT                   = 37;
const bool      TRAY_HEATER_LOG                                 = false;
const char      MOTORS_VELOCITY_DEFAULT[]                       = "420,190,4.8,1192";
const char      MOTORS_ACCELERATION_DEFAULT[]                   = "2500,423,200,0";
const char      MOTORS_DECELERATION_DEFAULT[]                   = "2500,423,200,0";
const char      MIN_MOTORS_DECELERATION[]                       = "1387,0,0,0";
const char      MOTORS_KILL_DECELERATION_DEFAULT[]              = "4125,1000,200,0";
const bool      DO_STARTUP_BIT                                  = false;
const float     PURGE_XSTART                                    = 3;
const int       PUMPS_TIME_DUTY_OFF_DEFAULT                     = 1500;
const int       TEST_PATTERN_X_POSITION_DEFAULT                 = 5;
const int       TEST_PATTERN_Y_LOCATION_DEFAULT                 = 250;
const float     TEST_PATTERN_Z_POSITION_DEFAULT                 = -8;
const int       NOZZLES_TEST_600_DPI_THICKNESS_UM_DEFAULT       = 0;
const float     PRODUCT_LT_M                                    = 4.0;
const float     Z_START_PRINT_POSITION_DEFAULT                  = -10;
const float     Z_AXIS_END_PRINT_POSITION_DEFAULT               = -10;
const int       HSW_UV_LAMPS_TIMEOUT_SEC                        = 1200; //bug 6267

const char *HEAD_MAP_ARRAY__CELL_DESCRIPTIONS[]={
	   "H7 (reference)",
	   "H6",
	   "H5",
	   "H4",
	   "H3",
	   "H2",
	   "H1",
       "H0"};

const char HEATERS_MASK_TABLE__CELL_DEFAULTS[]="1,1,1,1,1,1,1,1,1,1,1,1";

const char *HEATERS_MASK_TABLE__CELL_DESCRIPTION[]={
	   "H0 (Left most head)",
	   "H1",
	   "H2",
	   "H3",
	   "H4",
	   "H5",
	   "H6",
	   "H7 (Right most head)",
       "Block 0 - (Sup 0)",
       "Block 1 - (Sup 1)",
       "Block 2 - (Model 0)",
       "Block 3 - (Model 1)"};


const char *POTENTIOMETER_VALUES__CELL_DESCRIPTION[]={
	   "H0 (Left most head)",
	   "H1",
	   "H2",
	   "H3",
	   "H4",
	   "H5",
	   "H6",
	   "H7 (Right most head)"};


const char *REQUESTED_HEAD_VOLTAGES__CELL_DESCRIPTION[]={
	   "H0 (Left most head)",
	   "H1",
	   "H2",
	   "H3",
	   "H4",
	   "H5",
	   "H6",
	   "H7 (Right most head)"};

const char TYPES_ARRAY_PER_TANKS [] = "FullCure705,FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720";
const char TYPES_ARRAY_PER_PIPES [] = "FullCure705,FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720";

const char HSW_IllegalMaterials [] = "N/A,N/A";

#if defined OBJET_MACHINE
const char MATERIAL_WEIGHT_FACTOR[]     = "1.05,1.0,1.0,1.0";
const char MATERIAL_WEIGHT_FACTOR_PER_MODE[] = "1.0,1.1";
const char WEIGHT_OFFLINE_ARRAY[]       = "500,400,400,400,600";
#if defined OBJET_260
const char WEIGHT_LEVEL_LIMIT_ARRAY[]   = "100,100,100,100,7800";
#else
const char WEIGHT_LEVEL_LIMIT_ARRAY[]   = "100,100,100,100,9500";
#endif
const char ACTIVE_THERMISTORS[]         = "1,1,1,1,0,0"; //DM 3 mode (S,M1,M2,M3,S_M3,M1_M2)
//const char ACTIVE_THERMISTORS[]       = "0,0,0,0,1,1"; //SM mode (S,M1,M2,M3,S_M3,M1_M2)
const char TYPES_PER_CHAMBER[]          = "FullCure705,FullCure720,FullCure720,FullCure720,Waste";
const char ACTIVE_TANKS[]               = "0,2,4,6";
//DM 3 material
const char CHAMBER_TANK_RELATION[]      = "1,0,0,0,0,\
										   1,0,0,0,0,\
										   0,1,0,0,0,\
										   0,1,0,0,0,\
										   0,0,1,0,0,\
										   0,0,1,0,0,\
                                           0,0,0,1,0,\
                                           0,0,0,1,0,\
										   0,0,0,0,1";

/*
//DM 2a material                          C0,C1,C2,C3,W  (Tank)
const char CHAMBER_TANK_RELATION[]      = "1,0,0,0,0,\     S1
										   1,0,0,0,0,\     S2
										   0,1,1,0,0,\     M1
										   0,1,1,0,0,\     M2
										   0,1,1,0,0,\     M3
										   0,1,1,0,0,\     M4
										   0,0,0,1,0,\     M5
										   0,0,0,1,0,\     M6
										   0,0,0,0,1";     W

//DM 2b material                          C0,C1,C2,C3,W  (Tank)
const char CHAMBER_TANK_RELATION[]      = "1,0,0,1,0,\     S1
										   1,0,0,1,0,\     S2
										   0,1,0,0,0,\     M1
										   0,1,0,0,0,\     M2
										   0,0,1,0,0,\     M3
										   0,0,1,0,0,\     M4
										   0,0,0,1,0,\     M5
										   0,0,0,1,0,\     M6
										   0,0,0,0,1";     W

//SM
const char CHAMBER_TANK_RELATION[]      = "1,0,0,1,0,\
                                           1,0,0,1,0,\
                                           0,1,1,0,0,\
                                           0,1,1,0,0,\
										   0,1,1,0,0,\
										   0,1,1,0,0,\
										   0,0,0,1,0,\
                                           0,0,0,1,0,\
										   0,0,0,0,1";
*/
//DM 3 material
const char SEGMENT_TANK_RELATION[]      = "1,0,0,0,0,0,0,0,0,0,0,\
										   0,1,0,0,0,0,0,0,0,0,0,\
                                           0,0,1,0,0,0,0,0,1,0,0,\
                                           0,0,0,1,0,0,0,0,1,0,0,\
                                           0,0,0,0,1,0,0,0,0,1,0,\
                                           0,0,0,0,0,1,0,0,0,1,0,\
                                           0,0,0,0,0,0,1,0,0,0,1,\
                                           0,0,0,0,0,0,0,1,0,0,1,\
                                           0,0,0,0,0,0,0,0,0,0,0";
/*
   "S2_SEGMENT"
   ,"S1_SEGMENT"
   //,"S0_1_SEGMENT"
   //,"S0_1_H4_5_SEGMENT"
   ,"MODEL1_H0_1_SEGMENT"
   ,"MODEL2_H0_1_SEGMENT"
   ,"MODEL3_H2_3_SEGMENT"
   ,"MODEL4_H2_3_SEGMENT"
   ,"MODEL5_H4_5_SEGMENT"
   ,"MODEL6_H4_5_SEGMENT"
   //,"MODEL1_2_H2_3_SEGMENT"
   ,"H0_1_SEGMENT"
   ,"H2_3_SEGMENT"
   ,"H4_5_SEGMENT"

//DM 2 material                                                          (Tank)
const char SEGMENT_TANK_RELATION[]      = "1,0,0,0,0,0,0,0,0,0,0,\   S1
                                           0,1,0,0,0,0,0,0,0,0,0,\   S2
                                           0,0,1,0,0,0,0,0,1,0,0,\   M1
                                           0,0,0,1,0,0,0,0,1,0,0,\   M2
                                           0,0,0,0,1,0,0,0,0,1,0,\   M3
                                           0,0,0,0,0,1,0,0,0,1,0,\   M4
                                           0,0,0,0,0,0,1,0,0,0,1,\   M5
                                           0,0,0,0,0,0,0,1,0,0,1,\   M6
                                           0,0,0,0,0,0,0,0,0,0,0";   W

//SM                                                                    (Tank)
const char SEGMENT_TANK_RELATION[]      = "1,0,0,0,0,0,0,0,0,0,0,\  S1
                                           0,1,0,0,0,0,0,0,0,0,0,\  S2
                                           0,0,1,0,0,0,0,0,1,0,0,\  M1
                                           0,0,0,1,0,0,0,0,1,0,0,\  M2
										   0,0,0,0,1,0,0,0,0,1,0,\  M3
										   0,0,0,0,0,1,0,0,0,1,0,\  M4
                                           0,0,0,0,0,0,1,0,0,0,1,\  M5
                                           0,0,0,0,0,0,0,1,0,0,1,\  M6
                                           0,0,0,0,0,0,0,0,0,0,0";  W
*/

//DM 3 material
const char PUMP_TANK_RELATION[]         = "1,0,0,0,0,0,0,0,\
										   0,1,0,0,0,0,0,0,\
                                           0,0,1,0,0,0,0,0,\
                                           0,0,0,1,0,0,0,0,\
                                           0,0,0,0,1,0,0,0,\
                                           0,0,0,0,0,1,0,0,\
                                           0,0,0,0,0,0,1,0,\
                                           0,0,0,0,0,0,0,1,\
                                           0,0,0,0,0,0,0,0";
/*
/DM 3 material                       P0,P1,P2,P3,P4,P5,P6,P7   (Tank)
const char PUMP_TANK_RELATION[]         = "1,0,0,0,0,0,0,0,\     S1
										   0,1,0,0,0,0,0,0,\     S2
										   0,0,1,0,0,0,0,0,\     M1
										   0,0,0,1,0,0,0,0,\     M2
										   0,0,0,0,1,0,0,0,\     M3
										   0,0,0,0,0,1,0,0,\     M4
										   0,0,0,0,0,0,1,0,\     M5
										   0,0,0,0,0,0,0,1,\     M6
										   0,0,0,0,0,0,0,0";     W

//DM 2 material                                                  (Tank)
const char PUMP_TANK_RELATION[]         = "1,0,0,0,0,0,0,0,\   S1
                                           0,1,0,0,0,0,0,0,\   S2
                                           0,0,1,0,0,0,0,0,\   M1
                                           0,0,0,1,0,0,0,0,\   M2
                                           0,0,0,0,1,0,0,0,\   M3
                                           0,0,0,0,0,1,0,0,\   M4
                                           0,0,0,0,0,0,0,0,\   M5
                                           0,0,0,0,0,0,0,0,\   M6
                                           0,0,0,0,0,0,0,0";   W

//SM
const char PUMP_TANK_RELATION[]         = "0,1,0,0,0,0,0,0,\
										   1,0,0,0,0,0,0,0,\
										   0,0,1,0,0,0,0,0,\
										   0,0,0,1,0,0,0,0,\
										   0,0,0,0,1,0,0,0,\
                                           0,0,0,0,0,1,0,0,\
                                           0,0,0,0,0,0,0,0,\
                                           0,0,0,0,0,0,0,0,\
                                           0,0,0,0,0,0,0,0";
*/
const int RR_DELAY_BETWEEN_PURGES       = 5;
const int RR_LONG_CLEANSER_DRAIN_TIME   = 360;
const int RR_LONG_COLOR_TO_CLEAR_TIME   = 350;
const int RR_LONG_PURGE_CYCLES          = 3;
const int RR_LONG_RESIN_DRAIN_TIME      = 150;
const int RR_SHORT_CLEAR_TO_COLOR_TIME  = 150;
const int RR_SHORT_CLEANING_CYCLES      = 0;
const int RR_SHORT_PURGE_CYCLES         = 3;
const int MR_CHAMBER_DRAINING_TIMOUT_SECONDS = 90;

const char CLEANSER_PRIMING_TIME [] = "160,220";
const char CLEANSER_WASHING_TIME [] = "180,220";
const int PIPES_CLEARING_TIME   = 80;

const char SHD_RINSING_DURATION [] = "15,20,5";


/*************** For heads calibration**********************************************/
/**/  const char HIGH_RESISTORS[] = "30.1, 12.7";        		         /**/
/**/  const char LOW_RESISTORS[]  = "1.0, 0.422";        		         /**/
/**/  const char PULSE_WIDTH[]  = "9,9";                 		         /**/
/*  Please notice !!! For HS/DM modes, the value for PULSE_DELAY[]= "15,18";      **/
/* It was config under "Digital Materials Mode.cfg/High Speed Mode.cfg files" "   **/
/**/  const char PULSE_DELAY[]  = "30, 30";                                      /**/      		
/**/  const char POST_PULSER_DELAY[]  = "25, 25";        			 /**/
/**/  const char HEAD_DELAY_REGISTERS[]  = "1, 1";      			 /**/
/* End of head clibration parameters************************************************/


#endif
#endif
