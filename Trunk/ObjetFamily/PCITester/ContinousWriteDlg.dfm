object ContinousWriteForm: TContinousWriteForm
  Left = 392
  Top = 299
  BorderStyle = bsDialog
  Caption = 'Continous Write'
  ClientHeight = 102
  ClientWidth = 224
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 40
    Top = 24
    Width = 145
    Height = 13
    Caption = 'Continous write is in progress...'
  end
  object StopButton: TButton
    Left = 75
    Top = 62
    Width = 75
    Height = 25
    Caption = 'Stop'
    TabOrder = 0
    OnClick = StopButtonClick
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 1
    OnTimer = Timer1Timer
    Left = 40
    Top = 56
  end
end
