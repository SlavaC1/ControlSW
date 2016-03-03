object WeightsStatusFrame: TWeightsStatusFrame
  Left = 0
  Top = 0
  Width = 395
  Height = 247
  TabOrder = 0
  object SubTitle: TLabel
    Left = 3
    Top = 0
    Width = 260
    Height = 13
    Caption = 'Wait 10 full seconds for the load cell value to stabilize.'
  end
  object WeightsStatusGroupBox: TGroupBox
    Left = 24
    Top = 19
    Width = 329
    Height = 158
    TabOrder = 0
    object Label1: TLabel
      Left = 24
      Top = 72
      Width = 31
      Height = 13
      Caption = 'Waste'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 102
      Top = 19
      Width = 54
      Height = 13
      Caption = 'Cabinet A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 216
      Top = 19
      Width = 53
      Height = 13
      Caption = 'Cabinet B'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ValuesStableCheckBox: TCheckBox
      Left = 3
      Top = 138
      Width = 193
      Height = 17
      Caption = 'CheckBox Text'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
  end
end
