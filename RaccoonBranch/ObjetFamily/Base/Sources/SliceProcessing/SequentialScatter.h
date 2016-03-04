/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Random scatter implementation.                           *
 * Module Description:                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Rachel / Ran                                             *
 * Start date: 08/01/2002                                           *
 * Last upate: 26/10/2003                                           *
 ********************************************************************/

#ifndef _SEQUENCIAL_SCATTER_H_
#define _SEQUENCIAL_SCATTER_H_

#include "ScatterGenerator.h"
#define SEQUENCIAL_TABLE_LENGTH 1000 // should be >= then g_NozzlesInAllHeads

class CSequentialScatter : public CScatterGenerator {
  private:
    // Tables that contains the involved nozzles (one for odd numbers and one for even numbers) 
    int m_ScatterTable2[SEQUENCIAL_TABLE_LENGTH];
    int m_ScatterTable1[SEQUENCIAL_TABLE_LENGTH];

    // Current index in the scatter table
    int m_CurrentScatterIndex;

    int m_ScatterSize;

    int m_ScatterTableRefreshCounter;

    bool m_PreviousFromTable2;

    // Flag for indicating a change in the Y and H factors
    bool m_YHChanged;
    int m_FirstSliceTrayYOffset;
    bool m_FirstSlice;

    int m_PreviousH,m_PreviousY;

    TSliceSizeType m_FirstSliceSizeType;

    int GetNextScatterValue(void);

    // Shuffle the scatter table in random
    void ShuffleTable(int *Table,int TableSize);

    TSliceSizeType CalcTrayOffsetNext(int ModelHeight,int SliceYOffset,int& LowestScatter,int& HighestScatter);

  public:
    // Constructor
    CSequentialScatter(int HeadPrintWidth,int NozzlesSeqOffset);

    // Destructor
    ~CSequentialScatter(void);

    // Compute new scatter table (ovrride)
    void ScatterCompute(int MaxHeight,int SliceYOffset,bool ResumedJob);

    // Get next scatter value (YOffset is from the first slice (max height)) (ovrride)
    void ScatterGetNext(int SliceHeight,int SliceYOffset,int& NoOfPasses,
                        int& YScatterOffset,int& ScatterOnly, bool EvenOdd);

    int GetScatterSize();

    virtual int GetTrayYOffset(void) {
      return m_FirstSliceTrayYOffset;
    }
};

#endif

