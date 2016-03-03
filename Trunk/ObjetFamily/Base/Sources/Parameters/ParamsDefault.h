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
		"Reader 0"
	   ,"Reader 1"
#ifdef OBJET_MACHINE_KESHET
	   ,"Reader 2"
	   ,"Reader 3"
#endif
};

const char *RF_READER_CHANNELS__CELL_DESCRIPTIONS[]={
	    "Reader 0"
	   ,"Reader 1"
#ifdef OBJET_MACHINE_KESHET
	   ,"Reader 2"
	   ,"Reader 3"
#endif
};

const int  OCB_COM_NUM_DEFAULT  = 2;
const int  OHDB_COM_NUM_DEFAULT = 2;
const int  MCB_COM_NUM_DEFAULT  = 1;
const bool ENABLE_COM2_DEFAULT  = true;

const int       MOTOR_PURGE_T_OPEN_POSITION_DEFAULT             = 1500;
const int       MOTOR_PURGE_T_ACT_POSITION_DEFAULT              = 2100;
const char      MAX_MOTORS_DECELERATION[]                       = "30000,10000,10000,100000";
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
const int       SCATTER_FACTOR_MIN_DEFAULT                      = 100;
const int       SCATTER_FACTOR_MIN_MAX                          = 383;
const int       SCATTER_FACTOR_MAX_DEFAULT                      = 383;
const int       SCATTER_FACTOR_MAX_MAX                          = 383;
const char      UV_SENSOR_LAMPS_OFFSET[]                        = "0,0";
const char      UV_SENSOR_LAMPS_GAIN[]                          = "1.906,1.859";
const char       HIGHEST_POINT_DEFAULT[]                           = "0,0";
const float     HSW_BMP_WIDTH                                   = 4.1;
const int       TRAY_Y_SIZE_DEFAULT_MM                          = 350;
const int       TRAY_X_SIZE_DEFAULT_MM                          = 350;
const float     PRODUCT_LT_N                                    = 1.098;
const char      HSW_NUM_OF_SLICES[]                             = "17,29";
const bool      TRAY_HEATER_ENABLED                             = false;
const float     Y_REPLACE_WASTE_CONTAINER_POS_DEFAULT           = 62.795;
const float     X_CLEAN_WIPER_POSITION_DEFAULT                  = 450;
const int       Z_CLEAN_HEADS_POSITION_DEFAULT                  = 170.0;
const float     X_CLEAN_HEADS_POSITION_DEFAULT                  = 360;
const int       X_ENCODER_HOME_IN_STEP_DEFAULT                  = 1090;
const float     Y_CLEAN_HEADS_POSITION_DEFAULT                  = 100;
const char      X_LAMPS_UV_SENSOR_POSITION[]                    = "557,738";
const char      Y_LAMPS_UV_SENSOR_POSITION[]                    = "315,315";
const int       MAX_ALLOWED_MISSING_NOZZLES                     = 10;
#ifdef OBJET_1000
	const int       RIGHT_SIDE_SPACE_MM                             = 150;
#else
	const int       RIGHT_SIDE_SPACE_MM                             = 90;
#endif

const float     MOTOR_PURGE_Y_ACT_POSITION_DEFAULT              = 23;
const float     MOTOR_PURGE_Y_VELOCITY_DEFAULT                  = 20;
const float     Y_BACKLASH_MOVE_MM                              = 0;
const float     Y_STEPS_PER_PIXEL_DEFAULT                       = 4.02448;
const char      STEPS_PER_MM_DEFAULT[]                          = "24.242,47.2125,500,1";
const float     X_STEPS_PER_PIXEL_DEFAULT                       = 0.512;
const char      MIN_POSITION_STEP_DEFAULT[]                     = "0,-75,-5800,0";
const char      MAX_POSITION_STEP_DEFAULT[]                     = "17900,19000,96500,2192";
const int       TEST_PATTERN_X_POSITION_DEFAULT                 = 5;
const int       TEST_PATTERN_Y_LOCATION_DEFAULT                 = 250;
const float     Z_AXIS_END_PRINT_POSITION_DEFAULT               = -10;
const float     Z_MAX_POSITION_MARGIN_DEFAULT                   = 0 ;   //On Objet1000 need to calibrate per machine (On other machines the value is zero)
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
const float     TEST_PATTERN_Z_POSITION_DEFAULT                 = -8;
const int       NOZZLES_TEST_600_DPI_THICKNESS_UM_DEFAULT       = 0;
const float     PRODUCT_LT_M                                    = 4.0;
const float     Z_START_PRINT_POSITION_DEFAULT                  = -10;
const int       HSW_UV_LAMPS_TIMEOUT_SEC                        = 1200; //bug 6267

