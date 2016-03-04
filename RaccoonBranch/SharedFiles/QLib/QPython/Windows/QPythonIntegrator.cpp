/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: General                                                 *
 * Module: QPython                                                  *
 * Module Description: Integration module between python and QLib.  *
 *                                                                  *
 * Compilation: Borland C++ builder, Python-For-Delphi components   *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 20/06/2003                                           *
 * Last upate: 18/09/2003                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "QPythonIntegrator.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PythonEngine"
#pragma resource "*.dfm"

#include "QObjectsRoster.h"
#include "QComponent.h"
#include "QMonitor.h"
#include "QMethod.h"
#include "QThreadUtils.h"
#include "QTimer.h"
#include "MachineManager.h"
#include "Q2RTApplication.h"
#include "QFile.h"

#ifndef NO_PYTHON_BREAKPOINTS
#include "PythonBreakpointDlg.h"
#endif

#include "QPythonUtils.h"
#include "QStringList.h"

// Proxy object used as python type
struct TQComponentProxy {
  int ob_refcnt;
  PPyTypeObject ob_type;
  CQComponent *QComponent;
};

// Global string list object for various usages
static TStringList *gTmpStrList;

// Convert a string to a python object according to a type ID parameter
PPyObject StrToPyObjectUsingTypeID(TPythonEngine *PyEng,const QString& Str,TPropertyTypeID TypeID);

TQPythonIntegratorDM *QPythonIntegratorDM;
//---------------------------------------------------------------------------
__fastcall TQPythonIntegratorDM::TQPythonIntegratorDM(TComponent* Owner)
        : TDataModule(Owner)
{
}
//---------------------------------------------------------------------------

// Initialize QLib/Python integration
void TQPythonIntegratorDM::Init(void)
{
  CreateComponentsDictionary();
  
  // Process all registered components in the roster's list
  for(TQComponentsDict::iterator Itr = m_QComponentsDict.begin(); Itr != m_QComponentsDict.end(); Itr++)
  {
    // Create a new python type for each QComponent
    TPythonType *PyType = new TPythonType(this);
    PyType->OnInitialization = PythonTypeInitialize;
    PyType->TypeName = Itr->first;
    PyType->Name = PyType->TypeName;
    PyType->Engine = PythonEngine1;
  }

  PythonEngine1->LoadDll();

  // Add the subdirectory "Lib" to the python search path
  AnsiString LibPath = ExtractFilePath(Application->ExeName) + "\\Lib";
  GetPythonEngine()->PySys_SetPath(LibPath.c_str());

  CreatePythonReferences();

  m_ThreadsIDs.clear();
}

// Create a reference to a QComponent using a proxy
PPyObject TQPythonIntegratorDM::CreateQComponentRef(PPyObject self,PPyObject args)
{
  TPythonEngine *PyEng = GetPythonEngine();

  PyEng->PyTupleToStrings(args,gTmpStrList);

  // Check number of arguments
  if(gTmpStrList->Count != 1)
  {
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Invalid numbers of parameters for CreateQComponentRef");
    return NULL;
  }

  TQComponentProxy *Proxy = new TQComponentProxy;
  Proxy->ob_refcnt = 1;
  Proxy->ob_type = PyEng->TypeByName(gTmpStrList->Strings[0]);

  Proxy->QComponent = QPythonIntegratorDM->m_QComponentsDict[gTmpStrList->Strings[0]];

  if(Proxy->QComponent == NULL)
  {
    AnsiString ErrStr = "QObject \"" + gTmpStrList->Strings[0] + "\" could not be found";
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),ErrStr.c_str());
    return NULL;
  }

  return (PPyObject)Proxy;
}

