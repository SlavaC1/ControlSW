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
 * Last upate: 27/01/2002                                           *
 ********************************************************************/

#include <algorithm>
#include "ErrorHandler.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "AppLogFile.h"


// Maximum number of messages in queue
const int MAX_PENDING_ERROR_MESAGGES = 10;


// Singleton instance
CErrorHandler *CErrorHandler::m_SingletonInstance = 0;


void CErrorHandler::Init(void)
{
    m_SingletonInstance = new CErrorHandler();
    m_SingletonInstance->Resume();
}

void CErrorHandler::DeInit(void)
{
    if(m_SingletonInstance)
        delete m_SingletonInstance;
}

// Constructor
CErrorHandler::CErrorHandler(void) : CQThread(true, "ErrorHandler"), m_ErrorsQueue(MAX_PENDING_ERROR_MESAGGES, "ErrorHandlerMessageQueue",false)
{
    Priority = Q_PRIORITY_VERY_HIGH;
}

// Thread execute function (override)
void CErrorHandler::Execute(void)
{
    TErrorInfoBlock ErrorInfo;

    try {
    while(!Terminated)
    {
        // Wait for an error message to come
        if(m_ErrorsQueue.Receive(ErrorInfo) == QLib::wrReleased)
            break;

        // Update all the observers
        for(TErrorObserversList::iterator i = m_ErrorObserversList.begin(); i != m_ErrorObserversList.end(); ++i)
            // Execute the callback
            (*i->m_ObserverCallback)(ErrorInfo,i->m_Cockie);
    }
    } catch(...) {
       CQLog::Write(LOG_TAG_GENERAL, "CErrorHandler::Execute - unexpected error");
       if (!Terminated)
         throw EQException("CErrorHandler::Execute - unexpected error");
    }
}

// Interface function for error handling (version 1)
void CErrorHandler::ReportError(const QString& ErrMsg,const TQErrCode ErrCode,int Param1,int Param2)
{
    // Add an error info block to the queue
    m_ErrorsQueue.Send(TErrorInfoBlock(ErrMsg,ErrCode,Param1,Param2));
}

// Interface function for error handling (version 2)
void CErrorHandler::ReportError(TErrorInfoBlock ErrorInfoBlock)
{
    // Add an error info block to the queue
    m_ErrorsQueue.Send(ErrorInfoBlock);
}

// Add an error notification observer
void CErrorHandler::AddObserver(TErrorObserverCallback Callback,TGenericCockie Cockie)
{
    m_ErrorObserversList.push_back(TErrorObserver(Callback,Cockie));
}

// Remove an observer from the notification list
void CErrorHandler::UnregisterObserver(TErrorObserverCallback Callback)
{
    m_ErrorObserversList.erase(std::find(m_ErrorObserversList.begin(),m_ErrorObserversList.end(),Callback));
}


