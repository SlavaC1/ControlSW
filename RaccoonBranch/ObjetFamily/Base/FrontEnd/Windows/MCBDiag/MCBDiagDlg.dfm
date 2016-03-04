object MCBDiagForm: TMCBDiagForm
  Left = 448
  Top = 225
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'MCB SW Parameters'
  ClientHeight = 440
  ClientWidth = 383
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object CloseButton: TButton
    Left = 158
    Top = 408
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 0
    OnClick = CloseButtonClick
  end
  object DiagText: TMemo
    Left = 8
    Top = 128
    Width = 369
    Height = 265
    Lines.Strings = (
      '')
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 369
    Height = 113
    ActivePage = TabSheet1
    TabOrder = 2
    object TabSheet1: TTabSheet
      Caption = 'SW Parameters'
      object Label1: TLabel
        Left = 31
        Top = 8
        Width = 48
        Height = 13
        Caption = 'Parameter'
      end
      object Label2: TLabel
        Left = 103
        Top = 8
        Width = 27
        Height = 13
        Caption = 'Value'
      end
      object DiagParameterEdit: TEdit
        Left = 23
        Top = 24
        Width = 73
        Height = 21
        TabOrder = 0
      end
      object DiagValueEdit: TEdit
        Left = 103
        Top = 24
        Width = 73
        Height = 21
        TabOrder = 1
      end
      object ReadButton: TButton
        Left = 207
        Top = 8
        Width = 65
        Height = 25
        Caption = 'Read'
        TabOrder = 2
        OnClick = ReadButtonClick
      end
      object WriteButton: TButton
        Left = 207
        Top = 36
        Width = 65
        Height = 25
        Caption = 'Write'
        TabOrder = 3
        OnClick = WriteButtonClick
      end
      object ReadAllButton: TButton
        Left = 279
        Top = 8
        Width = 65
        Height = 25
        Caption = 'Read All'
        TabOrder = 4
        OnClick = ReadAllButtonClick
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'SW Download'
      ImageIndex = 1
      object Label3: TLabel
        Left = 8
        Top = 9
        Width = 59
        Height = 13
        Caption = 'File location:'
      end
      object Edit1: TEdit
        Left = 9
        Top = 25
        Width = 289
        Height = 21
        ReadOnly = True
        TabOrder = 0
        Text = '<empty>'
      end
      object Browse: TButton
        Left = 300
        Top = 26
        Width = 52
        Height = 20
        Caption = 'Browse...'
        TabOrder = 1
        OnClick = BrowseClick
      end
      object MCBSWLoadStart: TButton
        Left = 8
        Top = 54
        Width = 57
        Height = 25
        Caption = 'Start'
        Enabled = False
        TabOrder = 2
        OnClick = MCBSWLoadStartClick
      end
      object ProgressBar1: TProgressBar
        Left = 72
        Top = 56
        Width = 280
        Height = 21
        Smooth = True
        Step = 1
        TabOrder = 3
      end
    end
  end
  object Button1: TButton
    Left = 8
    Top = 408
    Width = 73
    Height = 25
    Caption = 'Help'
    TabOrder = 3
    Visible = False
    OnClick = Button1Click
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 352
    Top = 8
  end
end
