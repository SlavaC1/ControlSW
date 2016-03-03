object Form1: TForm1
  Left = 220
  Top = 188
  Width = 696
  Height = 480
  Caption = 'EncryptedLogViewer'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 688
    Height = 434
    Align = alClient
    Lines.Strings = (
      '')
    ScrollBars = ssBoth
    TabOrder = 0
  end
  object OpenDialog1: TOpenDialog
    Left = 632
    Top = 8
  end
  object MainMenu1: TMainMenu
    Left = 592
    Top = 8
    object File1: TMenuItem
      Caption = 'File'
      object Open1: TMenuItem
        Caption = '&Open'
        ShortCut = 16463
        OnClick = Open1Click
      end
      object Exit1: TMenuItem
        Caption = '&Exit'
        OnClick = Exit1Click
      end
    end
    object Edit1: TMenuItem
      Caption = 'Edit'
      object Find1: TMenuItem
        Action = SearchFind1
      end
      object FindNext1: TMenuItem
        Action = SearchFindNext1
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      object About1: TMenuItem
        Caption = '&About...'
        OnClick = About1Click
      end
    end
  end
  object ActionManager1: TActionManager
    Left = 552
    Top = 8
    object SearchFindNext1: TSearchFindNext
      Category = 'Search'
      Caption = 'Find &Next'
      Hint = 'Find Next|Repeats the last find'
      ImageIndex = 33
      SearchFind = SearchFind1
      ShortCut = 114
    end
    object SearchFind1: TSearchFind
      Category = 'Search'
      Caption = '&Find...'
      Dialog.OnFind = SearchFind1FindDialogFind
      Hint = 'Find|Finds the specified text'
      ImageIndex = 34
      ShortCut = 16454
    end
  end
end
