/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: BIT                                                      *
 * Module Description: Python scripts BIT tests support.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 31/07/2003                                           *
 * Last upate: 17/08/2003                                           *
 ********************************************************************/

#ifndef _PYTHON_TEST_GROUP_H_
#define _PYTHON_TEST_GROUP_H_

#include <map>
#include "TestGroup.h"
#include "VarPyth.hpp"


// Exception class for python test group class
class EPythonTestGroup : public EQException {
  public:
    EPythonTestGroup(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CPythonTestGroup;

typedef std::map<QString,PPyObject> TPyTestsLookup;

// Implement class CSingleTest for python scripts
class CPythonSingleTest : public CSingleTest {
  private:
    PPyObject m_PyTestObj;

    // Test start event (override)
    TTestResult Start(void);

    // Test end event (override)
    TTestResult End(void);

    // Python test stub function (override)
    TTestResult Execute(int StepNumber);

  public:
    CPythonSingleTest(const QString Name,TTestAttributes Attributes);
};

// Implement class CTestGroup for python scripts
class CPythonTestGroup : public CTestGroup {
  friend class CPythonSingleTest;

  private:
    QString m_ScriptFileName;

    TPyTestsLookup m_PyTestsLookup;

    // Help function for creating test objects from python objects
    CPythonSingleTest *CreateTestFromPyObject(PPyObject PyTestObj);

    // Help function for creating test step objects from python objects    
    TTestStep CreateTestStepFromPyObject(PPyObject PyTestStepObj);

    void ReRunScript(void);
    void RefreshTestsLookup(void);

  public:
    // Constructor
    CPythonTestGroup(const QString ScriptFileName);

    // Refresh tests list
    void Refresh(void);    
};

#endif
