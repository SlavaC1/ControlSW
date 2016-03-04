//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UserValuesEntryWizardFrame.h"
#include "QUtils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TUserValuesEntryPageFrame::TUserValuesEntryPageFrame(TComponent* Owner)
        : TFrame(Owner)
{
    int DefaultLabelWidth = SubTitleLabel->Width/TOTAL_NUMBER_OF_HEADS;
    int SpaceX            = DefaultLabelWidth/4;
    DefaultLabelWidth     -= SpaceX;
    FOR_ALL_HEADS(i)
    {
	LabeledEditArray[i]                   = new TLabeledEdit(this);
	LabeledEditArray[i]->Parent           = SubTitleLabel->Parent;
	LabeledEditArray[i]->Tag              = i;
	LabeledEditArray[i]->TabOrder         = i;
      	LabeledEditArray[i]->Text             = '0';
	LabeledEditArray[i]->Left             = (SpaceX + DefaultLabelWidth)*i;
        LabeledEditArray[i]->Width            = DefaultLabelWidth;
        LabeledEditArray[i]->Height           = 21;
        LabeledEditArray[i]->EditLabel->Width  = DefaultLabelWidth;
        LabeledEditArray[i]->EditLabel->Height = 13;
	LabeledEditArray[i]->Top              = 77;
	LabeledEditArray[i]->LabelPosition    = lpAbove;
	LabeledEditArray[i]->LabelSpacing     = 3;
	LabeledEditArray[i]->OnExit           = EditExit;
    }
}
//---------------------------------------------------------------------------

void TUserValuesEntryPageFrame::SetLabelsColor(int* Colors)
{
  TLabeledEdit* LabeledEdit;
  for (int i = 0; i < ControlCount; i++)
  {
    LabeledEdit = dynamic_cast<TLabeledEdit*> (Controls[i]);
    if (LabeledEdit != NULL)
      LabeledEdit->EditLabel->Font->Color = static_cast<TColor>(Colors[LabeledEdit->Tag]);
  }
}

void TUserValuesEntryPageFrame::SetEditLabels(QString *Labels)
{
  TLabeledEdit* LabeledEdit;
  for (int i = 0; i < ControlCount; i++)
  {
    LabeledEdit = dynamic_cast<TLabeledEdit*> (Controls[i]);
    if (LabeledEdit != NULL)
      LabeledEdit->EditLabel->Caption = Labels[LabeledEdit->Tag].c_str();
  }
}

void TUserValuesEntryPageFrame::GetUserEntryData(QString *Data)
{
  TLabeledEdit* LabeledEdit;
  for (int i = 0; i < ControlCount; i++)
  {
    LabeledEdit = dynamic_cast<TLabeledEdit*> (Controls[i]);
    if (LabeledEdit != NULL)
    {
      Data[LabeledEdit->Tag] = LabeledEdit->Text.c_str();

#ifdef EDEN_250
      switch (LabeledEdit->Tag)
      {
        case M2:
        case M3:
        case S2:
        case S3:
        {
          Data[LabeledEdit->Tag] = "5";
          break;
        }

        default:
          break;
      }
#endif
    }
  }
}

bool TUserValuesEntryPageFrame::CheckString(QString Str)
{
  switch(UserValuesType)
  {
    case evInt:
      return TypedStringChecker<int>(Str);

    case evFloat:
      return TypedStringChecker<float>(Str);

    case evUnsigned:
      return TypedStringChecker<unsigned>(Str);

    case evString:
      return true;
  }
  return false;
}

template<class T>
bool TUserValuesEntryPageFrame::TypedStringChecker(QString Str)
{
    if(Str.length() == 0)
      return false;

    try
    {
      QStrToValue<T>(Str);
      return true;
    } catch(...)
      {
        return false;
      }
}

void __fastcall TUserValuesEntryPageFrame::EditExit(TObject *Sender)
{
  TLabeledEdit *LabeledEdit = dynamic_cast<TLabeledEdit *>(Sender);
  if (!CheckString(LabeledEdit->Text.c_str()))
  {
    ShowMessage("The value entered is not valid");
    LabeledEdit->SetFocus();
  }
}
//---------------------------------------------------------------------------

void TUserValuesEntryPageFrame::SetData(QString *Data)
{
  TLabeledEdit* LabeledEdit;
  for (int i = 0; i < ControlCount; i++)
  {
    LabeledEdit = dynamic_cast<TLabeledEdit*> (Controls[i]);
    if (LabeledEdit != NULL)
     LabeledEdit->Text = Data[LabeledEdit->Tag].c_str();
  }

}

void TUserValuesEntryPageFrame::Clear(void)
{
  FOR_ALL_HEADS(i)
      LabeledEditArray[i]->Text = "0";
}


