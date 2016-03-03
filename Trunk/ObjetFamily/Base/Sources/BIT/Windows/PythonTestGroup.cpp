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
 * Last upate: 31/07/2003                                           *
 ********************************************************************/

#include "BITDefs.h"
#include "PythonTestGroup.h"
#include "QPythonIntegrator.h"
#include "QPythonUtils.h"
#include "QUtils.h"


// Class CPythonSingleTest implementation
// ********************************************************************

CPythonSingleTest::CPythonSingleTest(const QString Name,
                                     TTestAttributes Attributes) : CSingleTest(Name,Attributes)
{
}

// Test start event (override)
TTestResult CPythonSingleTest::Start(void)
{
  try
  {
    TPythonEngine *PyEng = GetPythonEngine();

    CPythonTestGroup *ParentGroup = dynamic_cast<CPythonTestGroup *>(m_ParentGroup);
    ParentGroup->RefreshTestsLookup();

    // Find the test object pointer
    m_PyTestObj = ParentGroup->m_PyTestsLookup[Name()];

    // If not found
    if(m_PyTestObj == NULL)
    {
      SetResultDescription("Python BIT integration error");
      return trNoGo;
    }

    // Call the "DoStart" function
    CPyAutoDecRef TmpObj(PyEng->PyObject_GetAttrString(m_PyTestObj,"DoStart"));

    // Call the function
    PPyObject RetValue(PyEng->PyObject_CallObject(TmpObj,NULL));
    PyEng->CheckError();

    // Extract the status strings
    SetResultDescription(GetAttrFromPyObjAsString(m_PyTestObj,"ErrorDescription"));

    return TTestResult(PyEng->PyInt_AsLong(RetValue));

  } catch(Exception& VCLErr)
  {
    // Translate VCL exceptions to QLib exceptions
    throw EPythonTestGroup(VCLErr.Message.c_str());
  }
}

// Test end event (override)
TTestResult CPythonSingleTest::End(void)
{
  try
  {
    TPythonEngine *PyEng = GetPythonEngine();

    // Call the "DoEnd" function
    CPyAutoDecRef TmpObj(PyEng->PyObject_GetAttrString(m_PyTestObj,"DoEnd"));

    // Call the function
    PPyObject RetValue(PyEng->PyObject_CallObject(TmpObj,NULL));
    PyEng->CheckError();

    return TTestResult(PyEng->PyInt_AsLong(RetValue));

  } catch(Exception& VCLErr)
  {
    // Translate VCL exceptions to QLib exceptions
    throw EPythonTestGroup(VCLErr.Message.c_str());
  }
}

// Python test stub function (override)
TTestResult CPythonSingleTest::Execute(int StepNumber)
{
  try
  {
    TPythonEngine *PyEng = GetPythonEngine();

   if(m_PyTestObj != NULL)
    {
      // Find the execute function
      CPyAutoDecRef TmpObj(PyEng->PyObject_GetAttrString(m_PyTestObj,"DoExecute"));

      // Check if found
      if(TmpObj == NULL)
        throw EPythonTestGroup("Test \"" + Name() + "\" does not have \"DoExecute\" function");

      // Prepare parameters for the execute function
      PPyObject ParamsTuple = PyEng->ArrayToPyTuple(ARRAYOFCONST((StepNumber)));

      // Call the execute function
      PPyObject RetValue(PyEng->PyObject_CallObject(TmpObj,ParamsTuple));
      PyEng->CheckError();

      // Extract the status strings
      SetResultDescription(GetAttrFromPyObjAsString(m_PyTestObj,"ErrorDescription"));

      // Extract the step result
      QString Requested = GetAttrFromPyObjAsString(m_PyTestObj,"RequestedValue");
      QString Actual = GetAttrFromPyObjAsString(m_PyTestObj,"ActualValue");
      QString Description = GetAttrFromPyObjAsString(m_PyTestObj,"Description");
      SetActualVsRequested(Requested,Actual,Description);
	  
	  //sets the Flag from python script
		if(PyEng->PyObject_HasAttrString(m_PyTestObj,"FlagResultWithoutHTML"))
			SetFlagResultWithoutHTML( QStrToInt(GetAttrFromPyObjAsString(m_PyTestObj,"FlagResultWithoutHTML")) );
		else
			SetFlagResultWithoutHTML(0);
		//temp = GetFlagResultWithoutHTML();
	  //else
		//SetFlagResultWithoutHTML(0);
		
      return TTestResult(PyEng->PyInt_AsLong(RetValue));
    }
  } catch(Exception& VCLErr)
    {
      // Translate VCL exceptions to QLib exceptions
      throw EPythonTestGroup(VCLErr.Message.c_str());
    }

  return trUnknown;
}

// Class CPythonTestGroup implementation
// ********************************************************************

// Constructor
CPythonTestGroup::CPythonTestGroup(const QString ScriptFileName) : CTestGroup(QExtractFileNameWithoutExt(ScriptFileName))
{
  m_ScriptFileName = ScriptFileName;
}

