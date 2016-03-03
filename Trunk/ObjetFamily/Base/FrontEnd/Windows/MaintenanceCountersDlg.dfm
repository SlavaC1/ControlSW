object MaintenanceCountersForm: TMaintenanceCountersForm
  Left = 384
  Top = 110
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Maintenance Counters (Hours)'
  ClientHeight = 566
  ClientWidth = 542
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object TotalTimeLabel: TLabel
    Tag = 100
    Left = 126
    Top = 9
    Width = 87
    Height = 13
    AutoSize = False
    Caption = 'Total'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ItemLabel: TLabel
    Tag = 100
    Left = 14
    Top = 9
    Width = 25
    Height = 13
    Caption = 'Item'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object DatedLabel: TLabel
    Tag = 100
    Left = 199
    Top = 9
    Width = 35
    Height = 13
    Caption = 'Dated'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PrintingHoursLabel: TLabel
    Left = 14
    Top = 35
    Width = 84
    Height = 13
    Caption = 'Total Printing time'
  end
  object WarningTimeLabel: TLabel
    Tag = 100
    Left = 280
    Top = 9
    Width = 79
    Height = 13
    Caption = 'Warning Time'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Bevel2: TBevel
    Left = 0
    Top = 0
    Width = 542
    Height = 9
    Align = alTop
    Shape = bsTopLine
  end
  object UserWarningLabel: TLabel
    Tag = 100
    Left = 423
    Top = 9
    Width = 78
    Height = 13
    Caption = 'User Warning'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PrintingHoursEdit: TEdit
    Left = 126
    Top = 31
    Width = 65
    Height = 21
    Enabled = False
    TabOrder = 0
  end
  object PrintingDateEdit: TEdit
    Left = 199
    Top = 31
    Width = 73
    Height = 21
    Enabled = False
    TabOrder = 1
  end
  object CloseBitBtn: TBitBtn
    Left = 186
    Top = 532
    Width = 70
    Height = 26
    Cancel = True
    Caption = 'Close'
    TabOrder = 2
    OnClick = CloseBitBtnClick
  end
  object PrintingWarnEdit: TEdit
    Left = 280
    Top = 31
    Width = 73
    Height = 21
    Enabled = False
    TabOrder = 3
  end
  object CountersScrollBox: TScrollBox
    Left = 13
    Top = 61
    Width = 513
    Height = 463
    VertScrollBar.Tracking = True
    BevelInner = bvNone
    BevelOuter = bvNone
    TabOrder = 4
  end
  object btnSave: TBitBtn
    Left = 262
    Top = 532
    Width = 70
    Height = 26
    Cancel = True
    Caption = 'Save'
    TabOrder = 5
    OnClick = btnSaveClick
  end
  object MainMenu1: TMainMenu
    Left = 7
    Top = 532
    object File1: TMenuItem
      Caption = 'File'
      GroupIndex = 1
      object SaveToFileMenuItem: TMenuItem
        Caption = 'Save To File'
        OnClick = SaveToFileMenuItemClick
      end
    end
    object Units1: TMenuItem
      Caption = 'Units'
      GroupIndex = 1
      object DaysMenuItem: TMenuItem
        Caption = 'Days'
        GroupIndex = 1
        RadioItem = True
        OnClick = DaysMenuItemClick
      end
      object HoursMenuItem: TMenuItem
        Caption = 'Hours'
        Checked = True
        GroupIndex = 1
        RadioItem = True
        OnClick = HoursMenuItemClick
      end
      object MinutesMenuItem: TMenuItem
        Caption = 'Minutes'
        GroupIndex = 1
        RadioItem = True
        OnClick = MinutesMenuItemClick
      end
      object SecondsMenuItem: TMenuItem
        Caption = 'Seconds'
        GroupIndex = 1
        RadioItem = True
        OnClick = SecondsMenuItemClick
      end
    end
  end
  object ActionList1: TActionList
    Left = 39
    Top = 532
    object EnterEditModeAction: TAction
      Caption = 'EnterEditModeAction'
      ShortCut = 49223
      OnExecute = EnterEditModeActionExecute
    end
    object OpenUserWarningAction: TAction
      Caption = 'OpenUserWarningAction'
      ShortCut = 49239
      OnExecute = OpenUserWarningActionExecute
    end
    object SaveValuesAction: TAction
      Caption = 'SaveValuesAction'
      ShortCut = 49235
      OnExecute = SaveValuesActionExecute
    end
    object ApplyWarningTimeAction: TAction
      Caption = 'ApplyWarningTimeAction'
      ShortCut = 49217
      OnExecute = ApplyWarningTimeActionExecute
    end
  end
  object WarnFieldPopupMenu: TPopupMenu
    Left = 449
    Top = 532
    object RestoreDefaultRestoreDefaultMenuItem: TMenuItem
      Caption = 'Restore Default'
      OnClick = RestoreDefaultRestoreDefaultMenuItemClick
    end
  end
  object MaintenanceSaveDialog: TSaveDialog
    Filter = 'Text Files|*.txt'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 477
    Top = 532
  end
end
