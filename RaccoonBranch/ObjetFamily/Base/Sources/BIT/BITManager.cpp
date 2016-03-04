/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Built-In-Tests managment                                 *
 * Module Description: Manager object for collection of tests.      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 27/07/2003                                           *
 * Last upate: 25/08/2003                                           *
 ********************************************************************/

#include <algorithm>
#include "BITManager.h"
#include "Q2RTErrors.h"
#include "QStringList.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"
#include "HTML_BITReport.h"
#include "TrayHeater.h"
#include "HeadHeaters.h"
#include "Q2RTApplication.h"
#include "MachineSequencer.h"
#include "Door.h"
#include "Motor.h"
#include "HeadFilling.h"


QString TestResultToStr(TTestResult TestResult)
{
  QString ResultsStr;

  switch(TestResult)
  {
    case trGo:
      ResultsStr = "Go";
      break;

    case trNoGo:
      ResultsStr = "NoGo";
      break;

    case trWarning:
      ResultsStr = "Warning";
      break;

    default:
      ResultsStr = "Unknown";
  }

  return ResultsStr;
}

// Figure out which attributes are dependent (e.g. home axes will enable a door attribute)
TTestAttributes ResolveAttributesDependency(TTestAttributes TestAttributes)
{
  // Add door attribute
  if(TestAttributes & (taHomeAxes | taHeadsHeating | taTrayHeating | taHeadsFilling))
    TestAttributes |= taDoorLock;

  // Add head heating attribute
  if(TestAttributes & taHeadsFilling)
    TestAttributes |= taHeadsHeating;

  return TestAttributes;
}

// Class CBITManager implementation
// ********************************************************************

// Constructor
CBITManager::CBITManager(void) : CQComponent("BITManager")
{
  INIT_METHOD(CBITManager,DoTest);
  INIT_METHOD(CBITManager,BeginTestGroupsEnumeration);
  INIT_METHOD(CBITManager,GetNextTestGroup);
  INIT_METHOD(CBITManager,BeginTestsEnumeration);
  INIT_METHOD(CBITManager,GetNextTest);
  INIT_METHOD(CBITManager,GetNextTestAttributes);
  INIT_METHOD(CBITManager,GetCurrentTestStepsNum);
  INIT_METHOD(CBITManager,GetLastTestResult);
  INIT_METHOD(CBITManager,GetLastTestErrorDescription);
  INIT_METHOD(CBITManager,BeginTestResultExEnumeration);
  INIT_METHOD(CBITManager,GetNextTestResultEx);
  INIT_METHOD(CBITManager,ResetTest);
  INIT_METHOD(CBITManager,ResetAllTests);
  INIT_METHOD(CBITManager,BeginBITSetsEnumeration);
  INIT_METHOD(CBITManager,GetNextBITSet);
  INIT_METHOD(CBITManager,BeginTestsInSetEnumeration);
  INIT_METHOD(CBITManager,GetNextTestInSet);
  INIT_METHOD(CBITManager,RunBITSet);
  INIT_METHOD(CBITManager,CreateReportFile);
  INIT_METHOD(CBITManager,GetCombinedResults);
  INIT_METHOD(CBITManager,StartBITSetDefine);
  INIT_METHOD(CBITManager,AddToBITSet);
  INIT_METHOD(CBITManager,SaveBITSet);
  INIT_METHOD(CBITManager,StartTest);
  INIT_METHOD(CBITManager,ExecNextStep);
  INIT_METHOD(CBITManager,EndTest);
  INIT_METHOD(CBITManager,GetCurrentStepName);

  m_CurrentGroupIndex = 0;
  m_CurrentTestID = 0;
  m_CurrentGroup = NULL;
  m_LastGroupUsed = NULL;
  m_TempBITSet = NULL;
  m_CurrentTest = NULL; 
}

// Destructor
CBITManager::~CBITManager(void)
{
  Clear();
  ClearBITSets();
}

