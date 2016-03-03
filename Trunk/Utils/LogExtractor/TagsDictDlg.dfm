object TagsDictForm: TTagsDictForm
  Left = 326
  Top = 268
  BorderStyle = bsDialog
  Caption = 'Tags dictionary'
  ClientHeight = 209
  ClientWidth = 330
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object BitBtn1: TBitBtn
    Left = 248
    Top = 9
    Width = 75
    Height = 25
    TabOrder = 0
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 248
    Top = 41
    Width = 75
    Height = 25
    TabOrder = 1
    Kind = bkCancel
  end
  object TagsCheckListBox: TCheckListBox
    Left = 8
    Top = 9
    Width = 225
    Height = 192
    ItemHeight = 13
    PopupMenu = PopupMenu1
    TabOrder = 2
  end
  object PopupMenu1: TPopupMenu
    Left = 272
    Top = 96
    object ClearAllMenuItem: TMenuItem
      Caption = 'Clear all'
      OnClick = ClearAllMenuItemClick
    end
    object SelectAllMenuItem: TMenuItem
      Caption = 'Select all'
      OnClick = SelectAllMenuItemClick
    end
  end
end
