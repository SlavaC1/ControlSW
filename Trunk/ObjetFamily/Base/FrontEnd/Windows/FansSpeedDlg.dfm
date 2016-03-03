object FansSpeedDlg: TFansSpeedDlg
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Fans Speed'
  ClientHeight = 509
  ClientWidth = 324
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object pnlFansSpeedTextBoxes: TPanel
    Left = 0
    Top = 0
    Width = 364
    Height = 513
    TabOrder = 0
    object lbl1: TLabel
      Tag = 11
      Left = 40
      Top = 40
      Width = 101
      Height = 13
      Caption = 'Left UV Fan 1 speed:'
    end
    object lbl2: TLabel
      Tag = 12
      Left = 40
      Top = 67
      Width = 101
      Height = 13
      Caption = 'Left UV Fan 2 speed:'
    end
    object lbl3: TLabel
      Tag = 13
      Left = 40
      Top = 94
      Width = 107
      Height = 13
      Caption = 'Right UV Fan 1 speed:'
    end
    object lbl4: TLabel
      Tag = 14
      Left = 40
      Top = 121
      Width = 107
      Height = 13
      Caption = 'Right UV Fan 2 speed:'
    end
    object lbl5: TLabel
      Tag = 15
      Left = 40
      Top = 176
      Width = 126
      Height = 13
      Caption = 'Left Material Fan 1 speed:'
    end
    object lbl6: TLabel
      Tag = 16
      Left = 40
      Top = 203
      Width = 126
      Height = 13
      Caption = 'Left Material Fan 2 speed:'
    end
    object lbl7: TLabel
      Tag = 17
      Left = 40
      Top = 230
      Width = 126
      Height = 13
      Caption = 'Left Material Fan 3 speed:'
    end
    object lbl8: TLabel
      Tag = 18
      Left = 40
      Top = 257
      Width = 132
      Height = 13
      Caption = 'Right Material Fan 1 speed:'
    end
    object lbl9: TLabel
      Tag = 19
      Left = 40
      Top = 284
      Width = 132
      Height = 13
      Caption = 'Right Material Fan 2 speed:'
    end
    object lbl10: TLabel
      Tag = 20
      Left = 40
      Top = 344
      Width = 115
      Height = 13
      Caption = 'Main Block Fan 1 speed:'
    end
    object lbl11: TLabel
      Tag = 21
      Left = 255
      Top = 40
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl12: TLabel
      Tag = 22
      Left = 255
      Top = 67
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl13: TLabel
      Tag = 23
      Left = 255
      Top = 94
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl14: TLabel
      Tag = 24
      Left = 255
      Top = 121
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl15: TLabel
      Tag = 25
      Left = 255
      Top = 176
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl16: TLabel
      Tag = 26
      Left = 255
      Top = 203
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl17: TLabel
      Tag = 27
      Left = 255
      Top = 230
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl18: TLabel
      Tag = 28
      Left = 255
      Top = 257
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl19: TLabel
      Tag = 29
      Left = 255
      Top = 284
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object lbl20: TLabel
      Tag = 30
      Left = 255
      Top = 344
      Width = 21
      Height = 13
      Caption = 'RPM'
    end
    object edtLeftUvFan1: TEdit
      Left = 179
      Top = 40
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 0
    end
    object edtLeftUvFan2: TEdit
      Tag = 1
      Left = 179
      Top = 67
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 1
    end
    object edtRightUvFan1: TEdit
      Tag = 5
      Left = 179
      Top = 94
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 2
    end
    object edtRightUvFan2: TEdit
      Tag = 6
      Left = 179
      Top = 121
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 3
    end
    object edtLeftMaterialFan1: TEdit
      Tag = 2
      Left = 179
      Top = 176
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 4
    end
    object edtLeftMaterialFan2: TEdit
      Tag = 3
      Left = 179
      Top = 203
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 5
    end
    object edtLeftMaterialFan3: TEdit
      Tag = 4
      Left = 179
      Top = 230
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 6
    end
    object edtRightMaterialFan1: TEdit
      Tag = 7
      Left = 179
      Top = 257
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 7
    end
    object edtRightMaterialFan2: TEdit
      Tag = 8
      Left = 179
      Top = 284
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 8
    end
    object edtMiddleFan: TEdit
      Tag = 9
      Left = 179
      Top = 344
      Width = 70
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 9
    end
    object chkMainFanSpeedDontShowWarningMessageAgain: TCheckBox
      Tag = 10
      Left = 40
      Top = 456
      Width = 244
      Height = 17
      Caption = 'Main Fan Speed: Don'#39't show warning message.'
      TabOrder = 10
      OnClick = chkMainFanSpeedDontShowWarningMessageAgainClick
    end
  end
end
