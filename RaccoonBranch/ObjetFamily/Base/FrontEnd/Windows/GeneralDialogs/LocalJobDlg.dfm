object LocalJobForm: TLocalJobForm
  Left = 404
  Top = 28
  BorderStyle = bsDialog
  Caption = 'Local print job settings'
  ClientHeight = 336
  ClientWidth = 770
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
  object FirstBMPGroupBox: TGroupBox
    Tag = 1
    Left = 8
    Top = 8
    Width = 377
    Height = 113
    Caption = '1-st Bitmap '
    TabOrder = 0
    object NumberOfSlices1Label: TLabel
      Tag = 1
      Left = 8
      Top = 85
      Width = 100
      Height = 13
      Caption = 'Number of slices:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object FileName1Edit: TEdit
      Tag = 1
      Left = 8
      Top = 50
      Width = 321
      Height = 21
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object OpenFileSelection1Button: TButton
      Tag = 1
      Left = 338
      Top = 50
      Width = 21
      Height = 21
      Caption = '...'
      TabOrder = 1
      OnClick = OpenFileSelection1ButtonClick
    end
    object TypeSelector1ComboBox: TComboBox
      Tag = 1
      Left = 8
      Top = 19
      Width = 321
      Height = 21
      Style = csDropDownList
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Verdana'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 2
      OnChange = TypeSelectorComboBoxChange
    end
    object NumberOfSlices1Edit: TEdit
      Tag = 1
      Left = 114
      Top = 81
      Width = 67
      Height = 21
      TabOrder = 3
      Text = '10'
    end
    object NoOfSlices1UpDown: TUpDown
      Tag = 1
      Left = 181
      Top = 81
      Width = 15
      Height = 21
      Associate = NumberOfSlices1Edit
      Min = 1
      Max = 32767
      Position = 10
      TabOrder = 4
    end
  end
  object OkButton: TBitBtn
    Left = 216
    Top = 304
    Width = 75
    Height = 25
    Caption = 'Ok'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = OkButtonClick
    Kind = bkOK
  end
  object CancelButton: TBitBtn
    Left = 296
    Top = 304
    Width = 75
    Height = 25
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    Kind = bkCancel
  end
  object SecondBMPGroupBox: TGroupBox
    Tag = 2
    Left = 8
    Top = 128
    Width = 377
    Height = 113
    Caption = '2-st Bitmap '
    TabOrder = 3
    object NumberOfSlices2Label: TLabel
      Tag = 2
      Left = 8
      Top = 85
      Width = 100
      Height = 13
      Caption = 'Number of slices:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object FileName2Edit: TEdit
      Tag = 2
      Left = 8
      Top = 50
      Width = 321
      Height = 21
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object OpenFileSelection2Button: TButton
      Tag = 2
      Left = 338
      Top = 50
      Width = 21
      Height = 21
      Caption = '...'
      TabOrder = 1
      OnClick = OpenFileSelection1ButtonClick
    end
    object TypeSelector2ComboBox: TComboBox
      Tag = 2
      Left = 8
      Top = 19
      Width = 321
      Height = 21
      Style = csDropDownList
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Verdana'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 2
      OnChange = TypeSelectorComboBoxChange
    end
    object NumberOfSlices2Edit: TEdit
      Tag = 2
      Left = 114
      Top = 81
      Width = 67
      Height = 21
      TabOrder = 3
      Text = '10'
    end
    object NoOfSlices2UpDown: TUpDown
      Tag = 2
      Left = 181
      Top = 81
      Width = 15
      Height = 21
      Associate = NumberOfSlices2Edit
      Min = 1
      Max = 32767
      Position = 10
      TabOrder = 4
    end
  end
  object PrintDoubleBMPCheckBox: TCheckBox
    Left = 8
    Top = 301
    Width = 121
    Height = 25
    Caption = 'Print Double Bitmap'
    Checked = True
    State = cbChecked
    TabOrder = 4
    OnClick = PrintDoubleBMPCheckBoxClick
  end
  object ListView1: TListView
    Left = 400
    Top = 15
    Width = 353
    Height = 95
    Columns = <
      item
        Caption = 'Type'
        Width = -1
        WidthType = (
          -1)
      end
      item
        AutoSize = True
        Caption = 'Bitmap File'
        WidthType = (
          -1)
      end>
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -8
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GridLines = True
    ParentFont = False
    TabOrder = 5
    ViewStyle = vsReport
  end
  object ListView2: TListView
    Left = 400
    Top = 135
    Width = 353
    Height = 95
    Columns = <
      item
        AutoSize = True
        Caption = 'Type'
      end
      item
        AutoSize = True
        Caption = 'Bitmap File'
      end>
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -8
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GridLines = True
    ParentFont = False
    TabOrder = 6
    ViewStyle = vsReport
  end
  object OpenDialog: TOpenPictureDialog
    DefaultExt = 'bmp'
    Filter = 'Bitmaps (*.bmp)|*.bmp|All files|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 360
    Top = 248
  end
end
