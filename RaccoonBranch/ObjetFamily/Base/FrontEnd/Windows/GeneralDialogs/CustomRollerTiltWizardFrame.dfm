object RollerTiltWizardFrame: TRollerTiltWizardFrame
  Left = 0
  Top = 0
  Width = 329
  Height = 183
  TabOrder = 0
  object SubTitleLine1Label: TLabel
    Left = 8
    Top = 8
    Width = 321
    Height = 17
    AutoSize = False
    Caption = 
      'Click the arrows to raise the tray until the indicator touches t' +
      'he roller.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object IndicatorInPlaceCheckBox: TCheckBox
    Left = 8
    Top = 160
    Width = 297
    Height = 17
    Caption = 'The tray height is adjusted.'
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
