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
 * Last upate: 21/09/2003                                           *
 ********************************************************************/

#include <vcl\SysUtils.hpp>
#include "TestGroup.h"
#include "HTML_BITReport.h"
#include "QStringList.h"
#include "Q2RTApplication.h"
#include "QVersionInfo.h"
#include "GlobalDefs.h"
#include "FEResources.h"

// Utility fuction for extarcting the group and test name from a string (e.g. MyGroup|MyTest)
QString TestResultToStr(TTestResult TestResult);


// Constructor
CHTML_BITReport::CHTML_BITReport(const QString ReportFileName) : CBITReport(ReportFileName)
{
  m_StrList = NULL;
}

// Destructor
CHTML_BITReport::~CHTML_BITReport(void)
{
  if(m_StrList != NULL)
    delete m_StrList;
}

// Convert a test result to colored HTML text
QString CHTML_BITReport::TestResultToHTMLStr(TTestResult TestResult)
{
  QString TestStr = TestResultToStr(TestResult);

  QString ColorStr;

  switch(TestResult)
  {
    case trGo:
      //ColorStr = "\"#00ff00\"";
      ColorStr = "\"#1DC35F\"";
      break;

    case trNoGo:
      ColorStr = "\"#ff0000\"";
      break;

    case trWarning:
      ColorStr = "\"#C3C12D\"";
      break;

    default:
      ColorStr = "\"#6600cc\"";
      break;
  }

  return ("<font color=" + ColorStr + ">" + TestStr + "</font>");
}

struct THTMLSpecialCharReplace {
  char SpecialChar;
  char ReplaceStr[8];
};

static THTMLSpecialCharReplace SpecialCharsLookup[]={{'<',"&lt;"},
                                                     {'>',"&gt;"},
                                                     {'&',"&amp;"},
                                                     {'\"',"&quot;"},
                                                     {'\n',"<br>"}};

static int SearchSpecialCharsLookup(char c)
{
  for(unsigned int i = 0; i < sizeof(SpecialCharsLookup) / sizeof(THTMLSpecialCharReplace); ++i)
    if(c == SpecialCharsLookup[i].SpecialChar)
      return i;

  return -1;
}

static unsigned CountSpecialChars(const QString Text)
{
  unsigned r = 0;

  for(unsigned i = 0; i < Text.length(); i++)
    if(SearchSpecialCharsLookup(Text[i]) != -1)
      r++;

  return r;
}

// Convert a string to HTML compatible string
QString CHTML_BITReport::TextToHTMLText(const QString Text)
{
  QString Result;

  Result.reserve(CountSpecialChars(Text) * 6 + Text.length());

  for(unsigned i = 0; i < Text.length(); i++)
  {
    char c = Text[i];

    int j = SearchSpecialCharsLookup(c);
    if(j != -1)
    {
      QString RepStr = SpecialCharsLookup[j].ReplaceStr;
      Result += RepStr;
    }
    else
      Result.append(1,c);
  }

  return Result;
}

// Create report file header (override)
void CHTML_BITReport::CreateHeader(TTestResult CombinedResults)
{
  if(m_StrList != NULL)
    delete m_StrList;

  m_StrList = new CQStringList;

  // Add title
  m_StrList->Add("<title>"+ QAPPLICATION_NAME +" BIT Report</title>");

  // Add Header
  m_StrList->Add("<font color=\"#0000ff\">");
  m_StrList->Add("<h1>Stratasys.</h1>");
  m_StrList->Add("</font>");

  m_StrList->Add("<h2><b>" + QAPPLICATION_NAME + " Built-In-Tests Report File</b></h2>");

  m_StrList->Add("<table border=\"0\">");
  m_StrList->Add("<tr><td>Report Date and Time</td><td>&nbsp;&nbsp;-&nbsp;&nbsp;" + TextToHTMLText(QDateTimeToStr(QGetCurrentTime()))
                  + "</td></tr>");
  m_StrList->Add("<tr><td>Machine Type</td><td>&nbsp;&nbsp;-&nbsp;&nbsp;" + QAPPLICATION_NAME + "</td></tr>");
  m_StrList->Add("<tr><td>Computer Name</td><td>&nbsp;&nbsp;-&nbsp;&nbsp;" + TextToHTMLText(Q2RTApplication->ComputerName.Value()) +
                 "</td></tr>");
  m_StrList->Add("<tr><td>App Version</td><td>&nbsp;&nbsp;-&nbsp;&nbsp;" + TextToHTMLText(QGetAppVersionStr()) + "</td></tr>");
  m_StrList->Add("<tr><td>Combined Results</td><td>&nbsp;&nbsp;-&nbsp;&nbsp;" + TestResultToHTMLStr(CombinedResults) + "</td></tr>");
  m_StrList->Add("</table>");

  // Add horizontal line
  m_StrList->Add("<hr width=\"100%\" size=\"2\"><br>");
}

