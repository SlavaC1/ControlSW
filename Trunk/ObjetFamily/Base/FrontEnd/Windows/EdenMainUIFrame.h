//---------------------------------------------------------------------------


#ifndef EdenMainUIFrameH
#define EdenMainUIFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include "LCDBaseScreen.h"
#include "MainMenuScreen.h"
#include "MainStatusScreen.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "MaintenanceStatusScreen.h"

#include <stack>
#include "QTypes.h"
#include <Menus.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "Gauges.hpp"
#include "ColorGradient.h"
#include "LCDBaseScreen.h"
#include "MainStatusScreen.h"
#include "EnhancedLabel.hpp"
#include "MainMenuScreen.h"
#include "ModesDefs.h"
#include "HostComm.h"
#include "ShutdownWizard.h"

// Define private windows message for status updates
#define WM_STATUS_UPDATE WM_USER

// Define private windows message for controls enable/disable
#define WM_CONTROL_ENABLE_DISABLE (WM_USER + 1)

class CResinReplacementWizard;
class CBumperCalibrationWizard;
class CQuickSHRWizard;
class CUVCalibrationWizard;
class CVacuumCalibrationWizard;
class CSinglePointWeightCalibrationWizard;
class CPrintingPositionWizard;
class CHostComm;
class CXOffsetWizard;
class CUVReplacementWizard;
class CHeadServiceWizard;
class CAutoWizard;
class CWiperCalibrationWizard;
//class CFactoryShutdownWizard; // deprecated

const int MAIN_STATUS_SCREEN_ID             = 0;
const int MAIN_MAINTENANCE_STATUS_SCREEN_ID = 1;
const int LCD_SCREENS_NUM                   = 2;

typedef void __fastcall (__closure *TControlEnableDisableEvent)(TObject *Sender,int ControlID,bool Value);

typedef void (*THaspStatusLabelUpdateCallback)(QString,TGenericCockie);

typedef enum {uimBusy,uimIdle}              TUIMode;

