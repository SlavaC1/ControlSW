// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Enhancedmenu.pas' rev: 11.00

#ifndef EnhancedmenuHPP
#define EnhancedmenuHPP

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
#include <Controls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Enhancedmenu
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TEnhancedMenu;
class PASCALIMPLEMENTATION TEnhancedMenu : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	Graphics::TBitmap* FLeftPart;
	Graphics::TBitmap* FMiddlePart;
	Graphics::TBitmap* FRightPart;
	Graphics::TBitmap* FHighlightedLeftPart;
	Graphics::TBitmap* FHighlightedMiddlePart;
	Graphics::TBitmap* FHighlightedRightPart;
	int FCaptionXOffset;
	int FCaptionYOffset;
	Classes::TAlignment FAlignment;
	int FMiddlePartsNum;
	Classes::TStringList* FItems;
	bool FTransparent;
	Graphics::TTransparentMode FTransparentMode;
	Graphics::TColor FTransparentColor;
	Graphics::TFont* FFont;
	Graphics::TFont* FHighlightedFont;
	int FSpacing;
	int FCurrentItem;
	bool FLeftButtonIsDown;
	int FItemAtMouseDown;
	Classes::TNotifyEvent FOnClick;
	bool FRestoreHighlightOnMouseUp;
	void __fastcall UpdateWidthAndHeight(void);
	bool __fastcall HasDimension(Graphics::TBitmap* Bmp);
	void __fastcall SetLeftPart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetMiddlePart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetRightPart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetHighlightedLeftPart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetHighlightedMiddlePart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetHighlightedRightPart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetCaptionXOffset(int X);
	void __fastcall SetCaptionYOffset(int Y);
	void __fastcall SetTransparent(bool Transparent);
	void __fastcall SetTransparentMode(Graphics::TTransparentMode NewMode);
	void __fastcall SetTransparentColor(Graphics::TColor NewColor);
	void __fastcall SetMiddlePartsNum(int PartsNum);
	void __fastcall SetItems(Classes::TStringList* NewStringList);
	void __fastcall SetSpacing(int NewValue);
	HIDESBASE void __fastcall SetFont(Graphics::TFont* NewFont);
	void __fastcall SetHighlightedFont(Graphics::TFont* NewFont);
	void __fastcall SetAlignment(Classes::TAlignment NewAlignment);
	void __fastcall DrawMenuItem(int X, int Y, AnsiString Caption, bool Highlighted);
	int __fastcall CalcMenuItemY(int ItemNum);
	void __fastcall HighlightItem(int ItemNum);
	int __fastcall FindItemAtGivenPosition(int X, int Y);
	void __fastcall MouseDownHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall MouseUpHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall MouseMoveHandler(System::TObject* Sender, Classes::TShiftState Shift, int X, int Y);
	
public:
	virtual void __fastcall Paint(void);
	void __fastcall Redraw(void);
	__fastcall virtual TEnhancedMenu(Classes::TComponent* AOwner);
	__fastcall virtual ~TEnhancedMenu(void);
	void __fastcall MoveDown(void);
	void __fastcall MoveUp(void);
	void __fastcall Activate(void);
	
__published:
	__property Graphics::TBitmap* LeftPart = {read=FLeftPart, write=SetLeftPart};
	__property Graphics::TBitmap* RightPart = {read=FRightPart, write=SetRightPart};
	__property Graphics::TBitmap* MiddlePart = {read=FMiddlePart, write=SetMiddlePart};
	__property Graphics::TBitmap* HighlightedLeftPart = {read=FHighlightedLeftPart, write=SetHighlightedLeftPart};
	__property Graphics::TBitmap* HighlightedRightPart = {read=FHighlightedRightPart, write=SetHighlightedRightPart};
	__property Graphics::TBitmap* HighlightedMiddlePart = {read=FHighlightedMiddlePart, write=SetHighlightedMiddlePart};
	__property int CaptionXOffset = {read=FCaptionXOffset, write=SetCaptionXOffset, nodefault};
	__property int CaptionYOffset = {read=FCaptionYOffset, write=SetCaptionYOffset, nodefault};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, nodefault};
	__property Graphics::TTransparentMode TransparentMode = {read=FTransparentMode, write=SetTransparentMode, nodefault};
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, nodefault};
	__property Classes::TAlignment Alignment = {read=FAlignment, write=SetAlignment, nodefault};
	__property int MiddlePartsNum = {read=FMiddlePartsNum, write=SetMiddlePartsNum, nodefault};
	__property Graphics::TFont* Font = {read=FFont, write=SetFont};
	__property Graphics::TFont* HighlightedFont = {read=FHighlightedFont, write=SetHighlightedFont};
	__property int Spacing = {read=FSpacing, write=SetSpacing, nodefault};
	__property Classes::TStringList* Items = {read=FItems, write=SetItems};
	__property int ItemIndex = {read=FCurrentItem, write=HighlightItem, nodefault};
	__property Classes::TNotifyEvent OnClick = {read=FOnClick, write=FOnClick};
	__property bool RestoreHighlightOnMouseUp = {read=FRestoreHighlightOnMouseUp, write=FRestoreHighlightOnMouseUp, nodefault};
	__property Visible  = {default=1};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Enhancedmenu */
using namespace Enhancedmenu;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Enhancedmenu