// Find a test group (throw exception if not found)
CTestGroup *CBITManager::FindGroup(const QString GroupName)
{
  for(unsigned i = 0; i < m_GroupsList.size(); i++)
	if(m_GroupsList[i]->Name() == GroupName)
      return m_GroupsList[i];

  throw EBITManager("Invalid test group name \"" + GroupName + "\"");
}

void CBITManager::RegisterTestGroup(CTestGroup *TestGroup)
{
  m_GroupsList.push_back(TestGroup);
}

// Do a single test by its group name and test name
TQErrCode CBITManager::DoTest(QString TestGroupName,QString TestName)
{
  // Execute test
  CTestGroup *TestGroup = FindGroup(TestGroupName);
  CSingleTest *Test = TestGroup->GetTest(TestName);

  // If the group changed, clear the "LastGroup" flag
  if(TestGroup != m_LastGroupUsed)
  {
    m_LastGroupUsed = TestGroup;
    m_LastGroupUsed->SetLastGroup(false);
  }

  Test->DoTest();

  return Q_NO_ERROR;
}

// Begin a group test by its name
TQErrCode CBITManager::BeginTestGroupsEnumeration(void)
{
  m_CurrentGroupIndex = 0;
  m_CurrentTestID = 0;
  return Q_NO_ERROR;
}

// Get the next group name
QString CBITManager::GetNextTestGroup(void)
{
  // Check if we are done
  if(m_CurrentGroupIndex < m_GroupsList.size())
  {
    m_CurrentGroupIndex++;
    return m_GroupsList[m_CurrentGroupIndex - 1]->Name();
  }

  return "";
}

// Begin test enumeration in specific group
TQErrCode CBITManager::BeginTestsEnumeration(QString GroupName)
{
  m_CurrentGroup = NULL;
  m_CurrentTestID = 0;

  // Find the given test group
  m_CurrentGroup = FindGroup(GroupName);
  return Q_NO_ERROR;
}

// Get the next test in current group
QString CBITManager::GetNextTest(void)
{
  // Check if we are done
  if(m_CurrentTestID < m_CurrentGroup->GetTestsNum())
  {
    m_CurrentTest = m_CurrentGroup->GetTest(m_CurrentTestID);

    m_CurrentTestID++;

    // Return test name
    return m_CurrentTest->Name();
  }

  return "";
}

unsigned CBITManager::GetNextTestAttributes(void)
{
  return m_CurrentTest->GetAttributes();
}

// Return last test result code
TTestResult CBITManager::GetLastTestResult(void)
{
  if(m_CurrentTest != NULL)
    return m_CurrentTest->GetLastResult();

  return trUnknown;
}

// Return last test error description string
QString CBITManager::GetLastTestErrorDescription(void)
{
  if(m_CurrentTest != NULL)
    return m_CurrentTest->GetLastResultErrorDescription();

  return "";
}

TQErrCode CBITManager::ResetTest(QString TestGroupName,QString TestName)
{
  FindGroup(TestGroupName)->GetTest(TestName)->Reset();
  return Q_NO_ERROR;
}

TQErrCode CBITManager::ResetAllTests(void)
{
  for(unsigned g = 0; g < m_GroupsList.size(); g++)
	for(unsigned t = 0; t < m_GroupsList[g]->GetTestsNum(); t++)
	  m_GroupsList[g]->GetTest(t)->Reset();

  // Clear the "last group" flag - this will force a python script to reload on next run
  if(m_LastGroupUsed)
    m_LastGroupUsed->SetLastGroup(false);

  return Q_NO_ERROR;
}

// Clear all BIT items
void CBITManager::Clear(void)
{
  // Delete all test group objects
  for(unsigned i = 0; i < m_GroupsList.size(); i++)
	delete m_GroupsList[i];

  m_GroupsList.clear();
  m_LastGroupUsed = NULL;
}

// Begin BIT sets enumeration
TQErrCode CBITManager::BeginBITSetsEnumeration(void)
{
  m_CurrentSet = 0;
  m_CurrentTestInSet = 0;
  return Q_NO_ERROR;
}

