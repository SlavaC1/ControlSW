//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("PCITesterMainUnit.cpp", MainForm);
USEFORM("ContinousWriteDlg.cpp", ContinousWriteForm);
USEFORM("HelpDlg.cpp", HelpForm);
USEFORM("..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TContinousWriteForm), &ContinousWriteForm);
		Application->CreateForm(__classid(THelpForm), &HelpForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		 Application->ShowException(&exception);
	}
	catch (...)
	{
		 try
		 {
			 throw Exception("");
		 }
		 catch (Exception &exception)
		 {
			 Application->ShowException(&exception);
		 }
	}
	return 0;
}
//---------------------------------------------------------------------------
