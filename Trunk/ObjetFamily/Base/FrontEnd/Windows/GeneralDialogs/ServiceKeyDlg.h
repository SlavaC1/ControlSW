//---------------------------------------------------------------------------

#ifndef ServiceKeyDlgH
#define ServiceKeyDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TServiceKey : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblMaintenanceMode;
	TLabel *lblServiceKey;
private:	// User declarations
public:		// User declarations
	__fastcall TServiceKey(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TServiceKey *ServiceKey;
//---------------------------------------------------------------------------
#endif
