/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module:  Power Fail Handler Thread                                *
 * Module Description: Responsible for                                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Shahar Behagen                                           *
 * Start date: 23/12/2008                                           *
 * Last upate: 23/12/2008                                           *
 ********************************************************************/

#ifndef _POWER_FAIL_H_
#define _POWER_FAIL_H_

#include "QThread.h"
#include "QMessageQueue.h"

// Exception class for the CLayerProcess class
class EPowerFail : public EQException {
  public:
    EPowerFail(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CPowerFail : public CQThread {

  private:

    HANDLE m_hPipe;
    LPTSTR m_lpszPipename;
    LPTSTR m_lpszRequestMessage;
    LPTSTR m_lpszResponseMessage;
    LPTSTR m_lpszUnexpectedMessage;
    CQEvent m_StopWaitingForConnectionEvent;

    // Thread execute function (override)
    void Execute(void);
    void GetAnswerToRequest(LPTSTR chRequest, LPTSTR chReply, LPDWORD pchBytes);
    void StopWaitingForConnection();

    // Destructor
    ~CPowerFail();

  public:

    // Constructor
    CPowerFail();

    // Shutdown() should be called instead of the D'tor.
    void Shutdown();
};


#endif