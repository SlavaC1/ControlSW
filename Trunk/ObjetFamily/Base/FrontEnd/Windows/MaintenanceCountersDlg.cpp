//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MaintenanceCountersDlg.h"
#include "FrontEnd.h"
#include "Q2RTApplication.h"
#include <fstream>
#include <string.h>
#include "QMonitor.h"
#include "MaintenanceCounters.h"
#include "BackEndInterface.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const int SECS_IN_MINUTE = 60;
const int SECS_IN_HOUR   = 60 * 60;
const int SECS_IN_DAY    = 60 * 60 * 24;

const TColor LABEL_WARNING_COLOR = clRed;
const TColor LABEL_NORMAL_COLOR  = clBlack;

const char MAINTENANCE_HISTORY_DIR_NAME[]  = "Maintenance History";

TMaintenanceCountersForm *MaintenanceCountersForm;

//---------------------------------------------------------------------------

__fastcall TMaintenanceCountersForm::TMaintenanceCountersForm(TComponent* Owner)
  : TForm(Owner)
{}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::ResetButtonClick(TObject *Sender)
{
  TButton* Button = dynamic_cast<TButton *>(Sender);
  CMaintenanceCounters* Counters = CMaintenanceCounters::GetInstance();

  QString Msg = QFormatStr("Are you sure you want to reset the '%s' counter?", CounterNameLookup[Button->Tag]);
  // confirm that the user wants to reset the counter
  if (MessageDlg(Msg.c_str(),mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,0) != mrYes)
	return;

  if( Button->Tag >= HEAD_S0_DC_COUNTER_ID && Button->Tag <= HEAD_M0_DC_COUNTER_ID)
	 writeHeadInformation(Button->Tag);

  // Reset the counter
  Counters->ResetCounter(Button->Tag);
  TotalTimeEdits[Button->Tag]->Text = SecondsToCurrentUnitsStr(Counters->GetElapsedSeconds(Button->Tag));
  DateEdits     [Button->Tag]->Text = Counters->GetResetDateAsString(Button->Tag).c_str();
  UpdateDisplay();


}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::FormShow(TObject *Sender)
{
  CreateMaintenanceCountersForm();
  m_MaintenanceDirName = Q2RTApplication->AppFilePath.Value() + MAINTENANCE_HISTORY_DIR_NAME;
  m_EditMode           = false;
  SetEditMode(m_EditMode);
  UserWarningActionExecute(true);
  ShowHideReset(false);
  UpdateDisplay();
}
//---------------------------------------------------------------------------
void __fastcall TMaintenanceCountersForm::CloseBitBtnClick(TObject *Sender)
{
  CMaintenanceCounters::GetInstance()->SaveToFile();
  Close();
}
//---------------------------------------------------------------------------


