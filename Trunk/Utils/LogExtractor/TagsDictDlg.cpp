//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TagsDictDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTagsDictForm *TagsDictForm;
//---------------------------------------------------------------------------
__fastcall TTagsDictForm::TTagsDictForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TTagsDictForm::SetTagsDictionary(TTagsDictionary& TagsDict)
{
  m_TagsDict = &TagsDict;

  TagsCheckListBox->Items->Clear();

  for(TTagsDictionary::const_iterator i = TagsDict.begin(); i != TagsDict.end(); i++)
    TagsCheckListBox->Items->Add((*i).TagName.c_str());
}

AnsiString TTagsDictForm::GetTagsString(void)
{
  AnsiString s;

  for(int i = 0; i < TagsCheckListBox->Items->Count; i++)
  {
    if(TagsCheckListBox->Checked[i])
    {
      // Search for the tag ID according to the tag name
      unsigned j;
      for(j = 0; j < m_TagsDict->size(); j++)
        if((*m_TagsDict)[j].TagName == TagsCheckListBox->Items->Strings[i].c_str())
          break;

      if(j < m_TagsDict->size())
        s = s + IntToStr((*m_TagsDict)[j].TagID) + ",";
    }
  }

  // Remove the last ','
  s = s.SubString(1,s.Length() - 1);

  return s;
}

void TTagsDictForm::MarkTags(const TTagList& TagList)
{
  for(TTagList::const_iterator i = TagList.begin(); i != TagList.end(); i++)
  {
    // Search for the tag ID according to the tag name
    unsigned j;
    for(j = 0; j < m_TagsDict->size(); j++)
      if((*m_TagsDict)[j].TagID == *i)
        break;

    if(j < m_TagsDict->size())
      TagsCheckListBox->Checked[j] = true;
  }
}

void __fastcall TTagsDictForm::ClearAllMenuItemClick(TObject *Sender)
{
  for(int i = 0; i < TagsCheckListBox->Items->Count; i++)
    TagsCheckListBox->Checked[i] = false;
}
//---------------------------------------------------------------------------

void __fastcall TTagsDictForm::SelectAllMenuItemClick(TObject *Sender)
{
  for(int i = 0; i < TagsCheckListBox->Items->Count; i++)
    TagsCheckListBox->Checked[i] = true;
}
//---------------------------------------------------------------------------

