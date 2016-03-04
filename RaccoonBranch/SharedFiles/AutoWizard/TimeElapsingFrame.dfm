object TimeElapsing: TTimeElapsing
  Left = 0
  Top = 0
  Width = 352
  Height = 158
  TabOrder = 0
  object SubTitleLabel: TLabel
    Left = 4
    Top = 16
    Width = 347
    Height = 16
    AutoSize = False
    Caption = 'Sub Title'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object TimeElapsingPanel: TPanel
    Left = 4
    Top = 53
    Width = 347
    Height = 41
    BevelOuter = bvLowered
    Caption = '00:00'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clInactiveCaption
    Font.Height = -27
    Font.Name = 'David'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 0
  end
  object TimeElapsingTimer: TTimer
    Enabled = False
    OnTimer = TimeElapsingTimerTimer
    Left = 40
    Top = 112
  end
end
