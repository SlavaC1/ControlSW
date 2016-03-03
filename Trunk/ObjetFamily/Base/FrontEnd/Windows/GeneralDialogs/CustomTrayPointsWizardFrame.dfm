object TrayPointsWizardFrame: TTrayPointsWizardFrame
  Left = 0
  Top = 0
  Width = 320
  Height = 183
  TabOrder = 0
  object SubTitleLine1Label: TLabel
    Left = 8
    Top = 0
    Width = 281
    Height = 17
    AutoSize = False
    Caption = '- Click the arrows to adjust the tray level.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object SubTitleLine2Label: TLabel
    Left = 8
    Top = 17
    Width = 248
    Height = 13
    Caption = '- Remove the right UV lamp and attach the indicator.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object IndicatorInPlaceCheckBox: TCheckBox
    Left = 8
    Top = 160
    Width = 297
    Height = 17
    Caption = 'The indicator is attached and resting on the tray.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
  inline TZAxisFrame1: TZAxisFrame
    Left = 85
    Top = 40
    Width = 137
    Height = 115
    AutoSize = True
    TabOrder = 0
    ExplicitLeft = 85
    ExplicitTop = 40
    ExplicitHeight = 115
    inherited UpDownAxisFrame: TUpDownAxisFrame
      Height = 115
      ExplicitHeight = 115
      inherited MainPanel: TPanel
        Height = 115
        ExplicitHeight = 115
        inherited UpSpeedButton: TSpeedButton
          Top = 19
          ExplicitTop = 19
        end
        inherited DownSpeedButton: TSpeedButton
          Top = 73
          ExplicitTop = 73
        end
        inherited Label: TLabel
          Top = 5
          Margins.Bottom = 0
          ExplicitTop = 5
        end
        inherited SlowModeCheckBox: TCheckBox
          Top = 150
          Visible = False
          ExplicitTop = 150
        end
      end
    end
  end
end
