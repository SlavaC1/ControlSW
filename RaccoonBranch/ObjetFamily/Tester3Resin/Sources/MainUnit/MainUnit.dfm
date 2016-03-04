object MainForm: TMainForm
  Left = 336
  Top = 239
  Caption = 'Objet Tester'
  ClientHeight = 186
  ClientWidth = 455
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object MainMenu: TMainMenu
    Left = 64
    Top = 88
    object Setup: TMenuItem
      Action = OpenSetupAction
    end
    object OCBsimulator: TMenuItem
      Action = OCBSimulatorAction
      Enabled = False
    end
    object OCBTester: TMenuItem
      Caption = 'OCBTester'
      Enabled = False
      OnClick = OCBTesterClick
    end
    object OHDBTester: TMenuItem
      Caption = 'OHDBTester'
      Enabled = False
      OnClick = OHDBTesterClick
    end
    object MCBSimulator: TMenuItem
      Caption = 'MCBSimulator'
      Enabled = False
      OnClick = MCBSimulatorClick
    end
    object RFID: TMenuItem
      Caption = 'RFID'
      OnClick = RFIDClick
    end
    object Aplications1: TMenuItem
      Caption = 'Applications'
      object HeadsTester: TMenuItem
        Caption = 'Heads Tester'
        Enabled = False
        OnClick = HeadsTesterClick
      end
      object ReliabilityTest: TMenuItem
        Caption = 'Reliability Test'
        Enabled = False
        OnClick = ReliabilityTestClick
      end
      object ChecksumGenMenuItem: TMenuItem
        Caption = 'Checksum Generator'
        OnClick = ChecksumGenMenuItemClick
      end
    end
  end
  object ActionList1: TActionList
    Left = 64
    Top = 128
    object ActivateParamsDialogAction: TAction
      Caption = 'ActivateParamsDialogAction'
      ShortCut = 49232
      OnExecute = ActivateParamsDialogActionExecute
    end
    object OpenSetupAction: TAction
      Caption = 'Setup'
      OnExecute = OpenSetupActionExecute
    end
    object OCBSimulatorAction: TAction
      Caption = 'OCBSimulator'
      OnExecute = OCBSimulatorActionExecute
    end
  end
  object OpenFileDialog: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 312
    Top = 152
  end
end
