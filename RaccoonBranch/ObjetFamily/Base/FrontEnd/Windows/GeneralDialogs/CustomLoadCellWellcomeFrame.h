//---------------------------------------------------------------------------

#ifndef CustomLoadCellWellcomeFrameH
#define CustomLoadCellWellcomeFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "WizardImages.h"
#include "FEResources.h"
#include "CustomLoadCellWellcomePage.h"
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TLoadCellWellcomeFrame : public TFrame
{
__published:	// IDE-managed Components
	TScrollBox *ScrollBox1;
	TImage *Image1;
private:	// User declarations
public:		// User declarations
	__fastcall TLoadCellWellcomeFrame(TComponent* Owner);
};

class CLoadCellWellcomePageViewer : public CCustomWizardPageViewer {
  private:
    TLoadCellWellcomeFrame *m_loadCellWellcomeFrame;

  public:
	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoadCellWellcomeFrame *LoadCellWellcomeFrame;
//---------------------------------------------------------------------------
#endif
