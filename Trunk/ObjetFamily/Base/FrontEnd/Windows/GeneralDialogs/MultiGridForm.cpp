//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "MultiGridForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "QMonitor.h"
#include "QMutex.h"

TMultiGridForm *MultiGridForm;




//---------------------------------------------------------------------------
__fastcall TMultiGridEdit::TMultiGridEdit(TComponent* AOwner, int TagsSize) : TEdit(AOwner), Tags(NULL), m_TagsSize(TagsSize)
{
   RowTag = 0;
   ColTag = 0;
   CellTag = 0;
   if(m_TagsSize)
     Tags = new int[m_TagsSize];
}
//---------------------------------------------------------------------------
__fastcall TMultiGridEdit::~TMultiGridEdit()
{
   if(Tags)
     delete[] Tags;
}
//---------------------------------------------------------------------------
__fastcall TMultiGridForm::TMultiGridForm(TComponent* Owner, int ColNum, int RowNum, int CellPartsNum, int TagsSize)
    : TForm(Owner), m_ColNum(ColNum), m_RowNum(RowNum), m_CellPartsNum(CellPartsNum)
{
  int BigLabelHeight = 40;
  int Width = 40;
  
  RowLabels = new TLabel**[m_RowNum];
  Edits = new TMultiGridEdit***[m_RowNum];
  OriginalValues = new QString**[m_RowNum];
  ColumnLabels = new TLabel*[m_ColNum];
  for (int row = 0; row < m_RowNum; row++)
  {
      RowLabels[row] = new TLabel*[m_CellPartsNum];
      Edits[row] = new TMultiGridEdit**[m_CellPartsNum];
      OriginalValues[row] = new QString*[m_CellPartsNum];

      for(int cell = 0; cell < m_CellPartsNum; cell++)
      {
          Edits[row][cell] = new TMultiGridEdit*[m_ColNum];
          OriginalValues[row][cell] = new QString[m_ColNum];
          for(int col = 0; col < m_ColNum; col++)
          {
             Edits[row][cell][col]           = new TMultiGridEdit(this, TagsSize);
             Edits[row][cell][col]->Parent   = MainPanel;
             Edits[row][cell][col]->Width    = Width;
             Edits[row][cell][col]->Height   = BigLabelHeight;
             Edits[row][cell][col]->RowTag   = row;
             Edits[row][cell][col]->ColTag   = col;
             Edits[row][cell][col]->CellTag  = cell;
             Edits[row][cell][col]->OnChange = OnChange;
             Edits[row][cell][col]->AutoSize = false;
          }

          RowLabels[row][cell] = new TLabel(this);
          RowLabels[row][cell]->Parent      = MainPanel;
          RowLabels[row][cell]->AutoSize    = false;
          RowLabels[row][cell]->Width       = Width;
          RowLabels[row][cell]->Height      = Edits[row][cell][0]->Height;
          RowLabels[row][cell]->Alignment   = taCenter;
          RowLabels[row][cell]->WordWrap    = true;
          RowLabels[row][cell]->Layout      = tlCenter;
          RowLabels[row][cell]->Transparent = true;
      }
  }

  for(int col = 0; col < m_ColNum; col++)
  {
      ColumnLabels[col] = new TLabel(this);
      ColumnLabels[col]->Parent      = MainPanel;
      ColumnLabels[col]->AutoSize    = false;
      ColumnLabels[col]->Width       = Edits[0][0][0]->Width;
      ColumnLabels[col]->Height      = BigLabelHeight;
      ColumnLabels[col]->Alignment   = taCenter;
      ColumnLabels[col]->Font->Style = TFontStyles()<< fsBold;
  }
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetRowLabelHeight(int Height, int Cell)
{
  if (Cell == -1)
  {
    for (int cell = 0; cell < m_CellPartsNum; cell++)
       SetRowLabelHeight(Height,cell);
    return;
  }

  for (int row = 0; row < m_RowNum; row++)
  {
      RowLabels[row][Cell]->Height = Height;
      for(int col = 0; col < m_ColNum; col++)
         Edits[row][Cell][col]->Height = Height;
  }
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetRowLabelWidth(int Width)
{
  for (int row = 0; row < m_RowNum; row++)
      for(int cell = 0; cell < m_CellPartsNum; cell++)
          RowLabels[row][cell]->Width = Width;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetColumnLabelColor(TColor Color, int Col)
{
   if (-1 == Col)
   {
      for(int col = 0; col < m_ColNum; col++)
         SetColumnLabelColor(Color, col);
   }
   else
      ColumnLabels[Col]->Color = Color;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetColumnLabelHeight(int Height, int Col)
{
   if (-1 == Col)
   {
      for(int col = 0; col < m_ColNum; col++)
         SetColumnLabelHeight(Height, col);
   }
   else
      ColumnLabels[Col]->Height = Height;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetColumnWidth(int Width, int Col)
{
   if (-1 == Col)
   {
      for(int col = 0; col < m_ColNum; col++)
      {
         SetColumnWidth(Width, col);
      }
   }
   else
   {
      ColumnLabels[Col]->Width = Width;
      for (int row = 0; row < m_RowNum; row++)
         for(int cell = 0; cell < m_CellPartsNum; cell++)
             Edits[row][cell][Col]->Width = Width;
   }
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetRowLabelCaption(QString& Caption, int Cell)
{
  if (Cell == -1)
  {
    for(int cell = 0; cell < m_CellPartsNum; cell++)
      SetRowLabelCaption(Caption,cell);
  }
  for (int row = 0; row < m_RowNum; row++)
      RowLabels[row][Cell]->Caption = Caption.c_str();
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetColumnLabelCaption(int Col, QString Caption)
{
   ColumnLabels[Col]->Caption = Caption.c_str();
}
//---------------------------------------------------------------------------
__fastcall TMultiGridForm::~TMultiGridForm()
{
  for (int row = 0; row < m_RowNum; row++)
  {
      for(int cell = 0; cell < m_CellPartsNum; cell++)
      {
          delete[] Edits[row][cell];
          delete[] OriginalValues[row][cell];

      }
      delete[] RowLabels[row];
      delete[] Edits[row];
      delete[] OriginalValues[row];
  }

  delete[] ColumnLabels;
  delete[] RowLabels;
  delete[] Edits;
  delete[] OriginalValues;
}
//---------------------------------------------------------------------------
void TMultiGridForm::RePosition(void)
{
  int Nibble = 5;
  int Top = 0;
  RowLabels[0][0]->Left = Nibble;
  int Left = RowLabels[0][0]->Left + RowLabels[0][0]->Width + Nibble;

  for(int col = 0; col < m_ColNum; col++)
  {
     ColumnLabels[col]->Left        = Left;
     ColumnLabels[col]->Top         = Top;
     Left += ColumnLabels[col]->Width+Nibble;
  }
  Top += ColumnLabels[0]->Height;
  
  for (int row = 0; row < m_RowNum; row++)
  {
      for(int cell = 0; cell < m_CellPartsNum; cell++)
      {
          RowLabels[row][cell]->Left        = RowLabels[0][0]->Left;
          RowLabels[row][cell]->Top         = Top;

          for(int col = 0; col < m_ColNum; col++)
          {
             Edits[row][cell][col]->Left   = ColumnLabels[col]->Left;
             Edits[row][cell][col]->Top    = Top;
          }
          Top += Edits[row][cell][0]->Height + Nibble;
      }
      Top += Nibble;
  }

  //Update MainPanel size
  MainPanel->ClientWidth = Left;
  MainPanel->ClientHeight = Top;
  MainPanel->Left = 0;
  MainPanel->Top  = 0;

  ClientWidth  = MainPanel->Width;

  CancelButton->Left  = ClientWidth - CancelButton->Width - Nibble;
  SaveButton->Left    = CancelButton->Left - SaveButton->Width - Nibble;
  RestoreButton->Left = SaveButton->Left - RestoreButton->Width - Nibble;

  CancelButton->Top   = MainPanel->Height + 2*Nibble;
  SaveButton->Top     = CancelButton->Top;
  RestoreButton->Top  = CancelButton->Top;

  ClientHeight = CancelButton->Top + CancelButton->Height + Nibble;
}
//---------------------------------------------------------------------------
QString TMultiGridForm::GetEditValue(int Row, int Cell, int Col)
{
   return Edits[Row][Cell][Col]->Text.c_str();
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetEditExitEvent(TNotifyEvent EditExitEvent, int Cell)
{
  for(int row = 0; row < m_RowNum; row++)
     for(int col = 0; col < m_ColNum; col++)
         Edits[row][Cell][col]->OnExit = EditExitEvent;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetKeyPressEvent(TKeyPressEvent KeyPressEvent, int Cell)
{
  for(int row = 0; row < m_RowNum; row++)
     for(int col = 0; col < m_ColNum; col++)
         Edits[row][Cell][col]->OnKeyPress = KeyPressEvent;
}
//---------------------------------------------------------------------------
int TMultiGridForm::GetEditTag(int TagIndex, int Row, int Cell, int Col)
{
   return Edits[Row][Cell][Col]->Tags[TagIndex];;
}
//---------------------------------------------------------------------------//---------------------------------------------------------------------------
void TMultiGridForm::SetEditTag(int Tag, int TagIndex, int Row, int Cell, int Col)
{
   if (-1 == Row)
       for(int row = 0; row < m_RowNum; row++)
          SetEditTag(Tag, TagIndex, row, Cell, Col);
   else if (-1 == Cell)
       for(int cell = 0; cell < m_CellPartsNum; cell++)
          SetEditTag(Tag, TagIndex, Row, cell, Col);
   else if (-1 == Col)
       for(int col = 0; col < m_ColNum; col++)
          SetEditTag(Tag, TagIndex, Row, Cell, col);
   else
      Edits[Row][Cell][Col]->Tags[TagIndex] = Tag;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetEditTag(int Tag, int Row, int Cell, int Col)
{
   if (-1 == Row)
       for(int row = 0; row < m_RowNum; row++)
          SetEditTag(Tag, row, Cell, Col);
   else if (-1 == Cell)
       for(int cell = 0; cell < m_CellPartsNum; cell++)
          SetEditTag(Tag, Row, cell, Col);
   else if (-1 == Col)
       for(int col = 0; col < m_ColNum; col++)
          SetEditTag(Tag, Row, Cell, col);
   else
      Edits[Row][Cell][Col]->Tag = Tag;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetEditTab(int Tab, int Row, int Cell, int Col)
{
   Edits[Row][Cell][Col]->TabOrder = Tab;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetEditValue(QString Text, int Row, int Cell, int Col)
{
   if (-1 == Row)
       for(int row = 0; row < m_RowNum; row++)
          SetEditValue(Text, row, Cell, Col);
   else if (-1 == Cell)
       for(int cell = 0; cell < m_CellPartsNum; cell++)
          SetEditValue(Text, Row, cell, Col);
   else if (-1 == Col)
       for(int col = 0; col < m_ColNum; col++)
          SetEditValue(Text, Row, Cell, col);
   else
   {
      Edits[Row][Cell][Col]->Text = Text.c_str();
      SetEditEnabled(true, Row, Cell, Col);
      //this line is not an error
      Edits[Row][Cell][Col]->Width = Edits[Row][Cell][Col]->Width;
      OriginalValues[Row][Cell][Col] = Text;
   }
}
//---------------------------------------------------------------------------
bool TMultiGridForm::GetEditEnabled(int Row, int Cell, int Col)
{
   return Edits[Row][Cell][Col]->Enabled;
}
//---------------------------------------------------------------------------
void TMultiGridForm::SetEditEnabled(bool Enabled, int Row, int Cell, int Col, TColor Color)
{
   if (-1 == Row)
       for(int row = 0; row < m_RowNum; row++)
          SetEditEnabled(Enabled, row, Cell, Col, Color);
   else if (-1 == Cell)
       for(int cell = 0; cell < m_CellPartsNum; cell++)
          SetEditEnabled(Enabled, Row, cell, Col, Color);
   else if (-1 == Col)
       for(int col = 0; col < m_ColNum; col++)
          SetEditEnabled(Enabled, Row, Cell, col, Color);
   else
   {
      Edits[Row][Cell][Col]->Color = (Enabled ? clWindow : Color);
      Edits[Row][Cell][Col]->Enabled = Enabled;
   }
}
//---------------------------------------------------------------------------
void TMultiGridForm::Clear(void)
{
  for (int row = 0; row < m_RowNum; row++)
      for(int cell = 0; cell < m_CellPartsNum; cell++)
          for(int col = 0; col < m_ColNum; col++)
          {
              Edits[row][cell][col]->Font->Color = clWindowText;
              SetEditEnabled(false, row, cell, col);
          }
}
//---------------------------------------------------------------------------
void __fastcall TMultiGridForm::OnChange(TObject *Sender)
{
  //Update 
  TMultiGridEdit* Edit = dynamic_cast<TMultiGridEdit *>(Sender);
  int row = Edit->RowTag;
  int cell = Edit->CellTag;
  int col = Edit->ColTag;
  Edit->Font->Color = ((OriginalValues[row][cell][col].compare( Edit->Text.c_str() ) != 0) ? clBlue : clWindowText);

  for (int row = 0; row < m_RowNum; row++)
      for(int cell = 0; cell < m_CellPartsNum; cell++)
          for(int col = 0; col < m_ColNum; col++)
          {
              if (!Edits[row][cell][col]->Enabled)
                 continue;

              if (Edits[row][cell][col]->Text == "")
              {
                 SaveButton->Enabled = (OriginalValues[row][cell][col] == "");
                 RestoreButton->Enabled = true;
                 if (!SaveButton->Enabled)
                    return;
              }
          }
  SaveButton->Enabled = true;
  RestoreButton->Enabled = false;
}
//---------------------------------------------------------------------------





/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TMultiGridForm::FormShow(TObject *Sender)
{
  for (int row = 0; row < m_RowNum; row++)
      for(int cell = 0; cell < m_CellPartsNum; cell++)
          for(int col = 0; col < m_ColNum; col++)
              Edits[row][cell][col]->Font->Color = clWindowText;
  RePosition();
}
//---------------------------------------------------------------------------
void __fastcall TMultiGridForm::RestoreButtonClick(TObject *Sender)
{
  for (int row = 0; row < m_RowNum; row++)
      for(int cell = 0; cell < m_CellPartsNum; cell++)
          for(int col = 0; col < m_ColNum; col++)
          {
              if (!Edits[row][cell][col]->Enabled)
                 continue;

              if (Edits[row][cell][col]->Text == "")
                 Edits[row][cell][col]->Text = OriginalValues[row][cell][col].c_str();
          }
}
//---------------------------------------------------------------------------
void __fastcall TMultiGridForm::SaveButtonClick(TObject *Sender)
{
  for (int row = 0; row < m_RowNum; row++)
      for(int cell = 0; cell < m_CellPartsNum; cell++)
          for(int col = 0; col < m_ColNum; col++)
              if (Edits[row][cell][col]->Font->Color == clRed)
                 QMonitor.ErrorMessage("Please correct the values in red or press 'Cancel'");
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
