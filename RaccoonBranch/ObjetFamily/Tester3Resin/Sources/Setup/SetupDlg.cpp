//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SetupDlg.h"
#include "QStdComPort.h"
#include "AppParams.h"
#include <FileCtrl.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetupForm *SetupForm;
//---------------------------------------------------------------------------
__fastcall TSetupForm::TSetupForm(TComponent* Owner)
  : TForm(Owner)
{

}
//---------------------------------------------------------------------------



void TSetupForm::FindAllAvailableComPorts()
{
  CQStdComPort TempCom;
  for (int ComNum = 1; ComNum <= MAX_NUM_COM_PORTS; ComNum++)
  {
    try
    {
     TempCom.Init(ComNum);
     TempCom.DeInit();
     TComPortItemInfo InfoItem = {ComNum,false};
     m_ComPortInfoList.push_back(InfoItem); // add the current com port to the list
    }
    catch(...)
    {
      continue; // the current com port does not exist or in use
    }
  }
}
//---------------------------------------------------------------------------


void __fastcall TSetupForm::ComComboBoxDropDown(TObject *Sender)
{
  TComboBox* CurrComboBox = reinterpret_cast<TComboBox*>(Sender);
  CurrComboBox->Clear();

  for (unsigned i = 0; i < m_ComPortInfoList.size(); i++)
  {
    if (m_ComPortInfoList[i].Used == false) // this com port is availabe
      CurrComboBox->Items->Add("Com " + IntToStr(m_ComPortInfoList[i].ComNum));
  }
}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::EmbeddedRadioGroupClick(TObject *Sender)
{
  switch(EmbeddedRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->EmbeddedConnectionKind = NONE;
            EmbeddedComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->EmbeddedComNum != 0)
              UpdateCommList(m_SystemParams->EmbeddedComNum,false);
            break;
    case DIRECT: m_SystemParams->EmbeddedConnectionKind = DIRECT;
            if (EmbeddedComboBox->Text != "")
               {
                m_SystemParams->EmbeddedComNum =  StrToInt(EmbeddedComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->EmbeddedComNum,true);

               }
            else
              {
                ShowMessage("Please choose the Comm number");
                EmbeddedRadioGroup->ItemIndex = 0;
              }
            break;
    }

}
//---------------------------------------------------------------------------


