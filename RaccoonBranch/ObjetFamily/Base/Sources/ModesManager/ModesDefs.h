/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project:            Eden 500                                     *
 * FileName:           ModesDefs.h                                  *
 * Module:             Modes definitions.                           *
 * Module Description: Heaters (Head,Tray,Ambient) related          *
 *                     definitions.                                 *
 *                                                                  *
 * Compilation:        Standard C++.                                *
 *                                                                  *
 * Author:             Oved Ben Zeev                                *
 * Start date:         07/05/2005                                   *
 * Last update:        05/05/2005                                   *
 ********************************************************************/

#ifndef _MODES_DEFS_H_
#define _MODES_DEFS_H_

#include "GlobalDefs.h"
#include "AppParams.h"
#include "FEResources.h"

////////////////////////////////////////////////////////////////
// Directories...
////////////////////////////////////////////////////////////////
QString GENERAL_MODES_DIR = LOAD_STRING(IDS_GENERAL_MODES);

#define MACHINE_QUALITY_MODES_DIR(qm, om)  (QReplaceSubStr(GetModeStr(qm,om), " ", "") + "Modes")
#define OPERATION_MODE_NAME(Mode)      ("~" + QReplaceSubStr(OperationModeToStr(Mode), " ", ""))

extern const QString MODEL_DEFAULT_MATERIALS_MODES_DIR;
extern const QString SUPPORT_DEFAULT_MATERIALS_MODES_DIR;

extern const QString MODEL_SERVICE_MATERIALS_MODES_DIR;
extern const QString SUPPORT_SERVICE_MATERIALS_MODES_DIR;

extern const QString DEFAULT_MODES_DIR;

////////////////////////////////////////////////////////////////
// Modes...
////////////////////////////////////////////////////////////////
QString HEAD_ALIGNMENT_MODE = LOAD_STRING(IDS_XO_WIZARD);
QString MRW_ECONOMY_MODE    = LOAD_STRING(IDS_MRW_ECONOMY_MODENAME);
QString MRW_HIGHPERF_MODE   = LOAD_STRING(IDS_MRW_HIGHPERF_MODENAME);

extern QString PER_MACHINE_MODE  [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
extern QString PRINT_MODE        [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
extern QString WEIGHT_TEST       [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
extern QString CALIBRATE_FOR_HSW [NUMBER_OF_QUALITY_MODES];
extern QString HSW_WEIGHT_TEST   [NUMBER_OF_QUALITY_MODES];


extern QString MODEL_MATERIALS_MODES_DIR   [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
extern QString SUPPORT_MATERIALS_MODES_DIR [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];

extern const QString DEFAULT_MODE;
extern const QString DEFAULT_MATERIALS_MODE;
extern const QString PER_MACHINE_MODE_DEFAULT;

////////////////////////////////////////////////////////////////
// Modes...
////////////////////////////////////////////////////////////////
#define IS_MODE_HIDDEN(_ModeName_) (_ModeName_[0] == '~')

#endif

