// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Memocomponentunit.pas' rev: 11.00

#ifndef MemocomponentunitHPP
#define MemocomponentunitHPP

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
#include <Forms.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Stdctrls.hpp>	// Pascal unit
#include <Objlist.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Memocomponentunit
{
//-- type declarations -------------------------------------------------------
typedef void __fastcall (__closure *TBreakpointClickEvent)(System::TObject* Sender, int BreakpointNum);

typedef void __fastcall (__closure *TCaretPosChanged)(System::TObject* Sender, int X, int Y);

typedef DynamicArray<void * >  TFormattedRangeArray;

#pragma pack(push,4)
struct TTextCell
{
	
public:
	int Row;
	int Col;
} ;
#pragma pack(pop)

struct TUndoOperation;
typedef TUndoOperation *PUndoOperation;

#pragma pack(push,4)
struct TUndoOperation
{
	
public:
	int RStart;
	int REnd;
	AnsiString NewText;
	TUndoOperation *NextItem;
} ;
#pragma pack(pop)

#pragma pack(push,4)
struct TBreakpoint
{
	
public:
	int LineNum;
	bool Enabled;
	SmallString<128>  BrkName;
	SmallString<128>  BrkConditional;
} ;
#pragma pack(pop)

class DELPHICLASS TMemoComponent;
class DELPHICLASS TIntegerList;
class PASCALIMPLEMENTATION TIntegerList : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	Classes::TList* FList;
	int __fastcall GetCount(void);
	int __fastcall GetItem(int ItemIndex);
	void __fastcall SetItem(int ItemIndex, const int Value);
	void __fastcall SetCount(const int Value);
	
public:
	__fastcall TIntegerList(void);
	__fastcall virtual ~TIntegerList(void);
	int __fastcall Add(int Item);
	void __fastcall Insert(int Index, int Item);
	void __fastcall Delete(int Index);
	DYNAMIC void __fastcall Clear(void);
	__property int Items[int ItemIndex] = {read=GetItem, write=SetItem};
	__property int Count = {read=GetCount, write=SetCount, nodefault};
};


class DELPHICLASS TMCRanges;
typedef TMetaClass* TRangeClass;

class DELPHICLASS TCustomRange;
class PASCALIMPLEMENTATION TMCRanges : public Objlist::TObjectContainer 
{
	typedef Objlist::TObjectContainer inherited;
	
private:
	TMetaClass* FItemClass;
	TMemoComponent* __fastcall NewGetOwner(void);
	
protected:
	TCustomRange* __fastcall NewGetItem(int ItemIndex);
	void __fastcall NewSetItem(int ItemIndex, const TCustomRange* Value);
	
public:
	__fastcall TMCRanges(TMemoComponent* AOwner);
	HIDESBASE TCustomRange* __fastcall Add(void)/* overload */;
	HIDESBASE TCustomRange* __fastcall Add(int Start, int Count)/* overload */;
	__property TMetaClass* ItemClass = {read=FItemClass, write=FItemClass};
	__property TCustomRange* Items[int ItemIndex] = {read=NewGetItem, write=NewSetItem};
	__property TMemoComponent* Owner = {read=NewGetOwner};
public:
	#pragma option push -w-inl
	/* TCollection.Destroy */ inline __fastcall virtual ~TMCRanges(void) { }
	#pragma option pop
	
};


