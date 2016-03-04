/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (message queue)                         *
 * Module Description: This class implement a cross-platform thread *
 *                     messages queue.                              *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 15/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _Q_MESSAGE_QUEUE_H_
#define _Q_MESSAGE_QUEUE_H_

#include <deque>
#include "QComponent.h"
#include "QEvent.h"
#include "QMutex.h"
#include "QErrors.h"


/* Implementaion notes
   -------------------
   Because of substantial differances between OS implementations of messages API this queue class
   implementaion does not use OS specific messaging API.
*/

// Exception class for the CQMessageQueue class
class EQMessageQueue : public EQOSWrapper {
  public:
    EQMessageQueue(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};
// Cross-Platform message queue class

// "T" is the type of a single message queue item
template <class T>
class CQMessageQueue : public CQComponent {
  private:
    // Events for signaling queue status
    CQEvent m_NotEmptyEvent;
    CQEvent m_NotFullEvent;

    // Mutex for data structure acces protection
    CQMutex m_GuardMutex;

    // Type for queue data place holder
    typedef std::deque<T> TMsgQueueHolder;

    // The acuall data structure
    TMsgQueueHolder m_Data;

    // Queue size in messages
    unsigned m_QueueSize;

    // This flag is used to signal the special case when the waiting on queue send or receive is canceled
    bool m_ReleaseFlag;

    // Get/Set functions for the QueueSize property
    unsigned GetQueueSize(void);
    void SetQueueSize(unsigned);

    // Get/Set functions for the ItemsCount property
    unsigned GetItemsCount(void);
    void SetItemsCount(unsigned) {}

  public:
    // Constructor
    CQMessageQueue(unsigned MaxQueueSize,const QString& Name = "MessageQueue",
                   bool RosterRegistration = true) : CQComponent(Name,RosterRegistration) {

      // Remember the maximum allowed number of messages for this queue
      m_QueueSize = MaxQueueSize;

      m_ReleaseFlag = false;

      // Initialize properties and methods
      INIT_PROPERTY(CQMessageQueue,QueueSize,SetQueueSize,GetQueueSize);
      INIT_PROPERTY(CQMessageQueue,ItemsCount,SetItemsCount,GetItemsCount);
      INIT_VAR_PROPERTY(MaxItemsCount,0);
      INIT_METHOD(CQMessageQueue,Flush);
    }

    // Destructor
    ~CQMessageQueue(void) {
      Release();
    }

    // Property for the queue size
    DEFINE_PROPERTY(CQMessageQueue<T>,unsigned,QueueSize);

    // Property for the current items count
    DEFINE_PROPERTY(CQMessageQueue<T>,unsigned,ItemsCount);

    // Property for indicating that the thread is running
    DEFINE_VAR_PROPERTY(unsigned,MaxItemsCount);

    // Clear the queue content
    DEFINE_METHOD(CQMessageQueue<T>,int,Flush);

    // Send a message to the queue
    QLib::TQWaitResult Send(T Item,TQWaitTime Timeout = Q_INFINITE_TIME);

    // Send an urgent message (to the top of the queue)
    QLib::TQWaitResult SendUrgent(T Item,TQWaitTime Timeout = Q_INFINITE_TIME);

    // Send an urgent message (to the top of the queue) and clear space for it (if needed)
    QLib::TQWaitResult SendUrgentAndBreakInto(T Item, T& RemovedItem, TQWaitTime Timeout = Q_INFINITE_TIME);
    // Receive a message from the queue
    QLib::TQWaitResult Receive(T& Item,TQWaitTime Timeout = Q_INFINITE_TIME);

    // Release waiting threads (the Send / Receivce functions return with the "wrReleased" code)
    void Release(void);

