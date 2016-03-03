
#ifndef WeightsStatusFrameH
#define WeightsStatusFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "WeightsStatusWizardPage.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "AppParams.h"
#include <ComCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class TWeightsStatusFrame : public TFrame
{
__published:	// IDE-managed Components
	TCheckBox *ValuesStableCheckBox;
	TGroupBox *WeightsStatusGroupBox;
	TLabel *SubTitle;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	//---------------------------------------------------------------------------

private:	// User declarations
public:		// User declarations
	TPanel*    m_containersPanel[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE];

	__fastcall TWeightsStatusFrame(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWeightsStatusFrame *WeightsStatusFrame;
//---------------------------------------------------------------------------

class CWeightsStatusPageViewer : public CCustomWizardPageViewer
{
private:
	TWeightsStatusFrame *m_WeightsStatusFrame;

	void __fastcall CheckBoxOnClick(TObject *Sender)
	{
		TCheckBox *MyCheckBox = dynamic_cast<TCheckBox *>(Sender);

		// Update only if the origin is Checkbox
		if (MyCheckBox)
		{
			GetParentWizard()->EnableDisableNext(MyCheckBox->Checked ? true : false);
			GetActivePage()->NotifyEvent(0, 0);
		}
	};


public:
	void Prepare(TWinControl *PageWindow, CWizardPage *WizardPage)
	{
		m_WeightsStatusFrame         = new TWeightsStatusFrame(PageWindow);
		m_WeightsStatusFrame->Parent = PageWindow;

		m_WeightsStatusFrame->ValuesStableCheckBox->OnClick = CheckBoxOnClick;

		if(! CAppParams::Instance()->DualWasteEnabled)
		{
			//m_WeightsStatusFrame->m_containersPanel[TYPE_TANK_WASTE_LEFT]->Left     = 45/*120*/;
			m_WeightsStatusFrame->m_containersPanel[TYPE_TANK_WASTE_RIGHT]->Visible = false;
			//m_WeightsStatusFrame->m_containersPanel[TYPE_TANK_WASTE_LEFT]->Caption  = "Waste";
		}
	}

	void Refresh(TWinControl *PageWindow, CWizardPage *WizardPage)
	{

		CWeightsStatusWizardPage *Page = dynamic_cast<CWeightsStatusWizardPage *>(WizardPage);

		if(m_WeightsStatusFrame == NULL)
			return;

		m_WeightsStatusFrame->ValuesStableCheckBox->Caption = "Values are stable";

		for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
		{
		   m_WeightsStatusFrame->m_containersPanel[i]->Caption = Page->m_WeightsStatusWizardPageData[i]->m_CurrValue;
		   if(Page->m_WeightsStatusWizardPageData[i]->m_enable)
			 m_WeightsStatusFrame->m_containersPanel[i]->Font->Color = clGray;
		   else
			 m_WeightsStatusFrame->m_containersPanel[i]->Font->Color = clWindowText;
        }

		/*for(int i = 0; i < m_WeightsStatusFrame->WeightsStatusGroupBox->ControlCount; i++)
		{
			TPanel *Panel = dynamic_cast<TPanel*>(m_WeightsStatusFrame->WeightsStatusGroupBox->Controls[i]);

			if(Panel)
			{
				Panel->Caption = Page->m_WeightsStatusWizardPageData[Panel->Tag]->m_CurrValue;
				if(Page->m_WeightsStatusWizardPageData[Panel->Tag]->m_enable)
					Panel->Font->Color = clGray;
				else
					Panel->Font->Color = clWindowText;
            }
        }*/
	}

	void Leave(TWinControl *PageWindow, CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
	{
		Q_SAFE_DELETE(m_WeightsStatusFrame);
	}
};

//---------------------------------------------------------------------------
#endif
