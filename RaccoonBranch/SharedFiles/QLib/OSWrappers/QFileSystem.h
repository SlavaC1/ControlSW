/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Cross-Platform file-system utilities and     *
 *                     support.                                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 06/02/2002                                           *
 * Last upate: 06/02/2002                                           *
 ********************************************************************/

#ifndef _Q_FILE_SYSTEM_H_
#define _Q_FILE_SYSTEM_H_

#include "QTypes.h"

#ifdef OS_WINDOWS

#include <Sysutils.hpp>

#ifndef __BCPLUSPLUS__
#error QLib: The QFileSystem module must be compiled with Borland C++ Builder
#endif

#endif

// Initialize the local file system (default partition is "c:")
TQErrCode QFileSystemInit(char Partition = 'c');

// Change the current work directory
TQErrCode QChangeDirectory(const QString Path);

// Convert a DOS/Windows path to local platform path
QString QConvertPath(const QString DosPath);

// Some related types
typedef TSearchRec TQFileSearchRec;
typedef int TQFileAttributes;

// Directory search routines
bool QFindFirst(const QString FilePath,TQFileAttributes Attributes,TQFileSearchRec& SearchRec);
bool QFindNext(TQFileSearchRec& SearchRec);
void QFindClose(TQFileSearchRec& SearchRec);
bool QFindFirstDir(const QString FilePath, TQFileSearchRec& SearchRec);
bool QFindNextDIR(TQFileSearchRec& SearchRec);
void QFindCloseDir(TQFileSearchRec& SearchRec);

// File delete, return false if error
bool QDeleteFile(const QString FileName);

// Copy file
bool QCopyFile(const QString SourceFileName,const QString DestFileName);

#endif


