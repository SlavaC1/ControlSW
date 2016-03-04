/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: Exception Handling base class.                           *
 * Module description: This module defines a base class for the QLib*
 *                     C++ exception handling hierarchy.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/07/2001                                           *
 * Last upate: 01/07/2001                                           *
 ********************************************************************/

#ifndef _Q_EXCEPTION_H_
#define _Q_EXCEPTION_H_

#include "QTypes.h"


// Base class for exceptions
class EQException {
  private:
    // Error message
    QString m_ErrMsg;

    // Error code
    TQErrCode m_ErrCode;

  public:
    // Default constructor
    EQException(void) {
      m_ErrMsg = "";
      m_ErrCode = 0;
    }

    // Constructor 1 - Error code only
    EQException(TQErrCode Code) {
      m_ErrMsg = "";
      m_ErrCode = Code;
    }

    // Constructor 2 - Error string (formatted) and error code (optional)
    EQException(const QString& ErrMsg,const TQErrCode ErrCode=0) {
      m_ErrMsg = ErrMsg;
      m_ErrCode = ErrCode;
    }

    // Return the error message
    QString& GetErrorMsg(void) {
      return m_ErrMsg;
    }

    // Return the error code
    TQErrCode GetErrorCode(void) {
      return m_ErrCode;
    }
};

#endif

