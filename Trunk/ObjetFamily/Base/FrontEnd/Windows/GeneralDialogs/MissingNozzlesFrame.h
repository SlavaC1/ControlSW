//---------------------------------------------------------------------------

#ifndef MissingNozzlesFrameH
#define MissingNozzlesFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "WizardViewer.h"
#include "GlobalDefs.h"
#include <vector>
//---------------------------------------------------------------------------
class TMissingNozzlesScaleBasedFrame : public TFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TLabel *Label1;
	TLabel *Label2;
	void __fastcall EditChange(TObject *Sender);
	void __fastcall MissingNozzlesEditKeyPress(TObject *Sender, char &Key);
	void __fastcall WeightEditKeyPress(TObject *Sender, char &Key);

private:	// User declarations
	TEdit* m_missingNozzlesEdit[TOTAL_NUMBER_OF_HEADS];
	TLabel* m_headsLbl[TOTAL_NUMBER_OF_HEADS];
	std::vector<int> v_missingNozzles;
	
public:		// User declarations
	__fastcall TMissingNozzlesScaleBasedFrame(TComponent* Owner);
	__fastcall ~TMissingNozzlesScaleBasedFrame();
	void SetMissingNozzlesArray(std::vector<int> &missingNozzlesVector);
};
//---------------------------------------------------------------------------
extern PACKAGE TMissingNozzlesScaleBasedFrame *MissingNozzlesScaleBasedFrame;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class CMissingNozzlesScaleBasedPageViewer : public CCustomWizardPageViewer
{
private:
	TMissingNozzlesScaleBasedFrame* m_missingNozzlesFrame;

    void __fastcall CheckBoxOnClick(TObject *Sender);
    
public:
	CMissingNozzlesScaleBasedPageViewer();
   ~CMissingNozzlesScaleBasedPageViewer();
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
	void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);
	void GetMissingNozzles(std::vector<int> &missingNozzlesVector);
};
class EOverMaxExist : public EQException
{
  public:
	EOverMaxExist(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};     
#endif
