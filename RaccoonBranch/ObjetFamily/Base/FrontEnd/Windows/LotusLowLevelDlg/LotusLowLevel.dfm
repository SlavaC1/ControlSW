object LotusLowLevelForm: TLotusLowLevelForm
  Left = 386
  Top = 173
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Lotus Low Level Access'
  ClientHeight = 424
  ClientWidth = 526
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 55
    Top = 372
    Width = 99
    Height = 13
    Caption = 'Execution Delay (ms)'
  end
  object Label4: TLabel
    Left = 16
    Top = 144
    Width = 77
    Height = 13
    Caption = 'Macro Recorder'
  end
  object Panel1: TPanel
    Left = 6
    Top = 9
    Width = 329
    Height = 128
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 8
      Width = 66
      Height = 13
      Caption = 'Address (Hex)'
    end
    object Label2: TLabel
      Left = 96
      Top = 8
      Width = 51
      Height = 13
      Caption = 'Data (Hex)'
    end
    object Label5: TLabel
      Left = 15
      Top = 51
      Width = 37
      Height = 13
      Caption = 'Bit view'
    end
    object LotusAddressEdit: TEdit
      Left = 16
      Top = 24
      Width = 65
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object LotusDataEdit: TEdit
      Left = 96
      Top = 24
      Width = 65
      Height = 21
      TabOrder = 1
    end
    object Button1: TButton
      Left = 240
      Top = 24
      Width = 75
      Height = 25
      Caption = 'Read'
      TabOrder = 2
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 240
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Write'
      TabOrder = 3
      OnClick = Button2Click
    end
    object BinaryNumberPanel: TPanel
      Left = 15
      Top = 66
      Width = 148
      Height = 54
      TabOrder = 4
      object Label91: TLabel
        Left = 123
        Top = 8
        Width = 6
        Height = 13
        Caption = '0'
      end
      object Label98: TLabel
        Left = 11
        Top = 8
        Width = 6
        Height = 13
        Caption = '7'
      end
      object BinaryNumCheckbox: TCheckBox
        Left = 120
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 0
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox18: TCheckBox
        Tag = 1
        Left = 104
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 1
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox19: TCheckBox
        Tag = 2
        Left = 88
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 2
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox20: TCheckBox
        Tag = 3
        Left = 72
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 3
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox21: TCheckBox
        Tag = 4
        Left = 56
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 4
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox22: TCheckBox
        Tag = 5
        Left = 40
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 5
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox23: TCheckBox
        Tag = 6
        Left = 24
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 6
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox24: TCheckBox
        Tag = 7
        Left = 8
        Top = 24
        Width = 17
        Height = 17
        TabOrder = 7
        OnClick = BinaryNumCheckboxClick
      end
    end
  end
  object Button3: TButton
    Left = 436
    Top = 393
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = Button3Click
  end
  object MacroMemo: TMemo
    Left = 8
    Top = 161
    Width = 505
    Height = 152
    TabOrder = 2
    WordWrap = False
  end
  object Button4: TButton
    Left = 352
    Top = 393
    Width = 75
    Height = 25
    Caption = 'Load'
    TabOrder = 3
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 264
    Top = 393
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 4
    OnClick = Button5Click
  end
  object MacroRecordCheckBox: TCheckBox
    Left = 8
    Top = 313
    Width = 73
    Height = 17
    Caption = 'Record'
    TabOrder = 5
  end
  object ExecuteButton: TButton
    Left = 8
    Top = 393
    Width = 75
    Height = 25
    Action = MacroExecute
    TabOrder = 6
  end
  object ExecutionDelayEdit: TEdit
    Left = 8
    Top = 368
    Width = 41
    Height = 21
    TabOrder = 7
    Text = '100'
  end
  object LoopExecutionCheckbox: TCheckBox
    Left = 8
    Top = 344
    Width = 97
    Height = 17
    Caption = 'Loop...'
    TabOrder = 8
  end
  object KeepReadingsCheckbox: TCheckBox
    Left = 72
    Top = 344
    Width = 97
    Height = 17
    Caption = 'Keep readings'
    TabOrder = 9
  end
  object MacroRestoreButton: TButton
    Left = 96
    Top = 393
    Width = 75
    Height = 25
    Caption = 'Restore'
    TabOrder = 10
    OnClick = MacroRestoreButtonClick
  end
  object WriteToLogCheckBox: TCheckBox
    Left = 168
    Top = 344
    Width = 81
    Height = 17
    Caption = 'Write to Log'
    TabOrder = 11
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 432
    Top = 345
  end
  object SaveDialog1: TSaveDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 472
    Top = 345
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 392
    Top = 344
  end
  object ActionList1: TActionList
    Left = 352
    Top = 344
    object MacroExecute: TAction
      Caption = 'Execute'
      OnExecute = MacroExecuteExecute
    end
    object MacroStop: TAction
      Caption = 'Stop'
      OnExecute = MacroStopExecute
    end
  end
end
