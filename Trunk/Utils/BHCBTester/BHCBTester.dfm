object BHCBTesterForm: TBHCBTesterForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'BHCB Tester'
  ClientHeight = 261
  ClientWidth = 516
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object FPGARegistersGroupbox: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 63
    Width = 510
    Height = 195
    Margins.Top = 0
    Align = alClient
    Caption = 'FPGA registers'
    TabOrder = 0
    object Label6: TLabel
      Left = 20
      Top = 28
      Width = 39
      Height = 13
      Caption = 'Address'
    end
    object Label7: TLabel
      Left = 20
      Top = 55
      Width = 23
      Height = 13
      Caption = 'Data'
    end
    object FPGARegAddressEdit: TEdit
      Left = 80
      Top = 24
      Width = 89
      Height = 21
      TabOrder = 0
      OnKeyPress = FilterWordKeyPress
    end
    object ReadFPGARegButton: TButton
      Left = 49
      Top = 152
      Width = 57
      Height = 25
      Caption = 'Read'
      TabOrder = 2
      OnClick = ReadFPGARegButtonClick
    end
    object FPGARegDataEdit: TEdit
      Left = 80
      Top = 51
      Width = 89
      Height = 21
      TabOrder = 1
      OnKeyPress = FilterWordKeyPress
    end
    object WriteFPGARegButton: TButton
      Left = 112
      Top = 152
      Width = 57
      Height = 25
      Caption = 'Write'
      TabOrder = 3
      OnClick = WriteFPGARegButtonClick
    end
    object GroupBox2: TGroupBox
      AlignWithMargins = True
      Left = 186
      Top = 15
      Width = 319
      Height = 175
      Margins.Top = 0
      Align = alRight
      Caption = 'Script'
      TabOrder = 4
      object Label1: TLabel
        Left = 79
        Top = 140
        Width = 25
        Height = 13
        Caption = 'mSec'
      end
      object FPGARegRecordMacroCheckbox: TCheckBox
        Left = 15
        Top = 16
        Width = 58
        Height = 17
        Caption = 'Record'
        TabOrder = 0
      end
      object WriteDelayCheckBox: TCheckBox
        Left = 15
        Top = 111
        Width = 82
        Height = 17
        Caption = 'Write delay'
        TabOrder = 1
      end
      object WriteDelayEdit: TEdit
        Left = 15
        Top = 134
        Width = 58
        Height = 21
        TabOrder = 2
        Text = '0'
      end
      object FPGARegMacroMemo: TMemo
        Left = 117
        Top = 16
        Width = 124
        Height = 145
        ScrollBars = ssVertical
        TabOrder = 3
      end
      object FPGARegLoadMacroButton: TButton
        Left = 252
        Top = 16
        Width = 57
        Height = 25
        Caption = 'Load'
        TabOrder = 4
        OnClick = FPGARegLoadMacroButtonClick
      end
      object FPGARegSaveMacroButton: TButton
        Left = 252
        Top = 47
        Width = 57
        Height = 25
        Caption = 'Save'
        TabOrder = 5
        OnClick = FPGARegSaveMacroButtonClick
      end
      object FPGARegMacroClearButton: TButton
        Left = 252
        Top = 78
        Width = 57
        Height = 25
        Caption = 'Clear'
        TabOrder = 6
        OnClick = FPGARegMacroClearButtonClick
      end
      object FPGARegExecuteMacroButton: TButton
        Left = 252
        Top = 136
        Width = 57
        Height = 25
        Caption = 'Execute'
        TabOrder = 7
        OnClick = FPGARegExecuteMacroButtonClick
      end
    end
  end
  object GroupBox1: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 510
    Height = 57
    Align = alTop
    Caption = 'Connection'
    TabOrder = 1
    object CommPortComboBox: TComboBox
      Left = 104
      Top = 24
      Width = 145
      Height = 21
      ItemHeight = 13
      TabOrder = 0
    end
    object ConnectButton: TButton
      Left = 288
      Top = 22
      Width = 75
      Height = 25
      Caption = 'Connect'
      TabOrder = 1
      OnClick = ConnectButtonClick
    end
  end
  object FPGAMacroOpenDialog: TOpenDialog
    Left = 120
    Top = 168
  end
  object FPGAMacroSaveDialog: TSaveDialog
    Left = 88
    Top = 168
  end
end