// Get the name of the next BIT set in the enumeration
QString CBITManager::GetNextBITSet(void)
{
  // Check if we are done
  if(m_CurrentSet < m_BITSetList.size())
  {
    m_CurrentSet++;
    return m_BITSetList[m_CurrentSet - 1]->Name();
  }

  return "";
}

// Begin enumeration for tests in a specific BIT sets
bool CBITManager::BeginTestsInSetEnumeration(QString SetName)
{
  int SetID = FindSet(SetName);

  // Return false is the requested BIT set could not be found
  if(SetID != -1)
  {
    m_CurrentSet = SetID;
    m_CurrentTestInSet = 0;
    return true;    
  }

  return false;
}

// Get next test in a BIT set enumeration
QString CBITManager::GetNextTestInSet(void)
{
  // Check if we are done
  if(m_CurrentTestInSet < m_BITSetList[m_CurrentSet]->Count())
    // Return test name
    return m_BITSetList[m_CurrentSet]->GetTest(m_CurrentTestInSet++);

  return "";
}

// Load a the BIT sets file
void CBITManager::LoadBITSetsFromFile(const QString BITSetsFileName)
{
  try
  {
    CQStringList Lines(BITSetsFileName);

    ClearBITSets();

    CBITSet *CurrentSet = NULL;

    // Process each line...
    for(unsigned i = 0; i < Lines.Count(); i++)
    {
      // Ignore blank lines
      if(Lines[i] == "")
        continue;

      // Check if the format of the line matches BIT set name
      if(IsSetNameStrFormat(Lines[i]))
      {
        // Add current set to the list
        if(CurrentSet)
			m_BITSetList.push_back(CurrentSet);

        // Create a new set object
        CurrentSet = new CBITSet(StripBITSetName(Lines[i]));
      } else
        {
          // A set must be initialized by now
          if(CurrentSet == NULL)
          {
            QMonitor.ErrorMessage("Invalid BIT set file format");
            break;
          }

          // Add group/test combo to current set
          CurrentSet->Add(Lines[i]);
        }
    }

    // Add last set
    if(CurrentSet)
      m_BITSetList.push_back(CurrentSet);

  } catch(...)
    {
      // Don't yell on error
    }
}

// Clear the BIT sets data structure
void CBITManager::ClearBITSets(void)
{
  // Delete current BIT Sets items
  for(TBITSetList::iterator i = m_BITSetList.begin(); i != m_BITSetList.end(); i++)
    delete (*i);

  m_BITSetList.clear();
}

// Return true if a given string is in the set name format (e.g. [MySet])
bool CBITManager::IsSetNameStrFormat(const QString Str)
{
  if(Str.length() < 3)
    return false;

  return ((Str[0] == '[') && (Str[Str.length() - 1] == ']')); 
}

// Remove the '[' and ']' characters from a set name
QString CBITManager::StripBITSetName(const QString Str)
{
  return Str.substr(1,Str.length() - 2);
}

// Find a set object
int CBITManager::FindSet(const QString SetName)
{
  for(unsigned i = 0; i < m_BITSetList.size(); i++)
    if(m_BITSetList[i]->Name() == SetName)
      return i;

  return -1;
}

// Refresh BIT sets
void CBITManager::RefreshBITSets(void)
{
  QString FileName = Q2RTApplication->AppFilePath.Value() + "BIT\\BITSets.txt";
  LoadBITSetsFromFile(FileName);
}

