
{*******************************************************}
{                                                       }
{       TMemo-Compatible Component v1.0                 }
{                                                       }
{       Copyright (c) 2000 Sebastian Reichelt           }
{                                                       }
{*******************************************************}

unit MemoComponentUnit;

interface

uses
	Windows, Messages, SysUtils, Classes, Forms, Graphics, Controls, StdCtrls, ObjList;

const
  MAX_BREAKPOINTS_NUM = 16;

type
	TMCRanges = class;
	TCustomRange = class;
	TMCRange = class;
	TWholeTextRange = class;
	TVisibleRange = class;
	TSelectionRange = class;
//	TCustomFormattedRange = class;
	TFormattedRange = class;
	TNormalFormattedRange = class;

  TBreakpointClickEvent = procedure (Sender: TObject; BreakpointNum : integer) of object;

  TCaretPosChanged = procedure (Sender: TObject; X,Y : integer) of object;

	TFormattedRangeArray = array of pointer;   //TCustomFormattedRange

	TIntegerList = class;

	TTextCell = record
		Row,
		Col: Integer;
	end;

	PUndoOperation = ^TUndoOperation;
	TUndoOperation = record
		RStart,
		REnd: Integer;
		NewText: string;
		NextItem: PUndoOperation;
	end;

  TBreakpoint = record
    LineNum : integer;
    Enabled : boolean;
    BrkName : string[128];
    BrkConditional : string[128];
  end;

	{	Note:
		The key element of the TMemoComponent class is the ReplaceText
		method.  Its intent is to replace a piece of text (range) with as
		little destruction as possible.  All values, for example the line
		index table and tracked ranges, are kept intact.  Do not attempt to
		call this procedure directly or to modify the memo's text directly
		using FText.  Instead, create a range and use its Text property.  The
		range can be tracked or not, but be sure to set the Editor property if
		it is not tracked.
		To change the behavior when drawing text, override the virtual
		CreateSplitRanges method.  The result must be a newly created
		TMCRanges collection with items of type TCustomFormattedRange }

	TMemoComponent = class(TCustomControl)
	private
		FHasFocus: Boolean;
		FCaretCreated: Boolean;
		FSelecting: Boolean;
		FLineStarts: TIntegerList;
		FScrollBars: TScrollStyle;
		FBorderStyle: TBorderStyle;
		FReadOnly: Boolean;
		FOnChange: TNotifyEvent;
		FText: TCaption;
		FTrackedRanges: TMCRanges;
		FWholeText: TCustomRange;
		FLines: TStrings;
		FVisibleRange: TVisibleRange;
		FSelection: TSelectionRange;
		FLongestLineLength: Integer;
		FAlwaysShowCaret: Boolean;
		FLeftMargin: Integer;
		FTopMargin: Integer;
		FTabSize: Integer;
		FOnSelectionChange: TNotifyEvent;
		FTextLength: Integer;
		FBitmapped: Boolean;
		FOnChangePrivate: TNotifyEvent;
		procedure CMFontChanged(var Message: TMessage); message cm_FontChanged;
		procedure WMSize(var Message: TWMSize); message wm_Size;
		procedure WMHScroll(var Message: TWMHScroll); message wm_HScroll;
		procedure WMVScroll(var Message: TWMVScroll); message wm_VScroll;
		procedure WMSetFocus(var Message: TWMSetFocus); message wm_SetFocus;
		procedure WMKillFocus(var Message: TWMKillFocus); message wm_KillFocus;
		procedure WMEraseBkgnd(var Message: TWMEraseBkgnd); message wm_EraseBkgnd;
		procedure CMWantSpecialKey(var Message: TCMWantSpecialKey); message cm_WantSpecialKey;
		procedure WMClear(var Message: TWMClear); message wm_Clear;
		procedure WMCut(var Message: TWMCut); message wm_Cut;
		procedure WMCopy(var Message: TWMCopy); message wm_Copy;
		procedure WMPaste(var Message: TWMPaste); message wm_Paste;
		procedure WMSetText(var Message: TWMSetText); message wm_SetText;
		procedure WMGetText(var Message: TWMGetText); message wm_GetText;
		procedure WMGetTextLength(var Message: TWMGetTextLength); message wm_GetTextLength;
		procedure WMTimer(var Message: TWMTimer); message wm_Timer;
		procedure EMUndo(var Message: TMessage); message em_Undo;
		procedure EMCanUndo(var Message: TMessage); message em_CanUndo;
		procedure SetText(const Value: TCaption);
		procedure SetScrollBars(const Value: TScrollStyle);
		procedure SetBorderStyle(const Value: TBorderStyle);
		procedure SetReadOnly(const Value: Boolean);
		procedure SetLines(const Value: TStrings);
		function GetLineCount: Integer;
		function GetLineLength(LineIndex: Integer): Integer;
		function GetVisualLineLength(LineIndex: Integer): Integer;
		function GetSelLength: Integer;
		function GetSelStart: Integer;
		procedure SetSelLength(const Value: Integer);
		procedure SetSelStart(const Value: Integer);
		procedure SetAlwaysShowCaret(const Value: Boolean);
		procedure SetLeftMargin(const Value: Integer);
		procedure SetTopMargin(const Value: Integer);
		procedure SetTabSize(const Value: Integer);
		function GetCanRedo: Boolean;
		function GetCanUndo: Boolean;
		procedure SetBitmapped(const Value: Boolean);

	protected
		FontHeight,
		FontWidth,
		PageHeight,
		PageWidth: Integer;
		DrawBmp: TBitmap;
		FUndoStack,
		FRedoStack: PUndoOperation;
		FInUndo,
		DontNotify: Boolean;

    FBreakpoints : array[0..MAX_BREAKPOINTS_NUM - 1] of TBreakpoint;
    FOnBreakpointClick : TBreakpointClickEvent;
    FOnCaretPosChanged : TCaretPosChanged;

    procedure ToggleBreakpoint(LineNum : integer);

		procedure CreateParams(var Params: TCreateParams); override;
		procedure CreateWnd; override;
		procedure ReplaceText(Range: TCustomRange; const NewText: string); virtual;
		procedure DrawTextLine(Range: TCustomRange; Left, Top: Integer; NextTabStop: Integer); virtual;
		function CreateSplitRanges(Range: TCustomRange): TFormattedRangeArray; virtual;
		procedure DrawBorder(LeftRect, TopRect: TRect; Canvas: TCanvas); virtual;
		procedure TextChangeNotification(StartPos, OldLength, NewLength: Integer); dynamic;
		procedure TextChangeNotificationAfter; dynamic;
		procedure Change; dynamic;
		procedure SelectionChange; dynamic;
		procedure UpdateFontSize; virtual;
		procedure UpdatePageSize; virtual;
		procedure UpdateDrawBmp; virtual;
		procedure ReCreateCaret; virtual;
		procedure FreeCaret; virtual;
		procedure Paint; override;
		procedure MouseDown(Button: TMouseButton; Shift: TShiftState; X, Y: Integer); override;
		procedure MouseMove(Shift: TShiftState; X, Y: Integer); override;
		procedure MouseUp(Button: TMouseButton; Shift: TShiftState; X, Y: Integer); override;

    procedure MyMouseWheelHandler(Sender : TObject; Shift: TShiftState; WheelDelta: Integer; MousePos:
                                  TPoint; var Handled: Boolean);

		procedure KeyPress(var Key: Char); override;
		procedure KeyDown(var Key: Word; Shift: TShiftState); override;
		function GetLastUndo: TUndoOperation; virtual;
		function GetLastRedo: TUndoOperation; virtual;
		function CreateUndoBeginEndBlock: PUndoOperation; virtual;
		function IsUndoBeginEndBlock(Op: PUndoOperation): Boolean; virtual;
		procedure MakeUndoOperation(Op: PUndoOperation); virtual;
		procedure MakeRedoOperation(Op: PUndoOperation); virtual;
	public
		constructor Create(AOwner: TComponent); override;
		destructor Destroy; override;
		procedure Clear; virtual;
		function CharIdxToCell(CharIdx: Integer): TTextCell; virtual;
		function CellToCharIdx(Cell: TTextCell): Integer; virtual;
		function ScrPointToScrCell(P: TPoint): TTextCell; virtual;
		function ScrCellToScrPoint(Cell: TTextCell): TPoint; virtual;
		function TabSpacesAtPos(P: Integer): Integer; virtual;
		function CellToScrCol(Cell: TTextCell): Integer; virtual;
		procedure CellFromScrCol(var Cell: TTextCell); virtual;
		function CellFromScrColToScrCol(var Cell: TTextCell): Integer; virtual;
		procedure SelectAll;
		procedure ClearSelection;
		procedure CutToClipboard;
		procedure CopyToClipboard;
		procedure PasteFromClipboard;
		procedure Undo;
		procedure Redo;
		procedure ClearUndo;
		procedure ClearRedo;
		procedure ScrollCaret; virtual;
		procedure ChangeIndent(Change: Integer); virtual;

    function GetBreakpointsNum : integer;
    function GetBreakpointByHandle(BrkHandle : integer) : TBreakpoint;
    function FindBreakpoint(LineNum : integer) : integer;
    procedure RemoveBreakpointByNumber(BrkNum : integer);
    function GetBreakpointByNumber(BrkNum : integer) : TBreakpoint;
    procedure SetBreakpointByNumber(BrkNum : integer; var Brk : TBreakpoint);
    procedure RemoveAllBreakpoints;
    procedure GotoPos(X,Y : integer);

		property Text: TCaption read FText write SetText;
		property TextLength: Integer read FTextLength;
		property TrackedRanges: TMCRanges read FTrackedRanges;
		property WholeText: TCustomRange read FWholeText;
		property LineCount: Integer read GetLineCount;
		property LongestLineLength: Integer read FLongestLineLength;
		property LineLength[LineIndex: Integer]: Integer read GetLineLength;
		property VisualLineLength[LineIndex: Integer]: Integer read GetVisualLineLength;
		property VisibleRange: TVisibleRange read FVisibleRange;
		property Selection: TSelectionRange read FSelection;
		property SelStart: Integer read GetSelStart write SetSelStart;
		property SelLength: Integer read GetSelLength write SetSelLength;
		property CanUndo: Boolean read GetCanUndo;
		property CanRedo: Boolean read GetCanRedo;
		property OnChangePrivate: TNotifyEvent read FOnChangePrivate write FOnChangePrivate;
	published
		property ScrollBars: TScrollStyle read FScrollBars write SetScrollBars default ssNone;
		property BorderStyle: TBorderStyle read FBorderStyle write SetBorderStyle default bsSingle;
		property Bitmapped: Boolean read FBitmapped write SetBitmapped;
		property ReadOnly: Boolean read FReadOnly write SetReadOnly default False;
		property OnChange: TNotifyEvent read FOnChange write FOnChange;
		property OnSelectionChange: TNotifyEvent read FOnSelectionChange write FOnSelectionChange;
		property Lines: TStrings read FLines write SetLines;
		property AlwaysShowCaret: Boolean read FAlwaysShowCaret write SetAlwaysShowCaret;
		property LeftMargin: Integer read FLeftMargin write SetLeftMargin;
		property TopMargin: Integer read FTopMargin write SetTopMargin;
		property TabSize: Integer read FTabSize write SetTabSize;
		property TabStop default True;
		property Align;
		property Anchors;
		property Color nodefault;
		property Constraints;
		property Ctl3D;
		property Enabled;
		property Font;
		property ParentColor;
		property ParentCtl3D;
		property ParentFont;
		property ParentShowHint;
		property PopupMenu;
		property ShowHint;
		property TabOrder;
		property Visible;
		property OnClick;
		property OnDblClick;
		property OnEnter;
		property OnExit;
		property OnKeyDown;
		property OnKeyPress;
		property OnKeyUp;
		property OnMouseDown;
		property OnMouseMove;
		property OnMouseUp;

    property OnBreakpointClick : TBreakpointClickEvent read FOnBreakpointClick write FOnBreakpointClick;
    property OnCaretPosChanged : TCaretPosChanged read FOnCaretPosChanged write FOnCaretPosChanged;
	end;

	TCustomRange = class(TContainerItem)
	private
		FEditor: TMemoComponent;
		FChanging: Integer;
		FOnOverwrite: TNotifyEvent;
		FOnChange: TNotifyEvent;
		function GetEndPoint: TPoint;
		function GetStartPoint: TPoint;
	protected
		procedure SetRStart(const Value: Integer); virtual;
		procedure SetREnd(const Value: Integer); virtual;
		procedure SetRLength(const Value: Integer); virtual;
		function GetRStart: Integer; virtual; abstract;
		function GetREnd: Integer; virtual; abstract;
		function GetRLength: Integer; virtual;
		function GetEndRowCol: TTextCell; virtual;
		function GetStartRowCol: TTextCell; virtual;
		procedure SetEndRowCol(const Value: TTextCell); virtual;
		procedure SetStartRowCol(const Value: TTextCell); virtual;
		procedure SetText(const Value: string); virtual;
		function GetText: string; virtual;
		procedure Changing; dynamic;
		procedure Change; dynamic;
		procedure DiscardChanges; dynamic;
	public
		constructor Create(Collection: TCollection); override;
		procedure AssignTo(Dest: TPersistent); override;
		procedure NotifyOverwrite; dynamic;
		procedure DoChanging;
		procedure DoChange;
		procedure DoDiscardChanges;
		procedure Clear; virtual;
		function CharInRange(CharIdx: Integer): Boolean;
		procedure DrawRange; virtual;
		procedure ScrollInView(FromBorder: Integer); virtual;
		property Editor: TMemoComponent read FEditor write FEditor;
		property StartRowCol: TTextCell read GetStartRowCol write SetStartRowCol;
		property EndRowCol: TTextCell read GetEndRowCol write SetEndRowCol;
		property StartPoint: TPoint read GetStartPoint;
		property EndPoint: TPoint read GetEndPoint;
		property Text: string read GetText write SetText;
	published
		property RStart: Integer read GetRStart write SetRStart;
		property REnd: Integer read GetREnd write SetREnd;
		property RLength: Integer read GetRLength write SetRLength;
		property OnChange: TNotifyEvent read FOnChange write FOnChange;
		property OnOverwrite: TNotifyEvent read FOnOverwrite write FOnOverwrite;
	end;

	TMCRange = class(TCustomRange)
	private
		FRStart: Integer;
		FREnd: Integer;
	protected
		procedure SetREnd(const Value: Integer); override;
		procedure SetRStart(const Value: Integer); override;
		function GetREnd: Integer; override;
		function GetRStart: Integer; override;
	public
		constructor Create(Collection: TCollection); override;
	end;

	TWholeTextRange = class(TCustomRange)
	protected
		function GetREnd: Integer; override;
		function GetRStart: Integer; override;
	end;

	TVisibleRange = class(TCustomRange)
	private
		FLeftCol: Integer;
		FTopRow: Integer;
		function GetRightCol: Integer;
		procedure SetRightCol(const Value: Integer);
		procedure SetLeftCol(const Value: Integer);
		procedure Update;
	protected
		VisibleTextRect: TRect;
		procedure SetRStart(const Value: Integer); override;
		procedure SetREnd(const Value: Integer); override;
		procedure SetRLength(const Value: Integer); override;
		function GetRStart: Integer; override;
		function GetREnd: Integer; override;
		function GetStartRowCol: TTextCell; override;
		function GetEndRowCol: TTextCell; override;
		procedure SetStartRowCol(const Value: TTextCell); override;
		procedure SetEndRowCol(const Value: TTextCell); override;
		procedure Changing; override;
		procedure Change; override;
	public
		constructor Create(Collection: TCollection); override;
	published
		property LeftCol: Integer read FLeftCol write SetLeftCol;
		property RightCol: Integer read GetRightCol write SetRightCol;
	end;

	TSelectionRange = class(TMCRange)
	private
		FOldSel: TCustomRange;
		FBackwards: Boolean;
		FHidden: Boolean;
		FCaretShowing: Boolean;
		FScrCol: Integer;
		function GetCursorPos: Integer;
		procedure SetCursorPos(const Value: Integer);
		procedure SetHidden(const Value: Boolean);
		function GetScrCol: Integer;
	protected
		procedure Changing; override;
		procedure Change; override;
		procedure DiscardChanges; override;
	public
		procedure AssignTo(Dest: TPersistent); override;
		procedure NoSelAtPos(Pos: Integer);
		procedure UpdateCaretPos;
		procedure ShowCaret;
		procedure HideCaret;
		function ScrColToCol(Row: Integer): Integer;
		property CursorPos: Integer read GetCursorPos write SetCursorPos;
		property ScrCol: Integer read GetScrCol write FScrCol;
	published
		property Backwards: Boolean read FBackwards write FBackwards;
		property Hidden: Boolean read FHidden write SetHidden;
	end;

	TCustomFormattedRange = class(TMCRange)
	protected
		function GetColor: TColor; virtual; abstract;
		function GetFont: TFont; virtual; abstract;
		procedure SetColor(const Value: TColor); virtual;
		procedure SetFont(const Value: TFont); virtual;
	public
		FreeWhenDone: Boolean;
		procedure AssignTo(Dest: TPersistent); override;
	published
		property Color: TColor read GetColor write SetColor;
		property Font: TFont read GetFont write SetFont;
	end;

	TFormattedRange = class(TCustomFormattedRange)
	private
		FFont: TFont;
		FColor: TColor;
	protected
		function GetColor: TColor; override;
		function GetFont: TFont; override;
		procedure SetColor(const Value: TColor); override;
		procedure SetFont(const Value: TFont); override;
	public
		constructor Create(Collection: TCollection); override;
		destructor Destroy; override;
	end;

	TNormalFormattedRange = class(TCustomFormattedRange)
	protected
		function GetColor: TColor; override;
		function GetFont: TFont; override;
	end;

	TRangeClass = class of TCustomRange;

	TMCRanges = class(TObjectContainer)
	private
		FItemClass: TRangeClass;
		function NewGetOwner: TMemoComponent;
	protected
		function NewGetItem(ItemIndex: Integer): TCustomRange;
		procedure NewSetItem(ItemIndex: Integer; const Value: TCustomRange);
	public
		constructor Create(AOwner: TMemoComponent);
		function Add: TCustomRange; overload;
		function Add(Start, Count: Integer): TCustomRange; overload;
		property ItemClass: TRangeClass read FItemClass write FItemClass;
		property Items[ItemIndex: Integer]: TCustomRange read NewGetItem write NewSetItem;
		property Owner: TMemoComponent read NewGetOwner;
	end;

	TIntegerList = class(TObject)
	private
		FList: TList;
		function GetCount: Integer;
		function GetItem(ItemIndex: Integer): Integer;
		procedure SetItem(ItemIndex: Integer; const Value: Integer);
		procedure SetCount(const Value: Integer);
	public
		constructor Create;
		destructor Destroy; override;
		function Add(Item: Integer): Integer;
		procedure Insert(Index: Integer; Item: Integer);
		procedure Delete(Index: Integer);
		procedure Clear; dynamic;
		property Items[ItemIndex: Integer]: Integer read GetItem write SetItem;
		property Count: Integer read GetCount write SetCount;
	end;

