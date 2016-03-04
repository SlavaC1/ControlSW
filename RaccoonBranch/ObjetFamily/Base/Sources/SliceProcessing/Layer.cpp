/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Single layer information and handling.                   *
 * Module Description: Encapsulate all the information required for *
 *                     a single layer.                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 09/01/2002                                           *
 * Last upate: 18/06/2003                                           *
 ********************************************************************/

#include <stdlib.h> 
#include "Layer.h"


// Utility function for the pass mask calculation
static bool CheckIfBufferIsBlank(void *Arr,unsigned Size)
{
  unsigned TmpSize = Size >> 2;

  unsigned i;

  for(i = 0; i < TmpSize; i++)
  {
    if(*((unsigned long *)Arr)++)
      return false;
  }

  TmpSize = Size & 3;

  for(i = 0; i < TmpSize; i++)
  {
    if(*((unsigned char *)Arr)++)
      return false;
  }

  return true;
}

unsigned CLayer::m_TotalPassesCounter = 0;
unsigned CLayer::m_ActualPassesCounter = 0;

// Constructor
CLayer::CLayer(PBYTE Buffer,unsigned BufferSize,int SliceNumber,int TotalNumCounter,int NumInSlice,
               int NoOfPasses,int YPosition,TPrintScanDirection PrintScanDirection)
{
  Init(Buffer,BufferSize,SliceNumber,TotalNumCounter,NumInSlice,NoOfPasses,YPosition,PrintScanDirection);
}

// Destructor
CLayer::~CLayer(void)
{
}

// Set the layer properties
void CLayer::Init(PBYTE Buffer,unsigned BufferSize,int SliceNumber,int TotalNumCounter,int NumInSlice,
                  int NoOfPasses,int YPosition,TPrintScanDirection PrintScanDirection)
{
  m_Buffer = Buffer;
  m_BufferSize = BufferSize;
  m_SliceNumber = SliceNumber;
  m_TotalNumCounter = TotalNumCounter;
  m_NumInSlice = NumInSlice;
  m_NoOfPasses = NoOfPasses;
  m_YPosition = YPosition;
  m_PrintScanDirection = PrintScanDirection;
  m_PassMask = 0xffffffff;
  m_PassCounter = 0;
}

// Update the internal pass mask
void CLayer::CalculatePassMask(void)
{
  // Pointer pass step inside layer buffer
  int PassStep = m_BufferSize / m_NoOfPasses;
  BYTE *PassStart = m_Buffer;

  m_PassMask = 0;
  m_PassCounter = 0;

  for(int i = 0; i < m_NoOfPasses; i++)
  {
    if(!CheckIfBufferIsBlank(PassStart,PassStep))
    {
      m_PassMask |= 1 << i;
      m_PassCounter++;
    }

    PassStart += PassStep;
  }

  m_ActualPassesCounter += m_PassCounter;
  m_TotalPassesCounter += m_NoOfPasses;
}

// DPC related parameters block
TDPCPCLayerParams *CLayer::GetDPCPCParams(void)
{
  return &m_DCPPCLayerParams;
}

TPrintScanDirection CLayer::GetPrintScanDirection(void)
{
  return m_PrintScanDirection;
}

unsigned CLayer::GetPassMask(void)
{
  return m_PassMask;;
}

unsigned CLayer::GetPassCounter(void)
{
  return m_PassCounter;
}

void CLayer::ClearTotalPassesCounter(void)
{
  m_TotalPassesCounter = m_ActualPassesCounter = 0;
}

unsigned CLayer::GetActualPassesCounter(void)
{
  return m_ActualPassesCounter;
}

unsigned CLayer::GetTotalPassesCounter(void)
{
  return m_TotalPassesCounter;
}

CEmptyLayer::CEmptyLayer(int SliceNumber) : CLayer(NULL, 0, SliceNumber)
{}

