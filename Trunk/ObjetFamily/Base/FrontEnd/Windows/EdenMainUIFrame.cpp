
#include <vcl.h>
#pragma hdrstop

#pragma package(smart_init)
#pragma link "LCDBaseScreen"
#pragma link "MainStatusScreen"
#pragma link "EnhancedLabel"

#include "ActuatorsControlDlg.h"
#include "LotusLowLevel.h" //for simulator only
#include "AppLogFile.h"
#include "AppParams.h"
#include "AutoWizardDlg.h"
#include "BackEndInterface.h"
#include "BITViewerDlg.h"
#include "BumperCalibrationWizard.h"
#include "CalibrationDlg.h"
#include "CustomResinMainSelectWizardFrame.h"
#include "CustomResinSelectAdvancedFrame.h"
#include "CustomResinSelectSummaryFrame.h"
#include "CustomTrayPointsWizardFrame.h"
#include "CustomZStartWizardFrame.h"
#include "CustomRollerTiltWizardFrame.h"
#include "DoorDlg.h"
#include "EdenMainUIFrame.h"
#include "FEResources.h"
#include "FrontEnd.h"
#include "FrontEndControlIDs.h"
#include "FrontEndParams.h"
#include "GeneralDevicesDlg.h"
#include "HeadsCheckBoxesFrame.h"
#include "MissingNozzlesFrame.h"
#include "HeadsControlDlg.h"
#include "PFFDlg.h"
#include "LocalJobDlg.h"
#include "MaintenanceCountersDlg.h"
#include "MainUnit.h"
#include "MCBDiagDlg.h"
#include "MotorsControlDlg.h"
#include "OHDBOptionsDlg.h"
#include "HeadServiceWizard.h"
#include "ParamsDialog.h"
#include "PCIOptionsDlg.h"
#include "PrintingPointsPositionsWizard.h"
#include "PythonAutoWizard.h"
#include "Q2RTApplication.h"
#include "QErrors.h"
#include "QMonitor.h"
#include "QThreadUtils.h"
#include "QuickSHRWizard.h"
#include "ResinReplacementWizard.h"
#include "RollerWizardYMovementFrame.h"
#include "AgreementPage.h"
#include "AgreementPageFrame.h"
#include "SinglePointWeightCalibrationWizard.h"
#include "StableValueFrame.h"
#include "StableValueWizardPage.h"
#include "UserValuesEntryPage.h"
#include "UserValuesEntryWizardFrame.h"
#include "UVCalibrartionWizard.h"
#include "UVLampsStatusFrame.h"
#include "UVLampsResultsFrame.h"
#include "UVLampsCalibrationFrame.h"
#include "UVReplacementWizard.h"
#include "VacuumCalibrartionWizard.h"
#include "VacuumCalibrationWizardPage.h"
#include "VacuumStatusFrame.h"
#include "WizardImages.h"
#include "XOffsetChooseBestPrintFrame.h"
#include <cstring>
#include "XOffsetWizard.h"
#include "BackEndInterface.h"
#include "ShutdownWizard.h"
#include "HeadsButtonsEntryPage.h"
#include "HeadsButtonsEntryFrame.h"
#include "WiperCalibrationWizard.h"
#include "CustomWiperCalibrationPage.h"
#include "CustomWiperCalibrationFrame.h"
#include "HaspPlugInfoDlg.h"
#include "QScopeRestorePoint.h"
#include "CustomLoadCellCalibrationPage.h"
#include "CustomLoadCellWellcomeFrame.h"
#include "CustomLoadCellWellcomePage.h"
#include "LoadCellCalibration.h"
#include "OcbOhdbControlDlg.h"
#include "WeightsStatusFrame.h"
#include "WeightsStatusWizardPage.h"
#include "ConfigBackupFile.h"
#include "MissingNozzlesScaleBasePage.h"
#include "MaterialReplacementDonePage.h"
#include "MaterialReplacementDoneFrame.h"
//#include "FactoryShutdownWizard.h" // deprecated
#pragma link "MainMenuScreen"
#pragma link "LCDBaseScreen"
#pragma link "MainMenuScreen"
#pragma link "MainStatusScreen"
#pragma link "MaintenanceStatusScreen"
#pragma resource "*.dfm"

// This global flag is used to mark that a program restart is required
extern bool AppRestartFlag;

const char COLLECT_DATA_SCRIPT_NAME[] = "Scripts\\~CollectData.py";

const int MinutesInHour = 60;
const int SecondsInMinute = 60;
//const int HoursPerDay = 24;
void ExploreFile(const char *FileName);


