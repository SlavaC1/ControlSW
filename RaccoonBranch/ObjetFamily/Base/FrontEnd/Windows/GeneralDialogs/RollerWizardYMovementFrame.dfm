object RollerWizardYMoveFrame: TRollerWizardYMoveFrame
  Left = 0
  Top = 0
  Width = 349
  Height = 240
  TabOrder = 0
  object SubTitleLine1Label: TLabel
    Left = 7
    Top = 8
    Width = 302
    Height = 26
    AutoSize = False
    Caption = 'Click the arrows to move the Y-Axis'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  inline TYAxisFrame1: TYAxisFrame
    Left = 91
    Top = 40
    Width = 137
    Height = 130
    AutoSize = True
    TabOrder = 0
    ExplicitLeft = 91
    ExplicitTop = 40
    ExplicitHeight = 130
    inherited UpDownAxisFrame: TUpDownAxisFrame
      Height = 130
      ExplicitHeight = 130
      inherited MainPanel: TPanel
        Height = 130
        ExplicitHeight = 130
        inherited UpSpeedButton: TSpeedButton
          OnClick = YUpSpeedButtonClick
          OnMouseDown = nil
          OnMouseUp = nil
        end
        inherited DownSpeedButton: TSpeedButton
          OnClick = YDownSpeedButtonClick
          OnMouseDown = nil
          OnMouseUp = nil
        end
        inherited SlowModeCheckBox: TCheckBox
          Top = 250
          Visible = False
          ExplicitTop = 250
        end
      end
    end
  end
end
