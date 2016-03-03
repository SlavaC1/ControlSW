object Form1: TForm1
  Left = 192
  Top = 168
  Width = 355
  Height = 313
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 24
    Top = 188
    Width = 22
    Height = 13
    Caption = 'Path'
  end
  object _pButton_sampleToFile: TButton
    Left = 128
    Top = 240
    Width = 75
    Height = 25
    Caption = 'Capture'
    TabOrder = 0
    OnClick = _pButton_sampleToFileClick
  end
  object _pEditBox_path: TEdit
    Left = 56
    Top = 184
    Width = 121
    Height = 21
    TabOrder = 1
    Text = 'c:\image.bmp'
  end
  object GroupBox1: TGroupBox
    Left = 16
    Top = 24
    Width = 305
    Height = 137
    Caption = 'Settings'
    TabOrder = 2
    object Label1: TLabel
      Left = 16
      Top = 32
      Width = 36
      Height = 13
      Caption = 'Gamma'
    end
    object _pButton_setSettings: TButton
      Left = 214
      Top = 96
      Width = 75
      Height = 25
      Caption = 'Set'
      TabOrder = 0
      OnClick = _pButton_setSettingsClick
    end
    object _pEditBox_gamma: TEdit
      Left = 64
      Top = 24
      Width = 121
      Height = 21
      TabOrder = 1
      Text = '1.0'
    end
  end
  object Button1: TButton
    Left = 264
    Top = 168
    Width = 75
    Height = 25
    Caption = 'Settings'
    TabOrder = 3
    OnClick = Button1Click
  end
end