function TextCell(CellRow, CellCol: Integer): TTextCell;

procedure Register;

implementation

uses
	ClipBrd;

const
	MaxScrollTolerance = 2;

procedure Register;
begin
	RegisterComponents('Objet', [TMemoComponent]);
end;

{ TMemoComponentStrings Definition }

type
	TMemoComponentStrings = class(TStrings)
	private
		Memo: TMemoComponent;
	protected
		function Get(LineIndex: Integer): string; override;
		function GetCount: Integer; override;
		function GetTextStr: string; override;
		procedure Put(LineIndex: Integer; const S: string); override;
		procedure SetTextStr(const Value: string); override;
	public
		procedure Clear; override;
		procedure Delete(LineIndex: Integer); override;
		procedure Insert(LineIndex: Integer; const S: string); override;
	end;

{ Helper Functions }

function TextCell(CellRow, CellCol: Integer): TTextCell;
begin
	with Result do begin
		Row := CellRow;
		Col := CellCol;
	end;
end;

{ TMemoComponent }

procedure TMemoComponent.ToggleBreakpoint(LineNum: integer);
var
  i : integer;
begin
  // Check if breakpoint already exists
  for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
    if FBreakpoints[i].Enabled and (FBreakpoints[i].LineNum = LineNum) then
      begin
        // Remove breakpoint
        FBreakpoints[i].Enabled := false;
        Exit;
      end;

  // Search an empty slot
  for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
    // Add a breakpoint
    if not FBreakpoints[i].Enabled then
    begin
      FBreakpoints[i].LineNum := LineNum;
      FBreakpoints[i].Enabled := true;
      FBreakpoints[i].BrkName := 'Breakpoint in line ' + IntToStr(LineNum);
      FBreakpoints[i].BrkConditional := '';
      break;
    end;
end;

procedure TMemoComponent.CellFromScrCol(var Cell: TTextCell);
var
	I,
	Col,
	Count: Integer;
