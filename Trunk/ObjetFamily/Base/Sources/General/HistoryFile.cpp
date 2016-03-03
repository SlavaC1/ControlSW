//---------------------------------------------------------------------------
#pragma hdrstop
#include "HistoryFile.h"
#pragma package(smart_init)

#include "FEResources.h"
#include <vcl\SysUtils.hpp>
#include "Q2RTApplication.h"

CHistoryFile::CHistoryFile(const QString FileName)
{
	// If directory doesn't exist, create it
	QString OutputDir = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_SERVICETOOLS_DIRNAME) + FileName;
	if (DirectoryExists(OutputDir.c_str()) == false)
		ForceDirectories(OutputDir.c_str()); // Force Directory creation

	m_FilePath = OutputDir + "\\" + FileName + ".txt";

	try
	{
		m_StringList = new CQStringList(m_FilePath);
	}
	catch(EQStringList &err)
	{
        // If file does not exist
		m_StringList = new CQStringList;
		m_StringList->SaveToFile(m_FilePath,"wt");
		Append(QFormatStr(LOAD_STRING(IDS_HISTORY_RESTRICTIONS), FileName.c_str()));
	}

	SkipLines(1);
	WriteMachineName();
	WriteDate();
	WriteLine();
	SkipLines(1);
}

CHistoryFile::~CHistoryFile()
{
    m_FilePath.clear(); 
	m_StringList->Clear();
	Q_SAFE_DELETE(m_StringList);
}

void CHistoryFile::Append(const QString Text)
{
    m_StringList->Clear();
	m_StringList->Add(Text);
	m_StringList->SaveToFile(m_FilePath,"at");
}

void CHistoryFile::WriteMachineName()
{
	Append(QFormatStr("Machine name - %s", Q2RTApplication->ComputerName.Value().c_str()));
}

void CHistoryFile::WriteLine()
{
	Append("-------------------------------------------------------------------------------------");
}

void CHistoryFile::WriteDate()
{
     // Prepare the date/time string
	char DateTimeStr[MAX_DATE_SIZE];
	time_t CurrentTime = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr, MAX_DATE_SIZE, "%d/%m/%Y %X", LocalTimeTM);

	Append(QFormatStr("Date - %s", DateTimeStr));
}

void CHistoryFile::SkipLines(int LinesToSkip)
{
	for(int i = 0; i < LinesToSkip; i++)
    	Append("\n");
}
