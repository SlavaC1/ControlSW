object Form1: TForm1
  Left = 367
  Top = 251
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Region Generator'
  ClientHeight = 301
  ClientWidth = 436
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 180
    Top = 138
    Width = 75
    Height = 25
    Caption = 'Go!'
    TabOrder = 0
    OnClick = Button1Click
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'h'
    Filter = 'Header files|*.h|All files|*.*'
    Left = 296
    Top = 224
  end
  object OpenDialog1: TOpenPictureDialog
    DefaultExt = 'bmp'
    Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
    Left = 336
    Top = 224
  end
end
