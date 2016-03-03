/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP ComPort)                        *
 * Module Description: This class emulate serial communication using*
 *                     TCP/IP server.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/02/2002                                           *
 * Last upate: 11/03/2002                                           *
 ********************************************************************/

#ifndef _Q_TCP_IP_SERVER_COM_PORT_H_
#define _Q_TCP_IP_SERVER_COM_PORT_H_

#include "QComPort.h"


class CQTcpIpServer;


// Interface base class for serial communication
class CQTcpIpServerComPort : public CQBaseComPort {
  private:
    TQWaitTime m_ReadTimeoutInTicks;

    CQTcpIpServer *m_TcpIpServer;

    bool TerminateFlag;

  public:
    // Initialization constructor (this initialization parameters are just for compatability with serial
    // com port initialization routines).
    CQTcpIpServerComPort(/*int ComNum = 0,int BaudRate = 0,char Parity = 0,int DataBits = 0,int StopBits = 0*/);

    // Destructor
    ~CQTcpIpServerComPort(void);

    // Implementation functions
    // ------------------------

    // Write data to the com port
    ULONG Write(void *Data,unsigned DataLength);

    // Read data from the com port
    ULONG Read(void *Data,unsigned DataLength);

    // Set the com port read timeout in ms
    void SetReadTimeout(ULONG TimeoutInMs);
};

#endif
