object HeadsFrame: THeadsFrame
  Left = 0
  Top = 0
  Width = 715
  Height = 447
  TabOrder = 0
  object ReqTempADUHeads: TLabel
    Left = 16
    Top = 210
    Width = 81
    Height = 26
    AutoSize = False
    Caption = 'Requested temp (A/D)'
    WordWrap = True
  end
  object CurTempADUHeads: TLabel
    Left = 16
    Top = 180
    Width = 65
    Height = 26
    AutoSize = False
    Caption = 'Current temp (A/D)'
    WordWrap = True
  end
  object CurTempCelHeads: TLabel
    Left = 16
    Top = 150
    Width = 68
    Height = 26
    AutoSize = False
    Caption = 'Current temp  ('#176'C)'
    WordWrap = True
  end
  object CurTempCelFront: TLabel
    Left = 16
    Top = 298
    Width = 68
    Height = 26
    AutoSize = False
    Caption = 'Current temp  ('#176'C)'
    Visible = False
    WordWrap = True
  end
  object CurTempADUFront: TLabel
    Left = 16
    Top = 330
    Width = 65
    Height = 26
    AutoSize = False
    Caption = 'Current temp (A/D)'
    Visible = False
    WordWrap = True
  end
  object ReqTempADUFront: TLabel
    Left = 16
    Top = 362
    Width = 81
    Height = 26
    AutoSize = False
    Caption = 'Requested temp (A/D)'
    Visible = False
    WordWrap = True
  end
  object ReqTempADUHeadsRear: TLabel
    Left = 16
    Top = 106
    Width = 81
    Height = 26
    AutoSize = False
    Caption = 'Requested temp (A/D)'
    Visible = False
    WordWrap = True
  end
  object CurTempADURear: TLabel
    Left = 16
    Top = 74
    Width = 65
    Height = 26
    AutoSize = False
    Caption = 'Current temp (A/D)'
    Visible = False
    WordWrap = True
  end
  object CurTempCelRear: TLabel
    Left = 16
    Top = 42
    Width = 68
    Height = 26
    AutoSize = False
    Caption = 'Current temp  ('#176'C)'
    Visible = False
    WordWrap = True
  end
  object HeadsFillingGroupBox: TGroupBox
    Left = 497
    Top = 42
    Width = 209
    Height = 335
    Caption = 'Heads Filling'
    TabOrder = 1
    object UpperThermistorsBevel: TBevel
      Left = 3
      Top = 41
      Width = 203
      Height = 8
      Shape = bsTopLine
    end
    object LowerThermistorsLabel: TLabel
      Left = 13
      Top = 109
      Width = 86
      Height = 13
      Caption = 'Lower thermistors'
    end
    object UpperThermistorsLabel: TLabel
      Left = 13
      Top = 25
      Width = 86
      Height = 13
      Caption = 'Upper thermistors'
    end
    object LowerThermistorsBevel: TBevel
      Left = 3
      Top = 125
      Width = 203
      Height = 9
      Shape = bsTopLine
    end
    object TurnHeadFillingButton: TBitBtn
      Left = 47
      Top = 289
      Width = 114
      Height = 30
      Action = HeadFillingOnAction
      Caption = 'Monitor'
      TabOrder = 0
      Layout = blGlyphRight
    end
  end
  object SaveParameterButton: TButton
    Left = 182
    Top = 407
    Width = 97
    Height = 30
    Caption = 'Save Parameters'
    TabOrder = 0
    OnClick = SaveParameterButtonClick
  end
  object TurnHeadHeatersButton: TBitBtn
    Left = 294
    Top = 407
    Width = 114
    Height = 30
    Action = HeadHeatersOnAction
    Caption = 'Turn On'
    TabOrder = 2
    Layout = blGlyphRight
  end
  object ActionList1: TActionList
    Left = 528
    object HeadHeatersOnAction: TAction
      Category = 'HeadHeaters'
      Caption = 'Turn On'
      ImageIndex = 0
      OnExecute = HeadHeatersOnActionExecute
    end
    object HeadHeatersOffAction: TAction
      Category = 'HeadHeaters'
      Caption = 'Turn Off'
      ImageIndex = 1
      OnExecute = HeadHeatersOffActionExecute
    end
    object HeadFillingOnAction: TAction
      Category = 'HeadFilling'
      Caption = 'Monitor'
      ImageIndex = 0
      OnExecute = HeadFillingOnActionExecute
    end
    object HeadFillingOffAction: TAction
      Category = 'HeadFilling'
      Caption = 'Monitor'
      ImageIndex = 1
      OnExecute = HeadFillingOffActionExecute
    end
  end
end
