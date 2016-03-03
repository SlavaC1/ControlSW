//---------------------------------------------------------------------------

#ifndef CHTML_ReportFileH
#define CHTML_ReportFileH

#include "QStringList.h"

// Class for HTML BIT report
class CHTML_ReportFile
{
  protected:
	QString m_ReportFileName;
	CQStringList *m_StrList;

	// Create report file header
	void CreateHeader(QString Title);

	// Create report file footer
	void CreateFooter(char * mode);

  public:
	// Constructor
	CHTML_ReportFile(const QString FileName, const QString DirName);
	// Destructor
	~CHTML_ReportFile();
	
	virtual TQErrCode Init() = 0;

	// Create the report file
	//void CreateActionsHistoryReport();

	//open the report file and show it to user
	virtual void OpenReportFile();
};
//---------------------------------------------------------------------------
#endif