//---------------------------------------------------------------------------
// Constructor
__fastcall TMainUIFrame::TMainUIFrame(TComponent* Owner)
        : TFrame(Owner)
{
  
   memset(&m_EnableDisableEvent, 0 , sizeof(TControlEnableDisableEvent));
   m_BkgdImage = NULL;
   m_TotalNumberOfSlices = 0 ;
   m_TotalModelHeight    = 0 ;
   m_AllowMaintenanceMode = false;
   m_CurrentScreen = NULL;
   m_PrintTimeSec = 0;
   m_ResinReplacementWizard = NULL;
   m_QuickSHRWizard = NULL;
   m_BumperCalibrationWizard = NULL;
   m_XOffsetWizard = NULL;
   m_UVCalibrationWizard = NULL;
   m_VacuumCalibrationWizard = NULL;
   m_SinglePointCalibrationWizard = NULL;
   m_PrintingPositionWizard = NULL;
   m_UVReplacementWizard = NULL;
   m_HeadServiceWizard = NULL;
   m_ShutdownWizard = NULL;
   m_WiperCalibrationWizard = NULL;
//   m_FactoryShutdownWizard = NULL; // deprecated
   for(int i = 0; i< LCD_SCREENS_NUM; i++)
   {
       m_ScreensLookup[i] = NULL;
   }
  // Remember the position of this panel
  m_ScreensPlaceHolderPosition.x = ScreensPlaceHolder->Left;
  m_ScreensPlaceHolderPosition.y = ScreensPlaceHolder->Top;
  m_ModesDisplayRefreshEvent     = NULL;
  m_ShowPrintTime                = false;
  CAutoWizard::SetAppDefaultImageID(IN_PROCESS_IMAGE_ID);
  TWizardViewerFrame::SetTimeOutMechanismEnable(true);

  m_HaspConnected                 = true;
  m_HaspStatusLabelUpdateCallback = NULL;
  m_Cockie                        = 0;
}
//---------------------------------------------------------------------------
// Destructor
__fastcall TMainUIFrame::~TMainUIFrame(void)
{
  Q_SAFE_DELETE( m_ResinReplacementWizard);
  Q_SAFE_DELETE( m_QuickSHRWizard);
  Q_SAFE_DELETE( m_BumperCalibrationWizard);
  Q_SAFE_DELETE( m_XOffsetWizard);
  Q_SAFE_DELETE( m_UVCalibrationWizard);
  Q_SAFE_DELETE( m_VacuumCalibrationWizard);
  Q_SAFE_DELETE( m_SinglePointCalibrationWizard);
  Q_SAFE_DELETE( m_PrintingPositionWizard);
  Q_SAFE_DELETE( m_UVReplacementWizard);
  Q_SAFE_DELETE( m_HeadServiceWizard);
  Q_SAFE_DELETE( m_ShutdownWizard);
  Q_SAFE_DELETE( m_WiperCalibrationWizard);
//  Q_SAFE_DELETE( m_FactoryShutdownWizard);
}
//---------------------------------------------------------------------------
// Message handler for the WM_STATUS_UPDATE message
void TMainUIFrame::HandleStatusUpdateMessage(TMessage &Message)
{
  int Tmp ;
  if (VALIDATE_CHAMBER(Tmp = Message.WParam - FE_SET_DRAIN_PUMPS_BASE))
    MainStatusFrame->UpdateDrainPumpsStatus((TChamberIndex)Tmp, (TTankIndex)Message.LParam);

  switch(Message.WParam)
  {
    case FE_TOTAL_NUM_OF_SLICES:
      // Remember for later
      m_TotalNumberOfSlices = Message.LParam;
      MainStatusFrame->PrintProgressBar->Max = Message.LParam;
      MainStatusFrame->PrintProgressBar->Position = 0;
      break;

    case FE_TOTAL_MODEL_HEIGHT:
      // Remember for later
      m_TotalModelHeight = *((float *)&Message.LParam);
      break;

    case FE_CURRENT_SLICE:
      {
        // Shot the slices to the user in "1" based counting
        int SliceNum = Message.LParam;
        float PercentDone;
        if (SliceNum == -1)
        {
           MainStatusFrame->SliceProgressLabel->Caption = "";
           PercentDone = 0;
        }
        else
        {
           PercentDone = (float)SliceNum * 100.0f / (float)(m_TotalNumberOfSlices);
           MainStatusFrame->SliceProgressLabel->Caption = Format("Slice %d of %d (%.1f%%)",
                                                            ARRAYOFCONST((SliceNum,m_TotalNumberOfSlices,PercentDone)));
        }
        MainStatusFrame->PrintProgressBar->Position = SliceNum;
      }
      break;

    case FE_PREVIOUS_JOB_LAST_SLICE:
      {
        int SliceNum = Message.LParam;
        MainStatusFrame->PreviousJobLabel->Caption = Format("Previous job last slice: %d",ARRAYOFCONST((SliceNum)));
      }
      break;

    case FE_CURRENT_LAYER:
      break;

    case FE_CURRENT_MACHINE_STATE:
    {
      MainStatusFrame->CurrentStateLabel->Caption = StatesLookup[Message.LParam].c_str();

      bool isStarted = ((msPowerUp != (Message.LParam)) && (msInitializing != (Message.LParam)));
      MainForm->OptionsMenuLabel->Enabled     = isStarted;
      MainForm->MaintenanceMenuLabel->Enabled = isStarted;


	  switch(Message.LParam)
      {
        case msPrePrint:
		  m_PrintTime = 0;
		  m_PrintTimeSec = 0;
		  break;
        case msPrinting:
          m_LastPrintTimeUpdate = Now();
          m_ShowPrintTime = true;
          break;

        case msStopped:
        case msPaused:
        case msIdle:
        case msStandby1:
        case msStandby2:
          m_ShowPrintTime = false;
          break;
      }
      break;
    }
    case FE_CURRENT_MODEL_HEIGHT:
      MainStatusFrame->HeightProgressLabel->Caption =   Format("Height %.2fmm of %.2fmm",
                                                        ARRAYOFCONST((*((float *)&Message.LParam),m_TotalModelHeight)));
      break;

    CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_CURRENT_TANK_RELATIVE_WEIGHT)
      MainStatusFrame->SetContainerData((TTankIndex)(Message.WParam-FE_CURRENT_TANK_RELATIVE_WEIGHT_BASE), (float)Message.LParam);
      break;

	CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_TANK_ENABLING_STATUS)
      MainStatusFrame->UpdateTankEnabledStatus((TTankIndex)(Message.WParam-FE_TANK_ENABLING_STATUS_BASE), Message.LParam != 0? true : false);
      break;

	CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_TANK_DISABLING_HINT)
    {
      char* StrPtr = reinterpret_cast<char *>(Message.LParam);
      MainStatusFrame->SetDisabledIconHint((TTankIndex)(Message.WParam-FE_TANK_DISABLING_HINT_BASE), StrPtr);
      free(StrPtr);
      break;
    }

    CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_CURRENT_TANK_WEIGHT)
    {
	  TTankIndex Index    = (TTankIndex)(Message.WParam - FE_CURRENT_TANK_WEIGHT_BASE);

	  // Show weight above 10 Kg in "Kg" units instead of "gr"
	  float Weight = (float)Message.LParam / (float)1000;
	  QString Caption = (Message.LParam >= 10000) ? Format("%.2f Kg", ARRAYOFCONST((Weight))).c_str() :
													Format("%d gr", ARRAYOFCONST((Message.LParam))).c_str();
	  // Updating the caption
	  MainStatusFrame->m_ResinWeightLabelArray[Index]->Caption = Caption.c_str();
      break;
    }

	CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_TANK_EXISTENCE_STATUS)
    {
      MainStatusFrame->SetContainerVisible((TTankIndex)(Message.WParam-FE_TANK_EXISTENCE_STATUS_BASE), Message.LParam != 0? true : false);
      break;
    }

	CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_CURRENT_TANK_STATUS)
    {
      MainStatusFrame->SetContainerStatus((TTankIndex)(Message.WParam-FE_CURRENT_TANK_STATUS_BASE), Message.LParam);
      break;
    }

	CASE_MESSAGE_ALL_TANKS_INCLUDING_WASTE(FE_SET_ACTIVE_TANK)
    {
      TTankIndex Index    = (TTankIndex)(Message.WParam-FE_SET_ACTIVE_TANK_BASE);
      MainStatusFrame->SetActiveContainer((TChamberIndex)Message.LParam, Index);
      break;
    }
    CASE_MESSAGE_ALL_TANKS(FE_UPDATE_RESIN_TYPE)
    {
      char* StrPtr = reinterpret_cast<char *>(Message.LParam);
      MainStatusFrame->UpdateResinType((TTankIndex)(Message.WParam-FE_UPDATE_RESIN_TYPE_BASE), StrPtr);
      free(StrPtr);
      break;
    }

    case FE_MACHINE_SUB_STATUS:
      {
        char *StrPtr = reinterpret_cast<char *>(Message.LParam);
        MainStatusFrame->SubStatusLabel->Caption = StrPtr;
        free(StrPtr);
        break;
      }

    case FE_UPDATE_UI_MODE:
        SetUIMode((Message.LParam == FE_UI_BUSY_MODE) ? uimBusy: uimIdle);
        break;


#ifdef ALLOW_OPENING_MRW_FROM_HOST
    case FE_HOST_RENDING_RESIN_REPLACEMENT:
         //Open Resin Replacement Wizard
         break;