void __fastcall TSetupForm::OCBSimRadioGroupClick(TObject *Sender)
{
  switch(OCBSimRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->OCBSimConnectionKind = NONE;
            OCBSimComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->OCBSimComNum != 0)
              UpdateCommList(m_SystemParams->OCBSimComNum,false);
            break;
    case DIRECT: m_SystemParams->OCBSimConnectionKind = DIRECT;
            if (OCBSimComboBox->Text != "")
               {
                m_SystemParams->OCBSimComNum =  StrToInt(OCBSimComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->OCBSimComNum,true);

               }
            else
              {
                ShowMessage("Please choose the Comm number");
                OCBSimRadioGroup->ItemIndex = 0;
              }
            break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::MCBRadioGroupClick(TObject *Sender)
{
  switch(MCBRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->MCBConnectionKind = NONE;
            MCBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->MCBComNum != 0)
              UpdateCommList(m_SystemParams->MCBComNum,false);
            break;
    case DIRECT: m_SystemParams->MCBConnectionKind = DIRECT;
            if (MCBComboBox->Text != "")
            {
                m_SystemParams->MCBComNum =  StrToInt(MCBComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->MCBComNum,true);
            }
            else
              {
                ShowMessage("Please choose the Comm number");
                MCBRadioGroup->ItemIndex = 0;
              }
            break;
    case THROUGH_EMBEDDED: m_SystemParams->MCBConnectionKind = THROUGH_EMBEDDED;
            MCBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->MCBComNum != 0)
              UpdateCommList(m_SystemParams->MCBComNum,false);
            break;

    }

}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::LCDRadioGroupClick(TObject *Sender)
{
  switch(LCDRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->LCDConnectionKind = NONE;
            LCDComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->LCDComNum != 0)
              UpdateCommList(m_SystemParams->LCDComNum,false);
            break;
    case DIRECT: m_SystemParams->LCDConnectionKind = DIRECT;
            if (LCDComboBox->Text != "")
            {
                m_SystemParams->LCDComNum =  StrToInt(LCDComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->LCDComNum,true);
            }
            else
              {
                ShowMessage("Please choose the Comm number");
                LCDRadioGroup->ItemIndex = 0;
              }
            break;
    case THROUGH_EMBEDDED: m_SystemParams->LCDConnectionKind = THROUGH_EMBEDDED;
            LCDComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->LCDComNum != 0)
              UpdateCommList(m_SystemParams->LCDComNum,false);
            break;

    }

}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::ContainersRadioGroupClick(TObject *Sender)
{
  switch(ContainersRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->ContainersConnectionKind = NONE;
            ContainersComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->ContainersComNum != 0)
              UpdateCommList(m_SystemParams->ContainersComNum,false);
            break;
    case DIRECT: m_SystemParams->ContainersConnectionKind = DIRECT;
            if (ContainersComboBox->Text != "")
            {
                m_SystemParams->ContainersComNum =  StrToInt(ContainersComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->ContainersComNum,true);
            }
            else
              {
                ShowMessage("Please choose the Comm number");
                ContainersRadioGroup->ItemIndex = 0;
              }
            break;
    case THROUGH_EMBEDDED: m_SystemParams->ContainersConnectionKind = THROUGH_EMBEDDED;
            ContainersComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->MCBComNum != 0)
              UpdateCommList(m_SystemParams->ContainersComNum,false);
            break;

    }
}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::OCBRadioGroupClick(TObject *Sender)
{
  switch(OCBRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->OCBConnectionKind = NONE;
            OCBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->OCBComNum != 0)
              UpdateCommList(m_SystemParams->OCBComNum,false);
            break;
    case DIRECT: m_SystemParams->OCBConnectionKind = DIRECT;
            if (OCBComboBox->Text != "")
            {
              m_SystemParams->OCBComNum =  StrToInt(OCBComboBox->Text.SubString(5,1)); // get the com number from the string
              UpdateCommList(m_SystemParams->OCBComNum,true);
            }
            else
              {
                ShowMessage("Please choose the Comm number");
                OCBRadioGroup->ItemIndex = 0;
              }
            break;
    case THROUGH_EMBEDDED: m_SystemParams->OCBConnectionKind = THROUGH_EMBEDDED;
            OCBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->OCBComNum != 0)
              UpdateCommList(m_SystemParams->OCBComNum,false);
            break;

    }
}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::OHDBRadioGroupClick(TObject *Sender)
{
  switch(OHDBRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->OHDBConnectionKind = NONE;
            OHDBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->OHDBComNum != 0)
              UpdateCommList(m_SystemParams->OHDBComNum,false);
            break;
    case DIRECT: m_SystemParams->OHDBConnectionKind = DIRECT;
            if (OHDBComboBox->Text != "")
            {
                m_SystemParams->OHDBComNum =  StrToInt(OHDBComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->OHDBComNum,true);
            }
            else
              {
                ShowMessage("Please choose the Comm number");
                OHDBRadioGroup->ItemIndex = 0;
              }
            break;
    case THROUGH_EMBEDDED: m_SystemParams->OHDBConnectionKind = THROUGH_EMBEDDED;
            OHDBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->OHDBComNum != 0)
              UpdateCommList(m_SystemParams->OHDBComNum,false);
            break;
    case THROUGH_OCB: m_SystemParams->OHDBConnectionKind = THROUGH_OCB;
            OHDBComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->OHDBComNum != 0)
              UpdateCommList(m_SystemParams->OHDBComNum,false);
            break;

    }
}
//---------------------------------------------------------------------------


