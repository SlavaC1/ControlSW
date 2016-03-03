object BITProgressForm: TBITProgressForm
  Left = 351
  Top = 187
  BorderIcons = []
  BorderStyle = bsSizeToolWin
  Caption = 'Tests progress'
  ClientHeight = 298
  ClientWidth = 399
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreateOrShow
  OnShow = FormCreateOrShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 7
    Top = 8
    Width = 385
    Height = 161
    Caption = 'Current Test'
    TabOrder = 0
    object Label1: TLabel
      Left = 18
      Top = 56
      Width = 97
      Height = 13
      Caption = 'Current Step/Status:'
    end
    object TestProgressGauge: TGauge
      Left = 18
      Top = 122
      Width = 350
      Height = 26
      ForeColor = clBlue
      Progress = 0
    end
    object Label2: TLabel
      Left = 18
      Top = 106
      Width = 68
      Height = 13
      Caption = 'Test Progress:'
    end
    object CurrentTestPanel: TPanel
      Left = 18
      Top = 22
      Width = 350
      Height = 26
      BevelOuter = bvLowered
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
    object CurrentStepPanel: TPanel
      Left = 18
      Top = 72
      Width = 350
      Height = 26
      BevelOuter = bvLowered
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
    end
  end
  object GroupBox2: TGroupBox
    Left = 7
    Top = 178
    Width = 385
    Height = 73
    Caption = 'Total progress'
    TabOrder = 1
    object TotalProgressGauge: TGauge
      Left = 17
      Top = 28
      Width = 350
      Height = 26
      ForeColor = clBlue
      Progress = 0
    end
  end
  object CancelButton: TButton
    Left = 162
    Top = 259
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancelButtonClick
  end
  object AliveTimer: TTimer
    OnTimer = AliveTimerTimer
    Left = 264
    Top = 256
  end
end
