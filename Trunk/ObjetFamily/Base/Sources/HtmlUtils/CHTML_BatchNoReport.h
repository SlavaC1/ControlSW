/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Objet.                                                *
 *                                                                  *
 * Class description: Write batch number table to html file.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Itamar Aharon                                            *
 * Start date: 27/03/2011                                           *
 * Last upate: 27/03/2011                                           *
 ********************************************************************/

#ifndef _HTML_BATCH_NO_REPORT_H_
#define _HTML_BATCH_NO_REPORT_H_

#include "QStringList.h"

// Class for HTML BIT report
class CHTML_BatchNoReport {
  private:
    QString m_ReportFileName;
    CQStringList *m_StrList;

  public:
    // Constructor
    CHTML_BatchNoReport(const QString ReportFileName);
    // Destructor
    ~CHTML_BatchNoReport();

	// Create report file header
    void CreateHeader();

	// Create the report file
	void CreateBatchNoReport();

	// Create report file footer
	void CreateFooter();

	//open the report file and show it to user
	void OpenReportFile();
};

#endif
