//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("MainUnit.cpp", TestForm);
USEFORM("..\WizardViewer.cpp", WizardViewerFrame); /* TFrame: File Type */
USEFORM("..\AutoWizardDlg.cpp", WizardForm);
USEFORM("..\..\QLib\QPython\Windows\PythonConsole.cpp", PythonConsoleForm);
USEFORM("..\..\QLib\QPython\Windows\QPythonIntegrator.cpp", QPythonIntegratorDM); /* TDataModule: File Type */
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TTestForm), &TestForm);
                 Application->CreateForm(__classid(TWizardForm), &WizardForm);
                 Application->CreateForm(__classid(TPythonConsoleForm), &PythonConsoleForm);
                 Application->CreateForm(__classid(TQPythonIntegratorDM), &QPythonIntegratorDM);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