PPyObject TQPythonIntegratorDM::BreakpointHandler(PPyObject self,PPyObject args)
{
  TPythonEngine *PyEng = GetPythonEngine();

  PyEng->PyTupleToStrings(args,gTmpStrList);

  // Must get three arguments...
  if(gTmpStrList->Count != 3)
  {
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Invalid number of arguments for function \'Breakpoint()\'");
    return NULL;
  }

  bool DoBreak = false;

  // If no condition specified, do breakpoint
  if(gTmpStrList->Strings[2] == "")
    DoBreak = true;
  else
  {
    try
    {
      // Check breakpoint condition...
      if(StrToInt(PyEng->EvalStringAsStr(gTmpStrList->Strings[2])))
        DoBreak = true;
    } catch(EQException& Err)
      {
        AnsiString Msg = "Invalid condition in breakpoint '" + gTmpStrList->Strings[1] + "'";
        PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),Msg.c_str());
        return NULL;
      }
      catch(Exception& Err)
      {
        AnsiString Msg = "Invalid condition in breakpoint '" + gTmpStrList->Strings[1] + "'";
        PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),Msg.c_str());
        return NULL;
      }
  }


  if(DoBreak)
  {
#ifndef NO_PYTHON_BREAKPOINTS
    PythonBreakpointForm->SetCaption(gTmpStrList->Strings[1]);
    PythonBreakpointForm->SetLocalDict(PyEng->PyTuple_GetItem(args,0));
    if(PythonBreakpointForm->ShowModal() == mrAbort)
    {
      PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Execution stopped by python debugger");
      return NULL;
    }
#endif    
  }

  return PyEng->ReturnNone();
}

void __fastcall TQPythonIntegratorDM::PythonModule1Initialization(TObject *Sender)
{
  TPythonModule *PyModule = dynamic_cast<TPythonModule *>(Sender);

  // Register instance creation function
  PyModule->AddMethod("CreateQComponentRef",CreateQComponentRef,"CreateQComponentRef");
  PyModule->AddMethod("QSleep",QSleepPy,"QSleep(Time) - Delay thread execution for some time (ticks)");
  PyModule->AddMethod("QSleepSec",QSleepSecPy,"QSleepSec(Time) - Delay thread execution for some time (seconds)");
  PyModule->AddMethod("QGetTicks",QGetTicksPy,"QGetTicks - Get system clock counter");
  PyModule->AddMethod("Breakpoint",BreakpointHandler,"Breakpoint(Dictionary,[Name,[Condition]]) - Python debugger breakpoint");

  // Add to the functions list (for documentation and help purposes only)
  m_QFunctionsList.push_back("QSleep()");
  m_QFunctionsList.push_back("QSleepSec()");
  m_QFunctionsList.push_back("QGetTicks()");
}
//---------------------------------------------------------------------------

