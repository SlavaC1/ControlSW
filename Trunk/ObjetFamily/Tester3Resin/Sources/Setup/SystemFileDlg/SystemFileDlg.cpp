//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SystemFileDlg.h"
#include "SetupDlg.h"
#include "QTypes.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSystemFileForm *SystemFileForm;
//---------------------------------------------------------------------------
__fastcall TSystemFileForm::TSystemFileForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSystemFileForm::FormShow(TObject *Sender)
{
  bool SystemDirExist = false;

  if (m_LastWorkingDir != "")
  {
    DirectoryListBox->Directory = m_LastWorkingDir;
    return;
  }

  for(char drive = 'c'; drive <= 'c'; drive++)
  {
    DriveComboBox->Drive = drive;
    if (LookForSystemDir())
    {
      SystemDirExist = true; // found a system directory
      break;
    }
  }
  if (!SystemDirExist)
  {
    DirectoryListBox->Directory = "\\";   // change the directory to the root directory
    ApproveBitBtn->Caption = "New";
  }

}
//---------------------------------------------------------------------------


bool TSystemFileForm::LookForSystemDir()
{
  AnsiString DriveName  = DriveComboBox->Drive;
  AnsiString DirName  =  DriveName + SYSTEM_DIRECTORY_NAME;
   if (DirectoryExists(DirName))
   {
     DirectoryListBox->Directory = DirName;
     return true;
   }
   else
    return false;

}
//---------------------------------------------------------------------------

void __fastcall TSystemFileForm::DriveComboBoxChange(TObject *Sender)
{
  LookForSystemDir();
}
//---------------------------------------------------------------------------

void __fastcall TSystemFileForm::ApproveBitBtnClick(TObject *Sender)
{
  AnsiString DriveName  = DriveComboBox->Drive;
  AnsiString DirName  =  DriveName + SYSTEM_DIRECTORY_NAME;

  if (ApproveBitBtn->Caption == "OK")
  {

    // check if the user selected a system directory
      QString Directory = DirectoryListBox->Directory.c_str();
      if ( Directory.find(SYSTEM_DIRECTORY_NAME) == -1)
        MessageDlg("Choose a system directory", mtError, TMsgDlgButtons() << mbOK,0);
     else
       {
        m_SelectedDiretory = DirectoryListBox->Directory.c_str();
        ModalResult = mrOk;
       }
  }
  else
  if (ApproveBitBtn->Caption == "New")
  {

    MessageDlg("Create directory " + DirName, mtConfirmation, TMsgDlgButtons() << mbOK,0);
    if (!CreateDir(DirName))
      {
        MessageDlg("Could not create the directory", mtError, TMsgDlgButtons() << mbOK,0);
        return;
      }
    DirectoryListBox->Directory = DirName;
    m_SelectedDiretory = DirectoryListBox->Directory.c_str();
    ApproveBitBtn->Caption = "OK";
    ModalResult = mrOk;

  }
}
//---------------------------------------------------------------------------

QString TSystemFileForm::GetSelectedDirectory()
{
  return m_SelectedDiretory;
}
//---------------------------------------------------------------------------

bool  TSystemFileForm::Open(AnsiString LastWorkingDirectory)
{
  m_LastWorkingDir = LastWorkingDirectory;
  if(ShowModal() == mrOk)
    return true;
  return false;
}
//---------------------------------------------------------------------------

