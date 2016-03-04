object AboutBox: TAboutBox
  Left = 511
  Top = 133
  BorderStyle = bsDialog
  Caption = 'About'
  ClientHeight = 437
  ClientWidth = 359
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  Position = poScreenCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label12: TLabel
    Left = 229
    Top = 409
    Width = 65
    Height = 20
    Caption = 'Label12'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = 20
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Panel1: TPanel
    Left = 1
    Top = 7
    Width = 350
    Height = 223
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = True
    ParentFont = False
    TabOrder = 0
    object Shape1: TShape
      Left = 11
      Top = 13
      Width = 124
      Height = 178
      Brush.Color = clBtnFace
      Pen.Style = psClear
    end
    object ProgramIcon: TImage
      Left = 11
      Top = 13
      Width = 123
      Height = 201
      Center = True
      Transparent = True
      IsControl = True
    end
    object ProductName: TLabel
      Left = 146
      Top = 50
      Width = 68
      Height = 13
      Caption = 'Product Name'
      IsControl = True
    end
    object Version: TLabel
      Left = 146
      Top = 69
      Width = 35
      Height = 13
      Caption = 'Version'
      IsControl = True
    end
    object ProductNameLabel: TLabel
      Left = 263
      Top = 52
      Width = 46
      Height = 13
      Caption = 'Eden 260'
    end
    object VersionNum: TLabel
      Left = 263
      Top = 71
      Width = 18
      Height = 13
      Caption = '???'
    end
    object Label3: TLabel
      Left = 147
      Top = 8
      Width = 91
      Height = 16
      Caption = 'Stratasys Ltd'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label2: TLabel
      Left = 146
      Top = 105
      Width = 52
      Height = 13
      Caption = 'Build Date:'
      IsControl = True
    end
    object Label7: TLabel
      Left = 146
      Top = 124
      Width = 52
      Height = 13
      Caption = 'Build Time:'
      IsControl = True
    end
    object BuildDateLabel: TLabel
      Left = 263
      Top = 105
      Width = 18
      Height = 13
      Caption = '???'
    end
    object BuildTimeLabel: TLabel
      Left = 263
      Top = 124
      Width = 18
      Height = 13
      Caption = '???'
    end
    object Label4: TLabel
      Left = 146
      Top = 142
      Width = 105
      Height = 13
      Caption = 'Elapsed Printing Time:'
    end
    object ElapsedPrintTimeLabel: TLabel
      Left = 263
      Top = 142
      Width = 18
      Height = 13
      Caption = '???'
    end
    object Label6: TLabel
      Left = 146
      Top = 32
      Width = 76
      Height = 13
      Caption = 'Computer Name'
      IsControl = True
    end
    object ComputerNameLabel: TLabel
      Left = 263
      Top = 32
      Width = 46
      Height = 13
      Caption = 'Eden 260'
    end
    object Label21: TLabel
      Left = 146
      Top = 87
      Width = 39
      Height = 13
      Caption = 'Release'
    end
    object ReleaseNumLabel: TLabel
      Left = 263
      Top = 90
      Width = 18
      Height = 13
      Caption = '???'
    end
    object GroupBox2: TGroupBox
      Left = 142
      Top = 165
      Width = 199
      Height = 49
      TabOrder = 0
      object Label18: TLabel
        Left = 4
        Top = 10
        Width = 77
        Height = 13
        Caption = 'Next PM type is '
      end
      object PMType: TLabel
        Left = 83
        Top = 10
        Width = 29
        Height = 13
        Caption = '3500'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label20: TLabel
        Left = 4
        Top = 29
        Width = 61
        Height = 13
        Caption = 'PM is due in '
      end
      object PMDueIn: TLabel
        Left = 65
        Top = 29
        Width = 29
        Height = 13
        Caption = '3500'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label22: TLabel
        Left = 99
        Top = 29
        Width = 17
        Height = 13
        Caption = 'hrs.'
      end
      object ResetPM: TPanel
        Left = 128
        Top = 10
        Width = 63
        Height = 32
        Caption = 'PM is done'
        Ctl3D = True
        ParentCtl3D = False
        TabOrder = 0
        OnClick = ResetPMClick
        OnMouseDown = ResetPMMouseDown
        OnMouseUp = ResetPMMouseUp
      end
    end
  end
  object OKButton: TButton
    Left = 136
    Top = 407
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
    OnClick = OKButtonClick
  end
  object PageControl2: TPageControl
    Left = 4
    Top = 236
    Width = 349
    Height = 165
    ActivePage = Patches
    TabOrder = 2
    object Versions: TTabSheet
      Caption = 'Versions'
      object GroupBox1: TGroupBox
        Left = 3
        Top = 2
        Width = 335
        Height = 132
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object OCBSWLabel: TLabel
          Left = 100
          Top = 11
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label1: TLabel
          Left = 10
          Top = 11
          Width = 46
          Height = 13
          Caption = 'OCB SW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label5: TLabel
          Left = 10
          Top = 44
          Width = 55
          Height = 13
          Caption = 'OHDB SW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object OHDBSWLabel: TLabel
          Left = 100
          Top = 44
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label8: TLabel
          Left = 10
          Top = 61
          Width = 56
          Height = 13
          Caption = 'OHDB HW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object OHDBHWLabel: TLabel
          Left = 100
          Top = 61
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object MotorSWLabel: TLabel
          Left = 160
          Top = 28
          Width = 47
          Height = 13
          Caption = 'MCB SW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label11: TLabel
          Left = 160
          Top = 62
          Width = 53
          Height = 13
          Caption = 'EDEN PCI:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object MCBSWLabel: TLabel
          Left = 250
          Top = 28
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object EdenPCILabel: TLabel
          Left = 250
          Top = 62
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object MotorHWLabel: TLabel
          Left = 160
          Top = 45
          Width = 48
          Height = 13
          Caption = 'MCB HW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object MCBHWLabel: TLabel
          Left = 250
          Top = 45
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label9: TLabel
          Left = 10
          Top = 27
          Width = 49
          Height = 13
          Caption = 'OCB A2D:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label10: TLabel
          Left = 160
          Top = 11
          Width = 58
          Height = 13
          Caption = 'OHDB A2D:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object OCBA2DLabel: TLabel
          Left = 100
          Top = 27
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object OHDBA2DLabel: TLabel
          Left = 250
          Top = 11
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label17: TLabel
          Left = 160
          Top = 95
          Width = 52
          Height = 13
          Caption = 'HASP API:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object HaspAPILabel: TLabel
          Left = 250
          Top = 95
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object PCIDriverLabel: TLabel
          Left = 250
          Top = 79
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
        end
        object Label19: TLabel
          Left = 160
          Top = 78
          Width = 51
          Height = 13
          Caption = 'PCI Driver:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object MSC1HWLabel: TLabel
          Left = 100
          Top = 79
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object MSC2HWLabel: TLabel
          Left = 100
          Top = 95
          Width = 18
          Height = 13
          Caption = '???'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label24: TLabel
          Left = 10
          Top = 95
          Width = 80
          Height = 13
          Caption = 'MSC2_HW+FW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object Label23: TLabel
          Left = 10
          Top = 79
          Width = 80
          Height = 13
          Caption = 'MSC1_HW+FW:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object Patches: TTabSheet
      Caption = 'Software Updates'
      ImageIndex = 1
      object TGroupBox
        Left = 3
        Top = 3
        Width = 335
        Height = 131
        TabOrder = 0
        object Label14: TLabel
          Left = 113
          Top = 7
          Width = 41
          Height = 13
          Caption = 'Version :'
        end
        object Label15: TLabel
          Left = 113
          Top = 25
          Width = 55
          Height = 13
          Caption = 'Build Date :'
        end
        object Label16: TLabel
          Left = 113
          Top = 45
          Width = 59
          Height = 13
          Caption = 'Description :'
        end
        object PatchVersion: TLabel
          Left = 180
          Top = 7
          Width = 30
          Height = 13
          Caption = '          '
          Color = clBtnFace
          ParentColor = False
        end
        object PatchBuildDate: TLabel
          Left = 180
          Top = 25
          Width = 30
          Height = 13
          Caption = '          '
          Color = clBtnFace
          ParentColor = False
        end
        object PatchDescription: TLabel
          Left = 180
          Top = 45
          Width = 149
          Height = 62
          AutoSize = False
          Caption = 'PatchDescription'
          WordWrap = True
        end
        object PatchesList: TListBox
          Left = 2
          Top = 7
          Width = 100
          Height = 121
          BevelInner = bvNone
          BevelKind = bkFlat
          BorderStyle = bsNone
          Color = clBtnHighlight
          Ctl3D = True
          ItemHeight = 13
          ParentCtl3D = False
          TabOrder = 0
          OnClick = PatchesListClick
        end
      end
    end
  end
end
