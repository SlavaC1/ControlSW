//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "HTMLHelpViewer.hpp"

#include "OcbOhdbControlDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EdenMainUIFrame"
#pragma link "FormSkin"
#pragma link "EnhancedButton"
#pragma link "LabelMenu"
#pragma link "edenmainuiframe"
#pragma link "EnhancedLabel"
#pragma link "HTMLHelpViewer"

#pragma resource "*.dfm"

#include <fstream>
#include <FileCtrl.hpp>
#include <algorithm>
#include "AllQLib.h"
#include "MiniDebugger.h"
#include "QMonitorDlg.h"
#include "QVersionInfo.h"
#include "MachineSequencer.h"
#include "MachineManager.h"
#include "RPCClient.h"
#include "FrontEndParams.h"
#include "BackEndInterface.h"
#include "ParamsDialog.h"
#include "AppParams.h"
#include "About.h"
#include "ImageViewDlg.h"
#include "ParamsExportDlg.h"
#include "FrontEndControlIDs.h"
#include "MainScreenRegion.h"
#include "MaintenanceCountersDlg.h"
#include "GeneralDevicesDlg.h"
#include "HeadsControlDlg.h"
#include "MotorsControlDlg.h"
#include "ActuatorsControlDlg.h"
#include "PythonConsole.h"
#include "QPythonIntegrator.h"
#include "QFileSystem.h"
#include "WizardViewer.h"
#include "PythonAutoWizard.h"
#include "AutoWizardDlg.h"
#include "CartridgeErrorDlg.h"
#include "PythonAppBIT.h"
#include "BITViewerDlg.h"
#include "DoorDlg.h"
#include "AppLogFile.h"
#include "HelpDlg.h"
#include "ModesDefs.h"
#include "MainStatusScreen.h"
#include "MaintenanceWarningForm.h"
#include "SplashScreen.h"
#include "KeyboardMapDlg.h"
#include "FEResources.h"
#include "TankIdentificationNotice.h"
#include "ModesManager.h"

#pragma warn -8057 // todo -oNobody -cNone: put all gui event handlers at the end of the file and move this pragma to apply only to them.

const int MAX_MODES_PATH_WIDTH_IN_PIZELS       = 330;
const int HELP_MENU_LABEL_POS                  = 230;
const int CHOOSE_MAINTENANCE_ITEMS_START_COUNT = 2;/*DefaultMode + Separator*/

TMainForm*    MainForm;
// This global flag is used to mark that a program restart is required
extern bool   AppRestartFlag;

//******************************************************************************
//* Description: ExternalAppExecute                                            *
//* Params     : None.                                                         *
//* Return     : None.                                                         *
//* Notes      : This function use the ShellExeute API function to activate the*                                                              *
//*              MS Application                                                *
//******************************************************************************
void ExternalAppExecute(const char* FileName, const char* CmdLineParam)
{
	int OsSystemError = (int)ShellExecute(Application->Handle,"open" ,FileName,
	                                      CmdLineParam,NULL,SW_NORMAL) ;

	switch(OsSystemError)
	{
		case ERROR_FILE_NOT_FOUND :
			MessageDlg("File not found",mtError,TMsgDlgButtons() << mbOK,0);
			break;

		case ERROR_PATH_NOT_FOUND :
			MessageDlg("Path not found",mtError,TMsgDlgButtons() << mbOK,0);
			break;

		case ERROR_BAD_FORMAT :
			MessageDlg("Bad format error",mtError,TMsgDlgButtons() << mbOK,0);
			break;

		case SE_ERR_SHARE :
			MessageDlg("Sharing violation occurred",mtError,TMsgDlgButtons() << mbOK,0);
			break;
	}
}

void ExploreFile(const char *FileName)
{
	int OsSystemError = (int)ShellExecute(Application->Handle,"explore" ,FileName,
	                                      "",NULL,SW_NORMAL) ;

	switch(OsSystemError)
	{
		case ERROR_FILE_NOT_FOUND :
			MessageDlg("File not found",mtError,TMsgDlgButtons() << mbOK,0);
			break;

		case ERROR_PATH_NOT_FOUND :
			MessageDlg("Path not found",mtError,TMsgDlgButtons() << mbOK,0);
			break;

		case ERROR_BAD_FORMAT :
			MessageDlg("Bad format error",mtError,TMsgDlgButtons() << mbOK,0);
			break;

		case SE_ERR_SHARE :
			MessageDlg("Sharing violation occurred",mtError,TMsgDlgButtons() << mbOK,0);
			break;
	}
}

// Launch an help file in an external viewer
void ShowHelpFile(const QString HelpFileName)
{
	QString FileName = Q2RTApplication->AppFilePath.Value() + "Help\\" + HelpFileName;

	VARIANT v1, v2, v3, v4;

	memset(&v1, 0, sizeof(VARIANT));
	memset(&v2, 0, sizeof(VARIANT));
	memset(&v3, 0, sizeof(VARIANT));
	memset(&v4, 0, sizeof(VARIANT));

	wchar_t szBuffer[254];

	StringToWideChar(FileName.c_str(), szBuffer, 254);
	ShowHelpForm->HelpWebBrowser->Navigate(szBuffer, &v1, &v2, &v3, &v4);
	ShowHelpForm->Visible = true;
}