void __fastcall TMaintenanceCountersForm::DaysMenuItemClick(TObject *Sender)
{
  DaysMenuItem->Checked = true;
  m_DispalyUnits        = duDays;
  Caption               = "Maintenance Counters (Days)";
  UpdateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::HoursMenuItemClick(TObject *Sender)
{
  HoursMenuItem->Checked = true;
  m_DispalyUnits         = duHours;
  Caption                = "Maintenance Counters (Hours)";
  UpdateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::MinutesMenuItemClick(TObject *Sender)
{
  MinutesMenuItem->Checked = true;
  m_DispalyUnits           = duMinutes;
  Caption                  = "Maintenance Counters (Minutes)";
  UpdateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::SecondsMenuItemClick(TObject *Sender)
{
  SecondsMenuItem->Checked = true;
  m_DispalyUnits           = duSeconds;
  Caption                  = "Maintenance Counters (Seconds)";
  UpdateDisplay();
}
//---------------------------------------------------------------------------

void TMaintenanceCountersForm::UpdateDisplay()
{
  // Display the counters data
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();
  TotalTimeEdits[0]->Text = SecondsToCurrentUnitsStr(Counters->GetElapsedSeconds(0));
  DateEdits     [0]->Text = Counters->GetResetDateAsString(0).c_str();
  int WarnTimeInSecs      = Counters->GetWarningTimeInSeconds(0);
  WarnTimeEdits [0]->Text = ((WarnTimeInSecs != -1) ? SecondsToCurrentUnitsStr(WarnTimeInSecs).c_str():"N/A");
  
  for (int i = 1; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
  {
	 if( IsCounterExistInDlg(i) )
	 {
		 TotalTimeEdits[i]->Text = SecondsToCurrentUnitsStr(Counters->GetElapsedSeconds(i));
		 DateEdits     [i]->Text = Counters->GetResetDateAsString(i).c_str();
		 WarnTimeInSecs = Counters->GetWarningTimeInSeconds(i);
		 WarnTimeEdits [i]->Text = ((WarnTimeInSecs != -1) ? SecondsToCurrentUnitsStr(WarnTimeInSecs).c_str():"N/A");
		 CheckBoxs[i]->Checked   = Counters->GetUserWarning(i);
		 NameLabels[i]->Font->Color = (Counters->IsWarningState(i) ? LABEL_WARNING_COLOR : LABEL_NORMAL_COLOR);
     }
  }
}

void __fastcall TMaintenanceCountersForm::FormCreate(TObject *Sender)
{
  m_ParamManager = CAppParams::Instance();
}

// Convert seconds to the current units type and return the result as string
AnsiString TMaintenanceCountersForm::SecondsToCurrentUnitsStr(long Secs)
{
  AnsiString Result;

  switch(m_DispalyUnits)
  {
    case duDays:
      Result = IntToStr(Secs / SECS_IN_DAY);
      break;

    case duHours:
      Result = IntToStr(Secs / SECS_IN_HOUR);
      break;

    case duMinutes:
      Result = IntToStr(Secs / SECS_IN_MINUTE);
      break;

    case duSeconds:
      Result = IntToStr(Secs);
      break;
  }

  return Result;
}

// Convert a string in the current units format to seconds
long TMaintenanceCountersForm::CurrentUnitsStrToSeconds(AnsiString Str)
{
  long ResultInSecs;

  if(Str == "N/A")
    return -1L;

  int StrAsInt = StrToInt(Str);

  if(StrAsInt == -1)
    return -1L;

  switch(m_DispalyUnits)
  {
    case duDays:
      ResultInSecs = StrAsInt * SECS_IN_DAY;
      break;

    case duHours:
      ResultInSecs = StrAsInt * SECS_IN_HOUR;
      break;

    case duMinutes:
      ResultInSecs = StrAsInt * SECS_IN_MINUTE;
      break;

    case duSeconds:
      ResultInSecs = StrAsInt;
      break;
  }

  return ResultInSecs;
}

void __fastcall TMaintenanceCountersForm::RestoreDefaultRestoreDefaultMenuItemClick(TObject *Sender)
{
  TEdit *Edit = dynamic_cast<TEdit *>(WarnFieldPopupMenu->PopupComponent);
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();

  if(Edit)
  {
    int CounterID = Edit->Tag;
    Counters->RestoreDefaultWarningTime(CounterID);

    // Update display
    long Secs = Counters->GetWarningTimeInSeconds(CounterID);
    Edit->Text = ((Secs != -1L) ? SecondsToCurrentUnitsStr(Secs).c_str() : "N/A");
  }
}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::EnterEditModeActionExecute(TObject *Sender)
{
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();
  m_EditMode = !m_EditMode;

  // Save the counters values (in seconds) bofore they are changed in the "secret" edit mode
  for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
    PrevCountersValue[i] = Counters->GetElapsedSeconds(i);

  // Toggle the "secret" edit mode
  SetEditMode(m_EditMode);
}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::SaveValuesActionExecute(TObject *Sender)
{
  if(!m_EditMode)
    return;

  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();

  for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
  {
	 if(IsCounterExistInDlg(i) ) 
	 {
		 if (SecondsToCurrentUnits(PrevCountersValue[i]) != QStrToInt(TotalTimeEdits[i]->Text.c_str()))
		 {
		   Counters->SetCounterTime(i,CurrentUnitsStrToSeconds(TotalTimeEdits[i]->Text));
		   PrevCountersValue[i] = CurrentUnitsStrToSeconds(TotalTimeEdits[i]->Text);
		 }
		 Counters->SetResetDateAsString(i,DateEdits[i]->Text.c_str());
		 NameLabels[i]->Font->Color = LABEL_NORMAL_COLOR;
	 }
  }
  Counters->ResetWarningState(ALL_COUNTERS);
  FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT,0,true);
  MessageDlg("Saved Ok",mtInformation,TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------

void TMaintenanceCountersForm::SetEditMode(bool EditMode)
{
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();
  for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
  {
	if( IsCounterExistInDlg(i) )
		PrevCountersValue[i] = Counters->GetElapsedSeconds(i);
  }

  for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
  {
	 if( IsCounterExistInDlg(i) )
	 {
		 TotalTimeEdits[i]->Enabled = EditMode;
		 DateEdits     [i]->Enabled = EditMode;
		 WarnTimeEdits [i]->Enabled = EditMode;
     }
  }
}

long TMaintenanceCountersForm::SecondsToCurrentUnits(long Secs)
{
  long Result;

  switch(m_DispalyUnits)
  {
    case duDays:
      Result = Secs / SECS_IN_DAY;
      break;

    case duHours:
      Result = Secs / SECS_IN_HOUR;
      break;

    case duMinutes:
      Result = Secs / SECS_IN_MINUTE;
      break;

    case duSeconds:
      Result = Secs;
      break;
  }

  return Result;

}

void __fastcall TMaintenanceCountersForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
//  HideUserWarningColumn();
    DestroyMaintenanceCountersForm();
}

//---------------------------------------------------------------------------
void __fastcall TMaintenanceCountersForm::OpenUserWarningActionExecute(TObject *Sender)
{
//  UserWarningActionExecute(true);
}

void TMaintenanceCountersForm::HideUserWarningColumn()
{
 // UserWarningActionExecute(false);
}

void __fastcall TMaintenanceCountersForm::UserWarningActionExecute(bool ToShow)
{
  // Allow only within maintenance mode
//  if (m_MaintenanceMode == false)
//     return;

  // Resizing the Form and the ScrollBox
  CountersScrollBox->Width = UserWarningLabel->Left - CountersScrollBox->Left + UserWarningLabel->Width;

  int firstCounter = 0;
  // Displaying the CheckBoxes
  for (int i = 1; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
	 if( IsCounterExistInDlg(i) )
	 {
		CheckBoxs[i]->Visible = ToShow;
		if (firstCounter == 0) firstCounter = i; 
	 }

  int BevelWidth;
  if (ToShow)
  {
	 CountersScrollBox->Width += CheckBoxs[firstCounter]->Width;
	 BevelWidth = CheckBoxs[firstCounter]->Left - NameLabels[firstCounter]->Left + CheckBoxs[firstCounter]->Width;
  }
  else
     BevelWidth = ResetButtons[firstCounter]->Left - NameLabels[firstCounter]->Left + ResetButtons[firstCounter]->Width;
  this->Width = CountersScrollBox->Width + CountersScrollBox->Left*3;

  for(vector<TBevel*>::iterator VectorIter = BevelsVector.begin(); VectorIter != BevelsVector.end(); VectorIter++)
      (*VectorIter)->Width = BevelWidth;

  // Displaying the Label
  UserWarningLabel->Visible = ToShow;
}

void TMaintenanceCountersForm::CreateMaintenanceFile(QString MaintenanceFileName)
{
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();

  ofstream *HistoryFile;
  QString   ResetDate;
  QString   CounterItem;
  QString   WarnTime;
  bool      UserWarning;
  int       WarnTimeInSeconds;
  int       ElapsedTimeInHours;

  try
  {
    try
    {
      // Search for .txt
      if(static_cast<int>(MaintenanceFileName.find_first_of('.')) == -1)
        MaintenanceFileName += ".txt";

      // Check if such file already exists...
      if (FileExists(MaintenanceFileName.c_str()) == true)
      {
        QString Message = "The file already exists. Overwrite?";

        if (MessageDlg(Message.c_str() ,mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,0) != mrYes)
          return;
      }

      // Creating the file...
      HistoryFile = new ofstream(MaintenanceFileName.c_str());

      // Initiating the file...
      *HistoryFile << "Item, Elapsed time (Hours), Reset Date, Warning Time, User Warning" << endl << endl;

	  for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
      {
		 if( IsCounterExistInDlg(i) )
		 {
			 CounterItem        = CounterNameLookup[i];
			 ElapsedTimeInHours = Counters->GetElapsedSeconds(i) / SECS_IN_HOUR;
			 ResetDate          = Counters->GetResetDateAsString(i);
			 WarnTimeInSeconds  = Counters->GetWarningTimeInSeconds(i);
			 WarnTime           = (WarnTimeInSeconds == -1) ? (QString)"N/A" : QIntToStr(WarnTimeInSeconds / SECS_IN_HOUR);
			 UserWarning        = Counters->GetUserWarning(i);
			 *HistoryFile << CounterItem << ", " << ElapsedTimeInHours << ", " << ResetDate << ", " << WarnTime << ", " << (UserWarning? "Yes":"No") << endl;
         }
	  }
    }
    catch (EQException Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }
  }
  __finally
  {
    delete HistoryFile;
  }
}

void TMaintenanceCountersForm::ShowHideReset(bool ShowReset)
{
#ifndef OBJET_MACHINE_KESHET
  ResetButtons[SERVICE_COUNTER_ID]->Visible = false; //ShowReset;
#endif  
  ResetButtons[PATTERN_TEST_COUNTER_ID]->Visible = ShowReset;
  ResetButtons[LAST_HCW_ACTIVATION_TIME_COUNTER_ID]->Visible = ShowReset;
  ResetButtons[UV_LAMPS_CALIBRATION_COUNTER_ID]->Visible = ShowReset;
  ResetButtons[HEAD_ALIGNMENT_COUNTER_ID]->Visible = ShowReset;
  ResetButtons[HOW_HS_HM_COUNTER_ID]->Visible = ShowReset;
  ResetButtons[HOW_HQ_COUNTER_ID]->Visible = ShowReset;
  ResetButtons[LOAD_CELL_CALIBRATION_COUNTER_ID]->Visible = ShowReset;

}

void __fastcall TMaintenanceCountersForm::SaveToFileMenuItemClick(TObject *Sender)
{
  // First Check if the 'Maintenance History' Directory exists...
  if (DirectoryExists(m_MaintenanceDirName.c_str()) == false)
    // Force Directory creation
    ForceDirectories(m_MaintenanceDirName.c_str());

  // Create a report file
  MaintenanceSaveDialog->InitialDir = m_MaintenanceDirName.c_str();

  if(MaintenanceSaveDialog->Execute())
    CreateMaintenanceFile(MaintenanceSaveDialog->FileName.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TMaintenanceCountersForm::ApplyWarningTimeActionExecute(TObject *Sender)
{
	// Display the counters data
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();
  try
  {
    long Secs;
	for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
    {
	   if( IsCounterExistInDlg(i) ) 
	   {
		   Secs = CurrentUnitsStrToSeconds(WarnTimeEdits[i]->Text);
		   Counters->SetWarningTimeInSecs(i,Secs);
		   Counters->SetUserWarning(i,Secs);
		   NameLabels[i]->Font->Color = LABEL_NORMAL_COLOR;
	   }
    }
    for (int i = 1; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
	   if( IsCounterExistInDlg(i) )
	   Counters->SetUserWarning(i, CheckBoxs[i]->Checked);
    Counters->SaveToFile();
    FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT,0,true);
  }
  catch(EQException& Err)
  {
      MessageDlg(Err.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
  }
  catch(EConvertError&)
  {
      MessageDlg("Invalid warning time value",mtError,TMsgDlgButtons() << mbOK,0);
  }
}
//---------------------------------------------------------------------------

void __fastcall TMaintenanceCountersForm::FormDestroy(TObject *Sender)
{
	DestroyMaintenanceCountersForm();
}

void TMaintenanceCountersForm::SaveChangesToCounters()
{
  // Display the counters data
  CMaintenanceCounters *Counters = CMaintenanceCounters::GetInstance();
  try
  {
    long Secs;
	for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
	{
	   if( IsCounterExistInDlg(i) )
	   {
		   Secs = CurrentUnitsStrToSeconds(WarnTimeEdits[i]->Text);
		   Counters->SetWarningTimeInSecs(i,Secs);
		   Counters->SetUserWarning(i,Secs);
		   NameLabels[i]->Font->Color = LABEL_NORMAL_COLOR;
	   }
    }
    for (int i = 1; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
	   if( IsCounterExistInDlg(i) )
	   	Counters->SetUserWarning(i, CheckBoxs[i]->Checked);
    Counters->SaveToFile();
    FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT,0,true);
  }
  catch(EQException& Err)
  {
      MessageDlg(Err.GetErrorMsg().c_str(),mtError,TMsgDlgButtons() << mbOK,0);
  }
  catch(EConvertError&)
  {
      MessageDlg("Invalid warning time value",mtError,TMsgDlgButtons() << mbOK,0);
  }

  for (int i = 0; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
  {
	 if( IsCounterExistInDlg(i) )
	 {
		 if (SecondsToCurrentUnits(PrevCountersValue[i]) != QStrToInt(TotalTimeEdits[i]->Text.c_str()))
		 {
		   Counters->SetCounterTime(i,CurrentUnitsStrToSeconds(TotalTimeEdits[i]->Text));
		   PrevCountersValue[i] = CurrentUnitsStrToSeconds(TotalTimeEdits[i]->Text);
		 }
		 Counters->SetResetDateAsString(i,DateEdits[i]->Text.c_str());
		 NameLabels[i]->Font->Color = LABEL_NORMAL_COLOR;
	 }
  }
  Counters->ResetWarningState(ALL_COUNTERS);
  FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT,0,true);
  MessageDlg("Saved Ok",mtInformation,TMsgDlgButtons() << mbOK,0);
}

//---------------------------------------------------------------------------


void __fastcall TMaintenanceCountersForm::btnSaveClick(TObject *Sender)
{
  SaveChangesToCounters();
  CMaintenanceCounters::GetInstance()->SaveToFile();
}
//---------------------------------------------------------------------------

void TMaintenanceCountersForm::CreateMaintenanceCountersForm()
{

   SetDisplayUnitsAtFormShow();
   map< QString,vector<int> > Groups;
   int firstCounter = 0;
   for(int i = 1/*Not including total printing time*/; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
   {
		if( IsCounterExistInDlg(i) )
		{
			Groups[ServiceGroupStrLookup[i]].push_back(i);
			if ( firstCounter == 0 ) firstCounter = i;
		}
   }

   TLabel*    Label;
   TBevel*    Bevel;
   int        Top = 10;
   short      i   = 0;
   
   
   for(map<QString, vector<int> >::iterator MapIter = Groups.begin(); MapIter != Groups.end(); MapIter++)
   {
	   Label              = new TLabel(this);
       Label->Parent      = CountersScrollBox;
       Label->Left        = 10;
       Label->Top         = Top;
       Top+=15;
       Label->Caption     = MapIter->first.c_str();
       Label->AutoSize    = true;
       Label->Alignment   = taLeftJustify;
       Label->Font->Style = TFontStyles()<< fsBold;
	   GroupLabelsVector.push_back(Label);

       Bevel              = new TBevel(this);
       Bevel->Parent      = CountersScrollBox;
       Bevel->Left        = Label->Left;
       Bevel->Top         = Top;
       Bevel->Width       = 450;
       Bevel->Height      = 1;
       Bevel->Style       = bsLowered;
       Bevel->Shape       = bsTopLine;
       BevelsVector.push_back(Bevel);
       Top+=15;
       
	   for(vector<int>::iterator VectorIter = MapIter->second.begin(); VectorIter != MapIter->second.end(); VectorIter++)
       {
          NameLabels[*VectorIter]                  = new TLabel(this);
          NameLabels[*VectorIter]->Parent          = CountersScrollBox;
          NameLabels[*VectorIter]->Left            = 10;
          NameLabels[*VectorIter]->Top             = Top;
          NameLabels[*VectorIter]->Caption         = CounterNameLookup[*VectorIter];
          NameLabels[*VectorIter]->AutoSize        = true;
          NameLabels[*VectorIter]->Alignment       = taLeftJustify;

          TotalTimeEdits[*VectorIter]              = new TEdit(this);
          TotalTimeEdits[*VectorIter]->Parent      = CountersScrollBox;
          TotalTimeEdits[*VectorIter]->Left        = 119;
          TotalTimeEdits[*VectorIter]->Top         = Top;
          TotalTimeEdits[*VectorIter]->Width       = 65;
          TotalTimeEdits[*VectorIter]->TabOrder    = i++;
          TotalTimeEdits[*VectorIter]->Tag         = *VectorIter;

          DateEdits[*VectorIter]                   = new TEdit(this);
          DateEdits[*VectorIter]->Parent           = CountersScrollBox;
          DateEdits[*VectorIter]->Left             = 192;
          DateEdits[*VectorIter]->Top              = Top;
          DateEdits[*VectorIter]->Width            = 73;
          DateEdits[*VectorIter]->TabOrder         = i++;
          DateEdits[*VectorIter]->Tag              = *VectorIter | 0x100;

          WarnTimeEdits[*VectorIter]               = new TEdit(this);
          WarnTimeEdits[*VectorIter]->Parent       = CountersScrollBox;
          WarnTimeEdits[*VectorIter]->Left         = 273;
          WarnTimeEdits[*VectorIter]->Top          = Top;
          WarnTimeEdits[*VectorIter]->Width        = 73;
          WarnTimeEdits[*VectorIter]->TabOrder     = i++;
          WarnTimeEdits[*VectorIter]->Tag          = *VectorIter | 0x200;

          ResetButtons[*VectorIter]                = new TButton(this);
          ResetButtons[*VectorIter]->Caption       = "Reset";
          ResetButtons[*VectorIter]->Parent        = CountersScrollBox;
          ResetButtons[*VectorIter]->Left          = 356;
          ResetButtons[*VectorIter]->Top           = Top;
          ResetButtons[*VectorIter]->Width         = 75;
          ResetButtons[*VectorIter]->TabOrder      = i++;
          ResetButtons[*VectorIter]->Tag           = *VectorIter;
          ResetButtons[*VectorIter]->OnClick       = ResetButtonClick;

          CheckBoxs[*VectorIter]                   = new TCheckBox(this);
          CheckBoxs[*VectorIter]->Parent           = CountersScrollBox;
          CheckBoxs[*VectorIter]->Left             = 448;
          CheckBoxs[*VectorIter]->Top              = Top;
          CheckBoxs[*VectorIter]->Width            = 20;
          CheckBoxs[*VectorIter]->TabOrder         = i++;
          CheckBoxs[*VectorIter]->Tag              = *VectorIter;
                                                   
		  Top+=30;
       }
   }

   TotalTimeEdits[0] = PrintingHoursEdit;
   TotalTimeEdits[0]->Left = CountersScrollBox->Left + TotalTimeEdits[firstCounter]->Left;
   TotalTimeLabel->Left = TotalTimeEdits[0]->Left;

   DateEdits     [0] = PrintingDateEdit;
   DateEdits[0]->Left = CountersScrollBox->Left + DateEdits[firstCounter]->Left;
   DatedLabel->Left = DateEdits[0]->Left;

   WarnTimeEdits [0] = PrintingWarnEdit;
   WarnTimeEdits[0]->Left = CountersScrollBox->Left + WarnTimeEdits[firstCounter]->Left;
   WarningTimeLabel->Left = WarnTimeEdits[0]->Left;

   NameLabels    [0] = PrintingHoursLabel;
   NameLabels[0]->Left = CountersScrollBox->Left + NameLabels[firstCounter]->Left;
   ItemLabel->Left = NameLabels[0]->Left;

   m_destroyed = false;
}

void TMaintenanceCountersForm::DestroyMaintenanceCountersForm()
{
   if(!m_destroyed)
   {
		for(int i = 1/*Not including total printing time*/; i < NUM_OF_MAINTENANCE_COUNTERS; i++)
	   {
		  if( IsCounterExistInDlg(i) )
		  {	  delete TotalTimeEdits[i];
			  delete DateEdits[i];
			  delete WarnTimeEdits[i];
			  delete CheckBoxs[i];
			  delete NameLabels[i];
			  delete ResetButtons[i];
		  }
	   }
	   for(vector<TBevel*>::iterator VectorIter = BevelsVector.begin(); VectorIter != BevelsVector.end(); VectorIter++)
		  delete *VectorIter;
	   for(vector<TLabel*>::iterator VectorIter = GroupLabelsVector.begin(); VectorIter != GroupLabelsVector.end(); VectorIter++)
		  delete *VectorIter;

	   BevelsVector.clear();
	   GroupLabelsVector.clear();
	   m_destroyed = true;
   }
}

void TMaintenanceCountersForm::SetDisplayUnitsAtFormShow()
{
  if(DaysMenuItem->Checked)
  {
	m_DispalyUnits        = duDays;
	Caption               = "Maintenance Counters (Days)";
  }
  else if(MinutesMenuItem->Checked)
  {
	m_DispalyUnits           = duMinutes;
	Caption                  = "Maintenance Counters (Minutes)";
  }
  else if(SecondsMenuItem->Checked)
  {
	m_DispalyUnits           = duSeconds;
	Caption                  = "Maintenance Counters (Seconds)";
  }
  else //HoursMenuItem->Checked
  {
    m_DispalyUnits         = duHours;
	Caption                = "Maintenance Counters (Hours)";
  }

}

bool TMaintenanceCountersForm::IsCounterExistInDlg( int i)
{
    if( ( ServiceIsAdvancedLookup[i] == false ||  m_ParamManager->ShowAdvancedCounters ) && //hide advanced counters if not service engineer
				CounterNameLookup[i][0] != '\0' )
		return true;
	else
		return false;
}

void TMaintenanceCountersForm::writeHeadInformation(int counterID)
{
	int headnum;
	THeadParameters  HeadParams ={0};;
	QString HeadReplacementLog;
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	for(headnum =0; headnum < TOTAL_NUMBER_OF_HEADS_HEATERS; headnum++)
		if(counterID == HeadsCounter_DC_ID[headnum])      break;

	  try
	  {
		 BackEnd->ReadHeadParametersFromE2PROM(headnum, &HeadParams);

		 HeadReplacementLog = QFormatStr("Manual Reset - %s was taken out. Replaced by S/N: %s. Previous Drop counter : %lu",
														GetHeadName(headnum).c_str(),
														HeadParams.SerialNumber,
												BackEnd->GetMaintenanceCounterElapsedSeconds(counterID));
	  }
	  catch(EQException& err)
	  {
		HeadReplacementLog = QFormatStr("Manual Reset of Head %s. Couldn't get head information due to communication error",GetHeadName(headnum).c_str());
	  }
	  BackEnd->WriteToHOWHistoryFile(HeadReplacementLog);
}
