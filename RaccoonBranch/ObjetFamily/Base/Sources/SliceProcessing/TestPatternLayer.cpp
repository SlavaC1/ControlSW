/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Test pattern Printing                                    *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 14/02/2002                                           *
 ********************************************************************/

#include <stdlib.h> 
#include "AppParams.h"
#include "TestPatternLayer.h"


// Constructor
CTestPatternLayer::CTestPatternLayer (void) : CLayer()
{
  // Initialize a pattern test layer
  PrepareTestPattern();
}

// Destructor
CTestPatternLayer::~CTestPatternLayer(void)
{
  if(GetBuffer() != NULL)
    free(GetBuffer());
}

void CTestPatternLayer::PrepareTestPattern(void)
{
  int PrintFactor;
  CAppParams *ParamsMgr = CAppParams::Instance();

  if (ParamsMgr->TestPatternPrintBiderection)
     PrintFactor = 2;
  else
     PrintFactor = 1;

  // Compute the memory size needed for the layer
  int TestPatternSize = ParamsMgr->TestPatternNoOfFires *  HEADS_FILL_SIZE_BYTE * PrintFactor;

  // Allocate memmory for the layer
  PBYTE TestPatternBufferTmpPtr = (PBYTE)malloc(TestPatternSize);

  // Check allocation
  if(TestPatternBufferTmpPtr == 0)
    throw ETestPattern("No memory is available for test pattern buffer");

  PBYTE TmpPtr = TestPatternBufferTmpPtr;

  for (int j=0; j < (ParamsMgr->TestPatternNoOfFires * PrintFactor); j++)
  {
    for (int i=0; i<NOZZLES_IN_HEAD; i++,TmpPtr++)
      *TmpPtr = BYTE(QHexToInt(ParamsMgr->TestPatternData[i])) & 0xff;
  }

  Init(TestPatternBufferTmpPtr,TestPatternSize,0,0,0,1,0,FORWARD_PRINT_SCAN);

  m_DCPPCLayerParams.StartOfPlot = 0;
  
  m_DCPPCLayerParams.EndOfPlot = m_DCPPCLayerParams.StartOfPlot +
    ParamsMgr->TestPatternNoOfFires*1200/ParamsMgr->DPI_InXAxis + ParamsMgr->AdvanceFire_1200DPI;

  m_DCPPCLayerParams.NoOfFires = ParamsMgr->TestPatternNoOfFires;
}


