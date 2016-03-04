
{*******************************************************}
{                                                       }
{       TSourceEdit Component v2.0                      }
{                                                       }
{       Now based on TMemoComponent instead of          }
{        TRichEdit                                      }
{                                                       }
{       Copyright (c) 2000 Sebastian Reichelt           }
{                                                       }
{*******************************************************}

unit SourceEditUnit;

interface

uses
	Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
	StdCtrls, Consts, ObjList, MemoComponentUnit;

const
	SymbolChars: set of Char = ['<', '{', '[', '(', ')', ']', '}', '>', ';', ':', ',', '.', '=', '+', '-', '*', '/', '\', '|', '"', '''', '!', '?', '&', '%', '#', '@', '^', '~'];
	MaxBeginEndTextLength = 20;

type
	ESourceEdit = class(Exception);

	TSyntaxColoring = class;
	TWordLists = class;
	TWordList = class;
	TCustomStyles = class;
	TCustomStyle = class;

	TSyntaxRange = class;

	TIgnoreChar = char; //string [1];
	TParenthesisStyle = (psOpening, psClosing);

	TDebuggableRange = record
		RStart,
		REnd: Integer;
		Text,
		ClassName: string;
	end;
	TDebuggableRanges = array of TDebuggableRange;

	TSourceEdit = class(TMemoComponent)
	private
		FAutoIndent: Boolean;
		FSyntaxColoring: TSyntaxColoring;
		FFirstSyntaxRange: TSyntaxRange;
		FLastSyntaxRange: TSyntaxRange;
    FAutoIndentIncrease: Boolean;
    FAutoIndentIncreaseStart: Char;
    FAutoIndentIncreaseEnd: Char;
		procedure CMFontChanged(var Message: TMessage); message cm_FontChanged;
		procedure SetSyntaxColoring(const Value: TSyntaxColoring);
	protected
		SyntaxStartRange,
		LastPRange: TSyntaxRange;
		LastCRange: TCustomRange;
		procedure KeyPress(var Key: Char); override;
		procedure TextChangeNotification(StartPos, OldLength, NewLength: Integer); override;
		procedure TextChangeNotificationAfter; override;
		procedure ReColor; virtual;
		procedure ReColorRange(Range: TCustomRange); virtual;
		procedure FreeAllSyntaxRanges; virtual;
		function ReplaceSyntaxRanges(NewRange: TSyntaxRange; var StartRange: TSyntaxRange): Boolean; virtual;
		function CreateSplitRanges(Range: TCustomRange): TFormattedRangeArray; override;
		procedure OverwriteRange(Sender: TObject);
		function FindSyntaxHole: TSyntaxRange; virtual;
		function FindSyntaxOverlap: TSyntaxRange; virtual;
		function MakeDebuggableRanges: TDebuggableRanges; virtual;
	public
		constructor Create(AOwner: TComponent); override;
		destructor Destroy; override;
		function SyntaxRangeAtPos(RangePos: Integer): TSyntaxRange; virtual;
		function SyntaxRangeAtPosWithTip(RangePos: Integer; Tip: TSyntaxRange): TSyntaxRange; virtual;
		property FirstSyntaxRange: TSyntaxRange read FFirstSyntaxRange write FFirstSyntaxRange;
		property LastSyntaxRange: TSyntaxRange read FLastSyntaxRange write FLastSyntaxRange;
	published
		property SyntaxColoring: TSyntaxColoring read FSyntaxColoring write SetSyntaxColoring;
		property AutoIndent: Boolean read FAutoIndent write FAutoIndent;
		property AutoIndentIncrease: Boolean read FAutoIndentIncrease write FAutoIndentIncrease;
		property AutoIndentIncreaseStart: Char read FAutoIndentIncreaseStart write FAutoIndentIncreaseStart;
		property AutoIndentIncreaseEnd: Char read FAutoIndentIncreaseEnd write FAutoIndentIncreaseEnd;
    property OnCaretPosChanged;
	end;

	TSyntaxRangeClass = class of TSyntaxRange;

	TSyntaxRange = class(TCustomFormattedRange)
	private
		FParenthesisLevel: Integer;
		FNextRange: TSyntaxRange;
		FPrevRange: TSyntaxRange;
	protected
		procedure SetNewParenthesisLevel; virtual;
		function GetNextParenthesisLevel: Integer; virtual;
		function GetColor: TColor; override;
		function ReplaceSyntaxRanges(var StartRange: TSyntaxRange): Boolean; virtual;
	public
		destructor Destroy; override;
		class function EqualEndings(Range1, Range2: TSyntaxRange): Boolean;
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; virtual;
		class function InsertRangeBefore(Range: TSyntaxRange; RangeClass: TSyntaxRangeClass; Editor: TSourceEdit): TSyntaxRange;
		function InsertBefore(RangeClass: TSyntaxRangeClass): TSyntaxRange;
		function InsertAfter(RangeClass: TSyntaxRangeClass): TSyntaxRange;
		class function NewRangeInsertedBefore(Range: TSyntaxRange; Editor: TSourceEdit): TSyntaxRange;
		class function NewRangeInsertedAfter(Range: TSyntaxRange; Editor: TSourceEdit): TSyntaxRange;
		procedure UpdateParenthesisLevel; virtual;
		property NextParenthesisLevel: Integer read GetNextParenthesisLevel;
	published
		property PrevRange: TSyntaxRange read FPrevRange write FPrevRange;
		property NextRange: TSyntaxRange read FNextRange write FNextRange;
		property ParenthesisLevel: Integer read FParenthesisLevel write FParenthesisLevel;
	end;

	TNormalTextRange = class(TSyntaxRange)
	protected
		function GetFont: TFont; override;
	public
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; override;
	end;

	TCustomStyleRange = class(TSyntaxRange)
	private
		FStyle: TCustomStyle;
	protected
		function GetFont: TFont; override;
	public
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; override;
	published
		property Style: TCustomStyle read FStyle write FStyle;
	end;

	TWordListRange = class(TSyntaxRange)
	private
		FWordList: TWordList;
	protected
		function GetFont: TFont; override;
	public
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; override;
	published
		property WordList: TWordList read FWordList write FWordList;
	end;

	TSymbolRange = class(TSyntaxRange)
	private
		FSymbol: string;
	protected
		function GetFont: TFont; override;
	public
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; override;
	published
		property Symbol: string read FSymbol write FSymbol;
	end;

	TNumberRange = class(TSyntaxRange)
	private
		FNumber: string;
	protected
		function GetFont: TFont; override;
	public
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; override;
	published
		property Number: string read FNumber write FNumber;
	end;

	TParenthesisRange = class(TSyntaxRange)
	private
		FStyle: TParenthesisStyle;
	protected
		procedure SetNewParenthesisLevel; override;
		function GetNextParenthesisLevel: Integer; override;
		function GetFont: TFont; override;
	public
		function EqualEndingsWith(Range: TSyntaxRange): Boolean; override;
		procedure UpdateParenthesisLevel; override;
	published
		property Style: TParenthesisStyle read FStyle write FStyle;
	end;

	TSyntaxColoring = class(TOwnedPersistent)
	private
		FNumberColor: TColor;
		FSymbolColor: TColor;
		FCustomStyles: TCustomStyles;
		FWordLists: TWordLists;
		FEnabled: Boolean;
		FUpdateDebth: Integer;
		FParenthesisColors: TStringList;
		FSymbolCustomStyle: Boolean;
		FNumberCustomStyle: Boolean;
		FNumberStyle: TFontStyles;
		FSymbolStyle: TFontStyles;
		FParenthesisCustomStyle: Boolean;
		FParenthesisStyle: TFontStyles;
		procedure SetCustomStyles(const Value: TCustomStyles);
		procedure SetNumberColor(const Value: TColor);
		procedure SetSymbolColor(const Value: TColor);
		procedure SetWordLists(const Value: TWordLists);
		procedure SetEnabled(const Value: Boolean);
		procedure SetParenthesisColors(const Value: TStringList);
		procedure SetNumberCustomStyle(const Value: Boolean);
		procedure SetNumberStyle(const Value: TFontStyles);
		procedure SetSymbolCustomStyle(const Value: Boolean);
		procedure SetSymbolStyle(const Value: TFontStyles);
		procedure SetParenthesisCustomStyle(const Value: Boolean);
		procedure SetParenthesisStyle(const Value: TFontStyles);
	protected
		SymbolFont,
		NumberFont,
		ParenthesisFont: TFont;
		procedure ChangeNotification(Sender: TObject);
	public
		constructor Create(AOwner: TPersistent); override;
		destructor Destroy; override;
		procedure ColoringChange; virtual;
		procedure Assign(Source: TPersistent); override;
		function GetParenthesisColor(Index: Integer): TColor;
		procedure BeginUpdate; virtual;
		procedure EndUpdate; virtual;
	published
		property Enabled: Boolean read FEnabled write SetEnabled;
		property SymbolColor: TColor read FSymbolColor write SetSymbolColor;
		property SymbolStyle: TFontStyles read FSymbolStyle write SetSymbolStyle;
		property SymbolCustomStyle: Boolean read FSymbolCustomStyle write SetSymbolCustomStyle;
		property NumberColor: TColor read FNumberColor write SetNumberColor;
		property NumberStyle: TFontStyles read FNumberStyle write SetNumberStyle;
		property NumberCustomStyle: Boolean read FNumberCustomStyle write SetNumberCustomStyle;
		property WordLists: TWordLists read FWordLists write SetWordLists;
		property CustomStyles: TCustomStyles read FCustomStyles write SetCustomStyles;
		property ParenthesisColors: TStringList read FParenthesisColors write SetParenthesisColors;
		property ParenthesisStyle: TFontStyles read FParenthesisStyle write SetParenthesisStyle;
		property ParenthesisCustomStyle: Boolean read FParenthesisCustomStyle write SetParenthesisCustomStyle;
	end;

	TWordLists = class(TCollection)
	private
		FColoring: TSyntaxColoring;
		function GetItem(Index: Integer): TWordList;
		procedure SetItem(Index: Integer; Value: TWordList);
	protected
		function GetOwner: TPersistent; override;
		procedure Update(Item: TCollectionItem); override;
	public
		constructor Create(AColoring: TSyntaxColoring);
		function Add: TWordList;
		function FindList(const S: string): TWordList;
		property Items[Index: Integer]: TWordList read GetItem write SetItem; default;
	end;

	TWordList = class(TCollectionItem)
	private
		FCustomStyle: Boolean;
		FCustomColor: Boolean;
		FColor: TColor;
		FStyle: TFontStyles;
		FWords: TStringList;
		FDisplayName: string;
		FCaseSensitive: Boolean;
		procedure SetColor(const Value: TColor);
		procedure SetCustomColor(const Value: Boolean);
		procedure SetCustomStyle(const Value: Boolean);
		procedure SetStyle(const Value: TFontStyles);
		procedure SetWords(const Value: TStringList);
		procedure SetCaseSensitive(const Value: Boolean);
	protected
		Font: TFont;
		function GetDisplayName: string; override;
		procedure SetDisplayName(const Value: string); override;
		procedure ListChange; virtual;
		procedure ChangeNotification(Sender: TObject);
	public
		constructor Create(Collection: TCollection); override;
		destructor Destroy; override;
		procedure Assign(Source: TPersistent); override;
		function WordInList(S: string): Boolean;
	published
		property Caption: string read FDisplayName write SetDisplayName;
		property CustomColor: Boolean read FCustomColor write SetCustomColor;
		property Color: TColor read FColor write SetColor;
		property CustomStyle: Boolean read FCustomStyle write SetCustomStyle;
		property Style: TFontStyles read FStyle write SetStyle;
		property CaseSensitive: Boolean read FCaseSensitive write SetCaseSensitive;
		property Words: TStringList read FWords write SetWords;
	end;

	TCustomStyles = class(TCollection)
	private
		FColoring: TSyntaxColoring;
		function GetItem(Index: Integer): TCustomStyle;
		procedure SetItem(Index: Integer; Value: TCustomStyle);
	protected
		function GetOwner: TPersistent; override;
		procedure Update(Item: TCollectionItem); override;
	public
		constructor Create(AColoring: TSyntaxColoring);
		function Add: TCustomStyle;
		function FindStyle(const S: string): TCustomStyle;
		property Items[Index: Integer]: TCustomStyle read GetItem write SetItem; default;
	end;

	TCustomStyle = class(TCollectionItem)
	private
		FCustomStyle: Boolean;
		FCustomColor: Boolean;
		FEndText: string;
		FBeginText: string;
		FColor: TColor;
		FStyle: TFontStyles;
		FIgnoreChar: TIgnoreChar;
		FDisplayName: string;
		FSwitchable: Boolean;
		FDontSwitchTo1: TCustomStyle;
		FDontSwitchTo2: TCustomStyle;
		procedure SetBeginText(Value: string);
		procedure SetColor(const Value: TColor);
		procedure SetCustomColor(const Value: Boolean);
		procedure SetCustomStyle(const Value: Boolean);
		procedure SetEndText(Value: string);
		procedure SetIgnoreChar(const Value: TIgnoreChar);
		procedure SetStyle(const Value: TFontStyles);
		procedure SetSwitchable(const Value: Boolean);
		procedure SetDontSwitchTo1(const Value: TCustomStyle);
		procedure SetDontSwitchTo2(const Value: TCustomStyle);
		function GetDontSwitchToIndex1: Integer;
		function GetDontSwitchToIndex2: Integer;
		procedure SetDontSwitchToIndex1(const Value: Integer);
		procedure SetDontSwitchToIndex2(const Value: Integer);
	protected
		Font: TFont;
		function GetDisplayName: string; override;
		procedure SetDisplayName(const Value: string); override;
		procedure StyleChange; virtual;
	public
		constructor Create(Collection: TCollection); override;
		destructor Destroy; override;
		procedure Assign(Source: TPersistent); override;
		property DontSwitchTo1: TCustomStyle read FDontSwitchTo1 write SetDontSwitchTo1;
		property DontSwitchTo2: TCustomStyle read FDontSwitchTo2 write SetDontSwitchTo2;
	published
		property Caption: string read FDisplayName write SetDisplayName;
		property CustomColor: Boolean read FCustomColor write SetCustomColor;
		property Color: TColor read FColor write SetColor;
		property CustomStyle: Boolean read FCustomStyle write SetCustomStyle;
		property Style: TFontStyles read FStyle write SetStyle;
		property BeginText: string read FBeginText write SetBeginText;
		property EndText: string read FEndText write SetEndText;
		property IgnoreChar: TIgnoreChar read FIgnoreChar write SetIgnoreChar;
		property Switchable: Boolean read FSwitchable write SetSwitchable;
		property DontSwitchToIndex1: Integer read GetDontSwitchToIndex1 write SetDontSwitchToIndex1;
		property DontSwitchToIndex2: Integer read GetDontSwitchToIndex2 write SetDontSwitchToIndex2;
	end;

	TSyntaxColoringCopy = class(TComponent)
	private
		FNumberColor: TColor;
		FSymbolColor: TColor;
		FCustomStyles: TCustomStyles;
		FWordLists: TWordLists;
		FEnabled: Boolean;
		FParenthesisColors: TStringList;
		FSymbolCustomStyle: Boolean;
		FNumberCustomStyle: Boolean;
		FNumberStyle: TFontStyles;
		FSymbolStyle: TFontStyles;
		FParenthesisCustomStyle: Boolean;
		FParenthesisStyle: TFontStyles;
		procedure SetCustomStyles(const Value: TCustomStyles);
		procedure SetParenthesisColors(const Value: TStringList);
		procedure SetWordLists(const Value: TWordLists);
	protected
	public
		procedure Assign(Source: TPersistent); override;
	published
		constructor Create(AOwner: TComponent); override;
		destructor Destroy; override;
		property Enabled: Boolean read FEnabled write FEnabled;
		property SymbolColor: TColor read FSymbolColor write FSymbolColor;
		property SymbolStyle: TFontStyles read FSymbolStyle write FSymbolStyle;
		property SymbolCustomStyle: Boolean read FSymbolCustomStyle write FSymbolCustomStyle;
		property NumberColor: TColor read FNumberColor write FNumberColor;
		property NumberStyle: TFontStyles read FNumberStyle write FNumberStyle;
		property NumberCustomStyle: Boolean read FNumberCustomStyle write FNumberCustomStyle;
		property WordLists: TWordLists read FWordLists write SetWordLists;
		property CustomStyles: TCustomStyles read FCustomStyles write SetCustomStyles;
		property ParenthesisColors: TStringList read FParenthesisColors write SetParenthesisColors;
		property ParenthesisStyle: TFontStyles read FParenthesisStyle write FParenthesisStyle;
		property ParenthesisCustomStyle: Boolean read FParenthesisCustomStyle write FParenthesisCustomStyle;
	end;

	TSectionType = (stText, stSymbol, stParenthesis, stCustomStyle);

function CharIsWordable(Ch: Char): Boolean;
function CharIsIdentifier(Ch: Char): Boolean;
function CharIsExtNumber(Ch: Char): Boolean;
function CharIsNumber(Ch: Char): Boolean;
function CharIsSymbol(Ch: Char): Boolean;
function CharIsParenthesis(Ch: Char): Boolean;

procedure Register;

implementation

//uses
//	UtilsDos;

function CharIsWordable(Ch: Char): Boolean;
begin
	Result := CharIsIdentifier (Ch) or (Ch in ['$', '#']);
end;

function CharIsIdentifier(Ch: Char): Boolean;
begin
	Result := Ch in ['A'..'Z', 'a'..'z', '0'..'9', '_'];
end;

function CharIsExtNumber(Ch: Char): Boolean;
begin
	Result := CharIsNumber (Ch) or (Ch in ['$', '#']);
end;

function CharIsNumber(Ch: Char): Boolean;
begin
	Result := Ch in ['0'..'9'];
end;

function CharIsSymbol(Ch: Char): Boolean;
begin
	Result := Ch in SymbolChars;
end;

function CharIsParenthesis(Ch: Char): Boolean;
begin
	Result := Ch in ['(', ')'];
end;

procedure Register;
begin
	RegisterComponents ('Objet', [TSourceEdit]);
end;

{ TSourceEdit }

procedure TSourceEdit.CMFontChanged(var Message: TMessage);
begin
	inherited;
	if Assigned (SyntaxColoring) then
		SyntaxColoring.ColoringChange;
end;

constructor TSourceEdit.Create(AOwner: TComponent);
begin
	inherited;
	FSyntaxColoring := TSyntaxColoring.Create (Self);
	FAutoIndent := True;
	FAutoIndentIncrease := False;
	FAutoIndentIncreaseStart := '{';
	FAutoIndentIncreaseEnd := '}';
end;

function TSourceEdit.CreateSplitRanges(Range: TCustomRange): TFormattedRangeArray;
var
	RS,
	RE: Integer;
begin
	if SyntaxColoring.Enabled then begin
		SyntaxStartRange := SyntaxRangeAtPosWithTip (Range.RStart, SyntaxStartRange);
		RE := Range.RStart - 1;
		while Assigned (SyntaxStartRange) and (SyntaxStartRange.RStart <= Range.REnd) do begin
			RS := SyntaxStartRange.RStart;
			if RS < RE + 1 then
				RS := RE + 1;
			if (not Selection.Hidden) and (Selection.RLength > 0) then begin
				RE := Selection.RStart - 1;
				if RE > Range.REnd then
					RE := Range.REnd;
				if RE > SyntaxStartRange.REnd then
					RE := SyntaxStartRange.REnd;
				if RE >= RS then begin
					SetLength (Result, Length (Result) + 1);
					if (RS <> SyntaxStartRange.RStart) or (RE <> SyntaxStartRange.REnd) then begin
						Result [High (Result)] := TFormattedRange.Create (nil);
						with TCustomFormattedRange(Result [High (Result)]) do begin
							FreeWhenDone := True;
							Editor := Self;
							RStart := RS;
							REnd := RE;
							Font := SyntaxStartRange.Font;
							Color := Self.Color;
						end;
					end else
						Result [High (Result)] := SyntaxStartRange;
				end;
				RS := Selection.RStart;
				if RS < Range.RStart then
					RS := Range.RStart;
				if RS < SyntaxStartRange.RStart then
					RS := SyntaxStartRange.RStart;
				RE := Selection.REnd;
				if RE > Range.REnd then
					RE := Range.REnd;
				if RE > SyntaxStartRange.REnd then
					RE := SyntaxStartRange.REnd;
				if RE >= RS then begin
					SetLength (Result, Length (Result) + 1);
					Result [High (Result)] := TFormattedRange.Create (nil);
					with TCustomFormattedRange(Result [High (Result)]) do begin
						FreeWhenDone := True;
						Editor := Self;
						RStart := RS;
						REnd := RE;
						Font := SyntaxStartRange.Font;
						Font.Color := clHighlightText;
						Color := clHighlight;
					end;
				end;
				RS := Selection.REnd + 1;
				if RS < Range.RStart then
					RS := Range.RStart;
				if RS < SyntaxStartRange.RStart then
					RS := SyntaxStartRange.RStart;
			end;
			RE := SyntaxStartRange.REnd;
			if RE > Range.REnd then
				RE := Range.REnd;
			if RE >= RS then begin
				SetLength (Result, Length (Result) + 1);
				if (RS <> SyntaxStartRange.RStart) or (RE <> SyntaxStartRange.REnd) then begin
					Result [High (Result)] := TFormattedRange.Create (nil);
					with TCustomFormattedRange(Result [High (Result)]) do begin
						FreeWhenDone := True;
						Editor := Self;
						RStart := RS;
						REnd := RE;
						Font := SyntaxStartRange.Font;
						Color := Self.Color;
					end;
				end else
					Result [High (Result)] := SyntaxStartRange;
			end;
			SyntaxStartRange := SyntaxStartRange.NextRange;
		end;
	end else
		Result := inherited CreateSplitRanges (Range);
end;

destructor TSourceEdit.Destroy;
begin
	if Assigned (FSyntaxColoring) then begin
		FSyntaxColoring.FUpdateDebth := 100;
		FSyntaxColoring.Free;
	end;
	inherited;
end;

function TSourceEdit.FindSyntaxHole: TSyntaxRange;
begin
	Result := FirstSyntaxRange;
	if Assigned (Result) and (Result.RStart = 1) then begin
		if LastSyntaxRange.REnd <> TextLength then
			Result := LastSyntaxRange
		else
			while Assigned (Result) and ((not Assigned (Result.NextRange)) or (Result.REnd + 1 = Result.NextRange.RStart)) do
				Result := Result.NextRange;
	end;
end;

function TSourceEdit.FindSyntaxOverlap: TSyntaxRange;
begin
	Result := FirstSyntaxRange;
	while Assigned (Result) and ((not Assigned (Result.NextRange)) or (Result.REnd < Result.NextRange.RStart)) do
		Result := Result.NextRange;
end;

procedure TSourceEdit.FreeAllSyntaxRanges;
begin
	while Assigned (LastSyntaxRange) do
		LastSyntaxRange.Free;
end;

function FirstNonWhiteSpace (const S: string): Integer;
var
  I: Integer;
begin
  for I := 1 to Length (S) do begin
    if not (S [I] in [' ', #9]) then begin
      Result := I;
      Exit;
    end;
  end;
  Result := Length (S) + 1;
end;

procedure TSourceEdit.KeyPress(var Key: Char);
var
	InsS: string;
	Rs,
	BeginLn,
	FirstChr: Integer;
begin
	if (Key = #13) and AutoIndent then begin
		BeginLn := CellToCharIdx (TextCell (Selection.StartRowCol.Row, 1));
		FirstChr := FirstNonWhiteSpace (Copy (Text, BeginLn, Selection.RStart - BeginLn));
		with Selection do begin
			Text := #13#10 + Copy (Self.Text, BeginLn, FirstChr - 1);
			ScrollInView (4);
		end;
		Key := #0;
	end;
	if (Key = AutoIndentIncreaseStart) and AutoIndentIncrease then begin
		BeginLn := CellToCharIdx (TextCell (Selection.StartRowCol.Row, 1));
		FirstChr := FirstNonWhiteSpace (Copy (Text, BeginLn, Selection.RStart - BeginLn));
		if (BeginLn + FirstChr - 1 > TextLength) or (Text [BeginLn + FirstChr - 1] = #13) then begin
			InsS := Key + #13#10 + Copy (Self.Text, BeginLn, FirstChr - 1) + #9;
			with Selection do begin
				RS := RStart;
				Text := InsS + #13#10 + Copy (Self.Text, BeginLn, FirstChr - 1) + AutoIndentIncreaseEnd;
				NoSelAtPos (RS + Length (InsS));
				ScrollInView (4);
			end;
			Key := #0;
		end;
	end;
	inherited;
end;

function TSourceEdit.MakeDebuggableRanges: TDebuggableRanges;
var
	Range: TSyntaxRange;
begin
	Range := FirstSyntaxRange;
	while Assigned (Range) do begin
		SetLength (Result, Length (Result) + 1);
		with Result [High (Result)] do begin
			RStart := Range.RStart;
			REnd := Range.REnd;
			Text := Range.Text;
			ClassName := Range.ClassName;
		end;
		Range := Range.NextRange;
	end;
end;

procedure TSourceEdit.OverwriteRange(Sender: TObject);
begin
	Sender.Free;
end;

procedure TSourceEdit.ReColor;
begin
	if TextLength > 0 then begin
		Selection.HideCaret;
		FreeAllSyntaxRanges;
		ReColorRange (WholeText);
		while Assigned (LastPRange) do begin
			LastPRange.SetNewParenthesisLevel;
			LastPRange := LastPRange.NextRange;
		end;
		VisibleRange.DrawRange;
		Selection.UpdateCaretPos;
		Selection.ShowCaret;
	end;
end;

procedure TSourceEdit.ReColorRange(Range: TCustomRange);
var
	LastRange: TSyntaxRange;
	KeepRunning,
	EndingsEqual: Boolean;
	SectionStart,
	SectionLength,
	CurPos: Integer;
	CustomStyle,
	NewCustomStyle: TCustomStyle;
	CurChar: Char;
	SectionType: TSectionType;
	L: Integer;
procedure ProcessSection;
var
	I,
	LastStart: Integer;
	CurWord: string;
	WordStyle: TWordList;
begin
	if SectionLength > 0 then begin
		case SectionType of
			stSymbol:
				with TSymbolRange (TSymbolRange.NewRangeInsertedBefore (LastRange, Self)) do begin
					RStart := SectionStart;
					RLength := SectionLength;
					Symbol := Text;
					EndingsEqual := ReplaceSyntaxRanges (LastRange);
				end;
			stParenthesis: begin
				with TParenthesisRange (TParenthesisRange.NewRangeInsertedBefore (LastRange, Self)) do begin
					RStart := SectionStart;
					RLength := SectionLength;
					if Self.Text [SectionStart] in [')', '}', ']'] then
						Style := psClosing
					else
						Style := psOpening;
					EndingsEqual := ReplaceSyntaxRanges (LastRange);
				end;
			end;
			stCustomStyle:
				with TCustomStyleRange (TCustomStyleRange.NewRangeInsertedBefore (LastRange, Self)) do begin
					RStart := SectionStart;
					RLength := SectionLength;
					Style := CustomStyle;
					EndingsEqual := ReplaceSyntaxRanges (LastRange);
				end;
		end;
		if SectionType = stText then begin
			CurWord := '';
			LastStart := SectionStart;
			for I := SectionStart to SectionStart + SectionLength do begin
				if (I < SectionStart + SectionLength) and CharIsWordable (Text [I]) then
					CurWord := CurWord + Text [I]
				else begin
					if CurWord <> '' then begin
						if CharIsExtNumber (CurWord [1]) then begin
							if I - Length (CurWord) > LastStart then
								with TNormalTextRange (TNormalTextRange.NewRangeInsertedBefore (LastRange, Self)) do begin
									RStart := LastStart;
									REnd := I - Length (CurWord) - 1;
									if RLength > 0 then
										EndingsEqual := ReplaceSyntaxRanges (LastRange)
									else
										Free;
								end;
							with TNumberRange (TNumberRange.NewRangeInsertedBefore (LastRange, Self)) do begin
								RStart := I - Length (CurWord);
								REnd := I - 1;
								Number := Text;
								EndingsEqual := ReplaceSyntaxRanges (LastRange);
								LastStart := I;
							end;
						end else begin
							WordStyle := SyntaxColoring.WordLists.FindList (CurWord);
							if Assigned (WordStyle) then begin
								if I - Length (CurWord) > LastStart then
									with TNormalTextRange (TNormalTextRange.NewRangeInsertedBefore (LastRange, Self)) do begin
										RStart := LastStart;
										REnd := I - Length (CurWord) - 1;
										if RLength > 0 then
											EndingsEqual := ReplaceSyntaxRanges (LastRange)
										else
											Free;
									end;
								with TWordListRange (TWordListRange.NewRangeInsertedBefore (LastRange, Self)) do begin
									RStart := I - Length (CurWord);
									REnd := I - 1;
									WordList := WordStyle;
									EndingsEqual := ReplaceSyntaxRanges (LastRange);
									LastStart := I;
								end;
							end;
						end;
						CurWord := '';
					end;
					if (I >= SectionStart + SectionLength) and (I > LastStart) then
						with TNormalTextRange (TNormalTextRange.NewRangeInsertedBefore (LastRange, Self)) do begin
							RStart := LastStart;
							REnd := I - 1;
							LastStart := I;
							if RLength > 0 then
								EndingsEqual := ReplaceSyntaxRanges (LastRange)
							else
								Free;
						end;
				end;
			end;
		end;
	end;
end;
procedure SetSectionType(NewType: TSectionType);
begin
	if (NewType <> SectionType) or (NewType = stParenthesis) or (NewType = stCustomStyle) then begin
		ProcessSection;
		SectionStart := CurPos;
		SectionLength := 0;
		SectionType := NewType;
	end;
end;
begin
	if SyntaxColoring.FUpdateDebth = 0 then begin
		if SyntaxColoring.Enabled and (TextLength > 0) then begin
			Inc (SyntaxColoring.FUpdateDebth);
			LastRange := SyntaxRangeAtPos (Range.RStart - 1);
			if Assigned (LastRange) then begin
				if Range.RStart > LastRange.RStart then
					Range.RStart := LastRange.RStart;
			end else
				Range.RStart := 1;
			CurPos := Range.RStart;
			SectionStart := CurPos;
			SectionLength := 0;
			SectionType := stText;
			CustomStyle := nil;
			EndingsEqual := False;
			KeepRunning := False;
			while (CurPos <= TextLength) and ((CurPos <= Range.REnd + 2) or (not EndingsEqual) or KeepRunning) do begin
				EndingsEqual := False;
				KeepRunning := False;
				CurChar := Text [CurPos];
				NewCustomStyle := SyntaxColoring.CustomStyles.FindStyle (Copy (Text, CurPos, MaxBeginEndTextLength));
				if (SectionType = stCustomStyle) and Assigned (CustomStyle) and (((NewCustomStyle = CustomStyle) and CustomStyle.Switchable) or (NewCustomStyle = CustomStyle.DontSwitchTo1) or (NewCustomStyle = CustomStyle.DontSwitchTo2)) then
					NewCustomStyle := nil;
				if (SectionType = stCustomStyle) and Assigned (CustomStyle) and CustomStyle.Switchable and (not Assigned (NewCustomStyle)) and ((Copy (Text, CurPos, Length (CustomStyle.EndText)) <> CustomStyle.EndText) or (Text [CurPos - 1] = CustomStyle.IgnoreChar)) then
					KeepRunning := True
				else begin
					if Assigned (NewCustomStyle) then begin
						SetSectionType (stCustomStyle);
						CustomStyle := NewCustomStyle;
						if CustomStyle.Switchable then begin
							KeepRunning := True;
							Inc (SectionLength);
							Inc (CurPos);
						end else begin
							repeat
								L := Pos (CustomStyle.EndText, Copy (Text, CurPos + Length (CustomStyle.BeginText), TextLength));
								Inc (CurPos, L + Length (CustomStyle.BeginText) + Length (CustomStyle.EndText) - 2);
							until (L < 1) or (CurPos <= Length (CustomStyle.EndText)) or (CurPos <= Length (CustomStyle.EndText)) or (Text [CurPos - Length (CustomStyle.EndText)] <> CustomStyle.IgnoreChar);
							if CustomStyle.EndText = #13 then
								Inc (CurPos);
							if (L < 1) or (CurPos <= Length (CustomStyle.EndText)) or (CurPos > TextLength) then
								CurPos := TextLength;
							Inc (CurPos);
							SectionLength := CurPos - SectionStart;
							SetSectionType (stText);
						end;
						Continue;
					end else if (SectionType = stCustomStyle) and Assigned (CustomStyle) and CustomStyle.Switchable then begin
						if CustomStyle.EndText = #13 then
							L := 2
						else
							L := Length (CustomStyle.EndText);
						Inc (SectionLength, L);
						Inc (CurPos, L);
						SetSectionType (stText);
						Continue;
					end;
					if CharIsParenthesis (CurChar) then
						SetSectionType (stParenthesis)
					else if CharIsSymbol (CurChar) then
						SetSectionType (stSymbol)
					else
						SetSectionType (stText);
				end;
				Inc (SectionLength);
				Inc (CurPos);
			end;
			Range.REnd := CurPos - 1;
			if CurPos > TextLength then
				ProcessSection;
			LastPRange := SyntaxRangeAtPosWithTip (Range.RStart, LastRange);
			Dec (SyntaxColoring.FUpdateDebth);
		end else
			FreeAllSyntaxRanges;
	end;
	{$IFDEF SyntaxDebug}
		if FindSyntaxHole <> nil then
			raise ESourceEdit.Create (SInvalidPropertyValue);
	{$ENDIF}
end;

function TSourceEdit.ReplaceSyntaxRanges(NewRange: TSyntaxRange;
	var StartRange: TSyntaxRange): Boolean;
var
	PrevRange: TSyntaxRange;
begin
	Result := False;
	while Assigned (StartRange) and ((StartRange.RStart > NewRange.RStart) or (StartRange = NewRange)) do
		StartRange := StartRange.PrevRange;
	if not Assigned (StartRange) then
		StartRange := FirstSyntaxRange;
	while Assigned (StartRange) and ((StartRange.RStart < NewRange.RStart) or (StartRange = NewRange)) do
		StartRange := StartRange.NextRange;
	if (not Assigned (StartRange)) and (LastSyntaxRange <> NewRange) then
		StartRange := LastSyntaxRange;
	while Assigned (StartRange) and (StartRange <> NewRange) and ((StartRange.RStart <= NewRange.REnd) or (StartRange.REnd <= NewRange.REnd)) do begin
		Result := StartRange.EqualEndingsWith (NewRange);
		PrevRange := StartRange;
		StartRange := StartRange.NextRange;
		PrevRange.Free;
	end;
	{$IFDEF SyntaxDebug}
		if FindSyntaxOverlap <> nil then
			raise ESourceEdit.Create (SInvalidPropertyValue);
	{$ENDIF}
end;

procedure TSourceEdit.SetSyntaxColoring(const Value: TSyntaxColoring);
begin
	FSyntaxColoring.Assign (Value);
end;

function TSourceEdit.SyntaxRangeAtPos(RangePos: Integer): TSyntaxRange;
begin
	Result := FirstSyntaxRange;
	while Assigned (Result) and (Result.REnd < RangePos) do
		Result := Result.NextRange;
end;

function TSourceEdit.SyntaxRangeAtPosWithTip(RangePos: Integer;
	Tip: TSyntaxRange): TSyntaxRange;
begin
	if Assigned (Tip) then begin
		Result := Tip;
		while Assigned (Result) and (Result.REnd >= RangePos) do
			Result := Result.PrevRange;
		if not Assigned (Result) then
			Result := FirstSyntaxRange;
	end else
		Result := FirstSyntaxRange;
	while Assigned (Result) and (Result.REnd < RangePos) do
		Result := Result.NextRange;
end;

procedure TSourceEdit.TextChangeNotification(StartPos, OldLength,
	NewLength: Integer);
begin
	inherited;
	LastPRange := nil;
	LastCRange := TMCRange.Create (nil);
	with LastCRange do begin
		Editor := Self;
		RStart := StartPos;
		RLength := NewLength;
		ReColorRange (LastCRange);
	end;
end;

procedure TSourceEdit.TextChangeNotificationAfter;
begin
	inherited;
	if Assigned (LastCRange) then begin
		while Assigned (LastPRange) do begin
			if LastPRange.REnd <= LastCRange.REnd then
				LastPRange.SetNewParenthesisLevel
			else
				LastPRange.UpdateParenthesisLevel;
			LastPRange := LastPRange.NextRange;
		end;
		LastCRange.DrawRange;
		LastCRange.Free;
		LastCRange := nil;
	end;
end;

{ TSyntaxColoring }

procedure TSyntaxColoring.Assign(Source: TPersistent);
begin
	if Source is TSyntaxColoring then begin
		BeginUpdate;
		FEnabled := TSyntaxColoring(Source).Enabled;
		FSymbolColor := TSyntaxColoring(Source).SymbolColor;
		FSymbolStyle := TSyntaxColoring(Source).SymbolStyle;
		FSymbolCustomStyle := TSyntaxColoring(Source).SymbolCustomStyle;
		FNumberColor := TSyntaxColoring(Source).NumberColor;
		FNumberStyle := TSyntaxColoring(Source).NumberStyle;
		FNumberCustomStyle := TSyntaxColoring(Source).NumberCustomStyle;
		FWordLists.Assign (TSyntaxColoring(Source).WordLists);
		FCustomStyles.Assign (TSyntaxColoring(Source).CustomStyles);
		FParenthesisColors.Assign (TSyntaxColoring(Source).ParenthesisColors);
		FParenthesisStyle := TSyntaxColoring(Source).ParenthesisStyle;
		FParenthesisCustomStyle := TSyntaxColoring(Source).ParenthesisCustomStyle;
		EndUpdate;
	end else if Source is TSyntaxColoringCopy then begin
		BeginUpdate;
		FEnabled := TSyntaxColoringCopy(Source).Enabled;
		FSymbolColor := TSyntaxColoringCopy(Source).SymbolColor;
		FSymbolStyle := TSyntaxColoringCopy(Source).SymbolStyle;
		FSymbolCustomStyle := TSyntaxColoringCopy(Source).SymbolCustomStyle;
		FNumberColor := TSyntaxColoringCopy(Source).NumberColor;
		FNumberStyle := TSyntaxColoringCopy(Source).NumberStyle;
		FNumberCustomStyle := TSyntaxColoringCopy(Source).NumberCustomStyle;
		FWordLists.Assign (TSyntaxColoringCopy(Source).WordLists);
		FCustomStyles.Assign (TSyntaxColoringCopy(Source).CustomStyles);
		FParenthesisColors.Assign (TSyntaxColoringCopy(Source).ParenthesisColors);
		FParenthesisStyle := TSyntaxColoringCopy(Source).ParenthesisStyle;
		FParenthesisCustomStyle := TSyntaxColoringCopy(Source).ParenthesisCustomStyle;
		EndUpdate;
	end else
		inherited Assign (Source);
end;

procedure TSyntaxColoring.BeginUpdate;
begin
	Inc (FUpdateDebth);
end;

procedure TSyntaxColoring.ChangeNotification(Sender: TObject);
begin
	ColoringChange;
end;

procedure TSyntaxColoring.ColoringChange;
var
	I: Integer;
begin
	if FUpdateDebth = 0 then
		if Assigned (Owner) then
			if Owner is TSourceEdit then
				with Owner as TSourceEdit do begin
					with SymbolFont do begin
						Assign (Font);
						Color := SymbolColor;
						if SymbolCustomStyle then
							Style := SymbolStyle;
					end;
					with NumberFont do begin
						Assign (Font);
						Color := NumberColor;
						if NumberCustomStyle then
							Style := NumberStyle;
					end;
					with ParenthesisFont do begin
						Assign (Font);
						Color := GetParenthesisColor (0);
						if ParenthesisCustomStyle then
							Style := ParenthesisStyle;
					end;
					with WordLists do
						for I := 0 to Count - 1 do
							with Items[I].Font do begin
								Assign (Font);
								if Items[I].CustomColor then
									Color := Items[I].Color;
								if Items[I].CustomStyle then
									Style := Items[I].Style;
							end;
					with CustomStyles do
						for I := 0 to Count - 1 do
							with Items[I].Font do begin
								Assign (Font);
								if Items[I].CustomColor then
									Color := Items[I].Color;
								if Items[I].CustomStyle then
									Style := Items[I].Style;
							end;
					ReColor;
				end;
end;

constructor TSyntaxColoring.Create(AOwner: TPersistent);
begin
	inherited;
	SymbolFont := TFont.Create;
	NumberFont := TFont.Create;
	ParenthesisFont := TFont.Create;
	FWordLists := TWordLists.Create (Self);
	FCustomStyles := TCustomStyles.Create (Self);
	FParenthesisColors := TStringList.Create;
	FParenthesisColors.Add ('$000000');
	FParenthesisColors.OnChange := ChangeNotification;
	FNumberColor := clGreen;
	FSymbolColor := clOlive;
	FEnabled := True;
end;

destructor TSyntaxColoring.Destroy;
begin
	FParenthesisColors.Free;
	FCustomStyles.Free;
	FWordLists.Free;
	NumberFont.Free;
	SymbolFont.Free;
	ParenthesisFont.Free;
	inherited;
end;

procedure TSyntaxColoring.EndUpdate;
begin
	Dec (FUpdateDebth);
	if FUpdateDebth = 0 then
		ColoringChange;
end;

function TSyntaxColoring.GetParenthesisColor(Index: Integer): TColor;
begin
	with ParenthesisColors do
		if (Count <= 0) or (Index < 0) then
			Result := clWindowText
		else
			try
				Result := StrToInt (Strings [Index mod Count]);
			except
				Result := clWindowText;
			end;
end;

procedure TSyntaxColoring.SetCustomStyles(const Value: TCustomStyles);
begin
	FCustomStyles.Assign (Value);
	ColoringChange;
end;

procedure TSyntaxColoring.SetEnabled(const Value: Boolean);
begin
	if FEnabled <> Value then begin
		FEnabled := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetNumberColor(const Value: TColor);
begin
	if FNumberColor <> Value then begin
		FNumberColor := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetNumberCustomStyle(const Value: Boolean);
begin
	if FNumberCustomStyle <> Value then begin
		FNumberCustomStyle := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetNumberStyle(const Value: TFontStyles);
begin
	if FNumberStyle <> Value then begin
		FNumberStyle := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetParenthesisColors(const Value: TStringList);
begin
	FParenthesisColors.Assign (Value);
end;

procedure TSyntaxColoring.SetParenthesisCustomStyle(const Value: Boolean);
begin
	if FParenthesisCustomStyle <> Value then begin
		FParenthesisCustomStyle := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetParenthesisStyle(const Value: TFontStyles);
begin
	if FParenthesisStyle <> Value then begin
		FParenthesisStyle := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetSymbolColor(const Value: TColor);
begin
	if FSymbolColor <> Value then begin
		FSymbolColor := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetSymbolCustomStyle(const Value: Boolean);
begin
	if FSymbolCustomStyle <> Value then begin
		FSymbolCustomStyle := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetSymbolStyle(const Value: TFontStyles);
begin
	if FSymbolStyle <> Value then begin
		FSymbolStyle := Value;
		ColoringChange;
	end;
end;

procedure TSyntaxColoring.SetWordLists(const Value: TWordLists);
begin
	FWordLists.Assign (Value);
	ColoringChange;
end;

{ TWordLists }

function TWordLists.Add: TWordList;
begin
	Result := TWordList (inherited Add);
end;

constructor TWordLists.Create(AColoring: TSyntaxColoring);
begin
	inherited Create (TWordList);
	FColoring := AColoring;
end;

function TWordLists.FindList(const S: string): TWordList;
var
	I: Integer;
begin
	Result := nil;
	for I := 0 to Count - 1 do
		if Items[I].WordInList (S) then begin
			Result := Items [I];
			Break;
		end;
end;

function TWordLists.GetItem(Index: Integer): TWordList;
begin
	Result := TWordList (inherited GetItem (Index));
end;

function TWordLists.GetOwner: TPersistent;
begin
	Result := FColoring;
end;

procedure TWordLists.SetItem(Index: Integer; Value: TWordList);
begin
	inherited SetItem (Index, Value);
end;

procedure TWordLists.Update(Item: TCollectionItem);
begin
	if Assigned (Item) then
		TWordList(Item).ListChange
	else
		if Assigned (FColoring) then
			FColoring.ColoringChange;
end;

{ TWordList }

procedure TWordList.Assign(Source: TPersistent);
begin
	if Source is TWordList then begin
		Caption := TWordList(Source).Caption;
		FCustomColor := TWordList(Source).CustomColor;
		FColor := TWordList(Source).Color;
		FCustomStyle := TWordList(Source).CustomStyle;
		FStyle := TWordList(Source).Style;
		FCaseSensitive := TWordList(Source).CaseSensitive;
		FWords.Assign (TWordList(Source).Words);
		ListChange;
	end else
		inherited Assign (Source);
end;

procedure TWordList.ChangeNotification(Sender: TObject);
begin
	ListChange;
end;

constructor TWordList.Create(Collection: TCollection);
begin
	inherited;
	Font := TFont.Create;
	FWords := TStringList.Create;
	FWords.Sorted := True;
	FWords.OnChange := ChangeNotification;
	FColor := clBlue;
	FCustomColor := True;
	FCaseSensitive := True;
end;

destructor TWordList.Destroy;
begin
	FWords.Free;
	Font.Free;
	inherited;
end;

function TWordList.GetDisplayName: string;
begin
	if FDisplayName = '' then
		Result := inherited GetDisplayName
	else
		Result := FDisplayName;
end;

procedure TWordList.ListChange;
begin
	if Assigned (Collection) then
		with TWordLists (Collection) do
			if Assigned (FColoring) then
				FColoring.ColoringChange;
end;

procedure TWordList.SetCaseSensitive(const Value: Boolean);
begin
	if FCaseSensitive <> Value then begin
		FCaseSensitive := Value;
		ListChange;
	end;
end;

procedure TWordList.SetColor(const Value: TColor);
begin
	if FColor <> Value then begin
		FColor := Value;
		ListChange;
	end;
end;

procedure TWordList.SetCustomColor(const Value: Boolean);
begin
	if FCustomColor <> Value then begin
		FCustomColor := Value;
		ListChange;
	end;
end;

procedure TWordList.SetCustomStyle(const Value: Boolean);
begin
	if FCustomStyle <> Value then begin
		FCustomStyle := Value;
		ListChange;
	end;
end;

procedure TWordList.SetDisplayName(const Value: string);
begin
	if FDisplayName <> Value then begin
		FDisplayName := Value;
		inherited;
	end;
end;

procedure TWordList.SetStyle(const Value: TFontStyles);
begin
	if FStyle <> Value then begin
		FStyle := Value;
		ListChange;
	end;
end;

procedure TWordList.SetWords(const Value: TStringList);
begin
	FWords.Assign (Value);
	ListChange;
end;

function TWordList.WordInList(S: string): Boolean;
var
	I: Integer;
begin
	if CaseSensitive then
		Result := Words.Find (S, I)
	else begin
		Result := False;
		S := UpperCase (S);
		for I := 0 to Words.Count - 1 do
			if UpperCase (Words [I]) = S then begin
				Result := True;
				Break;
			end;
	end;
end;

{ TCustomStyles }

function TCustomStyles.Add: TCustomStyle;
begin
	Result := TCustomStyle (inherited Add);
end;

constructor TCustomStyles.Create(AColoring: TSyntaxColoring);
begin
	inherited Create (TCustomStyle);
	FColoring := AColoring;
end;

function TCustomStyles.FindStyle(const S: string): TCustomStyle;
var
	I: Integer;
begin
	Result := nil;
	if S <> '' then
		for I := 0 to Count - 1 do
			if Items[I].BeginText = Copy (S, 1, Length (Items[I].BeginText)) then begin
				Result := Items [I];
				Break;
			end;
end;

function TCustomStyles.GetItem(Index: Integer): TCustomStyle;
begin
	Result := TCustomStyle (inherited GetItem (Index));
end;

function TCustomStyles.GetOwner: TPersistent;
begin
	Result := FColoring;
end;

procedure TCustomStyles.SetItem(Index: Integer; Value: TCustomStyle);
begin
	inherited SetItem (Index, Value);
end;

procedure TCustomStyles.Update(Item: TCollectionItem);
begin
	if Assigned (FColoring) then
		FColoring.ColoringChange;
end;

{ TCustomStyle }

procedure TCustomStyle.Assign(Source: TPersistent);
begin
	if Source is TCustomStyle then begin
		Caption := TCustomStyle(Source).Caption;
		FCustomColor := TCustomStyle(Source).CustomColor;
		FColor := TCustomStyle(Source).Color;
		FCustomStyle := TCustomStyle(Source).CustomStyle;
		FStyle := TCustomStyle(Source).Style;
		FBeginText := TCustomStyle(Source).BeginText;
		FEndText := TCustomStyle(Source).EndText;
		FIgnoreChar := TCustomStyle(Source).IgnoreChar;
		FSwitchable := TCustomStyle(Source).Switchable;
		DontSwitchToIndex1 := TCustomStyle(Source).DontSwitchToIndex1;
		DontSwitchToIndex2 := TCustomStyle(Source).DontSwitchToIndex2;
		StyleChange;
	end else
		inherited Assign (Source);
end;

constructor TCustomStyle.Create(Collection: TCollection);
begin
	inherited;
	Font := TFont.Create;
	FColor := clMaroon;
	FCustomColor := True;
end;

destructor TCustomStyle.Destroy;
var
	I: Integer;
begin
	if Assigned (Collection) then
		with Collection as TCustomStyles do
			for I := 0 to Count - 1 do
				with Items [I] do begin
					if FDontSwitchTo1 = Self then
						FDontSwitchTo1 := nil;
					if FDontSwitchTo2 = Self then
						FDontSwitchTo2 := nil;
				end;
	Font.Free;
	inherited;
end;

function TCustomStyle.GetDisplayName: string;
begin
	if FDisplayName = '' then
		Result := inherited GetDisplayName
	else
		Result := FDisplayName;
end;

function TCustomStyle.GetDontSwitchToIndex1: Integer;
begin
	if Assigned (DontSwitchTo1) then
		Result := DontSwitchTo1.Index
	else
		Result := -1;
end;

function TCustomStyle.GetDontSwitchToIndex2: Integer;
begin
	if Assigned (DontSwitchTo2) then
		Result := DontSwitchTo2.Index
	else
		Result := -1;
end;

procedure TCustomStyle.SetBeginText(Value: string);
begin
	if Value = '#13' then
		Value := #13;
	if FBeginText <> Value then begin
		FBeginText := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetColor(const Value: TColor);
begin
	if FColor <> Value then begin
		FColor := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetCustomColor(const Value: Boolean);
begin
	if FCustomColor <> Value then begin
		FCustomColor := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetCustomStyle(const Value: Boolean);
begin
	if FCustomStyle <> Value then begin
		FCustomStyle := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetDisplayName(const Value: string);
begin
	FDisplayName := Value;
end;

procedure TCustomStyle.SetDontSwitchTo1(const Value: TCustomStyle);
begin
	if FDontSwitchTo1 <> Value then begin
		FDontSwitchTo1 := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetDontSwitchTo2(const Value: TCustomStyle);
begin
	if FDontSwitchTo2 <> Value then begin
		FDontSwitchTo2 := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetDontSwitchToIndex1(const Value: Integer);
begin
	if Assigned (Collection) and (Value >= 0) and (Value < Collection.Count) then
		DontSwitchTo1 := (Collection as TCustomStyles).Items [Value]
	else
		DontSwitchTo1 := nil;
end;

procedure TCustomStyle.SetDontSwitchToIndex2(const Value: Integer);
begin
	if Assigned (Collection) and (Value >= 0) and (Value < Collection.Count) then
		DontSwitchTo2 := (Collection as TCustomStyles).Items [Value]
	else
		DontSwitchTo2 := nil;
end;

procedure TCustomStyle.SetEndText(Value: string);
begin
	if Value = '#13' then
		Value := #13;
	if FEndText <> Value then begin
		FEndText := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetIgnoreChar(const Value: TIgnoreChar);
begin
	if FIgnoreChar <> Value then begin
		FIgnoreChar := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetStyle(const Value: TFontStyles);
begin
	if FStyle <> Value then begin
		FStyle := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.SetSwitchable(const Value: Boolean);
begin
	if FSwitchable <> Value then begin
		FSwitchable := Value;
		StyleChange;
	end;
end;

procedure TCustomStyle.StyleChange;
begin
	if Assigned (Collection) then
		with TCustomStyles (Collection) do
			if Assigned (FColoring) then
				FColoring.ColoringChange;
end;

{ TSyntaxColoringCopy }

procedure TSyntaxColoringCopy.Assign(Source: TPersistent);
begin
	if Source is TSyntaxColoring then begin
		FEnabled := TSyntaxColoring(Source).Enabled;
		FSymbolColor := TSyntaxColoring(Source).SymbolColor;
		FSymbolStyle := TSyntaxColoring(Source).SymbolStyle;
		FSymbolCustomStyle := TSyntaxColoring(Source).SymbolCustomStyle;
		FNumberColor := TSyntaxColoring(Source).NumberColor;
		FNumberStyle := TSyntaxColoring(Source).NumberStyle;
		FNumberCustomStyle := TSyntaxColoring(Source).NumberCustomStyle;
		FWordLists.Assign (TSyntaxColoring(Source).WordLists);
		FCustomStyles.Assign (TSyntaxColoring(Source).CustomStyles);
		FParenthesisColors.Assign (TSyntaxColoring(Source).ParenthesisColors);
		FParenthesisStyle := TSyntaxColoring(Source).ParenthesisStyle;
		FParenthesisCustomStyle := TSyntaxColoring(Source).ParenthesisCustomStyle;
	end else
		inherited Assign (Source);
end;

constructor TSyntaxColoringCopy.Create(AOwner: TComponent);
begin
	inherited;
	FWordLists := TWordLists.Create (nil);
	FCustomStyles := TCustomStyles.Create (nil);
	FParenthesisColors := TStringList.Create;
	FParenthesisColors.Add ('$000000');
	FNumberColor := clGreen;
	FSymbolColor := clOlive;
	FEnabled := True;
end;

destructor TSyntaxColoringCopy.Destroy;
begin
	FParenthesisColors.Free;
	FCustomStyles.Free;
	FWordLists.Free;
	inherited;
end;

procedure TSyntaxColoringCopy.SetCustomStyles(const Value: TCustomStyles);
begin
	FCustomStyles.Assign (Value);
end;

procedure TSyntaxColoringCopy.SetParenthesisColors(
	const Value: TStringList);
begin
	FParenthesisColors.Assign (Value);
end;

procedure TSyntaxColoringCopy.SetWordLists(const Value: TWordLists);
begin
	FWordLists.Assign (Value);
end;

{ TSyntaxRange }

destructor TSyntaxRange.Destroy;
begin
	if Assigned (Editor) and ((Editor as TSourceEdit).SyntaxStartRange = Self) then
		(Editor as TSourceEdit).SyntaxStartRange := nil;
	if Assigned (PrevRange) then
		PrevRange.NextRange := NextRange
	else if Assigned (Editor) then
		(Editor as TSourceEdit).FirstSyntaxRange := NextRange;
	if Assigned (NextRange) then
		NextRange.PrevRange := PrevRange
	else if Assigned (Editor) then
		(Editor as TSourceEdit).LastSyntaxRange := PrevRange;
	inherited;
end;

class function TSyntaxRange.EqualEndings(Range1,
	Range2: TSyntaxRange): Boolean;
begin
	Result := Assigned (Range1) and Assigned (Range2) and Range1.EqualEndingsWith (Range2);
end;

function TSyntaxRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := REnd = Range.REnd;
end;

function TSyntaxRange.GetColor: TColor;
begin
	if Assigned (Editor) then
		Result := Editor.Color
	else
		Result := clWindow;
end;

function TSyntaxRange.GetNextParenthesisLevel: Integer;
begin
	Result := ParenthesisLevel;
end;

function TSyntaxRange.InsertAfter(RangeClass: TSyntaxRangeClass):
	TSyntaxRange;
begin
	Result := InsertRangeBefore (NextRange, RangeClass, Editor as TSourceEdit);
end;

function TSyntaxRange.InsertBefore(RangeClass: TSyntaxRangeClass):
	TSyntaxRange;
begin
	Result := InsertRangeBefore (Self, RangeClass, Editor as TSourceEdit);
end;

class function TSyntaxRange.InsertRangeBefore(Range: TSyntaxRange;
	RangeClass: TSyntaxRangeClass; Editor: TSourceEdit): TSyntaxRange;
begin
	// If Range is nil, new item is inserted at the end.
	if Assigned (Editor) then begin
		Result := RangeClass.Create (Editor.TrackedRanges);
		Result.OnOverwrite := Editor.OverwriteRange;
		if Assigned (Range) then
			Result.PrevRange := Range.PrevRange
		else
			Result.PrevRange := Editor.LastSyntaxRange;
		Result.NextRange := Range;
		if Assigned (Range) then begin
			if Assigned (Range.PrevRange) then
				Range.PrevRange.NextRange := Result
			else
				Editor.FirstSyntaxRange := Result;
		end else begin
			if not Assigned (Editor.FirstSyntaxRange) then
				Editor.FirstSyntaxRange := Result;
			if Assigned (Editor.LastSyntaxRange) then
				Editor.LastSyntaxRange.NextRange := Result;
			Editor.LastSyntaxRange := Result;
		end;
		if Assigned (Range) then
			Range.PrevRange := Result;
	end
	{$IFDEF SyntaxDebug}
  else
		raise ESourceEdit.Create (SInvalidPropertyValue);
	{$ENDIF}    
end;

class function TSyntaxRange.NewRangeInsertedAfter(Range: TSyntaxRange;
	Editor: TSourceEdit): TSyntaxRange;
begin
	if Assigned (Range) then
		Result := InsertRangeBefore (Range.NextRange, Self, Editor)
	else
		Result := nil;
end;

class function TSyntaxRange.NewRangeInsertedBefore(Range: TSyntaxRange;
	Editor: TSourceEdit): TSyntaxRange;
begin
	Result := InsertRangeBefore (Range, Self, Editor);
end;

function TSyntaxRange.ReplaceSyntaxRanges(var StartRange: TSyntaxRange):
	Boolean;
begin
	if Assigned (Editor) and (Editor is TSourceEdit) then
		Result := TSourceEdit(Editor).ReplaceSyntaxRanges (Self, StartRange)
	else
		Result := False;
end;

procedure TSyntaxRange.SetNewParenthesisLevel;
begin
	if Assigned (PrevRange) then
		ParenthesisLevel := PrevRange.NextParenthesisLevel
	else
		ParenthesisLevel := 0;
end;

procedure TSyntaxRange.UpdateParenthesisLevel;
begin
	SetNewParenthesisLevel;
end;

{ TNormalTextRange }

function TNormalTextRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := (Range is TNormalTextRange) and (inherited EqualEndingsWith (Range));
end;

function TNormalTextRange.GetFont: TFont;
begin
	if Assigned (Editor) then
		Result := Editor.Font
	else
		Result := nil;
end;

{ TCustomStyleRange }

function TCustomStyleRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := (Range is TCustomStyleRange) and (inherited EqualEndingsWith (Range)) and (Style = TCustomStyleRange(Range).Style);
end;

function TCustomStyleRange.GetFont: TFont;
begin
	if Assigned (Style) then
		Result := Style.Font
	else
		Result := nil;
end;

{ TWordListRange }

function TWordListRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := (Range is TWordListRange) and (inherited EqualEndingsWith (Range)) and (WordList = TWordListRange(Range).WordList);
end;

function TWordListRange.GetFont: TFont;
begin
	Result := WordList.Font;
end;

{ TSymbolRange }

function TSymbolRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := (Range is TSymbolRange) and (inherited EqualEndingsWith (Range)) and (Symbol = TSymbolRange(Range).Symbol);
end;

function TSymbolRange.GetFont: TFont;
begin
	if Assigned (Editor) then begin
		with Editor as TSourceEdit do
			if SyntaxColoring.Enabled then
				Result := SyntaxColoring.SymbolFont
			else
				Result := Font;
	end else
		Result := nil;
end;

{ TNumberRange }

function TNumberRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := (Range is TNumberRange) and (inherited EqualEndingsWith (Range)) and (Number = TNumberRange(Range).Number);
end;

function TNumberRange.GetFont: TFont;
begin
	if Assigned (Editor) then begin
		with Editor as TSourceEdit do
			if SyntaxColoring.Enabled then
				Result := SyntaxColoring.NumberFont
			else
				Result := Font;
	end else
		Result := nil;
end;

{ TParenthesisRange }

function TParenthesisRange.EqualEndingsWith(Range: TSyntaxRange): Boolean;
begin
	Result := (Range is TParenthesisRange) and (inherited EqualEndingsWith (Range)) and (Style = TParenthesisRange(Range).Style);
end;

function TParenthesisRange.GetFont: TFont;
begin
	if Assigned (Editor) then begin
		with Editor as TSourceEdit do
			if SyntaxColoring.Enabled then begin
				Result := SyntaxColoring.ParenthesisFont;
				Result.Color := SyntaxColoring.GetParenthesisColor (ParenthesisLevel - 1);
			end else
				Result := Font;
	end else
		Result := nil;
end;

function TParenthesisRange.GetNextParenthesisLevel: Integer;
begin
	if Style = psOpening then
		Result := inherited GetNextParenthesisLevel
	else
		Result := inherited GetNextParenthesisLevel - 1;
end;

procedure TParenthesisRange.SetNewParenthesisLevel;
begin
	if Style = psClosing then
		inherited SetNewParenthesisLevel
	else
		if Assigned (PrevRange) then
			ParenthesisLevel := PrevRange.NextParenthesisLevel + 1
		else
			ParenthesisLevel := 1;
end;

procedure TParenthesisRange.UpdateParenthesisLevel;
var
	Lev: Integer;
begin
	Lev := ParenthesisLevel;
	inherited;
	if Lev <> ParenthesisLevel then
		DrawRange;
end;

end.
