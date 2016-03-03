//---------------------------------------------------------------------------


#ifndef UserValuesEntryWizardFrameH
#define UserValuesEntryWizardFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CustomizeDlg.hpp>
#include <ExtCtrls.hpp>
#include "UserValuesEntryPage.h"
#include "WizardViewer.h"
#include "GlobalDefs.h"

//---------------------------------------------------------------------------



class TUserValuesEntryPageFrame : public TFrame
{
__published:	// IDE-managed Components
        TLabel *SubTitleLabel;
        void __fastcall EditExit(TObject *Sender);
private:	// User declarations
      bool CheckString(QString Str);
      QString m_Data[TOTAL_NUMBER_OF_HEADS];
      template<class T> bool TypedStringChecker(QString Str);

public:		// User declarations
       TUserValuesType UserValuesType;
       TLabeledEdit* LabeledEditArray[TOTAL_NUMBER_OF_HEADS];
       
        __fastcall TUserValuesEntryPageFrame(TComponent* Owner);
        
        void Clear           (void);
        void SetEditLabels   (QString *Labels);
        void SetLabelsColor  (int* Colors);
        void GetUserEntryData(QString *Data);
        void SetData         (QString *Data);

};
//---------------------------------------------------------------------------


class CUserEntryWizardPageViewer : public CCustomWizardPageViewer {
  private:
    TUserValuesEntryPageFrame *m_UserValuesEntryPageFrame;

  public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage) {
      CUserValuesEntryWizardPage* Page = dynamic_cast<CUserValuesEntryWizardPage *>(WizardPage);
      m_UserValuesEntryPageFrame = new TUserValuesEntryPageFrame(PageWindow);
      m_UserValuesEntryPageFrame->Parent = PageWindow;
      m_UserValuesEntryPageFrame->UserValuesType = Page->ValuesType;
      Refresh(PageWindow,WizardPage);
    }

    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage) {
      CUserValuesEntryWizardPage *UserValuesEntryPage = dynamic_cast<CUserValuesEntryWizardPage *>(WizardPage);
      m_UserValuesEntryPageFrame->SubTitleLabel->Caption = UserValuesEntryPage->SubTitle.c_str();
      m_UserValuesEntryPageFrame->SetEditLabels(UserValuesEntryPage->UserEntryDescriptions);
      m_UserValuesEntryPageFrame->SetData(UserValuesEntryPage->Data);
      m_UserValuesEntryPageFrame->SetLabelsColor(UserValuesEntryPage->LabelsColor);
    }

    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason){
      CUserValuesEntryWizardPage* Page = dynamic_cast<CUserValuesEntryWizardPage *>(WizardPage);
      m_UserValuesEntryPageFrame->GetUserEntryData(Page->Data);
    }

};


#endif






