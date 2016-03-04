object CustomResinAdvancedSettingsFrame: TCustomResinAdvancedSettingsFrame
  Left = 0
  Top = 0
  Width = 356
  Height = 190
  TabOrder = 0
  object bvl1: TBevel
    Left = 3
    Top = 89
    Width = 350
    Height = 2
  end
  object bvl2: TBevel
    Left = 3
    Top = 152
    Width = 350
    Height = 2
  end
  object pnlCartPosition: TPanel
    Left = 3
    Top = 3
    Width = 350
    Height = 83
    BevelOuter = bvNone
    TabOrder = 0
    object lblCartPosTitle: TLabel
      Left = 8
      Top = 0
      Width = 138
      Height = 15
      Caption = 'Cartridge positioning:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblCartPosNote: TLabel
      Left = 8
      Top = 60
      Width = 276
      Height = 15
      Caption = 'This selection applies to this use of the wizard only.'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object rbAutomatic: TRadioButton
      Left = 8
      Top = 18
      Width = 257
      Height = 17
      Caption = 'Automatic slot selection (Recommended)'
      Checked = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      TabStop = True
    end
    object rbManual: TRadioButton
      Left = 8
      Top = 38
      Width = 257
      Height = 17
      Caption = 'Manual slot selection'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
    end
  end
  object pnlMostUsed: TPanel
    Left = 3
    Top = 94
    Width = 350
    Height = 55
    BevelOuter = bvNone
    TabOrder = 1
    object lblPreferred: TLabel
      Left = 8
      Top = 1
      Width = 334
      Height = 15
      Caption = 'Select material most used in Single material mode (optional):'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object comboPreferred: TComboBox
      Left = 8
      Top = 22
      Width = 143
      Height = 23
      Style = csDropDownList
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ItemHeight = 0
      ParentFont = False
      TabOrder = 0
    end
  end
  object pnlSecondFlushing: TPanel
    Left = 3
    Top = 157
    Width = 350
    Height = 30
    BevelOuter = bvNone
    TabOrder = 2
    object chkSecondFlushingActive: TCheckBox
      Left = 8
      Top = 3
      Width = 193
      Height = 17
      Caption = '"Flush again" options'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
  end
end
