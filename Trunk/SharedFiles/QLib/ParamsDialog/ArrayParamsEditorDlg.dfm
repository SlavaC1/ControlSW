object ArrayParamsEditorForm: TArrayParamsEditorForm
  Left = 278
  Top = 175
  BorderStyle = bsDialog
  Caption = 'Array Parameter Editor -  '
  ClientHeight = 264
  ClientWidth = 411
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object StringGrid1: TStringGrid
    Left = 3
    Top = 4
    Width = 318
    Height = 255
    ColCount = 3
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goEditing, goThumbTracking]
    TabOrder = 0
    OnSelectCell = StringGrid1SelectCell
    ColWidths = (
      81
      84
      147)
  end
  object OkButton: TBitBtn
    Left = 331
    Top = 4
    Width = 75
    Height = 25
    Caption = '&Ok'
    TabOrder = 1
    OnClick = OkButtonClick
    Kind = bkOK
  end
  object CancelButton: TBitBtn
    Left = 331
    Top = 36
    Width = 75
    Height = 25
    Caption = '&Cancel'
    TabOrder = 2
    Kind = bkCancel
  end
end
