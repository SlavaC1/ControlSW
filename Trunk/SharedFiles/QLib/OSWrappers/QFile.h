/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers 			*
 * Module Description: This class is an implementation of the       *
 *                     QSemaphore class for Windows.                *
 *                                                                  *
 * Compilation: Standard C++. Win32                                 *
 *                                                                  *
 * Author: Shahar Behagen.                                               *
 * Start date: 23/04/2008                                           *
 * Last upate: 23/04/2008                                         *
 ********************************************************************/

#ifndef _Q_FILE_H_
#define _Q_FILE_H_

// OS specific headers
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include <stdio.h>
#include <map>
#include "QMutex.h"
#include "QTypes.h"
#include "QException.h"

// enum for our File types
typedef enum    {ftLog
                ,ftMode
                ,ftParameters
                ,ftMaintenanceCounters
				}TFileType;

typedef std::map<QString,CQMutex *> TMutexFilesMap;

// Exception class for all the QLib RTTI elements
class EQOSFileCreator : public EQException {
  public:
    EQOSFileCreator(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class QOSFileCreator
{
private:
  char m_FileType[15];
  char m_FileName[256];
  HANDLE m_Handle;

public:
  QOSFileCreator::QOSFileCreator();
  void CreateFile(QString FileName, char *type);
  FILE* toSTDIO(); // transforms the HANDLE to a stdio.h FILE* descriptor
};

class QMutexFilesMap
{

	private:
		//Container which holding the mutexs for each file,in order to make the Reading/Writing thread safe.
		TMutexFilesMap  MutexFilesMap;
        CQMutex         m_FileInsertionMutex;
	
	public:
		// Constructor
		QMutexFilesMap();
		// Destructor
		~QMutexFilesMap(void);
		void InsertFileMutex(QString FileName);
		void WaitFor(QString FileName);
		void Release(QString FileName);

};

extern  QMutexFilesMap MutexFilesMap;

#endif