//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UVLampsStatusFrame.h"
#include "FEResources.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TUVLampStatusFrame *UVLampStatusFrame;

void CUVLampsStatusPageViewer::UpdateUVLampsStatus(CUVLampsStatusWizardPage *Page)
{
   FOR_ALL_UV_LAMPS(l)
     {
         FOR_ALL_QUALITY_MODES(qm)
         {
			FOR_ALL_OPERATION_MODES(om)
             {
                 if(GetModeAccessibility(qm, om) == false)
                    continue;
                m_UVLampStatusFrame->m_ModeLabelArray[qm][om]->Caption = (GetModeStr(qm, om)).c_str();

                // Updating the BMPs
                LOAD_BITMAP(m_UVLampStatusFrame->m_ImagesArray[l][qm][om]->Picture->Bitmap, ((Page->LampStatus[l][qm][om] == true) ? IDB_OK : IDB_CANCEL));

                // Updating the Labels
                m_UVLampStatusFrame->m_UVStatusLabelArray[l][qm][om]->Caption = Page->UVStatusLabel[l][qm][om].c_str();
             }
         }
     }
}

void CUVLampsStatusPageViewer::UpdateUVLampsStatusPy(CPythonGenericCustomWizardPage *Page)
{

   FOR_ALL_UV_LAMPS(l)
   {
       FOR_ALL_QUALITY_MODES(qm)
       {
			FOR_ALL_OPERATION_MODES(om)
           {
               if(GetModeAccessibility(qm, om) == false)
                  continue;
              m_UVLampStatusFrame->m_ModeLabelArray[qm][om]->Caption = (GetModeStr(qm, om)).c_str();

              // Updating the BMPs
							LOAD_BITMAP(m_UVLampStatusFrame->m_ImagesArray[l][qm][om]->Picture->Bitmap, (( Page->GetArg( (qm * NUMBER_OF_QUALITY_MODES) + ( om * NUM_OF_OPERATION_MODES * NUMBER_OF_QUALITY_MODES) + l )/*Page->LampStatus[l][qm][om]*/ == LAMP_STATUS_OK) ? IDB_OK : IDB_CANCEL));

              // Updating the Labels
              m_UVLampStatusFrame->m_UVStatusLabelArray[l][qm][om]->Caption = Page->GetArg( (qm * NUMBER_OF_QUALITY_MODES) + ( om * NUM_OF_OPERATION_MODES * NUMBER_OF_QUALITY_MODES) + l + 6 ).c_str();//Page->UVStatusLabel[l][qm][om].c_str();
           }
       }
   }

}


void CUVLampsStatusPageViewer::Prepare(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  m_UVLampStatusFrame         = new TUVLampStatusFrame(PageWindow);
  m_UVLampStatusFrame->Parent = PageWindow;

  CUVLampsStatusWizardPage *Page = dynamic_cast<CUVLampsStatusWizardPage *>(WizardPage);
  CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);

  if( Page != NULL ) {
    UpdateUVLampsStatus(Page);
  }
  else {
    UpdateUVLampsStatusPy(PythonPage);
  }
}

void CUVLampsStatusPageViewer::Refresh(TWinControl *PageWindow,CWizardPage *WizardPage)
{
  CUVLampsStatusWizardPage *Page = dynamic_cast<CUVLampsStatusWizardPage *>(WizardPage);
  CPythonGenericCustomWizardPage *PythonPage = dynamic_cast<CPythonGenericCustomWizardPage *>(WizardPage);

  if( Page != NULL ) {
    UpdateUVLampsStatus(Page);
  }
  else {
    UpdateUVLampsStatusPy(PythonPage);
  }
}

void CUVLampsStatusPageViewer::Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason)
{
  if (m_UVLampStatusFrame)
  {
    delete m_UVLampStatusFrame;
    m_UVLampStatusFrame = NULL;
  }
}