#ifdef OBJET_500
	const char TP_TRAY_POINTS_DEFAULT[]                     = "265,40,480,380,50,380,0,0,0,0";
#else
	const char TP_TRAY_POINTS_DEFAULT[]                 = "375,655,375,20,1300,20,1300,655,837.2,337.5";
#endif

const char TP_Z_START_POINTS_DEFAULT[] =            "400,100,600,100,800,100,1000,100,1200,100,400,250,600,250,800,250,1000,250,1200,250,400,400,600,400,800,400,1000,400,1200,400,400,550,600,550,800,550,1000,550,1200,550,400,700,600,700,800,700,1000,700,1200,700";

const char *HEAD_MAP_ARRAY__CELL_DESCRIPTIONS[]={
	   "H0 (Support)",
	   "H1 (M7)",
	   "H2 (M6)",
	   "H3 (M5)",
	   "H4 (M4)",
	   "H5 (M3)",
	   "H6 (M2)",
	   "H7 (M1 - reference)"};
#ifdef OBJET_MACHINE_KESHET
const char HEATERS_MASK_TABLE__CELL_DEFAULTS[]="1,1,1,1,1,1,1,1,1,1,1,1,1";
const bool IS_BLOCK_TABLE_CONVERSION_EXIST = true;
#else
const char HEATERS_MASK_TABLE__CELL_DEFAULTS[]="1,1,1,1,1,1,1,1,1,1,1,1";
const bool IS_BLOCK_TABLE_CONVERSION_EXIST = false;
#endif

const char *HEATERS_MASK_TABLE__CELL_DESCRIPTION[]={
	   "Head 0 - Support 1",
	   "Head 1 - Support 0",
	   "Head 2 - Model 5",
	   "Head 3 - Model 4",
	   "Head 4 - Model 3",
	   "Head 5 - Model 2",
	   "Head 6 - Model 1",
	   "Head 7 - Model 0",
	   "Block 0 - (Sup 0)",
	   "Block 1 - (Sup 1)",
	   "Block 2 - (Model 0)",
	   "Block 3 - (Model 1)"};

const char *POTENTIOMETER_VALUES__CELL_DESCRIPTION[]={
	   "Head S1",
       "Head S0",
	   "Head M5",
       "Head M4",
	   "Head M3",
       "Head M2",
	   "Head M1",
       "Head M0"};


const char *REQUESTED_HEAD_VOLTAGES__CELL_DESCRIPTION[]={
       "Head S1",
	   "Head S0",
       "Head M5",
	   "Head M4",
       "Head M3",
	   "Head M2",
	   "Head M1",
	   "Head M0"};

const char MATERIAL_BMP_NUMBER_MAPPING_DEFAULTS[]="8,1,2,3,4,5,6,7";
const char *MATERIAL_BMP_NUMBER_MAPPING_DESCRIPTION[]={
	   "Support",
	   "M1",
	   "M2",
	   "M3",
	   "M4",
	   "M5",
	   "M6",
	   "Support"};

const float DEFAULT_HEAD_WEIGHT                         = 2.08;
const int  TP_X_RIGHT_SIDE_POINT_DEFAULT                = 490;
const float BUMPER_CALIBRATION_ROLLER_POSITION_Y_DEFAULT = 67;

const char HSW_IllegalMaterials [] = "M.Cleanser,S.Cleanser";
const char MATERIAL_WEIGHT_FACTOR_PER_MODE[] = "1.0,1.1";
const bool REMOVABLE_TRAY_ENABLED = false;

#ifdef OBJET_MACHINE_KESHET
const bool PURGE_BATH_ENABLED = true;
const bool HQ_HS_ALL_THERM_DEFAULT_CONFIG = true;
#else
const bool PURGE_BATH_ENABLED = false;
const bool HQ_HS_ALL_THERM_DEFAULT_CONFIG = false;
#endif
const int PURGE_BATH_STABILIZATION_WIDTH = 15;

