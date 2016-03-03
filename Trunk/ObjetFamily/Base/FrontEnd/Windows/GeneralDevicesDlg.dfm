object GeneralDevicesForm: TGeneralDevicesForm
  Left = -22
  Top = -14
  BorderStyle = bsDialog
  Caption = 'General Devices'
  ClientHeight = 628
  ClientWidth = 490
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  inline GeneralDeviceFrame: TGeneralDeviceFrame
    Left = 0
    Top = 0
    Width = 717
    Height = 628
    Align = alLeft
    TabOrder = 0
    ExplicitHeight = 628
    inherited GroupBox2: TGroupBox
      inherited Label5: TLabel
        Width = 64
        ExplicitWidth = 64
      end
      inherited Label17: TLabel
        Width = 11
        ExplicitWidth = 11
      end
      inherited Label18: TLabel
        Width = 20
        ExplicitWidth = 20
      end
    end
    inherited UVLampsGroupBox: TGroupBox
      inherited Label8: TLabel
        Width = 63
        ExplicitWidth = 63
      end
    end
    inherited GroupBox4: TGroupBox
      inherited TurnRollerButton: TSpeedButton
        Left = 89
        ExplicitLeft = 89
      end
      inherited Label10: TLabel
        Width = 66
        ExplicitWidth = 66
      end
      inherited Label11: TLabel
        Width = 30
        ExplicitWidth = 30
      end
      inherited SuctionValveOnLabel: TLabel
        Width = 112
        ExplicitWidth = 112
      end
      inherited RSSPumpOnValuePanel: TPanel
        Left = 163
        ExplicitLeft = 163
      end
      inherited RSSPumpOffValuePanel: TPanel
        Left = 163
        ExplicitLeft = 163
      end
    end
    inherited TrayHeaterGroupBox: TGroupBox
      inherited Label1: TLabel
        Width = 60
        ExplicitWidth = 60
      end
      inherited Label2: TLabel
        Width = 50
        ExplicitWidth = 50
      end
      inherited Label15: TLabel
        Width = 11
        ExplicitWidth = 11
      end
      inherited Label16: TLabel
        Width = 20
        ExplicitWidth = 20
      end
      inherited ReqTrayTempUpDown: TUpDown
        Width = 16
        ExplicitWidth = 16
      end
    end
    inherited GroupBox6: TGroupBox
      inherited Label4: TLabel
        Width = 40
        ExplicitWidth = 40
      end
      inherited Label12: TLabel
        Width = 59
        ExplicitWidth = 59
      end
      inherited Label19: TLabel
        Width = 43
        ExplicitWidth = 43
      end
      inherited Label20: TLabel
        Width = 20
        ExplicitWidth = 20
      end
    end
    inherited GroupBox7: TGroupBox
      Left = 8
      Top = 534
      Width = 474
      ExplicitLeft = 8
      ExplicitTop = 534
      ExplicitWidth = 474
      inherited Label7: TLabel
        Width = 51
        ExplicitWidth = 51
      end
    end
    inherited DisplayValuesModeCheckBox: TCheckBox
      Top = 607
      ExplicitTop = 607
    end
    inherited EvacuationIndicatorGroupBox: TGroupBox
      inherited lbl4: TLabel
        Width = 49
        ExplicitWidth = 49
      end
      inherited grpAirFlowGrpBx: TGroupBox
        inherited lbl3: TLabel
          Width = 20
          ExplicitWidth = 20
        end
        inherited lbl5: TLabel
          Width = 31
          ExplicitWidth = 31
        end
      end
    end
  end
  object ActionList1: TActionList
    Left = 224
    Top = 32
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
    Left = 224
    Top = 72
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
