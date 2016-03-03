object MainForm: TMainForm
  Left = 196
  Top = 115
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Eden windows front-end '
  ClientHeight = 212
  ClientWidth = 314
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PrintScale = poNone
  Scaled = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object EmulationLabelHintHolder: TPaintBox
    Left = 534
    Top = 12
    Width = 108
    Height = 16
    ParentShowHint = False
    ShowHint = True
    Visible = False
  end
  object StopButton: TEnhancedButton
    Left = 186
    Top = 629
    Width = 107
    Height = 46
    TransparentMode = tmFixed
    TransparentColor = 9830550
    Transparent = True
    OnMouseEnter = StopButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
    Action = MainUIFrame.StopPrintAction
    Enabled = False
  end
  object PauseButton: TEnhancedButton
    Left = 397
    Top = 629
    Width = 110
    Height = 46
    TransparentMode = tmFixed
    TransparentColor = 9830550
    Transparent = True
    OnMouseEnter = PauseButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
    Action = MainUIFrame.PausePrintAction
    Enabled = False
  end
  object FileMenuLabel: TLabelMenu
    Left = 55
    Top = 11
    Width = 22
    Height = 16
    Caption = 'File'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
    Menu = FilePopupMenu
    HighlightedColor = clWhite
    DownColor = clBlue
  end
  object OptionsMenuLabel: TLabelMenu
    Left = 87
    Top = 11
    Width = 46
    Height = 16
    Caption = 'Options'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
    Menu = OptionsPopupMenu
    HighlightedColor = clWhite
    DownColor = clBlue
  end
  object MaintenanceMenuLabel: TLabelMenu
    Left = 142
    Top = 11
    Width = 77
    Height = 16
    Caption = 'Maintenance'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
    Menu = ChooseMaintenanceModePopupMenu
    HighlightedColor = clWhite
    DownColor = clBlue
  end
  object HelpMenuLabel: TLabelMenu
    Left = 230
    Top = 11
    Width = 29
    Height = 16
    Caption = 'Help'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = True
    Menu = HelpPopupMenu
    HighlightedColor = clWhite
    DownColor = clBlue
  end
  object CloseEnhancedButton: TEnhancedButton
    Left = 719
    Top = 24
    Width = 16
    Height = 17
    TransparentMode = tmFixed
    TransparentColor = 9539985
    Transparent = True
    OnClick = CloseEnhancedButtonClick
    OnMouseEnter = CloseEnhancedButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
  end
  object MinimizeEnhancedButton: TEnhancedButton
    Left = 700
    Top = 24
    Width = 16
    Height = 17
    TransparentMode = tmFixed
    TransparentColor = 9539985
    Transparent = True
    OnClick = MinimizeEnhancedButtonClick
    OnMouseEnter = MinimizeEnhancedButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
  end
  object SpecialModeLabel: TLabel
    Left = 45
    Top = 551
    Width = 604
    Height = 16
    AutoSize = False
    Caption = 'Current Mode:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
    Visible = False
  end
  object OnlineButton: TEnhancedButton
    Left = 291
    Top = 627
    Width = 107
    Height = 47
    Hint = 'Go to offline mode'
    TransparentMode = tmFixed
    TransparentColor = 9830550
    Transparent = True
    OnMouseEnter = OnlineButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
    Action = MainUIFrame.OnlineAction
    Visible = False
    ShowHint = True
  end
  object OfflineButton: TEnhancedButton
    Left = 291
    Top = 627
    Width = 107
    Height = 49
    Hint = 'Go to online mode'
    TransparentMode = tmFixed
    TransparentColor = 9830550
    Transparent = True
    OnMouseEnter = OfflineButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
    Action = MainUIFrame.OfflineAction
    ShowHint = True
  end
  object EmulationLabel: TLabel
    Left = 540
    Top = 12
    Width = 97
    Height = 16
    Caption = '-- Emulation --'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    Transparent = True
    Visible = False
  end
  object JobNameLabel: TLabel
    Left = 45
    Top = 586
    Width = 597
    Height = 49
    AutoSize = False
    Caption = 'Job Name:'
    Color = clGreen
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    Transparent = True
    Visible = False
    WordWrap = True
  end
  object SelectButton: TEnhancedButton
    Left = 297
    Top = 508
    Width = 95
    Height = 22
    TransparentMode = tmFixed
    TransparentColor = 9539985
    Transparent = True
    OnMouseDown = SelectButtonMouseDown
    OnMouseUp = SelectButtonMouseUp
    OnMouseEnter = SelectButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
  end
  object ButtonLabel: TLabel
    Left = 45
    Top = 513
    Width = 78
    Height = 16
    Caption = 'Button_Label'
    Color = clGreen
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    Transparent = True
    Visible = False
  end
  object GoButton: TEnhancedButton
    Left = 397
    Top = 629
    Width = 108
    Height = 46
    TransparentMode = tmAuto
    TransparentColor = 9830550
    Transparent = True
    OnMouseEnter = GoButtonMouseEnter
    OnMouseLeave = ResetButtonLabelActionExecute
    UseMask = True
    ShowMask = False
    MaskXOffset = 0
    MaskYOffset = 0
    Action = MainUIFrame.ResumePrintAction
    Visible = False
  end
  object DateLabel: TLabel
    Left = 507
    Top = 54
    Width = 95
    Height = 18
    Alignment = taCenter
    AutoSize = False
    Caption = 'Date'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
  end
  object TimeLabel: TLabel
    Left = 615
    Top = 54
    Width = 47
    Height = 19
    Alignment = taCenter
    AutoSize = False
    Caption = 'Time'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
  end
  object MaintenanceModeEnhancedLabel: TEnhancedLabel
    Left = 339
    Top = 76
    Width = 0
    Height = 0
    CaptionXOffset = -4
    CaptionYOffset = 0
    Transparent = True
    TransparentMode = tmFixed
    TransparentColor = 9830550
    Alignment = taCenter
    MiddlePartsNum = 14
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ActiveBitmap = 0
    Caption = 'Maintenance Mode: High Quality'
    Visible = False
  end
  object MachineLogoImage: TImage
    Left = 671
    Top = 70
    Width = 97
    Height = 81
    AutoSize = True
  end
  object HaspStatusLabel: TLabel
    Left = 275
    Top = 12
    Width = 120
    Height = 16
    Caption = 'HaspStatusLabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
    Visible = False
  end
  object PackageNameShadowLabel: TLabel
    Left = 585
    Top = 549
    Width = 170
    Height = 18
    Caption = 'PackageNameShadow'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  object PackageNameLabel: TLabel
    Left = 575
    Top = 539
    Width = 108
    Height = 18
    Caption = 'PackageName'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = [fsBold, fsItalic]
    ParentFont = False
  end
  inline MainUIFrame: TMainUIFrame
    Left = 30
    Top = 106
    Width = 626
    Height = 390
    HorzScrollBar.Visible = False
    VertScrollBar.Visible = False
    Color = clSilver
    Ctl3D = True
    ParentColor = False
    ParentCtl3D = False
    TabOrder = 0
    ExplicitLeft = 30
    ExplicitTop = 106
    ExplicitWidth = 626
    ExplicitHeight = 390
    inherited UpperToolBar: TToolBar
      Width = 626
      ExplicitWidth = 626
    end
    inherited ScreensPlaceHolder: TPanel
      Width = 626
      Height = 390
      ExplicitWidth = 626
      ExplicitHeight = 390
      inherited MaintenanceStatusFrame: TMaintenanceStatusFrame
        inherited Panel1: TPanel
          inherited PaintBox1: TPaintBox
            Color = clNone
            ParentColor = False
          end
        end
      end
    end
    inherited OpenMaintenanceCountersAction: TActionList
      Left = 579
      Top = 344
      inherited OpenQuickSHRAction: TAction
        OnExecute = MainUIFrameOpenQuickSHRActionExecute
      end
      inherited ShutdownAction: TAction
        OnExecute = MainUIFrameShutdownActionExecute
      end
    end
    inherited DateTimeUpdateTimer: TTimer
      Left = 427
      Top = 344
    end
  end
  object ActionList1: TActionList
    Left = 611
    Top = 580
    object OpenMiniDebugger: TAction
      Category = 'Maintenance'
      Caption = 'Mini Debugger'
      ShortCut = 49220
      OnExecute = OpenMiniDebuggerExecute
    end
    object ShowMonitorAction: TAction
      Category = 'Logging'
      Caption = 'Show Monitor'
      OnExecute = ShowMonitorActionExecute
    end
    object OpenParamsDialogAction: TAction
      Category = 'Parameter Manager'
      Caption = 'Parameter Manager'
      OnExecute = OpenParamsDialogActionExecute
    end
    object ViewLogFileAction: TAction
      Category = 'Logging'
      Caption = 'View Log File'
      OnExecute = ViewLogFileActionExecute
    end
    object ShowIncomingSlicesAction: TAction
      Category = 'Maintenance'
      Caption = 'Show Incoming Slices...'
      OnExecute = ShowIncomingSlicesActionExecute
    end
    object GoOneModeBackAction: TAction
      Category = 'Modes Manager'
      Caption = 'Go One Mode Back'
      OnExecute = GoOneModeBackActionExecute
    end
    object GoBackToDefaultModeAction: TAction
      Category = 'Modes Manager'
      Caption = 'Go Back to Default Mode'
      OnExecute = GoBackToDefaultModeActionExecute
    end
    object ExportConfigurationAction: TAction
      Category = 'Modes Manager'
      Caption = 'Export'
      OnExecute = ExportConfigurationActionExecute
    end
    object ImportConfigurationAction: TAction
      Category = 'Modes Manager'
      Caption = 'Import'
      OnExecute = ImportConfigurationActionExecute
    end
    object ExtractFromLogFileAction: TAction
      Category = 'Logging'
      Caption = 'Extract From Log File...'
      OnExecute = ExtractFromLogFileActionExecute
    end
    object ShowHelpContentAction: TAction
      Category = 'Help Activities'
      Caption = 'User Guide'
      ShortCut = 112
      OnExecute = ShowHelpContentActionExecute
    end
    object EnterMaintenanceModeAction: TAction
      Category = 'Maintenance'
      Caption = 'EnterMaintenanceModeAction'
      ShortCut = 49229
      OnExecute = EnterMaintenanceModeActionExecute
    end
    object OpenPythonConsoleAction: TAction
      Category = 'Maintenance'
      Caption = 'OpenPythonConsoleAction'
      ShortCut = 49241
      OnExecute = OpenPythonConsoleActionExecute
    end
    object ExitMaintenanceModeAction: TAction
      Category = 'Maintenance'
      Caption = 'Exit Maintenance Mode'
      OnExecute = ExitMaintenanceModeActionExecute
    end
    object SwitchMaintenanceModeAction: TAction
      Category = 'Maintenance'
      Caption = 'Switch Maintenance Mode'
      OnExecute = SwitchMaintenanceModeActionExecute
    end
    object ChooseMaintenanceModeAction: TAction
      Category = 'Maintenance'
      OnExecute = ChooseMaintenanceModeActionExecute
    end
    object OpenServiceNotesAction: TAction
      Category = 'Maintenance'
      Caption = 'Service Notes'
      OnExecute = OpenServiceNotesActionExecute
    end
    object ResetButtonLabelAction: TAction
      Category = 'General'
      OnExecute = ResetButtonLabelActionExecute
    end
    object OpenUVLampsHistoryAction: TAction
      Category = 'Maintenance'
      Caption = 'UV Lamps History'
      OnExecute = OpenUVLampsHistoryActionExecute
    end
    object ShowKeyboardMapAction: TAction
      Category = 'Help Activities'
      Caption = 'Keyboard Map'
      OnExecute = ShowKeyboardMapActionExecute
    end
    object ScreensSelectionAction: TAction
      Category = 'General'
      Caption = 'ScreensSelectionAction'
      ShortCut = 123
      OnExecute = ScreensSelectionActionExecute
    end
    object OpenHaspToolAction: TAction
      Category = 'General'
      Caption = 'OpenHaspToolAction'
      OnExecute = OpenHaspToolActionExecute
    end
    object RunAutomationScriptAction: TAction
      Category = 'General'
      Caption = 'RunAutomationScriptAction'
      Enabled = False
      ShortCut = 32776
      Visible = False
      OnExecute = RunAutomationScriptActionExecute
    end
    object OpenFansSpeedAction: TAction
      Category = 'Maintenance'
      Caption = 'OpenFansSpeedAction'
      OnExecute = OpenFansSpeedActionExecute
    end
  end
  object FilePopupMenu: TPopupMenu
    Left = 54
    Top = 32
    object PFF1: TMenuItem
      Action = MainUIFrame.PFFPrintJobAction
      Caption = 'PFF...'
      ShortCut = 16464
    end
    object Loadlocalprintjob2: TMenuItem
      Action = MainUIFrame.LoadLocalPrintJobAction
      Caption = 'Load local print job...'
    end
    object LoadLocalPrintJobMenuSeperator: TMenuItem
      Caption = '-'
    end
    object Exit1: TMenuItem
      Caption = 'Exit'
      OnClick = ExitMenuItemClick
    end
  end
  object OptionsPopupMenu: TPopupMenu
    Left = 108
    Top = 32
    object TrayRemovalPosition1: TMenuItem
      Action = MainUIFrame.DoTrayRemovalPositionAction
    end
    object Gotopurgeposition2: TMenuItem
      Action = MainUIFrame.GotoPurgePositionAction
    end
    object Wipe2: TMenuItem
      Action = MainUIFrame.DoWipeAction
    end
    object GotoHeadInspectionPosition1: TMenuItem
      Action = MainUIFrame.GoToHeadInspectionAction
      Caption = 'Go To Head Inspection'
    end
    object DoTrayLowerPositionAction: TMenuItem
      Action = MainUIFrame.DoTrayLowerPositionAction
      Caption = 'Go to Tray Lower Position'
    end
    object N9: TMenuItem
      Caption = '-'
    end
    object ExecuteFireAllsequence2: TMenuItem
      Action = MainUIFrame.DoFireAllSequenceAction
      Visible = False
    end
    object ExecutePurgesequence2: TMenuItem
      Action = MainUIFrame.DoPurgeSequenceAction
      Caption = 'Purge Sequence'
    end
    object N13: TMenuItem
      Caption = '-'
    end
    object Testpattern2: TMenuItem
      Action = MainUIFrame.DoTestPatternAction
    end
    object N11: TMenuItem
      Caption = '-'
    end
    object Wizards1: TMenuItem
      Action = MainUIFrame.UserWizardsAction
      object ResinReplacementItem_Noi: TMenuItem
        Action = MainUIFrame.OpenResinReplacementWizardAction_Noi
        Caption = 'Material Replacement'
      end
      object OpenHeadFillingWizardActionItem: TMenuItem
        Action = MainUIFrame.OpenHeadFillingWizardAction
      end
      object CleanWiperWizardActionItem: TMenuItem
        Action = MainUIFrame.CleanWiperWizardAction
        Visible = False
      end
      object CleanHeadsWizardActionItem: TMenuItem
        Action = MainUIFrame.CleanHeadsWizardAction
        Caption = 'Cleaning'
      end
      object UVCalibrationGNR2: TMenuItem
        Action = MainUIFrame.UVCalibrationGNRWizard
        Caption = 'UV Calibration'
      end
      object UVLampReplacementItem: TMenuItem
        Action = MainUIFrame.UVReplacementWizard
      end
      object XOffsetWizardItem: TMenuItem
        Action = MainUIFrame.XOffsetWizardAction
      end
      object HeadReplacementMenuItem: TMenuItem
        Action = MainUIFrame.OpenQuickSHRAction
      end
      object HeadOptimizationScaleBased1: TMenuItem
        Action = MainUIFrame.HeadsOptimizationScalesBasedAction
        Caption = 'Head Optimization'
      end
      object LoadCellCalibrationActionItem: TMenuItem
        Action = MainUIFrame.LoadCellCalibrationAction
      end
      object ShutdownItem: TMenuItem
        Action = MainUIFrame.ShutdownAction
      end
    end
    object BuiltInTests1: TMenuItem
      Action = MainUIFrame.OpenBITViewerAction
    end
    object N7: TMenuItem
      Caption = '-'
    end
    object MaintenanceCounters1: TMenuItem
      Action = MainUIFrame.MaintenanceCountersAction
    end
    object CollectServiceData1: TMenuItem
      Action = MainUIFrame.CollectServiceDataAction
    end
    object N15: TMenuItem
      Caption = '-'
    end
  end
  object MaintenancePopupMenu: TPopupMenu
    OnPopup = MaintenancePopupMenuPopup
    Left = 162
    Top = 32
    object Parametersmana1: TMenuItem
      Action = OpenParamsDialogAction
    end
    object ShowMonitor1: TMenuItem
      Action = ShowMonitorAction
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object GeneralDevicesControl1: TMenuItem
      Action = MainUIFrame.OpenGeneralDevicesAction
    end
    object HeadsControl1: TMenuItem
      Action = MainUIFrame.OpenHeadsControlAction
    end
    object MotorsControl1: TMenuItem
      Action = MainUIFrame.OpenMotorsControlAction
    end
    object ActuatorsSensors2: TMenuItem
      Action = MainUIFrame.OpenActuatorsAndSensorsAction
    end
    object OCBOHDBVoltages1: TMenuItem
      Action = MainUIFrame.OpenOcbOhdbVoltageDlgAction
      Caption = 'HW Voltages...'
    end
    object Fans1: TMenuItem
      Action = OpenFansSpeedAction
      Caption = 'Fans Speed...'
    end
    object N8: TMenuItem
      Caption = '-'
    end
    object Calibration1: TMenuItem
      Action = MainUIFrame.OpenCalibartionDlgAction
    end
    object OHDBOptions2: TMenuItem
      Action = MainUIFrame.OpenOHDBOptionsDlgAction
    end
    object PCIOptions2: TMenuItem
      Action = MainUIFrame.OpenPCIOptionsDlgAction
    end
    object MCBSWDiagnostic: TMenuItem
      Action = MainUIFrame.OpenMCBDiagDlgAction
      Caption = 'MCB SW Parameters'
    end
    object Showincomingslices1: TMenuItem
      Action = ShowIncomingSlicesAction
    end
    object DoFireAll1: TMenuItem
      Action = MainUIFrame.DoFireAllAction
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object ModesMenuItem: TMenuItem
      Action = MainUIFrame.ModesMenuAction
      AutoHotkeys = maManual
    end
    object Parametersexportimport1: TMenuItem
      Action = MainUIFrame.ConfigurationExportImportAction
      object Export1: TMenuItem
        Action = ExportConfigurationAction
      end
      object Import1: TMenuItem
        Action = ImportConfigurationAction
      end
    end
    object ScriptsMenuItem: TMenuItem
      Action = MainUIFrame.ScriptsMenuAction
      AutoHotkeys = maManual
    end
    object N12: TMenuItem
      Caption = '-'
    end
    object WizardsMenuItem: TMenuItem
      Action = MainUIFrame.MaintenanceWizardsAction
      Caption = 'Service Wizards'
      object BumperCalibrationItem: TMenuItem
        Action = MainUIFrame.BumperCalibrationWizardAction
      end
      object PrintingPositionItem: TMenuItem
        Action = MainUIFrame.PrintingPositionAction
      end
      object RollerTiltItem: TMenuItem
        Action = MainUIFrame.RollerTiltWizardAction
      end
      object VacuumSensorCalibrationItem: TMenuItem
        Action = MainUIFrame.VacuumCalibrationAction
      end
      object WiperCalibrationWizardItem: TMenuItem
        Action = MainUIFrame.WiperCalibrationAction
      end
      object ZCalibration1: TMenuItem
        Action = MainUIFrame.OpenTrayPointsWizardAction
      end
      object OpenTrayPointsWizardAction1: TMenuItem
        Action = MainUIFrame.OpenZStartCalibrationWizardAction
        Caption = 'Highest Point Location'
      end
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object EnterStandby2: TMenuItem
      Action = MainUIFrame.EnterStandbyAction
    end
    object ExitStandby2: TMenuItem
      Action = MainUIFrame.ExitStandbyAction
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object Viewlogfile1: TMenuItem
      Action = ViewLogFileAction
    end
    object Extractfromlogfile1: TMenuItem
      Action = ExtractFromLogFileAction
    end
    object N10: TMenuItem
      Caption = '-'
    end
    object ServiceNotes1: TMenuItem
      Action = OpenServiceNotesAction
    end
    object HistoryMenuItem: TMenuItem
      Caption = 'History'
      object ShowUVLampsHistory: TMenuItem
        Action = OpenUVLampsHistoryAction
      end
      object MachineOperationsMenuItem: TMenuItem
        Action = MainUIFrame.MachineActionsAction
      end
    end
    object N16: TMenuItem
      Caption = '-'
    end
    object SwitchMaintenanceMode1: TMenuItem
      Action = SwitchMaintenanceModeAction
    end
    object ExitMaintenanceMode1: TMenuItem
      Action = ExitMaintenanceModeAction
    end
  end
  object HelpPopupMenu: TPopupMenu
    Left = 217
    Top = 32
    object HelpContentMenuItem: TMenuItem
      Action = ShowHelpContentAction
    end
    object MaterialBatchNoMenuItem: TMenuItem
      Action = MainUIFrame.MaterialBatchNoAction
    end
    object KeyboardMap1: TMenuItem
      Action = ShowKeyboardMapAction
    end
    object HASP1: TMenuItem
      Caption = 'Sentinel HASP'
      object LicenseUpdate1: TMenuItem
        Action = OpenHaspToolAction
        Caption = 'License update'
      end
      object CurrentData1: TMenuItem
        Action = MainUIFrame.OpenHaspInfoDlgAction
        Caption = 'Current data'
      end
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object About1: TMenuItem
      Caption = 'About...'
      OnClick = AboutMenuItemClick
    end
  end
  object ParametersImportDlg: TOpenDialog
    DefaultExt = 'cfg'
    Filter = 'cfg files|*.cfg|All files|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofFileMustExist, ofEnableSizing]
    Left = 509
    Top = 580
  end
  object EmulationLabelBlinkTimer: TTimer
    Enabled = False
    OnTimer = EmulationLabelBlinkTimerTimer
    Left = 459
    Top = 580
  end
  object ChooseMaintenanceModePopupMenu: TPopupMenu
    Left = 162
    Top = 64
    object DefaultModeMenuItem: TMenuItem
      Action = ChooseMaintenanceModeAction
      Caption = 'Default Mode'
    end
    object DefaultModeSeperatorMenuItem: TMenuItem
      Caption = '-'
    end
  end
  object MainBitmapFormSkin: TBitmapFormSkin
    TransparentColor = 9830550
    Preview = False
    Options = [soSkin, soAutoSize, soClientDrag]
    Active = False
    Left = 680
    Top = 280
  end
end
