/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (ComPort)                               *
 * Module Description: Cross platform serial com port class.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 27/02/2002                                           *
 ********************************************************************/

#ifndef _Q_COM_PORT_H_
#define _Q_COM_PORT_H_

#include "QObject.h"
#include "QOSWrapper.h"


// Exception class for all the com port classes
class EQComPort : public EQOSWrapper {
  public:
    EQComPort(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Interface base class for serial communication
class CQBaseComPort : public CQObject {
  public:
    // Write data to the com port, return the actual number of bytes read
    virtual ULONG Write(void *Data,unsigned DataLength) = 0;

    // Read data from the com port, return the actual number of bytes read
    virtual ULONG Read(void *Data,unsigned DataLength) = 0;

    // Set the com port read timeout in ms
    virtual void SetReadTimeout(ULONG TimeoutInMs) = 0;

    // Flush input and output buffers
    virtual void Flush(void) {};

    // Init function for the common properties
    virtual void Init(int ComNum,int BaudRate = 9600,char Parity = 'N',int DataBits = 8,int StopBits = 1) {};

    // Close the port (deinitialize)
    virtual void DeInit(void) {};

    // Get the current com port settings, return true if the com port is open
    virtual bool GetComSettings(int& ComNum,int& BaudRate,char& Parity,int& DataBits,int& StopBits) {
      return false;
    };

    // Different getters for the standard com port setting variables

    virtual int GetComNum(void) {
      return 0;
    }

    virtual int GetBaudRate(void) {
      return 0;
    }

    virtual int GetParity(void) {
      return 0;
    }

    virtual int GetDataBits(void) {
      return 0;
    }

    virtual int GetStopBits(void) {
      return 0;
    }
};

#endif
