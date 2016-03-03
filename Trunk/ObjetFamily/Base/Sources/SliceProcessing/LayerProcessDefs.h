/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Layers process defintions                                *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel                                                   *
 * Start date: 08/01/2002                                           *
 * Last upate: 18/12/2002                                           *
 ********************************************************************/

#ifndef _LAYER_PROCESS_DEF_H_
#define _LAYER_PROCESS_DEF_H_

#include "GlobalDefs.h"

// 65.2541 * 300 / 25.4 in 300 DPI
//Total number of heads
const int NUM_OF_LOGICAL_HEADS = 8;

//Number of bytes for a single head
const int NUM_OF_BYTES_SINGLE_HEAD    = (NOZZLES_PER_HEAD/8);// (192/8bit=24 (in KESHET))

//Number of bytes to hold one fire
const int NUM_OF_BYTES_SINGLE_FIRE     = (NUM_OF_BYTES_SINGLE_HEAD * NUM_OF_LOGICAL_HEADS); // 24*8=192
#ifdef OBJET_MACHINE_KESHET
const float LOGICAL_HEAD_PRINT_Y_DPI = 75; //DPI
#else
const float LOGICAL_HEAD_PRINT_Y_DPI = 37.5;
#endif

const int 	NOZZLES_SEQ_OFFSET_300_DPI = 4;//
const int   HEAD_PRINT_WIDTH_300_DPI  = (NOZZLES_PER_HEAD * NOZZLES_SEQ_OFFSET_300_DPI);  // 192 Nozzles in 75 DPI = (x4) = 768 Nozzles in 300 DPI   (300/75 = 4)

// Scatter options
typedef int TScatterAlgorithm;
const int SEQUENTIAL_SCATTER = 0;
const int RANDOM_SCATTER     = 1;

// Print directions
typedef int TPrintDirection;
const int BIDIRECTION_PRINT_MODE          = 0;
const int UNDIRECTION_FORWARD_PRINT_MODE  = 1;
const int UNDIRECTION_BACKWARD_PRINT_MODE = 2;

//Sequencer Type
const int ALTERNATE_SEQUENCER           = 3;
const int ALTERNATE_600_DPI_SEQUENCER   = 4;
const int FOUR_HEADS_300_X_2_SEQUENCER  = 5;
const int EIGHT_HEADS_300_X_2_SEQUENCER = 6;

const int BOTH_SUPPORT_HEADS = 0;
const int LEFT_SUPPORT_HEAD = 1;
const int RIGHT_SUPPORT_HEAD = 2;

typedef struct
{
  int             PrintRes_X;
  int             PrintRes_Y;
  TPrintDirection PrintDirection;
  int             XCloneDistance;
  int             X_OffsetMax_1200DPI;
  int             AdvanceFire_1200DPI;
  int             LastHeadOffset_1200DPI;
  bool            CloneStateEnabled;
  float           LayerHeight_mm;
  int             YInterlaceNoOfPixels;
  int             HeadsNum;
  int             YSecondaryInterlaceNoOfPixels;
  int             SliceHeightInterlaceModifier;
} TPrintConfigParams;

typedef struct
{
  int StartOfPlot;
  int EndOfPlot;
  int NoOfFires; 
} TDPCPCLayerParams;

typedef struct
{
  int Offset_X_1200DPI;
  int Offset_Y; 
} THeadMapEntry;

typedef int TPrintScanDirection;
const int BACKWARD_PRINT_SCAN  = 0;
const int FORWARD_PRINT_SCAN   = 1;
const int ALTERNATE_PRINT_SCAN = 2;

const int TRAY_Y_START_OFFSET_MM = 0;


#endif

