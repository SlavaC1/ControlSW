/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (message queue)                         *
 * Module Description: This class implement a simple low-level      *
 *                     cross-platform thread messages queue.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 15/07/2001                                           *
 * Last upate: 23/08/2001                                           *
 ********************************************************************/

#ifndef _Q_SIMPLE_QUEUE_H_
#define _Q_SIMPLE_QUEUE_H_

#include <vector>
#include "QComponent.h"

// The windows version is dependent on the services of the CQMessageQueue class
#ifdef OS_WINDOWS
#include "QMessageQueue.h"
#include "QSemaphore.h"
#include "QMutex.h"

#elif defined(OS_VXWORKS)

#include <msgQLib.h>
#include "QOSWrapper.h"
#endif


/* Implementaion notes
   -------------------
   This class implement a simple raw-data message queue. This queue is implemented by using
   special OS API or by using the more generic CQMessageQueue class.
*/


// Exception class for the CQSimpleQueue class
class EQSimpleQueue : public EQOSWrapper {
  public:
    EQSimpleQueue(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQOSWrapper(ErrMsg,ErrCode) {}
};

// Cross-Platform raw-data message queue class
class CQSimpleQueue : public CQComponent {
  private:

#ifdef OS_WINDOWS
    // Type for a single message queue block
    typedef struct {
      // Pointer to a buffer
      PVOID Buffer;

      // The length of the message in bytes
      unsigned MessageLength;

    } TSimpleQueueBlock;

    // Container of pointers to buffers
    typedef std::vector<PBYTE> TBuffersContainer;

    // Array of message queue buffers
    TBuffersContainer m_Buffers;

    // Mutex for protecting access to the buffers array
    CQSemaphore m_BuffersSemaphore;

    // Index to an empty buffer slot
    int m_ReadyBufferIndex;

    unsigned m_MaxMessageSize;

    // A mutex to protect the buffer and the index
    CQMutex m_ReadyBufferMutex;

    // A mutex to prevent simultaneous flush and receive
    CQMutex m_FlushReceiveMutex;

    CQMessageQueue<TSimpleQueueBlock> m_QueueHolder;

#elif defined(OS_VXWORKS)
    unsigned m_QueueSize;
    unsigned m_MaxMessageSize;
    MSG_Q_ID m_QID;

#endif

    // Get/Set functions for the QueueSize property
    unsigned GetQueueSize(void);
    void SetQueueSize(unsigned) {}

    // Get/Set functions for the ItemsCount property
    unsigned GetItemsCount(void);
    void SetItemsCount(unsigned) {}

    // Get/Set functions for the MaxItemsCount property    
    void SetMaxItemsCount(unsigned Value);
    unsigned GetMaxItemsCount(void);    

  public:
    // Constructor
    CQSimpleQueue(unsigned MaxQueueSize,unsigned MaxMessageSize,
                  const QString Name = "MessageQueue",
                  bool RosterRegistration = true);

    // Destructor
    ~CQSimpleQueue(void);

    // Property for the queue size
    DEFINE_PROPERTY(CQSimpleQueue,unsigned,QueueSize);

    // Property for the current items count
    DEFINE_PROPERTY(CQSimpleQueue,unsigned,ItemsCount);

    // Property for the maximum items count
    DEFINE_PROPERTY(CQSimpleQueue,unsigned,MaxItemsCount);

    // Clear the queue content
    DEFINE_METHOD(CQSimpleQueue,TQErrCode,Flush);

    // Send a message to the queue
    QLib::TQWaitResult Send(PVOID Data,unsigned DataSize,TQWaitTime Timeout = Q_INFINITE_TIME);

    // Send an urgent message (to the top of the queue)
    QLib::TQWaitResult SendUrgent(PVOID Data,unsigned DataSize,TQWaitTime Timeout = Q_INFINITE_TIME);

    // Get a message from the queue
    QLib::TQWaitResult Receive(PVOID Data,unsigned DataSize,unsigned *ActualMessageLength = NULL,
                         TQWaitTime Timeout = Q_INFINITE_TIME);

    // Release waiting threads (the Send / Receivce functions return with the "wrReleased" code)
    void Release(void);

    // Get an handle to the internal OS object (VxWorks only)
#ifdef OS_VXWORKS
    MSG_Q_ID Handle(void) {
      return m_QID;
    }
#endif
};

#endif

