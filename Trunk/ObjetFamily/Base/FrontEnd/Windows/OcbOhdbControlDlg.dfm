object OcbOhdbControlForm: TOcbOhdbControlForm
  Left = 540
  Top = 197
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Voltages'
  ClientHeight = 566
  ClientWidth = 683
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
  ObjectMenuItem = N1
  PopupMenu = PopupMenu1
  Position = poDesigned
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PowerSupply: TGroupBox
    Left = 5
    Top = 5
    Width = 333
    Height = 188
    Caption = 'OCB'
    TabOrder = 0
    object Label6: TLabel
      Left = 11
      Top = 43
      Width = 110
      Height = 13
      Caption = 'VS Power Supply ( 8V )'
    end
    object Label13: TLabel
      Left = 11
      Top = 72
      Width = 117
      Height = 13
      Caption = 'VCC Power Supply ( 5V )'
    end
    object Label14: TLabel
      Left = 11
      Top = 101
      Width = 87
      Height = 13
      Caption = '12V Power Supply'
    end
    object Label22: TLabel
      Left = 11
      Top = 130
      Width = 87
      Height = 13
      Caption = '24V Power Supply'
    end
    object Label23: TLabel
      Left = 253
      Top = 17
      Width = 20
      Height = 13
      Caption = 'A/D'
    end
    object Label24: TLabel
      Left = 168
      Top = 17
      Width = 18
      Height = 13
      Caption = 'Volt'
    end
    object VS_OCB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 40
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object VCC_OCB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 69
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
    object V_12V_OCB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 98
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
    object V_24V_OCB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 127
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 6
    end
    object A2D_VS_OCB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 40
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object A2D_VCC_OCB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 69
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object A2D_V_12V_OCB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 98
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 5
    end
    object A2D_V_24V_OCB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 127
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 7
    end
  end
  object GroupBox3: TGroupBox
    Left = 344
    Top = 5
    Width = 333
    Height = 188
    Caption = 'OHDB'
    TabOrder = 1
    object Label27: TLabel
      Left = 11
      Top = 43
      Width = 121
      Height = 13
      Caption = '40V Power Supply ( Vpp )'
    end
    object Label29: TLabel
      Left = 11
      Top = 73
      Width = 121
      Height = 13
      Caption = '12V Power Supply ( Vdd )'
    end
    object Label31: TLabel
      Left = 253
      Top = 17
      Width = 20
      Height = 13
      Caption = 'A/D'
    end
    object Label32: TLabel
      Left = 168
      Top = 17
      Width = 18
      Height = 13
      Caption = 'Volt'
    end
    object Label16: TLabel
      Left = 11
      Top = 101
      Width = 115
      Height = 13
      Caption = '5V Power Supply ( Vcc )'
    end
    object Label17: TLabel
      Left = 11
      Top = 130
      Width = 22
      Height = 13
      Caption = '3.3V'
    end
    object Label25: TLabel
      Left = 11
      Top = 159
      Width = 22
      Height = 13
      Caption = '1.2V'
    end
    object VPP_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 40
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object VDD_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 69
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
    object A2D_VPP_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 40
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object A2D_VDD_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 69
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object A2D_VCC_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 98
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
    object VCC_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 98
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 5
    end
    object A2D_3_3V_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 127
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 6
    end
    object Volts_3_3V_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 127
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 7
    end
    object A2D_1_2V_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 232
      Top = 156
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 8
    end
    object Volts_1_2V_OHDB_PowerSupplyStatusPanel: TPanel
      Left = 146
      Top = 156
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 9
    end
  end
  object GroupBox1: TGroupBox
    Left = 6
    Top = 199
    Width = 333
    Height = 178
    Caption = 'MSC1 '
    TabOrder = 2
    object Label9: TLabel
      Left = 24
      Top = 264
      Width = 34
      Height = 13
      Caption = '2_3.3V'
    end
    object Label12: TLabel
      Left = 247
      Top = 264
      Width = 13
      Height = 13
      Caption = '7V'
    end
    object Label18: TLabel
      Left = 11
      Top = 36
      Width = 87
      Height = 13
      Caption = '24V Power Supply'
    end
    object Label19: TLabel
      Left = 11
      Top = 71
      Width = 81
      Height = 13
      Caption = '7V Power Supply'
    end
    object Label20: TLabel
      Left = 11
      Top = 106
      Width = 81
      Height = 13
      Caption = '5V Power Supply'
    end
    object Label21: TLabel
      Left = 11
      Top = 140
      Width = 90
      Height = 13
      Caption = '3.3V Power Supply'
    end
    object Label5: TLabel
      Left = 168
      Top = 11
      Width = 18
      Height = 13
      Caption = 'Volt'
    end
    object Label7: TLabel
      Left = 253
      Top = 11
      Width = 20
      Height = 13
      Caption = 'A/D'
    end
    object I2C_1_24_A2D_StatusPanel: TPanel
      Left = 232
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object I2C_1_7_A2D_StatusPanel: TPanel
      Left = 232
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object I2C_1_5_A2D_StatusPanel: TPanel
      Left = 232
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 5
    end
    object I2C_1_3_A2D_StatusPanel: TPanel
      Left = 232
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 7
    end
    object I2C_A_8_StatusPanel: TPanel
      Left = 104
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 8
    end
    object I2C_1_24V_StatusPanel: TPanel
      Left = 146
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object I2C_1_5V_StatusPanel: TPanel
      Left = 146
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
    object I2C_1_3V_StatusPanel: TPanel
      Left = 146
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 6
    end
    object I2C_A_8V_StatusPanel: TPanel
      Left = 192
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 9
    end
    object I2C_1_7V_StatusPanel: TPanel
      Left = 146
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
  end
  object GroupBox2: TGroupBox
    Left = 345
    Top = 199
    Width = 333
    Height = 178
    Caption = 'MSC2 '
    TabOrder = 3
    object Label1: TLabel
      Left = 24
      Top = 264
      Width = 34
      Height = 13
      Caption = '2_3.3V'
    end
    object Label2: TLabel
      Left = 247
      Top = 264
      Width = 13
      Height = 13
      Caption = '7V'
    end
    object Label4: TLabel
      Left = 11
      Top = 140
      Width = 90
      Height = 13
      Caption = '3.3V Power Supply'
    end
    object Label8: TLabel
      Left = 253
      Top = 11
      Width = 20
      Height = 13
      Caption = 'A/D'
    end
    object Label10: TLabel
      Left = 168
      Top = 11
      Width = 18
      Height = 13
      Caption = 'Volt'
    end
    object Label3: TLabel
      Left = 11
      Top = 106
      Width = 81
      Height = 13
      Caption = '5V Power Supply'
    end
    object Label11: TLabel
      Left = 11
      Top = 71
      Width = 81
      Height = 13
      Caption = '7V Power Supply'
    end
    object Label15: TLabel
      Left = 11
      Top = 36
      Width = 87
      Height = 13
      Caption = '24V Power Supply'
    end
    object I2C_2_24_A2D_StatusPanel: TPanel
      Left = 232
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object I2C_2_7_A2D_StatusPanel: TPanel
      Left = 232
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object I2C_2_5_A2D_StatusPanel: TPanel
      Left = 232
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 5
    end
    object I2C_2_3_A2D_StatusPanel: TPanel
      Left = 232
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 7
    end
    object Panel5: TPanel
      Left = 104
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 8
    end
    object I2C_2_24V_StatusPanel: TPanel
      Left = 146
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object I2C_2_5V_StatusPanel: TPanel
      Left = 146
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
    object I2C_2_3V_StatusPanel: TPanel
      Left = 146
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 6
    end
    object Panel9: TPanel
      Left = 192
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 9
    end
    object I2C_2_7V_StatusPanel: TPanel
      Left = 146
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
  end
  object GroupBox5: TGroupBox
    Left = 6
    Top = 383
    Width = 333
    Height = 178
    Caption = 'MSC3 '
    TabOrder = 4
    object Label50: TLabel
      Left = 24
      Top = 264
      Width = 34
      Height = 13
      Caption = '2_3.3V'
    end
    object Label51: TLabel
      Left = 247
      Top = 264
      Width = 13
      Height = 13
      Caption = '7V'
    end
    object Label52: TLabel
      Left = 11
      Top = 140
      Width = 90
      Height = 13
      Caption = '3.3V Power Supply'
    end
    object Label53: TLabel
      Left = 253
      Top = 11
      Width = 20
      Height = 13
      Caption = 'A/D'
    end
    object Label54: TLabel
      Left = 168
      Top = 11
      Width = 18
      Height = 13
      Caption = 'Volt'
    end
    object Label55: TLabel
      Left = 11
      Top = 106
      Width = 81
      Height = 13
      Caption = '5V Power Supply'
    end
    object Label56: TLabel
      Left = 11
      Top = 71
      Width = 81
      Height = 13
      Caption = '7V Power Supply'
    end
    object Label57: TLabel
      Left = 11
      Top = 36
      Width = 87
      Height = 13
      Caption = '24V Power Supply'
    end
    object I2C_3_24_A2D_StatusPanel: TPanel
      Left = 232
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object I2C_3_7_A2D_StatusPanel: TPanel
      Left = 232
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object I2C_3_5_A2D_StatusPanel: TPanel
      Left = 232
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 5
    end
    object I2C_3_3_A2D_StatusPanel: TPanel
      Left = 232
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 7
    end
    object Panel54: TPanel
      Left = 104
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 8
    end
    object I2C_3_24V_StatusPanel: TPanel
      Left = 146
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object I2C_3_5V_StatusPanel: TPanel
      Left = 146
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
    object I2C_3_3V_StatusPanel: TPanel
      Left = 146
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 6
    end
    object Panel58: TPanel
      Left = 192
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 9
    end
    object I2C_3_7V_StatusPanel: TPanel
      Left = 146
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
  end
  object GroupBox6: TGroupBox
    Left = 345
    Top = 383
    Width = 333
    Height = 178
    Caption = 'MSC4 '
    TabOrder = 5
    object Label60: TLabel
      Left = 24
      Top = 264
      Width = 34
      Height = 13
      Caption = '2_3.3V'
    end
    object Label61: TLabel
      Left = 247
      Top = 264
      Width = 13
      Height = 13
      Caption = '7V'
    end
    object Label62: TLabel
      Left = 11
      Top = 140
      Width = 90
      Height = 13
      Caption = '3.3V Power Supply'
    end
    object Label63: TLabel
      Left = 253
      Top = 11
      Width = 20
      Height = 13
      Caption = 'A/D'
    end
    object Label64: TLabel
      Left = 168
      Top = 11
      Width = 18
      Height = 13
      Caption = 'Volt'
    end
    object Label65: TLabel
      Left = 11
      Top = 106
      Width = 81
      Height = 13
      Caption = '5V Power Supply'
    end
    object LAbel66: TLabel
      Left = 11
      Top = 71
      Width = 81
      Height = 13
      Caption = '7V Power Supply'
    end
    object LAbel67: TLabel
      Left = 11
      Top = 36
      Width = 87
      Height = 13
      Caption = '24V Power Supply'
    end
    object I2C_4_24_A2D_StatusPanel: TPanel
      Left = 232
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 1
    end
    object I2C_4_7_A2D_StatusPanel: TPanel
      Left = 232
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 3
    end
    object I2C_4_5_A2D_StatusPanel: TPanel
      Left = 232
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 5
    end
    object I2C_4_3_A2D_StatusPanel: TPanel
      Left = 232
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 7
    end
    object Panel64: TPanel
      Left = 104
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 8
    end
    object I2C_4_24V_StatusPanel: TPanel
      Left = 146
      Top = 33
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 0
    end
    object I2C_4_5V_StatusPanel: TPanel
      Left = 146
      Top = 103
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 4
    end
    object I2C_4_3V_StatusPanel: TPanel
      Left = 146
      Top = 137
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 6
    end
    object Panel68: TPanel
      Left = 192
      Top = 256
      Width = 49
      Height = 25
      BevelOuter = bvLowered
      Caption = '???'
      TabOrder = 9
    end
    object I2C_4_7V_StatusPanel: TPanel
      Left = 146
      Top = 68
      Width = 71
      Height = 20
      BevelOuter = bvLowered
      Caption = '???'
      ParentBackground = False
      TabOrder = 2
    end
  end
  object AlwaysOnTopAction: TAction
    Category = 'Form'
    AutoCheck = True
    Caption = 'Always on top'
    Enabled = False
    OnExecute = AlwaysOnTopActionExecute
  end
  object CloseDialogAction: TAction
    Caption = 'Close'
    OnExecute = CloseDialogActionExecute
  end
  object PopupMenu1: TPopupMenu
    Left = 272
    Top = 157
    object Alwaysontop1: TMenuItem
      Action = CloseDialogAction
    end
    object N1: TMenuItem
      Caption = '-'
      Enabled = False
    end
    object Close1: TMenuItem
      Action = CloseDialogAction
    end
  end
end
