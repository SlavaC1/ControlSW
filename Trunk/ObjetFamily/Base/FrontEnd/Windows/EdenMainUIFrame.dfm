object MainUIFrame: TMainUIFrame
  Left = 0
  Top = 0
  Width = 630
  Height = 480
  TabOrder = 0
  object UpperToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 630
    Height = 26
    AutoSize = True
    ButtonWidth = 46
    Caption = 'UpperToolBar'
    TabOrder = 0
  end
  object ScreensPlaceHolder: TPanel
    Left = 0
    Top = 0
    Width = 630
    Height = 480
    BevelOuter = bvNone
    Color = clNavy
    TabOrder = 1
    inline MainStatusFrame: TMainStatusFrame
      Left = 32
      Top = 32
      Width = 267
      Height = 153
      TabOrder = 0
      ExplicitLeft = 32
      ExplicitTop = 32
      ExplicitWidth = 267
      ExplicitHeight = 153
      inherited Panel1: TPanel
        Width = 267
        Height = 153
        ExplicitWidth = 267
        ExplicitHeight = 153
        inherited PaintBox1: TPaintBox
          Width = 267
          Height = 153
          ExplicitWidth = 267
          ExplicitHeight = 153
        end
      end
    end
    inline MainMenuFrame: TMainMenuFrame
      Left = 33
      Top = 205
      Width = 267
      Height = 163
      TabOrder = 1
      Visible = False
      ExplicitLeft = 33
      ExplicitTop = 205
      ExplicitWidth = 267
      ExplicitHeight = 163
      inherited Panel1: TPanel
        Width = 267
        Height = 163
        ExplicitWidth = 267
        ExplicitHeight = 163
        inherited PaintBox1: TPaintBox
          Width = 267
          Height = 163
          ExplicitWidth = 267
          ExplicitHeight = 163
        end
        inherited OptionsMenu: TEnhancedMenu
          OnClick = MainMenuFrameOptionsMenuClick
        end
      end
    end
    inline MaintenanceStatusFrame: TMaintenanceStatusFrame
      Left = 317
      Top = 30
      Width = 252
      Height = 155
      TabOrder = 2
      Visible = False
      ExplicitLeft = 317
      ExplicitTop = 30
      ExplicitWidth = 252
      ExplicitHeight = 155
      inherited Panel1: TPanel
        Width = 252
        Height = 155
        ExplicitWidth = 252
        ExplicitHeight = 155
        inherited PaintBox1: TPaintBox
          Width = 252
          Height = 155
          ExplicitWidth = 252
          ExplicitHeight = 155
        end
      end
    end
  end
  object OpenMaintenanceCountersAction: TActionList
    Left = 520
    Top = 232
    object BumperCalibrationWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Bumper Calibration'
      OnExecute = BumperCalibrationWizardActionExecute
    end
    object CleanHeadsWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Head Cleaning'
      OnExecute = CleanHeadsWizardActionExecute
    end
    object CleanWiperWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Wiper Cleaning'
      OnExecute = CleanWiperWizardActionExecute
    end
    object HeadsOptimizationAction: TAction
      Category = 'Wizards'
      Caption = 'Heads Optimization'
      OnExecute = HeadsOptimizationActionExecute
    end
    object LoadCellCalibrationAction: TAction
      Category = 'Wizards'
      Caption = 'Load Cell Calibration'
      OnExecute = LoadCellCalibrationActionExecute
    end
    object MaintenanceWizardsAction: TAction
      Category = 'Wizards'
      Caption = 'Wizards'
      OnExecute = MaintenanceWizardsActionExecute
    end
    object OpenHeadFillingWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Head Filling Calibration'
      OnExecute = OpenHeadFillingWizardActionExecute
    end
    object OpenQuickSHRAction: TAction
      Category = 'Wizards'
      Caption = 'Head Replacement'
      OnExecute = OpenQuickSHRActionExecute
    end
    object OfflineAction: TAction
      Category = 'Print'
      Caption = 'OfflineAction'
      ShortCut = 120
      Visible = False
      OnExecute = OfflineActionExecute
    end
    object OnlineAction: TAction
      Category = 'Print'
      Caption = 'OnlineAction'
      ShortCut = 8312
      OnExecute = OnlineActionExecute
    end
    object PausePrintAction: TAction
      Category = 'Print'
      Enabled = False
      Hint = 'Pause printing'
      ImageIndex = 1
      OnExecute = PausePrintActionExecute
    end
    object OpenTrayPointsWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Build Tray Adjustment'
      OnExecute = OpenTrayPointsWizardActionExecute
    end
    object ResumePrintAction: TAction
      Category = 'Print'
      OnExecute = ResumePrintActionExecute
    end
    object StopPrintAction: TAction
      Category = 'Print'
      Enabled = False
      Hint = 'Stop printing'
      ImageIndex = 2
      OnExecute = StopPrintActionExecute
    end
    object PrintingPositionAction: TAction
      Category = 'Wizards'
      Caption = 'Printing Position Adjustment'
      OnExecute = PrintingPositionActionExecute
    end
    object RollerTiltWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Roller Tilt Adjustment'
      OnExecute = RollerTiltWizardActionExecute
    end
    object CollectServiceDataAction: TAction
      Caption = 'Collect Service Data'
      ShortCut = 113
      OnExecute = CollectServiceDataActionExecute
    end
    object ConfigurationExportImportAction: TAction
      Caption = 'Configuration Export/Import'
      OnExecute = ConfigurationExportImportActionExecute
    end
    object DisplayOptimizationWizardAction: TAction
      Caption = 'Display Optimization Wizard'
      ShortCut = 49231
      OnExecute = DisplayOptimizationWizardActionExecute
    end
    object DoCleanHeads: TAction
      Caption = 'Clean Heads'
      Visible = False
    end
    object DoCleanWiper: TAction
      Caption = 'Clean Wiper'
      Visible = False
    end
    object DoFireAllAction: TAction
      Caption = 'Do Fire All'
      OnExecute = DoFireAllActionExecute
    end
    object DoFireAllSequenceAction: TAction
      Caption = 'Execute Fire All Sequence'
      OnExecute = DoFireAllSequenceActionExecute
    end
    object DoPurgeSequenceAction: TAction
      Caption = 'Execute Purge Sequence'
      ShortCut = 115
      OnExecute = DoPurgeSequenceActionExecute
    end
    object DoTestPatternAction: TAction
      Caption = 'Pattern Test'
      ShortCut = 114
      OnExecute = DoTestPatternActionExecute
    end
    object DoTrayRemovalPositionAction: TAction
      Caption = 'Go to Tray Upper Position'
      ShortCut = 116
      OnExecute = DoTrayUpperPositionActionExecute
    end
    object DoWipeAction: TAction
      Caption = 'Wipe'
      ShortCut = 118
      OnExecute = DoWipeActionExecute
    end
    object EnterAdvanceModeAction: TAction
      Caption = 'EnterAdvanceModeAction'
      ShortCut = 49217
      OnExecute = EnterAdvanceModeActionExecute
    end
    object EnterStandbyAction: TAction
      Caption = 'Enter Standby'
      OnExecute = EnterStandbyActionExecute
    end
    object ExitStandbyAction: TAction
      Caption = 'Exit Standby'
      OnExecute = ExitStandbyActionExecute
    end
    object GotoPurgePositionAction: TAction
      Caption = 'Go to Purge Position'
      ShortCut = 117
      OnExecute = GotoPurgePositionActionExecute
    end
    object PFFPrintJobAction: TAction
      Hint = 'Load bitmaps for PFF print job'
      OnExecute = PFFPrintJobActionExecute
    end
    object LoadLocalPrintJobAction: TAction
      Hint = 'Load bitmaps for local print job'
      ShortCut = 16463
      OnExecute = LoadLocalPrintJobActionExecute
    end
    object ModesMenuAction: TAction
      Caption = 'Modes'
      OnExecute = ModesMenuActionExecute
    end
    object OpenCalibartionDlgAction: TAction
      Caption = 'Calibration...'
      OnExecute = OpenCalibartionDlgActionExecute
    end
    object OpenMCBDiagDlgAction: TAction
      Caption = 'MCB SW diagnostic'
      Visible = False
      OnExecute = OpenMCBDiagDlgActionExecute
    end
    object OpenOHDBOptionsDlgAction: TAction
      Caption = 'OHDB Options'
      Visible = False
      OnExecute = OpenOHDBOptionsDlgActionExecute
    end
    object OpenPCIOptionsDlgAction: TAction
      Caption = 'PCI Options'
      Hint = 'Open PCI options dialog'
      Visible = False
      OnExecute = OpenPCIOptionsDlgActionExecute
    end
    object OptionBackAction: TAction
      ImageIndex = 4
      OnExecute = OptionBackActionExecute
    end
    object OptionSelectAction: TAction
      ImageIndex = 3
    end
    object ScriptsMenuAction: TAction
      Caption = 'Scripts'
      OnExecute = ScriptsMenuActionExecute
    end
    object SelectDownAction: TAction
      ImageIndex = 6
      OnExecute = SelectDownActionExecute
    end
    object SelectUpAction: TAction
      ImageIndex = 5
      OnExecute = SelectUpActionExecute
    end
    object ShutdownAction: TAction
      Category = 'Wizards'
      Caption = 'Shutdown Wizard'
      OnExecute = ShutdownActionExecute
    end
    object MaintenanceCountersAction: TAction
      Category = 'Dialogs'
      Caption = 'Maintenance Counters...'
      OnExecute = MaintenanceCountersActionExecute
    end
    object OpenActuatorsAndSensorsAction: TAction
      Category = 'Dialogs'
      Caption = 'Actuators && Sensors...'
      OnExecute = OpenActuatorsAndSensorsActionExecute
    end
    object OpenBITViewerAction: TAction
      Category = 'Dialogs'
      Caption = 'Built In Tests...'
      ShortCut = 49218
      OnExecute = OpenBITViewerActionExecute
    end
    object OpenGeneralDevicesAction: TAction
      Category = 'Dialogs'
      Caption = 'General Devices Control...'
      OnExecute = OpenGeneralDevicesActionExecute
    end
    object OpenOcbOhdbVoltageDlgAction: TAction
      Category = 'Dialogs'
      Caption = 'OCB/OHDB Voltages...'
      OnExecute = OpenOcbOhdbVoltageDlgActionExecute
    end
    object OpenHeadsControlAction: TAction
      Category = 'Dialogs'
      Caption = 'Heads Control...'
      OnExecute = OpenHeadsControlActionExecute
    end
    object OpenMotorsControlAction: TAction
      Category = 'Dialogs'
      Caption = 'Motors Control...'
      OnExecute = OpenMotorsControlActionExecute
    end
    object UserWizardsAction: TAction
      Category = 'Wizards'
      Caption = 'Wizards'
      OnExecute = UserWizardsActionExecute
    end
    object UVCalibrationWizard: TAction
      Category = 'Wizards'
      Caption = 'UV Calibration Wizard'
      OnExecute = UVCalibrationWizardExecute
    end
    object UVReplacementWizard: TAction
      Category = 'Wizards'
      Caption = 'UV Lamp Replacement'
      OnExecute = UVReplacementWizardExecute
    end
    object VacuumCalibrationAction: TAction
      Category = 'Wizards'
      Caption = 'Vacuum Sensor Calibration'
      OnExecute = VacuumCalibrationActionExecute
    end
    object XOffsetWizardAction: TAction
      Category = 'Wizards'
      Caption = 'Head Alignment'
      OnExecute = XOffsetWizardActionExecute
    end
    object UVCalibrationOptionsWizard: TAction
      Category = 'Wizards'
      Caption = 'UV Calibration Wizard'
      OnExecute = UVCalibrationOptionsWizardExecute
    end
    object UVMCalibrationWizard: TAction
      Category = 'Wizards'
      Caption = 'UVMCalibrationWizard'
      OnExecute = UVMCalibrationWizardExecute
    end
    object WiperCalibrationAction: TAction
      Category = 'Wizards'
      Caption = 'Wiper Calibration'
      OnExecute = WiperCalibrationActionExecute
    end
    object OpenHaspInfoDlgAction: TAction
      Caption = 'OpenHaspInfoDlgAction'
      OnExecute = OpenHaspInfoDlgActionExecute
    end
    object MaterialBatchNoAction: TAction
      Caption = 'Material Batch Number'
      OnExecute = MaterialBatchNoActionExecute
    end
    object MachineActionsAction: TAction
      Caption = 'Machine Operations'
      OnExecute = MachineActionsActionExecute
    end
    object GoToHeadInspectionAction: TAction
      Caption = 'GoToHeadInspection'
      ShortCut = 119
      OnExecute = GoToHeadInspectionActionExecute
    end
    object HeadsOptimizationScalesBasedAction: TAction
      Category = 'Wizards'
      Caption = 'HeadsOptimizationScalesBasedAction'
      OnExecute = HeadsOptimizationScalesBasedActionExecute
    end
    object OpenResinReplacementWizardAction_Noi: TAction
      Category = 'Wizards'
      Caption = 'Resin Replacement (Noi)'
      OnExecute = OpenResinReplacementWizardAction_NoiExecute
    end
    object UVCalibrationGNRWizard: TAction
      Category = 'Wizards'
      Caption = 'UV Calibration (G&&R)'
      OnExecute = UVCalibrationGNRWizardExecute
    end
    object DoTrayLowerPositionAction: TAction
      Caption = 'Do Tray Lower Position Action'
      ShortCut = 121
      OnExecute = DoTrayLowerPositionActionExecute
    end
    object OpenZStartCalibrationWizardAction: TAction
      Category = 'Wizards'
      Caption = 'OpenTrayPointsWizardAction'
      OnExecute = OpenZStartCalibrationWizardActionExecute
    end
  end
  object DateTimeUpdateTimer: TTimer
    Enabled = False
    OnTimer = DateTimeUpdateTimerTimer
    Left = 373
    Top = 232
  end
end
