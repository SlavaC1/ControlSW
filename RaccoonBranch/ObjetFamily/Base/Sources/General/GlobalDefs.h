/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Global defintions.  - GlobalDefs.h                                     *
 * Module Description: Global definitions of types and constants    *
 *                     that no one knows were to put.               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/09/2001                                           *
 * Last upate: 02/09/2001                                           *
 ********************************************************************/

#ifndef _GLOBAL_DEFS_H_ /*{*/
#define _GLOBAL_DEFS_H_

#pragma warn -8057 // Disable parameter never used warning

#include "QException.h"
#include "QComponent.h"

#include <vector>

#define APPLICATION_NAME LOAD_STRING(IDS_APPLICATION_NAME)
#define QAPPLICATION_NAME LOAD_QSTRING(IDS_APPLICATION_NAME)

//This is not the naive way of doing it but it overcomes a bug
#define DEL_LAST_2_CHARS(str)(str.erase(str.end() - 2, str.end()))

#define MODEL_CLEANSER "M.Cleanser"
#define SUPPORT_CLEANSER "S.Cleanser"

#if defined EDEN_250 || defined EDEN_260 || defined EDEN_350 /*{*/
#define OBJET_TWO_TANK_MACHINE
#elif defined EDEN_350_V || defined EDEN_500 || defined CONNEX_MACHINE
#define OBJET_FOUR_TANK_MACHINE
#elif defined OBJET_MACHINE  /*itamar objet*/
#define OBJET_EIGHT_TANK_MACHINE
#endif /*}*/

#define MM_PER_INCH    25.4
#define CM_PER_INCH    2.54
#define MICRON_IN_CM   10000

//#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined EDEN_260_V || defined CONNEX_MACHINE || defined OBJET_MACHINE
#define NOZZLES_IN_HEAD 96
#define DPI_Y_PER_HEAD 37.5
//#endif
 
#define DIMArray(Array) (sizeof(Array)/sizeof(Array[0]))

#ifndef PARAMS_EXP_IMP /*{*/


typedef enum {UVLampCalibrationMode,
              UVIntensityEvaluationMode} TUVMode;

/*//////////////////////////////////////////////////////////////////////////////
ACTUATORS
//////////////////////////////////////////////////////////////////////////////*/
#define DEFINE_ACTUATOR(id, descStr, formPartStr) ACTUATOR_ID_##id,
#define DEFINE_SENSOR(id, formPartStr)
typedef enum
{
#include "ActuatorDefs.h"
   MAX_ACTUATOR_ID
}TActuatorIndex;
#undef DEFINE_ACTUATOR
#undef DEFINE_SENSOR

#define DEFINE_ACTUATOR(id, descStr, formPartStr)
#define DEFINE_SENSOR(id, formPartStr) SENSOR_ID_##id,
////////////////////////OBJET/////////////////////////////
const int mtEden260        = 0;
const int mtEden330        = 1;
const int mtEden500        = 2;
const int mtEden250        = 3;
const int mtEden350        = 4;
const int mtEden350_V      = 5;
const int mtEden3Resin500v = 6;
const int mtEden3Resin350v = 7;
const int mtEden260_V      = 8;
const int mtEden3Resin260v = 9;
const int mtObjet260       = 10;
const int mtObjet350       = 11;
const int mtObjet500       = 12;
const int mtObjet1000      = 13;

const int mfEden        = 0;
const int mfConnex      = 1;
const int mfObjet     = 2;
typedef int TMachineFOM;

/*//////////////////////////////////////////////////////////////////////////////
SENSORS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
#include "ActuatorDefs.h"
   MAX_SENSOR_ID
}TSensorIndex;
#undef DEFINE_ACTUATOR
#undef DEFINE_SENSOR

#define ACTUATOR_ON  1
#define ACTUATOR_OFF 0

#endif/*}*/
/*//////////////////////////////////////////////////////////////////////////////
LAMPS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    RIGHT_UV_LAMP = 0
#ifndef EDEN_250 //fixme - other machine too?
   ,LEFT_UV_LAMP
#endif
   ,NUMBER_OF_UV_LAMPS
   ,ALL_UV_LAMPS = NUMBER_OF_UV_LAMPS
}TUVLampIndex;

#define VALIDATE_UV_LAMP(lamp) ((0 <= lamp) && (NUMBER_OF_UV_LAMPS > lamp))
#define FOR_ALL_UV_LAMPS(lamp)	  for(short lamp = 0; lamp < NUMBER_OF_UV_LAMPS; lamp++)

#ifdef EDEN_250
#define DEFINE_UV_ARRAY(_member_)  { RIGHT_UV_LAMP_##_member_};
#else
#define DEFINE_UV_ARRAY(_member_)  { RIGHT_UV_LAMP_##_member_,LEFT_UV_LAMP_##_member_ };
#endif /*}*/

