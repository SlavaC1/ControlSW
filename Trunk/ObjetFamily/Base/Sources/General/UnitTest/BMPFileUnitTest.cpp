//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("BMPFileUnitTest.res");
USEFORM("BMPFileUnitTestMain.cpp", Form1);
USELIB("..\..\..\..\..\SharedFiles\QLib\LibraryProject\Windows\QLibProject.lib");
USEUNIT("..\BMPFile.cpp");
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
