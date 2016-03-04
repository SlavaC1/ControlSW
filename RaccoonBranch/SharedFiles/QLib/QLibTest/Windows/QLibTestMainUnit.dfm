object MainForm: TMainForm
  Left = 276
  Top = 123
  Width = 498
  Height = 469
  Caption = 'Test Application'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = s
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 208
    Top = 32
    Width = 30
    Height = 24
    Caption = '???'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 24
    Top = 256
    Width = 18
    Height = 13
    Caption = '???'
  end
  object Label3: TLabel
    Left = 24
    Top = 272
    Width = 18
    Height = 13
    Caption = '???'
  end
  object Button3: TButton
    Left = 16
    Top = 32
    Width = 97
    Height = 25
    Caption = 'Mini Debugger'
    TabOrder = 0
    OnClick = Button3Click
  end
  object Button1: TButton
    Left = 168
    Top = 88
    Width = 113
    Height = 25
    Caption = 'Send Message'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 168
    Top = 136
    Width = 113
    Height = 25
    Caption = 'Monitor Test'
    TabOrder = 2
    OnClick = Button2Click
  end
  object SListWrite: TButton
    Left = 24
    Top = 120
    Width = 97
    Height = 25
    Caption = 'String list write'
    TabOrder = 3
    OnClick = SListWriteClick
  end
  object SListRead: TButton
    Left = 24
    Top = 152
    Width = 97
    Height = 25
    Caption = 'String list read'
    TabOrder = 4
    OnClick = SListReadClick
  end
  object Button4: TButton
    Left = 24
    Top = 224
    Width = 105
    Height = 25
    Caption = 'Get current time'
    TabOrder = 5
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 184
    Top = 184
    Width = 75
    Height = 25
    Caption = 'Safe list write'
    TabOrder = 6
    OnClick = Button5Click
  end
  object Button6: TButton
    Left = 184
    Top = 224
    Width = 75
    Height = 25
    Caption = 'Safe list read'
    TabOrder = 7
    OnClick = Button6Click
  end
  object GroupBox1: TGroupBox
    Left = 24
    Top = 312
    Width = 185
    Height = 97
    Caption = 'TCP/IP client test'
    TabOrder = 8
    object Button7: TButton
      Left = 16
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Connect'
      TabOrder = 0
      OnClick = Button7Click
    end
    object Button8: TButton
      Left = 96
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Disconnect'
      TabOrder = 1
      OnClick = Button8Click
    end
    object Button9: TButton
      Left = 16
      Top = 64
      Width = 113
      Height = 25
      Caption = 'Send something'
      TabOrder = 2
      OnClick = Button9Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 232
    Top = 312
    Width = 249
    Height = 97
    Caption = 'TCP/IP server test'
    TabOrder = 9
    object Button11: TButton
      Left = 144
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Disconnect'
      TabOrder = 0
      OnClick = Button11Click
    end
    object Button12: TButton
      Left = 16
      Top = 64
      Width = 113
      Height = 25
      Caption = 'Send something'
      TabOrder = 1
      OnClick = Button12Click
    end
    object Button13: TButton
      Left = 152
      Top = 64
      Width = 75
      Height = 25
      Caption = 'Status'
      TabOrder = 2
      OnClick = Button13Click
    end
  end
  object Edit1: TEdit
    Left = 352
    Top = 32
    Width = 73
    Height = 21
    TabOrder = 10
  end
  object Button10: TButton
    Left = 312
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Push'
    TabOrder = 11
    OnClick = Button10Click
  end
  object Button14: TButton
    Left = 400
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Pop'
    TabOrder = 12
    OnClick = Button14Click
  end
  object Button15: TButton
    Left = 24
    Top = 88
    Width = 97
    Height = 25
    Caption = 'String list load'
    TabOrder = 13
    OnClick = Button15Click
  end
  object s: TMainMenu
    Left = 120
    Top = 32
    object MonitorBase: TMenuItem
      Caption = 'Monitor'
    end
  end
end
