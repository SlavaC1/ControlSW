//---------------------------------------------------------------------------


#ifndef EdenInfoFrameH
#define EdenInfoFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TInfoFrame : public TFrame
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TLabel *Label1;
  TPanel *OCB_SW_VersionPanel;
  TButton *OCB_SW_VersionButton;
  TButton *OCB_HW_VersionButton;
  TPanel *OCB_HW_VersionPanel;
  TLabel *Label2;
  TLabel *Label3;
  TPanel *OHDB_SW_VersionPanel;
  TButton *OHDB_SW_VersionButton;
  TButton *OHDB_HW_VersionButton;
  TPanel *OHDB_HW_VersionPane;
  TLabel *Label4;
  TLabel *Label5;
  TPanel *Panel1;
  TButton *Button1;
  TLabel *Label6;
  TPanel *Panel2;
  TButton *Button2;
private:	// User declarations
public:		// User declarations
  __fastcall TInfoFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TInfoFrame *InfoFrame;
//---------------------------------------------------------------------------
#endif