// Run a BIT set, return the combined results
TTestResult CBITManager::RunBITSet(QString BITSetName)
{
  TTestResult CombinedResults = trUnknown;

  try
  {
    if(m_LastGroupUsed)
      m_LastGroupUsed->SetLastGroup(false);

    int BITSetIndex = FindSet(BITSetName);

    if(BITSetIndex == -1)
      return trUnknown;

    bool BreakTestsFlag = false;  

    // Process each test in the set
    for(unsigned i = 0; i < m_BITSetList[BITSetIndex]->Count(); i++)
    {
      QString TestGroupName,TestName;

      // Extract test group and name
      if(QBreakKeyAndValueStr(m_BITSetList[BITSetIndex]->GetTest(i),TestGroupName,TestName,'|'))
      {
        CTestGroup *TestGroup = FindGroup(TestGroupName);
        CSingleTest *Test = TestGroup->GetTest(TestName);

        TTestAttributes Attr = ResolveAttributesDependency(Test->GetAttributes());

        if(!PrepareForTest(Attr))
          break;

        // Execute test
        Test->DoTest();

        // Update the combined results variable
        switch(Test->GetLastResult())
        {
          case trGo:
            if(CombinedResults == trUnknown)
              CombinedResults = trGo;
            break;

          case trNoGo:
            CombinedResults = trNoGo;

            // If the break group attribute is set, stop the test sequence
            if(Test->GetAttributes() & taBreakAll)
              BreakTestsFlag = true;
            break;

          case trWarning:
            if(CombinedResults != trNoGo)
              CombinedResults = trWarning;
        }

        // Clean up after test (except door lock)
        UnPrepareForTest(Attr & ~taDoorLock);

        if(BreakTestsFlag)
          break;
      }
    }

  } catch(EQException& QException)
    {
      QMonitor.ErrorMessage(QException.GetErrorMsg());
      CombinedResults = trNoGo;
    }

  // Clean up everything (just to be on the safe side)
  UnPrepareForTest(/*taDoorLock |*/
                   taHeadsHeating |
                   taTrayHeating |
                   taHeadsFilling);

  return CombinedResults;
}

// Create a report using CBITReport object
void CBITManager::CreateReport(CBITReport *BITReport)
{
  BITReport->CreateHeader(GetCombinedResults());

  for(unsigned g = 0; g < m_GroupsList.size(); g++)
    for(unsigned t = 0; t < m_GroupsList[g]->GetTestsNum(); t++)
    {
      CSingleTest *Test = m_GroupsList[g]->GetTest(t);

      if(Test->GetLastResult() != trUnknown && (Test->GetFlagResultWithoutHTML()== true ))
        BITReport->AddTestReportWithoutHTML(m_GroupsList[g],Test);
      else if (Test->GetLastResult() != trUnknown)
        BITReport->AddTestReport(m_GroupsList[g],Test);
    }

  BITReport->CreateFooter();
}

// Create a report file
TQErrCode CBITManager::CreateReportFile(QString ReportFileName)
{
  try
  {
    // Create HTML report
	CHTML_BITReport HTML_BITReport(ReportFileName);
	CreateReport(&HTML_BITReport);
  }
  catch(EQException& QException)
  {
    QMonitor.ErrorMessage(QException.GetErrorMsg());
  }

  return Q_NO_ERROR;
}

// Figure out the combined test results
TTestResult CBITManager::GetCombinedResults(void)
{
  TTestResult CombinedResults = trUnknown;

  for(unsigned g = 0; g < m_GroupsList.size(); g++)
    for(unsigned t = 0; t < m_GroupsList[g]->GetTestsNum(); t++)
    {
      CSingleTest *Test = m_GroupsList[g]->GetTest(t);

      // Update the combined results variable
      switch(Test->GetLastResult())
      {
        case trGo:
          if(CombinedResults == trUnknown)
            CombinedResults = trGo;
          break;

        case trNoGo:
          CombinedResults = trNoGo;
          break;

        case trWarning:
          if(CombinedResults != trNoGo)
            CombinedResults = trWarning;
      }
    }

  return CombinedResults;
}

TQErrCode CBITManager::StartBITSetDefine(QString SetName)
{
  // Check if the temporary set is already defined
  if(m_TempBITSet != NULL)
  {
    m_BITSetList.push_back(m_TempBITSet);
    m_TempBITSet = NULL;
  }

  // Check is a BIT with the same name is already exists
  unsigned i;
  for(i = 0; i < m_BITSetList.size(); i++)
    if(m_BITSetList[i]->Name() == SetName)
    {
      m_TempBITSet = m_BITSetList[i];

      // Clear current set
      m_TempBITSet->Clear();
      break;
    }

  // Create new BIT set
  if(i == m_BITSetList.size())
    m_TempBITSet = new CBITSet(SetName);

  return Q_NO_ERROR;
}

