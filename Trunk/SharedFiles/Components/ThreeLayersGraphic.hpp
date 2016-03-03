// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Threelayersgraphic.pas' rev: 11.00

#ifndef ThreelayersgraphicHPP
#define ThreelayersgraphicHPP

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

namespace Threelayersgraphic
{
//-- type declarations -------------------------------------------------------
typedef void __fastcall (__closure *TLayer2DrawEvent)(System::TObject* Sender, Graphics::TCanvas* DrawCanvas);

#pragma option push -b-
enum TAlphaBlendMode { Blend7Bit, Blend8Bit };
#pragma option pop

class DELPHICLASS TThreeLayersGraphic;
class PASCALIMPLEMENTATION TThreeLayersGraphic : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	Graphics::TBitmap* FLayer1;
	Graphics::TBitmap* FLayer3;
	Graphics::TBitmap* FAlphaLayer;
	Graphics::TBitmap* FVScreen;
	int FLayer3XOffset;
	int FLayer3YOffset;
	TLayer2DrawEvent FLayer2DrawEvent;
	Graphics::TColor FBackgroundColor;
	bool FUseBackgroundColor;
	TAlphaBlendMode FAlphaBlendMode;
	bool FVisible;
	Graphics::TTransparentMode FTransparentMode;
	Graphics::TColor FTransparentColor;
	bool __fastcall HasDimension(Graphics::TBitmap* Bmp);
	void __fastcall ResizeVScreen(void);
	bool __fastcall GetLayer1Transparent(void);
	void __fastcall SetLayer1Transparent(bool Transparent);
	bool __fastcall GetLayer3Transparent(void);
	void __fastcall SetLayer3Transparent(bool Transparent);
	bool __fastcall GetVScreenTransparent(void);
	void __fastcall SetVScreenTransparent(bool Transparent);
	void __fastcall SetTransparentMode(Graphics::TTransparentMode NewMode);
	void __fastcall SetTransparentColor(Graphics::TColor NewColor);
	void __fastcall SetLayer1(Graphics::TBitmap* NewBitmap);
	void __fastcall SetLayer3(Graphics::TBitmap* NewBitmap);
	void __fastcall SetAlphaLayer(Graphics::TBitmap* NewBitmap);
	void __fastcall SetLayer3XOffset(int X);
	void __fastcall SetLayer3YOffset(int Y);
	void __fastcall SetBackgroundColor(Graphics::TColor NewColor);
	void __fastcall SetUseBackgroundColor(bool UseIt);
	void __fastcall SetAlphaBlendMode(TAlphaBlendMode AlphaBlendMode);
	HIDESBASE void __fastcall SetVisible(bool Visible);
	
public:
	virtual void __fastcall Paint(void);
	void __fastcall Redraw(void);
	__fastcall virtual TThreeLayersGraphic(Classes::TComponent* AOwner);
	__fastcall virtual ~TThreeLayersGraphic(void);
	
__published:
	__property Graphics::TBitmap* Layer1 = {read=FLayer1, write=SetLayer1};
	__property Graphics::TBitmap* Layer3 = {read=FLayer3, write=SetLayer3};
	__property Graphics::TBitmap* AlphaLayer = {read=FAlphaLayer, write=SetAlphaLayer};
	__property int Layer3XOffset = {read=FLayer3XOffset, write=SetLayer3XOffset, nodefault};
	__property int Layer3YOffset = {read=FLayer3YOffset, write=SetLayer3YOffset, nodefault};
	__property bool Layer1Transparent = {read=GetLayer1Transparent, write=SetLayer1Transparent, nodefault};
	__property bool Layer3Transparent = {read=GetLayer3Transparent, write=SetLayer3Transparent, nodefault};
	__property bool VScreenTransparent = {read=GetVScreenTransparent, write=SetVScreenTransparent, nodefault};
	__property Graphics::TTransparentMode TransparentMode = {read=FTransparentMode, write=SetTransparentMode, nodefault};
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, nodefault};
	__property TLayer2DrawEvent Layer2DrawEvent = {read=FLayer2DrawEvent, write=FLayer2DrawEvent};
	__property Graphics::TColor BackgroundColor = {read=FBackgroundColor, write=SetBackgroundColor, nodefault};
	__property bool UseBackgroundColor = {read=FUseBackgroundColor, write=SetUseBackgroundColor, nodefault};
	__property TAlphaBlendMode AlphaBlendMode = {read=FAlphaBlendMode, write=SetAlphaBlendMode, nodefault};
	__property bool Visible = {read=FVisible, write=SetVisible, nodefault};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Threelayersgraphic */
using namespace Threelayersgraphic;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Threelayersgraphic
