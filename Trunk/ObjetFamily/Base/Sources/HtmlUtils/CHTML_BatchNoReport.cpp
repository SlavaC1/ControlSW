//---------------------------------------------------------------------------


#pragma hdrstop

#include "CHTML_BatchNoReport.h"
#include "Q2RTApplication.h"
#include "QVersionInfo.h"
#include "GlobalDefs.h"
#include "FEResources.h"
#include "QLogFile.h"
#include "AppLogFile.h"
#include "CBatchNumberTable.h"

const char BATCH_NUMBER_FILE_NAME[]   = "Configs\\Material batch number.htm";

// Constructor
CHTML_BatchNoReport::CHTML_BatchNoReport(const QString ReportFileName)
{
  m_ReportFileName = ReportFileName;
  m_StrList = new CQStringList;
}

// Destructor
CHTML_BatchNoReport::~CHTML_BatchNoReport()
{
  Q_SAFE_DELETE(m_StrList);
}

// Create report file header (override)
void CHTML_BatchNoReport::CreateHeader()
{
  //m_StrList->Add("<html>");
  //m_StrList->Add("<body>");
  // Add title
  m_StrList->Add("<title>"+ QAPPLICATION_NAME +" Batch number report file</title>");

  // Add Header
  m_StrList->Add("<h1 style=\"color:blue\">Stratasys.</h1>");  //heading
}

// Create report file header (override)
void CHTML_BatchNoReport::CreateBatchNoReport()
{
   int NumOfBatches=0;
   CBatchNoTable * BatchNoTable = CBatchNoTable::GetInstance();
   if(!FileExists(m_ReportFileName.c_str()))
	 CreateHeader();

   m_StrList->Add("<p> Printing start: " + BatchNoTable->GetPrintStart() +  "</p>");
   m_StrList->Add("<p> Printing end: " + BatchNoTable->GetPrintEnd() + "</p>");
   m_StrList->Add("<p> Tray ID: " + BatchNoTable->GetTrayID() + "</p>");
   m_StrList->Add("<p> Printing mode: " + BatchNoTable->GetPrintingMode() + "</p>");

   m_StrList->Add("<STYLE TYPE=\"text/css\">TR{font-family:Arial;font-size:10pt;color:blue}</STYLE>");

  m_StrList->Add("<table border=\"1\" width=\"100%\">");

  for(int i=0;i<TOTAL_NUMBER_OF_CONTAINERS;i++)
  	m_StrList->Add("<col width=\"100\" />");


  m_StrList->Add("<tr>");
  for(int i=0;i<TOTAL_NUMBER_OF_CONTAINERS;i++)
  {
    m_StrList->Add("<th> Tank" + QIntToStr(i) + "</th>");
  }
  m_StrList->Add("</tr>");

  m_StrList->Add("<tr>");
  for(int i=0;i<TOTAL_NUMBER_OF_CONTAINERS;i++)
  {
	NumOfBatches = BatchNoTable->GetTankBatchNoIndex(i);
	  m_StrList->Add("<td>" + BatchNoTable->GetTankResinType(i) + "</br>");
	  for(int j=0;j<NumOfBatches;j++)
		m_StrList->Add("Batch#" + QIntToStr(j) + ": " + BatchNoTable->GetTankBatchNo(i,j).c_str() + "</br>");
			 //CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: BatchNumber %d: %s", i,batchNoArr[i].c_str());
	  m_StrList->Add("</td>");
  }
  m_StrList->Add("</tr>");
  m_StrList->Add("</table>");

  CreateFooter();
   m_StrList->Clear();
  BatchNoTable->ClearBatchNoTable();
}

// Create report file footer
void CHTML_BatchNoReport::CreateFooter()
{
  //m_StrList->Add("<body>");
  //m_StrList->Add("<html>");
  m_StrList->SaveToFile(m_ReportFileName, "at");
}

void CHTML_BatchNoReport::OpenReportFile()
{
  // View the report file
  if(m_ReportFileName != "")
	Q2RTApplication->LaunchFile(m_ReportFileName.c_str(),"");
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
