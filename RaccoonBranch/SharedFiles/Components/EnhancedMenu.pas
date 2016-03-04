{********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: EnhancedMenu component                                   *
 * Module Description: A graphic component for bitmaps based  menu. *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Last upate: 09/11/2003                                           *
 ********************************************************************}

unit EnhancedMenu;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Graphics;

type
  TEnhancedMenu = class(TGraphicControl)
  private
    // Each menu "line" is composed from left,middle and right parts
    FLeftPart,FMiddlePart,FRightPart : TBitmap;

    // Each menu "line" can be highlighted
    FHighlightedLeftPart,FHighlightedMiddlePart,FHighlightedRightPart : TBitmap;

    // Caption offset relative to the current menu line X and Y
    FCaptionXOffset,FCaptionYOffset : integer;

    // Caption alignment mode
    FAlignment : TAlignment;

    // Number of middle parts to draw
    FMiddlePartsNum : integer;

    // Menu items strings
    FItems : TStringList;

    // Transparency related options
    FTransparent : boolean;
    FTransparentMode : TTransparentMode;
    FTransparentColor : TColor;

    FFont,FHighlightedFont : TFont;

    FSpacing : integer;
    FCurrentItem : integer;

    FLeftButtonIsDown : boolean;
    FItemAtMouseDown : integer;

    FOnClick : TNotifyEvent;

    FRestoreHighlightOnMouseUp : boolean;

    procedure UpdateWidthAndHeight;

    function HasDimension(Bmp : TBitmap) : boolean;

    procedure SetLeftPart(NewBitmap : TBitmap);
    procedure SetMiddlePart(NewBitmap : TBitmap);
    procedure SetRightPart(NewBitmap : TBitmap);

    procedure SetHighlightedLeftPart(NewBitmap : TBitmap);
    procedure SetHighlightedMiddlePart(NewBitmap : TBitmap);
    procedure SetHighlightedRightPart(NewBitmap : TBitmap);

    procedure SetCaptionXOffset(X : integer);
    procedure SetCaptionYOffset(Y : integer);

    procedure SetTransparent(Transparent : boolean);
    procedure SetTransparentMode(NewMode : TTransparentMode);
    procedure SetTransparentColor(NewColor : TColor);

    procedure SetMiddlePartsNum(PartsNum : integer);
    procedure SetItems(NewStringList : TStringList);
    procedure SetSpacing(NewValue : integer);

    procedure SetFont(NewFont : TFont);

    procedure SetHighlightedFont(NewFont : TFont);

    procedure SetAlignment(NewAlignment : TAlignment);
    procedure DrawMenuItem(X,Y : integer; Caption : string; Highlighted : boolean);

    function CalcMenuItemY(ItemNum : integer) : integer;

    procedure HighlightItem(ItemNum : integer);

    function FindItemAtGivenPosition(X,Y : integer) : integer;

    // Mouse related events
    procedure MouseDownHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure MouseUpHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure MouseMoveHandler(Sender: TObject; Shift: TShiftState; X,Y: Integer);

  public
    { Public declarations }

    procedure Paint; override;

    procedure Redraw;

    constructor Create(AOwner : TComponent); override;
    destructor Destroy; override;

    // Move cursor one step down (support roll-over)
    procedure MoveDown;
    
    // Move cursor one step up (support roll-over)
    procedure MoveUp;

    // Activate current menu item (trigger the OnClick event)
    procedure Activate;

  published
    { Published declarations }

    property LeftPart : TBitmap read FLeftPart write SetLeftPart;
    property RightPart : TBitmap read FRightPart write SetRightPart;
    property MiddlePart : TBitmap read FMiddlePart write SetMiddlePart;

    property HighlightedLeftPart : TBitmap read FHighlightedLeftPart write SetHighlightedLeftPart;
    property HighlightedRightPart : TBitmap read FHighlightedRightPart write SetHighlightedRightPart;
    property HighlightedMiddlePart : TBitmap read FHighlightedMiddlePart write SetHighlightedMiddlePart;

    property CaptionXOffset : integer read FCaptionXOffset write SetCaptionXOffset;
    property CaptionYOffset : integer read FCaptionYOffset write SetCaptionYOffset;

    property Transparent : boolean read FTransparent write SetTransparent;
    property TransparentMode : TTransparentMode read FTransparentMode write SetTransparentMode;
    property TransparentColor : TColor read FTransparentColor write SetTransparentColor;

    property Alignment : TAlignment read FAlignment write SetAlignment;
    property MiddlePartsNum : integer read FMiddlePartsNum write SetMiddlePartsNum;
    property Font : TFont read FFont write SetFont;
    property HighlightedFont : TFont read FHighlightedFont write SetHighlightedFont;
    property Spacing : integer read FSpacing write SetSpacing;

    property Items : TStringList read FItems write SetItems;
    property ItemIndex : integer read FCurrentItem write HighlightItem;

    property OnClick : TNotifyEvent read FOnClick write FOnClick;
    property RestoreHighlightOnMouseUp : boolean read FRestoreHighlightOnMouseUp write FRestoreHighlightOnMouseUp;

    property Visible;
  end;

