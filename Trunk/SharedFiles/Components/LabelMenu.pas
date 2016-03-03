{********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Popup menu that appears when clicking on a label.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Last upate: 09/11/2003                                           *
 ********************************************************************}

unit LabelMenu;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, StdCtrls, Menus, Graphics;

type
  TLabelMenu = class(TLabel)
  private
    { Private declarations }

    FMenu : TPopupMenu;

    FDownColor,FUpColor,FHighlightedColor : TColor;

    procedure CMMouseEnter(var AMsg: TMessage); message CM_MOUSEENTER;
    procedure CMMouseLeave(var AMsg: TMessage); message CM_MOUSELEAVE;

    procedure MouseDownHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
    procedure MouseUpHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);

  public
    { Public declarations }

    constructor Create(AOwner : Tcomponent); override;
    destructor Destroy; override;

  published
    { Published declarations }

    property Menu : TPopupMenu read FMenu write FMenu;
    property HighlightedColor : TColor read FHighlightedColor write FHighlightedColor;
    property DownColor : TColor read FDownColor write FDownColor;
    property Color : TColor read FUpColor write FUpColor;
  end;

procedure Register;

implementation

uses Forms;

constructor TLabelMenu.Create(AOwner : TComponent);
begin
  inherited;

  FUpColor := Font.Color;
  self.Repaint;
  OnMouseDown := MouseDownHandler;
  OnMouseUp := MouseUpHandler;
end;

destructor TLabelMenu.Destroy;
begin
  FMenu.Free;

  inherited;
end;

procedure TLabelMenu.CMMouseEnter(var AMsg: TMessage);
begin
  Font.Color := FHighlightedColor;
end;

procedure TLabelMenu.CMMouseLeave(var AMsg: TMessage);
begin
  Font.Color := FUpColor;
end;

procedure TLabelMenu.MouseDownHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
var
  P : TPoint;
begin
  if Assigned(FMenu) then
  begin
    Font.Color := FDownColor;
    P := TForm(Owner).ClientToScreen(Point(Left,Top + Height));
    FMenu.Popup(P.X,P.Y);
    Font.Color := FUpColor;
  end;
end;

procedure TLabelMenu.MouseUpHandler(Sender: TObject; Button: TMouseButton;Shift: TShiftState; X, Y: Integer);
begin
  Font.Color := FUpColor;
end;

procedure Register;
begin
  RegisterComponents('Objet', [TLabelMenu]);
end;

end.
