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
 * Start date: 29/08/2001                                           *
 * Last upate: 22/01/2003                                           *
 ********************************************************************/

#ifndef _LAYER_H_
#define _LAYER_H_

#include "QObject.h"
#include "LayerProcessDefs.h"


// Single layer
class CLayer : public CQObject
{
  private:
    // Internal layer information data
    PBYTE m_Buffer;
    unsigned m_BufferSize;
    int m_SliceNumber;
    int m_TotalNumCounter;
    int m_NumInSlice;
    unsigned m_NoOfPasses;
    int m_YPosition;
    TPrintScanDirection m_PrintScanDirection;
    unsigned m_PassMask;

    unsigned m_PassCounter;

    static unsigned m_ActualPassesCounter;
    static unsigned m_TotalPassesCounter;

  protected:
    // DPC related parameters block
    TDPCPCLayerParams m_DCPPCLayerParams;

  public:

    // Constructor (ownership transfer for the layer buffer)
    CLayer(PBYTE Buffer = 0,unsigned BufferSize = 0,int SliceNumber = 0,int TotalNumCounter = 0,
           int NumInSlice = 0,unsigned NoOfPasses = 0,int YPosition = 0,
           TPrintScanDirection PrintScanDirection = FORWARD_PRINT_SCAN);

    // Destructor
    ~CLayer(void);

    // Set the layer properties
    void Init(PBYTE Buffer,unsigned BufferSize,int SliceNumber,int TotalNumCounter,int NumInSlice,
              unsigned NoOfPasses,int YPosition,TPrintScanDirection PrintScanDirection);

    // Update the internal pass mask
    void CalculatePassMask(void);

    // Getters for the layer properties
    inline PBYTE GetBuffer(void)
    {
       return m_Buffer;
    }

    inline unsigned GetBufferSize(void)
    {
      return m_BufferSize;
    }

    inline int GetNumInSlice(void)
    {
      return m_NumInSlice;
    }

    inline int GetTotalNumCounter(void)
    {
      return m_TotalNumCounter;
    }

    inline unsigned GetNoOfPasses(void)
    {
      return m_NoOfPasses;
    }

    inline int GetYPosition(void)
    {
      return m_YPosition;
    }

    inline int GetSliceNumber(void)
    {
      return m_SliceNumber;
    }
    // DPC related parameters block
    TDPCPCLayerParams *GetDPCPCParams(void);
    TPrintScanDirection GetPrintScanDirection(void);
    unsigned GetPassMask(void);
    // Return the number of actual passes (not including empty passes)
    unsigned GetPassCounter(void);
    static void ClearTotalPassesCounter(void);
    static unsigned GetActualPassesCounter(void);
    static unsigned GetTotalPassesCounter(void);
};

class CEmptyLayer : public CLayer
{
public:
  CEmptyLayer(int SliceNumber);
};

#endif

