object HeadsButtonsEntryFrame: THeadsButtonsEntryFrame
  Left = 0
  Top = 0
  Width = 365
  Height = 185
  AutoSize = True
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 365
    Height = 185
    TabOrder = 0
    object EnterGroupBox: TGroupBox
      Left = 7
      Top = 1
      Width = 49
      Height = 176
      Caption = 'Enter:'
      TabOrder = 1
      object ApplyButton: TButton
        Left = 4
        Top = 140
        Width = 40
        Height = 21
        Caption = 'Apply'
        Default = True
        TabOrder = 0
      end
    end
    object TrayMapGroupBox: TGroupBox
      Left = 61
      Top = 1
      Width = 292
      Height = 176
      Caption = 'Build tray map'
      TabOrder = 0
    end
  end
end
