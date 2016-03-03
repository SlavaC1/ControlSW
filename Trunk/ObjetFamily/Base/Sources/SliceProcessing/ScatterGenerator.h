/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Scatter handling base class.                              *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 21/08/2003                                           *
 ********************************************************************/

#ifndef _SCATTER_GENERATOR_H_
#define _SCATTER_GENERATOR_H_

#include "QObject.h"


typedef enum {sstSmall=0,sstMedium,sstLarge,sstLargeOnRelativeSmallTray} TSliceSizeType;


class CScatterGenerator : public CQObject
{
  private:
    int m_LowestScatter;
    int m_TrayYOffset;

    TSliceSizeType m_FirstSliceSizeType;

  protected:
    int  m_YHPrevious;
    int  m_DeltaH;
    int  m_MaxPassesNum;

    static int  s_InterlaceFrontDelta;
    static int  s_InterlaceRearDelta;
    static int  s_HeadPrintWidth;
    static long s_TrayStartYPosition;
    static int  s_EvenOddDelta;
    static long s_YMaxPosition;
    static long s_TotalTrayHeight;

    static int  s_NozzlesSeqOffset;
    static int  s_TrayYStartOffset;
    static int  s_SmallSliceMaxSize;
    static int  s_MediumSliceMaxSize;
    static int  s_LargeSliceMaxSize;
    static int  s_LargeSliceOnSmallTray;

    // Help function for calculating the tray offset and lowest/highest available scatter
    TSliceSizeType CalcTrayOffset(int ModelMaxHeight,bool ResumedJob,int& LowestScatter,int& HighestScatter);
    // Utility function for converting a Y value from nozzles to DPI related Y coordinate
    int ConvertNozzleToYDPI(int Y);

  public:

    static int  s_TrayRearOffset;
    static int  s_TrayFrontOffset;
    // Constructor
    CScatterGenerator();
    static void Init();
    // Destructor
    virtual ~CScatterGenerator(void);
    // Compute new scatter ranges
    virtual void ScatterCompute(int MaxHeight,int SliceYOffset,bool ResumedJob);
    // Get next scatter value (YOffset is a delta value from the first slice (max height))
    virtual void ScatterGetNext(int SliceHeight,int SliceYOffset,int& NoOfPasses,
                                int& YScatterOffset,int& ScatterOnly, bool EvenOdd);
    virtual int GetTrayYOffset(void);
    TSliceSizeType GetFirstSliceSizeType(void);
    int GetMaxPassesNum();
    static int GetTrayRearOffset();
    static int GetTrayFrontOffset();
    virtual int GetSmallSliceMaxSize();
    virtual int GetMediumSliceMaxSize();
    virtual int GetLargeOnSmallTraySliceMaxSize ();
    virtual int GetLargeSliceMaxSize();
};

#endif

