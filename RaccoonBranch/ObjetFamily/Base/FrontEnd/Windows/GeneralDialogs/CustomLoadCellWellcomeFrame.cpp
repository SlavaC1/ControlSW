//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CustomLoadCellWellcomeFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoadCellWellcomeFrame *LoadCellWellcomeFrame;
//---------------------------------------------------------------------------
__fastcall TLoadCellWellcomeFrame::TLoadCellWellcomeFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void CLoadCellWellcomePageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_loadCellWellcomeFrame = new TLoadCellWellcomeFrame(PageWindow);
  m_loadCellWellcomeFrame->Parent = PageWindow;
  CLoadCellWellcomeWizardPage* Page = dynamic_cast<CLoadCellWellcomeWizardPage *>(WizardPage);
  Graphics::TBitmap *TmpImage = new Graphics::TBitmap;
  TmpImage->Transparent = true;
  LOAD_BITMAP(TmpImage,Page->m_imageIndex);
  m_loadCellWellcomeFrame->Image1->Transparent = true;
  m_loadCellWellcomeFrame->Image1->Picture->Assign(TmpImage);
  Page->Refresh();
}