// Add test entry (override)
void CHTML_BITReport::AddTestReport(CTestGroup *TestGroup,CSingleTest *Test)
{
  // Add test name
  m_StrList->Add("<b>" + TestGroup->Name() + " | " + Test->Name() + "</b><br>");

  // Test summary
  m_StrList->Add("<table border=\"0\">");
  m_StrList->Add("<tr><td>Test Result</td><td>  - " + TestResultToHTMLStr(Test->GetLastResult()) + "</td></tr>");

  QString Summary = Test->GetLastResultErrorDescription();

  // Add result summary if different then blank
  if(Summary != "")
    m_StrList->Add("<tr><td>Result Summary</td><td>  - " + TextToHTMLText(Summary) + "</td></tr>");

  m_StrList->Add("</table>");

  unsigned StepsNum = Test->BeginResultExEnumeration();

  if(StepsNum > 0)
  {
    // Steps table
    m_StrList->Add("<table border=\"1\" cellpadding=\"4\">");
    m_StrList->Add("<tr><th>#</th><th>Step</th><th>Requested</th><th>Actual</th><th>Description</th></tr>");

    for(unsigned i = 0; i < StepsNum; i++)
    {
      QString StepName = TextToHTMLText(Test->GetStepName());
      QString Requested = TextToHTMLText(Test->GetStepRequestedValue());
      QString Actual = TextToHTMLText(Test->GetStepActualValue());
      QString Description = TextToHTMLText(Test->GetStepDescription());

      if(StepName == "")
        StepName = "&nbsp;";

      if(Requested == "")
        Requested = "&nbsp;";

      if(Actual == "")
        Actual = "&nbsp;";

      if(Description == "")
        Description = "&nbsp;";

      m_StrList->Add("<tr><td>" + QIntToStr(i + 1) + "</td><td>" + StepName + "</td><td>" + Requested +
                     "</td><td>" + Actual + "</td><td>" + Description + "</td>");

      Test->MoveToNextResultEx();
    }
    
    m_StrList->Add("</table>");
  }

  // Add horizontal line
  m_StrList->Add("<hr width=\"100%\" size=\"2\"><br>");
}
/**/
// Add test entry (override)
void CHTML_BITReport::AddTestReportWithoutHTML(CTestGroup *TestGroup,CSingleTest *Test)
{
  // Add test name
  m_StrList->Add("<b>" + TestGroup->Name() + " | " + Test->Name() + "</b><br>");

  // Test summary
  m_StrList->Add("<table border=\"0\">");
  m_StrList->Add("<tr><td>Test Result</td><td>  - " + TestResultToHTMLStr(Test->GetLastResult()) + "</td></tr>");

  QString Summary = Test->GetLastResultErrorDescription();

  // Add result summary if different then blank
  if(Summary != "")
    m_StrList->Add(Summary );

  m_StrList->Add("</table>");

/*  unsigned StepsNum = Test->BeginResultExEnumeration();

  if(StepsNum > 0)
  {
    // Steps table
    m_StrList->Add("<table border=\"1\" cellpadding=\"4\">");
    m_StrList->Add("<tr><th>#</th><th>Step</th><th>Requested</th><th>Actual</th><th>Description</th></tr>");

    for(unsigned i = 0; i < StepsNum; i++)
    {
      QString StepName = TextToHTMLText(Test->GetStepName());
      QString Requested = TextToHTMLText(Test->GetStepRequestedValue());
      QString Actual = TextToHTMLText(Test->GetStepActualValue());
      QString Description = TextToHTMLText(Test->GetStepDescription());

      if(StepName == "")
        StepName = "&nbsp;";

      if(Requested == "")
        Requested = "&nbsp;";

      if(Actual == "")
        Actual = "&nbsp;";

      if(Description == "")
        Description = "&nbsp;";

      m_StrList->Add("<tr><td>" + QIntToStr(i + 1) + "</td><td>" + StepName + "</td><td>" + Requested +
                     "</td><td>" + Actual + "</td><td>" + Description + "</td>");

      Test->MoveToNextResultEx();
    }
    
    m_StrList->Add("</table>");
  }
  */

  // Add horizontal line
  m_StrList->Add("<hr width=\"100%\" size=\"2\"><br>");
}
/**/
// Create report file footer (override)
void CHTML_BITReport::CreateFooter(void)
{
  m_StrList->SaveToFile(m_ReportFileName);
}
