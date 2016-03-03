//---------------------------------------------------------------------------
#pragma hdrstop
#include "ReportFile.h"
#pragma package(smart_init)

#include <vcl\SysUtils.hpp>

#define MAX_DATE_SIZE 32

CReportBase::CReportBase(const QString &FilePath, const TTestData &TestData)
{
	// If file doesn't exist, create it
	if (DirectoryExists(ExtractFilePath(FilePath.c_str())) == false)
		ForceDirectories(ExtractFilePath(FilePath.c_str())); // Force Directory creation

	m_FilePath = FilePath;
	m_TestData = TestData;

	try
	{
		m_StringList = new CQStringList(m_FilePath);
	}
	catch(EQStringList &err)
	{
        // If file does not exist
		m_StringList = new CQStringList;
		m_StringList->SaveToFile(m_FilePath,"wt");
	}
}

CReportBase::~CReportBase()
{
    m_FilePath.clear(); 
	m_StringList->Clear();
	Q_SAFE_DELETE(m_StringList);
}

// Append text to a report file
void CReportBase::Append(const QString &Text)
{
    m_StringList->Clear();
	m_StringList->Add(Text);
	m_StringList->SaveToFile(m_FilePath,"at");
}

// Get the name of the PC as string
QString CReportBase::GetMachineName()
{
	char CompNameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
	ULONG CompNameBufferLength = sizeof(CompNameBuffer);

	// Get computer name
	if(::GetComputerName(CompNameBuffer,&CompNameBufferLength) != 0)
	{
		CompNameBuffer[CompNameBufferLength + 1] = NULL;
		return QString(CompNameBuffer);
	}
	else
		return QString("__Unknown__");
}

// Get current date and time as string
QString CReportBase::GetDate()
{
	 // Prepare the date/time string
	char DateTimeStr[MAX_DATE_SIZE];
	time_t CurrentTime     = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr, MAX_DATE_SIZE, "%d/%m/%Y %X", LocalTimeTM);

	return QFormatStr("%s", DateTimeStr);
}

TTestData CReportBase::GetData()
{
	return m_TestData;
}

//-----------------------------------------------------------------------------

CReportTXT::CReportTXT(const QString &FilePath, const TTestData &TestData) : CReportBase (FilePath, TestData)
{
	Generate();
}

CReportTXT::~CReportTXT()
{
}

// Generate the TXT report content
void CReportTXT::Generate()
{
	TTestData Data = GetData();
	QString TestHeader = QFormatStr("Machine: %s, Date: %s, Iterations: %d, Selected container: %s, Pass/Fail criteria: %d%", GetMachineName().c_str(),
																															  GetDate().c_str(),
																															  Data.TestingCycles,
																															  ContainerNames[Data.SelectedContainer].c_str(),
																															  Data.PassFailCriteria);
    SkipLines(1);
    Append(TestHeader);
	WriteLine();

	for(int i = 0; i < NUM_OF_CONTAINERS; i++)
	{
		int ReadingError = Data.TestResults.Percentage[i];
		QString text     = "";

        if(Data.TestResults.ResultsPerTag[i].empty())
		{
        	text = QFormatStr("%s:\tTag ID - %d,\tCounts - %d", ContainerNames[i].c_str(), 0, 0);
			text += QString(",\tReading error - ---"); // This split is because of a weird QFormatStr bug
			Append(text);
		}
		else
		{
            TResultsTable::iterator it = Data.TestResults.ResultsPerTag[i].begin();
			for(; it != Data.TestResults.ResultsPerTag[i].end(); ++it)
			{
				text = QFormatStr("%s:\tTag ID - %I64d,\tCounts - %d", ContainerNames[i].c_str(), it->first, it->second);
				text += (ReadingError != 0) ? QFormatStr(",\tReading error - %s%", QIntToStr(ReadingError).c_str()) : QString(",\tReading error - ---"); // This split is because of a weird QFormatStr bug
				Append(text);
			}
		}
	}

    SkipLines(1);
	QString TestResult = QFormatStr("Test result: %s",((Data.TestResults.Passed) ? "OK" : "Failed"));
	Append(TestResult);

}

void CReportTXT::WriteLine()
{
	Append("---------------------------------------------------------------------------------------------------------");
}

void CReportTXT::SkipLines(int LinesToSkip)
{
	for(int i = 0; i < LinesToSkip; i++)
    	Append("\n");
}
