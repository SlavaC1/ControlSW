object ChooseSilentMRWModeDLG: TChooseSilentMRWModeDLG
  Left = 395
  Top = 284
  BorderStyle = bsDialog
  Caption = 'Choose Printing Mode'
  ClientHeight = 174
  ClientWidth = 318
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 7
    Top = 13
    Width = 302
    Height = 13
    Caption = 'Choose the printing mode for using the material you have loaded'
  end
  object OK: TButton
    Left = 6
    Top = 145
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 236
    Top = 145
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = CancelClick
  end
  object GroupBox1: TGroupBox
    Left = 7
    Top = 40
    Width = 305
    Height = 89
    Caption = 'Printing Modes'
    TabOrder = 2
    object SingleModeRadio: TRadioButton
      Left = 8
      Top = 24
      Width = 281
      Height = 17
      Caption = 'Single Material Printing Mode (HS,HQ)'
      Checked = True
      TabOrder = 0
      TabStop = True
    end
    object DigitalModeRadio: TRadioButton
      Left = 8
      Top = 56
      Width = 281
      Height = 17
      Caption = 'Digital Material Printing Mode (DM)'
      TabOrder = 1
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 500
    OnTimer = Timer1Timer
    Left = 200
    Top = 144
  end
end
