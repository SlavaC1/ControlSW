// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Sourceeditunit.pas' rev: 11.00

#ifndef SourceeditunitHPP
#define SourceeditunitHPP

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
#include <Stdctrls.hpp>	// Pascal unit
#include <Consts.hpp>	// Pascal unit
#include <Objlist.hpp>	// Pascal unit
#include <Memocomponentunit.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Sourceeditunit
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS ESourceEdit;
class PASCALIMPLEMENTATION ESourceEdit : public Sysutils::Exception 
{
	typedef Sysutils::Exception inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall ESourceEdit(const AnsiString Msg) : Sysutils::Exception(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall ESourceEdit(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall ESourceEdit(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall ESourceEdit(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall ESourceEdit(const AnsiString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall ESourceEdit(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall ESourceEdit(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall ESourceEdit(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~ESourceEdit(void) { }
	#pragma option pop
	
};


typedef char TIgnoreChar;

#pragma option push -b-
enum TParenthesisStyle { psOpening, psClosing };
#pragma option pop

#pragma pack(push,4)
struct TDebuggableRange
{
	
public:
	int RStart;
	int REnd;
	AnsiString Text;
	AnsiString ClassName;
} ;
#pragma pack(pop)

typedef DynamicArray<TDebuggableRange >  TDebuggableRanges;

class DELPHICLASS TSourceEdit;
class DELPHICLASS TSyntaxColoring;
class DELPHICLASS TCustomStyles;
class DELPHICLASS TCustomStyle;
class PASCALIMPLEMENTATION TCustomStyles : public Classes::TCollection 
{
	typedef Classes::TCollection inherited;
	
public:
	TCustomStyle* operator[](int Index) { return Items[Index]; }
	
private:
	TSyntaxColoring* FColoring;
	HIDESBASE TCustomStyle* __fastcall GetItem(int Index);
	HIDESBASE void __fastcall SetItem(int Index, TCustomStyle* Value);
	
protected:
	DYNAMIC Classes::TPersistent* __fastcall GetOwner(void);
	virtual void __fastcall Update(Classes::TCollectionItem* Item);
	
public:
	__fastcall TCustomStyles(TSyntaxColoring* AColoring);
	HIDESBASE TCustomStyle* __fastcall Add(void);
	TCustomStyle* __fastcall FindStyle(const AnsiString S);
	__property TCustomStyle* Items[int Index] = {read=GetItem, write=SetItem/*, default*/};
public:
	#pragma option push -w-inl
	/* TCollection.Destroy */ inline __fastcall virtual ~TCustomStyles(void) { }
	#pragma option pop
	
};


class DELPHICLASS TWordLists;
class DELPHICLASS TWordList;
class PASCALIMPLEMENTATION TWordLists : public Classes::TCollection 
{
	typedef Classes::TCollection inherited;
	
public:
	TWordList* operator[](int Index) { return Items[Index]; }
	
private:
	TSyntaxColoring* FColoring;
	HIDESBASE TWordList* __fastcall GetItem(int Index);
	HIDESBASE void __fastcall SetItem(int Index, TWordList* Value);
	
protected:
	DYNAMIC Classes::TPersistent* __fastcall GetOwner(void);
	virtual void __fastcall Update(Classes::TCollectionItem* Item);
	
public:
	__fastcall TWordLists(TSyntaxColoring* AColoring);
	HIDESBASE TWordList* __fastcall Add(void);
	TWordList* __fastcall FindList(const AnsiString S);
	__property TWordList* Items[int Index] = {read=GetItem, write=SetItem/*, default*/};
public:
	#pragma option push -w-inl
	/* TCollection.Destroy */ inline __fastcall virtual ~TWordLists(void) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TSyntaxColoring : public Objlist::TOwnedPersistent 
{
	typedef Objlist::TOwnedPersistent inherited;
	
private:
	Graphics::TColor FNumberColor;
	Graphics::TColor FSymbolColor;
	TCustomStyles* FCustomStyles;
	TWordLists* FWordLists;
	bool FEnabled;
	int FUpdateDebth;
	Classes::TStringList* FParenthesisColors;
	bool FSymbolCustomStyle;
	bool FNumberCustomStyle;
	Graphics::TFontStyles FNumberStyle;
	Graphics::TFontStyles FSymbolStyle;
	bool FParenthesisCustomStyle;
	Graphics::TFontStyles FParenthesisStyle;
	void __fastcall SetCustomStyles(const TCustomStyles* Value);
	void __fastcall SetNumberColor(const Graphics::TColor Value);
	void __fastcall SetSymbolColor(const Graphics::TColor Value);
	void __fastcall SetWordLists(const TWordLists* Value);
	void __fastcall SetEnabled(const bool Value);
	void __fastcall SetParenthesisColors(const Classes::TStringList* Value);
	void __fastcall SetNumberCustomStyle(const bool Value);
	void __fastcall SetNumberStyle(const Graphics::TFontStyles Value);
	void __fastcall SetSymbolCustomStyle(const bool Value);
	void __fastcall SetSymbolStyle(const Graphics::TFontStyles Value);
	void __fastcall SetParenthesisCustomStyle(const bool Value);
	void __fastcall SetParenthesisStyle(const Graphics::TFontStyles Value);
	
protected:
	Graphics::TFont* SymbolFont;
	Graphics::TFont* NumberFont;
	Graphics::TFont* ParenthesisFont;
	void __fastcall ChangeNotification(System::TObject* Sender);
	
public:
	__fastcall virtual TSyntaxColoring(Classes::TPersistent* AOwner);
	__fastcall virtual ~TSyntaxColoring(void);
	virtual void __fastcall ColoringChange(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	Graphics::TColor __fastcall GetParenthesisColor(int Index);
	virtual void __fastcall BeginUpdate(void);
	virtual void __fastcall EndUpdate(void);
	
__published:
	__property bool Enabled = {read=FEnabled, write=SetEnabled, nodefault};
	__property Graphics::TColor SymbolColor = {read=FSymbolColor, write=SetSymbolColor, nodefault};
	__property Graphics::TFontStyles SymbolStyle = {read=FSymbolStyle, write=SetSymbolStyle, nodefault};
	__property bool SymbolCustomStyle = {read=FSymbolCustomStyle, write=SetSymbolCustomStyle, nodefault};
	__property Graphics::TColor NumberColor = {read=FNumberColor, write=SetNumberColor, nodefault};
	__property Graphics::TFontStyles NumberStyle = {read=FNumberStyle, write=SetNumberStyle, nodefault};
	__property bool NumberCustomStyle = {read=FNumberCustomStyle, write=SetNumberCustomStyle, nodefault};
	__property TWordLists* WordLists = {read=FWordLists, write=SetWordLists};
	__property TCustomStyles* CustomStyles = {read=FCustomStyles, write=SetCustomStyles};
	__property Classes::TStringList* ParenthesisColors = {read=FParenthesisColors, write=SetParenthesisColors};
	__property Graphics::TFontStyles ParenthesisStyle = {read=FParenthesisStyle, write=SetParenthesisStyle, nodefault};
	__property bool ParenthesisCustomStyle = {read=FParenthesisCustomStyle, write=SetParenthesisCustomStyle, nodefault};
};


class DELPHICLASS TSyntaxRange;
typedef TMetaClass* TSyntaxRangeClass;

class PASCALIMPLEMENTATION TSyntaxRange : public Memocomponentunit::TCustomFormattedRange 
{
	typedef Memocomponentunit::TCustomFormattedRange inherited;
	
private:
	int FParenthesisLevel;
	TSyntaxRange* FNextRange;
	TSyntaxRange* FPrevRange;
	
protected:
	virtual void __fastcall SetNewParenthesisLevel(void);
	virtual int __fastcall GetNextParenthesisLevel(void);
	virtual Graphics::TColor __fastcall GetColor(void);
	virtual bool __fastcall ReplaceSyntaxRanges(TSyntaxRange* &StartRange);
	
public:
	__fastcall virtual ~TSyntaxRange(void);
	/*         class method */ static bool __fastcall EqualEndings(TMetaClass* vmt, TSyntaxRange* Range1, TSyntaxRange* Range2);
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
	/*         class method */ static TSyntaxRange* __fastcall InsertRangeBefore(TMetaClass* vmt, TSyntaxRange* Range, TMetaClass* RangeClass, TSourceEdit* Editor);
	TSyntaxRange* __fastcall InsertBefore(TMetaClass* RangeClass);
	TSyntaxRange* __fastcall InsertAfter(TMetaClass* RangeClass);
	/*         class method */ static TSyntaxRange* __fastcall NewRangeInsertedBefore(TMetaClass* vmt, TSyntaxRange* Range, TSourceEdit* Editor);
	/*         class method */ static TSyntaxRange* __fastcall NewRangeInsertedAfter(TMetaClass* vmt, TSyntaxRange* Range, TSourceEdit* Editor);
	virtual void __fastcall UpdateParenthesisLevel(void);
	__property int NextParenthesisLevel = {read=GetNextParenthesisLevel, nodefault};
	
__published:
	__property TSyntaxRange* PrevRange = {read=FPrevRange, write=FPrevRange};
	__property TSyntaxRange* NextRange = {read=FNextRange, write=FNextRange};
	__property int ParenthesisLevel = {read=FParenthesisLevel, write=FParenthesisLevel, nodefault};
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TSyntaxRange(Classes::TCollection* Collection) : Memocomponentunit::TCustomFormattedRange(Collection) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TSourceEdit : public Memocomponentunit::TMemoComponent 
{
	typedef Memocomponentunit::TMemoComponent inherited;
	
private:
	bool FAutoIndent;
	TSyntaxColoring* FSyntaxColoring;
	TSyntaxRange* FFirstSyntaxRange;
	TSyntaxRange* FLastSyntaxRange;
	bool FAutoIndentIncrease;
	char FAutoIndentIncreaseStart;
	char FAutoIndentIncreaseEnd;
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Messages::TMessage &Message);
	void __fastcall SetSyntaxColoring(const TSyntaxColoring* Value);
	
protected:
	TSyntaxRange* SyntaxStartRange;
	TSyntaxRange* LastPRange;
	Memocomponentunit::TCustomRange* LastCRange;
	DYNAMIC void __fastcall KeyPress(char &Key);
	DYNAMIC void __fastcall TextChangeNotification(int StartPos, int OldLength, int NewLength);
	DYNAMIC void __fastcall TextChangeNotificationAfter(void);
	virtual void __fastcall ReColor(void);
	virtual void __fastcall ReColorRange(Memocomponentunit::TCustomRange* Range);
	virtual void __fastcall FreeAllSyntaxRanges(void);
	virtual bool __fastcall ReplaceSyntaxRanges(TSyntaxRange* NewRange, TSyntaxRange* &StartRange);
	virtual Memocomponentunit::TFormattedRangeArray __fastcall CreateSplitRanges(Memocomponentunit::TCustomRange* Range);
	void __fastcall OverwriteRange(System::TObject* Sender);
	virtual TSyntaxRange* __fastcall FindSyntaxHole(void);
	virtual TSyntaxRange* __fastcall FindSyntaxOverlap(void);
	virtual TDebuggableRanges __fastcall MakeDebuggableRanges();
	
public:
	__fastcall virtual TSourceEdit(Classes::TComponent* AOwner);
	__fastcall virtual ~TSourceEdit(void);
	virtual TSyntaxRange* __fastcall SyntaxRangeAtPos(int RangePos);
	virtual TSyntaxRange* __fastcall SyntaxRangeAtPosWithTip(int RangePos, TSyntaxRange* Tip);
	__property TSyntaxRange* FirstSyntaxRange = {read=FFirstSyntaxRange, write=FFirstSyntaxRange};
	__property TSyntaxRange* LastSyntaxRange = {read=FLastSyntaxRange, write=FLastSyntaxRange};
	
__published:
	__property TSyntaxColoring* SyntaxColoring = {read=FSyntaxColoring, write=SetSyntaxColoring};
	__property bool AutoIndent = {read=FAutoIndent, write=FAutoIndent, nodefault};
	__property bool AutoIndentIncrease = {read=FAutoIndentIncrease, write=FAutoIndentIncrease, nodefault};
	__property char AutoIndentIncreaseStart = {read=FAutoIndentIncreaseStart, write=FAutoIndentIncreaseStart, nodefault};
	__property char AutoIndentIncreaseEnd = {read=FAutoIndentIncreaseEnd, write=FAutoIndentIncreaseEnd, nodefault};
	__property OnCaretPosChanged ;
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TSourceEdit(HWND ParentWindow) : Memocomponentunit::TMemoComponent(ParentWindow) { }
	#pragma option pop
	
};


class DELPHICLASS TNormalTextRange;
class PASCALIMPLEMENTATION TNormalTextRange : public TSyntaxRange 
{
	typedef TSyntaxRange inherited;
	
protected:
	virtual Graphics::TFont* __fastcall GetFont(void);
	
public:
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
public:
	#pragma option push -w-inl
	/* TSyntaxRange.Destroy */ inline __fastcall virtual ~TNormalTextRange(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TNormalTextRange(Classes::TCollection* Collection) : TSyntaxRange(Collection) { }
	#pragma option pop
	
};


class DELPHICLASS TCustomStyleRange;
class PASCALIMPLEMENTATION TCustomStyleRange : public TSyntaxRange 
{
	typedef TSyntaxRange inherited;
	
private:
	TCustomStyle* FStyle;
	
protected:
	virtual Graphics::TFont* __fastcall GetFont(void);
	
public:
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
	
__published:
	__property TCustomStyle* Style = {read=FStyle, write=FStyle};
public:
	#pragma option push -w-inl
	/* TSyntaxRange.Destroy */ inline __fastcall virtual ~TCustomStyleRange(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TCustomStyleRange(Classes::TCollection* Collection) : TSyntaxRange(Collection) { }
	#pragma option pop
	
};


class DELPHICLASS TWordListRange;
class PASCALIMPLEMENTATION TWordListRange : public TSyntaxRange 
{
	typedef TSyntaxRange inherited;
	
private:
	TWordList* FWordList;
	
protected:
	virtual Graphics::TFont* __fastcall GetFont(void);
	
public:
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
	
__published:
	__property TWordList* WordList = {read=FWordList, write=FWordList};
public:
	#pragma option push -w-inl
	/* TSyntaxRange.Destroy */ inline __fastcall virtual ~TWordListRange(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TWordListRange(Classes::TCollection* Collection) : TSyntaxRange(Collection) { }
	#pragma option pop
	
};


class DELPHICLASS TSymbolRange;
class PASCALIMPLEMENTATION TSymbolRange : public TSyntaxRange 
{
	typedef TSyntaxRange inherited;
	
private:
	AnsiString FSymbol;
	
protected:
	virtual Graphics::TFont* __fastcall GetFont(void);
	
public:
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
	
__published:
	__property AnsiString Symbol = {read=FSymbol, write=FSymbol};
public:
	#pragma option push -w-inl
	/* TSyntaxRange.Destroy */ inline __fastcall virtual ~TSymbolRange(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TSymbolRange(Classes::TCollection* Collection) : TSyntaxRange(Collection) { }
	#pragma option pop
	
};


class DELPHICLASS TNumberRange;
class PASCALIMPLEMENTATION TNumberRange : public TSyntaxRange 
{
	typedef TSyntaxRange inherited;
	
private:
	AnsiString FNumber;
	
protected:
	virtual Graphics::TFont* __fastcall GetFont(void);
	
public:
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
	
__published:
	__property AnsiString Number = {read=FNumber, write=FNumber};
public:
	#pragma option push -w-inl
	/* TSyntaxRange.Destroy */ inline __fastcall virtual ~TNumberRange(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TNumberRange(Classes::TCollection* Collection) : TSyntaxRange(Collection) { }
	#pragma option pop
	
};


class DELPHICLASS TParenthesisRange;
class PASCALIMPLEMENTATION TParenthesisRange : public TSyntaxRange 
{
	typedef TSyntaxRange inherited;
	
private:
	TParenthesisStyle FStyle;
	
protected:
	virtual void __fastcall SetNewParenthesisLevel(void);
	virtual int __fastcall GetNextParenthesisLevel(void);
	virtual Graphics::TFont* __fastcall GetFont(void);
	
public:
	virtual bool __fastcall EqualEndingsWith(TSyntaxRange* Range);
	virtual void __fastcall UpdateParenthesisLevel(void);
	
__published:
	__property TParenthesisStyle Style = {read=FStyle, write=FStyle, nodefault};
public:
	#pragma option push -w-inl
	/* TSyntaxRange.Destroy */ inline __fastcall virtual ~TParenthesisRange(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TMCRange.Create */ inline __fastcall virtual TParenthesisRange(Classes::TCollection* Collection) : TSyntaxRange(Collection) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TWordList : public Classes::TCollectionItem 
{
	typedef Classes::TCollectionItem inherited;
	
private:
	bool FCustomStyle;
	bool FCustomColor;
	Graphics::TColor FColor;
	Graphics::TFontStyles FStyle;
	Classes::TStringList* FWords;
	AnsiString FDisplayName;
	bool FCaseSensitive;
	void __fastcall SetColor(const Graphics::TColor Value);
	void __fastcall SetCustomColor(const bool Value);
	void __fastcall SetCustomStyle(const bool Value);
	void __fastcall SetStyle(const Graphics::TFontStyles Value);
	void __fastcall SetWords(const Classes::TStringList* Value);
	void __fastcall SetCaseSensitive(const bool Value);
	
protected:
	Graphics::TFont* Font;
	virtual AnsiString __fastcall GetDisplayName();
	virtual void __fastcall SetDisplayName(const AnsiString Value);
	virtual void __fastcall ListChange(void);
	void __fastcall ChangeNotification(System::TObject* Sender);
	
public:
	__fastcall virtual TWordList(Classes::TCollection* Collection);
	__fastcall virtual ~TWordList(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	bool __fastcall WordInList(AnsiString S);
	
__published:
	__property AnsiString Caption = {read=FDisplayName, write=SetDisplayName};
	__property bool CustomColor = {read=FCustomColor, write=SetCustomColor, nodefault};
	__property Graphics::TColor Color = {read=FColor, write=SetColor, nodefault};
	__property bool CustomStyle = {read=FCustomStyle, write=SetCustomStyle, nodefault};
	__property Graphics::TFontStyles Style = {read=FStyle, write=SetStyle, nodefault};
	__property bool CaseSensitive = {read=FCaseSensitive, write=SetCaseSensitive, nodefault};
	__property Classes::TStringList* Words = {read=FWords, write=SetWords};
};


class PASCALIMPLEMENTATION TCustomStyle : public Classes::TCollectionItem 
{
	typedef Classes::TCollectionItem inherited;
	
private:
	bool FCustomStyle;
	bool FCustomColor;
	AnsiString FEndText;
	AnsiString FBeginText;
	Graphics::TColor FColor;
	Graphics::TFontStyles FStyle;
	char FIgnoreChar;
	AnsiString FDisplayName;
	bool FSwitchable;
	TCustomStyle* FDontSwitchTo1;
	TCustomStyle* FDontSwitchTo2;
	void __fastcall SetBeginText(AnsiString Value);
	void __fastcall SetColor(const Graphics::TColor Value);
	void __fastcall SetCustomColor(const bool Value);
	void __fastcall SetCustomStyle(const bool Value);
	void __fastcall SetEndText(AnsiString Value);
	void __fastcall SetIgnoreChar(const char Value);
	void __fastcall SetStyle(const Graphics::TFontStyles Value);
	void __fastcall SetSwitchable(const bool Value);
	void __fastcall SetDontSwitchTo1(const TCustomStyle* Value);
	void __fastcall SetDontSwitchTo2(const TCustomStyle* Value);
	int __fastcall GetDontSwitchToIndex1(void);
	int __fastcall GetDontSwitchToIndex2(void);
	void __fastcall SetDontSwitchToIndex1(const int Value);
	void __fastcall SetDontSwitchToIndex2(const int Value);
	
protected:
	Graphics::TFont* Font;
	virtual AnsiString __fastcall GetDisplayName();
	virtual void __fastcall SetDisplayName(const AnsiString Value);
	virtual void __fastcall StyleChange(void);
	
public:
	__fastcall virtual TCustomStyle(Classes::TCollection* Collection);
	__fastcall virtual ~TCustomStyle(void);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	__property TCustomStyle* DontSwitchTo1 = {read=FDontSwitchTo1, write=SetDontSwitchTo1};
	__property TCustomStyle* DontSwitchTo2 = {read=FDontSwitchTo2, write=SetDontSwitchTo2};
	
__published:
	__property AnsiString Caption = {read=FDisplayName, write=SetDisplayName};
	__property bool CustomColor = {read=FCustomColor, write=SetCustomColor, nodefault};
	__property Graphics::TColor Color = {read=FColor, write=SetColor, nodefault};
	__property bool CustomStyle = {read=FCustomStyle, write=SetCustomStyle, nodefault};
	__property Graphics::TFontStyles Style = {read=FStyle, write=SetStyle, nodefault};
	__property AnsiString BeginText = {read=FBeginText, write=SetBeginText};
	__property AnsiString EndText = {read=FEndText, write=SetEndText};
	__property char IgnoreChar = {read=FIgnoreChar, write=SetIgnoreChar, nodefault};
	__property bool Switchable = {read=FSwitchable, write=SetSwitchable, nodefault};
	__property int DontSwitchToIndex1 = {read=GetDontSwitchToIndex1, write=SetDontSwitchToIndex1, nodefault};
	__property int DontSwitchToIndex2 = {read=GetDontSwitchToIndex2, write=SetDontSwitchToIndex2, nodefault};
};


class DELPHICLASS TSyntaxColoringCopy;
class PASCALIMPLEMENTATION TSyntaxColoringCopy : public Classes::TComponent 
{
	typedef Classes::TComponent inherited;
	
private:
	Graphics::TColor FNumberColor;
	Graphics::TColor FSymbolColor;
	TCustomStyles* FCustomStyles;
	TWordLists* FWordLists;
	bool FEnabled;
	Classes::TStringList* FParenthesisColors;
	bool FSymbolCustomStyle;
	bool FNumberCustomStyle;
	Graphics::TFontStyles FNumberStyle;
	Graphics::TFontStyles FSymbolStyle;
	bool FParenthesisCustomStyle;
	Graphics::TFontStyles FParenthesisStyle;
	void __fastcall SetCustomStyles(const TCustomStyles* Value);
	void __fastcall SetParenthesisColors(const Classes::TStringList* Value);
	void __fastcall SetWordLists(const TWordLists* Value);
	
public:
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	
__published:
	__fastcall virtual TSyntaxColoringCopy(Classes::TComponent* AOwner);
	__fastcall virtual ~TSyntaxColoringCopy(void);
	__property bool Enabled = {read=FEnabled, write=FEnabled, nodefault};
	__property Graphics::TColor SymbolColor = {read=FSymbolColor, write=FSymbolColor, nodefault};
	__property Graphics::TFontStyles SymbolStyle = {read=FSymbolStyle, write=FSymbolStyle, nodefault};
	__property bool SymbolCustomStyle = {read=FSymbolCustomStyle, write=FSymbolCustomStyle, nodefault};
	__property Graphics::TColor NumberColor = {read=FNumberColor, write=FNumberColor, nodefault};
	__property Graphics::TFontStyles NumberStyle = {read=FNumberStyle, write=FNumberStyle, nodefault};
	__property bool NumberCustomStyle = {read=FNumberCustomStyle, write=FNumberCustomStyle, nodefault};
	__property TWordLists* WordLists = {read=FWordLists, write=SetWordLists};
	__property TCustomStyles* CustomStyles = {read=FCustomStyles, write=SetCustomStyles};
	__property Classes::TStringList* ParenthesisColors = {read=FParenthesisColors, write=SetParenthesisColors};
	__property Graphics::TFontStyles ParenthesisStyle = {read=FParenthesisStyle, write=FParenthesisStyle, nodefault};
	__property bool ParenthesisCustomStyle = {read=FParenthesisCustomStyle, write=FParenthesisCustomStyle, nodefault};
};


#pragma option push -b-
enum TSectionType { stText, stSymbol, stParenthesis, stCustomStyle };
#pragma option pop

//-- var, const, procedure ---------------------------------------------------
extern PACKAGE Set<char, 0, 255>  SymbolChars;
static const Shortint MaxBeginEndTextLength = 0x14;
extern PACKAGE bool __fastcall CharIsWordable(char Ch);
extern PACKAGE bool __fastcall CharIsIdentifier(char Ch);
extern PACKAGE bool __fastcall CharIsExtNumber(char Ch);
extern PACKAGE bool __fastcall CharIsNumber(char Ch);
extern PACKAGE bool __fastcall CharIsSymbol(char Ch);
extern PACKAGE bool __fastcall CharIsParenthesis(char Ch);
extern PACKAGE void __fastcall Register(void);

}	/* namespace Sourceeditunit */
using namespace Sourceeditunit;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Sourceeditunit