/*//////////////////////////////////////////////////////////////////////////////
OPERATION MODES
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
	 SINGLE_MATERIAL_OPERATION_MODE = 0
	,DIGITAL_MATERIAL_OPERATION_MODE
    ,NUM_OF_OPERATION_MODES
    ,FIRST_OPERATION_MODE = SINGLE_MATERIAL_OPERATION_MODE
    ,LAST_OPERATION_MODE = NUM_OF_OPERATION_MODES
    ,ALL_OPERATION_MODES = NUM_OF_OPERATION_MODES
}TOperationModeIndex;
typedef enum
{
  RESISTOR_HIGH_OLD = 0,
  RESISTOR_HIGH_NEW,
  NUM_OF_HIGH_RESISTORS
}THighResistors;

typedef enum
{
  RESISTOR_LOW_OLD = 0,
  RESISTOR_LOW_NEW,
  NUM_OF_LOW_RESISTORS
}TLowResistors;

#define VALIDATE_OPERATION_MODE(opm)      ((FIRST_OPERATION_MODE <= opm) && (LAST_OPERATION_MODE > opm))
#define FOR_ALL_OPERATION_MODES(opm)	  for(short opm = FIRST_OPERATION_MODE; opm < LAST_OPERATION_MODE; opm++)
/*//////////////////////////////////////////////////////////////////////////////
QUALITY MODES
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
#ifndef EDEN_350
    HS_INDEX  = 0
#endif
   ,HQ_INDEX
   ,LOW_TEMPATURE_INDEX = 0 
   ,HIGH_TEMPATURE_INDEX = HQ_INDEX
   ,HIGH_RESOLUTION_INDEX = HQ_INDEX 
   ,NUMBER_OF_QUALITY_MODES
   ,FIRST_QUALITY_MODE         = 0
   ,LAST_QUALITY_MODE          = NUMBER_OF_QUALITY_MODES
   ,DEFAULT_QUALITY_MODE_INDEX = NUMBER_OF_QUALITY_MODES
   ,ALL_QUALITY_MODES          = NUMBER_OF_QUALITY_MODES
}TQualityModeIndex;

#define VALIDATE_QUALITY_MODE(mode)    ((0 <= mode) && (NUMBER_OF_QUALITY_MODES > mode))
#define FOR_ALL_QUALITY_MODES(qm)      for(short qm = FIRST_QUALITY_MODE; qm < LAST_QUALITY_MODE; qm++)

#define FOR_ALL_MODES(qm, om)  for(int qm = 0; qm < NUMBER_OF_QUALITY_MODES; qm++) \
                                   for(int om = 0; om < NUM_OF_OPERATION_MODES; om++)

/*//////////////////////////////////////////////////////////////////////////////
PUMPS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    PUMP_0              = 0
   ,FIRST_SUPPORT_PUMP  = PUMP_0
   ,PUMP_1
#ifdef OBJET_TWO_TANK_MACHINE
   ,FIRST_MODEL_PUMP    = PUMP_1
#elif defined OBJET_FOUR_TANK_MACHINE || defined OBJET_MACHINE
   ,PUMP_2
   //,MODEL_PUMP1		= PUMP_2 , objet_machine - no need
   ,FIRST_MODEL_PUMP    = PUMP_2
   ,PUMP_3
   //,MODEL_PUMP2		= PUMP_3   objet_machine - no need
   ,PUMP_4
   ,PUMP_5
#ifdef OBJET_MACHINE
   ,PUMP_6
   ,PUMP_7
#endif
#endif
 #if defined CONNEX_MACHINE || defined OBJET_MACHINE
   ,SUPPORT_PUMP1           = PUMP_1
   ,SUPPORT_PUMP2           = PUMP_0
 #endif
 #ifdef CONNEX_MACHINE
   ,MODEL2_H2_3_PUMP        = PUMP_2
   ,MODEL1_H2_3_PUMP        = PUMP_3
   ,MODEL2_H0_1_PUMP        = PUMP_4
   ,MODEL1_H0_1_PUMP        = PUMP_5
 #endif
 #ifdef OBJET_MACHINE
   ,MODEL1_H0_1_PUMP   		= PUMP_2
   ,MODEL2_H0_1_PUMP  		= PUMP_3
   ,MODEL3_H2_3_PUMP   		= PUMP_4
   ,MODEL4_H2_3_PUMP  		= PUMP_5
   ,MODEL5_H4_5_PUMP  		= PUMP_6
   ,MODEL6_H4_5_PUMP  		= PUMP_7
 #endif
   ,NUMBER_OF_PUMPS
   ,LAST_SUPPORT_PUMP       = FIRST_MODEL_PUMP
   ,LAST_MODEL_PUMP         = NUMBER_OF_PUMPS
   ,NUMBER_OF_SUPPORT_PUMPS = LAST_SUPPORT_PUMP - FIRST_SUPPORT_PUMP
   ,NUMBER_OF_MODEL_PUMPS   = NUMBER_OF_PUMPS   - NUMBER_OF_SUPPORT_PUMPS
   ,FIRST_PUMP              = FIRST_SUPPORT_PUMP
}TPumpIndex;

QString PumpIndexToDescription(TPumpIndex i);

/*//////////////////////////////////////////////////////////////////////////////
SEGMENTS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    FIRST_SEGMENT
#ifdef CONNEX_MACHINE
   ,FIRST_MODEL_SEGMENT = FIRST_SEGMENT
   ,MODEL2_H2_3_SEGMENT = FIRST_MODEL_SEGMENT
   ,MODEL1_H2_3_SEGMENT
   ,MODEL2_H0_1_SEGMENT
   ,MODEL1_H0_1_SEGMENT
   ,H0_1_SEGMENT            /*Left Top*/
   ,H2_3_SEGMENT            /*Right Top*/
   ,LAST_MODEL_SEGMENT = H2_3_SEGMENT
   // Support-Tanks Segments
   ,FIRST_SUPPORT_SEGMENT
   ,S2_SEGMENT = FIRST_SUPPORT_SEGMENT // Todo Shahar: move Support segments to beginning of enum, and check if the third segment is really needed.
   ,S1_SEGMENT                         // Yes, support left-right segments are reversed. Checkout TTankIndex's enum
   ,LAST_SUPPORT_SEGMENT = S1_SEGMENT
   ,NUMBER_OF_SEGMENTS
