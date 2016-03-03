/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (message queue)                         *
 * Module Description: This class implement the class CQSimpleQueue *
 *                     for windows.                                 *
 *                                                                  *
 * Compilation: Standard C++, Win32.                                *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 15/07/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#include <mem.h>
#include <assert.h>
#include "QSimpleQueue.h"


// Constructor
CQSimpleQueue::CQSimpleQueue(unsigned MaxQueueSize,unsigned MaxMessageSize,const QString Name,
                             bool RosterRegistration)
  // Initialize the base class
  : CQComponent(Name,RosterRegistration),

  // Initialize internal member objects
  m_QueueHolder(MaxQueueSize,"",false),

  // Set the protecting mutex to non-initial owner
  m_BuffersSemaphore(MaxQueueSize,MaxQueueSize)
{
  // Initialize properties and methods
  INIT_PROPERTY(CQSimpleQueue,QueueSize,SetQueueSize,GetQueueSize);
  INIT_PROPERTY(CQSimpleQueue,ItemsCount,SetItemsCount,GetItemsCount);
  INIT_PROPERTY(CQSimpleQueue,MaxItemsCount,SetMaxItemsCount,GetMaxItemsCount);
  INIT_METHOD(CQSimpleQueue,Flush);

  m_MaxMessageSize = MaxMessageSize;
  m_ReadyBufferIndex = 0;

  try
  {
    // Pre-allocate buffers container
    m_Buffers.resize(QueueSize);

    // Pre-allocate buffers
    for(TBuffersContainer::iterator i =  m_Buffers.begin(); i != m_Buffers.end(); ++i)
      *i = new BYTE[MaxMessageSize];

  } catch(...)
    {
      throw EQSimpleQueue("Can not pre-allocate buffers for queue");
    }
}

// Destructor
CQSimpleQueue::~CQSimpleQueue(void)
{
  // Dispose buffers
  for(TBuffersContainer::iterator i =  m_Buffers.begin(); i != m_Buffers.end(); ++i)
    delete [](*i);
}

// Put a message into the queue
QLib::TQWaitResult CQSimpleQueue::Send(PVOID Data,unsigned DataSize,TQWaitTime Timeout)
{
  // If the message is larger than maximum allowed size - make it the maximum size. 
  if (DataSize >= m_MaxMessageSize)
    DataSize = m_MaxMessageSize - 1;


  // Protect the buffers array with the semaphore
  QLib::TQWaitResult WaitResult = m_BuffersSemaphore.WaitFor(Timeout);
  if (WaitResult == QLib::wrSignaled)
  {
    m_ReadyBufferMutex.WaitFor();

    // Get a buffer
    PBYTE TmpBuffer = m_Buffers[m_ReadyBufferIndex];

    // Set the index for the next buffer
    m_ReadyBufferIndex = (m_ReadyBufferIndex + 1) % m_Buffers.size();

    m_ReadyBufferMutex.Release();

    // Copy the message content into the current available buffer and remember also the message length
    memcpy(TmpBuffer,Data,DataSize);

    // Prepare a message block structure (this is the actual data added to the queue)
    TSimpleQueueBlock MessageBlock = {TmpBuffer,DataSize};

    // Add to the queue
    m_QueueHolder.Send(MessageBlock);
  }
  return WaitResult;
}

// Send an urgent message (to the top of the queue)
QLib::TQWaitResult CQSimpleQueue::SendUrgent(PVOID Data,unsigned DataSize,TQWaitTime Timeout)
{
  assert(DataSize < m_MaxMessageSize);

  // Protect the buffers array with the semaphore
  QLib::TQWaitResult WaitResult = m_BuffersSemaphore.WaitFor(Timeout);
  if (WaitResult == QLib::wrSignaled)
  {
    m_ReadyBufferMutex.WaitFor();

    // Get a buffer
    PBYTE TmpBuffer = m_Buffers[m_ReadyBufferIndex];

    // Set the index for the next buffer
    m_ReadyBufferIndex = (m_ReadyBufferIndex + 1) % m_Buffers.size();

    m_ReadyBufferMutex.Release();

    // Copy the message content into the current available buffer and remember also the message length
    memcpy(TmpBuffer,Data,DataSize);

    // Prepare a message block structure (this is the actual data added to the queue)
    TSimpleQueueBlock MessageBlock = {TmpBuffer,DataSize};

    // Add to the queue
    m_QueueHolder.SendUrgent(MessageBlock);
  }
  return WaitResult;
}

// Get a message from the queue
QLib::TQWaitResult CQSimpleQueue::Receive(PVOID Data,unsigned DataSize,unsigned *ActualMessageLength,TQWaitTime Timeout)
{


  TSimpleQueueBlock MessageBlock;

  m_FlushReceiveMutex.WaitFor();

  // Get the an item
  QLib::TQWaitResult WaitResult = m_QueueHolder.Receive(MessageBlock,Timeout);

  // If ok, copy the message data to the supplied buffer
  if(WaitResult == QLib::wrSignaled)
  {
    // Copy the minimum number of bytes according to the rquested data size and the message length
    memcpy(Data,MessageBlock.Buffer,min(DataSize,MessageBlock.MessageLength));

    // Return the actual messsage length in the
    if(ActualMessageLength != NULL)
      *ActualMessageLength = MessageBlock.MessageLength;

    m_BuffersSemaphore.Release();

  }

  m_FlushReceiveMutex.Release();

  return WaitResult;
}

// Clear the queue content
TQErrCode CQSimpleQueue::Flush(void)
{
  m_FlushReceiveMutex.WaitFor();

  while(QueueSize > 0)
    m_BuffersSemaphore.Release();

  m_QueueHolder.Flush();

  m_FlushReceiveMutex.Release();

  return Q_NO_ERROR;
}

// Get the number of messages currently in the queue
unsigned CQSimpleQueue::GetItemsCount(void)
{
  return m_QueueHolder.ItemsCount;
}

// Get the queue maximum size
unsigned CQSimpleQueue::GetQueueSize(void)
{
  return m_QueueHolder.QueueSize;
}

// Release waiting threads (the Send / Receivce functions return with the "wrReleased" code)
void CQSimpleQueue::Release(void)
{
  m_QueueHolder.Release();
}

// Get/Set functions for the MaxItemsCount property
void CQSimpleQueue::SetMaxItemsCount(unsigned Value)
{
  m_QueueHolder.MaxItemsCount = Value;
}

unsigned CQSimpleQueue::GetMaxItemsCount(void)
{
  return m_QueueHolder.MaxItemsCount;
}