//---------------------------------------------------------------------------
class TMainUIFrame : public TFrame
{
__published:	// IDE-managed Components
        TToolBar *UpperToolBar;
        TActionList *OpenMaintenanceCountersAction;
        TAction *PausePrintAction;
		TAction *StopPrintAction;
		TAction *PFFPrintJobAction;
        TAction *LoadLocalPrintJobAction;
        TAction *OpenPCIOptionsDlgAction;
        TAction *OpenOHDBOptionsDlgAction;
        TAction *GotoPurgePositionAction;
        TAction *DoWipeAction;
        TAction *DoTestPatternAction;
        TAction *DoFireAllAction;
        TAction *DoFireAllSequenceAction;
        TAction *DoPurgeSequenceAction;
        TAction *EnterStandbyAction;
        TAction *ExitStandbyAction;
        TAction *OptionSelectAction;
        TAction *OptionBackAction;
        TAction *SelectUpAction;
        TAction *SelectDownAction;
        TPanel *ScreensPlaceHolder;
        TMainStatusFrame *MainStatusFrame;
        TMainMenuFrame *MainMenuFrame;
        TMaintenanceStatusFrame *MaintenanceStatusFrame;
        TTimer *DateTimeUpdateTimer;
        TAction *DoCleanHeads;
        TAction *DoCleanWiper;
        TAction *OpenCalibartionDlgAction;
		TAction *OpenTrayPointsWizardAction;
        TAction *DoTrayRemovalPositionAction;//[Deprecated in Objet]
        TAction *ShutdownAction;
        TAction *OpenHeadFillingWizardAction;
        TAction *EnterAdvanceModeAction;
        TAction *OpenGeneralDevicesAction;
        TAction *OpenHeadsControlAction;
        TAction *OpenMotorsControlAction;
        TAction *OpenActuatorsAndSensorsAction;
        TAction *MaintenanceCountersAction;
        TAction *UserWizardsAction;
        TAction *MaintenanceWizardsAction;
        TAction *ModesMenuAction;
        TAction *OnlineAction;
        TAction *OfflineAction;
        TAction *ConfigurationExportImportAction;
        TAction *RollerTiltWizardAction;
        TAction *CleanHeadsWizardAction;
        TAction *CleanWiperWizardAction;
        TAction *ResumePrintAction;
        TAction *OpenMCBDiagDlgAction;
        TAction *OpenBITViewerAction;
        TAction *ScriptsMenuAction;
        TAction *CollectServiceDataAction;
        TAction *BumperCalibrationWizardAction;
        TAction *OpenQuickSHRAction;
        TAction *XOffsetWizardAction;
        TAction *UVCalibrationWizard;
        TAction *VacuumCalibrationAction;
        TAction *LoadCellCalibrationAction;
        TAction *PrintingPositionAction;
        TAction *UVReplacementWizard;
        TAction *UVCalibrationOptionsWizard;
        TAction *UVMCalibrationWizard;
		TAction *HeadsOptimizationAction;
        TAction *HeadsOptimizationScalesBasedAction;
        TAction *DisplayOptimizationWizardAction;
    TAction *WiperCalibrationAction;
    TAction *OpenHaspInfoDlgAction;
	TAction *MaterialBatchNoAction;
	TAction *MachineActionsAction;
	TAction *GoToHeadInspectionAction;
	TAction *OpenOcbOhdbVoltageDlgAction;
	TAction *OpenResinReplacementWizardAction_Noi;
	TAction *UVCalibrationGNRWizard;
	TAction *DoTrayLowerPositionAction;
	TAction *OpenZStartCalibrationWizardAction;
//        TAction *FactoryShutdownWizardAction; // deprecated
        void __fastcall PausePrintActionExecute(TObject *Sender);
		void __fastcall StopPrintActionExecute(TObject *Sender);
		void __fastcall PFFPrintJobActionExecute(TObject *Sender);
        void __fastcall LoadLocalPrintJobActionExecute(TObject *Sender);
        void __fastcall OpenPCIOptionsDlgActionExecute(TObject *Sender);
        void __fastcall OpenOHDBOptionsDlgActionExecute(TObject *Sender);
        void __fastcall DoWipeActionExecute(TObject *Sender);
        void __fastcall GotoPurgePositionActionExecute(TObject *Sender);
        void __fastcall DoTestPatternActionExecute(TObject *Sender);
        void __fastcall DoFireAllActionExecute(TObject *Sender);
        void __fastcall DoFireAllSequenceActionExecute(TObject *Sender);
        void __fastcall DoPurgeSequenceActionExecute(TObject *Sender);
        void __fastcall EnterStandbyActionExecute(TObject *Sender);
        void __fastcall ExitStandbyActionExecute(TObject *Sender);
        void __fastcall SelectUpActionExecute(TObject *Sender);
        void __fastcall SelectDownActionExecute(TObject *Sender);
        void __fastcall SoftSelectBtnMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall SoftSelectBtnMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall OptionBackActionExecute(TObject *Sender);
        void __fastcall DateTimeUpdateTimerTimer(TObject *Sender);
  void __fastcall OpenCalibartionDlgActionExecute(TObject *Sender);
  void __fastcall OpenTrayPointsWizardActionExecute(TObject *Sender);
  void __fastcall OpenZStartCalibrationWizardActionExecute(TObject *Sender);
        void __fastcall DoTrayUpperPositionActionExecute(TObject *Sender);
        void __fastcall MainMenuFrameOptionsMenuClick(TObject *Sender);
        void __fastcall ShutdownActionExecute(TObject *Sender);
  void __fastcall OpenHeadFillingWizardActionExecute(TObject *Sender);
        void __fastcall EnterAdvanceModeActionExecute(TObject *Sender);
        void __fastcall OpenGeneralDevicesActionExecute(TObject *Sender);
        void __fastcall OpenHeadsControlActionExecute(TObject *Sender);
        void __fastcall OpenMotorsControlActionExecute(TObject *Sender);
        void __fastcall OpenActuatorsAndSensorsActionExecute(TObject *Sender);
        void __fastcall MaintenanceCountersActionExecute(TObject *Sender);
        void __fastcall UserWizardsActionExecute(TObject *Sender);
        void __fastcall MaintenanceWizardsActionExecute(TObject *Sender);
        void __fastcall ModesMenuActionExecute(TObject *Sender);
        void __fastcall OnlineActionExecute(TObject *Sender);
        void __fastcall OfflineActionExecute(TObject *Sender);
        void __fastcall ConfigurationExportImportActionExecute(TObject *Sender);
        void __fastcall RollerTiltWizardActionExecute(TObject *Sender);
        void __fastcall CleanHeadsWizardActionExecute(TObject *Sender);
        void __fastcall CleanWiperWizardActionExecute(TObject *Sender);
        void __fastcall ResumePrintActionExecute(TObject *Sender);
        void __fastcall OpenMCBDiagDlgActionExecute(TObject *Sender);
        void __fastcall OpenBITViewerActionExecute(TObject *Sender);
        void __fastcall ScriptsMenuActionExecute(TObject *Sender);
        void __fastcall CollectServiceDataActionExecute(TObject *Sender);
        void __fastcall BumperCalibrationWizardActionExecute(TObject *Sender);
        void __fastcall OpenQuickSHRActionExecute(TObject *Sender);
        void __fastcall XOffsetWizardActionExecute(TObject *Sender);
        void __fastcall UVCalibrationWizardExecute(TObject *Sender);
        void __fastcall VacuumCalibrationActionExecute(TObject *Sender);
        void __fastcall LoadCellCalibrationActionExecute(TObject *Sender);
        void __fastcall PrintingPositionActionExecute(TObject *Sender);
        void __fastcall UVReplacementWizardExecute(TObject *Sender);
//        void __fastcall FactoryShutdownWizardExecute(TObject *Sender); // deprecated
        void __fastcall DisplayOptimizationWizardActionExecute(TObject *Sender);
		void __fastcall HeadsOptimizationActionExecute(TObject *Sender);
		void __fastcall UVCalibrationOptionsWizardExecute(TObject *Sender);
        void __fastcall UVMCalibrationWizardExecute(TObject *Sender);
    void __fastcall WiperCalibrationActionExecute(TObject *Sender);
    void __fastcall OpenHaspInfoDlgActionExecute(TObject *Sender);
	void __fastcall MaterialBatchNoActionExecute(TObject *Sender);
	void __fastcall MachineActionsActionExecute(TObject *Sender);
	void __fastcall GoToHeadInspectionActionExecute(TObject *Sender);
	void __fastcall HeadsOptimizationScalesBasedActionExecute(TObject *Sender);
		void __fastcall OpenOcbOhdbVoltageDlgActionExecute(TObject *Sender);
	void __fastcall OpenResinReplacementWizardAction_NoiExecute(TObject *Sender);
	void __fastcall UVCalibrationGNRWizardExecute(TObject *Sender);
//        void __fastcall FactoryShutdownWizardActionExecute(TObject *Sender); // deprecated
	void __fastcall DoTrayLowerPositionActionExecute(TObject *Sender);

// User declarations
private: //Variables
    Graphics::TBitmap*          m_BkgdImage;
    TPoint                      m_ScreensPlaceHolderPosition;
    int                         m_TotalNumberOfSlices;
    float                       m_TotalModelHeight;
    bool                        m_ShowPrintTime;
    bool                        m_AllowMaintenanceMode;
    // Pointer to the current visible screen (frame)
    TLCDBaseFrame*              m_CurrentScreen;
    std::stack<TLCDBaseFrame *> m_ScreensNavigateStack;
    TLCDBaseFrame*              m_ScreensLookup[LCD_SCREENS_NUM];
    TControlEnableDisableEvent  m_EnableDisableEvent;
    TNotifyEvent                m_ModesDisplayRefreshEvent;
    TQualityModeIndex           m_MaintenanceMode;
    TOperationModeIndex         m_OperationMode;
    TDateTime                   m_PrintTime,m_LastPrintTimeUpdate;
	bool                        m_HaspConnected;
	int m_PrintTimeSec;      

