object ActuatorsControlForm: TActuatorsControlForm
  Left = 265
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Actuators & Sensors Control'
  ClientHeight = 550
  ClientWidth = 543
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Padding.Left = 5
  Padding.Top = 5
  Padding.Right = 5
  Padding.Bottom = 5
  Menu = MainMenu1
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object SensorsGroupBox: TGroupBox
    Left = 402
    Top = 3
    Width = 139
    Height = 542
    Caption = 'Sensors'
    TabOrder = 1
    object Image2: TImage
      Tag = 11
      Left = 107
      Top = 45
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label51: TLabel
      Left = 11
      Top = 45
      Width = 72
      Height = 13
      Caption = 'Right UV Lamp'
    end
    object Label52: TLabel
      Left = 11
      Top = 69
      Width = 65
      Height = 13
      Caption = 'Left UV Lamp'
    end
    object Image17: TImage
      Tag = 9
      Left = 107
      Top = 69
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object InterlockLbl: TLabel
      Left = 11
      Top = 93
      Width = 41
      Height = 13
      Caption = 'Interlock'
    end
    object InterlockImg: TImage
      Tag = 10
      Left = 107
      Top = 93
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object TrayInsertedLabel: TLabel
      Left = 11
      Top = 315
      Width = 62
      Height = 13
      Caption = 'Tray Inserted'
    end
    object TrayInsertedImg: TImage
      Tag = 8
      Left = 107
      Top = 313
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object serviceDoorImg: TImage
      Tag = 2
      Left = 107
      Top = 446
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object FrontRightDoorImg: TImage
      Tag = 3
      Left = 107
      Top = 467
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object FrontLeftDoorImg: TImage
      Tag = 4
      Left = 107
      Top = 489
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object PurgeBathImg: TImage
      Left = 107
      Top = 21
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object ServiceKeyDoorImg: TImage
      Tag = 5
      Left = 107
      Top = 426
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object lblServiceDoorLabel: TLabel
      Left = 11
      Top = 446
      Width = 62
      Height = 13
      Caption = 'Service Door'
    end
    object FrontRightDoorLabel: TLabel
      Left = 11
      Top = 467
      Width = 78
      Height = 13
      Caption = 'Front Right Door'
    end
    object FrontLeftDoorLabel: TLabel
      Left = 11
      Top = 492
      Width = 71
      Height = 13
      Caption = 'Front Left Door'
    end
    object PurgeBathLabel: TLabel
      Left = 11
      Top = 21
      Width = 89
      Height = 13
      Caption = 'Purge Bath Sensor'
    end
    object ServiceKeyDoorLabel: TLabel
      Left = 11
      Top = 427
      Width = 57
      Height = 13
      Caption = 'Service Key'
    end
    object EmergencyRearImg: TImage
      Tag = 6
      Left = 107
      Top = 380
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object EmergencyFrontImg: TImage
      Tag = 7
      Left = 107
      Top = 401
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object EmergencyRearlbl: TLabel
      Left = 11
      Top = 380
      Width = 79
      Height = 13
      Caption = 'Emergency Rear'
    end
    object EmergencyFrontlbl: TLabel
      Left = 11
      Top = 401
      Width = 80
      Height = 13
      Caption = 'Emergency Front'
    end
  end
  object ActuatorsGroupBox: TGroupBox
    Left = 5
    Top = 3
    Width = 391
    Height = 542
    Caption = 'Actuators'
    TabOrder = 0
    object Image1: TImage
      Tag = 26
      Left = 344
      Top = 42
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label29: TLabel
      Left = 200
      Top = 44
      Width = 72
      Height = 13
      Caption = 'Right UV Lamp'
    end
    object Label31: TLabel
      Left = 200
      Top = 65
      Width = 65
      Height = 13
      Caption = 'Left UV Lamp'
    end
    object Label41: TLabel
      Left = 11
      Top = 465
      Width = 42
      Height = 13
      Caption = 'Air Valve'
    end
    object Image3: TImage
      Tag = 25
      Left = 344
      Top = 63
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image14: TImage
      Tag = 21
      Left = 152
      Top = 465
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label44: TLabel
      Left = 200
      Top = 202
      Width = 44
      Height = 13
      Caption = 'Odor Fan'
    end
    object Image25: TImage
      Tag = 16
      Left = 344
      Top = 200
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object TrayHeaterLabel: TLabel
      Left = 200
      Top = 267
      Width = 56
      Height = 13
      Caption = 'Tray Heater'
    end
    object YZTMotorsPowerLabel: TLabel
      Left = 200
      Top = 245
      Width = 101
      Height = 13
      Caption = 'Y, Z, T Motors Power'
    end
    object Label58: TLabel
      Left = 11
      Top = 445
      Width = 69
      Height = 13
      Caption = 'Vacuum Valve'
    end
    object Label63: TLabel
      Left = 11
      Top = 405
      Width = 76
      Height = 13
      Caption = 'Wiper Dirt pump'
    end
    object Label64: TLabel
      Left = 11
      Top = 425
      Width = 57
      Height = 13
      Caption = 'Roller Pump'
    end
    object Label65: TLabel
      Left = 200
      Top = 223
      Width = 41
      Height = 13
      Caption = 'Interlock'
    end
    object TrayHeaterImage: TImage
      Tag = 27
      Left = 344
      Top = 265
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object YZTMotorsPowerImage: TImage
      Tag = 24
      Left = 344
      Top = 243
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image28: TImage
      Tag = 20
      Left = 152
      Top = 445
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image29: TImage
      Tag = 19
      Left = 152
      Top = 405
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image30: TImage
      Tag = 18
      Left = 152
      Top = 425
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image31: TImage
      Tag = 17
      Left = 344
      Top = 221
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image34: TImage
      Tag = 28
      Left = 344
      Top = 424
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object ACControlLabel: TLabel
      Left = 200
      Top = 426
      Width = 50
      Height = 13
      Caption = 'AC Control'
    end
    object Label69: TLabel
      Left = 200
      Top = 405
      Width = 38
      Height = 13
      Caption = 'VPP On'
    end
    object Image35: TImage
      Tag = 29
      Left = 344
      Top = 403
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image36: TImage
      Tag = 30
      Left = 344
      Top = 361
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label70: TLabel
      Left = 200
      Top = 363
      Width = 110
      Height = 13
      Caption = 'Heads && Block Heaters'
    end
    object Label2: TLabel
      Tag = 100
      Left = 16
      Top = 19
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'Container Pumps'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel5: TBevel
      Left = 8
      Top = 34
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Label4: TLabel
      Tag = 100
      Left = 16
      Top = 380
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'General Pumps'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel2: TBevel
      Left = 8
      Top = 397
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Label5: TLabel
      Tag = 100
      Left = 205
      Top = 19
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'UV Lamps'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel3: TBevel
      Left = 197
      Top = 34
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Label6: TLabel
      Tag = 100
      Left = 205
      Top = 177
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'Misc.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel4: TBevel
      Left = 197
      Top = 192
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Bevel6: TBevel
      Left = 197
      Top = 353
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Label7: TLabel
      Tag = 100
      Left = 205
      Top = 338
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'Power Supply'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ACControlLabel2: TLabel
      Left = 199
      Top = 443
      Width = 99
      Height = 13
      Caption = '(X Driver, UV Lamps)'
    end
    object Label71: TLabel
      Left = 200
      Top = 384
      Width = 87
      Height = 13
      Caption = 'Power Supply 24V'
    end
    object Image37: TImage
      Tag = 31
      Left = 344
      Top = 382
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object RollerSuctionSystemLabel: TLabel
      Tag = 100
      Left = 200
      Top = 90
      Width = 129
      Height = 13
      AutoSize = False
      Caption = 'Roller Suction System'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object RollerSuctionSystemBevel: TBevel
      Left = 200
      Top = 106
      Width = 165
      Height = 7
      Shape = bsTopLine
      Visible = False
    end
    object RollerSuctionValveLabel: TLabel
      Tag = 3
      Left = 203
      Top = 132
      Width = 96
      Height = 13
      Caption = 'Roller Suction Valve'
      Visible = False
    end
    object RollerSuctionPumpLabel: TLabel
      Tag = 2
      Left = 203
      Top = 112
      Width = 96
      Height = 13
      Caption = 'Roller Suction Pump'
      Visible = False
    end
    object RollerSuctionPumpImage: TImage
      Tag = 2
      Left = 343
      Top = 112
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object RollerSuctionValveImage: TImage
      Tag = 3
      Left = 343
      Top = 132
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object WasteRightLabel: TLabel
      Left = 11
      Top = 485
      Width = 59
      Height = 13
      Caption = 'Waste Right'
      Enabled = False
      Visible = False
    end
    object WasteRightImg: TImage
      Tag = -1
      Left = 152
      Top = 485
      Width = 16
      Height = 16
      Transparent = True
      Visible = False
    end
    object SignalTowerLabel: TLabel
      Tag = 100
      Left = 16
      Top = 288
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'Signal Tower'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object SignalTowerBevel: TBevel
      Left = 8
      Top = 305
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object RedLightLabel: TLabel
      Left = 11
      Top = 315
      Width = 46
      Height = 13
      Caption = 'Red Light'
    end
    object RedLightImage: TImage
      Tag = -1
      Left = 152
      Top = 315
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object GreenLightLabel: TLabel
      Left = 11
      Top = 335
      Width = 55
      Height = 13
      Caption = 'Green Light'
    end
    object GreenLightImage: TImage
      Tag = -1
      Left = 152
      Top = 335
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object YellowLightLabel: TLabel
      Left = 11
      Top = 355
      Width = 57
      Height = 13
      Caption = 'Yellow Light'
    end
    object YellowLightImage: TImage
      Tag = -1
      Left = 152
      Top = 355
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object AmbientHeater1Label: TLabel
      Left = 200
      Top = 289
      Width = 94
      Height = 13
      Caption = 'Ambient Heater Left'
    end
    object AmbientHeater1Image: TImage
      Tag = -1
      Left = 344
      Top = 289
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object AmbientHeater2Label: TLabel
      Left = 200
      Top = 313
      Width = 101
      Height = 13
      Caption = 'Ambient Heater Right'
    end
    object AmbientHeater2Image: TImage
      Tag = -1
      Left = 344
      Top = 313
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Button1: TButton
      Tag = 26
      Left = 314
      Top = 41
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
    end
    object Button3: TButton
      Tag = 25
      Left = 314
      Top = 62
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
    object Button25: TButton
      Tag = 21
      Left = 121
      Top = 465
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 19
    end
    object Button31: TButton
      Tag = 16
      Left = 314
      Top = 199
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 4
    end
    object TrayHeaterButton: TButton
      Tag = 27
      Left = 314
      Top = 264
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 10
    end
    object YZTMotorsPowerButton: TButton
      Tag = 24
      Left = 314
      Top = 242
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 8
    end
    object Button38: TButton
      Tag = 20
      Left = 121
      Top = 445
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 17
    end
    object Button39: TButton
      Tag = 19
      Left = 121
      Top = 405
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 13
    end
    object Button42: TButton
      Tag = 18
      Left = 121
      Top = 425
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 15
    end
    object Button43: TButton
      Tag = 17
      Left = 314
      Top = 220
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 6
    end
    object Button50: TButton
      Tag = 28
      Left = 314
      Top = 423
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 20
    end
    object Button51: TButton
      Tag = 29
      Left = 314
      Top = 402
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 18
    end
    object Button54: TButton
      Tag = 30
      Left = 314
      Top = 360
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 14
    end
    object Button55: TButton
      Tag = 31
      Left = 314
      Top = 381
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 16
    end
    object RollerSuctionValveButton: TButton
      Tag = 3
      Left = 314
      Top = 132
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
    end
    object RollerSuctionPumpButton: TButton
      Tag = 2
      Left = 314
      Top = 112
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object WasteRightBtn: TButton
      Left = 122
      Top = 485
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      TabOrder = 21
      Visible = False
    end
    object RedLightButton: TButton
      Tag = -1
      Left = 121
      Top = 315
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 5
    end
    object GreenLightButton: TButton
      Tag = -1
      Left = 121
      Top = 335
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 7
    end
    object YellowLightButton: TButton
      Tag = -1
      Left = 121
      Top = 355
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 9
    end
    object AmbientHeater1Button: TButton
      Tag = -1
      Left = 314
      Top = 288
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 11
    end
    object AmbientHeater2Button: TButton
      Tag = -1
      Left = 314
      Top = 312
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 12
    end
  end
  object ActionList1: TActionList
    Left = 312
    Top = 514
    object AlwaysOnTopAction: TAction
      Caption = 'Always on top'
      OnExecute = AlwaysOnTopActionExecute
    end
    object CloseDialogAction: TAction
      Caption = 'Close'
      OnExecute = CloseDialogActionExecute
    end
    object ActuatorOnAction: TAction
      Caption = 'On'
      OnExecute = ActuatorOnActionExecute
    end
    object ActuatorOffAction: TAction
      Caption = 'Off'
      OnExecute = ActuatorOffActionExecute
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 360
    Top = 514
    object Alwaysontop1: TMenuItem
      Action = AlwaysOnTopAction
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Close1: TMenuItem
      Action = CloseDialogAction
    end
  end
  object MainMenu1: TMainMenu
    Left = 264
    Top = 512
  end
end
