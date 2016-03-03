/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Error handler.                                           *
 * Module Description: This class implement the upper level of error*
 *                     reporting. It is intended mainly for         *
 *                     asynchronous errors.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 08/10/2001                                           *
 * Last upate: 10/10/2001                                           *
 ********************************************************************/

#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_

#include "QThread.h"
#include "QMessageQueue.h"


// This structre is used to represent all the information related to
// single error.
class TErrorInfoBlock : public EQException {
private:
  int m_Param1,m_Param2;
public:
  // Default constructor
  TErrorInfoBlock(void) : EQException("",0) {
    m_Param1 = m_Param2 = 0;
  }

  TErrorInfoBlock(const QString& ErrMsg,const TQErrCode ErrCode = 0,
                  int Param1 = 0,int Param2 = 0) : 
      EQException(ErrMsg,ErrCode),m_Param1(Param1),m_Param2(Param2){}

  int GetParam1(void) const {
     return m_Param1;
  }
  int GetParam2(void) const {
     return m_Param2;
  }
};


// The error handler class is a static singleton
class CErrorHandler : private CQThread {
  private:
    // Type for an error observer callback function
    typedef void (*TErrorObserverCallback)(TErrorInfoBlock& ErrorInfo,TGenericCockie Cockie);

    // Error observer structure
    struct TErrorObserver {
      TErrorObserverCallback m_ObserverCallback;
      TGenericCockie m_Cockie;

      // Constructor
      TErrorObserver(TErrorObserverCallback ObserverCallback,TGenericCockie Cockie) {
        m_ObserverCallback = ObserverCallback;
        m_Cockie = Cockie;
      }

      bool operator != (TErrorObserverCallback ObserverCallback) const {
        return (ObserverCallback != m_ObserverCallback);
      }

	  bool operator == (TErrorObserverCallback ObserverCallback) const {
        return (ObserverCallback == m_ObserverCallback);
      }
    };

    // Type for a list of error observers
    typedef std::vector<TErrorObserver> TErrorObserversList;

    // Errors queue
    CQMessageQueue<TErrorInfoBlock> m_ErrorsQueue;

    // List of error callbacks (observers)
    TErrorObserversList m_ErrorObserversList;

    // Private constructor
    CErrorHandler(void);

    // Singleton instance (not pointer!)
    static CErrorHandler *m_SingletonInstance;

    // Thread execute function (override)
    void Execute(void);

  public:

    // Destructor
    ~CErrorHandler(void) {}

    static void Init(void);
    static void DeInit(void);

    // Return an instance to the global singleton object
    static CErrorHandler *Instance(void) {
      return m_SingletonInstance;
    }

    // Interface function for error handling (version 1)
    void ReportError(const QString& ErrMsg,const TQErrCode ErrCode = 0,
                     int Param1 = 0,int Param2 = 0);

    // Interface function for error handling (version 2)
    void ReportError(TErrorInfoBlock ErrorInfoBlock);

    // Add an error notification observer
    void AddObserver(TErrorObserverCallback Callback,TGenericCockie Cockie);

    // Remove an observer from the notification list
    void UnregisterObserver(TErrorObserverCallback Callback);
};

#endif

