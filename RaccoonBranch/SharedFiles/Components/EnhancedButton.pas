unit EnhancedButton;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Graphics;

type
  TCurrentButtonState = (bsUp,bsHighlighted,bsDown);

  TEnhancedButton = class(TGraphicControl)
  private
    { Private declarations }

    FUpBmp,FHighlightedBmp,FDownBmp,FDisabledBmp : TBitmap;

    FTransparentMode : TTransparentMode;
    FTransparentColor : TColor;
    FTransparent : boolean;
    FMouseIsDown : boolean;

    MouseIsIn   : boolean;

    FOnClick : TNotifyEvent;
    FOnMouseDown,FOnMouseUp : TMouseEvent;
    FOnMouseMove : TMouseMoveEvent;
    FOnMouseEnter : TNotifyEvent;
    FOnMouseLeave : TNotifyEvent;

    CurrentState : TCurrentButtonState;

    FMask : TBitmap;
    FUseMask,FShowMask : boolean;
    FMaskXOffset,FMaskYOffset : integer;

    FAction : TBasicAction;

    function HitTest(Bmp: TBitmap; TransparentColor : TColor; X,Y : integer) : boolean;

    function HasDimension(Bmp : TBitmap) : boolean;

    procedure SetTransparentMode(NewMode : TTransparentMode);
    procedure SetTransparentColor(NewColor : TColor);

    procedure SetUpBmp(NewBitmap : TBitmap);
    procedure SetHighlightedBmp(NewBitmap : TBitmap);
    procedure SetDownBmp(NewBitmap : TBitmap);
    procedure SetDisabledBmp(NewBitmap : TBitmap);

    procedure SetTransparent(NewMode : boolean);

    procedure SetMask(NewMask : TBitmap);
    procedure SetShowMask(ShowMask : boolean);
    procedure SetMaskXOffset(NewOffset : integer);
    procedure SetMaskYOffset(NewOffset : integer);

    procedure OnMouseDownHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure OnMouseUpHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure OnMouseMoveHandler(Sender: TObject; Shift: TShiftState; X, Y: Integer);

    procedure CMMouseEnter(var AMsg: TMessage); message CM_MOUSEENTER;
    procedure CMMouseLeave(var AMsg: TMessage); message CM_MOUSELEAVE;

  public
    { Public declarations }

    procedure Paint; override;
    procedure Redraw;

    constructor Create(AOwner : TComponent); override;
    destructor Destroy; override;

    procedure SetEnabled(Value: Boolean); override;    

  published
    { Published declarations }

    property UpBmp : TBitmap read FUpBmp write SetUpBmp;
    property HighlightedBmp : TBitmap read FHighlightedBmp write SetHighlightedBmp;
    property DownBmp : TBitmap read FDownBmp write SetDownBmp;
    property DisabledBmp : TBitmap read FDisabledBmp write SetDisabledBmp;

    property TransparentMode : TTransparentMode read FTransparentMode write SetTransparentMode;
    property TransparentColor : TColor read FTransparentColor write SetTransparentColor;
    property Transparent : boolean read FTransparent write SetTransparent;

    property OnClick : TNotifyEvent read FOnClick write FOnClick;
    property OnMouseDown : TMouseEvent read FOnMouseDown write FOnMouseDown;
    property OnMouseUp : TMouseEvent read FOnMouseUp write FOnMouseUp;
    property OnMouseMove : TMouseMoveEvent read FOnMouseMove write FOnMouseMove;
    property OnMouseEnter : TNotifyEvent read FOnMouseEnter write FOnMouseEnter;
    property OnMouseLeave : TNotifyEvent read FOnMouseLeave write FOnMouseLeave;

    property Mask : TBitmap read FMask write SetMask;
    property UseMask : boolean read FUseMask write FUseMask;
    property ShowMask : boolean read FShowMask write SetShowMask;
    property MaskXOffset : integer read FMaskXOffset write SetMaskXOffset;
    property MaskYOffset : integer read FMaskYOffset write SetMaskYOffset;

    property Action : TBasicAction read FAction write FAction;
    property Enabled;
    property Visible;
    property Hint;
    property ShowHint;
  end;

procedure Register;

implementation

{$R ENHANCEDBUTTON.DCR}

