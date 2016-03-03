object CustomResinAdvancedSettingsFrame: TCustomResinAdvancedSettingsFrame
  Left = 0
  Top = 0
  Width = 356
  Height = 190
  TabOrder = 0
  object bvl1: TBevel
    Left = 3
    Top = 96
    Width = 350
    Height = 2
  end
  object pnlCartPosition: TPanel
    Left = 3
    Top = 3
    Width = 350
    Height = 87
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
      Top = 67
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
      Top = 21
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
      Top = 44
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
    Top = 104
    Width = 350
    Height = 34
    BevelOuter = bvNone
    TabOrder = 1
    object lblOptionalCaption: TLabel
      Left = 8
      Top = 0
      Width = 112
      Height = 15
      Caption = 'Optional settings:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object pnlSecondFlushing: TPanel
    Left = 3
    Top = 136
    Width = 350
    Height = 50
    BevelOuter = bvNone
    TabOrder = 2
    object lblOptNote: TLabel
      Left = 8
      Top = 22
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
    object chkSecondFlushingActive: TCheckBox
      Left = 8
      Top = 2
      Width = 193
      Height = 17
      Caption = 'Enable "flush again" options'
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
