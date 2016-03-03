object BreakpointPropertiesForm: TBreakpointPropertiesForm
  Left = 302
  Top = 301
  BorderStyle = bsDialog
  Caption = 'Breakpoint Properties'
  ClientHeight = 129
  ClientWidth = 444
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
  object Bevel1: TBevel
    Left = 8
    Top = 8
    Width = 337
    Height = 113
  end
  object BrkNameEdit: TLabeledEdit
    Left = 16
    Top = 33
    Width = 305
    Height = 21
    EditLabel.Width = 85
    EditLabel.Height = 13
    EditLabel.Caption = 'Breakpoint Name:'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 0
  end
  object BrkConEdit: TLabeledEdit
    Left = 16
    Top = 89
    Width = 305
    Height = 21
    EditLabel.Width = 101
    EditLabel.Height = 13
    EditLabel.Caption = 'Breakpoint Condition:'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 1
  end
  object BitBtn1: TBitBtn
    Left = 360
    Top = 8
    Width = 75
    Height = 25
    Caption = '&Ok'
    TabOrder = 2
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 360
    Top = 40
    Width = 75
    Height = 25
    Caption = '&Cancel'
    TabOrder = 3
    Kind = bkCancel
  end
end