void __fastcall TQPythonIntegratorDM::DataModuleCreate(TObject *Sender)
{
  gTmpStrList = new TStringList;
  m_OnSyntaxErrorEvent = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TQPythonIntegratorDM::DataModuleDestroy(TObject *Sender)
{
  delete gTmpStrList;

  for(TPCharList::iterator i = m_MethodsCleanup.begin(); i != m_MethodsCleanup.end(); i++)
    free(*i);
}
//---------------------------------------------------------------------------

// Execute string object, make sure that no exceptions can be thrown
void TQPythonIntegratorDM::ExecStrings(TStrings *Strings,bool ClearDictionary)
{
  CQMutexHolder MutexHolder(&m_PythonEngine1_Mutex);
  if(ClearDictionary)
    PythonEngine1->ClearModuleDictionary();

  PythonEngine1->ExecStrings(Strings);
}

// Execute string object, make sure that no exceptions can be thrown
void TQPythonIntegratorDM::ExecStringsSafe(TStrings *Strings,bool ClearDictionary)
{
 CQMutexHolder MutexHolder(&m_PythonEngine1_Mutex);

  bool IsWizardRunning;
  try
  {
    bool IsPythonWizardRunning = Q2RTApplication->GetMachineManager()->IsPythonWizardRunning();
    bool IsBITRunning = Q2RTApplication->GetMachineManager()->IsBITRunning();
	IsWizardRunning = Q2RTApplication->GetMachineManager()->IsWizardRunning();

    if(ClearDictionary && !IsPythonWizardRunning && !IsBITRunning) // Don't clear the functions list (of the wizard), if running the script from within the Python Wizard
      PythonEngine1->ClearModuleDictionary();

    PythonEngine1->ExecStrings(Strings);
  } // Catch QExceptions
    catch(EQException& Err)
    {
      QMonitor.ErrorMessage("EQException: " + Err.GetErrorMsg());
    }
    // Catch syntax exceptions
    catch(EPySyntaxError& SyntaxErr)
    {
      HandleSyntaxError(SyntaxErr.Message.c_str());

      TThreadsIDsSet::iterator it = m_ThreadsIDs.find(GetCurrentThreadId());
	  if(IsWizardRunning && it != m_ThreadsIDs.end())
		throw EQPython(SyntaxErr.Message.c_str());

	  QMonitor.ErrorMessage(QFormatStr("The printer encountered a problem (%s). Please restart the printer application.", SyntaxErr.Message.c_str()));
    }
    // Catch VCL exceptions
    catch(Exception& Err)
    {
      TThreadsIDsSet::iterator it = m_ThreadsIDs.find(GetCurrentThreadId());
	  if(IsWizardRunning && it != m_ThreadsIDs.end())
		throw EQPython(Err.Message.c_str());
	  QMonitor.ErrorMessage(QFormatStr("The printer encountered a problem (%s). Please restart the printer application.", Err.Message.c_str()));
    }
    // Catch everything else
    catch(...)
    {
      QMonitor.ErrorMessage("Unexpected exception when executing python script");
    }
}

// Execute python file
void TQPythonIntegratorDM::ExecFile(AnsiString FileName,bool ClearDictionary)
{
  TStringList *StrList = new TStringList;

  try
  {
    StrList->LoadFromFile(FileName);
    ExecStrings(StrList);

  } __finally
    {
      delete StrList;
    }
}

// Execute python file
void TQPythonIntegratorDM::ExecFileSafe(AnsiString FileName,bool ClearDictionary)
{
	TStringList *StrList = new TStringList;

	try
	{
		StrList->LoadFromFile(FileName);
		ExecStringsSafe(StrList);
	} 
	catch(EQPython &err)
	{
        TThreadsIDsSet::iterator it = m_ThreadsIDs.find(GetCurrentThreadId());
		if(Q2RTApplication->GetMachineManager()->IsWizardRunning() && it != m_ThreadsIDs.end())
			throw EQPython(err);
	}	
	// Catch VCL exceptions
	catch(Exception& Err)
	{
		QMonitor.ErrorMessage(Err.Message.c_str());
	}
	// Catch everything else
	catch(...)
	{
		QMonitor.ErrorMessage("Unexpected exception when executing python file");
	}

	delete StrList;
}

void __fastcall TQPythonIntegratorDM::PythonInputOutput1SendData(TObject *Sender, const AnsiString Data)
{
  // Ignore empty strings
  if(Data != "")
    QMonitor.Print(Data.c_str());
}
//---------------------------------------------------------------------------

void QComponentProxyDealloc(PPyObject obj)
{
  TQComponentProxy *ProxyPtr = reinterpret_cast<TQComponentProxy *>(obj);
  delete ProxyPtr;
}

// Implement the python "get attributes" standard function 
PPyObject QComponentProxyGetAttr(PPyObject obj,PChar key)
{
  CQComponent *QComponent = reinterpret_cast<TQComponentProxy *>(obj)->QComponent;

  TPythonEngine *PyEng = GetPythonEngine();
  PPyObject Result = NULL;

  // Search for a property
  int i;
  for(i = 0; i < QComponent->PropertyCount(); i++)
    if(key == QComponent->Properties[i]->Name())
    {
      QString PropValue = QComponent->Properties[i]->ValueAsString();
      Result = StrToPyObjectUsingTypeID(PyEng,PropValue,QComponent->Properties[i]->GetTypeID());
      break;
    }

  // If not found, check if it is a metohd
  if(i == QComponent->PropertyCount())
  {
    Result = PyEng->Py_FindMethod(PyEng->MethodsByName(QComponent->Name().c_str()), obj, key);

    if(!Result)
      PyEng->PyErr_SetString(*(PyEng->PyExc_AttributeError),"Unknown attribute");
  }

  return Result;
}

// Implement the python "set attributes" standard function
int QComponentProxySetAttr(PPyObject obj,PChar key,PPyObject value)
{
  CQComponent *QComponent = reinterpret_cast<TQComponentProxy *>(obj)->QComponent;

  TPythonEngine *PyEng = GetPythonEngine();
  int Result = -1;

  // Search for a property
  for(int i = 0; i < QComponent->PropertyCount(); i++)
    if(key == QComponent->Properties[i]->Name())
    {
      AnsiString ValueStr = PyEng->PyObjectAsString(value);

      // Assign new value
      QComponent->Properties[i]->AssignFromString(ValueStr.c_str());
      Result = 0;
      break;
    }

  return Result;
}

// String representation
PPyObject QComponentProxyRepr(PPyObject obj)
{
  CQComponent *Component = reinterpret_cast<TQComponentProxy *>(obj)->QComponent;
  return GetPythonEngine()->PyString_FromString(const_cast<char *>(Component->Name().c_str()));
}

// All method calls are directed to this function
PPyObject TQPythonIntegratorDM::QComponentCallWrapper(void *MethodObjPtr,PPyObject self,PPyObject args)
{
  TPythonEngine *PyEng = GetPythonEngine();

  PyEng->PyTupleToStrings(args,gTmpStrList);

  // Check number of arguments to method
  CQMethodObject *Method = reinterpret_cast<CQMethodObject *>(MethodObjPtr);

  if(Method == NULL)
  {
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Invalid method name");
    return NULL;
  }

  if(gTmpStrList->Count != (int)Method->GetMethodArgumentsCount())
  {
    AnsiString ErrStr = "Invalid numbers of arguments for method \"" + AnsiString(Method->Name().c_str()) + "\"";
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),ErrStr.c_str());
    return NULL;
  }

  // Prepare method arguments
  QString MethodArgs[MAX_QMETHOD_ARGUMENTS];

  for(unsigned i = 0; i < Method->GetMethodArgumentsCount(); i++)
    MethodArgs[i] = gTmpStrList->Strings[i].c_str();

  QString Ret;

  // Call the method
  try
  {
    Ret = Method->Invoke(MethodArgs,Method->GetMethodArgumentsCount());
  } catch(EQException& Err)
    {
      PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),const_cast<char *>(Err.GetErrorMsg().c_str()));
      return NULL;
    }
    catch(Exception& Err)
    {
      PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),Err.Message.c_str());
      return NULL;
    }
    catch(...)
    {
      AnsiString ErrStr = "Function \"" + AnsiString(Method->Name().c_str()) + "\" throw unexpected exception";
      PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),ErrStr.c_str());
      return NULL;
    }

  return StrToPyObjectUsingTypeID(PyEng,Ret,Method->GetReturnTypeID());
}