class DELPHICLASS TVisibleRange;
class PASCALIMPLEMENTATION TCustomRange : public Objlist::TContainerItem 
{
	typedef Objlist::TContainerItem inherited;
	
private:
	TMemoComponent* FEditor;
	int FChanging;
	Classes::TNotifyEvent FOnOverwrite;
	Classes::TNotifyEvent FOnChange;
	Types::TPoint __fastcall GetEndPoint();
	Types::TPoint __fastcall GetStartPoint();
	
protected:
	virtual void __fastcall SetRStart(const int Value);
	virtual void __fastcall SetREnd(const int Value);
	virtual void __fastcall SetRLength(const int Value);
	virtual int __fastcall GetRStart(void) = 0 ;
	virtual int __fastcall GetREnd(void) = 0 ;
	virtual int __fastcall GetRLength(void);
	virtual TTextCell __fastcall GetEndRowCol();
	virtual TTextCell __fastcall GetStartRowCol();
	virtual void __fastcall SetEndRowCol(const TTextCell &Value);
	virtual void __fastcall SetStartRowCol(const TTextCell &Value);
	virtual void __fastcall SetText(const AnsiString Value);
	virtual AnsiString __fastcall GetText();
	DYNAMIC void __fastcall Changing(void);
	DYNAMIC void __fastcall Change(void);
	DYNAMIC void __fastcall DiscardChanges(void);
	
public:
	__fastcall virtual TCustomRange(Classes::TCollection* Collection);
	virtual void __fastcall AssignTo(Classes::TPersistent* Dest);
	DYNAMIC void __fastcall NotifyOverwrite(void);
	void __fastcall DoChanging(void);
	void __fastcall DoChange(void);
	void __fastcall DoDiscardChanges(void);
	virtual void __fastcall Clear(void);
	bool __fastcall CharInRange(int CharIdx);
	virtual void __fastcall DrawRange(void);
	virtual void __fastcall ScrollInView(int FromBorder);
	__property TMemoComponent* Editor = {read=FEditor, write=FEditor};
	__property TTextCell StartRowCol = {read=GetStartRowCol, write=SetStartRowCol};
	__property TTextCell EndRowCol = {read=GetEndRowCol, write=SetEndRowCol};
	__property Types::TPoint StartPoint = {read=GetStartPoint};
	__property Types::TPoint EndPoint = {read=GetEndPoint};
	__property AnsiString Text = {read=GetText, write=SetText};
	
__published:
	__property int RStart = {read=GetRStart, write=SetRStart, nodefault};
	__property int REnd = {read=GetREnd, write=SetREnd, nodefault};
	__property int RLength = {read=GetRLength, write=SetRLength, nodefault};
	__property Classes::TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
	__property Classes::TNotifyEvent OnOverwrite = {read=FOnOverwrite, write=FOnOverwrite};
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TCustomRange(void) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TVisibleRange : public TCustomRange 
{
	typedef TCustomRange inherited;
	
private:
	int FLeftCol;
	int FTopRow;
	int __fastcall GetRightCol(void);
	void __fastcall SetRightCol(const int Value);
	void __fastcall SetLeftCol(const int Value);
	void __fastcall Update(void);
	
protected:
	Types::TRect VisibleTextRect;
	virtual void __fastcall SetRStart(const int Value);
	virtual void __fastcall SetREnd(const int Value);
	virtual void __fastcall SetRLength(const int Value);
	virtual int __fastcall GetRStart(void);
	virtual int __fastcall GetREnd(void);
	virtual TTextCell __fastcall GetStartRowCol();
	virtual TTextCell __fastcall GetEndRowCol();
	virtual void __fastcall SetStartRowCol(const TTextCell &Value);
	virtual void __fastcall SetEndRowCol(const TTextCell &Value);
	DYNAMIC void __fastcall Changing(void);
	DYNAMIC void __fastcall Change(void);
	
public:
	__fastcall virtual TVisibleRange(Classes::TCollection* Collection);
	
__published:
	__property int LeftCol = {read=FLeftCol, write=SetLeftCol, nodefault};
	__property int RightCol = {read=GetRightCol, write=SetRightCol, nodefault};
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TVisibleRange(void) { }
	#pragma option pop
	
};


class DELPHICLASS TSelectionRange;
class DELPHICLASS TMCRange;
class PASCALIMPLEMENTATION TMCRange : public TCustomRange 
{
	typedef TCustomRange inherited;
	
private:
	int FRStart;
	int FREnd;
	
protected:
	virtual void __fastcall SetREnd(const int Value);
	virtual void __fastcall SetRStart(const int Value);
	virtual int __fastcall GetREnd(void);
	virtual int __fastcall GetRStart(void);
	
public:
	__fastcall virtual TMCRange(Classes::TCollection* Collection);
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TMCRange(void) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TSelectionRange : public TMCRange 
{
	typedef TMCRange inherited;
	
private:
	TCustomRange* FOldSel;
	bool FBackwards;
	bool FHidden;
	bool FCaretShowing;
	int FScrCol;
	int __fastcall GetCursorPos(void);
	void __fastcall SetCursorPos(const int Value);
	void __fastcall SetHidden(const bool Value);
	int __fastcall GetScrCol(void);
	
protected:
	DYNAMIC void __fastcall Changing(void);
	DYNAMIC void __fastcall Change(void);
	DYNAMIC void __fastcall DiscardChanges(void);
	
public:
	virtual void __fastcall AssignTo(Classes::TPersistent* Dest);
	void __fastcall NoSelAtPos(int Pos);
	void __fastcall UpdateCaretPos(void);
	void __fastcall ShowCaret(void);
	void __fastcall HideCaret(void);
	int __fastcall ScrColToCol(int Row);
	__property int CursorPos = {read=GetCursorPos, write=SetCursorPos, nodefault};
	__property int ScrCol = {read=GetScrCol, write=FScrCol, nodefault};
	
__published:
	__property bool Backwards = {read=FBackwards, write=FBackwards, nodefault};
	__property bool Hidden = {read=FHidden, write=SetHidden, nodefault};
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TSelectionRange(Classes::TCollection* Collection) : TMCRange(Collection) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TSelectionRange(void) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TMemoComponent : public Controls::TCustomControl 
{
	typedef Controls::TCustomControl inherited;
	
private:
	bool FHasFocus;
	bool FCaretCreated;
	bool FSelecting;
	TIntegerList* FLineStarts;
	Stdctrls::TScrollStyle FScrollBars;
	Forms::TFormBorderStyle FBorderStyle;
	bool FReadOnly;
	Classes::TNotifyEvent FOnChange;
	AnsiString FText;
	TMCRanges* FTrackedRanges;
	TCustomRange* FWholeText;
	Classes::TStrings* FLines;
	TVisibleRange* FVisibleRange;
	TSelectionRange* FSelection;
	int FLongestLineLength;
	bool FAlwaysShowCaret;
	int FLeftMargin;
	int FTopMargin;
	int FTabSize;
	Classes::TNotifyEvent FOnSelectionChange;
	int FTextLength;
	bool FBitmapped;
	Classes::TNotifyEvent FOnChangePrivate;
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMSize(Messages::TWMSize &Message);
	HIDESBASE MESSAGE void __fastcall WMHScroll(Messages::TWMScroll &Message);
	HIDESBASE MESSAGE void __fastcall WMVScroll(Messages::TWMScroll &Message);
	HIDESBASE MESSAGE void __fastcall WMSetFocus(Messages::TWMSetFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMKillFocus(Messages::TWMKillFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMEraseBkgnd(Messages::TWMEraseBkgnd &Message);
	MESSAGE void __fastcall CMWantSpecialKey(Messages::TWMKey &Message);
	MESSAGE void __fastcall WMClear(Messages::TWMNoParams &Message);
	MESSAGE void __fastcall WMCut(Messages::TWMNoParams &Message);
	MESSAGE void __fastcall WMCopy(Messages::TWMNoParams &Message);
	MESSAGE void __fastcall WMPaste(Messages::TWMNoParams &Message);
	MESSAGE void __fastcall WMSetText(Messages::TWMSetText &Message);
	MESSAGE void __fastcall WMGetText(Messages::TWMGetText &Message);
	MESSAGE void __fastcall WMGetTextLength(Messages::TWMNoParams &Message);
	MESSAGE void __fastcall WMTimer(Messages::TWMTimer &Message);
	MESSAGE void __fastcall EMUndo(Messages::TMessage &Message);
	MESSAGE void __fastcall EMCanUndo(Messages::TMessage &Message);
	HIDESBASE void __fastcall SetText(const AnsiString Value);
	void __fastcall SetScrollBars(const Stdctrls::TScrollStyle Value);
	void __fastcall SetBorderStyle(const Forms::TBorderStyle Value);
	void __fastcall SetReadOnly(const bool Value);
	void __fastcall SetLines(const Classes::TStrings* Value);
	int __fastcall GetLineCount(void);
	int __fastcall GetLineLength(int LineIndex);
	int __fastcall GetVisualLineLength(int LineIndex);
	int __fastcall GetSelLength(void);
	int __fastcall GetSelStart(void);
	void __fastcall SetSelLength(const int Value);
	void __fastcall SetSelStart(const int Value);
	void __fastcall SetAlwaysShowCaret(const bool Value);
	void __fastcall SetLeftMargin(const int Value);
	void __fastcall SetTopMargin(const int Value);
	void __fastcall SetTabSize(const int Value);
	bool __fastcall GetCanRedo(void);
	bool __fastcall GetCanUndo(void);
	void __fastcall SetBitmapped(const bool Value);
	
protected:
	int FontHeight;
	int FontWidth;
	int PageHeight;
	int PageWidth;
	Graphics::TBitmap* DrawBmp;
	TUndoOperation *FUndoStack;
	TUndoOperation *FRedoStack;
	bool FInUndo;
	bool DontNotify;
	TBreakpoint FBreakpoints[16];
	TBreakpointClickEvent FOnBreakpointClick;
	TCaretPosChanged FOnCaretPosChanged;
	void __fastcall ToggleBreakpoint(int LineNum);
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	virtual void __fastcall CreateWnd(void);
	virtual void __fastcall ReplaceText(TCustomRange* Range, const AnsiString NewText);
	virtual void __fastcall DrawTextLine(TCustomRange* Range, int Left, int Top, int NextTabStop);
	virtual TFormattedRangeArray __fastcall CreateSplitRanges(TCustomRange* Range);
	virtual void __fastcall DrawBorder(const Types::TRect &LeftRect, const Types::TRect &TopRect, Graphics::TCanvas* Canvas);
	DYNAMIC void __fastcall TextChangeNotification(int StartPos, int OldLength, int NewLength);
	DYNAMIC void __fastcall TextChangeNotificationAfter(void);
	DYNAMIC void __fastcall Change(void);
	DYNAMIC void __fastcall SelectionChange(void);
	virtual void __fastcall UpdateFontSize(void);
	virtual void __fastcall UpdatePageSize(void);
	virtual void __fastcall UpdateDrawBmp(void);
	virtual void __fastcall ReCreateCaret(void);
	virtual void __fastcall FreeCaret(void);
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall MyMouseWheelHandler(System::TObject* Sender, Classes::TShiftState Shift, int WheelDelta, const Types::TPoint &MousePos, bool &Handled);
	DYNAMIC void __fastcall KeyPress(char &Key);
	DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
	virtual TUndoOperation __fastcall GetLastUndo();
	virtual TUndoOperation __fastcall GetLastRedo();
	virtual PUndoOperation __fastcall CreateUndoBeginEndBlock(void);
	virtual bool __fastcall IsUndoBeginEndBlock(PUndoOperation Op);
	virtual void __fastcall MakeUndoOperation(PUndoOperation Op);
	virtual void __fastcall MakeRedoOperation(PUndoOperation Op);
	
public:
	__fastcall virtual TMemoComponent(Classes::TComponent* AOwner);
	__fastcall virtual ~TMemoComponent(void);
	virtual void __fastcall Clear(void);
	virtual TTextCell __fastcall CharIdxToCell(int CharIdx);
	virtual int __fastcall CellToCharIdx(const TTextCell &Cell);
	virtual TTextCell __fastcall ScrPointToScrCell(const Types::TPoint &P);
	virtual Types::TPoint __fastcall ScrCellToScrPoint(const TTextCell &Cell);
	virtual int __fastcall TabSpacesAtPos(int P);
	virtual int __fastcall CellToScrCol(const TTextCell &Cell);
	virtual void __fastcall CellFromScrCol(TTextCell &Cell);
	virtual int __fastcall CellFromScrColToScrCol(TTextCell &Cell);
	void __fastcall SelectAll(void);
	void __fastcall ClearSelection(void);
	void __fastcall CutToClipboard(void);
	void __fastcall CopyToClipboard(void);
	void __fastcall PasteFromClipboard(void);
	void __fastcall Undo(void);
	void __fastcall Redo(void);
	void __fastcall ClearUndo(void);
	void __fastcall ClearRedo(void);
	virtual void __fastcall ScrollCaret(void);
	virtual void __fastcall ChangeIndent(int Change);
	int __fastcall GetBreakpointsNum(void);
	TBreakpoint __fastcall GetBreakpointByHandle(int BrkHandle);
	int __fastcall FindBreakpoint(int LineNum);
	void __fastcall RemoveBreakpointByNumber(int BrkNum);
	TBreakpoint __fastcall GetBreakpointByNumber(int BrkNum);
	void __fastcall SetBreakpointByNumber(int BrkNum, TBreakpoint &Brk);
	void __fastcall RemoveAllBreakpoints(void);
	void __fastcall GotoPos(int X, int Y);
	__property AnsiString Text = {read=FText, write=SetText};
	__property int TextLength = {read=FTextLength, nodefault};
	__property TMCRanges* TrackedRanges = {read=FTrackedRanges};
	__property TCustomRange* WholeText = {read=FWholeText};
	__property int LineCount = {read=GetLineCount, nodefault};
	__property int LongestLineLength = {read=FLongestLineLength, nodefault};
	__property int LineLength[int LineIndex] = {read=GetLineLength};
	__property int VisualLineLength[int LineIndex] = {read=GetVisualLineLength};
	__property TVisibleRange* VisibleRange = {read=FVisibleRange};
	__property TSelectionRange* Selection = {read=FSelection};
	__property int SelStart = {read=GetSelStart, write=SetSelStart, nodefault};
	__property int SelLength = {read=GetSelLength, write=SetSelLength, nodefault};
	__property bool CanUndo = {read=GetCanUndo, nodefault};
	__property bool CanRedo = {read=GetCanRedo, nodefault};
	__property Classes::TNotifyEvent OnChangePrivate = {read=FOnChangePrivate, write=FOnChangePrivate};
	
__published:
	__property Stdctrls::TScrollStyle ScrollBars = {read=FScrollBars, write=SetScrollBars, default=0};
	__property Forms::TBorderStyle BorderStyle = {read=FBorderStyle, write=SetBorderStyle, default=1};
	__property bool Bitmapped = {read=FBitmapped, write=SetBitmapped, nodefault};
	__property bool ReadOnly = {read=FReadOnly, write=SetReadOnly, default=0};
	__property Classes::TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
	__property Classes::TNotifyEvent OnSelectionChange = {read=FOnSelectionChange, write=FOnSelectionChange};
	__property Classes::TStrings* Lines = {read=FLines, write=SetLines};
	__property bool AlwaysShowCaret = {read=FAlwaysShowCaret, write=SetAlwaysShowCaret, nodefault};
	__property int LeftMargin = {read=FLeftMargin, write=SetLeftMargin, nodefault};
	__property int TopMargin = {read=FTopMargin, write=SetTopMargin, nodefault};
	__property int TabSize = {read=FTabSize, write=SetTabSize, nodefault};
	__property TabStop  = {default=1};
	__property Align  = {default=0};
	__property Anchors  = {default=3};
	__property Color ;
	__property Constraints ;
	__property Ctl3D ;
	__property Enabled  = {default=1};
	__property Font ;
	__property ParentColor  = {default=1};
	__property ParentCtl3D  = {default=1};
	__property ParentFont  = {default=1};
	__property ParentShowHint  = {default=1};
	__property PopupMenu ;
	__property ShowHint ;
	__property TabOrder  = {default=-1};
	__property Visible  = {default=1};
	__property OnClick ;
	__property OnDblClick ;
	__property OnEnter ;
	__property OnExit ;
	__property OnKeyDown ;
	__property OnKeyPress ;
	__property OnKeyUp ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
	__property TBreakpointClickEvent OnBreakpointClick = {read=FOnBreakpointClick, write=FOnBreakpointClick};
	__property TCaretPosChanged OnCaretPosChanged = {read=FOnCaretPosChanged, write=FOnCaretPosChanged};
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TMemoComponent(HWND ParentWindow) : Controls::TCustomControl(ParentWindow) { }
	#pragma option pop
	
};


class DELPHICLASS TWholeTextRange;
class PASCALIMPLEMENTATION TWholeTextRange : public TCustomRange 
{
	typedef TCustomRange inherited;
	
protected:
	virtual int __fastcall GetREnd(void);
	virtual int __fastcall GetRStart(void);
public:
	#pragma option push -w-inl
	/* TCustomRange.Create */ inline __fastcall virtual TWholeTextRange(Classes::TCollection* Collection) : TCustomRange(Collection) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TWholeTextRange(void) { }
	#pragma option pop
	
};


class DELPHICLASS TCustomFormattedRange;
class PASCALIMPLEMENTATION TCustomFormattedRange : public TMCRange 
{
	typedef TMCRange inherited;
	
protected:
	virtual Graphics::TColor __fastcall GetColor(void) = 0 ;
	virtual Graphics::TFont* __fastcall GetFont(void) = 0 ;
	virtual void __fastcall SetColor(const Graphics::TColor Value);
	virtual void __fastcall SetFont(const Graphics::TFont* Value);
	
public:
	bool FreeWhenDone;
	virtual void __fastcall AssignTo(Classes::TPersistent* Dest);
	
__published:
	__property Graphics::TColor Color = {read=GetColor, write=SetColor, nodefault};
	__property Graphics::TFont* Font = {read=GetFont, write=SetFont};
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TCustomFormattedRange(Classes::TCollection* Collection) : TMCRange(Collection) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TCustomFormattedRange(void) { }
	#pragma option pop
	
};


class DELPHICLASS TFormattedRange;
class PASCALIMPLEMENTATION TFormattedRange : public TCustomFormattedRange 
{
	typedef TCustomFormattedRange inherited;
	
private:
	Graphics::TFont* FFont;
	Graphics::TColor FColor;
	
protected:
	virtual Graphics::TColor __fastcall GetColor(void);
	virtual Graphics::TFont* __fastcall GetFont(void);
	virtual void __fastcall SetColor(const Graphics::TColor Value);
	virtual void __fastcall SetFont(const Graphics::TFont* Value);
	
public:
	__fastcall virtual TFormattedRange(Classes::TCollection* Collection);
	__fastcall virtual ~TFormattedRange(void);
};


class DELPHICLASS TNormalFormattedRange;
class PASCALIMPLEMENTATION TNormalFormattedRange : public TCustomFormattedRange 
{
	typedef TCustomFormattedRange inherited;
	
protected:
	virtual Graphics::TColor __fastcall GetColor(void);
	virtual Graphics::TFont* __fastcall GetFont(void);
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TNormalFormattedRange(Classes::TCollection* Collection) : TCustomFormattedRange(Collection) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TNormalFormattedRange(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
static const Shortint MAX_BREAKPOINTS_NUM = 0x10;
extern PACKAGE void __fastcall Register(void);
extern PACKAGE TTextCell __fastcall TextCell(int CellRow, int CellCol);

}	/* namespace Memocomponentunit */
using namespace Memocomponentunit;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Memocomponentunit
