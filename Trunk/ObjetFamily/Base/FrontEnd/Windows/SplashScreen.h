//---------------------------------------------------------------------------

#ifndef SplashScreenH
#define SplashScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TSplashScreenForm : public TForm
{
__published:	// IDE-managed Components
        TImage *SplashImage;
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TSplashScreenForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSplashScreenForm *SplashScreenForm;
//---------------------------------------------------------------------------
#endif