// Initialize
void __fastcall TQPythonIntegratorDM::PythonTypeInitialize(TObject *Sender)
{
  TPythonType *TmpPyType = dynamic_cast<TPythonType *>(Sender);

  TmpPyType->TheType.tp_basicsize = sizeof(TQComponentProxy);
  TmpPyType->TheType.tp_dealloc = QComponentProxyDealloc;
  TmpPyType->TheType.tp_getattr = QComponentProxyGetAttr;
  TmpPyType->TheType.tp_setattr = QComponentProxySetAttr;
  TmpPyType->TheType.tp_repr = QComponentProxyRepr;
  TmpPyType->TheType.tp_str = QComponentProxyRepr;

  CQComponent *Component = QPythonIntegratorDM->m_QComponentsDict[TmpPyType->TypeName];

  // Add all registered methods in the component to python type definitions
  for(int i = 0; i < Component->MethodCount(); i++)
  {
    CQMethodObject *Method = Component->Methods[i];

    // Duplicate method name into a new memory block
    char *MethodName = strdup(Method->Name().c_str());

    // Add for later cleanup
    m_MethodsCleanup.push_back(MethodName);

    AnsiString MethodDoc = AnsiString(MethodName) + " (" + IntToStr(Method->GetMethodArgumentsCount()) + " arguments)";
    char *MethodDocCStr = strdup(MethodDoc.c_str());

    // Add for later cleanup
    m_MethodsCleanup.push_back(MethodDocCStr);
    
    // Add the method to this type, connect the method object as an "extra" argument
    TmpPyType->AddMethodWithExtraArg(MethodName,QComponentCallWrapper,Method,MethodDocCStr);
  }
}

// Create python instances for every registered QComponent object
void TQPythonIntegratorDM::CreatePythonReferences(void)
{
  TStringList *PythonCode = new TStringList;

  try
  {
    // All the QComponents are registered under the "QLib" python module
    PythonCode->Add("import QLib");

    for(TQComponentsDict::iterator Itr = m_QComponentsDict.begin(); Itr != m_QComponentsDict.end(); Itr++)
      PythonCode->Add("QLib." + Itr->first + "=QLib.CreateQComponentRef('" + Itr->first + "')");

    PythonEngine1->ExecStrings(PythonCode);

  } __finally
    {
      delete PythonCode;
    }
}

