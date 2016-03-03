object OCBTesterForm: TOCBTesterForm
  Left = 216
  Top = 45
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'OCB Tester'
  ClientHeight = 663
  ClientWidth = 522
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object bh: TPageControl
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 516
    Height = 640
    Margins.Bottom = 0
    ActivePage = TabSheet5
    Align = alClient
    TabOrder = 0
    object TabSheet4: TTabSheet
      Caption = 'General'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox3: TGroupBox
        AlignWithMargins = True
        Left = 3
        Top = 3
        Width = 502
        Height = 606
        Align = alClient
        Padding.Left = 5
        Padding.Right = 5
        Padding.Bottom = 5
        TabOrder = 0
        object GroupBox1: TGroupBox
          Left = 7
          Top = 207
          Width = 488
          Height = 126
          Align = alTop
          Caption = 'UV Lamps'
          TabOrder = 2
          object Label99: TLabel
            Left = 129
            Top = 76
            Width = 75
            Height = 13
            Hint = 'A/D Units'
            Caption = 'Ignition Timeout'
            ParentShowHint = False
            ShowHint = True
          end
          object RightUVLampImage: TImage
            Tag = 26
            Left = 413
            Top = 34
            Width = 16
            Height = 16
            AutoSize = True
            Center = True
            Transparent = True
          end
          object Label91: TLabel
            Left = 400
            Top = 14
            Width = 43
            Height = 13
            Caption = 'Right UV'
          end
          object LeftUVLampImage: TImage
            Tag = 26
            Left = 367
            Top = 34
            Width = 16
            Height = 16
            AutoSize = True
            Center = True
            Transparent = True
          end
          object Label86: TLabel
            Left = 357
            Top = 14
            Width = 36
            Height = 13
            Caption = 'Left UV'
          end
          object Label115: TLabel
            Left = 253
            Top = 76
            Width = 99
            Height = 13
            Hint = 'A/D Units'
            Caption = 'Post Ignition Timeout'
            ParentShowHint = False
            ShowHint = True
          end
          object Label116: TLabel
            Left = 362
            Top = 76
            Width = 70
            Height = 13
            Hint = 'A/D Units'
            Caption = 'Sensor Bypass'
            ParentShowHint = False
            ShowHint = True
          end
          object UVLampsButton: TButton
            Left = 17
            Top = 30
            Width = 75
            Height = 25
            Action = UVLampsOn
            TabOrder = 1
          end
          object IgnitionTimeoutEdit: TEdit
            Left = 123
            Top = 90
            Width = 86
            Height = 21
            Hint = 'A/D Units'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 3
          end
          object SetUVLampsParameterButton: TButton
            Left = 14
            Top = 88
            Width = 81
            Height = 25
            Caption = 'Set Parameters'
            TabOrder = 2
            OnClick = SetUVLampsParameterButtonClick
          end
          object PostIgnitionTimeoutEdit: TEdit
            Left = 259
            Top = 90
            Width = 86
            Height = 21
            TabOrder = 4
          end
          object SensorBypassCheckBox: TCheckBox
            Left = 384
            Top = 92
            Width = 25
            Height = 17
            TabOrder = 5
          end
          object ActiveLampsMaskGroupBox: TGroupBox
            Left = 123
            Top = 19
            Width = 222
            Height = 46
            Caption = 'Active Lamps Mask'
            TabOrder = 0
            object Label87: TLabel
              Left = 11
              Top = 28
              Width = 26
              Height = 13
              Hint = 'A/D Units'
              Caption = 'None'
              ParentShowHint = False
              ShowHint = True
            end
            object Label88: TLabel
              Left = 117
              Top = 28
              Width = 49
              Height = 13
              Hint = 'A/D Units'
              Caption = 'Right Only'
              ParentShowHint = False
              ShowHint = True
            end
            object Label98: TLabel
              Left = 61
              Top = 28
              Width = 42
              Height = 13
              Hint = 'A/D Units'
              Caption = 'Left Only'
              ParentShowHint = False
              ShowHint = True
            end
            object Label117: TLabel
              Left = 189
              Top = 28
              Width = 22
              Height = 13
              Hint = 'A/D Units'
              Caption = 'Both'
              ParentShowHint = False
              ShowHint = True
            end
            object RadioButton1: TRadioButton
              Left = 16
              Top = 13
              Width = 17
              Height = 17
              Checked = True
              TabOrder = 0
              TabStop = True
            end
            object RadioButton2: TRadioButton
              Tag = 2
              Left = 74
              Top = 13
              Width = 17
              Height = 17
              TabOrder = 1
            end
            object RadioButton3: TRadioButton
              Tag = 1
              Left = 133
              Top = 13
              Width = 17
              Height = 17
              TabOrder = 2
            end
            object RadioButton4: TRadioButton
              Tag = 3
              Left = 192
              Top = 13
              Width = 17
              Height = 17
              TabOrder = 3
            end
          end
        end
        object TrayHeaterGroupBox: TGroupBox
          Left = 7
          Top = 86
          Width = 488
          Height = 121
          Align = alTop
          Caption = 'Tray Heating'
          TabOrder = 1
          object Label79: TLabel
            Left = 133
            Top = 70
            Width = 60
            Height = 13
            Hint = 'A/D Units'
            Caption = 'Temperature'
            ParentShowHint = False
            ShowHint = True
          end
          object Label80: TLabel
            Left = 256
            Top = 70
            Width = 49
            Height = 13
            Hint = 'Percentage'
            Caption = 'Margin (%)'
            ParentShowHint = False
            ShowHint = True
          end
          object Label81: TLabel
            Left = 379
            Top = 70
            Width = 38
            Height = 13
            Hint = 'In Seconds'
            Caption = 'Timeout'
            ParentShowHint = False
            ShowHint = True
          end
          object Label82: TLabel
            Left = 115
            Top = 14
            Width = 97
            Height = 13
            Hint = 'A/D Units'
            Caption = 'Current Temperature'
            ParentShowHint = False
            ShowHint = True
          end
          object Label83: TLabel
            Left = 227
            Top = 14
            Width = 107
            Height = 13
            Caption = 'Temperature In Margin'
          end
          object TrayInsertedImage: TImage
            Tag = 26
            Left = 390
            Top = 34
            Width = 16
            Height = 16
            AutoSize = True
            Center = True
            Transparent = True
          end
          object Label84: TLabel
            Left = 367
            Top = 14
            Width = 62
            Height = 13
            Caption = 'Tray Inserted'
          end
          object HeatingBottun: TButton
            Left = 17
            Top = 30
            Width = 75
            Height = 25
            Action = HeatingOnAction
            TabOrder = 0
          end
          object SetTrayTempEdit: TEdit
            Left = 115
            Top = 84
            Width = 97
            Height = 21
            Hint = 'A/D Units'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 4
          end
          object SetTrayMarginEdit: TEdit
            Left = 232
            Top = 84
            Width = 97
            Height = 21
            Hint = 'Percentage'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 5
            Text = '10'
          end
          object SetTrayTimeoutEdit: TEdit
            Left = 350
            Top = 84
            Width = 97
            Height = 21
            Hint = 'In Seconds'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 6
            Text = '30'
          end
          object CurrentTemperaturePanel: TPanel
            Left = 115
            Top = 32
            Width = 97
            Height = 21
            Hint = 'A/D Units'
            BevelOuter = bvLowered
            Caption = '???'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
          object SetTrayParametersButton: TButton
            Left = 14
            Top = 82
            Width = 81
            Height = 25
            Caption = 'Set Parameters'
            TabOrder = 3
            OnClick = SetTrayParametersButtonClick
          end
          object TemperatureOKPanel: TPanel
            Left = 232
            Top = 32
            Width = 97
            Height = 21
            BevelOuter = bvLowered
            Caption = 'No'
            TabOrder = 2
          end
        end
        object PowerGroupBox: TGroupBox
          Left = 7
          Top = 15
          Width = 488
          Height = 71
          Align = alTop
          Caption = 'Power'
          TabOrder = 0
          object PowerImage: TImage
            Tag = 26
            Left = 390
            Top = 29
            Width = 16
            Height = 16
            AutoSize = True
            Center = True
            Transparent = True
          end
          object Label85: TLabel
            Left = 383
            Top = 14
            Width = 30
            Height = 13
            Caption = 'Power'
          end
          object PowerButton: TButton
            Left = 17
            Top = 25
            Width = 75
            Height = 25
            Action = PowerOnAction
            TabOrder = 0
          end
          object PowerOffDelayEdit: TLabeledEdit
            Left = 256
            Top = 27
            Width = 85
            Height = 21
            BiDiMode = bdLeftToRight
            EditLabel.Width = 77
            EditLabel.Height = 13
            EditLabel.BiDiMode = bdLeftToRight
            EditLabel.Caption = 'Power Off Delay'
            EditLabel.ParentBiDiMode = False
            ParentBiDiMode = False
            TabOrder = 2
            Text = '0'
          end
          object PowerOnDelayEdit: TLabeledEdit
            Left = 128
            Top = 27
            Width = 85
            Height = 21
            BiDiMode = bdLeftToRight
            EditLabel.Width = 77
            EditLabel.Height = 13
            EditLabel.BiDiMode = bdLeftToRight
            EditLabel.Caption = 'Power On Delay'
            EditLabel.ParentBiDiMode = False
            ParentBiDiMode = False
            TabOrder = 1
            Text = '0'
          end
        end
        object PurgeBox: TGroupBox
          Left = 7
          Top = 333
          Width = 488
          Height = 70
          Align = alTop
          Caption = 'Purge'
          TabOrder = 3
          object Label219: TLabel
            Left = 139
            Top = 12
            Width = 54
            Height = 13
            Caption = 'Purge Time'
          end
          object Label100: TLabel
            Left = 368
            Top = 12
            Width = 61
            Height = 13
            Caption = 'Purge Status'
          end
          object PurgeStatusImage: TImage
            Tag = 26
            Left = 390
            Top = 30
            Width = 16
            Height = 16
            AutoSize = True
            Center = True
            Transparent = True
          end
          object PerformPurge_TimeEdit: TEdit
            Left = 123
            Top = 28
            Width = 86
            Height = 21
            TabOrder = 1
          end
          object PerformPurgeButton: TButton
            Left = 14
            Top = 26
            Width = 83
            Height = 25
            Caption = 'Perform Purge'
            TabOrder = 0
            OnClick = PerformPurgeButtonClick
          end
        end
        object SoftwareGroupBox: TGroupBox
          Left = 7
          Top = 403
          Width = 488
          Height = 122
          Align = alTop
          Caption = 'Software'
          TabOrder = 4
          object Label246: TLabel
            Left = 138
            Top = 12
            Width = 76
            Height = 13
            Caption = 'External Version'
          end
          object Label247: TLabel
            Left = 139
            Top = 60
            Width = 73
            Height = 13
            Caption = 'Internal Version'
          end
          object ExternalVersionEdit: TEdit
            Tag = 143
            Left = 137
            Top = 28
            Width = 75
            Height = 21
            TabOrder = 2
          end
          object InternalVersionEdit: TEdit
            Tag = 144
            Left = 137
            Top = 76
            Width = 75
            Height = 21
            TabOrder = 3
          end
          object SWResetButton: TButton
            Left = 17
            Top = 26
            Width = 75
            Height = 25
            Caption = 'SW Reset'
            TabOrder = 0
            OnClick = SWResetButtonClick
          end
          object GetOCBSoftwareVersion: TButton
            Left = 17
            Top = 73
            Width = 89
            Height = 25
            Caption = 'Get SW Version'
            TabOrder = 1
            OnClick = GetOCBSoftwareVersionClick
          end
          object MSC1HWEdit: TLabeledEdit
            Left = 241
            Top = 28
            Width = 51
            Height = 21
            EditLabel.Width = 46
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC1 hw'
            TabOrder = 4
          end
          object MSC3FWEdit: TLabeledEdit
            Left = 298
            Top = 76
            Width = 53
            Height = 21
            EditLabel.Width = 43
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC3 fw'
            TabOrder = 5
          end
          object MSC1FWEdit: TLabeledEdit
            Left = 298
            Top = 28
            Width = 53
            Height = 21
            EditLabel.Width = 43
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC1 fw'
            TabOrder = 6
          end
          object MSC3HWEdit: TLabeledEdit
            Left = 241
            Top = 76
            Width = 51
            Height = 21
            EditLabel.Width = 46
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC3 hw'
            TabOrder = 7
          end
          object MSC2HWEdit: TLabeledEdit
            Left = 357
            Top = 28
            Width = 51
            Height = 21
            EditLabel.Width = 46
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC2 hw'
            TabOrder = 8
          end
          object MSC4HWEdit: TLabeledEdit
            Left = 357
            Top = 76
            Width = 51
            Height = 21
            EditLabel.Width = 46
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC4 hw'
            TabOrder = 9
          end
          object MSC2FWEdit: TLabeledEdit
            Left = 414
            Top = 28
            Width = 53
            Height = 21
            EditLabel.Width = 43
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC2 fw'
            TabOrder = 10
          end
          object MSC4FWEdit: TLabeledEdit
            Left = 414
            Top = 76
            Width = 53
            Height = 21
            EditLabel.Width = 43
            EditLabel.Height = 13
            EditLabel.Caption = 'MSC4 fw'
            TabOrder = 11
          end
        end
        object DoorGroupBox: TGroupBox
          Left = 7
          Top = 525
          Width = 488
          Height = 74
          Align = alClient
          Caption = 'Door'
          TabOrder = 5
          object Label101: TLabel
            Left = 164
            Top = 10
            Width = 56
            Height = 13
            Hint = 'A/D Units'
            Caption = 'Door Status'
            ParentShowHint = False
            ShowHint = True
          end
          object DoorButton: TButton
            Left = 17
            Top = 24
            Width = 75
            Height = 25
            Action = LockDoorAction
            TabOrder = 0
          end
          object DoorStatusEdit: TEdit
            Left = 120
            Top = 26
            Width = 145
            Height = 21
            Hint = 'A/D Units'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
        end
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'A/D'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Just_a_GroupBox: TGroupBox
        Left = 8
        Top = 3
        Width = 481
        Height = 598
        TabOrder = 0
        object A2DGroupBox: TGroupBox
          Left = 12
          Top = 10
          Width = 457
          Height = 423
          Caption = 'A/D Values'
          TabOrder = 0
          object Label19: TLabel
            Left = 235
            Top = 24
            Width = 39
            Height = 13
            Caption = 'Vacuum'
          end
          object Label20: TLabel
            Left = 235
            Top = 57
            Width = 78
            Height = 13
            Caption = 'Vacuum Voltage'
          end
          object Label21: TLabel
            Left = 235
            Top = 91
            Width = 108
            Height = 13
            Caption = 'Vacuum Pump Voltage'
          end
          object Label22: TLabel
            Left = 235
            Top = 124
            Width = 78
            Height = 13
            Caption = 'Tray Temprature'
          end
          object Label23: TLabel
            Left = 235
            Top = 158
            Width = 75
            Height = 13
            Caption = 'Spare (ANLG 1)'
          end
          object Label24: TLabel
            Left = 235
            Top = 191
            Width = 75
            Height = 13
            Caption = 'Spare (ANLG 2)'
          end
          object Label25: TLabel
            Left = 235
            Top = 225
            Width = 75
            Height = 13
            Caption = 'Spare (ANLG 3)'
          end
          object Label26: TLabel
            Left = 235
            Top = 258
            Width = 75
            Height = 13
            Caption = 'Spare (ANLG 4)'
          end
          object Label59: TLabel
            Left = 235
            Top = 292
            Width = 75
            Height = 13
            Caption = 'Spare (ANLG 5)'
          end
          object Label60: TLabel
            Left = 235
            Top = 325
            Width = 75
            Height = 13
            Caption = 'Spare (ANLG 6)'
          end
          object Label61: TLabel
            Left = 235
            Top = 359
            Width = 83
            Height = 13
            Caption = 'Spare (ANLOG 1)'
          end
          object Label62: TLabel
            Left = 235
            Top = 393
            Width = 83
            Height = 13
            Caption = 'Spare (ANLOG 2)'
          end
          object Label8: TLabel
            Left = 16
            Top = 24
            Width = 72
            Height = 13
            Caption = 'Right UV Lamp'
          end
          object Label9: TLabel
            Left = 16
            Top = 57
            Width = 65
            Height = 13
            Caption = 'Left UV Lamp'
          end
          object Label5: TLabel
            Left = 16
            Top = 91
            Width = 75
            Height = 13
            Caption = 'Model 1 Weight'
          end
          object Label11: TLabel
            Left = 16
            Top = 124
            Width = 75
            Height = 13
            Caption = 'Model 2 Weight'
          end
          object Label10: TLabel
            Left = 16
            Top = 158
            Width = 75
            Height = 13
            Caption = 'Model 3 Weight'
          end
          object Label12: TLabel
            Left = 16
            Top = 191
            Width = 83
            Height = 13
            Caption = 'Support 1 Weight'
          end
          object Label13: TLabel
            Left = 16
            Top = 225
            Width = 83
            Height = 13
            Caption = 'Support 2 Weight'
          end
          object Label14: TLabel
            Left = 16
            Top = 258
            Width = 83
            Height = 13
            Caption = 'Support 3 Weight'
          end
          object Label15: TLabel
            Left = 16
            Top = 292
            Width = 68
            Height = 13
            Caption = 'Waste Weight'
          end
          object Label16: TLabel
            Left = 16
            Top = 325
            Width = 58
            Height = 13
            Caption = 'Tray Weight'
          end
          object Label17: TLabel
            Left = 16
            Top = 393
            Width = 104
            Height = 13
            Caption = 'Ambient 2 Temprature'
          end
          object Label18: TLabel
            Left = 16
            Top = 359
            Width = 104
            Height = 13
            Caption = 'Ambient 1 Temprature'
          end
          object A2DPanel24: TPanel
            Tag = 32
            Left = 364
            Top = 389
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 23
          end
          object A2DPanel23: TPanel
            Tag = 24
            Left = 364
            Top = 355
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 21
          end
          object A2DPanel22: TPanel
            Tag = 29
            Left = 364
            Top = 321
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 19
          end
          object A2DPanel21: TPanel
            Tag = 12
            Left = 364
            Top = 288
            Width = 68
            Height = 19
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 17
          end
          object A2DPanel20: TPanel
            Tag = 31
            Left = 364
            Top = 254
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 15
          end
          object A2DPanel19: TPanel
            Tag = 30
            Left = 364
            Top = 221
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 13
          end
          object A2DPanel18: TPanel
            Tag = 10
            Left = 364
            Top = 187
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 11
          end
          object A2DPanel17: TPanel
            Tag = 11
            Left = 364
            Top = 154
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 9
          end
          object A2DPanel16: TPanel
            Tag = 17
            Left = 364
            Top = 120
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 7
          end
          object A2DPanel15: TPanel
            Tag = 26
            Left = 364
            Top = 87
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 5
          end
          object A2DPanel14: TPanel
            Tag = 25
            Left = 364
            Top = 53
            Width = 68
            Height = 19
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 3
          end
          object A2DPanel13: TPanel
            Tag = 20
            Left = 364
            Top = 20
            Width = 68
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 1
          end
          object A2DPanel12: TPanel
            Tag = 21
            Left = 142
            Top = 355
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 20
          end
          object A2DPanel11: TPanel
            Tag = 22
            Left = 142
            Top = 389
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 22
          end
          object A2DPanel10: TPanel
            Tag = 27
            Left = 142
            Top = 321
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 18
          end
          object A2DPanel9: TPanel
            Tag = 16
            Left = 142
            Top = 288
            Width = 67
            Height = 19
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 16
          end
          object A2DPanel8: TPanel
            Tag = 13
            Left = 142
            Top = 254
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 14
          end
          object A2DPanel7: TPanel
            Tag = 23
            Left = 142
            Top = 221
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 12
          end
          object A2DPanel6: TPanel
            Tag = 19
            Left = 142
            Top = 187
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 10
          end
          object A2DPanel5: TPanel
            Tag = 14
            Left = 142
            Top = 154
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 8
          end
          object A2DPanel4: TPanel
            Tag = 15
            Left = 142
            Top = 120
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 6
          end
          object A2DPanel3: TPanel
            Tag = 18
            Left = 142
            Top = 87
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 4
          end
          object A2DPanel2: TPanel
            Tag = 28
            Left = 142
            Top = 53
            Width = 67
            Height = 19
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 2
          end
          object A2DPanel1: TPanel
            Tag = 9
            Left = 142
            Top = 20
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 0
          end
        end
        object A2D_CPU_GroupBox: TGroupBox
          Left = 12
          Top = 439
          Width = 457
          Height = 153
          Caption = 'CPU A/D'
          TabOrder = 1
          object Label1: TLabel
            Left = 16
            Top = 24
            Width = 82
            Height = 13
            Caption = 'VS Power Supply'
          end
          object Label2: TLabel
            Left = 16
            Top = 57
            Width = 89
            Height = 13
            Caption = 'VCC Power Supply'
          end
          object Label3: TLabel
            Left = 16
            Top = 123
            Width = 87
            Height = 13
            Caption = '24V Power Supply'
          end
          object Label4: TLabel
            Left = 16
            Top = 90
            Width = 87
            Height = 13
            Caption = '12V Power Supply'
          end
          object Label6: TLabel
            Left = 235
            Top = 24
            Width = 37
            Height = 13
            Caption = 'Spare 1'
          end
          object Label7: TLabel
            Left = 235
            Top = 57
            Width = 37
            Height = 13
            Caption = 'Spare 2'
          end
          object Label27: TLabel
            Left = 235
            Top = 90
            Width = 37
            Height = 13
            Caption = 'Spare 3'
          end
          object Label28: TLabel
            Left = 235
            Top = 123
            Width = 37
            Height = 13
            Caption = 'Spare 4'
          end
          object Panel1: TPanel
            Tag = 1
            Left = 142
            Top = 20
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 0
          end
          object Panel2: TPanel
            Tag = 2
            Left = 142
            Top = 53
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 2
          end
          object Panel3: TPanel
            Tag = 3
            Left = 142
            Top = 86
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 4
          end
          object Panel4: TPanel
            Tag = 4
            Left = 142
            Top = 119
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 6
          end
          object Panel5: TPanel
            Tag = 5
            Left = 364
            Top = 20
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 1
          end
          object Panel6: TPanel
            Tag = 6
            Left = 364
            Top = 53
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 3
          end
          object Panel7: TPanel
            Tag = 7
            Left = 364
            Top = 86
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 5
          end
          object Panel8: TPanel
            Tag = 8
            Left = 364
            Top = 119
            Width = 67
            Height = 20
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 7
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Actuators'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object ActuatorsGroupBox: TGroupBox
        Left = 8
        Top = 3
        Width = 481
        Height = 598
        Caption = 'Actuators'
        TabOrder = 0
        object Image1: TImage
          Tag = 26
          Left = 196
          Top = 24
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label29: TLabel
          Left = 19
          Top = 26
          Width = 72
          Height = 13
          Caption = 'Right UV Lamp'
        end
        object Label30: TLabel
          Left = 19
          Top = 98
          Width = 66
          Height = 13
          Caption = 'M1 Left Pump'
        end
        object Label31: TLabel
          Left = 19
          Top = 62
          Width = 65
          Height = 13
          Caption = 'Left UV Lamp'
        end
        object Label32: TLabel
          Left = 19
          Top = 135
          Width = 73
          Height = 13
          Caption = 'M1 Right Pump'
        end
        object Label33: TLabel
          Left = 19
          Top = 171
          Width = 66
          Height = 13
          Caption = 'M2 Left Pump'
        end
        object Label34: TLabel
          Left = 19
          Top = 208
          Width = 73
          Height = 13
          Caption = 'M2 Right Pump'
        end
        object Label35: TLabel
          Left = 19
          Top = 244
          Width = 66
          Height = 13
          Caption = 'M3 Left Pump'
        end
        object Label36: TLabel
          Left = 19
          Top = 281
          Width = 73
          Height = 13
          Caption = 'M3 Right Pump'
        end
        object Label37: TLabel
          Left = 19
          Top = 317
          Width = 32
          Height = 13
          Caption = 'Pumps'
        end
        object Label38: TLabel
          Left = 21
          Top = 566
          Width = 68
          Height = 13
          Caption = 'Model Valve 3'
        end
        object Label39: TLabel
          Left = 21
          Top = 386
          Width = 35
          Height = 13
          Caption = 'Dimmer'
        end
        object Label40: TLabel
          Left = 21
          Top = 423
          Width = 32
          Height = 13
          Caption = 'Buzzer'
        end
        object Label41: TLabel
          Left = 21
          Top = 459
          Width = 42
          Height = 13
          Caption = 'Air Valve'
        end
        object Label42: TLabel
          Left = 21
          Top = 496
          Width = 68
          Height = 13
          Caption = 'Model Valve 1'
        end
        object Label43: TLabel
          Left = 21
          Top = 532
          Width = 68
          Height = 13
          Caption = 'Model Valve 2'
        end
        object Image3: TImage
          Tag = 25
          Left = 196
          Top = 60
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image4: TImage
          Tag = 32
          Left = 196
          Top = 96
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image5: TImage
          Tag = 33
          Left = 196
          Top = 133
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image6: TImage
          Tag = 40
          Left = 196
          Top = 169
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image7: TImage
          Tag = 41
          Left = 196
          Top = 206
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image8: TImage
          Tag = 42
          Left = 196
          Top = 242
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image9: TImage
          Tag = 43
          Left = 196
          Top = 279
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image10: TImage
          Tag = 15
          Left = 196
          Top = 315
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image11: TImage
          Tag = 5
          Left = 198
          Top = 564
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image12: TImage
          Tag = 4
          Left = 198
          Top = 384
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image13: TImage
          Tag = 7
          Left = 198
          Top = 421
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image14: TImage
          Tag = 21
          Left = 198
          Top = 457
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image15: TImage
          Tag = 22
          Left = 198
          Top = 494
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image16: TImage
          Tag = 23
          Left = 198
          Top = 530
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label44: TLabel
          Left = 20
          Top = 353
          Width = 50
          Height = 13
          Caption = 'Oddor Fan'
        end
        object Image25: TImage
          Tag = 16
          Left = 198
          Top = 351
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label56: TLabel
          Left = 259
          Top = 26
          Width = 56
          Height = 13
          Caption = 'Tray Heater'
        end
        object Label57: TLabel
          Left = 259
          Top = 62
          Width = 50
          Height = 13
          Caption = 'Y, Z, T On'
        end
        object Label58: TLabel
          Left = 259
          Top = 98
          Width = 39
          Height = 13
          Caption = 'Vacuum'
        end
        object Label63: TLabel
          Left = 259
          Top = 135
          Width = 61
          Height = 13
          Caption = 'Waste Pump'
        end
        object Label64: TLabel
          Left = 259
          Top = 171
          Width = 57
          Height = 13
          Caption = 'Roller Pump'
        end
        object Label65: TLabel
          Left = 259
          Top = 208
          Width = 24
          Height = 13
          Caption = 'Lock'
        end
        object Image26: TImage
          Tag = 27
          Left = 444
          Top = 24
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image27: TImage
          Tag = 24
          Left = 444
          Top = 60
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image28: TImage
          Tag = 20
          Left = 444
          Top = 96
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image29: TImage
          Tag = 19
          Left = 444
          Top = 133
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image30: TImage
          Tag = 18
          Left = 444
          Top = 169
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image31: TImage
          Tag = 17
          Left = 444
          Top = 206
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image32: TImage
          Tag = 8
          Left = 444
          Top = 242
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label66: TLabel
          Left = 259
          Top = 244
          Width = 55
          Height = 13
          Caption = 'Port Enable'
        end
        object Label67: TLabel
          Left = 259
          Top = 281
          Width = 31
          Height = 13
          Caption = 'PC On'
        end
        object Image33: TImage
          Tag = 6
          Left = 444
          Top = 279
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image34: TImage
          Tag = 28
          Left = 444
          Top = 315
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label68: TLabel
          Left = 259
          Top = 317
          Width = 50
          Height = 13
          Caption = 'AC Control'
        end
        object Label69: TLabel
          Left = 259
          Top = 354
          Width = 38
          Height = 13
          Caption = 'VPP On'
        end
        object Image35: TImage
          Tag = 29
          Left = 444
          Top = 352
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image36: TImage
          Tag = 30
          Left = 444
          Top = 388
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label70: TLabel
          Left = 259
          Top = 390
          Width = 87
          Height = 13
          Caption = 'Power Supply 24V'
        end
        object Label71: TLabel
          Left = 259
          Top = 427
          Width = 96
          Height = 13
          Caption = 'Power Supply Spare'
        end
        object Image37: TImage
          Tag = 31
          Left = 444
          Top = 425
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image38: TImage
          Tag = 34
          Left = 444
          Top = 461
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label72: TLabel
          Left = 259
          Top = 463
          Width = 88
          Height = 13
          Caption = 'Support Left Pump'
        end
        object Label73: TLabel
          Left = 259
          Top = 500
          Width = 95
          Height = 13
          Caption = 'Support Right Pump'
        end
        object Image39: TImage
          Tag = 35
          Left = 444
          Top = 498
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image40: TImage
          Tag = 1
          Left = 444
          Top = 534
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label74: TLabel
          Left = 259
          Top = 536
          Width = 57
          Height = 13
          Caption = 'Out Spare 3'
        end
        object Label75: TLabel
          Left = 258
          Top = 573
          Width = 57
          Height = 13
          Caption = 'Out Spare 4'
        end
        object Image41: TImage
          Left = 444
          Top = 571
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Button1: TButton
          Tag = 26
          Left = 119
          Top = 23
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnClick = ActuatorOnClick
        end
        object Button2: TButton
          Tag = 26
          Left = 147
          Top = 23
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          OnClick = ActuatorOffClick
        end
        object Button3: TButton
          Tag = 25
          Left = 118
          Top = 59
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 4
          OnClick = ActuatorOnClick
        end
        object Button4: TButton
          Tag = 25
          Left = 146
          Top = 59
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 5
          OnClick = ActuatorOffClick
        end
        object Button5: TButton
          Tag = 32
          Left = 119
          Top = 95
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 8
          OnClick = ActuatorOnClick
        end
        object Button6: TButton
          Tag = 32
          Left = 147
          Top = 95
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 9
          OnClick = ActuatorOffClick
        end
        object Button7: TButton
          Tag = 33
          Left = 120
          Top = 132
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 12
          OnClick = ActuatorOnClick
        end
        object Button8: TButton
          Tag = 33
          Left = 148
          Top = 132
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 13
          OnClick = ActuatorOffClick
        end
        object Button9: TButton
          Tag = 40
          Left = 120
          Top = 168
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 16
          OnClick = ActuatorOnClick
        end
        object Button10: TButton
          Tag = 40
          Left = 148
          Top = 168
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 17
          OnClick = ActuatorOffClick
        end
        object Button11: TButton
          Tag = 41
          Left = 120
          Top = 205
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 20
          OnClick = ActuatorOnClick
        end
        object Button12: TButton
          Tag = 41
          Left = 148
          Top = 205
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 21
          OnClick = ActuatorOffClick
        end
        object Button13: TButton
          Tag = 42
          Left = 120
          Top = 241
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 24
          OnClick = ActuatorOnClick
        end
        object Button14: TButton
          Tag = 42
          Left = 148
          Top = 241
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 25
          OnClick = ActuatorOffClick
        end
        object Button15: TButton
          Tag = 43
          Left = 120
          Top = 278
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 28
          OnClick = ActuatorOnClick
        end
        object Button16: TButton
          Tag = 43
          Left = 148
          Top = 278
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 29
          OnClick = ActuatorOffClick
        end
        object Button17: TButton
          Tag = 15
          Left = 120
          Top = 314
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 32
          OnClick = ActuatorOnClick
        end
        object Button18: TButton
          Tag = 15
          Left = 148
          Top = 314
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 33
          OnClick = ActuatorOffClick
        end
        object Button19: TButton
          Tag = 5
          Left = 122
          Top = 563
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 60
          OnClick = ActuatorOnClick
        end
        object Button20: TButton
          Tag = 5
          Left = 150
          Top = 563
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 61
          OnClick = ActuatorOffClick
        end
        object Button21: TButton
          Tag = 4
          Left = 122
          Top = 383
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 40
          OnClick = ActuatorOnClick
        end
        object Button22: TButton
          Tag = 4
          Left = 150
          Top = 383
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 41
          OnClick = ActuatorOffClick
        end
        object Button23: TButton
          Tag = 7
          Left = 122
          Top = 420
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 44
          OnClick = ActuatorOnClick
        end
        object Button24: TButton
          Tag = 7
          Left = 150
          Top = 420
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 45
          OnClick = ActuatorOffClick
        end
        object Button25: TButton
          Tag = 21
          Left = 122
          Top = 456
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 48
          OnClick = ActuatorOnClick
        end
        object Button26: TButton
          Tag = 21
          Left = 150
          Top = 456
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 49
          OnClick = ActuatorOffClick
        end
        object Button27: TButton
          Tag = 22
          Left = 122
          Top = 493
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 52
          OnClick = ActuatorOnClick
        end
        object Button28: TButton
          Tag = 22
          Left = 150
          Top = 493
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 53
          OnClick = ActuatorOffClick
        end
        object Button29: TButton
          Tag = 23
          Left = 122
          Top = 529
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 56
          OnClick = ActuatorOnClick
        end
        object Button30: TButton
          Tag = 23
          Left = 150
          Top = 529
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 57
          OnClick = ActuatorOffClick
        end
        object Button31: TButton
          Tag = 16
          Left = 121
          Top = 350
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 36
          OnClick = ActuatorOnClick
        end
        object Button32: TButton
          Tag = 16
          Left = 149
          Top = 350
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 37
          OnClick = ActuatorOffClick
        end
        object Button33: TButton
          Tag = 27
          Left = 367
          Top = 23
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          OnClick = ActuatorOnClick
        end
        object Button34: TButton
          Tag = 27
          Left = 395
          Top = 23
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 3
          OnClick = ActuatorOffClick
        end
        object Button35: TButton
          Tag = 24
          Left = 366
          Top = 59
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 6
          OnClick = ActuatorOnClick
        end
        object Button36: TButton
          Tag = 24
          Left = 394
          Top = 59
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 7
          OnClick = ActuatorOffClick
        end
        object Button37: TButton
          Tag = 20
          Left = 395
          Top = 95
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 11
          OnClick = ActuatorOffClick
        end
        object Button38: TButton
          Tag = 20
          Left = 367
          Top = 95
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 10
          OnClick = ActuatorOnClick
        end
        object Button39: TButton
          Tag = 19
          Left = 368
          Top = 132
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 14
          OnClick = ActuatorOnClick
        end
        object Button40: TButton
          Tag = 19
          Left = 396
          Top = 132
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 15
          OnClick = ActuatorOffClick
        end
        object Button41: TButton
          Tag = 18
          Left = 396
          Top = 168
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 19
          OnClick = ActuatorOffClick
        end
        object Button42: TButton
          Tag = 18
          Left = 368
          Top = 168
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 18
          OnClick = ActuatorOnClick
        end
        object Button43: TButton
          Tag = 17
          Left = 368
          Top = 205
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 22
          OnClick = ActuatorOnClick
        end
        object Button44: TButton
          Tag = 17
          Left = 396
          Top = 205
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 23
          OnClick = ActuatorOffClick
        end
        object Button45: TButton
          Tag = 8
          Left = 396
          Top = 241
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 27
          OnClick = ActuatorOnClick
        end
        object Button46: TButton
          Tag = 8
          Left = 368
          Top = 241
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 26
          OnClick = ActuatorOffClick
        end
        object Button47: TButton
          Tag = 6
          Left = 368
          Top = 278
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 30
          OnClick = ActuatorOnClick
        end
        object Button48: TButton
          Tag = 6
          Left = 396
          Top = 278
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 31
          OnClick = ActuatorOffClick
        end
        object Button49: TButton
          Tag = 28
          Left = 396
          Top = 314
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 35
          OnClick = ActuatorOffClick
        end
        object Button50: TButton
          Tag = 28
          Left = 368
          Top = 314
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 34
          OnClick = ActuatorOnClick
        end
        object Button51: TButton
          Tag = 29
          Left = 368
          Top = 351
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 38
          OnClick = ActuatorOnClick
        end
        object Button52: TButton
          Tag = 29
          Left = 396
          Top = 351
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 39
          OnClick = ActuatorOffClick
        end
        object Button53: TButton
          Tag = 30
          Left = 396
          Top = 387
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 43
          OnClick = ActuatorOffClick
        end
        object Button54: TButton
          Tag = 30
          Left = 368
          Top = 387
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 42
          OnClick = ActuatorOnClick
        end
        object Button55: TButton
          Tag = 31
          Left = 368
          Top = 424
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 46
          OnClick = ActuatorOnClick
        end
        object Button56: TButton
          Tag = 31
          Left = 396
          Top = 424
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 47
          OnClick = ActuatorOffClick
        end
        object Button57: TButton
          Tag = 34
          Left = 396
          Top = 460
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 51
          OnClick = ActuatorOffClick
        end
        object Button58: TButton
          Tag = 34
          Left = 368
          Top = 460
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 50
          OnClick = ActuatorOnClick
        end
        object Button59: TButton
          Tag = 35
          Left = 368
          Top = 497
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 54
          OnClick = ActuatorOnClick
        end
        object Button60: TButton
          Tag = 35
          Left = 396
          Top = 497
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 55
          OnClick = ActuatorOffClick
        end
        object Button61: TButton
          Tag = 1
          Left = 396
          Top = 533
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 59
          OnClick = ActuatorOffClick
        end
        object Button62: TButton
          Tag = 1
          Left = 368
          Top = 533
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 58
          OnClick = ActuatorOnClick
        end
        object Button63: TButton
          Left = 367
          Top = 570
          Width = 25
          Height = 18
          Caption = 'ON'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 62
          OnClick = ActuatorOnClick
        end
        object Button64: TButton
          Left = 395
          Top = 570
          Width = 25
          Height = 18
          Caption = 'OFF'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 63
          OnClick = ActuatorOffClick
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Inputs'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object SensorsGroupBox: TGroupBox
        Left = 8
        Top = 3
        Width = 481
        Height = 598
        Caption = 'Sensors'
        TabOrder = 0
        object Label45: TLabel
          Left = 11
          Top = 107
          Width = 77
          Height = 13
          BiDiMode = bdLeftToRight
          Caption = 'M1 Left Inserted'
          ParentBiDiMode = False
        end
        object Label46: TLabel
          Left = 11
          Top = 148
          Width = 77
          Height = 13
          Caption = 'M2 Left Inserted'
        end
        object Label47: TLabel
          Left = 11
          Top = 189
          Width = 77
          Height = 13
          Caption = 'M3 Left Inserted'
        end
        object Label48: TLabel
          Left = 11
          Top = 229
          Width = 87
          Height = 13
          Caption = 'Support 1 Inserted'
        end
        object Label49: TLabel
          Left = 11
          Top = 270
          Width = 87
          Height = 13
          Caption = 'Support 2 Inserted'
        end
        object Image2: TImage
          Tag = 11
          Left = 137
          Top = 24
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label51: TLabel
          Left = 11
          Top = 26
          Width = 72
          Height = 13
          Caption = 'Right UV Lamp'
        end
        object Label52: TLabel
          Left = 11
          Top = 66
          Width = 65
          Height = 13
          Caption = 'Left UV Lamp'
        end
        object Image17: TImage
          Tag = 9
          Left = 137
          Top = 64
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image18: TImage
          Tag = 16
          Left = 137
          Top = 105
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image19: TImage
          Tag = 20
          Left = 137
          Top = 146
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image20: TImage
          Tag = 22
          Left = 137
          Top = 187
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image21: TImage
          Tag = 18
          Left = 137
          Top = 227
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image22: TImage
          Tag = 19
          Left = 137
          Top = 268
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label76: TLabel
          Left = 11
          Top = 474
          Width = 41
          Height = 13
          Caption = 'Interlock'
        end
        object Image42: TImage
          Tag = 10
          Left = 137
          Top = 472
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label77: TLabel
          Left = 11
          Top = 515
          Width = 49
          Height = 13
          Caption = 'In Spare 1'
        end
        object Image43: TImage
          Left = 137
          Top = 513
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label78: TLabel
          Left = 11
          Top = 556
          Width = 49
          Height = 13
          Caption = 'In Spare 2'
        end
        object Image44: TImage
          Tag = 1
          Left = 137
          Top = 554
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image24: TImage
          Tag = 8
          Left = 137
          Top = 350
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label53: TLabel
          Left = 11
          Top = 352
          Width = 62
          Height = 13
          Caption = 'Tray in Place'
        end
        object Label54: TLabel
          Left = 11
          Top = 392
          Width = 79
          Height = 13
          Caption = 'Model On Tray 1'
        end
        object Image45: TImage
          Tag = 12
          Left = 137
          Top = 390
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Image46: TImage
          Tag = 13
          Left = 137
          Top = 431
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label55: TLabel
          Left = 11
          Top = 433
          Width = 79
          Height = 13
          Caption = 'Model On Tray 2'
        end
        object Label129: TLabel
          Left = 218
          Top = 107
          Width = 84
          Height = 13
          BiDiMode = bdLeftToRight
          Caption = 'M1 Right Inserted'
          ParentBiDiMode = False
        end
        object Image47: TImage
          Tag = 17
          Left = 324
          Top = 104
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label130: TLabel
          Left = 218
          Top = 146
          Width = 84
          Height = 13
          Caption = 'M2 Right Inserted'
        end
        object Image48: TImage
          Tag = 21
          Left = 324
          Top = 146
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
        object Label131: TLabel
          Left = 218
          Top = 189
          Width = 84
          Height = 13
          Caption = 'M3 Right Inserted'
        end
        object Image49: TImage
          Tag = 23
          Left = 325
          Top = 187
          Width = 16
          Height = 16
          AutoSize = True
          Center = True
          Transparent = True
        end
      end
    end
    object HeadsTab: TTabSheet
      Caption = 'Heads'
      ImageIndex = 4
      OnShow = HeadsTabExecute
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox2: TGroupBox
        Left = 8
        Top = 6
        Width = 481
        Height = 598
        TabOrder = 0
        object ModelHeadsGroupBox: TGroupBox
          Left = 12
          Top = 123
          Width = 457
          Height = 295
          Caption = 'Head Parameters'
          TabOrder = 1
          object Label142: TLabel
            Left = 63
            Top = 136
            Width = 86
            Height = 13
            Caption = 'M1 Level Low Th.'
          end
          object Label143: TLabel
            Left = 63
            Top = 92
            Width = 88
            Height = 13
            Caption = 'M1 Level High Th.'
          end
          object Label156: TLabel
            Left = 181
            Top = 187
            Width = 94
            Height = 13
            Caption = 'Time Pump ON (ms)'
          end
          object Label162: TLabel
            Left = 339
            Top = 187
            Width = 98
            Height = 13
            Caption = 'Time Pump OFF (ms)'
          end
          object Label169: TLabel
            Left = 35
            Top = 187
            Width = 67
            Height = 13
            Caption = 'Filling Timeout'
          end
          object Label95: TLabel
            Left = 357
            Top = 92
            Width = 80
            Height = 13
            Caption = 'S Level High Th.'
          end
          object Label94: TLabel
            Left = 358
            Top = 136
            Width = 78
            Height = 13
            Caption = 'S Level Low Th.'
          end
          object Label119: TLabel
            Left = 164
            Top = 136
            Width = 86
            Height = 13
            Caption = 'M2 Level Low Th.'
          end
          object Label120: TLabel
            Left = 157
            Top = 92
            Width = 88
            Height = 13
            Caption = 'M2 Level High Th.'
          end
          object Label121: TLabel
            Left = 16
            Top = 164
            Width = 41
            Height = 13
            Caption = 'Common'
          end
          object Label113: TLabel
            Left = 259
            Top = 92
            Width = 88
            Height = 13
            Caption = 'M3 Level High Th.'
          end
          object Label122: TLabel
            Left = 259
            Top = 136
            Width = 86
            Height = 13
            Caption = 'M3 Level Low Th.'
          end
          object Label123: TLabel
            Left = 165
            Top = 11
            Width = 82
            Height = 13
            Caption = 'M Level High Th.'
          end
          object Label124: TLabel
            Left = 262
            Top = 11
            Width = 80
            Height = 13
            Caption = 'S Level High Th.'
          end
          object Label125: TLabel
            Left = 169
            Top = 49
            Width = 80
            Height = 13
            Caption = 'M Level Low Th.'
          end
          object Label126: TLabel
            Left = 264
            Top = 49
            Width = 78
            Height = 13
            Caption = 'S Level Low Th.'
          end
          object TimeoutFillingEdit: TEdit
            Left = 16
            Top = 206
            Width = 105
            Height = 21
            TabOrder = 15
          end
          object Model1LevelThermistorHighEdit: TEdit
            Left = 78
            Top = 111
            Width = 65
            Height = 21
            TabOrder = 5
          end
          object Model1LevelThermistorLowEdit: TEdit
            Left = 78
            Top = 152
            Width = 65
            Height = 21
            TabOrder = 9
          end
          object PumpOnEdit: TEdit
            Left = 176
            Top = 206
            Width = 105
            Height = 21
            TabOrder = 16
          end
          object SetHeadsParametersButton: TButton
            Left = 16
            Top = 32
            Width = 103
            Height = 25
            Caption = 'Set Parameters'
            TabOrder = 2
            OnClick = SetHeadsParametersButtonClick
          end
          object PumpOffEdit: TEdit
            Left = 336
            Top = 204
            Width = 105
            Height = 21
            TabOrder = 14
          end
          object SupportLevelThermistorLowEdit: TEdit
            Left = 362
            Top = 152
            Width = 65
            Height = 21
            TabOrder = 12
          end
          object SupportLevelThermistorHighEdit: TEdit
            Left = 361
            Top = 111
            Width = 65
            Height = 21
            TabOrder = 8
          end
          object Model2LevelThermistorLowEdit: TEdit
            Left = 174
            Top = 152
            Width = 65
            Height = 21
            TabOrder = 10
          end
          object Model2LevelThermistorHighEdit: TEdit
            Left = 174
            Top = 111
            Width = 65
            Height = 21
            TabOrder = 6
          end
          object Panel10: TPanel
            Left = 16
            Top = 179
            Width = 425
            Height = 3
            TabOrder = 13
          end
          object Model3LevelThermistorHighEdit: TEdit
            Left = 269
            Top = 111
            Width = 65
            Height = 21
            TabOrder = 7
          end
          object Model3LevelThermistorLowEdit: TEdit
            Left = 269
            Top = 152
            Width = 65
            Height = 21
            TabOrder = 11
          end
          object ModelLevelThermistorHighEdit: TEdit
            Left = 174
            Top = 26
            Width = 65
            Height = 21
            TabOrder = 0
          end
          object SLevelThermistorHighEdit: TEdit
            Left = 269
            Top = 26
            Width = 65
            Height = 21
            TabOrder = 1
          end
          object ModelLevelThermistorLowEdit: TEdit
            Left = 174
            Top = 65
            Width = 65
            Height = 21
            TabOrder = 3
          end
          object SLevelThermistorLowEdit: TEdit
            Left = 269
            Top = 65
            Width = 65
            Height = 21
            TabOrder = 4
          end
          object MHighThCheckBox: TCheckBox
            Left = 252
            Top = 238
            Width = 97
            Height = 17
            Caption = 'M High Th.'
            TabOrder = 18
          end
          object SHighThCheckBox: TCheckBox
            Left = 163
            Top = 238
            Width = 83
            Height = 17
            Caption = 'S High Th.'
            TabOrder = 17
          end
          object SLowThCheckBox: TCheckBox
            Left = 343
            Top = 266
            Width = 68
            Height = 17
            Caption = 'S low Th.'
            TabOrder = 22
          end
          object M1LowThCheckBox: TCheckBox
            Left = 252
            Top = 266
            Width = 78
            Height = 17
            Caption = 'M1 Low Th.'
            TabOrder = 21
          end
          object M2LowThCheckBox: TCheckBox
            Left = 164
            Top = 266
            Width = 76
            Height = 17
            Caption = 'M2 Low Th.'
            TabOrder = 20
          end
          object M3LowThCheckBox: TCheckBox
            Left = 79
            Top = 266
            Width = 75
            Height = 17
            Caption = 'M3 Low Th.'
            TabOrder = 19
          end
        end
        object GeneralHeadsGroupBox: TGroupBox
          Left = 12
          Top = 10
          Width = 457
          Height = 111
          Caption = 'General'
          TabOrder = 0
          object Label90: TLabel
            Left = 140
            Top = 63
            Width = 55
            Height = 13
            Caption = 'S Level Th.'
          end
          object Label93: TLabel
            Left = 373
            Top = 63
            Width = 63
            Height = 13
            Caption = 'M1 Level Th.'
          end
          object Label89: TLabel
            Left = 107
            Top = 16
            Width = 35
            Height = 13
            Caption = 'Monitor'
          end
          object FillingImage: TImage
            Tag = 11
            Left = 116
            Top = 32
            Width = 16
            Height = 16
            AutoSize = True
            Center = True
            Transparent = True
          end
          object Label118: TLabel
            Left = 295
            Top = 63
            Width = 66
            Height = 13
            Caption = 'M2  Level Th.'
          end
          object Label107: TLabel
            Left = 215
            Top = 63
            Width = 63
            Height = 13
            Caption = 'M3 Level Th.'
          end
          object Label127: TLabel
            Left = 218
            Top = 16
            Width = 55
            Height = 13
            Caption = 'S Level Th.'
          end
          object Label128: TLabel
            Left = 295
            Top = 16
            Width = 57
            Height = 13
            Caption = 'M Level Th.'
          end
          object HeadsFillingButton: TButton
            Left = 22
            Top = 27
            Width = 75
            Height = 25
            Action = HeadsFillingOn
            TabOrder = 0
          end
          object Model1ThermistorLevelPanel: TPanel
            Left = 370
            Top = 80
            Width = 69
            Height = 21
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 6
          end
          object SupportThermistorLevelPanel: TPanel
            Left = 133
            Top = 80
            Width = 69
            Height = 21
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 3
          end
          object Model2ThermistorLevelPanel: TPanel
            Left = 291
            Top = 80
            Width = 69
            Height = 21
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 5
          end
          object Model3ThermistorLevelPanel: TPanel
            Left = 212
            Top = 80
            Width = 69
            Height = 21
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 4
          end
          object ModelThermistorLevelPanel: TPanel
            Left = 291
            Top = 35
            Width = 69
            Height = 21
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 2
          end
          object SThermistorLevelPanel: TPanel
            Left = 212
            Top = 35
            Width = 69
            Height = 21
            BevelOuter = bvLowered
            Caption = '???'
            TabOrder = 1
          end
        end
        object GroupBox7: TGroupBox
          Left = 12
          Top = 421
          Width = 457
          Height = 170
          Caption = 'Set Chamber Tank'
          Ctl3D = True
          ParentCtl3D = False
          TabOrder = 2
          object SetActivePumpsButton: TButton
            Left = 11
            Top = 36
            Width = 75
            Height = 25
            Caption = 'Set'
            TabOrder = 1
            OnClick = SetActivePumpsButtonClick
          end
          object Model1ActivePumpsGroupBox: TGroupBox
            Left = 112
            Top = 88
            Width = 225
            Height = 37
            Caption = 'Model_1'
            TabOrder = 3
            object Moedl1LeftPumpRadioButton: TRadioButton
              Tag = 2
              Left = 16
              Top = 16
              Width = 89
              Height = 17
              Caption = 'Left Container'
              TabOrder = 0
              OnClick = Model1PumpSelectExecte
            end
            object Moedl1RightPumpRadioButton: TRadioButton
              Tag = 3
              Left = 113
              Top = 16
              Width = 96
              Height = 17
              Caption = 'Right Container'
              TabOrder = 1
              OnClick = Model1PumpSelectExecte
            end
          end
          object SupportActivePumpsGroupBox: TGroupBox
            Left = 112
            Top = 127
            Width = 225
            Height = 37
            Caption = 'Support'
            TabOrder = 4
            object SupportPump1RadioButton: TRadioButton
              Left = 16
              Top = 17
              Width = 89
              Height = 17
              Caption = 'Left Container'
              TabOrder = 1
              OnClick = SupportPumpSelectExecute
            end
            object SupportPump2RadioButton: TRadioButton
              Tag = 1
              Left = 113
              Top = 16
              Width = 96
              Height = 17
              Caption = 'Right Container'
              TabOrder = 0
              OnClick = SupportPumpSelectExecute
            end
          end
          object Model2ActivePumpsGroupBox: TGroupBox
            Left = 112
            Top = 48
            Width = 225
            Height = 37
            Caption = 'Model_2'
            TabOrder = 2
            object Moedl2LeftPumpRadioButton: TRadioButton
              Tag = 4
              Left = 16
              Top = 16
              Width = 97
              Height = 17
              Caption = 'Left Container'
              TabOrder = 0
              OnClick = Model2PumpSelectExecte
            end
            object Moedl2RightPumpRadioButton: TRadioButton
              Tag = 5
              Left = 112
              Top = 16
              Width = 105
              Height = 17
              Caption = 'Right Container'
              TabOrder = 1
              OnClick = Model2PumpSelectExecte
            end
          end
          object Model3ActivePumpsGroupBox: TGroupBox
            Left = 112
            Top = 9
            Width = 225
            Height = 37
            Caption = 'Model_3'
            TabOrder = 0
            object Moedl3LeftPumpRadioButton: TRadioButton
              Tag = 6
              Left = 16
              Top = 16
              Width = 89
              Height = 17
              Caption = 'Left Container'
              TabOrder = 0
              OnClick = Model3PumpSelectExecte
            end
            object Moedl3RightPumpRadioButton: TRadioButton
              Tag = 7
              Left = 113
              Top = 16
              Width = 96
              Height = 17
              Caption = 'Right Container'
              TabOrder = 1
              OnClick = Model3PumpSelectExecte
            end
          end
        end
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Liquid Tanks'
      ImageIndex = 5
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox4: TGroupBox
        Left = 8
        Top = 3
        Width = 481
        Height = 598
        Padding.Left = 5
        Padding.Top = 5
        Padding.Right = 5
        Padding.Bottom = 5
        TabOrder = 0
        object TanksWeightGroupBox: TGroupBox
          Left = 238
          Top = 20
          Width = 236
          Height = 571
          Align = alClient
          Caption = 'Liquid Tanks Weights'
          Padding.Left = 5
          Padding.Top = 5
          Padding.Right = 5
          Padding.Bottom = 5
          TabOrder = 0
          ExplicitLeft = 241
          ExplicitTop = 19
          object LabeledEdit10: TLabeledEdit
            Left = 24
            Top = 34
            Width = 73
            Height = 21
            EditLabel.Width = 58
            EditLabel.Height = 13
            EditLabel.Caption = 'Support Left'
            ReadOnly = True
            TabOrder = 0
            Text = '???'
          end
          object LabeledEdit18: TLabeledEdit
            Tag = 1
            Left = 136
            Top = 34
            Width = 73
            Height = 21
            EditLabel.Width = 65
            EditLabel.Height = 13
            EditLabel.Caption = 'Support Right'
            ReadOnly = True
            TabOrder = 1
            Text = '???'
          end
          object LabeledEdit19: TLabeledEdit
            Tag = 2
            Left = 24
            Top = 87
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M1 L'
            ReadOnly = True
            TabOrder = 2
            Text = '???'
          end
          object LabeledEdit20: TLabeledEdit
            Tag = 3
            Left = 136
            Top = 87
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M1 R'
            ReadOnly = True
            TabOrder = 3
            Text = '???'
          end
          object LabeledEdit21: TLabeledEdit
            Tag = 4
            Left = 24
            Top = 139
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M2 L'
            ReadOnly = True
            TabOrder = 4
            Text = '???'
          end
          object LabeledEdit22: TLabeledEdit
            Tag = 5
            Left = 136
            Top = 139
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M2 R'
            ReadOnly = True
            TabOrder = 5
            Text = '???'
          end
          object LabeledEdit23: TLabeledEdit
            Tag = 6
            Left = 24
            Top = 192
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M3 L'
            ReadOnly = True
            TabOrder = 6
            Text = '???'
          end
          object LabeledEdit24: TLabeledEdit
            Tag = 7
            Left = 136
            Top = 192
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M3 R'
            ReadOnly = True
            TabOrder = 7
            Text = '???'
          end
          object LabeledEdit25: TLabeledEdit
            Tag = 8
            Left = 24
            Top = 244
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M4 L'
            ReadOnly = True
            TabOrder = 8
            Text = '???'
          end
          object LabeledEdit26: TLabeledEdit
            Tag = 9
            Left = 136
            Top = 244
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M4 R'
            ReadOnly = True
            TabOrder = 9
            Text = '???'
          end
          object LabeledEdit27: TLabeledEdit
            Tag = 10
            Left = 24
            Top = 297
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M5 L'
            ReadOnly = True
            TabOrder = 10
            Text = '???'
          end
          object LabeledEdit28: TLabeledEdit
            Tag = 11
            Left = 136
            Top = 297
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M5 R'
            ReadOnly = True
            TabOrder = 11
            Text = '???'
          end
          object LabeledEdit29: TLabeledEdit
            Tag = 12
            Left = 24
            Top = 349
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M6 L'
            ReadOnly = True
            TabOrder = 12
            Text = '???'
          end
          object LabeledEdit30: TLabeledEdit
            Tag = 13
            Left = 136
            Top = 349
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M6 R'
            ReadOnly = True
            TabOrder = 13
            Text = '???'
          end
          object LabeledEdit31: TLabeledEdit
            Tag = 14
            Left = 24
            Top = 402
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M7 L'
            ReadOnly = True
            TabOrder = 14
            Text = '???'
          end
          object LabeledEdit32: TLabeledEdit
            Tag = 15
            Left = 136
            Top = 402
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M7 R'
            ReadOnly = True
            TabOrder = 15
            Text = '???'
          end
          object LabeledEdit33: TLabeledEdit
            Tag = 16
            Left = 24
            Top = 455
            Width = 73
            Height = 21
            EditLabel.Width = 52
            EditLabel.Height = 13
            EditLabel.Caption = 'Waste Left'
            ReadOnly = True
            TabOrder = 16
            Text = '???'
          end
          object LabeledEdit34: TLabeledEdit
            Tag = 17
            Left = 136
            Top = 455
            Width = 73
            Height = 21
            EditLabel.Width = 59
            EditLabel.Height = 13
            EditLabel.Caption = 'Waste Right'
            ReadOnly = True
            TabOrder = 17
            Text = '???'
          end
        end
        object TanksExistenceGroupBox: TGroupBox
          AlignWithMargins = True
          Left = 7
          Top = 20
          Width = 226
          Height = 571
          Margins.Left = 0
          Margins.Top = 0
          Margins.Right = 5
          Margins.Bottom = 0
          Align = alLeft
          Caption = 'Liquid Tanks Existence'
          Padding.Left = 5
          Padding.Top = 5
          Padding.Right = 5
          Padding.Bottom = 5
          TabOrder = 1
          object LabeledEdit1: TLabeledEdit
            Left = 16
            Top = 34
            Width = 73
            Height = 21
            EditLabel.Width = 58
            EditLabel.Height = 13
            EditLabel.Caption = 'Support Left'
            ReadOnly = True
            TabOrder = 0
            Text = 'In / Out'
          end
          object LabeledEdit2: TLabeledEdit
            Tag = 1
            Left = 128
            Top = 34
            Width = 73
            Height = 21
            EditLabel.Width = 65
            EditLabel.Height = 13
            EditLabel.Caption = 'Support Right'
            ReadOnly = True
            TabOrder = 1
            Text = 'In / Out'
          end
          object LabeledEdit3: TLabeledEdit
            Tag = 10
            Left = 16
            Top = 297
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M5 L'
            ReadOnly = True
            TabOrder = 2
            Text = 'In / Out'
          end
          object LabeledEdit4: TLabeledEdit
            Tag = 8
            Left = 16
            Top = 244
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M4 L'
            ReadOnly = True
            TabOrder = 3
            Text = 'In / Out'
          end
          object LabeledEdit5: TLabeledEdit
            Tag = 6
            Left = 16
            Top = 192
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M3 L'
            ReadOnly = True
            TabOrder = 4
            Text = 'In / Out'
          end
          object LabeledEdit6: TLabeledEdit
            Tag = 4
            Left = 16
            Top = 139
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M2 L'
            ReadOnly = True
            TabOrder = 5
            Text = 'In / Out'
          end
          object LabeledEdit7: TLabeledEdit
            Tag = 2
            Left = 16
            Top = 87
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M1 L'
            ReadOnly = True
            TabOrder = 6
            Text = 'In / Out'
          end
          object LabeledEdit8: TLabeledEdit
            Tag = 14
            Left = 16
            Top = 402
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M7 L'
            ReadOnly = True
            TabOrder = 7
            Text = 'In / Out'
          end
          object LabeledEdit9: TLabeledEdit
            Tag = 12
            Left = 16
            Top = 349
            Width = 73
            Height = 21
            EditLabel.Width = 24
            EditLabel.Height = 13
            EditLabel.Caption = 'M6 L'
            ReadOnly = True
            TabOrder = 8
            Text = 'In / Out'
          end
          object LabeledEdit11: TLabeledEdit
            Tag = 3
            Left = 128
            Top = 87
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M1 R'
            ReadOnly = True
            TabOrder = 9
            Text = 'In / Out'
          end
          object LabeledEdit12: TLabeledEdit
            Tag = 5
            Left = 128
            Top = 139
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M2 R'
            ReadOnly = True
            TabOrder = 10
            Text = 'In / Out'
          end
          object LabeledEdit13: TLabeledEdit
            Tag = 7
            Left = 128
            Top = 192
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M3 R'
            ReadOnly = True
            TabOrder = 11
            Text = 'In / Out'
          end
          object LabeledEdit14: TLabeledEdit
            Tag = 9
            Left = 128
            Top = 244
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M4 R'
            ReadOnly = True
            TabOrder = 12
            Text = 'In / Out'
          end
          object LabeledEdit15: TLabeledEdit
            Tag = 11
            Left = 128
            Top = 297
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M5 R'
            ReadOnly = True
            TabOrder = 13
            Text = 'In / Out'
          end
          object LabeledEdit16: TLabeledEdit
            Tag = 13
            Left = 128
            Top = 349
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M6 R'
            ReadOnly = True
            TabOrder = 14
            Text = 'In / Out'
          end
          object LabeledEdit17: TLabeledEdit
            Tag = 15
            Left = 128
            Top = 402
            Width = 73
            Height = 21
            EditLabel.Width = 26
            EditLabel.Height = 13
            EditLabel.Caption = 'M7 R'
            ReadOnly = True
            TabOrder = 15
            Text = 'In / Out'
          end
        end
      end
    end
    object TabSheet6: TTabSheet
      Caption = 'Odour Fan'
      ImageIndex = 6
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox8: TGroupBox
        Left = 8
        Top = 3
        Width = 481
        Height = 70
        TabOrder = 0
        object Label92: TLabel
          Left = 143
          Top = 13
          Width = 27
          Height = 13
          Caption = 'Value'
        end
        object OdourOnOffButton: TButton
          Left = 17
          Top = 26
          Width = 75
          Height = 25
          Action = OdourOn
          TabOrder = 0
        end
        object OdourValueEdit: TEdit
          Left = 120
          Top = 28
          Width = 73
          Height = 21
          TabOrder = 1
        end
      end
    end
    object TabSheet7: TTabSheet
      Caption = 'D/A'
      ImageIndex = 7
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox9: TGroupBox
        Left = 8
        Top = 3
        Width = 481
        Height = 70
        Caption = 'D/A Device 1'
        TabOrder = 0
        object Label96: TLabel
          Left = 143
          Top = 13
          Width = 27
          Height = 13
          Caption = 'Value'
        end
        object SetD2AValueDevice1Button: TButton
          Left = 17
          Top = 26
          Width = 75
          Height = 25
          Caption = 'Set'
          TabOrder = 0
          OnClick = SetD2AValueDevice1ButtonClick
        end
        object SetD2AValueDevice1Edit: TEdit
          Left = 120
          Top = 28
          Width = 73
          Height = 21
          TabOrder = 1
          Text = '0'
        end
      end
      object GroupBox10: TGroupBox
        Left = 8
        Top = 81
        Width = 481
        Height = 70
        Caption = 'D/A Device 2'
        TabOrder = 1
        object Label97: TLabel
          Left = 143
          Top = 13
          Width = 27
          Height = 13
          Caption = 'Value'
        end
        object SetD2AValueDevice2Button: TButton
          Left = 17
          Top = 26
          Width = 75
          Height = 25
          Caption = 'Set'
          TabOrder = 0
          OnClick = SetD2AValueDevice2ButtonClick
        end
        object SetD2AValueDevice2Edit: TEdit
          Left = 120
          Top = 28
          Width = 73
          Height = 21
          TabOrder = 1
          Text = '0'
        end
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 643
    Width = 522
    Height = 20
    Panels = <
      item
        Style = psOwnerDraw
        Width = 50
      end>
    OnDrawPanel = StatusBar1DrawPanel
  end
  object LedTimer: TTimer
    OnTimer = LedTimerTimer
    Left = 488
  end
  object LEDsImageList: TImageList
    Left = 456
    Bitmap = {
      494C010104000600040010001000FFFFFFFFFF00FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000002000000001002000000000000020
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000012C0600012C0600012C0600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000001580B0001580B0001580B00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000D0C49000D0C49000D0C4900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000191792001917920019179200000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000115030001390700014C0A0001550B0001570B0001570B00014B0A000137
      0700011704000000000000000000000000000000000000000000000000000000
      0000012A060001720E000197130001AA150001AE160001AD150001961300016D
      0D00012D07000000000000000000000000000000000000000000000000000000
      00000606220010105D001414780018178000191980001918800016147600100E
      5A00080822000000000000000000000000000000000000000000000000000000
      00000B0B44001F1FBA002828EF00302DFF003232FF003230FF002C28EC001F1C
      B400101044000000000000000000000000000000000000000000000000000130
      070001510A00015C0C00076312000E64180011651B000C631600076312000154
      0B00014E0A00013207000000000000000000000000000000000000000000015F
      0D0001A1140001B817000EC624001CC7300021C9350017C62C000DC5230001A8
      1500019C140001630D0000000000000000000000000000000000000000000D0D
      500014167C001E1E8000272980002E2E8000313080002D2C8000272980001A16
      80001415790010104E0000000000000000000000000000000000000000001919
      A000282CF8003B3BFF004D52FF005B5BFF00615FFF005957FF004D51FF00332C
      FF00282AF2001F1F9C000000000000000000000000000000000000000000014D
      0A0001570B0001610D000B631600086313000A6314000A631500066211000160
      0C0001570B000148090000000000000000000000000000000000000000000199
      130001AE160002C2190016C62B0010C5250013C6280014C529000CC3220001C0
      180001AD15000190120000000000000000000000000000000000000000001514
      78001A198000232380002B2B80002B298000292B80002B2B8000272880002123
      80001A1880001214720000000000000000000000000000000000000000002928
      F0003332FF004546FF005656FF005551FF005256FF005655FF004E50FF004145
      FF003330FF002328E4000000000000000000000000000000000001350700014F
      0A0001540B0001540A0001540B0001520A0001520A0001530A0001550B000155
      0B0001530B00014D0A0001360700000000000000000000000000016A0E00019D
      130001A7150001A6140001A8150001A3140001A4140001A5140001A9150001A9
      150001A5150001991300016B0D000000000000000000000000000E0E58001515
      790016177E001815810018177F0014157E0018157F0015167E0016187F001618
      7F0018167E0015147800101056000000000000000000000000001C1CAF002929
      F2002C2DFC00302AFF00302DFD00282AFC00302AFD00292CFC002C30FD002C30
      FD00302CFC002928F000201FAB00000000000000000000000000014008000147
      090001490900014A090001500A0001530B0001540B0001530B00014F0A00014B
      0900014B090001480900013F0800000000000000000000000000017F1000018D
      1200019112000193120001A0140001A6150001A8150001A51500019D13000195
      12000195120001901200017D0F000000000000000000000000000F1167001312
      7200111374001214750015157C0019167F0015177F0018167E00151579001414
      75001514750011147100121264000000000000000000000000001D22CD002623
      E4002226E8002328E9002A29F700322CFD002A2DFE00302CFC002929F2002828
      E9002A28E9002228E2002323C800000000000000000000000000013D08000142
      09000142090001540B000B64160015661F001866210014651D0006621100014F
      0A000144090001400800013D080000000000000000000000000001790F000183
      11000183110001A8150016C72B002ACB3D002FCC410027CA3A000CC32200019D
      130001871100017F100001790F00000000000000000000000000101064001412
      69000F106C001A177E002B2D8000333580003636800032328000272780001515
      790011136B000F106A0010106400000000000000000000000000201FC8002823
      D2001D20D800342DFC005559FF006669FF006B6BFF006464FF004E4EFF002929
      F2002226D5001D1FD3002020C700000000000000000000000000013C08000140
      080001560B002B6C33002F6C3600296B3100286A30002A6B3100336D3A002469
      2C0001540B0001410800013C080000000000000000000000000001770F000180
      100001AB150056D765005ED86C0051D661004FD45F0053D6620065D9730048D2
      580001A715000182100001780F000000000000000000000000000F1062001111
      68001A187F0045478000494980004345800044438000444680004D4C80004040
      800019197A0013116800101161000000000000000000000000001D1FC3002222
      D0003430FD008A8EFF009192FF00858AFF008785FF00878BFF009998FF008080
      FF003232F4002622D0001F22C200000000000000000000000000013A08000148
      0900286A30004F7454004B734F00437148003C6F420046724B00547558005E78
      610025692D00014709000138070000000000000000000000000001730F000190
      120050D460009EE7A70095E59E0085E2900078DE84008CE39600A8EAB000BBEF
      C10049D25900018E120001700E000000000000000000000000000D105E001412
      74004443800062628000605F800058598000545480005B5B8000676580006C6B
      80004141800016156D000E0F5D000000000000000000000000001920BB002823
      E8008786FF00C4C4FF00C0BDFF00B0B2FF00A7A7FF00B6B6FF00CECAFF00D8D6
      FF008181FF002C29D9001C1DBA00000000000000000000000000013007000145
      0900336D3A006B7B6D006B7B6D00637966005C775F0064796700737D7400737D
      7400306C37000148090001360700000000000000000000000000015F0D000189
      110066D97400D5F5D900D6F5DA00C6F1CB00B8EEBE00C8F2CD00E5F9E700E6FA
      E8005FD86D00018F1100016C0E000000000000000000000000000D0D50001210
      71004D4D80007474800074748000706F80006B6B800071708000787980007979
      80004B49800017176A00111152000000000000000000000000001919A0002320
      E2009999FF00E7E7FF00E8E8FF00E0DEFF00D5D6FF00E1DFFF00F0F1FF00F1F2
      FF009592FF002D2DD3002222A40000000000000000000000000000000000013E
      080001560B005A775E00777E7800767E7700717D7300747D7500747D75004C73
      5100015A0C00013F08000000000000000000000000000000000000000000017B
      0F0001AB1500B4EDBB00ECFAEE00ECFBEE00E2F9E500E7F9E900E7F9E90098E6
      A10002B41700017D100000000000000000000000000000000000000000000F10
      650019187F00696A80007C7C81007A7B80007878800079798000797980006060
      80001F2279001412630000000000000000000000000000000000000000001D20
      C9003230FD00D2D3FF00F5F5FF00F4F5FF00EFF0FF00F2F2FF00F2F2FF00BFC0
      FF003E44F1002823C6000000000000000000000000000000000000000000012B
      0600013E080001550B003D704300687A6A00747D7500607863002E6C3500014F
      0A00013D08000130060000000000000000000000000000000000000000000156
      0B00017C100001AA150079DF8500D0F4D400E7F9E900C0EFC6005BD86A00019D
      1300017A0F00015F0C0000000000000000000000000000000000000000000C0D
      4500111165001A187E005455800072728000797980006D6D8000474980001816
      77000E1064001210470000000000000000000000000000000000000000001719
      8A002222CA003330FC00A7A9FF00E4E4FF00F2F2FF00DADAFF008E91FF00302C
      ED001C20C70023208E0000000000000000000000000000000000000000000000
      000001150300012C0600013E080001510B0001560B0001500A00013F0800012C
      0600011603000000000000000000000000000000000000000000000000000000
      00000125060001580C00017C100001A2150001AC1500019F1400017E10000158
      0B00012A06000000000000000000000000000000000000000000000000000000
      000008081F000C0C4B00101165001B1A77001E1D7900181A7400101362000D0C
      49000A081F000000000000000000000000000000000000000000000000000000
      00000D0D3700171795002022CA003633ED003B3AF1003034E8002026C4001917
      920014103B000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000012C0600012C0600012C0600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000001580B0001580B0001580B00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000D0C49000D0C49000D0C4900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000191792001917920019179200000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000200000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000FC1FFC1FFC1FFC1FF007F007F007F007
      E003E003E003E003C001C001C001C001C001C001C001C0018000800080008000
      8000800080008000800080008000800080008000800080008000800080008000
      8000800080008000C001C001C001C001C001C001C001C001E003E003E003E003
      F007F007F007F007FC1FFC1FFC1FFC1F}
  end
  object juh: TActionList
    Left = 424
    object PowerOnAction: TAction
      Caption = 'Power On'
      OnExecute = PowerOnActionExecute
    end
    object PowerOffAction: TAction
      Caption = 'Power Off'
      OnExecute = PowerOffActionExecute
    end
    object HeatingOnAction: TAction
      Caption = 'Heating On'
      OnExecute = HeatingOnClick
    end
    object HeatingOffAction: TAction
      Caption = 'Heating Off'
      OnExecute = HeatingOffClick
    end
    object UVLampsOn: TAction
      Caption = 'On'
      OnExecute = UVLampsOnExecute
    end
    object UVLampsOff: TAction
      Caption = 'Off'
      OnExecute = UVLampsOffExecute
    end
    object HeadsFillingOn: TAction
      Caption = 'Filling On'
      OnExecute = HeadsFillingOnExecute
    end
    object HeadsFillingOff: TAction
      Caption = 'Filling Off'
      OnExecute = HeadsFillingOffExecute
    end
    object LockDoorAction: TAction
      Caption = 'Lock Door'
      OnExecute = LockDoorExecute
    end
    object UnLockDoorAction: TAction
      Caption = 'Unlock Door'
      OnExecute = UnLockDoorExecute
    end
    object OdourOn: TAction
      Caption = 'On'
      OnExecute = OdourOnExecute
    end
    object OdourOff: TAction
      Caption = 'Off'
      OnExecute = OdourOffExecute
    end
  end
end