    // Wizards
	CResinReplacementWizard*             m_ResinReplacementWizard;
	CQuickSHRWizard*                     m_QuickSHRWizard;
	CBumperCalibrationWizard*            m_BumperCalibrationWizard;
    CXOffsetWizard*                      m_XOffsetWizard;
    CUVCalibrationWizard*                m_UVCalibrationWizard;
    CVacuumCalibrationWizard*            m_VacuumCalibrationWizard;
    CSinglePointWeightCalibrationWizard* m_SinglePointCalibrationWizard;
    CPrintingPositionWizard*             m_PrintingPositionWizard;
    CUVReplacementWizard*                m_UVReplacementWizard;
    CHeadServiceWizard*                  m_HeadServiceWizard;
	ShutdownWizard::CShutdownWizard*     m_ShutdownWizard;
    CWiperCalibrationWizard*             m_WiperCalibrationWizard;
//    CFactoryShutdownWizard *             m_FactoryShutdownWizard; // deprecated

private: //Methods
    TColor HeaterValueToColor                     (int Value);
    void UpdateHeadHeaterPanel                    (TPanel *Panel,int HeadNo,int Value);
    void UpdateTrayHeaterPanel                    (TPanel *Panel,int Value);
    void UpdateAmbientPanel                       (TPanel *Panel,int Value);
    void UpdateHeadFillingsPanel                  (TPanel *Panel,int Value);
    void UpdateIfHeadIsFilledPanel                (TPanel *Panel,int Filled);
    void RunHeadAlignmentWizard                   ();
    void UpdatePrintTime                          ();
    bool RunPrintingWizard                        (CAutoWizard *Wizard,int StartingPage = 0);
    void ChangeOperationModeByResinReplacement    (int NewOperationMode, TObject *Sender);

