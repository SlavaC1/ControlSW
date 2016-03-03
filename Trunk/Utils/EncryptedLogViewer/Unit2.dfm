object AboutForm: TAboutForm
  Left = 468
  Top = 280
  Width = 330
  Height = 223
  Caption = 'About EncryptedLogViewer'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 73
    Top = 16
    Width = 176
    Height = 25
    Alignment = taCenter
    Caption = 'Objet Geometries Ltd'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label1: TLabel
    Left = 64
    Top = 72
    Width = 139
    Height = 13
    Caption = 'EncryptedLogViewer Version:'
  end
  object VersionLabel: TLabel
    Left = 232
    Top = 72
    Width = 18
    Height = 13
    Caption = '???'
  end
  object Button1: TButton
    Left = 124
    Top = 160
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = Button1Click
  end
end
