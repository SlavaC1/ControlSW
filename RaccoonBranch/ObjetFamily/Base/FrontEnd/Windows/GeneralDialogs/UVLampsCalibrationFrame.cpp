//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UVLampsCalibrationFrame.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TUVLampsCalibrationWizardFrame::TUVLampsCalibrationWizardFrame(TComponent* Owner)
  : TFrame(Owner)
{
}
//---------------------------------------------------------------------------

void TUVLampsCalibrationWizardFrame::SetWizardInstance(CAutoWizard *Wizard)
{
  m_Wizard = Wizard;
}
