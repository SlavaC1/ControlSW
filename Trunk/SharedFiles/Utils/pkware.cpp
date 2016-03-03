/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Module: PKWARE compression utilities (uses the implode library). *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 16/12/2002                                           *
 * Last upate: 16/12/2002                                           *
 ********************************************************************/

#include <stdlib.h>
#include <algorithm>
#include <mem.h>
#include "implode.h"


// Source buffer variables
static unsigned char *gSrcBuffer;
static unsigned gBytesInSrcBuffer;
static unsigned gCurrentPosInSrcBuffer;

// Destination buffer variables
static unsigned char *gDstBuffer;
static unsigned gBytesInDstBuffer;
static unsigned gCurrentPosInDstBuffer;

// Temporary buffer for PKWARE extraction routine 
static char *gScratchPad;


// Callback funstion for the PKWARE progressive buffer read
unsigned ReadBuffer(char *buffer,unsigned *iSize, void */*pParam*/)
{
  unsigned BytesRead;

  if(gCurrentPosInSrcBuffer < gBytesInSrcBuffer)
  {
    unsigned BytesLeft = gBytesInSrcBuffer - gCurrentPosInSrcBuffer;

    if(BytesLeft < *iSize)
      *iSize = BytesLeft;

    // Copy data and update counters
    memcpy(buffer,gSrcBuffer + gCurrentPosInSrcBuffer,*iSize);
    gCurrentPosInSrcBuffer += *iSize;

    BytesRead = *iSize;

  } else
      BytesRead = 0;

  return BytesRead;
}

// Callback funstion for the PKWARE progressive buffer write
void WriteBuffer(char *buffer,unsigned *iSize, void */*pParam*/)
{
  // If not out of destination buffer space
  if(gCurrentPosInDstBuffer < gBytesInDstBuffer)
  {
    unsigned SpaceInBuffer = gBytesInDstBuffer - gCurrentPosInDstBuffer;

    // Copy data and update counters
    unsigned BytesToCopy = std::min(*iSize,SpaceInBuffer);
    memcpy(gDstBuffer + gCurrentPosInDstBuffer,buffer,BytesToCopy);
    gCurrentPosInDstBuffer += BytesToCopy;
  }

  return;
}

void InitPKExtract(void)
{
  gScratchPad = new char[CMP_BUFFER_SIZE];
}

void DeInitPKExtract(void)
{
  delete []gScratchPad;
}

int PKCompress(void *SourceBuffer,void *DestBuffer,unsigned SourceBufferSize,
              unsigned DestBufferMaxSize,unsigned& DestBufferActualSize)
{
  unsigned DataType = CMP_BINARY;
  unsigned DictSize = 4096;

  gSrcBuffer = (unsigned char *)SourceBuffer;
  gDstBuffer = (unsigned char *)DestBuffer;
  gBytesInSrcBuffer = SourceBufferSize;
  gCurrentPosInSrcBuffer = 0;
  gBytesInDstBuffer = DestBufferMaxSize;
  gCurrentPosInDstBuffer = 0;

  int RetCode = implode(ReadBuffer,WriteBuffer,gScratchPad,NULL,&DataType,&DictSize);
  DestBufferActualSize = gCurrentPosInDstBuffer;

  return RetCode;
}

int PKExtract(void *SourceBuffer,void *DestBuffer,unsigned SourceBufferSize,
              unsigned DestBufferMaxSize,unsigned& DestBufferActualSize)
{
  gSrcBuffer = (unsigned char *)SourceBuffer;
  gDstBuffer = (unsigned char *)DestBuffer;
  gBytesInSrcBuffer = SourceBufferSize;
  gBytesInDstBuffer = DestBufferMaxSize;
  gCurrentPosInSrcBuffer = 0;
  gCurrentPosInDstBuffer = 0;

  int RetCode = explode(ReadBuffer,WriteBuffer,gScratchPad,NULL);
  DestBufferActualSize = gCurrentPosInDstBuffer;

  return RetCode;
}
