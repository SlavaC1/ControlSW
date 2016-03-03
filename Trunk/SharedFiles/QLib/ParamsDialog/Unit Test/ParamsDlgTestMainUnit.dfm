object MainForm: TMainForm
  Left = 279
  Top = 212
  Width = 377
  Height = 257
  Caption = 'Parameters Dialog Unit Test'
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
    Left = 72
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Open Dialog'
    TabOrder = 0
    OnClick = Button1Click
  end
  object RadioGroup1: TRadioGroup
    Left = 200
    Top = 48
    Width = 137
    Height = 73
    Caption = 'Buttons Mode'
    ItemIndex = 0
    Items.Strings = (
      'OK / Cancel'
      'Apply / Close')
    TabOrder = 1
  end
end
