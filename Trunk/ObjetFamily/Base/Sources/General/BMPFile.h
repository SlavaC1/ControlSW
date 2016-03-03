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
 * Last upate: 03/09/2001                                           *
 ********************************************************************/

#ifndef _BMP_FILE_H_
#define _BMP_FILE_H_

#include <stdio.h>
#include "QObject.h"
#include "QException.h"


// Exception class for the BMP file read module
class EBMPFile : public EQException {
  public:
    EBMPFile(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CBMPFile : public CQObject {
  private:
    // Pointer to current file handle
    FILE *m_fp;

    int m_Width,m_Height;
    int m_DataSizeInBytes;
    int m_LineSizeInBytes;

    // True if the image orientation is forward (height field is negative)
    bool m_Orientation;

  public:
    // Constructor
    CBMPFile(const QString& FileName);

    // Default constructor
    CBMPFile(void);

    // Destructor
    ~CBMPFile(void);

    // Open a BMP file
    void Open(const QString& FileName);

    // Close the file
    void Close();

    // Get the width in pixels
    int GetWidth(void) {
      return m_Width;
    }

    // Get the width in pixels
    int GetHeight(void) {
      return m_Height;
    }

    // Get the required image buffer size
    unsigned GetDataSizeInBytes(void) {
      return m_DataSizeInBytes;
    }

    // Get the size of a single line in bytes
    unsigned GetLineSizeInBytes(void) {
      return m_LineSizeInBytes;
    }

    // Read data image to buffer
    void ReadImage(PVOID Buffer,int LinesToSkip);
};

#endif

