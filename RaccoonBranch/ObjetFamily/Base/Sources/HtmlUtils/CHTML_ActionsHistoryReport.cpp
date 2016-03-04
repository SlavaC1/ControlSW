//---------------------------------------------------------------------------


#pragma hdrstop

#include "CHTML_ActionsHistoryReport.h"

//---------------------------------------------------------------------------
#include "Q2RTApplication.h"
#include "FEResources.h"
#include "AppLogFile.h"
#include "CActionsHistoryTable.h"
#include "About.h"

const char ACTIONS_HISTORY_FILE_NAME[]   = "ServiceTools\\Recent Maintenance Actions Performed\\RecentActions.htm";
const char ACTIONS_HISTORY_DIR_NAME[]   = "ServiceTools\\Recent Maintenance Actions Performed";

// Constructor
CHTML_ActionsHistoryReport::CHTML_ActionsHistoryReport():CHTML_ReportFile(ACTIONS_HISTORY_FILE_NAME, ACTIONS_HISTORY_DIR_NAME)
{}

TQErrCode CHTML_ActionsHistoryReport::Init()
{
  try
  {
	 if(FileExists(m_ReportFileName.c_str()))
	 {
	   m_StrList = new CQStringList(m_ReportFileName);
	   ReadActionHistoryData();
	   m_StrList->Clear();
	 }
	 else //first time, file doesn't exist
	   m_StrList = new CQStringList;
  }
  catch (std::bad_alloc& ba)
  {
	 m_StrList = NULL;
	 CQLog::Write(LOG_TAG_HOST_COMM,"CHTML_ActionsHistoryReport m_StrList - Memory allocation failed: %s",ba.what());
  }

  return Q_NO_ERROR;
}

// Create report file header (override)
void CHTML_ActionsHistoryReport::CreateActionsHistoryReport()
{
  CActionsHistoryTable * ActionsHistoryTable = CActionsHistoryTable::GetInstance();

  CreateHeader("Recent Maintenance Actions Report File");

  m_StrList->Add("<p style=\"color:green;text-align:left;margin:0\">User actions</p>");
  m_StrList->Add("<p style=\"color:blue;text-align:left;margin:0\">CSE actions</p>");

  m_StrList->Add("<table style=\"border-collapse:collapse\" border=\"1\" width=\"90%\" frame=\"box\">");
  //m_StrList->Add("<table border=\"1\" width=\"90%\" frame=\"box\">");

  m_StrList->Add("<col width=\"10\" align=\"left\"/>");
  m_StrList->Add("<col width=\"120\" align=\"left\"/>");
  m_StrList->Add("<col width=\"70\" align=\"center\"/>");
  m_StrList->Add("<col width=\"70\" align=\"center\"/>");
  m_StrList->Add("<col width=\"70\" align=\"center\"/>");

  //first row - headers
  m_StrList->Add("<tr>"); //start of writing columns headers
  m_StrList->Add("<th> Id </th>"); //first column header
  m_StrList->Add("<th> Maintenance Actions Performed </th>"); //second column header
  m_StrList->Add("<th colspan=\"" + QIntToStr(NUM_OF_MOST_RECENT_DATES) + "\">" + QIntToStr(NUM_OF_MOST_RECENT_DATES) +
						" Most Recent Dates</th>"); //second column header
  m_StrList->Add("</tr>");//end of writing columns headers

  for(int i=0;i<NUM_OF_ACTIONS;i++)
  {
	if(i<NUM_OF_USER_ACTIONS)
	  m_StrList->Add("<tr style=\"color:green\">");
	else
      m_StrList->Add("<tr style=\"color:blue\">");

	m_StrList->Add("<td>" + QIntToStr(ActionsHistoryTable->GetActionID(i)) + "</td>");
	m_StrList->Add("<td>" + ActionsHistoryTable->GetActionName(i) + "</td>");
	int dateIndex = ActionsHistoryTable->GetCurrentDateIndex(i);
	for(int j=0;j<NUM_OF_MOST_RECENT_DATES;j++)
	{
	   m_StrList->Add("<td>" + ActionsHistoryTable->GetDate(i,dateIndex) + "</td>");
	   //CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: HistoryTable %d: %s", i,ActionsHistoryTable->GetDate(i,dateIndex).c_str());
	   dateIndex = (dateIndex+1)%NUM_OF_MOST_RECENT_DATES;
	}
	m_StrList->Add("</tr>");
  }

  m_StrList->Add("</table>");

  m_StrList->Add("<p> Next PM type is: " + AboutBox->GetPMType() +  " due in " + AboutBox->GetPMDueIn() + " hours.</p>");

  CreateFooter("wt");
  m_StrList->Clear();
}
//Parse the stringList with its (HTML) action data, and retrieve the most recent dates.
//refer to SRS for table structure
void CHTML_ActionsHistoryReport::ReadActionHistoryData()
{
  CActionsHistoryTable * ActionsHistoryTable = CActionsHistoryTable::GetInstance();
  QString cellStr = "<td>";
  QString cellStr2 = "</td>";
  size_t pos=0,pos2=0;
  unsigned int startIndex=0; //the first row of action (action id)

  //find the first data cell
  for(; startIndex<m_StrList->Count();startIndex++)
  {
	if((pos = m_StrList->GetItem(startIndex).find(cellStr))!= QString::npos)
	{
	  CQLog::Write(LOG_TAG_OBJET_MACHINE,"Objet: First data cell %d: %s", startIndex,m_StrList->GetItem(startIndex).c_str());
	  break;
	}
  }
  //retrieve dates from table
  startIndex += 2;
  for(int i=0;i<NUM_OF_ACTIONS;i++)
  {
	for(int j=0;j<NUM_OF_MOST_RECENT_DATES;j++)
	{
		pos = m_StrList->GetItem(startIndex).find(cellStr);
		pos2 = m_StrList->GetItem(startIndex).find(cellStr2);
		if((pos+4)==pos2) //cell is empty (no dates for this action)
		{
		  startIndex += (NUM_OF_MOST_RECENT_DATES-j);
		  break;
		}
		QString resStr = m_StrList->GetItem(startIndex).substr(pos+4, pos2-pos-4);
		ActionsHistoryTable->SetDate(resStr,i);
		//CQLog::Write(LOG_TAG_OBJET_MACHINE,"=> %s",resStr.c_str());
		startIndex++;
	}
	startIndex += 4; //go to next row
  }
  return;
}

void CHTML_ActionsHistoryReport::OpenReportFile()
{
   CreateActionsHistoryReport();
   CHTML_ReportFile::OpenReportFile();
}

#pragma package(smart_init)