begin
	if Cell.Row < 1 then
		Cell.Row := 1;
	if Cell.Row > LineCount then
		Cell.Row := LineCount;
	if TabSize <> 1 then begin
		Count := 0;
		I := CellToCharIdx (TextCell (Cell.Row, 1));
		Col := Cell.Col;
		Cell.Col := 1;
		while Count < Col do begin
			if (I <= TextLength) and (Text [I] = #9) then
				Count := (Count div TabSize + 1) * TabSize
			else
				Inc (Count);
			if Count < Col then begin
				Inc (I);
				Inc (Cell.Col);
			end;
		end;
	end;
	if Cell.Col < 1 then
		Cell.Col := 1;
	if Cell.Col > LineLength [Cell.Row] + 1 then
		Cell.Col := LineLength [Cell.Row] + 1;
end;

function TMemoComponent.CellFromScrColToScrCol(var Cell: TTextCell):
	Integer;
var
	I,
	Col,
	Count: Integer;
begin
	if Cell.Row < 1 then
		Cell.Row := 1;
	if Cell.Row > LineCount then
		Cell.Row := LineCount;
	if TabSize = 1 then
		Result := Cell.Col
	else begin
		Result := 1;
		Count := 0;
		I := CellToCharIdx (TextCell (Cell.Row, 1));
		Col := Cell.Col;
		Cell.Col := 1;
		while Count < Col do begin
			Result := Count + 1;
			if (I <= TextLength) and (Text [I] = #9) then
				Count := (Count div TabSize + 1) * TabSize
			else
				Inc (Count);
			if Count < Col then begin
				Inc (I);
				Inc (Cell.Col);
			end;
		end;
	end;
	if Cell.Col < 1 then
		Cell.Col := 1;
	if Cell.Col > LineLength [Cell.Row] + 1 then
		Cell.Col := LineLength [Cell.Row] + 1;
end;

function TMemoComponent.CellToCharIdx(Cell: TTextCell): Integer;
begin
	with Cell do
		if Row <= 0 then
			Result := Col
		else if Row > LineCount then
			Result := TextLength + 2 + Col
		else
			Result := FLineStarts.Items [Row - 1] + Col - 1;
end;

function TMemoComponent.CellToScrCol(Cell: TTextCell): Integer;
var
	I,
	Idx: Integer;
begin
	if TabSize = 1 then
		Result := Cell.Col
	else begin
		Result := 0;
		Idx := CellToCharIdx (TextCell (Cell.Row, 1));
		for I := Idx to Idx + Cell.Col - 2 do begin
			if (I > 0) and (I <= TextLength) and (Text [I] = #9) then
				Result := (Result div TabSize + 1) * TabSize
			else
				Inc (Result);
		end;
		Inc (Result);
	end;
end;

procedure TMemoComponent.Change;
begin
	if not DontNotify then begin
		inherited Changed;
		if Assigned (FOnChange) then
			FOnChange (Self);
		if Assigned (FOnChangePrivate) then
			FOnChangePrivate (Self);
	end;
end;

procedure TMemoComponent.ChangeIndent(Change: Integer);
var
	I,
	RS,
	RE,
	L,
	CurPos: Integer;
begin
	if Change <> 0 then begin
		DontNotify := True;
		VisibleRange.DoChanging;
		MakeUndoOperation (CreateUndoBeginEndBlock);
		RS := Selection.StartRowCol.Row;
		RE := Selection.EndRowCol.Row;
		if RE < RS then
			RE := RS;
		for I := RS to RE do begin
			CurPos := CellToCharIdx (TextCell (I, 1));
			if Change > 0 then begin
				while (CurPos <= TextLength) and (Text [CurPos] in [#9, #21]) do
					Inc (CurPos);
				L := Change;
				with TMCRange.Create (nil) do begin
					Editor := Self;
					RStart := CurPos;
					RLength := 0;
					Text := StringOfChar (#9, L);
					if (Selection.RLength > 0) and (Selection.RStart = REnd + 1) then
						Selection.RStart := RStart;
					Free;
				end;
			end else begin
				L := 0;
				while (CurPos <= TextLength) and (Text [CurPos] in [#9, #21]) do begin
					Inc (CurPos);
					Inc (L);
				end;
				if L > -Change then
					L := -Change;
				with TMCRange.Create (nil) do begin
					Editor := Self;
					RStart := CurPos - L;
					REnd := CurPos - 1;
					Text := '';
					Free;
				end;
			end;
		end;
		MakeUndoOperation (CreateUndoBeginEndBlock);
		VisibleRange.DoDiscardChanges;
		Selection.HideCaret;
		VisibleRange.DrawRange;
		Selection.UpdateCaretPos;
		Selection.ShowCaret;
		DontNotify := False;
		Self.Change;
		SelectionChange;
	end;
end;

function TMemoComponent.CharIdxToCell(CharIdx: Integer): TTextCell;
var
	LineIdx: Integer;
begin
	with FLineStarts do begin
		if TextLength > 0 then
			LineIdx := Count * CharIdx div TextLength - 1
		else
			LineIdx := 0;
		if LineIdx < 0 then
			LineIdx := 0;
		if LineIdx >= Count then
			LineIdx := Count - 1;
		while (LineIdx < Count - 1) and (Items [LineIdx] < CharIdx) do
			Inc (LineIdx);
		while (LineIdx > 0) and (Items [LineIdx] > CharIdx) do
			Dec (LineIdx);
		with Result do begin
			Row := LineIdx + 1;
			Col := CharIdx - Items [LineIdx] + 1;
		end;
	end;
end;

procedure TMemoComponent.Clear;
begin
	Text := '';
end;

procedure TMemoComponent.ClearRedo;
begin
	while CanRedo do
		GetLastRedo;
end;

procedure TMemoComponent.ClearSelection;
begin
	Perform (wm_Clear, 0, 0);
end;

procedure TMemoComponent.ClearUndo;
begin
	while CanRedo do
		GetLastRedo;
	while CanUndo do
		GetLastUndo;
	Change;
end;

procedure TMemoComponent.CMFontChanged(var Message: TMessage);
begin
	inherited;
	UpdateFontSize;
end;

procedure TMemoComponent.CMWantSpecialKey(var Message: TCMWantSpecialKey);
begin
	inherited;
	if not (csDesigning in ComponentState) then
		if Message.CharCode in [vk_Left, vk_Right, vk_Up, vk_Down, vk_Prior, vk_Next, vk_Home, vk_End, vk_Tab, vk_Clear, vk_Delete, vk_Insert] then
			Message.Result := 1;
end;

procedure TMemoComponent.CopyToClipboard;
begin
	Perform (wm_Copy, 0, 0);
end;

constructor TMemoComponent.Create(AOwner: TComponent);
begin
	inherited;
	FBitmapped := False;
	FText := '';
	FLineStarts := TIntegerList.Create;
	FLineStarts.Add (1);
	FLines := TMemoComponentStrings.Create;
	TMemoComponentStrings(FLines).Memo := Self;
	FTrackedRanges := TMCRanges.Create (Self);
	FWholeText := TWholeTextRange.Create (nil);
	FWholeText.Editor := Self;
	FVisibleRange := TVisibleRange.Create (TrackedRanges);
	FSelection := TSelectionRange.Create (TrackedRanges);
	FTabSize := 2;
	FScrollBars := ssBoth;
	FBorderStyle := bsSingle;
	FLeftMargin := 2;
	FTopMargin := 0;
	ControlStyle := ControlStyle + [csOpaque] - [csNoStdEvents];
	DoubleBuffered := False;
	Constraints.MinWidth := 64;
	Constraints.MinHeight := 64;
	TabStop := True;
	ParentColor := False;
	Color := clWindow;
	Font.Name := 'Courier New';
	Font.Size := 10;
	Width := 129;
	Height := 129;
  OnMouseWheel := MyMouseWheelHandler;
end;

procedure TMemoComponent.CreateParams(var Params: TCreateParams);
const
	ScrollBar: array [TScrollStyle] of DWORD = (0, WS_HSCROLL, WS_VSCROLL,
		WS_HSCROLL or WS_VSCROLL);
begin
	inherited;
	with Params do begin
		Style := Style or ScrollBar [FScrollBars];
		if NewStyleControls and Ctl3D and (FBorderStyle = bsSingle) then begin
			Style := Style and not WS_BORDER;
			ExStyle := ExStyle or WS_EX_CLIENTEDGE;
		end;
	end;
end;

function TMemoComponent.CreateSplitRanges(Range: TCustomRange): TFormattedRangeArray;
var
	RS,
	RE: Integer;
begin
	RS := Range.RStart;
	if (not Selection.Hidden) and (Selection.RLength > 0) then begin
		RE := Selection.RStart - 1;
		if RE > Range.REnd then
			RE := Range.REnd;
		if RE >= RS then begin
			SetLength (Result, Length (Result) + 1);
			Result [High (Result)] := pointer(TNormalFormattedRange.Create (nil));
			with TCustomFormattedRange(Result [High (Result)]) do begin
				FreeWhenDone := True;
				Editor := Self;
				RStart := RS;
				REnd := RE;
			end;
		end;
		RS := Selection.RStart;
		if RS < Range.RStart then
			RS := Range.RStart;
		RE := Selection.REnd;
		if RE > Range.REnd then
			RE := Range.REnd;
		if RE >= RS then begin
			SetLength (Result, Length (Result) + 1);
			Result [High (Result)] := pointer(TFormattedRange.Create (nil));
			with TCustomFormattedRange(Result [High (Result)]) do begin
				FreeWhenDone := True;
				Editor := Self;
				RStart := RS;
				REnd := RE;
				Color := clHighlight;
				Font.Assign (Self.Font);
				Font.Color := clHighlightText;
			end;
		end;
		RS := Selection.REnd + 1;
		if RS < Range.RStart then
			RS := Range.RStart;
	end;
	RE := Range.REnd;
	if RE >= RS then begin
		SetLength (Result, Length (Result) + 1);
		Result [High (Result)] := pointer(TNormalFormattedRange.Create (nil));
		with TCustomFormattedRange(Result [High (Result)]) do begin
			FreeWhenDone := True;
			Editor := Self;
			RStart := RS;
			REnd := RE;
		end;
	end;
end;

function TMemoComponent.CreateUndoBeginEndBlock: PUndoOperation;
begin
	New (Result);
	with Result^ do begin
		RStart := -1;
		REnd := -1;
		NewText := '';
	end;
end;

procedure TMemoComponent.CreateWnd;
begin
	inherited;
	UpdateFontSize;
	if HandleAllocated and not (csDesigning in ComponentState) then
		SetClassLong (Handle, gcl_HCursor, LoadCursor (0, idc_IBeam));
end;

procedure TMemoComponent.CutToClipboard;
begin
	Perform (wm_Cut, 0, 0);
end;

destructor TMemoComponent.Destroy;
begin
	FSelection.Free;
	FVisibleRange.Free;
	FLines.Free;
	FTrackedRanges.Free;
	FWholeText.Free;
	FLineStarts.Free;
	if Assigned (DrawBmp) then begin
		DrawBmp.Free;
		DrawBmp := nil;
	end;
	inherited;
end;

procedure TMemoComponent.DrawBorder(LeftRect, TopRect: TRect;
  Canvas: TCanvas);
begin
	Canvas.Brush.Color := Color;
	Canvas.FillRect (LeftRect);
	Canvas.FillRect (TopRect);
end;

procedure TMemoComponent.DrawTextLine(Range: TCustomRange; Left, Top: Integer; NextTabStop: Integer);
var
	I,
	SP,
	X,
	Y,
	TextFlags: Integer;
	R: TRect;
	Ranges: TFormattedRangeArray;
	S: string;
	Cnv: TCanvas;
begin
	if HandleAllocated and ((Range.RLength > 0) or (Range.REnd >= TextLength)) then begin
		if Bitmapped then begin
			Cnv := DrawBmp.Canvas;
			TextFlags := eto_Opaque or eto_Clipped;
		end else begin
			Cnv := Canvas;
			TextFlags := eto_Opaque or eto_Clipped;
		end;
		SetLength (Ranges, 0);
		Ranges := CreateSplitRanges (Range);
		R := Rect (Left, Top, Left, Top + FontHeight);
		for I := Low (Ranges) to High (Ranges) do
			with TCustomFormattedRange(Ranges [I]) do begin
				if RLength > 0 then begin
					Cnv.Brush.Color := Color;
					Cnv.Font.Assign (Font);
					if Self.Text [REnd] = #10 then
						S := Copy (Self.Text, RStart, RLength - 2)
					else if Self.Text [REnd] = #13 then
						S := Copy (Self.Text, RStart, RLength - 1)
					else
						S := Copy (Self.Text, RStart, RLength);
					SP := 1;
					while SP <= Length (S) do begin
						if S [SP] = #9 then begin
							System.Delete (S, SP, 1);
							System.Insert (StringOfChar (' ', NextTabStop), S, SP);
							Inc (SP, NextTabStop);
							NextTabStop := TabSize;
						end else begin
							Inc (SP);
							Dec (NextTabStop);
							if NextTabStop <= 0 then
								Inc (NextTabStop, TabSize);
						end;
					end;
					if (REnd <= TextLength) and (Self.Text [REnd] in [#10, #13]) then begin
						R.Right := ClientWidth;
					end else
						R.Right := R.Left + FontWidth * Length (S);
					X := R.Left;
					Y := R.Top;
					if R.Left < LeftMargin then
						R.Left := LeftMargin;
					if R.Right > R.Left then
						ExtTextOut (Cnv.Handle, X, Y, TextFlags, @R, PChar (S), Length (S), nil);
					R.Left := R.Right;
				end;
				if FreeWhenDone then
					Free;
			end;
		if Range.REnd >= TextLength then begin
			if R.Left < LeftMargin then
				R.Left := LeftMargin;
			R.Right := ClientWidth;
			Cnv.Brush.Color := Color;
			Cnv.FillRect (R);
		end;
	end;
end;

procedure TMemoComponent.EMCanUndo(var Message: TMessage);
begin
	if Message.WParam = 1 then
		Message.Result := Integer (Assigned (FRedoStack))
	else
		Message.Result := Integer (Assigned (FUndoStack));
end;

procedure TMemoComponent.EMUndo(var Message: TMessage);
var
	Op: TUndoOperation;
	NewOp: PUndoOperation;
	Repeating: Boolean;
	CurSel: TMCRange;
begin
	if Perform (em_CanUndo, Message.WParam, 0) <> 0 then
		with Message do begin
			FInUndo := True;
			Repeating := False;
			CurSel := nil;
			repeat
				if WParam = 1 then
					Op := GetLastRedo
				else
					Op := GetLastUndo;
				if IsUndoBeginEndBlock (@Op) then begin
					Repeating := not Repeating;
					if Repeating then begin
						DontNotify := True;
						VisibleRange.DoChanging;
					end else begin
						VisibleRange.DoDiscardChanges;
						Selection.HideCaret;
						VisibleRange.DrawRange;
						if Assigned (CurSel) then
							Selection.Assign (CurSel);
						Selection.UpdateCaretPos;
						Selection.ShowCaret;
						DontNotify := False;
						Self.Change;
						SelectionChange;
					end;
					if WParam = 1 then
						MakeUndoOperation (CreateUndoBeginEndBlock)
					else
						MakeRedoOperation (CreateUndoBeginEndBlock);
				end else begin
					with TMCRange.Create (nil) do begin
						Editor := Self;
						New (NewOp);
						RStart := Op.RStart;
						REnd := Op.REnd;
						NewOp.NewText := Text;
						Text := Op.NewText;
						NewOp.RStart := RStart;
						NewOp.REnd := REnd;
						if WParam = 1 then
							MakeUndoOperation (NewOp)
						else
							MakeRedoOperation (NewOp);
						if Repeating then begin
							if Assigned (CurSel) then begin
								if REnd + 1 > CurSel.RStart then
									CurSel.RStart := REnd + 1;
							end else begin
								CurSel := TMCRange.Create (TrackedRanges);
								CurSel.RStart := REnd + 1;
							end;
						end else
							AssignTo (Selection);
						Free;
					end;
				end;
			until not Repeating;
			if Assigned (CurSel) then
				CurSel.Free;
			FInUndo := False;
			Change;
		end;
end;

procedure TMemoComponent.FreeCaret;
begin
	if FCaretCreated then begin
		Selection.HideCaret;
		DestroyCaret;
		FCaretCreated := False;
	end;
end;

function TMemoComponent.GetCanRedo: Boolean;
begin
	Result := Perform (em_CanUndo, 1, 0) <> 0;
end;

function TMemoComponent.GetCanUndo: Boolean;
begin
	Result := Perform (em_CanUndo, 0, 0) <> 0;
end;

function TMemoComponent.GetLastRedo: TUndoOperation;
begin
	if Assigned (FRedoStack) then begin
		Result := FRedoStack^;
		Dispose (FRedoStack);
		FRedoStack := Result.NextItem;
	end;
end;

function TMemoComponent.GetLastUndo: TUndoOperation;
begin
	if Assigned (FUndoStack) then begin
		Result := FUndoStack^;
		Dispose (FUndoStack);
		FUndoStack := Result.NextItem;
	end;
end;

function TMemoComponent.GetLineCount: Integer;
begin
	Result := FLineStarts.Count;
end;

function TMemoComponent.GetLineLength(LineIndex: Integer): Integer;
begin
	Result := CellToCharIdx (TextCell (LineIndex + 1, 0)) - CellToCharIdx (TextCell (LineIndex, 0)) - 2;
end;

function TMemoComponent.GetSelLength: Integer;
begin
	Result := Selection.RLength;
end;

function TMemoComponent.GetSelStart: Integer;
begin
	Result := Selection.RStart - 1;
end;

function TMemoComponent.GetVisualLineLength(LineIndex: Integer): Integer;
begin
	Result := CellToScrCol (TextCell (LineIndex, GetLineLength (LineIndex) + 1)) - 1;
end;

function TMemoComponent.IsUndoBeginEndBlock(Op: PUndoOperation): Boolean;
begin
	Result := Op.RStart = -1;
end;

procedure TMemoComponent.KeyDown(var Key: Word; Shift: TShiftState);
var
	NewPos: Integer;
	Cell: TTextCell;
	SavScrCol: Integer;
	InWord: Boolean;
begin
	inherited;
	SavScrCol := -1;
	NewPos := Low (Integer);
	if ReadOnly then
		case Key of
			vk_Left: Perform (wm_HScroll, sb_LineLeft, 0);
			vk_Right: Perform (wm_HScroll, sb_LineRight, 0);
			vk_Up: Perform (wm_VScroll, sb_LineUp, 0);
			vk_Down: Perform (wm_VScroll, sb_LineDown, 0);
			vk_Prior: Perform (wm_VScroll, sb_PageUp, 0);
			vk_Next: Perform (wm_VScroll, sb_PageDown, 0);
			vk_Home: begin
				if ssCtrl in Shift then
					Perform (wm_VScroll, sb_Top, 0);
				Perform (wm_HScroll, sb_Top, 0);
			end;
			vk_End: begin
				if ssCtrl in Shift then
					Perform (wm_VScroll, sb_Bottom, 0)
				else
					Perform (wm_HScroll, sb_Bottom, 0);
			end;
			vk_Insert:
				if Shift = [ssCtrl] then
					CopyToClipboard;
		end
	else
		with Selection do begin
			case Key of
				vk_Clear: begin
					Clear;
					ScrollInView (4);
				end;
				vk_Delete: begin
					if Shift = [] then begin
						if RLength = 0 then begin
							DoChanging;
							RLength := 1;
							if (RLength = 1) and (Text [1] in [#10, #13]) then
								RLength := 2;
							DoDiscardChanges;
						end;
						Clear;
						ScrollInView (4);
					end else if Shift = [ssShift] then
						CutToClipboard;
				end;
				vk_Insert: begin
					if Shift = [ssShift] then
						PasteFromClipboard
					else if Shift = [ssCtrl] then
						CopyToClipboard;
				end;
				vk_Back:
					if Shift = [] then begin
						if RLength = 0 then begin
							DoChanging;
							RStart := RStart - 1;
							if (RLength = 1) and (Text [1] in [#10, #13]) then
								RStart := RStart - 1;
							DoDiscardChanges;
						end;
						Clear;
						ScrollInView (4);
					end else if Shift = [ssAlt] then
						Undo
					else if Shift = [ssAlt, ssShift] then
						Redo;
				vk_Left: begin
					if (not (ssShift in Shift)) and (RLength > 0) and (not AlwaysShowCaret) then
						REnd := RStart - 1
					else
						if (ssCtrl in Shift) then begin
							NewPos := CursorPos;
							InWord := (NewPos > 1) and (NewPos <= TextLength + 1) and (Self.Text [NewPos - 1] in ['A'..'Z', 'a'..'z', '0'..'9', '_']);
							while (NewPos > 1) and ((Self.Text [NewPos - 1] in ['A'..'Z', 'a'..'z', '0'..'9', '_']) = InWord) do begin
								if Self.Text [NewPos - 1] in [#10, #13] then
									Dec (NewPos, 2)
								else
									Dec (NewPos);
							end;
						end else begin
							if (CursorPos > 1) and (Self.Text [CursorPos - 1] in [#10, #13]) then
								NewPos := CursorPos - 2
							else
								NewPos := CursorPos - 1;
						end;
				end;
				vk_Right: begin
					if (not (ssShift in Shift)) and (RLength > 0) and (not AlwaysShowCaret) then
						RStart := REnd + 1
					else
						if (ssCtrl in Shift) then begin
							NewPos := CursorPos;
							InWord := (NewPos >= 1) and (NewPos <= TextLength) and (Self.Text [NewPos] in ['A'..'Z', 'a'..'z', '0'..'9', '_']);
							while (NewPos <= TextLength) and ((Self.Text [NewPos] in ['A'..'Z', 'a'..'z', '0'..'9', '_']) = InWord) do begin
								if Self.Text [NewPos] in [#10, #13] then
									Inc (NewPos, 2)
								else
									Inc (NewPos);
							end;
						end else begin
							if (CursorPos <= TextLength) and (Self.Text [CursorPos] in [#10, #13]) then
								NewPos := CursorPos + 2
							else
								NewPos := CursorPos + 1;
						end;
				end;
				vk_Up: begin
					if (not (ssShift in Shift)) and (RLength > 0) and (not AlwaysShowCaret) then
						REnd := RStart - 1
					else begin
						SavScrCol := ScrCol;
						Cell := CharIdxToCell (CursorPos);
						Dec (Cell.Row);
						Cell.Col := ScrColToCol (Cell.Row);
						NewPos := CellToCharIdx (Cell);
					end;
				end;
				vk_Down: begin
					if (not (ssShift in Shift)) and (RLength > 0) and (not AlwaysShowCaret) then
						RStart := REnd + 1
					else begin
						SavScrCol := ScrCol;
						Cell := CharIdxToCell (CursorPos);
						Inc (Cell.Row);
						Cell.Col := ScrColToCol (Cell.Row);
						NewPos := CellToCharIdx (Cell);
					end;
				end;
				vk_Prior: begin
					SavScrCol := ScrCol;
					Cell := CharIdxToCell (CursorPos);
					Dec (Cell.Row, PageHeight - 1);
					Cell.Col := ScrColToCol (Cell.Row);
					NewPos := CellToCharIdx (Cell);
				end;
				vk_Next: begin
					SavScrCol := ScrCol;
					Cell := CharIdxToCell (CursorPos);
					Inc (Cell.Row, PageHeight - 1);
					Cell.Col := ScrColToCol (Cell.Row);
					NewPos := CellToCharIdx (Cell);
				end;
				vk_Home: begin
					Cell := CharIdxToCell (CursorPos);
					if ssCtrl in Shift then
						Cell.Row := 1;
					Cell.Col := 1;
					NewPos := CellToCharIdx (Cell);
				end;
				vk_End: begin
					Cell := CharIdxToCell (CursorPos);
					if ssCtrl in Shift then
						Cell.Row := LineCount;
					Cell.Col := LineLength [Cell.Row] + 1;
					NewPos := CellToCharIdx (Cell);
				end;
			end;
			if NewPos <> Low (Integer) then begin
				if ssShift in Shift then
					CursorPos := NewPos
				else
					NoSelAtPos (NewPos);
				ScrCol := SavScrCol;
			end;
			ScrollInView (0);
		end;
	if Shift = [ssCtrl] then
		case UpCase (Char (Key)) of
			'X': if not ReadOnly then CutToClipboard;
			'C': CopyToClipboard;
			'V': if not ReadOnly then PasteFromClipboard;
		end;
	if (UpCase (Char (Key)) = 'Z') and (ssCtrl in Shift) then begin
		if ssShift in Shift then
			Redo
		else
			Undo;
	end;
end;

procedure TMemoComponent.KeyPress(var Key: Char);
begin
	inherited;
	if (Key >= #32) or (Key in [#9, #13]) then
		if not ReadOnly then
			with Selection do begin
				if Key = #13 then
					Text := #13#10
				else
					Text := Key;
				ScrollInView (4);
			end;
end;

procedure TMemoComponent.MakeRedoOperation(Op: PUndoOperation);
begin
	if Assigned (Op) then begin
		Op.NextItem := FRedoStack;
		FRedoStack := Op;
	end;
end;

procedure TMemoComponent.MakeUndoOperation(Op: PUndoOperation);
begin
	if Assigned (Op) then begin
		Op.NextItem := FUndoStack;
		FUndoStack := Op;
	end;
end;

procedure TMemoComponent.MouseDown(Button: TMouseButton;
	Shift: TShiftState; X, Y: Integer);
var
	Cell: TTextCell;
  BrkNum : integer;
begin
	try
		SetFocus;
	except end;
	if (Button = mbLeft) or (Button = mbRight) then begin //and (Selection.RLength <= 0) then begin
		Cell := ScrPointToScrCell (Point (X, Y));
    Inc (Cell.Row, VisibleRange.StartRowCol.Row - 1);
    Inc (Cell.Col, VisibleRange.LeftCol - 1);
    CellFromScrCol (Cell);

    if X < FLeftMargin then
    begin
      if Button = mbRight then
      begin
        // If there is a breakpoint in this line...
        BrkNum := FindBreakpoint(Cell.Row);
        if BrkNum <> -1 then
          if Assigned(FOnBreakpointClick) then
            FOnBreakpointClick(Self,BrkNum);
      end else
      begin
        ToggleBreakpoint(Cell.Row);
        Paint;
      end;
    end else
    begin
      inherited;

      if Button = mbLeft then
      begin
        with TMCRange.Create (nil) do begin
          Editor := Self;
          StartRowCol := Cell;
          Selection.NoSelAtPos (RStart);
          Free;
        end;
        FSelecting := True;
        if HandleAllocated then
          SetTimer (Handle, 1, 20, nil);
      end;
    end;
	end;
end;

procedure TMemoComponent.MouseMove(Shift: TShiftState; X, Y: Integer);
var
	Cell: TTextCell;
begin
	inherited;
	if FSelecting and (X >= 0) and (Y >= 0) and (X < ClientWidth) and (Y < ClientHeight) then begin
		Cell := ScrPointToScrCell (Point (X, Y));
		Inc (Cell.Row, VisibleRange.StartRowCol.Row - 1);
		Inc (Cell.Col, VisibleRange.LeftCol - 1);
		CellFromScrCol (Cell);
		with TMCRange.Create (nil) do begin
			Editor := Self;
			StartRowCol := Cell;
			Selection.CursorPos := RStart;
			Free;
		end;
	end;
end;

procedure TMemoComponent.MouseUp(Button: TMouseButton; Shift: TShiftState;
	X, Y: Integer);
begin
	inherited;
	if (Button = mbLeft) and FSelecting then begin
		if HandleAllocated then
			KillTimer (Handle, 1);
		FSelecting := False;
	end;
end;

procedure TMemoComponent.Paint;
begin
	inherited;
	Selection.HideCaret;
	if Bitmapped then begin
		UpdateDrawBmp;
		Canvas.Draw (0, 0, DrawBmp)
	end else begin
		DrawBorder (Rect (0, 0, LeftMargin, ClientHeight), Rect (0, 0, ClientWidth, TopMargin), Canvas);
		VisibleRange.DrawRange;
	end;
	Selection.ShowCaret;
end;

procedure TMemoComponent.PasteFromClipboard;
begin
	Perform (wm_Paste, 0, 0);
end;

procedure TMemoComponent.ReCreateCaret;
begin
	if FHasFocus and HandleAllocated then begin
		FreeCaret;
		CreateCaret (Handle, 0, 2, FontHeight - 2);
		FCaretCreated := True;
		with Selection do begin
			UpdateCaretPos;
			ShowCaret;
		end;
	end;
end;

procedure TMemoComponent.Redo;
begin
	Perform (em_Undo, 1, 0);
end;

procedure TMemoComponent.ReplaceText(Range: TCustomRange;
	const NewText: string);
var
	RS,
	I,
	L,
	LI,
	EI,
	LC,
	P,
	PP,
	BC,
	RE,
	LnCh: Integer;
	S: string;
	BlUndo,
	PUN,
	RMod: Boolean;
	Op: PUndoOperation;
begin
	PUN := False;
	LnCh := 0;
	with Selection do begin
		DoChanging;
		FOldSel.Free;
		FOldSel := nil;
	end;
	RS := Range.RStart;
	S := AdjustLineBreaks (NewText);
	L := Length (S);
	if not FInUndo then begin
		ClearRedo;
		BlUndo := False;
		Op := FUndoStack;
		if Assigned (Op) then begin
			if Range.RLength <= 0 then begin
				if (L > 0) and (Length (Op.NewText) <= 0) and (Op.REnd >= Op.RStart) then begin
					if Op.REnd + 1 = RS then begin
						Inc (Op.REnd, L);
						BlUndo := True;
					end;
				end;
			end else begin
				if (L <= 0) and (Length (Op.NewText) > 0) and (Op.REnd < Op.RStart) then begin
					if Op.RStart = RS then begin
						Op.NewText := Op.NewText + Range.Text;
						BlUndo := True;
					end else if Op.RStart = Range.REnd + 1 then begin
						Dec (Op.RStart, Range.RLength);
						Dec (Op.REnd, Range.RLength);
						Op.NewText := Range.Text + Op.NewText;
						BlUndo := True;
					end;
				end;
			end;
		end;
		if not BlUndo then begin
			New (Op);
			Op.RStart := RS;
			Op.REnd := RS + L - 1;
			Op.NewText := Range.Text;
			MakeUndoOperation (Op);
		end;
	end;
	LI := CharIdxToCell(Range.RStart).Row;
	EI := CharIdxToCell(Range.REnd+1).Row;
	LC := L - Range.RLength;
	if VisualLineLength [EI] >= FLongestLineLength then begin
		FLongestLineLength := 0;
		PUN := True;
	end;
	with FLineStarts do
		if (Range.RStart = 1) and (Range.REnd = TextLength) then begin
			LnCh := Count - 1;
			Clear;
			Add (1);
			FLongestLineLength := 0;
			PUN := True;
		end else
			for I := EI - 1 downto LI do begin
				if VisualLineLength [I + 1] >= FLongestLineLength then begin
					FLongestLineLength := 0;
					PUN := True;
				end;
				Delete (I);
				Dec (LnCh);
			end;
	Delete (FText, RS, Range.RLength);
	Insert (S, FText, RS);
	FTextLength := Length (FText);
	BC := 0;
	PP := 0;
	repeat
		P := Pos (#13#10, S);
		if P > 0 then begin
			FLineStarts.Insert (LI + BC, Range.RStart + P + 1 + PP);
			Inc (LnCh);
			Delete (S, 1, P + 1);
			Inc (PP, P + 1);
			Inc (BC);
		end;
	until P <= 0;
	with FLineStarts do begin
		for I := LI + BC to Count - 1 do
			Items [I] := Items [I] + LC;
		if FLongestLineLength <= 0 then begin
			LI := 0;
			BC := Count - 1;
		end;
		for I := LI to LI + BC do
			if (I >= 0) and (I < Count) then begin
				P := VisualLineLength [I + 1];
				if P > FLongestLineLength then begin
					FLongestLineLength := P;
					PUN := True;
				end;
			end;
	end;
	with TrackedRanges do
		for I := Count - 1 downto 0 do
			if (Items [I] <> Range) and (Items [I] <> VisibleRange) then
				with Items [I] do begin
					DoChanging;
					RMod := False;
					if LC > 0 then begin
						if (REnd >= Range.RStart) and (REnd <= Range.REnd) then begin
							if RStart > Range.RStart then
								RStart := Range.RStart;
							REnd := Range.RStart - 1;
							RMod := True;
						end else
							if REnd > Range.REnd then
								REnd := REnd + LC;
						if (RStart >= Range.RStart) and (RStart <= Range.REnd) then begin
							RStart := Range.RStart;
							RMod := True;
						end else
							if RStart > Range.REnd then
								RStart := RStart + LC;
					end else begin
						RE := REnd;
						if (RStart >= Range.RStart) and (RStart <= Range.REnd) then begin
							RStart := Range.RStart;
							RMod := True;
						end else
							if RStart > Range.REnd then
								RStart := RStart + LC;
						if (RE >= Range.RStart) and (RE <= Range.REnd) then begin
							if RStart > Range.RStart then
								RStart := Range.RStart;
							REnd := Range.RStart - 1;
							RMod := True;
						end else
							if RE > Range.REnd then
								REnd := RE + LC;
					end;
					DoChange;
					if RMod and (RLength <= 0) then
						NotifyOverwrite;
				end;
	if LnCh <> 0 then
		with VisibleRange do begin
			if LI < StartRowCol.Row then
				Inc (VisibleRange.FTopRow, LnCh);
		end;
	if Range is TSelectionRange then begin
		TSelectionRange(Range).NoSelAtPos (RS + L);
	end else
		Range.RLength := L;
	TextChangeNotification (RS, L - LC, L);
	if PUN or (LnCh <> 0) then
		UpdatePageSize
	else begin
		with TMCRange.Create (nil) do begin
			Editor := Self;
			RStart := RS - 1;
			EndRowCol := TextCell (LI + 1, 0);
			DrawRange;
			Free;
		end;
	end;
	TextChangeNotificationAfter;
	Selection.DoChange;
	Change;
end;

function TMemoComponent.ScrCellToScrPoint(Cell: TTextCell): TPoint;
begin
	with Cell do
		Result := Point ((Col - 1) * FontWidth + LeftMargin, (Row - 1) * FontHeight + TopMargin);
end;

procedure TMemoComponent.ScrollCaret;
begin
	Selection.ScrollInView (4);
end;

function TMemoComponent.ScrPointToScrCell(P: TPoint): TTextCell;
begin
	with P do
		Result := TextCell ((Y - TopMargin) div FontHeight + 1, (X - LeftMargin + FontWidth div 2) div FontWidth + 1);
end;

procedure TMemoComponent.SelectAll;
begin
	Selection.Assign (WholeText);
end;

procedure TMemoComponent.SelectionChange;
begin
	if not DontNotify then begin
		if Assigned (FOnSelectionChange) then
			FOnSelectionChange(Self);
	end;
end;

procedure TMemoComponent.SetAlwaysShowCaret(const Value: Boolean);
begin
	if FAlwaysShowCaret <> Value then begin
		FAlwaysShowCaret := Value;
		Selection.ShowCaret;
	end;
end;

procedure TMemoComponent.SetBitmapped(const Value: Boolean);
begin
	FBitmapped := Value;
	if (not Value) and Assigned (DrawBmp) then begin
		DrawBmp.Free;
		DrawBmp := nil;
	end;
end;

procedure TMemoComponent.SetBorderStyle(const Value: TBorderStyle);
begin
	if FBorderStyle <> Value then begin
		FBorderStyle := Value;
		RecreateWnd;
	end;
end;

procedure TMemoComponent.SetLeftMargin(const Value: Integer);
begin
	FLeftMargin := Value;
end;

procedure TMemoComponent.SetLines(const Value: TStrings);
begin
	FLines.Assign (Value);
end;

procedure TMemoComponent.SetReadOnly(const Value: Boolean);
begin
	if FReadOnly <> Value then begin
		FReadOnly := Value;
		Selection.ShowCaret;
	end;
end;

procedure TMemoComponent.SetScrollBars(const Value: TScrollStyle);
begin
	if FScrollBars <> Value then begin
		FScrollBars := Value;
		RecreateWnd;
	end;
end;

procedure TMemoComponent.SetSelLength(const Value: Integer);
begin
	Selection.RLength := Value;
end;

procedure TMemoComponent.SetSelStart(const Value: Integer);
begin
	Selection.NoSelAtPos (Value + 1);
end;

procedure TMemoComponent.SetTabSize(const Value: Integer);
var
	I: Integer;
begin
	if FTabSize <> Value then begin
		FTabSize := Value;
		if FTabSize < 1 then
			FTabSize := 1;
		Selection.DoChanging;
		FLongestLineLength := 0;
		for I := 0 to LineCount - 1 do
			if FLongestLineLength < VisualLineLength [I] then
				FLongestLineLength := VisualLineLength [I];
		VisibleRange.Update;
		Selection.DoChange;
	end;
end;

procedure TMemoComponent.SetText(const Value: TCaption);
begin
	WholeText.Text := Value;
end;

procedure TMemoComponent.SetTopMargin(const Value: Integer);
begin
	FTopMargin := Value;
end;

function TMemoComponent.TabSpacesAtPos(P: Integer): Integer;
var
	I: Integer;
	RS: TTextCell;
	Ps: Integer;
begin
	if TabSize <= 1 then
		Result := TabSize
	else begin
		RS := CharIdxToCell (P);
		RS.Col := 1;
		Ps := 0;
		for I := CellToCharIdx (RS) to P - 1 do begin
			if Text [I] = #9 then
				Ps := (Ps div TabSize + 1) * TabSize
			else
				Inc (Ps);
		end;
		Result := TabSize - Ps mod TabSize;
	end;
end;

procedure TMemoComponent.TextChangeNotification(StartPos, OldLength,
  NewLength: Integer);
begin
end;

procedure TMemoComponent.TextChangeNotificationAfter;
begin
end;

procedure TMemoComponent.Undo;
begin
	Perform (em_Undo, 0, 0);
end;

procedure TMemoComponent.UpdateDrawBmp;
begin
	if Bitmapped then begin
		if not Assigned (DrawBmp) then
			DrawBmp := TBitmap.Create;
		if (DrawBmp.Width <> ClientWidth) or (DrawBmp.Height <> ClientHeight) then begin
			DrawBmp.Width := ClientWidth;
			DrawBmp.Height := ClientHeight;
			DrawBorder (Rect (0, 0, LeftMargin, ClientHeight), Rect (0, 0, ClientWidth, TopMargin), DrawBmp.Canvas);
		end;
	end else
		if Assigned (DrawBmp) then begin
			DrawBmp.Free;
			DrawBmp := nil;
		end;
end;

procedure TMemoComponent.UpdateFontSize;
begin
	if HandleAllocated and Assigned (Parent) then begin
		Canvas.Font.Assign (Font);
		FontHeight := Canvas.TextHeight ('Q');
		FontWidth := Canvas.TextWidth ('M');
		UpdatePageSize;
	end;
end;

procedure TMemoComponent.UpdatePageSize;
var
	ScrollInfo: TScrollInfo;
begin
	if HandleAllocated and Assigned (Parent) then begin
		UpdateDrawBmp;
		if FontHeight <= 0 then
			FontHeight := 13;
		if FontWidth <= 0 then
			FontWidth := 8;
		if HandleAllocated and Assigned (Parent) then begin
			PageHeight := (ClientHeight - TopMargin) div FontHeight;
			PageWidth := (ClientWidth - LeftMargin) div FontWidth;
		end else begin
			PageHeight := 1;
			PageWidth := 1;
		end;
		if PageHeight < 1 then
			PageHeight := 1;
		if PageWidth < 1 then
			PageWidth := 1;
		VisibleRange.Update;
		if HandleAllocated then begin
			Selection.HideCaret;
			VisibleRange.DrawRange;
			Selection.UpdateCaretPos;
			Selection.ShowCaret;
			with ScrollInfo do begin
				cbSize := SizeOf (ScrollInfo);
				fMask := sif_All or sif_DisableNoScroll;
				nMin := 1;
				nMax := LineCount;
				nPos := VisibleRange.StartRowCol.Row;
				nPage := PageHeight;
			end;
			SetScrollInfo (Handle, sb_Vert, ScrollInfo, True);
			with ScrollInfo do begin
				nMin := 1;
				nMax := LongestLineLength;
				nPos := VisibleRange.LeftCol;
				nPage := PageWidth;
			end;
			SetScrollInfo (Handle, sb_Horz, ScrollInfo, True);
		end;
	end;
end;

procedure TMemoComponent.WMClear(var Message: TWMClear);
begin
	inherited;
	Selection.Clear;
end;

procedure TMemoComponent.WMCopy(var Message: TWMCopy);
begin
	inherited;
	if Selection.RLength > 0 then
		Clipboard.AsText := Selection.Text;
end;

procedure TMemoComponent.WMCut(var Message: TWMCut);
begin
	inherited;
	if Selection.RLength > 0 then begin
		Clipboard.AsText := Selection.Text;
		Selection.Clear;
	end;
end;

procedure TMemoComponent.WMEraseBkgnd(var Message: TWMEraseBkgnd);
begin
	Message.Result := 1;
end;

procedure TMemoComponent.WMGetText(var Message: TWMGetText);
begin
	StrPLCopy (Message.Text, Text, Message.TextMax);
	Message.Result := StrLen (Message.Text);
end;

procedure TMemoComponent.WMGetTextLength(var Message: TWMGetTextLength);
begin
	Message.Result := TextLength;
end;

procedure TMemoComponent.WMHScroll(var Message: TWMHScroll);
var
	ScrollPos: Integer;
	OldPos: Integer;
begin
	VisibleRange.DoChanging;
	inherited;
	OldPos := VisibleRange.LeftCol;
	ScrollPos := OldPos;
	with Message do begin
		if ScrollCode in [sb_ThumbTrack, sb_ThumbPosition] then
			ScrollPos := Pos
		else begin
			case ScrollCode of
				sb_Top: ScrollPos := 1;
				sb_Bottom: ScrollPos := LongestLineLength - PageWidth + 1;
				sb_LineLeft: ScrollPos := OldPos - 1;
				sb_LineRight: ScrollPos := OldPos + 1;
				sb_PageLeft: ScrollPos := OldPos - PageWidth;
				sb_PageRight: ScrollPos := OldPos + PageWidth;
			end;
		end;
		Result := 0;
	end;
	if ScrollPos > LongestLineLength - PageWidth + 1 then
		ScrollPos := LongestLineLength - PageWidth + 1;
	if ScrollPos < 1 then
		ScrollPos := 1;
	if ScrollPos <> OldPos then begin
		SetScrollPos (Handle, sb_Horz, ScrollPos, True);
		VisibleRange.FLeftCol := ScrollPos;
		VisibleRange.DoChange;
		Update;
	end else
		VisibleRange.DoDiscardChanges;
end;

procedure TMemoComponent.WMKillFocus(var Message: TWMKillFocus);
begin
	inherited;
	FHasFocus := False;
	FreeCaret;
end;

procedure TMemoComponent.WMPaste(var Message: TWMPaste);
begin
	inherited;
	Selection.Text := Clipboard.AsText;
end;

procedure TMemoComponent.WMSetFocus(var Message: TWMSetFocus);
begin
	inherited;
	FHasFocus := True;
	ReCreateCaret;
end;

procedure TMemoComponent.WMSetText(var Message: TWMSetText);
begin
	Text := StrPas (Message.Text);
	Message.Result := 1;
end;

procedure TMemoComponent.WMSize(var Message: TWMSize);
begin
	inherited;
	UpdatePageSize;
end;

procedure TMemoComponent.WMTimer(var Message: TWMTimer);
var
	P: TPoint;
	Cell: TTextCell;
begin
	inherited;
	if (Message.TimerID = 1) and FSelecting then begin
		P := ScreenToClient (Mouse.CursorPos);
		Cell := ScrPointToScrCell (P);
		Inc (Cell.Row, VisibleRange.StartRowCol.Row - 1);
		Inc (Cell.Col, VisibleRange.LeftCol - 1);
		CellFromScrCol (Cell);
		with TMCRange.Create (nil) do begin
			Editor := Self;
			StartRowCol := Cell;
			Selection.CursorPos := RStart;
			Selection.ScrollInView (0);
			Free;
		end;
	end;
end;

procedure TMemoComponent.WMVScroll(var Message: TWMVScroll);
var
	ScrollPos: Integer;
	OldPos: Integer;
begin
	VisibleRange.DoChanging;
	inherited;
	OldPos := GetScrollPos (Handle, sb_Vert);
	ScrollPos := OldPos;
	with Message do begin
		if ScrollCode in [sb_ThumbTrack, sb_ThumbPosition] then
			ScrollPos := Pos
		else begin
			case ScrollCode of
				sb_Top: ScrollPos := 1;
				sb_Bottom: ScrollPos := LineCount - PageHeight + 1;
				sb_LineUp: ScrollPos := OldPos - 1;
				sb_LineDown: ScrollPos := OldPos + 1;
				sb_PageUp: ScrollPos := OldPos - PageHeight;
				sb_PageDown: ScrollPos := OldPos + PageHeight;
			end;
		end;
		Result := 0;
	end;
	if ScrollPos > LineCount - PageHeight + 1 then
		ScrollPos := LineCount - PageHeight + 1;
	if ScrollPos < 1 then
		ScrollPos := 1;
	if ScrollPos <> OldPos then begin
		SetScrollPos (Handle, sb_Vert, ScrollPos, True);
		VisibleRange.FTopRow := ScrollPos;
		VisibleRange.DoChange;
		Update;
	end else
		VisibleRange.DoDiscardChanges;
end;

function TMemoComponent.GetBreakpointByHandle(BrkHandle: integer): TBreakpoint;
var
  i : integer;
  EnableCounter : integer;
begin
  EnableCounter := 0;

  for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
    if FBreakpoints[i].Enabled then
    begin
      if EnableCounter = BrkHandle then
      begin
        Result := FBreakpoints[i];
        Exit;
      end;

      Inc(EnableCounter);
    end;
end;

function TMemoComponent.GetBreakpointsNum: integer;
var
  i : integer;
begin
  Result := 0;

  for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
    if FBreakpoints[i].Enabled then
      Inc(Result);
end;

function TMemoComponent.FindBreakpoint(LineNum: integer): integer;
var
  i : integer;
begin
  Result := -1;

  for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
    if FBreakpoints[i].LineNum = LineNum then
    begin
      Result := i;
      break;
    end;
end;

function TMemoComponent.GetBreakpointByNumber(BrkNum : integer): TBreakpoint;
begin
  Result := FBreakpoints[BrkNum];
end;

procedure TMemoComponent.RemoveBreakpointByNumber(BrkNum: integer);
begin
  FBreakpoints[BrkNum].Enabled := false;
  Paint;
end;

procedure TMemoComponent.SetBreakpointByNumber(BrkNum: integer; var Brk: TBreakpoint);
begin
  FBreakpoints[BrkNum] := Brk;
end;

procedure TMemoComponent.RemoveAllBreakpoints;
var
  i : integer;
begin
  for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
    FBreakpoints[i].Enabled := false; 
end;

procedure TMemoComponent.MyMouseWheelHandler(Sender: TObject;Shift: TShiftState; WheelDelta: Integer;
                                           MousePos: TPoint; var Handled: Boolean);
begin
  if WheelDelta > 0 then
    Perform (wm_VScroll, sb_LineUp, 0)
  else
    Perform (wm_VScroll, sb_LineDown, 0);
end;

procedure TMemoComponent.GotoPos(X, Y: integer);
var
  Cell : TTextCell;
begin
  with Selection do
  begin
    Cell.Row := Y;
    Cell.Col := X;
    ScrCol := X;
    CursorPos := CellToCharIdx(Cell);

    NoSelAtPos(CursorPos);
    ScrollInView (4);
  end;
end;

{ TIntegerList }

function TIntegerList.Add(Item: Integer): Integer;
begin
	Result := FList.Add (Pointer (Item));
end;

procedure TIntegerList.Clear;
begin
	FList.Clear;
end;

constructor TIntegerList.Create;
begin
	inherited;
	FList := TList.Create;
end;

procedure TIntegerList.Delete(Index: Integer);
begin
	FList.Delete (Index);
end;

destructor TIntegerList.Destroy;
begin
	FList.Free;
	inherited;
end;

function TIntegerList.GetCount: Integer;
begin
	Result := FList.Count;
end;

function TIntegerList.GetItem(ItemIndex: Integer): Integer;
begin
	Result := Integer (FList.Items [ItemIndex]);
end;

procedure TIntegerList.Insert(Index, Item: Integer);
begin
	FList.Insert (Index, Pointer (Item));
end;

procedure TIntegerList.SetCount(const Value: Integer);
begin
	FList.Count := Value;
end;

procedure TIntegerList.SetItem(ItemIndex: Integer; const Value: Integer);
begin
	FList.Items [ItemIndex] := Pointer (Value);
end;

{ TMemoComponentStrings }

procedure TMemoComponentStrings.Clear;
begin
	Memo.Clear;
end;

procedure TMemoComponentStrings.Delete(LineIndex: Integer);
var
	Range: TCustomRange;
begin
	Range := TMCRange.Create (nil);
	with Range do begin
		Editor := Memo;
		if LineIndex < Memo.LineCount - 1 then begin
			StartRowCol := TextCell (LineIndex + 1, 1);
			EndRowCol := TextCell (LineIndex + 2, 0);
		end else begin
			StartRowCol := TextCell (LineIndex + 1, -1);
			EndRowCol := TextCell (LineIndex + 2, -2);
		end;
		Clear;
		Free;
	end;
end;

function TMemoComponentStrings.Get(LineIndex: Integer): string;
var
	Range: TCustomRange;
begin
	Range := TMCRange.Create (nil);
	with Range do begin
		Editor := Memo;
		StartRowCol := TextCell (LineIndex + 1, 1);
		EndRowCol := TextCell (LineIndex + 2, -2);
		Result := Text;
		Free;
	end;
end;

function TMemoComponentStrings.GetCount: Integer;
begin
	if Memo.TextLength > 0 then
		Result := Memo.LineCount
	else
		Result := 0;
end;

function TMemoComponentStrings.GetTextStr: string;
begin
	Result := Memo.Text;
end;

procedure TMemoComponentStrings.Insert(LineIndex: Integer; const S: string);
var
	Range: TCustomRange;
begin
	if Memo.TextLength > 0 then begin
		Range := TMCRange.Create (nil);
		with Range do begin
			Editor := Memo;
			if LineIndex < Memo.LineCount then begin
				StartRowCol := TextCell (LineIndex + 1, 1);
				RLength := 0;
				Text := S + #13#10;
			end else begin
				RStart := Memo.TextLength + 1;
				RLength := 0;
				Text := #13#10 + S;
			end;
			Free;
		end;
	end else
		Memo.Text := S;
end;

procedure TMemoComponentStrings.Put(LineIndex: Integer; const S: string);
var
	Range: TCustomRange;
begin
	Range := TMCRange.Create (nil);
	with Range do begin
		Editor := Memo;
		StartRowCol := TextCell (LineIndex + 1, 1);
		EndRowCol := TextCell (LineIndex + 2, -2);
		Text := S;
		Free;
	end;
end;

procedure TMemoComponentStrings.SetTextStr(const Value: string);
begin
	Memo.Text := Value;
end;

{ TMCRanges }

function TMCRanges.Add(Start, Count: Integer): TCustomRange;
begin
	Result := Add;
	with Result do begin
		RStart := Start;
		RLength := Count;
	end;
end;

function TMCRanges.Add: TCustomRange;
begin
	Result := FItemClass.Create(Self);
end;

constructor TMCRanges.Create(AOwner: TMemoComponent);
begin
	inherited Create (AOwner, TCustomRange);
	FItemClass := TMCRange;
end;

function TMCRanges.NewGetItem(ItemIndex: Integer): TCustomRange;
begin
	Result := TCustomRange (GetItem (ItemIndex));
end;

function TMCRanges.NewGetOwner: TMemoComponent;
begin
	Result := TMemoComponent (GetOwner);
end;

procedure TMCRanges.NewSetItem(ItemIndex: Integer;
	const Value: TCustomRange);
begin
	SetItem (ItemIndex, Value);
end;

{ TCustomRange }

procedure TCustomRange.SetText(const Value: string);
begin
	if Assigned (Editor) then
		Editor.ReplaceText (Self, Value);
end;

function TCustomRange.GetText: string;
begin
	if Assigned (Editor) then
		Result := Copy (Editor.Text, RStart, RLength)
	else
		Result := '';
end;

function TCustomRange.GetEndRowCol: TTextCell;
begin
	if Assigned (Editor) then
		Result := Editor.CharIdxToCell (REnd)
	else
		Result := TextCell (1, 0);
end;

function TCustomRange.GetStartRowCol: TTextCell;
begin
	if Assigned (Editor) then
		Result := Editor.CharIdxToCell (RStart)
	else
		Result := TextCell (1, 1);
end;

procedure TCustomRange.SetEndRowCol(const Value: TTextCell);
begin
	if Assigned (Editor) then
		REnd := Editor.CellToCharIdx (Value);
end;

procedure TCustomRange.SetStartRowCol(const Value: TTextCell);
begin
	if Assigned (Editor) then
		RStart := Editor.CellToCharIdx (Value);
end;

function TCustomRange.GetRLength: Integer;
begin
	Result := REnd - RStart + 1;
end;

procedure TCustomRange.SetRLength(const Value: Integer);
begin
	REnd := RStart + Value - 1;
end;

procedure TCustomRange.SetREnd(const Value: Integer);
begin
end;

procedure TCustomRange.SetRStart(const Value: Integer);
begin
end;

procedure TCustomRange.NotifyOverwrite;
begin
	if Assigned (FOnOverwrite) then
		FOnOverwrite (Self);
end;

procedure TCustomRange.Clear;
begin
	if RLength > 0 then
		Text := '';
end;

function TCustomRange.CharInRange(CharIdx: Integer): Boolean;
begin
	Result := (CharIdx >= RStart) and (CharIdx <= REnd);
end;

procedure TCustomRange.DrawRange;
var
	Part: TMCRange;
	I,
	LC,
	TR,
	RowS,
	RowE,
	ColS,
	ColE,
	ScrCol: Integer;
	AfterText: Boolean;
	SC,
	EC,
	Cell: TTextCell;
	Cnv: TCanvas;
  BreakpointY : integer;
begin
	if Assigned (Editor) and Editor.HandleAllocated and Assigned (Editor.Parent) and (Editor.VisibleRange.FChanging = 0) and ((RLength > 0) or (Self is TVisibleRange) or (Self is TWholeTextRange) or (RStart >= Editor.TextLength)) then begin
		if Editor.Bitmapped then begin
			Editor.UpdateDrawBmp;
			Cnv := Editor.DrawBmp.Canvas;
		end else begin
			Cnv := Editor.Canvas;
			if (RStart >= Editor.Selection.CursorPos - 1) and (REnd <= Editor.Selection.CursorPos) then
				Editor.Selection.HideCaret;
		end;
		if Editor.TextLength <= 0 then begin
			Cnv.Brush.Color := Editor.Color;
			Cnv.FillRect (Editor.ClientRect)
		end else begin
			LC := Editor.VisibleRange.LeftCol;
			TR := Editor.VisibleRange.StartRowCol.Row;
			SC := StartRowCol;
			EC := EndRowCol;

      // Draw breakpoints border
      Cnv.Brush.Color := clBtnFace;
      Cnv.FillRect(Rect(0,0,Editor.LeftMargin,Editor.Height));

      // Set breakpoints color
      Cnv.Brush.Color := clRed;

      // Draw breakpoints
      for i := 0 to MAX_BREAKPOINTS_NUM - 1 do
        if Editor.FBreakpoints[i].Enabled then
        begin
          BreakpointY := Editor.TopMargin + (Editor.FBreakpoints[i].LineNum - TR) * Editor.FontHeight + 3;
          Cnv.Ellipse(4,BreakpointY,14,BreakpointY + 10);
        end;

			Part := TMCRange.Create (nil);
			Part.Editor := Editor;
			RowS := SC.Row;
			if RowS < TR then
				RowS := TR;
			RowE := EC.Row;
			if REnd >= Editor.TextLength then
				Inc (RowE);
			I := TR + Editor.PageHeight + 1;
			if Editor.VisibleRange.REnd >= Editor.TextLength then
				Inc (I);
			if RowE > I then
				RowE := I;
			for I := RowS to RowE do begin
				if I = SC.Row then begin
					ColS := SC.Col;
					Cell := TextCell (I, Editor.CellToScrCol (SC))
				end else begin
					ColS := 1;
					Cell := TextCell (I, 1);
				end;
				if Cell.Col < Editor.VisibleRange.LeftCol then
					Cell.Col := Editor.VisibleRange.LeftCol;
				ScrCol := Editor.CellFromScrColToScrCol (Cell);
				if I = EC.Row then
					ColE := EC.Col
				else
					ColE := Editor.LineLength [I] + 2;
				if ColE > Editor.VisibleRange.RightCol then
					ColE := Editor.VisibleRange.RightCol;
				AfterText := Editor.CellToCharIdx (TextCell (I, ColS - 1)) > Editor.TextLength;
				if AfterText then begin
					Part.StartRowCol := TextCell (I + 1, -1);
					if Editor.Selection.CharInRange (Part.RStart) then
						Cnv.Brush.Color := clHighlight
					else begin
						Cnv.Brush.Color := Editor.Color;
						if (Editor.Selection.RStart >= Part.RStart) and (Editor.Selection.RStart <= Part.RStart + 2) then
							Editor.Selection.HideCaret;
					end;
					Cnv.FillRect (Rect (Editor.LeftMargin, Editor.TopMargin + (I - TR) * Editor.FontHeight, Editor.ClientWidth, Editor.TopMargin + (I - TR + 1) * Editor.FontHeight));
				end else begin
					if ColE >= ColS then begin
						Part.StartRowCol := Cell;
						Part.EndRowCol := TextCell (I, ColE);
						Editor.DrawTextLine (Part, Editor.LeftMargin + (ScrCol - LC) * Editor.FontWidth, Editor.TopMargin + (I - TR) * Editor.FontHeight, Editor.TabSize - (ScrCol - 1) mod Editor.TabSize);
					end;
				end;
				if (I >= Editor.LineCount) or AfterText then begin
					if Editor.Selection.RStart >= Editor.TextLength then
						Editor.Selection.HideCaret;
					Cnv.Brush.Color := Editor.Color;
					Cnv.FillRect (Rect (Editor.LeftMargin, Editor.TopMargin + (Editor.LineCount - TR + 1) * Editor.FontHeight, Editor.ClientWidth, Editor.ClientHeight));
				end;
			end;
			Part.Free;
		end;
		if Editor.Bitmapped then
			Editor.Invalidate
		else
			Editor.Selection.ShowCaret;
	end;
end;

constructor TCustomRange.Create(Collection: TCollection);
begin
	inherited;
	if Collection is TMCRanges then
		Editor := TMCRanges(Collection).Owner;
end;

procedure TCustomRange.Change;
begin
	if Assigned (FOnChange) then
		FOnChange (Self);
end;

function TCustomRange.GetEndPoint: TPoint;
var
	Cell: TTextCell;
	I: Integer;
	Ps: Integer;
begin
	if Assigned (Editor) then begin
		Cell := EndRowCol;
		with Cell do begin
			if (REnd > 0) and (REnd <= Editor.TextLength) and (Editor.Text [REnd] in [#10, #13]) then begin
				Inc (Row);
				Col := 0;
			end;
			if Editor.TabSize = 1 then
				Ps := Col
			else begin
				Ps := 0;
				for I := Editor.CellToCharIdx (TextCell (Row, 1)) to REnd do begin
					if (I > 0) and (I <= Editor.TextLength) and (Editor.Text [I] = #9) then
						Ps := (Ps div Editor.TabSize + 1) * Editor.TabSize
					else
						Inc (Ps);
				end;
			end;
			with Editor.VisibleRange do
				Result := Editor.ScrCellToScrPoint (TextCell (Row - StartRowCol.Row + 1, Ps - LeftCol + 2))
		end;
	end else
		Result := Point (0, 0);
end;

function TCustomRange.GetStartPoint: TPoint;
var
	Cell: TTextCell;
	I: Integer;
	Ps: Integer;
begin
	if Assigned (Editor) then begin
		Cell := StartRowCol;
		with Cell do begin
			if Editor.TabSize = 1 then
				Ps := Col - 1
			else begin
				Ps := 0;
				for I := Editor.CellToCharIdx (TextCell (Row, 1)) to RStart - 1 do begin
					if (I > 0) and (I <= Editor.TextLength) and (Editor.Text [I] = #9) then
						Ps := (Ps div Editor.TabSize + 1) * Editor.TabSize
					else
						Inc (Ps);
				end;
			end;
			with Editor.VisibleRange do
				Result := Editor.ScrCellToScrPoint (TextCell (Row - StartRowCol.Row + 1, Ps - LeftCol + 2))
		end;
	end else
		Result := Point (0, 0);
end;

procedure TCustomRange.Changing;
begin
end;

procedure TCustomRange.DoChange;
begin
	Dec (FChanging);
	if FChanging = 0 then
		Change;
end;

procedure TCustomRange.DoChanging;
begin
	if FChanging = 0 then
		Changing;
	Inc (FChanging);
end;

procedure TCustomRange.DiscardChanges;
begin
end;

procedure TCustomRange.DoDiscardChanges;
begin
	Dec (FChanging);
	if FChanging = 0 then
		DiscardChanges;
end;

procedure TCustomRange.ScrollInView(FromBorder: Integer);
var
	Cell: TTextCell;
	LC,
	Tolerance: Integer;
	U: Boolean;
begin
	if Assigned (Editor) then begin
		if FromBorder < MaxScrollTolerance then
			Tolerance := FromBorder
		else
			Tolerance := MaxScrollTolerance;
		if (Self is TSelectionRange) and TSelectionRange(Self).Backwards then
			Cell := StartRowCol
		else
			Cell := Editor.CharIdxToCell (REnd + 1);
		Cell.Col := Editor.CellToScrCol (Cell);
		with Editor.VisibleRange do begin
			DoChanging;
			U := True;
			if StartRowCol.Row > Cell.Row - Tolerance then
				StartRowCol := TextCell (Cell.Row - FromBorder, 1)
			else if StartRowCol.Row < Cell.Row - Editor.PageHeight + 1 then
				StartRowCol := TextCell (Cell.Row - Editor.PageHeight + 1 + FromBorder, 1)
			else
				U := False;
			if LeftCol > Cell.Col - Tolerance then begin
				LC := Cell.Col - FromBorder;
				if LC < 1 then
					LC := 1;
				if LeftCol <> LC then begin
					LeftCol := LC;
					U := True;
				end;
			end else if LeftCol < Cell.Col - Editor.PageWidth then begin
				LC := Cell.Col - Editor.PageWidth + FromBorder;
				if LC < 1 then
					LC := 1;
				if LeftCol <> LC then begin
					LeftCol := LC;
					U := True;
				end;
			end;
			if U then
				DoChange
			else
				DoDiscardChanges;
		end;
	end;
end;

procedure TCustomRange.AssignTo(Dest: TPersistent);
begin
	if Dest is TCustomRange then begin
		with Dest as TCustomRange do begin
			DoChanging;
			RStart := Self.RStart;
			REnd := Self.REnd;
			DoChange;
		end;
		Exit;
	end;
	inherited;
end;

{ TMCRange }

constructor TMCRange.Create(Collection: TCollection);
begin
	inherited;
	FRStart := 1;
end;

function TMCRange.GetREnd: Integer;
begin
	Result := FREnd;
end;

function TMCRange.GetRStart: Integer;
begin
	Result := FRStart;
end;

procedure TMCRange.SetREnd(const Value: Integer);
begin
	DoChanging;
	FREnd := Value;
	if FREnd < RStart - 1 then
		FREnd := RStart - 1;
	if Assigned (Editor) and (FREnd > Editor.TextLength) then
		FREnd := Editor.TextLength;
	DoChange;
end;

procedure TMCRange.SetRStart(const Value: Integer);
begin
	DoChanging;
	FRStart := Value;
	if FRStart < 1 then
		FRStart := 1;
	if Assigned (Editor) and (FRStart > Editor.TextLength + 1) then
		FRStart := Editor.TextLength + 1;
	REnd := REnd;
	DoChange;
end;

{ TWholeTextRange }

function TWholeTextRange.GetREnd: Integer;
begin
	if Assigned (Editor) then
		Result := Editor.TextLength
	else
		Result := 0;
end;

function TWholeTextRange.GetRStart: Integer;
begin
	Result := 1;
end;

{ TVisibleRange }

procedure TVisibleRange.Change;
function DivDown(Div1, Div2: Integer): Integer;
begin
	Result := Div1 div Div2;
end;
function DivUp(Div1, Div2: Integer): Integer;
begin
	Result := (Div1 - 1) div Div2 + 1;
end;
var
	NewTextRect,
	EditorRect,
	UpdateRect: TRect;
begin
	inherited;
	if Assigned (Editor) and Editor.HandleAllocated then begin
		NewTextRect := Rect (FLeftCol, FTopRow, FLeftCol + Editor.PageWidth + 1, FTopRow + Editor.PageHeight + 1);
		if not EqualRect (VisibleTextRect, NewTextRect) then
			with Editor.Selection do begin
				HideCaret;
				if (NewTextRect.Left <> VisibleTextRect.Left) or (NewTextRect.Top >= VisibleTextRect.Bottom - 1) or (VisibleTextRect.Top >= NewTextRect.Bottom - 1) then
					Self.DrawRange
				else with Editor do begin
					EditorRect := ClientRect;
					Inc (EditorRect.Left, Editor.LeftMargin);
					Inc (EditorRect.Top, Editor.TopMargin);
					if Bitmapped and Assigned (DrawBmp) then begin
						if VisibleTextRect.Top > NewTextRect.Top then begin
							BitBlt (DrawBmp.Canvas.Handle, LeftMargin, TopMargin, DrawBmp.Width - LeftMargin, DrawBmp.Height - TopMargin - (NewTextRect.Top - VisibleTextRect.Top) * FontHeight, DrawBmp.Canvas.Handle, LeftMargin, TopMargin + (NewTextRect.Top - VisibleTextRect.Top) * FontHeight, SrcCopy);
							UpdateRect := Rect (LeftMargin, TopMargin, ClientWidth, TopMargin + (VisibleTextRect.Top - NewTextRect.Top) * FontHeight);
						end else begin
							BitBlt (DrawBmp.Canvas.Handle, LeftMargin, TopMargin + (VisibleTextRect.Top - NewTextRect.Top) * FontHeight, DrawBmp.Width - LeftMargin, DrawBmp.Height - TopMargin - (VisibleTextRect.Top - NewTextRect.Top) * FontHeight, DrawBmp.Canvas.Handle, LeftMargin, TopMargin, SrcCopy);
							UpdateRect := Rect (LeftMargin, ClientHeight - (NewTextRect.Top - VisibleTextRect.Top) * FontHeight, ClientWidth, ClientHeight);
						end;
					end else
						ScrollWindowEx (Handle, 0, (VisibleTextRect.Top - NewTextRect.Top) * FontHeight, @EditorRect, @EditorRect, 0, @UpdateRect, 0);
					if not IsRectEmpty (UpdateRect) then
						with TMCRange.Create (nil) do begin
							Editor := Self.Editor;
							StartRowCol := TextCell (DivDown (UpdateRect.Top, FontHeight) + FTopRow, 1);
							EndRowCol := TextCell (DivUp (UpdateRect.Bottom, FontHeight) + FTopRow, 1);
							DrawRange;
							Free;
						end;
					if Bitmapped and Assigned (DrawBmp) then
						Invalidate;
				end;
				UpdateCaretPos;
				ShowCaret;
			end;
		end;
end;

procedure TVisibleRange.Changing;
begin
	if Assigned (Editor) then
		VisibleTextRect := Rect (FLeftCol, FTopRow, FLeftCol + Editor.PageWidth + 1, FTopRow + Editor.PageHeight + 1)
	else
		VisibleTextRect := Rect (0, 0, 0, 0);
end;

constructor TVisibleRange.Create(Collection: TCollection);
begin
	inherited;
	FTopRow := 1;
	FLeftCol := 1;
end;

function TVisibleRange.GetEndRowCol: TTextCell;
begin
	if Assigned (Editor) then begin
		Result := TextCell (FTopRow + Editor.PageHeight + 1, 0);
		if Editor.CellToCharIdx (Result) > Editor.TextLength then
			Result := Editor.CharIdxToCell (Editor.TextLength);
	end else
		Result := TextCell (FTopRow, 0);
end;

function TVisibleRange.GetREnd: Integer;
begin
	if Assigned (Editor) then
		Result := Editor.CellToCharIdx (EndRowCol)
	else
		Result := 0;
end;

function TVisibleRange.GetRightCol: Integer;
begin
	if Assigned (Editor) then
		Result := LeftCol + Editor.PageWidth + 1
	else
		Result := LeftCol;
end;

function TVisibleRange.GetRStart: Integer;
begin
	if Assigned (Editor) then
		Result := Editor.CellToCharIdx (StartRowCol)
	else
		Result := 1;
end;

function TVisibleRange.GetStartRowCol: TTextCell;
begin
	Result := TextCell (FTopRow, 1);
end;

procedure TVisibleRange.SetEndRowCol(const Value: TTextCell);
begin
end;

procedure TVisibleRange.SetLeftCol(const Value: Integer);
begin
	DoChanging;
	FLeftCol := Value;
	Update;
	DoChange;
end;

procedure TVisibleRange.SetREnd(const Value: Integer);
begin
end;

procedure TVisibleRange.SetRightCol(const Value: Integer);
begin
	if Assigned (Editor) then
		LeftCol := Value - Editor.PageWidth
	else
		LeftCol := Value;
end;

procedure TVisibleRange.SetRLength(const Value: Integer);
begin
end;

procedure TVisibleRange.SetRStart(const Value: Integer);
begin
	if Assigned (Editor) then
		StartRowCol := Editor.CharIdxToCell (Value);
end;

procedure TVisibleRange.SetStartRowCol(const Value: TTextCell);
begin
	DoChanging;
	FTopRow := Value.Row;
	Update;
	DoChange;
end;

procedure TVisibleRange.Update;
begin
	if Assigned (Editor) and (FTopRow > Editor.LineCount - Editor.PageHeight + 1) then
		FTopRow := Editor.LineCount - Editor.PageHeight + 1;
	if FTopRow < 1 then
		FTopRow := 1;
	if Assigned (Editor) and (FLeftCol > Editor.LongestLineLength - Editor.PageWidth + 1) then
		FLeftCol := Editor.LongestLineLength - Editor.PageWidth + 1;
	if FLeftCol < 1 then
		FLeftCol := 1;
	if Assigned (Editor) and Editor.HandleAllocated then begin
		SetScrollPos (Editor.Handle, sb_Vert, FTopRow, True);
		SetScrollPos (Editor.Handle, sb_Horz, FLeftCol, True);
	end;
end;

{ TSelectionRange }

procedure TSelectionRange.AssignTo(Dest: TPersistent);
begin
	inherited;
	if Dest is TSelectionRange then
		with Dest as TSelectionRange do
			Backwards := Self.Backwards;
end;

procedure TSelectionRange.Change;
var
	RS: Integer;
begin
	inherited;
	if Assigned (FOldSel) then
		with FOldSel do begin
			if Self.RStart = RStart then begin
				if Self.REnd > REnd then begin
					RStart := REnd + 1;
					REnd := Self.REnd;
				end else
					RStart := Self.REnd + 1;
			end else if Self.REnd = REnd then begin
				if Self.RStart < RStart then begin
					RS := RStart;
					RStart := Self.RStart;
					REnd := RS - 1;
				end else
					REnd := Self.RStart - 1;
			end else
				Self.DrawRange;
			DrawRange;
			Free;
		end;
	FOldSel := nil;
	UpdateCaretPos;
	ShowCaret;
	if Assigned (Editor) then
		Editor.SelectionChange;
end;

procedure TSelectionRange.Changing;
begin
	inherited;
	HideCaret;
	FScrCol := -1;
	FOldSel := TMCRange.Create (nil);
	FOldSel.Editor := Editor;
	FOldSel.Assign (Self);
end;

procedure TSelectionRange.DiscardChanges;
begin
	inherited;
	if Assigned (FOldSel) then
		FOldSel.Free;
	FOldSel := nil;
	UpdateCaretPos;
	ShowCaret;
end;

function TSelectionRange.GetCursorPos: Integer;
begin
	if Backwards then
		Result := RStart
	else
		Result := REnd + 1;
end;

function TSelectionRange.GetScrCol: Integer;
begin
	if FScrCol <= 0 then begin
		if Assigned (Editor) then
			FScrCol := Editor.CellToScrCol (Editor.CharIdxToCell (CursorPos))
		else
			FScrCol := 1;
	end;
	Result := FScrCol;
end;

procedure TSelectionRange.HideCaret;
begin
	if Assigned (Editor) and Editor.FCaretCreated and FCaretShowing	and Editor.HandleAllocated then begin
		Windows.HideCaret (Editor.Handle);
		FCaretShowing := False;
	end;
end;

procedure TSelectionRange.NoSelAtPos(Pos: Integer);
begin
	DoChanging;
	Backwards := False;
	RStart := Pos;
	REnd := Pos - 1;
	DoChange;
end;

function TSelectionRange.ScrColToCol(Row: Integer): Integer;
var
	Cell: TTextCell;
begin
	Cell.Row := Row;
	Cell.Col := ScrCol;
	if Assigned (Editor) then begin
		Editor.CellFromScrCol (Cell);
		if Cell.Col > Editor.LineLength [Row] + 1 then
			Cell.Col := Editor.LineLength [Row] + 1;
	end;
	Result := Cell.Col;
end;

procedure TSelectionRange.SetCursorPos(const Value: Integer);
begin
	if Backwards then begin
		if Value <= REnd then
			RStart := Value
		else begin
			Backwards := False;
			RStart := REnd + 1;
			REnd := Value - 1;
		end;
	end else begin
		if Value - 1 >= RStart - 1 then
			REnd := Value - 1
		else begin
			Backwards := True;
			REnd := RStart - 1;
			RStart := Value;
		end;
	end;
end;

procedure TSelectionRange.SetHidden(const Value: Boolean);
begin
	if FHidden <> Value then begin
		FHidden := Value;
		DrawRange;
	end;
end;

procedure TSelectionRange.ShowCaret;
begin
	if FChanging <= 0 then begin
		if Assigned (Editor) and Editor.FCaretCreated and ((RLength = 0) or (Editor.AlwaysShowCaret)) and (not Hidden) and (not Editor.ReadOnly) and Editor.HandleAllocated then begin
			if not FCaretShowing then begin
				Windows.ShowCaret (Editor.Handle);
				FCaretShowing := True;
			end;
		end else
			HideCaret;
	end;
end;

procedure TSelectionRange.UpdateCaretPos;
var
	P: TPoint;
  TextCell : TTextCell;
begin
	if Assigned (Editor) and Editor.FCaretCreated and (FChanging <= 0) then begin
		if Backwards then
			P := StartPoint
		else
			P := EndPoint;

    if Assigned(Editor.FOnCaretPosChanged) then
    begin
      TextCell := Editor.ScrPointToScrCell(P);
      Inc(TextCell.Row, Editor.VisibleRange.StartRowCol.Row - 1);
      Inc(TextCell.Col, Editor.VisibleRange.LeftCol - 1);

      Editor.FOnCaretPosChanged(Editor,TextCell.Col,TextCell.Row);
    end;
      
		SetCaretPos (P.X - 1, P.Y + 1);
	end;
end;

{ TCustomFormattedRange }

procedure TCustomFormattedRange.AssignTo(Dest: TPersistent);
begin
	inherited;
	if Dest is TCustomFormattedRange then
		with Dest as TCustomFormattedRange do begin
			Color := Self.Color;
			Font := Self.Font;
		end;
end;

procedure TCustomFormattedRange.SetColor(const Value: TColor);
begin
end;

procedure TCustomFormattedRange.SetFont(const Value: TFont);
begin
end;

{ TFormattedRange }

constructor TFormattedRange.Create(Collection: TCollection);
begin
	inherited;
	FFont := TFont.Create;
end;

destructor TFormattedRange.Destroy;
begin
	FFont.Free;
	inherited;
end;

function TFormattedRange.GetColor: TColor;
begin
	Result := FColor;
end;

function TFormattedRange.GetFont: TFont;
begin
	Result := FFont;
end;

procedure TFormattedRange.SetColor(const Value: TColor);
begin
	FColor := Value;
end;

procedure TFormattedRange.SetFont(const Value: TFont);
begin
	if Assigned (Value) then
		FFont.Assign (Value);
end;

{ TNormalFormattedRange }

function TNormalFormattedRange.GetColor: TColor;
begin
	if Assigned (Editor) then
		Result := Editor.Color
	else
		Result := clWindow;
end;

function TNormalFormattedRange.GetFont: TFont;
begin
	if Assigned (Editor) then
		Result := Editor.Font
	else
		Result := nil;
end;

end.

