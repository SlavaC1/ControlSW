//---------------------------------------------------------------------------

#ifndef GeneralDevicesDlgH
#define GeneralDevicesDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "EdenGeneralDeviceFrame.h"
#include <ActnList.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TGeneralDevicesForm : public TForm
{
__published:	// IDE-managed Components
  TGeneralDeviceFrame *GeneralDeviceFrame;
        TActionList *ActionList1;
        TAction *AlwaysOnTopAction;
        TAction *CloseDialogAction;
        TPopupMenu *PopupMenu1;
        TMenuItem *Alwaysontop1;
        TMenuItem *N1;
        TMenuItem *Close1;
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormShow(TObject *Sender);
        void __fastcall AlwaysOnTopActionExecute(TObject *Sender);
        void __fastcall CloseDialogActionExecute(TObject *Sender);
        void __fastcall GeneralDeviceFrameTurnRollerButtonClick(
          TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TGeneralDevicesForm(TComponent* Owner);

  TGeneralDeviceFrame* GetUIFrame();
};
//---------------------------------------------------------------------------
extern PACKAGE TGeneralDevicesForm *GeneralDevicesForm;
//---------------------------------------------------------------------------
#endif
