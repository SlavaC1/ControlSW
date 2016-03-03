//---------------------------------------------------------------------------

#ifndef MaintenanceWarningFormH
#define MaintenanceWarningFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TMaintenanceUserWarningForm : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TLabel *AttentionLabel;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TButton *WarningOKButton;
        TButton *WarningCancelButton;
private:	// User declarations
public:		// User declarations
        __fastcall TMaintenanceUserWarningForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMaintenanceUserWarningForm *MaintenanceUserWarningForm;
//---------------------------------------------------------------------------
#endif
