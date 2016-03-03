//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "QApplication.h"
#include "Q2RTApplication.h"
#include "QFileParamsStream.h"
#include "MainUnit.h"
#include "QMonitor.h"
#include "QMonitorDlg.h"
#include "AppParams.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern int AppRetVal;

const int NUM_OF_CMD_LINE_PARAMS_NEEDED = 3;
const int OP_MODE_INDEX  = 1;
const int IN_FILE_INDEX  = 2;
const int OUT_FILE_INDEX = 3;

#define FILE_NOT_FOUND                      10
#define IN_FILE_LOADING_PROBLEM             11
#define PARAMETERS_IMPORT_FAILED            12
#define IMPORT_EXPORT_OPERATION_NOT_DEFINED 13
#define INVALID_NUMBER_OF_ARGUMENTS         14

TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner),
          m_Silent(false)
{
  m_MyQApp = NULL;
  m_TempParamsMgr = NULL;
  m_ParamsMgr = NULL;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CloseButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  m_MyQApp = new CQApplication;
  m_MyQApp->Init();
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

void TMainForm::AddParamForExport(CQParameterBase *Param)
{
  // Check if not already in selection set
  if(ForExportListBox->Items->IndexOf(Param->Name().c_str()) == -1)
    ForExportListBox->Items->AddObject(Param->Name().c_str(),(TObject *)Param);
}

void __fastcall TMainForm::FormShow(TObject *Sender)
{

  m_MyQApp->Start();

  // Initialize the QMonitor form when the QApplication is initialized
  TQMonitorForm::Init();
  int  Params         = ParamCount();
  bool SwitchesInLine = false;
  if (ParamStr(Params) == "\\Q")
  {
      m_Silent       = true;
      SwitchesInLine = true;
  }

  if (Params == NUM_OF_CMD_LINE_PARAMS_NEEDED || (Params == (NUM_OF_CMD_LINE_PARAMS_NEEDED+1) && SwitchesInLine))
  {
    // Prepare the import file name
    QString ImportFileName = ParamStr(IN_FILE_INDEX).c_str();

    if (!FileExists(ParamStr(IN_FILE_INDEX)))
    {
      AppRetVal = FILE_NOT_FOUND;
      if (m_Silent == false)
      {
        QString Err = "Could not open the file " +  ImportFileName;
        MessageDlg(Err.c_str(),mtError,TMsgDlgButtons() << mbOK,0);
      }
      Application->Terminate();
      return;
    }

    QString ParametersFileName = ParamStr(OUT_FILE_INDEX).c_str();
//    ParametersFileName += "\\q2rt.cfg";
    if (!FileExists(ParametersFileName.c_str()))
    {
      AppRetVal = FILE_NOT_FOUND;
      if (m_Silent == false)
      {
        QString Err = "Could not open the file " +  ParametersFileName;
        MessageDlg(Err.c_str(),mtError,TMsgDlgButtons() << mbOK,0);
      }
      Application->Terminate();
      return;
    }
//    ParametersFileName = ParamStr(OUT_FILE_INDEX).c_str();
//    ParametersFileName  += "\\";

    if(ParamStr(OP_MODE_INDEX) == "export")
    {
      ForExportListBox->Clear();
      CQParamsFileStream *FileStream = NULL;

      // Prepare a parameters file stream and import the stream
      try
      {
        FileStream = new CQParamsFileStream(ImportFileName);

        try
        {
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
		  //Silent - means if to promt user(in Export import) : to generate new checksum or do it in a quiet mode. 
		  m_ParamsMgr = CAppParams::Init(ParametersFileName,true, m_Silent);

          // Update the parameters in the temporary set
          for(int i = 0; i < m_TempParamsMgr->PropertyCount(); i++)
          {
            CQParameterBase *Param = dynamic_cast<CQParameterBase *>(m_TempParamsMgr->Properties[i]);
            Param->AssignFromString(m_ParamsMgr->Properties[i]->ValueAsString());
          }

          ExportFile(ParamStr(IN_FILE_INDEX));
          Application->Terminate();

        }
        catch(EQException& E)
        {
          AppRetVal = IN_FILE_LOADING_PROBLEM;
          if (m_Silent == false)
            MessageDlg(E.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
        }

      } __finally
        {
          delete FileStream;
        }
    }
    else if (ParamStr(OP_MODE_INDEX) == "import")
    {
      try
      {

		TInitStatus_E StatusInit = INIT_FAIL_E; //init
		m_ParamsMgr = new CAppParams(ParametersFileName, StatusInit, false);
        m_ParamsMgr->LoadAll();

        CQParamsFileStream *FileStream = new CQParamsFileStream(ParamStr(IN_FILE_INDEX).c_str());
        m_ParamsMgr->Import(FileStream,false);
		TQErrCode saveAllParameters(CAppParams *); // This opens a backdoor to SaveAll function. See comments in CAppParams declaration
		saveAllParameters(m_ParamsMgr);
        delete FileStream;
      }
      catch(EQException& Err)
      {
         AppRetVal = PARAMETERS_IMPORT_FAILED;
         if (m_Silent == false)
           MessageDlg("Parameters import failed",mtError,TMsgDlgButtons() << mbOK,0);
      }
   }
    else if (ParamStr(OP_MODE_INDEX) == "add")
    {
      try
      {

		TInitStatus_E StatusInit = INIT_FAIL_E; //init
		m_ParamsMgr = new CAppParams(ParametersFileName, StatusInit, false);
        m_ParamsMgr->LoadAll();

        CQParamsFileStream *FileStream = new CQParamsFileStream(ParamStr(IN_FILE_INDEX).c_str());
        m_ParamsMgr->Import(FileStream,false);
		TQErrCode saveAllParametersRestrictedToImportStream(CAppParams *, CQParamsStream *); // This opens a backdoor to SaveAll function. See comments in CAppParams declaration
		saveAllParametersRestrictedToImportStream(m_ParamsMgr, FileStream);
        delete FileStream;
      }
      catch(EQException& Err)
      {
         AppRetVal = PARAMETERS_IMPORT_FAILED;
         if (m_Silent == false)
           MessageDlg("Parameters add failed",mtError,TMsgDlgButtons() << mbOK,0);
      }
   }
    else
    {
      AppRetVal = IMPORT_EXPORT_OPERATION_NOT_DEFINED;
      if (m_Silent == false)
        MessageDlg("No operation (import/export) was selected, or operation is not defined.",mtError,TMsgDlgButtons() << mbOK,0);
    }
  }
  else
  {
    AnsiString Err;
    bool Help = true;
    if (ParamStr(OP_MODE_INDEX) != "help")
    {
      Help = false;
      Err = "Invalid number of parameters (" + IntToStr(ParamCount()) + " instead of " + IntToStr(NUM_OF_CMD_LINE_PARAMS_NEEDED) + ")\n\n";
      AppRetVal = INVALID_NUMBER_OF_ARGUMENTS;
    }
      
    if (m_Silent == false)
    {
       Err += "Export usage:\nParametersExportAndImport export [Specefic machine parameters file] [Source file for export]\n\n";
       Err += "Import usage:\nParametersExportAndImport import [Import parametres file] [Destination file for import]";
       MessageDlg(Err,Help? mtInformation: mtError,TMsgDlgButtons() << mbOK,0);
    }
  }

  Application->Terminate();
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
 if(m_MyQApp)
 { 
    delete m_MyQApp;
  }
 if(m_TempParamsMgr)
 {
   delete m_TempParamsMgr;
  }
 if(m_ParamsMgr)
  {
    delete m_ParamsMgr;
  }
}
//---------------------------------------------------------------------------

void TMainForm::ExportFile(const AnsiString FileName)
{
  // Try to delete the current file
  DeleteFile(FileName.c_str());

  // Create a file params stream
  CQParamsFileStream *FileStream = new CQParamsFileStream(FileName.c_str());

  try
  {
    for(int i = 0; i < ForExportListBox->Items->Count; i++)
      m_TempParamsMgr->SaveSingleParameter((CQParameterBase *)ForExportListBox->Items->Objects[i],FileStream);

  } __finally
    {
      delete FileStream;
    }
}

TQErrCode saveAllParameters(CAppParams *p)
{
	return p->SaveAll();
}

TQErrCode saveAllParametersRestrictedToImportStream(CAppParams *p, CQParamsStream *ImportStream)
{
	return p->SaveAllRestrictedToImportStream(ImportStream);
}