// Create a unique QComponents dictionary with only one entry per type (QComponent)
void TQPythonIntegratorDM::CreateComponentsDictionary(void)
{
  // Get a pointer to the objects roster
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  // Roster might not be initalized yet...
  if(Roster == NULL)
    return;

  m_QComponentsDict.clear();

  // Process all registered components in the roster's list
  for(TQComponentList::iterator ComponentsItr = Roster->GetComponentList()->begin(); ComponentsItr !=
      Roster->GetComponentList()->end(); ComponentsItr++)
  {
    CQComponent *Component = *ComponentsItr;

    // Check if the dictionary has already an entry for this component
    if(m_QComponentsDict.find(Component->Name().c_str()) != m_QComponentsDict.end())
    {
      int NameSuffix = 1;

      // Loop until finding a non-colliding name
      while(m_QComponentsDict.find(Component->Name().c_str() + IntToStr(NameSuffix)) != m_QComponentsDict.end())
        NameSuffix++;

      // Add the entry to the dictionary
      m_QComponentsDict[Component->Name().c_str() + IntToStr(NameSuffix)] = Component;
    } else
        m_QComponentsDict[Component->Name().c_str()] = Component;
  }
}

// Implement QSleep
PPyObject TQPythonIntegratorDM::QSleepPy(PPyObject self,PPyObject args)
{
  TPythonEngine *PyEng = GetPythonEngine();

  // We are expecting one argument
  if(PyEng->PyTuple_Size(args) != 1)
  {
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Invalid numbers of parameters for QSleep");
    return NULL;
  }

  // Get the time parameter
  PPyObject SleepTime = PyEng->PyTuple_GetItem(args,0);
  QSleep(PyEng->PyInt_AsLong(SleepTime));

  return PyEng->ReturnNone();
}

// Implement QSleepSec
PPyObject TQPythonIntegratorDM::QSleepSecPy(PPyObject self,PPyObject args)
{
  TPythonEngine *PyEng = GetPythonEngine();

  // We are expecting one argument
  if(PyEng->PyTuple_Size(args) != 1)
  {
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Invalid numbers of parameters for QSleepSec");
    return NULL;
  }

  // Get the time parameter
  PPyObject SleepTime = PyEng->PyTuple_GetItem(args,0);
  long SleepTimeSec = PyEng->PyInt_AsLong(SleepTime);
  QSleepSec(SleepTimeSec);

  return PyEng->ReturnNone();
}

// Implement QGetTicks
PPyObject TQPythonIntegratorDM::QGetTicksPy(PPyObject self,PPyObject args)
{
  TPythonEngine *PyEng = GetPythonEngine();

  // We are expecting no argument
  if(PyEng->PyTuple_Size(args) != 0)
  {
    PyEng->PyErr_SetString(*(PyEng->PyExc_RuntimeError),"Invalid numbers of parameters for QGetTicks");
    return NULL;
  }

  // Get the current tick
  int Tick = QGetTicks();
  return PyEng->PyInt_FromLong(Tick);
}

void __fastcall TQPythonIntegratorDM::PythonInputOutput1ReceiveData(
      TObject *Sender, AnsiString &Data)
{
  Data = InputBox("Python input","","");

  if(Data == "")
    Data = "\n";
}

//---------------------------------------------------------------------------

void TQPythonIntegratorDM::ExecString(const AnsiString String)
{
  PythonEngine1->ExecString(String);
}

// Extract line number and position of the error
void TQPythonIntegratorDM::HandleSyntaxError(const QString ErrStr)
{
  int Line,Offset;

  if(sscanf(ErrStr.c_str(),"SyntaxError: invalid syntax (line %d, offset %d):",&Line,&Offset) == 2)
  {
    // Call the user "Syntax Error" callback
    if(m_OnSyntaxErrorEvent != NULL)
      m_OnSyntaxErrorEvent(this,Line,Offset);    
  }
}

void TQPythonIntegratorDM::RegisterExceptionThrowingThread(DWORD ThreadID)
{
    m_ThreadsIDs.insert(ThreadID);
}

// Utility functions
// ********************************************************************

// Verify that an attribute is a member of an object, raise exception if not
static void VerifyAttribute(TPythonEngine *PyEng,PPyObject PyObj,const QString AttributeName)
{
 if(!PyEng->PyObject_HasAttrString(PyObj,const_cast<char *>(AttributeName.c_str())))
 {
   QString ObjStr = PyEng->PyObjectAsString(PyObj).c_str();
   throw EQPython("Attribute \"" + AttributeName + "\" could not be found in object \"" + ObjStr + "\".");
 }
}

