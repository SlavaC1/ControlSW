//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("ParamsDlgTest.res");
USEFORM("ParamsDlgTestMainUnit.cpp", MainForm);
USELIB("..\..\LibraryProject\Windows\QLibProject.lib");
USEFORM("..\ParamsDialog.cpp", ParamsForm);
USEFORM("..\ArrayParamsEditorDlg.cpp", ArrayParamsEditorForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
