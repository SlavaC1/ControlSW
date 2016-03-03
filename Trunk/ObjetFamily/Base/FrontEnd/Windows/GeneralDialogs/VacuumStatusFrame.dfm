object VacuumStatusFrame: TVacuumStatusFrame
  Left = 0
  Top = 0
  Width = 369
  Height = 167
  TabOrder = 0
  object lbl1: TLabel
    Left = 24
    Top = 0
    Width = 296
    Height = 13
    Caption = 'Select the checkbox below when the active values are stable.'
  end
  object VacuumGroupBox: TGroupBox
    Left = 43
    Top = 19
    Width = 270
    Height = 113
    Caption = 'Vacuum'
    TabOrder = 0
    object Label1: TLabel
      Left = 32
      Top = 74
      Width = 65
      Height = 13
      Caption = 'Current Level'
    end
    object Label2: TLabel
      Left = 32
      Top = 26
      Width = 70
      Height = 13
      Caption = 'Average Value'
    end
    object CurrentValuePanel: TPanel
      Left = 161
      Top = 68
      Width = 77
      Height = 25
      BevelInner = bvLowered
      BevelOuter = bvNone
      Caption = '1820'
      TabOrder = 0
    end
    object AverageValuePanel: TPanel
      Left = 161
      Top = 20
      Width = 77
      Height = 25
      BevelInner = bvLowered
      BevelOuter = bvNone
      Caption = '1780'
      TabOrder = 1
    end
  end
  object ValuesStableCheckBox: TCheckBox
    Left = 43
    Top = 137
    Width = 230
    Height = 17
    Caption = 'The vacuum values have stabilized.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
  end
end
