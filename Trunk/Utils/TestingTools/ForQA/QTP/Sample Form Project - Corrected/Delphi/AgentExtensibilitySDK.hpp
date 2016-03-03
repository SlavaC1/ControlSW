// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AgentExtensibilitySDK.pas' rev: 6.00

#ifndef AgentExtensibilitySDKHPP
#define AgentExtensibilitySDKHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Math.hpp>	// Pascal unit
#include <DB.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <TypInfo.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Agentextensibilitysdk
{
//-- type declarations -------------------------------------------------------
typedef OleVariant AgentExtensibilitySDK__1[33];

struct TServiceParam
{
	OleVariant Parameters[33];
	int NumOfParams;
} ;

class DELPHICLASS TMicAO;
class PASCALIMPLEMENTATION TMicAO : public System::TObject 
{
	typedef System::TObject inherited;
	
public:
	virtual void __fastcall Init(System::TObject* obj, TServiceParam &Param);
	virtual System::TObject* __fastcall GetSubObjectEx(AnsiString propname, AnsiString &SubObjectProp);
	virtual HRESULT __fastcall GetPropertyEx(AnsiString propname, OleVariant &Value);
	
protected:
	System::TObject* FObject;
	TServiceParam *pParam;
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAO(void) : System::TObject() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAO(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicControlAO;
class PASCALIMPLEMENTATION TMicControlAO : public TMicAO 
{
	typedef TMicAO inherited;
	
protected:
	Controls::TWinControl* __fastcall Get_Parent(void);
	
__published:
	__property Controls::TWinControl* Parent = {read=Get_Parent};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicControlAO(void) : TMicAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicControlAO(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicWinControlAO;
class PASCALIMPLEMENTATION TMicWinControlAO : public TMicControlAO 
{
	typedef TMicControlAO inherited;
	
public:
	virtual System::TObject* __fastcall GetSubObjectEx(AnsiString propname, AnsiString &SubObjectProp);
	
protected:
	int __fastcall GetControlCount(void);
	HWND __fastcall Get_Handle(void);
	HWND __fastcall Get_ParentWindow(void);
	bool __fastcall Get_Showing(void);
	Controls::TTabOrder __fastcall Get_TabOrder(void);
	bool __fastcall Get_TabStop(void);
	
__published:
	__property int ControlCount = {read=GetControlCount, nodefault};
	__property HWND Handle = {read=Get_Handle, nodefault};
	__property HWND ParentWindow = {read=Get_ParentWindow, nodefault};
	__property bool Showing = {read=Get_Showing, nodefault};
	__property Controls::TTabOrder TabOrder = {read=Get_TabOrder, nodefault};
	__property bool TabStop = {read=Get_TabStop, nodefault};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicWinControlAO(void) : TMicControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicWinControlAO(void) { }
	#pragma option pop
	
};


typedef DynamicArray<OleVariant >  AgentExtensibilitySDK__5;

#pragma pack(push, 4)
struct TRecordLine
{
	AnsiString Cmd;
	DynamicArray<OleVariant >  Params;
	int SendMode;
} ;
#pragma pack(pop)

class DELPHICLASS TMicRecordableAO;
class PASCALIMPLEMENTATION TMicRecordableAO : public TMicWinControlAO 
{
	typedef TMicWinControlAO inherited;
	
protected:
	virtual bool __fastcall ProcessMessage(unsigned handle, const Messages::TMessage &Msg, TRecordLine &RecLine) = 0 ;
	
private:
	bool __fastcall GetMessageParm(Messages::TMessage &Msg)/* overload */;
	bool __fastcall GetMessageParm(unsigned &handle, Messages::TMessage &Msg)/* overload */;
	OleVariant __fastcall PackRecordLine(AnsiString Cmd, const OleVariant * Params, const int Params_Size, int SendMode);
	virtual OleVariant __fastcall Get_Recording();
	
__published:
	__property OleVariant __QTPRecording = {read=Get_Recording};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicRecordableAO(void) : TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicRecordableAO(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicGridAOBase;
class PASCALIMPLEMENTATION TMicGridAOBase : public TMicRecordableAO 
{
	typedef TMicRecordableAO inherited;
	
protected:
	virtual AnsiString __fastcall GetCellDataEx(int row, int col) = 0 ;
	virtual void __fastcall SetCellDataEx(int row, int col, AnsiString Value) = 0 ;
	virtual bool __fastcall CaptureTableEx(AnsiString filename) = 0 ;
	virtual int __fastcall RowCountEx(void) = 0 ;
	virtual int __fastcall ColCountEx(void) = 0 ;
	virtual int __fastcall GetSelectedRowEx(void) = 0 ;
	virtual BOOL __fastcall MakeCellVisibleEx(int row, int col) = 0 ;
	virtual int __fastcall GridIdxToVisibleRowNum(AnsiString row_str) = 0 ;
	virtual bool __fastcall GetCellRectEx(int Row, int Column, Types::TRect &rect) = 0 ;
	BOOL __fastcall GetCellParm(AnsiString &row_str, AnsiString &col_str);
	int __fastcall RowStrToInt(AnsiString row_str);
	int __fastcall ColumnStrToInt(AnsiString col_str);
	AnsiString __fastcall ReplaceIllegalCharacters(AnsiString S);
	AnsiString __fastcall GetStringFromTField(Db::TField* Field);
	void __fastcall SetStringToTField(Db::TField* Field, AnsiString Value);
	
private:
	virtual AnsiString __fastcall Get_CellRect();
	AnsiString __fastcall Get_CellData();
	void __fastcall Set_CellData(AnsiString Value);
	int __fastcall Get_RowCount(void);
	int __fastcall Get_ColCount(void);
	AnsiString __fastcall Get_SelectedCell();
	bool __fastcall Get_TableContent(void);
	
__published:
	__property AnsiString __CellRect = {read=Get_CellRect};
	__property AnsiString __CellData = {read=Get_CellData, write=Set_CellData};
	__property bool __TableContent = {read=Get_TableContent, nodefault};
	__property int RowCount = {read=Get_RowCount, nodefault};
	__property int ColCount = {read=Get_ColCount, nodefault};
	__property AnsiString SelectedCell = {read=Get_SelectedCell};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicGridAOBase(void) : TMicRecordableAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicGridAOBase(void) { }
	#pragma option pop
	
};


class DELPHICLASS TCustomGridAOBase;
class PASCALIMPLEMENTATION TCustomGridAOBase : public TMicGridAOBase 
{
	typedef TMicGridAOBase inherited;
	
protected:
	virtual bool __fastcall ProcessMessage(unsigned handle, const Messages::TMessage &Msg, TRecordLine &RecLine);
	virtual int __fastcall RowCountEx(void);
	virtual int __fastcall ColCountEx(void);
	virtual int __fastcall GetSelectedRowEx(void);
	virtual BOOL __fastcall MakeCellVisibleEx(int row, int col);
	virtual bool __fastcall GetCellRectEx(int Row, int Column, Types::TRect &rect);
	virtual bool __fastcall CaptureTableEx(AnsiString filename);
	bool __fastcall GetCellFromPoint(System::TObject* obj, const Types::TPoint &p, int &row, int &col);
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TCustomGridAOBase(void) : TMicGridAOBase() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TCustomGridAOBase(void) { }
	#pragma option pop
	
};


typedef HRESULT __stdcall (*TCreateAOFromHandle)(System::TObject* obj, TMicAO* &AgentObject);

//-- var, const, procedure ---------------------------------------------------
static const Shortint MAX_PARAM = 0x20;
static const short ERR_INT = 0xfffffc19;
static const Shortint MIC_MAX_HEADER_LENGTH = 0x19;
#define FAIL_TO_GET_CELL_VALUE "Fail_TO_GET_CELL_VALUE"
static const Shortint SEND_LINE = 0x0;
static const Shortint KEEP_LINE = 0x1;
static const Shortint KEEP_LINE_NO_TIMEOUT = 0x2;
static const Shortint REPLACE_AND_KEEP_LINE_NO_TIMEOUT = 0x3;
static const Shortint REPLACE_AND_SEND_LINE = 0x4;
static const Shortint REPLACE_AND_KEEP_LINE = 0x5;
static const Shortint CLEAN_UP = 0x6;
static const Shortint STORED_LINE = 0x7;
static const Shortint NO_PROCESS = 0x8;
extern PACKAGE void __fastcall AddExtensibilityServer(TCreateAOFromHandle VCLObj2AO);
extern PACKAGE Word __fastcall IsInheritFrom(System::TObject* Obj, AnsiString AClass);
extern PACKAGE int __fastcall MicStrToInt(AnsiString S);
extern PACKAGE Word __fastcall TypeKind2VarType(Typinfo::TTypeKind Kind);
extern PACKAGE bool __fastcall ParseIndexedProperty(AnsiString RawProp, AnsiString &PropName, int &Index);
extern PACKAGE bool __fastcall ParseSubProperty(AnsiString RawProp, AnsiString &PropertyName, AnsiString &SubPropertyName);

}	/* namespace Agentextensibilitysdk */
using namespace Agentextensibilitysdk;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AgentExtensibilitySDK