#endif   
#ifdef OBJET_MACHINE
   ,FIRST_SUPPORT_SEGMENT = FIRST_SEGMENT
   ,S1_SEGMENT = FIRST_SUPPORT_SEGMENT
   ,S2_SEGMENT
   //,S0_1_SEGMENT         /*Support Top*/
   //,S0_1_H4_5_SEGMENT    /*Support/Model*/
   ,LAST_SUPPORT_SEGMENT = S2_SEGMENT
   ,FIRST_MODEL_SEGMENT
   ,MODEL1_H0_1_SEGMENT = FIRST_MODEL_SEGMENT
   ,MODEL2_H0_1_SEGMENT
   ,MODEL3_H2_3_SEGMENT
   ,MODEL4_H2_3_SEGMENT
   ,MODEL5_H4_5_SEGMENT
   ,MODEL6_H4_5_SEGMENT
   //,MODEL1_2_H2_3_SEGMENT
   ,FIRST_SHARED_SEGMENT
   ,H0_1_SEGMENT = FIRST_SHARED_SEGMENT /*First Model Top*/
   ,H2_3_SEGMENT            			/*Second Model Top*/
   ,H4_5_SEGMENT            			/*Third Model Top*/
   ,LAST_MODEL_SEGMENT = H4_5_SEGMENT
   ,LAST_SHARED_SEGMENT = LAST_MODEL_SEGMENT
   ,NUMBER_OF_SEGMENTS
#endif   

}TSegmentIndex;

 #ifndef PARAMS_EXP_IMP /*{*/
 extern const TActuatorIndex PumpsActuatorID[NUMBER_OF_PUMPS];
#ifdef CONNEX_MACHINE
QString SegmentDescription[NUMBER_OF_SEGMENTS] =
{
    "MODEL2_H2_3_SEGMENT"
   ,"MODEL1_H2_3_SEGMENT"
   ,"MODEL2_H0_1_SEGMENT"
   ,"MODEL1_H0_1_SEGMENT"
   ,"H0_1_SEGMENT"
   ,"H2_3_SEGMENT"
   ,"S2_SEGMENT"
   ,"S1_SEGMENT"
};
#elif defined OBJET_MACHINE
QString SegmentDescription[NUMBER_OF_SEGMENTS] =
{
   "SL"
   ,"SR"
   ,"M1L"
   ,"M1R"
   ,"M2L"
   ,"M2R"
   ,"M3L"
   ,"M3R"
   ,"M1_LONG"
   ,"M2_LONG"
   ,"M3_LONG"
};
#endif
 #endif /*}*/

/*//////////////////////////////////////////////////////////////////////////////
CHAMBERS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    TYPE_CHAMBER_SUPPORT           = 0
   ,TYPE_CHAMBER_MODEL1
   ,FIRST_SUPPORT_CHAMBER_INDEX    = TYPE_CHAMBER_SUPPORT
   ,FIRST_MODEL_CHAMBER_INDEX      = TYPE_CHAMBER_MODEL1
   ,MAX_NUMBER_OF_SUPPORT_CHAMBERS = TYPE_CHAMBER_MODEL1
   ,LAST_SUPPORT_CHAMBER_INDEX     = MAX_NUMBER_OF_SUPPORT_CHAMBERS

#if defined CONNEX_MACHINE || defined OBJET_MACHINE
   ,TYPE_CHAMBER_MODEL2
#endif
#ifdef OBJET_MACHINE
   ,TYPE_CHAMBER_MODEL3 
#endif
   ,NUMBER_OF_CHAMBERS

   ,LAST_MODEL_CHAMBER_INDEX       = NUMBER_OF_CHAMBERS
   ,MAX_NUMBER_OF_MODEL_CHAMBERS   = NUMBER_OF_CHAMBERS - MAX_NUMBER_OF_SUPPORT_CHAMBERS

   ,TYPE_CHAMBER_WASTE             = NUMBER_OF_CHAMBERS
   ,NUMBER_OF_CHAMBERS_INCLUDING_WASTE

   ,FIRST_CHAMBER_TYPE             = TYPE_CHAMBER_SUPPORT
   ,LAST_CHAMBER_TYPE              = NUMBER_OF_CHAMBERS

   ,FIRST_CHAMBER_TYPE_INCLUDING_WASTE = TYPE_CHAMBER_SUPPORT
   ,LAST_CHAMBER_TYPE_INCLUDING_WASTE  = NUMBER_OF_CHAMBERS_INCLUDING_WASTE

   ,MINIMUM_REQUIRED_MODEL_NUM         = 1
   ,MINIMUM_REQUIRED_SUPPORT_NUM       = 1
   ,MINIMUM_REQUIRED_RESIN_NUM         = (MINIMUM_REQUIRED_MODEL_NUM+MINIMUM_REQUIRED_SUPPORT_NUM)

   ,SM_MODEL_NUM                       = MINIMUM_REQUIRED_MODEL_NUM
   ,MM_MODEL_NUM                       = MAX_NUMBER_OF_MODEL_CHAMBERS
   ,SM_RESIN_NUM                       = MINIMUM_REQUIRED_RESIN_NUM
   ,MM_RESIN_NUM                       = NUMBER_OF_CHAMBERS

} TChamberIndex;


/*//////////////////////////////////////////////////////////////////////////////
RESIN TYPES
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
   TYPE_RESIN_SUPPORT
  ,TYPE_RESIN_MODEL
  ,NUMBER_OF_RESIN_TYPES
}TResinTypeIndex;

