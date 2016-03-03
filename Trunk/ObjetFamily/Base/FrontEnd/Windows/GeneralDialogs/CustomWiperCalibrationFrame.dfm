object CustomWiperCalibrationFrame: TCustomWiperCalibrationFrame
  Left = 0
  Top = 0
  Width = 353
  Height = 187
  TabOrder = 0
  DesignSize = (
    353
    187)
  object Label1: TLabel
    Left = 25
    Top = 134
    Width = 66
    Height = 39
    Caption = 'Confirm when positioned  correctly:'
    WordWrap = True
  end
  object Bevel1: TBevel
    Left = 226
    Top = 0
    Width = 120
    Height = 120
  end
  object Bevel3: TBevel
    Left = 226
    Top = 83
    Width = 120
    Height = 9
    Shape = bsTopLine
  end
  object XCheckBox: TCheckBox
    Left = 124
    Top = 158
    Width = 103
    Height = 17
    Caption = 'X-axis OK'
    Enabled = False
    TabOrder = 0
  end
  object YCheckBox: TCheckBox
    Left = 231
    Top = 138
    Width = 97
    Height = 17
    Caption = 'Y-axis OK'
    Enabled = False
    TabOrder = 1
  end
  object TAxeCheckBox: TCheckBox
    Left = 231
    Top = 158
    Width = 97
    Height = 17
    Caption = 'T-axis OK'
    Enabled = False
    TabOrder = 2
  end
  object WiperBladeTiltCheckBox: TCheckBox
    Left = 124
    Top = 138
    Width = 92
    Height = 17
    Caption = 'Wiper tilt OK'
    TabOrder = 3
  end
  inline TAxisTFrame1: TAxisTFrame
    Left = 262
    Top = 1
    Width = 50
    Height = 80
    AutoSize = True
    TabOrder = 4
    ExplicitLeft = 262
    ExplicitTop = 1
    ExplicitWidth = 50
    ExplicitHeight = 80
    inherited UpDownAxisFrame: TUpDownAxisFrame
      Width = 50
      Height = 80
      ExplicitWidth = 50
      ExplicitHeight = 80
      inherited MainPanel: TPanel
        Width = 50
        Height = 80
        BevelOuter = bvNone
        ExplicitWidth = 50
        ExplicitHeight = 80
        inherited UpSpeedButton: TSpeedButton
          Left = 12
          Top = 16
          OnMouseDown = UpDownAxisFrameUpSpeedButtonMouseDown
          OnMouseUp = UpDownAxisFrameUpSpeedButtonMouseUp
          ExplicitLeft = 12
          ExplicitTop = 16
        end
        inherited DownSpeedButton: TSpeedButton
          Left = 12
          Top = 48
          OnMouseUp = UpDownAxisFrameUpSpeedButtonMouseUp
          ExplicitLeft = 12
          ExplicitTop = 48
        end
        inherited Label: TLabel
          Left = 22
          Top = 2
          ExplicitLeft = 22
          ExplicitTop = 2
        end
        inherited SlowModeCheckBox: TCheckBox
          Visible = False
        end
      end
    end
  end
  inline TXYAxisFrame1: TXYAxisFrame
    Left = 6
    Top = 0
    Width = 137
    Height = 157
    AutoSize = True
    TabOrder = 5
    ExplicitLeft = 6
    ExplicitHeight = 157
    inherited Panel1: TPanel
      Height = 120
      ExplicitHeight = 120
      inherited XLeftSpeedButton: TSpeedButton
        Top = 46
        ExplicitTop = 46
      end
      inherited XRightSpeedButton: TSpeedButton
        Top = 46
        ExplicitTop = 46
      end
      inherited XLabel: TLabel
        Top = 53
        ExplicitTop = 53
      end
      inherited TYAxisFrame1: TYAxisFrame
        Top = 1
        Height = 106
        AutoSize = False
        ExplicitTop = 1
        ExplicitHeight = 106
        inherited UpDownAxisFrame: TUpDownAxisFrame
          Height = 106
          ExplicitHeight = 106
          inherited MainPanel: TPanel
            Height = 106
            ExplicitHeight = 106
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
          end
        end
      end
    end
    inherited SlowModeCheckBox: TCheckBox
      Left = 0
      Top = 177
      Anchors = [akLeft, akBottom]
      Enabled = False
      Visible = False
      ExplicitLeft = 0
      ExplicitTop = 140
    end
  end
  object TMoveEdit: TEdit
    Left = 274
    Top = 92
    Width = 43
    Height = 21
    Hint = 'Relative value in steps'
    TabOrder = 6
    Text = '0'
    OnChange = TMoveEditChange
  end
  object GoButton: TButton
    Left = 245
    Top = 90
    Width = 25
    Height = 25
    Caption = 'Go'
    Default = True
    TabOrder = 7
    OnClick = GoButtonClick
  end
  object SlowModeCheckBox: TCheckBox
    Left = 146
    Top = 56
    Width = 77
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = 'Slow Speed'
    Checked = True
    State = cbChecked
    TabOrder = 8
    OnClick = SlowModeCheckBoxClick
  end
  object TMoveUpDown: TUpDown
    Left = 317
    Top = 92
    Width = 15
    Height = 21
    Associate = TMoveEdit
    Min = -3000
    Max = 3000
    TabOrder = 9
  end
end
