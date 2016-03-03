//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "BITExtResultsDlg.h"
#include "BITViewerDlg.h"
#include "QStringList.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBITExtendedResultsForm *BITExtendedResultsForm;


//---------------------------------------------------------------------------
__fastcall TBITExtendedResultsForm::TBITExtendedResultsForm(TComponent* Owner)
        : TForm(Owner)
{
	m_ImageList = NULL;
}
//---------------------------------------------------------------------------
void TBITExtendedResultsForm::UpdateResults(AnsiString GroupName,AnsiString TestName,AnsiString ErrorSummary,
                                            TImageList *ImageList,int StatusImageIndex)
{
  m_GroupName = GroupName;
  m_TestName = TestName;
  m_ImageList = ImageList;

  TestNameLabel->Caption = GroupName + " | " + TestName;
  StatusImage->Picture->Bitmap->Width = 0;
  StatusImage->Picture->Bitmap->Height = 0;  
  ImageList->GetBitmap(StatusImageIndex,StatusImage->Picture->Bitmap);
  Memo1->Text = ErrorSummary;
}
//---------------------------------------------------------------------------
void TBITExtendedResultsForm::StartResultsTableUpdate(void)
{
  ExtResultsTable->Clear();
}
//---------------------------------------------------------------------------
void TBITExtendedResultsForm::AddTableEntry(const QString Entry)
{
  // Use string list to break the Entry string to its sub items
  CQStringList TmpStrList;

  TmpStrList.LoadFromString(Entry,'\n');

  if(TmpStrList.Count() > 0)
  {
    // Create the new item
    ExtResultsTable->AddItem(TmpStrList[0].c_str(),NULL);
    TListItem *Item = ExtResultsTable->Items->Item[ExtResultsTable->Items->Count - 1];

    for(unsigned i = 1; i < TmpStrList.Count(); i++)
      Item->SubItems->Add(TmpStrList[i].c_str());
  }
}


/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TBITExtendedResultsForm::RunTestButton1Click(TObject *Sender)
{
  BITViewerForm->TestAndUpdateSelectedItem();
}
//---------------------------------------------------------------------------
void __fastcall TBITExtendedResultsForm::CloseButton1Click(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------
void __fastcall TBITExtendedResultsForm::RunTestButtonClick(TObject *Sender)
{
  RunTestButton->Enabled = false;
  CloseButton->Enabled   = false;

  BITViewerForm->TestAndUpdateSelectedItem();

  RunTestButton->Enabled = true;
  CloseButton->Enabled   = true;
}
//---------------------------------------------------------------------------
void __fastcall TBITExtendedResultsForm::CloseButtonClick(TObject *Sender)
{
  Close();        
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