extern QString ResinTypeDescription[NUMBER_OF_RESIN_TYPES];

/*//////////////////////////////////////////////////////////////////////////////
TANKS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    TYPE_TANK_SUPPORT1              = 0 // left  support
#if defined OBJET_FOUR_TANK_MACHINE || defined OBJET_MACHINE			// right support (yes, it is inverted relatevely to the GUI and the labels on the physical machine compartments)
    ,TYPE_TANK_SUPPORT2
#endif
    ,TYPE_TANK_MODEL1					// left  model
#if defined OBJET_FOUR_TANK_MACHINE || defined OBJET_MACHINE
    ,TYPE_TANK_MODEL2					// right model
#endif
#ifdef OBJET_MACHINE
    ,TYPE_TANK_MODEL3
    ,TYPE_TANK_MODEL4
    ,TYPE_TANK_MODEL5
    ,TYPE_TANK_MODEL6
#endif
   ,TOTAL_NUMBER_OF_CONTAINERS
   ,NUMBER_OF_SUPPORT_TANKS         = TYPE_TANK_MODEL1
   ,NUMBER_OF_MODEL_TANKS           = TOTAL_NUMBER_OF_CONTAINERS - NUMBER_OF_SUPPORT_TANKS
   ,FIRST_TANK_TYPE                 = TYPE_TANK_SUPPORT1
   ,LAST_TANK_TYPE                  = TOTAL_NUMBER_OF_CONTAINERS
   ,FIRST_MODEL_TANK_TYPE           = TYPE_TANK_MODEL1
   ,LAST_MODEL_TANK_TYPE            = TOTAL_NUMBER_OF_CONTAINERS
   ,FIRST_SUPPORT_TANK_TYPE         = TYPE_TANK_SUPPORT1
   ,LAST_SUPPORT_TANK_TYPE          = NUMBER_OF_SUPPORT_TANKS
   ,TYPE_TANK_WASTE                 = TOTAL_NUMBER_OF_CONTAINERS
   ,TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE
   ,FIRST_TANK_INCLUDING_WASTE_TYPE = TYPE_TANK_SUPPORT1
   ,LAST_TANK_INCLUDING_WASTE_TYPE  = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE

   ,ACTIVE_TANK
   ,ALL_TANKS
   ,NO_TANK                         = (unsigned)(-1)
} TTankIndex;

////////////////////////////////////////////////////////////////////////////////
//THERMISTORS
////////////////////////////////////////////////////////////////////////////////
typedef enum{
  SUPPORT_CHAMBER_THERMISTOR,
  M1_CHAMBER_THERMISTOR,
  M2_CHAMBER_THERMISTOR,
#ifdef OBJET_MACHINE
  M3_CHAMBER_THERMISTOR,
  SUPPORT_M3_CHAMBER_THERMISTOR,
  M1_M2_CHAMBER_THERMISTOR,
#endif
  NUM_OF_CHAMBERS_THERMISTORS,
#ifdef OBJET_MACHINE
  NUM_OF_DM_CHAMBERS_THERMISTORS = SUPPORT_M3_CHAMBER_THERMISTOR,
  FIRST_SM_CHAMBER = NUM_OF_DM_CHAMBERS_THERMISTORS,
#else
  NUM_OF_DM_CHAMBERS_THERMISTORS = NUM_OF_CHAMBERS_THERMISTORS,
#endif
 NUM_OF_SM_CHAMBERS_THERMISTORS = NUM_OF_CHAMBERS_THERMISTORS-NUM_OF_DM_CHAMBERS_THERMISTORS
}CHAMBERS_THERMISTORS_EN;

typedef enum
{
  HIGH_THERMISTORS_MODE,
  LOW_THERMISTORS_MODE,
  SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE,
  SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE,
  NUM_OF_THERMISTORS_MODES
}TThermistorsOperationMode;

//Chamber tank relation is determined according to MRW 
const int ChamberTankRelationArr[][NUMBER_OF_CHAMBERS_INCLUDING_WASTE*TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] =
							{{1,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
							{1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
							{1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1},
							{1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
							{1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1},
							{1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1}};



typedef enum
{
  S_M1_ACTIVE_TANKS_MODE,
  S_M2_ACTIVE_TANKS_MODE,
  S_M1_M2_M3_ACTIVE_TANKS_MODE,
  S_M1_M2_ACTIVE_TANKS_MODE,
  S_M1_M3_ACTIVE_TANKS_MODE,
  S_M2_M3_ACTIVE_TANKS_MODE,
  NUM_OF_TANKS_OPERATION_MODES
}TTankOperationMode;


/*
//(0), S M1 active tanks
const char CHAMBER_TANK_RELATION[]      = {1,0,0,1,0,
										   1,0,0,1,0,
										   0,1,1,0,0,
										   0,1,1,0,0,
										   0,0,0,0,0,
										   0,0,0,0,0,
										   0,0,0,0,0,
										   0,0,0,0,0,
										   0,0,0,0,1}

//(1), S M2 active tanks
const char CHAMBER_TANK_RELATION[]      = {1,0,0,1,0,
										   1,0,0,1,0,
										   0,0,0,0,0,
										   0,0,0,0,0,
										   0,1,1,0,0,
										   0,1,1,0,0,
										   0,0,0,0,0,
										   0,0,0,0,0,
										   0,0,0,0,1}

//S M1 M2 M3 active tanks
const int ChamberTankRelationArr =   {1,0,0,0,0,
									  1,0,0,0,0,
									  0,1,0,0,0,
									  0,1,0,0,0,
									  0,0,1,0,0,
									  0,0,1,0,0,
									  0,0,0,1,0,
									  0,0,0,1,0,
									  0,0,0,0,1}


//S M1 M2 active tanks (3)                      C0,C1,C2,C3,W  (Tank)
const char CHAMBER_TANK_RELATION[]      = (1,0,0,1,0,
										   1,0,0,1,0,
										   0,1,0,0,0,
										   0,1,0,0,0,
										   0,0,1,0,0,
										   0,0,1,0,0,
										   0,0,0,0,0,
										   0,0,0,0,0,
										   0,0,0,0,1}

//S M1 M3 active tanks                    C0,C1,C2,C3,W  (Tank)
const char CHAMBER_TANK_RELATION[]      = {1,0,0,0,0,\     S1
										   1,0,0,0,0,\     S2
										   0,1,1,0,0,\     M1
										   0,1,1,0,0,\     M2
										   0,0,0,0,0,\     M3
										   0,0,0,0,0,\     M4
										   0,0,0,1,0,\     M5
										   0,0,0,1,0,\     M6
										   0,0,0,0,1}     W

//S M2 M3 active tanks			          C0,C1,C2,C3,W  (Tank)
const char CHAMBER_TANK_RELATION[]      = {1,0,0,0,0,\     S1
										   1,0,0,0,0,\     S2
										   0,0,0,0,0,\     M1
										   0,0,0,0,0,\     M2
										   0,1,1,0,0,\     M3
										   0,1,1,0,0,\     M4
										   0,0,0,1,0,\     M5
										   0,0,0,1,0,\     M6
										   0,0,0,0,1}     W


*/
#define VALIDATE_CHAMBER_THERMISTOR(ch_th) ValidateChamberThermistor(ch_th)