// Get a value from python wizard page object as string
QString GetAttrFromPyObjAsString(PPyObject PyObj,const QString Str)
{
  TPythonEngine *PyEng = GetPythonEngine();

  VerifyAttribute(PyEng,PyObj,Str);
  CPyAutoDecRef TmpObj(PyEng->PyObject_GetAttr(PyObj,PyEng->PyString_FromString(const_cast<char *>(Str.c_str()))));
  AnsiString Ret = PyEng->PyObjectAsString(TmpObj);

  return Ret.c_str();
}

// Get a value from python wizard page object as int
int GetAttrFromPyObjAsInt(PPyObject PyObj,const QString Str)
{
  TPythonEngine *PyEng = GetPythonEngine();

  VerifyAttribute(PyEng,PyObj,Str);
  CPyAutoDecRef TmpObj(PyEng->PyObject_GetAttr(PyObj,PyEng->PyString_FromString(const_cast<char *>(Str.c_str()))));

  int Ret = PyEng->PyInt_AsLong(TmpObj);
  return Ret;
}

// Get a value from python wizard page object as float
double GetAttrFromPyObjAsFloat(PPyObject PyObj,const QString Str)
{
  TPythonEngine *PyEng = GetPythonEngine();

  VerifyAttribute(PyEng,PyObj,Str);
  CPyAutoDecRef TmpObj(PyEng->PyObject_GetAttr(PyObj,PyEng->PyString_FromString(const_cast<char *>(Str.c_str()))));

  double Ret = PyEng->PyFloat_AsDouble(TmpObj);
  return Ret;
}

// Get a value from python wizard page object as string list
void GetAttrFromPyObjAsStringList(PPyObject PyObj,const QString Str,CQStringList& StrList)
{
  TPythonEngine *PyEng = GetPythonEngine();

  VerifyAttribute(PyEng,PyObj,Str);
  CPyAutoDecRef ListObj(PyEng->PyObject_GetAttr(PyObj,PyEng->PyString_FromString(const_cast<char *>(Str.c_str()))));

  // Check if the return type is a list
  if(!PyEng->PyList_Check(ListObj))
    throw EQPython("Object \"" + Str + "\" is not a list");

  StrList.Clear();

  int ItemNum = PyEng->PyList_Size(ListObj);

  for(int i = 0; i < ItemNum; i++)
  {
    PPyObject ItemObj = PyEng->PyList_GetItem(ListObj,i);

    AnsiString AnsiStr = PyEng->PyObjectAsString(ItemObj);
    StrList.Add(AnsiStr.c_str());
  }
}

// Set an attribute of an object (as string)
void SetAttrOfPyObjAsString(PPyObject PageObj,const QString Attr,const QString Value)
{
  TPythonEngine *PyEng = GetPythonEngine();

  CPyAutoDecRef ValueObj(PyEng->PyString_FromString(const_cast<char *>(Value.c_str())));
  PyEng->PyObject_SetAttr(PageObj,PyEng->PyString_FromString(const_cast<char *>(Attr.c_str())),ValueObj);
}

// Set an attribute of an object (as int)
void SetAttrOfPyObjAsInt(PPyObject PageObj,const QString Attr,int Value)
{
  TPythonEngine *PyEng = GetPythonEngine();

  CPyAutoDecRef ValueObj(PyEng->PyLong_FromLong(Value));
  PyEng->PyObject_SetAttr(PageObj,PyEng->PyString_FromString(const_cast<char *>(Attr.c_str())),ValueObj);
}

// Set an attribute of an object (as float)
void SetAttrOfPyObjAsFloat(PPyObject PageObj,const QString Attr,double Value)
{
  TPythonEngine *PyEng = GetPythonEngine();

  CPyAutoDecRef ValueObj(PyEng->PyFloat_FromDouble(Value));
  PyEng->PyObject_SetAttr(PageObj,PyEng->PyString_FromString(const_cast<char *>(Attr.c_str())),ValueObj);
}

// Convert a string to a python object according to a type ID parameter
PPyObject StrToPyObjectUsingTypeID(TPythonEngine *PyEng,const QString& Str,TPropertyTypeID TypeID)
{
  switch(TypeID)
  {
    //case ptGeneric:

    case ptInt:
      return PyEng->PyInt_FromLong(QStrToInt(Str));

    case ptReal:
      return PyEng->PyFloat_FromDouble(QStrToFloat(Str));

    case ptBool:
      return PyEng->PyInt_FromLong((QStrToInt(Str) != 0) ? 1 : 0);
  }

  // Generic case...
  return PyEng->PyString_FromString(const_cast<char *>(Str.c_str()));
}

