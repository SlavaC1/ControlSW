object Form1: TForm1
  Left = 361
  Top = 268
  Width = 231
  Height = 210
  Caption = 'BPE compression unit test'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 32
    Top = 72
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object Label2: TLabel
    Left = 32
    Top = 88
    Width = 32
    Height = 13
    Caption = 'Label2'
  end
  object Label3: TLabel
    Left = 32
    Top = 151
    Width = 26
    Height = 13
    Caption = 'Time:'
  end
  object Label4: TLabel
    Left = 64
    Top = 151
    Width = 32
    Height = 13
    Caption = 'Label4'
  end
  object Label5: TLabel
    Left = 32
    Top = 120
    Width = 37
    Height = 13
    Caption = 'Percent'
  end
  object Label6: TLabel
    Left = 88
    Top = 120
    Width = 32
    Height = 13
    Caption = 'Label6'
  end
  object Button1: TButton
    Left = 80
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Compress'
    TabOrder = 0
    OnClick = Button1Click
  end
  object OpenDialog1: TOpenDialog
    Left = 80
    Top = 72
  end
  object SaveDialog1: TSaveDialog
    Left = 120
    Top = 72
  end
end
