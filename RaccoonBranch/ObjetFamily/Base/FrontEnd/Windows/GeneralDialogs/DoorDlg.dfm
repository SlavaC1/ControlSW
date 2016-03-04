object DoorCloseDlg: TDoorCloseDlg
  Left = 297
  Top = 277
  BorderStyle = bsDialog
  Caption = 'Warning'
  ClientHeight = 136
  ClientWidth = 308
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 13
    Top = 16
    Width = 284
    Height = 23
    Alignment = taCenter
    AutoSize = False
    Caption = 'Printer cover is open.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 47
    Width = 297
    Height = 18
    Alignment = taCenter
    AutoSize = False
    Caption = 'To continue, close the cover.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 8
    Top = 78
    Width = 289
    Height = 18
    Alignment = taCenter
    AutoSize = False
    Caption = 'Press '#39'Cancel'#39' to stop.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object DoorCancelBitBtn1: TBitBtn
    Left = 115
    Top = 103
    Width = 75
    Height = 25
    TabOrder = 0
    OnClick = DoorCancelBitBtn1Click
    Kind = bkCancel
  end
end
