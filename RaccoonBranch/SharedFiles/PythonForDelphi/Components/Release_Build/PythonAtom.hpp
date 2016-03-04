// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Pythonatom.pas' rev: 11.00

#ifndef PythonatomHPP
#define PythonatomHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Comobj.hpp>	// Pascal unit
#include <Pythonengine.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Pythonatom
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TPythonAtom;
class PASCALIMPLEMENTATION TPythonAtom : public System::TInterfacedObject 
{
	typedef System::TInterfacedObject inherited;
	
private:
	Pythonengine::PyObject *FPythonObject;
	OleVariant __fastcall GetAsAtom();
	
protected:
	virtual void __fastcall SetPythonObject(const Pythonengine::PPyObject Value);
	void __fastcall PythonAddRef(void);
	void __fastcall PythonRelease(void);
	
public:
	HRESULT __stdcall GetTypeInfoCount(/* out */ int &Count);
	HRESULT __stdcall GetTypeInfo(int Index, int LocaleID, /* out */ void *TypeInfo);
	HRESULT __stdcall GetIDsOfNames(const GUID &IID, void * Names, int NameCount, int LocaleID, void * DispIDs);
	HRESULT __stdcall Invoke(int DispID, const GUID &IID, int LocaleID, Word Flags, void *Params, void * VarResult, void * ExcepInfo, void * ArgErr);
	__fastcall virtual TPythonAtom(Pythonengine::PPyObject pObject);
	__fastcall virtual ~TPythonAtom(void);
	void __fastcall AddRef(void);
	void __fastcall Release(void);
	__property Pythonengine::PPyObject PythonObject = {read=FPythonObject, write=SetPythonObject};
	__property OleVariant asAtom = {read=GetAsAtom};
private:
	void *__IDispatch;	/* IDispatch */
	
public:
	#if defined(MANAGED_INTERFACE_OPERATORS)
	operator _di_IDispatch()
	{
		_di_IDispatch intf;
		GetInterface(intf);
		return intf;
	}
	#else
	operator IDispatch*(void) { return (IDispatch*)&__IDispatch; }
	#endif
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE OleVariant __fastcall GetAtom(Pythonengine::PPyObject pObject);

}	/* namespace Pythonatom */
using namespace Pythonatom;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Pythonatom
