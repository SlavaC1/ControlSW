object StableValueFrame: TStableValueFrame
  Left = 0
  Top = 0
  Width = 369
  Height = 167
  TabOrder = 0
  object StableValueGroupBox: TGroupBox
    Left = 43
    Top = 8
    Width = 270
    Height = 113
    Caption = 'Panel Caption'
    TabOrder = 0
    object Label1: TLabel
      Left = 24
      Top = 50
      Width = 63
      Height = 13
      Caption = 'Current Level'
    end
    object UnitsLabel: TLabel
      Left = 168
      Top = 26
      Width = 63
      Height = 13
      Caption = 'Units Caption'
    end
    object CurrLevelValuePanel: TPanel
      Left = 161
      Top = 44
      Width = 77
      Height = 25
      BevelInner = bvLowered
      BevelOuter = bvNone
      Caption = '1780'
      TabOrder = 0
    end
  end
  object ValuesStableCheckBox: TCheckBox
    Left = 43
    Top = 137
    Width = 193
    Height = 17
    Caption = 'CheckBox Text'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
  end
end
