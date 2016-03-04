//---------------------------------------------------------------------------


#ifndef UVLampsCalibrationFrameH
#define UVLampsCalibrationFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "UVLampsCalibrationWizardPage.h"
#include "PythonWizardPages.h"
#include <ExtCtrls.hpp>
#include "FEResources.h"
#include "AutoWizard.h"
#include <ComCtrls.hpp>

#define LAMP_STAB_PROGRESS  	0
#define MEASURE_UV_PROGRESS 	1
#define CURRENT_LAMP_STRING 	2
#define CURRENT_MODE_STRING 	3
#define DURING_LAMP_STAB    	4
#define DURING_MEASURE_UV   	5
#define LAMP_STAB_TIME      	6
#define MEASURE_UV_CYCLES   	7
#define OVERALL_PROGRESS    	8
#define LAMP_OPERATION_STRING   9

//const int wptUVLampsCalibrationWizardPage = wptCustom + 10;

//---------------------------------------------------------------------------
class TUVLampsCalibrationWizardFrame : public TFrame
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *CurrentLampLabel;
	TLabel *CurrentModeLabel;
	TLabel *Label4;
	TGroupBox *LampStabPanel;
	TProgressBar *LampStabProgressBar;
	TLabel *StabTimeLabel;
	TGroupBox *MeasureUVPanel;
	TLabel *CyclesNumLabel;
	TProgressBar *MeasureUVProgressBar;
	TGroupBox *GroupBox3;
	TProgressBar *OverallProgressBar;
private:	// User declarations

  CAutoWizard *m_Wizard;

public:		// User declarations
  __fastcall TUVLampsCalibrationWizardFrame(TComponent* Owner);

  void SetWizardInstance(CAutoWizard *Wizard);

};
//---------------------------------------------------------------------------
extern PACKAGE TUVLampsCalibrationWizardFrame *UVLampsCalibrationWizardFrame;
//---------------------------------------------------------------------------

class CUVLampsCalibrationWizardViewer : public CCustomWizardPageViewer {
  private:
    TUVLampsCalibrationWizardFrame *m_UVLampsCalibrationWizardFrame;

