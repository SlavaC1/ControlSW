{********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: EnhancedLabel component                                  *
 * Module Description: A graphic component made from three different*
 *                     bitmaps.                                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Last upate: 09/11/2003                                           *
 ********************************************************************}

unit EnhancedLabel;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Graphics;

type
  TEnhancedLabel = class(TGraphicControl)
  private
    // Three parts in three layers
    FLeftPart,FMiddlePart,FRightPart : TBitmap;
    FLeftPart1,FMiddlePart1,FRightPart1 : TBitmap;
    FLeftPart2,FMiddlePart2,FRightPart2 : TBitmap;

    // Caption offset relative to the component left,top
    FCaptionXOffset,FCaptionYOffset : integer;

    // Caption alignment mode
    FAlignment : TAlignment;

    // Number of middle parts to draw
    FMiddlePartsNum : integer;

    FCaption : string;

    // Transparency related options
    FTransparent : boolean;
    FTransparentMode : TTransparentMode;
    FTransparentColor : TColor;

    // Current layer to draw
    FActiveBitmap : integer;

    // Objects allocation
    procedure AllocateBitmaps;

    procedure UpdateWidthAndHeight;

    // Return true if a bitmap has some dimensions
    function HasDimension(Bmp : TBitmap) : boolean;

    // Set new bitmap routine for each part and each layer
    procedure SetLeftPart(NewBitmap : TBitmap);
    procedure SetMiddlePart(NewBitmap : TBitmap);
    procedure SetRightPart(NewBitmap : TBitmap);

    procedure SetLeftPart1(NewBitmap : TBitmap);
    procedure SetMiddlePart1(NewBitmap : TBitmap);
    procedure SetRightPart1(NewBitmap : TBitmap);

    procedure SetLeftPart2(NewBitmap : TBitmap);
    procedure SetMiddlePart2(NewBitmap : TBitmap);
    procedure SetRightPart2(NewBitmap : TBitmap);

    procedure SetCaptionXOffset(X : integer);
    procedure SetCaptionYOffset(Y : integer);

    procedure SetTransparent(Transparent : boolean);
    procedure SetTransparentMode(NewMode : TTransparentMode);
    procedure SetTransparentColor(NewColor : TColor);

    procedure SetMiddlePartsNum(PartsNum : integer);

    procedure SetCaption(NewCaption : string);
    procedure SetFont(NewFont : TFont);
    function GetFont : TFont;
    procedure SetAlignment(NewAlignment : TAlignment);

    procedure SetActiveBitmap(Value : integer);

    function GetActiveLeftPart : TBitmap;
    function GetActiveMiddlePart : TBitmap;
    function GetActiveRightPart : TBitmap;

  public
    { Public declarations }

    procedure Paint; override;

    procedure Redraw;

    constructor Create(AOwner : TComponent); override;
    destructor Destroy; override;

  published
    { Published declarations }

    property LeftPart : TBitmap read FLeftPart write SetLeftPart;
    property RightPart : TBitmap read FRightPart write SetRightPart;
    property MiddlePart : TBitmap read FMiddlePart write SetMiddlePart;

    property LeftPart1 : TBitmap read FLeftPart1 write SetLeftPart1;
    property RightPart1 : TBitmap read FRightPart1 write SetRightPart1;
    property MiddlePart1 : TBitmap read FMiddlePart1 write SetMiddlePart1;

    property LeftPart2 : TBitmap read FLeftPart2 write SetLeftPart2;
    property RightPart2 : TBitmap read FRightPart2 write SetRightPart2;
    property MiddlePart2 : TBitmap read FMiddlePart2 write SetMiddlePart2;

    property CaptionXOffset : integer read FCaptionXOffset write SetCaptionXOffset;
    property CaptionYOffset : integer read FCaptionYOffset write SetCaptionYOffset;

    property Transparent : boolean read FTransparent write SetTransparent;
    property TransparentMode : TTransparentMode read FTransparentMode write SetTransparentMode;
    property TransparentColor : TColor read FTransparentColor write SetTransparentColor;

    property Alignment : TAlignment read FAlignment write SetAlignment;
    property MiddlePartsNum : integer read FMiddlePartsNum write SetMiddlePartsNum;
    property Font : TFont read GetFont write SetFont;

    property ActiveBitmap : integer read FActiveBitmap write SetActiveBitmap;

    property Caption : string read FCaption write SetCaption;
    property OnMouseDown;
    property OnMouseUp;
    property OnClick;
    property OnDblClick;
    property Visible;
  end;

procedure Register;

implementation

{$R ENHANCEDLABEL.DCR}

uses Math;

const
  NUMBER_OF_BITMAPS = 3;

constructor TEnhancedLabel.Create(AOwner : TComponent);
begin
  inherited;

  Width := 60;
  Height := 16;

  FMiddlePartsNum := 1;

  AllocateBitmaps;
end;

destructor TEnhancedLabel.Destroy;
begin
  FRightPart.Free;
  FMiddlePart.Free;
  FLeftPart.Free;

  FRightPart1.Free;
  FMiddlePart1.Free;
  FLeftPart1.Free;

  FRightPart2.Free;
  FMiddlePart2.Free;
  FLeftPart2.Free;

  inherited;
