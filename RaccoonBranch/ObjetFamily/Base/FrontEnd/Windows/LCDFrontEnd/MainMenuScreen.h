//---------------------------------------------------------------------------

#ifndef MainMenuScreenH
#define MainMenuScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "EnhancedMenu.hpp"
#include "LCDBaseScreen.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TMainMenuFrame : public TLCDBaseFrame
{
__published:	// IDE-managed Components
        TEnhancedMenu *OptionsMenu;
        void __fastcall SelectSoftButtonClick(TObject *Sender);
        void __fastcall OptionsMenuClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TMainMenuFrame(TComponent* Owner);

  // (Override)
  void NavigateButtonDown(void);
  void NavigateButtonUp(void);
};

#endif