procedure Register;

implementation

{$R ENHANCEDMENU.DCR}

uses Math;

const
  DEFAULT_WIDTH  = 32;
  DEFAULT_HEIGHT = 16;
  DEFAULT_SPACING = 5;

constructor TEnhancedMenu.Create(AOwner : TComponent);
begin
  inherited;

  Width := DEFAULT_WIDTH;
  Height := DEFAULT_HEIGHT;
  FSpacing := DEFAULT_SPACING;
  FCurrentItem := -1;
  FItemAtMouseDown := -1;

  FMiddlePartsNum := 1;

  FLeftPart := TBitmap.Create;
  FMiddlePart := TBitmap.Create;
  FRightPart := TBitmap.Create;
  FHighlightedLeftPart := TBitmap.Create;
  FHighlightedMiddlePart := TBitmap.Create;
  FHighlightedRightPart := TBitmap.Create;
  FFont := TFont.Create;
  FHighlightedFont := TFont.Create;

  FItems := TStringList.Create;

  // Bind mouse events
  OnMouseDown := MouseDownHandler;
  OnMouseUp := MouseUpHandler;
  OnMouseMove := MouseMoveHandler;
end;

destructor TEnhancedMenu.Destroy;
begin
  FRightPart.Free;
  FMiddlePart.Free;
  FLeftPart.Free;
  FHighlightedLeftPart.Free;
  FHighlightedMiddlePart.Free;
  FHighlightedRightPart.Free;
  FFont.Free;
  FHighlightedFont.Free;

  FItems.Free;

  inherited;
end;

procedure TEnhancedMenu.SetTransparent(Transparent : boolean);
begin
  FTransparent := Transparent;

  FLeftPart.Transparent := Transparent;
  FMiddlePart.Transparent := Transparent;
  FRightPart.Transparent := Transparent;
  FHighlightedLeftPart.Transparent := Transparent;
  FHighlightedMiddlePart.Transparent := Transparent;
  FHighlightedRightPart.Transparent := Transparent;

  Invalidate;
end;

procedure TEnhancedMenu.SetTransparentMode(NewMode : TTransparentMode);
begin
  FTransparentMode := NewMode;

  FLeftPart.TransparentMode := NewMode;
  FMiddlePart.TransparentMode := NewMode;
  FRightPart.TransparentMode := NewMode;
  FHighlightedLeftPart.TransparentMode := NewMode;
  FHighlightedMiddlePart.TransparentMode := NewMode;
  FHighlightedRightPart.TransparentMode := NewMode;

  Invalidate;
end;

procedure TEnhancedMenu.SetTransparentColor(NewColor : TColor);
begin
  FTransparentColor := NewColor;

  FLeftPart.TransparentColor := NewColor;
  FMiddlePart.TransparentColor := NewColor;
  FRightPart.TransparentColor := NewColor;
  FHighlightedLeftPart.TransparentColor := NewColor;
  FHighlightedMiddlePart.TransparentColor := NewColor;
  FHighlightedRightPart.TransparentColor := NewColor;

  Invalidate;
end;

procedure TEnhancedMenu.SetMiddlePartsNum(PartsNum : integer);
begin
  FMiddlePartsNum := PartsNum;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedMenu.SetLeftPart(NewBitmap : TBitmap);
begin
  FLeftPart.Assign(NewBitmap);
  FLeftPart.TransparentColor := FTransparentColor;
  FLeftPart.TransparentMode := FTransparentMode;
  FLeftPart.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedMenu.SetMiddlePart(NewBitmap : TBitmap);
begin
  FMiddlePart.Assign(NewBitmap);
  FMiddlePart.TransparentColor := FTransparentColor;
  FMiddlePart.TransparentMode := FTransparentMode;
  FMiddlePart.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedMenu.SetRightPart(NewBitmap : TBitmap);
begin
  FRightPart.Assign(NewBitmap);
  FRightPart.TransparentColor := FTransparentColor;
  FRightPart.TransparentMode := FTransparentMode;
  FRightPart.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedMenu.SetHighlightedLeftPart(NewBitmap : TBitmap);
