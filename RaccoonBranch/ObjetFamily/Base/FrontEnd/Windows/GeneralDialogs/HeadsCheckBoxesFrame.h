//---------------------------------------------------------------------------


#ifndef HeadsCheckBoxesFrameH
#define HeadsCheckBoxesFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "GlobalDefs.h"
#include "HeadsCheckBoxesWizardPage.h"

//---------------------------------------------------------------------------
class THeadsCheckBoxFrame : public TFrame
{
__published:	// IDE-managed Components
        TLabel *SubTitleLabel;
        TGroupBox *ChooseHeadsGroupBox;
private:	// User declarations
public:		// User declarations

        __fastcall THeadsCheckBoxFrame(TComponent* Owner);
        TCheckBox* CheckBoxArray[TOTAL_NUMBER_OF_HEADS];
        TLabel*    LabelArray   [TOTAL_NUMBER_OF_HEADS];
  // Setting the checkboxes according to the input
  void SetCheckedBoxes (bool CheckedArray[]);
  void GetCheckedBoxes (bool OutputArray[]);
};
//---------------------------------------------------------------------------
extern PACKAGE THeadsCheckBoxFrame* HeadsCheckBoxFrame;
//---------------------------------------------------------------------------

class CHeadsCheckBoxesPageViewer : public CCustomWizardPageViewer
{
private:
    THeadsCheckBoxFrame* m_HeadsCheckBoxFrame;
    int m_NoOfCheckedCheckboxes;

    void __fastcall CheckBoxOnClick(TObject *Sender);
    
public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
};

#endif