#endif

    case FE_NEW_JOB:
         if(Message.LParam == FE_UI_NO_MODE)
            MainForm->EnableDisableAllMaintenanceModes(true);
         else
         {
            int OperationMode, QualityMode;
            TAG_TO_MODE(Message.LParam, QualityMode, OperationMode);
            MainForm->EnableDisableMaintenanceModes(QualityMode,OperationMode);
            MainForm->LeaveMaintenanceMode();
         }
         break;

    case FE_REFRESH_MODES_DISPLAY:
      // Call the display refresh event
      if(m_ModesDisplayRefreshEvent)
        m_ModesDisplayRefreshEvent(this);
      break;

    case FE_SERVICE_ALERT:
      {
        int LookupIndex = Message.LParam;

        // Verify string lookup index
        if((LookupIndex >= 0) && (static_cast<unsigned>(LookupIndex) < (sizeof(ServiceWarningLookup) / sizeof(char *))))
        {
          MainStatusFrame->ServiceAlertLabel->Caption = ServiceWarningLookup[LookupIndex];
          MainStatusFrame->ServiceAlertLabel->Visible = true;
        }
      }
      break;

    case FE_SHOW_BIT_RESULT:
      OpenBITViewerAction->Execute();
      break;

    case FE_CLEAR_SERVICE_ALERT:
      MainStatusFrame->ServiceAlertLabel->Visible = false;
      break;

    case FE_CURRENT_MODEL_NAME:
    {
	  char *StrPtr = reinterpret_cast<char *>(Message.LParam);
      MainForm->JobNameLabel->Caption = StrPtr;
      MainForm->JobNameLabel->Visible = true;
      free(StrPtr);
      break;
    }

    case FEOP_RUN_MRW_WIZARD_SILENTLY:
      ExecuteOpenMRWSilentlyAction(this, NO_BLOCKING, (bool)Message.LParam /*update Segments*/);
      break;
      
    case FEOP_RUN_BLOCKING_MRW_WIZARD_SILENTLY:
      ExecuteOpenMRWSilentlyAction(this, BLOCKING, (bool)Message.LParam /*update Segments*/);
      break;

    case FEOP_RUN_MRW_WIZARD:
      OpenResinReplacementWizardAction_NoiExecute(this);//Open Resin Replacement Wizard
      break;

	case FEOP_RUN_SHD_WIZARD:
      ShutdownActionExecute(this);
      break;

    case FE_HASP_UPDATE_STATUS_LABEL:
      if(m_HaspStatusLabelUpdateCallback)
      {
		char *StrPtr = reinterpret_cast<char *>(Message.LParam);
        (*m_HaspStatusLabelUpdateCallback)(StrPtr, m_Cockie);
      }
      break;
	case FE_HEAD_CLEANING_WIZARD:  //itamar, Super purge
        CleanHeadsWizardActionExecute(this);
		break;

	case FE_SET_MACHINE_LOGO:
		{
			char *PackageName = reinterpret_cast<char *>(Message.LParam);
			MainForm->PackageNameLabel->Caption       = PackageName;
			MainForm->PackageNameShadowLabel->Caption = PackageName;
		}
		break;
	case FE_SET_DOOR_DLG_LABEL:
		{
		   //	int *messageIndex = reinterpret_cast<int *>(Message.LParam);
			DoorCloseDlg->ChangeDoorsGuiLabels(Message.LParam);
			break;
		}
  }

  // Update maintenance related statuses
  MaintenanceStatusFrame->UpdateStatusIndicators(Message.WParam,Message.LParam);
}
//---------------------------------------------------------------------------
// Message handler for the WM_CONTROL_ENABLE_DISABLE message
void TMainUIFrame::HandleControlEnableDisableMessage(TMessage &Message)
{
  if(m_EnableDisableEvent)
    m_EnableDisableEvent(this,Message.WParam,Message.LParam);
}
//---------------------------------------------------------------------------
// Update status on the frame (integer version)
void TMainUIFrame::UpdateStatus(int ControlID,int Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}
//---------------------------------------------------------------------------
// Update status on the frame (float version)
void TMainUIFrame::UpdateStatus(int ControlID,float Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}
//---------------------------------------------------------------------------
// Update status on the frame (string version)
void TMainUIFrame::UpdateStatus(int ControlID,QString Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}
//---------------------------------------------------------------------------
void TMainUIFrame::GetMaintenanceMode(TQualityModeIndex&   MaintenanceMode,
                                      TOperationModeIndex& OperationMode)
{
  MaintenanceMode = m_MaintenanceMode;
  OperationMode   = m_OperationMode;
}
//---------------------------------------------------------------------------
void TMainUIFrame::SetMaintenanceMode(TQualityModeIndex   MaintenanceMode,
                                      TOperationModeIndex OperationMode)
{
  m_MaintenanceMode = MaintenanceMode;
  m_OperationMode   = OperationMode;
}
//---------------------------------------------------------------------------
// Enable/Disable a control (thread safe)
void TMainUIFrame::EnableDisableControl(int ControlID,bool Enable)
{
  PostMessage(Handle,WM_CONTROL_ENABLE_DISABLE,ControlID,Enable ? 1 : 0);
}
//---------------------------------------------------------------------------
void TMainUIFrame::InitUIComponents(void)
{
  m_QuickSHRWizard               = new CQuickSHRWizard;
  m_ResinReplacementWizard       = new CResinReplacementWizard;
  m_BumperCalibrationWizard      = new CBumperCalibrationWizard;
  m_XOffsetWizard                = new CXOffsetWizard;
  m_UVCalibrationWizard          = new CUVCalibrationWizard;
  m_VacuumCalibrationWizard      = new CVacuumCalibrationWizard;
  m_SinglePointCalibrationWizard = new CSinglePointWeightCalibrationWizard;
  m_PrintingPositionWizard       = new CPrintingPositionWizard;
  m_UVReplacementWizard          = new CUVReplacementWizard;
  m_HeadServiceWizard            = new CHeadServiceWizard;
  m_ShutdownWizard               = new ShutdownWizard::CShutdownWizard;
  m_WiperCalibrationWizard       = new CWiperCalibrationWizard;
//  m_FactoryShutdownWizard        = new CFactoryShutdownWizard; // deprecated

  m_ResinReplacementWizard->SetWizardForm(WizardForm);

  // Load the main screen (MAX screen) background image from a resource
  m_BkgdImage = new Graphics::TBitmap;
  LOAD_BITMAP(m_BkgdImage, IDB_LCD_BKGD);

  // Assign the background image to all the LCD screens
  MainStatusFrame->SetBackgroundImage(m_BkgdImage);
  MainMenuFrame->SetBackgroundImage(m_BkgdImage);
  MaintenanceStatusFrame->SetBackgroundImage(m_BkgdImage);

  // Adjust the frame size to the frames holder size
  MainStatusFrame->Align        = alClient;
  MainMenuFrame->Align          = alClient;
  MaintenanceStatusFrame->Align = alClient;

  m_CurrentScreen = MainStatusFrame;

  // Preprae the LCD screens lookup
  m_ScreensLookup[MAIN_STATUS_SCREEN_ID] = MainStatusFrame;
  //  m_ScreensLookup[MAIN_MENU_SCREEN_ID] = MainMenuFrame;
  m_ScreensLookup[MAIN_MAINTENANCE_STATUS_SCREEN_ID] = MaintenanceStatusFrame;

  // Set the waste weight display visibility
  MaintenanceStatusFrame->WasteEnhancedLabel->Visible       = CAppParams::Instance()->ShowWasteWeight;
  MaintenanceStatusFrame->WasteWeightEnhancedLabel->Visible = CAppParams::Instance()->ShowWasteWeight;
}
//---------------------------------------------------------------------------
void TMainUIFrame::DeInitUIComponents(void)
{
    delete m_BkgdImage;
}
//---------------------------------------------------------------------------
TPoint TMainUIFrame::GetLCDRelativeCoordinates(void)
{
  return ScreensPlaceHolder->ClientToScreen(Point(0,0));
}
//---------------------------------------------------------------------------
// Set the frame to compact mode (true = compact mode)
void TMainUIFrame::SetCompactMode(bool CompactMode)
{
  if(!CompactMode)
  {
    ScreensPlaceHolder->Left = m_ScreensPlaceHolderPosition.x;
    ScreensPlaceHolder->Top = m_ScreensPlaceHolderPosition.y;
  }
}
//---------------------------------------------------------------------------
// Change view to a new screen (use screen ID)
void TMainUIFrame::ChangeScreen(TLCDBaseFrame *NewScreen)
{
  // Hide current screen and show new screen
  m_CurrentScreen->Visible = false;
  m_CurrentScreen = NewScreen;
  m_CurrentScreen->Visible = true;
}
//---------------------------------------------------------------------------
// Go to a new screen and add to the navigation stack
void TMainUIFrame::NavigateForward(int NewScreenID)
{
  TLCDBaseFrame *NewScreen = m_ScreensLookup[NewScreenID];

  // If different screen
  if(NewScreen != m_CurrentScreen)
  {
    // Add to the screens stack
    m_ScreensNavigateStack.push(m_CurrentScreen);

    ChangeScreen(NewScreen);
  }
}
//---------------------------------------------------------------------------
// Go one screen backward
void TMainUIFrame::NavigateBackward(void)
{
  if(!m_ScreensNavigateStack.empty())
  {
    TLCDBaseFrame *PrevScreen = m_ScreensNavigateStack.top();
    m_ScreensNavigateStack.pop();

    ChangeScreen(PrevScreen);
  }
}
//---------------------------------------------------------------------------
void TMainUIFrame::SetHaspStatus(bool Connected)
{
    m_HaspConnected = Connected;
}
//---------------------------------------------------------------------------
bool TMainUIFrame::GetHaspStatus()
{
    return m_HaspConnected;
}
// Set the user interface mode
void TMainUIFrame::SetUIMode(TUIMode UIMode)
{
  bool BusyFlag = (UIMode != uimBusy);

  m_AllowMaintenanceMode                    = BusyFlag;
  OpenMotorsControlAction->Enabled          = BusyFlag;
  PFFPrintJobAction->Enabled          		= BusyFlag & m_HaspConnected;
  LoadLocalPrintJobAction->Enabled          = BusyFlag & m_HaspConnected;
  DoTrayRemovalPositionAction->Enabled      = BusyFlag;
  GotoPurgePositionAction->Enabled          = BusyFlag;
  DoWipeAction->Enabled                     = BusyFlag;
  DoFireAllSequenceAction->Enabled          = BusyFlag;
  DoPurgeSequenceAction->Enabled            = BusyFlag;
  DoCleanWiper->Enabled                     = BusyFlag;
  DoCleanHeads->Enabled                     = BusyFlag;
  DoTestPatternAction->Enabled              = BusyFlag;
  OpenResinReplacementWizardAction_Noi->Enabled = BusyFlag;
  OpenTrayPointsWizardAction->Enabled       = BusyFlag;
  OpenZStartCalibrationWizardAction->Enabled = BusyFlag;
  OpenHeadFillingWizardAction->Enabled      = BusyFlag;
  ShutdownAction->Enabled                   = BusyFlag;
  OpenCalibartionDlgAction->Enabled         = BusyFlag;
  DoFireAllAction->Enabled                  = BusyFlag;
  EnterStandbyAction->Enabled               = BusyFlag;
  ExitStandbyAction->Enabled                = BusyFlag;
  MaintenanceWizardsAction->Enabled         = BusyFlag;
  UserWizardsAction->Enabled                = BusyFlag;
  ModesMenuAction->Enabled                  = BusyFlag & m_HaspConnected;
  ConfigurationExportImportAction->Enabled  = BusyFlag;
  ScriptsMenuAction->Enabled                = BusyFlag;
  OpenBITViewerAction->Enabled              = BusyFlag;
  CollectServiceDataAction->Enabled         = BusyFlag;
  GoToHeadInspectionAction->Enabled			= BusyFlag;
  DoTrayLowerPositionAction->Enabled       	= BusyFlag;

  HeadsOptimizationScalesBasedAction->Enabled = BusyFlag & m_HaspConnected;

  if(BusyFlag)
    MainForm->OpenParamsDialogAction->Enabled = BusyFlag;
  MainForm->OpenHaspToolAction->Enabled     = BusyFlag;

  // Allow 'Apply' just in debug cases...
  MainForm->GetParamsDialog()->ApplyEnabled = BusyFlag;

  // Show/Hide Bit Menu item according to parameter
  if(CAppParams::Instance()->ShowBitMenuItem == false)
    OpenBITViewerAction->Visible = false;

  // Show/Hide Scripts Menu item according to parameter
  if(CAppParams::Instance()->ShowScriptsMenuItem == false)
    ScriptsMenuAction->Visible = false;

  if(!BusyFlag)
    if(MotorsControlForm->Visible)
      MotorsControlForm->Close();

    MainForm->OpenHaspToolAction->Enabled = BusyFlag;
}
//---------------------------------------------------------------------------
QString TMainUIFrame::PythonWizardFileName(const QString WizardName)
{
  return (Q2RTApplication->AppFilePath.Value() + "Wizards\\" + WizardName + ".py");
}

