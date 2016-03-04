// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Labelmenu.pas' rev: 11.00

#ifndef LabelmenuHPP
#define LabelmenuHPP

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
#include <Stdctrls.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Labelmenu
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TLabelMenu;
class PASCALIMPLEMENTATION TLabelMenu : public Stdctrls::TLabel 
{
	typedef Stdctrls::TLabel inherited;
	
private:
	Menus::TPopupMenu* FMenu;
	Graphics::TColor FDownColor;
	Graphics::TColor FUpColor;
	Graphics::TColor FHighlightedColor;
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(Messages::TMessage &AMsg);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &AMsg);
	void __fastcall MouseDownHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall MouseUpHandler(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	
public:
	__fastcall virtual TLabelMenu(Classes::TComponent* AOwner);
	__fastcall virtual ~TLabelMenu(void);
	
__published:
	__property Menus::TPopupMenu* Menu = {read=FMenu, write=FMenu};
	__property Graphics::TColor HighlightedColor = {read=FHighlightedColor, write=FHighlightedColor, nodefault};
	__property Graphics::TColor DownColor = {read=FDownColor, write=FDownColor, nodefault};
	__property Graphics::TColor Color = {read=FUpColor, write=FUpColor, nodefault};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Labelmenu */
using namespace Labelmenu;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Labelmenu