TQErrCode CBITManager::AddToBITSet(QString TestGroupName,QString TestName)
{
  if(m_TempBITSet)
    m_TempBITSet->Add(TestGroupName + "|" + TestName);

  return Q_NO_ERROR;
}

TQErrCode CBITManager::SaveBITSet(void)
{
  // If the temporary BIT set is currently opened, add it to the bit list before saving
  if(m_TempBITSet != NULL)
    // If the temporary set is not already in the sets list, add it now
    if(std::find(m_BITSetList.begin(),m_BITSetList.end(),m_TempBITSet) == m_BITSetList.end())
      m_BITSetList.push_back(m_TempBITSet);

  m_TempBITSet = NULL;

  CQStringList StrList;

  for(unsigned i = 0; i < m_BITSetList.size(); i++)
  {
    // Add selection section header
    StrList.Add("[" + m_BITSetList[i]->Name() + "]");

    // Add entries
    for(unsigned j = 0; j < m_BITSetList[i]->Count(); j++)
      StrList.Add(m_BITSetList[i]->GetTest(j));

    // Empty line (just for beauty)
    StrList.Add("");
  }

  // Save to BIT set file
  QString FileName = Q2RTApplication->AppFilePath.Value() + "BIT\\BITSets.txt";
  StrList.SaveToFile(FileName);

  return Q_NO_ERROR;
}

// Begin test execution
TTestResult CBITManager::StartTest(QString TestGroupName,QString TestName)
{
  m_CurrentTest = FindGroup(TestGroupName)->GetTest(TestName);
  return m_CurrentTest->StartTest();
}

// Execute next step in a test
TTestResult CBITManager::ExecNextStep(void)
{
  if(m_CurrentTest != NULL)
    return m_CurrentTest->ExecNextStep();

  return trUnknown;
}

// Finish test execution
TTestResult CBITManager::EndTest(void)
{
  if(m_CurrentTest != NULL)
    return m_CurrentTest->EndTest();

  return trUnknown;
}

int CBITManager::GetCurrentTestStepsNum(void)
{
  if(m_CurrentTest != NULL)
    return m_CurrentTest->GetStepsNum();

  return 0;
}

QString CBITManager::GetCurrentStepName(void)
{
  if(m_CurrentTest != NULL)
    return m_CurrentTest->GetCurrentStepName();

  return "";
}

int CBITManager::BeginTestResultExEnumeration(QString TestGroupName,QString TestName)
{
  m_CurrentTest = FindGroup(TestGroupName)->GetTest(TestName);
  return m_CurrentTest->BeginResultExEnumeration();
}

QString CBITManager::GetNextTestResultEx(void)
{
  QString Result = m_CurrentTest->GetStepName() + "\n" + m_CurrentTest->GetStepRequestedValue() + "\n" +
                   m_CurrentTest->GetStepActualValue() + "\n" + m_CurrentTest->GetStepDescription();

  m_CurrentTest->MoveToNextResultEx();

  return Result;
}

