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

#ifndef _BLOCKS_ALLOCATOR_H_
#define _BLOCKS_ALLOCATOR_H_

#include <vector>
#include "QComponent.h"
#include "QMutex.h"


// Exception class for the blocks allocator
class EBlocksAllocator : public EQException {
  public:
    EBlocksAllocator(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Class for a single memory block
class CSingleBlock : public CQObject
{
private:
    void *m_Buffer;
    bool m_Allocated;

public:
    CSingleBlock(void); // Default constructor
    CSingleBlock(unsigned BufferSize); // Allocation constructor
    ~CSingleBlock(void); // Destructor
    unsigned IsAllocated(void); // Return allocation status
    void *GetBuffer(void); // Return buffer pointer
    void *RequestBuffer(void); // Mark allocation flag and return buffer pointer
    // Allocate the block, return true is success
    bool Allocate(unsigned BufferSize);
    void UnAlloc(void); // Mark as unallocated (don't free memory)
    void Free(void); // Free the block
};

// Blocks allocator class
class CBlocksAllocator : public CQObject
{
private:
    // Type for a list of blocks
    typedef std::vector<CSingleBlock *> TBlockList;
    TBlockList m_Blocks;
    CQMutex m_Guard;

    unsigned m_BlockSize; // single block size
    void Clear(void); // free all allocated blocks

public:

    CBlocksAllocator(void); // Default constructor
    CBlocksAllocator(unsigned BlocksNum,unsigned BlockSize); // Constructor
    ~CBlocksAllocator(void); // Destructor

    // Initialize allocator
    void Init(unsigned BlocksNum,unsigned BlockSize);

    // De-initialize (thread safe clear)
    void DeInit(void);

    // Get a memory block, return NULL if allocation failed
    void *GetBlock(unsigned RequestedSize,bool CleanBuffer = false);
    void ReleaseBlocks(); //bug 5885
    void ReleaseBlock(void *Buffer); // Release memory block
    bool IsAllocated(void); // Return true if allocated
};

#endif
