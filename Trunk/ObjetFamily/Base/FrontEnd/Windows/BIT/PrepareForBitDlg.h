//---------------------------------------------------------------------------

#ifndef PrepareForBitDlgH
#define PrepareForBitDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <vector>
#include "BackEndInterface.h"
const String TRAY_INSIDE = "Tray is inside";
const String TRAY_EMPTY = "Tray is empty";
class CBackEndInterface;
//---------------------------------------------------------------------------
class TPrepareBitDlg : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *ItemGroupBox;
	TLabel *Title;
	TButton *OK;
	TButton *CancellBttn;
	void __fastcall OKClick(TObject *Sender);
	void __fastcall CancellBttnClick(TObject *Sender);
	void __fastcall SetCheckboxEvent(TObject *Sender);
private:	// User declarations
std::vector<TCheckBox*> m_CheckBoxVector;
std::vector<String> m_attributesVector;
bool m_wasCancelled;
bool m_wasClosed;
public:		// User declarations
	__fastcall TPrepareBitDlg(TComponent* Owner);
	__fastcall virtual ~TPrepareBitDlg()
	{
      Clean();
	}
	void AddCheckBox(String checkboxName,int index);
	void AddAttribute(String attribute);
    bool RemoveAttribute(String attribute);
	void  UpdateView(bool needToCreateChBx);
	void Clean();
	bool NeedToShow();
	void CloseDialog();
	bool WasCancelled();
	bool WasClosed();
	void SelectCheckBox(String checkboxName, bool check);
	void EnableCheckBox(String checkboxName, bool enable);
	bool IsAttributeExists(String attribute);
        bool IsTrayEmptyIsOnlyAttribute();
};
//---------------------------------------------------------------------------
extern PACKAGE TPrepareBitDlg *PrepareBitDlg;
//---------------------------------------------------------------------------
#endif
