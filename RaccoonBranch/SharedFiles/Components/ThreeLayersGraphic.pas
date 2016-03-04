unit ThreeLayersGraphic;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Graphics;

type
  TLayer2DrawEvent = procedure (Sender: TObject; DrawCanvas : TCanvas) of object;
  TAlphaBlendMode = (Blend7Bit,Blend8Bit);

  TThreeLayersGraphic = class(TGraphicControl)
  private
    { Private declarations }

    FLayer1,FLayer3,FAlphaLayer,FVScreen : TBitmap;
    FLayer3XOffset,FLayer3YOffset : integer;

    FLayer2DrawEvent : TLayer2DrawEvent;

    FBackgroundColor : TColor;
    FUseBackgroundColor : boolean;

    FAlphaBlendMode : TAlphaBlendMode;
    FVisible : boolean;

    FTransparentMode : TTransparentMode;
    FTransparentColor : TColor;

    function HasDimension(Bmp : TBitmap) : boolean;

    // Set a new size to the VScreen buffer
    procedure ResizeVScreen;

    function GetLayer1Transparent : boolean;
    procedure SetLayer1Transparent(Transparent : boolean);

    function GetLayer3Transparent : boolean;
    procedure SetLayer3Transparent(Transparent : boolean);

    function GetVScreenTransparent : boolean;
    procedure SetVScreenTransparent(Transparent : boolean);

    procedure SetTransparentMode(NewMode : TTransparentMode);
    procedure SetTransparentColor(NewColor : TColor);

    procedure SetLayer1(NewBitmap : TBitmap);
    procedure SetLayer3(NewBitmap : TBitmap);
    procedure SetAlphaLayer(NewBitmap : TBitmap);

    procedure SetLayer3XOffset(X : integer);
    procedure SetLayer3YOffset(Y : integer);

    procedure SetBackgroundColor(NewColor : TColor);
    procedure SetUseBackgroundColor(UseIt : boolean);

    procedure SetAlphaBlendMode(AlphaBlendMode : TAlphaBlendMode);
    procedure SetVisible(Visible : boolean);


  public
    { Public declarations }

    procedure Paint; override;    
    procedure Redraw;

    constructor Create(AOwner : TComponent); override;
    destructor Destroy; override;

  published
    { Published declarations }

    property Layer1 : TBitmap read FLayer1 write SetLayer1;
    property Layer3 : TBitmap read FLayer3 write SetLayer3;
    property AlphaLayer : TBitmap read FAlphaLayer write SetAlphaLayer;

    property Layer3XOffset : integer read FLayer3XOffset write SetLayer3XOffset;
    property Layer3YOffset : integer read FLayer3YOffset write SetLayer3YOffset;

    property Layer1Transparent : boolean read GetLayer1Transparent write SetLayer1Transparent;
    property Layer3Transparent : boolean read GetLayer3Transparent write SetLayer3Transparent;
    property VScreenTransparent : boolean read GetVScreenTransparent write SetVScreenTransparent;

    property TransparentMode : TTransparentMode read FTransparentMode write SetTransparentMode;
    property TransparentColor : TColor read FTransparentColor write SetTransparentColor;

    property Layer2DrawEvent : TLayer2DrawEvent read FLayer2DrawEvent write FLayer2DrawEvent;

    property BackgroundColor : TColor read FBackgroundColor write SetBackgroundColor;
    property UseBackgroundColor : boolean read FUseBackgroundColor write SetUseBackgroundColor;

    property AlphaBlendMode : TAlphaBlendMode read FAlphaBlendMode write SetAlphaBlendMode;
    property Visible : boolean read FVisible write SetVisible;

  end;

procedure Register;

implementation

{$R THREELAYERSGRAPHIC.DCR}

uses Math;

const
  DEFAULT_BACKGROUND_COLOR = clWhite;

type
  // Raw pixel type
  TRaw24BitPixel = packed record
    B,G,R : Byte;
  end;

  // Raw pixel pointer
  PRaw24BitPixel = ^TRaw24BitPixel;


// 7 Bit blending - supports brighter or darker changes to an image
procedure Apply7BitAlphaMask(Bmp,Mask : TBitmap);
var
  x,y,W,H : integer;
  BmpPtr : PRaw24BitPixel;
  MaskPtr : PByte;
  CurrMaskValue : Byte;
  Tmp : integer;
  Color : Longint;
  TR,TG,TB : Byte;

