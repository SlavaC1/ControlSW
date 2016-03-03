//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainMenuScreen.h"
#include "MainUnit.h"
#include "QMonitor.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EnhancedMenu"
#pragma link "LCDBaseScreen"
#pragma resource "*.dfm"

const int MENU_SELECT_MAINTENANCE_MODE = 0;
const int MENU_SELECT_PATTERN_TEST = 1;



//---------------------------------------------------------------------------
__fastcall TMainMenuFrame::TMainMenuFrame(TComponent* Owner)
        : TLCDBaseFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TMainMenuFrame::NavigateButtonDown(void)
{
  OptionsMenu->MoveDown();
}
//---------------------------------------------------------------------------
void TMainMenuFrame::NavigateButtonUp(void)
{
  OptionsMenu->MoveUp();
}
//---------------------------------------------------------------------------


/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TMainMenuFrame::SelectSoftButtonClick(TObject *Sender)
{
  SelectSoftButton->ItemIndex = -1;
  OptionsMenu->Activate();
}
//---------------------------------------------------------------------------
void __fastcall TMainMenuFrame::OptionsMenuClick(TObject *Sender)
{
  TMainUIFrame *MainUIFrame = MainForm->GetUIFrame();

  switch(OptionsMenu->ItemIndex)
  {
    case MENU_SELECT_PATTERN_TEST:
      MainUIFrame->DoTestPatternAction->Execute();
      break;

    case MENU_SELECT_MAINTENANCE_MODE:
      MainForm->GetUIFrame()->NavigateForward(MAIN_MAINTENANCE_STATUS_SCREEN_ID);
      break;
  }
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
