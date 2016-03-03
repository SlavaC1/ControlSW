object PrepareBitDlg: TPrepareBitDlg
  Left = 227
  Top = 173
  BorderStyle = bsDialog
  Caption = 'BIT'
  ClientHeight = 233
  ClientWidth = 306
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  PixelsPerInch = 96
  TextHeight = 13
  object Title: TLabel
    Left = 8
    Top = 8
    Width = 140
    Height = 14
    Caption = 'Please check next items :'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object ItemGroupBox: TGroupBox
    Left = 8
    Top = 40
    Width = 273
    Height = 137
    TabOrder = 0
    object Button1: TButton
      Left = 184
      Top = 169
      Width = 75
      Height = 25
      Caption = 'OK'
      TabOrder = 0
    end
  end
  object OK: TButton
    Left = 150
    Top = 183
    Width = 75
    Height = 25
    Caption = 'OK'
    Enabled = False
    TabOrder = 1
    OnClick = OKClick
  end
  object CancellBttn: TButton
    Left = 69
    Top = 183
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancellBttnClick
  end
end