// Prepare for a given test according to its attributes
bool CBITManager::PrepareForTest(TTestAttributes TestAttributes)
{
  CMachineSequencer *MachineSequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  // Verify that the tray is inserted
  if(TestAttributes & taTrayInsert)
  {
    CTrayBase *Tray = MachineSequencer->GetTrayInstance();
    if(!Tray->VerifyTrayInserted())
      return false;
  }

  // Close the door
  if(TestAttributes & taDoorLock)
  {
    CDoorBase *Door = MachineSequencer->GetDoorInstance();

    if(Door->Enable() != Q_NO_ERROR)
      return false;
  }

  // Axes homing
  if(TestAttributes & taHomeAxes)
  {
    CMotorsBase *Motor = MachineSequencer->GetMotorsInstance();
    Motor->SetMotorEnableDisable(AXIS_ALL, true);
    Motor->GoWaitHome(AXIS_ALL);
  }

  // Heads should be hot
  if(TestAttributes & taHeadsHeating)
  {
    CHeadHeatersBase *HeadHeater = MachineSequencer->GetHeadHeatersInstance();

    if(HeadHeater->SetDefaultHeateresTemperature() != Q_NO_ERROR)
      throw EBITManager("Head heaters set temperature error");

    if(HeadHeater->SetDefaultOnOff(true) != Q_NO_ERROR)
      throw EBITManager("Head heaters set On/Off error");

    if(HeadHeater->WaitForHeadsTemperatureOK(false) != Q_NO_ERROR)
      throw EBITManager("Head heaters wait error");
  }

  // Tray should be hot
  if(TestAttributes & taTrayHeating)
  {
    CTrayBase *TrayHeater = MachineSequencer->GetTrayInstance();

    if(TrayHeater->SetDefaultTrayStartTemperature() != Q_NO_ERROR)
      throw EBITManager("Tray heater set temperature error");

    if(TrayHeater->SetTrayOnOff(true) != Q_NO_ERROR)
      throw EBITManager("Tray heater set On/Off error");

    if(TrayHeater->WaitForTrayInTemperature() != Q_NO_ERROR)
      throw EBITManager("Tray heaters wait error");
  }

  // Heads filling
  if(TestAttributes & taHeadsFilling)
  {
    CHeadFillingBase *HeadFilling = MachineSequencer->GetHeadFillingInstance();

    if(HeadFilling->SetDefaultParms() != Q_NO_ERROR)
      throw EBITManager("Head filling set parameters error");

    if(HeadFilling->HeadFillingOnOff(true) != Q_NO_ERROR)
      throw EBITManager("Head filling monitoring off error");

    if(HeadFilling->WaitForFilledHeadContainer() != Q_NO_ERROR)
      throw EBITManager("Head filling wait error");
  }

  return true;
}

// Clean up after prepare for test
void CBITManager::UnPrepareForTest(TTestAttributes TestAttributes)
{
  CMachineSequencer *MachineSequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  // Turn off head heaters
  if(TestAttributes & taHeadsHeating)
  {
    CHeadHeatersBase *HeadHeater = MachineSequencer->GetHeadHeatersInstance();
    HeadHeater->SetDefaultOnOff(false);
  }

  // Turn off tray heater
  if(TestAttributes & taTrayHeating)
  {
    CTrayBase *TrayHeater = MachineSequencer->GetTrayInstance();
    TrayHeater->SetTrayOnOff(false);
  }

  // Turn off head filling
  if(TestAttributes & taHeadsFilling)
  {
    CHeadFillingBase *HeadFilling = MachineSequencer->GetHeadFillingInstance();
    HeadFilling->HeadFillingOnOff(false);
  }

  // Open the door
  if(TestAttributes & taDoorLock)
  {
    CDoorBase *Door = MachineSequencer->GetDoorInstance();
    Door->Disable();
  }
}

// Help function for the groups sorting
static bool TestGroupCompare(CTestGroup *A,CTestGroup *B)
{
  return (A->Name() < B->Name());
}

// Sort according to group names
void CBITManager::SortByGroup(void)
{
  std::sort(m_GroupsList.begin(),m_GroupsList.end(),TestGroupCompare);
}
// Sort according to Objet SRS
void CBITManager::Sort(void)
{
  TTestGroupList tmpList;
   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Data Card" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
    for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Environment" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Parameters" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "RFID" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Communication" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	  for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "General" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	  for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Head Cards" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	  for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Heaters" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	  for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "System Voltages")
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	   
	   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "UVLamps" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	    for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Motors" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Filling" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	    for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Encoder" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	   for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Vacuum" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
	  for(int i = 0;i< m_GroupsList.size();i++)
	  {
		if(m_GroupsList[i]->Name() == "Interlock" )
		{
			tmpList.push_back(m_GroupsList[i]);
			i = m_GroupsList.size();
			break;
		}
	  }
  m_GroupsList.clear();
  for(int i = 0;i<tmpList.size();i++)
	 m_GroupsList.push_back(tmpList[i]);

  tmpList.clear();
 }