//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
  
  //register application for power updates
  m_BatteryPowerNotify = RegisterPowerSettingNotification(this->Handle,&GUID_BATTERY_PERCENTAGE_REMAINING,0);
  m_PowerSourceNotify  = RegisterPowerSettingNotification(this->Handle,&GUID_ACDC_POWER_SOURCE ,0);
  
	m_IsMouseDown   = false;
	m_IsDragChanged = false;
	m_ParamsDlg     = NULL;
	//OBJET_MACHINE
	m_ValidPassword = false;
  m_PowerSource   = PoAc;   // Initialize to Power Source: AC

	m_HaspExpirationMessageAlreadyShown = false;

	LOAD_BITMAP(MainBitmapFormSkin->Skin, IDB_MAIN_BITMAP);

	MachineLogoImage->Left                            = INT_FROM_RESOURCE(IDN_LOGO_LEFT);
	MachineLogoImage->Top                             = INT_FROM_RESOURCE(IDN_LOGO_TOP);

	MainUIFrame->ScreensPlaceHolder->Width            = INT_FROM_RESOURCE(IDN_CENTER_FRAME_WIDTH);
	MainUIFrame->ScreensPlaceHolder->Height           = INT_FROM_RESOURCE(IDN_CENTER_FRAME_HEIGHT);
	MainUIFrame->Left                                 = INT_FROM_RESOURCE(IDN_CENTER_FRAME_LEFT);
	MainUIFrame->Top                                  = INT_FROM_RESOURCE(IDN_CENTER_FRAME_TOP);

	/*GoButton*/
	LOAD_BITMAP(GoButton->DisabledBmp,                  IDB_DISABLED_GO_BUTTON);
	LOAD_BITMAP(GoButton->DownBmp,                      IDB_DOWN_GO_BUTTON);
	LOAD_BITMAP(GoButton->HighlightedBmp,               IDB_HIGHLIGHTED_GO_BUTTON);
	LOAD_BITMAP(GoButton->Mask,                         IDB_MASK_GO_BUTTON);
	LOAD_BITMAP(GoButton->UpBmp,                        IDB_UP_GO_BUTTON);
	GoButton->TransparentColor                        = TRANSPARENT_COLOR;
	GoButton->Left                                    = INT_FROM_RESOURCE(IDN_GO_BUTTON_LEFT);
	GoButton->Top                                     = INT_FROM_RESOURCE(IDN_GO_BUTTON_TOP);
	GoButton->Width                                   = INT_FROM_RESOURCE(IDN_MAX_BUTTON_WIDTH);

	/*PauseButton*/
	LOAD_BITMAP(PauseButton->DisabledBmp,               IDB_DISABLED_PAUSE_BUTTON);
	LOAD_BITMAP(PauseButton->DownBmp,                   IDB_DOWN_PAUSE_BUTTON);
	LOAD_BITMAP(PauseButton->HighlightedBmp,            IDB_HIGHLIGHTED_PAUSE_BUTTON);
	LOAD_BITMAP(PauseButton->Mask,                      IDB_MASK_PAUSE_BUTTON);
	LOAD_BITMAP(PauseButton->UpBmp,                     IDB_UP_PAUSE_BUTTON);
	PauseButton->TransparentColor                     = TRANSPARENT_COLOR;
	PauseButton->Left                                 = INT_FROM_RESOURCE(IDN_GO_BUTTON_LEFT);
	PauseButton->Top                                  = INT_FROM_RESOURCE(IDN_GO_BUTTON_TOP);
	PauseButton->Width                                = INT_FROM_RESOURCE(IDN_MAX_BUTTON_WIDTH);

	/*OnlineButton*/
	LOAD_BITMAP(OnlineButton->DisabledBmp,              IDB_DISABLED_ONLINE_BUTTON);
	LOAD_BITMAP(OnlineButton->DownBmp,                  IDB_DOWN_ONLINE_BUTTON);
	LOAD_BITMAP(OnlineButton->HighlightedBmp,           IDB_HIGHLIGHTED_ONLINE_BUTTON);
	LOAD_BITMAP(OnlineButton->Mask,                     IDB_MASK_ONLINE_BUTTON);
	LOAD_BITMAP(OnlineButton->UpBmp,                    IDB_UP_ONLINE_BUTTON);
	OnlineButton->TransparentColor                    = TRANSPARENT_COLOR;
	OnlineButton->Left                                = INT_FROM_RESOURCE(IDN_ONLINE_BUTTON_LEFT);
	OnlineButton->Top                                 = INT_FROM_RESOURCE(IDN_ONLINE_BUTTON_TOP);
	OnlineButton->Width                               = INT_FROM_RESOURCE(IDN_MAX_BUTTON_WIDTH);

	/*OfflineButton*/
	LOAD_BITMAP(OfflineButton->DisabledBmp,             IDB_DISABLED_OFFLINE_BUTTON);
	LOAD_BITMAP(OfflineButton->DownBmp,                 IDB_DOWN_OFFLINE_BUTTON);
	LOAD_BITMAP(OfflineButton->HighlightedBmp,          IDB_HIGHLIGHTED_OFFLINE_BUTTON);
	LOAD_BITMAP(OfflineButton->Mask,                    IDB_MASK_OFFLINE_BUTTON);
	LOAD_BITMAP(OfflineButton->UpBmp,                   IDB_UP_OFFLINE_BUTTON);
	OfflineButton->TransparentColor                   = TRANSPARENT_COLOR;
	OfflineButton->Left                               = INT_FROM_RESOURCE(IDN_ONLINE_BUTTON_LEFT);
	OfflineButton->Top                                = INT_FROM_RESOURCE(IDN_ONLINE_BUTTON_TOP);
	OfflineButton->Width                              = INT_FROM_RESOURCE(IDN_MAX_BUTTON_WIDTH);

	/*StopButton*/
	LOAD_BITMAP(StopButton->DisabledBmp,                IDB_DISABLED_STOP_BUTTON);
	LOAD_BITMAP(StopButton->DownBmp,                    IDB_DOWN_STOP_BUTTON);
	LOAD_BITMAP(StopButton->HighlightedBmp,             IDB_HIGHLIGHTED_STOP_BUTTON);
	LOAD_BITMAP(StopButton->Mask,                       IDB_MASK_STOP_BUTTON);
	LOAD_BITMAP(StopButton->UpBmp,                      IDB_UP_STOP_BUTTON);
	StopButton->TransparentColor                      = TRANSPARENT_COLOR;
	StopButton->Left                                  = INT_FROM_RESOURCE(IDN_STOP_BUTTON_LEFT);
	StopButton->Top                                   = INT_FROM_RESOURCE(IDN_GO_BUTTON_TOP);
	StopButton->Width                                 = INT_FROM_RESOURCE(IDN_MAX_BUTTON_WIDTH);

	/*SelectButton*/
	LOAD_BITMAP(SelectButton->DownBmp,                  IDB_DOWN_SELECT_BUTTON);
	LOAD_BITMAP(SelectButton->HighlightedBmp,           IDB_HIGHLIGHTED_SELECT_BUTTON);
	LOAD_BITMAP(SelectButton->Mask,                     IDB_MASK_SELECT_BUTTON);
	LOAD_BITMAP(SelectButton->UpBmp,                    IDB_UP_SELECT_BUTTON);
	SelectButton->TransparentColor                    = TRANSPARENT_COLOR;
	SelectButton->Left                                = INT_FROM_RESOURCE(IDN_SELECT_BUTTON_LEFT);
	SelectButton->Top                                 = INT_FROM_RESOURCE(IDN_SELECT_BUTTON_TOP);
	SelectButton->Width                               = INT_FROM_RESOURCE(IDN_MAX_BUTTON_WIDTH);

	/*MinimizeEnhancedButton*/
	LOAD_BITMAP(MinimizeEnhancedButton->DownBmp,        IDB_DOWN_MINIMIZE_BUTTON);
	LOAD_BITMAP(MinimizeEnhancedButton->HighlightedBmp, IDB_HIGHLIGHTED_MINIMIZE_BUTTON);
	LOAD_BITMAP(MinimizeEnhancedButton->Mask,           IDB_MASK_MINIMIZE_BUTTON);
	LOAD_BITMAP(MinimizeEnhancedButton->UpBmp,          IDB_UP_MINIMIZE_BUTTON);
	MinimizeEnhancedButton->TransparentColor          = TRANSPARENT_COLOR;
	MinimizeEnhancedButton->Left                      = INT_FROM_RESOURCE(IDN_MINIMIZE_BUTTON_LEFT);
	MinimizeEnhancedButton->Top                       = INT_FROM_RESOURCE(IDN_MINIMIZE_BUTTON_TOP);

	/*CloseEnhancedButton*/
	LOAD_BITMAP(CloseEnhancedButton->DownBmp,           IDB_DOWN_CLOSE_BUTTON);
	LOAD_BITMAP(CloseEnhancedButton->HighlightedBmp,    IDB_HIGHLIGHTED_CLOSE_BUTTON);
	LOAD_BITMAP(CloseEnhancedButton->Mask,              IDB_MASK_CLOSE_BUTTON);
	LOAD_BITMAP(CloseEnhancedButton->UpBmp,             IDB_UP_CLOSE_BUTTON);
	CloseEnhancedButton->TransparentColor             = TRANSPARENT_COLOR;
	CloseEnhancedButton->Left                         = INT_FROM_RESOURCE(IDN_CLOSE_BUTTON_LEFT);
	CloseEnhancedButton->Top                          = INT_FROM_RESOURCE(IDN_MINIMIZE_BUTTON_TOP);

	ButtonLabel->Left                                 = INT_FROM_RESOURCE(IDN_BOTTOM_LABEL_LEFT);
	SpecialModeLabel->Left                            = INT_FROM_RESOURCE(IDN_BOTTOM_LABEL_LEFT);
	JobNameLabel->Left                                = INT_FROM_RESOURCE(IDN_BOTTOM_LABEL_LEFT);

	ButtonLabel->Top                                  = INT_FROM_RESOURCE(IDN_BOTTOM_LABEL_TOP);
	SpecialModeLabel->Top  = ButtonLabel->Top + ButtonLabel->Height + INT_FROM_RESOURCE(IDN_GAP);
	JobNameLabel->Top      = SpecialModeLabel->Top + SpecialModeLabel->Height + INT_FROM_RESOURCE(IDN_GAP);
  OptionsMenuLabel->Enabled              = false;

	LOAD_BITMAP(MaintenanceModeEnhancedLabel->LeftPart,     IDB_MAINTENANCE_LABEL_LEFT);
	LOAD_BITMAP(MaintenanceModeEnhancedLabel->MiddlePart,   IDB_MAINTENANCE_LABEL_MIDDLE);
	LOAD_BITMAP(MaintenanceModeEnhancedLabel->RightPart,    IDB_MAINTENANCE_LABEL_RIGHT);
	MaintenanceModeEnhancedLabel->MiddlePartsNum          = INT_FROM_RESOURCE(IDN_MAINTENANCE_MIDDLE_PARTS_NUM);
	MaintenanceModeEnhancedLabel->Left = MainUIFrame->Left + MainUIFrame->MainStatusFrame->SubStatusLabel->Left;
	MaintenanceModeEnhancedLabel->Top  = MainUIFrame->Top - MaintenanceModeEnhancedLabel->Height;
  MaintenanceMenuLabel->Enabled          = false;

	FileMenuLabel->Color                   = LOAD_COLOR(IDC_MENU_COLOR);
	FileMenuLabel->DownColor               = LOAD_COLOR(IDC_MENU_DOWN_COLOR);
	FileMenuLabel->HighlightedColor        = LOAD_COLOR(IDC_MENU_HIGHLIGHTED_COLOR);

	OptionsMenuLabel->Color                = LOAD_COLOR(IDC_MENU_COLOR);
	OptionsMenuLabel->DownColor            = LOAD_COLOR(IDC_MENU_DOWN_COLOR);
	OptionsMenuLabel->HighlightedColor     = LOAD_COLOR(IDC_MENU_HIGHLIGHTED_COLOR);

	MaintenanceMenuLabel->Color            = LOAD_COLOR(IDC_MENU_COLOR);
	MaintenanceMenuLabel->DownColor        = LOAD_COLOR(IDC_MENU_DOWN_COLOR);
	MaintenanceMenuLabel->HighlightedColor = LOAD_COLOR(IDC_MENU_HIGHLIGHTED_COLOR);

	HelpMenuLabel->Color                   = LOAD_COLOR(IDC_MENU_COLOR);
	HelpMenuLabel->DownColor               = LOAD_COLOR(IDC_MENU_DOWN_COLOR);
	HelpMenuLabel->HighlightedColor        = LOAD_COLOR(IDC_MENU_HIGHLIGHTED_COLOR);

	PackageNameLabel->Font->Color = Graphics::TColor(RGB(63,89,152));
	PackageNameLabel->Font->Style = TFontStyles() << fsBold << fsItalic;
	PackageNameLabel->Font->Name  = "Arial";
	PackageNameLabel->Left        = 575;
	PackageNameLabel->Top         = 540;
	PackageNameLabel->Transparent = true;

	PackageNameShadowLabel->Font->Color = clWhite;
	PackageNameShadowLabel->Font->Style = TFontStyles() << fsBold << fsItalic;
	PackageNameShadowLabel->Font->Name  = "Arial";
	PackageNameShadowLabel->Left        = PackageNameLabel->Left + 1;
	PackageNameShadowLabel->Top         = PackageNameLabel->Top + 1;
	PackageNameShadowLabel->Transparent = true;
}

