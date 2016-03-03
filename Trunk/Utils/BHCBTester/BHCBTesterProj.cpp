//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("BHCBTester.cpp", BHCBTesterForm);
USEFORM("..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		SetApplicationMainFormOnTaskBar(Application, true);
		Application->Title = "BHCB Tester";
		Application->CreateForm(__classid(TBHCBTesterForm), &BHCBTesterForm);
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
