/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Windows BMP file support.                                *
 * Module Description: Cross-platform implementation of a windows   *
 *                     BMP file reader. Currently only supports 1BPP*
 *                     formats.                                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/09/2001                                           *
 * Last upate: 23/09/2001                                           *
 ********************************************************************/

#include <string.h>
#include "BMPFile.h"
#include "BMPFileDefs.h"


// Constructor
CBMPFile::CBMPFile(const QString& FileName)
{
  m_Width = m_Height = m_DataSizeInBytes = m_LineSizeInBytes = 0;
  m_Orientation = false;
  m_fp = NULL;
  Open(FileName);
}

// Defualt constructor
CBMPFile::CBMPFile(void)
{
  m_fp = NULL;
  m_Width = m_Height = m_DataSizeInBytes = m_LineSizeInBytes = 0;
  m_Orientation = false;
}

// Destructor
CBMPFile::~CBMPFile(void)
{
  try
  {
    Close();
  } catch(...)
    {
    }
}

// Open a BMP file
void CBMPFile::Open(const QString& FileName)
{
  // If currently open, close before reopen
  if(m_fp != NULL)
    Close();

  m_fp = fopen(FileName.c_str(),"rb");

  if(m_fp == NULL)
    throw EBMPFile("Can not open BMP file \"" + FileName + "\"");

  TBMPFileHeader BMPFileHeader;

  // Read file header
  if(fread(&BMPFileHeader,sizeof(TBMPFileHeader),1,m_fp) != 1)
  {
    fclose(m_fp);
    throw EBMPFile("Can not read BMP file header");
  }

  // Verify header
  if(memcmp(&(BMPFileHeader.FileType),"BM",2))
  {
    fclose(m_fp);
    throw EBMPFile("Invalid BMP file signature");
  }

  TBitmapHeader BitmapHeader;

  // Read the bitmap header
  if(fread(&BitmapHeader,sizeof(TBitmapHeader),1,m_fp) != 1)
  {
    fclose(m_fp);
    throw EBMPFile("Can not read BMP file bitmap header");
  }

  // Verify BMP file version
  if(BitmapHeader.Size != sizeof(TBitmapHeader))
  {
    fclose(m_fp);
    throw EBMPFile("BMP file version not supported");
  }

  // Verify BPP and compression values
  if(((BitmapHeader.BitsPerPixel != 1) && (BitmapHeader.BitsPerPixel != 4)) || (BitmapHeader.Compression != BMP_NO_COMPRESSION))
  {
    fclose(m_fp);
    throw EBMPFile("Only uncompressed 1BPP/4BPP BMP files are supported");
  }

  // Remember width and height for later
  m_Width = BitmapHeader.Width;
  m_Height = BitmapHeader.Height;

  if(m_Height < 0)
  {
    m_Orientation = false;
    m_Height = -m_Height;
  } else
      m_Orientation = true;

  // Calculate image line and buffer sizes
  // Note: BMP data padded to 4 bytes lines (not pixels)

  m_LineSizeInBytes = (((BitmapHeader.BitsPerPixel*m_Width) + 31) / 32) * 4;
  m_DataSizeInBytes = m_LineSizeInBytes * m_Height;

  // Reposition the file pointer to the data start area
  fseek(m_fp,BMPFileHeader.BitmapOffset,SEEK_SET);
}

// Close the file
void CBMPFile::Close()
{
  if(m_fp != NULL)
  {
    if(fclose(m_fp) != 0)
    {
      m_fp = NULL;
      throw EBMPFile("Can not close BMP file");
    }

    m_fp = NULL;
  }
}

// Read data image to buffer
void CBMPFile::ReadImage(PVOID Buffer,int LinesToSkip)
{
  int LinesToRead = m_Height / (LinesToSkip + 1);

  // If the image orientation is forward
  if(m_Orientation)
  {
    LinesToSkip = -(LinesToSkip + 2);

    // Position the file pointer on the beginning of the last line
    if(fseek(m_fp,-m_LineSizeInBytes,SEEK_END) != 0)
      throw EBMPFile("Can not read BMP file data (1)");
  }

  for(int i = 0; i < LinesToRead; i++)
  {
    // Read single line
    if(fread(&static_cast<char *>(Buffer)[m_LineSizeInBytes * i],m_LineSizeInBytes,1,m_fp) != 1)
      throw EBMPFile("Can not read BMP file data (2)");

    // Skip lines
    if(i < LinesToRead - 1)
      if(fseek(m_fp,m_LineSizeInBytes * LinesToSkip,SEEK_CUR) != 0)
        throw EBMPFile("Can not read BMP file data (3)");
  }
}


