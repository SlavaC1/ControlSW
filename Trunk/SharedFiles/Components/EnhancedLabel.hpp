// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Enhancedlabel.pas' rev: 11.00

#ifndef EnhancedlabelHPP
#define EnhancedlabelHPP

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

namespace Enhancedlabel
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TEnhancedLabel;
class PASCALIMPLEMENTATION TEnhancedLabel : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	Graphics::TBitmap* FLeftPart;
	Graphics::TBitmap* FMiddlePart;
	Graphics::TBitmap* FRightPart;
	Graphics::TBitmap* FLeftPart1;
	Graphics::TBitmap* FMiddlePart1;
	Graphics::TBitmap* FRightPart1;
	Graphics::TBitmap* FLeftPart2;
	Graphics::TBitmap* FMiddlePart2;
	Graphics::TBitmap* FRightPart2;
	int FCaptionXOffset;
	int FCaptionYOffset;
	Classes::TAlignment FAlignment;
	int FMiddlePartsNum;
	AnsiString FCaption;
	bool FTransparent;
	Graphics::TTransparentMode FTransparentMode;
	Graphics::TColor FTransparentColor;
	int FActiveBitmap;
	void __fastcall AllocateBitmaps(void);
	void __fastcall UpdateWidthAndHeight(void);
	bool __fastcall HasDimension(Graphics::TBitmap* Bmp);
	void __fastcall SetLeftPart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetMiddlePart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetRightPart(Graphics::TBitmap* NewBitmap);
	void __fastcall SetLeftPart1(Graphics::TBitmap* NewBitmap);
	void __fastcall SetMiddlePart1(Graphics::TBitmap* NewBitmap);
	void __fastcall SetRightPart1(Graphics::TBitmap* NewBitmap);
	void __fastcall SetLeftPart2(Graphics::TBitmap* NewBitmap);
	void __fastcall SetMiddlePart2(Graphics::TBitmap* NewBitmap);
	void __fastcall SetRightPart2(Graphics::TBitmap* NewBitmap);
	void __fastcall SetCaptionXOffset(int X);
	void __fastcall SetCaptionYOffset(int Y);
	void __fastcall SetTransparent(bool Transparent);
	void __fastcall SetTransparentMode(Graphics::TTransparentMode NewMode);
	void __fastcall SetTransparentColor(Graphics::TColor NewColor);
	void __fastcall SetMiddlePartsNum(int PartsNum);
	void __fastcall SetCaption(AnsiString NewCaption);
	HIDESBASE void __fastcall SetFont(Graphics::TFont* NewFont);
	Graphics::TFont* __fastcall GetFont(void);
	void __fastcall SetAlignment(Classes::TAlignment NewAlignment);
	void __fastcall SetActiveBitmap(int Value);
	Graphics::TBitmap* __fastcall GetActiveLeftPart(void);
	Graphics::TBitmap* __fastcall GetActiveMiddlePart(void);
	Graphics::TBitmap* __fastcall GetActiveRightPart(void);
	
public:
	virtual void __fastcall Paint(void);
	void __fastcall Redraw(void);
	__fastcall virtual TEnhancedLabel(Classes::TComponent* AOwner);
	__fastcall virtual ~TEnhancedLabel(void);
	
__published:
	__property Graphics::TBitmap* LeftPart = {read=FLeftPart, write=SetLeftPart};
	__property Graphics::TBitmap* RightPart = {read=FRightPart, write=SetRightPart};
	__property Graphics::TBitmap* MiddlePart = {read=FMiddlePart, write=SetMiddlePart};
	__property Graphics::TBitmap* LeftPart1 = {read=FLeftPart1, write=SetLeftPart1};
	__property Graphics::TBitmap* RightPart1 = {read=FRightPart1, write=SetRightPart1};
	__property Graphics::TBitmap* MiddlePart1 = {read=FMiddlePart1, write=SetMiddlePart1};
	__property Graphics::TBitmap* LeftPart2 = {read=FLeftPart2, write=SetLeftPart2};
	__property Graphics::TBitmap* RightPart2 = {read=FRightPart2, write=SetRightPart2};
	__property Graphics::TBitmap* MiddlePart2 = {read=FMiddlePart2, write=SetMiddlePart2};
	__property int CaptionXOffset = {read=FCaptionXOffset, write=SetCaptionXOffset, nodefault};
	__property int CaptionYOffset = {read=FCaptionYOffset, write=SetCaptionYOffset, nodefault};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, nodefault};
	__property Graphics::TTransparentMode TransparentMode = {read=FTransparentMode, write=SetTransparentMode, nodefault};
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, nodefault};
	__property Classes::TAlignment Alignment = {read=FAlignment, write=SetAlignment, nodefault};
	__property int MiddlePartsNum = {read=FMiddlePartsNum, write=SetMiddlePartsNum, nodefault};
	__property Graphics::TFont* Font = {read=GetFont, write=SetFont};
	__property int ActiveBitmap = {read=FActiveBitmap, write=SetActiveBitmap, nodefault};
	__property AnsiString Caption = {read=FCaption, write=SetCaption};
	__property OnMouseDown ;
	__property OnMouseUp ;
	__property OnClick ;
	__property OnDblClick ;
	__property Visible  = {default=1};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Enhancedlabel */
using namespace Enhancedlabel;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Enhancedlabel
