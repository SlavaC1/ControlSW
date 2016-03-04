object ActuatorsControlForm: TActuatorsControlForm
  Left = 250
  Top = 3
  BorderStyle = bsDialog
  Caption = 'Actuators & Sensors Control'
  ClientHeight = 455
  ClientWidth = 546
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
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 152
    Top = 452
    Width = 243
    Height = 29
    Caption = 'Hidden Part - Eden500'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object SensorsGroupBox: TGroupBox
    Left = 402
    Top = 5
    Width = 139
    Height = 442
    Caption = 'Sensors'
    TabOrder = 0
    object Label45: TLabel
      Left = 11
      Top = 141
      Width = 59
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'Model 1 Left'
      ParentBiDiMode = False
    end
    object Label46: TLabel
      Left = 11
      Top = 165
      Width = 66
      Height = 13
      Caption = 'Model 1 Right'
    end
    object Label48: TLabel
      Left = 11
      Top = 93
      Width = 58
      Height = 13
      Caption = 'Support Left'
    end
    object Label49: TLabel
      Left = 11
      Top = 117
      Width = 65
      Height = 13
      Caption = 'Support Right'
    end
    object Image2: TImage
      Tag = 11
      Left = 107
      Top = 42
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
      Top = 66
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model1LeftMSImg: TImage
      Tag = 16
      Left = 107
      Top = 138
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model1RightMSImg: TImage
      Tag = 17
      Left = 107
      Top = 162
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object SupportLeftMSImg: TImage
      Left = 107
      Top = 90
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object SupportRightMSImg: TImage
      Left = 107
      Top = 114
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label76: TLabel
      Left = 11
      Top = 293
      Width = 41
      Height = 13
      Caption = 'Interlock'
    end
    object Image42: TImage
      Tag = 10
      Left = 107
      Top = 290
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label8: TLabel
      Left = 11
      Top = 316
      Width = 62
      Height = 13
      Caption = 'Tray Inserted'
    end
    object Image4: TImage
      Tag = 8
      Left = 107
      Top = 313
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model2LeftMSImg: TImage
      Left = 107
      Top = 186
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model2RightMSImg: TImage
      Left = 107
      Top = 210
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model3LeftMSImg: TImage
      Tag = 22
      Left = 107
      Top = 234
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model3RightMSImg: TImage
      Tag = 23
      Left = 107
      Top = 258
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model3InsertedLabel: TLabel
      Left = 11
      Top = 189
      Width = 59
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'Model 2 Left'
      ParentBiDiMode = False
    end
    object Model4InsertedLabel: TLabel
      Left = 11
      Top = 213
      Width = 66
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'Model 2 Right'
      ParentBiDiMode = False
    end
    object Model5InsertedLabel: TLabel
      Left = 11
      Top = 237
      Width = 59
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'Model 3 Left'
      ParentBiDiMode = False
    end
    object Model6InsertedLabel: TLabel
      Left = 11
      Top = 261
      Width = 66
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'Model 3 Right'
      ParentBiDiMode = False
    end
  end
  object ActuatorsGroupBox: TGroupBox
    Left = 5
    Top = 5
    Width = 382
    Height = 442
    Caption = 'Actuators'
    TabOrder = 1
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
    object Model1LeftPumpLbl: TLabel
      Tag = 9
      Left = 11
      Top = 44
      Width = 59
      Height = 13
      Caption = 'Model 1 Left'
    end
    object Label31: TLabel
      Left = 200
      Top = 65
      Width = 65
      Height = 13
      Caption = 'Left UV Lamp'
    end
    object Model2LeftPumpLbl: TLabel
      Tag = 12
      Left = 11
      Top = 86
      Width = 59
      Height = 13
      Caption = 'Model 2 Left'
    end
    object S1PumpLabel: TLabel
      Tag = 11
      Left = 11
      Top = 129
      Width = 58
      Height = 13
      Caption = 'Support Left'
    end
    object S2PumpLabel: TLabel
      Tag = 14
      Left = 11
      Top = 151
      Width = 65
      Height = 13
      Caption = 'Support Right'
    end
    object Label41: TLabel
      Left = 11
      Top = 380
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
    object Model1LeftPumpImg: TImage
      Tag = 32
      Left = 152
      Top = 42
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model2LeftPumpImg: TImage
      Left = 152
      Top = 84
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object S1PumpImage: TImage
      Left = 152
      Top = 127
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object S2PumpImage: TImage
      Left = 152
      Top = 149
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image14: TImage
      Tag = 21
      Left = 152
      Top = 378
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
      Top = 244
      Width = 56
      Height = 13
      Caption = 'Tray Heater'
    end
    object YZTMotorsPowerLabel: TLabel
      Left = 200
      Top = 265
      Width = 101
      Height = 13
      Caption = 'Y, Z, T Motors Power'
    end
    object Label58: TLabel
      Left = 11
      Top = 359
      Width = 69
      Height = 13
      Caption = 'Vacuum Valve'
    end
    object Label63: TLabel
      Left = 11
      Top = 318
      Width = 76
      Height = 13
      Caption = 'Wiper Dirt pump'
    end
    object Label64: TLabel
      Left = 11
      Top = 338
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
      Top = 242
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object YZTMotorsPowerImage: TImage
      Tag = 24
      Left = 344
      Top = 263
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image28: TImage
      Tag = 20
      Left = 152
      Top = 357
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image29: TImage
      Tag = 19
      Left = 152
      Top = 316
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image30: TImage
      Tag = 18
      Left = 152
      Top = 336
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
      Top = 379
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object ACControlLabel: TLabel
      Left = 200
      Top = 381
      Width = 50
      Height = 13
      Caption = 'AC Control'
    end
    object Label69: TLabel
      Left = 200
      Top = 360
      Width = 38
      Height = 13
      Caption = 'VPP On'
    end
    object Image35: TImage
      Tag = 29
      Left = 344
      Top = 358
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Image36: TImage
      Tag = 30
      Left = 344
      Top = 316
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label70: TLabel
      Left = 200
      Top = 318
      Width = 110
      Height = 13
      Caption = 'Heads && Block Heaters'
    end
    object M1DrainPumpImage: TImage
      Tag = 3
      Left = 152
      Top = 200
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object M1DrainPumpLabel: TLabel
      Left = 11
      Top = 202
      Width = 96
      Height = 13
      Caption = 'Model 1 Drain Pump'
      Visible = False
    end
    object M2DrainPumpLabel: TLabel
      Left = 11
      Top = 223
      Width = 96
      Height = 13
      Caption = 'Model 2 Drain Pump'
      Visible = False
    end
    object M2DrainPumpImage: TImage
      Tag = 2
      Left = 152
      Top = 221
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object S1DrainPumpImage: TImage
      Tag = 1
      Left = 152
      Top = 242
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object S1DrainPumpLabel: TLabel
      Left = 11
      Top = 244
      Width = 104
      Height = 13
      Caption = 'Support 1 Drain Pump'
    end
    object S2DrainPumpLabel: TLabel
      Left = 11
      Top = 265
      Width = 104
      Height = 13
      Caption = 'Support 2 Drain Pump'
    end
    object S2DrainPumpImage: TImage
      Left = 152
      Top = 263
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
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
    object DrainPumpsLabel: TLabel
      Tag = 100
      Left = 16
      Top = 177
      Width = 115
      Height = 13
      AutoSize = False
      Caption = 'Drain Pumps'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object DrainPumpsBevel: TBevel
      Left = 8
      Top = 192
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Label4: TLabel
      Tag = 100
      Left = 16
      Top = 293
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
      Top = 308
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
      Top = 308
      Width = 165
      Height = 7
      Shape = bsTopLine
    end
    object Label7: TLabel
      Tag = 100
      Left = 205
      Top = 293
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
      Top = 398
      Width = 99
      Height = 13
      Caption = '(X Driver, UV Lamps)'
    end
    object Label71: TLabel
      Left = 200
      Top = 339
      Width = 87
      Height = 13
      Caption = 'Power Supply 24V'
    end
    object Image37: TImage
      Tag = 31
      Left = 344
      Top = 337
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model1RightPumpImg: TImage
      Tag = 33
      Left = 152
      Top = 63
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model1RightPumpLbl: TLabel
      Tag = 10
      Left = 11
      Top = 65
      Width = 66
      Height = 13
      Caption = 'Model 1 Right'
    end
    object Model2RightPumpImg: TImage
      Left = 152
      Top = 106
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model2RightPumpLbl: TLabel
      Tag = 13
      Left = 11
      Top = 108
      Width = 66
      Height = 13
      Caption = 'Model 2 Right'
    end
    object RollerSuctionSystemLabel: TLabel
      Tag = 100
      Left = 200
      Top = 97
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
      Top = 112
      Width = 165
      Height = 7
      Shape = bsTopLine
      Visible = False
    end
    object RollerSuctionValveLabel: TLabel
      Tag = 3
      Left = 203
      Top = 142
      Width = 96
      Height = 13
      Caption = 'Roller Suction Valve'
      Visible = False
    end
    object RollerSuctionPumpLabel: TLabel
      Tag = 2
      Left = 203
      Top = 121
      Width = 96
      Height = 13
      Caption = 'Roller Suction Pump'
      Visible = False
    end
    object RollerSuctionPumpImage: TImage
      Tag = 2
      Left = 343
      Top = 119
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
      Top = 142
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Model3LeftPumpLbl: TLabel
      Tag = 12
      Left = 11
      Top = 401
      Width = 55
      Height = 13
      Caption = 'Model 3 left'
    end
    object Model3RightPumpLbl: TLabel
      Tag = 13
      Left = 11
      Top = 421
      Width = 61
      Height = 13
      Caption = 'Model 3 right'
    end
    object Model3LeftPumpImg: TImage
      Tag = 42
      Left = 152
      Top = 398
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Model3RightPumpImg: TImage
      Tag = 3
      Left = 152
      Top = 419
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
      TabOrder = 0
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
      TabOrder = 1
    end
    object Model1LeftPumpBtn: TButton
      Tag = 32
      Left = 122
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
      TabOrder = 2
    end
    object Model2LeftPumpBtn: TButton
      Left = 122
      Top = 83
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
    object S1PumpBtn: TButton
      Left = 122
      Top = 126
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
    object S2PumpBtn: TButton
      Left = 122
      Top = 148
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
    object Button25: TButton
      Tag = 21
      Left = 122
      Top = 377
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
      TabOrder = 7
    end
    object TrayHeaterButton: TButton
      Tag = 27
      Left = 314
      Top = 241
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
    object YZTMotorsPowerButton: TButton
      Tag = 24
      Left = 314
      Top = 262
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
    object Button38: TButton
      Tag = 20
      Left = 122
      Top = 356
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
    object Button39: TButton
      Tag = 19
      Left = 122
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
      TabOrder = 11
    end
    object Button42: TButton
      Tag = 18
      Left = 122
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
      TabOrder = 12
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
      TabOrder = 13
    end
    object Button50: TButton
      Tag = 28
      Left = 314
      Top = 378
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
    object Button51: TButton
      Tag = 29
      Left = 314
      Top = 357
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
    object Button54: TButton
      Tag = 30
      Left = 314
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
      TabOrder = 16
    end
    object M1DrainPumpBtn: TButton
      Tag = 3
      Left = 122
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
      TabOrder = 17
    end
    object M2DrainPumpBtn: TButton
      Tag = 2
      Left = 122
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
      TabOrder = 18
    end
    object S1DrainPumpBtn: TButton
      Tag = 1
      Left = 122
      Top = 241
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
    object S2DrainPumpBtn: TButton
      Left = 122
      Top = 262
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
    object Button55: TButton
      Tag = 31
      Left = 314
      Top = 336
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 21
    end
    object Model1RightPumpBtn: TButton
      Tag = 33
      Left = 122
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
      TabOrder = 22
    end
    object Model2RightPumpBtn: TButton
      Left = 122
      Top = 105
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 23
    end
    object RollerSuctionValveButton: TButton
      Tag = 3
      Left = 314
      Top = 141
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 24
    end
    object RollerSuctionPumpButton: TButton
      Tag = 2
      Left = 314
      Top = 119
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 25
    end
    object Model3LeftPumpBtn: TButton
      Tag = 42
      Left = 122
      Top = 397
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 26
    end
    object Model3RightPumpBtn: TButton
      Tag = 43
      Left = 122
      Top = 418
      Width = 25
      Height = 18
      Action = ActuatorOnAction
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 27
    end
  end
  object GroupBox1: TGroupBox
    Left = 5
    Top = 479
    Width = 382
    Height = 194
    Caption = ' Spare/Unused Actuators '
    TabOrder = 2
    object Label33: TLabel
      Left = 189
      Top = 108
      Width = 85
      Height = 13
      Caption = 'SPARE_PUMP_1'
      Visible = False
    end
    object Label36: TLabel
      Left = 14
      Top = 87
      Width = 85
      Height = 13
      Caption = 'SPARE_PUMP_2'
      Visible = False
    end
    object Label37: TLabel
      Left = 14
      Top = 108
      Width = 38
      Height = 13
      Caption = 'PUMPS'
      Visible = False
    end
    object Label39: TLabel
      Left = 189
      Top = 23
      Width = 44
      Height = 13
      Caption = 'DIMMER'
      Visible = False
    end
    object Label40: TLabel
      Left = 189
      Top = 44
      Width = 44
      Height = 13
      Caption = 'BUZZER'
      Visible = False
    end
    object Image6: TImage
      Tag = 10
      Left = 331
      Top = 106
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image9: TImage
      Tag = 13
      Left = 148
      Top = 85
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image10: TImage
      Tag = 15
      Left = 148
      Top = 106
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image12: TImage
      Tag = 4
      Left = 331
      Top = 21
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image13: TImage
      Tag = 7
      Left = 331
      Top = 42
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Label66: TLabel
      Left = 14
      Top = 45
      Width = 55
      Height = 13
      Caption = 'Port Enable'
    end
    object Image32: TImage
      Tag = 8
      Left = 148
      Top = 43
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label67: TLabel
      Left = 14
      Top = 66
      Width = 31
      Height = 13
      Caption = 'PC On'
    end
    object Image33: TImage
      Tag = 6
      Left = 148
      Top = 64
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Button9: TButton
      Tag = 10
      Left = 301
      Top = 105
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
    object Button15: TButton
      Tag = 13
      Left = 118
      Top = 84
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
    object Button17: TButton
      Tag = 15
      Left = 118
      Top = 105
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
    object Button21: TButton
      Tag = 4
      Left = 301
      Top = 20
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
    object Button23: TButton
      Tag = 7
      Left = 301
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
      TabOrder = 4
    end
    object Button46: TButton
      Tag = 8
      Left = 118
      Top = 42
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
    object Button47: TButton
      Tag = 6
      Left = 118
      Top = 63
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
  end
  object GroupBox2: TGroupBox
    Left = 400
    Top = 479
    Width = 139
    Height = 194
    Caption = ' Spare/Unused Sensors '
    TabOrder = 3
    object Label47: TLabel
      Left = 11
      Top = 44
      Width = 51
      Height = 13
      Caption = '(Not Used)'
      Visible = False
    end
    object Label50: TLabel
      Left = 11
      Top = 117
      Width = 51
      Height = 13
      Caption = '(Not Used)'
      Visible = False
    end
    object Image20: TImage
      Tag = 6
      Left = 107
      Top = 42
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image23: TImage
      Tag = 7
      Left = 107
      Top = 115
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Label77: TLabel
      Left = 11
      Top = 141
      Width = 51
      Height = 13
      Caption = '(Not Used)'
      Visible = False
    end
    object Image43: TImage
      Left = 107
      Top = 139
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Label78: TLabel
      Left = 11
      Top = 166
      Width = 51
      Height = 13
      Caption = '(Not Used)'
      Visible = False
    end
    object Image44: TImage
      Tag = 1
      Left = 107
      Top = 164
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image24: TImage
      Tag = 8
      Left = 107
      Top = 18
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
    end
    object Label53: TLabel
      Left = 11
      Top = 20
      Width = 62
      Height = 13
      Caption = 'Tray in Place'
    end
    object Label54: TLabel
      Left = 11
      Top = 68
      Width = 51
      Height = 13
      Caption = '(Not Used)'
      Visible = False
    end
    object Image45: TImage
      Tag = 12
      Left = 107
      Top = 66
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Image46: TImage
      Tag = 13
      Left = 107
      Top = 91
      Width = 16
      Height = 16
      AutoSize = True
      Center = True
      Transparent = True
      Visible = False
    end
    object Label55: TLabel
      Left = 11
      Top = 93
      Width = 51
      Height = 13
      Caption = '(Not Used)'
      Visible = False
    end
  end
  object ActionList1: TActionList
    Left = 464
    Top = 397
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
    Left = 432
    Top = 397
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
end
