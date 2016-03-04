//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

#include "Q2RTApplication.h"
#include "EdenProtocolEngine.h"
#include "EdenProtocolClient.h"
#include "QStdComPort.h"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include "EdenMainUIFrame.h"
#include <ExtCtrls.hpp>
#include "FormSkin.hpp"
#include "EnhancedButton.hpp"
#include "LabelMenu.hpp"
#include "edenmainuiframe.h"
#include <Graphics.hpp>
#include "EnhancedLabel.hpp"
#include "GlobalDefs.h"

#define WM_SLICE_IMAGE_UPDATE WM_USER
extern UINT   WM_OBJET_EMBEDED_UNIQUE_MSG;

class TParamsDialog;
class TImageViewForm;
class CQParameterBase;

class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TActionList *ActionList1;
        TAction *OpenMiniDebugger;
        TAction *ShowMonitorAction;
        TAction *OpenParamsDialogAction;
        TAction *ViewLogFileAction;
        TMainUIFrame *MainUIFrame;
        TEnhancedButton *StopButton;
        TEnhancedButton *PauseButton;
        TEnhancedButton *SelectButton;
        TLabelMenu *FileMenuLabel;
        TLabelMenu *OptionsMenuLabel;
        TLabelMenu *MaintenanceMenuLabel;
        TLabelMenu *HelpMenuLabel;
        TPopupMenu *FilePopupMenu;
        TPopupMenu *OptionsPopupMenu;
        TPopupMenu *MaintenancePopupMenu;
        TPopupMenu *HelpPopupMenu;
        TMenuItem *Loadlocalprintjob2;
        TMenuItem *LoadLocalPrintJobMenuSeperator;
        TMenuItem *Exit1;
        TMenuItem *Gotopurgeposition2;
        TMenuItem *Wipe2;
        TMenuItem *N9;
        TMenuItem *ExecuteFireAllsequence2;
        TMenuItem *ExecutePurgesequence2;
        TMenuItem *Testpattern2;
        TMenuItem *N11;
        TMenuItem *GeneralDevicesControl1;
        TMenuItem *HeadsControl1;
        TMenuItem *MotorsControl1;
        TMenuItem *ActuatorsSensors2;
        TMenuItem *OHDBOptions2;
        TMenuItem *PCIOptions2;
        TMenuItem *N12;
        TMenuItem *EnterStandby2;
        TMenuItem *ExitStandby2;
        TMenuItem *About1;
        TMenuItem *N13;
        TAction *ShowIncomingSlicesAction;
        TMenuItem *Showincomingslices1;
        TMenuItem *ModesMenuItem;
        TMenuItem *N2;
        TAction *GoOneModeBackAction;
        TAction *GoBackToDefaultModeAction;
        TEnhancedButton *CloseEnhancedButton;
        TEnhancedButton *MinimizeEnhancedButton;
        TLabel *SpecialModeLabel;
  TMenuItem *Calibration1;
        TAction *ExportConfigurationAction;
        TMenuItem *Parametersexportimport1;
        TAction *ImportConfigurationAction;
        TMenuItem *Export1;
        TMenuItem *Import1;
        TOpenDialog *ParametersImportDlg;
  TMenuItem *WizardsMenuItem;
  TMenuItem *N3;
    TMenuItem *TrayPointsCalibrationItem;
        TMenuItem *TrayRemovalPosition1;
        TMenuItem *N4;
        TMenuItem *HelpContentMenuItem;
        TMenuItem *N5;
        TMenuItem *Parametersmana1;
        TMenuItem *ShowMonitor1;
        TMenuItem *N6;
        TMenuItem *Wizards1;
	TMenuItem *ResinReplacementItem_Noi;
        TMenuItem *Viewlogfile1;
        TMenuItem *DoFireAll1;
        TMenuItem *N7;
    TMenuItem *ShutdownItem;
    TMenuItem *OpenHeadFillingWizardActionItem;
        TAction *ExtractFromLogFileAction;
        TMenuItem *Extractfromlogfile1;
        TMenuItem *N8;
        TAction *ShowHelpContentAction;
        TEnhancedButton *OnlineButton;
        TEnhancedButton *OfflineButton;
        TEnhancedButton *GoButton;
    TMenuItem *RollerTiltItem;
    TMenuItem *PrintingPositionItem;
        TLabel *EmulationLabel;
        TTimer *EmulationLabelBlinkTimer;
        TPaintBox *EmulationLabelHintHolder;
    TMenuItem *CleanHeadsWizardActionItem;
    TMenuItem *CleanWiperWizardActionItem;
        TAction *EnterMaintenanceModeAction;
        TMenuItem *MCBSWDiagnostic;
        TAction *OpenPythonConsoleAction;
        TMenuItem *ScriptsMenuItem;
        TMenuItem *N10;
        TAction *ExitMaintenanceModeAction;
        TMenuItem *ExitMaintenanceMode1;
        TMenuItem *BuiltInTests1;
        TMenuItem *CollectServiceData1;
    TMenuItem *BumperCalibrationItem;
        TLabel *JobNameLabel;
    TMenuItem *HeadReplacementMenuItem;
        TMenuItem *MaintenanceCounters1;
    TMenuItem *XOffsetWizardItem;
        TPopupMenu *ChooseMaintenanceModePopupMenu;
        TMenuItem *DefaultModeMenuItem;
        TMenuItem *DefaultModeSeperatorMenuItem;
        TMenuItem *SwitchMaintenanceMode1;
        TAction *SwitchMaintenanceModeAction;
        TAction *ChooseMaintenanceModeAction;
        TAction *OpenServiceNotesAction;
        TMenuItem *ServiceNotes1;
        TLabel *ButtonLabel;
        TAction *ResetButtonLabelAction;
        TLabel *DateLabel;
        TLabel *TimeLabel;
        TEnhancedLabel *MaintenanceModeEnhancedLabel;
        TBitmapFormSkin *MainBitmapFormSkin;
        TAction *OpenUVLampsHistoryAction;
        TMenuItem *ShowUVLampsHistory;
        TAction *ShowTroubleshootingAction;
        TAction *ShowKeyboardMapAction;
        TMenuItem *Troubleshooting1;
        TMenuItem *KeyboardMap1;
        TImage *MachineLogoImage;
    TMenuItem *VacuumSensorCalibrationItem;
        TAction *ScreensSelectionAction;
    TMenuItem *UVLampReplacementItem;
        TMenuItem *N15;
        TMenuItem *N16;
    TMenuItem *UVCalibrationWizardItem;
    TMenuItem *UVMCalibrationWizardItem;
    TMenuItem *WiperCalibrationWizardItem;
    TMenuItem *LoadCellCalibrationActionItem;
    TLabel *HaspStatusLabel;
    TMenuItem *HASP1;
    TMenuItem *LicenseUpdate1;
    TMenuItem *CurrentData1;
    TAction *OpenHaspToolAction;
	TMenuItem *MaterialBatchNoMenuItem;
	TMenuItem *MachineOperationsMenuItem;
	TMenuItem *HistoryMenuItem;
	TMenuItem *GotoHeadInspectionPosition1;
	TLabel *PackageNameLabel;
	TLabel *PackageNameShadowLabel;
	TMenuItem *HeadOptimizationScaleBased1;
	TMenuItem *UVCalibrationGNR2;
        void __fastcall OpenMiniDebuggerExecute(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall ShowMonitorActionExecute(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ExitMenuItemClick(TObject *Sender);
        void __fastcall OpenParamsDialogActionExecute(TObject *Sender);
        void __fastcall Gotopurgeposition1Click(TObject *Sender);
        void __fastcall Wipe1Click(TObject *Sender);
        void __fastcall Testpattern1Click(TObject *Sender);
        void __fastcall DoFireAll1Click(TObject *Sender);
        void __fastcall AboutMenuItemClick(TObject *Sender);
        void __fastcall ViewLogFileActionExecute(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall SelectButtonMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall SelectButtonMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall ShowIncomingSlicesActionExecute(TObject *Sender);
        void __fastcall GoOneModeBackActionExecute(TObject *Sender);
        void __fastcall GoBackToDefaultModeActionExecute(TObject *Sender);
        void __fastcall CloseEnhancedButtonClick(TObject *Sender);
        void __fastcall MinimizeEnhancedButtonClick(TObject *Sender);
        void __fastcall ExportConfigurationActionExecute(TObject *Sender);
        void __fastcall ImportConfigurationActionExecute(TObject *Sender);
        void __fastcall ExtractFromLogFileActionExecute(TObject *Sender);
        void __fastcall MaintenancePopupMenuPopup(TObject *Sender);
        void __fastcall Calibration1Click(TObject *Sender);
        void __fastcall OHDBOptions2Click(TObject *Sender);
        void __fastcall ShowHelpContentActionExecute(TObject *Sender);
        void __fastcall EmulationLabelBlinkTimerTimer(TObject *Sender);
        void __fastcall CleanHeadsWizardActionClick(TObject *Sender);
        void __fastcall EnterMaintenanceModeActionExecute(TObject *Sender);
        void __fastcall MCBSWDiagnosticClick(TObject *Sender);
        void __fastcall OpenPythonConsoleActionExecute(TObject *Sender);
        void __fastcall ExitMaintenanceModeActionExecute(TObject *Sender);
        void __fastcall MainUIFrameOpenQuickSHRActionExecute(TObject *Sender);
        void __fastcall SwitchMaintenanceModeActionExecute(TObject *Sender);
        void __fastcall ChooseMaintenanceModeActionExecute(TObject *Sender);
        void __fastcall OpenServiceNotesActionExecute(TObject *Sender);
        void __fastcall SelectButtonMouseEnter(TObject *Sender);
        void __fastcall StopButtonMouseEnter(TObject *Sender);
        void __fastcall OfflineButtonMouseEnter(TObject *Sender);
        void __fastcall OnlineButtonMouseEnter(TObject *Sender);
        void __fastcall PauseButtonMouseEnter(TObject *Sender);
        void __fastcall GoButtonMouseEnter(TObject *Sender);
        void __fastcall ResetButtonLabelActionExecute(TObject *Sender);
        void __fastcall MinimizeEnhancedButtonMouseEnter(TObject *Sender);
        void __fastcall CloseEnhancedButtonMouseEnter(TObject *Sender);
        void __fastcall BackGroundImageMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall BackGroundImageMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall BackGroundImageMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall OpenUVLampsHistoryActionExecute(TObject *Sender);
        void __fastcall ShowTroubleshootingActionExecute(TObject *Sender);
        void __fastcall ShowKeyboardMapActionExecute(TObject *Sender);
        void __fastcall ScreensSelectionActionExecute(TObject *Sender);
        void __fastcall ModesMenuItemClick(TObject *Sender);
	      void __fastcall MainUIFrameShutdownActionExecute(TObject *Sender);
        void __fastcall UVCalibrationWizard2Click(TObject *Sender);
        void __fastcall UVMCalibrationWizard1Click(TObject *Sender);
    void __fastcall CurrentData1Click(TObject *Sender);
    void __fastcall OpenHaspToolActionExecute(TObject *Sender);
    void __fastcall LicenseUpdate1Click(TObject *Sender);
	void __fastcall MaterialBatchNoMenuItemClick(TObject *Sender);
    void __fastcall OCBOHDBVoltages1Click(TObject *Sender);
	void __fastcall HeadsControl1Click(TObject *Sender);
	void __fastcall Wizards1Click(TObject *Sender);
	void __fastcall HeadOptimizationScaleBased1Click(TObject *Sender);


private:	// User declarations

  int             m_LastDay;
  int             m_LastMinute;
  TMenuItem*      m_ModesMenuItem[NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
  int             m_CurrentParamsLevel;
  //OBJET_MACHINE
  bool 			  m_ValidPassword;
  
  TPoint          m_ClickPoint;
  TPoint          m_FormPosWhileClick;
  bool            m_IsMouseDown;
  bool            m_IsDragChanged;
  TParamsDialog*  m_ParamsDlg;
  bool            m_AppInitOk;
  TImageViewForm* m_ImageViewForm[NUMBER_OF_CHAMBERS];
  TMenuItem*      m_CurrentSelectedModeMenuItem;
  bool            m_CurrentMaintenanceMode;
  bool            m_HaspExpirationMessageAlreadyShown;

  void            SetAppCaptionAndHint(void);
  void __fastcall ControlEnableDisableEventHandler(TObject *Sender,int ControlID,bool Value);
  // Refresh the modes display
  void            RefreshModesDisplay(void);
  void __fastcall ModeEnterOnClick(TObject *Sender);
  void __fastcall ParametersApplyEventHandler(TObject *Sender);
  void __fastcall ModesDisplayRefreshEvent(TObject *Sender);
  void __fastcall ScriptMenuEventHandler(TObject *Sender);
  void            RefreshScriptsMenu  (void);
  void            HandleMutexHandleMessage(TMessage &Message);
  // Message handler for the WM_SLICE_IMAGE_UPDATE message - handle slice preview in the main thread
  void            HandleSliceImageUpdateMessage(TMessage &Message);
  void            UpdateModesLabel    (void);
  // Enter/exit to/from maintenance mode, return the previous mode
  bool            SetMaintenanceMode  (bool NewMode);
  bool            DoStartUpBIT        (void);
  // This callback is called by the back-end in order to initiate slice preview event
  static void     SlicePreviewUpdateCallback(TGenericCockie Cockie);
  // Handle the wizard proxy object "run wizard" requests
  static bool     OnRunWizardCallback          (void);

  void CloseMessageDlg();

  //Handle returned from HandlePowerStatus function
  HPOWERNOTIFY m_BatteryPowerNotify;
  HPOWERNOTIFY m_PowerSourceNotify;
  bool 		   m_PowerSource;
  //registers the application for receiving msgs regarding power status changes
  MESSAGE void HandlePowerStatus(TMessage &msg);
  //==== End of Automation Related prototypes ====
  

public:		// User declarations
  __fastcall      TMainForm                       (TComponent* Owner);
  void            LeaveMaintenanceMode            ();
  void            UpdateParameterManager          (int NewLevel);
  void            EnableDisableMaintenanceModes   (int QualityMode, int OperationMode);
  void            EnableDisableAllMaintenanceModes(bool Enabled);
  
  static void ShowDefaultMaintenanceModeObserver(CQParameterBase *Param,TGenericCockie Cockie);
  
  int GetCurrentParamLevel();
  TMainUIFrame *GetUIFrame(void);
  TParamsDialog *GetParamsDialog(void);
  void SetCurrentParamLevel(int ParamLevel);
  //OBJET_MACHINE
  bool GetValidPassword();
  void SetValidPassword(bool ValidPassword);
     
  void UpdateDateTimeDisplay(TDateTime CurrentTime);

  static void HaspStatusLabelUpdate(QString Status, TGenericCockie Cockie);

// Disable warnings about inline functions
#pragma option push -w-inl

virtual void __fastcall Dispatch(void *Message)
{
  if (WM_OBJET_EMBEDED_UNIQUE_MSG == ((PMessage)Message)->Msg)
			HandleMutexHandleMessage(*((TMessage *)Message));
  else if (WM_SLICE_IMAGE_UPDATE == ((PMessage)Message)->Msg)
			HandleSliceImageUpdateMessage(*((TMessage *)Message));
  else if(WM_POWERBROADCAST == ((PMessage)Message)->Msg)
			HandlePowerStatus(*((TMessage *)Message));
  else
      TForm::Dispatch(Message);
}

// Restore previous options
#pragma option pop

};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
