//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PrepareForBitDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPrepareBitDlg *PrepareBitDlg;


//---------------------------------------------------------------------------
__fastcall TPrepareBitDlg::TPrepareBitDlg(TComponent* Owner)
	: TForm(Owner)
{
	m_CheckBoxVector.clear();
	m_attributesVector.clear();
	m_wasCancelled = false;
	m_wasClosed = false;
}
//---------------------------------------------------------------------------
void TPrepareBitDlg::AddAttribute(String attribute)
{
  bool exists = false;
  for(unsigned i = 0; i < m_attributesVector.size();++i)
	{
		if(m_attributesVector[i] == attribute)
		{
			exists = true;
			break;
		}
	}
   if(exists == false)
		m_attributesVector.push_back(attribute);
}
bool TPrepareBitDlg::RemoveAttribute(String attribute)
{
  vector <String>::iterator Iter;
  for ( Iter = m_attributesVector.begin( ) ; Iter != m_attributesVector.end( ) ; ++Iter )
  {
     if( (*Iter) == attribute)
     {
         m_attributesVector.erase( Iter);
         return true;
     }
  }
  return false;

}

bool TPrepareBitDlg::IsTrayEmptyIsOnlyAttribute()
{
        if( (m_attributesVector.size() == 1)&&(m_attributesVector[0] == TRAY_EMPTY))
        {
        	return true;
        }
	return false;
}
//---------------------------------------------------------------------------
bool TPrepareBitDlg::IsAttributeExists(String attribute)
{
  bool exists = false;
  for(unsigned i = 0; i < m_attributesVector.size(); ++i)
  {
  	if(m_attributesVector[i] == attribute)
        {
        	exists = true;
                break;
  	}
  }
   return  exists;
}
//---------------------------------------------------------------------------
void  TPrepareBitDlg::UpdateView(bool needToCreateChBx)  // add checkbox to view screen
{
  m_wasCancelled = false;
  if(needToCreateChBx == true)
  {
	for(unsigned i = 0; i < m_attributesVector.size(); ++i)
	{
		AddCheckBox(m_attributesVector[i],i);
	}
  }
}
//---------------------------------------------------------------------------
void TPrepareBitDlg::AddCheckBox(String checkboxName,int index)
{
     //make sure the checkbox is not exist
      bool exists = false;
      for(unsigned i = 0; i < m_CheckBoxVector.size();++i)
      {
          if(m_CheckBoxVector[i]->Caption == checkboxName)
          {
              exists = true;
              break;
          }
       }
       if(exists == false)
       {
	TCheckBox *checkBox = new  TCheckBox(this);
	checkBox->Caption = checkboxName;
	checkBox->Parent    = ItemGroupBox;
	checkBox->Left      = 18;
	checkBox->Top       = 32*(index+1);
	checkBox->Width     = 200;
	checkBox->Checked   = false;
	checkBox->Height     = 21;
	checkBox->OnClick = SetCheckboxEvent;
	m_CheckBoxVector.push_back(checkBox);
       }

 }
//--------------------------------------------------------------------------- 
 void TPrepareBitDlg::SelectCheckBox(String checkboxName, bool check)
 {
   for(unsigned i = 0; i < m_CheckBoxVector.size(); ++i)
	{
		if(m_CheckBoxVector[i]->Caption == checkboxName)
			{
				m_CheckBoxVector[i]->Checked = check;
				break;
			}
	}
 }
//---------------------------------------------------------------------------
 void TPrepareBitDlg::EnableCheckBox(String checkboxName, bool enable)
 {
   for(unsigned i = 0; i < m_CheckBoxVector.size(); ++i)
   {
   	if(m_CheckBoxVector[i]->Caption == checkboxName)
        {
        	m_CheckBoxVector[i]->Enabled = enable;
                break;
        }
   }
 }
//---------------------------------------------------------------------------
void TPrepareBitDlg::Clean()
{
   for(unsigned i = 0; i < m_CheckBoxVector.size(); ++i)
   {
        delete m_CheckBoxVector[i];
   }
   m_CheckBoxVector.clear();
   m_attributesVector.clear();
   m_wasClosed = false;
   OK->Enabled = false;
 }
//---------------------------------------------------------------------------
void TPrepareBitDlg::CloseDialog()
{
  m_wasClosed = true;
  Close();
}
//---------------------------------------------------------------------------
bool TPrepareBitDlg::NeedToShow()
{
  if(! m_attributesVector.empty())
	return true;
  else
  	return false;
}
//---------------------------------------------------------------------------
bool TPrepareBitDlg::WasCancelled()
{
  return  m_wasCancelled;
}
//---------------------------------------------------------------------------
bool TPrepareBitDlg::WasClosed()
{
  return  m_wasClosed;
}





/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TPrepareBitDlg::OKClick(TObject *Sender)
{
  CloseDialog();
}
//---------------------------------------------------------------------------
void __fastcall TPrepareBitDlg::CancellBttnClick(TObject *Sender)
{
   m_wasCancelled = true;
   CBackEndInterface* BackEnd = CBackEndInterface::Instance();
   BackEnd->EnableDoor(false);
   CloseDialog();
   Clean();
}
//---------------------------------------------------------------------------
void __fastcall TPrepareBitDlg::SetCheckboxEvent(TObject *Sender)
{
	bool allItemsChecked = true;
   for(unsigned i = 0; i < m_CheckBoxVector.size(); i++)
   {
	 if(m_CheckBoxVector[i]->Checked == false)
	 {
		allItemsChecked = false;
		break;
	 }
   }
   if(allItemsChecked == true)
	OK->Enabled = true;
   else
    OK->Enabled = false;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
