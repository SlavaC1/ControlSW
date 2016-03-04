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

#ifndef QPythonIntegratorH
#define QPythonIntegratorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PythonEngine.hpp"
#include <vector>
#include <map>
#include <set>
#include "QTypes.h"
#include "QException.h"
#include "QMutex.h"

class CQComponent;
class CQStringList;

typedef std::vector<PCHAR> TPCharList;
typedef std::map<AnsiString,CQComponent *> TQComponentsDict;
typedef std::set<DWORD> TThreadsIDsSet;
typedef std::vector<AnsiString> TQFunctionsList;

typedef void __fastcall (__closure *TSyntaxErrorEvent)(TObject *Sender,int Line,int Offset);

// Exception class for the QPython library
class EQPython : public EQException {
  public:
    EQPython(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

//---------------------------------------------------------------------------
class TQPythonIntegratorDM : public TDataModule
{
__published:	// IDE-managed Components
        TPythonEngine *PythonEngine1;
        TPythonInputOutput *PythonInputOutput1;
        TPythonModule *PythonModule1;
        void __fastcall PythonModule1Initialization(TObject *Sender);
        void __fastcall DataModuleCreate(TObject *Sender);
        void __fastcall DataModuleDestroy(TObject *Sender);
        void __fastcall PythonInputOutput1SendData(TObject *Sender,
          const AnsiString Data);
        void __fastcall PythonInputOutput1ReceiveData(TObject *Sender,
          AnsiString &Data);
private:	// User declarations
  TPCharList m_MethodsCleanup;

  // QComponents dictionary
  TQComponentsDict m_QComponentsDict;

  // Functions list
  TQFunctionsList m_QFunctionsList;

  TSyntaxErrorEvent m_OnSyntaxErrorEvent;

  // Create a unique QComponents dictionary with only one entry per type (QComponent)
  void CreateComponentsDictionary(void);

  void CreatePythonReferences(void);

  void __fastcall PythonTypeInitialize(TObject *Sender);

  void HandleSyntaxError(const QString ErrStr);

  static PPyObject CreateQComponentRef(PPyObject self,PPyObject args);
  static PPyObject QComponentCallWrapper(void *MethodObjPtr,PPyObject self,PPyObject args);

  // Utility QLib functions that need to be exposed to python
  static PPyObject QSleepPy(PPyObject self,PPyObject args);
  static PPyObject QSleepSecPy(PPyObject self,PPyObject args);
  static PPyObject QGetTicksPy(PPyObject self,PPyObject args);

  static PPyObject BreakpointHandler(PPyObject self,PPyObject args);

  // This set holds threads IDs, which allow exception throwing from ExecStringsSafe and ExecFileSafe
  TThreadsIDsSet m_ThreadsIDs;

  CQMutex m_PythonEngine1_Mutex;

public:		// User declarations
  __fastcall TQPythonIntegratorDM(TComponent* Owner);

  void Init(void);

  void ExecStrings(TStrings *Strings,bool ClearDictionary = true);
  void ExecStringsSafe(TStrings *Strings,bool ClearDictionary = true);
  void ExecString(const AnsiString String);
  void ExecFile(AnsiString FileName,bool ClearDictionary = true);
  void ExecFileSafe(AnsiString FileName,bool ClearDictionary = true);

  // Return a reference to the components dictionary
  const TQComponentsDict& GetComponentsDict(void) {
    return m_QComponentsDict;
  }

  // Return a reference to the Q fucntions list
  const TQFunctionsList& GetQFunctionsList(void) {
    return m_QFunctionsList;
  }

  void RegisterExceptionThrowingThread(DWORD ThreadID);
  
  __property TSyntaxErrorEvent OnSyntaxErrorEvent = {read=m_OnSyntaxErrorEvent,write=m_OnSyntaxErrorEvent};
};

// Some utility functions...

// Get a value from python object as string
QString GetAttrFromPyObjAsString(PPyObject PyObj,const QString Str);

// Get a value from python object as int
int GetAttrFromPyObjAsInt(PPyObject PyObj,const QString Str);

// Get a value from python object as float
double GetAttrFromPyObjAsFloat(PPyObject PyObj,const QString Str);

// Get a value from python object as string list
void GetAttrFromPyObjAsStringList(PPyObject PyObj,const QString Str,CQStringList& StrList);

// Set an attribute of an object (as srting)
void SetAttrOfPyObjAsString(PPyObject PyObj,const QString Attr,const QString Value);

// Set an attribute of an object (as int)
void SetAttrOfPyObjAsInt(PPyObject PyObj,const QString Attr,int Value);

// Set an attribute of an object (as float)
void SetAttrOfPyObjAsFloat(PPyObject PyObj,const QString Attr,double Value);

//---------------------------------------------------------------------------
extern PACKAGE TQPythonIntegratorDM *QPythonIntegratorDM;
//---------------------------------------------------------------------------
#endif
