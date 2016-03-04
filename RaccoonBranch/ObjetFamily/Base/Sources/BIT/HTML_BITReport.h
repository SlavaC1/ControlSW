/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Built-In-Tests managment                                 *
 * Module Description: BIT Report base class.                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 18/08/2003                                           *
 * Last upate: 18/08/2003                                           *
 ********************************************************************/

#ifndef _HTML_BIT_REPORT_H_
#define _HTML_BIT_REPORT_H_

#include "BITReport.h"


class CQStringList;

// Class for HTML BIT report
class CHTML_BITReport : public CBITReport {
  private:
    CQStringList *m_StrList;

    // Convert a test result to colored HTML text
    QString TestResultToHTMLStr(TTestResult TestResult);

    // Convert a string to HTML compatible string
    QString TextToHTMLText(const QString Text);

  public:
    // Constructor
    CHTML_BITReport(const QString ReportFileName);

    // Destructor
    ~CHTML_BITReport(void);

    // Create report file header (override)
    void CreateHeader(TTestResult CombinedResults);

    // Add test entry (override)
    void AddTestReport(CTestGroup *TestGroup,CSingleTest *Test);

    //Add test entry without HTMLTO TEXT(override)
    void AddTestReportWithoutHTML(CTestGroup *TestGroup,CSingleTest *Test);
    // Create report file footer (override)
    void CreateFooter(void);
};

#endif
