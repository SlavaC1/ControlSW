object PythonBreakpointForm: TPythonBreakpointForm
  Left = 329
  Top = 292
  BorderStyle = bsDialog
  Caption = 'Python Breakpoint'
  ClientHeight = 260
  ClientWidth = 423
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 7
    Top = 98
    Width = 33
    Height = 13
    Caption = 'Result:'
  end
  object ExpressionEdit: TLabeledEdit
    Left = 7
    Top = 67
    Width = 409
    Height = 21
    EditLabel.Width = 54
    EditLabel.Height = 13
    EditLabel.Caption = 'Expression:'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 0
  end
  object ResultMemo: TMemo
    Left = 7
    Top = 114
    Width = 409
    Height = 89
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object NewValueEdit: TLabeledEdit
    Left = 7
    Top = 232
    Width = 409
    Height = 21
    EditLabel.Width = 54
    EditLabel.Height = 13
    EditLabel.Caption = 'New value:'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 2
    OnChange = NewValueEditChange
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 423
    Height = 47
    ButtonHeight = 39
    Caption = 'ToolBar1'
    EdgeBorders = [ebTop, ebBottom]
    TabOrder = 3
    object EvaluateButton: TSpeedButton
      Left = 0
      Top = 2
      Width = 60
      Height = 39
      Action = EvaluateAction
      Flat = True
      Glyph.Data = {
        F6030000424DF603000000000000360000002800000014000000100000000100
        180000000000C0030000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        0000000000000000000000000000000000000000000000000000000000000000
        00000000C8D0D4C8D0D4000000000000C8D0D4C8D0D4C8D0D4C8D0D4000000FF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000
        C8D0D4000000000000000000C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000000000000000
        0000000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFF000000848484C6C6C6C6C6C6848484000000000000C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFF000000848484C6C6C6C6C6C6FFFF00848484848484000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
        00C6C6C6C6C6C6C6C6C6C6C6C6848484C6C6C6000000C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000C6C6C6
        FFFF00C6C6C6C6C6C6848484C6C6C6000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000848484FFFF00FF
        FF00C6C6C6848484848484000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000848484C6C6C6C6C6
        C6848484000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFF000000C6C6C6000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFF000000000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4000000000000000000000000000000000000000000000000
        000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
      Layout = blGlyphTop
    end
    object ModifyButton: TSpeedButton
      Left = 60
      Top = 2
      Width = 60
      Height = 39
      Action = ModifyAction
      Flat = True
      Glyph.Data = {
        F6030000424DF603000000000000360000002800000014000000100000000100
        180000000000C0030000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400000000
        0000000000000000000000000000000000000000000000000000000000000000
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFF000000000000
        FFFFFF000000000000000000000000000000FFFFFF000000C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFF000000000000FFFFFF0000000000
        00000000000000000000FFFFFF000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFF000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FF
        FFFFFFFFFF000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        000000FFFFFF000000000000FFFFFFFFFFFFFFFFFF000000C6C6C6000000FFFF
        FF000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000FF
        FFFF000000C6C6C6000000FFFFFF000000C6C6C6000000C6C6C6000000000000
        000000C8D0D4840000840000C8D0D4C8D0D4C8D0D4C8D0D4000000FFFFFFFFFF
        FF000000C6C6C6000000C6C6C6000000C6C6C6000000C6C6C6C6C6C6C6C6C600
        0000840000840000C8D0D4C8D0D4C8D0D4C8D0D4000000000000000000000000
        000000C6C6C6000000C6C6C6000000C6C6C6C6C6C6C6C6C6C6C6C6C6C6C68400
        00840000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400
        0000C6C6C6000000C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6840000840000
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000
        00C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6C6000000840000840000C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000
        000000000000000000000000000000C8D0D4840000840000C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
      Layout = blGlyphTop
    end
    object ToolButton1: TToolButton
      Left = 120
      Top = 2
      Width = 17
      Caption = 'ToolButton1'
      Style = tbsDivider
    end
    object ContinueButton: TSpeedButton
      Left = 137
      Top = 2
      Width = 60
      Height = 39
      Action = ContinueAction
      Flat = True
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF0000FF00000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400FF0000FF0000
        FF00000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF0000FF0000FF0000FF00000000C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400FF0000FF0000
        FF0000FF0000FF00000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF0000FF0000FF0000FF0000FF0000FF00000000C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400FF0000FF0000
        FF0000FF0000FF0000FF0000FF00000000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF0000FF0000FF0000FF0000FF0000FF0000FF00C6C6
        C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400FF0000FF0000
        FF0000FF0000FF0000FF00C6C6C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF0000FF0000FF0000FF0000FF00C6C6C6C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400FF0000FF0000
        FF0000FF00C6C6C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF0000FF0000FF00C6C6C6C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400FF0000FF00C6
        C6C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400FF00C6C6C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
      Layout = blGlyphTop
    end
    object StopButton: TSpeedButton
      Left = 197
      Top = 2
      Width = 60
      Height = 39
      Action = StopAction
      Flat = True
      Glyph.Data = {
        36030000424D3603000000000000360000002800000010000000100000000100
        18000000000000030000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        808080808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D40000FF000080000080808080C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D40000FF808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000FF
        000080000080000080808080C8D0D4C8D0D4C8D0D40000FF0000800000808080
        80C8D0D4C8D0D4C8D0D4C8D0D40000FF000080000080000080000080808080C8
        D0D40000FF000080000080000080000080808080C8D0D4C8D0D4C8D0D4C8D0D4
        0000FF0000800000800000800000808080800000800000800000800000800000
        80808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000FF00008000008000008000
        0080000080000080000080000080808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D40000FF000080000080000080000080000080000080808080C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400008000008000
        0080000080000080808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF000080000080000080000080808080C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000FF00008000008000
        0080000080000080808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D40000FF000080000080000080808080000080000080000080808080C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000FF000080000080000080808080C8
        D0D40000FF000080000080000080808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        0000FF000080000080808080C8D0D4C8D0D4C8D0D40000FF0000800000800000
        80808080C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000FF000080C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D40000FF000080000080000080C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000FF0000
        800000FFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
      Layout = blGlyphTop
    end
  end
  object CatchEnterButton: TBitBtn
    Left = 176
    Top = 267
    Width = 75
    Height = 25
    Action = EvaluateAction
    Caption = 'Evaluate'
    Default = True
    TabOrder = 4
  end
  object CatchCancelButton: TBitBtn
    Left = 48
    Top = 270
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'CatchCancelButton'
    ModalResult = 2
    TabOrder = 5
  end
  object ActionList1: TActionList
    Left = 288
    Top = 96
    object EvaluateAction: TAction
      Caption = 'Evaluate'
      ShortCut = 16453
      OnExecute = EvaluateActionExecute
    end
    object ModifyAction: TAction
      Caption = 'Modify'
      ShortCut = 16461
      OnExecute = ModifyActionExecute
    end
    object ContinueAction: TAction
      Caption = 'Continue'
      ShortCut = 120
      OnExecute = ContinueActionExecute
    end
    object StopAction: TAction
      Caption = 'Stop'
      ShortCut = 16497
      OnExecute = StopActionExecute
    end
  end
end
