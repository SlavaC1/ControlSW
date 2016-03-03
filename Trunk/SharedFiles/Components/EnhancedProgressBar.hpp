// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Enhancedprogressbar.pas' rev: 11.00

#ifndef EnhancedprogressbarHPP
#define EnhancedprogressbarHPP

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

namespace Enhancedprogressbar
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
enum TEnhancedProgressBarOrientation { pbHorizontal, pbVertical };
#pragma option pop

class DELPHICLASS TEnhancedProgressBar;
class PASCALIMPLEMENTATION TEnhancedProgressBar : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	Graphics::TBitmap* FBodyBitmap;
	Graphics::TBitmap* FProgressBitmap;
	Graphics::TTransparentMode FTransparentMode;
	Graphics::TColor FTransparentColor;
	int FMin;
	int FMax;
	int FPosition;
	TEnhancedProgressBarOrientation FOrientation;
	int FMaxProgressBitmaps;
	bool FReverseDirection;
	int FXOffset;
	int FYOffset;
	void __fastcall SetOrientation(TEnhancedProgressBarOrientation NewValue);
	void __fastcall SetTransparentMode(Graphics::TTransparentMode NewMode);
	void __fastcall SetTransparentColor(Graphics::TColor NewColor);
	bool __fastcall GetBodyBitmapTransparent(void);
	void __fastcall SetBodyBitmapTransparent(bool Transparent);
	bool __fastcall GetProgressBitmapTransparent(void);
	void __fastcall SetProgressBitmapTransparent(bool Transparent);
	void __fastcall SetBodyBitmap(Graphics::TBitmap* NewBitmap);
	void __fastcall SetProgressBitmap(Graphics::TBitmap* NewBitmap);
	void __fastcall SetMin(int NewValue);
	void __fastcall SetMax(int NewValue);
	void __fastcall SetPosition(int NewValue);
	void __fastcall SetMaxProgressBitmaps(int NewValue);
	void __fastcall Redraw(void);
	bool __fastcall HasDimension(Graphics::TBitmap* Bmp);
	void __fastcall SetReverseDirection(bool Direction);
	void __fastcall SetXOffset(int NewValue);
	void __fastcall SetYOffset(int NewValue);
	
public:
	virtual void __fastcall Paint(void);
	__fastcall virtual TEnhancedProgressBar(Classes::TComponent* AOwner);
	__fastcall virtual ~TEnhancedProgressBar(void);
	
__published:
	__property Graphics::TBitmap* BodyBitmap = {read=FBodyBitmap, write=SetBodyBitmap};
	__property Graphics::TBitmap* ProgressBitmap = {read=FProgressBitmap, write=SetProgressBitmap};
	__property Graphics::TTransparentMode TransparentMode = {read=FTransparentMode, write=SetTransparentMode, nodefault};
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, nodefault};
	__property bool BodyBitmapTransparent = {read=GetBodyBitmapTransparent, write=SetBodyBitmapTransparent, nodefault};
	__property bool ProgressBitmapTransparent = {read=GetProgressBitmapTransparent, write=SetProgressBitmapTransparent, nodefault};
	__property int Min = {read=FMin, write=SetMin, nodefault};
	__property int Max = {read=FMax, write=SetMax, nodefault};
	__property int Position = {read=FPosition, write=SetPosition, nodefault};
	__property TEnhancedProgressBarOrientation Orientation = {read=FOrientation, write=SetOrientation, nodefault};
	__property int MaxProgressBitmaps = {read=FMaxProgressBitmaps, write=SetMaxProgressBitmaps, nodefault};
	__property bool ReverseDirection = {read=FReverseDirection, write=SetReverseDirection, nodefault};
	__property int XOffset = {read=FXOffset, write=SetXOffset, nodefault};
	__property int YOffset = {read=FYOffset, write=SetYOffset, nodefault};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Enhancedprogressbar */
using namespace Enhancedprogressbar;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Enhancedprogressbar
