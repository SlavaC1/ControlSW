object OHDBOptionsForm: TOHDBOptionsForm
  Left = 433
  Top = 308
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'OHDB Options'
  ClientHeight = 260
  ClientWidth = 342
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 6
    Top = 9
    Width = 329
    Height = 83
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 8
      Width = 38
      Height = 13
      Caption = 'Address'
    end
    object Label2: TLabel
      Left = 96
      Top = 8
      Width = 23
      Height = 13
      Caption = 'Data'
    end
    object XilinxAddressEdit: TEdit
      Left = 16
      Top = 24
      Width = 65
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object XilinxDataEdit: TEdit
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
  object Button3: TButton
    Left = 260
    Top = 225
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = Button3Click
  end
  object MacroMemo: TMemo
    Left = 8
    Top = 97
    Width = 137
    Height = 153
    TabOrder = 2
  end
  object Button4: TButton
    Left = 160
    Top = 97
    Width = 75
    Height = 25
    Caption = 'Load'
    TabOrder = 3
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 160
    Top = 129
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 4
    OnClick = Button5Click
  end
  object MacroRecordCheckBox: TCheckBox
    Left = 160
    Top = 169
    Width = 73
    Height = 17
    Caption = 'Record'
    TabOrder = 5
  end
  object ExecuteButton: TButton
    Left = 160
    Top = 225
    Width = 75
    Height = 25
    Caption = 'Execute'
    TabOrder = 6
    OnClick = ExecuteButtonClick
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 168
    Top = 201
  end
  object SaveDialog1: TSaveDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 200
    Top = 201
  end
end
