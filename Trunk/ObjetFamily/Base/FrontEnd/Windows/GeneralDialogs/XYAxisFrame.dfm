object XYAxisFrame: TXYAxisFrame
  Left = 0
  Top = 0
  Width = 137
  Height = 145
  AutoScroll = False
  AutoSize = True
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 137
    Height = 145
    BevelOuter = bvLowered
    TabOrder = 0
    object XLeftSpeedButton: TSpeedButton
      Left = 28
      Top = 55
      Width = 27
      Height = 27
      NumGlyphs = 2
    end
    object XRightSpeedButton: TSpeedButton
      Left = 82
      Top = 55
      Width = 27
      Height = 27
      NumGlyphs = 2
    end
    object XLabel: TLabel
      Left = 18
      Top = 62
      Width = 7
      Height = 13
      Caption = 'X'
    end
    inline TYAxisFrame1: TYAxisFrame
      Left = 55
      Top = 0
      Width = 27
      Height = 145
      AutoSize = True
      TabOrder = 0
      inherited UpDownAxisFrame: TUpDownAxisFrame
        Width = 27
        inherited MainPanel: TPanel
          Width = 27
          BevelOuter = bvNone
          inherited UpSpeedButton: TSpeedButton
            Left = 0
          end
          inherited DownSpeedButton: TSpeedButton
            Left = 0
          end
          inherited Label: TLabel
            Left = 10
          end
          inherited SlowModeCheckBox: TCheckBox
            Top = 220
          end
        end
      end
    end
  end
  object SlowModeCheckBox: TCheckBox
    Left = 30
    Top = 119
    Width = 77
    Height = 17
    Caption = 'Slow Mode'
    TabOrder = 1
    OnClick = SlowModeCheckBoxClick
  end
end
