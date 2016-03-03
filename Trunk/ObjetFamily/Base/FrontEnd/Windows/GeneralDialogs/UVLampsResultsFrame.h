//---------------------------------------------------------------------------


#ifndef UVLampsResultsFrameH
#define UVLampsResultsFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "UVLampsResultsWizardPage.h"
#include "PythonWizardPages.h"
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "FEResources.h"


const int RESULTS_OK_BMP_INDEX     = 0;
const int RESULTS_CANCEL_BMP_INDEX = 1;

/*
const int HS_R_RESULT = 0;
const int HS_L_RESULT = 1;
const int HQ_R_RESULT = 2;
const int HQ_L_RESULT = 3;
const int HS_R_LABEL = 4;
const int HS_L_LABEL = 5;
const int HQ_R_LABEL = 6;
const int HQ_L_LABEL = 7;
*/

#define LAMP_RESULT_OK "OK"

//---------------------------------------------------------------------------
class TUVLampResultsFrame : public TFrame
{
__published:	// IDE-managed Components
        TImageList *ImageList1;
        TPanel *Panel1;
	TImage *HQ_LeftLampImage;
	TImage *HQ_RightLampImage;
				TLabel *HQ_LeftUVResultsLabel;
				TLabel *HQ_RightUVResultsLabel;
				TLabel *RightUVLampLabel;
				TLabel *LeftUVLampLabel;
	TImage *HS_LeftLampImage;
	TImage *HS_RightLampImage;
        TLabel *HS_LeftUVResultsLabel;
        TLabel *HS_RightUVResultsLabel;
	TLabel *lblM1;
	TLabel *lblM2;
        TBevel *TopHorizontalBevel;
        TBevel *DownHorizontalBevel;
        TBevel *VerticalBevel;
	TBevel *bvl2;
	TLabel *lbl1;
	TImage *img1;
	TLabel *lbl2;
	TImage *img2;
	TLabel *lblM3;
	TBevel *bvl3;
private:	// User declarations
public:		// User declarations
        __fastcall TUVLampResultsFrame(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TUVLampResultsFrame *UVLampResultsFrame;
//---------------------------------------------------------------------------

class CUVLampsResultsPageViewer : public CCustomWizardPageViewer {
	private:
		TUVLampResultsFrame *m_UVLampResultsFrame;

		void UpdateUVLampsResults(CUVLampsResultsWizardPage *Page)
		{
/*			m_UVLampResultsFrame->HQ_RightLampImage->Picture = NULL;
			m_UVLampResultsFrame->HS_RightLampImage->Picture = NULL;
			m_UVLampResultsFrame->HQ_LeftLampImage->Picture  = NULL;
			m_UVLampResultsFrame->HS_LeftLampImage->Picture  = NULL;

			// Updating the BMPs

			Graphics::TBitmap *pBitmap;

			// Right Lamp
			if(Page->HQ_RightLampResults == true)
				//m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_OK_BMP_INDEX, m_UVLampResultsFrame->HQ_RightLampImage->Picture->Bitmap);
				LOAD_BITMAP(m_UVLampResultsFrame->HQ_RightLampImage->Picture->Bitmap, IDB_V);
			else
				//m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_CANCEL_BMP_INDEX, m_UVLampResultsFrame->HQ_RightLampImage->Picture->Bitmap);
				LOAD_BITMAP(m_UVLampResultsFrame->HQ_RightLampImage->Picture->Bitmap, IDB_X);

			m_UVLampResultsFrame->HQ_RightLampImage->Picture->Bitmap->TransparentColor = clOlive;
			m_UVLampResultsFrame->HQ_RightLampImage->Picture->Bitmap->Transparent = true;

//			Form1->Canvas->Draw(0,0,pBitmap);

			if(Page->HS_RightLampResults == true)
				m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_OK_BMP_INDEX, m_UVLampResultsFrame->HS_RightLampImage->Picture->Bitmap);
			else
				m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_CANCEL_BMP_INDEX, m_UVLampResultsFrame->HS_RightLampImage->Picture->Bitmap);

			m_UVLampResultsFrame->HS_RightLampImage->Picture->Bitmap->TransparentColor = clOlive;
			m_UVLampResultsFrame->HS_RightLampImage->Picture->Bitmap->Transparent = true;

			// Left Lamp
			if(Page->HQ_LeftLampResults == true)
				m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_OK_BMP_INDEX, m_UVLampResultsFrame->HQ_LeftLampImage->Picture->Bitmap);
			else
				m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_CANCEL_BMP_INDEX, m_UVLampResultsFrame->HQ_LeftLampImage->Picture->Bitmap);

			m_UVLampResultsFrame->HQ_LeftLampImage->Picture->Bitmap->TransparentColor = clOlive;
			m_UVLampResultsFrame->HQ_LeftLampImage->Picture->Bitmap->Transparent = true;

			if(Page->HS_LeftLampResults == true)
				m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_OK_BMP_INDEX, m_UVLampResultsFrame->HS_LeftLampImage->Picture->Bitmap);
			else
				m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_CANCEL_BMP_INDEX, m_UVLampResultsFrame->HS_LeftLampImage->Picture->Bitmap);

			m_UVLampResultsFrame->HS_LeftLampImage->Picture->Bitmap->TransparentColor = clOlive;
			m_UVLampResultsFrame->HS_LeftLampImage->Picture->Bitmap->Transparent = true;

			// Updating the Labels
			m_UVLampResultsFrame->HQ_RightUVResultsLabel->Caption = Page->HQ_RightUVResultsLabel.c_str();
			m_UVLampResultsFrame->HS_RightUVResultsLabel->Caption = Page->HS_RightUVResultsLabel.c_str();
			m_UVLampResultsFrame->HQ_LeftUVResultsLabel->Caption  = Page->HQ_LeftUVResultsLabel.c_str();
			m_UVLampResultsFrame->HS_LeftUVResultsLabel->Caption  = Page->HS_LeftUVResultsLabel.c_str();
*/
		}

		void UpdateUVLampsResultsPy(CPythonGenericCustomWizardPage *Page)
		{
			// Updating the Labels
			for( int i = 0; i < this->m_UVLampResultsFrame->Panel1->ControlCount; i++)
			{
				TLabel *label = dynamic_cast<TLabel *>(this->m_UVLampResultsFrame->Panel1->Controls[i]);
				if(label)
				{
					if( label->Tag != -1 )
					{
						label->Caption = Page->GetArg( label->Tag ).c_str();
					}
				}

				TImage *image = dynamic_cast<TImage *>(this->m_UVLampResultsFrame->Panel1->Controls[i]);
				if(image)
				{
					if( image->Tag != -1 )
					{
						image->Transparent = true;
						image->Picture->Bitmap = NULL;

						if( Page->GetArg( image->Tag ) == LAMP_RESULT_OK )
							m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_OK_BMP_INDEX, image->Picture->Bitmap);
						else
							m_UVLampResultsFrame->ImageList1->GetBitmap(RESULTS_CANCEL_BMP_INDEX, image->Picture->Bitmap);

					}
				}
			}
	}

  public:


	void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      m_UVLampResultsFrame         = new TUVLampResultsFrame(PageWindow);
      m_UVLampResultsFrame->Parent = PageWindow;

      CUVLampsResultsWizardPage *Page = dynamic_cast<CUVLampsResultsWizardPage *>(WizardPage);
      CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);

      if( Page != NULL ) {
        UpdateUVLampsResults(Page);
      }
      else {
        UpdateUVLampsResultsPy(PythonPage);
      }
    }

    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
    {
      CUVLampsResultsWizardPage *Page = dynamic_cast<CUVLampsResultsWizardPage *>(WizardPage);
      CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);
      
      if( Page != NULL ) {
        UpdateUVLampsResults(Page);
      }
      else {
        UpdateUVLampsResultsPy(PythonPage);
      }
    }

    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
    {
      if (m_UVLampResultsFrame)
      {
        delete m_UVLampResultsFrame;
        m_UVLampResultsFrame = NULL;
      }
    }
};

#endif
