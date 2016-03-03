/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (TCP/IP ComPort)                        *
 * Module Description: This class emulate serial communication using*
 *                     TCP/IP client.                               *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/02/2002                                           *
 * Last upate: 27/02/2002                                           *
 ********************************************************************/

#ifndef _Q_TCP_IP_CLIENT_COM_PORT_H_
#define _Q_TCP_IP_CLIENT_COM_PORT_H_

#include "QComPort.h"
#include "QEvent.h"


class CQTcpIpClient;


// Interface base class for serial communication
class CQTcpIpClientComPort : public CQBaseComPort {
  private:
    TQWaitTime m_ReadTimeoutInTicks;

    CQTcpIpClient *m_TcpIpClient;

    CQEvent m_ReadReleaseEvent;
    bool TerminateFlag;

  public:
    CQTcpIpClientComPort(const QString& ServerAddress);

    // Destructor
    ~CQTcpIpClientComPort(void);

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
