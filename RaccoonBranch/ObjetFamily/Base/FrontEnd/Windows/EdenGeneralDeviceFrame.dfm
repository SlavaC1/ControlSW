object GeneralDeviceFrame: TGeneralDeviceFrame
  Left = 0
  Top = 0
  Width = 501
  Height = 720
  TabOrder = 0
  object GroupBox1: TGroupBox
    Left = 10
    Top = 7
    Width = 230
    Height = 186
    Caption = 'General'
    TabOrder = 0
    object PowerButton: TSpeedButton
      Left = 16
      Top = 20
      Width = 97
      Height = 25
      Action = PowerOnAction
      AllowAllUp = True
      Layout = blGlyphRight
    end
    object LockDoorButton: TSpeedButton
      Left = 16
      Top = 52
      Width = 97
      Height = 25
      Action = LockDoorAction
      AllowAllUp = True
      Layout = blGlyphRight
    end
    object OCBResetButton: TButton
      Left = 16
      Top = 84
      Width = 97
      Height = 25
      Caption = 'OCB SW Reset'
      TabOrder = 0
    end
    object OHDBResetButton: TButton
      Left = 16
      Top = 116
      Width = 97
      Height = 25
      Caption = 'OHDB SW Reset'
      TabOrder = 1
    end
    object MCBResetButton: TButton
      Left = 16
      Top = 148
      Width = 97
      Height = 25
      Action = MCBResetAction
      TabOrder = 2
    end
    object PowerStatusPanel: TPanel
      Left = 135
      Top = 22
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = 'Off'
      Color = clRed
      ParentBackground = False
      TabOrder = 3
    end
    object DoorStatusPanel: TPanel
      Left = 135
      Top = 54
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = 'Unlocked'
      Color = clRed
      ParentBackground = False
      TabOrder = 4
    end
  end
  object GroupBox2: TGroupBox
    Left = 246
    Top = 199
    Width = 236
    Height = 139
    Caption = 'Ambient Temperature'
    TabOrder = 3
    object Label5: TLabel
      Left = 16
      Top = 39
      Width = 66
      Height = 13
      Caption = 'Current Temp'
    end
    object Label3: TLabel
      Left = 16
      Top = 72
      Width = 51
      Height = 13
      Caption = 'Fan status'
    end
    object OdourFanButton: TSpeedButton
      Left = 16
      Top = 97
      Width = 97
      Height = 25
      Action = AmbientOdourFanOnAction
      Layout = blGlyphRight
    end
    object Label17: TLabel
      Left = 116
      Top = 15
      Width = 12
      Height = 13
      Caption = #176'C'
    end
    object Label18: TLabel
      Left = 183
      Top = 15
      Width = 18
      Height = 13
      Caption = 'A/D'
    end
    object A2D_CurrAmbientTempPanel: TPanel
      Left = 164
      Top = 35
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object FanStatusPanel: TPanel
      Left = 164
      Top = 68
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = 'Off'
      Color = clRed
      ParentBackground = False
      TabOrder = 1
    end
    object OdourFanValueEdit: TEdit
      Left = 164
      Top = 99
      Width = 43
      Height = 21
      TabOrder = 2
      Text = '128'
    end
    object OdourFanValueUpDown: TUpDown
      Left = 207
      Top = 99
      Width = 15
      Height = 21
      Associate = OdourFanValueEdit
      Min = 1
      Max = 255
      Position = 128
      TabOrder = 3
    end
    object C_CurrAmbientTempPanel: TPanel
      Left = 92
      Top = 35
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
  end
  object UVLampsGroupBox: TGroupBox
    Left = 8
    Top = 343
    Width = 474
    Height = 186
    Caption = 'UV Lamps'
    TabOrder = 2
    object Label8: TLabel
      Left = 16
      Top = 32
      Width = 62
      Height = 13
      Caption = 'Sensor Value'
    end
    object Label21: TLabel
      Left = 16
      Top = 74
      Width = 98
      Height = 13
      Caption = 'Sensor Frozen Value'
    end
    object UVSensor: TPanel
      Left = 16
      Top = 47
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object RestartAveragingButton: TButton
      Left = 16
      Top = 140
      Width = 81
      Height = 25
      Caption = 'Reset Avg.'
      TabOrder = 4
      OnClick = RestartAveragingButtonClick
    end
    object SensorSyncAveragePanel: TPanel
      Left = 16
      Top = 88
      Width = 73
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
    object UVDimmingGroupBox: TGroupBox
      Left = 238
      Top = 15
      Width = 227
      Height = 158
      Caption = ' UV Dimming (Level)  '
      TabOrder = 3
      object SetUVDimmingButton: TButton
        Left = 68
        Top = 125
        Width = 97
        Height = 25
        Caption = 'Set'
        TabOrder = 0
        OnClick = SetUVDimmingButtonClick
      end
    end
    object UVOnOffGroupBox: TGroupBox
      Left = 120
      Top = 15
      Width = 113
      Height = 158
      Caption = 'UV Lamps On/Off'
      TabOrder = 0
      object TurnUVLampsButton: TSpeedButton
        Left = 17
        Top = 125
        Width = 80
        Height = 25
        Action = UVLampsOnAction
        AllowAllUp = True
        Layout = blGlyphRight
      end
    end
  end
  object GroupBox4: TGroupBox
    Left = 246
    Top = 7
    Width = 236
    Height = 186
    Caption = 'Roller'
    TabOrder = 1
    object TurnRollerButton: TSpeedButton
      Left = 81
      Top = 148
      Width = 72
      Height = 25
      Action = RollerOnAction
      AllowAllUp = True
      Layout = blGlyphRight
    end
    object Label9: TLabel
      Left = 16
      Top = 55
      Width = 84
      Height = 13
      Caption = 'Requested speed'
    end
    object Label11: TLabel
      Left = 16
      Top = 21
      Width = 31
      Height = 13
      Caption = 'Status'
    end
    object SuctionValveOnLabel: TLabel
      Left = 16
      Top = 98
      Width = 110
      Height = 13
      Caption = 'Suction Valve On (A/D)'
    end
    object SuctionValveOffLabel: TLabel
      Left = 17
      Top = 126
      Width = 112
      Height = 13
      Caption = 'Suction Valve Off (A/D)'
    end
    object ReqRollerSpeedEdit: TEdit
      Left = 164
      Top = 47
      Width = 44
      Height = 21
      TabOrder = 0
      Text = '-1'
    end
    object RollerStatusPanel: TPanel
      Left = 164
      Top = 14
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = 'Off'
      Color = clRed
      ParentBackground = False
      TabOrder = 1
    end
    object ReqRollerSpeedUpDown: TUpDown
      Left = 208
      Top = 47
      Width = 15
      Height = 21
      Associate = ReqRollerSpeedEdit
      Min = -1
      Max = -1
      Position = -1
      TabOrder = 2
    end
    object RSSPumpOnValuePanel: TPanel
      Left = 164
      Top = 93
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object RSSPumpOffValuePanel: TPanel
      Left = 164
      Top = 121
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
  end
  object GroupBox5: TGroupBox
    Left = 265
    Top = 607
    Width = 217
    Height = 159
    Caption = 'Tray'
    TabOrder = 7
    Visible = False
    object Label1: TLabel
      Left = 12
      Top = 39
      Width = 64
      Height = 13
      Caption = 'Current temp'
    end
    object Label2: TLabel
      Left = 12
      Top = 76
      Width = 49
      Height = 13
      Caption = 'Req.Temp'
    end
    object TurnTrayButton: TSpeedButton
      Left = 16
      Top = 120
      Width = 71
      Height = 24
      Action = TrayOnAction
      AllowAllUp = True
      Layout = blGlyphRight
    end
    object Label15: TLabel
      Left = 102
      Top = 15
      Width = 12
      Height = 13
      Caption = #176'C'
    end
    object Label16: TLabel
      Left = 165
      Top = 15
      Width = 18
      Height = 13
      Caption = 'A/D'
    end
    object A2D_TrayTemperaturePanel: TPanel
      Left = 146
      Top = 35
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object TrayTemperatureEdit: TEdit
      Left = 78
      Top = 72
      Width = 44
      Height = 21
      TabOrder = 1
      Text = '0'
    end
    object ReqTrayTempUpDown: TUpDown
      Left = 122
      Top = 72
      Width = 15
      Height = 21
      Associate = TrayTemperatureEdit
      Max = 700
      TabOrder = 2
    end
    object C_TrayTemperaturePanel: TPanel
      Left = 78
      Top = 35
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object A2DTrayTemperaturePanel: TPanel
      Left = 146
      Top = 72
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 4
    end
  end
  object GroupBox6: TGroupBox
    Left = 10
    Top = 199
    Width = 230
    Height = 138
    Caption = 'Vacuum'
    TabOrder = 4
    object Label4: TLabel
      Left = 12
      Top = 60
      Width = 41
      Height = 13
      Caption = 'Average'
    end
    object Label12: TLabel
      Left = 12
      Top = 96
      Width = 62
      Height = 13
      Caption = 'Current level'
    end
    object Label19: TLabel
      Left = 86
      Top = 33
      Width = 40
      Height = 13
      Caption = 'CM/H2O'
    end
    object Label20: TLabel
      Left = 165
      Top = 33
      Width = 18
      Height = 13
      Caption = 'A/D'
    end
    object A2D_VacuumSensorAveragePanel: TPanel
      Left = 146
      Top = 56
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object A2D_VacuumSensorLastValuePanel: TPanel
      Left = 146
      Top = 92
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object H2O_VacuumSensorAveragePanel: TPanel
      Left = 78
      Top = 56
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
    object H2O_VacuumSensorLastValuePanel: TPanel
      Left = 78
      Top = 92
      Width = 59
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
  end
  object GroupBox7: TGroupBox
    Left = 10
    Top = 535
    Width = 471
    Height = 50
    Caption = 'Material Cooling Fans'
    TabOrder = 5
    object CoolingFansSpeedButton: TSpeedButton
      Left = 360
      Top = 16
      Width = 97
      Height = 25
      Action = CoolingFansOnAction
      AllowAllUp = True
      Layout = blGlyphRight
    end
    object Label7: TLabel
      Left = 24
      Top = 20
      Width = 52
      Height = 13
      Caption = 'Duty Cycle'
    end
    object CoolingFansDutyCycleEdit: TEdit
      Left = 135
      Top = 16
      Width = 56
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object CoolingFansDutyCycleUpDown: TUpDown
      Left = 191
      Top = 16
      Width = 15
      Height = 21
      Associate = CoolingFansDutyCycleEdit
      TabOrder = 1
    end
  end
  object DisplayValuesModeCheckBox: TCheckBox
    Left = 10
    Top = 600
    Width = 199
    Height = 17
    Caption = 'Display values in LCD Frame in A/D'
    TabOrder = 6
    OnClick = DisplayValuesModeCheckBoxClick
  end
  object ActionList1: TActionList
    Left = 176
    object PowerOnAction: TAction
      Category = 'Power'
      Caption = 'Power      '
      ImageIndex = 0
      OnExecute = PowerOnActionExecute
    end
    object PowerOffAction: TAction
      Category = 'Power'
      Caption = 'Power      '
      ImageIndex = 1
      OnExecute = PowerOffActionExecute
    end
    object LockDoorAction: TAction
      Category = 'Door'
      Caption = 'Lock Door'
      ImageIndex = 2
      OnExecute = LockDoorActionExecute
    end
    object UnlockDoorAction: TAction
      Category = 'Door'
      Caption = 'Unlock Door'
      ImageIndex = 3
      OnExecute = UnlockDoorActionExecute
    end
    object MCBResetAction: TAction
      Caption = 'MCB SW Reset'
      OnExecute = MCBResetActionExecute
    end
    object TrayOnAction: TAction
      Category = 'Tray'
      Caption = 'Turn On'
      ImageIndex = 0
      OnExecute = TrayOnActionExecute
    end
    object TrayOffAction: TAction
      Category = 'Tray'
      Caption = 'Turn Off'
      ImageIndex = 1
      OnExecute = TrayOffActionExecute
    end
    object AmbientTempOnAction: TAction
      Category = 'AmbientTemperature'
      Caption = 'Monitor'
      ImageIndex = 0
    end
    object AmbientTempOffAction: TAction
      Category = 'AmbientTemperature'
      Caption = 'Monitor'
      ImageIndex = 1
    end
    object RollerOnAction: TAction
      Category = 'Roller'
      Caption = 'Turn On'
      ImageIndex = 0
      OnExecute = RollerOnActionExecute
    end
    object RollerOffAction: TAction
      Category = 'Roller'
      Caption = 'Turn Off'
      ImageIndex = 1
      OnExecute = RollerOffActionExecute
    end
    object UVLampsOnAction: TAction
      Category = 'UVLamps'
      Caption = 'Turn On'
      ImageIndex = 0
      OnExecute = UVLampsOnActionExecute
    end
    object UVLampsOffAction: TAction
      Category = 'UVLamps'
      Caption = 'Turn Off'
      ImageIndex = 1
      OnExecute = UVLampsOffActionExecute
    end
    object CoolingFansOnAction: TAction
      Category = 'CoolingFans'
      Caption = 'Turn On'
      ImageIndex = 0
      OnExecute = CoolingFansOnActionExecute
    end
    object CoolingFansOffAction: TAction
      Category = 'CoolingFans'
      Caption = 'Turn Off'
      ImageIndex = 1
      OnExecute = CoolingFansOffActionExecute
    end
    object AmbientOdourFanOnAction: TAction
      Category = 'AmbientTemperature'
      Caption = 'Fan On'
      ImageIndex = 0
      OnExecute = AmbientOdourFanOnActionExecute
    end
    object AmbientOdourFanOffAction: TAction
      Category = 'AmbientTemperature'
      Caption = 'Fan Off'
      ImageIndex = 1
      OnExecute = AmbientOdourFanOffActionExecute
    end
    object UVDimmingChangAction: TAction
      Category = 'UVLamps'
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 136
  end
end