// Refresh tests list
void CPythonTestGroup::Refresh(void)
{
  try
  {
    // Execute associated python script before accessing any script attributes
    ReRunScript();

    TPythonEngine *PyEng = GetPythonEngine();

    // Every test script need a "GetTests" function
    CPyAutoDecRef GetTestsFunc(PyEng->FindFunction("__main__","GetTests"));

    if(GetTestsFunc == NULL)
      throw EPythonTestGroup("Test group script must have \"GetTests\" function");

    PPyObject TestsList(PyEng->PyObject_CallObject(GetTestsFunc,NULL));
    PyEng->CheckError();

    // Check if the return type is a list
    if(!PyEng->PyList_Check(TestsList))
      throw EPythonTestGroup("Invalid \"GetTests\" return value (must be list)");

    // Create a test item for each list item
    int TestsNum = PyEng->PyList_Size(TestsList);
    for(int i = 0; i < TestsNum; i++)
    {
      PPyObject TestObj = PyEng->PyList_GetItem(TestsList,i);

      CPythonSingleTest *Test = CreateTestFromPyObject(TestObj);
      Add(Test);
    }
  } catch(Exception& VCLErr)
    {
      // Translate VCL exceptions to QLib exceptions
      throw EPythonTestGroup(VCLErr.Message.c_str());
    }
}

// Help function for creating test objects from python objects
CPythonSingleTest *CPythonTestGroup::CreateTestFromPyObject(PPyObject PyTestObj)
{
  // Extract the test name and attributes mask
  QString TestName = GetAttrFromPyObjAsString(PyTestObj,"Name");
  unsigned AttrMask = GetAttrFromPyObjAsInt(PyTestObj,"Attributes");

  CPythonSingleTest *PyTest = new CPythonSingleTest(TestName,AttrMask);

  // Add steps
  try
  {
    TPythonEngine *PyEng = GetPythonEngine();

    CPyAutoDecRef GetStepsFunc;

    if(PyEng->PyObject_HasAttrString(PyTestObj,"GetSteps"))
      GetStepsFunc = PyEng->PyObject_GetAttrString(PyTestObj,"GetSteps");

    if(GetStepsFunc != NULL)
    {
      PPyObject StepsList(PyEng->PyObject_CallObject(GetStepsFunc,NULL));
      PyEng->CheckError();

      // Check if the return type is a list
      if(!PyEng->PyList_Check(StepsList))
        throw EPythonTestGroup("Invalid \"GetSteps\" return value (must be list)");

      int StepsNum = PyEng->PyList_Size(StepsList);

      for(int i = 0; i < StepsNum; i++)
      {
        PPyObject StepObj = PyEng->PyList_GetItem(StepsList,i);

        TTestStep TestStep = CreateTestStepFromPyObject(StepObj);
        PyTest->AddStep(TestStep);
      }
    }
  } catch(Exception& VCLErr)
    {
      // Translate VCL exceptions to QLib exceptions
      throw EPythonTestGroup(VCLErr.Message.c_str());
    }

  return PyTest;
}

// Help function for creating test step objects from python objects
TTestStep CPythonTestGroup::CreateTestStepFromPyObject(PPyObject PyTestStepObj)
{
  TPythonEngine *PyEng = GetPythonEngine();

  TTestStep Result(PyEng->PyObjectAsString(PyTestStepObj).c_str());

  return Result;
}

void CPythonTestGroup::ReRunScript(void)
{
  // Run associated script
  QPythonIntegratorDM->ExecFile(m_ScriptFileName.c_str());
}

void CPythonTestGroup::RefreshTestsLookup(void)
{
  // If the "LastGroup" flag is on, the last executed test belongs to the current group
//  if(IsLastGroup())
    // Do nothing
//    return;

  try
  {
    ReRunScript();

    m_PyTestsLookup.clear();

    // Find the "GetTests function"
    TPythonEngine *PyEng = GetPythonEngine();

    // Every wizard script need a "GetPages" function
    CPyAutoDecRef GetTestsFunc(PyEng->FindFunction("__main__","GetTests"));

    if(GetTestsFunc == NULL)
      throw EPythonTestGroup("Test group script must have \"GetTests\" function");

    PPyObject TestsList(PyEng->PyObject_CallObject(GetTestsFunc,NULL));
    PyEng->CheckError();

    // Check if the return type is a list
    if(!PyEng->PyList_Check(TestsList))
      throw EPythonTestGroup("Invalid \"GetTests\" return value (must be list)");

    // Add each test to a lookup table
    int TestsNum = PyEng->PyList_Size(TestsList);
    for(int i = 0; i < TestsNum; i++)
    {
      PPyObject TestObj = PyEng->PyList_GetItem(TestsList,i);

      QString TestName = GetAttrFromPyObjAsString(TestObj,"Name");
      m_PyTestsLookup[TestName] = TestObj;
    }
  } catch(Exception& VCLErr)
    {
      // Translate VCL exceptions to QLib exceptions
      throw EPythonTestGroup(VCLErr.Message.c_str());
    }
}



