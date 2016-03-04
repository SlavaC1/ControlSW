/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Windows BMP file related definitions.
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/09/2001                                           *
 * Last upate: 03/09/2001                                           *
 ********************************************************************/

#ifndef _BMP_FILE_DEFS_H_
#define _BMP_FILE_DEFS_H_

#include "QTypes.h"

// Align all structures to byte boundry
#ifdef OS_WINDOWS

// The following pragma is for BCB only
#ifndef __BCPLUSPLUS__
#error Q2RT: This module contain pragma statements for Borland C++ Builder
#endif

#pragma pack(push,1)

#endif

#ifdef OS_VXWORKS
#define STRUCT_ATTRIBUTE __attribute__ ((packed))
#elif defined(OS_WINDOWS)
#define STRUCT_ATTRIBUTE
#endif


const int BMP_NO_COMPRESSION = 0;


// BMP file header
struct TBMPFileHeader {
  USHORT FileType;
  ULONG  FileSize;
  USHORT Reserved1;
  USHORT Reserved2;
  ULONG  BitmapOffset;

} STRUCT_ATTRIBUTE;

// This bitmap header is for BMP version 3
struct TBitmapHeader {
  ULONG  Size;
  ULONG  Width;
  ULONG  Height;
  USHORT Planes;
  USHORT BitsPerPixel;
  ULONG  Compression;
  ULONG  SizeOfBitmap;
  long   HorzResolution;
  long   VertResolution;
  ULONG  ColorsUsed;
  ULONG  ColorsImportant;

} STRUCT_ATTRIBUTE;


// Restore previous align settings
#ifdef OS_WINDOWS
#pragma pack(pop)
#endif

#endif

