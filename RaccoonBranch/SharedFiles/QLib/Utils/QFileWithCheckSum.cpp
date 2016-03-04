/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: File with check sum                                      *
 * Module Description: File that its reliability is checked         *
                       by a 4 byte check sum and a backup file      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Nir Sade                                                *
 * Start date: 26/06/2003                                           *
 * Last upate: 26/06/2003                                           *
 ********************************************************************/

#include "QFileWithCheckSum.h"
#include "QErrors.h"
#include <mem.h>
#include "QFile.h"


// Default constructor
CQFileWithCheckSum::CQFileWithCheckSum(void)
{
  Init("","");
}

// Construct the 'FileWithCheckSum' object, the backup file name is the same as the file but with ".bak" extension
CQFileWithCheckSum::CQFileWithCheckSum(QString FileName)
{
  QString BackupFileName = FileName;
  BackupFileName.replace(BackupFileName.find_last_of(".")+1,3,"bak");
  Init(FileName, BackupFileName);
}

// Construct the 'FileWithCheckSum' object
CQFileWithCheckSum::CQFileWithCheckSum(QString FileName, QString BackupFileName)
{
  Init(FileName, BackupFileName);
}

// Assign file to object
void CQFileWithCheckSum::AssignFile(const QString FileName)
{
  QString BackupFileName = FileName;
  BackupFileName.replace(BackupFileName.find_last_of(".")+1,3,"bak");
  Init(FileName, BackupFileName);
}

// Assign file and backup file to object
void CQFileWithCheckSum::AssignFile(const QString FileName,const QString BackupFileName)
{
  Init(FileName, BackupFileName);
}

void CQFileWithCheckSum::Init(QString FileName, QString BackupFileName)
{
  m_FileName = FileName;
  m_BackupFileName = BackupFileName;
  m_FileSize = 0;
  m_CalculatedCheckSum = 0;
  m_FileCheckSum = 0;
  m_BackupFileCheckSum = 0;
  m_FileState = fsClose;
  m_CurrFileIndex = 0;
}

// Open the file for writing
void CQFileWithCheckSum::BeginWrite()
{
  if (m_FileState != fsClose)
    throw EQFileWithCheckSum("File is already open");

  m_FileState = fsOpenWrite;
  m_FileSize = 0;
  m_FileBuffer.resize(0);
}

// Write 'Data' to the file
void CQFileWithCheckSum::Write(void const* Data, unsigned long DataLength)
{
  if (m_FileState != fsOpenWrite)
    throw EQFileWithCheckSum("File is not open for writing");

  m_FileSize += DataLength;
  m_FileBuffer.resize(m_FileSize);
  memcpy(&m_FileBuffer[m_FileSize-DataLength], Data, DataLength);
}

