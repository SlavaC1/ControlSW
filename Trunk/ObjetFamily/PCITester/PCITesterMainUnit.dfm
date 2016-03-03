object MainForm: TMainForm
  Left = 175
  Top = 182
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Objet PCI card tester'
  ClientHeight = 484
  ClientWidth = 1023
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
  object GroupBox2: TGroupBox
    Left = 8
    Top = 210
    Width = 265
    Height = 247
    Caption = 'FIFO Manual Access'
    TabOrder = 0
    object Label5: TLabel
      Left = 170
      Top = 18
      Width = 86
      Height = 13
      Caption = 'Number of dwords'
    end
    object Label13: TLabel
      Left = 103
      Top = 19
      Width = 23
      Height = 13
      Caption = 'Data'
    end
    object Label14: TLabel
      Left = 170
      Top = 70
      Width = 86
      Height = 13
      Caption = 'Number of dwords'
    end
    object Label15: TLabel
      Left = 103
      Top = 71
      Width = 23
      Height = 13
      Caption = 'Data'
    end
    object CapacityErrorLabel: TLabel
      Left = 158
      Top = 181
      Width = 66
      Height = 13
      Caption = 'Capacity Error'
      Visible = False
    end
    object FIFOManualWriteButton: TButton
      Left = 15
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Write'
      TabOrder = 0
      OnClick = FIFOManualWriteButtonClick
    end
    object FIFOWriteCountEdit: TEdit
      Left = 170
      Top = 34
      Width = 49
      Height = 21
      TabOrder = 1
      Text = '1'
    end
    object FIFOWriteUpDown: TUpDown
      Left = 219
      Top = 34
      Width = 15
      Height = 21
      Associate = FIFOWriteCountEdit
      Min = 1
      Max = 30000
      Position = 1
      TabOrder = 2
    end
    object FIFOManualReadButton: TButton
      Left = 15
      Top = 84
      Width = 75
      Height = 25
      Caption = 'Read'
      TabOrder = 3
      OnClick = FIFOManualReadButtonClick
    end
    object Panel2: TPanel
      Left = 15
      Top = 122
      Width = 209
      Height = 50
      BevelOuter = bvLowered
      TabOrder = 4
      object Label7: TLabel
        Left = 10
        Top = 10
        Width = 19
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'F'
      end
      object FLed: TShape
        Left = 12
        Top = 26
        Width = 15
        Height = 15
        Shape = stCircle
      end
      object AFFLed: TShape
        Left = 46
        Top = 26
        Width = 15
        Height = 15
        Shape = stCircle
      end
      object Label8: TLabel
        Left = 39
        Top = 10
        Width = 28
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'AF-F'
      end
      object HFAFLed: TShape
        Left = 81
        Top = 26
        Width = 15
        Height = 15
        Shape = stCircle
      end
      object Label9: TLabel
        Left = 74
        Top = 10
        Width = 29
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'HF-AF'
      end
      object AEHFLed: TShape
        Left = 115
        Top = 26
        Width = 15
        Height = 15
        Shape = stCircle
      end
      object Label10: TLabel
        Left = 107
        Top = 10
        Width = 31
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'AE-HF'
      end
      object EAELed: TShape
        Left = 150
        Top = 26
        Width = 15
        Height = 15
        Shape = stCircle
      end
      object Label11: TLabel
        Left = 143
        Top = 10
        Width = 29
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'E-AE'
      end
      object ELed: TShape
        Left = 185
        Top = 26
        Width = 15
        Height = 15
        Shape = stCircle
      end
      object Label6: TLabel
        Left = 183
        Top = 10
        Width = 19
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'E'
      end
    end
    object RefreshStatusButton: TButton
      Left = 15
      Top = 178
      Width = 81
      Height = 19
      Caption = 'Refresh Status'
      TabOrder = 5
      OnClick = RefreshStatusButtonClick
    end
    object FIFOManuaDataReadEdit: TEdit
      Left = 103
      Top = 86
      Width = 58
      Height = 21
      TabOrder = 6
      Text = '0'
    end
    object FIFOReadCountEdit: TEdit
      Left = 170
      Top = 86
      Width = 49
      Height = 21
      TabOrder = 7
      Text = '1'
    end
    object FIFOReadUpDown: TUpDown
      Left = 219
      Top = 86
      Width = 15
      Height = 21
      Associate = FIFOReadCountEdit
      Min = 1
      Max = 30000
      Position = 1
      TabOrder = 8
    end
    object FIFOManuaDataWriteEdit: TEdit
      Left = 103
      Top = 34
      Width = 58
      Height = 21
      TabOrder = 9
      Text = '0'
    end
    object ResetFIFOButton: TButton
      Left = 15
      Top = 208
      Width = 75
      Height = 25
      Caption = 'Reset'
      TabOrder = 10
      OnClick = ResetFIFOButtonClick
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 9
    Width = 761
    Height = 195
    Caption = 'Low-Level PCI access'
    TabOrder = 1
    object BarRadioGroup1: TRadioGroup
      Left = 341
      Top = 13
      Width = 99
      Height = 93
      Caption = 'Select Bar'
      ItemIndex = 2
      Items.Strings = (
        'Bar 0'
        'Bar 1'
        'Bar 2')
      TabOrder = 0
    end
    object Panel1: TPanel
      Left = 6
      Top = 19
      Width = 329
      Height = 86
      BevelOuter = bvLowered
      TabOrder = 1
      object Label1: TLabel
        Left = 15
        Top = 12
        Width = 73
        Height = 13
        Caption = 'Address (offset)'
      end
      object Label2: TLabel
        Left = 101
        Top = 12
        Width = 23
        Height = 13
        Caption = 'Data'
      end
      object AddrressEdit: TEdit
        Left = 15
        Top = 28
        Width = 65
        Height = 21
        TabOrder = 0
        Text = '0'
      end
      object DataEdit: TEdit
        Left = 101
        Top = 28
        Width = 65
        Height = 21
        TabOrder = 1
      end
      object ReadButton: TButton
        Left = 239
        Top = 12
        Width = 75
        Height = 25
        Caption = 'Read'
        TabOrder = 2
        OnClick = ReadButtonClick
      end
      object WriteButton: TButton
        Left = 239
        Top = 44
        Width = 75
        Height = 25
        Caption = 'Write'
        TabOrder = 3
        OnClick = WriteButtonClick
      end
    end
    object BinaryNumberPanel: TPanel
      Left = 6
      Top = 112
      Width = 563
      Height = 65
      BevelOuter = bvLowered
      TabOrder = 2
      object Bevel1: TBevel
        Left = 280
        Top = 23
        Width = 4
        Height = 34
        Shape = bsLeftLine
      end
      object Bevel2: TBevel
        Left = 140
        Top = 28
        Width = 3
        Height = 25
        Shape = bsLeftLine
      end
      object Bevel3: TBevel
        Left = 420
        Top = 28
        Width = 3
        Height = 25
        Shape = bsLeftLine
      end
      object Label3: TLabel
        Left = 4
        Top = 16
        Width = 12
        Height = 13
        Caption = '31'
      end
      object Label4: TLabel
        Left = 549
        Top = 16
        Width = 6
        Height = 13
        Caption = '0'
      end
      object CheckBox1: TCheckBox
        Tag = 31
        Left = 4
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 0
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox2: TCheckBox
        Tag = 30
        Left = 21
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 1
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox3: TCheckBox
        Tag = 29
        Left = 39
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 2
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox4: TCheckBox
        Tag = 28
        Left = 56
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 3
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox5: TCheckBox
        Tag = 27
        Left = 73
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 4
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox8: TCheckBox
        Tag = 24
        Left = 125
        Top = 32
        Width = 14
        Height = 17
        TabOrder = 5
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox9: TCheckBox
        Tag = 25
        Left = 108
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 6
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox10: TCheckBox
        Tag = 26
        Left = 90
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 7
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox6: TCheckBox
        Tag = 16
        Left = 265
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 8
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox7: TCheckBox
        Tag = 17
        Left = 248
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 9
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox11: TCheckBox
        Tag = 18
        Left = 231
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 10
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox12: TCheckBox
        Tag = 19
        Left = 214
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 11
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox13: TCheckBox
        Tag = 20
        Left = 196
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 12
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox14: TCheckBox
        Tag = 21
        Left = 179
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 13
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox15: TCheckBox
        Tag = 22
        Left = 162
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 14
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox16: TCheckBox
        Tag = 23
        Left = 145
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 15
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox17: TCheckBox
        Left = 544
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 16
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox18: TCheckBox
        Tag = 1
        Left = 527
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 17
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox19: TCheckBox
        Tag = 2
        Left = 510
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 18
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox20: TCheckBox
        Tag = 3
        Left = 493
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 19
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox21: TCheckBox
        Tag = 4
        Left = 476
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 20
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox22: TCheckBox
        Tag = 5
        Left = 459
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 21
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox23: TCheckBox
        Tag = 6
        Left = 442
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 22
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox24: TCheckBox
        Tag = 7
        Left = 425
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 23
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox25: TCheckBox
        Tag = 8
        Left = 404
        Top = 32
        Width = 14
        Height = 17
        TabOrder = 24
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox26: TCheckBox
        Tag = 9
        Left = 387
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 25
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox27: TCheckBox
        Tag = 10
        Left = 369
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 26
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox28: TCheckBox
        Tag = 11
        Left = 352
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 27
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox29: TCheckBox
        Tag = 12
        Left = 335
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 28
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox30: TCheckBox
        Tag = 13
        Left = 318
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 29
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox31: TCheckBox
        Tag = 14
        Left = 300
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 30
        OnClick = BinaryNumCheckboxClick
      end
      object CheckBox32: TCheckBox
        Tag = 15
        Left = 283
        Top = 32
        Width = 17
        Height = 17
        TabOrder = 31
        OnClick = BinaryNumCheckboxClick
      end
    end
    object GroupBox1: TGroupBox
      Left = 575
      Top = 13
      Width = 174
      Height = 93
      Caption = 'EEPROM'
      TabOrder = 3
      object Label12: TLabel
        Left = 99
        Top = 13
        Width = 23
        Height = 13
        Caption = 'Data'
      end
      object Label21: TLabel
        Left = 11
        Top = 13
        Width = 38
        Height = 13
        Caption = 'Address'
      end
      object EEPROMDataEdit: TEdit
        Left = 99
        Top = 29
        Width = 65
        Height = 21
        TabOrder = 0
      end
      object EEPROMAddrEdit: TEdit
        Left = 11
        Top = 29
        Width = 65
        Height = 21
        TabOrder = 1
        Text = '0'
      end
      object EEPROMReadButton: TButton
        Left = 11
        Top = 61
        Width = 64
        Height = 25
        Caption = 'Read'
        TabOrder = 2
        OnClick = EEPROMReadButtonClick
      end
      object EEPROMWriteButton: TButton
        Left = 99
        Top = 61
        Width = 65
        Height = 25
        Caption = 'Write'
        TabOrder = 3
        OnClick = EEPROMWriteButtonClick
      end
    end
  end
  object GroupBox4: TGroupBox
    Left = 279
    Top = 210
    Width = 235
    Height = 119
    Caption = 'FIFO Test'
    TabOrder = 2
    object HelpButton: TSpeedButton
      Left = 188
      Top = 32
      Width = 33
      Height = 33
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000D40E0000D40E00000000000000000000C6C3C6C6C3C6
        C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6000000C6C3
        C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6
        C3C6C6C3C6C6C3C6000000000000C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6C6C3C6
        C6C3C684828484828484828484828484828484828400000000FFFF0000008482
        84C6C3C6C6C3C6C6C3C6C6C3C6C6C3C600000000000000000000000000000000
        0000000000FFFFFFFFFFFF000000000000848284C6C3C6C6C3C6C6C3C6000000
        FFFFFFFFFFFFFFFFFF00FFFFFFFFFF00FFFFFFFFFF00FFFFFFFFFFFFFFFFFFFF
        FF000000848284C6C3C6C6C3C6000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFF84
        0000840000FFFFFFFFFFFF00FFFFFFFFFF000000848284C6C3C6C6C3C6000000
        FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFF
        FF000000848284C6C3C6C6C3C6000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFF84
        0000848284FFFFFFFFFFFF00FFFFFFFFFF000000848284C6C3C6C6C3C6000000
        FFFFFFFFFFFFFFFFFF00FFFFFFFFFF848284840000C6C3C6FFFFFFFFFFFFFFFF
        FF000000848284C6C3C6C6C3C6000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFF00
        FFFF84828484000084828400FFFFFFFFFF000000848284C6C3C6C6C3C6000000
        FFFFFFFFFFFFFFFFFF840000848284FFFFFFFFFFFF840000840000FFFFFFFFFF
        FF000000848284C6C3C6C6C3C6000000FFFFFF00FFFFFFFFFF84000084000000
        FFFFC6C3C684000084000000FFFFFFFFFF000000848284C6C3C6C6C3C6000000
        FFFFFFFFFFFFFFFFFFC6C3C6840000840000840000840000C6C3C6FFFFFFFFFF
        FF000000848284C6C3C6C6C3C6000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFF00
        FFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFF000000848284C6C3C6C6C3C6000000
        FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFF
        FF000000C6C3C6C6C3C6C6C3C6C6C3C600000000000000000000000000000000
        0000000000000000000000000000000000C6C3C6C6C3C6C6C3C6}
      OnClick = HelpButtonClick
    end
    object NonInterruptTestButton: TButton
      Left = 72
      Top = 32
      Width = 104
      Height = 25
      Caption = 'Non Interrupt Test'
      TabOrder = 0
      OnClick = NonInterruptTestButtonClick
    end
    object InterruptTestButton: TButton
      Left = 72
      Top = 64
      Width = 104
      Height = 25
      Caption = 'Interrupt Test'
      TabOrder = 1
      OnClick = InterruptTestButtonClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 279
    Top = 335
    Width = 235
    Height = 122
    Caption = 'Continous Write'
    TabOrder = 3
    object ContinousWriteGoButton: TButton
      Left = 72
      Top = 44
      Width = 104
      Height = 25
      Caption = 'Start...'
      TabOrder = 0
      OnClick = ContinousWriteGoButtonClick
    end
  end
  object GroupBox6: TGroupBox
    Left = 520
    Top = 210
    Width = 249
    Height = 247
    Caption = 'Macro Recorder'
    TabOrder = 4
    object IOWriteMemo: TMemo
      Left = 16
      Top = 24
      Width = 137
      Height = 209
      TabOrder = 0
    end
    object Button4: TButton
      Left = 162
      Top = 24
      Width = 75
      Height = 25
      Caption = 'Load'
      TabOrder = 1
      OnClick = Button4Click
    end
    object Button5: TButton
      Left = 162
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Save'
      TabOrder = 2
      OnClick = Button5Click
    end
    object RecordCheckBox: TCheckBox
      Left = 164
      Top = 135
      Width = 73
      Height = 17
      Caption = 'Record'
      TabOrder = 3
    end
    object ExecuteButton: TButton
      Left = 162
      Top = 208
      Width = 75
      Height = 25
      Caption = 'Execute'
      TabOrder = 4
      OnClick = ExecuteButtonClick
    end
  end
  object DataPathTestGroupbox: TGroupBox
    Left = 775
    Top = 8
    Width = 240
    Height = 449
    Caption = 'Data path test'
    Padding.Left = 5
    Padding.Right = 5
    TabOrder = 5
    object GoButton: TButton
      Left = 157
      Top = 410
      Width = 75
      Height = 25
      Caption = 'Go'
      TabOrder = 0
      OnClick = GoButtonClick
    end
    object DataPathDataInput: TRichEdit
      Left = 7
      Top = 15
      Width = 226
      Height = 306
      Align = alTop
      ScrollBars = ssVertical
      TabOrder = 1
      OnKeyPress = MemoKeyPress
    end
    object SaveDataButton: TBitBtn
      Left = 76
      Top = 337
      Width = 75
      Height = 25
      Caption = 'Save'
      TabOrder = 2
      OnClick = SaveDataButtonClick
    end
    object LoadDataButton: TBitBtn
      Left = 157
      Top = 337
      Width = 75
      Height = 25
      Caption = 'Load'
      TabOrder = 3
      OnClick = LoadDataButtonClick
    end
    object ClearMemoButton: TBitBtn
      Left = 8
      Top = 337
      Width = 33
      Height = 25
      Hint = 'Clear input data'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      OnClick = ClearMemoButtonClick
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 465
    Width = 1023
    Height = 19
    Panels = <
      item
        Alignment = taRightJustify
        Width = 50
      end>
    ExplicitTop = 472
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'io'
    Filter = 'I/O files|*.io|All files|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 609
    Top = 137
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'io'
    Filter = 'I/O files|*.io|All files|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 640
    Top = 137
  end
  object IconImageList: TImageList
    Left = 976
    Top = 376
    Bitmap = {
      494C010103000500040010001000FFFFFFFFFF00FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      00000000000000000000000000000000000000000000C0A18D00874A20005357
      5500535755005357550053575500535755005357550053575500535755005357
      5500874A2000874A2000874A2000C0A18D000000000000000000CAE2EC0064AD
      CA005CAAC900DCE6E90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000001192A2001293A30060B6C100DDEEF0000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0A18D00874A2000CCA98E00927B
      6300E3E3E300D6D6D600BABABA00BABABA00BABABA00B9B9B900B9B9B900947C
      6300CE9F7200DCB89800EAD2BE00874A2000000000008DC3D80061B0CE0071C1
      DB0061AFCD00A5B9C000DCDCDC0000000000000000000000000051975500609C
      6200000000000000000000000000000000000000000000000000000000000000
      000000000000000000001C98A7004FC1D0005BD3E10032B4C5000F92A20055B1
      BD00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000CCA98E00DCB89800927B
      6200E1E1E100E6E6E6005357550053575500C5C5C500C2C2C200B2B2B200937A
      6100CE9E7100DBB69400EAD2BE00874A200064ADCA006BBDD70078CCE30077CA
      E2005CAAC9004987A000437C93004E8FAA00519CB1001882330018A73000097A
      1200DDEBF0000000000000000000000000000000000000000000000000000000
      00000000000039A6B30038B4C4005FD8E70026CADF004ED4E5006DD9E70034B3
      C4001896A600D8ECEE0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200927B
      6200C5C5C500E1E1E1005357550053575500C5C5C500C5C5C500BFBFBF00937A
      6100CE9E7100DBB69400EAD2BE00874A20005CAAC9007AD2E70079D0E60078CE
      E4005CAAC9005796B0005690A8005BA0B2001F903D001EAF3B001CB539000A7E
      14003D8F85000000000000000000000000000000000000000000000000000000
      0000BBDFE300209CAC006CDAE80010C5DC0003C2DA0003C2DA0015C6DC005BD7
      E70055C6D4001193A300BFE0E500000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200927A
      6100C5C5C500C5C5C5005357550053575500E2E2E200C5C5C500B9B9B900937A
      6100CE9E7100DBB69400EAD2BE00874A20005CAAC9007CD6EA007BD4E8007AD2
      E7005CAAC9005A9BB6004C9897001F963D0023B6470023BB46001CB5390014A7
      29000B8A1600217C2500CEE0CE00000000000000000000000000000000000000
      00002A9FAE005ECFDD0046DAED0018D0E70011CBE30007C4DC0003C2DA0003C2
      DA004CD3E40058CDDC000D91A100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F72009C7E
      6000BABABA00C5C5C500C5C5C500E1E1E100E7E7E700D7D7D700BABAB900997B
      5D00CE9E7000DBB69400EAD2BE00874A20005CAAC9007DD9EC007DD7EB007CD6
      EA005CAAC9005CA9C8002FAB440047C8680029C1530023BB46001CB5390016AF
      2D0010A921000A9F140012771500CEE0CE000000000000000000000000000000
      00001795A6008BEDFB003CE5FC0037E4FB002FDEF60023D7EE0014CDE50004C3
      DB0003C2DA0056D6E60035B5C50056B3BE000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200B08C
      6900947C6200947C6200947C6200947C6200947C6200947C6200947C6200B08C
      6900DBB69400CF9F7200EAD2BE00874A20005CAAC9007FDDEE007EDBED007EDA
      ED007ED9EC0077D0E50030B14800BBF6C00048CC6A0023BB46001CB5390016AF
      2D0010A921000AA3150005970A00418A44000000000000000000000000009FCD
      D80035B1C0008AEFFD005FEAFD0061EBFD0052E9FD003CE6FD002ADBF30018D0
      E70010C6DC0049D2E40067D4E2001595A5000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200CF9F
      7200CF9F7200CF9F7200CF9F7200CF9F7200CF9F7200CF9F7200CF9F7200CF9F
      7200CF9F7200CF9F7200EAD2BE00874A20005CAAC90081E1F10080E0F00080DE
      EF007FDEEF007FDDEE004FAAAB003DB75300BAF5BF004DCC68001CB539001093
      20000D951B000AA31500059E0900047507000000000000000000000000002745
      AE001442AC0084EAFB006BECFD0084EFFD006DECFD0052E9FD0044E5FB0065E3
      F30077DDEB0048BFCD0024A0B0001493A3000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200FEFE
      FE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFE
      FE00FEFEFE00CF9F7200EAD2BE00874A20005CAAC90082E4F30082E3F30082E3
      F20081E2F20081E2F1005CAAC90059B9BF003DB65400AAEDAF005ED06F001095
      210065B06C000A8F1400059E0900057A0A00000000009FD2D8001394A4002FA6
      BB00284ABB001D38B80061D8F60063EAFD006CEBFD007DEEFD0088EAF80047BD
      CC000B91A0005BB5C000BEE0E400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200FEFE
      FE00F4F4F400F4F4F400F4F4F400F4F4F400F4F4F400F4F4F400F4F4F400F4F4
      F400F4F4F400CF9F7200EAD2BE00874A20005CAAC90084E8F60083E7F50083E6
      F50083E6F40083E6F4005CAAC9006CC4EA005EC2CB0025AB3F00A9EDAE003FAE
      4A000000000047A25000059E0900067F0C0000000000A4D4DA00088F9F004FC8
      D80068DCEC00336DC8000D1BAB0062B1D70072DCEA0047BDCB001092A30060B7
      C100000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200FEFE
      FE00D7D7D700D7D7D700D7D7D700D7D7D700D7D7D700D7D7D700D7D7D700D7D7
      D700F4F4F400CF9F7200EAD2BE00874A20005CAAC90085EBF80085EAF70085EA
      F70085E9F70085E9F6005CAAC9006FC9F00070CDF50063CCD7002BAD4C002BA4
      4D000000000057B25E00059E090015861E0000000000000000007DAFC0000086
      9A0041BDCD0077DEEB001D6CB0001A4AA8002CA1AF0087C7CF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200FEFE
      FE00F4F4F400F4F4F400F4F4F400F4F4F400F4F4F400F4F4F400F4F4F400F4F4
      F400F4F4F400CF9F7200EAD2BE00874A20005CAAC90087EEFA0086EEF90086ED
      F90086EDF90086EDF9005CAAC90070CCF50072CFF80075D5FF0075D5FF005EAE
      CE00C2E3C7000C9A18000995120084BF8A00000000006994B0000A5484000B76
      B2000188A10031ACBC0028A3B300C1E1E5000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200F9F9
      F900E2E2E200E2E2E200E2E2E200E2E2E200E2E2E200E2E2E200E2E2E200E2E2
      E200F9F9F900CF9F7200EAD2BE00874A20005CAAC90088F1FC0088F1FC0088F0
      FB0087F0FB0087F0FB005CAAC90072D0F90074D3FD0075D5FF0075D5FF0051AD
      B00012A024000E9E1B004AAA5400000000008FAFC3000C5889000F7ABC00107D
      C1000C598800309AAB00008B9C00D5EAED000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00CF9F7200F9F9
      F900F9F9F900F9F9F900F9F9F900F9F9F900F9F9F900F9F9F900F9F9F900F9F9
      F900F9F9F900CF9F7200EAD2BE00874A20005CAAC90089F4FD0089F3FD0089F3
      FD0086EEF90078D7E9005CAAC9008BDFFF008BE0FF008BE0FF00ABEFFF0052B0
      B20014A5290079C683000000000000000000115582000C72B200107DC1000A6B
      A90024638C00000000007CC2CB00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000874A2000EAD2BE00EED9C8003EAF
      FC003EAFFC003EAFFC003EAFFC003EAFFC003EAFFC003EAFFC003EAFFC003EAF
      FC003EAFFC00EAD2BE00EAD2BE00874A20005CAAC9008AF5FF0078D9EA006AC1
      DA005CAAC9007FC5DA00A9E4EE00CCFFFF00CCFFFF00CCFFFF00B8F1F70069B6
      D30000000000000000000000000000000000195A86000A69A6000B6DAB00094F
      7E00BACCD9000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000AE866B00874A2000874A2000005C
      CE00005CCE00005CCE00005CCE00005CCE00005CCE00005CCE00005CCE00005C
      CE00005CCE00874A2000874A2000AE866B0079B8D1005CAAC9005EADCD0060B1
      D10061B4D50062B5D60062B5D60062B5D60062B5D60063B7D8006BBAD800C2E1
      ED0000000000000000000000000000000000C3D3DE0024638D001B5C8700B7CA
      D800000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF008000C3FFFE1F0000000081CFFC0F0000
      00000007F803000000000007F001000000000001F001000000000000F0000000
      00000000E000000000000000E0000000000000008001000000000008800F0000
      00000008C03F00000000000080FF00000000000100FF00000000000305FF0000
      0000000F07FF00000000000F0FFF0000}
  end
  object DataFileOpenDialog: TOpenDialog
    DefaultExt = 'txt'
    Filter = 'TXT files|*.txt|All files|*.*'
    Left = 944
    Top = 376
  end
  object DataFileSaveDialog: TSaveDialog
    DefaultExt = 'txt'
    Filter = 'TXT files|*.txt|All files|*.*'
    Left = 912
    Top = 376
  end
end
