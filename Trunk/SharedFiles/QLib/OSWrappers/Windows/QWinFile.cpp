/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers 										*
 * Module Description: This class is an implementation of the       *
 *                                                                  *
 * Compilation: Standard C++. Win32                                 *
 *                                                                  *
 * Author: Shahar Behagen.                                          *
 * Start date: 23/04/2008                                           *
 * Last upate: 23/04/2008                                         	*
 ********************************************************************/
#pragma init_seg(compiler)
#include "QFile.h"
#include "QUtils.h"
#include "io.h"

//Class which hold Container that holds the mutexs for each file,in order to make the Reading/Writing thread safe.
QMutexFilesMap  MutexFilesMap;

// Constructor
QOSFileCreator::QOSFileCreator()
{}

void QOSFileCreator::CreateFile(QString FileName, char *FileType)
{
    DWORD dwDesiredAccess = 0;
    DWORD dwShareMode     = 0;
    DWORD dwCreationDistribution  = 0;
	DWORD dwFlagsAndAttributes    = 0;


	strcpy (m_FileName, FileName.c_str());
    strcpy (m_FileType, FileType);

    // When getting a request to open for read, don't go through the CreateFile()
    if (strchr(FileType, 'r'))
    {
      /*
      dwDesiredAccess |= GENERIC_READ;
      dwShareMode     |= FILE_SHARE_READ;
      dwCreationDistribution = OPEN_ALWAYS;
      dwFlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;
      */

      return;
    }

    // fread() and fwrite() mis-behave when Handle obtained via CreateFile()
    // I currently wont support it.
    if (strchr(FileType, 'b'))
      return;

    if (strchr(FileType, 'w') || strchr(FileType, 'a'))
    {
      dwDesiredAccess |= GENERIC_WRITE;
	  dwShareMode     |= (FILE_SHARE_READ | FILE_SHARE_WRITE);

	  if (strchr(FileType, 'w'))
	  {
		dwCreationDistribution = CREATE_ALWAYS;
	  }
      else
        dwCreationDistribution = OPEN_ALWAYS;

	  dwFlagsAndAttributes |= (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH);
    }

    // todo -oNobody -cNone: check why CreateFile only worked when (for read, and write streams)
	dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    
	m_Handle = ::CreateFile(FileName.c_str(),
							dwDesiredAccess,
                            dwShareMode,
                            NULL,
                            dwCreationDistribution,
                            dwFlagsAndAttributes,
							NULL);

	if((m_Handle == INVALID_HANDLE_VALUE) && (FileName != ""))
	{
		QString str = QFormatStr("QOSFileCreator: Unable to create '%s'\n", FileName.c_str());
        str +=					 "Reason: invalid Handle";
        
		throw EQOSFileCreator(str,GetLastError());
	}

}

FILE* QOSFileCreator::toSTDIO()
{

    FILE *h_FILE = NULL;

    // When getting a request to open for read, don't go through the CreateFile()
    if (strchr(m_FileType, 'r'))
      return fopen(m_FileName, m_FileType);

    // fread() and fwrite() mis-behave when Handle obtained via CreateFile()
    // Since fread/fwite are used in our code only when dealing with binary files:
    if (strchr(m_FileType, 'b'))
      return fopen(m_FileName, m_FileType);

    int h_osf = _open_osfhandle(((long)m_Handle), 0);

    // Note that I return NULL on all kinds of failures - to be comaptible with the fopen() return values.
    if (-1 == h_osf)
      return NULL;

    h_FILE = _fdopen(h_osf, m_FileType);

    // todo -oNobody -cNone: For some reason, the _fdopen(), causes problems in fread() / fwrite() calls. (it doesn't read/write all bytes at the stream)

    // This didn't help much either:..
    setvbuf(h_FILE, NULL, _IOFBF, (64*1024));


    // Handle File opened for appending.  
    if (strchr(m_FileType, 'a'))
      fseek(h_FILE, 0L, SEEK_END);

    /*
    The following fscan() loop seems to work well in replacing fread() calls. (which don't work well)

    DWORD wmRead = 0;
    while (wmRead < FileSize)
      if (0 == fscanf(h_FILE,"%c",&buf[wmRead++]))
        break;
    */

    return h_FILE;
}


