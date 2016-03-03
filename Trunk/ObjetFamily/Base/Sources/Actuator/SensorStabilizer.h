/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Stabilizer for Sensors ON 8051 h/w                       *
 * Module Description: This class implement stabilization for       *
 *                     services related to the sensors on 0\8051 h/w*
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Iddan Kalo                                               *
 * Start date: 31/Jul/14                                            *
 ********************************************************************/

#ifndef _SENSOR_STABILIZER_H_
#define _SENSOR_STABILIZER_H_

#include "FixedSizeQueue.h"
#include "QMutex.h"

class CSensorStabilizer : protected CFixedSizeQueue
{
  private:
    unsigned int m_TrueCounter;
    bool         m_LastComputedStatus;

  protected:
//    CQMutex m_MutexWndStatus;

    //Computes the status based on NumOfSamples (0 = all history size)
    void CalcHistoryStatus();

  public:
    // c-tor/d-tor
    CSensorStabilizer(unsigned int HistorySize, bool startVal);
    virtual ~CSensorStabilizer();

    //Inserts a new data to the moving window and returns
    //the gliding status considering the entire window (same as GetHistoryStatus)
    int UpdateHistory(bool Status);

    //This is the last computed "gliding" status
    bool GetHistoryLastStatus() { return m_LastComputedStatus; }
};


#endif /* _SENSOR_STABILIZER_H_ */