  public:
	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage) {
      m_UVLampsCalibrationWizardFrame = new TUVLampsCalibrationWizardFrame(PageWindow);
      m_UVLampsCalibrationWizardFrame->Parent = PageWindow;

      CUVLampsCalibrationWizardPage *Page = dynamic_cast<CUVLampsCalibrationWizardPage *>(WizardPage);
      CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);

      m_UVLampsCalibrationWizardFrame->SetWizardInstance(dynamic_cast<CAutoWizard *>(GetParentWizard()));
	  
      if( Page != NULL )
        UpdateUVLampsCalibration(Page);
      if( PythonPage != NULL )
        UpdateUVLampsCalibrationPy(PythonPage);
    }

	void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage) {
      CUVLampsCalibrationWizardPage *Page = dynamic_cast<CUVLampsCalibrationWizardPage *>(WizardPage);
      CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);
      
      if( Page != NULL )
        UpdateUVLampsCalibration(Page);
      if( PythonPage != NULL )
        UpdateUVLampsCalibrationPy(PythonPage);
    }
	
	void UpdateUVLampsCalibration(CUVLampsCalibrationWizardPage *Page)
	{
		m_UVLampsCalibrationWizardFrame->CurrentLampLabel->Caption = Page->CurrentLampString.c_str();
		m_UVLampsCalibrationWizardFrame->CurrentModeLabel->Caption = Page->CurrentModeString.c_str();
		if( Page->DuringLampStab ) {
			m_UVLampsCalibrationWizardFrame->LampStabProgressBar->Position = Page->LampStabProgress;
			m_UVLampsCalibrationWizardFrame->LampStabPanel->Font->Style = TFontStyles() << fsBold;
			m_UVLampsCalibrationWizardFrame->StabTimeLabel->Caption = Page->LampStabTime.c_str();
			m_UVLampsCalibrationWizardFrame->StabTimeLabel->Visible = true;
		}
		else {
			m_UVLampsCalibrationWizardFrame->LampStabProgressBar->Position = 0;
			m_UVLampsCalibrationWizardFrame->LampStabPanel->Font->Style = TFontStyles();
			m_UVLampsCalibrationWizardFrame->StabTimeLabel->Visible = false;
		}

		if( Page->DuringMeasureUV ) {
			m_UVLampsCalibrationWizardFrame->MeasureUVProgressBar->Position = Page->MeasureUVProgress;			
			m_UVLampsCalibrationWizardFrame->MeasureUVPanel->Font->Style = TFontStyles() << fsBold;
			m_UVLampsCalibrationWizardFrame->CyclesNumLabel->Caption = Page->MeasureUVCycles.c_str();
			m_UVLampsCalibrationWizardFrame->CyclesNumLabel->Visible = true;
		}
		else {
			m_UVLampsCalibrationWizardFrame->MeasureUVProgressBar->Position = 0;
			m_UVLampsCalibrationWizardFrame->MeasureUVPanel->Font->Style = TFontStyles();
			m_UVLampsCalibrationWizardFrame->CyclesNumLabel->Visible = false;
		}
		m_UVLampsCalibrationWizardFrame->OverallProgressBar->Position = Page->OverallProgress;
	}

	void UpdateUVLampsCalibrationPy(CPythonGenericCustomWizardPage *Page)
	{
		m_UVLampsCalibrationWizardFrame->CurrentLampLabel->Caption = Page->GetArg( CURRENT_LAMP_STRING ).c_str();
		m_UVLampsCalibrationWizardFrame->CurrentModeLabel->Caption = Page->GetArg( CURRENT_MODE_STRING ).c_str();
		if( "TRUE" == Page->GetArg( DURING_LAMP_STAB ) ) {
			m_UVLampsCalibrationWizardFrame->LampStabProgressBar->Position = atoi(Page->GetArg( LAMP_STAB_PROGRESS ).c_str());
			m_UVLampsCalibrationWizardFrame->LampStabPanel->Font->Style = TFontStyles() << fsBold;
			m_UVLampsCalibrationWizardFrame->LampStabPanel->Caption = Page->GetArg(LAMP_OPERATION_STRING).c_str();
			m_UVLampsCalibrationWizardFrame->StabTimeLabel->Caption = Page->GetArg( LAMP_STAB_TIME ).c_str();
			m_UVLampsCalibrationWizardFrame->StabTimeLabel->Visible = true;
		}
		else {
			m_UVLampsCalibrationWizardFrame->LampStabProgressBar->Position = 0;
			m_UVLampsCalibrationWizardFrame->LampStabPanel->Font->Style = TFontStyles();
			m_UVLampsCalibrationWizardFrame->StabTimeLabel->Visible = false;
		}

		if( "TRUE" == Page->GetArg( DURING_MEASURE_UV ) ) {
			m_UVLampsCalibrationWizardFrame->MeasureUVProgressBar->Position = atoi(Page->GetArg( MEASURE_UV_PROGRESS ).c_str());
			m_UVLampsCalibrationWizardFrame->MeasureUVPanel->Font->Style = TFontStyles() << fsBold;
			m_UVLampsCalibrationWizardFrame->CyclesNumLabel->Caption = Page->GetArg( MEASURE_UV_CYCLES ).c_str();
			m_UVLampsCalibrationWizardFrame->CyclesNumLabel->Visible = true;
		}
		else {
			m_UVLampsCalibrationWizardFrame->MeasureUVProgressBar->Position = 0;
			m_UVLampsCalibrationWizardFrame->MeasureUVPanel->Font->Style = TFontStyles();
			m_UVLampsCalibrationWizardFrame->CyclesNumLabel->Visible = false;
		}

		m_UVLampsCalibrationWizardFrame->OverallProgressBar->Position = atoi( Page->GetArg( OVERALL_PROGRESS ).c_str() );
	}
		
	void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
    {
	  if (m_UVLampsCalibrationWizardFrame)
	  {
		delete m_UVLampsCalibrationWizardFrame;
        m_UVLampsCalibrationWizardFrame = NULL;
      }
    }
};

//---------------------------------------------------------------------------
#endif