end;

procedure TEnhancedLabel.SetTransparent(Transparent : boolean);
begin
  FTransparent := Transparent;

  FLeftPart.Transparent := Transparent;
  FMiddlePart.Transparent := Transparent;
  FRightPart.Transparent := Transparent;

  FLeftPart1.Transparent := Transparent;
  FMiddlePart1.Transparent := Transparent;
  FRightPart1.Transparent := Transparent;

  FLeftPart2.Transparent := Transparent;
  FMiddlePart2.Transparent := Transparent;
  FRightPart2.Transparent := Transparent;

  Invalidate;
end;

procedure TEnhancedLabel.SetTransparentMode(NewMode : TTransparentMode);
begin
  FTransparentMode := NewMode;

  FLeftPart.TransparentMode := NewMode;
  FMiddlePart.TransparentMode := NewMode;
  FRightPart.TransparentMode := NewMode;

  FLeftPart1.TransparentMode := NewMode;
  FMiddlePart1.TransparentMode := NewMode;
  FRightPart1.TransparentMode := NewMode;

  FLeftPart2.TransparentMode := NewMode;
  FMiddlePart2.TransparentMode := NewMode;
  FRightPart2.TransparentMode := NewMode;

  Invalidate;
end;

procedure TEnhancedLabel.SetTransparentColor(NewColor : TColor);
begin
  FTransparentColor := NewColor;

  FLeftPart.TransparentColor := NewColor;
  FMiddlePart.TransparentColor := NewColor;
  FRightPart.TransparentColor := NewColor;

  FLeftPart1.TransparentColor := NewColor;
  FMiddlePart1.TransparentColor := NewColor;
  FRightPart1.TransparentColor := NewColor;

  FLeftPart2.TransparentColor := NewColor;
  FMiddlePart2.TransparentColor := NewColor;
  FRightPart2.TransparentColor := NewColor;

  Invalidate;
end;

procedure TEnhancedLabel.SetMiddlePartsNum(PartsNum : integer);
begin
  FMiddlePartsNum := PartsNum;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetLeftPart(NewBitmap : TBitmap);
begin
  FLeftPart.Assign(NewBitmap);
  FLeftPart.TransparentColor := FTransparentColor;
  FLeftPart.TransparentMode := FTransparentMode;
  FLeftPart.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetMiddlePart(NewBitmap : TBitmap);
begin
  FMiddlePart.Assign(NewBitmap);
  FMiddlePart.TransparentColor := FTransparentColor;
  FMiddlePart.TransparentMode := FTransparentMode;
  FMiddlePart.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetRightPart(NewBitmap : TBitmap);
begin
  FRightPart.Assign(NewBitmap);
  FRightPart.TransparentColor := FTransparentColor;
  FRightPart.TransparentMode := FTransparentMode;
  FRightPart.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetLeftPart1(NewBitmap : TBitmap);
begin
  FLeftPart1.Assign(NewBitmap);
  FLeftPart1.TransparentColor := FTransparentColor;
  FLeftPart1.TransparentMode := FTransparentMode;
  FLeftPart1.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetMiddlePart1(NewBitmap : TBitmap);
begin
  FMiddlePart1.Assign(NewBitmap);
  FMiddlePart1.TransparentColor := FTransparentColor;
  FMiddlePart1.TransparentMode := FTransparentMode;
  FMiddlePart1.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetRightPart1(NewBitmap : TBitmap);
begin
  FRightPart1.Assign(NewBitmap);
  FRightPart1.TransparentColor := FTransparentColor;
  FRightPart1.TransparentMode := FTransparentMode;
  FRightPart1.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetLeftPart2(NewBitmap : TBitmap);
begin
  FLeftPart2.Assign(NewBitmap);
  FLeftPart2.TransparentColor := FTransparentColor;
  FLeftPart2.TransparentMode := FTransparentMode;
  FLeftPart2.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetMiddlePart2(NewBitmap : TBitmap);
begin
  FMiddlePart2.Assign(NewBitmap);
  FMiddlePart2.TransparentColor := FTransparentColor;
  FMiddlePart2.TransparentMode := FTransparentMode;
  FMiddlePart2.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetRightPart2(NewBitmap : TBitmap);
begin
  FRightPart2.Assign(NewBitmap);
  FRightPart2.TransparentColor := FTransparentColor;
  FRightPart2.TransparentMode := FTransparentMode;
  FRightPart2.Transparent := FTransparent;
  UpdateWidthAndHeight;
  Redraw;
end;

procedure TEnhancedLabel.SetCaptionXOffset(X : integer);
begin
  FCaptionXOffset := X;
  Redraw;
end;

procedure TEnhancedLabel.SetCaptionYOffset(Y : integer);
begin
  FCaptionYOffset := Y;
  Redraw;
end;

procedure TEnhancedLabel.Paint;
begin
  Redraw;
end;

procedure TEnhancedLabel.Redraw;
var
  X,LabelStartX,LabelRefHeight : integer;
  LabelSize : TSize;
  i : integer;
