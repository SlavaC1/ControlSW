//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("DetectorUI.cpp", DetectorForm);
USEFORM("..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
USEFORM("About.cpp", AboutBox);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		SetApplicationMainFormOnTaskBar(Application, true);
		Application->CreateForm(__classid(TDetectorForm), &DetectorForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
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
