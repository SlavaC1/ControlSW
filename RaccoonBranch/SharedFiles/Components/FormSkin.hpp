// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Formskin.pas' rev: 11.00

#ifndef FormskinHPP
#define FormskinHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <Extctrls.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Formskin
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS EFormSkinException;
class PASCALIMPLEMENTATION EFormSkinException : public Sysutils::Exception 
{
	typedef Sysutils::Exception inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EFormSkinException(const AnsiString Msg) : Sysutils::Exception(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EFormSkinException(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EFormSkinException(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EFormSkinException(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EFormSkinException(const AnsiString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EFormSkinException(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EFormSkinException(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EFormSkinException(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EFormSkinException(void) { }
	#pragma option pop
	
};


#pragma option push -b-
enum TSkinOption { soCaption, soBorder, soControls, soSkin, soAutoSize, soClientDrag };
#pragma option pop

typedef Set<TSkinOption, soCaption, soClientDrag>  TSkinOptions;

#pragma option push -b-
enum THitArea { haNone, haClient, haCaptionBar, haSysMenu, haMinimizeButton, haMaximizeButton, haCloseButton, haTopBorder, haBottomBorder, haLeftBorder, haRightBorder, haTopLeftCorner, haTopRightCorner, haBottomLeftCorner, haBottomRightCorner, haGrowBox };
#pragma option pop

typedef void __fastcall (__closure *THitAreaEvent)(System::TObject* Sender, int X, int Y, THitArea &Area);

typedef void __fastcall (__closure *TTransparencyEvent)(System::TObject* Sender, int X, int Y, bool &Transparent);

typedef void __fastcall (__closure *TControlTransparencyEvent)(System::TObject* Sender, Controls::TControl* Control, bool &Transparent);

class DELPHICLASS TCustomFormSkin;
class PASCALIMPLEMENTATION TCustomFormSkin : public Classes::TComponent 
{
	typedef Classes::TComponent inherited;
	
private:
	HRGN FRegion;
	bool FLockSizeMessages;
	TSkinOptions FOptions;
	bool FActive;
	Menus::TPopupMenu* FPopupMenu;
	void *FDefaultProc;
	void *FHookProc;
	THitAreaEvent FOnHitArea;
	TTransparencyEvent FOnTransparency;
	TControlTransparencyEvent FOnControlTransparency;
	void __fastcall SetOptions(const TSkinOptions Value);
	void __fastcall SetActive(const bool Value);
	int __fastcall BorderHeight(void);
	int __fastcall BorderWidth(void);
	void __fastcall HookProc(Messages::TMessage &Message);
	int __fastcall HitAreaToHitTest(THitArea Value);
	THitArea __fastcall HitTestToHitArea(int Value);
	
protected:
	virtual void __fastcall HitArea(int X, int Y, THitArea &Area);
	virtual HRGN __fastcall CreateRegion(void);
	virtual HRGN __fastcall CreateCaptionRegion(void);
	virtual HRGN __fastcall CreateBorderRegion(void);
	virtual HRGN __fastcall CreateControlsRegion(void);
	virtual HRGN __fastcall CreateSkinRegion(void);
	virtual int __fastcall GetSkinWidth(void);
	virtual int __fastcall GetSkinHeight(void);
	virtual bool __fastcall IsTransparent(int X, int Y);
	virtual bool __fastcall IsTransparentControl(Controls::TControl* Control);
	__property TSkinOptions Options = {read=FOptions, write=SetOptions, nodefault};
	__property bool Active = {read=FActive, write=SetActive, nodefault};
	__property Menus::TPopupMenu* PopupMenu = {read=FPopupMenu, write=FPopupMenu};
	__property THitAreaEvent OnHitArea = {read=FOnHitArea, write=FOnHitArea};
	__property TTransparencyEvent OnTransparency = {read=FOnTransparency, write=FOnTransparency};
	__property TControlTransparencyEvent OnControlTransparency = {read=FOnControlTransparency, write=FOnControlTransparency};
	
public:
	__fastcall virtual ~TCustomFormSkin(void);
	virtual void __fastcall Update(void);
public:
	#pragma option push -w-inl
	/* TComponent.Create */ inline __fastcall virtual TCustomFormSkin(Classes::TComponent* AOwner) : Classes::TComponent(AOwner) { }
	#pragma option pop
	
};


class DELPHICLASS TSimpleFormSkin;
class PASCALIMPLEMENTATION TSimpleFormSkin : public TCustomFormSkin 
{
	typedef TCustomFormSkin inherited;
	
__published:
	__property Options ;
	__property Active ;
	__property PopupMenu ;
	__property OnHitArea ;
	__property OnTransparency ;
	__property OnControlTransparency ;
public:
	#pragma option push -w-inl
	/* TCustomFormSkin.Destroy */ inline __fastcall virtual ~TSimpleFormSkin(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TComponent.Create */ inline __fastcall virtual TSimpleFormSkin(Classes::TComponent* AOwner) : TCustomFormSkin(AOwner) { }
	#pragma option pop
	
};


class DELPHICLASS TBitmapFormSkin;
class PASCALIMPLEMENTATION TBitmapFormSkin : public TCustomFormSkin 
{
	typedef TCustomFormSkin inherited;
	
private:
	Graphics::TBrush* FDesignBrush;
	Graphics::TColor FTransparentColor;
	Graphics::TBitmap* FSkin;
	bool FPreview;
	bool FMouseDownFlag;
	Types::TPoint FMouseDownPoint;
	Types::TPoint FFormStartDragPoint;
	Byte *FPreCompiledBuffer;
	unsigned FPreCompiledBufferSize;
	void __fastcall SetTransparentColor(const Graphics::TColor Value);
	void __fastcall SetSkin(const Graphics::TBitmap* Value);
	void __fastcall SetPreview(const bool Value);
	void __fastcall AssignBrush(void);
	void __fastcall RestoreBrush(void);
	void __fastcall SkinChange(System::TObject* Sender);
	void __fastcall FormPaintHandler(System::TObject* Sender);
	void __fastcall FormMouseDownHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall FormMouseUpHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall FormMouseMoveHandler(System::TObject* Sender, Classes::TShiftState Shift, int X, int Y);
	
protected:
	virtual int __fastcall GetSkinWidth(void);
	virtual int __fastcall GetSkinHeight(void);
	virtual bool __fastcall IsTransparent(int X, int Y);
	virtual HRGN __fastcall CreateSkinRegion(void);
	
public:
	__fastcall virtual TBitmapFormSkin(Classes::TComponent* AOwner);
	__fastcall virtual ~TBitmapFormSkin(void);
	virtual void __fastcall Update(void);
	void __fastcall SetPreCompiledRegion(System::PByte Buffer, unsigned BufferSize);
	
__published:
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, nodefault};
	__property Graphics::TBitmap* Skin = {read=FSkin, write=SetSkin};
	__property bool Preview = {read=FPreview, write=SetPreview, nodefault};
	__property Options ;
	__property Active ;
	__property PopupMenu ;
	__property OnHitArea ;
	__property OnControlTransparency ;
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Formskin */
using namespace Formskin;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Formskin
