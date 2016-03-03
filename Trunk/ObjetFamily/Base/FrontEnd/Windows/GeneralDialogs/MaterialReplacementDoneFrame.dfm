object MaterialReplacementDoneFrame: TMaterialReplacementDoneFrame
  AlignWithMargins = True
  Left = 0
  Top = 0
  Width = 783
  Height = 352
  TabOrder = 0
  object TitlePanel: TPanel
    Left = 0
    Top = 0
    Width = 783
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    Caption = 'TitlePanel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Times New Roman'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 451
  end
  inline TanksSummaryFrame: TTanksSummaryFrame
    Left = 0
    Top = 41
    Width = 783
    Height = 286
    Align = alCustom
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 1
    ExplicitTop = 41
    ExplicitWidth = 451
    ExplicitHeight = 386
    inherited TanksGroupBox: TGroupBox
      Width = 783
      Height = 286
      ExplicitWidth = 451
      ExplicitHeight = 386
      inherited OpModePanel: TPanel
        Top = 243
        Width = 769
        Anchors = [akLeft, akRight]
        ExplicitTop = 343
        ExplicitWidth = 437
        inherited btnStart: TButton
          Left = 680
          ExplicitLeft = 348
        end
      end
    end
  end
  object FootNotePanel: TPanel
    Left = 0
    Top = 328
    Width = 283
    Height = 20
    Align = alCustom
    BevelEdges = []
    BevelOuter = bvNone
    Caption = 'Click '#8216'Done'#8217' to restart the printer application.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 2
    Visible = False
  end
end
