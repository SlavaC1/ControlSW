object ParamsExportForm: TParamsExportForm
  Left = 363
  Top = 105
  BorderStyle = bsDialog
  Caption = 'Configuration Export'
  ClientHeight = 385
  ClientWidth = 447
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
  object Label1: TLabel
    Left = 7
    Top = 8
    Width = 67
    Height = 13
    Caption = 'All Parameters'
  end
  object Label2: TLabel
    Left = 247
    Top = 8
    Width = 48
    Height = 13
    Caption = 'For Export'
  end
  object AddToSelectionButton: TSpeedButton
    Left = 211
    Top = 112
    Width = 25
    Height = 24
    Caption = '>'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    OnClick = AddToSelectionButtonClick
  end
  object RemoveFromSelectionButton: TSpeedButton
    Left = 211
    Top = 144
    Width = 25
    Height = 24
    Caption = '<'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    OnClick = RemoveFromSelectionButtonClick
  end
  object Bevel1: TBevel
    Left = 247
    Top = 284
    Width = 193
    Height = 53
  end
  object AllParamsListBox: TListBox
    Left = 7
    Top = 24
    Width = 193
    Height = 241
    DragMode = dmAutomatic
    ItemHeight = 13
    MultiSelect = True
    Sorted = True
    TabOrder = 0
    OnDblClick = AllParamsListBoxDblClick
  end
  object ForExportListBox: TListBox
    Left = 247
    Top = 24
    Width = 193
    Height = 241
    ItemHeight = 13
    MultiSelect = True
    PopupMenu = PopupMenu1
    TabOrder = 1
    OnDragDrop = ForExportListBoxDragDrop
    OnDragOver = ForExportListBoxDragOver
  end
  object GroupBox1: TGroupBox
    Left = 7
    Top = 280
    Width = 193
    Height = 97
    Caption = 'Selection Set'
    TabOrder = 2
    object Label3: TLabel
      Left = 9
      Top = 52
      Width = 32
      Height = 13
      Caption = 'Modes'
    end
    object AllButton: TButton
      Left = 7
      Top = 24
      Width = 41
      Height = 25
      Caption = 'All'
      TabOrder = 0
      OnClick = AllButtonClick
    end
    object NoneButton: TButton
      Left = 54
      Top = 24
      Width = 41
      Height = 25
      Caption = 'None'
      TabOrder = 1
      OnClick = NoneButtonClick
    end
    object ModesComboBox: TComboBox
      Left = 8
      Top = 70
      Width = 177
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
      OnClick = ModesComboBoxClick
      Items.Strings = (
        '- None -')
    end
  end
  object SaveAsButton: TButton
    Left = 379
    Top = 298
    Width = 55
    Height = 25
    Caption = 'Save as...'
    TabOrder = 3
    OnClick = SaveAsButtonClick
  end
  object CloseButton: TButton
    Left = 374
    Top = 354
    Width = 66
    Height = 25
    Caption = 'Close'
    TabOrder = 4
    OnClick = CloseButtonClick
  end
  object EditCollectionButton: TButton
    Left = 252
    Top = 298
    Width = 55
    Height = 25
    Caption = 'Edit'
    TabOrder = 5
    OnClick = EditCollectionButtonClick
  end
  object SaveButton: TButton
    Left = 315
    Top = 298
    Width = 55
    Height = 25
    Caption = 'Save'
    TabOrder = 6
    OnClick = SaveButtonClick
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'cfg'
    Filter = 'cfg files|*.cfg|All files|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofNoChangeDir, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 213
    Top = 296
  end
  object PopupMenu1: TPopupMenu
    Left = 213
    Top = 264
    object Clear1: TMenuItem
      Caption = 'Clear '
      OnClick = Clear1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Export1: TMenuItem
      Caption = 'Export...'
      OnClick = SaveAsButtonClick
    end
  end
  object ActionList1: TActionList
    Left = 213
    Top = 224
    object OpenParametersDialogAction: TAction
      Caption = 'OpenParametersDialogAction'
      ShortCut = 49232
      OnExecute = OpenParametersDialogActionExecute
    end
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 213
    Top = 328
  end
end
