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

// 65.2541 * 300 / 25.4 in 300 DPI
// Number of nozzles in a single head
const int   NOZZLES_PER_HEAD          = 96;
const int   HEAD_PRINT_WIDTH_300_DPI  = 768;

// One head tick data in bytes
const int HEADS_FILL_SIZE_BYTE        = (24 * 4);

// Number of bytes in a single head
const int HEAD_SIZE_BYTE              = 12;

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

// Total number of heads
const int HEADS_NUM = 8;

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

const int TRAY_Y_LIMITS_IN_PIXELS_FOR_260_MACHINES = 3313;


#endif

