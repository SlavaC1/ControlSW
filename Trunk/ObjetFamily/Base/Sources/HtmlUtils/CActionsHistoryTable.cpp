//---------------------------------------------------------------------------
#pragma hdrstop

#include "CActionsHistoryTable.h"
#include "Q2RTApplication.h"
#include "Q2RTWizard.h"

CActionsHistoryTable *CActionsHistoryTable::m_this = NULL;

void CActionHistoryData::SetDate(QString eventDate)
{
   m_MostRecentDatesArr[m_eventDateIndex] = eventDate;
   m_eventDateIndex = (m_eventDateIndex+1)%NUM_OF_MOST_RECENT_DATES;
   m_currNumOfDates++;
}

int CActionHistoryData::GetCurrentDateIndex() const
{
  if(m_currNumOfDates>=NUM_OF_MOST_RECENT_DATES)
	return m_eventDateIndex;
  else
	return 0;
}

QString CActionHistoryData::GetCurrentDate() const
{
   int currIndex = (m_eventDateIndex+(NUM_OF_MOST_RECENT_DATES-1))%NUM_OF_MOST_RECENT_DATES;
   return m_MostRecentDatesArr[currIndex];
}

void CActionsHistoryTable::Init()
{
  int actionIndex=0;
  m_ActionIndexMap[WIZARD_TITLE(IDS_RR_WIZARD)] = actionIndex++;
  //m_ActionIndexMap[WIZARD_TITLE(IDS_WIPER_CLEANING_WIZARD)] = actionIndex++;/// wiper cleaning
  m_ActionIndexMap[WIZARD_TITLE(IDS_HEADS_CLEANING_WIZARD)] = actionIndex++;/// Heads cleaning
//  m_ActionIndexMap[WIZARD_TITLE(IDS_UV_TESTING_CALIBRATION_WITH_EXTERNAL_DEVICE)] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_UVC_WIZARD)] = actionIndex++;  ///UV calibration (UVM)
  m_ActionIndexMap[WIZARD_TITLE(IDS_UVR_WIZARD)] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_XO_WIZARD)] = actionIndex++;
  m_ActionIndexMap["Head Optimization Wizard HQ"] = actionIndex++;
  m_ActionIndexMap["Head Optimization Wizard HS/HM"] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_SP_WIZARD)] = actionIndex++;
  m_ActionIndexMap["BIT"] = actionIndex++;   ///BIT
  m_ActionIndexMap["Pattern Test"] = actionIndex++;   ///Pattern test
  m_ActionIndexMap[WIZARD_TITLE(IDS_SHD_WIZARD)] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_HEAD_FILLING_CALIBRATION_WIZARD)] = actionIndex++;  ///Head filling calibration
  m_ActionIndexMap[WIZARD_TITLE(IDS_BC_WIZARD)] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_PP_WIZARD)] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_ROLLER_TILT_WIZARD)] = actionIndex++;  ///Roller tilt
  m_ActionIndexMap[WIZARD_TITLE(IDS_TRAY_POINTS_CALIBRATION_WIZARD)] = actionIndex++;  ///Tray calibration
  m_ActionIndexMap[WIZARD_TITLE(IDS_VC_WIZARD)] = actionIndex++;
  m_ActionIndexMap[WIZARD_TITLE(IDS_WC_WIZARD)] = actionIndex++;
  m_ActionIndexMap["Transparency HS"] = actionIndex++; //19

  if(actionIndex!=NUM_OF_ACTIONS)
	CQLog::Write(LOG_TAG_OBJET_MACHINE,"HistoryTable size doesn't match NUM_OF_ACTIONS value: %d", actionIndex);
  map<QString,int>::iterator Itr;
  for(Itr=m_ActionIndexMap.begin(); Itr!=m_ActionIndexMap.end(); ++Itr)
  {
	SetActionName((*Itr).first, (*Itr).second);
	SetActionID((*Itr).second,((*Itr).second)+1); //((actionIndex, id)
  }

  m_actionsHistoryReport = new CHTML_ActionsHistoryReport;
  m_actionsHistoryReport->Init();
}

CActionsHistoryTable* CActionsHistoryTable::GetInstance()
{
	if(m_this == NULL)
		m_this = new CActionsHistoryTable();
	return m_this;
}

void CActionsHistoryTable::DeInit()
{
	Q_SAFE_DELETE(m_this);
}

CActionsHistoryTable::~CActionsHistoryTable()
{
  Q_SAFE_DELETE(m_actionsHistoryReport);
}

void CActionsHistoryTable::ShowMachineActionsReport()
{
  m_actionsHistoryReport->OpenReportFile();
}

void CActionsHistoryTable::PrintActionsHistoryTable()
{
  m_actionsHistoryReport->CreateActionsHistoryReport();
}

bool CActionsHistoryTable::AddDate(QString actionName)
{
   // Prepare the date/time string
	char DateTimeStr[MAX_DATE_SIZE];
	time_t CurrentTime = time(NULL);
	struct tm *LocalTimeTM = localtime(&CurrentTime);
	strftime(DateTimeStr,MAX_DATE_SIZE,"%d/%m/%y (%X %p)",LocalTimeTM);
	QString date = DateTimeStr;

	if (m_ActionIndexMap.find(actionName) == m_ActionIndexMap.end())
		return false; //action wasn't found

	int actionIndex = m_ActionIndexMap[actionName];
   /*	if(actionIndex<0 || actionIndex>=NUM_OF_ACTIONS)
	  return false; //action wasn't found */
	m_actionsHistoryArr[actionIndex].SetDate(date);
	PrintActionsHistoryTable();
	return true;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
