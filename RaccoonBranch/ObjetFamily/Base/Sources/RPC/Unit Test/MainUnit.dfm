object MainForm: TMainForm
  Left = 246
  Top = 115
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'RPC Unit Test'
  ClientHeight = 334
  ClientWidth = 288
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
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 6
    Top = 129
    Width = 60
    Height = 13
    Caption = 'Object name'
  end
  object Label2: TLabel
    Left = 6
    Top = 177
    Width = 109
    Height = 13
    Caption = 'Property/Method name'
  end
  object Label3: TLabel
    Left = 6
    Top = 225
    Width = 27
    Height = 13
    Caption = 'Value'
  end
  object Label4: TLabel
    Left = 6
    Top = 289
    Width = 30
    Height = 13
    Caption = 'Result'
  end
  object Label5: TLabel
    Left = 208
    Top = 128
    Width = 38
    Height = 13
    Caption = 'Timeout'
  end
  object OpKindRadioGroup: TRadioGroup
    Left = 6
    Top = 9
    Width = 185
    Height = 105
    Caption = 'Operation'
    ItemIndex = 0
    Items.Strings = (
      'Property Read'
      'Property Write'
      'Method Invocation')
    TabOrder = 6
  end
  object ObjectNameEdit: TEdit
    Left = 6
    Top = 145
    Width = 185
    Height = 21
    TabOrder = 0
  end
  object RPCItemEdit: TEdit
    Left = 6
    Top = 193
    Width = 185
    Height = 21
    TabOrder = 1
  end
  object ExecButton: TButton
    Left = 206
    Top = 14
    Width = 75
    Height = 25
    Caption = 'Execute'
    Default = True
    TabOrder = 4
    OnClick = ExecButtonClick
  end
  object ValueEdit: TEdit
    Left = 6
    Top = 241
    Width = 185
    Height = 21
    TabOrder = 2
  end
  object Button2: TButton
    Left = 207
    Top = 48
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 5
    OnClick = Button2Click
  end
  object ResultEdit: TEdit
    Left = 6
    Top = 305
    Width = 185
    Height = 21
    TabOrder = 3
  end
  object Edit1: TEdit
    Left = 208
    Top = 144
    Width = 41
    Height = 21
    TabOrder = 7
    Text = '1'
  end
  object TimeoutUpDown: TUpDown
    Left = 249
    Top = 144
    Width = 15
    Height = 21
    Associate = Edit1
    Min = 0
    Position = 1
    TabOrder = 8
    Wrap = False
  end
end
