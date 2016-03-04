/*********************************************************************
 *                        Objet Geometries LTD.                      *
 *                        ---------------------                      *
 * Project: Objet.                                                 *
 *                                                                   *
 * Class description: Holds actions history data					 *
 *                                                                   *
 * Compilation: Standard C++.                                        *
 *                                                                   *
 * Author: Itamar Aharon                                             *
 * Start date: 18/07/2011                                            *
 * Last upate: 18/07/2011                                            *
 ********************************************************************/
//---------------------------------------------------------------------------

#ifndef CActionsHistoryTableH
#define CActionsHistoryTableH

#include "QTypes.h"
#include "GlobalDefs.h"
#include "CHTML_ActionsHistoryReport.h"
#include <map>

//Number of event to be saved for each operation
const int NUM_OF_MOST_RECENT_DATES = 3;
//Number of actions to track in the table
const int NUM_OF_ACTIONS = 20;
//Actions available for the user (in contrast to CSE actions)
//note: User actions are placed first
const int NUM_OF_USER_ACTIONS = 12;

//Holds the most recent dates of the action
class CActionHistoryData
{
private:
  QString m_actionName;
  QString m_MostRecentDatesArr[NUM_OF_MOST_RECENT_DATES];
  int m_eventDateIndex;
  int m_actionID;
  int m_currNumOfDates;

public:
  //Constructor
  CActionHistoryData():m_eventDateIndex(0), m_actionID(0), m_currNumOfDates(0) {}
  void SetActionID(int id) { m_actionID = id; }
  void SetActionName(QString actionName) { m_actionName = actionName; }
  void SetDate(QString eventDate);
  QString GetActionName() { return m_actionName; }
  int GetActionID() { return m_actionID; }
  QString GetCurrentDate();
  int GetCurrentDateIndex();
  QString GetDate(int eventIndex) { return m_MostRecentDatesArr[eventIndex]; }
};

//The class (singleton) handles all history data of all monitored actions
class CActionsHistoryTable
{
private:
   CActionHistoryData m_actionsHistoryArr[NUM_OF_ACTIONS];
   map<QString,int> m_ActionIndexMap;
   static CActionsHistoryTable* m_this;
   CHTML_ActionsHistoryReport * m_actionsHistoryReport;
  //constructor
  CActionsHistoryTable():m_actionsHistoryReport(NULL) {}
  //destructor
  ~CActionsHistoryTable();

public:
  static CActionsHistoryTable* GetInstance();
  void Init();
  void DeInit();
  void SetActionID(int actionIndex, int id) { m_actionsHistoryArr[actionIndex].SetActionID(id); }
  void SetActionName(QString actionName, int actionIndex) { m_actionsHistoryArr[actionIndex].SetActionName(actionName); }
  void SetDate(QString eventDate, int actionIndex) { m_actionsHistoryArr[actionIndex].SetDate(eventDate); }
  QString GetActionName(int actionIndex) { return m_actionsHistoryArr[actionIndex].GetActionName(); }
  int GetActionID(int actionIndex) { return m_actionsHistoryArr[actionIndex].GetActionID(); }
  QString GetCurrentDate(int actionIndex)
  {
	return m_actionsHistoryArr[actionIndex].GetCurrentDate();
  }
  GetCurrentDateIndex(int actionIndex)
  {
	return m_actionsHistoryArr[actionIndex].GetCurrentDateIndex();
  }
  QString GetDate(int actionIndex, int eventIndex)
  {
	return m_actionsHistoryArr[actionIndex].GetDate(eventIndex);
  }
  bool AddDate(QString actionName);
  void ShowMachineActionsReport();
  void PrintActionsHistoryTable();
};

//---------------------------------------------------------------------------
#endif