    // Make sure that the queue is not in the released state
    void UnRelease(void);
};

// Put a message into the queue
template <class T>
QLib::TQWaitResult CQMessageQueue<T>::Send(T Item,TQWaitTime Timeout)
{
  for(;;)
  {
    // Wait for the guarding mutex
    m_GuardMutex.WaitFor();

    // Check if we can insert data into the queue
    if(m_Data.size() < m_QueueSize) {
      // Add the item
      m_Data.push_back(Item);

      // Update the maximum items counter
      MaxItemsCount = max((unsigned)MaxItemsCount,m_Data.size());
      break;
    }

    m_GuardMutex.Release();

    // Wait for notification that queue is not full
	if(m_NotFullEvent.WaitFor(Timeout) == QLib::wrTimeout)
      // If we got out with a timeout return immediately
	  return QLib::wrTimeout;

    // Check if we got the "release" signal
    if(m_ReleaseFlag) {
      // Clear the release flag for next time
      m_ReleaseFlag = false;
	  return QLib::wrReleased;
    }
  }

  // Notify that queue is not empty
  m_NotEmptyEvent.SetEvent();

  m_GuardMutex.Release();
  return QLib::wrSignaled;
}

// Send an urgent message (to the top of the queue)
template <class T>
QLib::TQWaitResult CQMessageQueue<T>::SendUrgent(T Item,TQWaitTime Timeout)
{
  for(;;)
  {
    // Wait for the guarding mutex
    m_GuardMutex.WaitFor();

    // Check if we can insert data into the queue
    if(m_Data.size() < m_QueueSize) {
      // Add the item in the front of queue
      m_Data.push_front(Item);

      // Update the maximum items counter
      MaxItemsCount = max((unsigned)MaxItemsCount,m_Data.size());
      break;
    }

    m_GuardMutex.Release();

    // Wait for notification that queue is not full
	if(m_NotFullEvent.WaitFor(Timeout) == QLib::wrTimeout)
      // If we got out with a timeout return immediately
	  return QLib::wrTimeout;

    // Check if we got the "release" signal
    if(m_ReleaseFlag) {
      // Clear the release flag for next time
      m_ReleaseFlag = false;
	  return QLib::wrReleased;
    }
  }

  // Notify that queue is not empty
  m_NotEmptyEvent.SetEvent();

  m_GuardMutex.Release();
  return QLib::wrSignaled;
}

// Send an urgent message (to the top of the queue) and clear space for it (if needed)
template <class T>
QLib::TQWaitResult CQMessageQueue<T>::SendUrgentAndBreakInto(T Item, T& RemovedItem, TQWaitTime Timeout)
{
  // Wait for the guarding mutex
  m_GuardMutex.WaitFor();

  // Check if we can insert data into the queue
  if(m_Data.size() < m_QueueSize)
  {
    // Add the item in the front of queue
    m_Data.push_front(Item);

    // Update the maximum items counter
    MaxItemsCount = max((unsigned)MaxItemsCount,m_Data.size());
  }
  else
  {
    // Reomve the first item from the queue to make space for the urgent item
    RemovedItem = *m_Data.begin();
    m_Data.pop_front();

    // Add the item in the front of queue
    m_Data.push_front(Item);
  }

  // Notify that queue is not empty
  m_NotEmptyEvent.SetEvent();

  m_GuardMutex.Release();
  return QLib::wrSignaled;
}
// Get a message from the queue
template <class T>
QLib::TQWaitResult CQMessageQueue<T>::Receive(T& Item,TQWaitTime Timeout)
{
  for(;;)
  {
    // Wait for the guarding mutex
    m_GuardMutex.WaitFor();

    // Try to get data from the queue
    if(!m_Data.empty()) {
      Item = *m_Data.begin();
      m_Data.pop_front();
      break;
    }

    // Release the mutex so other threads can put data
    m_GuardMutex.Release();

    // Wait for notification that queue is not empty
	if(m_NotEmptyEvent.WaitFor(Timeout) == QLib::wrTimeout)
      // If we got out with a timeout return immediately
      return QLib::wrTimeout;

    // Check if we got the "release" signal
    if(m_ReleaseFlag) {
      // Clear the release flag for next time
      m_ReleaseFlag = false;
	  return QLib::wrReleased;
    }
  }

  // Notify that queue is not full
  m_NotFullEvent.SetEvent();

  m_GuardMutex.Release();
  return QLib::wrSignaled;
}

// Clear the queue content
template <class T>
TQErrCode CQMessageQueue<T>::Flush(void)
{
  // Clear the data
  m_GuardMutex.WaitFor();
  m_Data.clear();

  // We are not full anymore
  m_NotFullEvent.SetEvent();

  m_GuardMutex.Release();

  return Q_NO_ERROR;
}

// Get the number of messages currently in the queue
template <class T>
unsigned CQMessageQueue<T>::GetItemsCount(void)
{
  unsigned Count;

  m_GuardMutex.WaitFor();
  Count = m_Data.size();
  m_GuardMutex.Release();

  return Count;
}

// Release waiting threads (the Send / Receivce functions return with the "wrReleased" code)
template <class T>
void CQMessageQueue<T>::Release(void)
{
  m_ReleaseFlag = true;
  m_NotFullEvent.SetEvent();
  m_NotEmptyEvent.SetEvent();
}

// Make sure that the queue is not in the released state
template <class T>
void CQMessageQueue<T>::UnRelease(void)
{
  m_ReleaseFlag = false;
  m_NotFullEvent.ResetEvent();
  m_NotEmptyEvent.ResetEvent();
}

// Set the queue size
template <class T>
void CQMessageQueue<T>::SetQueueSize(unsigned NewSize)
{
  m_GuardMutex.WaitFor();

  if (NewSize < m_Data.size())
  {
    m_GuardMutex.Release();
    throw EQMessageQueue("Message queue resize error - can not resize to a size lower than the current.");
  }

  unsigned PrevSize  = m_QueueSize;
  m_QueueSize = NewSize;

  // If a thread waits to insert message to the queue, set the 'm_NotFullEvent' event
  if (PrevSize == m_Data.size())
    m_NotFullEvent.SetEvent();

  m_GuardMutex.Release();

}
template <class T>
unsigned CQMessageQueue<T>::GetQueueSize(void)
{
  unsigned Size;

  m_GuardMutex.WaitFor();
  Size = m_QueueSize;
  m_GuardMutex.Release();

  return Size;
}
#endif