begin
  // Don't redraw if hidden (always draw in design time)
  if not(csDesigning in ComponentState) then
    if not Visible then
      Exit;

  if csLoading in ComponentState then
    Exit;

  if HasDimension(GetActiveLeftPart) then
  begin
    Canvas.Draw(0,0,GetActiveLeftPart);
    X := GetActiveLeftPart.Width;
  end else
    X := 0;

  LabelStartX := X;

  if HasDimension(GetActiveMiddlePart) then
  begin
    for i := 0 to FMiddlePartsNum - 1 do
    begin
      Canvas.Draw(X,0,GetActiveMiddlePart);
      Inc(X,GetActiveMiddlePart.Width);
    end;

    LabelRefHeight := GetActiveMiddlePart.Height;
  end else
    LabelRefHeight := Height;

  if HasDimension(GetActiveRightPart) then
    Canvas.Draw(X,0,GetActiveRightPart);

  // Calculate the label dimensions
  LabelSize := Canvas.TextExtent(FCaption);

  case FAlignment of
    taLeftJustify: Inc(LabelStartX,FCaptionXOffset);
    taRightJustify: LabelStartX := X - LabelSize.cx + FCaptionXOffset;
    taCenter: LabelStartX := ((X - LabelStartX) - LabelSize.cx) div 2 + LabelStartX + FCaptionXOffset;
  end;

  // Draw the label text in transparent mode
  SetBkMode(Canvas.Handle,Windows.TRANSPARENT);
  Canvas.TextOut(LabelStartX,(LabelRefHeight - LabelSize.cy) div 2 + FCaptionYOffset,FCaption);
end;

procedure TEnhancedLabel.SetCaption(NewCaption : string);
begin
  FCaption := NewCaption;
  Redraw;
end;

procedure TEnhancedLabel.SetFont(NewFont : TFont);
begin
  Canvas.Font.Assign(NewFont);
  Redraw;
end;

function TEnhancedLabel.GetFont : TFont;
begin
  Result := Canvas.Font;
end;

procedure TEnhancedLabel.SetAlignment(NewAlignment : TAlignment);
begin
  FAlignment := NewAlignment;
  Redraw;
end;

function TEnhancedLabel.HasDimension(Bmp : TBitmap) : boolean;
begin
  Result := (Bmp.Width > 0) and (Bmp.Height > 0);
end;

procedure TEnhancedLabel.UpdateWidthAndHeight;
var
  W,H : integer;
begin
  if HasDimension(FLeftPart) then
  begin
    W := GetActiveLeftPart.Width;
    H := GetActiveLeftPart.Height;
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

  Width := W;
  Height := H;
end;

// Allocate and initialize internal bitmaps
procedure TEnhancedLabel.AllocateBitmaps;
var
  i : integer;
  Part : TBitmap;
begin
  FLeftPart := TBitmap.Create;
  FMiddlePart := TBitmap.Create;
  FRightPart := TBitmap.Create;
  FLeftPart1 := TBitmap.Create;
  FMiddlePart1 := TBitmap.Create;
  FRightPart1 := TBitmap.Create;
  FLeftPart2 := TBitmap.Create;
  FMiddlePart2 := TBitmap.Create;
  FRightPart2 := TBitmap.Create;

  for i := 0 to NUMBER_OF_BITMAPS - 1 do
  begin
    FActiveBitmap := i;

    Part := GetActiveLeftPart;
    Part.TransparentMode := FTransparentMode;
    Part.TransparentColor := FTransparentColor;
    Part.Transparent := FTransparent;

    Part := GetActiveMiddlePart;
    Part.TransparentMode := FTransparentMode;
    Part.TransparentColor := FTransparentColor;
    Part.Transparent := FTransparent;

    Part := GetActiveRightPart;
    Part.TransparentMode := FTransparentMode;
    Part.TransparentColor := FTransparentColor;
    Part.Transparent := FTransparent;
  end;

  FActiveBitmap := 0;
end;

procedure TEnhancedLabel.SetActiveBitmap(Value : integer);
begin
  if (Value >= 0) and (Value < NUMBER_OF_BITMAPS) then
  begin
    FActiveBitmap := Value;
    Invalidate;
  end;
end;

function TEnhancedLabel.GetActiveLeftPart : TBitmap;
begin
  case FActiveBitmap of
    0: Result := FLeftPart;
    1: Result := FLeftPart1;
    2: Result := FLeftPart2;
  else
    Result := nil;
  end;
end;

function TEnhancedLabel.GetActiveMiddlePart : TBitmap;
begin
  case FActiveBitmap of
    0: Result := FMiddlePart;
    1: Result := FMiddlePart1;
    2: Result := FMiddlePart2;
  else
    Result := nil;
  end;
end;

function TEnhancedLabel.GetActiveRightPart : TBitmap;
begin
  case FActiveBitmap of
    0: Result := FRightPart;
    1: Result := FRightPart1;
    2: Result := FRightPart2;
  else
    Result := nil;
  end;
end;

procedure Register;
begin
  RegisterComponents('Objet', [TEnhancedLabel]);
end;

end.
