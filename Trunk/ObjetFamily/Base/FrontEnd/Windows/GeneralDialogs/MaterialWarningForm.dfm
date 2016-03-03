object MaterialWarningForm: TMaterialWarningForm
  Left = 331
  Top = 180
  BorderIcons = []
  BorderStyle = bsDialog
  BorderWidth = 10
  Caption = 'Please Notice!'
  ClientHeight = 103
  ClientWidth = 477
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object TextLabel: TLabel
    Left = 0
    Top = 9
    Width = 409
    Height = 20
    Alignment = taCenter
    Caption = 'The resin in use does not require any special preparations.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object OKButton: TButton
    Left = 192
    Top = 64
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = OKButtonClick
  end
end
