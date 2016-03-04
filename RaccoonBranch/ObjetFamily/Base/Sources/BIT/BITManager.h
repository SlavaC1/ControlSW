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

#ifndef _BIT_MANAGER_H_
#define _BIT_MANAGER_H_

#include "QComponent.h"
#include "TestGroup.h"


class CTestGroup;
class CBITReport;

// Exception class for BIT manager class
class EBITManager : public EQException {
  public:
    EBITManager(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

// Type for a list of test objects
typedef std::vector<CSingleTest *> TTestsList;

class CQStringList;

// Class for a collection of tests
class CBITSet : public CQObject {
  private:
    typedef std::vector<QString> TStrList;

    // Set name
    QString m_Name;

    TStrList m_TestsList;

  public:
    // Default constructor
    CBITSet(void) {}

    // Init constructor
    CBITSet(const QString Name) {
      m_Name = Name;
    }

    // Return number of tests associated with the set
    unsigned Count(void) {
      return m_TestsList.size();
    }

    QString Name(void) {
      return m_Name;
    }

    // Clear all tests items
    void Clear(void) {
      m_TestsList.clear();
    }

    // Add a test item
    void Add(const QString TestName) {
      m_TestsList.push_back(TestName);
    }

    // Access to a specific item
    QString GetTest(int Index) {
      return m_TestsList[Index];
    }
};

// BIT manager class
class CBITManager : public CQComponent {
  private:
    // Type for a list of test groups
    typedef std::vector<CTestGroup *> TTestGroupList;

    // Type for a list of BIT sets
    typedef std::vector<CBITSet *> TBITSetList;

    TTestGroupList m_GroupsList;

    TBITSetList m_BITSetList;
    CBITSet *m_TempBITSet;

    // Used for progressive tests
    unsigned m_CurrentGroupIndex;
    unsigned m_CurrentTestID;

    CTestGroup *m_CurrentGroup;
    CTestGroup *m_LastGroupUsed;
    CSingleTest *m_CurrentTest;

    unsigned m_CurrentSet;
    unsigned m_CurrentTestInSet;

    // Prepare for a given test according to its attributes
    bool PrepareForTest(TTestAttributes TestAttributes);

    // Clean up after prepare for test
    void UnPrepareForTest(TTestAttributes TestAttributes);

    // Find a test group (throw exception if not found)
    CTestGroup *FindGroup(const QString GroupName);

    // Find a set object, return -1 if not found
    int FindSet(const QString SetName);

    // Clear the BIT sets data structure
    void ClearBITSets(void);

    // Return true if a given string is in the set name format (e.g. [MySet])
    bool IsSetNameStrFormat(const QString Str);

    // Remove the '[' and ']' characters from a set name
    QString StripBITSetName(const QString Str);

    // Load a the BIT sets file
    void LoadBITSetsFromFile(const QString BITSetsFileName);

    void CreateReport(CBITReport *BITReport);

  public:
    // Constructor
    CBITManager(void);

    // Destructor
    ~CBITManager(void);

    void RegisterTestGroup(CTestGroup *TestGroup);

    // Clear all BIT items
    void Clear(void);

    // Sort according to group names
	void SortByGroup(void);
	void Sort();

    // Refresh BIT sets
    void RefreshBITSets(void);

    // Do a single test by its group name and test name
    DEFINE_METHOD_2(CBITManager,TQErrCode,DoTest,QString /*TestGroupName*/,QString /*TestName*/);

    // Begin groups enumeration
    DEFINE_METHOD(CBITManager,TQErrCode,BeginTestGroupsEnumeration);

    // Get the name of the next test group in the enumeration
    DEFINE_METHOD(CBITManager,QString,GetNextTestGroup);

    DEFINE_METHOD(CBITManager,int,GetCurrentTestStepsNum);

    // Begin enumeration for tests in a given group
    DEFINE_METHOD_1(CBITManager,TQErrCode,BeginTestsEnumeration,QString /*GroupName*/);

    // Get next test name and attributes
    DEFINE_METHOD(CBITManager,QString,GetNextTest);
    DEFINE_METHOD(CBITManager,unsigned,GetNextTestAttributes);
    DEFINE_METHOD(CBITManager,int,GetNextTestStepsNum);

    // Return last test result code
    DEFINE_METHOD(CBITManager,TTestResult,GetLastTestResult);

    // Return last test error description string
    DEFINE_METHOD(CBITManager,QString,GetLastTestErrorDescription);

    // Begin enumeration for extended test results, return number of step results
    DEFINE_METHOD_2(CBITManager,int,BeginTestResultExEnumeration,QString /*TestGroupName*/,QString /*TestName*/);

    // Return a string representing result for a single step, each sub category is seperated by newline
    DEFINE_METHOD(CBITManager,QString,GetNextTestResultEx);

    // Reset a specific test to unknown state
    DEFINE_METHOD_2(CBITManager,TQErrCode,ResetTest,QString /*TestGroupName*/,QString /*TestName*/);

    // Reset all tests to unknown state
    DEFINE_METHOD(CBITManager,TQErrCode,ResetAllTests);

    // Begin BIT sets enumeration
    DEFINE_METHOD(CBITManager,TQErrCode,BeginBITSetsEnumeration);

    // Get the name of the next BIT set in the enumeration
    DEFINE_METHOD(CBITManager,QString,GetNextBITSet);

    // Begin enumeration for tests in a specific BIT sets
    DEFINE_METHOD_1(CBITManager,bool,BeginTestsInSetEnumeration,QString /*SetName*/);

    // Get next test in a BIT set enumeration
    DEFINE_METHOD(CBITManager,QString,GetNextTestInSet);

    // Run a BIT set, return the combined results
    DEFINE_METHOD_1(CBITManager,TTestResult,RunBITSet,QString /*BITSetName*/);

    DEFINE_METHOD_1(CBITManager,TQErrCode,CreateReportFile,QString /*ReportFileName*/);

    // Figure out the combined test results for all the tests in the manager
    DEFINE_METHOD(CBITManager,TTestResult,GetCombinedResults);

    // New BIt set defintion routines
    DEFINE_METHOD_1(CBITManager,TQErrCode,StartBITSetDefine,QString /*SetName*/);
    DEFINE_METHOD_2(CBITManager,TQErrCode,AddToBITSet,QString /*TestGroupName*/,QString /*TestName*/);
    DEFINE_METHOD(CBITManager,TQErrCode,SaveBITSet);

    // Begin test execution
    DEFINE_METHOD_2(CBITManager,TTestResult,StartTest,QString /*TestGroupName*/,QString /*TestName*/);

    // Execute next step in a test
    DEFINE_METHOD(CBITManager,TTestResult,ExecNextStep);

    // Finish test execution
    DEFINE_METHOD(CBITManager,TTestResult,EndTest);

    // Get current step name
    DEFINE_METHOD(CBITManager,QString,GetCurrentStepName);
};

#endif
