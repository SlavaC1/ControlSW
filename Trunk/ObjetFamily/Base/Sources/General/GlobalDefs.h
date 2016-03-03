/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Global defintions.  - GlobalDefs.h                       *
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
#define DEL_LAST_X_CHARS(str,numChars)(str.erase(str.end() - (numChars), str.end()))

#define MODEL_CLEANSER "M.Cleanser"
#define SUPPORT_CLEANSER "S.Cleanser"

#define MM_PER_INCH    25.4
#define CM_PER_INCH    2.54
#define MICRON_IN_CM   10000

#ifdef OBJET_MACHINE_KESHET
#define NOZZLES_PER_HEAD 192 // Number of nozzles in a single head
#define DPI_Y_PER_HEAD   75 // Native resolution of a single head in Y
#else
#define NOZZLES_PER_HEAD 96
#define DPI_Y_PER_HEAD   37.5
#endif
//#endif
 
#define DIMArray(Array) (sizeof(Array)/sizeof(Array[0]))

#ifndef PARAMS_EXP_IMP /*{*/

#define CONVERT_HEAD_VPP_A2D_TO_VOLT(x) ((16.9863 * 2.4 * (x)) / 1024)

#define CONVERT_HEAD_POT_VALUE_TO_VOLTS(x)   (1.285 * ((22.1 + 1.2 + 0.91) / (0.91 + 1.2 * ((6.0 / 255.0) + ((237.0 / 255.0) * (1.0 - ((x) / 255.0)))))))
#define CONVERT_HEAD_VOLTAGE_TO_POT_VALUE(x) (255.0 - (255.0 * 255.0 / 237.0) * ((22.1 + 1.2 + 0.91) * (1.285 / ((x) * 1.2)) - (6.0 / 255.0) - (0.91 / 1.2)))

#define CONVERT_VPP_A2D_TO_VOLT(x) (21 * 2.4 * (x) / 4096)
#define CONVERT_24V_A2D_TO_VOLT(x) (11 * 2.4 * (x) / 4096)
#define CONVERT_VCC_A2D_TO_VOLT(x) (2.5 * 2.4 * (x) / 4096)
#define CONVERT_VDD_A2D_TO_VOLT(x) (5.75 * 2.4 * (x) / 4096)
#define CONVERT_3_3_A2D_TO_VOLT(x) (2 * 2.4 * (x) / 4096)
#define CONVERT_1_2_A2D_TO_VOLT(x) (2.4 * (x) / 4096)

typedef enum {UVLampCalibrationMode,
              UVIntensityEvaluationMode} TUVMode;

/*//////////////////////////////////////////////////////////////////////////////
ACTUATORS
//////////////////////////////////////////////////////////////////////////////*/
#define DEFINE_ACTUATOR(id, descStr, formPartStr) ACTUATOR_ID_##id,
#define DEFINE_ACTUATOR_SAME_VALUE(destID, srcID, descStr, formPartStr) ACTUATOR_ID_##destID=ACTUATOR_ID_##srcID,
#define DEFINE_SENSOR(id, formPartStr)
typedef enum
{
#include "ActuatorDefs.h"
   MAX_ACTUATOR_ID
}TActuatorIndex;
#undef DEFINE_ACTUATOR
#undef DEFINE_SENSOR
#undef DEFINE_ACTUATOR_SAME_VALUE

#define DEFINE_ACTUATOR(id, descStr, formPartStr)
#define DEFINE_ACTUATOR_SAME_VALUE(destID, srcID, descStr, formPartStr)
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
#undef DEFINE_ACTUATOR_SAME_VALUE
#undef DEFINE_SENSOR

#define ACTUATOR_ON  1
#define ACTUATOR_OFF 0

#endif/*}*/

//RF Readers:
enum {
   RFRDR1 = 0
  ,RFRDR2
#ifdef OBJET_MACHINE_KESHET
  ,RFRDR3
  ,RFRDR4
#endif
  ,NUM_OF_RF_READERS
};