/*//////////////////////////////////////////////////////////////////////////////
VOLTAGE
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    LOW_VOLTAGE = 0
   ,HIGH_VOLTAGE
   ,NUMBER_OF_VOLTAGE_MODES
   ,FIRST_VOLTAGE_MODE = LOW_VOLTAGE
   ,LAST_VOLTAGE_MODE = NUMBER_OF_VOLTAGE_MODES
}TVoltageMode;

#define VALIDATE_VOLTAGE_MODE(vm) ((FIRST_VOLTAGE_MODE <= vm) && (LAST_VOLTAGE_MODE > vm))
#define FOR_ALL_VOLTAGE_MODES(vm) for(short vm = 0; vm < NUMBER_OF_VOLTAGE_MODES; vm++)

/*//////////////////////////////////////////////////////////////////////////////
HEADS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
   HEAD_0 = 0
  ,FIRST_HEAD = HEAD_0
  ,HEAD_1
  ,HEAD_2
  ,HEAD_3
#ifndef EDEN_250
  ,HEAD_4
  ,HEAD_5
  ,HEAD_6
  ,HEAD_7
#endif
  ,LAST_HEAD
  ,TOTAL_NUMBER_OF_HEADS = LAST_HEAD
  ,TOTAL_NUMBER_OF_HEADS_HEATERS = TOTAL_NUMBER_OF_HEADS
  ,BLOCK_0 = TOTAL_NUMBER_OF_HEADS
  ,FIRST_BLOCK = BLOCK_0 //PH front
  ,BLOCK_1				 //PH rear
  ,BLOCK_2               //rear block
  ,BLOCK_3 //11          //front block
#ifndef OBJET_MACHINE
  ,PRE_HEATER
#endif
  ,TOTAL_NUMBER_OF_HEATERS
  ,LAST_BLOCK = TOTAL_NUMBER_OF_HEATERS
  ,TOTAL_NUMBER_OF_BLOCKS_HEATERS = LAST_BLOCK - FIRST_BLOCK
  ,ALL_HEADS = TOTAL_NUMBER_OF_HEADS
} THeadIndex;

typedef enum   //runtime objet
{
  OFFICE_MACHINE,
  PROFESSIONAL_MACHINE
} OfficeProfessionalType;

typedef enum
{
   NUM_OF_UV_ON
  ,DELAY_BETWEEN_LAYERS
  ,NUM_OF_FLEXIBLE_PARAMETERS
} TFlexibleParametersEN;

typedef enum
{
  FIRST_MSC_CARD,
  SECOND_MSC_CARD,
  NUM_OF_MSC_CARDS
} MSC_CARD_INDEX;