//---------------------------------------------------------------------------
void TMainUIFrame::UpdatePrintTime()
{
  int h,m,s;
  m_PrintTimeSec += 1; //in secs
  //d = InSeconds / (SecondsInMinute * MinutesInHour * HoursInDay);
  h = m_PrintTimeSec/(SecondsInMinute * MinutesInHour);
  m = (m_PrintTimeSec/SecondsInMinute)%SecondsInMinute;
  s = m_PrintTimeSec % SecondsInMinute;

  MainStatusFrame->PrintTimeLabel->Caption = Format("Print Time %.2d:%.2d:%.2d",ARRAYOFCONST((h,m,s)));
}
//---------------------------------------------------------------------------
void TMainUIFrame::LoadSuitableMode()
{
  // Load the 'Maintenance Mode' only if the machine's state allow it
  if (!m_AllowMaintenanceMode)
     return;

  if(!VALIDATE_QUALITY_MODE(m_MaintenanceMode) ||
     !VALIDATE_OPERATION_MODE(m_OperationMode))
      return;

  CBackEndInterface* BackEndInterface = CBackEndInterface::Instance();
  BackEndInterface->GotoDefaultMode();
  // Loading the suitable mode according to the selected one....
  BackEndInterface->EnterMode(PRINT_MODE[m_MaintenanceMode][m_OperationMode],
                       MACHINE_QUALITY_MODES_DIR(m_MaintenanceMode,m_OperationMode));
}
//---------------------------------------------------------------------------
void TMainUIFrame::LoadWizardButtonsBmp()
{
  LOAD_BITMAP(WizardForm->WizardViewerFrame->NextButton->Glyph,IDB_NEXT);
  LOAD_BITMAP(WizardForm->WizardViewerFrame->PrevButton->Glyph,IDB_PREV);
  LOAD_BITMAP(WizardForm->WizardViewerFrame->HelpButton->Glyph,IDB_HELP);
}
//---------------------------------------------------------------------------
bool TMainUIFrame::RunPythonWizard(const QString PythonFileName)
{
  bool ret = false;
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  LoadWizardButtonsBmp();
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  BackEnd->MarkPythonWizardRunning(true);
  CQ2RTPythonAutoWizard Wizard(PythonWizardFileName(PythonFileName));

  try
  {
	ret = RunWizard(&Wizard);
  }
  __finally
  {
	BackEnd->MarkPythonWizardRunning(false);
  }
  return ret;
}
//---------------------------------------------------------------------------
bool TMainUIFrame::RunWizard(CAutoWizard *Wizard,int StartingPage)
{
  bool ret = false;
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  LoadWizardButtonsBmp();
  BackEnd->MarkWizardRunning(true);
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  try
  {
	  ret = WizardForm->RunWizard(Wizard, StartingPage);
  }
  __finally
  {
	  BackEnd->MarkWizardRunning(false);
  }
  return ret;
}
//---------------------------------------------------------------------------
void TMainUIFrame::RunHeadAlignmentWizard()
{
  WizardForm->RegisterCustomPageViewer(wptXOffsetWizardPage             , new CCustomXOffsetSelectPrintWizardPageViewer);
  WizardForm->RegisterCustomPageViewer(wptHeadsCheckboxesWizardPage     , new CHeadsCheckBoxesPageViewer);

  RunPrintingWizard(m_XOffsetWizard);
}
//---------------------------------------------------------------------------
TLCDBaseFrame* TMainUIFrame::GetCurrentScreen(void)
{
  return m_CurrentScreen;
}
//---------------------------------------------------------------------------
void TMainUIFrame::SetModesDisplayRefreshEvent(TNotifyEvent ModesDisplayRefreshEvent)
{
  m_ModesDisplayRefreshEvent = ModesDisplayRefreshEvent;
}
 //---------------------------------------------------------------------------