/*//////////////////////////////////////////////////////////////////////////////
LAMPS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
	RIGHT_UV_LAMP = 0
   ,LEFT_UV_LAMP
   ,NUMBER_OF_UV_LAMPS
   ,ALL_UV_LAMPS = NUMBER_OF_UV_LAMPS
}TUVLampIndex;

#define VALIDATE_UV_LAMP(lamp) ((0 <= lamp) && (NUMBER_OF_UV_LAMPS > lamp))
#define FOR_ALL_UV_LAMPS(lamp)	  for(short lamp = 0; lamp < NUMBER_OF_UV_LAMPS; lamp++)
#define DEFINE_UV_ARRAY(_member_)  { RIGHT_UV_LAMP_##_member_,LEFT_UV_LAMP_##_member_ };

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


#define VALIDATE_OPERATION_MODE(opm)      ((FIRST_OPERATION_MODE <= opm) && (LAST_OPERATION_MODE > opm))
#define FOR_ALL_OPERATION_MODES(opm)	  for(short opm = FIRST_OPERATION_MODE; opm < LAST_OPERATION_MODE; opm++)
/*//////////////////////////////////////////////////////////////////////////////
QUALITY MODES
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    HS_INDEX  = 0
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
	SUPPORT_PUMP1
   ,SUPPORT_PUMP2
   ,MODEL1_PUMP
   ,MODEL2_PUMP
   ,MODEL3_PUMP
   ,MODEL4_PUMP
   ,MODEL5_PUMP
   ,MODEL6_PUMP
#ifdef OBJET_MACHINE_KESHET
   ,MODEL7_PUMP
   ,MODEL8_PUMP
   ,MODEL9_PUMP
   ,MODEL10_PUMP
   ,MODEL11_PUMP
   ,MODEL12_PUMP
   ,MODEL13_PUMP
   ,MODEL14_PUMP
#endif
   ,NUMBER_OF_PUMPS
   ,FIRST_SUPPORT_PUMP = SUPPORT_PUMP1
   ,FIRST_MODEL_PUMP = MODEL1_PUMP
   ,FIRST_PUMP              = FIRST_SUPPORT_PUMP
   ,LAST_SUPPORT_PUMP       = FIRST_MODEL_PUMP
   ,LAST_MODEL_PUMP         = NUMBER_OF_PUMPS
   ,NUMBER_OF_SUPPORT_PUMPS = LAST_SUPPORT_PUMP - FIRST_SUPPORT_PUMP
   ,NUMBER_OF_MODEL_PUMPS   = NUMBER_OF_PUMPS   - NUMBER_OF_SUPPORT_PUMPS
}TPumpIndex;

QString PumpIndexToDescription(TPumpIndex i);

/*//////////////////////////////////////////////////////////////////////////////
SEGMENTS
//////////////////////////////////////////////////////////////////////////////*/
typedef enum
{
    FIRST_SEGMENT
   ,FIRST_SUPPORT_SEGMENT = FIRST_SEGMENT
   ,S1_SEGMENT = FIRST_SUPPORT_SEGMENT
   ,S2_SEGMENT
   ,LAST_SUPPORT_SEGMENT = S2_SEGMENT
   ,FIRST_MODEL_SEGMENT
   ,MODEL1_SEGMENT = FIRST_MODEL_SEGMENT
   ,MODEL2_SEGMENT
   ,MODEL3_SEGMENT
   ,MODEL4_SEGMENT
   ,MODEL5_SEGMENT
   ,MODEL6_SEGMENT
#ifdef OBJET_MACHINE_KESHET
   ,MODEL7_SEGMENT
   ,MODEL8_SEGMENT
   ,MODEL9_SEGMENT
   ,MODEL10_SEGMENT
   ,MODEL11_SEGMENT
   ,MODEL12_SEGMENT
   ,MODEL13_SEGMENT
   ,MODEL14_SEGMENT
#endif
   ,FIRST_SHARED_SEGMENT
   ,M1_2_SEGMENT = FIRST_SHARED_SEGMENT /*First Model Top*/
   ,M3_4_SEGMENT            			/*Second Model Top*/
   ,M5_6_SEGMENT            			/*Third Model Top*/
#ifdef OBJET_MACHINE_KESHET
   ,M7_8_SEGMENT
   ,M9_10_SEGMENT
   ,M11_12_SEGMENT
   ,M13_14_SEGMENT
   ,LAST_MODEL_SEGMENT = M13_14_SEGMENT
   ,SUPPORT_MODEL_LONG = M13_14_SEGMENT
   ,SUPPORT_MODEL_SEGMENTL = MODEL13_SEGMENT
   ,SUPPORT_MODEL_SEGMENTR = MODEL14_SEGMENT
#else
   ,LAST_MODEL_SEGMENT = M5_6_SEGMENT
   ,SUPPORT_MODEL_SEGMENTL = MODEL5_SEGMENT
   ,SUPPORT_MODEL_SEGMENTR = MODEL6_SEGMENT
   ,SUPPORT_MODEL_LONG = M5_6_SEGMENT 
#endif
   ,LAST_SHARED_SEGMENT = LAST_MODEL_SEGMENT
   ,NUMBER_OF_SEGMENTS
}TSegmentIndex;

