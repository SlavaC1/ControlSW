object Form1: TForm1
  Left = 196
  Top = 204
  Width = 441
  Height = 308
  Caption = 'BMP file read test'
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
    Left = 108
    Top = 91
    Width = 210
    Height = 57
    Caption = 'Convert 1Bit BMP file to 8Bit Raw file'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 112
    Top = 184
    Width = 57
    Height = 21
    TabOrder = 1
    Text = '0'
  end
  object UpDown1: TUpDown
    Left = 169
    Top = 184
    Width = 15
    Height = 21
    Associate = Edit1
    Min = 0
    Position = 0
    TabOrder = 2
    Wrap = False
  end
  object OpenDialog1: TOpenDialog
    Left = 80
    Top = 40
  end
  object SaveDialog1: TSaveDialog
    Left = 120
    Top = 40
  end
end
