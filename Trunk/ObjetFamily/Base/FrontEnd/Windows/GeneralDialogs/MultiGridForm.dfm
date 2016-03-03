object MultiGridForm: TMultiGridForm
  Left = 435
  Top = 55
  Anchors = []
  BorderIcons = []
  BorderStyle = bsSingle
  BorderWidth = 10
  ClientHeight = 80
  ClientWidth = 342
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object MainPanel: TPanel
    Left = 0
    Top = 0
    Width = 329
    Height = 41
    BevelOuter = bvNone
    BiDiMode = bdLeftToRight
    ParentBiDiMode = False
    TabOrder = 0
  end
  object RestoreButton: TButton
    Left = 48
    Top = 55
    Width = 75
    Height = 25
    Caption = 'Restore'
    TabOrder = 3
    Visible = False
    OnClick = RestoreButtonClick
  end
  object CancelButton: TButton
    Left = 176
    Top = 55
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object SaveButton: TButton
    Left = 264
    Top = 55
    Width = 75
    Height = 25
    Caption = 'Save'
    ModalResult = 1
    TabOrder = 1
    OnClick = SaveButtonClick
  end
end
