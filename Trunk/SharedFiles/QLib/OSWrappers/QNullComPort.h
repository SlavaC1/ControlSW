/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (ComPort - empty implementation)        *
 * Module Description: Empty implementation of a serial com port    *
 *                     intended mainly for debug.                   *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _Q_NULL_COM_PORT_H_
#define _Q_NULL_COM_PORT_H_

#include "QComPort.h"
#include "QEvent.h"
#include "QThreadUtils.h"


// Com port class implementation for the standard ports
class CQNullComPort : public CQBaseComPort {
  private:
    // This "handle" is used to detect access to the port when not initialized
    bool m_Handle;

    // Com port settings
    int m_ComNum;
    int m_BaudRate;
    char m_Parity;
    int m_DataBits;
    int m_StopBits;

    // Flag indicating that the send function should output to the monitor
    bool m_SendOutputToMonitor;

    // The current read timeouts
    TQWaitTime m_ReadTimeoutInMs;

    // Event used for read timeout emulation
    CQEvent m_TimeoutEvent;

  public:
    // Default constructor
    CQNullComPort(void);

    // Initialization constructor
    CQNullComPort(int ComNum,int BaudRate = 9600,char Parity = 'N',int DataBits = 8,int StopBits = 1);

    // Destructor
    ~CQNullComPort(void);

    // Implementation functions
    // ------------------------

    // Write data to the com port
    ULONG Write(void *Data,unsigned DataLength);

    // Read data from the com port
    ULONG Read(void *Data,unsigned DataLength);

    // Set the com port read timeout in ms
    void SetReadTimeout(ULONG TimeoutInMs);

    // Flush input and output buffers
    void Flush(void);

    // Init function for the common properties
    void Init(int ComNum,int BaudRate = 9600,char Parity = 'N',int DataBits = 8,int StopBits = 1);

    // Get the current com port settings, return true if the com port is open
    bool GetComSettings(int& ComNum,int& BaudRate,char& Parity,int& DataBits,int& StopBits);

    // Close the port (deinit)
    void DeInit(void);

    // Different getters for the standard com port setting variables

    int GetComNum(void) {
      return m_ComNum;
    }

    int GetBaudRate(void) {
      return m_BaudRate;
    }

    int GetParity(void) {
      return m_Parity;
    }

    int GetDataBits(void) {
      return m_DataBits;
    }

    int GetStopBits(void) {
      return m_StopBits;
    }

    // Other functions
    // ---------------

    void SetMonitorOutput(bool SendOutputToMonitor) {
      m_SendOutputToMonitor = SendOutputToMonitor;
    }
};

#endif