begin
  H := Min(Bmp.Height,Mask.Height);
  W := Min(Bmp.Width,Mask.Width);
  
  Color := ColorToRGB(Bmp.TransparentColor);
  TR := Color;
  TG := Color shr 8;
  TB := Color shr 16;

  // Check if bitmaps are in the right format
  if (Bmp.PixelFormat = pf24bit) and (Mask.PixelFormat = pf8bit) then
    // Y loop
    for y := 0 to H - 1 do
    begin
      BmpPtr := Bmp.ScanLine[y];
      MaskPtr := Mask.ScanLine[y];

      // X loop
      for x := 0 to W - 1 do
      begin
        CurrMaskValue := MaskPtr^;
        if (CurrMaskValue = 0) then
        begin
           BmpPtr^.B := TB;
           BmpPtr^.G := TG;
           BmpPtr^.R := TR;
        end
        else if not ((BmpPtr^.B = TB) and (BmpPtr^.G = TG) and (BmpPtr^.R = TR)) then

        begin
        Tmp := (BmpPtr^.B * CurrMaskValue) shr 7;

        if Tmp > 255 then
          Tmp := 255;

        BmpPtr^.B := Tmp;

        Tmp := (BmpPtr^.G * CurrMaskValue) shr 7;

        if Tmp > 255 then
          Tmp := 255;

        BmpPtr^.G := Tmp;

        Tmp := (BmpPtr^.R * CurrMaskValue) shr 7;

        if Tmp > 255 then
          Tmp := 255;

        BmpPtr^.R := Tmp;

        end;
        Inc(BmpPtr);
        Inc(MaskPtr);
      end;
    end;
end;

// 8 Bit blending - supports darkening an image, somewhat faster then 7 bit blending
procedure Apply8BitAlphaMask(Bmp,Mask : TBitmap);
var
  x,y,W,H : integer;
  BmpPtr : PRaw24BitPixel;
  MaskPtr : PByte;
  CurrMaskValue : Byte;
  Color : Longint;
  TR,TG,TB : Byte;

begin
  H := Min(Bmp.Height,Mask.Height);
  W := Min(Bmp.Width,Mask.Width);
  
  Color := ColorToRGB(Bmp.TransparentColor);
  TR := Color;
  TG := Color shr 8;
  TB := Color shr 16;

  // Check if bitmaps are in the right format
  if (Bmp.PixelFormat = pf24bit) and (Mask.PixelFormat = pf8bit) then
    // Y loop
    for y := 0 to H - 1 do
    begin
      BmpPtr := Bmp.ScanLine[y];
      MaskPtr := Mask.ScanLine[y];

      // X loop
      for x := 0 to W - 1 do
      begin
        CurrMaskValue := MaskPtr^;
        if (CurrMaskValue = 0) then
        begin
           BmpPtr^.B := TB;
           BmpPtr^.G := TG;
           BmpPtr^.R := TR;
        end
        else if not ((BmpPtr^.B = TB) and (BmpPtr^.G = TG) and (BmpPtr^.R = TR)) then

        begin
           BmpPtr^.B := (BmpPtr^.B * CurrMaskValue) shr 8;
           BmpPtr^.G := (BmpPtr^.G * CurrMaskValue) shr 8;
           BmpPtr^.R := (BmpPtr^.R * CurrMaskValue) shr 8;
        end;

        Inc(BmpPtr);
        Inc(MaskPtr);
      end;
    end;
end;

constructor TThreeLayersGraphic.Create(AOwner : TComponent);
begin
  inherited;

  Width := 102;
  Height := 102;
  FVisible := true;

  FBackgroundColor := DEFAULT_BACKGROUND_COLOR;

  FLayer1 := TBitmap.Create;
  FLayer1.TransparentMode := tmAuto;

  FLayer3 := TBitmap.Create;
  FLayer1.TransparentMode := tmAuto;

  FVScreen := TBitmap.Create;
  FVScreen.PixelFormat := pf24bit;
  FVScreen.Canvas.Brush.Style := bsSolid;
  FVScreen.Canvas.Pen.Style := psClear;

  FAlphaLayer := TBitmap.Create;
end;

destructor TThreeLayersGraphic.Destroy;
begin
  FAlphaLayer.Free;
  FVScreen.Free;
  FLayer3.Free;
  FLayer1.Free;

  inherited;
end;

function TThreeLayersGraphic.GetLayer1Transparent : boolean;
begin
  Result := FLayer1.Transparent;
end;

procedure TThreeLayersGraphic.SetLayer1Transparent(Transparent : boolean);
begin
  FLayer1.Transparent := Transparent;
  Redraw;
end;

function TThreeLayersGraphic.GetLayer3Transparent : boolean;
begin
  Result := FLayer3.Transparent;
end;

procedure TThreeLayersGraphic.SetLayer3Transparent(Transparent : boolean);
begin
  FLayer3.Transparent := Transparent;
  Redraw;
end;

procedure TThreeLayersGraphic.SetLayer1(NewBitmap : TBitmap);
begin
  FLayer1.Assign(NewBitmap);
  ResizeVScreen;
  Redraw;
end;

procedure TThreeLayersGraphic.SetLayer3(NewBitmap : TBitmap);
begin
  FLayer3.Assign(NewBitmap);
  ResizeVScreen;
  Redraw;
end;

