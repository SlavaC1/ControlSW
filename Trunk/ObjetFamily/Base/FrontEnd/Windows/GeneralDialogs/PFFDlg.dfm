object PFFJobForm: TPFFJobForm
  Left = 0
  Top = 0
  Caption = 'PFF'
  ClientHeight = 175
  ClientWidth = 462
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object edtFolderName: TEdit
    Tag = 1
    Left = 64
    Top = 58
    Width = 297
    Height = 21
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object btnOpenFolderSelection: TButton
    Tag = 1
    Left = 374
    Top = 58
    Width = 21
    Height = 21
    Caption = '...'
    TabOrder = 1
    OnClick = btnOpenFolderSelectionClick
  end
  object txt1: TStaticText
    Left = 64
    Top = 35
    Width = 304
    Height = 17
    Caption = 'Please select the folder in which the parameters and BMP files:'
    TabOrder = 2
  end
  object btnCancelButton: TBitBtn
    Left = 232
    Top = 129
    Width = 75
    Height = 25
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    Kind = bkCancel
  end
  object btnOkButton: TBitBtn
    Left = 152
    Top = 129
    Width = 75
    Height = 25
    Caption = 'Ok'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    Kind = bkOK
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'bmp'
    Filter = 'Bitmaps (*.bmp)|*.bmp|All files|*.*'
    Left = 384
    Top = 136
  end
end
