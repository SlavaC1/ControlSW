// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'MicAOFactoryMgr.pas' rev: 6.00

#ifndef MicAOFactoryMgrHPP
#define MicAOFactoryMgrHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <MicDelphiGridSupport.hpp>	// Pascal unit
#include <MicAOFactory.hpp>	// Pascal unit
#include <AgentExtensibilitySDK.hpp>	// Pascal unit
#include <CheckLst.hpp>	// Pascal unit
#include <ComCtrls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit
#include <TypInfo.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Micaofactorymgr
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TMicAOWrp;
class PASCALIMPLEMENTATION TMicAOWrp : public System::TObject 
{
	typedef System::TObject inherited;
	
public:
	System::TObject* FObject;
	Agentextensibilitysdk::TMicAO* FAO;
	Agentextensibilitysdk::TServiceParam FParam;
	void __fastcall Init(System::TObject* obj, Agentextensibilitysdk::TMicAO* AO);
	HRESULT __fastcall GetProperty(AnsiString propname, OleVariant &value);
	HRESULT __fastcall SetProperty(AnsiString propname, const Variant &value);
	HRESULT __fastcall GetAllPropertyNames(AnsiString &PropNames, int &Count)/* overload */;
	HRESULT __fastcall GetAllPropertyNames(AnsiString Container, AnsiString &PropNames, int &Count)/* overload */;
	
protected:
	HRESULT __fastcall GetObjectProperty(System::TObject* obj, AnsiString propname, OleVariant &value);
	HRESULT __fastcall SetObjectProperty(System::TObject* obj, AnsiString propname, const Variant &value);
	HRESULT __fastcall GetObjectAllPropertyNames(System::TObject* Obj, AnsiString &PropNames, int &Count)/* overload */;
	HRESULT __fastcall GetObjectAllPropertyNames(System::TObject* obj, AnsiString Container, AnsiString &PropNames, int &Count)/* overload */;
	Word __fastcall GetObjectPropertyType(System::TObject* obj, AnsiString propname);
	
private:
	bool __fastcall IsBitOn(int Value, Byte Bit);
	AnsiString __fastcall GetValOfSet(System::TObject* obj, Typinfo::PPropInfo PropInfo);
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAOWrp(void) : System::TObject() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAOWrp(void) { }
	#pragma option pop
	
};


#pragma pack(push, 4)
struct TExtensibilityFactory
{
	int hLib;
	AnsiString szLibPath;
	Agentextensibilitysdk::TCreateAOFromHandle VCLObj2AO;
} ;
#pragma pack(pop)

typedef TExtensibilityFactory MicAOFactoryMgr__3[1025];

class DELPHICLASS TAOFactoryMgr;
class PASCALIMPLEMENTATION TAOFactoryMgr : public System::TObject 
{
	typedef System::TObject inherited;
	
public:
	__fastcall TAOFactoryMgr(void);
	__fastcall virtual ~TAOFactoryMgr(void);
	void __fastcall CreateAOFromHandle(System::TObject* obj, TMicAOWrp* &AgentObject);
	void __fastcall AddExtensibilityServer(AnsiString szLibPath)/* overload */;
	void __fastcall AddExtensibilityServer(Agentextensibilitysdk::TCreateAOFromHandle VCLObj2AO)/* overload */;
	
private:
	TExtensibilityFactory ExtensibilityFactories[1025];
	int FactoriesCount;
};


//-- var, const, procedure ---------------------------------------------------
static const Shortint MAX_PARAM = 0x20;
static const Word MAX_EXT_FACTORIES = 0x400;
extern PACKAGE TAOFactoryMgr* __fastcall GetAOFactoryMgr(void);

}	/* namespace Micaofactorymgr */
using namespace Micaofactorymgr;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MicAOFactoryMgr
