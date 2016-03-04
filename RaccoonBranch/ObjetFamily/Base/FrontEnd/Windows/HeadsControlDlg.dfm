object HeadsControlForm: THeadsControlForm
  Left = 467
  Top = 115
  BorderStyle = bsDialog
  Caption = 'Heads Control'
  ClientHeight = 448
  ClientWidth = 719
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  OnClose = FormClose
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  inline HeadsFrame: THeadsFrame
    Left = 0
    Top = 0
    Width = 715
    Height = 447
    TabOrder = 0
    inherited ReqTempADUHeads: TLabel
      Width = 65
      ExplicitWidth = 65
    end
    inherited CurTempADUHeads: TLabel
      Width = 63
      ExplicitWidth = 63
    end
    inherited CurTempCelHeads: TLabel
      Width = 66
      ExplicitWidth = 66
    end
    inherited HeadsFillingGroupBox: TGroupBox
      inherited LowerThermistorsLabel: TLabel
        Width = 82
        ExplicitWidth = 82
      end
      inherited UpperThermistorsLabel: TLabel
        Width = 82
        ExplicitWidth = 82
      end
    end
  end
  object ActionList1: TActionList
    Left = 248
    Top = 8
    object AlwaysOnTopAction: TAction
      Caption = 'Always on top'
      OnExecute = AlwaysOnTopActionExecute
    end
    object CloseDialogAction: TAction
      Caption = 'Close'
      OnExecute = CloseDialogActionExecute
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 296
    Top = 8
    object Alwaysontop1: TMenuItem
      Action = AlwaysOnTopAction
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Close1: TMenuItem
      Action = CloseDialogAction
    end
  end
end
