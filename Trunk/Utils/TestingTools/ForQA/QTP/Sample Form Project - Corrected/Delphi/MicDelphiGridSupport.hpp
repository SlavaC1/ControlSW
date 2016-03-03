// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'MicDelphiGridSupport.pas' rev: 6.00

#ifndef MicDelphiGridSupportHPP
#define MicDelphiGridSupportHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Mask.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <DBCtrls.hpp>	// Pascal unit
#include <DBGrids.hpp>	// Pascal unit
#include <Grids.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <DBTables.hpp>	// Pascal unit
#include <DB.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <AgentExtensibilitySDK.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Micdelphigridsupport
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TMicAODBGrid;
class PASCALIMPLEMENTATION TMicAODBGrid : public Agentextensibilitysdk::TCustomGridAOBase 
{
	typedef Agentextensibilitysdk::TCustomGridAOBase inherited;
	
protected:
	virtual bool __fastcall ProcessMessage(unsigned handle, const Messages::TMessage &Msg, Agentextensibilitysdk::TRecordLine &RecLine);
	virtual AnsiString __fastcall GetCellDataEx(int row, int column);
	virtual void __fastcall SetCellDataEx(int row, int column, AnsiString Value);
	virtual int __fastcall RowCountEx(void);
	virtual int __fastcall ColCountEx(void);
	virtual int __fastcall GridIdxToVisibleRowNum(AnsiString row_str);
	virtual int __fastcall GetSelectedRowEx(void);
	virtual bool __fastcall CaptureTableEx(AnsiString filename);
	
public:
	virtual System::TObject* __fastcall GetSubObjectEx(AnsiString propname, AnsiString &SubObjectProp);
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAODBGrid(void) : Agentextensibilitysdk::TCustomGridAOBase() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAODBGrid(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicAODBGridInplaceEdit;
class PASCALIMPLEMENTATION TMicAODBGridInplaceEdit : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
protected:
	AnsiString __fastcall Get_Recording();
	
__published:
	__property AnsiString __QTPRecording = {read=Get_Recording};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAODBGridInplaceEdit(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAODBGridInplaceEdit(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicAOPopupListbox;
class PASCALIMPLEMENTATION TMicAOPopupListbox : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
private:
	AnsiString __fastcall GetItemFromPoint(const Types::TPoint &p);
	int __fastcall ItemToIndex(AnsiString item);
	
protected:
	AnsiString __fastcall Get_Recording();
	HWND __fastcall Get_MercHandle(void);
	AnsiString __fastcall Get_ItemPoint();
	
__published:
	__property AnsiString __QTPRecording = {read=Get_Recording};
	__property HWND __MercHandle = {read=Get_MercHandle, nodefault};
	__property AnsiString __ItemPoint = {read=Get_ItemPoint};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAOPopupListbox(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAOPopupListbox(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicAOStringGrid;
class PASCALIMPLEMENTATION TMicAOStringGrid : public Agentextensibilitysdk::TCustomGridAOBase 
{
	typedef Agentextensibilitysdk::TCustomGridAOBase inherited;
	
protected:
	virtual AnsiString __fastcall GetCellDataEx(int row, int column);
	virtual void __fastcall SetCellDataEx(int row, int column, AnsiString Value);
	virtual int __fastcall GridIdxToVisibleRowNum(AnsiString row_str);
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAOStringGrid(void) : Agentextensibilitysdk::TCustomGridAOBase() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAOStringGrid(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicAODBLookupComboBox;
class PASCALIMPLEMENTATION TMicAODBLookupComboBox : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
protected:
	AnsiString __fastcall Get_Text();
	bool __fastcall Get_MercEOF(void);
	
__published:
	__property AnsiString Text = {read=Get_Text};
	__property bool __MercEOF = {read=Get_MercEOF, nodefault};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAODBLookupComboBox(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAODBLookupComboBox(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMicAODBLookupListBox;
class PASCALIMPLEMENTATION TMicAODBLookupListBox : public Agentextensibilitysdk::TMicWinControlAO 
{
	typedef Agentextensibilitysdk::TMicWinControlAO inherited;
	
protected:
	AnsiString __fastcall Get_Recording();
	HWND __fastcall Get_MercHandle(void);
	AnsiString __fastcall Get_Text();
	
__published:
	__property AnsiString __QTPRecording = {read=Get_Recording};
	__property HWND __MercHandle = {read=Get_MercHandle, nodefault};
	__property AnsiString Text = {read=Get_Text};
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TMicAODBLookupListBox(void) : Agentextensibilitysdk::TMicWinControlAO() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMicAODBLookupListBox(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------

}	/* namespace Micdelphigridsupport */
using namespace Micdelphigridsupport;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MicDelphiGridSupport
