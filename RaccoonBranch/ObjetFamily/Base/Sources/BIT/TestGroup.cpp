/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Built-In-Tests managment                                 *
 * Module Description: Test group class.                            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 27/07/2003                                           *
 * Last upate: 30/07/2003                                           *
 ********************************************************************/

#include <algorithm>
#include "TestGroup.h"


// Class CSingleTest implementation
// ********************************************************************

// Constructor
CSingleTest::CSingleTest(const QString Name,TTestAttributes Attributes)
{
  m_Name = Name;
  m_Attributes = Attributes;
  m_LastResult = trUnknown;
  m_ParentGroup = NULL;
  m_StepResultExIndex = 0;
  m_NextStep = 0;
  m_FlagResultWithoutHTML = false ;
}

// Start test execution
TTestResult CSingleTest::StartTest(void)
{
  m_NextStep = 0;
  m_FlagResultWithoutHTML = false ;
  // Check test execution dependency before doing test
  if(!m_ParentGroup->CheckExecutionDependency(this))
  {
    m_LastResult = trWarning;
    m_LastResultErrorDescription = m_ParentGroup->GetDependencyErrorString();
    return trWarning;

  } else
    {
      // Clear reuslt variables before execution
      m_LastResult = trUnknown;
      m_LastResultErrorDescription = "";
    }

  try
  {
    // Call user start event
    m_LastResult = Start();

    // Mark the last group used flag
    m_ParentGroup->SetLastGroup(true);

    // In a case of an exception the test is considered to fail
  } catch(EQException& Err)
    {
      m_LastResultErrorDescription = Err.GetErrorMsg();
      m_LastResult = trNoGo;
    }
    catch(...)
    {
      m_LastResultErrorDescription = "Unexpected exception";
      m_LastResult = trNoGo;
    }

  return m_LastResult;
}

// Do a specific test step
TTestResult CSingleTest::ExecNextStep(void)
{
  try
  {
    if(m_NextStep < m_Steps.size())
    {
      m_LastResult = Execute(m_NextStep);
      m_NextStep++;
    }
    else
      throw ETestGroup("No test step found");

    // In a case of an exception the test is considered to fail
  } catch(EQException& Err)
    {
      m_LastResultErrorDescription = Err.GetErrorMsg();
      m_LastResult = trNoGo;
    }
    catch(...)
    {
      m_LastResultErrorDescription = "Unexpected exception";
      m_LastResult = trNoGo;
    }

  return m_LastResult;
}

// Close the test sequence
TTestResult CSingleTest::EndTest(void)
{
  TTestResult EndResult;

  // In a case that the result is still unknown, reset the steps counter. In this way
  // there will be no result indication for each steps executed so far.
  if(m_LastResult == trUnknown)
    m_NextStep = 0;

  try
  {
    // Call user end event (don't save end result in the history variable)
    EndResult = End();

    // In a case of an exception the test is considered to fail
  } catch(EQException& Err)
    {
      m_LastResultErrorDescription = Err.GetErrorMsg();
      m_LastResult = trNoGo;
    }
    catch(...)
    {
      m_LastResultErrorDescription = "Unexpected exception";
      m_LastResult = trNoGo;
    }

  return EndResult;
}

// Set the current step results
void CSingleTest::SetActualVsRequested(const QString Requested,const QString Actual,const QString Description)
{
  // Sanity check
  if(m_NextStep < m_Steps.size())
  {
    m_Steps[m_NextStep].RequestedValue = Requested;
    m_Steps[m_NextStep].ActualValue = Actual;
    m_Steps[m_NextStep].Description = Description;
  }
}

// Return the name of the current (before execution) step name
QString CSingleTest::GetCurrentStepName(void)
{
  if(m_NextStep < m_Steps.size())
    return m_Steps[m_NextStep].StepName;

  return "";
}

