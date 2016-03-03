object DetectorForm: TDetectorForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'RFID Crosstalk Detector'
  ClientHeight = 428
  ClientWidth = 634
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Padding.Left = 5
  Padding.Right = 5
  Padding.Bottom = 5
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ResultsMsgLabel: TLabel
    Left = 8
    Top = 395
    Width = 136
    Height = 19
    Caption = 'ResultsMsgLabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object StartButton: TButton
    Left = 551
    Top = 395
    Width = 75
    Height = 25
    Caption = 'Start'
    TabOrder = 3
    OnClick = StartButtonClick
  end
  object SettingsGroupBox: TGroupBox
    Left = 5
    Top = 0
    Width = 624
    Height = 161
    Align = alTop
    Caption = 'Settings'
    TabOrder = 0
    object ContainerSelectorLabel: TLabel
      Left = 16
      Top = 32
      Width = 77
      Height = 13
      Caption = 'Select container'
    end
    object CyclesLabel: TLabel
      Left = 16
      Top = 96
      Width = 67
      Height = 13
      Caption = 'Testing cycles'
    end
    object PassFailLabel: TLabel
      Left = 200
      Top = 96
      Width = 84
      Height = 13
      Caption = 'Pass / Fail criteria'
    end
    object Label1: TLabel
      Left = 291
      Top = 112
      Width = 16
      Height = 19
      Caption = '%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 200
      Top = 32
      Width = 78
      Height = 13
      Caption = 'Select COM port'
    end
    object ContainerSelectorCombo: TComboBox
      Left = 16
      Top = 48
      Width = 120
      Height = 21
      ItemHeight = 13
      TabOrder = 0
    end
    object CyclesEdit: TEdit
      Left = 16
      Top = 112
      Width = 85
      Height = 21
      TabOrder = 3
    end
    object PassFailEdit: TEdit
      Left = 200
      Top = 112
      Width = 85
      Height = 21
      TabOrder = 4
    end
    object COMPortCombo: TComboBox
      Left = 200
      Top = 48
      Width = 120
      Height = 21
      ItemHeight = 13
      TabOrder = 1
    end
    object GenerateReportCheckBox: TCheckBox
      Left = 384
      Top = 48
      Width = 105
      Height = 17
      Caption = 'Generate Report'
      TabOrder = 2
    end
  end
  object ProgressBar: TProgressBar
    AlignWithMargins = True
    Left = 5
    Top = 166
    Width = 624
    Height = 33
    Margins.Left = 0
    Margins.Top = 5
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alTop
    Step = 1
    TabOrder = 1
  end
  object ResultsGroupbox: TGroupBox
    Left = 5
    Top = 199
    Width = 624
    Height = 190
    Align = alTop
    Caption = 'Results'
    TabOrder = 2
    object ResultsGrid: TListView
      Left = 2
      Top = 15
      Width = 620
      Height = 173
      Align = alClient
      Columns = <
        item
          Caption = 'Channel'
          MinWidth = 200
          Width = 200
        end
        item
          Alignment = taCenter
          Caption = 'Tag ID'
          MinWidth = 180
          Width = 180
        end
        item
          Alignment = taCenter
          Caption = 'Counts'
          MinWidth = 100
          Width = 100
        end
        item
          Alignment = taCenter
          Caption = 'Reading error %'
          MinWidth = 100
          Width = 120
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      GridLines = True
      ReadOnly = True
      ParentFont = False
      ParentShowHint = False
      ShowHint = False
      TabOrder = 0
      ViewStyle = vsReport
    end
  end
  object MainMenu1: TMainMenu
    Left = 392
    Top = 104
    object File1: TMenuItem
      Caption = 'File'
      object Exit1: TMenuItem
        Caption = 'Exit'
        OnClick = Exit1Click
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      object About1: TMenuItem
        Caption = 'About'
        OnClick = About1Click
      end
    end
  end
end