constructor TEnhancedButton.Create(AOwner : TComponent);
begin
  inherited;

  Width := 102;
  Height := 102;

  FUpBmp := TBitmap.Create;
  FUpBmp.TransparentMode := tmAuto;

  FHighlightedBmp := TBitmap.Create;
  FHighlightedBmp.TransparentMode := tmAuto;

  FDownBmp := TBitmap.Create;
  FDownBmp.TransparentMode := tmAuto;

  FMask := TBitmap.Create;
  FMask.TransparentMode := tmAuto;
  FMask.Transparent := true;

  MouseIsIn := false;

  FDisabledBmp := TBitmap.Create;
  FDisabledBmp.TransparentMode := tmAuto;

  CurrentState := bsUp;

  inherited OnMouseMove := OnMouseMoveHandler;
  inherited OnMouseDown := OnMouseDownHandler;
  inherited OnMouseUp   := OnMouseUpHandler;
end;

destructor TEnhancedButton.Destroy;
begin
  DownBmp.Free;
  HighlightedBmp.Free;
  UpBmp.Free;
  Mask.Free;
  DisabledBmp.Free;
  inherited;
end;

procedure TEnhancedButton.SetUpBmp(NewBitmap : TBitmap);
begin
  FUpBmp.Assign(NewBitmap);
  Redraw;
end;

procedure TEnhancedButton.SetHighlightedBmp(NewBitmap : TBitmap);
begin
  HighlightedBmp.Assign(NewBitmap);
  Redraw;
end;

procedure TEnhancedButton.SetDownBmp(NewBitmap : TBitmap);
begin
  FDownBmp.Assign(NewBitmap);
  Redraw;
end;

procedure TEnhancedButton.SetDisabledBmp(NewBitmap : TBitmap);
begin
  FDisabledBmp.Assign(NewBitmap);
  Redraw;
end;

procedure TEnhancedButton.Paint;
begin
  Redraw;
end;

procedure TEnhancedButton.Redraw;
begin
  if csLoading in ComponentState then
    Exit;

  if Enabled then
  begin
    if HasDimension(FUpBmp) then
      Canvas.Draw(0,0,FUpBmp);
  end else
    begin
      if HasDimension(FDisabledBmp) then
        Canvas.Draw(0,0,FDisabledBmp);
    end;

  if FShowMask then
    Canvas.Draw(FMaskXOffset,FMaskYOffset,FMask);
end;

function TEnhancedButton.HasDimension(Bmp : TBitmap) : boolean;
begin
  Result := (Bmp.Width > 0) and (Bmp.Height > 0);
end;

procedure TEnhancedButton.SetTransparentMode(NewMode : TTransparentMode);
begin
  FTransparentMode := NewMode;

  FUpBmp.TransparentMode          := NewMode;
  FHighlightedBmp.TransparentMode := NewMode;
  FDownBmp.TransparentMode        := NewMode;
  FDisabledBmp.TransparentMode    := NewMode;
  Invalidate;
end;

procedure TEnhancedButton.SetEnabled(Value : boolean);
begin
  Inherited;
  Invalidate;
end;

procedure TEnhancedButton.SetTransparentColor(NewColor : TColor);
begin
  FTransparentColor := NewColor;
  FUpBmp.TransparentColor          := NewColor;
  FHighlightedBmp.TransparentColor := NewColor;
  FDownBmp.TransparentColor        := NewColor;
  FDisabledBmp.TransparentColor    := NewColor;
  Invalidate;
end;

procedure TEnhancedButton.SetTransparent(NewMode : boolean);
begin
  FTransparent := NewMode;
  FUpBmp.Transparent          := NewMode;
  FHighlightedBmp.Transparent := NewMode;
  FDownBmp.Transparent        := NewMode;
  FDisabledBmp.Transparent    := NewMode;
  Invalidate;
end;

procedure TEnhancedButton.OnMouseMoveHandler(Sender: TObject; Shift: TShiftState; X, Y: Integer);
var
  RefBmp : TBitmap;
  RequiredState : TCurrentButtonState;
  HitFlag : boolean;
begin
  if not Enabled then
    Exit;

  if FMouseIsDown then
    begin
      RefBmp := FDownBmp;
      RequiredState := bsDown;
    end
  else
    begin
      RefBmp := FHighlightedBmp;
      RequiredState := bsHighlighted;
    end;

  if FUseMask then
    HitFlag := HitTest(FMask,clBlack,X - FMaskXOffset,Y - FMaskYOffset)
  else
    HitFlag := HitTest(FUpBmp,FTransparentColor,X,Y);

  if HitFlag then
    begin

    if not MouseIsIn then
      begin
        MouseIsIn := true;
        if Assigned(FOnMouseEnter) then
          FOnMouseEnter(Self);
      end;