// Execute the test sequence and return test result
TTestResult CSingleTest::DoTest(void)
{
  // Start execution
  m_LastResult = StartTest();

  if(m_LastResult == trGo)
  {
    // If there is at least one step...
    if(m_Steps.size() > 0)
    {
      // Execute all test steps
      for(unsigned i = 0; i < m_Steps.size(); i++)
        if(ExecNextStep() != trUnknown)
          break;
    } else
        m_LastResult = trUnknown;

    // End the test
    TTestResult EndResult = EndTest();

    // If everything is Ok in the end step, return error code so far
    if(EndResult == trGo)
      return m_LastResult;

    return EndResult;      
  }

  return m_LastResult;
}

// Reset test to unknown state
void CSingleTest::Reset(void)
{
  m_LastResult = trUnknown;
  m_LastResultErrorDescription = "";
  m_NextStep = 0;
}

// Add a step descriptor
void CSingleTest::AddStep(const QString StepName,const QString RequestedValue)
{
  m_Steps.push_back(TTestStep(StepName,RequestedValue));
}

// Add a step descriptor (version 2)
void CSingleTest::AddStep(const TTestStep TestStep)
{
  m_Steps.push_back(TestStep);
}

unsigned CSingleTest::BeginResultExEnumeration(void)
{
  m_StepResultExIndex = 0;
  return m_NextStep;
}

QString CSingleTest::GetStepName(void)
{
  if(m_StepResultExIndex < m_Steps.size())
    return m_Steps[m_StepResultExIndex].StepName;

  return "";
}

QString CSingleTest::GetStepRequestedValue(void)
{
  if(m_StepResultExIndex < m_Steps.size())
    return m_Steps[m_StepResultExIndex].RequestedValue;

  return "";
}

QString CSingleTest::GetStepActualValue(void)
{
  if(m_StepResultExIndex < m_Steps.size())
    return m_Steps[m_StepResultExIndex].ActualValue;

  return "";
}

QString CSingleTest::GetStepDescription(void)
{
  if(m_StepResultExIndex < m_Steps.size())
    return m_Steps[m_StepResultExIndex].Description;

  return "";
}

// Class CTestGroup implementation
// ********************************************************************

// Constructor
CTestGroup::CTestGroup(const QString Name)
{
  m_Name = Name;
  m_LastGroup = false;
}

// Destructor
CTestGroup::~CTestGroup(void)
{
  // Delete all items from the test group
  for(unsigned i = 0; i < m_Tests.size(); i++)
    delete m_Tests[i];
}

// Add a test to the group (ownership transfer)
void CTestGroup::Add(CSingleTest *Test)
{
  Test->SetParentTestGroup(this);
  m_Tests.push_back(Test);
}

// Return number of registered tests
unsigned CTestGroup::GetTestsNum(void)
{
  return m_Tests.size();
}

// Get a test by its ID
CSingleTest *CTestGroup::GetTest(unsigned TestID)
{
  // Sanity check
  if(TestID >= m_Tests.size())
    throw ETestGroup("Invalid test index");

  return m_Tests[TestID];
}

// Get a test by its name
CSingleTest *CTestGroup::GetTest(const QString TestName)
{
  for(unsigned i = 0; i < m_Tests.size(); i++)
    if(m_Tests[i]->Name() == TestName)
      return m_Tests[i];

  throw ETestGroup("Invalid test name \"" + TestName + "\"");
}

bool CTestGroup::CheckExecutionDependency(CSingleTest *Test)
{
  // Check if all the tests so far are Ok
  for(int i = 0; Test != m_Tests[i]; i++)
  {
    // Check only tests with the "mandatory" attribute
    TTestAttributes Attr = m_Tests[i]->GetAttributes();
    if(Attr & taBreakGroup)
    {
      // If the test status is different then "go" or "warning" return false
      TTestResult Result = m_Tests[i]->GetLastResult();

      // If the test status is unknown
      if(Result == trUnknown)
      {
        m_DependecyErrorStr = "Dependency error - Test \"" + m_Tests[i]->Name() + "\" has not been executed yet.";
        return false;
      }

      // If the test status has failed
      if(Result != trGo)
      {
        m_DependecyErrorStr = "Dependency error - Test \"" + m_Tests[i]->Name() + "\" has failed";
        return false;
      }
    }
  }

  return true;
}






