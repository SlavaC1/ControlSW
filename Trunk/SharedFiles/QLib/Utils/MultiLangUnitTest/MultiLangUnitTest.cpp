//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("MultiLangUnitTest.res");
USEFORM("MainUnit.cpp", Form1);
USELIB("..\..\LibraryProject\Windows\QLibProject.lib");
USERC("test1.rc");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
