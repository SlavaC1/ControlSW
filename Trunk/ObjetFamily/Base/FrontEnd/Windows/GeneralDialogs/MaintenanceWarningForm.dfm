object MaintenanceUserWarningForm: TMaintenanceUserWarningForm
  Left = 212
  Top = 123
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Access Confirmation Message'
  ClientHeight = 260
  ClientWidth = 326
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 4
    Top = 4
    Width = 317
    Height = 217
    TabOrder = 0
    object AttentionLabel: TLabel
      Left = 95
      Top = 4
      Width = 126
      Height = 32
      Caption = 'Attention!'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clRed
      Font.Height = -29
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
    end
    object Label1: TLabel
      Left = 28
      Top = 47
      Width = 204
      Height = 21
      Caption = 'Access to this Maintenance'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 28
      Top = 71
      Width = 240
      Height = 21
      Caption = 'Screen is prohibited to all users!'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentFont = False
    end
    object Label3: TLabel
      Left = 28
      Top = 106
      Width = 257
      Height = 19
      Caption = 'Users may enter this screen only under full '
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentFont = False
    end
    object Label4: TLabel
      Left = 28
      Top = 128
      Width = 278
      Height = 19
      Caption = 'supervision and guidance of authorized service'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentFont = False
    end
    object Label5: TLabel
      Left = 28
      Top = 151
      Width = 245
      Height = 19
      Caption = 'personnel to avoid any damage or injury.'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentFont = False
    end
    object Label6: TLabel
      Left = 28
      Top = 187
      Width = 244
      Height = 19
      Caption = 'Click [OK] to accept or [Cancel] to exit'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentFont = False
    end
  end
  object WarningOKButton: TButton
    Left = 72
    Top = 229
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
  object WarningCancelButton: TButton
    Left = 178
    Top = 229
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
end