#ifndef PARAMS_EXP_IMP /*{*/
 extern const TActuatorIndex PumpsActuatorID[NUMBER_OF_PUMPS];

QString SegmentDescription[NUMBER_OF_SEGMENTS] =
{
    "SlL" // DM6
   ,"SlR" // DM6
   ,"M1L"
   ,"M1R"
   ,"M2L"
   ,"M2R"
   ,"M3L"
   ,"M3R"
#ifdef OBJET_MACHINE_KESHET
   ,"M4L"
   ,"M4R"
   ,"M5L"
   ,"M5R"
   ,"M6L"
   ,"M6R"
   ,"ShL"//DM6 changed from "M7L"
   ,"ShR"//DM6 changed from "M7R"
#endif
   ,"M1_LONG"
   ,"M2_LONG"
   ,"M3_LONG"
#ifdef OBJET_MACHINE_KESHET
   ,"M4_LONG"
   ,"M5_LONG"
   ,"M6_LONG"
   ,"S_LONG"//DM6 changed from "M7_LONG"
#endif   
   };


#endif /*}*/

/*//////////////////////////////////////////////////////////////////////////////
CHAMBERS
//////////////////////////////////////////////////////////////////////////////*/

#define TYPE_CHAMBER_COUNT_HI_THERMISTOR_MODE (NUMBER_OF_CHAMBERS/2)

typedef enum
{
	TYPE_CHAMBER_SUPPORT           = 0
   ,TYPE_CHAMBER_MODEL1
   ,FIRST_SUPPORT_CHAMBER_INDEX    = TYPE_CHAMBER_SUPPORT
   ,FIRST_MODEL_CHAMBER_INDEX      = TYPE_CHAMBER_MODEL1
   ,MAX_NUMBER_OF_SUPPORT_CHAMBERS = TYPE_CHAMBER_MODEL1
   ,LAST_SUPPORT_CHAMBER_INDEX     = MAX_NUMBER_OF_SUPPORT_CHAMBERS
   ,TYPE_CHAMBER_MODEL2
   ,TYPE_CHAMBER_MODEL3
#ifdef OBJET_MACHINE_KESHET
   ,TYPE_CHAMBER_MODEL4
   ,TYPE_CHAMBER_MODEL5
   ,TYPE_CHAMBER_MODEL6
   ,TYPE_CHAMBER_MODEL7
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
	 TYPE_TANK_SUPPORT1  = 0 // left  support
	,TYPE_TANK_SUPPORT2  // right support (yes, it is inverted relatevely to the GUI and the labels on the physical machine compartments)
	,TYPE_TANK_MODEL1					// left  model
    ,TYPE_TANK_MODEL2					// right model                                                                                   
    ,TYPE_TANK_MODEL3
    ,TYPE_TANK_MODEL4
	,TYPE_TANK_MODEL5
	,TYPE_TANK_MODEL6
#ifdef OBJET_MACHINE_KESHET
	,TYPE_TANK_MODEL7
	,TYPE_TANK_MODEL8
	,TYPE_TANK_MODEL9
	,TYPE_TANK_MODEL10
	,TYPE_TANK_MODEL11
	,TYPE_TANK_MODEL12
	,TYPE_TANK_MODEL13
	,TYPE_TANK_MODEL14
	,SUPPORT_MODEL_TANK_L = TYPE_TANK_MODEL13
	,SUPPORT_MODEL_TANK_R = TYPE_TANK_MODEL14
#else
	,SUPPORT_MODEL_TANK_L =  TYPE_TANK_MODEL5
	,SUPPORT_MODEL_TANK_R =  TYPE_TANK_MODEL6
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
	,TYPE_TANK_WASTE_LEFT            = TOTAL_NUMBER_OF_CONTAINERS
	,FIRST_WASTE_TANK_TYPE           = TYPE_TANK_WASTE_LEFT
	,TYPE_TANK_WASTE_RIGHT
	,TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE
	,FIRST_TANK_INCLUDING_WASTE_TYPE = TYPE_TANK_SUPPORT1
	,LAST_TANK_INCLUDING_WASTE_TYPE  = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE

	,ACTIVE_TANK
	,ALL_TANKS
	,NO_TANK   = (unsigned)(-1)
} TTankIndex;

