
#include "PackageReader.h"
#include <stdio.h>
#include "hasp_api_cpp_.h"

CPackageReader::CPackageReader()
{
	m_FileBuffer = NULL; 
}

QString CPackageReader::DecryptDatabaseFile(QString FilePath)
{
	unsigned int FileSize = ReadFileIntoBuffer(FilePath.c_str());
	if(FileSize == 0)
        throw EPackageReader("The file is empty");

	int Feature = 0;
	Chasp hasp(ChaspFeature::fromFeature(Feature));
	try
	{
		haspStatus status = hasp.login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);

		if(HASP_SUCCEEDED(status))
		{
		   status = hasp.decrypt(m_FileBuffer, FileSize);
		   if(HASP_SUCCEEDED(status))
		   {
			   m_FileBuffer[FileSize] = '\0';
			   return reinterpret_cast<const char*>(m_FileBuffer);
		   }
		}
	}
	__finally
	{
		hasp.logout();
		delete[] m_FileBuffer;
	}

	return "";
}

unsigned int CPackageReader::ReadFileIntoBuffer(QString FilePath)
{
	FILE *File = fopen(FilePath.c_str(),"rb");

	try
	{
		if(File)
		{
			fseek(File, 0, SEEK_END);
			long FileSize = ftell(File);
			rewind(File);

			m_FileBuffer = new BYTE[FileSize];

			return fread(m_FileBuffer,sizeof(BYTE),FileSize,File);
		}
	}
	__finally
	{
		fclose(File);
	}

	return 0;
}

QString CPackageReader::ReadDatabaseFile(QString FilePath)
{
	AnsiString XML = DecryptDatabaseFile(FilePath).c_str();
	return XML.c_str();
}