procedure TThreeLayersGraphic.SetAlphaLayer(NewBitmap : TBitmap);
begin
  FAlphaLayer.Assign(NewBitmap);
  Redraw;
end;

procedure TThreeLayersGraphic.SetLayer3XOffset(X : integer);
begin
  FLayer3XOffset := X;
  Redraw;
end;

procedure TThreeLayersGraphic.SetLayer3YOffset(Y : integer);
begin
  FLayer3YOffset := Y;
  Redraw;
end;

procedure TThreeLayersGraphic.SetVisible(Visible : boolean);
begin
  FVisible := Visible;
  Redraw;
end;

procedure TThreeLayersGraphic.Paint;
begin
  Redraw;
end;

procedure TThreeLayersGraphic.Redraw;
begin
  if csLoading in ComponentState then
    Exit;


  if not Visible then
    if  not (csDesigning in ComponentState) then
      Exit;

  ResizeVScreen;

  if FUseBackgroundColor then
  with FVScreen.Canvas do
  begin
    Brush.Color := FBackgroundColor;
    FillRect(Rect(0,0,FVScreen.Width,FVScreen.Height));
  end
  else
  with FVScreen.Canvas do
  begin
    Brush.Color := FTransparentColor;
    FillRect(Rect(0,0,FVScreen.Width,FVScreen.Height));
  end;
  

  if HasDimension(FLayer1) then
    FVScreen.Canvas.Draw(0,0,FLayer1);

  if Assigned(FLayer2DrawEvent) then
    FLayer2DrawEvent(Self,FVScreen.Canvas);

  if HasDimension(FLayer3) then
    FVScreen.Canvas.Draw(FLayer3XOffset,FLayer3YOffset,FLayer3);

  if HasDimension(FAlphaLayer) then
    // Alpha mask must be in 8 bpp format
    if FAlphaLayer.PixelFormat = pf8bit then
      case FAlphaBlendMode of
        Blend7Bit:
          Apply7BitAlphaMask(FVScreen,FAlphaLayer);

        Blend8Bit:
          Apply8BitAlphaMask(FVScreen,FAlphaLayer);
      end;

  Canvas.Draw(0,0,FVScreen);
end;

// Set a new size to the VScreen buffer
procedure TThreeLayersGraphic.ResizeVScreen;
var
  w,h : integer;
begin
  w := Width;
  h := Height;

  if HasDimension(FLayer1) and HasDimension(FLayer3) then
  begin
    w := Max(FLayer1.Width,FLayer3.Width);
    h := Max(FLayer1.Height,FLayer3.Height);
  end else
    if HasDimension(FLayer1) then
    begin
      w := FLayer1.Width;
      h := FLayer1.Height;
    end
      else if HasDimension(FLayer3) then
      begin
        w := FLayer3.Width;
        h := FLayer3.Height;
      end;

  // Update the VScreen size only if different from current size
  if (w <> FVScreen.Width) or (h <> FVScreen.Height) then
  begin
    FVScreen.Width := w;
    FVScreen.Height := h;
  end;
end;

procedure TThreeLayersGraphic.SetBackgroundColor(NewColor : TColor);
begin
  FBackgroundColor := NewColor;

  Redraw;
end;

procedure TThreeLayersGraphic.SetUseBackgroundColor(UseIt : boolean);
begin
  FUseBackgroundColor := UseIt;
  Redraw;
end;

function TThreeLayersGraphic.HasDimension(Bmp : TBitmap) : boolean;
begin
  Result := (Bmp.Width > 0) and (Bmp.Height > 0);
end;

function TThreeLayersGraphic.GetVScreenTransparent : boolean;
begin
  Result := FVScreen.Transparent;
end;

procedure TThreeLayersGraphic.SetVScreenTransparent(Transparent : boolean);
begin
  FVScreen.Transparent := Transparent;

  if Transparent then
    FUseBackgroundColor := False;

  Redraw;
end;

procedure TThreeLayersGraphic.SetAlphaBlendMode(AlphaBlendMode : TAlphaBlendMode);
begin
  FAlphaBlendMode := AlphaBlendMode;
  Redraw;
end;

procedure TThreeLayersGraphic.SetTransparentMode(NewMode : TTransparentMode);
begin
  FTransparentMode := NewMode;

  FLayer1.TransparentMode  := NewMode;
  FLayer3.TransparentMode  := NewMode;
  FVScreen.TransparentMode := NewMode;
  Redraw;
end;

procedure TThreeLayersGraphic.SetTransparentColor(NewColor : TColor);
begin
  FTransparentColor := NewColor;

  FLayer1.TransparentColor  := NewColor;
  FLayer3.TransparentColor  := NewColor;
  FVScreen.TransparentColor := NewColor;
  Redraw;
end;


procedure Register;
begin
  RegisterComponents('Objet', [TThreeLayersGraphic]);
end;

end.