#ifndef PARAMS_EXP_IMP /*{*/
 extern const int HeadsCounterID[TOTAL_NUMBER_OF_HEADS];
#endif
 
typedef THeadIndex THeaterIndex;

#if defined CONNEX_MACHINE || defined OBJET_MACHINE
extern const int mapYOffset[];
#endif
/*
#define FOR_ALL_TANKS(i)                      for(int i = LAST_TANK_TYPE-1; i >= FIRST_TANK_TYPE; i--)
#define FOR_ALL_TANKS_AND_INDEXED(i,j)        int i = LAST_TANK_TYPE-1, j = 0;for(;i >= FIRST_TANK_TYPE; i--, j++)
#define FOR_ALL_CHAMBERS(i)                   for(int i = LAST_CHAMBER_TYPE-1; i >= FIRST_CHAMBER_TYPE; i--)
#define FOR_ALL_CHAMBERS_AND_INDEXED(i,j)     int i = LAST_CHAMBER_TYPE-1, j = 0;for(;i >= FIRST_CHAMBER_TYPE; i--, j++)
#define FOR_ALL_SEGMENTS(i)                   for(int i = NUMBER_OF_SEGMENTS-1; i >= FIRST_SEGMENT; i--)
*/
//----Head To Chamber----------------------
extern const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS];


#ifndef PARAMS_EXP_IMP /*{*/

#if !defined (EDEN_250) && !defined (OBJET_MACHINE)
#define M0 HEAD_7
#define M1 HEAD_6
#define M2 HEAD_5
#define M3 HEAD_4
#define S0 HEAD_3
#define S1 HEAD_2
#define S2 HEAD_1
#define S3 HEAD_0
#elif defined (OBJET_MACHINE)  
#define M0 HEAD_7
#define M1 HEAD_6
#define M2 HEAD_5
#define M3 HEAD_4
#define M4 HEAD_3
#define M5 HEAD_2
#define S0 HEAD_1
#define S1 HEAD_0
#else
#define M0 HEAD_3
#define M1 HEAD_2
#define S0 HEAD_1
#define S1 HEAD_0
#endif

#define FIRST_SUPPORT_HEAD       FIRST_HEAD
#define LAST_SUPPORT_HEAD        (S0+1)
#define NUMBER_OF_SUPPORT_HEADS  (LAST_SUPPORT_HEAD-FIRST_SUPPORT_HEAD)

#define FIRST_MODEL_HEAD         (S0+1)
#define LAST_MODEL_HEAD          (M0+1)
#define NUMBER_OF_MODEL_HEADS    (TOTAL_NUMBER_OF_HEADS-NUMBER_OF_SUPPORT_HEADS)

#define VALIDATE_HEATER_INCLUDING_PREHEATER(h)  ValidateHeaterIncludingPreheater(h)

#define IS_SUPPORT_TANK(tank)   ((FIRST_SUPPORT_TANK_TYPE <= tank) && (LAST_SUPPORT_TANK_TYPE > tank))
#define IS_MODEL_TANK(tank)     ((FIRST_MODEL_TANK_TYPE <= tank)   && (LAST_MODEL_TANK_TYPE > tank))

#define IS_SUPPORT_CHAMBER(ch)  ((FIRST_SUPPORT_CHAMBER_INDEX <= ch) && (LAST_SUPPORT_CHAMBER_INDEX > ch))
#define IS_MODEL_CHAMBER(ch)    ((FIRST_MODEL_CHAMBER_INDEX <= ch)   && (LAST_MODEL_CHAMBER_INDEX > ch))

#define IS_SUPPORT_HEAD(head)   ((FIRST_SUPPORT_HEAD <= head) && (LAST_SUPPORT_HEAD > head))
#define IS_MODEL_HEAD(head)     ((FIRST_MODEL_HEAD <= head)   && (LAST_MODEL_HEAD > head))

#define IS_SUPPORT_PUMP(head)   ((FIRST_SUPPORT_PUMP <= head) && (LAST_SUPPORT_PUMP > head))
#define IS_MODEL_PUMP(head)     ((FIRST_MODEL_PUMP <= head)   && (LAST_MODEL_PUMP > head))

typedef std::vector<TTankIndex>    TTankIndexVector;
typedef TTankIndexVector::iterator TTankIndexVectorIter;

#define VALIDATE_CHAMBER(ch) ValidateChamber(ch)
#define VALIDATE_CHAMBER_INCLUDING_WASTE(ch) ((FIRST_CHAMBER_TYPE_INCLUDING_WASTE <= ch) && (LAST_CHAMBER_TYPE_INCLUDING_WASTE > ch))

#define VALIDATE_TANK(tank) ((FIRST_TANK_TYPE <= tank) && (LAST_TANK_TYPE > tank))
#define VALIDATE_TANK_INCLUDING_WASTE(tank) ((FIRST_TANK_INCLUDING_WASTE_TYPE <= tank) && (LAST_TANK_INCLUDING_WASTE_TYPE > tank))

#define MIN_LIQUID_WEIGHT 200/*gr*/
#define HSW_MIN_LIQUID_WEIGHT 500/*gr*/
#define VALIDATE_HEAD(head) ((0 <= head) && (TOTAL_NUMBER_OF_HEADS > head))
#define FOR_ALL_HEADS(head) for(short head = 0; head < TOTAL_NUMBER_OF_HEADS; head++)

