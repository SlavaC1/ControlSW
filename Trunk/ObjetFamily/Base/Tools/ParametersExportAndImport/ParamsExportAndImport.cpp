//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("MainUnit.cpp", MainForm);
USEFORM("..\..\..\..\SharedFiles\QLib\Monitor\Windows\QMonitorDlg.cpp", QMonitorForm);
//---------------------------------------------------------------------------
int AppRetVal;

#define APP_UNHANDLED_EXCEPTION 1
#define UNHANDLED_EXCEPTION     2

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        AppRetVal = 0;
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TMainForm), &MainForm);
     Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
                 AppRetVal = APP_UNHANDLED_EXCEPTION;
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
                         AppRetVal = UNHANDLED_EXCEPTION;
                 }
        }
        exit( AppRetVal );
        return 0;
}
//---------------------------------------------------------------------------