//     if CurrentState <> RequiredState then
//       begin
          Canvas.Draw(0,0,RefBmp);
          CurrentState := RequiredState;
//       end;

     if Assigned(FOnMouseMove) then
       FOnMouseMove(Self,Shift,X,Y);
    end
  else
    begin

      if MouseIsIn then
        begin
          MouseIsIn := false;
          if Assigned(FOnMouseLeave) then
            FOnMouseLeave(Self);
        end;

      if CurrentState <> bsUp then
      begin
        Canvas.Draw(0,0,FUpBmp);
        CurrentState := bsUp;
      end;
    end;
end;

procedure TEnhancedButton.OnMouseDownHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if not Enabled then
    Exit;

  if FUseMask then
    FMouseIsDown := (Button = mbLeft) and HitTest(FMask,clBlack,X - FMaskXOffset,Y - FMaskYOffset)
  else
    FMouseIsDown := (Button = mbLeft) and HitTest(FHighlightedBmp,FTransparentColor,X,Y);

  if FMouseIsDown then
  begin
    Canvas.Draw(0,0,FDownBmp);
    CurrentState := bsDown;

    if Assigned(FOnMouseDown) then
      FOnMouseDown(Self,Button,Shift,X,Y);
  end;
end;

procedure TEnhancedButton.OnMouseUpHandler(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if not Enabled then
    Exit;

  if FMouseIsDown then
  begin
    FMouseIsDown := false;

    if not FUseMask then
    begin
      // Normal mode
      if HitTest(FUpBmp,FTransparentColor,X,Y) then
        Canvas.Draw(0,0,FHighlightedBmp)
      else
        Redraw;

      if HitTest(FDownBmp,FTransparentColor,X,Y) then
      begin
        if Assigned(FOnClick) then
          FOnClick(Self);

        if Assigned(FAction) then
          FAction.Execute;
      end;
    end else
      begin
        // Mask mode
        if HitTest(FMask,clBlack,X - FMaskXOffset,Y - FMaskYOffset) then
        begin
          Canvas.Draw(0,0,FHighlightedBmp);

          if Assigned(FOnClick) then
            FOnClick(Self);

          if Assigned(FAction) then
            FAction.Execute;
        end else
          Redraw;
      end;

    CurrentState := bsUp;

    if Assigned(FOnMouseUp) then
      FOnMouseUp(Self,Button,Shift,X,Y);
  end;
end;

function TEnhancedButton.HitTest(Bmp: TBitmap; TransparentColor : TColor; X,Y : integer) : boolean;
begin
  if (X >= 0) and (X < Bmp.Width) and (Y >= 0) and (Y < Bmp.Height) then
    Result := Bmp.Canvas.Pixels[X,Y] <> TransparentColor
  else
    Result := false;
end;

procedure TEnhancedButton.CMMouseLeave(var AMsg: TMessage);
begin
  if not Enabled then
    Exit;

  CurrentState := bsUp;
  Canvas.Draw(0,0,FUpBmp);

  if MouseIsIn then
    begin
      MouseIsIn := false;
      if Assigned(FOnMouseLeave) then
        FOnMouseLeave(Self);
    end
end;

procedure TEnhancedButton.CMMouseEnter(var AMsg: TMessage);
var
  p       : TPoint;
begin
  if not Enabled then
    Exit;

  p := ScreenToClient(Mouse.CursorPos);
  OnMouseMoveHandler(nil,[],p.X,p.Y);
end;

procedure TEnhancedButton.SetMask(NewMask : TBitmap);
begin
  FMask.Assign(NewMask);
  FMask.TransparentMode := tmAuto;
  FMask.Transparent := true;
  Redraw;
end;

procedure TEnhancedButton.SetShowMask(ShowMask : boolean);
begin
  FShowMask := ShowMask;
  Invalidate;
end;

procedure TEnhancedButton.SetMaskXOffset(NewOffset : integer);
begin
  FMaskXOffset := NewOffset;
  Invalidate;
end;

procedure TEnhancedButton.SetMaskYOffset(NewOffset : integer);
begin
  FMaskYOffset := NewOffset;
  Invalidate;
end;


procedure Register;
begin
  RegisterComponents('Objet', [TEnhancedButton]);
end;

end.
