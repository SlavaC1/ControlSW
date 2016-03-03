/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (ComPort - standrad)                    *
 * Module Description: Cross platform serial com port class         *
 *                     implementation for the standard ports.       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _Q_STD_COM_PORT_H_
#define _Q_STD_COM_PORT_H_

// OS specific headers
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include "QComPort.h"
#include "QEvent.h"
#include "QTimer.h"

// Define com port handle type
#ifdef OS_WINDOWS
typedef HANDLE TComPortHandle;
#elif defined(OS_VXWORKS)
typedef int TComPortHandle;
#endif


// Com port class implementation for the standard ports
class CQStdComPort : public CQBaseComPort {
  private:
    // OS handle to the com port
    TComPortHandle m_Handle;

    // Com port settings
    int m_ComNum;
    int m_BaudRate;
    char m_Parity;
    int m_DataBits;
    int m_StopBits;

    // Com port read timeout
    ULONG m_ReadTimeout;

#ifdef OS_WINDOWS
    OVERLAPPED m_ReadOverlapped,m_WriteOverlapped;
    CQEvent m_ReadEvent,m_WriteEvent;

#elif defined(OS_VXWORKS)

    // Timer used for read timeouts
    CQTimer m_ReadTimeoutTimer;

    // Cancel read operation
    void CancelRead(void);

    static void ReadTimeoutCallback(TGenericCockie Cockie);
#endif

  public:
    // Default constructor
    CQStdComPort(void);

    // Initialization constructor
    CQStdComPort(int ComNum,int BaudRate = 9600,char Parity = 'N',int DataBits = 8,int StopBits = 1);

    // Destructor
    ~CQStdComPort(void);

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

    TComPortHandle Handle(void) {
      return m_Handle;
    }
};

#endif

