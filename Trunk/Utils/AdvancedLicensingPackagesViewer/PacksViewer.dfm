object PacksViewerForm: TPacksViewerForm
  Left = 0
  Top = 0
  Caption = 'Packages viewer'
  ClientHeight = 367
  ClientWidth = 503
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ConnectionStatusBar: TStatusBar
    Left = 0
    Top = 347
    Width = 503
    Height = 20
    Panels = <
      item
        Style = psOwnerDraw
        Width = 50
      end>
    OnDrawPanel = ConnectionStatusBarDrawPanel
  end
  object MainMenu: TMainMenu
    Left = 8
    Top = 312
    object FileMenu: TMenuItem
      Caption = 'File'
      object OpenMenu: TMenuItem
        Caption = 'Open packages file'
        OnClick = OpenMenuClick
      end
      object ExportXMLMenu: TMenuItem
        Caption = 'Export XML'
        OnClick = ExportXMLMenuClick
      end
      object ExitMenu: TMenuItem
        Caption = 'Exit'
        OnClick = ExitMenuClick
      end
    end
    object HelpMenu: TMenuItem
      Caption = 'Help'
      object AboutMenu: TMenuItem
        Caption = 'About'
        OnClick = AboutMenuClick
      end
    end
  end
  object LedsImageList: TImageList
    Left = 40
    Top = 312
    Bitmap = {
      494C010102000400040010001000FFFFFFFFFF00FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000001580B0001580B0001580B00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000191792001917920019179200000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000012A060001720E000197130001AA150001AE160001AD150001961300016D
      0D00012D07000000000000000000000000000000000000000000000000000000
      00000B0B44001F1FBA002828EF00302DFF003232FF003230FF002C28EC001F1C
      B400101044000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000015F
      0D0001A1140001B817000EC624001CC7300021C9350017C62C000DC5230001A8
      1500019C140001630D0000000000000000000000000000000000000000001919
      A000282CF8003B3BFF004D52FF005B5BFF00615FFF005957FF004D51FF00332C
      FF00282AF2001F1F9C0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000199
      130001AE160002C2190016C62B0010C5250013C6280014C529000CC3220001C0
      180001AD15000190120000000000000000000000000000000000000000002928
      F0003332FF004546FF005656FF005551FF005256FF005655FF004E50FF004145
      FF003330FF002328E40000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000016A0E00019D
      130001A7150001A6140001A8150001A3140001A4140001A5140001A9150001A9
      150001A5150001991300016B0D000000000000000000000000001C1CAF002929
      F2002C2DFC00302AFF00302DFD00282AFC00302AFD00292CFC002C30FD002C30
      FD00302CFC002928F000201FAB00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000017F1000018D
      1200019112000193120001A0140001A6150001A8150001A51500019D13000195
      12000195120001901200017D0F000000000000000000000000001D22CD002623
      E4002226E8002328E9002A29F700322CFD002A2DFE00302CFC002929F2002828
      E9002A28E9002228E2002323C800000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000001790F000183
      11000183110001A8150016C72B002ACB3D002FCC410027CA3A000CC32200019D
      130001871100017F100001790F00000000000000000000000000201FC8002823
      D2001D20D800342DFC005559FF006669FF006B6BFF006464FF004E4EFF002929
      F2002226D5001D1FD3002020C700000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000001770F000180
      100001AB150056D765005ED86C0051D661004FD45F0053D6620065D9730048D2
      580001A715000182100001780F000000000000000000000000001D1FC3002222
      D0003430FD008A8EFF009192FF00858AFF008785FF00878BFF009998FF008080
      FF003232F4002622D0001F22C200000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000001730F000190
      120050D460009EE7A70095E59E0085E2900078DE84008CE39600A8EAB000BBEF
      C10049D25900018E120001700E000000000000000000000000001920BB002823
      E8008786FF00C4C4FF00C0BDFF00B0B2FF00A7A7FF00B6B6FF00CECAFF00D8D6
      FF008181FF002C29D9001C1DBA00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000015F0D000189
      110066D97400D5F5D900D6F5DA00C6F1CB00B8EEBE00C8F2CD00E5F9E700E6FA
      E8005FD86D00018F1100016C0E000000000000000000000000001919A0002320
      E2009999FF00E7E7FF00E8E8FF00E0DEFF00D5D6FF00E1DFFF00F0F1FF00F1F2
      FF009592FF002D2DD3002222A400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000017B
      0F0001AB1500B4EDBB00ECFAEE00ECFBEE00E2F9E500E7F9E900E7F9E90098E6
      A10002B41700017D100000000000000000000000000000000000000000001D20
      C9003230FD00D2D3FF00F5F5FF00F4F5FF00EFF0FF00F2F2FF00F2F2FF00BFC0
      FF003E44F1002823C60000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000156
      0B00017C100001AA150079DF8500D0F4D400E7F9E900C0EFC6005BD86A00019D
      1300017A0F00015F0C0000000000000000000000000000000000000000001719
      8A002222CA003330FC00A7A9FF00E4E4FF00F2F2FF00DADAFF008E91FF00302C
      ED001C20C70023208E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000125060001580C00017C100001A2150001AC1500019F1400017E10000158
      0B00012A06000000000000000000000000000000000000000000000000000000
      00000D0D3700171795002022CA003633ED003B3AF1003034E8002026C4001917
      920014103B000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000001580B0001580B0001580B00000000000000
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
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF00FC1FFC1F00000000F007F00700000000
      E003E00300000000C001C00100000000C001C001000000008000800000000000
      8000800000000000800080000000000080008000000000008000800000000000
      8000800000000000C001C00100000000C001C00100000000E003E00300000000
      F007F00700000000FC1FFC1F00000000}
  end
  object OpenPackageDialog: TOpenDialog
    Left = 72
    Top = 312
  end
  object SaveFileDialog: TSaveDialog
    Left = 104
    Top = 312
  end
  object DongleConnectionTimer: TTimer
    Interval = 300
    OnTimer = DongleConnectionTimerTimer
    Left = 136
    Top = 312
  end
end