void TSetupForm::UpdateCommList(int ComNum, bool Used)
{
  for (unsigned i = 0; i < m_ComPortInfoList.size(); i++)
  {
    if (m_ComPortInfoList[i].ComNum == ComNum)
    {
      m_ComPortInfoList[i].Used = Used;
      break;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::FormShow(TObject *Sender)
{
  int index;
  // load the default values or the values from the ini file
  m_SystemParams->LoadAll();
  DateMaskEdit->Text = m_SystemParams->Date.Value().c_str();
  TimeMaskEdit->Text = m_SystemParams->Time.Value().c_str();
  OperNameEdit->Text = m_SystemParams->OperatorName.Value().c_str();
  System_SNEdit->Text = m_SystemParams->System_SN.Value().c_str();
  Containers_SNEdit->Text = m_SystemParams->Containers_SN.Value().c_str();
  LCD_SNEdit->Text = m_SystemParams->LCD_SN.Value().c_str();
  MCB_SNEdit->Text = m_SystemParams->MCB_SN.Value().c_str();
  OCB_SNEdit->Text = m_SystemParams->OCB_SN.Value().c_str();
  OHDB_SNEdit->Text = m_SystemParams->OHDB_SN.Value().c_str();

  if (m_SystemParams->EmbeddedComNum != 0 && m_SystemParams->EmbeddedConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->EmbeddedComNum))
    {
      index = EmbeddedComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->EmbeddedComNum.ValueAsString().c_str());
      EmbeddedComboBox->ItemIndex = index;
      EmbeddedRadioGroup->ItemIndex = m_SystemParams->EmbeddedConnectionKind;
    }
  }
  else
  {
    m_SystemParams->EmbeddedComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    EmbeddedRadioGroup->ItemIndex = m_SystemParams->EmbeddedConnectionKind;
  }

  if (m_SystemParams->MCBComNum != 0 && m_SystemParams->MCBConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->MCBComNum))
    {
      index = MCBComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->MCBComNum.ValueAsString().c_str());
      MCBComboBox->ItemIndex = index;
      MCBRadioGroup->ItemIndex = m_SystemParams->MCBConnectionKind;
    }
  }
  else
  {
    m_SystemParams->MCBComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    MCBRadioGroup->ItemIndex = m_SystemParams->MCBConnectionKind;
  }

  if (m_SystemParams->ContainersComNum != 0 && m_SystemParams->ContainersConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->ContainersComNum))
    {
      index = ContainersComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->ContainersComNum.ValueAsString().c_str());
      ContainersComboBox->ItemIndex = index;
      ContainersRadioGroup->ItemIndex = m_SystemParams->ContainersConnectionKind;
    }
  }
  else
  {
    m_SystemParams->ContainersComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    ContainersRadioGroup->ItemIndex = m_SystemParams->ContainersConnectionKind;
  }

  if (m_SystemParams->LCDComNum != 0 && m_SystemParams->LCDConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->LCDComNum))
    {
      index = LCDComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->LCDComNum.ValueAsString().c_str());
      LCDComboBox->ItemIndex = index;
      LCDRadioGroup->ItemIndex = m_SystemParams->LCDConnectionKind;
    }
  }
  else
  {
    m_SystemParams->LCDComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    LCDRadioGroup->ItemIndex = m_SystemParams->LCDConnectionKind;
  }

  if (m_SystemParams->OCBComNum != 0 && m_SystemParams->OCBConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->OCBComNum))
    {
      index = OCBComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->OCBComNum.ValueAsString().c_str());
      OCBComboBox->ItemIndex = index;
      OCBRadioGroup->ItemIndex = m_SystemParams->OCBConnectionKind;
    }
  }
  else
  {
    m_SystemParams->OCBComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    OCBRadioGroup->ItemIndex = m_SystemParams->OCBConnectionKind;
  }

  if (m_SystemParams->OHDBComNum != 0 && m_SystemParams->OHDBConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->OHDBComNum))
    {
      index = OHDBComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->OHDBComNum.ValueAsString().c_str());
      OHDBComboBox->ItemIndex = index;
      OHDBRadioGroup->ItemIndex = m_SystemParams->OHDBConnectionKind;
    }

  }
  else
  {
    m_SystemParams->OHDBComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    OHDBRadioGroup->ItemIndex = m_SystemParams->OHDBConnectionKind;
  }

  if (m_SystemParams->OCBSimComNum != 0 && m_SystemParams->OCBSimConnectionKind == DIRECT)
  {
    if(ComPortIsAvailable(m_SystemParams->OCBSimComNum))
    {
      index = OCBSimComboBox->Items->Add(AnsiString("Com ") + m_SystemParams->OCBSimComNum.ValueAsString().c_str());
      OCBSimComboBox->ItemIndex = index;
      OCBSimRadioGroup->ItemIndex = m_SystemParams->OCBSimConnectionKind;
    }
  }
  else
  {
    m_SystemParams->OCBSimComNum = 0;  // if the connnection kind is not 'Direct' the com number shold be zero
    OCBSimRadioGroup->ItemIndex = m_SystemParams->OCBSimConnectionKind;
  }



}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::OKBitBtnClick(TObject *Sender)
{
  if (!DirectoryExists(SYSTEM_ROOT_DIR + System_SNEdit->Text)) // if a new system S/N was entered
  {
    if (!CreateDir(SYSTEM_ROOT_DIR + System_SNEdit->Text))  // create a new directory for the new system
    {
      MessageDlg("Could not create the directory", mtError, TMsgDlgButtons() << mbOK,0);
      return;
    }
  }
  if (System_SNEdit->Text != m_SystemParams->System_SN.Value().c_str()) // if a new serial number was entered switch to the right INI file
  {
    m_SystemParams->AssignToINIFile((SYSTEM_ROOT_DIR + System_SNEdit->Text + SYSTEM_CONFIG_FILE_NAME).c_str());
  }


  m_SystemParams->Date = DateMaskEdit->Text.c_str();
  m_SystemParams->Time = TimeMaskEdit->Text.c_str();
  m_SystemParams->OperatorName = OperNameEdit->Text.c_str();
  m_SystemParams->System_SN = System_SNEdit->Text.c_str();
  m_SystemParams->Containers_SN = Containers_SNEdit->Text.c_str();
  m_SystemParams->LCD_SN = LCD_SNEdit->Text.c_str();
  m_SystemParams->MCB_SN = MCB_SNEdit->Text.c_str();
  m_SystemParams->OCB_SN = OCB_SNEdit->Text.c_str();
  m_SystemParams->OHDB_SN = OHDB_SNEdit->Text.c_str();
  if (EmbeddedComboBox->Text != "")
    m_SystemParams->EmbeddedComNum = StrToInt(EmbeddedComboBox->Text.SubString(5,1));
  m_SystemParams->EmbeddedConnectionKind = EmbeddedRadioGroup->ItemIndex;
  if (ContainersComboBox->Text != "")
    m_SystemParams->ContainersComNum = StrToInt(ContainersComboBox->Text.SubString(5,1));
  m_SystemParams->ContainersConnectionKind = ContainersRadioGroup->ItemIndex;
  if (LCDComboBox->Text != "")
    m_SystemParams->LCDComNum = StrToInt(LCDComboBox->Text.SubString(5,1));
  m_SystemParams->LCDConnectionKind = LCDRadioGroup->ItemIndex;
  if (MCBComboBox->Text != "")
    m_SystemParams->MCBComNum = StrToInt(MCBComboBox->Text.SubString(5,1));
  m_SystemParams->MCBConnectionKind = MCBRadioGroup->ItemIndex;
  if (OCBComboBox->Text != "")
    m_SystemParams->OCBComNum = StrToInt(OCBComboBox->Text.SubString(5,1));
  m_SystemParams->OCBConnectionKind = OCBRadioGroup->ItemIndex;
  if (OHDBComboBox->Text != "")
    m_SystemParams->OHDBComNum = StrToInt(OHDBComboBox->Text.SubString(5,1));
  m_SystemParams->OHDBConnectionKind = OHDBRadioGroup->ItemIndex;
  if (OCBSimComboBox->Text != "")
    m_SystemParams->OCBSimComNum = StrToInt(OCBSimComboBox->Text.SubString(5,1));
  m_SystemParams->OCBSimConnectionKind = OCBSimRadioGroup->ItemIndex;


}
//---------------------------------------------------------------------------

