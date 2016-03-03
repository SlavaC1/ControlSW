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
 * Last upate: 14/08/2003                                           *
 ********************************************************************/

#ifndef _TEST_GROUP_H_
#define _TEST_GROUP_H_

#include <vector>
#include "QObject.h"
#include "QException.h"
#include "BITDefs.h"


// Exception class for test group class
class ETestGroup : public EQException {
  public:
    ETestGroup(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CTestGroup;

// Struct to represent a single test step
struct TTestStep {
  QString StepName;
  QString RequestedValue;
  QString ActualValue;
  QString Description;

  // Initialization constructor
  TTestStep(const QString StepName_ = "",
            const QString RequestedValue_ = "",
            const QString ActualValue_ = "",
            const QString Description_ = "") {
    StepName = StepName_;
    RequestedValue = RequestedValue_;
    ActualValue = ActualValue_;
    Description = Description_;
  }
};

// Class for representing a single test
class CSingleTest : CQObject {
  private:
    typedef std::vector<TTestStep> TTestStepList;

    TTestStepList m_Steps;

    QString m_Name;
    TTestAttributes m_Attributes;

    // Last test result error code
    TTestResult m_LastResult;

    // Last test result description string
    QString m_LastResultErrorDescription;

    // Next step to be executed
    unsigned m_NextStep;

    // Used for extended result enumeration
    unsigned m_StepResultExIndex;

	//used for writting to HTML file without the function TextToHTMLText 
	bool m_FlagResultWithoutHTML;
  protected:

    CTestGroup *m_ParentGroup;

    // Set the standard result description
    void SetResultDescription(QString ResultDescription) {
      m_LastResultErrorDescription = ResultDescription;
    }

    // Set the current step result
    void SetActualVsRequested(const QString Requested,const QString Actual,const QString Description = "");

    // Test start event (override is not mandatory).
    // The start function must return trGo in order for the test to start. 
    virtual TTestResult Start(void) {
      return trGo;
    }

    // Test end event (override is not mandatory)
    virtual TTestResult End(void) {
      return trGo;
    }

    // Test implementation function, when the execute function returns with a value
    // different then trUnknown, the test is completed.
    virtual TTestResult Execute(int StepNumber) = 0;

  public:
    // Constructor
    CSingleTest(const QString Name,TTestAttributes Attributes = taNormal);

    // Execute the test sequence, return the test result
    TTestResult DoTest(void);

    // Reset test to unknown state
    void Reset(void);

    // Add a step descriptor (version 1)
    void AddStep(const QString StepName,const QString RequestedValue = "");

    // Add a step descriptor (version 2)
    void AddStep(const TTestStep &TestStep);

    // Progressive test execution routines
    // -----------------------------------
    
    // Start test execution
    TTestResult StartTest(void);

    // Execute the next step in the sequence
    TTestResult ExecNextStep(void);

    // Close the test sequence
    TTestResult EndTest(void);

    // Return the name of the current (before execution) step name
    QString GetCurrentStepName(void);

    // Results function
    unsigned BeginResultExEnumeration(void);

    // Get step name in this context is refering to the results enumeration
    QString GetStepName(void);
    QString GetStepRequestedValue(void);
    QString GetStepActualValue(void);
    QString GetStepDescription(void);

    // Point to next result item
    void MoveToNextResultEx(void) {
      m_StepResultExIndex++;
    }

    void SetParentTestGroup(CTestGroup *ParentGroup) {
      m_ParentGroup = ParentGroup;
    }

    // Return the test name
    QString Name(void) {
      return m_Name;
    }

    // Return the test attributes mask
    TTestAttributes GetAttributes(void) {
      return m_Attributes;
    }

    // Return the last test result
    TTestResult GetLastResult(void) {
      return m_LastResult;
    }

    // Return last test result description string
    QString GetLastResultErrorDescription(void) {
      return m_LastResultErrorDescription;
    }

    // Return the number of registered steps in the test
    int GetStepsNum(void) {
      return m_Steps.size();
    }
	// Set the flag if need to write without html ot not
    void SetFlagResultWithoutHTML(bool FlagResultWithoutHTML) {
      m_FlagResultWithoutHTML = FlagResultWithoutHTML;
    }
	//get the flag if need to write without html or not
    bool GetFlagResultWithoutHTML(void) {
      return  m_FlagResultWithoutHTML;
    }
};

// Class for representing a collection of tests
class CTestGroup : public CQObject {
  friend class CSingleTest;

  private:
    typedef std::vector<CSingleTest *> TTestList;

    QString m_Name;

    // Lists of tests
    TTestList m_Tests;

    // If this flag is true, a test from this group was the last test executed
    bool m_LastGroup;

    QString m_DependecyErrorStr;

    bool CheckExecutionDependency(CSingleTest *Test);

    // Return last dependency error string
    QString GetDependencyErrorString(void) {
      return m_DependecyErrorStr;
    }

  public:
    // Constructor
    CTestGroup(const QString Name);

    // Destructor
    virtual ~CTestGroup(void);

    // Add a test to the group (ownership transfer)
    void Add(CSingleTest *Test);

    // Return number of registered tests
    unsigned GetTestsNum(void);

    // Return the group name
    QString Name(void) const {
      return m_Name;
    }

    bool IsLastGroup(void) {
      return m_LastGroup;
    }

    void SetLastGroup(bool LastGroup) {
      m_LastGroup = LastGroup;
    }

    // Get a test by its ID
    CSingleTest *GetTest(unsigned TestID);

    // Get a test by its name
    CSingleTest *GetTest(const QString TestName);
};

#endif
