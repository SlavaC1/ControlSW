object OHDBTesterForm: TOHDBTesterForm
  Left = 314
  Top = 84
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'OHDB Tester'
  ClientHeight = 632
  ClientWidth = 581
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object BitBtn1: TBitBtn
    Left = 494
    Top = 597
    Width = 75
    Height = 25
    TabOrder = 0
    Kind = bkClose
  end
  object BumperTabSheet: TPageControl
    Left = 16
    Top = 16
    Width = 553
    Height = 575
    ActivePage = PrintingTabSheet
    TabOrder = 1
    object XilinxTabSheet: TTabSheet
      Caption = 'FPGA'
      object GroupBox3: TGroupBox
        Left = 8
        Top = 205
        Width = 247
        Height = 204
        Caption = 'Macro Recorder'
        TabOrder = 0
        object Label87: TLabel
          Left = 34
          Top = 24
          Width = 89
          Height = 13
          Caption = '[Add] space [Data]'
        end
        object MacroMemo: TMemo
          Left = 10
          Top = 40
          Width = 136
          Height = 151
          TabOrder = 0
        end
        object MacroLoadButton: TButton
          Left = 158
          Top = 40
          Width = 75
          Height = 25
          Caption = 'Load'
          TabOrder = 1
          OnClick = MacroLoadButtonClick
        end
        object MacroSaveButton: TButton
          Left = 158
          Top = 71
          Width = 75
          Height = 25
          Caption = 'Save'
          TabOrder = 2
          OnClick = MacroSaveButtonClick
        end
        object MacroExecuteButton: TButton
          Left = 158
          Top = 166
          Width = 75
          Height = 25
          Caption = 'Execute'
          TabOrder = 3
          OnClick = MacroExecuteButtonClick
        end
      end
      object GroupBox4: TGroupBox
        Left = 8
        Top = 16
        Width = 247
        Height = 183
        TabOrder = 1
        object Label1: TLabel
          Left = 39
          Top = 44
          Width = 38
          Height = 13
          Caption = 'Address'
        end
        object Label2: TLabel
          Left = 38
          Top = 79
          Width = 23
          Height = 13
          Caption = 'Data'
        end
        object XilinxAddressEdit: TEdit
          Left = 111
          Top = 40
          Width = 120
          Height = 21
          TabOrder = 0
        end
        object XilinxDataEdit: TEdit
          Left = 111
          Top = 75
          Width = 120
          Height = 21
          TabOrder = 1
          OnClick = DataEditClick
        end
        object XilinxReadButton: TButton
          Left = 77
          Top = 131
          Width = 75
          Height = 25
          Caption = 'Read'
          TabOrder = 2
          OnClick = XilinxReadButtonClick
        end
        object XilinxWriteButton: TButton
          Left = 158
          Top = 131
          Width = 75
          Height = 25
          Caption = 'Write'
          TabOrder = 3
          OnClick = XilinxWriteButtonClick
        end
        object MacroRecordCheckBox: TCheckBox
          Left = 17
          Top = 12
          Width = 64
          Height = 17
          Caption = 'Record'
          TabOrder = 4
        end
      end
    end
    object E2promTabSheet: TTabSheet
      Caption = 'E2PROM'
      ImageIndex = 1
      object GroupBox38: TGroupBox
        Left = 16
        Top = 16
        Width = 405
        Height = 175
        TabOrder = 0
        object Label3: TLabel
          Left = 18
          Top = 38
          Width = 38
          Height = 13
          Caption = 'Address'
        end
        object Label4: TLabel
          Left = 17
          Top = 83
          Width = 23
          Height = 13
          Caption = 'Data'
        end
        object Label7: TLabel
          Left = 261
          Top = 16
          Width = 67
          Height = 13
          Caption = 'Select Device'
        end
        object E2PROMAddressEdit: TEdit
          Left = 71
          Top = 32
          Width = 120
          Height = 21
          TabOrder = 0
        end
        object E2PROMDataEdit: TEdit
          Left = 71
          Top = 79
          Width = 120
          Height = 21
          TabOrder = 1
          OnClick = DataEditClick
        end
        object E2PROMComboBox: TComboBox
          Left = 238
          Top = 32
          Width = 112
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          TabOrder = 2
          Items.Strings = (
            'E2PROM 1 (S3)'
            'E2PROM 2 (S2)'
            'E2PROM 3 (S1)'
            'E2PROM 4 (S0)'
            'E2PROM 5 (M3)'
            'E2PROM 6 (M2)'
            'E2PROM 7 (M1)'
            'E2PROM 8 (M0)')
        end
        object E2PROMWriteButton: TButton
          Left = 127
          Top = 131
          Width = 64
          Height = 25
          Caption = 'Write'
          TabOrder = 3
          OnClick = E2PROMWriteButtonClick
        end
        object E2PROMReadButton: TButton
          Left = 40
          Top = 131
          Width = 64
          Height = 25
          Caption = 'Read'
          TabOrder = 4
          OnClick = E2PROMReadButtonClick
        end
      end
    end
    object PotentiometerTabSheet: TTabSheet
      Caption = 'Poteniometer'
      ImageIndex = 2
      object GroupBox39: TGroupBox
        Left = 40
        Top = 16
        Width = 357
        Height = 128
        TabOrder = 0
        object Label47: TLabel
          Left = 223
          Top = 22
          Width = 67
          Height = 13
          Caption = 'Select Device'
        end
        object Label6: TLabel
          Left = 94
          Top = 22
          Width = 23
          Height = 13
          Caption = 'Data'
        end
        object PotentiometerComboBox: TComboBox
          Left = 200
          Top = 38
          Width = 112
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          TabOrder = 0
          Items.Strings = (
            'Potentiometer 1 (S3)'
            'Potentiometer 2 (S2)'
            'Potentiometer 3 (S1)'
            'Potentiometer 4 (S0)'
            'Potentiometer 5 (M3)'
            'Potentiometer 6 (M2)'
            'Potentiometer 7 (M1)'
            'Potentiometer 8 (M0)')
        end
        object PotenmtrDataEdit: TEdit
          Left = 46
          Top = 38
          Width = 120
          Height = 21
          TabOrder = 1
          OnClick = DataEditClick
        end
        object PotenmtrWriteButton: TButton
          Left = 46
          Top = 81
          Width = 64
          Height = 25
          Caption = 'Write'
          TabOrder = 2
          OnClick = PotenmtrWriteButtonClick
        end
      end
    end
    object A2DTabSheet: TTabSheet
      Caption = 'A/D'
      ImageIndex = 3
      object GroupBox41: TGroupBox
        Left = 16
        Top = 16
        Width = 457
        Height = 409
        Caption = 'A/D Values'
        TabOrder = 0
        object Label19: TLabel
          Tag = 13
          Left = 220
          Top = 24
          Width = 122
          Height = 13
          Caption = 'In 13 (Model Liquid Level)'
        end
        object Label20: TLabel
          Tag = 14
          Left = 220
          Top = 56
          Width = 78
          Height = 13
          Caption = 'In 14 (Not Used)'
        end
        object Label21: TLabel
          Tag = 15
          Left = 220
          Top = 88
          Width = 130
          Height = 13
          Caption = 'In 15 (Support Liquid Level)'
        end
        object Label22: TLabel
          Tag = 16
          Left = 220
          Top = 120
          Width = 78
          Height = 13
          Caption = 'In 16 (Not Used)'
        end
        object Label23: TLabel
          Tag = 17
          Left = 220
          Top = 152
          Width = 78
          Height = 13
          Caption = 'In 17 (Not Used)'
        end
        object Label24: TLabel
          Tag = 18
          Left = 220
          Top = 184
          Width = 102
          Height = 13
          Caption = 'In 18 (Vacum Sensor)'
        end
        object Label25: TLabel
          Tag = 19
          Left = 220
          Top = 216
          Width = 78
          Height = 13
          Caption = 'In 19 (Not Used)'
        end
        object Label26: TLabel
          Tag = 20
          Left = 220
          Top = 248
          Width = 78
          Height = 13
          Caption = 'In 20 (Not Used)'
        end
        object Label59: TLabel
          Tag = 21
          Left = 220
          Top = 280
          Width = 52
          Height = 13
          Caption = 'In 21 (Vpp)'
        end
        object Label60: TLabel
          Tag = 22
          Left = 220
          Top = 312
          Width = 52
          Height = 13
          Caption = 'In 22 (24V)'
        end
        object Label61: TLabel
          Tag = 23
          Left = 220
          Top = 344
          Width = 56
          Height = 13
          Caption = 'In 23 (VDD)'
        end
        object Label62: TLabel
          Tag = 24
          Left = 220
          Top = 377
          Width = 54
          Height = 13
          Caption = 'In 24 (VCC)'
        end
        object Label8: TLabel
          Tag = 1
          Left = 16
          Top = 24
          Width = 40
          Height = 13
          Caption = 'In 1 (S3)'
        end
        object Label9: TLabel
          Tag = 2
          Left = 16
          Top = 56
          Width = 40
          Height = 13
          Caption = 'In 2 (S2)'
        end
        object Label5: TLabel
          Tag = 3
          Left = 16
          Top = 88
          Width = 40
          Height = 13
          Caption = 'In 3 (S1)'
        end
        object Label11: TLabel
          Tag = 4
          Left = 16
          Top = 120
          Width = 40
          Height = 13
          Caption = 'In 4 (S0)'
        end
        object Label10: TLabel
          Tag = 5
          Left = 16
          Top = 152
          Width = 42
          Height = 13
          Caption = 'In 5 (M3)'
        end
        object Label12: TLabel
          Tag = 6
          Left = 16
          Top = 184
          Width = 42
          Height = 13
          Caption = 'In 6 (M2)'
        end
        object Label13: TLabel
          Tag = 7
          Left = 16
          Top = 216
          Width = 42
          Height = 13
          Caption = 'In 7 (M1)'
        end
        object Label14: TLabel
          Tag = 8
          Left = 16
          Top = 248
          Width = 42
          Height = 13
          Caption = 'In 8 (M0)'
        end
        object Label15: TLabel
          Tag = 9
          Left = 16
          Top = 280
          Width = 91
          Height = 13
          Caption = 'In 9 (Support Front)'
        end
        object Label16: TLabel
          Tag = 10
          Left = 16
          Top = 312
          Width = 96
          Height = 13
          Caption = 'In 10 (Support Rear)'
        end
        object Label17: TLabel
          Tag = 11
          Left = 16
          Top = 377
          Width = 88
          Height = 13
          Caption = 'In 11 (Model Rear)'
        end
        object Label18: TLabel
          Tag = 12
          Left = 16
          Top = 344
          Width = 89
          Height = 13
          Caption = 'In 12 (Model Front)'
        end
        object A2DPanel24: TPanel
          Tag = 24
          Left = 364
          Top = 373
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 0
        end
        object A2DPanel23: TPanel
          Tag = 23
          Left = 364
          Top = 340
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 1
        end
        object A2DPanel22: TPanel
          Tag = 22
          Left = 364
          Top = 308
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 2
        end
        object A2DPanel21: TPanel
          Tag = 21
          Left = 364
          Top = 276
          Width = 68
          Height = 19
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 3
        end
        object A2DPanel20: TPanel
          Tag = 20
          Left = 364
          Top = 244
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 4
        end
        object A2DPanel19: TPanel
          Tag = 19
          Left = 364
          Top = 212
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 5
        end
        object A2DPanel18: TPanel
          Tag = 18
          Left = 364
          Top = 180
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 6
        end
        object A2DPanel17: TPanel
          Tag = 17
          Left = 364
          Top = 148
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 7
        end
        object A2DPanel16: TPanel
          Tag = 16
          Left = 364
          Top = 116
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 8
        end
        object A2DPanel15: TPanel
          Tag = 15
          Left = 364
          Top = 84
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 9
        end
        object A2DPanel14: TPanel
          Tag = 14
          Left = 364
          Top = 52
          Width = 68
          Height = 19
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 10
        end
        object A2DPanel13: TPanel
          Tag = 13
          Left = 364
          Top = 20
          Width = 68
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 11
        end
        object A2DPanel12: TPanel
          Tag = 12
          Left = 127
          Top = 340
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 12
        end
        object A2DPanel11: TPanel
          Tag = 11
          Left = 127
          Top = 373
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 13
        end
        object A2DPanel10: TPanel
          Tag = 10
          Left = 127
          Top = 308
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 14
        end
        object A2DPanel9: TPanel
          Tag = 9
          Left = 127
          Top = 276
          Width = 67
          Height = 19
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 15
        end
        object A2DPanel8: TPanel
          Tag = 8
          Left = 127
          Top = 244
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 16
        end
        object A2DPanel7: TPanel
          Tag = 7
          Left = 127
          Top = 212
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 17
        end
        object A2DPanel6: TPanel
          Tag = 6
          Left = 127
          Top = 180
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 18
        end
        object A2DPanel5: TPanel
          Tag = 5
          Left = 127
          Top = 148
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 19
        end
        object A2DPanel4: TPanel
          Tag = 4
          Left = 127
          Top = 116
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 20
        end
        object A2DPanel3: TPanel
          Tag = 3
          Left = 127
          Top = 84
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 21
        end
        object A2DPanel2: TPanel
          Tag = 2
          Left = 127
          Top = 52
          Width = 67
          Height = 19
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 22
        end
        object A2DPanel1: TPanel
          Tag = 1
          Left = 127
          Top = 20
          Width = 67
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 23
        end
      end
    end
    object HeatersTabSheet: TTabSheet
      Caption = 'Heaters'
      ImageIndex = 4
      object SetHeadsHeatersGroupBox: TGroupBox
        Left = 4
        Top = 11
        Width = 341
        Height = 518
        Caption = 'Set/Get Heaters Tempratures'
        TabOrder = 0
        object Label27: TLabel
          Left = 5
          Top = 36
          Width = 67
          Height = 13
          Caption = 'Model Head 0'
        end
        object Label28: TLabel
          Left = 5
          Top = 65
          Width = 67
          Height = 13
          Caption = 'Model Head 1'
        end
        object Label29: TLabel
          Left = 5
          Top = 94
          Width = 67
          Height = 13
          Caption = 'Model Head 2'
        end
        object Label30: TLabel
          Left = 5
          Top = 123
          Width = 67
          Height = 13
          Caption = 'Model Head 3'
        end
        object Label31: TLabel
          Left = 5
          Top = 152
          Width = 75
          Height = 13
          Caption = 'Support Head 0'
        end
        object Label32: TLabel
          Left = 5
          Top = 181
          Width = 75
          Height = 13
          Caption = 'Support Head 1'
        end
        object Label33: TLabel
          Left = 5
          Top = 211
          Width = 75
          Height = 13
          Caption = 'Support Head 2'
        end
        object Label34: TLabel
          Left = 5
          Top = 240
          Width = 75
          Height = 13
          Caption = 'Support Head 3'
        end
        object Label35: TLabel
          Left = 5
          Top = 327
          Width = 94
          Height = 13
          Caption = 'Support Block Front'
        end
        object Label36: TLabel
          Left = 5
          Top = 356
          Width = 93
          Height = 13
          Caption = 'Support Block Rear'
        end
        object Label37: TLabel
          Left = 5
          Top = 298
          Width = 85
          Height = 13
          Caption = 'Model Block Rear'
        end
        object Label38: TLabel
          Left = 5
          Top = 269
          Width = 86
          Height = 13
          Caption = 'Model Block Front'
        end
        object Label58: TLabel
          Left = 5
          Top = 386
          Width = 117
          Height = 13
          Caption = 'External Liquid Container'
        end
        object Label80: TLabel
          Left = 5
          Top = 416
          Width = 70
          Height = 13
          Caption = 'Low Threshold'
        end
        object Label81: TLabel
          Left = 5
          Top = 446
          Width = 72
          Height = 13
          Caption = 'High Threshold'
        end
        object Label82: TLabel
          Left = 251
          Top = 16
          Width = 69
          Height = 13
          Caption = 'Current Values'
        end
        object ModelHead0TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 32
          Width = 80
          Height = 21
          TabOrder = 0
        end
        object ModelHead1TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 61
          Width = 80
          Height = 21
          TabOrder = 1
        end
        object ModelHead2TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 90
          Width = 80
          Height = 21
          TabOrder = 2
        end
        object ModelHead3TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 119
          Width = 80
          Height = 21
          TabOrder = 3
        end
        object SupportHead0TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 148
          Width = 80
          Height = 21
          TabOrder = 4
        end
        object SupportHead1TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 177
          Width = 80
          Height = 21
          TabOrder = 5
        end
        object SupportHead2TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 207
          Width = 80
          Height = 21
          TabOrder = 6
        end
        object SupportHead3TempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 236
          Width = 80
          Height = 21
          TabOrder = 7
        end
        object SupportBlockFrontTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 323
          Width = 80
          Height = 21
          TabOrder = 10
        end
        object SupportBlockRearTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 352
          Width = 80
          Height = 21
          TabOrder = 11
        end
        object ModelBlockRearTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 294
          Width = 80
          Height = 21
          TabOrder = 9
        end
        object ModelBlockFrontTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 265
          Width = 80
          Height = 21
          TabOrder = 8
        end
        object SetHeatersTempButton: TButton
          Left = 129
          Top = 479
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 15
          OnClick = SetHeatersTempButtonClick
        end
        object ExternalLiquidTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 382
          Width = 80
          Height = 21
          TabOrder = 12
        end
        object LowThresholdTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 412
          Width = 80
          Height = 21
          TabOrder = 13
        end
        object HighThresholdTempEdit: TEdit
          Tag = -1
          Left = 126
          Top = 442
          Width = 80
          Height = 21
          TabOrder = 14
        end
        object GetModelHead0TempEdit: TEdit
          Tag = 7
          Left = 245
          Top = 32
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 16
        end
        object GetModelHead1TempEdit: TEdit
          Tag = 6
          Left = 245
          Top = 61
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 17
        end
        object GetModelHead2TempEdit: TEdit
          Tag = 5
          Left = 245
          Top = 90
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 18
        end
        object GetModelHead3TempEdit: TEdit
          Tag = 4
          Left = 245
          Top = 119
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 19
        end
        object GetSupportHead0TempEdit: TEdit
          Tag = 3
          Left = 245
          Top = 148
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 20
        end
        object GetSupportHead1TempEdit: TEdit
          Tag = 2
          Left = 245
          Top = 177
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 21
        end
        object GetSupportHead2TempEdit: TEdit
          Tag = 1
          Left = 245
          Top = 207
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 22
        end
        object GetSupportHead3TempEdit: TEdit
          Left = 245
          Top = 236
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 23
        end
        object GetModelBlockFrontTempEdit: TEdit
          Tag = 11
          Left = 245
          Top = 265
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 24
        end
        object GetModelBlockRearTempEdit: TEdit
          Tag = 10
          Left = 245
          Top = 294
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 25
        end
        object GetSupportBlockFrontTempEdit: TEdit
          Tag = 8
          Left = 245
          Top = 323
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 26
        end
        object GetSupportBlockRearTempEdit: TEdit
          Tag = 9
          Left = 245
          Top = 352
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 27
        end
        object GetExternalLiquidTempEdit: TEdit
          Tag = 12
          Left = 245
          Top = 382
          Width = 80
          Height = 21
          ReadOnly = True
          TabOrder = 28
        end
      end
      object GroupBox2: TGroupBox
        Left = 354
        Top = 447
        Width = 183
        Height = 82
        Caption = 'Is Head Temperature Ok'
        TabOrder = 1
        object Label39: TLabel
          Left = 90
          Top = 17
          Width = 87
          Height = 13
          Caption = 'Temprature Status'
        end
        object TemperatureStatusPanel: TPanel
          Left = 102
          Top = 39
          Width = 71
          Height = 25
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 0
        end
        object IsHeadTempOkButton: TButton
          Left = 12
          Top = 39
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 1
          OnClick = IsHeadTempOkButtonClick
        end
      end
      object GroupBox5: TGroupBox
        Left = 354
        Top = 11
        Width = 183
        Height = 431
        Caption = 'Set Heaters On Off'
        TabOrder = 2
        object TurnOffHeatersButton: TButton
          Left = 54
          Top = 396
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 0
          OnClick = TurnOffHeatersButtonClick
        end
        object GroupBox11: TGroupBox
          Left = 6
          Top = 16
          Width = 163
          Height = 81
          TabOrder = 1
          object SetHeatersOffRadioButton: TRadioButton
            Left = 8
            Top = 48
            Width = 57
            Height = 17
            Caption = 'Off'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object SetHeatersOnRadioButton: TRadioButton
            Left = 8
            Top = 16
            Width = 49
            Height = 17
            Caption = 'On'
            TabOrder = 1
          end
        end
        object GroupBox13: TGroupBox
          Left = 6
          Top = 103
          Width = 163
          Height = 283
          TabOrder = 2
          object Head1CheckBox: TCheckBox
            Left = 24
            Top = 156
            Width = 73
            Height = 17
            Caption = 'Support 3'
            Checked = True
            State = cbChecked
            TabOrder = 0
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head2CheckBox: TCheckBox
            Tag = 1
            Left = 24
            Top = 135
            Width = 73
            Height = 17
            Caption = 'Support 2'
            Checked = True
            State = cbChecked
            TabOrder = 1
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head3CheckBox: TCheckBox
            Tag = 2
            Left = 24
            Top = 114
            Width = 73
            Height = 17
            Caption = 'Support 1'
            Checked = True
            State = cbChecked
            TabOrder = 2
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head4CheckBox: TCheckBox
            Tag = 3
            Left = 24
            Top = 94
            Width = 73
            Height = 16
            Caption = 'Support 0'
            Checked = True
            State = cbChecked
            TabOrder = 3
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head5CheckBox: TCheckBox
            Tag = 4
            Left = 24
            Top = 73
            Width = 61
            Height = 16
            Caption = 'Model 3'
            Checked = True
            State = cbChecked
            TabOrder = 4
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head6CheckBox: TCheckBox
            Tag = 5
            Left = 24
            Top = 52
            Width = 61
            Height = 16
            Caption = 'Model 2'
            Checked = True
            State = cbChecked
            TabOrder = 5
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head7CheckBox: TCheckBox
            Tag = 6
            Left = 24
            Top = 31
            Width = 61
            Height = 17
            Caption = 'Model 1'
            Checked = True
            State = cbChecked
            TabOrder = 6
            OnClick = HeatersMaskCheckBoxClick
          end
          object Head8CheckBox: TCheckBox
            Tag = 7
            Left = 24
            Top = 11
            Width = 61
            Height = 17
            Caption = 'Model 0'
            Checked = True
            State = cbChecked
            TabOrder = 7
            OnClick = HeatersMaskCheckBoxClick
          end
          object BlockHead1CheckBox: TCheckBox
            Tag = 8
            Left = 24
            Top = 177
            Width = 88
            Height = 16
            Caption = 'Support Front'
            Checked = True
            State = cbChecked
            TabOrder = 8
            OnClick = HeatersMaskCheckBoxClick
          end
          object BlockHead2CheckBox: TCheckBox
            Tag = 9
            Left = 24
            Top = 197
            Width = 88
            Height = 18
            Caption = 'Support Rear'
            Checked = True
            State = cbChecked
            TabOrder = 9
            OnClick = HeatersMaskCheckBoxClick
          end
          object BlockHead3CheckBox: TCheckBox
            Tag = 10
            Left = 24
            Top = 218
            Width = 88
            Height = 17
            Caption = 'Model Rear'
            Checked = True
            State = cbChecked
            TabOrder = 10
            OnClick = HeatersMaskCheckBoxClick
          end
          object BlockHead4CheckBox: TCheckBox
            Tag = 11
            Left = 24
            Top = 239
            Width = 88
            Height = 17
            Caption = 'Model Front'
            Checked = True
            State = cbChecked
            TabOrder = 11
            OnClick = HeatersMaskCheckBoxClick
          end
          object ExternalLiquidCheckBox: TCheckBox
            Tag = 12
            Left = 24
            Top = 260
            Width = 86
            Height = 17
            Caption = 'External Liquid'
            Checked = True
            State = cbChecked
            TabOrder = 12
            OnClick = HeatersMaskCheckBoxClick
          end
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Heads Voltages'
      ImageIndex = 8
      object HeadsVoltagesGroupBox: TGroupBox
        Left = 13
        Top = 16
        Width = 436
        Height = 365
        Caption = 'Heads Voltages'
        TabOrder = 0
        object Label48: TLabel
          Left = 30
          Top = 296
          Width = 13
          Height = 13
          Caption = 'S3'
        end
        object Label49: TLabel
          Left = 30
          Top = 261
          Width = 13
          Height = 13
          Caption = 'S2'
        end
        object Label50: TLabel
          Left = 30
          Top = 227
          Width = 13
          Height = 13
          Caption = 'S1'
        end
        object Label51: TLabel
          Left = 30
          Top = 192
          Width = 13
          Height = 13
          Caption = 'S0'
        end
        object Label52: TLabel
          Left = 30
          Top = 158
          Width = 15
          Height = 13
          Caption = 'M3'
        end
        object Label53: TLabel
          Left = 30
          Top = 123
          Width = 15
          Height = 13
          Caption = 'M2'
        end
        object Label54: TLabel
          Left = 30
          Top = 88
          Width = 15
          Height = 13
          Caption = 'M1'
        end
        object Label55: TLabel
          Left = 30
          Top = 53
          Width = 15
          Height = 13
          Caption = 'M0'
        end
        object Label56: TLabel
          Left = 75
          Top = 22
          Width = 95
          Height = 13
          Caption = 'Potentiometer Value'
        end
        object Label57: TLabel
          Left = 324
          Top = 22
          Width = 73
          Height = 13
          Caption = 'Current Voltage'
        end
        object Label85: TLabel
          Left = 216
          Top = 22
          Width = 57
          Height = 13
          Caption = 'Current A/D'
        end
        object PotentiometerS3Edit: TEdit
          Left = 81
          Top = 292
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 7
          Text = '100'
        end
        object PotentiometerS2Edit: TEdit
          Tag = 1
          Left = 81
          Top = 257
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 6
          Text = '100'
        end
        object PotentiometerS1Edit: TEdit
          Tag = 2
          Left = 81
          Top = 223
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 5
          Text = '100'
        end
        object PotentiometerS0Edit: TEdit
          Tag = 3
          Left = 81
          Top = 188
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
          Text = '100'
        end
        object PotentiometerM3Edit: TEdit
          Tag = 4
          Left = 81
          Top = 154
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
          Text = '100'
        end
        object PotentiometerM2Edit: TEdit
          Tag = 5
          Left = 81
          Top = 119
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          Text = '100'
        end
        object PotentiometerM1Edit: TEdit
          Tag = 6
          Left = 81
          Top = 84
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '100'
        end
        object PotentiometerM0Edit: TEdit
          Tag = 7
          Left = 81
          Top = 50
          Width = 64
          Height = 21
          Hint = '(0 - 255)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          Text = '100'
        end
        object S3VoltageA2DPanel: TPanel
          Left = 212
          Top = 292
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 8
        end
        object S2VoltageA2DPanel: TPanel
          Tag = 1
          Left = 212
          Top = 257
          Width = 65
          Height = 22
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 9
        end
        object S1VoltageA2DPanel: TPanel
          Tag = 2
          Left = 212
          Top = 223
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 10
        end
        object S0VoltageA2DPanel: TPanel
          Tag = 3
          Left = 212
          Top = 188
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 11
        end
        object M3VoltageA2DPanel: TPanel
          Tag = 4
          Left = 212
          Top = 154
          Width = 65
          Height = 20
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 12
        end
        object M2VoltageA2DPanel: TPanel
          Tag = 5
          Left = 212
          Top = 119
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 13
        end
        object M1VoltageA2DPanel: TPanel
          Tag = 6
          Left = 212
          Top = 84
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 14
        end
        object M0VoltageA2DPanel: TPanel
          Tag = 7
          Left = 212
          Top = 50
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 15
        end
        object SetPotnmtrValuesButton: TButton
          Left = 81
          Top = 331
          Width = 64
          Height = 21
          Caption = 'Set Values'
          TabOrder = 16
          OnClick = SetPotnmtrValuesButtonClick
        end
        object PotentiometerM0UpDown: TUpDown
          Left = 145
          Top = 50
          Width = 15
          Height = 21
          Associate = PotentiometerM0Edit
          Max = 255
          Position = 100
          TabOrder = 17
        end
        object PotentiometerM1UpDown: TUpDown
          Left = 145
          Top = 84
          Width = 15
          Height = 21
          Associate = PotentiometerM1Edit
          Max = 255
          Position = 100
          TabOrder = 18
        end
        object PotentiometerM2UpDown: TUpDown
          Left = 145
          Top = 119
          Width = 15
          Height = 21
          Associate = PotentiometerM2Edit
          Max = 255
          Position = 100
          TabOrder = 19
        end
        object PotentiometerM3UpDown: TUpDown
          Left = 145
          Top = 188
          Width = 15
          Height = 21
          Associate = PotentiometerS0Edit
          Max = 255
          Position = 100
          TabOrder = 20
        end
        object PotentiometerS0UpDown: TUpDown
          Left = 145
          Top = 154
          Width = 15
          Height = 21
          Associate = PotentiometerM3Edit
          Max = 255
          Position = 100
          TabOrder = 21
        end
        object PotentiometerS1UpDown: TUpDown
          Left = 145
          Top = 223
          Width = 15
          Height = 21
          Associate = PotentiometerS1Edit
          Max = 255
          Position = 100
          TabOrder = 22
        end
        object PotentiometerS2UpDown: TUpDown
          Left = 145
          Top = 257
          Width = 15
          Height = 21
          Associate = PotentiometerS2Edit
          Max = 255
          Position = 100
          TabOrder = 23
        end
        object PotentiometerS3UpDown: TUpDown
          Left = 145
          Top = 292
          Width = 15
          Height = 21
          Associate = PotentiometerS3Edit
          Max = 255
          Position = 100
          TabOrder = 24
        end
        object M0VoltagePanel: TPanel
          Tag = 15
          Left = 328
          Top = 50
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 25
        end
        object M1VoltagePanel: TPanel
          Tag = 14
          Left = 328
          Top = 84
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 26
        end
        object M2VoltagePanel: TPanel
          Tag = 13
          Left = 328
          Top = 119
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 27
        end
        object M3VoltagePanel: TPanel
          Tag = 12
          Left = 328
          Top = 153
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 28
        end
        object S0VoltagePanel: TPanel
          Tag = 11
          Left = 328
          Top = 188
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 29
        end
        object S1VoltagePanel: TPanel
          Tag = 10
          Left = 328
          Top = 222
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 30
        end
        object S2VoltagePanel: TPanel
          Tag = 9
          Left = 328
          Top = 257
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 31
        end
        object S3VoltagePanel: TPanel
          Tag = 8
          Left = 328
          Top = 292
          Width = 65
          Height = 21
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 32
        end
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Heads Data'
      ImageIndex = 9
      object GroupBox14: TGroupBox
        Left = 13
        Top = 8
        Width = 353
        Height = 529
        Caption = 'Heads Data'
        TabOrder = 0
        object Label74: TLabel
          Left = 19
          Top = 23
          Width = 59
          Height = 13
          Caption = 'Select Head'
        end
        object SetHeadDataButton: TButton
          Left = 40
          Top = 494
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 0
          OnClick = SetHeadDataButtonClick
        end
        object GetHeadDataButton: TButton
          Left = 140
          Top = 494
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 1
          OnClick = GetHeadDataButtonClick
        end
        object HeadNumComboBox: TComboBox
          Left = 111
          Top = 19
          Width = 96
          Height = 21
          ItemHeight = 13
          TabOrder = 2
          Items.Strings = (
            'H1 odd'
            'H1 even'
            'H2 odd'
            'H2 even'
            'H3 odd'
            'H3 even'
            'H4 odd'
            'H4 even')
        end
        object GroupBox15: TGroupBox
          Left = 11
          Top = 170
          Width = 331
          Height = 143
          Caption = 'Weight/Voltage line equation'
          TabOrder = 3
          object Label64: TLabel
            Left = 9
            Top = 35
            Width = 64
            Height = 13
            Caption = 'Model 10KHz'
          end
          object Label65: TLabel
            Left = 9
            Top = 63
            Width = 64
            Height = 13
            Caption = 'Model 20KHz'
          end
          object Label66: TLabel
            Left = 9
            Top = 90
            Width = 72
            Height = 13
            Caption = 'Support 10KHz'
          end
          object Label67: TLabel
            Left = 9
            Top = 118
            Width = 72
            Height = 13
            Caption = 'Support 20KHz'
          end
          object Label68: TLabel
            Left = 145
            Top = 16
            Width = 7
            Height = 13
            Caption = 'A'
          end
          object Label69: TLabel
            Left = 263
            Top = 16
            Width = 7
            Height = 13
            Caption = 'B'
          end
          object Support10KhzAEdit: TEdit
            Left = 100
            Top = 86
            Width = 96
            Height = 21
            TabOrder = 4
            OnClick = DataEditClick
          end
          object Model20KhzAEdit: TEdit
            Left = 100
            Top = 59
            Width = 96
            Height = 21
            TabOrder = 2
            OnClick = DataEditClick
          end
          object Support20KhzAEdit: TEdit
            Left = 100
            Top = 114
            Width = 96
            Height = 21
            TabOrder = 6
            OnClick = DataEditClick
          end
          object Support20KhzBEdit: TEdit
            Left = 219
            Top = 114
            Width = 96
            Height = 21
            TabOrder = 7
            OnClick = DataEditClick
          end
          object Support10KhzBEdit: TEdit
            Left = 219
            Top = 86
            Width = 96
            Height = 21
            TabOrder = 5
            OnClick = DataEditClick
          end
          object Model10KhzAEdit: TEdit
            Left = 100
            Top = 31
            Width = 96
            Height = 21
            TabOrder = 0
            OnClick = DataEditClick
          end
          object Model20KhzBEdit: TEdit
            Left = 219
            Top = 59
            Width = 96
            Height = 21
            TabOrder = 3
            OnClick = DataEditClick
          end
          object Model10KhzBEdit: TEdit
            Left = 219
            Top = 31
            Width = 96
            Height = 21
            TabOrder = 1
            OnClick = DataEditClick
          end
        end
        object GroupBox16: TGroupBox
          Left = 11
          Top = 45
          Width = 331
          Height = 123
          Caption = 'General Info'
          TabOrder = 4
          object Label70: TLabel
            Left = 9
            Top = 61
            Width = 77
            Height = 13
            Caption = 'Production Date'
          end
          object Label73: TLabel
            Left = 9
            Top = 92
            Width = 38
            Height = 13
            Caption = 'X Offset'
          end
          object Label63: TLabel
            Left = 9
            Top = 29
            Width = 66
            Height = 13
            Caption = 'Serial Number'
          end
          object XOffsetEdit: TEdit
            Left = 100
            Top = 88
            Width = 96
            Height = 21
            TabOrder = 0
            OnClick = DataEditClick
          end
          object ProductionDateTimePicker: TDateTimePicker
            Left = 100
            Top = 57
            Width = 96
            Height = 21
            Date = 37768.540918217600000000
            Time = 37768.540918217600000000
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 1
          end
          object SerialNumberEdit: TEdit
            Left = 100
            Top = 25
            Width = 96
            Height = 21
            MaxLength = 8
            TabOrder = 2
            OnClick = DataEditClick
          end
        end
        object GroupBox17: TGroupBox
          Left = 11
          Top = 317
          Width = 331
          Height = 88
          Caption = 'Temperature'
          TabOrder = 5
          object Label71: TLabel
            Left = 9
            Top = 32
            Width = 12
            Height = 13
            Caption = '60'
          end
          object Label72: TLabel
            Left = 22
            Top = 26
            Width = 6
            Height = 13
            Caption = 'o'
          end
          object Label75: TLabel
            Left = 30
            Top = 32
            Width = 7
            Height = 13
            Caption = 'C'
          end
          object Label76: TLabel
            Left = 9
            Top = 59
            Width = 12
            Height = 13
            Caption = '80'
          end
          object Label77: TLabel
            Left = 22
            Top = 53
            Width = 6
            Height = 13
            Caption = 'o'
          end
          object Label78: TLabel
            Left = 30
            Top = 59
            Width = 7
            Height = 13
            Caption = 'C'
          end
          object Label79: TLabel
            Left = 127
            Top = 11
            Width = 20
            Height = 13
            Caption = 'A/D'
          end
          object A2DValuefor80CEdit: TEdit
            Left = 100
            Top = 55
            Width = 96
            Height = 21
            TabOrder = 1
            OnClick = DataEditClick
          end
          object A2DValuefor60CEdit: TEdit
            Left = 100
            Top = 28
            Width = 96
            Height = 21
            TabOrder = 0
            OnClick = DataEditClick
          end
        end
        object GroupBox40: TGroupBox
          Left = 11
          Top = 411
          Width = 331
          Height = 73
          Caption = 'Validation'
          TabOrder = 6
          object HeadsDataCheckSumEdit: TLabeledEdit
            Left = 51
            Top = 35
            Width = 96
            Height = 21
            EditLabel.Width = 50
            EditLabel.Height = 13
            EditLabel.Caption = 'Checksum'
            ReadOnly = True
            TabOrder = 0
          end
          object HeadsDataValidityEdit: TLabeledEdit
            Left = 184
            Top = 35
            Width = 97
            Height = 21
            EditLabel.Width = 33
            EditLabel.Height = 13
            EditLabel.Caption = 'Validity'
            ReadOnly = True
            TabOrder = 1
          end
        end
        object VerifyHeadDataButton: TButton
          Left = 240
          Top = 494
          Width = 74
          Height = 25
          Caption = 'Verify'
          TabOrder = 7
          OnClick = VerifyHeadDataButtonClick
        end
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'Roller'
      ImageIndex = 5
      object GroupBox6: TGroupBox
        Left = 8
        Top = 8
        Width = 183
        Height = 104
        Caption = 'Set Roller On Off'
        TabOrder = 0
        object SetRollerOnOffButton: TButton
          Left = 55
          Top = 71
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 0
          OnClick = SetRollerOnOffButtonClick
        end
        object RollerOffRadioButton: TRadioButton
          Left = 16
          Top = 24
          Width = 112
          Height = 17
          Caption = 'Off'
          Checked = True
          TabOrder = 1
          TabStop = True
        end
        object RollerOnRadioButton: TRadioButton
          Left = 16
          Top = 48
          Width = 112
          Height = 17
          Caption = 'On'
          TabOrder = 2
        end
      end
      object GroupBox7: TGroupBox
        Left = 8
        Top = 135
        Width = 183
        Height = 104
        Caption = 'Set Roller Speed'
        TabOrder = 1
        object Label40: TLabel
          Left = 16
          Top = 36
          Width = 31
          Height = 13
          Caption = 'Speed'
        end
        object setRollerSpeedButton: TButton
          Left = 55
          Top = 71
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 0
          OnClick = setRollerSpeedButtonClick
        end
        object RollerSpeedEdit: TEdit
          Left = 63
          Top = 32
          Width = 73
          Height = 21
          TabOrder = 1
        end
      end
      object GroupBox8: TGroupBox
        Left = 8
        Top = 261
        Width = 183
        Height = 128
        Caption = 'Get Roller Status '
        TabOrder = 2
        object Label41: TLabel
          Left = 16
          Top = 61
          Width = 31
          Height = 13
          Caption = 'Speed'
        end
        object Label42: TLabel
          Left = 16
          Top = 29
          Width = 30
          Height = 13
          Caption = 'Status'
        end
        object GetRollerStatusButton: TButton
          Left = 55
          Top = 95
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 0
          OnClick = GetRollerStatusButtonClick
        end
        object RollerStatusPanel: TPanel
          Left = 63
          Top = 24
          Width = 81
          Height = 22
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 1
        end
        object RollerSpeedPanel: TPanel
          Left = 63
          Top = 55
          Width = 81
          Height = 22
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 2
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Loopback Test'
      ImageIndex = 6
      object LoopbackLoadButton: TButton
        Left = 29
        Top = 99
        Width = 74
        Height = 25
        Caption = 'Load File'
        Enabled = False
        TabOrder = 0
        OnClick = LoopbackLoadButtonClick
      end
      object LoopbackGoBitBtn: TBitBtn
        Left = 29
        Top = 143
        Width = 74
        Height = 25
        Caption = 'Go'
        Enabled = False
        TabOrder = 1
        OnClick = LoopbackGoBitBtnClick
        Kind = bkRetry
      end
      object LoopbackStopBitBtn: TBitBtn
        Left = 29
        Top = 184
        Width = 74
        Height = 25
        Caption = 'Stop'
        Enabled = False
        TabOrder = 2
        OnClick = LoopbackStopBitBtnClick
        Kind = bkCancel
      end
      object ContinuousCheckBox: TCheckBox
        Left = 29
        Top = 32
        Width = 96
        Height = 17
        Caption = 'Continuous test'
        TabOrder = 3
      end
      object EnableOHDBCheckBox: TCheckBox
        Left = 29
        Top = 61
        Width = 159
        Height = 18
        Caption = 'Enable OHDB Loopback test'
        TabOrder = 4
      end
      object GroupBox9: TGroupBox
        Left = 29
        Top = 263
        Width = 270
        Height = 95
        Caption = 'FIFO Status'
        TabOrder = 5
        object Label43: TLabel
          Left = 20
          Top = 41
          Width = 29
          Height = 13
          Caption = 'Empty'
        end
        object Label44: TLabel
          Left = 223
          Top = 43
          Width = 16
          Height = 13
          Caption = 'Full'
        end
        object FIFOStatusProgressBar: TProgressBar
          Left = 55
          Top = 41
          Width = 160
          Height = 16
          TabOrder = 0
        end
      end
    end
    object PrintingTabSheet: TTabSheet
      Caption = 'Printing'
      ImageIndex = 11
      object GroupBox28: TGroupBox
        Left = 16
        Top = 5
        Width = 313
        Height = 207
        Caption = 'Printing Pass Parameters'
        TabOrder = 0
        object GroupBox29: TGroupBox
          Left = 16
          Top = 16
          Width = 128
          Height = 144
          Caption = 'Printing'
          TabOrder = 0
          object StartPegParamEdit: TLabeledEdit
            Left = 37
            Top = 32
            Width = 53
            Height = 21
            Hint = '(0 - 30,000)'
            EditLabel.Width = 44
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 30,000)'
            EditLabel.Caption = 'Start Peg'
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
          end
          object EndPegParamEdit: TLabeledEdit
            Left = 37
            Top = 71
            Width = 53
            Height = 21
            Hint = '(0 - 30,000)'
            EditLabel.Width = 41
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 30,000)'
            EditLabel.Caption = 'End Peg'
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
          object NumberOfFireParamEdit: TLabeledEdit
            Left = 37
            Top = 111
            Width = 53
            Height = 21
            Hint = '(0 - 30,000)'
            EditLabel.Width = 79
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 30,000)'
            EditLabel.Caption = 'Number Of Fires '
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 2
          end
        end
        object GroupBox30: TGroupBox
          Left = 166
          Top = 16
          Width = 127
          Height = 144
          Caption = 'Bumper'
          TabOrder = 1
          object BumperStartPegParamEdit: TLabeledEdit
            Left = 38
            Top = 42
            Width = 53
            Height = 21
            Hint = '(0 - 30,000)'
            EditLabel.Width = 44
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 30,000)'
            EditLabel.Caption = 'Start Peg'
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
          end
          object BumperEndPegParamEdit: TLabeledEdit
            Left = 37
            Top = 91
            Width = 53
            Height = 21
            Hint = '(0 - 30,000)'
            EditLabel.Width = 41
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 30,000)'
            EditLabel.Caption = 'End Peg'
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
        end
        object SetPrintingPassParametersButton: TButton
          Left = 119
          Top = 174
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 2
          OnClick = SetPrintingPassParametersButtonClick
        end
      end
      object GroupBox31: TGroupBox
        Left = 202
        Top = 384
        Width = 128
        Height = 107
        Caption = 'Printing Circuit'
        TabOrder = 3
        object EnablePrintingCircuitButton: TButton
          Left = 27
          Top = 75
          Width = 74
          Height = 24
          Caption = 'Set'
          TabOrder = 0
          OnClick = EnablePrintingCircuitButtonClick
        end
        object EnablePrintingCircuitRadioButton: TRadioButton
          Left = 8
          Top = 23
          Width = 88
          Height = 17
          Caption = 'Enable Circuit'
          Checked = True
          TabOrder = 1
          TabStop = True
        end
        object DisablePrintingCircuitRadioButton: TRadioButton
          Left = 8
          Top = 47
          Width = 96
          Height = 16
          Caption = 'Disable Circuit'
          TabOrder = 2
        end
      end
      object GroupBox32: TGroupBox
        Left = 16
        Top = 387
        Width = 183
        Height = 104
        TabOrder = 2
        object ResetDriverCircuitButton: TButton
          Left = 55
          Top = 75
          Width = 74
          Height = 24
          Caption = 'Apply'
          TabOrder = 0
          OnClick = ResetDriverCircuitButtonClick
        end
        object ResetDriverCircuitCheckBox: TCheckBox
          Left = 8
          Top = 12
          Width = 136
          Height = 17
          Caption = 'Reset Driver Circuit'
          TabOrder = 1
        end
        object ResetDriverStateMachineCheckBox: TCheckBox
          Left = 8
          Top = 34
          Width = 152
          Height = 17
          Caption = 'Reset Driver State Machine'
          TabOrder = 2
        end
        object DefaultPrintParamsCheckBox: TCheckBox
          Left = 8
          Top = 55
          Width = 160
          Height = 18
          Caption = 'Default Print Params'
          TabOrder = 3
        end
      end
      object GroupBox33: TGroupBox
        Left = 16
        Top = 219
        Width = 313
        Height = 159
        Caption = 'Fire All'
        TabOrder = 1
        object GroupBox34: TGroupBox
          Left = 16
          Top = 16
          Width = 128
          Height = 136
          Caption = 'Perform Fire All'
          TabOrder = 0
          object FireAllButton: TButton
            Left = 27
            Top = 103
            Width = 74
            Height = 25
            Caption = 'Fire All'
            TabOrder = 2
            OnClick = FireAllButtonClick
          end
          object FireAllFrequencyEdit: TLabeledEdit
            Left = 27
            Top = 32
            Width = 74
            Height = 21
            EditLabel.Width = 72
            EditLabel.Height = 13
            EditLabel.Caption = 'Frequency (Hz)'
            TabOrder = 0
          end
          object FireAllTimeEdit: TLabeledEdit
            Left = 27
            Top = 71
            Width = 74
            Height = 21
            EditLabel.Width = 59
            EditLabel.Height = 13
            EditLabel.Caption = 'Time (mSec)'
            TabOrder = 1
          end
        end
        object GroupBox35: TGroupBox
          Left = 166
          Top = 16
          Width = 127
          Height = 136
          Caption = 'Fire All Status'
          TabOrder = 1
          object GetFireAllStatusButton: TButton
            Left = 27
            Top = 103
            Width = 74
            Height = 25
            Caption = 'Fire All Status'
            TabOrder = 2
            OnClick = GetFireAllStatusButtonClick
          end
          object FireAllStatusEdit: TLabeledEdit
            Left = 27
            Top = 32
            Width = 74
            Height = 21
            EditLabel.Width = 64
            EditLabel.Height = 13
            EditLabel.Caption = 'Fire All Status'
            ReadOnly = True
            TabOrder = 0
          end
          object FireAllCounterEdit: TLabeledEdit
            Left = 27
            Top = 71
            Width = 74
            Height = 21
            EditLabel.Width = 71
            EditLabel.Height = 13
            EditLabel.Caption = 'Fire All Counter'
            ReadOnly = True
            TabOrder = 1
          end
        end
      end
      object EncoderGroupbox: TGroupBox
        Left = 16
        Top = 494
        Width = 314
        Height = 50
        Caption = 'Encoder position'
        TabOrder = 4
        object EncoderValueEdit: TEdit
          Left = 43
          Top = 17
          Width = 74
          Height = 21
          TabOrder = 0
        end
        object EncoderReadButton: TButton
          Left = 213
          Top = 15
          Width = 75
          Height = 25
          Caption = 'Read'
          TabOrder = 1
          OnClick = EncoderReadButtonClick
        end
      end
    end
    object TabSheet8: TTabSheet
      Caption = 'Bumper'
      ImageIndex = 12
      object GroupBox18: TGroupBox
        Left = 238
        Top = 24
        Width = 151
        Height = 96
        Caption = 'Bumper Activity'
        TabOrder = 1
        object SetBumperOnRadioButton: TRadioButton
          Left = 8
          Top = 19
          Width = 65
          Height = 17
          Caption = 'On'
          Checked = True
          TabOrder = 0
          TabStop = True
        end
        object SetBumperOffRadioButton: TRadioButton
          Left = 8
          Top = 42
          Width = 49
          Height = 17
          Caption = 'Off'
          TabOrder = 1
        end
        object SetBumperOnOffButton: TButton
          Left = 39
          Top = 63
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 2
          OnClick = SetBumperOnOffButtonClick
        end
      end
      object GroupBox36: TGroupBox
        Left = 48
        Top = 24
        Width = 151
        Height = 193
        Caption = 'Bumper Parameters'
        TabOrder = 0
        object SetBumperSensitivityButton: TButton
          Left = 39
          Top = 160
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 3
          OnClick = SetBumperSensitivityButtonClick
        end
        object SetBumperSensitivityEdit: TLabeledEdit
          Left = 39
          Top = 30
          Width = 74
          Height = 21
          Hint = '0 (very sensitive) - 4095 (less sensitive)'
          EditLabel.Width = 47
          EditLabel.Height = 13
          EditLabel.Caption = 'Sensitivity'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
        end
        object SetBumperResetTimeEdit: TLabeledEdit
          Left = 39
          Top = 75
          Width = 74
          Height = 21
          EditLabel.Width = 90
          EditLabel.Height = 13
          EditLabel.Caption = 'Reset Time (mSec)'
          TabOrder = 1
        end
        object SetBumperImpactCounterEdit: TLabeledEdit
          Left = 39
          Top = 121
          Width = 74
          Height = 21
          EditLabel.Width = 72
          EditLabel.Height = 13
          EditLabel.Caption = 'Impcat Counter'
          TabOrder = 2
        end
      end
      object GroupBox37: TGroupBox
        Left = 48
        Top = 246
        Width = 151
        Height = 96
        Caption = 'Bumper Status'
        TabOrder = 3
        object BumperOnOffLedImage: TImage
          Left = 111
          Top = 16
          Width = 25
          Height = 25
          Transparent = True
        end
        object BumperStatusLabel: TLabel
          Left = 17
          Top = 17
          Width = 30
          Height = 13
          Caption = 'Status'
        end
        object BumperStatusResetButton: TButton
          Left = 39
          Top = 63
          Width = 74
          Height = 25
          Caption = 'Reset'
          Enabled = False
          TabOrder = 0
          OnClick = BumperStatusResetButtonClick
        end
      end
      object BumperPeg: TGroupBox
        Left = 238
        Top = 121
        Width = 151
        Height = 96
        Caption = 'Bumper Peg On/Off'
        TabOrder = 2
        object SetBumperPegOnRadioButton: TRadioButton
          Left = 8
          Top = 19
          Width = 65
          Height = 17
          Caption = 'On'
          Checked = True
          TabOrder = 0
          TabStop = True
        end
        object SetBumperPegOffRadioButton: TRadioButton
          Left = 8
          Top = 42
          Width = 49
          Height = 17
          Caption = 'Off'
          TabOrder = 1
        end
        object SetBumperPegOnOffButton: TButton
          Left = 39
          Top = 63
          Width = 74
          Height = 25
          Caption = 'Set'
          TabOrder = 2
          OnClick = SetBumperPegOnOffButtonClick
        end
      end
    end
    object GeneralTabSheet: TTabSheet
      Caption = 'General'
      ImageIndex = 10
      object GroupBox21: TGroupBox
        Left = 8
        Top = 197
        Width = 278
        Height = 309
        Caption = 'Vacuum'
        TabOrder = 1
        object GroupBox22: TGroupBox
          Left = 8
          Top = 16
          Width = 128
          Height = 281
          Caption = 'Vacuum Parameters'
          TabOrder = 0
          object SetVacuumModelHighValueEdit: TLabeledEdit
            Left = 27
            Top = 46
            Width = 74
            Height = 21
            Hint = '(0 - 4095)'
            EditLabel.Width = 78
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 4095)'
            EditLabel.Caption = 'Model High Limit'
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
          end
          object SetVacuumModelLowValueEdit: TLabeledEdit
            Left = 27
            Top = 105
            Width = 74
            Height = 21
            Hint = '(0 - 4095)'
            EditLabel.Width = 76
            EditLabel.Height = 13
            EditLabel.Hint = '(0 - 4095)'
            EditLabel.Caption = 'Model Low Limit'
            EditLabel.ParentShowHint = False
            EditLabel.ShowHint = True
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
          object SetVacuumModelValueButton: TButton
            Left = 27
            Top = 220
            Width = 74
            Height = 25
            Caption = 'Set'
            TabOrder = 2
            OnClick = SetVacuumModelValueButtonClick
          end
        end
        object GroupBox23: TGroupBox
          Left = 143
          Top = 16
          Width = 128
          Height = 130
          Caption = 'Vacuum Status'
          TabOrder = 1
          object GetVacuumStatusButton: TButton
            Left = 27
            Top = 79
            Width = 74
            Height = 25
            Caption = 'Get Status'
            TabOrder = 0
            OnClick = GetVacuumStatusButtonClick
          end
          object GetVacuumStatusEdit: TLabeledEdit
            Left = 27
            Top = 32
            Width = 74
            Height = 21
            EditLabel.Width = 72
            EditLabel.Height = 13
            EditLabel.Caption = 'Vacuum Status'
            ReadOnly = True
            TabOrder = 1
          end
        end
        object GroupBox24: TGroupBox
          Left = 143
          Top = 152
          Width = 128
          Height = 145
          Caption = 'Vacum Sensor'
          TabOrder = 2
          object GetVacuumSensorStatusButton: TButton
            Left = 27
            Top = 83
            Width = 74
            Height = 25
            Caption = 'Get'
            TabOrder = 0
            OnClick = GetVacuumSensorStatusButtonClick
          end
          object GetVacuumSensorStatusEdit: TEdit
            Left = 27
            Top = 36
            Width = 74
            Height = 21
            ReadOnly = True
            TabOrder = 1
          end
        end
      end
      object GroupBox25: TGroupBox
        Left = 8
        Top = 16
        Width = 278
        Height = 175
        Caption = 'Material'
        TabOrder = 0
        object GroupBox19: TGroupBox
          AlignWithMargins = True
          Left = 5
          Top = 18
          Width = 128
          Height = 152
          Align = alLeft
          Caption = 'Material Level Sensors'
          TabOrder = 0
          object Model: TLabel
            Left = 50
            Top = 22
            Width = 29
            Height = 13
            Caption = 'Model'
          end
          object Edit80: TLabel
            Left = 46
            Top = 69
            Width = 37
            Height = 13
            Caption = 'Support'
          end
          object GetMaterialLevelStatusButton: TButton
            Left = 27
            Top = 115
            Width = 74
            Height = 25
            Caption = 'Get Status'
            TabOrder = 2
            OnClick = GetMaterialLevelStatusButtonClick
          end
          object GetSupportMaterialStatusLevelEdit: TEdit
            Left = 24
            Top = 83
            Width = 80
            Height = 21
            ReadOnly = True
            TabOrder = 1
          end
          object GetModelMaterialStatusLevelEdit: TEdit
            Left = 24
            Top = 36
            Width = 80
            Height = 21
            ReadOnly = True
            TabOrder = 0
          end
        end
        object GroupBox20: TGroupBox
          AlignWithMargins = True
          Left = 139
          Top = 18
          Width = 134
          Height = 152
          Align = alClient
          Caption = 'Material Cooling Fan'
          TabOrder = 1
          object SetMaterialCoolingFanOnBtn: TRadioButton
            Left = 13
            Top = 48
            Width = 64
            Height = 17
            Caption = 'On'
            TabOrder = 1
            OnClick = SetMaterialCoolingFanOnBtnClick
          end
          object SetMaterialCoolingFanOffBtn: TRadioButton
            Left = 13
            Top = 21
            Width = 40
            Height = 17
            Caption = 'Off'
            Checked = True
            TabOrder = 0
            TabStop = True
            OnClick = SetMaterialCoolingFanOffBtnClick
          end
          object SetMaterialCoolingFanButton: TButton
            Left = 27
            Top = 115
            Width = 74
            Height = 25
            Caption = 'Set'
            TabOrder = 2
            OnClick = SetMaterialCoolingFanButtonClick
          end
          object SetMaterialCoolingFanEdit: TLabeledEdit
            Left = 18
            Top = 83
            Width = 74
            Height = 21
            EditLabel.Width = 68
            EditLabel.Height = 13
            EditLabel.Caption = 'Duty Cycle (%)'
            Enabled = False
            ReadOnly = True
            TabOrder = 3
            Text = '50'
          end
          object SetMaterialCoolingFanUpDown: TUpDown
            Left = 92
            Top = 83
            Width = 15
            Height = 21
            Associate = SetMaterialCoolingFanEdit
            Position = 50
            TabOrder = 4
          end
        end
      end
      object GroupBox26: TGroupBox
        Left = 293
        Top = 16
        Width = 236
        Height = 175
        Caption = 'Ambient temperature'
        TabOrder = 2
        object GetAmbientTemperatureButton: TButton
          Left = 79
          Top = 130
          Width = 74
          Height = 24
          Caption = 'Get'
          TabOrder = 0
          OnClick = GetAmbientTemperatureButtonClick
        end
        object GetAmbientTemperatureA2DEdit: TLabeledEdit
          Left = 79
          Top = 33
          Width = 74
          Height = 21
          EditLabel.Width = 20
          EditLabel.Height = 13
          EditLabel.Caption = 'A/D'
          ReadOnly = True
          TabOrder = 1
        end
        object GetAmbientTemperatureEdit: TLabeledEdit
          Left = 79
          Top = 77
          Width = 74
          Height = 21
          EditLabel.Width = 11
          EditLabel.Height = 13
          EditLabel.Caption = #176'C'
          MaxLength = 4
          ReadOnly = True
          TabOrder = 2
        end
      end
      object GroupBox27: TGroupBox
        Left = 293
        Top = 197
        Width = 236
        Height = 309
        Caption = 'Power Supplies Voltages'
        TabOrder = 3
        object Label83: TLabel
          Left = 86
          Top = 19
          Width = 20
          Height = 13
          Caption = 'A/D'
        end
        object Label84: TLabel
          Left = 180
          Top = 19
          Width = 7
          Height = 13
          Caption = 'V'
        end
        object GetPowerSuppliesVoltagesButton: TButton
          Left = 93
          Top = 275
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 0
          OnClick = GetPowerSuppliesVoltagesButtonClick
        end
        object VppPowerSupplyEdit: TLabeledEdit
          Left = 59
          Top = 37
          Width = 74
          Height = 21
          EditLabel.Width = 50
          EditLabel.Height = 13
          EditLabel.BiDiMode = bdLeftToRight
          EditLabel.Caption = '40V (Vpp) '
          EditLabel.Color = clBtnFace
          EditLabel.ParentBiDiMode = False
          EditLabel.ParentColor = False
          LabelPosition = lpLeft
          ReadOnly = True
          TabOrder = 1
        end
        object Heater24VPowerSupplyEdit: TLabeledEdit
          Left = 59
          Top = 76
          Width = 74
          Height = 21
          EditLabel.Width = 22
          EditLabel.Height = 13
          EditLabel.Caption = '24V '
          LabelPosition = lpLeft
          ReadOnly = True
          TabOrder = 2
        end
        object VDDPowerSupplyEdit: TLabeledEdit
          Left = 59
          Top = 115
          Width = 74
          Height = 21
          EditLabel.Width = 50
          EditLabel.Height = 13
          EditLabel.Caption = '12V (Vdd) '
          LabelPosition = lpLeft
          ReadOnly = True
          TabOrder = 3
        end
        object VCCPowerSupplyEdit: TLabeledEdit
          Left = 59
          Top = 154
          Width = 74
          Height = 21
          EditLabel.Width = 44
          EditLabel.Height = 13
          EditLabel.Caption = '5V (Vcc) '
          LabelPosition = lpLeft
          ReadOnly = True
          TabOrder = 4
        end
        object VppPowerSupplyCEdit: TEdit
          Left = 148
          Top = 37
          Width = 74
          Height = 21
          TabOrder = 5
        end
        object Heater24VPowerSupplyCEdit: TEdit
          Left = 148
          Top = 76
          Width = 74
          Height = 21
          TabOrder = 6
        end
        object VDDPowerSupplyCEdit: TEdit
          Left = 148
          Top = 115
          Width = 74
          Height = 21
          TabOrder = 7
        end
        object VCCPowerSupplyCEdit: TEdit
          Left = 148
          Top = 154
          Width = 74
          Height = 21
          TabOrder = 8
        end
        object V_3_3_PowerSupplyEdit: TLabeledEdit
          Left = 59
          Top = 193
          Width = 74
          Height = 21
          EditLabel.Width = 25
          EditLabel.Height = 13
          EditLabel.Caption = '3.3 V'
          LabelPosition = lpLeft
          ReadOnly = True
          TabOrder = 9
        end
        object V_1_2_PowerSupplyEdit: TLabeledEdit
          Left = 59
          Top = 232
          Width = 74
          Height = 21
          EditLabel.Width = 25
          EditLabel.Height = 13
          EditLabel.Caption = '1.2 V'
          LabelPosition = lpLeft
          ReadOnly = True
          TabOrder = 10
        end
        object V_3_3_PowerSupplyCEdit: TEdit
          Left = 148
          Top = 193
          Width = 74
          Height = 21
          TabOrder = 11
        end
        object V_1_2_PowerSupplyCEdit: TEdit
          Left = 148
          Top = 232
          Width = 74
          Height = 21
          TabOrder = 12
        end
      end
      object SoftwareResetButton: TButton
        Left = 228
        Top = 512
        Width = 89
        Height = 26
        Caption = 'Software Reset'
        TabOrder = 4
        OnClick = SoftwareResetButtonClick
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Version'
      ImageIndex = 7
      object GroupBox10: TGroupBox
        AlignWithMargins = True
        Left = 5
        Top = 3
        Width = 535
        Height = 137
        Margins.Left = 5
        Margins.Right = 5
        Margins.Bottom = 5
        Align = alTop
        Caption = 'OHDB software versions'
        TabOrder = 0
        object Label45: TLabel
          Left = 35
          Top = 24
          Width = 48
          Height = 13
          Caption = 'Main CPU'
        end
        object Label46: TLabel
          Left = 259
          Top = 24
          Width = 28
          Height = 13
          Caption = 'FPGA'
        end
        object Label86: TLabel
          Left = 147
          Top = 24
          Width = 40
          Height = 13
          Caption = 'AD CPU'
        end
        object SoftwareVersionPanel: TPanel
          Left = 35
          Top = 45
          Width = 81
          Height = 24
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 0
        end
        object GetSoftwareVersionButton: TButton
          Left = 35
          Top = 87
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 1
          OnClick = GetSoftwareVersionButtonClick
        end
        object HardwareVersionPanel: TPanel
          Left = 259
          Top = 45
          Width = 81
          Height = 24
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 2
        end
        object GetHardwareVersionButton: TButton
          Left = 259
          Top = 87
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 3
          OnClick = GetHardwareVersionButtonClick
        end
        object ADVersionPanel: TPanel
          Left = 147
          Top = 45
          Width = 81
          Height = 24
          BevelOuter = bvLowered
          Caption = '???'
          TabOrder = 4
        end
        object GetADVersionButton: TButton
          Left = 147
          Top = 87
          Width = 74
          Height = 25
          Caption = 'Get'
          TabOrder = 5
          OnClick = GetADVersionButtonClick
        end
      end
    end
  end
  object MacroOpenDialog: TOpenDialog
    Filter = 'Macro files|*.mcr|All files|*.*'
    Left = 4
    Top = 600
  end
  object MacroSaveDialog: TSaveDialog
    DefaultExt = '*.mcr'
    Filter = 'Macro files|*.mcr|All files|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 44
    Top = 600
  end
  object LoopbackOpenDialog: TOpenDialog
    Left = 80
    Top = 600
  end
  object FIFOStatusTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = FIFOStatusTimerTimer
    Left = 120
    Top = 600
  end
  object BumperLedsImageList: TImageList
    Left = 160
    Top = 600
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
      00000000000000000000000000005C5C5C005C5C5C005C5C5C00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000191792001917920019179200000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000001580B0001580B0001580B00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000005C5C5C005C5C5C005C5C5C00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000434343006A6A6A007C7C7C008686860089898900888888007C7C7C006767
      6700464646000000000000000000000000000000000000000000000000000000
      00000B0B44001F1FBA002828EF00302DFF003232FF003230FF002C28EC001F1C
      B400101044000000000000000000000000000000000000000000000000000000
      0000012A060001720E000197130001AA150001AE160001AD150001961300016D
      0D00012D07000000000000000000000000000000000000000000000000000000
      0000434343006A6A6A007C7C7C008686860089898900888888007C7C7C006767
      6700464646000000000000000000000000000000000000000000000000006060
      6000828282008F8F8F009C9C9C00A1A1A100A4A4A4009F9F9F009B9B9B008686
      86007E7E7E006565650000000000000000000000000000000000000000001919
      A000282CF8003B3BFF004D52FF005B5BFF00615FFF005957FF004D51FF00332C
      FF00282AF2001F1F9C000000000000000000000000000000000000000000015F
      0D0001A1140001B817000EC624001CC7300021C9350017C62C000DC5230001A8
      1500019C140001630D0000000000000000000000000000000000000000006060
      6000828282008F8F8F009C9C9C00A1A1A100A4A4A4009F9F9F009B9B9B008686
      86007E7E7E006565650000000000000000000000000000000000000000007C7C
      7C0089898900959595009E9E9E009C9C9C009E9E9E009E9E9E009B9B9B009494
      9400888888007979790000000000000000000000000000000000000000002928
      F0003332FF004546FF005656FF005551FF005256FF005655FF004E50FF004145
      FF003330FF002328E40000000000000000000000000000000000000000000199
      130001AE160002C2190016C62B0010C5250013C6280014C529000CC3220001C0
      180001AD15000190120000000000000000000000000000000000000000007C7C
      7C0089898900959595009E9E9E009C9C9C009E9E9E009E9E9E009B9B9B009494
      9400888888007979790000000000000000000000000000000000656565007E7E
      7E00848484008484840085858500828282008383830083838300868686008686
      8600848484007C7C7C00686868000000000000000000000000001C1CAF002929
      F2002C2DFC00302AFF00302DFD00282AFC00302AFD00292CFC002C30FD002C30
      FD00302CFC002928F000201FAB00000000000000000000000000016A0E00019D
      130001A7150001A6140001A8150001A3140001A4140001A5140001A9150001A9
      150001A5150001991300016B0D00000000000000000000000000656565007E7E
      7E00848484008484840085858500828282008383830083838300868686008686
      8600848484007C7C7C0068686800000000000000000000000000707070007777
      7700797979007A7A7A00808080008585850085858500848484007E7E7E007A7A
      7A007B7B7B0078787800717171000000000000000000000000001D22CD002623
      E4002226E8002328E9002A29F700322CFD002A2DFE00302CFC002929F2002828
      E9002A28E9002228E2002323C800000000000000000000000000017F1000018D
      1200019112000193120001A0140001A6150001A8150001A51500019D13000195
      12000195120001901200017D0F00000000000000000000000000707070007777
      7700797979007A7A7A00808080008585850085858500848484007E7E7E007A7A
      7A007B7B7B0078787800717171000000000000000000000000006E6E6E007373
      73007171710085858500A0A0A000A8A8A800AAAAAA00A5A5A5009A9A9A007E7E
      7E00757575006F6F6F006D6D6D00000000000000000000000000201FC8002823
      D2001D20D800342DFC005559FF006669FF006B6BFF006464FF004E4EFF002929
      F2002226D5001D1FD3002020C70000000000000000000000000001790F000183
      11000183110001A8150016C72B002ACB3D002FCC410027CA3A000CC32200019D
      130001871100017F100001790F000000000000000000000000006E6E6E007373
      73007171710085858500A0A0A000A8A8A800AAAAAA00A5A5A5009A9A9A007E7E
      7E00757575006F6F6F006D6D6D000000000000000000000000006C6C6C007171
      710087878700BABABA00BCBCBC00B8B8B800B7B7B700B9B9B900BFBFBF00B3B3
      B30085858500717171006E6E6E000000000000000000000000001D1FC3002222
      D0003430FD008A8EFF009192FF00858AFF008785FF00878BFF009998FF008080
      FF003232F4002622D0001F22C20000000000000000000000000001770F000180
      100001AB150056D765005ED86C0051D661004FD45F0053D6620065D9730048D2
      580001A715000182100001780F000000000000000000000000006C6C6C007171
      710087878700BABABA00BCBCBC00B8B8B800B7B7B700B9B9B900BFBFBF00B3B3
      B30085858500717171006E6E6E000000000000000000000000006A6A6A007878
      7800B7B7B700D3D3D300D0D0D000CACACA00C6C6C600CDCDCD00D6D6D600DCDC
      DC00B4B4B40078787800696969000000000000000000000000001920BB002823
      E8008786FF00C4C4FF00C0BDFF00B0B2FF00A7A7FF00B6B6FF00CECAFF00D8D6
      FF008181FF002C29D9001C1DBA0000000000000000000000000001730F000190
      120050D460009EE7A70095E59E0085E2900078DE84008CE39600A8EAB000BBEF
      C10049D25900018E120001700E000000000000000000000000006A6A6A007878
      7800B7B7B700D3D3D300D0D0D000CACACA00C6C6C600CDCDCD00D6D6D600DCDC
      DC00B4B4B4007878780069696900000000000000000000000000606060007474
      7400BFBFBF00E4E4E400E4E4E400DFDFDF00DBDBDB00E0E0E000E9E9E900EAEA
      EA00BDBDBD007A7A7A00686868000000000000000000000000001919A0002320
      E2009999FF00E7E7FF00E8E8FF00E0DEFF00D5D6FF00E1DFFF00F0F1FF00F1F2
      FF009592FF002D2DD3002222A400000000000000000000000000015F0D000189
      110066D97400D5F5D900D6F5DA00C6F1CB00B8EEBE00C8F2CD00E5F9E700E6FA
      E8005FD86D00018F1100016C0E00000000000000000000000000606060007474
      7400BFBFBF00E4E4E400E4E4E400DFDFDF00DBDBDB00E0E0E000E9E9E900EAEA
      EA00BDBDBD007A7A7A0068686800000000000000000000000000000000006E6E
      6E0087878700D9D9D900ECECEC00ECECEC00E9E9E900EAEAEA00EBEBEB00D1D1
      D1008E8E8E007171710000000000000000000000000000000000000000001D20
      C9003230FD00D2D3FF00F5F5FF00F4F5FF00EFF0FF00F2F2FF00F2F2FF00BFC0
      FF003E44F1002823C6000000000000000000000000000000000000000000017B
      0F0001AB1500B4EDBB00ECFAEE00ECFBEE00E2F9E500E7F9E900E7F9E90098E6
      A10002B41700017D100000000000000000000000000000000000000000006E6E
      6E0087878700D9D9D900ECECEC00ECECEC00E9E9E900EAEAEA00EBEBEB00D1D1
      D1008E8E8E007171710000000000000000000000000000000000000000005B5B
      5B007070700087878700C7C7C700E2E2E200EAEAEA00DDDDDD00BCBCBC007F7F
      7F006D6D6D006363630000000000000000000000000000000000000000001719
      8A002222CA003330FC00A7A9FF00E4E4FF00F2F2FF00DADAFF008E91FF00302C
      ED001C20C70023208E0000000000000000000000000000000000000000000156
      0B00017C100001AA150079DF8500D0F4D400E7F9E900C0EFC6005BD86A00019D
      1300017A0F00015F0C0000000000000000000000000000000000000000005B5B
      5B007070700087878700C7C7C700E2E2E200EAEAEA00DDDDDD00BCBCBC007F7F
      7F006D6D6D006363630000000000000000000000000000000000000000000000
      0000414141005C5C5C0070707000848484008989890083838300717171005C5C
      5C00454545000000000000000000000000000000000000000000000000000000
      00000D0D3700171795002022CA003633ED003B3AF1003034E8002026C4001917
      920014103B000000000000000000000000000000000000000000000000000000
      00000125060001580C00017C100001A2150001AC1500019F1400017E10000158
      0B00012A06000000000000000000000000000000000000000000000000000000
      0000414141005C5C5C0070707000848484008989890083838300717171005C5C
      5C00454545000000000000000000000000000000000000000000000000000000
      00000000000000000000000000005C5C5C005C5C5C005C5C5C00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000191792001917920019179200000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000001580B0001580B0001580B00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000005C5C5C005C5C5C005C5C5C00000000000000
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
end