////////////////////////////////////////////////////////////////////////////////
//THERMISTORS
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    /* low thermistors */
	SUPPORT_CHAMBER_THERMISTOR = TYPE_CHAMBER_SUPPORT,
	M1_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL1,
	M2_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL2,
	M3_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL3,
#ifdef OBJET_MACHINE_KESHET
	M4_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL4,
	M5_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL5,
	M6_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL6,
	M7_CHAMBER_THERMISTOR      = TYPE_CHAMBER_MODEL7,
#endif
    /* high thermistors */
	SUPPORT_MODEL_CHAMBER_THERMISTOR,
	M1_M2_CHAMBER_THERMISTOR,
#ifdef OBJET_MACHINE_KESHET
	M3_M4_CHAMBER_THERMISTOR,
	M5_M6_CHAMBER_THERMISTOR,
#endif	
	NUM_OF_CHAMBERS_THERMISTORS,
	NUM_OF_DM_CHAMBERS_THERMISTORS = SUPPORT_MODEL_CHAMBER_THERMISTOR,
	FIRST_SM_CHAMBER               = NUM_OF_DM_CHAMBERS_THERMISTORS,
	NUM_OF_SM_CHAMBERS_THERMISTORS = NUM_OF_CHAMBERS_THERMISTORS - NUM_OF_DM_CHAMBERS_THERMISTORS 
}CHAMBERS_THERMISTORS_EN;


typedef enum
{
  HIGH_THERMISTORS_MODE,                  //SM
  LOW_THERMISTORS_MODE,                   //DM3 / DM7(keshet) / DM6v2 (keshet)
  SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE,//DM2 / DM6(keshet) / DM6v2 (keshet)
  SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE,//DM2 / Undefined(keshet)
  //Add modes here in case KESHET works with additional modes besides DM7 and DM3
  HIGH_AND_LOW_THERMISTORS_MODE, /* another mode for 3 materials HS in keshet keep ThermistorSetOpMode() [OHDB] updated as well */
  NUM_OF_THERMISTORS_MODES
}TThermistorsOperationMode;


#ifdef OBJET_MACHINE_KESHET
const int ChamberTankRelationArr[][NUMBER_OF_CHAMBERS_INCLUDING_WASTE*TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] =
//                S1                S2                M1                M2                M3                M4                M5                M6                M7                M8                M9                M10               M11               M12               M13               M14               W1                W2
//             C- 0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W,0,1,2,3,4,5,6,7,W
				{{1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M1_M3_M5_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M1_M3_M6_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M1_M4_M5_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M1_M4_M6_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M2_M3_M5_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M2_M3_M6_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M2_M4_M5_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}, // S_M2_M4_M6_ACTIVE_TANKS_MODE // DM6v2
				 {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1}};// S_M1_M7_ACTIVE_TANKS_MODE    // DM6v2

typedef enum
{
  S_M1_M3_M5_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M1_M3_M6_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M1_M4_M5_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M1_M4_M6_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M2_M3_M5_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M2_M3_M6_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M2_M4_M5_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M2_M4_M6_ACTIVE_TANKS_MODE,      //4 High thermistors (DM3)
  S_M1_M7_ACTIVE_TANKS_MODE, //8 lower thermistors only, all tanks are active - S & M1-M7, (DM7)
  NUM_OF_TANKS_OPERATION_MODES
}TTankOperationMode;

const bool Chamber2TankOperationModeRelation[NUM_OF_TANKS_OPERATION_MODES][NUMBER_OF_CHAMBERS] =
{//  S 1 2 3 4 5 6 7
	{1,1,0,1,0,1,0,1}, //S_M1_M3_M5_ACTIVE_TANKS_MODE
	{1,1,0,1,0,0,1,1}, //S_M1_M3_M6_ACTIVE_TANKS_MODE
	{1,1,0,0,1,1,0,1}, //S_M1_M4_M5_ACTIVE_TANKS_MODE
	{1,1,0,0,1,0,1,1}, //S_M1_M4_M6_ACTIVE_TANKS_MODE
	{1,0,1,1,0,1,0,1}, //S_M2_M3_M5_ACTIVE_TANKS_MODE
	{1,0,1,1,0,0,1,1}, //S_M2_M3_M6_ACTIVE_TANKS_MODE
	{1,0,1,0,1,1,0,1}, //S_M2_M4_M5_ACTIVE_TANKS_MODE
	{1,0,1,0,1,0,1,1}, //S_M2_M4_M6_ACTIVE_TANKS_MODE
	{1,1,1,1,1,1,1,1}, //S_M1_M7_ACTIVE_TANKS_MODE
};

