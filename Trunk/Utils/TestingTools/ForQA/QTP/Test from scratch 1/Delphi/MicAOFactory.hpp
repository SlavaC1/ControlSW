// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'MicAOFactory.pas' rev: 6.00

#ifndef MicAOFactoryHPP
#define MicAOFactoryHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <AgentExtensibilitySDK.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Micaofactory
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TMicMenuItem;
class PASCALIMPLEMENTATION TMicMenuItem : public Agentextensibilitysdk::TMicAO 
{
	typedef Agentextensibilitysdk::TMicAO inherited;
	
public:
	int __fastcall GetCount(void);
	virtual System::TObject* __fastcall GetSubObjectEx(AnsiString propname, AnsiString &SubObjectProp);
	
__published:
	__property int Count = {read=GetCount, nodefault};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicMenuItem(void) : Agentextensibilitysdk::TMicAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicMenuItem(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicToolBar;
class PASCALIMPLEMENTATION TMicToolBar : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
public:
	virtual System::TObject* __fastcall GetSubObjectEx(AnsiString propname, AnsiString &SubObjectProp);
	int __fastcall GetButtonCount(void);
	
__published:
	__property int ButtonCount = {read=GetButtonCount, nodefault};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicToolBar(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicToolBar(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicCheckListBox;
class PASCALIMPLEMENTATION TMicCheckListBox : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
public:
	virtual HRESULT __fastcall GetPropertyEx(AnsiString propname, OleVariant &Value);
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicCheckListBox(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicCheckListBox(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicApplication;
class PASCALIMPLEMENTATION TMicApplication : public Agentextensibilitysdk::TMicAO 
{
	typedef Agentextensibilitysdk::TMicAO inherited;
	
protected:
	System::TObject* __fastcall GetMainForm(void);
	
__published:
	__property System::TObject* MainForm = {read=GetMainForm};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicApplication(void) : Agentextensibilitysdk::TMicAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicApplication(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicForm;
class PASCALIMPLEMENTATION TMicForm : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
protected:
	int __fastcall ReplayRestore(void);
	int __fastcall ReplayMinimize(void);
	
__published:
	__property int __QTPReplayMtd_Restore = {read=ReplayRestore, nodefault};
	__property int __QTPReplayMtd_Minimize = {read=ReplayMinimize, nodefault};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicForm(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicForm(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE HRESULT __stdcall MicVCLObj2AO(System::TObject* obj, Agentextensibilitysdk::TMicAO* &AgentObject);

}	/* namespace Micaofactory */
using namespace Micaofactory;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MicAOFactory
