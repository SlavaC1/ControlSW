//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


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



/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
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
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/



