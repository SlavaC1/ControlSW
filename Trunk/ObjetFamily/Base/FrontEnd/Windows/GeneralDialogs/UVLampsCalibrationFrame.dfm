object UVLampsCalibrationWizardFrame: TUVLampsCalibrationWizardFrame
  Left = 0
  Top = 0
  Width = 354
  Height = 212
  TabOrder = 0
  object Label1: TLabel
    Left = 30
    Top = 8
    Width = 84
    Height = 13
    Caption = 'Currently reading'
  end
  object CurrentLampLabel: TLabel
    Left = 118
    Top = 8
    Width = 79
    Height = 13
    Caption = 'Right UV lamp'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object CurrentModeLabel: TLabel
    Left = 217
    Top = 8
    Width = 103
    Height = 13
    Caption = 'High Quality mode'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 203
    Top = 8
    Width = 8
    Height = 13
    Caption = 'in'
  end
  object LampStabPanel: TGroupBox
    Left = 32
    Top = 40
    Width = 289
    Height = 41
    Caption = 'Lamp Initialization'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 0
    object StabTimeLabel: TLabel
      Left = 217
      Top = 16
      Width = 45
      Height = 13
      Caption = '(~3 Min.)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object LampStabProgressBar: TProgressBar
      Left = 11
      Top = 17
      Width = 200
      Height = 16
      Smooth = True
      Step = 1
      TabOrder = 0
    end
  end
  object MeasureUVPanel: TGroupBox
    Left = 32
    Top = 87
    Width = 289
    Height = 41
    Caption = 'UV measurement'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 1
    object CyclesNumLabel: TLabel
      Left = 217
      Top = 16
      Width = 54
      Height = 13
      Caption = '(25 Cycles)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object MeasureUVProgressBar: TProgressBar
      Left = 11
      Top = 17
      Width = 200
      Height = 16
      Smooth = True
      Step = 1
      TabOrder = 0
    end
  end
  object GroupBox3: TGroupBox
    Left = 32
    Top = 134
    Width = 289
    Height = 41
    Caption = 'Overall progress'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 2
    object OverallProgressBar: TProgressBar
      Left = 11
      Top = 17
      Width = 262
      Height = 16
      Smooth = True
      Step = 1
      TabOrder = 0
    end
  end
end