//---------------------------------------------------------------------------
__fastcall TUVLampStatusFrame::TUVLampStatusFrame(TComponent* Owner)
        : TFrame(Owner)
{
   int Top             = 55;
   int TopGap          = 50;
   int TopGapCount     = 0;
   int Nibble          = 10;
   int LableWidth      = 120;
   int LableHeight     = 16;
   int ImageRibSize    = 20;

   TWinControl* Parent = TopHorizontalBevel->Parent;
   
   TopHorizontalBevel->Left  = Nibble;
   TopHorizontalBevel->Width = LableWidth*(NUMBER_OF_UV_LAMPS+1) + Nibble;

  	FOR_ALL_OPERATION_MODES(om)
       FOR_ALL_QUALITY_MODES(qm)
       {
           if(GetModeAccessibility(qm, om) == false)
              continue;

          m_ModeLabelArray[qm][om]                = new TLabel(this);
          m_ModeLabelArray[qm][om]->Parent        = Parent;
          m_ModeLabelArray[qm][om]->AutoSize      = false;
          m_ModeLabelArray[qm][om]->Left          = 20/*TopHorizontalBevel->Left*/;
          m_ModeLabelArray[qm][om]->Top           = Top + TopGapCount*TopGap;
          m_ModeLabelArray[qm][om]->Height        = LableHeight;
          m_ModeLabelArray[qm][om]->Transparent   = false;
          m_ModeLabelArray[qm][om]->Alignment     = taCenter;
          m_ModeLabelArray[qm][om]->AutoSize      = true;
          m_ModeLabelArray[qm][om]->Width         = LableWidth;
          TopGapCount++;
      }

   VerticalBevel->Left   = LableWidth + Nibble - 20;
   VerticalBevel->Top    = 25;
   VerticalBevel->Height = Top + TopGapCount*TopGap + Nibble;

   Height = VerticalBevel->Top + VerticalBevel->Height + Nibble;
   Width  = TopHorizontalBevel->Left + TopHorizontalBevel->Width + Nibble;

   FOR_ALL_UV_LAMPS(l)
   {
       m_UVLampLabelArray[l]              = new TLabel(this);
       m_UVLampLabelArray[l]->Parent      = Parent;
       m_UVLampLabelArray[l]->AutoSize    = false;
       m_UVLampLabelArray[l]->Left        = VerticalBevel->Left + Nibble + (NUMBER_OF_UV_LAMPS-l-1)*LableWidth;
       m_UVLampLabelArray[l]->Top         = VerticalBevel->Top;
       m_UVLampLabelArray[l]->Height      = LableHeight;
       m_UVLampLabelArray[l]->Width       = LableWidth;
       m_UVLampLabelArray[l]->Transparent = true;
       m_UVLampLabelArray[l]->Alignment   = taCenter;
       m_UVLampLabelArray[l]->Caption     = GetUVLampStr(l).c_str();


       FOR_ALL_QUALITY_MODES(qm)
       {
    	FOR_ALL_OPERATION_MODES(om)
           {
               if(GetModeAccessibility(qm, om) == false)
                  continue;
                  
               m_ImagesArray[l][qm][om]                    = new TImage(this);
               m_ImagesArray[l][qm][om]->Parent            = Parent;
               m_ImagesArray[l][qm][om]->Top               = m_ModeLabelArray[qm][om]->Top;
               m_ImagesArray[l][qm][om]->Height            = ImageRibSize;
               m_ImagesArray[l][qm][om]->Width             = ImageRibSize;
							 m_ImagesArray[l][qm][om]->Left              = m_UVLampLabelArray[l]->Left + m_UVLampLabelArray[l]->Width/2 - m_ImagesArray[l][qm][om]->Width/2;
               m_ImagesArray[l][qm][om]->Transparent       = true;

               m_UVStatusLabelArray[l][qm][om]              = new TLabel(this);
               m_UVStatusLabelArray[l][qm][om]->Parent      = Parent;
               m_UVStatusLabelArray[l][qm][om]->AutoSize    = true;
               m_UVStatusLabelArray[l][qm][om]->Left        = m_UVLampLabelArray[l]->Left;
               m_UVStatusLabelArray[l][qm][om]->Top         = m_ImagesArray[l][qm][om]->Top + m_ImagesArray[l][qm][om]->Height;
               m_UVStatusLabelArray[l][qm][om]->Height      = LableHeight*2;
               m_UVStatusLabelArray[l][qm][om]->Width       = m_UVLampLabelArray[l]->Width;
               m_UVStatusLabelArray[l][qm][om]->Alignment   = taCenter;
           }
       }
   }
   TopHorizontalBevel->Top = m_UVLampLabelArray[0]->Top + m_UVLampLabelArray[0]->Height;
}
//---------------------------------------------------------------------------