void __fastcall TMainForm::OpenMiniDebuggerExecute(TObject *Sender)
{
	MiniDebuggerForm->Show();
}

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	Caption = APPLICATION_NAME;
	// Set the window skin region using pre-compiled data
	MainBitmapFormSkin->SetPreCompiledRegion(MainScreenRegion,MAIN_SCREEN_REGION_BUFFER_SIZE);
	MainBitmapFormSkin->Active = true;
	m_CurrentMaintenanceMode   = false;
	// Create the Q2RT application object
	Q2RTApplication = new CQ2RTApplication;
	m_AppInitOk     = Q2RTApplication->Init();
	CFrontEndParams::Init(Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_FRONT_END_CFG_FILE_NAME));

	if(! m_AppInitOk)
		return;

	// Initialize the back end interafce object
	CBackEndInterface::Init();
	UpdateParameterManager(SERVICE_LEVEL);
	LOAD_BITMAP(MachineLogoImage->Picture->Bitmap, IDB_MACHINE_LOGO);

	// Initialize the UI components
	MainUIFrame->InitUIComponents();
	MainUIFrame->SetModesDisplayRefreshEvent(ModesDisplayRefreshEvent);

	SetAppCaptionAndHint();

	MainUIFrame->OnEnableDisableEvent = ControlEnableDisableEventHandler;

	MainUIFrame->SetHaspStatusLabelUpdateCallback(HaspStatusLabelUpdate, reinterpret_cast<TGenericCockie>(this));

	// Change the time for hints display
	Application->HintHidePause = 7000;

	// Create image preview windows
	for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
	{
		m_ImageViewForm[i] = new TImageViewForm(Application);
		m_ImageViewForm[i]->SetCaption(ChamberToStr((TChamberIndex)i, true).c_str());
	}

	// Bind the slice preview callback with the back-end
	CBackEndInterface::Instance()->SetSlicePreviewUpdateEvent(SlicePreviewUpdateCallback,
	        reinterpret_cast<TGenericCockie>(this));

	RefreshModesDisplay();
	RefreshScriptsMenu();
	TPythonConsoleForm::SetDefaultString("from Q2RT import *\n\r");
	TTankIDNotice::CreateTankIDNoticeForms();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
	MainUIFrame->DeInitUIComponents();

	delete m_ParamsDlg;
	TTankIDNotice::DestroyTankIDNoticeForms();
	// DeInitialize the back end interafce object
	CBackEndInterface::DeInit();
  
  // Unregister from power notification msgs
  if(NULL != m_BatteryPowerNotify)
	UnregisterPowerSettingNotification(m_BatteryPowerNotify);

  if(NULL != m_PowerSourceNotify)
	UnregisterPowerSettingNotification(m_PowerSourceNotify);

	delete Q2RTApplication;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ShowMonitorActionExecute(TObject *Sender)
{
	TQMonitorForm::Instance()->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
	m_LastDay    = -1;
	m_LastMinute = -1;


	UpdateDateTimeDisplay(Now());

	if(SplashScreenForm)
	{
		delete SplashScreenForm;
		SplashScreenForm = NULL;
	}

	if(m_AppInitOk)
	{
		Q2RTApplication->Start();
		MainUIFrame->DateTimeUpdateTimer->Enabled = true;

		// Set maintenance mode according to command line and the present of a debugger
		SetMaintenanceMode(IsDebuggerPresent() || (ParamStr(1) == "-M"));

		CAppParams* ParamMgr = CAppParams::Instance();
		if(ParamMgr)
		{
			// Show / hide the resin replacement wizard
			MainUIFrame->OpenQuickSHRAction->Visible = !(ParamMgr->QR_HideWizard);
			MainUIFrame->OpenResinReplacementWizardAction_Noi->Visible = !(ParamMgr->RR_HideWizard);
			ParamMgr->ShowDefaultMaintenanceMode.AddGenericObserver(ShowDefaultMaintenanceModeObserver,reinterpret_cast<TGenericCockie>(this));
			// Show / hide 'Maintenance Default Mode'
			DefaultModeMenuItem->Visible          = (ParamMgr->ShowDefaultMaintenanceMode);
			DefaultModeSeperatorMenuItem->Visible = (ParamMgr->ShowDefaultMaintenanceMode);
		}
		QPythonIntegratorDM->Init();

		Q2RTApplication->RegisterToPythonExceptionThrowers(GetCurrentThreadId());

		//Can only enter 'Materials Default' mode after python engine is initialized
		CModesManager::Instance()->EnterMode(DEFAULT_MATERIALS_MODE, DEFAULT_MODES_DIR);

		// Bind the python wizard "run" event
		TWizardViewerFrame::SetRunWizardEvent(OnRunWizardCallback);
		// Add python scripts to the BIT manager
		PythonAppBIT::Register(Q2RTApplication->GetBITManager());
		DoStartUpBIT();
	}
	else
		//Q2RTApplication->Terminate();
		Close();

	TQualityModeIndex   LastQmIndexAdded;
	TOperationModeIndex LastOmIndexAdded;
	FOR_ALL_QUALITY_MODES(qm)
	{
		FOR_ALL_OPERATION_MODES(om)
		{
			if(GetModeAccessibility(qm, om) == false)
			{
				m_ModesMenuItem[qm][om] = NULL;
				continue;
			}

			m_ModesMenuItem[qm][om] = new TMenuItem(this);
			m_ModesMenuItem[qm][om]->Action = ChooseMaintenanceModeAction;
			m_ModesMenuItem[qm][om]->Caption = GetModeStr(qm,om).c_str();
			MODE_TO_TAG(m_ModesMenuItem[qm][om]->Tag, qm, om);
			ChooseMaintenanceModePopupMenu->Items->Add(m_ModesMenuItem[qm][om]);
			LastQmIndexAdded = (TQualityModeIndex)qm;
			LastOmIndexAdded = (TOperationModeIndex)om;
		}
	}

	if((ChooseMaintenanceModePopupMenu->Items->Count - CHOOSE_MAINTENANCE_ITEMS_START_COUNT) == 1)
	{
		MaintenanceMenuLabel->Menu = MaintenancePopupMenu;
		SwitchMaintenanceModeAction->Visible = false;
		// Updating other forms...
		MainUIFrame->SetMaintenanceMode(LastQmIndexAdded,LastOmIndexAdded);
		ParamsExportForm->SetMaintenanceMode(LastQmIndexAdded,LastOmIndexAdded);
	}
	DefaultModeMenuItem->Tag = DEFAULT_QUALITY_MODE_INDEX;
	OpenParamsDialogAction->Enabled = false;

	BumperCalibrationItem->Caption = LOAD_STRING(IDS_BC_WIZARD);
	//OpenHeadFillingWizardActionItem->Caption =
	//PrintingPositionItem->Caption = LOAD_STRING(IDS_PP_WIZARD);
	//RollerTiltItem->Caption =
	//TrayPointsCalibrationItem->Caption
	VacuumSensorCalibrationItem->Caption = LOAD_STRING(IDS_VC_WIZARD);
	//UVCalibrationWizardItem->Caption
	//+"UVM" UVMCalibrationWizardItem->Caption = LOAD_STRING(IDS_UVC_WIZARD);
	WiperCalibrationWizardItem->Caption = LOAD_STRING(IDS_WC_WIZARD);
	ResinReplacementItem_Noi->Caption = LOAD_STRING(IDS_RR_WIZARD);
	//CleanWiperWizardActionItem->Caption
	//CleanHeadsWizardActionItem->Caption
	UVLampReplacementItem->Caption = LOAD_STRING(IDS_UVR_WIZARD);
	XOffsetWizardItem->Caption = LOAD_STRING(IDS_XO_WIZARD);
	HeadReplacementMenuItem->Caption = LOAD_STRING(IDS_QR_WIZARD);
	// HeadsOptimizationItem->Caption = LOAD_STRING(IDS_HEAD_SERVICE_WIZARD);
	LoadCellCalibrationActionItem->Caption = LOAD_STRING(IDS_SP_WIZARD);
	//UVCalibrationWizard2Item->Caption = LOAD_STRING(IDS_BC_WIZARD);
	ShutdownItem->Caption = LOAD_STRING(IDS_SHD_WIZARD);

	GetUIFrame()->GetMaintenanceStatusFrame()->SetLabelsPosition();

	// Relevant for Objet1000 machines
	CAppParams *ParamMgr = CAppParams::Instance();
	if(ParamMgr)
	{
		if(ParamMgr->DualWaste)
		{
			GetUIFrame()->GetMaintenanceStatusFrame()->HideWasteBar();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(GeneralDevicesForm->Visible)
		GeneralDevicesForm->Close();

	if(HeadsControlForm->Visible)
		HeadsControlForm->Close();

	if(MotorsControlForm->Visible)
		MotorsControlForm->Close();

	if(ActuatorsControlForm->Visible)
		ActuatorsControlForm->Close();

	if(BITViewerForm->Visible)
		BITViewerForm->Close();

	if(OcbOhdbControlForm->Visible)
		OcbOhdbControlForm->Close();

	Q2RTApplication->Stop();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExitMenuItemClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ParametersApplyEventHandler(TObject *Sender)
{
	// Apply changes
	Q2RTApplication->ApplyParametersChange();

	TQErrCode saveAllParameters(CAppParams *); // This opens a backdoor to SaveAll function. See comments in CAppParams declaration
	saveAllParameters(CAppParams::Instance());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Gotopurgeposition1Click(TObject *Sender)
{
	MainUIFrame->GotoPurgePositionActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Wipe1Click(TObject *Sender)
{
	MainUIFrame->DoWipeActionExecute(Sender);

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Testpattern1Click(TObject *Sender)
{
	MainUIFrame->DoTestPatternActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DoFireAll1Click(TObject *Sender)
{
	MainUIFrame->DoFireAllActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AboutMenuItemClick(TObject *Sender)
{
	AboutBox->SetMaintenanceMode(m_CurrentMaintenanceMode);

	// Show about box
	AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ViewLogFileActionExecute(TObject *Sender)
{
	// Open log file using associated application
	QString s = Q2RTApplication->AppFilePath.Value() + "Log\\" + Q2RTApplication->GetLastLogFileName();
	ExternalAppExecute(s.c_str(),"");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	// Check we are in the middle of automatic restart sequence
	if(AppRestartFlag)
	{
		CanClose = true;
		return;
	}

	// Enable close if we are not in power-up state
	if(Q2RTApplication->GetMachineManager())
	{

		CanClose = Q2RTApplication->GetMachineManager()->GetCurrentState() != msPowerUp;

		if(CanClose)
		{
			CanClose = MessageDlg("Are you sure you want to exit?",mtConfirmation,
			                      TMsgDlgButtons() << mbYes << mbNo,0) == mrYes;
		}
	}
	else
	{
		CanClose = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SelectButtonMouseDown(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y)
{
	MainUIFrame->GetCurrentScreen()->SelectSoftButtonPressed();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SelectButtonMouseUp(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y)
{
	MainUIFrame->GetCurrentScreen()->SelectSoftButtonReleased();
}
//---------------------------------------------------------------------------


// Open/Close the slice preview windows
void __fastcall TMainForm::ShowIncomingSlicesActionExecute(TObject *Sender)
{
	ShowIncomingSlicesAction->Checked = !ShowIncomingSlicesAction->Checked;
	CFrontEndParams* FrontEndParams   = CFrontEndParams::Instance();

	int LastIndex = GetActiveChambersNum();
	if(ShowIncomingSlicesAction->Checked)
	{
		for(int i = FIRST_CHAMBER_TYPE; i < LastIndex; i++)
		{
			m_ImageViewForm[i]->Left = FrontEndParams->ImageViewFormLeftArray[i];
			m_ImageViewForm[i]->Top  = FrontEndParams->ImageViewFormTopArray[i];
			m_ImageViewForm[i]->Show();
		}
	}
	else
	{
		for(int i = FIRST_CHAMBER_TYPE; i < LastIndex; i++)
		{
			FrontEndParams->ImageViewFormLeftArray[i] = m_ImageViewForm[i]->Left;
			FrontEndParams->ImageViewFormTopArray[i]  = m_ImageViewForm[i]->Top;
			FrontEndParams->SaveSingleParameter(&FrontEndParams->ImageViewFormLeftArray);
			FrontEndParams->SaveSingleParameter(&FrontEndParams->ImageViewFormTopArray);
			m_ImageViewForm[i]->Close();
		}
	}
}
//---------------------------------------------------------------------------
void TMainForm::HandleMutexHandleMessage(TMessage &Message)
{
	if((UINT)Message.LParam == WM_OBJET_EMBEDED_UNIQUE_MSG)
	{
		Application->Restore();
		Application->BringToFront();
	}
}

// Message handler for the WM_SLICE_IMAGE_UPDATE message - handle slice preview in the main thread
void TMainForm::HandleSliceImageUpdateMessage(TMessage &Message)
{
	CAppParams* ParamMgr = CAppParams::Instance();
	int LastIndex = GetActiveChambersNum();
	for(int i = FIRST_CHAMBER_TYPE; i < LastIndex; i++)
	{
		m_ImageViewForm[i]->DisplayBMP(ParamMgr->TypesArrayPerChamber[i].c_str(),
		                               i,
		                               CBackEndInterface::RequestBMPAndLock,
		                               CBackEndInterface::ReleaseBMPBuffersLock,
		                               0);
	}
}

// This callback is called by the back-end in order to initiate slice preview event
void TMainForm::SlicePreviewUpdateCallback(TGenericCockie Cockie)
{
	TMainForm *FrmPtr = reinterpret_cast<TMainForm *>(Cockie);

	if(FrmPtr->ShowIncomingSlicesAction->Checked)
		SendMessage(FrmPtr->Handle,WM_SLICE_IMAGE_UPDATE,0,0);
}

// Refresh the modes display
void TMainForm::RefreshModesDisplay(void)
{
	// Clear current modes menu items
	ModesMenuItem->Clear();
	m_CurrentSelectedModeMenuItem         = NULL;
	CBackEndInterface*   BackEndInterface = CBackEndInterface::Instance();
	QString              ModeName,ModesDirName;
	TMenuItem*           TempItem;
	TQualityModeIndex    MaintenanceMode;
	TOperationModeIndex  OperationMode;
	MainUIFrame->GetMaintenanceMode(MaintenanceMode, OperationMode);

	BackEndInterface->BeginModesEnumeration();
	if(!VALIDATE_QUALITY_MODE(MaintenanceMode) ||
	        !VALIDATE_OPERATION_MODE(OperationMode))
		ModesDirName = GENERAL_MODES_DIR;
	else
		ModesDirName = MACHINE_QUALITY_MODES_DIR(MaintenanceMode,OperationMode);

	// Adding Modes from the 'GeneralModes' Directory
	while((ModeName = BackEndInterface->GetNextModeName(ModesDirName)) != "")
	{
		// Check if the mode file is not hidden
		if(IS_MODE_HIDDEN(ModeName) == false)
		{
			// Add to the "modes" menu
			TempItem = new TMenuItem(this);
			TempItem->RadioItem = true;
			TempItem->GroupIndex = 1;
			TempItem->OnClick = ModeEnterOnClick;
			TempItem->Caption = ModeName.c_str();
			ModesMenuItem->Add(TempItem);
		}
	}
	// Add seperator
	TempItem = new TMenuItem(this);
	TempItem->Caption = "-";
	ModesMenuItem->Add(TempItem);

	// Add the "go one mode back" option
	TempItem = new TMenuItem(this);
	TempItem->Action = GoOneModeBackAction;
	ModesMenuItem->Add(TempItem);

	// Add the "goto back to default mode" option
	TempItem = new TMenuItem(this);
	TempItem->Action = GoBackToDefaultModeAction;
	ModesMenuItem->Add(TempItem);

	UpdateModesLabel();
}

void __fastcall TMainForm::ModeEnterOnClick(TObject *Sender)
{
	TMenuItem *MenuItem = dynamic_cast<TMenuItem *>(Sender);

	// Prevent from activating the same mode twice
	if(MenuItem != m_CurrentSelectedModeMenuItem)
	{
		m_CurrentSelectedModeMenuItem = MenuItem;
		MenuItem->Checked = true;

		CBackEndInterface*   BackEndInterface = CBackEndInterface::Instance();
		TQualityModeIndex    MaintenanceMode;
		TOperationModeIndex  OperationMode;
		MainUIFrame->GetMaintenanceMode(MaintenanceMode, OperationMode);
		QString              ModesDirName;

		if(!VALIDATE_QUALITY_MODE(MaintenanceMode) ||
		        !VALIDATE_OPERATION_MODE(OperationMode))
			ModesDirName = GENERAL_MODES_DIR;
		else
			ModesDirName = MACHINE_QUALITY_MODES_DIR(MaintenanceMode,OperationMode);

		try
		{
			BackEndInterface->EnterMode(MenuItem->Caption.c_str(),ModesDirName);
		}
		catch(EQException& err)
		{
			// In a case of an error, give a message and go back to default mode
			QMonitor.ErrorMessage(err.GetErrorMsg());
			GoBackToDefaultModeAction->Execute();
		}
	}
}

void __fastcall TMainForm::GoOneModeBackActionExecute(TObject *Sender)
{
	CBackEndInterface *BackEndInterface = CBackEndInterface::Instance();
	try
	{
		CBackEndInterface::Instance()->GoOneModeBack();
		QString ModeStr = BackEndInterface->GetModesStackPath();
		// If the stack is empty, remove the radio button highlighting
		if(ModeStr == "")
		{
			if(m_CurrentSelectedModeMenuItem)
			{
				m_CurrentSelectedModeMenuItem->Checked = false;
				m_CurrentSelectedModeMenuItem = NULL;
			}
		}
		else
		{
			// Highlight current mode
			QString TmpStr = BackEndInterface->GetCurrentModeName();
			TMenuItem *PrevMenu = ModesMenuItem->Find(TmpStr.c_str());
			if(PrevMenu != NULL)
			{
				m_CurrentSelectedModeMenuItem = PrevMenu;
				PrevMenu->Checked = true;
			}
		}
		UpdateModesLabel();
	}
	catch(EQException& err)
	{
		// In a case of an error, give a message and go back to default mode
		QMonitor.ErrorMessage(err.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GoBackToDefaultModeActionExecute(TObject *Sender)
{
	if(m_CurrentSelectedModeMenuItem)
	{
		m_CurrentSelectedModeMenuItem->Checked = false;
		m_CurrentSelectedModeMenuItem = NULL;
		SpecialModeLabel->Visible = false;

		try
		{
			CBackEndInterface::Instance()->GotoDefaultMode();
		}
		catch(EQException& err)
		{
			// In a case of an error, give a message and go back to default mode
			QMonitor.ErrorMessage(err.GetErrorMsg());
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CloseEnhancedButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MinimizeEnhancedButtonClick(TObject *Sender)
{
	Application->Minimize();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExportConfigurationActionExecute(TObject *Sender)
{
	ParamsExportForm->ShowModal();
	// The modes might have changed by the params export dialog
	RefreshModesDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ImportConfigurationActionExecute(TObject *Sender)
{
	if(ParametersImportDlg->Execute())
		CBackEndInterface::Instance()->ImportConfiguration(ParametersImportDlg->FileName.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ControlEnableDisableEventHandler(TObject *Sender,int ControlID,bool Value)
{
	switch(ControlID)
	{
		case FE_ONLINE_ENABLE:
			OfflineButton->Enabled              = Value & MainUIFrame->GetHaspStatus();
			OnlineButton->Enabled               = Value & MainUIFrame->GetHaspStatus();
			MainUIFrame->OnlineAction->Enabled  = Value & MainUIFrame->GetHaspStatus();
			MainUIFrame->OfflineAction->Enabled = Value & MainUIFrame->GetHaspStatus();
			break;

		case FE_PAUSE:
			PauseButton->Enabled = Value;
			MainUIFrame->PausePrintAction->Enabled = Value;
			break;

		case FE_STOP:
			StopButton->Enabled = Value;
			MainUIFrame->StopPrintAction->Enabled = Value;
			break;

		case FE_ONLINE_MODE:
			OnlineButton->Visible = Value;
			OfflineButton->Visible = !Value;
			break;

		case FE_RESUME_VISIBLE:
			GoButton->Visible = Value;
			PauseButton->Visible = !Value;
			break;

		case FEOP_RESUME_BY_CARTRIDGE_ERR_DLG:
			CartridgeErrorForm->SetHandleStopAndResume(Value);
			break;

		case FE_HASP_UI_CONTROLS_STATUS:
		{
			if(Value&&!MainUIFrame->GetHaspStatus())
			{
				OfflineButton->Enabled = Value;
				OnlineButton->Enabled  = Value;
			}
			else
			{
				OfflineButton->Enabled = Value&&OfflineButton->Enabled;
				OnlineButton->Enabled  = Value&&OnlineButton->Enabled;
			}

			MainUIFrame->SetHaspStatus(Value);

			MainUIFrame->LoadLocalPrintJobAction->Enabled       = Value;
			MainUIFrame->XOffsetWizardAction->Enabled           = Value;
			MainUIFrame->HeadsOptimizationAction->Enabled       = Value;
			MainUIFrame->BumperCalibrationWizardAction->Enabled = Value;
			MainUIFrame->ModesMenuAction->Enabled               = Value;
			MainUIFrame->OnlineAction->Enabled                  = Value;
			MainUIFrame->OfflineAction->Enabled                 = Value;
			MainUIFrame->PausePrintAction->Enabled              = Value;
			MainUIFrame->StopPrintAction->Enabled               = Value;
			MainUIFrame->ResumePrintAction->Enabled             = Value;
			MainUIFrame->HeadsOptimizationScalesBasedAction->Enabled = Value;

			HaspStatusLabel->Visible = !Value;

			if(CBackEndInterface::Instance()->GetHaspConnectionStatus() == hsDisconnected)
				OpenHaspToolAction->Enabled = false;
			else
				OpenHaspToolAction->Enabled = true;

			break;
		}

		case FE_HASP_INCOMING_SLICES_PROTECTION:
		{
			ShowIncomingSlicesAction->Visible = Value;
			break;
		}

		case FE_HASP_STATUS_LABEL_ENABLED:
		{
			HaspStatusLabel->Visible = Value;
			break;
		}

		case FE_CTRL_DOOR_DLG:
			if(Value)
				DoorCloseDlg->Show();
			else
				DoorCloseDlg->Close();
			break;
	}
}

//Elad
void __fastcall TMainForm::OCBOHDBVoltages1Click(TObject *Sender)
{
	MainUIFrame->OpenOcbOhdbVoltageDlgActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExtractFromLogFileActionExecute(TObject *Sender)
{
	QString LogFileExtractor = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_LOG_FILE_EXTRACTOR);
	QString s = Q2RTApplication->AppFilePath.Value() + "Log\\" + Q2RTApplication->GetLastLogFileName();
	ExternalAppExecute(LogFileExtractor.c_str(),s.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MaintenancePopupMenuPopup(TObject *Sender)
{
	// Update the check marks of all the "windows" menu items
	MainUIFrame->OpenGeneralDevicesAction->Checked      = GeneralDevicesForm->Visible;
	MainUIFrame->OpenHeadsControlAction->Checked        = HeadsControlForm->Visible;
	MainUIFrame->OpenMotorsControlAction->Checked       = MotorsControlForm->Visible;
	MainUIFrame->OpenActuatorsAndSensorsAction->Checked = ActuatorsControlForm->Visible;
	MainUIFrame->OpenBITViewerAction->Checked           = BITViewerForm->Visible;
	MainUIFrame->OpenOcbOhdbVoltageDlgAction->Checked   = OcbOhdbControlForm->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Calibration1Click(TObject *Sender)
{
	MainUIFrame->OpenCalibartionDlgActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OHDBOptions2Click(TObject *Sender)
{
	MainUIFrame->OpenOHDBOptionsDlgActionExecute(Sender);

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ShowHelpContentActionExecute(TObject *Sender)
{
	QString FileName = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_CONTENT_AND_INDEX_FILENAME);
	ExternalAppExecute(FileName.c_str(), NULL);
}
//---------------------------------------------------------------------------

void TMainForm::UpdateModesLabel(void)
{
	QString ModeStr = CBackEndInterface::Instance()->GetModesStackPath();
	if(ModeStr != "")
	{
		// Show the modes "path" string limits to drawing area of the screen
		AnsiString MinimizedModeStr = MinimizeName(ModeStr.c_str(),Canvas,MAX_MODES_PATH_WIDTH_IN_PIZELS);
		SpecialModeLabel->Caption = "Current Mode: " + MinimizedModeStr;
		SpecialModeLabel->Visible = true;
	}
	else
		SpecialModeLabel->Visible = false;
}

void __fastcall TMainForm::ModesDisplayRefreshEvent(TObject *Sender)
{
	QString CurrentModeName = CBackEndInterface::Instance()->GetCurrentModeName();

	if(CurrentModeName != "")
	{
		// Update the current menu selection
		for(int i = 0; i < ModesMenuItem->Count; i++)
		{
			TMenuItem *TmpItem = ModesMenuItem->Find(CurrentModeName.c_str());

			if(TmpItem != NULL)
			{
				m_CurrentSelectedModeMenuItem = TmpItem;
				TmpItem->Checked = true;
			}
		}
	}
	else if(m_CurrentSelectedModeMenuItem != NULL)
	{
		m_CurrentSelectedModeMenuItem->Checked = false;
		m_CurrentSelectedModeMenuItem = NULL;
	}

	UpdateModesLabel();
	m_ParamsDlg->RefreshDisplay();
	HeadsControlForm->Repaint();

}

void TMainForm::SetAppCaptionAndHint(void)
{
	QString AppName = APPLICATION_NAME;
	CAppParams *ParamsMgr = CAppParams::Instance();
	AnsiString EmulationStr;
	bool EmulationFlag = false;

	// Check all the machine offline parameters
	if(CHECK_EMULATION(ParamsMgr->MCB_Emulation))
	{
		EmulationStr = "MCB is emulated";
		EmulationFlag = true;
	}

	if(CHECK_EMULATION(ParamsMgr->OCB_Emulation))
	{
		if(EmulationStr != "")
			EmulationStr += "\n\r";

		EmulationStr += "OCB is emulated";
		EmulationFlag = true;
	}

	if(CHECK_EMULATION(ParamsMgr->OHDB_Emulation))
	{
		if(EmulationStr != "")
			EmulationStr += "\n\r";

		EmulationStr += "OHDB is emulated";
		EmulationFlag = true;
	}

	if(CHECK_EMULATION(ParamsMgr->DataCard_Emulation))
	{
		if(EmulationStr != "")
			EmulationStr += "\n\r";

		EmulationStr += "DataCard is emulated";
		EmulationFlag = true;
	}
	if(CHECK_EMULATION(ParamsMgr->HSW_SCALE_EMULATION))
	{
		if(EmulationStr != "")
			EmulationStr += "\n\r";

		EmulationStr += "Scale is emulated";
		EmulationFlag = true;
	}

	if(EmulationFlag)
	{
		EmulationLabelBlinkTimer->Enabled = true;
		EmulationLabelHintHolder->Visible = true;
		EmulationLabel->Hint = EmulationStr;
		EmulationLabelHintHolder->Hint = EmulationStr;
	}
}

void __fastcall TMainForm::EmulationLabelBlinkTimerTimer(TObject *Sender)
{
	if(EmulationLabel->Tag == 0)
	{
		EmulationLabel->Visible = true;
		EmulationLabel->Tag = 1;
	}
	else
	{
		EmulationLabel->Visible = false;
		EmulationLabel->Tag = 0;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CleanHeadsWizardActionClick(TObject *Sender)
{
	MainUIFrame->CleanHeadsWizardActionExecute(Sender);
}
//---------------------------------------------------------------------------

//Enter/exit to/from maintenance mode, return the previous mode
bool TMainForm::SetMaintenanceMode(bool NewMode)
{
	bool CurrentMode = m_CurrentMaintenanceMode;

	m_CurrentMaintenanceMode = NewMode;

	OpenMiniDebugger->Enabled = NewMode;
	OpenParamsDialogAction->Enabled = NewMode;
	OpenPythonConsoleAction->Enabled = NewMode;
	MainUIFrame->LoadLocalPrintJobAction->Visible = NewMode;
	LoadLocalPrintJobMenuSeperator->Visible = NewMode;
	MaintenanceCountersForm->SetMaintenanceMode(NewMode);
//OBJET_MACHINE //batch number req.
	MainUIFrame->MaterialBatchNoAction->Visible = NewMode;

	//If we need to go into maintenance mode
	if(NewMode)
	{
		MaintenanceMenuLabel->Visible = true;
		HelpMenuLabel->Left = HELP_MENU_LABEL_POS;

//PopupMenu = MaintenancePopupMenu;

	}
	else
	{
		MaintenanceMenuLabel->Visible = false;
		HelpMenuLabel->Left = MaintenanceMenuLabel->Left;

		PopupMenu = NULL;
	}

	return CurrentMode;
}

void __fastcall TMainForm::EnterMaintenanceModeActionExecute(TObject *Sender)
{
	TPoint p = Mouse->CursorPos;
	int tmpX = abs((int)(p.x - Screen->Width - 1));
	int tmpY = abs((int)(p.y - Screen->Height - 1));
	//This "hidden" option is used to accelerate the entrance to maintenance mode...
	//Place your mouse on the lower right corner of the screen and press [Ctrl+Alt+M].
	if((tmpX < 20) && (tmpY < 20))
		SetMaintenanceMode(true);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MCBSWDiagnosticClick(TObject *Sender)
{
	//Open MCB SW diagnostic dialog
	MainUIFrame->OpenMCBDiagDlgActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenPythonConsoleActionExecute(TObject *Sender)
{
	TPythonConsoleForm::CreateNewConsoleWindow();
}
//---------------------------------------------------------------------------

void TMainForm::RefreshScriptsMenu(void)
{
	ScriptsMenuItem->Clear();

	//Get a list of all available script files
	TQFileSearchRec SearchRec;

	QString PathStr = Q2RTApplication->AppFilePath.Value() + "\\Scripts\\*.py";

	std::vector<QString> ScriptList;

	//Enumerate modes directory
	if(QFindFirst(PathStr,faAnyFile,SearchRec))
		do
		{
			QString ScriptFileName = QExtractFileNameWithoutExt(SearchRec.Name.c_str());

			//Ignore scripts that starts with the '~' character
			if(ScriptFileName[0] != '~')
				ScriptList.push_back(ScriptFileName);

		}
		while(QFindNext(SearchRec));

	QFindClose(SearchRec);

	//Sort by name
	std::sort(ScriptList.begin(),ScriptList.end());

	for(std::vector<QString>::iterator i = ScriptList.begin(); i != ScriptList.end(); i++)
	{
		//Create sub-menus
		TMenuItem *TempItem = new TMenuItem(this);
		TempItem->Caption = i->c_str();
		TempItem->OnClick = ScriptMenuEventHandler;

		ScriptsMenuItem->Add(TempItem);
	}

	//If no sripts found, hide the scripts menu
	ScriptsMenuItem->Visible = ScriptList.size() > 0;
}

void __fastcall TMainForm::ScriptMenuEventHandler(TObject *Sender)
{
	TMenuItem *MenuItem = dynamic_cast<TMenuItem *>(Sender);

	//Figure out the full python script file name
	QString ScriptFileName = Q2RTApplication->AppFilePath.Value() + "Scripts\\" + MenuItem->Caption.c_str() + ".py";
	//Execute script file
	Q2RTApplication->ExecFileSafe(ScriptFileName);
}

//Handle the wizard proxy object "run wizard" requests
bool TMainForm::OnRunWizardCallback(void)
{
	bool ret = false;

	// Run a python wizard (use current python context without an external file)
	CPythonAutoWizard PythonWizard;

	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	try
	{
		BackEnd->MarkPythonWizardRunning(true);
		ret = WizardForm->RunWizard(&PythonWizard);
	}
	__finally
	{
		BackEnd->MarkPythonWizardRunning(false);
	}
	return ret;
}

bool TMainForm::DoStartUpBIT(void)
{
	if(CAppParams::Instance()->DoStartUpBIT)
	{
		BITViewerForm->RefreshTests();

		//Run the start-up test set
		TTestResult TestResult = BITViewerForm->RunBITSet("Start-Up");

		AnsiString ReportFileName = Q2RTApplication->AppFilePath.Value().c_str() + AnsiString("BITReport.htm");

		switch(TestResult)
		{
			case trWarning:
			{
				QString Msg = "Warning: At least one start-up BIT test issued a warning.";
				CQLog::Write(LOG_TAG_GENERAL,Msg);
				QMonitor.WarningMessage(Msg);
				BITViewerForm->CreateBITReportFile(ReportFileName);
			}
			break;

			case trNoGo:
			{
				QString Msg = "Error: At least one start-up BIT test has failed.";
				CQLog::Write(LOG_TAG_GENERAL,Msg);
				QMonitor.ErrorMessage(Msg);
				BITViewerForm->CreateBITReportFile(ReportFileName);
			}
			break;
		}

		return (TestResult != trNoGo);
	}

	return true;
}

void __fastcall TMainForm::ExitMaintenanceModeActionExecute(TObject *Sender)
{
	LeaveMaintenanceMode();
	SetMaintenanceMode(false);
	m_ValidPassword = false; //OBJET_MACHINE
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::MainUIFrameOpenQuickSHRActionExecute(TObject *Sender)
{
	MainUIFrame->OpenQuickSHRActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenParamsDialogActionExecute(TObject *Sender)
{
	TQualityModeIndex    MaintenanceMode;
	TOperationModeIndex  OperationMode;
	MainUIFrame->GetMaintenanceMode(MaintenanceMode, OperationMode);

	CBackEndInterface* BackEndInterface     = CBackEndInterface::Instance();
	bool               AllowMaintenanceMode = MainUIFrame->IsMaintenanceModeAllowed();
	QString            CurrentModeName      = BackEndInterface->GetCurrentModeName();

	//Load the 'Maintenance Mode' only if the machine's state allow it
	if((AllowMaintenanceMode) && (CurrentModeName == DEFAULT_MODE) &&
	        VALIDATE_QUALITY_MODE(MaintenanceMode) && VALIDATE_OPERATION_MODE(OperationMode))
	{
		BackEndInterface->GotoDefaultMode();
		BackEndInterface->EnterMode(PRINT_MODE[MaintenanceMode][OperationMode],
		                            MACHINE_QUALITY_MODES_DIR(MaintenanceMode,OperationMode));
	}

	UpdateParameterManager(GetCurrentParamLevel());

	m_ParamsDlg->Execute();
	AppRestartFlag = m_ParamsDlg->IsRestartRequired();

	if(AppRestartFlag)
		Close();
}

//---------------------------------------------------------------------------


void __fastcall TMainForm::SwitchMaintenanceModeActionExecute(TObject *Sender)
{
	TPoint PopUpPoint = MaintenancePopupMenu->PopupPoint;

	LeaveMaintenanceMode();

	int x = PopUpPoint.x;
	int y = PopUpPoint.y;
	ChooseMaintenanceModePopupMenu->Popup(x,y);
}

//---------------------------------------------------------------------------

void TMainForm::LeaveMaintenanceMode()
{
	CBackEndInterface *BackEndInterface = CBackEndInterface::Instance();
	bool AllowMaintenanceMode = MainUIFrame->IsMaintenanceModeAllowed();

	MaintenanceModeEnhancedLabel->Visible = false;
	MaintenanceModeEnhancedLabel->Caption = "";

	//Resetting Mode (in case not printing...)
	if(AllowMaintenanceMode)
		BackEndInterface->GotoDefaultMode();

	if((ChooseMaintenanceModePopupMenu->Items->Count - CHOOSE_MAINTENANCE_ITEMS_START_COUNT) == 1)
		MaintenanceMenuLabel->Menu = MaintenancePopupMenu;
	else
		MaintenanceMenuLabel->Menu = ChooseMaintenanceModePopupMenu;
}

//---------------------------------------------------------------------------

void TMainForm::EnableDisableAllMaintenanceModes(bool Enabled)
{
	FOR_ALL_QUALITY_MODES(qm)
	{
		FOR_ALL_OPERATION_MODES(om)
		{
			if(GetModeAccessibility(qm, om) == false)
				continue;
			if(m_ModesMenuItem[qm][om])
				m_ModesMenuItem[qm][om]->Enabled = Enabled;
		}
	}
}

void TMainForm::EnableDisableMaintenanceModes(int QualityMode,int OperationMode)
{
	if(!VALIDATE_QUALITY_MODE(QualityMode) ||
	        !VALIDATE_OPERATION_MODE(OperationMode))
		return;

	EnableDisableAllMaintenanceModes(false);
	m_ModesMenuItem[QualityMode][OperationMode]->Enabled = true;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::ChooseMaintenanceModeActionExecute(TObject *Sender)
{
	TModalResult ModalResult = mrOk;

	bool AllowMaintenanceMode = MainUIFrame->IsMaintenanceModeAllowed();
	TAction *ActionItem = dynamic_cast<TAction *>(Sender);
	if(ActionItem == NULL)
		throw("@$%@#$#@#!%");

	TMenuItem *MenuItem = dynamic_cast<TMenuItem *>(ActionItem->ActionComponent);

	if(MenuItem != NULL)
	{
		CBackEndInterface *BackEndInterface = CBackEndInterface::Instance();

		if(CAppParams::Instance()->DisplayMaintenanceWarning)
			ModalResult = MaintenanceUserWarningForm->ShowModal();

		//Continue only if the user approves...
		if(ModalResult == mrOk)
		{
			int qm,om;
			TAG_TO_MODE(MenuItem->Tag,qm,om);
			MaintenanceModeEnhancedLabel->Caption = ("Maintenance Mode: " + GetModeStr(qm, om)).c_str();

			MaintenanceModeEnhancedLabel->Repaint();
			MaintenanceModeEnhancedLabel->Visible = true;

			//Update relevant Forms
			MainUIFrame->SetMaintenanceMode((TQualityModeIndex)qm,(TOperationModeIndex)om);
			ParamsExportForm->SetMaintenanceMode((TQualityModeIndex)qm,(TOperationModeIndex)om);

			//Resetting Mode (in case not printing...)
			if(AllowMaintenanceMode)
				BackEndInterface->GotoDefaultMode();

			MaintenanceMenuLabel->Menu = MaintenancePopupMenu;
			RefreshModesDisplay();

			//Displaying the 'MaintenancePopupMenu' Menu....
			TPoint PopUpPoint = ChooseMaintenanceModePopupMenu->PopupPoint;
			int x = PopUpPoint.x;
			int y = PopUpPoint.y;
			MaintenancePopupMenu->Popup(x,y);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenServiceNotesActionExecute(TObject *Sender)
{
	QString DirName  = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_SERVICE_NOTES_DIR_NAME);
	QString FileName = DirName + LOAD_STRING(IDS_SERVICE_NOTES_FILE_NAME);
	QString Content  = LOAD_STRING(IDS_SERVICE_NOTES_FILE_RESTRICTIONS);
	Content += LOAD_STRING(IDS_SERVICE_NOTES_FILE_INTENTED);
	CBackEndInterface::Instance()->ForceFileCreation(FileName, Content);
	ExternalAppExecute(FileName.c_str(), "");
}/*OpenServiceNotesActionExecute*/

void __fastcall TMainForm::SelectButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Select";
	ButtonLabel->Visible = true;
}/*SelectButtonMouseEnter*/

void __fastcall TMainForm::StopButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Stop Print";
	ButtonLabel->Visible = true;
}/*StopButtonMouseEnter*/

void __fastcall TMainForm::OfflineButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Go Online";
	ButtonLabel->Visible = true;
}/*OfflineButtonMouseEnter*/

void __fastcall TMainForm::OnlineButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Go Offline";
	ButtonLabel->Visible = true;
}/*OnlineButtonMouseEnter*/

void __fastcall TMainForm::PauseButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Pause Print";
	ButtonLabel->Visible = true;
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::GoButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Resume Print";
	ButtonLabel->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ResetButtonLabelActionExecute(TObject *Sender)
{
	ButtonLabel->Caption = "";
	ButtonLabel->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MinimizeEnhancedButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Minimize";
	ButtonLabel->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CloseEnhancedButtonMouseEnter(TObject *Sender)
{
	ButtonLabel->Caption = "Close";
	ButtonLabel->Visible = true;
}
//---------------------------------------------------------------------------

//Refresh the date/time indicators in the upper right corner
void TMainForm::UpdateDateTimeDisplay(TDateTime CurrentTime)
{
	static AnsiString MonthLookup[13] = {"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
	                                     "Sep","Oct","Nov","Dec"
	                                    };

	USHORT h,m,s,ms;

	DecodeTime(CurrentTime,h,m,s,ms);

	//Update the time if the minutes changed
	if(m != m_LastMinute)
	{
		TimeLabel->Caption = Format("%d:%.2d",ARRAYOFCONST((h,m)));
		m_LastMinute = m;

		USHORT Year,Month,Day;

		//Update the date if the days changed
		DecodeDate(CurrentTime,Year,Month,Day);
		if(Day != m_LastDay)
		{
			DateLabel->Caption = Format("%.2d %s %.2d",ARRAYOFCONST((Day,MonthLookup[Month],Year)));
			m_LastDay = Day;
		}
	}
}


void __fastcall TMainForm::BackGroundImageMouseUp(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(m_IsDragChanged == true)
	{
		m_IsDragChanged = false;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, true, 0, SPIF_SENDWININICHANGE);
	}

	m_IsMouseDown = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BackGroundImageMouseMove(TObject *Sender,
        TShiftState Shift, int X, int Y)
{
	if(m_IsMouseDown == true)
	{
		TPoint CurrPosOnScreen = ClientToScreen(Point(X,Y));

		if((m_ClickPoint.x != CurrPosOnScreen.x) || (m_ClickPoint.y != CurrPosOnScreen.y))
		{

			Left = m_FormPosWhileClick.x + (CurrPosOnScreen.x - m_ClickPoint.x);
			Top  = m_FormPosWhileClick.y + (CurrPosOnScreen.y - m_ClickPoint.y);

			//Updating positions
			m_ClickPoint.x = CurrPosOnScreen.x;
			m_ClickPoint.y = CurrPosOnScreen.y;

			m_FormPosWhileClick = (Point(Left,Top));
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BackGroundImageMouseDown(TObject *Sender,
        TMouseButton Button, TShiftState Shift, int X, int Y)
{
	bool IsDrag;
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &IsDrag, 0);


	if(IsDrag == true)
	{
		m_IsDragChanged = true;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, false, 0, SPIF_SENDWININICHANGE);
	}

	m_ClickPoint = ClientToScreen(Point(X,Y));
	m_FormPosWhileClick = (Point(Left,Top));
	m_IsMouseDown = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenUVLampsHistoryActionExecute(TObject *Sender)
{
	QString DirName  = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_UV_LAMPS_HISTORY_DIR_NAME);
	QString FileName = DirName + LOAD_STRING(IDS_UV_LAMPS_HISTORY_FILE_NAME);
	CBackEndInterface::Instance()->ForceFileCreation(FileName, LOAD_STRING(IDS_UV_LAMPS_HISTORY_RESTRICTIONS));
	ExternalAppExecute(FileName.c_str(), "");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ShowTroubleshootingActionExecute(TObject *Sender)
{
	QString FileName = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_TROUBLESHOOTING_FILENAME);
	ExternalAppExecute(FileName.c_str(), NULL);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::ShowKeyboardMapActionExecute(TObject *Sender)
{
	//If the form is already open (but somehow hidden) show it again...
	if(KeyboardMapForm->Visible == true)
		KeyboardMapForm->Visible = false;

	KeyboardMapForm->Visible = true;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::ScreensSelectionActionExecute(TObject *Sender)
{
	MainUIFrame->GetCurrentScreen()->SelectSoftButtonPressed();
	MainUIFrame->GetCurrentScreen()->SelectSoftButtonReleased();
}
//---------------------------------------------------------------------------

void TMainForm::UpdateParameterManager(int NewLevel)
{
	m_CurrentParamsLevel = NewLevel;

	// Initialize the parameters dialog
	if(! m_ParamsDlg)
	{
		m_ParamsDlg = new TParamsDialog();
	}
	else
	{
		TParamsDialog* NewParamsDlg = new TParamsDialog(*m_ParamsDlg);
		Q_SAFE_DELETE(m_ParamsDlg);
		m_ParamsDlg = NewParamsDlg;
	}

	CAppParams *ParamMgr = CAppParams::Instance();

	CQStringList HidedTabs;
	if(!ParamMgr->RemovableTray)  //runtime objet
	{
		HidedTabs.Add(LOAD_STRING(IDS_TRAY_HEATER));
	}
	/*
	#if !defined (CONNEX_260) && !defined (OBJET_260)
	HidedTabs.Add(LOAD_STRING(IDS_TRAY_HEATER));
	#endif
	*/
	//OBJET_MACHINE
	m_ParamsDlg->AddParamsContainer(ParamMgr, NewLevel, false, &HidedTabs, m_ValidPassword);

	m_ParamsDlg->ButtonsMode = pbApplyClose;
	m_ParamsDlg->OnApply     = ParametersApplyEventHandler;
}

void __fastcall TMainForm::ModesMenuItemClick(TObject *Sender)
{
	MainUIFrame->ModesMenuActionExecute(Sender);
}

void TMainForm::ShowDefaultMaintenanceModeObserver(CQParameterBase* Param,TGenericCockie Cockie)
{
	bool Visible = CAppParams::Instance()->ShowDefaultMaintenanceMode;
	// Show / hide 'Maintenance Default Mode'
	MainForm->DefaultModeMenuItem->Visible          = Visible;
	MainForm->DefaultModeSeperatorMenuItem->Visible = Visible;
}
//---------------------------------------------------------------------------


TQErrCode saveAllParameters(CAppParams *p)
{
	return p->SaveAll();
}



void __fastcall TMainForm::MainUIFrameShutdownActionExecute(
    TObject *Sender)
{
	MainUIFrame->ShutdownActionExecute(Sender);

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UVCalibrationWizard2Click(TObject *Sender)
{
	MainUIFrame->UVCalibrationWizardExecute(Sender);

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UVMCalibrationWizard1Click(TObject *Sender)
{
	MainUIFrame->UVMCalibrationWizardExecute(Sender);

}
//---------------------------------------------------------------------------

int TMainForm::GetCurrentParamLevel()
{
	return m_CurrentParamsLevel;
}
//OBJET_MACHINE
bool TMainForm::GetValidPassword()
{
	return m_ValidPassword;
}


TMainUIFrame *TMainForm::GetUIFrame(void)
{
	return MainUIFrame;
}

TParamsDialog *TMainForm::GetParamsDialog(void)
{
	return m_ParamsDlg;
}

void TMainForm::SetCurrentParamLevel(int ParamLevel)
{
	m_CurrentParamsLevel = ParamLevel;
}
//OBJET_MACHINE
void TMainForm::SetValidPassword(bool ValidPassword)
{
	m_ValidPassword = ValidPassword;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::CurrentData1Click(TObject *Sender)
{
	MainUIFrame->OpenHaspInfoDlgActionExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenHaspToolActionExecute(TObject *Sender)
{
	THaspPlugInfo PlugInfo = CBackEndInterface::Instance()->GetHaspPlugInfo();
	int DaysToExpiration   = CBackEndInterface::Instance()->GetHaspDaysToExpiration();

	if((PlugInfo.LicenseType == "trial") && (DaysToExpiration < CAppParams::Instance()->HASP_DaysToExpirationNotification) && (DaysToExpiration != -1))
		QMonitor.NotificationMessageWaitOk(QFormatStr(LOAD_STRING(IDS_HASP_TRIAL_EXPIRATION_WARNING), DaysToExpiration));

	QString DirName  = Q2RTApplication->AppFilePath.Value() + LOAD_STRING(IDS_HASP_TOOLS_DIR_NAME);
	QString FileName = DirName + LOAD_STRING(IDS_HASP_TOOLS_FILE_NAME);
	ExternalAppExecute(FileName.c_str(), "");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::LicenseUpdate1Click(TObject *Sender)
{
	OpenHaspToolActionExecute(Sender);
}
//---------------------------------------------------------------------------

void TMainForm::HaspStatusLabelUpdate(QString Status, TGenericCockie Cockie)
{
	TMainForm *MainForm = reinterpret_cast<TMainForm *>(Cockie);
	MainForm->HaspStatusLabel->Caption = Status.c_str();

	if(! CBackEndInterface::Instance()->IsPrintingWizardRunning())
	{
		if(Status.compare(LOAD_STRING(IDS_HASP_OK)) == 0)
		{
			MainForm->CloseMessageDlg();
		}
		else if(Status.compare(LOAD_STRING(IDS_HASP_LICENSE_EXPIRED)) == 0)
		{
			MainForm->CloseMessageDlg();
			QMonitor.NotificationMessage(LOAD_STRING(IDS_HASP_LICENSE_EXPIRED_MESSAGE));
			MainForm->m_HaspExpirationMessageAlreadyShown = false;
		}
		else if(Status.compare(LOAD_STRING(IDS_HASP_NOT_DETECTED)) == 0)
		{
			MainForm->CloseMessageDlg();
			QMonitor.NotificationMessage(LOAD_STRING(IDS_HASP_NOT_DETECTED_MESSAGE));
			MainForm->m_HaspExpirationMessageAlreadyShown = false;
		}
		else if(Status.compare(LOAD_STRING(IDS_HASP_INVALID)) == 0)
		{
			MainForm->CloseMessageDlg();
			QMonitor.NotificationMessage(LOAD_STRING(IDS_HASP_INVALID_MESSAGE));
			MainForm->m_HaspExpirationMessageAlreadyShown = false;
		}
		else // Assuming that the only message that left is 'days to expiration'
		{
			// Show this message only once at application startup or when new plug in experation range is inserted
			if(! MainForm->m_HaspExpirationMessageAlreadyShown)
			{
				MainForm->CloseMessageDlg();
				int days = -1;
				sscanf(Status.c_str(), "%*s %*s %*s %d", &days);

				if(Status == LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_TODAY))
				{
					CQEncryptedLog::Write(LOG_TAG_HASP, "HASP: " + QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_TODAY_MESSAGE)));
					QMonitor.NotificationMessage(QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_TODAY_MESSAGE)));
				}
				else
				{
					CQEncryptedLog::Write(LOG_TAG_HASP, "HASP: " + QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_IN_MESSAGE), days));
					QMonitor.NotificationMessage(QFormatStr(LOAD_STRING(IDS_HASP_LICENSE_EXPIRES_IN_MESSAGE), days));
				}
				MainForm->m_HaspExpirationMessageAlreadyShown = true;
			}
		}
	}
}

void TMainForm::CloseMessageDlg()
{
	HANDLE MessageFormHDL = NULL;
	MessageFormHDL = FindWindow("TMessageForm", "Information");
	if(MessageFormHDL != NULL)
		SendMessage(MessageFormHDL, WM_CLOSE, 0, 0);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
//OBJET_MACHINE
void __fastcall TMainForm::MaterialBatchNoMenuItemClick(TObject *Sender)
{
	MainUIFrame->MaterialBatchNoActionExecute(Sender);
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::HeadsControl1Click(TObject *Sender)
{
	MainUIFrame->OpenHeadsControlActionExecute(Sender);

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Wizards1Click(TObject *Sender)
{
	MainUIFrame->UserWizardsActionExecute(Sender);
}
void __fastcall TMainForm::HeadOptimizationScaleBased1Click(TObject *Sender)
{
	MainUIFrame->HeadsOptimizationScalesBasedActionExecute(Sender);
}
//---------------------------------------------------------------------------

MESSAGE void TMainForm::HandlePowerStatus(TMessage &msg)
{
    CAppParams      * ParamsMgr = CAppParams::Instance();
    CMachineManager * Mgr       = Q2RTApplication->GetMachineManager();
    // If we're receiving this message on application boot, MachineManager might not be built yet.
    // We're allowed to ignore this case since we'll soon receive another GUID_BATTERY_PERCENTAGE_REMAINING event.
    if (!Mgr)
        return;

    if(msg.Msg != WM_POWERBROADCAST)
    {
        return;
    }

    if (msg.WParam == PBT_POWERSETTINGCHANGE)
    {
        PPOWERBROADCAST_SETTING set = (PPOWERBROADCAST_SETTING)msg.LParam;
        if (set->PowerSetting == GUID_BATTERY_PERCENTAGE_REMAINING)
        {
            if (sizeof(DWORD) == set->DataLength)
            {
                int percentage = set->Data[0];

                CQLog::Write(LOG_TAG_GENERAL,"Battery percentage changed. Remaining: %d%", percentage);
                if ((PoAc != m_PowerSource) && (percentage < ParamsMgr->SHD_BatteryPercentageRemainingThreshold))
                {
                    CQLog::Write(LOG_TAG_GENERAL,"Remaining battery percentage has dropped below %d%. Performing unattended shutdown.", ParamsMgr->SHD_BatteryPercentageRemainingThreshold.Value());
                    if (!(Mgr->GetMachineSequencer()->GetIsPowerByPassIgnored()))
                    {
                        Mgr->PowerFailShutdown();
                    }
                }
            }
        }
        else //	This is a power source change notification
        if (set->PowerSetting == GUID_ACDC_POWER_SOURCE)
        {
            if(sizeof(DWORD) == set->DataLength)
            {
                m_PowerSource = set->Data[0];
                CQLog::Write(LOG_TAG_GENERAL,"Power source has changed. Power source is: %s", (PoAc != m_PowerSource) ? "UPS Battery" : "AC power");
            }
        }
    }
}