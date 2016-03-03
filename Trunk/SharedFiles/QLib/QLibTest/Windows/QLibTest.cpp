//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("QLibTestMainUnit.cpp", MainForm);
USELIB("..\..\LibraryProject\Windows\QLibProject.lib");
USEFORM("..\..\MiniDebugger\Windows\MiniDebugger.cpp", MiniDebuggerForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TMiniDebuggerForm), &MiniDebuggerForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
