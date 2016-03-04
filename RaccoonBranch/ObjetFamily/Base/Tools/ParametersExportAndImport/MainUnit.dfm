object MainForm: TMainForm
  Left = 108
  Top = 111
  Width = 277
  Height = 188
  Caption = 'MainForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
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
    Visible = False
  end
  object Label2: TLabel
    Left = 143
    Top = 8
    Width = 48
    Height = 13
    Caption = 'For Export'
    Visible = False
  end
  object AllParamsListBox: TListBox
    Left = 7
    Top = 24
    Width = 114
    Height = 89
    DragMode = dmAutomatic
    ItemHeight = 13
    MultiSelect = True
    Sorted = True
    TabOrder = 0
    Visible = False
  end
  object ForExportListBox: TListBox
    Left = 143
    Top = 24
    Width = 114
    Height = 89
    ItemHeight = 13
    MultiSelect = True
    PopupMenu = PopupMenu1
    TabOrder = 1
    Visible = False
  end
  object CloseButton: TButton
    Left = 190
    Top = 122
    Width = 66
    Height = 25
    Caption = 'Close'
    TabOrder = 2
    Visible = False
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'cfg'
    Filter = 'cfg files|*.cfg|All files|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 117
    Top = 96
  end
  object PopupMenu1: TPopupMenu
    Left = 117
    Top = 64
    object Clear1: TMenuItem
      Caption = 'Clear '
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Export1: TMenuItem
      Caption = 'Export...'
    end
  end
  object ActionList1: TActionList
    Left = 117
    Top = 32
    object OpenParametersDialogAction: TAction
      Caption = 'OpenParametersDialogAction'
      ShortCut = 49232
    end
  end
end
