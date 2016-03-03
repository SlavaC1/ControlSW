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

#ifndef _BIT_REPORT_H_
#define _BIT_REPORT_H_

#include "QObject.h"
#include "BITDefs.h"

class CSingleTest;
class CTestGroup;


// Base class for BIT report
class CBITReport : public CQObject {
  protected:
    QString m_ReportFileName;

  public:
    CBITReport(const QString ReportFileName) {
      m_ReportFileName = ReportFileName;
    }

    // Create report file header
    virtual void CreateHeader(TTestResult CombinedResults) = 0;

    // Add test entry
    virtual void AddTestReport(CTestGroup *TestGroup,CSingleTest *Test) = 0;
    //Add test entry without HTMLTOTEXT
    virtual void AddTestReportWithoutHTML(CTestGroup *TestGroup,CSingleTest *Test) = 0;

    // Create report file footer
    virtual void CreateFooter(void) = 0;
};

#endif