extern const int DPI[NUMBER_OF_QUALITY_MODES];
extern const int MICRON_IN_HEAD_Y;

QString       TankToStr                 (TTankIndex          Tank);
/* Warning! Read Container's CContainerBase::UpdateChambers info first.
   Returns a chamber that is associated with the given tank, taking into
   consideration the current (dynamic) OPERATION MODE.
   Also, if no chamber is related to the tank
   (i.e. the physical chamber connected to the tank is being flooded,
   or the given tank is inactive) then NUMBER_OF_CHAMBERS is returned. */
TChamberIndex TankToChamber             (TTankIndex          Tank);
// Returns the physical chamber connected to the given tank, REGARDLESS of current operation mode
TChamberIndex TankToStaticChamber		(TTankIndex          Tank);
bool          IsSegmentRelatedToTank    (TSegmentIndex Segment, TTankIndex Tank);
//Checks if the tank is related to the Dynamic (!) chamber.
//i.e., if the chamber is flooded, then its physical tanks ARE not related to the chamber
bool          IsChamberRelatedToTank	(TChamberIndex Chamber, TTankIndex Tank);
bool          IsPumpRelatedToTank       (TPumpIndex Pump, TTankIndex Tank);
TChamberIndex GetHeadsChamber           (int Head,  int PrintingOperationMode, bool AsModeFunc = false);
TChamberIndex GetHeadsChamberForHSW     (int Head,  int PrintingOperationMode, bool AsModeFunc = false);
TChamberIndex GetSiblingChamber			(TChamberIndex Chamber);
TTankIndex    GetSiblingTank            (TTankIndex TankIndex);
CHAMBERS_THERMISTORS_EN GetTankRelatedHighThermistor(TTankIndex Tank);
CHAMBERS_THERMISTORS_EN GetTankRelatedLowThermistor(TTankIndex Tank);
bool 		  IsTankInSupportBlockHalf  (TTankIndex Tank);
bool 		  IsTankInModelBlockHalf    (TTankIndex Tank);
QString       ChamberToStr              (TChamberIndex Chamber, bool AsModeFunc = false, TOperationModeIndex Mode = SINGLE_MATERIAL_OPERATION_MODE);
QString       ChamberThermistorToStr(int ChamberThermistor);
QString       GetUVLampStr              (int/*TUVLampIndex*/       Lamp);
QString       GetQualityModeStr         (TQualityModeIndex   QualityMode);
QString       GetModeStr                (int QualityMode,    int OperationMode);
QString       GetModeShortStr           (int QualityMode,    int OperationMode);
bool          GetModeFromShortStr       (QString ShortStr, int& QualityMode, int& OperationMode);
bool          Is_6_ModelHeads           ();
bool          GetModeAccessibility      (int QualityMode,    int OperationMode);
int           GetActiveChambersNum      ();
int           GetHeadsNum				(); //Returns the number of heads installed in the machine
TTankIndex	  GetSpecificTank			(TChamberIndex Chamber, int Num); //Num is the occurence index, 1-based 

QString       GetHeaterStr                            (int Heater);
QString       GetHeadName                             (int Head);
QString       OperationModeToStr                      (int OperationMode);
TSegmentIndex GetTankAdjacentSegment                  (TTankIndex Tank);
int           OperationModeToBitmapsNums              (int OperationMode);
int           OperationModeToActiveTankNum            (int OperationMode);
TPumpIndex    TankToPump                              (TTankIndex TankIndex);
TTankIndex    PumpToTank                              (TPumpIndex PumpIndex);
//TanksOperationMode Dependant 
bool          IsChamberFlooded						  (TChamberIndex ChamberIndex);
TSegmentIndex TankToShortSegment                      (TTankIndex Tank);
#endif/*}*/

bool ValidateChamber(int Chamber);
bool ValidateHeaterIncludingPreheater(int h);
bool ValidateChamberThermistor(int ct);

#ifndef PARAMS_EXP_IMP /*{*/
int           ModelActiveContainersNum                (void);
int           SupportActiveContainersNum              (void);



#define MAX_FILE_PATH        _MAX_PATH
#define TFileNamesArraySIZE (MAX_FILE_PATH * NUMBER_OF_CHAMBERS)

typedef char*         TBMPFileName/*[MAX_FILE_PATH]*/;
typedef TBMPFileName* TFileNamesArray/*[NUMBER_OF_CHAMBERS]*/;

#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_MACHINE
#define INIT_MSG_COUNTER_ID_ARRAY(_FieldName_)  {\
    SUPPORT_2##_FieldName_##_COUNTER_ID, \
    SUPPORT_1##_FieldName_##_COUNTER_ID, \
    MODEL_2##_FieldName_##_COUNTER_ID, \
    MODEL_1##_FieldName_##_COUNTER_ID, \
    MODEL_2##_FieldName_##_COUNTER_ID, \
    MODEL_1##_FieldName_##_COUNTER_ID  \
    }
#elif defined OBJET_MACHINE
#define INIT_MSG_COUNTER_ID_ARRAY(_FieldName_)  {\
    SUPPORT_1##_FieldName_##_COUNTER_ID, \
    SUPPORT_2##_FieldName_##_COUNTER_ID, \
    MODEL_1##_FieldName_##_COUNTER_ID, \
    MODEL_2##_FieldName_##_COUNTER_ID, \
    MODEL_3##_FieldName_##_COUNTER_ID, \
    MODEL_4##_FieldName_##_COUNTER_ID, \
    MODEL_5##_FieldName_##_COUNTER_ID, \
    MODEL_6##_FieldName_##_COUNTER_ID  \
    }
