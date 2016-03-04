//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ParamsExportDlg.h"
#include "ParamsDialog.h"
#include "AppParams.h"
#include "QFileParamsStream.h"
#include "Q2RTApplication.h"
#include "BackEndInterface.h"
#include "MainUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


TParamsExportForm *ParamsExportForm;
//---------------------------------------------------------------------------
__fastcall TParamsExportForm::TParamsExportForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::CloseButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::FormCreate(TObject *Sender)
{
  TInitStatus_E StatusInit = INIT_FAIL_E; //init
  m_TempParamsMgr = new CAppParams("", StatusInit, false);

  // Prepare the parameters list box
  for(int i = 0; i < m_TempParamsMgr->PropertyCount(); i++)
  {
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(m_TempParamsMgr->Properties[i]);
    AllParamsListBox->Items->AddObject(Param->Name().c_str(),(TObject *)Param);

    Param->AddAttribute(paHidden);
  }
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::AllButtonClick(TObject *Sender)
{
  for(int i = 0; i < AllParamsListBox->Items->Count; i++)
    AllParamsListBox->Selected[i] = true;

  ModesComboBox->ItemIndex = 0;
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::NoneButtonClick(TObject *Sender)
{
  for(int i = 0; i < AllParamsListBox->Items->Count; i++)
    AllParamsListBox->Selected[i] = false;

  ModesComboBox->ItemIndex = 0;
  SaveButton->Enabled = false;  
}
//---------------------------------------------------------------------------

void TParamsExportForm::AddParamForExport(CQParameterBase *Param)
{
  // Check if not already in selection set
  if(ForExportListBox->Items->IndexOf(Param->Name().c_str()) == -1)
    ForExportListBox->Items->AddObject(Param->Name().c_str(),(TObject *)Param);
}

void __fastcall TParamsExportForm::AddToSelectionButtonClick(TObject *Sender)
{
  bool SomethingWasAdded = false;
  CAppParams *ParamsMgr = CAppParams::Instance();

  for(int i = 0; i < AllParamsListBox->Items->Count; i++)
    if(AllParamsListBox->Selected[i])
    {
      CQParameterBase *Param = (CQParameterBase *)AllParamsListBox->Items->Objects[i];

      CQParameterBase *OriginalParam = (CQParameterBase *)ParamsMgr->FindProperty(Param->Name());

      // Sanity check, this value should never be NULL
      if(OriginalParam != NULL)
      {
        Param->AssignFromString(OriginalParam->ValueAsString());
        AddParamForExport(Param);
        SomethingWasAdded = true;
      }
    }

  if(SomethingWasAdded)
  {
    ModesComboBox->ItemIndex = 0;
    SaveButton->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::RemoveFromSelectionButtonClick(TObject *Sender)
{
  bool SomethingWasRemoved = false;

  while(ForExportListBox->SelCount > 0)
    for(int i = 0; i < ForExportListBox->Items->Count; i++)
      if(ForExportListBox->Selected[i])
      {
        SomethingWasRemoved = true;
        ForExportListBox->Items->Delete(i);
        break;
      }

  if(SomethingWasRemoved)
  {
    ModesComboBox->ItemIndex = 0;
    SaveButton->Enabled = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::FormShow(TObject *Sender)
{
  ModesComboBox->ItemIndex = 0;
  SaveButton->Enabled = false;
  ForExportListBox->Clear();

  for(int i = 0; i < AllParamsListBox->Items->Count; i++)
    AllParamsListBox->Selected[i] = false;

  CAppParams *ParamsMgr = CAppParams::Instance();

  // Update the parameters in the temporary set
  for(int i = 0; i < m_TempParamsMgr->PropertyCount(); i++)
  {
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(m_TempParamsMgr->Properties[i]);
    Param->AssignFromString(ParamsMgr->Properties[i]->ValueAsString());
  }

  RefreshModesList();
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::ForExportListBoxDragOver(
      TObject *Sender, TObject *Source, int X, int Y, TDragState State,
      bool &Accept)
{
  Accept = true;
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::ForExportListBoxDragDrop(
      TObject *Sender, TObject *Source, int X, int Y)
{
  AddToSelectionButton->Click();
}
//---------------------------------------------------------------------------

void TParamsExportForm::SelectParamByName(AnsiString ParamName)
{
  int i = AllParamsListBox->Items->IndexOf(ParamName);

  if(i != -1)
    AllParamsListBox->Selected[i] = true;
}

void __fastcall TParamsExportForm::SaveAsButtonClick(TObject *Sender)
{
  // If the selection set is empty, do nothing
  // If selection set is empty, do nothing
  if(ForExportListBox->Items->Count == 0)
    return;

  // Set default save dialog directory to the "modes" directory
  SaveDialog1->InitialDir = (Q2RTApplication->AppFilePath.Value() + "Modes\\" + m_ModesDirName).c_str();

  if(SaveDialog1->Execute())
    ExportFile(SaveDialog1->FileName.c_str());
  else
    {
      // Refresh modes list after openning save dialog
      CBackEndInterface::Instance()->RefreshModesList();

      // Refresh modes display
      RefreshModesList();
    }
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::AllParamsListBoxDblClick(TObject *Sender)
{
  AddToSelectionButton->Click();
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::Clear1Click(TObject *Sender)
{
  ForExportListBox->Clear();
  ModesComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::FormDestroy(TObject *Sender)
{
  delete m_TempParamsMgr;
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::EditCollectionButtonClick(TObject *Sender)
{
  // If selection set is empty, do nothing
  if(ForExportListBox->Items->Count == 0)
    return;

  TParamsDialog *ParamsDlg = new TParamsDialog;
  ParamsDlg->ButtonsMode = pbApplyClose;

  try
  {
    // Unhide parameters in export set
    for(int i = 0; i < ForExportListBox->Items->Count; i++)
    {
      CQParameterBase *Param = (CQParameterBase *)ForExportListBox->Items->Objects[i];
      Param->RemoveAttribute(paHidden);
    }

    // Build the dialog layout and open
    ParamsDlg->AddParamsContainer(m_TempParamsMgr);
    ParamsDlg->Execute();

    // Hide parameters in export set
    for(int i = 0; i < ForExportListBox->Items->Count; i++)
    {
      CQParameterBase *Param = (CQParameterBase *)ForExportListBox->Items->Objects[i];
      Param->AddAttribute(paHidden);
    }

  } __finally
    {
      delete ParamsDlg;
    }
}
//---------------------------------------------------------------------------

void TParamsExportForm::RefreshModesList(void)
{
  CBackEndInterface *BackEndInterface = CBackEndInterface::Instance();
  QString ModeName;

  BackEndInterface->BeginModesEnumeration();

  ModesComboBox->Clear();
  ModesComboBox->Items->Add("- None -");

  if(!VALIDATE_QUALITY_MODE(m_MaintenanceMode) ||
     !VALIDATE_OPERATION_MODE(m_OperationMode))
      m_ModesDirName = GENERAL_MODES_DIR;
  else
      m_ModesDirName = MACHINE_QUALITY_MODES_DIR(m_MaintenanceMode,m_OperationMode);



  while((ModeName = BackEndInterface->GetNextModeName(m_ModesDirName)) != "")
    ModesComboBox->Items->Add(ModeName.c_str());

  ModesComboBox->ItemIndex = 0;
}

void __fastcall TParamsExportForm::ModesComboBoxClick(TObject *Sender)
{
  ForExportListBox->Clear();

  if(ModesComboBox->ItemIndex < 1)
  {
    SaveButton->Enabled = false; 
    return;
  }

  // Prepare the import file name
  QString ImportFileName = Q2RTApplication->AppFilePath.Value() + "Modes\\" + m_ModesDirName + "\\" + ModesComboBox->Text.c_str() + ".cfg";

  CQParamsFileStream *FileStream = NULL;

  // Prepare a parameters file stream and import the stream
  try
  {
    FileStream = new CQParamsFileStream(ImportFileName);

    m_TempParamsMgr->Import(FileStream,true);

    // Transfer the imported parameters to the selection set
    for(int i = 0; i < m_TempParamsMgr->PropertyCount(); i++)
    {
      CQParameterBase *Param = dynamic_cast<CQParameterBase *>(m_TempParamsMgr->Properties[i]);
      if(!Param->IsStackEmpty())
      {
        Param->CollapseStack();
        AddParamForExport(Param);
      }
    }

    // Enable the save button
    SaveButton->Enabled = true;

  } __finally
    {
      delete FileStream;
    }
}
//---------------------------------------------------------------------------


void __fastcall TParamsExportForm::OpenParametersDialogActionExecute(TObject *Sender)
{
  MainForm->OpenParamsDialogAction->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TParamsExportForm::SaveButtonClick(TObject *Sender)
{
  // Check if a mode is currently selected
  if(ModesComboBox->ItemIndex != 0)
  {
    AnsiString Str = Q2RTApplication->AppFilePath.Value().c_str();

    // Prepare the import file name
    AnsiString ModeFileName = Str + "Modes\\" + m_ModesDirName.c_str() + "\\" + ModesComboBox->Text + ".cfg";

    ExportFile(ModeFileName);
  }
}
//---------------------------------------------------------------------------

void TParamsExportForm::ExportFile(const AnsiString FileName)
{
  // Try to delete the current file
  DeleteFile(FileName.c_str());

  // Create a file params stream
  CQParamsFileStream *FileStream = new CQParamsFileStream(FileName.c_str());

  try
  {
    for(int i = 0; i < ForExportListBox->Items->Count; i++)
      m_TempParamsMgr->SaveSingleParameter((CQParameterBase *)ForExportListBox->Items->Objects[i],FileStream);

    // Refresh modes list after save
    CBackEndInterface::Instance()->RefreshModesList();

    // Refresh modes display
    RefreshModesList();

    // Set the current item in the modes combo to match the new exported file
    QString ModeNameOnly = QExtractFileNameWithoutExt(FileName.c_str());
    int NewItemIndex = ModesComboBox->Items->IndexOf(ModeNameOnly.c_str());

    if(NewItemIndex != -1)
    {
      ModesComboBox->ItemIndex = NewItemIndex;
      SaveButton->Enabled = true;
    } else
      {
        ModesComboBox->ItemIndex = 0;
        SaveButton->Enabled = false;
      }

  } __finally
    {
      delete FileStream;
    }
}

//---------------------------------------------------------------------------

void TParamsExportForm::SetMaintenanceMode(TQualityModeIndex QualityMode, TOperationModeIndex OperationMode)
{
  m_MaintenanceMode = QualityMode;
  m_OperationMode   = OperationMode;

  // Update the 'Modes' List
  RefreshModesList();
}

