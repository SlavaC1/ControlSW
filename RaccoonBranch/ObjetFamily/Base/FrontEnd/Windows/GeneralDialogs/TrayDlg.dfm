object TrayInsertDlg: TTrayInsertDlg
  Left = 254
  Top = 189
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'TrayInsertDlg'
  ClientHeight = 158
  ClientWidth = 444
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 12
    Top = 38
    Width = 420
    Height = 23
    Alignment = taCenter
    AutoSize = False
    Caption = 'Printer tray is out.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 12
    Top = 77
    Width = 420
    Height = 18
    Alignment = taCenter
    AutoSize = False
    Caption = 'To continue, insert the tray.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 8
    Top = 8
  end
end