TMaintenanceStatusFrame* TMainUIFrame::GetMaintenanceStatusFrame()
{
  return MaintenanceStatusFrame;
}
//---------------------------------------------------------------------------
void TMainUIFrame::SetHaspStatusLabelUpdateCallback(THaspStatusLabelUpdateCallback HaspStatusLabelUpdateCallback, TGenericCockie Cockie)
{
    m_HaspStatusLabelUpdateCallback = HaspStatusLabelUpdateCallback;
    m_Cockie                        = Cockie;
}
//---------------------------------------------------------------------------
bool TMainUIFrame::RunPrintingWizard(CAutoWizard *Wizard,int StartingPage)
{
  QString Msg;
  bool ret = true;
  CAppParams* ParamMgr = CAppParams::Instance();
  // Disable materials warning messages during the wizard
  FrontEndInterface->EnableMaterialWarning(false);
  if (ParamMgr->IsPrintingAllowed(Msg) == false)
  {
     QMonitor.NotificationMessage(Msg);
     ret = false;
  }
  else
  {
   if (StartingPage != 0)
    if (!QMonitor.AskYesNo("Last wizard session was aborted.\n\rDo you wish to continue previous session?"))
      StartingPage = 0;
   CBackEndInterface::Instance()->MarkPrintingWizardRunning(true);
   TParamAttributes ParamAttributes = ParamMgr->AtLeastDelayTimeBetweenLayers.GetAttributes();
   bool LimitsMode = ParamMgr->AtLeastDelayTimeBetweenLayers.GetLimitsMode();
   int MaxLimit = ParamMgr->AtLeastDelayTimeBetweenLayers.MaxLimit();
   try
   {
    CScopeRestorePoint<int> scKeepDelay(ParamMgr->AtLeastDelayTimeBetweenLayers);
    ParamMgr->AtLeastDelayTimeBetweenLayers = ParamMgr->AtLeastDelayTimeBetweenLayers.DefaultValue();
    ParamMgr->AtLeastDelayTimeBetweenLayers.SetMaxLimit(ParamMgr->AtLeastDelayTimeBetweenLayers);
    ParamMgr->AtLeastDelayTimeBetweenLayers.AddAttribute(paLimitsClip);
    ParamMgr->AtLeastDelayTimeBetweenLayers.SetLimitsMode(true);
   ret = RunWizard(Wizard, StartingPage);
   }
   __finally
   {
        CBackEndInterface::Instance()->MarkPrintingWizardRunning(false);
    ParamMgr->AtLeastDelayTimeBetweenLayers.SetMaxLimit(MaxLimit);
    ParamMgr->AtLeastDelayTimeBetweenLayers.SetAttributes(ParamAttributes);
    ParamMgr->AtLeastDelayTimeBetweenLayers.SetLimitsMode(LimitsMode);
   }
  }
  // Enable materials warning messages after the wiazrd
  FrontEndInterface->EnableMaterialWarning(true);
  return ret;
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::MainMenuFrameOptionsMenuClick(TObject *Sender)
{
  MainMenuFrame->OptionsMenuClick(Sender);
}

 
/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

// PFF print job
void __fastcall TMainUIFrame::PFFPrintJobActionExecute(TObject *Sender)
{
  if(PFFJobForm->ShowModal() == mrOk)
  {
  	try
	{
//		LocalJobForm->GetJobSettings(FileNames1,BitmapResolution,QualityMode,OperationMode,SliceNum1);
		CBackEndInterface::Instance()->PFFPrintJob(PFFJobForm->edtFolderName->Text.c_str());
	}
	catch(EQException& Err)
	{
	  QMonitor.ErrorMessage(Err.GetErrorMsg());
	}
  }

}



// Load local print job
void __fastcall TMainUIFrame::LoadLocalPrintJobActionExecute(TObject *Sender)
{
  if(LocalJobForm->ShowModal() == mrOk)
  {
    TFileNamesArray FileNames1 = (TFileNamesArray)malloc(TFileNamesArraySIZE);
	TFileNamesArray FileNames2 = (TFileNamesArray)malloc(TFileNamesArraySIZE);


    try
	{
		int BitmapResolution, QualityMode,OperationMode,SliceNum1,SliceNum2;
	   if (LocalJobForm->IsPrintDoubleBMPEnabled())
       {
         LocalJobForm->GetJobSettings(FileNames1,FileNames2,BitmapResolution,QualityMode,OperationMode,SliceNum1,SliceNum2);
         CBackEndInterface::Instance()->LoadLocalPrintJob(FileNames1,FileNames2,BitmapResolution,QualityMode,OperationMode,SliceNum1,SliceNum2);
       }
       else
       {
         LocalJobForm->GetJobSettings(FileNames1,BitmapResolution,QualityMode,OperationMode,SliceNum1);
         CBackEndInterface::Instance()->LoadLocalPrintJob(FileNames1,BitmapResolution,QualityMode,OperationMode,SliceNum1);
       }
    }
    catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }
    free(FileNames1);
    free(FileNames2);
  }

}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::PausePrintActionExecute(TObject *Sender)
{
  try
  {
    CBackEndInterface::Instance()->PausePrint();
  }
  catch(EQException& Err)
  {
    QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::StopPrintActionExecute(TObject *Sender)
{
  if(!QMonitor.AskYesNo("Are you sure you want to stop?"))
     return;

  try
  {
    CBackEndInterface::Instance()->StopPrint(Q2RT_EMBEDDED_RESPONSE);
  }
  catch(EQException& Err)
  {
    QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenPCIOptionsDlgActionExecute(TObject *Sender)
{
  PCIOptionsForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenOHDBOptionsDlgActionExecute(TObject *Sender)
{
  //OHDBOptionsForm->Show();
  if (CAppParams::Instance()->SimulatorMode == true)//LUDA simulator
	LotusLowLevelForm->Show();
  else
	 OHDBOptionsForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoWipeActionExecute(TObject *Sender)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
    if(!QMonitor.AskYesNo("Perform wipe?"))
       return;

    BEInterface->SetOnlineOffline(false);

    BEInterface->SetMotorParameters(AXIS_X);
    BEInterface->SetMotorParameters(AXIS_Y);
    BEInterface->SetMotorParameters(AXIS_T);
    BEInterface->DoWipe();

  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::GotoPurgePositionActionExecute(TObject *Sender)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
    if(!QMonitor.AskYesNo("Go to Purge position?"))
       return;

    BEInterface->SetOnlineOffline(false);

    BEInterface->SetMotorParameters(AXIS_X);
    BEInterface->SetMotorParameters(AXIS_Y);
    BEInterface->SetMotorParameters(AXIS_T);

    BEInterface->GotoPurgePosition();

  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoTestPatternActionExecute(TObject *Sender)
{
  // Activate test pattern
  try
  {
    if(!QMonitor.AskYesNo("Perform Pattern Test?"))
       return;
    CBackEndInterface::Instance()->SetOnlineOffline(false);
	CBackEndInterface::Instance()->DoTestPattern();
	CBackEndInterface::Instance()->ResetMaintenanceCounter(PATTERN_TEST_COUNTER_ID);
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoFireAllActionExecute(TObject *Sender)
{
// do fire all
  try
  {
    if(!QMonitor.AskYesNo("Perform Fire All?"))
       return;
    CBackEndInterface::Instance()->SetOnlineOffline(false);

    CBackEndInterface::Instance()->SetMotorParameters(AXIS_X);
    CBackEndInterface::Instance()->SetMotorParameters(AXIS_Y);
    CBackEndInterface::Instance()->SetMotorParameters(AXIS_T);

    CBackEndInterface::Instance()->DoFireAll();
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoFireAllSequenceActionExecute(TObject *Sender)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
    if(!QMonitor.AskYesNo("Perform Fire All sequence?"))
       return;

    BEInterface->SetOnlineOffline(false);

    BEInterface->DoFireAllSequence();

  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoPurgeSequenceActionExecute(TObject *Sender)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
    if(!QMonitor.AskYesNo("Perform Purge sequence?"))
       return;

    BEInterface->SetOnlineOffline(false);

    BEInterface->SetMotorParameters(AXIS_X);
    BEInterface->SetMotorParameters(AXIS_Y);
    BEInterface->SetMotorParameters(AXIS_T);

    BEInterface->DoPurgeSequence( false /* PerformSinglePurgeInSequence */);

  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::EnterStandbyActionExecute(TObject *Sender)
{
  try
  {
    CBackEndInterface::Instance()->EnterStandbyState();
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::ExitStandbyActionExecute(TObject *Sender)
{
  try
  {
    CBackEndInterface::Instance()->ExitStandbyState();
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::SelectUpActionExecute(TObject *Sender)
{
  m_CurrentScreen->NavigateButtonUp();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::SelectDownActionExecute(TObject *Sender)
{
  m_CurrentScreen->NavigateButtonDown();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::SoftSelectBtnMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  m_CurrentScreen->SelectSoftButtonPressed();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::SoftSelectBtnMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  m_CurrentScreen->SelectSoftButtonReleased();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OptionBackActionExecute(TObject *Sender)
{
  NavigateBackward();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DateTimeUpdateTimerTimer(TObject *Sender)
{
  // Get current time
  TDateTime CurrentTime = Now();

  if(m_ShowPrintTime)
    UpdatePrintTime();

  // Update clock
  m_CurrentScreen->UpdateDateTimeDisplay(CurrentTime);
  MainForm->UpdateDateTimeDisplay(CurrentTime);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenCalibartionDlgActionExecute(TObject *Sender)
{
  LoadSuitableMode(); 
  CalibrationForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenTrayPointsWizardActionExecute(TObject *Sender)
{
	if(Q2RTApplication->GetMachineType() == (mtObjet1000 ))
	{
	   WizardForm->RegisterCustomPageViewer(wptCustomTrayPointsPage,new CCustomZStartWizardViewer);
	}
	else
	{
		WizardForm->RegisterCustomPageViewer(wptCustomTrayPointsPage,new CCustomTrayPointsWizardViewer);
	}
  RunPythonWizard("TrayPointsCalibration");

}
//---------------------------------------------------------------------------
 void __fastcall TMainUIFrame::OpenZStartCalibrationWizardActionExecute(TObject *Sender)
{
  WizardForm->RegisterCustomPageViewer(wptCustomZStartPage,new CCustomZStartWizardViewer);
  RunPythonWizard("ZStartCalibration");
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoTrayUpperPositionActionExecute(TObject *Sender)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
	if(!QMonitor.AskYesNo("Go to tray upper position?"))
       return;

    BEInterface->SetOnlineOffline(false);
	BEInterface->GotoTrayRemovalPosition();
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DoTrayLowerPositionActionExecute( TObject *Sender)
{
  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
	if(!QMonitor.AskYesNo("Go to tray lower position?"))
       return;

    BEInterface->SetOnlineOffline(false);
	BEInterface->GotoTrayLowerPosition();
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
//This function is auto. associated with the OnClick event of the menu item
void __fastcall TMainUIFrame::OpenResinReplacementWizardAction_NoiExecute(TObject *Sender)
{
	CBackEndInterface::Instance()->SetOnlineOffline(false);
	//MAX_NUMBER_OF_MODEL_CHAMBERS = 3	(Triplex version) //   for GUI
	//=> NUMBER_OF_MODEL_TANKS = 6	//for props in WizPage
	//=> TOTAL_NUMBER_OF_CONTAINERS = NUMBER_OF_MODEL_TANKS + 2 (support) = 8
	//C'tor's size argument must match the corresponding wizard-page's c'tor's size argument; typically: MAX_NUMBER_OF_MODEL_CHAMBERS.
	WizardForm->RegisterCustomPageViewer(wptCustResinSelectionWizardPage, new CCustomResinMainSelectWizardPageViewer(MAX_NUMBER_OF_MODEL_CHAMBERS));
	WizardForm->RegisterCustomPageViewer(wptCustResinSelectionAdvancedWizardPage, new CCustomResinAdvancedSettingsPageViewer());
	WizardForm->RegisterCustomPageViewer(wptCustResinSelectionSummaryWizardPage, new CCustomResinSelectSummaryPageViewer());
	WizardForm->RegisterCustomPageViewer(wptMaterialReplacementDonePage, new CCustomMaterialReplacementDonePageViewer());

	RunWizard(m_ResinReplacementWizard); //start off page number 0
	
    CBackEndInterface::Instance()->SetSleepingChannels();

  //restart application
  if (m_ResinReplacementWizard->IsRestartRequired())
  {
    AppRestartFlag = true;
	MainForm->Close();
  }    
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::ExecuteOpenMRWSilentlyAction(TObject *Sender, bool isBlocking, bool updateSegments/*Update Segments*/)
{
}
//---------------------------------------------------------------------------
// Run shutdown wizard
void __fastcall TMainUIFrame::ShutdownActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  CBackEndInterface::Instance()->IgnorePowerByPass(true);

  RunWizard(m_ShutdownWizard);

  CBackEndInterface::Instance()->IgnorePowerByPass(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenHeadFillingWizardActionExecute(TObject *Sender)
{
  RunPythonWizard("HeadFillingCalibration");
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::EnterAdvanceModeActionExecute(TObject *Sender)
{
  OpenPCIOptionsDlgAction->Visible = true;
  OpenOHDBOptionsDlgAction->Visible = true;
  OpenMCBDiagDlgAction->Visible = true;
  DoCleanHeads->Visible = true;
  DoCleanWiper->Visible = true;
//  FactoryShutdownWizardAction->Visible = true; // deprecated
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenGeneralDevicesActionExecute(TObject *Sender)
{
  OpenGeneralDevicesAction->Checked = (! GeneralDevicesForm->Visible);

  if(OpenGeneralDevicesAction->Checked)
  {
    CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
    LoadSuitableMode();

    // Set the "always on top" option before showing the dialog
    GeneralDevicesForm->FormStyle = FrontEndParams->DevicesDlgAlwaysOnTop ? fsStayOnTop : fsNormal;
    GeneralDevicesForm->Show();
  }
  else
    GeneralDevicesForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenOcbOhdbVoltageDlgActionExecute(TObject *Sender)
{
  
  
  OpenOcbOhdbVoltageDlgAction->Checked = !OpenOcbOhdbVoltageDlgAction->Checked;

  if(OpenOcbOhdbVoltageDlgAction->Checked)
  {
    CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
    LoadSuitableMode();

    // Set the "always on top" option before showing the dialog
    OcbOhdbControlForm->FormStyle = FrontEndParams->DevicesDlgAlwaysOnTop ? fsStayOnTop : fsNormal;
    OcbOhdbControlForm->Show();
  }
  else
    OcbOhdbControlForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenHeadsControlActionExecute(TObject *Sender)
{
  OpenHeadsControlAction->Checked = (! HeadsControlForm->Visible);

  if(OpenHeadsControlAction->Checked)
  {
    CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
    // Set the "always on top" option before showing the dialog
    HeadsControlForm->FormStyle = FrontEndParams->HeadsDlgAlwaysOnTop ? fsStayOnTop : fsNormal;
    LoadSuitableMode();
    HeadsControlForm->Show();
  }
  else
    HeadsControlForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenMotorsControlActionExecute(TObject *Sender)
{
  if(MCBDiagForm->Visible)
  {
     QMonitor.NotificationMessage("The MCB SW parameters window is open. Please close it and try again.");
     return;
  }
  OpenMotorsControlAction->Checked = (! MotorsControlForm->Visible);
  if(OpenMotorsControlAction->Checked)
  {
    CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

    // Set the "always on top" option before showing the dialog
    MotorsControlForm->FormStyle = FrontEndParams->MotorsDlgAlwaysOnTop ? fsStayOnTop : fsNormal;
    MotorsControlForm->Show();
  }
  else
    MotorsControlForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenActuatorsAndSensorsActionExecute(TObject *Sender)
{
  OpenActuatorsAndSensorsAction->Checked = (! ActuatorsControlForm->Visible);

  if(OpenActuatorsAndSensorsAction->Checked)
  {
    CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

    // Set the "always on top" option before showing the dialog
    ActuatorsControlForm->FormStyle = FrontEndParams->ActuatorsDlgAlwaysOnTop ? fsStayOnTop : fsNormal;
    ActuatorsControlForm->Show();
  }
  else
    ActuatorsControlForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::MaintenanceCountersActionExecute(TObject *Sender)
{
  MaintenanceCountersForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::UserWizardsActionExecute(TObject *Sender)
{
  // Do not erase!
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::MaintenanceWizardsActionExecute(TObject *Sender)
{
  // Do not erase!
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::ModesMenuActionExecute(TObject *Sender)
{
  // Do not erase!
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OnlineActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OfflineActionExecute(TObject *Sender)
{
  THaspPlugInfo PlugInfo = CBackEndInterface::Instance()->GetHaspPlugInfo();
  int DaysToExpiration   = CBackEndInterface::Instance()->GetHaspDaysToExpiration();

  if((DaysToExpiration < CAppParams::Instance()->HASP_DaysToExpirationNotification) && (DaysToExpiration != -1))
  {
     if(DaysToExpiration == 0)
     {
        CQEncryptedLog::Write(LOG_TAG_HASP, "HASP: " + QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_TODAY_MESSAGE)));
        QMonitor.NotificationMessage(QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_TODAY_MESSAGE)));
     }
     else
     {
        CQEncryptedLog::Write(LOG_TAG_HASP, "HASP: " + QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_IN_MESSAGE), DaysToExpiration));
        QMonitor.NotificationMessage(QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_IN_MESSAGE), DaysToExpiration));
     }
  }
     
  CBackEndInterface::Instance()->SetOnlineOffline(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::ConfigurationExportImportActionExecute(TObject *Sender)
{
  // Do not erase!
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::RollerTiltWizardActionExecute(TObject *Sender)
{
  WizardForm->RegisterCustomPageViewer(wptCustomRollerTiltPage,new CCustomRollerTiltWizardViewer);
  WizardForm->RegisterCustomPageViewer(wptRollerYMovementPage, new CCustomRollerYMoveWizardViewer);
  RunPythonWizard("RollerTilt");
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::CleanHeadsWizardActionExecute(TObject *Sender)
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  BackEnd->SendOHDBCleanHeadWizardStartedNotification();

  if(RunPythonWizard("CleanHeads"))
		BackEnd->ResetMaintenanceCounter(LAST_HCW_ACTIVATION_TIME_COUNTER_ID);  //itamar, Super purge

  BackEnd->SendOHDBCleanHeadWizardEndedNotification();		
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::CleanWiperWizardActionExecute(TObject *Sender)
{
  RunPythonWizard("CleanWiper");
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::ResumePrintActionExecute(TObject *Sender)
{
  try
  {
    CBackEndInterface::Instance()->ResumePrint();
  } catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }        
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenMCBDiagDlgActionExecute(TObject *Sender)
{
  if(MotorsControlForm->Visible)
  {
     QMonitor.NotificationMessage("The Motors control window is open. Please close it and try again.");
     return;
  }
  if(QMonitor.AskYesNo("This will cause MCB to enter a diagnostic mode.\n\rDo you wish to continue?"))
     MCBDiagForm->Show();
}
//-----------------------------------------------------
void __fastcall TMainUIFrame::OpenBITViewerActionExecute(TObject *Sender)
{
  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

  // Set the UI to "busy" mode whenever the BIT viewer form is open
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  EnableDisableControl(FE_ONLINE_ENABLE,false);
  SetUIMode(uimBusy);
  
  // Set the "always on top" option before showing the dialog
  BITViewerForm->FormStyle = FrontEndParams->BITDlgAlwaysOnTop ? fsStayOnTop : fsNormal;
  BITViewerForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::ScriptsMenuActionExecute(TObject *Sender)
{
  // Do not erase!
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::CollectServiceDataActionExecute(TObject *Sender)
{
  if(MessageDlg("Are you sure you want to generate service data file?",mtConfirmation,
                TMsgDlgButtons() << mbYes << mbNo,0) == mrYes)
  {
	CConfigBackup *ConfigBackup = new CConfigBackup();

	try
	{
		Screen->Cursor = crHourGlass;
		ConfigBackup->CollectServiceData();
		Screen->Cursor = crDefault;

		QString Tmp = Q2RTApplication->AppFilePath.Value() + SERVICE_DATA_DIR;
		ExploreFile(Tmp.c_str());
	}
	__finally
	{
    	Q_SAFE_DELETE(ConfigBackup);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::BumperCalibrationWizardActionExecute(TObject *Sender)
{
  RunPrintingWizard(m_BumperCalibrationWizard);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenQuickSHRActionExecute(TObject *Sender)
{
  WizardForm->RegisterCustomPageViewer(wptUserValuesEntryPage       , new CUserEntryWizardPageViewer); // todo -oNobody -cNone: it is unclear who is responsible to destroy these objects.  
  WizardForm->RegisterCustomPageViewer(wptAgreementPage             , new CAgreementPageViewer);
  WizardForm->RegisterCustomPageViewer(wptHeadsCheckboxesWizardPage , new CHeadsCheckBoxesPageViewer);
  if(m_QuickSHRWizard != NULL)
  {
   RunPrintingWizard(m_QuickSHRWizard, (CAppParams::Instance()->QR_LastState != 0) ?
                                       m_QuickSHRWizard->GetContinuePageNumber() : 0);

   // If the user selected to run the 'Head Alignment wizard' run it now
   if(m_QuickSHRWizard->RunHeadAlignment() == true)
   {   RunHeadAlignmentWizard();}
 }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::XOffsetWizardActionExecute(TObject *Sender)
{
  RunHeadAlignmentWizard();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::UVCalibrationWizardExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetUVWizardLimited(false);
  CBackEndInterface::Instance()->SetOnlineOffline(false);  

  WizardForm->RegisterCustomPageViewer(wptUVLampsStatusWizardPage, new CUVLampsStatusPageViewer);
  //WizardForm->RegisterCustomPageViewer(wptAgreementPage,          new CAgreementPageViewer);

  RunPythonWizard("UVCalibration");
}
//---------------------------------------------------------------------------
bool TMainUIFrame::IsMaintenanceModeAllowed()
{
  return m_AllowMaintenanceMode;
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::VacuumCalibrationActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  WizardForm->RegisterCustomPageViewer(wptVacuumStatusWizardPage, new CVacuumStatusPageViewer);

  RunWizard(m_VacuumCalibrationWizard);
  if (m_VacuumCalibrationWizard->IsRestartRequired())
  {
    AppRestartFlag = true;
    MainForm->Close();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::LoadCellCalibrationActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  //OBJET_MACHINE adjustment
   WizardForm->RegisterCustomPageViewer(wptCustomLoadCellCalibrationPage, new CLoadCellCalibrationFramePageViewer);
   WizardForm->RegisterCustomPageViewer(wptWeightsStatusWizardPage, new CWeightsStatusPageViewer);
   WizardForm->RegisterCustomPageViewer(wptLoadCellWellcomePage, new CLoadCellWellcomePageViewer);
  RunWizard(m_SinglePointCalibrationWizard);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::PrintingPositionActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);
  RunWizard(m_PrintingPositionWizard);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::UVReplacementWizardExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);

  //WizardForm->RegisterCustomPageViewer(wptUVLampsStatusWizardPage, new CUVLampsStatusPageViewer);
  RunWizard(m_UVReplacementWizard);
  // If the user selected to run the 'UV Calibration wizard' run it now
  if(m_UVReplacementWizard->RunUVCalibrationWizard() == true)
  {
	WizardForm->RegisterCustomPageViewer(wptUVLampsStatusWizardPage, new CUVLampsStatusPageViewer);
	WizardForm->RegisterCustomPageViewer(wptUVLampsResultsWizardPage, new CUVLampsResultsPageViewer);
	WizardForm->RegisterCustomPageViewer(wptUVLampsCalibrationWizardPage, new CUVLampsCalibrationWizardViewer);
	RunPythonWizard("UVPCalibration");
  }
}


//---------------------------------------------------------------------------
//void __fastcall TMainUIFrame::FactoryShutdownWizardExecute(TObject *Sender)
//{ // deprecated
//    RunPrintingWizard(m_FactoryShutdownWizard);
//}


//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::DisplayOptimizationWizardActionExecute(TObject *Sender)
{
  HeadsOptimizationAction->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::HeadsOptimizationActionExecute(TObject *Sender)
{
//  WizardForm->RegisterCustomPageViewer(wptHeadsButtonsEntryPage    ,  new CHeadsButtonsEntryFrameWizardPageViewer);
  WizardForm->RegisterCustomPageViewer(wptAgreementPage            , new CAgreementPageViewer);
  WizardForm->RegisterCustomPageViewer(wptHeadsCheckboxesWizardPage, new CHeadsCheckBoxesPageViewer);
 // RunPrintingWizard(m_HeadServiceWizard, (CAppParams::Instance()->HSW_LastState != 0) ?
 //										  m_HeadServiceWizard->GetContinuePageNumber() : 0);
 // for Objet was decided every time to start from the first screen
	RunPrintingWizard(m_HeadServiceWizard, 0);
  // If the user selected to run the 'Head Alignment wizard' run it now
  if(m_HeadServiceWizard->RunHeadAlignment() == true)
	RunHeadAlignmentWizard();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::UVMCalibrationWizardExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);

  // first go into High Quality mode
  CBackEndInterface::Instance()->GotoDefaultMode();
  CBackEndInterface::Instance()->EnterMode(PRINT_MODE[0][0], MACHINE_QUALITY_MODES_DIR(0,0));

  //and now set the heads velocity to the velocity of HQ
  float Velocity_StepsPerMM = CAppParams::Instance()->MotorsVelocity[AXIS_X] * CAppParams::Instance()->StepsPerMM[AXIS_X];
  CBackEndInterface::Instance()->SetMotorSpeed(AXIS_X, (long)Velocity_StepsPerMM );
//  Motors.SetVelocity(AXIS_X, UV_POLLING_VELOCITY_MM_PER_SEC, muMM)
//  Motors.SetVelocity(AXIS_X, Motors.GetAxisCurrentVelocity(AXIS_X), muSteps)

  WizardForm->RegisterCustomPageViewer(wptUVLampsStatusWizardPage, new CUVLampsStatusPageViewer);
  WizardForm->RegisterCustomPageViewer(wptAgreementPage         , new CAgreementPageViewer);

  RunWizard(m_UVCalibrationWizard);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::WiperCalibrationActionExecute(TObject *Sender)
{
  if(MotorsControlForm->Visible)
  {
	 QMonitor.NotificationMessage("The Motors control window is open. Please close it and try again.");
     return;
  }
  WizardForm->RegisterCustomPageViewer(wptCustomWiperCalibrationPage , new CCustomWiperCalibrationPageViewer);
  RunWizard(m_WiperCalibrationWizard);
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::OpenHaspInfoDlgActionExecute(TObject *Sender)
{
    HaspPlugInformationDlg->ShowModal();
}
//---------------------------------------------------------------------------
//OBJET_MACHINE feature
void __fastcall TMainUIFrame::MaterialBatchNoActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->ShowMaterialBatchNoReport();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::MachineActionsActionExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->ShowMachineActionsReport();
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::UVCalibrationOptionsWizardExecute(
      TObject *Sender)
{
  CBackEndInterface::Instance()->SetUVWizardLimited(true);
  CBackEndInterface::Instance()->SetOnlineOffline(false);

  WizardForm->RegisterCustomPageViewer(wptUVLampsStatusWizardPage, new CUVLampsStatusPageViewer);
  RunPythonWizard("UVCalibration");
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::GoToHeadInspectionActionExecute(TObject *Sender)
{

  CBackEndInterface *BEInterface = CBackEndInterface::Instance();

  try
  {
	if(!QMonitor.AskYesNo("Go to head inspection position?"))
       return;

    BEInterface->SetOnlineOffline(false);
	BEInterface->GotoHeadInspectionPosition();
  }
  catch(EQException& Err)
  {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::HeadsOptimizationScalesBasedActionExecute(
      TObject *Sender)
{
 // WizardForm->RegisterCustomPageViewer(wptHeadsButtonsEntryPage    ,  new CHeadsButtonsEntryFrameWizardPageViewer);
  WizardForm->RegisterCustomPageViewer(wptAgreementPage            , new CAgreementPageViewer);
  WizardForm->RegisterCustomPageViewer(wptHeadsCheckboxesWizardPage, new CHeadsCheckBoxesPageViewer);
  WizardForm->RegisterCustomPageViewer(wptMissingNozzlesWizardPage, new CMissingNozzlesScaleBasedPageViewer);

  RunPrintingWizard(m_HeadServiceWizard, 0);
   //Restart application if required
  if (m_HeadServiceWizard->IsRestartRequired())
  {
    AppRestartFlag = true;
    MainForm->Close();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainUIFrame::UVCalibrationGNRWizardExecute(TObject *Sender)
{
  CBackEndInterface::Instance()->SetOnlineOffline(false);

  WizardForm->RegisterCustomPageViewer(wptUVLampsResultsWizardPage, new CUVLampsResultsPageViewer);
  WizardForm->RegisterCustomPageViewer(wptUVLampsCalibrationWizardPage, new CUVLampsCalibrationWizardViewer);
  RunPythonWizard("UVPCalibration");
}


//---------------------------------------------------------------------------
//void __fastcall TMainUIFrame::FactoryShutdownWizardActionExecute(TObject *Sender)
//{ // deprecated
//    RunPrintingWizard(m_FactoryShutdownWizard);
//}

/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
