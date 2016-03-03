//---------------------------------------------------------------------------


#ifndef HeadsButtonsEntryFrameH
#define HeadsButtonsEntryFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Buttons.hpp>
#include "WizardViewer.h"
#include "AppParams.h"
#include "HeatersDefs.h"
#include "Errorhandler.h"
#include <map>
#include <Buttons.hpp>
#include "MultiGridForm.h"

class CHeadsButtonsEntryFrameWizardPageViewer;

//---------------------------------------------------------------------------
class THeadsButtonsEntryFrame : public TFrame
{
  friend CHeadsButtonsEntryFrameWizardPageViewer;
  
__published:	// IDE-managed Components
    TPanel *Panel1;
    TButton *ApplyButton;
    TGroupBox *EnterGroupBox;
    TGroupBox *TrayMapGroupBox;
    void __fastcall MissingNozzlesEditKeyPress(TObject *Sender, char &Key);
    void __fastcall WeightEditKeyPress(TObject *Sender, char &Key);
    void __fastcall MainMissingNozzlesEditExit(TObject *Sender);
    void __fastcall MissingNozzlesEditExit(TObject *Sender);
    void __fastcall WeightEditExit(TObject *Sender);
    void __fastcall ButtonClick(TObject *Sender);
    
private:	// User declarations

    float*** Weights;
    int*** MissingNozzles;

    TMultiGridForm* m_EditWTValuesDlg;
    TLabel* m_NameLabels[TOTAL_NUMBER_OF_HEADS];
    TSpeedButton* m_Buttons[TOTAL_NUMBER_OF_HEADS][NUMBER_OF_VOLTAGE_MODES][NUMBER_OF_QUALITY_MODES];
    TMultiGridEdit** InputEdits;
    TLabel** InputLabels;

    bool*** MarkHeadsToBeReplaced;
    int &StepsNum, &QualityIndex, &VoltageIndex, &HeadIndex;
    bool* HeadsToBeSkipped;
    
    int MissingNozzlesStrToInt();
    void ButtonClear(TSpeedButton* Button);
    void ButtonMark(TSpeedButton* Button);
    void ButtonMarkSkipped(TSpeedButton* Button);
    static void EnterEditMode(void* obj);
    void CopyToGrid(void);
    void CopyFromGrid(void);
    bool __fastcall MissingNozzlesEditExitEvent(TObject *Sender/*, bool Silent=false*/);
    bool __fastcall WeightEditExitEvent(TObject *Sender, float HighVoltageWeight, float LowVoltageWeight);
    bool __fastcall MainWeightEditExit(TObject *Sender);

    void DoMissingNozzlesSanityCheck(TMultiGridEdit* Edit);
    void DoWeightSanityCheck(TMultiGridEdit* Edit, float HighVoltageWeight, float LowVoltageWeight);
    void UpdateTags(void);
    bool OnError(TMultiGridEdit* Edit, QString Msg, bool Silent=false);
    bool YesNoOnError(TMultiGridEdit* Edit, QString Msg, bool Silent=false, bool SilentAnswer=true);
    bool IsHeadMarkedReplaced(int HeadIndex);
    void UpdateIndexedToNext(int& HeadI, int& VoltageI, int& QualityI);
    void UpdateIndexedToNext(int& HeadI, int& VoltageI, int& QualityI, int& StepsNumI, bool UpdateGUI);
    float GetCurrentWeight(void);
    int GetCurrentMissingNozzlesValue();
    int CalcRow(/*int HeadI,*/ int VoltageI, int QualityI);
    int CalcCol(int HeadI, int VoltageI, int QualityI);

public:		// User declarations - Members

public:		// User declarations - Methods
    __fastcall ~THeadsButtonsEntryFrame();
    __fastcall THeadsButtonsEntryFrame(TComponent* Owner, float*** Weighs, int*** MissingNozzles, int& StepsNum, int& QualityIndex, int& VoltageIndex, int& HeadIndex, bool* aHeadsToBeSkipped,bool*** aMarkHeadsToBeReplaced);

    bool CheckPreviousButtonStatus(void);
    void UpdateIndexedToNext(void);
    void UpdateIndexedToPrev(void);
    void RefreshButtons();


};
//---------------------------------------------------------------------------
extern PACKAGE THeadsButtonsEntryFrame *HeadsButtonsEntryFrame;
//---------------------------------------------------------------------------


class CHeadsButtonsEntryFrameWizardPageViewer : public CCustomWizardPageViewer
{


private:
   THeadsButtonsEntryFrame *m_HeadsButtonsEntryFrame;

   bool CheckPreviousButtonStatus(CWizardPage *WizardPage);
   void UpdateGotoNextIndex(void);
   void GotoNextIndex(void);

public:
   CHeadsButtonsEntryFrameWizardPageViewer();
   ~CHeadsButtonsEntryFrameWizardPageViewer();
   void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
   void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
   void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);

   void __fastcall ApplyButtonClick(TObject *Sender);
};


#endif
