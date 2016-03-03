/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib parameters stream for disk files.                   *
 * Module Description: Implement the base class CQParamsStream for  *
 *                     standard files I/O.                          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 04/07/2001                                           *
 * Last upate: 23/08/2001                                           *
 ********************************************************************/

#ifndef _Q_MONITOR_PARAMS_STREAM_H_
#define _Q_MONITOR_PARAMS_STREAM_H_

#include "QParamsContainer.h"


class CQMonitorParamsStream : public CQParamsStream {
  public:
    // Start the write to the monitor
    void StartWrite(void);

    // End the write to the monitor
    void EndWrite(void);

    // Write a single parameter
    void WriteParameter(CQParameterBase *Param);

    // Read a single parameter
    void ReadParameter(CQParameterBase *Param);
};

#endif

