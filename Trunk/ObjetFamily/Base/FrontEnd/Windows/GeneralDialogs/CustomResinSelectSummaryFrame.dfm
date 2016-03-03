object CustomResinSelectSummaryFrame: TCustomResinSelectSummaryFrame
  Left = 0
  Top = 0
  Width = 1076
  Height = 460
  Padding.Left = 5
  Padding.Top = 5
  Padding.Right = 5
  Padding.Bottom = 5
  TabOrder = 0
  object TitlePanel: TPanel
    Left = 5
    Top = 5
    Width = 1066
    Height = 36
    Align = alTop
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvNone
    TabOrder = 0
    object lblMainTitle: TLabel
      Left = 0
      Top = 0
      Width = 1066
      Height = 19
      Align = alTop
      Alignment = taCenter
      BiDiMode = bdLeftToRight
      Caption = 'Cartridge Positioning and Process Progression'
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentBiDiMode = False
      ParentColor = False
      ParentFont = False
      ExplicitWidth = 376
    end
  end
  object ProgressGroupBox: TGroupBox
    AlignWithMargins = True
    Left = 5
    Top = 386
    Width = 1066
    Height = 69
    Margins.Left = 0
    Margins.Top = 5
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alBottom
    Caption = 'Replacement progress'
    Ctl3D = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    Padding.Left = 5
    Padding.Top = 5
    Padding.Right = 5
    Padding.Bottom = 5
    ParentCtl3D = False
    ParentFont = False
    TabOrder = 2
    object pbMaterialReplacementProgressBar: TGauge
      Left = 7
      Top = 20
      Width = 1052
      Height = 20
      Align = alTop
      Color = clBtnFace
      ForeColor = clSkyBlue
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Progress = 0
      ExplicitLeft = 32
      ExplicitTop = 11
      ExplicitWidth = 413
    end
    object lblEtaProgressBar: TLabel
      Left = 7
      Top = 42
      Width = 1052
      Height = 20
      Align = alBottom
      Alignment = taCenter
      AutoSize = False
      Caption = 'Remaining Time: XX:XX  min'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
      ExplicitLeft = 32
      ExplicitTop = 29
      ExplicitWidth = 409
    end
  end
  inline TanksSummaryFrame: TTanksSummaryFrame
    Left = 5
    Top = 41
    Width = 1066
    Height = 340
    Align = alClient
    TabOrder = 1
    ExplicitLeft = 5
    ExplicitTop = 41
    ExplicitWidth = 1066
    ExplicitHeight = 340
    inherited TanksGroupBox: TGroupBox
      Width = 1066
      Height = 340
      Caption = 
        'Insert the new material cartridge as shown below, and click '#39'Sta' +
        'rt...'#39
      ParentFont = False
      ExplicitWidth = 1066
      ExplicitHeight = 340
      inherited OpModePanel: TPanel
        Top = 297
        Width = 1052
        ExplicitTop = 297
        ExplicitWidth = 1052
        inherited btnStart: TButton
          Left = 960
          ExplicitLeft = 960
        end
      end
    end
  end
  object CartridgesRemark: TPanel
    Left = 16
    Top = 344
    Width = 441
    Height = 28
    Alignment = taLeftJustify
    BevelOuter = bvNone
    Font.Charset = ANSI_CHARSET
    Font.Color = clRed
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
  end
end
