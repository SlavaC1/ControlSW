/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Module: BPE (bytes-pairs-encoding) compression utilities.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 12/01/2002                                           *
 * Last upate: 21/01/2002                                           *
 ********************************************************************/

#include <string.h>

#define BLOCKSIZE 8000   // Maximum block size
#define HASHSIZE  2048   // Size of hash table
#define MAXCHARS   200   // Char set per block
#define THRESHOLD    3   // Minimum pair count


static unsigned char buffer[BLOCKSIZE]; // Data block
static unsigned char leftcode[256];     // Pair table
static unsigned char rightcode[256];    // Pair table
static unsigned char left[HASHSIZE];    // Hash table
static unsigned char right[HASHSIZE];   // Hash table
static unsigned char count[HASHSIZE];   // Pair count
static int size;                        // Size of current data block


// Return index of character pair in hash table. Deleted nodes have count of 1 for hashing
static int Lookup(unsigned char a,unsigned char b)
{
  // Compute hash key from both characters
  int index = (a ^ (b << 5)) & (HASHSIZE - 1);

  // Search for pair or first empty slot
  while ((left[index] != a || right[index] != b) &&
        count[index] != 0)
    index = (index + 1) & (HASHSIZE - 1);

  // Store pair in table
  left[index] = a;
  right[index]= b;
  return index;
}

// Read next block from input buffer
static int BlockRead(unsigned char *SourceBuffer,int BufferSize)
{
  int c, index, used=0;
  int BufferIndex = 0;

  // Reset hash table and pair table
  memset(count,0,HASHSIZE);

  for (c = 0; c < 256; c++)
    leftcode[c] = c;

  memset(rightcode,0,256);

  size = 0;

  c = SourceBuffer[BufferIndex++];
  buffer[size++] = c;

  // Use rightcode to flag data chars found
  if (!rightcode[c])
  {
    rightcode[c] = 1;
    used++;
  }

  while (size < BLOCKSIZE && used < MAXCHARS &&
        (BufferIndex < BufferSize))
  {
    c = SourceBuffer[BufferIndex++];

      index = Lookup(buffer[size-1],c);

      if (count[index] < 255)
        ++count[index];

    buffer[size++] = c;

    // Use rightcode to flag data chars found
    if (!rightcode[c])
    {
      rightcode[c] = 1;
      used++;
    }
  }

  return BufferIndex;
}

// Write each pair table and data block to output
static int BlockWrite(unsigned char *DstBuffer,int DstBufferMaxSize)
{
  int i, len, c = 0;
  int BufferIndex = 0;

  // For each character 0..255
  while (c < 256)
  {
    // If not a pair code, count run of literals
    if (c == leftcode[c])
    {
      len = 1;
      c++;
      while(len < 127 && c < 256 && c == leftcode[c])
      {
        len++;
        c++;
      }

      if(BufferIndex >= DstBufferMaxSize)
        return -1;

      DstBuffer[BufferIndex++] = len + 127;
      len = 0;

      if(c == 256)
        break;
    }
      // Else count run of pair codes
      else
      {
        len = 0;
        c++;
        while(len<127 && c<256 && c!=leftcode[c] ||
            len<125 && c<254 && c+1!=leftcode[c+1])
        {
          len++;
          c++;
        }

        if(BufferIndex >= DstBufferMaxSize)
          return -1;

        DstBuffer[BufferIndex++] = len;
        c -= len + 1;
      }

    // Write range of pairs to output
    for (i = 0; i <= len; i++)
    {
      if(BufferIndex >= DstBufferMaxSize)
        return -1;

      DstBuffer[BufferIndex++] = leftcode[c];

      if (c != leftcode[c])
      {
        if(BufferIndex >= DstBufferMaxSize)
          return -1;

        DstBuffer[BufferIndex++] = rightcode[c];
      }

      c++;
    }
  }

  if(BufferIndex > DstBufferMaxSize - 2 - size)
    return -1;

  // Write size bytes and compressed data block
  DstBuffer[BufferIndex++] = size / 256;
  DstBuffer[BufferIndex++] = size % 256;
  memcpy(&DstBuffer[BufferIndex],buffer,size);
  BufferIndex += size;

  return BufferIndex;
}

