/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Module: BPE (bytes-pairs-encoding) compression utilities.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/01/2002                                           *
 * Last upate: 13/01/2002                                           *
 ********************************************************************/

#ifndef _BPE_COMPRESS_H_
#define _BPE_COMPRESS_H_

// Compress a buffer from memory to memory, return compressed buffer size or -1 if destination
// buffer requires more than DstBufferMaxSize bytes.
int BPE_CompressBuffer(unsigned char *SrcBuffer,int SrcBufferSize,unsigned char *DstBuffer,int DstBufferMaxSize);

// Decompress a buffer from memory to memory, return decompressed buffer size or -1 if destination
// buffer requires more than DstBufferMaxSize bytes.
int BPE_DecompressBuffer(unsigned char *SrcBuffer,int SrcBufferSize,unsigned char *DstBuffer,int DstBufferMaxSize);

#endif
