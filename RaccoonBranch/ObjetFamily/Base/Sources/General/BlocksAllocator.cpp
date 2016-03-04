/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Memory managment.                                        *
 * Module Description: Memory manager for allocation of large       *
 *                     blocks.                                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 11/08/2003                                           *
 * Last upate: 11/08/2003                                           *
 ********************************************************************/

#include <stdlib.h>
#include "BlocksAllocator.h"

#include "QLogFile.h" //bug 5885
#include "AppLogFile.h" //bug 5885


// Class CSingleBlock implementation
// ********************************************************************

// Allocate the block, return true is success
bool CSingleBlock::Allocate(unsigned BufferSize)
{
  m_Buffer = malloc(BufferSize);

  // The "Allocated" flag is used to mark that the block has been acquired
  m_Allocated = false;

  return (m_Buffer != NULL);
}

// Free the block
void CSingleBlock::Free(void)
{
  if(m_Buffer != NULL)
  {
    free(m_Buffer);
    m_Allocated = false;
  }
}

// Class CBlocksAllocator implementation
// ********************************************************************

// Default constructor
CBlocksAllocator::CBlocksAllocator(void)
{
  m_BlockSize = 0;
}

// Constructor
CBlocksAllocator::CBlocksAllocator(unsigned BlocksNum,unsigned BlockSize)
{
  Init(BlocksNum,BlockSize);
}

// Destructor
CBlocksAllocator::~CBlocksAllocator(void)
{
  DeInit();
}

// Initialize allocator
void CBlocksAllocator::Init(unsigned BlocksNum,unsigned BlockSize)
{
  CQMutexHolder GuardHolder(&m_Guard);
  // If already initialized, de-init first, but only if the amount of memory changed
  if (m_Blocks.size() > 0)
  {
     if ((BlocksNum != BlocksNum) || (BlockSize != m_BlockSize))
        Clear();
     else
     {
        // Just clear allocation markers
        for(unsigned i = 0; i < m_Blocks.size(); i++)
           m_Blocks[i]->UnAlloc();
        return; // We are done
     }
  }
  m_Blocks.resize(BlocksNum);
  m_BlockSize = 0;
  // Allocate blocks
  for(unsigned i = 0; i < BlocksNum; i++)
  {
     m_Blocks[i] = new CSingleBlock;
     // Allocate and check allocation
     if(!m_Blocks[i]->Allocate(BlockSize))
        throw EBlocksAllocator(QFormatStr("Cannot allocate %d blocks with size of %d bytes.\nClose and restart the printer application.",BlocksNum,BlockSize));
  }
  m_BlockSize = BlockSize;
}

// free all allocated blocks
void CBlocksAllocator::Clear(void)
{
  // De-allocate all blocks
  for(unsigned i = 0; i < m_Blocks.size(); i++)
    delete m_Blocks[i];
  m_Blocks.clear();
}

// De-initialize (free all allocated blocks)
void CBlocksAllocator::DeInit(void)
{
  CQMutexHolder GuardHolder(&m_Guard);
  Clear();
}

// Get a memory block, return NULL if allocation failed
void *CBlocksAllocator::GetBlock(unsigned RequestedSize,bool CleanBuffer)
{
  void *Tmp = NULL;
  unsigned i = 0;
  CQMutexHolder GuardHolder(&m_Guard);
  // If the requested block size is less or equal what we have, we are Ok
  if (RequestedSize <= m_BlockSize)
  {
     // Find a free block
     for(i = 0; i < m_Blocks.size(); i++)
     {
        if(!m_Blocks[i]->IsAllocated())
        {
            Tmp = m_Blocks[i]->RequestBuffer();
            if (CleanBuffer)
               memset(Tmp,0,m_BlockSize);
            break;
        }
     }
     if(i==m_Blocks.size())
     CQLog::Write(LOG_TAG_PROCESS,"CBlocksAllocator: cannot find free block");
  }
  else
     CQLog::Write(LOG_TAG_PROCESS,"CBlocksAllocator: Requested block size (%d) is bigger than m_BlockSize",RequestedSize);
  return Tmp;
}
void CBlocksAllocator::ReleaseBlocks()
{
  CQMutexHolder GuardHolder(&m_Guard);
  for(unsigned i = 0; i < m_Blocks.size(); i++)
  {
      if(m_Blocks[i])
        m_Blocks[i]->UnAlloc();
  }
  return;
}

// Release memory block
void CBlocksAllocator::ReleaseBlock(void *Buffer)
{
  CQMutexHolder GuardHolder(&m_Guard);
  // Find the associated block
  for(unsigned i = 0; i < m_Blocks.size(); i++)
    if(m_Blocks[i]->GetBuffer() == Buffer)
    {
      m_Blocks[i]->UnAlloc();
      return;
    }
  // Trying to release an unknown buffer...
  throw EBlocksAllocator("Can not release unknown buffer");
}

// Return true if allocated
bool CBlocksAllocator::IsAllocated(void)
{
  CQMutexHolder GuardHolder(&m_Guard);
  return (m_Blocks.size() > 0);
}

// Default constructor
CSingleBlock::CSingleBlock(void)
{
  m_Buffer = NULL;
  m_Allocated = false;
}

// Allocation constructor
CSingleBlock::CSingleBlock(unsigned BufferSize)
{
  Allocate(BufferSize);
}

// Destructor
CSingleBlock::~CSingleBlock(void)
{
  Free();
}

// Return allocation status
unsigned CSingleBlock::IsAllocated(void)
{
  return m_Allocated;
}

// Return buffer pointer
void *CSingleBlock::GetBuffer(void)
{
  return m_Buffer;
}

// Mark allocation flag and return buffer pointer
void *CSingleBlock::RequestBuffer(void)
{
  m_Allocated = true;
  return m_Buffer;
}

void CSingleBlock::UnAlloc(void)
{
  m_Allocated = false;
}