// Compress a buffer from memory to memory, return compressed buffer size
int BPE_CompressBuffer(unsigned char *SrcBuffer,int SrcBufferSize,unsigned char *DstBuffer,int DstBufferMaxSize)
{
  int leftch, rightch, code, oldsize;
  int index, r, w, best;

  int SrcBufferIndex = 0;
  int DstBufferIndex = 0;

  // Compress each data block until end of file
  while(SrcBufferIndex < SrcBufferSize)
  {
    SrcBufferIndex += BlockRead(&SrcBuffer[SrcBufferIndex],SrcBufferSize - SrcBufferIndex);

    code = 256;

    // Compress this block
    for (;;)
    {
      // Get next unused char for pair code
      for (code--; code >= 0; code--)
        if (code==leftcode[code] && !rightcode[code])
          break;

      // Must quit if no unused chars left
      if(code < 0)
        break;

      // Find most frequent pair of chars
      for (best=2, index=0; index<HASHSIZE; index++)
        if (count[index] > best)
        {
          best = count[index];
          leftch = left[index];
          rightch = right[index];
        }

      // Done if no more compression possible
      if(best < THRESHOLD)
        break;

      // Replace pairs in data, adjust pair counts
      oldsize = size - 1;
      for (w = 0, r = 0; r < oldsize; r++)
      {
        if (buffer[r] == leftch &&
            buffer[r+1] == rightch) {

          if (r > 0) {
            index = Lookup(buffer[w-1],leftch);
            if (count[index] > 1) --count[index];
            index = Lookup(buffer[w-1],code);
            if (count[index] < 255) ++count[index];
          }
          if (r < oldsize - 1) {
            index = Lookup(rightch,buffer[r+2]);
            if (count[index] > 1) --count[index];
            index = Lookup(code,buffer[r+2]);
            if (count[index] < 255) ++count[index];
          }
          buffer[w++] = code;
          r++; size--;
        }
        else buffer[w++] = buffer[r];
      }
      buffer[w] = buffer[r];

      // Add to pair substitution table
      leftcode[code] = leftch;
      rightcode[code] = rightch;

      // Delete pair from hash table
     index = Lookup(leftch,rightch);
     count[index] = 1;
    }

    int BytesWritten = BlockWrite(&DstBuffer[DstBufferIndex],DstBufferMaxSize - DstBufferIndex);

    if(BytesWritten == -1)
      return -1;

    DstBufferIndex += BytesWritten;
  }

  return DstBufferIndex;
}

// Decompress a buffer from memory to memory, return decompressed buffer size
int BPE_DecompressBuffer(unsigned char *SrcBuffer,int SrcBufferSize,unsigned char *DstBuffer,int DstBufferMaxSize)
{
  unsigned char left[256], right[256], stack[30];
  int c, count, i, size;

  int SrcIndex = 0;
  int DstIndex = 0;

  // Unpack each block until end of file
  while(SrcIndex < SrcBufferSize)
  {
    count = SrcBuffer[SrcIndex++];

    // Set left to itself as literal flag 
    for (i = 0; i < 256; i++)
      left[i] = i;

    // Read pair table 
    for (c = 0;;)
    {
      // Skip range of literal bytes 
      if (count > 127)
      {
        c += count - 127;
        count = 0;
      }
      if (c == 256)
        break;

      // Read pairs, skip right if literal 
      for (i = 0; i <= count; i++, c++)
      {
        left[c] = SrcBuffer[SrcIndex++];
        if (c != left[c])
          right[c] = SrcBuffer[SrcIndex++];
      }
      if(c == 256)
        break;

      count = SrcBuffer[SrcIndex++];
    }

    // Calculate packed data block size
    size = 256 * SrcBuffer[SrcIndex++];
    size += SrcBuffer[SrcIndex++];

    // Unpack data block
    for(i = 0;;)
    {
      // Pop byte from stack or read byte
      if(i)
        c = stack[--i];
      else
      {
        if(!size--)
          break;
        c = SrcBuffer[SrcIndex++];
      }

      // Output byte or push pair on stack
      if (c == left[c])
      {
        if(DstIndex >= DstBufferMaxSize)
          return -1;
          
        DstBuffer[DstIndex++] = c;
      }
      else
      {
        stack[i++] = right[c];
        stack[i++] = left[c];
      }
    }
  }

  return DstIndex;
}




