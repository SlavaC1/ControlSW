// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Enhancedbutton.pas' rev: 11.00

#ifndef EnhancedbuttonHPP
#define EnhancedbuttonHPP

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

namespace Enhancedbutton
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
enum TCurrentButtonState { bsUp, bsHighlighted, bsDown };
#pragma option pop

class DELPHICLASS TEnhancedButton;
class PASCALIMPLEMENTATION TEnhancedButton : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	Graphics::TBitmap* FUpBmp;
	Graphics::TBitmap* FHighlightedBmp;
	Graphics::TBitmap* FDownBmp;
	Graphics::TBitmap* FDisabledBmp;
	Graphics::TTransparentMode FTransparentMode;
	Graphics::TColor FTransparentColor;
	bool FTransparent;
	bool FMouseIsDown;
	bool MouseIsIn;
	Classes::TNotifyEvent FOnClick;
	Controls::TMouseEvent FOnMouseDown;
	Controls::TMouseEvent FOnMouseUp;
	Controls::TMouseMoveEvent FOnMouseMove;
	Classes::TNotifyEvent FOnMouseEnter;
	Classes::TNotifyEvent FOnMouseLeave;
	TCurrentButtonState CurrentState;
	Graphics::TBitmap* FMask;
	bool FUseMask;
	bool FShowMask;
	int FMaskXOffset;
	int FMaskYOffset;
	Classes::TBasicAction* FAction;
	bool __fastcall HitTest(Graphics::TBitmap* Bmp, Graphics::TColor TransparentColor, int X, int Y);
	bool __fastcall HasDimension(Graphics::TBitmap* Bmp);
	void __fastcall SetTransparentMode(Graphics::TTransparentMode NewMode);
	void __fastcall SetTransparentColor(Graphics::TColor NewColor);
	void __fastcall SetUpBmp(Graphics::TBitmap* NewBitmap);
	void __fastcall SetHighlightedBmp(Graphics::TBitmap* NewBitmap);
	void __fastcall SetDownBmp(Graphics::TBitmap* NewBitmap);
	void __fastcall SetDisabledBmp(Graphics::TBitmap* NewBitmap);
	void __fastcall SetTransparent(bool NewMode);
	void __fastcall SetMask(Graphics::TBitmap* NewMask);
	void __fastcall SetShowMask(bool ShowMask);
	void __fastcall SetMaskXOffset(int NewOffset);
	void __fastcall SetMaskYOffset(int NewOffset);
	void __fastcall OnMouseDownHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall OnMouseUpHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall OnMouseMoveHandler(System::TObject* Sender, Classes::TShiftState Shift, int X, int Y);
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(Messages::TMessage &AMsg);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &AMsg);
	
public:
	virtual void __fastcall Paint(void);
	void __fastcall Redraw(void);
	__fastcall virtual TEnhancedButton(Classes::TComponent* AOwner);
	__fastcall virtual ~TEnhancedButton(void);
	virtual void __fastcall SetEnabled(bool Value);
	
__published:
	__property Graphics::TBitmap* UpBmp = {read=FUpBmp, write=SetUpBmp};
	__property Graphics::TBitmap* HighlightedBmp = {read=FHighlightedBmp, write=SetHighlightedBmp};
	__property Graphics::TBitmap* DownBmp = {read=FDownBmp, write=SetDownBmp};
	__property Graphics::TBitmap* DisabledBmp = {read=FDisabledBmp, write=SetDisabledBmp};
	__property Graphics::TTransparentMode TransparentMode = {read=FTransparentMode, write=SetTransparentMode, nodefault};
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, nodefault};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, nodefault};
	__property Classes::TNotifyEvent OnClick = {read=FOnClick, write=FOnClick};
	__property Controls::TMouseEvent OnMouseDown = {read=FOnMouseDown, write=FOnMouseDown};
	__property Controls::TMouseEvent OnMouseUp = {read=FOnMouseUp, write=FOnMouseUp};
	__property Controls::TMouseMoveEvent OnMouseMove = {read=FOnMouseMove, write=FOnMouseMove};
	__property Classes::TNotifyEvent OnMouseEnter = {read=FOnMouseEnter, write=FOnMouseEnter};
	__property Classes::TNotifyEvent OnMouseLeave = {read=FOnMouseLeave, write=FOnMouseLeave};
	__property Graphics::TBitmap* Mask = {read=FMask, write=SetMask};
	__property bool UseMask = {read=FUseMask, write=FUseMask, nodefault};
	__property bool ShowMask = {read=FShowMask, write=SetShowMask, nodefault};
	__property int MaskXOffset = {read=FMaskXOffset, write=SetMaskXOffset, nodefault};
	__property int MaskYOffset = {read=FMaskYOffset, write=SetMaskYOffset, nodefault};
	__property Classes::TBasicAction* Action = {read=FAction, write=FAction};
	__property Enabled  = {default=1};
	__property Visible  = {default=1};
	__property Hint ;
	__property ShowHint ;
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Enhancedbutton */
using namespace Enhancedbutton;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Enhancedbutton
