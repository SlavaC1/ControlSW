//---------------------------------------------------------------------------

// todo -oNobody -cNone: some compiler/linker warning messages are disabled. Reenabling them cause the linker error "Fatal: Access violation.  Link terminated.". Figure out how to compile with more warnings enaled...

// JCL_DEBUG_EXPERT_GENERATEJDBG OFF
#include "SplashScreen.h"
#include <vcl.h>
#pragma hdrstop
USEFORM("..\..\..\Base\FrontEnd\Windows\MainUnit.cpp", MainForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\EdenMainUIFrame.cpp", MainUIFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\PCIOptionsDlg\PCIOptionsDlg.cpp", PCIOptionsForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\OHDBOptionsDlg\OHDBOptionsDlg.cpp", OHDBOptionsForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\About.cpp", AboutBox);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\ImageViewDlg.cpp", ImageViewForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CalibrationDlg.cpp", CalibrationForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\ParamsExportDlg.cpp", ParamsExportForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CartridgeErrorDlg.cpp", CartridgeErrorForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CustomTrayPointsWizardFrame.cpp", TrayPointsWizardFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\LocalJobDlg.cpp", LocalJobForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\RollerWizardYMovementFrame.cpp", RollerWizardYMoveFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\UserValuesEntryWizardFrame.cpp", UserValuesEntryPageFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\DoorDlg.cpp", DoorCloseDlg);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\XOffsetChooseBestPrintFrame.cpp", XOffsetChooseBestPrintFram); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\HelpDlg.cpp", ShowHelpForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\MaintenanceWarningForm.cpp", MaintenanceUserWarningForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\KeyboardMapDlg.cpp", KeyboardMapForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\UVLampsStatusFrame.cpp", UVLampStatusFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\VacuumStatusFrame.cpp", VacuumStatusFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\StableValueFrame.cpp", StableValueFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\HeadsCheckBoxesFrame.cpp", HeadsCheckBoxFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\LoadCellCalibration.cpp", LoadCellCalibrationCheckBoxFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\MaterialWarningForm.cpp", MaterialWarningForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\WeightsStatusFrame.cpp", WeightsStatusFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\TankIdentificationNotice.cpp", TankIDNotice);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\ChooseSilentMRWMode.cpp", ChooseSilentMRWModeDLG);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\HeadsButtonsEntryFrame.cpp", UserValuesEntryFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\AgreementPageFrame.cpp", AgreementFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\MultiGridForm.cpp", MultiGridForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\UpDownAxisFrame.cpp", UpDownAxisFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\YAxisFrame.cpp", YAxisFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\ZAxisFrame.cpp", ZAxisFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\TAxisFrame.cpp", AxisTFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\XYAxisFrame.cpp", XYAxisFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CustomWiperCalibrationFrame.cpp", CustomWiperCalibrationFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\TrayDlg.cpp", TrayInsertDlg);
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CustomLoadCellWellcomeFrame.cpp", LoadCellWellcomeFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\MissingNozzlesFrame.cpp", MissingNozzlesScaleBasedFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CustomResinMainSelectWizardFrame.cpp", ResinMainSelectionFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CustomResinSelectAdvancedFrame.cpp", ResinSelectionAdvancedSettingsFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\CustomResinSelectSummaryFrame.cpp", ResinSelectionSummaryFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\TanksSummaryFrame.cpp", TanksSummaryFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\MaterialReplacementDoneFrame.cpp", MaterialReplacementDoneFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\UVLampsCalibrationFrame.cpp", UVLampsCalibrationWizardFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDialogs\UVLampsResultsFrame.cpp", UVLampResultsFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\EdenGeneralDeviceFrame.cpp", GeneralDeviceFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\EdenHeadsFrame.cpp", HeadsFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\GeneralDevicesDlg.cpp", GeneralDevicesForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\HeadsControlDlg.cpp", HeadsControlForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\MotorsControlDlg.cpp", MotorsControlForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\ActuatorsControlDlg.cpp", ActuatorsControlForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\LCDFrontEnd\LCDBaseScreen.cpp", LCDBaseFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\LCDFrontEnd\MainStatusScreen.cpp", MainStatusFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\LCDFrontEnd\MainMenuScreen.cpp", MainMenuFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\LCDFrontEnd\MaintenanceStatusScreen.cpp", MaintenanceStatusFrame); /* TFrame: File Type */
USEFORM("..\..\..\Base\FrontEnd\Windows\MaintenanceCountersDlg.cpp", MaintenanceCountersForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\MCBDiag\MCBDiagDlg.cpp", MCBDiagForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\BIT\BITViewerDlg.cpp", BITViewerForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\BIT\BITProgressDlg.cpp", BITProgressForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\BIT\BITExtResultsDlg.cpp", BITExtendedResultsForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\BIT\PrepareForBitDlg.cpp", PrepareBitDlg);
USEFORM("..\..\..\Base\FrontEnd\Windows\SplashScreen.cpp", SplashScreenForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\OcbOhdbControlDlg.cpp", OcbOhdbControlForm);
USEFORM("..\..\..\Base\FrontEnd\Windows\LotusLowLevelDlg\LotusLowLevel.cpp", LotusLowLevelForm);
USEFORM("..\..\..\Base\Sources\Wizards\HSWMissingNozzlesPage.cpp", Form1);
USEFORM("..\..\..\..\SharedFiles\QLib\MiniDebugger\Windows\MiniDebugger.cpp", MiniDebuggerForm);
USEFORM("..\..\..\..\SharedFiles\QLib\ParamsDialog\ParamsDialog.cpp", ParamsForm);
USEFORM("..\..\..\..\SharedFiles\QLib\ParamsDialog\ArrayParamsEditorDlg.cpp", ArrayParamsEditorForm);
USEFORM("..\..\..\..\SharedFiles\QLib\ParamsDialog\ParamsLevelExposure.cpp", ParametersExposureLevelForm);
USEFORM("..\..\..\..\SharedFiles\QLib\QPython\Windows\PythonConsole.cpp", PythonConsoleForm);
USEFORM("..\..\..\..\SharedFiles\QLib\QPython\Windows\QPythonIntegrator.cpp", QPythonIntegratorDM); /* TDataModule: File Type */
USEFORM("..\..\..\..\SharedFiles\QLib\QPython\Windows\PythonBreakpointDlg.cpp", PythonBreakpointForm);
USEFORM("..\..\..\..\SharedFiles\QLib\QPython\Windows\BreakpointPropertiesDlg.cpp", BreakpointPropertiesForm);
USEFORM("..\..\..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
USEFORM("..\..\..\..\SharedFiles\AutoWizard\WizardViewer.cpp", WizardViewerFrame); /* TFrame: File Type */
USEFORM("..\..\..\..\SharedFiles\AutoWizard\AutoWizardDlg.cpp", WizardForm);
USEFORM("..\..\..\..\SharedFiles\AutoWizard\TimeElapsingFrame.cpp", TimeElapsing); /* TFrame: File Type */
USEFORM("..\..\..\..\SharedFiles\HASP\HaspPlugInfoDlg.cpp", HaspPlugInformationDlg);
//---------------------------------------------------------------------------
bool AppRestartFlag;
const char AppMutexName[]              = "OBJET_EMBEDED_APPLICATION_MUTEX";
HANDLE     AppMutex                    = 0;
UINT       WM_OBJET_EMBEDED_UNIQUE_MSG = 0;

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);


WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR CmdLineParam, int)
{
        // If the program has been restarted, wait some time before starting
        if(AnsiString(CmdLineParam) == "Restart")
          Sleep(3000);

        HANDLE tmp    = CreateMutex(NULL,FALSE,AppMutexName);
        WM_OBJET_EMBEDED_UNIQUE_MSG = RegisterWindowMessage(AppMutexName);

        if(ERROR_ALREADY_EXISTS == GetLastError())
        {
           if (WM_OBJET_EMBEDED_UNIQUE_MSG != 0)
              SendMessage(HWND_BROADCAST,WM_OBJET_EMBEDED_UNIQUE_MSG,0,(LPARAM)WM_OBJET_EMBEDED_UNIQUE_MSG);
           return 0;
        }
        AppMutex = tmp;

        try
        {
          try
          {
                 SplashScreenForm = new TSplashScreenForm(NULL);
                 SplashScreenForm->Show();
                 SplashScreenForm->Repaint();

                 AppRestartFlag = false;
                 Application->Initialize();
                 Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TMiniDebuggerForm), &MiniDebuggerForm);
		Application->CreateForm(__classid(TPCIOptionsForm), &PCIOptionsForm);
		Application->CreateForm(__classid(TOHDBOptionsForm), &OHDBOptionsForm);
		Application->CreateForm(__classid(TLotusLowLevelForm), &LotusLowLevelForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TGeneralDevicesForm), &GeneralDevicesForm);
		Application->CreateForm(__classid(THeadsControlForm), &HeadsControlForm);
		Application->CreateForm(__classid(TMotorsControlForm), &MotorsControlForm);
		Application->CreateForm(__classid(TActuatorsControlForm), &ActuatorsControlForm);
		Application->CreateForm(__classid(TMaintenanceCountersForm), &MaintenanceCountersForm);
		Application->CreateForm(__classid(TCalibrationForm), &CalibrationForm);
		Application->CreateForm(__classid(TParamsExportForm), &ParamsExportForm);
		Application->CreateForm(__classid(TCartridgeErrorForm), &CartridgeErrorForm);
		Application->CreateForm(__classid(TWizardForm), &WizardForm);
		Application->CreateForm(__classid(TDoorCloseDlg), &DoorCloseDlg);
		Application->CreateForm(__classid(TLocalJobForm), &LocalJobForm);
		Application->CreateForm(__classid(TMCBDiagForm), &MCBDiagForm);
		Application->CreateForm(__classid(TQPythonIntegratorDM), &QPythonIntegratorDM);
		Application->CreateForm(__classid(TPythonBreakpointForm), &PythonBreakpointForm);
		Application->CreateForm(__classid(TBreakpointPropertiesForm), &BreakpointPropertiesForm);
		Application->CreateForm(__classid(TBITViewerForm), &BITViewerForm);
		Application->CreateForm(__classid(TBITProgressForm), &BITProgressForm);
		Application->CreateForm(__classid(TBITExtendedResultsForm), &BITExtendedResultsForm);
		Application->CreateForm(__classid(TShowHelpForm), &ShowHelpForm);
		Application->CreateForm(__classid(TMaintenanceUserWarningForm), &MaintenanceUserWarningForm);
		Application->CreateForm(__classid(TKeyboardMapForm), &KeyboardMapForm);
		Application->CreateForm(__classid(TParametersExposureLevelForm), &ParametersExposureLevelForm);
		Application->CreateForm(__classid(TMaterialWarningForm), &MaterialWarningForm);
		Application->CreateForm(__classid(TTankIDNotice), &TankIDNotice);
		Application->CreateForm(__classid(TChooseSilentMRWModeDLG), &ChooseSilentMRWModeDLG);
		Application->CreateForm(__classid(TMultiGridForm), &MultiGridForm);
		Application->CreateForm(__classid(TTrayInsertDlg), &TrayInsertDlg);
		Application->CreateForm(__classid(THaspPlugInformationDlg), &HaspPlugInformationDlg);
		Application->CreateForm(__classid(TLoadCellWellcomeFrame), &LoadCellWellcomeFrame);
		Application->CreateForm(__classid(TPrepareBitDlg), &PrepareBitDlg);
		Application->CreateForm(__classid(TMissingNozzlesScaleBasedFrame), &MissingNozzlesScaleBasedFrame);
		Application->CreateForm(__classid(TOcbOhdbControlForm), &OcbOhdbControlForm);
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();
          }
          catch (Exception &exception)
          {
                   Application->ShowException(&exception);
          }
        }
        __finally
        {
          if (SplashScreenForm)
          {
           delete SplashScreenForm;
           SplashScreenForm = NULL;
          }
          
          if(AppMutex)
          {
            ::CloseHandle(AppMutex);
            AppMutex = NULL;
          }
        }


        if(AppRestartFlag)
          ExternalAppExecute(Application->ExeName.c_str(),"Restart");

        return 0;
}
//---------------------------------------------------------------------------