    // Change view to a new screen
    void ChangeScreen                             (TLCDBaseFrame *NewScreen);
    // Message handler for the WM_STATUS_UPDATE message
    MESSAGE void HandleStatusUpdateMessage        (TMessage &Message);
    // Message handler for the WM_CONTROL_ENABLE_DISABLE message
    MESSAGE void HandleControlEnableDisableMessage(TMessage &Message);
    // Set the user interface mode
    void SetUIMode                                (TUIMode UIMode);
    QString PythonWizardFileName                  (const QString WizardName);
    void LoadWizardButtonsBmp();
    void __fastcall ExecuteOpenMRWSilentlyAction(TObject *Sender,bool isBlocking, bool updateSegments);

    THaspStatusLabelUpdateCallback m_HaspStatusLabelUpdateCallback;
    TGenericCockie                 m_Cockie;

public:		// User declarations

  // Constructor
  __fastcall TMainUIFrame(TComponent* Owner);
  // Destructor
  __fastcall ~TMainUIFrame(void);
  // Update status on the frame (thread safe)
  void       UpdateStatus       (int ControlID,int Status);
  void       UpdateStatus       (int ControlID,float Status);
  void       UpdateStatus       (int ControlID,QString Status);
  void       GetMaintenanceMode (TQualityModeIndex&   MaintenanceMode,
                                 TOperationModeIndex& OperationMode);
  void       SetMaintenanceMode (TQualityModeIndex   MaintenanceMode,
                                TOperationModeIndex  OperationMode);

  // Enable/Disable a control (thread safe)
  void       EnableDisableControl(int ControlID,bool Enable);
  bool       RunPythonWizard     (const QString PythonFileName);
  bool       RunWizard           (CAutoWizard *Wizard,int StartingPage = 0);

  void SetHaspStatus(bool Connected);
  bool GetHaspStatus();
  void SetHaspStatusLabelUpdateCallback(THaspStatusLabelUpdateCallback, TGenericCockie);

// Disable warnings about inline functions
#pragma option push -w-inl

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_STATUS_UPDATE,TMessage,HandleStatusUpdateMessage);
  MESSAGE_HANDLER(WM_CONTROL_ENABLE_DISABLE,TMessage,HandleControlEnableDisableMessage);
END_MESSAGE_MAP(TFrame);

// Restore previous options
#pragma option pop

  void   InitUIComponents(void);
  void   DeInitUIComponents(void);
  TPoint GetLCDRelativeCoordinates(void);
  void   SetCompactMode(bool Mode);
  void   NavigateForward(int NewScreenID);
  void   NavigateBackward(void);
  void   LoadSuitableMode();

  TLCDBaseFrame* GetCurrentScreen(void);
  void SetModesDisplayRefreshEvent(TNotifyEvent ModesDisplayRefreshEvent);
  bool IsMaintenanceModeAllowed();
  TMaintenanceStatusFrame* GetMaintenanceStatusFrame();

__published:

  __property TControlEnableDisableEvent OnEnableDisableEvent = {read=m_EnableDisableEvent,write=m_EnableDisableEvent};
};

//---------------------------------------------------------------------------
#endif
