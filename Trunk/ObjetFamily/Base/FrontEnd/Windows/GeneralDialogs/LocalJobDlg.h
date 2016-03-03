//---------------------------------------------------------------------------

#ifndef LocalJobDlgH
#define LocalJobDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>

#include "QTypes.h"
#include <ExtDlgs.hpp>
#include "GlobalDefs.h"
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TLocalJobForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *FirstBMPGroupBox;
        TEdit *FileName1Edit;
        TButton *OpenFileSelection1Button;
        TComboBox *TypeSelector1ComboBox;
        TOpenPictureDialog *OpenDialog;
        TBitBtn *OkButton;
        TBitBtn *CancelButton;
        TGroupBox *SecondBMPGroupBox;
        TEdit *FileName2Edit;
        TButton *OpenFileSelection2Button;
        TComboBox *TypeSelector2ComboBox;
        TCheckBox *PrintDoubleBMPCheckBox;
        TLabel *NumberOfSlices1Label;
        TEdit *NumberOfSlices1Edit;
        TUpDown *NoOfSlices1UpDown;
        TLabel *NumberOfSlices2Label;
        TEdit *NumberOfSlices2Edit;
        TUpDown *NoOfSlices2UpDown;
	TListView *ListView1;
	TListView *ListView2;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
        void __fastcall OpenFileSelection1ButtonClick(TObject *Sender);
        void __fastcall TypeSelectorComboBoxChange(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall ModeRadioButtonsClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall PrintDoubleBMPCheckBoxClick(TObject *Sender);
private:	// User declarations
        void __fastcall Prepare(TOperationModeIndex OperationMode);

public:		// User declarations
        __fastcall TLocalJobForm(TComponent* Owner);

  // Get the print job settings
  void GetJobSettings(TFileNamesArray    FileNames,
                      int&               BitmapResolution,
                      int&               QualityMode,
                      int&               OperationMode,
                      int&               SliceNum);

  void GetJobSettings(TFileNamesArray    FileNames1,
                      TFileNamesArray    FileNames2,
                      int&               BitmapResolution,
                      int&               QualityMode,
                      int&               OperationMode,
                      int&               SliceNum1,
                      int&               SliceNum2);

  bool IsPrintDoubleBMPEnabled();

private:	// User declarations- Variables
        TFileNamesArray m_FileNamesArray1, m_FileNamesArray2;
        TChamberIndex   m_PrevItemIndex1, m_PrevItemIndex2;
        TRadioButton*   m_ModesRadioButton[NUMBER_OF_QUALITY_MODES]
                                          [NUM_OF_OPERATION_MODES];

};
//---------------------------------------------------------------------------
extern PACKAGE TLocalJobForm *LocalJobForm;
//---------------------------------------------------------------------------
#endif