#else
#define INIT_MSG_COUNTER_ID_ARRAY(_FieldName_)  {\
    SUPPORT_1##_FieldName_##_COUNTER_ID, \
    MODEL_1##_FieldName_##_COUNTER_ID  \
    }
#endif

#if defined(EDEN_500) || defined(EDEN_350_V) || defined CONNEX_MACHINE
#define INIT_MSG_VALUES_ARRAY(_Msg_,_FieldName_)  {\
    _Msg_->SupportTank2##_FieldName_,  \
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->ModelTank2##_FieldName_,    \
    _Msg_->ModelTank1##_FieldName_     \
    }
#elif defined OBJET_MACHINE
#define INIT_MSG_VALUES_ARRAY(_Msg_,_FieldName_)  {\
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->SupportTank2##_FieldName_,  \
    _Msg_->ModelTank1##_FieldName_,    \
    _Msg_->ModelTank2##_FieldName_,     \
    _Msg_->ModelTank3##_FieldName_,    \
    _Msg_->ModelTank4##_FieldName_,     \
    _Msg_->ModelTank5##_FieldName_,    \
    _Msg_->ModelTank6##_FieldName_     \
    }
#else
#define INIT_MSG_VALUES_ARRAY(_Msg_,_FieldName_) { \
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->ModelTank1##_FieldName_     \
  }
#endif

#if defined(EDEN_500) || defined(EDEN_350_V) || defined CONNEX_MACHINE
#define INIT_MSG_VALUES_W_ARRAY(_Msg_,_FieldName_) { \
    _Msg_->SupportTank2##_FieldName_,  \
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->ModelTank2##_FieldName_,    \
    _Msg_->ModelTank1##_FieldName_,    \
    _Msg_->WasteTank##_FieldName_      \
  }
#elif defined OBJET_MACHINE 
#define INIT_MSG_VALUES_W_ARRAY(_Msg_,_FieldName_) { \
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->SupportTank2##_FieldName_,  \
    _Msg_->ModelTank1##_FieldName_,    \
    _Msg_->ModelTank2##_FieldName_,     \
    _Msg_->ModelTank3##_FieldName_,    \
    _Msg_->ModelTank4##_FieldName_,    \
    _Msg_->ModelTank5##_FieldName_,    \
    _Msg_->ModelTank6##_FieldName_,     \
    _Msg_->WasteTank##_FieldName_      \
  }
#else
#define INIT_MSG_VALUES_ARRAY(_Msg_,_FieldName_) { \
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->ModelTank1##_FieldName_,    \
    _Msg_->WasteTank##_FieldName_      \
  }
#endif

class ESequenceCanceled : public EQException {
  public:
	ESequenceCanceled(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class EInvalidResult : public EQException {
  public:
	EInvalidResult(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CAppGlobalDefs : public CQComponent
{
   CAppGlobalDefs();

public:
   static void            Init  (void);
   static void            DeInit(void);
   static CAppGlobalDefs* Instance(void);

   //bug 6258
   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstModelTank); 
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastModelTank); 
   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstSupportTank); 
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastSupportTank); 
   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstTank); 
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastTank); 

   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstChamber);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastChamber);

   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstModelChamber);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastModelChamber);
   DEFINE_METHOD_1(CAppGlobalDefs,QString,GetChamberStr,int);
   DEFINE_METHOD_1(CAppGlobalDefs,TPumpIndex,GetChamberActivePumpID,int /* TChamberIndex */);
   DEFINE_METHOD_1(CAppGlobalDefs,int,GetChamberActivePumpActuatorID,int);  

   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstQualityMode);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastQualityMode);

   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstUserLevelOperationMode);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastUserLevelOperationMode);

   DEFINE_METHOD_2(CAppGlobalDefs,bool,GetAppModeAccessibility,int,int);
   DEFINE_METHOD_2(CAppGlobalDefs,QString,GetModeDirStr,int,int);
   DEFINE_METHOD_2(CAppGlobalDefs,QString,GetModeName,int,int);



private:
   static CAppGlobalDefs* m_Instance;
};

#endif/*}*/

#define TAG_TO_MODE(_Tag_, _QM_, _OM_) _OM_=_Tag_%100;_QM_=(_Tag_-_OM_)/100
#define MODE_TO_TAG(_Tag_, _QM_, _OM_) _Tag_ = _QM_*100+_OM_

const float MICRONS_TO_PIXELS_1200DPI = 1200/(1000*MM_PER_INCH);
const float PIXELS_TO_MICRONS_1200DPI = (1000*MM_PER_INCH)/1200;

const int MAX_DATE_SIZE = 32;

const int MS_IN_SEC = 1000;

float MilliGramsToGrams    (int          Weight);
int   GramsToMilliGrams    (float        Weight);
int   NanoGramsToMilliGrams(unsigned int Weight);


enum {	  SUP705,
		  SUP706,
		  NumOfSupportMaterials};

enum {SUP705_RESIN_ID = 705,
	  SUP706_RESIN_ID = 706,
	  SUP707_RESIN_ID = 707};

#endif/*}*/