//const int FSW_DEFAULT_FIXTURE_PURGE_ZPOS    = 95000; // deprecated
//const int FSW_DEFUALT_MIN_CLEANSER_WEIGHT   = 720; // deprecated
//const int FSW_DEFAULT_LEFT_PURGE_DURATION   = 60; // deprecated
//const int FSW_DEFAULT_RIGHT_PURGE_DURATION  = 60; // deprecated
//const int FSW_DEFAULT_TUBE_DRAIN_AIR_TIME   = 120; // deprecated
//const int FSW_DEFAULT_BLOCK_WASH_CYCLES     = 2; // deprecated
//const int FSW_DEFAULT_BLOCK_WASH_DRAIN_TIME = 120; // deprecated

#ifdef OBJET_1000
	const bool SIGNAL_TOWER_ENABLED       = true;
	const int SHD_NEEDED_CARTRIDGE_WEIGHT = 1000;
	const int BUMPER_CALIBRATION_DELTA    = 245;
	const int CARTIDGE_FULL_WEIGHT        = 17500;
	const int CARTIDGE_INITIAL_WEIGHT     = 18000;
	const bool DUAL_WASTE_ENABLED         = true;
#else
	const bool SIGNAL_TOWER_ENABLED       = false;
	#ifdef OBJET_MACHINE_KESHET
	  const int SHD_NEEDED_CARTRIDGE_WEIGHT = 400;
	#else
	  const int SHD_NEEDED_CARTRIDGE_WEIGHT = 560;
	#endif
	const int BUMPER_CALIBRATION_DELTA    = 0;
	const int CARTIDGE_FULL_WEIGHT        = 3500;
	const int CARTIDGE_INITIAL_WEIGHT     = 3600;
	const bool DUAL_WASTE_ENABLED         = false;
#endif

const bool AMBIENT_HEATER_ENABLED = false;
const bool EVACUATION_INDICATOR_ENABLED = false;
const bool IS_SAFETY_SYSTEM_EXIST  = false ;
const bool IS_NORMALLY_OPEN        = false ;

#ifdef OBJET_MACHINE_KESHET
const char TEST_PATTERN_DATA[] =
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
							 "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF";
							 
const char HEAD_TEST_V_LOW[] 	  = "26,28,28,28,28,28,28,28";
const char HEAD_TEST_V_HIGH[]     = "30,30,30,30,30,30,30,30";
const char WEIGHT_OFFLINE_ARRAY[] = "500,400,400,400,400,400,400,400,600";
const char MATERIAL_WEIGHT_FACTOR[]     = "1.05,1.0,1.0,1.0,1.0,1.0,1.0,1.0";
const char TYPES_ARRAY_PER_TANKS [] = "FullCure705,FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720";
const char TYPES_ARRAY_PER_PIPES [] = "FullCure705,FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720";
const char WEIGHT_LEVEL_LIMIT_ARRAY[]   = "100,100,100,100,100,100,100,100,9500";
const char ACTIVE_THERMISTORS[]         = "1,1,1,1,1,1,1,1,0,0,0,0"; /* DM6v2 *///DM 7 mode ([low]S,M1,M2,M3,M4,M5,M6,M7,[high]S_M7,M1_M2,M3_M4,M5_M6) // DM6v2
const char TYPES_PER_CHAMBER[]          = "FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure705,Waste";//S,C1,C2,C3,C4,C5,C6,C7 // DM6
const char ACTIVE_TANKS[]               = "0,2,4,6,8,10,12,0,16"; /* DM6 *///S,C1,C2,C3,C4,C5,C6,C7,W

const char HSW_FIRST_TIME_CALIBRATION_CELL_DEFAULTS[]="1,1";
const char *SUPPORT_MATERIAL_TYPE[] = {"FullCure705","SUP706"};

//DM7 [C0,C1,C2,C3,C4,C5,C6,C7,W] X [S1,S2,M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14,W1,W2]
const char CHAMBER_TANK_RELATION[] = "1,0,0,0,0,0,0,0,0,\
									  1,0,0,0,0,0,0,0,0,\
									  0,1,0,0,0,0,0,0,0,\
									  0,1,0,0,0,0,0,0,0,\
									  0,0,1,0,0,0,0,0,0,\
									  0,0,1,0,0,0,0,0,0,\
									  0,0,0,1,0,0,0,0,0,\
									  0,0,0,1,0,0,0,0,0,\
									  0,0,0,0,1,0,0,0,0,\
									  0,0,0,0,1,0,0,0,0,\
									  0,0,0,0,0,1,0,0,0,\
									  0,0,0,0,0,1,0,0,0,\
									  0,0,0,0,0,0,1,0,0,\
									  0,0,0,0,0,0,1,0,0,\
									  0,0,0,0,0,0,0,1,0,\
									  0,0,0,0,0,0,0,1,0,\
									  0,0,0,0,0,0,0,0,1,\
									  0,0,0,0,0,0,0,0,1";


