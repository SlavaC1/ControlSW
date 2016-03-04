object PCIOptionsForm: TPCIOptionsForm
  Left = 214
  Top = 131
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Objet PCI card'
  ClientHeight = 346
  ClientWidth = 547
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object BarRadioGroup1: TRadioGroup
    Left = 343
    Top = 4
    Width = 81
    Height = 89
    Caption = 'Select Bar'
    ItemIndex = 2
    Items.Strings = (
      'Bar 0'
      'Bar 1'
      'Bar 2')
    TabOrder = 0
  end
  object Panel1: TPanel
    Left = 5
    Top = 9
    Width = 329
    Height = 83
    TabOrder = 1
    object Label1: TLabel
      Left = 8
      Top = 8
      Width = 73
      Height = 13
      Caption = 'Address (offset)'
    end
    object Label2: TLabel
      Left = 96
      Top = 8
      Width = 23
      Height = 13
      Caption = 'Data'
    end
    object AddrressEdit: TEdit
      Left = 16
      Top = 24
      Width = 65
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object DataEdit: TEdit
      Left = 96
      Top = 24
      Width = 65
      Height = 21
      TabOrder = 1
    end
    object Button1: TButton
      Left = 240
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Read'
      TabOrder = 2
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 240
      Top = 40
      Width = 75
      Height = 25
      Caption = 'Write'
      TabOrder = 3
      OnClick = Button2Click
    end
  end
  object BinaryNumberPanel: TPanel
    Left = 5
    Top = 100
    Width = 537
    Height = 65
    TabOrder = 2
    object Bevel1: TBevel
      Left = 266
      Top = 23
      Width = 4
      Height = 34
      Shape = bsLeftLine
    end
    object Bevel2: TBevel
      Left = 132
      Top = 28
      Width = 3
      Height = 25
      Shape = bsLeftLine
    end
    object Bevel3: TBevel
      Left = 401
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
      Left = 520
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
      OnClick = CheckBox17Click
    end
    object CheckBox2: TCheckBox
      Tag = 30
      Left = 20
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 1
      OnClick = CheckBox17Click
    end
    object CheckBox3: TCheckBox
      Tag = 29
      Left = 36
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 2
      OnClick = CheckBox17Click
    end
    object CheckBox4: TCheckBox
      Tag = 28
      Left = 52
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 3
      OnClick = CheckBox17Click
    end
    object CheckBox5: TCheckBox
      Tag = 27
      Left = 68
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 4
      OnClick = CheckBox17Click
    end
    object CheckBox8: TCheckBox
      Tag = 24
      Left = 116
      Top = 32
      Width = 14
      Height = 17
      TabOrder = 5
      OnClick = CheckBox17Click
    end
    object CheckBox9: TCheckBox
      Tag = 25
      Left = 100
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 6
      OnClick = CheckBox17Click
    end
    object CheckBox10: TCheckBox
      Tag = 26
      Left = 84
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 7
      OnClick = CheckBox17Click
    end
    object CheckBox6: TCheckBox
      Tag = 16
      Left = 248
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 8
      OnClick = CheckBox17Click
    end
    object CheckBox7: TCheckBox
      Tag = 17
      Left = 232
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 9
      OnClick = CheckBox17Click
    end
    object CheckBox11: TCheckBox
      Tag = 18
      Left = 216
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 10
      OnClick = CheckBox17Click
    end
    object CheckBox12: TCheckBox
      Tag = 19
      Left = 200
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 11
      OnClick = CheckBox17Click
    end
    object CheckBox13: TCheckBox
      Tag = 20
      Left = 184
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 12
      OnClick = CheckBox17Click
    end
    object CheckBox14: TCheckBox
      Tag = 21
      Left = 168
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 13
      OnClick = CheckBox17Click
    end
    object CheckBox15: TCheckBox
      Tag = 22
      Left = 152
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 14
      OnClick = CheckBox17Click
    end
    object CheckBox16: TCheckBox
      Tag = 23
      Left = 136
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 15
      OnClick = CheckBox17Click
    end
    object CheckBox17: TCheckBox
      Left = 517
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 16
      OnClick = CheckBox17Click
    end
    object CheckBox18: TCheckBox
      Tag = 1
      Left = 501
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 17
      OnClick = CheckBox17Click
    end
    object CheckBox19: TCheckBox
      Tag = 2
      Left = 485
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 18
      OnClick = CheckBox17Click
    end
    object CheckBox20: TCheckBox
      Tag = 3
      Left = 469
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 19
      OnClick = CheckBox17Click
    end
    object CheckBox21: TCheckBox
      Tag = 4
      Left = 453
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 20
      OnClick = CheckBox17Click
    end
    object CheckBox22: TCheckBox
      Tag = 5
      Left = 437
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 21
      OnClick = CheckBox17Click
    end
    object CheckBox23: TCheckBox
      Tag = 6
      Left = 421
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 22
      OnClick = CheckBox17Click
    end
    object CheckBox24: TCheckBox
      Tag = 7
      Left = 405
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 23
      OnClick = CheckBox17Click
    end
    object CheckBox25: TCheckBox
      Tag = 8
      Left = 385
      Top = 32
      Width = 14
      Height = 17
      TabOrder = 24
      OnClick = CheckBox17Click
    end
    object CheckBox26: TCheckBox
      Tag = 9
      Left = 369
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 25
      OnClick = CheckBox17Click
    end
    object CheckBox27: TCheckBox
      Tag = 10
      Left = 353
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 26
      OnClick = CheckBox17Click
    end
    object CheckBox28: TCheckBox
      Tag = 11
      Left = 337
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 27
      OnClick = CheckBox17Click
    end
    object CheckBox29: TCheckBox
      Tag = 12
      Left = 321
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 28
      OnClick = CheckBox17Click
    end
    object CheckBox30: TCheckBox
      Tag = 13
      Left = 305
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 29
      OnClick = CheckBox17Click
    end
    object CheckBox31: TCheckBox
      Tag = 14
      Left = 289
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 30
      OnClick = CheckBox17Click
    end
    object CheckBox32: TCheckBox
      Tag = 15
      Left = 273
      Top = 32
      Width = 17
      Height = 17
      TabOrder = 31
      OnClick = CheckBox17Click
    end
  end
  object Button3: TButton
    Left = 467
    Top = 316
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 3
    OnClick = Button3Click
  end
  object IOWriteMemo: TMemo
    Left = 7
    Top = 185
    Width = 137
    Height = 153
    TabOrder = 4
  end
  object Button4: TButton
    Left = 159
    Top = 185
    Width = 75
    Height = 25
    Caption = 'Load'
    TabOrder = 5
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 159
    Top = 217
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 6
    OnClick = Button5Click
  end
  object RecordCheckBox: TCheckBox
    Left = 159
    Top = 257
    Width = 73
    Height = 17
    Caption = 'Record'
    TabOrder = 7
  end
  object ExecuteButton: TButton
    Left = 159
    Top = 313
    Width = 75
    Height = 25
    Caption = 'Execute'
    TabOrder = 8
    OnClick = ExecuteButtonClick
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 263
    Top = 161
  end
  object SaveDialog1: TSaveDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 295
    Top = 161
  end
end