// Write the checksum to the file and close it
TQErrCode CQFileWithCheckSum::EndWrite()
{
	QOSFileCreator OSFile;

	if (m_FileState != fsOpenWrite)
	throw EQFileWithCheckSum("File is not open for writing");

	TQErrCode RetVal = Q_NO_ERROR;

	// Calculate the check sum
	unsigned long CheckSum = CalculateCheckSum(&(*m_FileBuffer.begin()), m_FileSize);

    MutexFilesMap.InsertFileMutex(m_FileName);//Creating and inserting a mutex for this specific file(if not inserted already)

    //Entering the critical section.
   	MutexFilesMap.WaitFor(m_FileName);

	try
    {
		// Open the file for write
		OSFile.CreateFile(m_FileName,"wb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(m_FileName);
		throw Err;
	}

	FILE *File = OSFile.toSTDIO();

	if (File != NULL)
	{	
		unsigned long NumOfWrittenBytes = fwrite(&(*m_FileBuffer.begin()),sizeof(BYTE),m_FileSize,File);
		if (NumOfWrittenBytes < m_FileSize)
		{
            fclose(File);
			MutexFilesMap.Release(m_FileName);
			throw EQFileWithCheckSum("Error while writing to the file");
		}

		NumOfWrittenBytes = fwrite(&CheckSum,sizeof(unsigned long),1,File);
		if (NumOfWrittenBytes < 1)
		{
            fclose(File);
			MutexFilesMap.Release(m_FileName);
			throw EQFileWithCheckSum("Error while writing to the file");
		}

		fclose(File);
		MutexFilesMap.Release(m_FileName);
	}
	else
    {
        MutexFilesMap.Release(m_FileName);
		RetVal = Q_OPEN_FILE_ERR;
    }

    //-----------------------------------------------------------------------
    MutexFilesMap.InsertFileMutex(m_BackupFileName);

    //Entering the critical section.
	MutexFilesMap.WaitFor(m_BackupFileName);

	try
    {
		// Open the file for write
		OSFile.CreateFile(m_BackupFileName,"wb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(m_BackupFileName);
		throw Err;
	}

	File = OSFile.toSTDIO();

	if (File != NULL)
	{
		unsigned long NumOfWrittenBytes = fwrite(&(*m_FileBuffer.begin()),sizeof(BYTE),m_FileSize,File);
		if (NumOfWrittenBytes < m_FileSize)
		{
            fclose(File);
			MutexFilesMap.Release(m_BackupFileName);
			throw EQFileWithCheckSum("Error while writing to the file");
		}

		NumOfWrittenBytes = fwrite(&CheckSum,sizeof(unsigned long),1,File);
		if (NumOfWrittenBytes < 1)
		{
            fclose(File);
			MutexFilesMap.Release(m_BackupFileName);
			throw EQFileWithCheckSum("Error while writing to the file");
		}

		fclose(File);
		MutexFilesMap.Release(m_BackupFileName);
	}
	else
	{
        MutexFilesMap.Release(m_BackupFileName);
        
		if (RetVal == Q_OPEN_FILE_ERR)
		  RetVal = Q_OPEN_FILE_AND_BACKUP_FILE_ERR;
		else
		  RetVal = Q_OPEN_BACKUP_FILE_ERR;
	}

	m_FileBuffer.resize(0);
	m_FileSize = 0;
	m_FileState = fsClose;

	return RetVal;
}

// Open the file for writing
TQErrCode CQFileWithCheckSum::BeginRead()
{
	QOSFileCreator OSFile;

	if (m_FileState != fsClose)
	    throw EQFileWithCheckSum("File is already open");

	unsigned long FileCS;

    MutexFilesMap.InsertFileMutex(m_FileName);//Creating and inserting a mutex for this specific file(if not inserted already)

    //Entering the critical section.
	MutexFilesMap.WaitFor(m_FileName);

	try
    {
		// Open the file for read
		OSFile.CreateFile(m_FileName,"rb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(m_FileName);
		throw Err;
	}


	FILE *File = OSFile.toSTDIO();

	if (File != NULL)
	{
		// Find the size of the file
		fseek(File, 0, SEEK_END);
		long FileSize = ftell(File) - sizeof(unsigned long); // The four last bytes of the file are the check sum
		fseek(File, 0, SEEK_SET);

		m_FileBuffer.resize(FileSize);

		// Read all the file except the 4 bytes of the check sum
		size_t NumOfReadBytes = fread(&(*m_FileBuffer.begin()),sizeof(BYTE),FileSize,File);
		if ((long)NumOfReadBytes < FileSize)
		{
          fclose(File);
		  MutexFilesMap.Release(m_FileName);
		  throw EQFileWithCheckSum("Error while reading from the file");
		}

		NumOfReadBytes = fread(&FileCS,sizeof(unsigned long),1,File);
		if (NumOfReadBytes < 1)
		{
            fclose(File);
			MutexFilesMap.Release(m_FileName);
			throw EQFileWithCheckSum("Error while reading from the file");
		}

		fclose(File);
		MutexFilesMap.Release(m_FileName);

		if (CalculateCheckSum(&(*m_FileBuffer.begin()), FileSize) == FileCS)
		{
		  m_FileState = fsOpenRead;
		  return Q_NO_ERROR;
		}
	}
    else
    {
         MutexFilesMap.Release(m_FileName);
    }

    //----------------------------------------------------------------------
    MutexFilesMap.InsertFileMutex(m_BackupFileName);//Creating and inserting a mutex for this specific file(if not inserted already)

	//Entering the critical section.
	MutexFilesMap.WaitFor(m_BackupFileName);
    
	// Since the reading from the file failed, try the backup file
	try
    {
		// Open the file for write
		OSFile.CreateFile(m_BackupFileName,"rb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(m_BackupFileName);
		throw Err;
	}

	File = OSFile.toSTDIO();

	if (File != NULL)
	{
		// Find the size of the file
		fseek(File, 0, SEEK_END);
		long FileSize = ftell(File) - sizeof(unsigned long); // The four last bytes of the file are the check sum
		fseek(File, 0, SEEK_SET);

		m_FileBuffer.resize(FileSize);

		// Read all the file except the 4 bytes of the check sum
		size_t NumOfReadBytes = fread(&(*m_FileBuffer.begin()),sizeof(BYTE),FileSize,File);
		if ((long)NumOfReadBytes < FileSize)
		{
            fclose(File);
			MutexFilesMap.Release(m_BackupFileName);
			throw EQFileWithCheckSum("Error while reading from the file");
		}

		NumOfReadBytes = fread(&FileCS,sizeof(unsigned long),1,File);
		if (NumOfReadBytes < 1)
		{
            fclose(File);
			MutexFilesMap.Release(m_BackupFileName);
			throw EQFileWithCheckSum("Error while reading from the file");
		}

		fclose(File);
		MutexFilesMap.Release(m_BackupFileName);

		if (CalculateCheckSum(&(*m_FileBuffer.begin()), FileSize) == FileCS)
		{
			m_FileState = fsOpenRead;
			return Q_NO_ERROR;
		}
	}
    else
    {
       MutexFilesMap.Release(m_BackupFileName);
    }

	return Q_OPEN_FILE_AND_BACKUP_FILE_ERR; 
}

// Read from the file memory buffer to 'Data', if end of "file", return false
bool CQFileWithCheckSum::Read(void *Data, unsigned long DataLength)
{
  if (m_FileState != fsOpenRead)
    throw EQFileWithCheckSum("File is not open for reading");

  if(m_CurrFileIndex < m_FileBuffer.size())
  {
    memcpy(Data, &(*m_FileBuffer.begin()) + m_CurrFileIndex, DataLength);
    m_CurrFileIndex += DataLength;
    return true;
  }

  return false;
}

// Close the file and compare the checksum
void CQFileWithCheckSum::EndRead()
{
  if (m_FileState != fsOpenRead)
    throw EQFileWithCheckSum("File is not open for writing");

  m_FileState = fsClose;
  m_FileBuffer.resize(0);
}

// Read the file into 'Data' and calculate check sum, if failed try again with the backup file
TQErrCode CQFileWithCheckSum::LoadFromFile(QString FileName, QString BackupFileName, BYTE* Data, unsigned long DataLength)
{
	unsigned long FileCS;
	QOSFileCreator OSFile;

    MutexFilesMap.InsertFileMutex(FileName);//Creating and inserting a mutex for this specific file(if not inserted already)

    //Entering the critical section.
	MutexFilesMap.WaitFor(FileName);

	try
    {
		// Open the file for write
		OSFile.CreateFile(FileName,"rb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(FileName);
		throw Err;
	}

	FILE *File = OSFile.toSTDIO();

	if (File != NULL)
	{
		fread(Data,sizeof(BYTE),DataLength,File);

		fread(&FileCS,sizeof(unsigned long),1,File);

		fclose(File);

		MutexFilesMap.Release(FileName);

		if (CalculateCheckSum(Data, DataLength) == FileCS)
		{
			return Q_NO_ERROR;
		}
	}
    else
    {
        MutexFilesMap.Release(FileName);
    }                                                     

    //----------------------------------------------------
    MutexFilesMap.InsertFileMutex(BackupFileName);//Creating and inserting a mutex for this specific file(if not inserted already)

	//Entering the critical section.
    MutexFilesMap.WaitFor(BackupFileName);

	try
    {
		// Open the file for write
		OSFile.CreateFile(BackupFileName,"rb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(BackupFileName);
		throw Err;
	}

	File = OSFile.toSTDIO();

	if (File != NULL)
	{
		fread(Data,sizeof(BYTE),DataLength,File);

		fread(&FileCS,sizeof(unsigned long),1,File);

		fclose(File);

        MutexFilesMap.Release(BackupFileName);
        
		if (CalculateCheckSum(Data, DataLength) == FileCS)
		{
			return Q_NO_ERROR;
		}
	}
    else
    {
        MutexFilesMap.Release(BackupFileName);
    }
	return Q_OPEN_FILE_AND_BACKUP_FILE_ERR;
}

// The same as the previous, the backup file name is identical to the 'FileName' but with '.bak' extension
TQErrCode CQFileWithCheckSum::LoadFromFile(QString FileName, BYTE* Data, unsigned long DataLength)
{
  QString BackupFileName = FileName;
  BackupFileName.replace(BackupFileName.find_last_of(".")+1,3,"bak");

  return LoadFromFile(FileName, BackupFileName, Data, DataLength);
}

// Write 'Data' to the file 'FileName' and to the backup file with the calculated check sum
TQErrCode CQFileWithCheckSum::SaveToFile(QString FileName, QString BackupFileName, BYTE* Data, unsigned long DataLength)
{
	QOSFileCreator OSFile;

	TQErrCode RetVal = Q_NO_ERROR;
	unsigned long CheckSum = CalculateCheckSum(Data, DataLength);

    MutexFilesMap.InsertFileMutex(FileName);//Creating and inserting a mutex for this specific file(if not inserted already)

	//Entering the critical section.
	MutexFilesMap.WaitFor(FileName);

	try
    {
		// Open the file for write
		OSFile.CreateFile(FileName,"wb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(FileName);
		throw Err;
	}


	FILE *File = OSFile.toSTDIO();
	if (File != NULL)
	{
		fwrite(Data,sizeof(BYTE),DataLength,File);

		fwrite(&CheckSum,sizeof(unsigned long),1,File);

		fclose(File);
		MutexFilesMap.Release(FileName);
	}
	else
    {
        MutexFilesMap.Release(FileName);
		RetVal = Q_OPEN_FILE_ERR;
    }

    //---------------------------------------------------------
    MutexFilesMap.InsertFileMutex(BackupFileName);//Creating and inserting a mutex for this specific file(if not inserted already)

	//Entering the critical section.
	MutexFilesMap.WaitFor(BackupFileName);

	try
    {
		// Open the file for write
		OSFile.CreateFile(BackupFileName,"wb");
	}
	catch(EQOSFileCreator& Err)
	{
        MutexFilesMap.Release(BackupFileName);
		throw Err;
	}

	File = OSFile.toSTDIO();

	if (File != NULL)
	{
		fwrite(Data,sizeof(BYTE),DataLength,File);

		fwrite(&CheckSum,sizeof(unsigned long),1,File);

		fclose(File);
		MutexFilesMap.Release(BackupFileName);
	}
	else
	{
        MutexFilesMap.Release(BackupFileName);
        
		if (RetVal == Q_OPEN_FILE_ERR)
			RetVal = Q_OPEN_FILE_AND_BACKUP_FILE_ERR;
		else
			RetVal = Q_OPEN_BACKUP_FILE_ERR;
	}

	return RetVal;
}

// The same as the previous, the backup file name is identical to the 'FileName' but with '.bak' extension
TQErrCode CQFileWithCheckSum::SaveToFile(QString FileName, BYTE* Data, unsigned long DataLength)
{
  QString BackupFileName = FileName;
  BackupFileName.replace(BackupFileName.find_last_of(".")+1,3,"bak");

  return SaveToFile(FileName, BackupFileName, Data, DataLength);

}


unsigned long CQFileWithCheckSum::CalculateCheckSum(BYTE *Buffer, unsigned long Size)
{
  unsigned long CheckSum = 0;

  for (unsigned long i = 0; i < Size; i++)
    CheckSum += Buffer[i];

  return CheckSum;
}