#else
//Chamber tank relation is determined according to MRW
const int ChamberTankRelationArr[][NUMBER_OF_CHAMBERS_INCLUDING_WASTE*TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE] =
				{{1,0,0,1,0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
				 {1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
				 {1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1},
				 {1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
				 {1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1},
				 {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1}};

typedef enum
{
  S_M1_ACTIVE_TANKS_MODE,      //S-M1-M3-M5, High thermistors only
  S_M2_ACTIVE_TANKS_MODE,
  S_M1_M2_M3_ACTIVE_TANKS_MODE,//all tanks are active - S & M1-M7, (DM7), lower thermistors only
  S_M1_M2_ACTIVE_TANKS_MODE,
  S_M1_M3_ACTIVE_TANKS_MODE,
  S_M2_M3_ACTIVE_TANKS_MODE,
  NUM_OF_TANKS_OPERATION_MODES
}TTankOperationMode;

const bool Chamber2TankOperationModeRelation[NUM_OF_TANKS_OPERATION_MODES][NUMBER_OF_CHAMBERS] =
{//  S 1 2 3
	{1,1,0,0}, //S_M1_ACTIVE_TANKS_MODE
	{1,0,1,0}, //S_M2_ACTIVE_TANKS_MODE
	{1,1,1,1}, //S_M1_M2_M3_ACTIVE_TANKS_MODE
	{1,1,1,0}, //S_M1_M2_ACTIVE_TANKS_MODE
	{1,1,0,1}, //S_M1_M3_ACTIVE_TANKS_MODE
	{1,0,1,1}  //S_M2_M3_ACTIVE_TANKS_MODE
};
#endif

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

//S-M1-M3-M5 active tanks, High thermistors only
									  C0,C1,C2,C3,C4,C5,C6,C7,W
const int ChamberTankRelationArr =   {1 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0,        S1
									  1 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0,        S2
									  0 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0,        M1
									  0 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0,        M2
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        M3
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        M4
									  0 ,0 ,0 ,1 ,1 ,0 ,0 ,0 ,0,        M5
									  0 ,0 ,0 ,1 ,1 ,0 ,0 ,0 ,0,        M6
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        M7
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,		M8
									  0 ,0 ,0 ,0 ,0 ,1 ,1 ,0 ,0,		M9
									  0 ,0 ,0 ,0 ,0 ,1 ,1 ,0 ,0,		M10
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,		M11
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,		M12
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,		M13
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,		M14
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1,		W1
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1,		W2

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

//S M1-M7 active tanks, lower thermistors only
									  C0,C1,C2,C3,C4,C5,C6,C7,W
const int ChamberTankRelationArr =   {1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        S1
									  1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        S2
									  0 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        M1
									  0 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0,        M2
									  0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,0,        M3
									  0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,0,        M4
									  0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0,        M5
									  0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0,        M6
									  0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0,        M7
									  0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0,		M8
									  0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0,		M9
									  0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0,		M10
									  0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0,		M11
									  0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0,		M12
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0,		M13
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0,		M14
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1,		W1
									  0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1,		W2



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
  ,HEAD_4
  ,HEAD_5
  ,HEAD_6
  ,HEAD_7
  ,LAST_HEAD
  ,TOTAL_NUMBER_OF_HEADS = LAST_HEAD
  ,TOTAL_NUMBER_OF_HEADS_HEATERS = TOTAL_NUMBER_OF_HEADS
  ,BLOCK_0 = TOTAL_NUMBER_OF_HEADS
  ,FIRST_BLOCK = BLOCK_0 //PH front
  ,BLOCK_1				 //PH rear
  ,BLOCK_2               //rear block
  ,BLOCK_3 //11          //front block
//OBJET_MACHINE, PreHeater
 #ifdef OBJET_MACHINE_KESHET
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
  ,ROLLER_SPEED
  ,NUM_OF_FLEXIBLE_PARAMETERS
} TFlexibleParametersEN;

typedef enum
{
	MSC_CARD_1,
	MSC_CARD_2,
#ifdef OBJET_MACHINE_KESHET
	MSC_CARD_3,
	MSC_CARD_4,
#endif	
	NUM_OF_MSC_CARDS
} MSC_CARD_INDEX;

#ifndef PARAMS_EXP_IMP /*{*/
 extern const int HeadsCounterID[TOTAL_NUMBER_OF_HEADS];
  extern const int HeadsCounter_DC_ID[TOTAL_NUMBER_OF_HEADS];
  extern const int SleepingChannelsCounterID[MAX_NUMBER_OF_MODEL_CHAMBERS-1]; //size of (MAX_NUMBER_OF_MODEL_CHAMBERS-1) since MODEL7 is also support in DM6 and shouldn't be taken into account here
#endif
 
typedef THeadIndex THeaterIndex;

extern const int mapYOffset[];

//----Head To Chamber----------------------
extern const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS];


#ifndef PARAMS_EXP_IMP /*{*/

//OBJET_MACHINE
#define M0 HEAD_7
#define M1 HEAD_6
#define M2 HEAD_5
#define M3 HEAD_4
#define M4 HEAD_3
#define M5 HEAD_2
#define M6 HEAD_1
#define S0 HEAD_0

#define FIRST_SUPPORT_HEAD       FIRST_HEAD
#define LAST_SUPPORT_HEAD        (S0+1)
#define NUMBER_OF_SUPPORT_HEADS  (LAST_SUPPORT_HEAD-FIRST_SUPPORT_HEAD)

#define FIRST_MODEL_HEAD         (S0+1)
#define LAST_MODEL_HEAD          (M0+1)
#define NUMBER_OF_MODEL_HEADS    (TOTAL_NUMBER_OF_HEADS-NUMBER_OF_SUPPORT_HEADS)

#define VALIDATE_HEATER_INCLUDING_PREHEATER(h)  ValidateHeaterIncludingPreheater(h)

// DM6
#define IS_SUPPORT_TANK(tank)   (((FIRST_SUPPORT_TANK_TYPE <= tank) && (LAST_SUPPORT_TANK_TYPE > tank)) || \
                                 ((TYPE_TANK_MODEL13       <= tank) && (TYPE_TANK_MODEL14      >= tank))  )
#define IS_MODEL_TANK(tank)     ((FIRST_MODEL_TANK_TYPE <= tank)   && (TYPE_TANK_MODEL13 > tank))

// DM6
#define IS_SUPPORT_CHAMBER(ch)  (((FIRST_SUPPORT_CHAMBER_INDEX <= ch) && (LAST_SUPPORT_CHAMBER_INDEX > ch)) || \
                                 ((TYPE_CHAMBER_MODEL7 == ch))                                                 )
#define IS_MODEL_CHAMBER(ch)    ((FIRST_MODEL_CHAMBER_INDEX <= ch)   && (TYPE_CHAMBER_MODEL7 > ch))

#define IS_SUPPORT_HEAD(head)   ((FIRST_SUPPORT_HEAD <= head) && (LAST_SUPPORT_HEAD > head))
#define IS_MODEL_HEAD(head)     ((FIRST_MODEL_HEAD <= head)   && (LAST_MODEL_HEAD > head))

#define IS_SUPPORT_PUMP(head)   ((FIRST_SUPPORT_PUMP <= head) && (LAST_SUPPORT_PUMP > head))
#define IS_MODEL_PUMP(head)     ((FIRST_MODEL_PUMP <= head)   && (LAST_MODEL_PUMP > head))

typedef std::vector<TTankIndex>    TTankIndexVector;
typedef TTankIndexVector::iterator TTankIndexVectorIter;

#define VALIDATE_CHAMBER(ch) ValidateChamber(ch)
#define VALIDATE_CHAMBER_INCLUDING_WASTE(ch) ((FIRST_CHAMBER_TYPE_INCLUDING_WASTE <= ch) && (LAST_CHAMBER_TYPE_INCLUDING_WASTE > ch))
#define VALIDATE_CHAMBER_MODEL(ch) ((TYPE_CHAMBER_MODEL1 <= ch) && (NUMBER_OF_CHAMBERS > ch))

#define VALIDATE_TANK(tank) ((FIRST_TANK_TYPE <= tank) && (LAST_TANK_TYPE > tank))
#define VALIDATE_TANK_INCLUDING_WASTE(tank) ((FIRST_TANK_INCLUDING_WASTE_TYPE <= tank) && (LAST_TANK_INCLUDING_WASTE_TYPE > tank))

#define MIN_LIQUID_WEIGHT 200/*gr*/
#ifndef OBJET_MACHINE_KESHET
#define HSW_MIN_LIQUID_WEIGHT 500/*gr*/
#else
#define HSW_MIN_LIQUID_WEIGHT 200/*gr*/
#endif
#define VALIDATE_HEAD(head) ((0 <= head) && (TOTAL_NUMBER_OF_HEADS > head))
#define FOR_ALL_HEADS(head) for(short head = 0; head < TOTAL_NUMBER_OF_HEADS; head++)
#define FOR_ALL_DOUBLE_HEADS(head) for(short head = 0; head < TOTAL_NUMBER_OF_HEADS/2; head++)
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
TChamberIndex Head2BmpIndex             (int Head, int PrintingOperationMode, bool AsModeFunc = false);
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
TThermistorsOperationMode GetThermOpModeForHQHS(void);
bool          GetModeAccessibility      (int QualityMode,    int OperationMode);
int           GetActiveChambersNum      ();
int           GetHeadsNumPerMaterial    (); //Returns the number of heads installed in the machine
TTankIndex	  GetSpecificTank           (TChamberIndex Chamber, int Num); //Num is the occurence index, 1-based
char        * GetTankCleanserLabel      (TTankIndex aTank);

QString       GetHeaterStr                            (int Heater);
QString       GetHeadName                             (int Head);
QString       GetDoubleHeadName(int Head);
QString       OperationModeToStr                      (int OperationMode);
TSegmentIndex GetTankAdjacentSegment                  (TTankIndex Tank);
int           OperationModeToBitmapsNums              (int OperationMode);
int           OperationModeToActiveTankNum            (int OperationMode);
TPumpIndex    TankToPump                              (TTankIndex TankIndex);
TTankIndex    PumpToTank                              (TPumpIndex PumpIndex);
//TanksOperationMode Dependant 
bool          IsChamberFlooded						  (TChamberIndex ChamberIndex);
TSegmentIndex TankToShortSegment                      (TTankIndex Tank);
TSegmentIndex ChamberToLongSegment                    (TChamberIndex Chamber);
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

#ifndef OBJET_MACHINE_KESHET //xl, triplex
#define INIT_MSG_VALUES_W_ARRAY(_Msg_,_FieldName_) { \
    _Msg_->SupportTank1##_FieldName_,  \
    _Msg_->SupportTank2##_FieldName_,  \
    _Msg_->ModelTank1##_FieldName_,    \
	_Msg_->ModelTank2##_FieldName_,    \
	_Msg_->ModelTank3##_FieldName_,    \
	_Msg_->ModelTank4##_FieldName_,    \
	_Msg_->ModelTank5##_FieldName_,    \
	_Msg_->ModelTank6##_FieldName_,    \
	_Msg_->WasteTankLeft##_FieldName_, \
	_Msg_->WasteTankRight##_FieldName_ \
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
   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstThermistor);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastThermistor);

   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstChamber);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastChamber);

   DEFINE_METHOD  (CAppGlobalDefs,int,GetFirstModelChamber);
   DEFINE_METHOD  (CAppGlobalDefs,int,GetLastModelChamber);
   DEFINE_METHOD_1(CAppGlobalDefs,QString,GetChamberStr,int);
   DEFINE_METHOD_1(CAppGlobalDefs,QString,GetTankStr,int);
   DEFINE_METHOD_1(CAppGlobalDefs,TPumpIndex,GetChamberActivePumpID,int /* TChamberIndex */);
   DEFINE_METHOD_1(CAppGlobalDefs,int,GetChamberActivePumpActuatorID,int);  
   DEFINE_METHOD_1(CAppGlobalDefs,QString,GetChamberThermistorToStr,int);

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
enum {SUP705,
	  SUP706,
	  NumOfSupportMaterials};

enum {SUP705_RESIN_ID = 705,
	  SUP706_RESIN_ID = 706};


// If updated, need to update also in Keil Fans.h
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

#define OFF -1
#define NA	-2

#define RIGHT_FANS_MASK		0x01
#define LEFT_FANS_MASK		0x02
#define MIDDLE_FANS_MASK	0x04
#define MATERIAL_FANS_DUTY_CYCLE_100 	0xFF


#endif/*}*/
