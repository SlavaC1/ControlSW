{********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: EnhancedProgressBar component                            *
 * Module Description: A graphic progress bar component.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Last upate: 09/11/2003                                           *
 ********************************************************************}

unit EnhancedProgressBar;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Graphics;

type
  TEnhancedProgressBarOrientation = (pbHorizontal,pbVertical);

  TEnhancedProgressBar = class(TGraphicControl)
  private
    // Progress background bitmap
    FBodyBitmap : TBitmap;

    // Progress icon bitmap 
    FProgressBitmap : TBitmap;

    FTransparentMode : TTransparentMode;
    FTransparentColor : TColor;

    FMin,FMax,FPosition : integer;

    // The progress bar can be horizontal or vertical
    FOrientation : TEnhancedProgressBarOrientation;

    FMaxProgressBitmaps : integer;

    FReverseDirection : boolean;

    FXOffset,FYOffset : integer;

    procedure SetOrientation(NewValue : TEnhancedProgressBarOrientation);

    procedure SetTransparentMode(NewMode : TTransparentMode);
    procedure SetTransparentColor(NewColor : TColor);

    function GetBodyBitmapTransparent : boolean;
    procedure SetBodyBitmapTransparent(Transparent : boolean);
    function GetProgressBitmapTransparent : boolean;
    procedure SetProgressBitmapTransparent(Transparent : boolean);

    procedure SetBodyBitmap(NewBitmap : TBitmap);
    procedure SetProgressBitmap(NewBitmap : TBitmap);

    procedure SetMin(NewValue : integer);
    procedure SetMax(NewValue : integer);
    procedure SetPosition(NewValue : integer);

    procedure SetMaxProgressBitmaps(NewValue : integer);    

    procedure Redraw;
    function HasDimension(Bmp : TBitmap) : boolean;

    procedure SetReverseDirection(Direction : boolean);

    procedure SetXOffset(NewValue : integer);
    procedure SetYOffset(NewValue : integer);

  public
    { Public declarations }

    procedure Paint; override;

    constructor Create(AOwner : TComponent); override;
    destructor Destroy; override;

  published
    { Published declarations }

    property BodyBitmap : TBitmap read FBodyBitmap write SetBodyBitmap;
    property ProgressBitmap : TBitmap read FProgressBitmap write SetProgressBitmap;

    property TransparentMode : TTransparentMode read FTransparentMode write SetTransparentMode;
    property TransparentColor : TColor read FTransparentColor write SetTransparentColor;
    property BodyBitmapTransparent : boolean read GetBodyBitmapTransparent write SetBodyBitmapTransparent;
    property ProgressBitmapTransparent : boolean read GetProgressBitmapTransparent write SetProgressBitmapTransparent;

    property Min : integer read FMin write SetMin;
    property Max : integer read FMax write SetMax;
    property Position : integer read FPosition write SetPosition;

    property Orientation : TEnhancedProgressBarOrientation read FOrientation write SetOrientation;
    property MaxProgressBitmaps : integer read FMaxProgressBitmaps write SetMaxProgressBitmaps;

    property ReverseDirection : boolean read FReverseDirection write SetReverseDirection;

    property XOffset : integer read FXOffset write SetXOffset;
    property YOffset : integer read FYOffset write SetYOffset;
  end;

procedure Register;

implementation

{$R ENHANCEDPROGRESSBAR.DCR}

constructor TEnhancedProgressBar.Create(AOwner : TComponent);
begin
  inherited;

  Width := 60;
  Height := 16;

  FBodyBitmap := TBitmap.Create;
  FProgressBitmap := TBitmap.Create;
  FMax := 100;
  FMaxProgressBitmaps := 10;
end;

destructor TEnhancedProgressBar.Destroy;
begin
  FBodyBitmap.Free;
  FProgressBitmap.Free;

  inherited;
end;

procedure TEnhancedProgressBar.SetTransparentMode(NewMode : TTransparentMode);
begin
  FTransparentMode := NewMode;

  FBodyBitmap.TransparentMode     := NewMode;
  FProgressBitmap.TransparentMode := NewMode;
end;

procedure TEnhancedProgressBar.SetTransparentColor(NewColor : TColor);
begin
  FTransparentColor := NewColor;

  FBodyBitmap.TransparentColor     := NewColor;
  FProgressBitmap.TransparentColor := NewColor;
end;