begin
  FHighlightedLeftPart.Assign(NewBitmap);
  FHighlightedLeftPart.TransparentColor := FTransparentColor;
  FHighlightedLeftPart.TransparentMode := FTransparentMode;
  FHighlightedLeftPart.Transparent := FTransparent;
  Redraw;
end;

procedure TEnhancedMenu.SetHighlightedMiddlePart(NewBitmap : TBitmap);
begin
  FHighlightedMiddlePart.Assign(NewBitmap);
  FHighlightedMiddlePart.TransparentColor := FTransparentColor;
  FHighlightedMiddlePart.TransparentMode := FTransparentMode;
  FHighlightedMiddlePart.Transparent := FTransparent;
  Redraw;
end;

procedure TEnhancedMenu.SetHighlightedRightPart(NewBitmap : TBitmap);
begin
  FHighlightedRightPart.Assign(NewBitmap);
  FHighlightedRightPart.TransparentColor := FTransparentColor;
  FHighlightedRightPart.TransparentMode := FTransparentMode;
  FHighlightedRightPart.Transparent := FTransparent;
  Redraw;
end;

procedure TEnhancedMenu.SetCaptionXOffset(X : integer);
begin
  FCaptionXOffset := X;
  Redraw;
end;

procedure TEnhancedMenu.SetCaptionYOffset(Y : integer);
begin
  FCaptionYOffset := Y;
  Redraw;
end;

procedure TEnhancedMenu.Paint;
begin
  Redraw;
end;

procedure TEnhancedMenu.DrawMenuItem(X,Y : integer; Caption : string; Highlighted : boolean);
var
  LabelStartX,LabelRefHeight : integer;
  LabelSize : TSize;
  i : integer;
  CurrentLeft,CurrentMiddle,CurrentRight : TBitmap;
  CurrentFont : TFont;
begin
  if Highlighted then
  begin
    CurrentLeft   := FHighlightedLeftPart;
    CurrentMiddle := FHighlightedMiddlePart;
    CurrentRight  := FHighlightedRightPart;
    CurrentFont   := FHighlightedFont;
  end else
    begin
      CurrentLeft   := FLeftPart;
      CurrentMiddle := FMiddlePart;
      CurrentRight  := FRightPart;
      CurrentFont   := FFont;
    end;

  if HasDimension(CurrentLeft) then
  begin
    Canvas.Draw(X,Y,CurrentLeft);
    Inc(X,CurrentLeft.Width);
  end;

  LabelStartX := X;

  if HasDimension(CurrentMiddle) then
  begin
    for i := 0 to FMiddlePartsNum - 1 do
    begin
      Canvas.Draw(X,Y,CurrentMiddle);
      Inc(X,CurrentMiddle.Width);
    end;

    LabelRefHeight := CurrentMiddle.Height;
  end else
    LabelRefHeight := Height;

  if HasDimension(CurrentRight) then
    Canvas.Draw(X,Y,CurrentRight);

  Canvas.Font.Assign(CurrentFont);

  // Calculate the label dimensions
  LabelSize := Canvas.TextExtent(Caption);

  case FAlignment of
    taLeftJustify: Inc(LabelStartX,FCaptionXOffset);
    taRightJustify: LabelStartX := X - LabelSize.cx + FCaptionXOffset;
    taCenter: LabelStartX := ((X - LabelStartX) - LabelSize.cx) div 2 + LabelStartX + FCaptionXOffset;
  end;

  // Draw the label text in transparent mode
  SetBkMode(Canvas.Handle,Windows.TRANSPARENT);
  Canvas.TextOut(LabelStartX,Y + (LabelRefHeight - LabelSize.cy) div 2 + FCaptionYOffset,Caption);
end;

procedure TEnhancedMenu.Redraw;
var
  CurrentY,i : integer;
begin
  if csLoading in ComponentState then
    Exit;

  CurrentY := 0;

  for i := 0 to FItems.Count - 1 do
  begin
    if i = FCurrentItem then
      DrawMenuItem(0,CurrentY,FItems[i],true)
    else
      DrawMenuItem(0,CurrentY,FItems[i],false);

    Inc(CurrentY,FMiddlePart.Height + FSpacing);
  end;
end;

procedure TEnhancedMenu.SetItems(NewStringList : TStringList);
begin
  FItems.Assign(NewStringList);
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedMenu.SetSpacing(NewValue : integer);
begin
  FSpacing := NewValue;
  Invalidate;
end;

procedure TEnhancedMenu.SetFont(NewFont : TFont);
begin
  FFont.Assign(NewFont);
  Redraw;
end;

procedure TEnhancedMenu.SetHighlightedFont(NewFont : TFont);
begin
  FHighlightedFont.Assign(NewFont);
  Redraw;
end;

