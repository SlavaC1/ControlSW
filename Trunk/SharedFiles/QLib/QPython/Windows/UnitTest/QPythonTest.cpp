//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("..\PythonConsole.cpp", PythonConsoleForm);
USEFORM("..\QPythonIntegrator.cpp", QPythonIntegratorDM); /* TDataModule: File Type */
USEFORM("QPythonTestMainUnit.cpp", Form1);
USEFORM("..\PythonBreakpointDlg.cpp", PythonBreakpointForm);
USEFORM("..\BreakpointPropertiesDlg.cpp", BreakpointPropertiesForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TQPythonIntegratorDM), &QPythonIntegratorDM);
                 Application->CreateForm(__classid(TPythonBreakpointForm), &PythonBreakpointForm);
                 Application->CreateForm(__classid(TBreakpointPropertiesForm), &BreakpointPropertiesForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
