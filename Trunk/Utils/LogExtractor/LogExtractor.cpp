//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("LogExtractorMainUnit.cpp", MainForm);
USEFORM("TagsDictDlg.cpp", TagsDictForm);
USEFORM("..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
//---------------------------------------------------------------------------
AnsiString gFileNameToOpen;

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR Args, int)
{
  gFileNameToOpen = Args;

        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TTagsDictForm), &TagsDictForm);
		Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