bool TSetupForm::Open(CSystemParams* SystemParams)
{
  m_SystemParams = SystemParams;
  if (ShowModal() == mrOk)
    return true;
  return false;
}
//---------------------------------------------------------------------------

void __fastcall TSetupForm::FormCreate(TObject *Sender)
{
  FindAllAvailableComPorts();
}
//---------------------------------------------------------------------------


// check if a com port is in the available port list
bool TSetupForm::ComPortIsAvailable(int ComNum)
{
  for (TComPortItemInfo *i = (&(*m_ComPortInfoList.begin())); i != (&(*m_ComPortInfoList.end())); i++)
  {
    if (i->ComNum == ComNum)
      return true;
  }
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TSetupForm::RFIDRadioGroupClick(TObject *Sender)
{
  switch(RFIDRadioGroup->ItemIndex)
  {
    case NONE: m_SystemParams->RFIDConnectionKind = NONE;
            RFIDComboBox->ItemIndex = -1;  // clear the combo box data
            if (m_SystemParams->RFIDComNum != 0)
              UpdateCommList(m_SystemParams->RFIDComNum,false);
            break;
    case DIRECT: m_SystemParams->RFIDConnectionKind = DIRECT;
            if (RFIDComboBox->Text != "")
            {
                m_SystemParams->RFIDComNum =  StrToInt(RFIDComboBox->Text.SubString(5,1)); // get the com number from the string
                UpdateCommList(m_SystemParams->RFIDComNum,true);
                CAppParams::GetInstance()->RFReadersConnection[0] = StrToInt(RFIDComboBox->Text.SubString(5,1));
                CAppParams::GetInstance()->RFReadersConnection[1] = 0;
            }
            else
              {
                ShowMessage("Please choose the Comm number");
                RFIDRadioGroup->ItemIndex = 0;
              }
            break;
    }
}