procedure TEnhancedMenu.SetAlignment(NewAlignment : TAlignment);
begin
  FAlignment := NewAlignment;
  Redraw;
end;

function TEnhancedMenu.HasDimension(Bmp : TBitmap) : boolean;
begin
  Result := (Bmp.Width > 0) and (Bmp.Height > 0);
end;

procedure TEnhancedMenu.UpdateWidthAndHeight;
var
  W,H : integer;
begin
  Exit;

  if HasDimension(FLeftPart) then
  begin
    W := FLeftPart.Width;
    H := FLeftPart.Height;
  end else
    begin
      W := 0;
      H := 0;
    end;

  if HasDimension(FMiddlePart) then
  begin
    Inc(W,FMiddlePartsNum * FMiddlePart.Width);
    H := Max(H,FMiddlePart.Height);
  end;

  if HasDimension(FRightPart) then
  begin
    Inc(W,FRightPart.Width);
    H := Max(H,FRightPart.Height);
  end;

  Width := Max(DEFAULT_WIDTH,W);
  Height := Max(DEFAULT_HEIGHT,H * FItems.Count + FSpacing * (FItems.Count - 1));
end;

function TEnhancedMenu.CalcMenuItemY(ItemNum : integer) : integer;
begin
  Result := ItemNum * (FMiddlePart.Height + FSpacing);
end;

procedure TEnhancedMenu.HighlightItem(ItemNum : integer);
begin
  // If something has changed
  if ItemNum = FCurrentItem then
    Exit;

  if FCurrentItem <> -1 then
    // Un-highlight previous item
    DrawMenuItem(0,CalcMenuItemY(FCurrentItem),FItems[FCurrentItem],false);

  if (ItemNum >= 0) and (ItemNum < FItems.Count) then
  begin
    // Highlight new item
    DrawMenuItem(0,CalcMenuItemY(ItemNum),FItems[ItemNum],true);
    FCurrentItem := ItemNum;
  end else
    if ItemNum = -1 then
      FCurrentItem := ItemNum;
end;

function TEnhancedMenu.FindItemAtGivenPosition(X,Y : integer) : integer;
var
  i : integer;
  p : TPoint;
  R : TRect;
  W,YTop : integer;
begin
  p := Point(X,Y);

  W := FLeftPart.Width + FMiddlePart.Width * FMiddlePartsNum + FRightPart.Width;

  for i := 0 to FItems.Count - 1 do
  begin
    YTop := CalcMenuItemY(i);
    R := Rect(0,YTop,W,YTop + FMiddlePart.Height);
    if PtInRect(R,P) then
    begin
      Result := i;
      Exit;
    end;
  end;

  Result := -1;
end;

// Move cursor one step down (support roll-over)
procedure TEnhancedMenu.MoveDown;
var
  NextItem : integer;
begin
  NextItem := (FCurrentItem + 1) mod FItems.Count;
  HighlightItem(NextItem);
end;

// Move cursor one step up (support roll-over)
procedure TEnhancedMenu.MoveUp;
var
  NextItem : integer;
begin
  NextItem := FCurrentItem - 1;
  if NextItem < 0 then
    NextItem := FItems.Count - 1;

  HighlightItem(NextItem);
end;

// Activate current menu item (trigger the OnClick event)
procedure TEnhancedMenu.Activate;
begin
  if FCurrentItem <> -1 then
    if Assigned(FOnClick) then
      FOnClick(Self);
end;

// Mouse related events
procedure TEnhancedMenu.MouseDownHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  i : integer;
begin
  if Button = mbLeft then
  begin
    i := FindItemAtGivenPosition(X,Y);

    if i <> - 1 then
    begin
      FLeftButtonIsDown := true;
      FItemAtMouseDown := i;
      HighlightItem(i);
    end;
  end;
end;

procedure TEnhancedMenu.MouseUpHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  i : integer;
begin
  if not FLeftButtonIsDown then
    Exit;
    
  if Button = mbLeft then
  begin
    FLeftButtonIsDown := false;

    i := FindItemAtGivenPosition(X,Y);
    HighlightItem(i);

    // Activate the menu event if the user release the mouse over the selected item
    if i <> - 1 then
      Activate
    else
      if FRestoreHighlightOnMouseUp then
        HighlightItem(FItemAtMouseDown);
  end;
end;

procedure TEnhancedMenu.MouseMoveHandler(Sender: TObject; Shift: TShiftState; X,Y: Integer);
var
  i : integer;
begin
  if FLeftButtonIsDown then
  begin
    i := FindItemAtGivenPosition(X,Y);
    HighlightItem(i);
  end;
end;


procedure Register;
begin
  RegisterComponents('Objet', [TEnhancedMenu]);
end;

end.
