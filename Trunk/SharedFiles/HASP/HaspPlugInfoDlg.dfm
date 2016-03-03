object HaspPlugInformationDlg: THaspPlugInformationDlg
  Left = 481
  Top = 290
  BorderStyle = bsDialog
  Caption = 'HASP'
  ClientHeight = 296
  ClientWidth = 350
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PacksVersionLabel: TLabel
    Left = 8
    Top = 236
    Width = 173
    Height = 16
    Caption = 'Packages database version:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object OKButton: TButton
    Left = 138
    Top = 263
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKButtonClick
  end
  object HelpButton: TButton
    Left = 267
    Top = 263
    Width = 75
    Height = 25
    Caption = 'Help'
    Default = True
    TabOrder = 1
    OnClick = HelpButtonClick
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 7
    Width = 334
    Height = 219
    ActivePage = LicensedPackagesTab
    TabOrder = 2
    object PlugPropertiesTab: TTabSheet
      Caption = 'Plug properties'
      object HaspInfoGrid: TValueListEditor
        Left = 0
        Top = 0
        Width = 326
        Height = 191
        Align = alClient
        DisplayOptions = [doAutoColResize, doKeyColFixed]
        GridLineWidth = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goThumbTracking]
        TabOrder = 0
        ColWidths = (
          150
          170)
      end
    end
    object LicensedPackagesTab: TTabSheet
      Caption = 'Licensed packages'
      ImageIndex = 1
      object PackagesTreeView: TTreeView
        Left = 0
        Top = 0
        Width = 326
        Height = 191
        Align = alClient
        Indent = 19
        ReadOnly = True
        TabOrder = 0
      end
    end
  end
end
