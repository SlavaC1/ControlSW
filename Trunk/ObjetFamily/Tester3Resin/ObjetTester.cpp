//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
USEFORM("..\..\SharedFiles\QLib\ParamsDialog\ArrayParamsEditorDlg.cpp", ArrayParamsEditorForm);
USEFORM("Sources\OCBSimualtor\OCBSimulatorDlg.cpp", OCBSimulatorForm);
USEFORM("Sources\Setup\SystemFileDlg\SystemFileDlg.cpp", SystemFileForm);
USEFORM("Sources\Setup\SetupDlg.cpp", SetupForm);
USEFORM("Sources\OHDBTester\OHDBTesterDlg.cpp", OHDBTesterForm);
USEFORM("Sources\OCBTester\OCBTesterDlg.cpp", OCBTesterForm);
USEFORM("Sources\MainUnit\MainUnit.cpp", MainForm);
USEFORM("Sources\MCBSimulator\MCBSimulatorDlg.cpp", MCBSimulatorForm);
USEFORM("Sources\HeadsTester\HeadsTester.cpp", HeadsTesterForm);
USEFORM("Sources\HeadsTester\GenFourHeadsTester.cpp", GenFourHeadsTesterForm);
USEFORM("Sources\HeadsTester\NozzlesMap.cpp", NozzlesMapFrame); /* TFrame: File Type */
USEFORM("Sources\HeadsTester\FireControl.cpp", FireControlFrame); /* TFrame: File Type */
USEFORM("Sources\HeadsTester\HeadsControl.cpp", HeadsControlFrame); /* TFrame: File Type */
USEFORM("Sources\ReliabilityTest\ReliabilityTest.cpp", ReliabilityTestForm);
USEFORM("Sources\RFIDTester\RFIDTester.cpp", RFIDForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
     Application->Initialize();
     Application->Title = "Stratasys tester";
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TOCBSimulatorForm), &OCBSimulatorForm);
		Application->CreateForm(__classid(TSystemFileForm), &SystemFileForm);
		Application->CreateForm(__classid(TSetupForm), &SetupForm);
		Application->CreateForm(__classid(THeadsTesterForm), &HeadsTesterForm);
		Application->CreateForm(__classid(TOHDBTesterForm), &OHDBTesterForm);
		Application->CreateForm(__classid(TOCBTesterForm), &OCBTesterForm);
		Application->CreateForm(__classid(TMCBSimulatorForm), &MCBSimulatorForm);
		Application->CreateForm(__classid(TReliabilityTestForm), &ReliabilityTestForm);
		Application->CreateForm(__classid(TRFIDForm), &RFIDForm);
		Application->CreateForm(__classid(TFireControlFrame), &FireControlFrame);
		Application->CreateForm(__classid(TGenFourHeadsTesterForm), &GenFourHeadsTesterForm);
		Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
