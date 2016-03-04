//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("RPCUnitTest.res");
USEFORM("MainUnit.cpp", MainForm);
USEUNIT("..\..\EdenSerialProtocol\EdenProtocolEngine.cpp");
USEUNIT("..\..\EdenSerialProtocol\EdenProtocolClient.cpp");
USELIB("..\..\..\..\..\SharedFiles\QLib\LibraryProject\Windows\QLibProject.lib");
USEUNIT("..\RPCServer.cpp");
USEUNIT("..\RPCClient.cpp");
USEUNIT("..\PascalString.cpp");
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
