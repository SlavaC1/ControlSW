/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (message queue)                         *
 * Module Description: This class implement the class CQSimpleQueue *
 *                     for VxWorks.                                 *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 15/07/2001                                           *
 * Last upate: 23/08/2001                                           *
 ********************************************************************/

#include "QSimpleQueue.h"
#include "QErrors.h"


// Constructor
CQSimpleQueue::CQSimpleQueue(unsigned MaxQueueSize,unsigned MaxMessageSize,const QString Name,
                             bool RosterRegistration)
  // Initialize the base class
  : CQComponent(Name,RosterRegistration)
{
  // Initialize properties and methods
  INIT_PROPERTY(CQSimpleQueue,QueueSize,SetQueueSize,GetQueueSize);
  INIT_PROPERTY(CQSimpleQueue,ItemsCount,SetItemsCount,GetItemsCount);
  INIT_PROPERTY(CQSimpleQueue,MaxItemsCount,SetMaxItemsCount,GetMaxItemsCount);
  INIT_METHOD(CQSimpleQueue,Flush);

  // Create the actual OS queue
  m_QID = msgQCreate(MaxQueueSize,MaxMessageSize,MSG_Q_FIFO);
  
  m_QueueSize = MaxQueueSize;
  m_MaxMessageSize = MaxMessageSize;

  // Check if error
  if(m_QID == NULL)
    throw EQSimpleQueue("Error creating message queue",errno);
}

// Destructor
CQSimpleQueue::~CQSimpleQueue(void)
{
  if(m_QID != NULL)
    if(msgQDelete(m_QID) == ERROR)
      throw EQSimpleQueue("Error deleting message queue",errno);
}

// Send a message to the queue
TQWaitResult CQSimpleQueue::Send(PVOID Data,unsigned DataSize,TQWaitTime Timeout)
{
  TOSErr RetCode = msgQSend(m_QID,(PCHAR)Data,DataSize,Timeout,MSG_PRI_NORMAL);

  // If error
  if(RetCode == ERROR)
  {
    switch(errno)
    {
      // We got timeout
      case S_objLib_OBJ_TIMEOUT:
        return wrTimeout;

      // The queue was released
      case S_objLib_OBJ_DELETED:
        return wrReleased;
    }

    // Some other error
    return wrError;
  }

  return wrSignaled;
}

// Send an urgent message (to the top of the queue)
TQWaitResult CQSimpleQueue::SendUrgent(PVOID Data,unsigned DataSize,TQWaitTime Timeout)
{
  TOSErr RetCode = msgQSend(m_QID,(PCHAR)Data,DataSize,Timeout,MSG_PRI_URGENT);

  // If error
  if(RetCode == ERROR)
  {
    switch(errno)
    {
      // We got timeout
      case S_objLib_OBJ_TIMEOUT:
        return wrTimeout;

      // The queue was released
      case S_objLib_OBJ_DELETED:
        return wrReleased;
    }

    // Some other error
    return wrError;
  }

  return wrSignaled;
}

// Get a message from the queue
TQWaitResult CQSimpleQueue::Receive(PVOID Data,unsigned DataSize,unsigned *ActualMessageLength,TQWaitTime Timeout)
{
  TOSErr RetCode = msgQReceive(m_QID,(PCHAR)Data,DataSize,Timeout);

  // If error
  if(RetCode == ERROR)
  {
    switch(errno)
    {
      // We got timeout
      case S_objLib_OBJ_TIMEOUT:
        return wrTimeout;

      // The queue was released
      case S_objLib_OBJ_DELETED:
        return wrReleased;
    }

    // Some other error
    return wrError;
  } else
    {
      // Return the actual message length
      if(ActualMessageLength != NULL)
        *ActualMessageLength = RetCode;
    }

  return wrSignaled;
}

// Clear the queue content
TQErrCode CQSimpleQueue::Flush(void)
{
  int ItemsCount = GetItemsCount();

  for(int i = 0; i < ItemsCount; i++)
    Receive(NULL,0);

  return Q_NO_ERROR;
}

// Release waiting threads (the Send / Receivce functions return with the "wrReleased" code)
void CQSimpleQueue::Release(void)
{
  if(m_QID != NULL)
  {
    // Delete the queue for release
    if(msgQDelete(m_QID) == ERROR)
      throw EQSimpleQueue("Error deleting message queue for release",errno);

    // Re-create the queue
    if((m_QID = msgQCreate(m_QueueSize,m_MaxMessageSize,MSG_Q_FIFO)) == NULL)
      throw EQSimpleQueue("Error creating message queue after release",errno);
  }
}

// Get the number of messages currently in the queue
unsigned CQSimpleQueue::GetItemsCount(void)
{
  int n = msgQNumMsgs(m_QID);

  if(n == ERROR)
    throw EQSimpleQueue("Can not get queue items count",errno);

  return (unsigned)n;
}

// Get the queue maximum size
unsigned CQSimpleQueue::GetQueueSize(void)
{
  return m_QueueSize;
}

// Get/Set functions for the MaxItemsCount property
void CQSimpleQueue::SetMaxItemsCount(unsigned /*Value*/)
{
  // Not implemented
}

unsigned CQSimpleQueue::GetMaxItemsCount(void)
{
  // Not implemented
  return 0;
}

