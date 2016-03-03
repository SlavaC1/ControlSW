//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("PacksViewer.cpp", PacksViewerForm);
USEFORM("About.cpp", AboutForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		SetApplicationMainFormOnTaskBar(Application, true);
		Application->Title = "Advanced licensing packages viewer";
		Application->CreateForm(__classid(TPacksViewerForm), &PacksViewerForm);
		Application->CreateForm(__classid(TAboutForm), &AboutForm);
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
