//---------------------------------------------------------------------------

#ifndef HaspPlugInfoDlgH
#define HaspPlugInfoDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <ComCtrls.hpp>
#include "QTypes.h"
//---------------------------------------------------------------------------
class THaspPlugInformationDlg : public TForm
{
__published:	// IDE-managed Components
    TButton *OKButton;
    TButton *HelpButton;
	TPageControl *PageControl1;
	TTabSheet *PlugPropertiesTab;
	TValueListEditor *HaspInfoGrid;
	TTabSheet *LicensedPackagesTab;
	TTreeView *PackagesTreeView;
	TLabel *PacksVersionLabel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OKButtonClick(TObject *Sender);
    void __fastcall HelpButtonClick(TObject *Sender);
private:	// User declarations
	
public:		// User declarations
    __fastcall THaspPlugInformationDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THaspPlugInformationDlg *HaspPlugInformationDlg;
//---------------------------------------------------------------------------
#endif