//DM7 material [S1,S2,M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14,M1_M2,M3_M4,M5_M6,M7_M8,M9_M10,M11_M12,M13_M14] X [S1,S2,M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14]
const char SEGMENT_TANK_RELATION[]      = "1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
										   0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
										   0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,\
										   0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,\
										   0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,\
										   0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,\
										   0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,\
										   0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,\
										   0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,\
										   0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,\
										   0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,\
										   0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,\
										   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1";


//DM7 material [PS1,PS2,PM1,PM2,PM3,PM4,PM5,PM6,PM7,PM8,PM9,PM10,PM11,PM12,PM13,PM14] X [S1,S2,M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14]
const char PUMP_TANK_RELATION[]         = "1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
										   0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
										   0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,\
										   0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,\
										   0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,\
										   0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,\
										   0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,\
										   0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,\
										   0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,\
										   0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,\
										   0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,\
										   0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,\
										   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1";

#else
const char      TEST_PATTERN_DATA[]                             =
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,"\
                             "FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF";
const char HEAD_TEST_V_LOW[]      = "26,28,28,28";
const char HEAD_TEST_V_HIGH[]     = "32,34,34,34";
const char WEIGHT_OFFLINE_ARRAY[] = "500,400,400,400,600";
const char MATERIAL_WEIGHT_FACTOR[]     = "1.05,1.0,1.0,1.0";
const char TYPES_ARRAY_PER_TANKS [] = "FullCure705,FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720";
const char TYPES_ARRAY_PER_PIPES [] = "FullCure705,FullCure705,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720,FullCure720";
const char WEIGHT_LEVEL_LIMIT_ARRAY[]   = "100,100,100,100,7800";
const char ACTIVE_THERMISTORS[]         = "1,1,1,1,0,0"; //DM 3 mode (S,M1,M2,M3,S_M3(high),M1_M2(high))
const char TYPES_PER_CHAMBER[]          = "FullCure705,FullCure720,FullCure720,FullCure720,Waste";
const char ACTIVE_TANKS[]               = "0,2,4,6,8";

//DM 3 material
const char CHAMBER_TANK_RELATION[]      = "1,0,0,0,0,\
										   1,0,0,0,0,\
										   0,1,0,0,0,\
										   0,1,0,0,0,\
										   0,0,1,0,0,\
										   0,0,1,0,0,\
										   0,0,0,1,0,\
										   0,0,0,1,0,\
										   0,0,0,0,1,\
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
										   0,0,0,0,0,0,0,1,0,0,1";
/*

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
										   0,0,0,0,0,0,0,1";
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
const char PUMP_TANK_RELATION[]         = "1,0,0,0,0,0,0,0,\
										   0,1,0,0,0,0,0,0,\
										   0,0,1,0,0,0,0,0,\
										   0,0,0,1,0,0,0,0,\
										   0,0,0,0,1,0,0,0,\
                                           0,0,0,0,0,1,0,0,\
                                           0,0,0,0,0,0,0,0,\
                                           0,0,0,0,0,0,0,0,\
                                           0,0,0,0,0,0,0,0";
*/

#endif

const int RR_DELAY_BETWEEN_PURGES       = 5;
const int RR_LONG_CLEANSER_DRAIN_TIME   = 360;
const int RR_LONG_COLOR_TO_CLEAR_TIME   = 350;
const int RR_LONG_PURGE_CYCLES          = 3;
const int RR_LONG_RESIN_DRAIN_TIME      = 150;
const int RR_SHORT_CLEAR_TO_COLOR_TIME  = 150;
const int RR_SHORT_CLEANING_CYCLES      = 0;
const int RR_SHORT_PURGE_CYCLES         = 3;
const int MR_CHAMBER_DRAINING_TIMOUT_SECONDS = 90;
const int PIPES_CLEARING_TIME   = 80;
const char SHD_RINSING_DURATION [] = "5,12,12,18,22";
#endif
