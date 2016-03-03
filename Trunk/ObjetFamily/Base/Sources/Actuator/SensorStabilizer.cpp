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

#include "AppLogFile.h"
#include "SensorStabilizer.h"

//  ==================================
//	======= CSensorStabilizer ========

//Init'ing history with HISTORY_MOVING_WND_SIZE times false values.
CSensorStabilizer::CSensorStabilizer(unsigned int HistorySize, bool startVal)
: CFixedSizeQueue(HistorySize, startVal), m_LastComputedStatus(startVal)
{
    m_TrueCounter = ((startVal) ? HistorySize : 0);
}


CSensorStabilizer::~CSensorStabilizer() {}


// this function is not re-entrant. Assuming it is only called from the "get sensors" callback
int CSensorStabilizer::UpdateHistory(bool Status)
{
    bool poppedStatus = Insert(Status); //update the cyclic array

    if (poppedStatus != Status)
    {
        //There's an actual change in the counters.
        //Note: There's no worry we would de/increase counter out-of-bounds,
        //      because m_HistoryWnd is a cyclic array of a fixed size.
        //      i.e. Values will get saturated automatically as long as they're init'd correctly.
        if (poppedStatus) --m_TrueCounter;
        else              ++m_TrueCounter; // only case left is (Status == true)
    }

    CalcHistoryStatus();
    if (!Status)
    { // should change this message if it becomes generic
        CQLog::Write(LOG_TAG_ROLLER_SUCTION_SYSTEM,
                     QFormatStr("Purge Bath Overflow: read %d (accumulated %d/%d, reporting %d)",
                                Status, (GetSize()-m_TrueCounter), GetSize(), m_LastComputedStatus));
    }

    return m_LastComputedStatus;
}


void CSensorStabilizer::CalcHistoryStatus()
{
    if (m_TrueCounter > (unsigned int)GetSize())
    {
        m_TrueCounter = GetSize();
    }
    if (m_TrueCounter == (unsigned int)GetSize())
    {
        m_LastComputedStatus = true;
    }
    else if (m_TrueCounter == 0)
    {
        m_LastComputedStatus = false;
    }
}

//	======= CSensorStabilizer (END) ==
//  =====================================

