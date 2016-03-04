//---------------------------------------------------------------------------

#ifndef BITExtResultsDlgH
#define BITExtResultsDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "QTypes.h"
#include <Buttons.hpp>

//---------------------------------------------------------------------------
class TBITExtendedResultsForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *TestNameLabel;
        TLabel *Label3;
        TImage *StatusImage;
        TMemo *Memo1;
        TLabel *Label2;
        TListView *ExtResultsTable;
        TBitBtn *CloseButton;
        TBitBtn *RunTestButton;
        void __fastcall RunTestButton1Click(TObject *Sender);
        void __fastcall CloseButton1Click(TObject *Sender);
        void __fastcall RunTestButtonClick(TObject *Sender);
        void __fastcall CloseButtonClick(TObject *Sender);
private:	// User declarations
  TImageList *m_ImageList;
  AnsiString m_GroupName;
  AnsiString m_TestName;

public:		// User declarations
        __fastcall TBITExtendedResultsForm(TComponent* Owner);

  void UpdateResults(AnsiString GroupName,AnsiString TestName,AnsiString ErrorSummary,TImageList *ImageList,
                     int StatusImageIndex);

  void StartResultsTableUpdate(void);
  void AddTableEntry(const QString Entry);
};
//---------------------------------------------------------------------------
extern PACKAGE TBITExtendedResultsForm *BITExtendedResultsForm;
//---------------------------------------------------------------------------
#endif
