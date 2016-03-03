(*              Greatis Form Skin             *)
(*            unit version 1.01.003           *)
(*  Copyright (C) 2001-2002 Greatis Software  *)
(*         web: http://www.greatis.com        *)
(*         e-mail: b-team@greatis.com         *)

unit FormSkin;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, Menus;

type

  EFormSkinException = class(Exception);

  TSkinOption = (soCaption,soBorder,soControls,soSkin,soAutoSize,soClientDrag);
  TSkinOptions = set of TSkinOption;

  THitArea = (
    haNone,
    haClient,
    haCaptionBar,
    haSysMenu,
    haMinimizeButton,
    haMaximizeButton,
    haCloseButton,
    haTopBorder,
    haBottomBorder,
    haLeftBorder,
    haRightBorder,
    haTopLeftCorner,
    haTopRightCorner,
    haBottomLeftCorner,
    haBottomRightCorner,
    haGrowBox);

  THitAreaEvent = procedure(Sender: TObject; X,Y: Integer; var Area: THitArea) of object;
  TTransparencyEvent = procedure(Sender: TObject; X,Y: Integer; var Transparent: Boolean) of object;
  TControlTransparencyEvent = procedure(Sender: TObject; Control: TControl; var Transparent: Boolean) of object;

  TCustomFormSkin = class(TComponent)
  private
    { Private declarations }
    FRegion: HRGN;
    FLockSizeMessages: Boolean;
    FOptions: TSkinOptions;
    FActive: Boolean;
    FPopupMenu: TPopupMenu;
    FDefaultProc: TFarProc;
    FHookProc: Pointer;
    FOnHitArea: THitAreaEvent;
    FOnTransparency: TTransparencyEvent;
    FOnControlTransparency: TControlTransparencyEvent;
    procedure SetOptions(const Value: TSkinOptions);
    procedure SetActive(const Value: Boolean);
    function BorderHeight: Integer;
    function BorderWidth: Integer;
    procedure HookProc(var Message: TMessage);
    function HitAreaToHitTest(Value: THitArea): Integer;
    function HitTestToHitArea(Value: Integer): THitArea;

  protected
    { Protected declarations }
    procedure HitArea(X,Y: Integer; var Area: THitArea); virtual;
    function CreateRegion: HRGN; virtual;
    function CreateCaptionRegion: HRGN; virtual;
    function CreateBorderRegion: HRGN; virtual;
    function CreateControlsRegion: HRGN; virtual;
    function CreateSkinRegion: HRGN; virtual;
    function GetSkinWidth: Integer; virtual;
    function GetSkinHeight: Integer; virtual;
    function IsTransparent(X,Y: Integer): Boolean; virtual;
    function IsTransparentControl(Control: TControl): Boolean; virtual;
    property Options: TSkinOptions read FOptions write SetOptions;
    property Active: Boolean read FActive write SetActive;
    property PopupMenu: TPopupMenu read FPopupMenu write FPopupMenu;
    property OnHitArea: THitAreaEvent read FOnHitArea write FOnHitArea;
    property OnTransparency: TTransparencyEvent read FOnTransparency write FOnTransparency;
    property OnControlTransparency: TControlTransparencyEvent read FOnControlTransparency write FOnControlTransparency;
  public
    { Public declarations }
    destructor Destroy; override;
    procedure Update; virtual;
  published
    { Published declarations }
  end;

  TSimpleFormSkin = class(TCustomFormSkin)
  published
    { Published declarations }
    property Options;
    property Active;
    property PopupMenu;
    property OnHitArea;
    property OnTransparency;
    property OnControlTransparency;
  end;

  TBitmapFormSkin = class(TCustomFormSkin)
  private
    { Private declarations }
    FDesignBrush: TBrush;
    FTransparentColor: TColor;
    FSkin: TBitmap;
    FPreview: Boolean;

    FMouseDownFlag : Boolean;
    FMouseDownPoint : TPoint;
    FFormStartDragPoint : TPoint;
    FPreCompiledBuffer : pByte;
    FPreCompiledBufferSize : cardinal;

    procedure SetTransparentColor(const Value: TColor);
    procedure SetSkin(const Value: TBitmap);
    procedure SetPreview(const Value: Boolean);
    procedure AssignBrush;
    procedure RestoreBrush;
    procedure SkinChange(Sender: TObject);

    procedure FormPaintHandler(Sender : TObject);
    procedure FormMouseDownHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
    procedure FormMouseUpHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
    procedure FormMouseMoveHandler(Sender: TObject; Shift: TShiftState; X, Y: Integer);

  protected
    { Protected declarations }
    function GetSkinWidth: Integer; override;
    function GetSkinHeight: Integer; override;
    function IsTransparent(X,Y: Integer): Boolean; override;

    function CreateSkinRegion: HRGN; override;
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    procedure Update; override;

    procedure SetPreCompiledRegion(Buffer : pByte; BufferSize : cardinal);

  published
    { Published declarations }
    property TransparentColor: TColor read FTransparentColor write SetTransparentColor;
    property Skin: TBitmap read FSkin write SetSkin;
    property Preview: Boolean read FPreview write SetPreview;
    property Options;
    property Active;
    property PopupMenu;
    property OnHitArea;
    property OnControlTransparency;
  end;

procedure Register;

implementation

procedure TCustomFormSkin.SetOptions(const Value: TSkinOptions);
begin
  if FOptions<>Value then
  begin
    FOptions:=Value;
    Update;
  end;
end;

procedure TCustomFormSkin.SetActive(const Value: Boolean);
begin
  if FActive<>Value then
  begin
    FActive:=Value;
    if Assigned(Owner) then
      if FActive then
      begin
        FDefaultProc:=TFarProc(GetWindowLong(TForm(Owner).Handle,GWL_WNDPROC));
        FHookProc:=MakeObjectInstance(HookProc);
        SetWindowLong(TForm(Owner).Handle,GWL_WNDPROC,Integer(FHookProc));
      end
      else
      begin
        if Assigned(FDefaultProc) then SetWindowLong(TForm(Owner).Handle,GWL_WNDPROC,Integer(FDefaultProc));
        if Assigned(FHookProc) then FreeObjectInstance(FHookProc);
        FDefaultProc:=nil;
        FHookProc:=nil;
      end;
    if not (csDestroying in ComponentState) then Update;
  end;
end;

function TCustomFormSkin.BorderHeight: Integer;
begin
  case TForm(Owner).BorderStyle of
    bsSingle,bsDialog,bsToolWindow: Result:=GetSystemMetrics(SM_CYFIXEDFRAME);
    bsSizeToolWin,bsSizeable: Result:=GetSystemMetrics(SM_CYSIZEFRAME);
  else Result:=0;
  end;
end;

function TCustomFormSkin.BorderWidth: Integer;
begin
  case TForm(Owner).BorderStyle of
    bsSingle,bsDialog,bsToolWindow: Result:=GetSystemMetrics(SM_CXFIXEDFRAME);
    bsSizeToolWin,bsSizeable: Result:=GetSystemMetrics(SM_CXSIZEFRAME);
  else Result:=0;
  end;
end;

procedure TCustomFormSkin.HitArea(X,Y: Integer; var Area: THitArea);
begin
  if Assigned(FOnHitArea) then FOnHitArea(Self,X,Y,Area);
end;

function TCustomFormSkin.CreateRegion: HRGN;
var
  RGN: HRGN;
begin
  Result:=0;
  if soCaption in FOptions then
  begin
    if Result=0 then Result:=CreateRectRgn(0,0,0,0);
    RGN:=CreateCaptionRegion;
    try
      CombineRgn(Result,Result,RGN,RGN_OR);
    finally
      DeleteObject(RGN);
    end;
  end;
  if soBorder in FOptions then
  begin
    if Result=0 then Result:=CreateRectRgn(0,0,0,0);
    RGN:=CreateBorderRegion;
    try
      CombineRgn(Result,Result,RGN,RGN_OR);
    finally
      DeleteObject(RGN);
    end;
  end;
  if soSkin in FOptions then
  begin
    if Result=0 then Result:=CreateRectRgn(0,0,0,0);
    RGN:=CreateSkinRegion;
    try
      CombineRgn(Result,Result,RGN,RGN_OR);
    finally
      DeleteObject(RGN);
    end;
  end;
  if soControls in FOptions then
  begin
    if Result=0 then Result:=CreateRectRgn(0,0,0,0);
    RGN:=CreateControlsRegion;
    try
      CombineRgn(Result,Result,RGN,RGN_OR);
    finally
      DeleteObject(RGN);
    end;
  end;
end;

function TCustomFormSkin.CreateCaptionRegion: HRGN;
var
  R: TRect;
begin
  with TForm(Owner),R do
    if BorderStyle=bsNone then Result:=0
    else
    begin
      ZeroMemory(@R,SizeOf(R));
      if BorderStyle in [bsToolWindow,bsSizeToolWin] then
        Bottom:=Pred(GetSystemMetrics(SM_CYSMCAPTION))
      else Bottom:=Pred(GetSystemMetrics(SM_CYCAPTION));
      Right:=Width;
      InflateRect(R,-Self.BorderWidth,0);
      OffsetRect(R,0,BorderHeight);
      Result:=CreateRectRgn(Left,Top,Right,Bottom);
    end;
end;

function TCustomFormSkin.CreateBorderRegion: HRGN;
var
  RGN: HRGN;
begin
  with TForm(Owner) do
  begin
    Result:=CreateRectRgn(0,0,Width,Height);
    RGN:=CreateRectRgn(Self.BorderWidth,BorderHeight,Width-Self.BorderWidth,Height-BorderHeight);
    try
      CombineRgn(Result,Result,RGN,RGN_XOR);
    finally
      DeleteObject(RGN);
    end;
  end;
end;

function TCustomFormSkin.CreateControlsRegion: HRGN;
var
  RGN: HRGN;
  i: Integer;
begin
  with TForm(Owner) do
  begin
    Result:=CreateRectRgn(0,0,0,0);
    for i:=0 to Pred(ControlCount) do
      if not IsTransparentControl(Controls[i]) then
        with Controls[i].BoundsRect do
        begin
          RGN:=CreateRectRgn(Left,Top,Right,Bottom);
          try
            with TForm(Self.Owner),ClientOrigin do OffsetRgn(RGN,X-Left,Y-Top);
            CombineRgn(Result,Result,RGN,RGN_OR);
          finally
            DeleteObject(RGN);
          end;
        end;
  end;
end;

function TCustomFormSkin.CreateSkinRegion: HRGN;
var
  RGN: HRGN;
  X,XStart,Y: Integer;
  TRP: Boolean;
begin
  Result:=CreateRectRgn(0,0,0,0);
  for Y:=0 to Pred(GetSkinHeight) do
  begin
    XStart:=0;
    TRP:=IsTransparent(0,Y);
    for X:=0 to Pred(GetSkinWidth) do
    begin
      if IsTransparent(X,Y)<>TRP then
      begin
        if TRP then XStart:=X
        else
        begin
          RGN:=CreateRectRgn(XStart,Y,X,Succ(Y));
          try
            with TForm(Owner),ClientOrigin do OffsetRgn(RGN,X-Left,Y-Top);
            CombineRgn(Result,Result,RGN,RGN_OR);
          finally
            DeleteObject(RGN);
          end;
        end;
        TRP:=IsTransparent(X,Y);
      end;
    end;
    if not TRP then
    begin
      RGN:=CreateRectRgn(XStart,Y,GetSkinWidth,Succ(Y));
      try
        with TForm(Owner),ClientOrigin do OffsetRgn(RGN,X-Left,Y-Top);
        CombineRgn(Result,Result,RGN,RGN_OR);
      finally
        DeleteObject(RGN);
      end;
    end;
  end;
end;

function TCustomFormSkin.GetSkinWidth: Integer;
begin
  if Assigned(Owner) then Result:=TForm(Owner).ClientWidth
  else Result:=0;
end;

function TCustomFormSkin.GetSkinHeight: Integer;
begin
  if Assigned(Owner) then Result:=TForm(Owner).ClientHeight
  else Result:=0;
end;

function TCustomFormSkin.IsTransparent(X,Y: Integer): Boolean;
begin
  if Assigned(FOnTransparency) then FOnTransparency(Self,X,Y,Result);
end;

function TCustomFormSkin.IsTransparentControl(Control: TControl): Boolean;
begin
  Result:=False;
  if Assigned(FOnControlTransparency) then
    FOnControlTransparency(Self,Control,Result);
end;

procedure TCustomFormSkin.HookProc(var Message: TMessage);

var
  Command: Word;
  HA: THitArea;
  P: TPoint;

  function CallDefault: Integer;
  begin
    with Message do
      CallDefault:=CallWindowProc(FDefaultProc,TForm(Owner).Handle,Msg,wParam,lParam);
  end;

begin
  if Assigned(Owner) then
    with Message do
      case Msg of
        WM_NCHITTEST:
          if not (csDesigning in ComponentState) then
          begin
            Result:=CallDefault;
            HA:=HitTestToHitArea(Result);
            P:=TForm(Owner).ScreenToClient(Point(LoWord(lParam),HiWord(lParam)));
            HitArea(P.X,P.Y,HA);
            Result:=HitAreaToHitTest(HA);
            //if ((Result=HTCLIENT) or (Result=HTNOWHERE)) and (soClientDrag in FOptions) then Result:=HTCAPTION
          end
          else Result:=CallDefault;
        WM_NCLBUTTONDOWN:
          if not (csDesigning in ComponentState) then
          begin
            case wParam of
              HTSYSMENU:
              begin
                if Assigned(FPopupMenu) then FPopupMenu.Popup(LoWord(lParam),HiWord(lParam));
                Exit;
              end;
              HTREDUCE: Command:=SC_MINIMIZE;
              HTZOOM: Command:=SC_MAXIMIZE;
              HTCLOSE: Command:=SC_CLOSE;
            else
            begin
              Result:=CallDefault;
              Exit;
            end;
            end;
            SendMessage(TForm(Owner).Handle,WM_SYSCOMMAND,Command,lParam);
          end
          else Result:=CallDefault;
        WM_GETMINMAXINFO:
          if not (csLoading in ComponentState) and (soAutoSize in FOptions) and not FLockSizeMessages then
            with PMinMaxInfo(lParam)^,TForm(Owner) do
            begin
              ptMinTrackSize.X:=Width;
              ptMinTrackSize.Y:=Height;
              ptMaxTrackSize:=ptMinTrackSize;
            end
          else Result:=CallDefault;
        WM_SIZE:
        begin
          Result:=CallDefault;
          if not FLockSizeMessages then Update;
        end;
        // test start
        WM_ERASEBKGND:
        begin
          //TForm(Owner).Canvas.Draw(0,0,TBitmapFormSkin(Self).FSkin);
        end;
        // test end
      else Result:=CallDefault;
      end;
end;

function TCustomFormSkin.HitAreaToHitTest(Value: THitArea): Integer;
begin
  case Value of
    haClient: Result:=HTCLIENT;
    haCaptionBar: Result:=HTCAPTION;
    haSysMenu: Result:=HTSYSMENU;
    haMinimizeButton: Result:=HTREDUCE;
    haMaximizeButton: Result:=HTZOOM;
    haCloseButton: Result:=HTCLOSE;
    haTopBorder: Result:=HTTOP;
    haBottomBorder: Result:=HTBOTTOM;
    haLeftBorder: Result:=HTLEFT;
    haRightBorder: Result:=HTRIGHT;
    haTopLeftCorner: Result:=HTTOPLEFT;
    haTopRightCorner: Result:=HTTOPRIGHT;
    haBottomLeftCorner: Result:=HTBOTTOMLEFT;
    haBottomRightCorner: Result:=HTBOTTOMRIGHT;
    haGrowBox: Result:=HTGROWBOX;
  else Result:=HTNOWHERE;
  end;
end;

function TCustomFormSkin.HitTestToHitArea(Value: Integer): THitArea;
begin
  case Value of
    HTCLIENT: Result:=haClient;
    HTCAPTION: Result:=haCaptionBar;
    HTSYSMENU: Result:=haSysMenu;
    HTREDUCE: Result:=haMinimizeButton;
    HTZOOM: Result:=haMaximizeButton;
    HTCLOSE: Result:=haCloseButton;
    HTTOP: Result:=haTopBorder;
    HTBOTTOM: Result:=haBottomBorder;
    HTLEFT: Result:=haLeftBorder;
    HTRIGHT: Result:=haRightBorder;
    HTTOPLEFT: Result:=haTopLeftCorner;
    HTTOPRIGHT: Result:=haTopRightCorner;
    HTBOTTOMLEFT: Result:=haBottomLeftCorner;
    HTBOTTOMRIGHT: Result:=haBottomRightCorner;
    HTGROWBOX: Result:=haGrowBox;
  else Result:=haNone;
  end;
end;

destructor TCustomFormSkin.Destroy;
begin
  Active:=False;
  inherited;
end;

procedure TCustomFormSkin.Update;
begin
  if Assigned(Owner) and (Owner is TForm) then
    if not (csDesigning in ComponentState) then
    begin
      if FActive then
      begin
        DeleteObject(FRegion);
        if (soSkin in FOptions) and (soAutoSize in FOptions) then
        begin
          FLockSizeMessages:=True;
          try
            with TForm(Owner) do
            begin
              ClientHeight:=GetSkinHeight;
              ClientWidth:=GetSkinWidth;
            end;
          finally
            FLockSizeMessages:=False;
          end;
        end;
        FRegion:=CreateRegion;
        if FRegion<>0 then SetWindowRgn(TForm(Owner).Handle,FRegion,True);
      end
      else
      begin
        SetWindowRgn(TForm(Owner).Handle,0,True);
        DeleteObject(FRegion);
        FRegion:=0;
      end
    end
    else
    begin
      if (soSkin in FOptions) and (soAutoSize in FOptions) then
      begin
        FLockSizeMessages:=True;
        try
          with TForm(Owner) do
          begin
            ClientHeight:=GetSkinHeight;
            ClientWidth:=GetSkinWidth;
          end;
        finally
          FLockSizeMessages:=False;
        end;
      end;
    end
  else EFormSkinException.Create('Owner of TCustomFormSkin must be derived from TForm.');
end;

procedure TBitmapFormSkin.SetTransparentColor(const Value: TColor);
begin
  if FTransparentColor<>Value then
  begin
    FTransparentColor:=Value;
    Update;
  end;
end;

procedure TBitmapFormSkin.SetSkin(const Value: TBitmap);
begin
  FSkin.Assign(Value);
  Update;
end;

procedure TBitmapFormSkin.SetPreview(const Value: Boolean);
begin
  if FPreview<>Value then
  begin
    FPreview:=Value;
    if (csDesigning in ComponentState) and Assigned(Owner) then
    begin
      if FPreview then AssignBrush
      else RestoreBrush;
      TForm(Owner).Invalidate;
    end;
  end;
end;

procedure TBitmapFormSkin.AssignBrush;
begin
  with TForm(Owner) do
  begin
    FDesignBrush.Assign(Brush);
    Brush.Bitmap:=TBitmap.Create;
    Brush.Bitmap.Assign(FSkin);
    Brush.Bitmap.Width:=ClientWidth;
    Brush.Bitmap.Height:=ClientHeight;
  end;
end;

procedure TBitmapFormSkin.RestoreBrush;
begin
  with TForm(Owner) do
  begin
    Brush.Bitmap.Free;
    Brush.Assign(FDesignBrush);
  end;
end;

procedure TBitmapFormSkin.SkinChange(Sender: TObject);
begin
  Update;
end;

function TBitmapFormSkin.GetSkinWidth: Integer;
begin
  if FSkin.Empty then Result:=TForm(Owner).ClientWidth
  else Result:=FSkin.Width;
end;

function TBitmapFormSkin.GetSkinHeight: Integer;
begin
  if FSkin.Empty then Result:=TForm(Owner).ClientHeight
  else Result:=FSkin.Height;
end;

function TBitmapFormSkin.IsTransparent(X,Y: Integer): Boolean;
begin
  Result:=FSkin.Canvas.Pixels[X,Y]=FTransparentColor;
end;

constructor TBitmapFormSkin.Create(AOwner: TComponent);
begin
  inherited;
  FSkin:=TBitmap.Create;
  FSkin.OnChange:=SkinChange;
  FDesignBrush:=TBrush.Create;
  FPreCompiledBuffer := nil;

  if Assigned(Owner) then
    if Owner is TForm then
    begin
      TForm(Owner).OnPaint := FormPaintHandler;
      TForm(Owner).OnMouseDown := FormMouseDownHandler;
      TForm(Owner).OnMouseUp := FormMouseUpHandler;
      TForm(Owner).OnMouseMove := FormMouseMoveHandler;
    end;

  Update;
end;

destructor TBitmapFormSkin.Destroy;
begin
  FSkin.Free;
  if FRegion<>0 then DeleteObject(FRegion);
  FRegion:=0;
  FDesignBrush.Free;
  inherited;
end;

procedure TBitmapFormSkin.Update;
begin
  inherited;
  if (FPreview and (csDesigning in ComponentState)) or
    (not (csDesigning in ComponentState)) and (soSkin in FOptions) and Assigned(FSkin) then
  begin
    RestoreBrush;
    AssignBrush;
    TForm(Owner).Invalidate;
  end;
end;

procedure TBitmapFormSkin.FormPaintHandler(Sender : TObject);
begin
  TForm(Owner).Canvas.Draw(0,0,TBitmapFormSkin(Self).FSkin);
end;

procedure TBitmapFormSkin.FormMouseDownHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
begin
  if not (soClientDrag in FOptions) then
    Exit;

  if Button = mbLeft then
  begin
    FMouseDownFlag := true;
    FMouseDownPoint := TForm(Owner).ClientToScreen(Point(X,Y));
    FFormStartDragPoint := Point(TForm(Owner).Left,TForm(Owner).Top);
  end;
end;

procedure TBitmapFormSkin.FormMouseUpHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
begin
  if not (soClientDrag in FOptions) then
    Exit;

  if Button = mbLeft then
    FMouseDownFlag := false;
end;

procedure TBitmapFormSkin.FormMouseMoveHandler(Sender: TObject; Shift: TShiftState; X, Y: Integer);
var
  P : TPoint;
begin
  if not (soClientDrag in FOptions) then
    Exit;

  if FMouseDownFlag then
  begin
    P := TForm(Owner).ClientToScreen(Point(X,Y));
    TForm(Owner).Left := FFormStartDragPoint.X + P.X - FMouseDownPoint.X;
    TForm(Owner).Top := FFormStartDragPoint.Y + P.Y - FMouseDownPoint.Y;
  end;
end;

procedure TBitmapFormSkin.SetPreCompiledRegion(Buffer : pByte; BufferSize : cardinal);
begin
  FPreCompiledBuffer := Buffer;
  FPreCompiledBufferSize := BufferSize;
end;

function TBitmapFormSkin.CreateSkinRegion: HRGN;
var
  XFORM : TXFORM;
begin
  if FPreCompiledBuffer = nil then
    Result := inherited CreateSkinRegion
  else
  begin
    XFORM.eM11 := 1;
    XFORM.eM12 := 0;
    XFORM.eM21 := 0;
    XFORM.eM22 := 1;

    with TForm(Self.Owner),ClientOrigin do
    begin
      XFORM.eDx := X - Left;
      XFORM.eDy := Y - Top;
    end;

    Result := ExtCreateRegion(@XFORM,FPreCompiledBufferSize,pRgnData(FPreCompiledBuffer)^);
  end;
end;



procedure Register;
begin
  RegisterComponents('Greatis', [TSimpleFormSkin, TBitmapFormSkin]);
end;

initialization
  {$IFDEF FORMSKINTRIAL}
  MessageBox(
    0,
    'You are using trial version of Greatis Form Skin.'#13+
      'To get registered version visit'#13+
      'http://www.greatis.com/formskinbuy.htm',
    'Greatis Form Skin - Trial Version',
    MB_OK or MB_ICONEXCLAMATION);
  {$ENDIF}
end.
