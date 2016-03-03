//---------------------------------------------------------------------------

#ifndef NozzlesMapH
#define NozzlesMapH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "GenFourTesterDefs.h"
//---------------------------------------------------------------------------

const int NOZZLES_IN_COLUMN = 32;

class TNozzlesMapFrame : public TFrame
{
__published:	// IDE-managed Components
	TLabel *LeftLabel;
	TLabel *RightLabel;
private:	// User declarations

		bool m_AllState;

		bool m_ColumnsState   [NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN];
		bool m_CheckboxesState[NOZZLES_IN_GEN4_HEAD];

		TCheckBox *m_Checkboxes[NOZZLES_IN_GEN4_HEAD];
		TButton   *m_Buttons   [NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN];

		void __fastcall SelectColumnButtonClick(TObject *Sender);
		void __fastcall SetCheckboxEvent(TObject *Sender);
		
		void ColumnToNozzleRange(int &StartIndex, int &EndIndex, int Col);
		void CreateCheckboxes();
		
public:		// User declarations
	__fastcall TNozzlesMapFrame(TComponent* Owner);

	void __fastcall SetAllButtonClick(TObject *Sender);
	void SetCheckboxesInRange(int Start, int End, bool State);
	void SetAllState(bool State);
	bool* GetNozzles();
	void SetNozzles(bool *Nozzles);
};


//---------------------------------------------------------------------------
extern PACKAGE TNozzlesMapFrame *NozzlesMapFrame;
//---------------------------------------------------------------------------

#endif
