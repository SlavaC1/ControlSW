//---------------------------------------------------------------------------
#ifndef ReportFileH
#define ReportFileH

#include "QUtils.h"
#include "QStringList.h"
#include "ReaderDefs.h"

class CReportBase 
{
private:

	QString       m_FilePath;
	QString       m_Title;
	CQStringList *m_StringList;
	TTestData     m_TestData;

protected:

	QString   GetMachineName();
	QString   GetDate();
	TTestData GetData();
	void      Append(const QString &Text);

public:

	CReportBase(const QString &FilePath, const TTestData &TestData);
	virtual ~CReportBase();

	virtual void Generate() = 0; 
};

class CReportTXT : public CReportBase
{
private:

	void WriteLine();
	void SkipLines(int LinesToSkip);

public:
	CReportTXT(const QString &FilePath, const TTestData &TestData);
	~CReportTXT();

	void Generate();
};

// Not yet implemented
class CReportXML : public CReportBase
{
private:
public:

	CReportXML(const QString &FilePath, const TTestData &TestData) : CReportBase (FilePath, TestData){}
	~CReportXML(){}
	void Generate(){}
};

//---------------------------------------------------------------------------
#endif