// Constructor
QMutexFilesMap::QMutexFilesMap()
{
	MutexFilesMap.clear();
}

QMutexFilesMap::~QMutexFilesMap(void)
{
	TMutexFilesMap::iterator it;
	try{
		for ( it = MutexFilesMap.begin(); it != MutexFilesMap.end(); ++it )
			delete (*it).second;
	}
	catch(...)
	{
	}

}

void QMutexFilesMap::InsertFileMutex(QString FileName)
{
    m_FileInsertionMutex.WaitFor();
    
	try{
		TMutexFilesMap::iterator it;
		CQMutex *FileFlagMutex;

		it = MutexFilesMap.find(FileName); //trying to find the specific mutex for this file.

		if( it == MutexFilesMap.end() )
		{
			// no mutex found for the specific file.
			//Relate the mutex for the file.

			//curently we are not deleting those mutex,
			//there is a problem to know when to delete .Inadition we dont create many files.
			FileFlagMutex = new  CQMutex;

			MutexFilesMap[FileName] = FileFlagMutex;//Insert to the container.

			//If another thread at this point already inserted mutex for this file
			// and its not the current FileFlagMutex- delete it.
			it = MutexFilesMap.find(FileName);
			if((*it).second != FileFlagMutex)
				delete FileFlagMutex;
		}
		else // mutex found for the specific file
		{
		   FileFlagMutex = (*it).second;
		}
	}
	catch(...)
	{
        m_FileInsertionMutex.Release();
        
		QString str = "QMutexFilesMap: Error to insert mutex for the file: ";
		str += FileName.c_str();
	   	throw EQOSFileCreator(str);
	}

    m_FileInsertionMutex.Release();
}

void QMutexFilesMap::WaitFor(QString FileName)
{

	bool AlreadyInserted = false;
	TMutexFilesMap::iterator it;
	CQMutex *FileFlagMutex;

	try{

		it = MutexFilesMap.find(FileName); //trying to find the specific mutex for this file.

		if( it == MutexFilesMap.end() )
		{
			// no mutex found for the specific file.
			QString str = "QMutexFilesMap: There is no such mutex for the file: ";
			str += FileName.c_str();

			throw EQOSFileCreator(str);
		}
		else // mutex found for the specific file
		{
		   FileFlagMutex = (*it).second;
		   AlreadyInserted = true;
		}

		FileFlagMutex->WaitFor();
	}
	catch(EQOSFileCreator& Err)//this throw is before waiting, so there is no need to release the mutex. 
	{
	   throw EQOSFileCreator(Err.GetErrorMsg());
	}
	catch(...)
	{
		if(AlreadyInserted)
			FileFlagMutex->Release();

		QString str = "QMutexFilesMap: Error to do mutex-wait for the file: ";
		str += FileName.c_str();
		throw EQOSFileCreator(str);
	}
}

void QMutexFilesMap::Release(QString FileName)
{

	try{

		TMutexFilesMap::iterator it;
		CQMutex *FileFlagMutex;

		it = MutexFilesMap.find(FileName); //trying to find the specific mutex for this file.

		if( it == MutexFilesMap.end() )
		{
			// no mutex found for the specific file.
			QString str = "QMutexFilesMap: There is no such mutex for the file: ";
			str += FileName.c_str();

			throw EQOSFileCreator(str);
		}
		else // mutex found for the specific file
		{
		   FileFlagMutex = (*it).second;
		}

		FileFlagMutex->Release();
	}
	catch(EQOSFileCreator& Err)
	{
	   throw EQOSFileCreator(Err.GetErrorMsg());
	}
	catch(...)
	{
		QString str = "QMutexFilesMap: Error to do mutex-release for the file: ";
		str += FileName.c_str();
	   	throw EQOSFileCreator(str);
	}
}
