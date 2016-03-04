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

#ifndef _RANDOM_SCATTER_H_
#define _RANDOM_SCATTER_H_

#include "ScatterGenerator.h"
#include "LayerProcessDefs.h"

const int SCATTER_TABLE_SIZE = 1000;


class CRandomScatter : public CScatterGenerator {
  private:
    // Tables that contains the involved nozzles (one for odd numbers and one for even numbers) 
    int m_ScatterTable2[SCATTER_TABLE_SIZE];
    int m_ScatterTable1[SCATTER_TABLE_SIZE];
    int m_SortedScatterTable[NOZZLES_PER_HEAD * 4]; // should actually be (someting like) NOZZLES_PER_MODEL_HEADS, we have at most 4 model heads. todo: generalize. 

    // Current index in the scatter table
    int m_CurrentScatterIndex;

    int m_ScatterTableRefreshCounter;

    bool m_PreviousFromTable2;

    // Flag for indicating a change in the Y and H factors
    bool m_YHChanged;
    int m_FirstSliceTrayYOffset;
    bool m_FirstSlice;

    int m_PreviousH,m_PreviousY;

    TSliceSizeType m_FirstSliceSizeType;

    int m_uniqueScatterValuesNum;

    bool m_dontShuffle;

    int GetNextScatterValue(void);

    // Shuffle the scatter table in random
    void ShuffleTable(int *Table,int TableSize);

    TSliceSizeType CalcTrayOffsetNext(int ModelHeight,int SliceYOffset,int& LowestScatter,int& HighestScatter);

    void KeepSortedScatterTable();
	int GetDefaultScatter(int MaxHeight);
    
  public:
    // Constructor
    CRandomScatter(int HeadPrintWidth,int NozzlesSeqOffset);

    // Destructor
    ~CRandomScatter(void);

    // Compute new scatter table (ovrride)
    void ScatterCompute(int MaxHeight,int SliceYOffset,bool ResumedJob);

    // Get next scatter value (YOffset is from the first slice (max height)) (ovrride)
    void ScatterGetNext(int SliceHeight,int SliceYOffset,int& NoOfPasses,
                        int& YScatterOffset,int& ScatterOnly, bool EvenOdd);

    virtual int GetTrayYOffset(void) {
      return m_FirstSliceTrayYOffset;
    }

    int* GetSortedScatterTable(unsigned int& TableSize);

    void DontShuffle() {m_dontShuffle = true;}
};

#endif

