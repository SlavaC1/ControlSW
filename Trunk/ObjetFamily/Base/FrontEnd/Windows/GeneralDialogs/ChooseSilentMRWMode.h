//----------------------------------------------------------------------------
#ifndef ChooseSilentMRWModeH
#define ChooseSilentMRWModeH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include "GlobalDefs.h"
//----------------------------------------------------------------------------
typedef enum
{
  mrSingleMode = mrYesToAll	+ 1 // mrYesToAll is the last predefined enum.
 ,mrDMMode
} TmrMode;

class TChooseSilentMRWModeDLG : public TForm
{
__published:
  TButton *OK;
  TButton *Cancel;
  TLabel *Label1;
  TGroupBox *GroupBox1;
  TRadioButton *SingleModeRadio;
  TRadioButton *DigitalModeRadio;
  TTimer *Timer1;
  void __fastcall OKClick(TObject *Sender);
  void __fastcall CancelClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Timer1Timer(TObject *Sender);
private:
  bool m_Abort;
  TTankIndex m_RelatesToCartridge;
public:
	virtual __fastcall TChooseSilentMRWModeDLG(TComponent* AOwner);
  void AbortDialog();
  void ConfirmDialog();
  void SetRelatedCartridge(TTankIndex Cartridge) {m_RelatesToCartridge = Cartridge;}
  bool IsRelatedDialogPending(TTankIndex Cartridge);
};
//----------------------------------------------------------------------------
extern PACKAGE TChooseSilentMRWModeDLG *ChooseSilentMRWModeDLG;
//----------------------------------------------------------------------------
#endif
