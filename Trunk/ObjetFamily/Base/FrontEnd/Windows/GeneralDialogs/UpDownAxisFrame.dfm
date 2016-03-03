object UpDownAxisFrame: TUpDownAxisFrame
  Left = 0
  Top = 0
  Width = 137
  Height = 145
  AutoSize = True
  TabOrder = 0
  object MainPanel: TPanel
    Left = 0
    Top = 0
    Width = 137
    Height = 145
    BevelOuter = bvLowered
    TabOrder = 0
    object UpSpeedButton: TSpeedButton
      Tag = 4
      Left = 55
      Top = 28
      Width = 27
      Height = 27
      Enabled = False
      NumGlyphs = 2
      OnMouseDown = ButtonMouseDown
      OnMouseUp = ButtonMouseUp
    end
    object DownSpeedButton: TSpeedButton
      Tag = 5
      Left = 55
      Top = 82
      Width = 27
      Height = 27
      Enabled = False
      NumGlyphs = 2
      OnMouseDown = ButtonMouseDown
      OnMouseUp = ButtonMouseUp
    end
    object Label: TLabel
      Left = 43
      Top = 14
      Width = 50
      Height = 13
      Caption = 'Axis Name'
    end
    object SlowModeCheckBox: TCheckBox
      Left = 30
      Top = 119
      Width = 77
      Height = 17
      Caption = 'Slow Mode'
      TabOrder = 0
    end
  end
  object LimitsTimer: TTimer
    Enabled = False
    OnTimer = LimitsTimerTimer
    Left = 104
    Top = 56
  end
end
