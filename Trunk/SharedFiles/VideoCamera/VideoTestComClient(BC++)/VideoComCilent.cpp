//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <ComObj.HPP>

#include "VideoCameraAutomationLib_TLB.h"
#include "VideoComCilent.h"


#include <windows.h>
#include <dialogs.hpp>

#include <memory>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
//	CoInitialize(0);

//    ShowMessage("Attach now to debug");
	try
    {
		_camera = CoVideoCameraComWrapper::Create();
    }
    catch(EOleException &e)
    {
    	int i = 0;
    }

/*
    IVideoCamera *pCamera = 0;
    CoCreateInstance(CLSID_VideoCameraComWrapper,
        NULL,CLSCTX_INPROC_SERVER,
        IID_IVideoCamera,
        (void **)&pCamera);
*/


}
//---------------------------------------------------------------------------
void __fastcall TForm1::_pButton_sampleToFileClick(TObject *Sender)
{
    HBITMAP hb;
    hb = _camera.sampleToBitmap();

    std::auto_ptr<Graphics::TBitmap> btmp(new Graphics::TBitmap());
    btmp->Handle = hb;

    std::auto_ptr<TFileStream> s(new TFileStream(_pEditBox_path->Text, fmCreate));

    btmp->SaveToStream(s.get());

}
//---------------------------------------------------------------------------



void __fastcall TForm1::_pButton_setSettingsClick(TObject *Sender)
{

	_camera.set_Gamma(StrToFloat(_pEditBox_gamma->Text));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	_camera.openSettingsDialog(0);
}
//---------------------------------------------------------------------------

