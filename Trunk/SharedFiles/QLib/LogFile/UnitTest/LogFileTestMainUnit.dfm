object Form1: TForm1
  Left = 265
  Top = 183
  Width = 696
  Height = 480
  Caption = 'Log file test'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 256
    Top = 152
    Width = 121
    Height = 25
    Caption = 'Write text'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 256
    Top = 112
    Width = 129
    Height = 21
    TabOrder = 1
    Text = 'Edit1'
  end
  object Button2: TButton
    Left = 256
    Top = 184
    Width = 121
    Height = 25
    Caption = 'Write Unformatted'
    TabOrder = 2
    OnClick = Button2Click
  end
end
