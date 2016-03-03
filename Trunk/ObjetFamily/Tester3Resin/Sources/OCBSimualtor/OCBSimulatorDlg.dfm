object OCBSimulatorForm: TOCBSimulatorForm
  Left = 7
  Top = 25
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'OCB Simulator'
  ClientHeight = 619
  ClientWidth = 996
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
  object Label50: TLabel
    Left = 17
    Top = 455
    Width = 64
    Height = 13
    Caption = 'Message Log'
  end
  object MessageLogMemo: TMemo
    Left = 16
    Top = 472
    Width = 409
    Height = 121
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object PageControl1: TPageControl
    Left = 16
    Top = 14
    Width = 961
    Height = 411
    ActivePage = TabSheet6
    TabOrder = 1
    object TabSheet6: TTabSheet
      Caption = 'Power'
      ImageIndex = 5
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label9: TLabel
        Left = 13
        Top = 60
        Width = 74
        Height = 13
        Caption = 'SetPowerOnOff'
      end
      object Label10: TLabel
        Left = 13
        Top = 92
        Width = 77
        Height = 13
        Caption = 'GetPowerStatus'
      end
      object Label32: TLabel
        Left = 192
        Top = 20
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label33: TLabel
        Left = 352
        Top = 20
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label34: TLabel
        Left = 597
        Top = 20
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label35: TLabel
        Left = 748
        Top = 20
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Label78: TLabel
        Left = 13
        Top = 124
        Width = 81
        Height = 13
        Caption = 'SetPowerParams'
      end
      object Edit3: TEdit
        Tag = 107
        Left = 152
        Top = 56
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object ComboBox3: TComboBox
        Tag = 107
        Left = 312
        Top = 56
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 1
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox3: TCheckBox
        Tag = 107
        Left = 476
        Top = 58
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 2
      end
      object Edit20: TEdit
        Left = 608
        Top = 55
        Width = 73
        Height = 21
        TabOrder = 3
        Text = '0'
      end
      object ComboBox20: TComboBox
        Tag = 363
        Left = 720
        Top = 55
        Width = 121
        Height = 21
        ItemHeight = 0
        TabOrder = 4
      end
      object Edit4: TEdit
        Tag = 108
        Left = 152
        Top = 88
        Width = 121
        Height = 21
        TabOrder = 5
      end
      object ComboBox4: TComboBox
        Tag = 108
        Left = 312
        Top = 88
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 6
        Items.Strings = (
          'PowerStatus')
      end
      object CheckBox4: TCheckBox
        Tag = 108
        Left = 476
        Top = 90
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 7
      end
      object UpDown7: TUpDown
        Tag = 107
        Left = 681
        Top = 55
        Width = 15
        Height = 21
        Associate = Edit20
        TabOrder = 8
      end
      object BitBtn6: TBitBtn
        Left = 331
        Top = 159
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 9
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object Edit23: TEdit
        Tag = 106
        Left = 152
        Top = 120
        Width = 121
        Height = 21
        TabOrder = 10
      end
      object ComboBox23: TComboBox
        Tag = 106
        Left = 312
        Top = 120
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 11
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox8: TCheckBox
        Tag = 106
        Left = 476
        Top = 122
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 12
      end
      object Button1: TButton
        Tag = 107
        Left = 852
        Top = 55
        Width = 91
        Height = 21
        Caption = 'Send Notification'
        TabOrder = 13
        OnClick = SendNotificationButtonClick
      end
    end
    object TabSheet7: TTabSheet
      Caption = 'Tray'
      ImageIndex = 6
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label36: TLabel
        Left = 192
        Top = 15
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label59: TLabel
        Left = 352
        Top = 15
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label60: TLabel
        Left = 597
        Top = 15
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label65: TLabel
        Left = 748
        Top = 15
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Label7: TLabel
        Left = 13
        Top = 36
        Width = 65
        Height = 13
        Caption = 'SetTrayOnOff'
      end
      object Label53: TLabel
        Left = 13
        Top = 72
        Width = 91
        Height = 13
        Caption = 'SetTrayTemprature'
      end
      object Label8: TLabel
        Left = 13
        Top = 109
        Width = 68
        Height = 13
        Caption = 'GetTrayStatus'
      end
      object Label79: TLabel
        Left = 13
        Top = 145
        Width = 67
        Height = 13
        Caption = 'IsTrayInserted'
      end
      object BitBtn7: TBitBtn
        Left = 331
        Top = 177
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 0
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object Edit1: TEdit
        Tag = 100
        Left = 152
        Top = 32
        Width = 121
        Height = 21
        TabOrder = 1
      end
      object ComboBox1: TComboBox
        Tag = 100
        Left = 312
        Top = 32
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 2
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox34: TCheckBox
        Tag = 100
        Left = 476
        Top = 34
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 3
      end
      object Edit8: TEdit
        Left = 608
        Top = 32
        Width = 73
        Height = 21
        TabOrder = 4
        Text = '0'
      end
      object ComboBox8: TComboBox
        Tag = 356
        Left = 720
        Top = 32
        Width = 121
        Height = 21
        ItemHeight = 0
        TabOrder = 5
      end
      object Edit38: TEdit
        Tag = 101
        Left = 152
        Top = 68
        Width = 121
        Height = 21
        TabOrder = 6
      end
      object ComboBox39: TComboBox
        Tag = 101
        Left = 312
        Top = 68
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 7
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox39: TCheckBox
        Tag = 101
        Left = 478
        Top = 70
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 8
      end
      object Edit2: TEdit
        Tag = 102
        Left = 152
        Top = 105
        Width = 121
        Height = 21
        TabOrder = 9
      end
      object ComboBox2: TComboBox
        Tag = 102
        Left = 312
        Top = 105
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 10
        Items.Strings = (
          'TrayStatus')
      end
      object CheckBox2: TCheckBox
        Tag = 102
        Left = 478
        Top = 107
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 11
      end
      object UpDown1: TUpDown
        Tag = 100
        Left = 681
        Top = 32
        Width = 15
        Height = 21
        Associate = Edit8
        TabOrder = 12
      end
      object Button2: TButton
        Tag = 100
        Left = 852
        Top = 32
        Width = 91
        Height = 21
        Caption = 'Send Notification'
        TabOrder = 13
        OnClick = SendNotificationButtonClick
      end
      object Edit24: TEdit
        Tag = 103
        Left = 152
        Top = 141
        Width = 121
        Height = 21
        TabOrder = 14
      end
      object ComboBox24: TComboBox
        Tag = 103
        Left = 312
        Top = 141
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 15
        Items.Strings = (
          'TrayInStatus - In'
          'TrayInStatus - Out')
      end
      object CheckBox15: TCheckBox
        Tag = 103
        Left = 478
        Top = 143
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 16
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'UV Lamps'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label1: TLabel
        Left = 192
        Top = 18
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label2: TLabel
        Left = 352
        Top = 18
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label11: TLabel
        Left = 13
        Top = 46
        Width = 90
        Height = 13
        Caption = 'SetUVLampsOnOff'
      end
      object Label12: TLabel
        Left = 13
        Top = 77
        Width = 66
        Height = 13
        Caption = 'SetUVParams'
      end
      object Label13: TLabel
        Left = 13
        Top = 109
        Width = 93
        Height = 13
        Caption = 'GetUVLampsStatus'
      end
      object Label14: TLabel
        Left = 597
        Top = 18
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label21: TLabel
        Left = 748
        Top = 18
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Edit5: TEdit
        Tag = 109
        Left = 152
        Top = 42
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object Edit6: TEdit
        Tag = 110
        Left = 152
        Top = 73
        Width = 121
        Height = 21
        TabOrder = 1
      end
      object Edit7: TEdit
        Tag = 111
        Left = 152
        Top = 105
        Width = 121
        Height = 21
        TabOrder = 2
      end
      object DefaultsBitBtn: TBitBtn
        Left = 331
        Top = 159
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 3
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object CheckBox5: TCheckBox
        Tag = 109
        Left = 476
        Top = 44
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 4
      end
      object CheckBox6: TCheckBox
        Tag = 110
        Left = 476
        Top = 75
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 5
      end
      object CheckBox7: TCheckBox
        Tag = 111
        Left = 476
        Top = 107
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 6
      end
      object ComboBox5: TComboBox
        Tag = 109
        Left = 312
        Top = 42
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 7
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object ComboBox6: TComboBox
        Tag = 110
        Left = 312
        Top = 73
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 8
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object ComboBox7: TComboBox
        Tag = 111
        Left = 312
        Top = 105
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 9
        Items.Strings = (
          'UvLampsStatus')
      end
      object ComboBox15: TComboBox
        Tag = 365
        Left = 720
        Top = 42
        Width = 121
        Height = 21
        ItemHeight = 0
        TabOrder = 10
      end
      object Edit15: TEdit
        Left = 608
        Top = 42
        Width = 73
        Height = 21
        TabOrder = 11
        Text = '0'
      end
      object UpDown2: TUpDown
        Tag = 109
        Left = 681
        Top = 42
        Width = 15
        Height = 21
        Associate = Edit15
        TabOrder = 12
      end
      object Button3: TButton
        Left = 852
        Top = 42
        Width = 91
        Height = 21
        Caption = 'Send Notification'
        TabOrder = 13
        OnClick = SendNotificationButtonClick
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Ambient Temp'
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label51: TLabel
        Left = 192
        Top = 20
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label52: TLabel
        Left = 352
        Top = 20
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label54: TLabel
        Left = 3
        Top = 53
        Width = 126
        Height = 13
        Caption = 'AmbientTemperatureOnOff'
      end
      object Label55: TLabel
        Left = 3
        Top = 88
        Width = 140
        Height = 13
        Caption = 'SetAmbientTemperatureLevel'
      end
      object Label56: TLabel
        Left = 3
        Top = 124
        Width = 145
        Height = 13
        Caption = 'GetAmbientTemperatureStatus'
      end
      object Label24: TLabel
        Left = 589
        Top = 20
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label25: TLabel
        Left = 740
        Top = 20
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Edit43: TEdit
        Tag = 114
        Left = 152
        Top = 46
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object ComboBox40: TComboBox
        Tag = 114
        Left = 312
        Top = 45
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 1
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox40: TCheckBox
        Tag = 114
        Left = 478
        Top = 48
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 2
      end
      object Edit44: TEdit
        Tag = 115
        Left = 152
        Top = 82
        Width = 121
        Height = 21
        TabOrder = 3
      end
      object ComboBox41: TComboBox
        Tag = 115
        Left = 312
        Top = 81
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 4
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox41: TCheckBox
        Tag = 115
        Left = 478
        Top = 84
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 5
      end
      object Edit45: TEdit
        Tag = 116
        Left = 152
        Top = 118
        Width = 121
        Height = 21
        TabOrder = 6
      end
      object ComboBox42: TComboBox
        Tag = 116
        Left = 312
        Top = 117
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 7
        Items.Strings = (
          'AmbientTemperatureStatus')
      end
      object CheckBox42: TCheckBox
        Tag = 116
        Left = 478
        Top = 120
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 8
      end
      object BitBtn5: TBitBtn
        Left = 328
        Top = 162
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 9
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object Edit17: TEdit
        Left = 600
        Top = 45
        Width = 73
        Height = 21
        TabOrder = 10
        Text = '0'
      end
      object UpDown4: TUpDown
        Tag = 114
        Left = 673
        Top = 45
        Width = 15
        Height = 21
        Associate = Edit17
        TabOrder = 11
      end
      object ComboBox17: TComboBox
        Tag = 370
        Left = 712
        Top = 45
        Width = 121
        Height = 21
        ItemHeight = 0
        TabOrder = 12
      end
      object Button4: TButton
        Tag = 114
        Left = 852
        Top = 45
        Width = 91
        Height = 21
        Caption = 'Send Notification'
        TabOrder = 13
        OnClick = SendNotificationButtonClick
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Inputs && Actuators'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label3: TLabel
        Left = 192
        Top = 21
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label4: TLabel
        Left = 352
        Top = 21
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label17: TLabel
        Left = 25
        Top = 49
        Width = 113
        Height = 13
        Caption = 'TimedSetActuatorOnOff'
      end
      object Label18: TLabel
        Left = 25
        Top = 85
        Width = 84
        Height = 13
        Caption = 'SetActuatorOnOff'
      end
      object Label19: TLabel
        Left = 25
        Top = 120
        Width = 87
        Height = 13
        Caption = 'GetActuatorStatus'
      end
      object Label20: TLabel
        Left = 25
        Top = 156
        Width = 71
        Height = 13
        Caption = 'GetInputStatus'
      end
      object Label26: TLabel
        Left = 589
        Top = 21
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label29: TLabel
        Left = 740
        Top = 21
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Edit11: TEdit
        Tag = 117
        Left = 152
        Top = 45
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object Edit12: TEdit
        Tag = 118
        Left = 152
        Top = 81
        Width = 121
        Height = 21
        TabOrder = 1
      end
      object Edit13: TEdit
        Tag = 119
        Left = 152
        Top = 116
        Width = 121
        Height = 21
        TabOrder = 2
      end
      object Edit14: TEdit
        Tag = 120
        Left = 152
        Top = 152
        Width = 121
        Height = 21
        TabOrder = 3
      end
      object BitBtn1: TBitBtn
        Left = 336
        Top = 189
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 4
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object CheckBox11: TCheckBox
        Tag = 117
        Left = 475
        Top = 47
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 5
      end
      object CheckBox12: TCheckBox
        Tag = 118
        Left = 475
        Top = 83
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 6
      end
      object CheckBox13: TCheckBox
        Tag = 119
        Left = 475
        Top = 118
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 7
      end
      object CheckBox14: TCheckBox
        Tag = 120
        Left = 475
        Top = 154
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 8
      end
      object ComboBox11: TComboBox
        Tag = 117
        Left = 312
        Top = 45
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 9
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object ComboBox12: TComboBox
        Tag = 118
        Left = 312
        Top = 80
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 10
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object ComboBox13: TComboBox
        Tag = 119
        Left = 312
        Top = 115
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 11
        Items.Strings = (
          'ActuatorsStatus')
      end
      object ComboBox14: TComboBox
        Tag = 120
        Left = 312
        Top = 150
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 12
        Items.Strings = (
          'InputsStatus')
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Purge'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label5: TLabel
        Left = 192
        Top = 20
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label6: TLabel
        Left = 352
        Top = 20
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label27: TLabel
        Left = 40
        Top = 48
        Width = 64
        Height = 13
        Caption = 'PerformPurge'
      end
      object Label28: TLabel
        Left = 40
        Top = 84
        Width = 75
        Height = 13
        Caption = 'GetPurgeStatus'
      end
      object Label22: TLabel
        Left = 589
        Top = 20
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label23: TLabel
        Left = 740
        Top = 20
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Edit21: TEdit
        Tag = 112
        Left = 152
        Top = 44
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object Edit22: TEdit
        Tag = 113
        Left = 152
        Top = 80
        Width = 121
        Height = 21
        TabOrder = 1
      end
      object BitBtn2: TBitBtn
        Left = 328
        Top = 116
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 2
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object CheckBox21: TCheckBox
        Tag = 112
        Left = 472
        Top = 46
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 3
      end
      object CheckBox22: TCheckBox
        Tag = 113
        Left = 472
        Top = 82
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 4
      end
      object ComboBox21: TComboBox
        Tag = 112
        Left = 312
        Top = 44
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 5
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object ComboBox22: TComboBox
        Tag = 113
        Left = 312
        Top = 79
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 6
        Items.Strings = (
          'PurgeStatus - Idle'
          'PurgeStatus - Active')
      end
      object Edit16: TEdit
        Left = 600
        Top = 44
        Width = 73
        Height = 21
        TabOrder = 7
        Text = '0'
      end
      object UpDown3: TUpDown
        Tag = 112
        Left = 673
        Top = 44
        Width = 15
        Height = 21
        Associate = Edit16
        TabOrder = 8
      end
      object ComboBox16: TComboBox
        Tag = 368
        Left = 712
        Top = 44
        Width = 121
        Height = 21
        ItemHeight = 0
        TabOrder = 9
      end
      object Button5: TButton
        Tag = 112
        Left = 852
        Top = 44
        Width = 91
        Height = 21
        Caption = 'Send Notification'
        TabOrder = 10
        OnClick = SendNotificationButtonClick
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Status'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label37: TLabel
        Left = 224
        Top = 17
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label38: TLabel
        Left = 384
        Top = 17
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label39: TLabel
        Left = 8
        Top = 45
        Width = 97
        Height = 13
        Caption = 'GetOcbGlobalStatus'
      end
      object Label40: TLabel
        Left = 8
        Top = 81
        Width = 67
        Height = 13
        Caption = 'GetOcbStatus'
      end
      object Label41: TLabel
        Left = 8
        Top = 116
        Width = 114
        Height = 13
        Caption = 'GetOcbSoftwareVersion'
      end
      object Label42: TLabel
        Left = 8
        Top = 152
        Width = 118
        Height = 13
        Caption = 'GetOcbHardwareVersion'
      end
      object Label43: TLabel
        Left = 8
        Top = 187
        Width = 153
        Height = 13
        Caption = 'GetHeadsDriverSoftwareVersion'
      end
      object Label44: TLabel
        Left = 8
        Top = 223
        Width = 155
        Height = 13
        Caption = 'GetheadsDriverHardwareVersion'
      end
      object Label45: TLabel
        Left = 8
        Top = 258
        Width = 90
        Height = 13
        Caption = 'OcbSoftwareReset'
      end
      object Label46: TLabel
        Left = 8
        Top = 294
        Width = 129
        Height = 13
        Caption = 'HeadsDriverSoftwareReset'
      end
      object Label30: TLabel
        Left = 619
        Top = 17
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label31: TLabel
        Left = 770
        Top = 17
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object CheckBox1: TCheckBox
        Tag = 130
        Left = 505
        Top = 43
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 0
      end
      object CheckBox31: TCheckBox
        Tag = 131
        Left = 505
        Top = 79
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 1
      end
      object CheckBox32: TCheckBox
        Tag = 132
        Left = 505
        Top = 114
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 2
      end
      object CheckBox33: TCheckBox
        Tag = 133
        Left = 505
        Top = 148
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 3
      end
      object ComboBox31: TComboBox
        Tag = 130
        Left = 344
        Top = 41
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 4
        Items.Strings = (
          'OcbGlobalStatus')
      end
      object ComboBox32: TComboBox
        Tag = 131
        Left = 344
        Top = 76
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 5
        Items.Strings = (
          'OcbStatus')
      end
      object ComboBox33: TComboBox
        Tag = 132
        Left = 344
        Top = 111
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 6
        Items.Strings = (
          'OcbSoftwareVersion')
      end
      object ComboBox34: TComboBox
        Tag = 133
        Left = 344
        Top = 146
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 7
        Items.Strings = (
          'OcbHardwareVersion')
      end
      object Edit31: TEdit
        Tag = 130
        Left = 184
        Top = 41
        Width = 121
        Height = 21
        TabOrder = 8
      end
      object Edit32: TEdit
        Tag = 131
        Left = 184
        Top = 77
        Width = 121
        Height = 21
        TabOrder = 9
      end
      object Edit33: TEdit
        Tag = 132
        Left = 184
        Top = 112
        Width = 121
        Height = 21
        TabOrder = 10
      end
      object Edit34: TEdit
        Tag = 133
        Left = 184
        Top = 148
        Width = 121
        Height = 21
        TabOrder = 11
      end
      object BitBtn4: TBitBtn
        Left = 360
        Top = 329
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 12
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object Edit39: TEdit
        Tag = 134
        Left = 184
        Top = 183
        Width = 121
        Height = 21
        TabOrder = 13
      end
      object ComboBox35: TComboBox
        Tag = 134
        Left = 344
        Top = 181
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 14
        Items.Strings = (
          'HeadsDriverSoftareVersion')
      end
      object CheckBox35: TCheckBox
        Tag = 134
        Left = 505
        Top = 183
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 15
      end
      object CheckBox36: TCheckBox
        Tag = 135
        Left = 505
        Top = 219
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 16
      end
      object ComboBox36: TComboBox
        Tag = 135
        Left = 344
        Top = 217
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 17
        Items.Strings = (
          'HeadsDriverHardwareVersion')
      end
      object Edit40: TEdit
        Tag = 135
        Left = 184
        Top = 219
        Width = 121
        Height = 21
        TabOrder = 18
      end
      object Edit41: TEdit
        Tag = 136
        Left = 184
        Top = 254
        Width = 121
        Height = 21
        TabOrder = 19
      end
      object ComboBox37: TComboBox
        Tag = 136
        Left = 344
        Top = 252
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 20
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox37: TCheckBox
        Tag = 136
        Left = 505
        Top = 269
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 21
      end
      object Edit42: TEdit
        Tag = 137
        Left = 184
        Top = 290
        Width = 121
        Height = 21
        TabOrder = 22
      end
      object ComboBox38: TComboBox
        Tag = 137
        Left = 344
        Top = 287
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 23
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox38: TCheckBox
        Tag = 137
        Left = 505
        Top = 289
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 24
      end
    end
    object TabSheet8: TTabSheet
      Caption = 'Door'
      ImageIndex = 7
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label62: TLabel
        Left = 13
        Top = 55
        Width = 47
        Height = 13
        Caption = 'LockDoor'
      end
      object Label63: TLabel
        Left = 13
        Top = 86
        Width = 57
        Height = 13
        Caption = 'UnlockDoor'
      end
      object Label64: TLabel
        Left = 13
        Top = 118
        Width = 70
        Height = 13
        Caption = 'GetDoorStatus'
      end
      object Label66: TLabel
        Left = 160
        Top = 20
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label67: TLabel
        Left = 280
        Top = 20
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label68: TLabel
        Left = 509
        Top = 20
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label69: TLabel
        Left = 676
        Top = 28
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Edit51: TEdit
        Tag = 127
        Left = 120
        Top = 51
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object ComboBox51: TComboBox
        Tag = 127
        Left = 256
        Top = 51
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 1
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox51: TCheckBox
        Tag = 127
        Left = 412
        Top = 53
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 2
      end
      object Edit18: TEdit
        Left = 528
        Top = 51
        Width = 57
        Height = 21
        TabOrder = 3
        Text = '0'
      end
      object ComboBox18: TComboBox
        Tag = 383
        Left = 616
        Top = 51
        Width = 217
        Height = 21
        ItemHeight = 0
        TabOrder = 4
      end
      object Edit52: TEdit
        Tag = 128
        Left = 120
        Top = 82
        Width = 121
        Height = 21
        TabOrder = 5
      end
      object ComboBox52: TComboBox
        Tag = 128
        Left = 256
        Top = 82
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 6
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox52: TCheckBox
        Tag = 128
        Left = 412
        Top = 84
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 7
      end
      object Edit19: TEdit
        Left = 528
        Top = 82
        Width = 57
        Height = 21
        TabOrder = 8
        Text = '0'
      end
      object ComboBox19: TComboBox
        Tag = 384
        Left = 616
        Top = 82
        Width = 217
        Height = 21
        ItemHeight = 0
        TabOrder = 9
      end
      object Edit53: TEdit
        Tag = 129
        Left = 120
        Top = 114
        Width = 121
        Height = 21
        TabOrder = 10
      end
      object ComboBox53: TComboBox
        Tag = 129
        Left = 256
        Top = 114
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 11
        Items.Strings = (
          'DoorStatus')
      end
      object CheckBox53: TCheckBox
        Tag = 129
        Left = 412
        Top = 116
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 12
      end
      object BitBtn8: TBitBtn
        Left = 331
        Top = 154
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 13
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object UpDown5: TUpDown
        Tag = 127
        Left = 585
        Top = 51
        Width = 15
        Height = 21
        Associate = Edit18
        TabOrder = 14
      end
      object UpDown6: TUpDown
        Tag = 128
        Left = 585
        Top = 82
        Width = 15
        Height = 21
        Associate = Edit19
        TabOrder = 15
      end
      object Button6: TButton
        Tag = 127
        Left = 848
        Top = 51
        Width = 95
        Height = 22
        Caption = 'Send Notification'
        TabOrder = 16
        OnClick = SendNotificationButtonClick
      end
      object Button7: TButton
        Tag = 128
        Left = 848
        Top = 80
        Width = 97
        Height = 25
        Caption = 'Send Notification'
        TabOrder = 17
        OnClick = SendNotificationButtonClick
      end
    end
    object TabSheet9: TTabSheet
      Caption = 'EOL'
      ImageIndex = 8
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label15: TLabel
        Left = 13
        Top = 52
        Width = 59
        Height = 13
        Caption = 'SetEolOnOff'
      end
      object Label16: TLabel
        Left = 13
        Top = 83
        Width = 66
        Height = 13
        Caption = 'SetEolParams'
      end
      object Label47: TLabel
        Left = 13
        Top = 115
        Width = 62
        Height = 13
        Caption = 'GetEolStatus'
      end
      object Label70: TLabel
        Left = 192
        Top = 23
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label71: TLabel
        Left = 352
        Top = 23
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label72: TLabel
        Left = 597
        Top = 23
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label73: TLabel
        Left = 748
        Top = 23
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Edit9: TEdit
        Tag = 121
        Left = 152
        Top = 48
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object ComboBox9: TComboBox
        Tag = 121
        Left = 312
        Top = 48
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 1
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox9: TCheckBox
        Tag = 121
        Left = 476
        Top = 50
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 2
      end
      object Edit10: TEdit
        Tag = 122
        Left = 152
        Top = 79
        Width = 121
        Height = 21
        TabOrder = 3
      end
      object ComboBox10: TComboBox
        Tag = 122
        Left = 312
        Top = 79
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 4
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox10: TCheckBox
        Tag = 122
        Left = 476
        Top = 81
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 5
      end
      object Edit35: TEdit
        Tag = 123
        Left = 152
        Top = 111
        Width = 121
        Height = 21
        TabOrder = 6
      end
      object ComboBox47: TComboBox
        Tag = 123
        Left = 312
        Top = 111
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 7
        Items.Strings = (
          'EolStatus')
      end
      object CheckBox47: TCheckBox
        Tag = 123
        Left = 476
        Top = 113
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 8
      end
      object BitBtn9: TBitBtn
        Left = 331
        Top = 158
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 9
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
    end
    object TabSheet10: TTabSheet
      Caption = 'Heads'
      ImageIndex = 9
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label74: TLabel
        Left = 192
        Top = 11
        Width = 46
        Height = 13
        Caption = 'Recieved'
      end
      object Label75: TLabel
        Left = 352
        Top = 11
        Width = 55
        Height = 13
        Caption = 'Transimtted'
      end
      object Label76: TLabel
        Left = 597
        Top = 11
        Width = 101
        Height = 13
        Caption = 'Notification time (sec)'
      end
      object Label77: TLabel
        Left = 748
        Top = 11
        Width = 53
        Height = 13
        Caption = 'Notification'
      end
      object Label48: TLabel
        Left = 3
        Top = 39
        Width = 118
        Height = 13
        Caption = 'HeadsFillingControlOnOff'
      end
      object Label49: TLabel
        Left = 3
        Top = 70
        Width = 108
        Height = 13
        Caption = 'SetHeadsFillingParams'
      end
      object Label61: TLabel
        Left = 3
        Top = 102
        Width = 104
        Height = 13
        Caption = 'GetHeadsFillingStatus'
      end
      object Label57: TLabel
        Left = 3
        Top = 139
        Width = 107
        Height = 13
        Caption = 'SetHeadsTemperature'
      end
      object Label58: TLabel
        Left = 3
        Top = 175
        Width = 138
        Height = 13
        Caption = 'GetHeadsTemperatureStatus'
      end
      object BitBtn10: TBitBtn
        Left = 331
        Top = 208
        Width = 105
        Height = 33
        Caption = 'Default values'
        TabOrder = 0
        OnClick = DefaultsBitBtnClick
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333444444
          33333333333F8888883F33330000324334222222443333388F3833333388F333
          000032244222222222433338F8833FFFFF338F3300003222222AAAAA22243338
          F333F88888F338F30000322222A33333A2224338F33F8333338F338F00003222
          223333333A224338F33833333338F38F00003222222333333A444338FFFF8F33
          3338888300003AAAAAAA33333333333888888833333333330000333333333333
          333333333333333333FFFFFF000033333333333344444433FFFF333333888888
          00003A444333333A22222438888F333338F3333800003A2243333333A2222438
          F38F333333833338000033A224333334422224338338FFFFF8833338000033A2
          22444442222224338F3388888333FF380000333A2222222222AA243338FF3333
          33FF88F800003333AA222222AA33A3333388FFFFFF8833830000333333AAAAAA
          3333333333338888883333330000333333333333333333333333333333333333
          0000}
        NumGlyphs = 2
      end
      object Edit36: TEdit
        Tag = 124
        Left = 152
        Top = 35
        Width = 121
        Height = 21
        TabOrder = 1
      end
      object ComboBox48: TComboBox
        Tag = 124
        Left = 312
        Top = 35
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 2
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object CheckBox48: TCheckBox
        Tag = 124
        Left = 476
        Top = 37
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 3
      end
      object CheckBox49: TCheckBox
        Tag = 125
        Left = 476
        Top = 68
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 4
      end
      object ComboBox49: TComboBox
        Tag = 125
        Left = 312
        Top = 66
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 5
        Items.Strings = (
          'Ack - Success'
          'Ack - Failure')
      end
      object Edit37: TEdit
        Tag = 125
        Left = 152
        Top = 66
        Width = 121
        Height = 21
        TabOrder = 6
      end
      object Edit50: TEdit
        Tag = 126
        Left = 152
        Top = 98
        Width = 121
        Height = 21
        TabOrder = 7
      end
      object ComboBox50: TComboBox
        Tag = 126
        Left = 312
        Top = 98
        Width = 145
        Height = 21
        ItemHeight = 13
        TabOrder = 8
        Items.Strings = (
          'HeadsFillingStatus')
      end
      object CheckBox50: TCheckBox
        Tag = 126
        Left = 476
        Top = 100
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 9
      end
      object Edit46: TEdit
        Tag = 138
        Left = 152
        Top = 135
        Width = 121
        Height = 21
        TabOrder = 10
      end
      object ComboBox43: TComboBox
        Tag = 138
        Left = 312
        Top = 133
        Width = 145
        Height = 21
        ItemHeight = 0
        TabOrder = 11
      end
      object CheckBox43: TCheckBox
        Tag = 138
        Left = 478
        Top = 137
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 12
      end
      object Edit47: TEdit
        Tag = 139
        Left = 152
        Top = 171
        Width = 121
        Height = 21
        TabOrder = 13
      end
      object ComboBox44: TComboBox
        Tag = 139
        Left = 312
        Top = 169
        Width = 145
        Height = 21
        ItemHeight = 0
        TabOrder = 14
      end
      object CheckBox44: TCheckBox
        Tag = 139
        Left = 478
        Top = 173
        Width = 97
        Height = 17
        Caption = 'Change Value'
        TabOrder = 15
      end
    end
  end
  object ResetLogBitBtn: TBitBtn
    Left = 432
    Top = 560
    Width = 75
    Height = 33
    Caption = 'Reset Log'
    TabOrder = 2
    OnClick = ResetLogBitBtnClick
  end
  object BitBtn3: TBitBtn
    Left = 878
    Top = 560
    Width = 99
    Height = 33
    TabOrder = 3
    Kind = bkClose
  end
end