function TEnhancedProgressBar.GetBodyBitmapTransparent : boolean;
begin
  Result := FBodyBitmap.Transparent;
end;

procedure TEnhancedProgressBar.SetBodyBitmapTransparent(Transparent : boolean);
begin
  FBodyBitmap.Transparent := Transparent;
end;

function TEnhancedProgressBar.GetProgressBitmapTransparent : boolean;
begin
  Result := FProgressBitmap.Transparent;
end;

procedure TEnhancedProgressBar.SetProgressBitmapTransparent(Transparent : boolean);
begin
  FProgressBitmap.Transparent := Transparent;
end;

procedure TEnhancedProgressBar.SetBodyBitmap(NewBitmap : TBitmap);
begin
  FBodyBitmap.Assign(NewBitmap);
  Redraw;
end;

procedure TEnhancedProgressBar.SetProgressBitmap(NewBitmap : TBitmap);
begin
  FProgressBitmap.Assign(NewBitmap);
  Redraw;
end;

procedure TEnhancedProgressBar.Redraw;
var
  Gain : single;
  N,i,Step,j : integer;
begin
  if csLoading in ComponentState then
    Exit;

  // Draw the background
  if HasDimension(FBodyBitmap) then
    Canvas.Draw(0,0,FBodyBitmap);

  if FMax = FMin then
    Exit;

  // Draw the progress
  Gain := FMaxProgressBitmaps / (FMax - FMin);
  N := Round((FPosition - FMin) * Gain);

  if FOrientation = pbHorizontal then
  begin
    if FReverseDirection then
    begin
      Step := -FProgressBitmap.Width;
      j := FBodyBitmap.Width + Step;
    end else
      begin
        Step := FProgressBitmap.Width;
        j := 0;
      end;

    for i := 1 to N do
    begin
      Canvas.Draw(FXOffset + j,FYOffset,FProgressBitmap);
      Inc(j,Step);
    end;
  end else
    begin
      if FReverseDirection then
      begin
        Step := -FProgressBitmap.Height;
        j := FBodyBitmap.Height + Step;
      end else
        begin
          Step := FProgressBitmap.Height;
          j := 0;
        end;

      for i := 1 to N do
      begin
        Canvas.Draw(FXOffset,FYOffset + j,FProgressBitmap);
        Inc(j,Step);
      end;
    end;
end;

function TEnhancedProgressBar.HasDimension(Bmp : TBitmap) : boolean;
begin
  Result := (Bmp.Width > 0) and (Bmp.Height > 0);
end;

procedure TEnhancedProgressBar.SetMin(NewValue : integer);
begin
  if FMin = NewValue then
    Exit;

  if NewValue < FMax then
    FMin := NewValue;

  Redraw;
end;

procedure TEnhancedProgressBar.SetMax(NewValue : integer);
begin
  if FMax = NewValue then
    Exit;

  if NewValue > FMin then
    FMax := NewValue;

  Redraw;
end;

procedure TEnhancedProgressBar.SetPosition(NewValue : integer);
begin
  if FPosition = NewValue then
    Exit;

  // Clip value between min and max
  if NewValue > FMax then
    FPosition := FMax
  else if NewValue < FMin then
    FPosition := FMin
  else
    FPosition := NewValue;

   Redraw;
end;

procedure TEnhancedProgressBar.Paint;
begin
  Redraw;
end;

procedure TEnhancedProgressBar.SetOrientation(NewValue : TEnhancedProgressBarOrientation);
begin
  if FOrientation = NewValue then
    Exit;

  FOrientation := NewValue;
  Redraw;
end;

procedure TEnhancedProgressBar.SetMaxProgressBitmaps(NewValue : integer);
begin
  if MaxProgressBitmaps = NewValue then
    Exit;

  FMaxProgressBitmaps := NewValue;  
  Redraw;
end;

procedure TEnhancedProgressBar.SetReverseDirection(Direction : boolean);
begin
  FReverseDirection := Direction;
  Redraw;
end;

procedure TEnhancedProgressBar.SetXOffset(NewValue : integer);
begin
  FXOffset := NewValue;
  Redraw;
end;

procedure TEnhancedProgressBar.SetYOffset(NewValue : integer);
begin
  FYOffset := NewValue;
  Redraw;  
end;

procedure Register;
begin
  RegisterComponents('Objet', [TEnhancedProgressBar]);
end;

end.
