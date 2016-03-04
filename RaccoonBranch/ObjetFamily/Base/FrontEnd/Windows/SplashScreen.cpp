//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma warn -8057 // Disable "Parameter is never used" warning.

#include "SplashScreen.h"
#include "FEResources.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplashScreenForm *SplashScreenForm;
//---------------------------------------------------------------------------
__fastcall TSplashScreenForm::TSplashScreenForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSplashScreenForm::FormCreate(TObject *Sender)
{
  Graphics::TBitmap *TmpImage;

  try
  {
    TmpImage = new Graphics::TBitmap;
    LOAD_BITMAP(TmpImage, IDB_SPLASH_SCREEN);

    SplashImage->Picture->Assign(TmpImage);
  }
  catch(...)
  {}

  delete TmpImage;

}
//---------------------------------------------------------------------------

