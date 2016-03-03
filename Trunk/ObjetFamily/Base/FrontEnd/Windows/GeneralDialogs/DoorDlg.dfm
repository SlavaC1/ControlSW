object DoorCloseDlg: TDoorCloseDlg
  Left = 297
  Top = 277
  BorderStyle = bsDialog
  Caption = 'Warning'
  ClientHeight = 136
  ClientWidth = 322
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
    Left = 8
    Top = 40
    Width = 292
    Height = 23
    Alignment = taCenter
    AutoSize = False
    Caption = 'Printer cover is open.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 1
    Top = 77
    Width = 313
    Height = 26
    Alignment = taCenter
    AutoSize = False
    Caption = 'To continue, close the cover.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
  end
end
