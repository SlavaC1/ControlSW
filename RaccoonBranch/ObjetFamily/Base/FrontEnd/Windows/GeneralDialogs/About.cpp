//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
#include "FrontEnd.h"
#include "FEResources.h"
#include "MaintenanceCountersDefs.h"
#include "Q2RTApplication.h"
#include <Registry.hpp>
#include "AppParams.h" //runtime objet
#include <fstream> 

//runtime objet
const QString MachineTypeArr[] =
{
"Eden260",
"Eden260v",
"Eden330",
"Eden500",
"Eden250",
"Eden350",
"Eden350v",
"Connex500",
"Connex350",
"Connex260",
"Objet260",
"Objet350",
"Objet500"
};

QString TAboutBox::PadNumber(int Num, int NumOfDigitsInPaddingFormat)
{
  int NumOfDigits = 0;
  int Num_tag = Num;
  
  if(Num==0) {
    NumOfDigits = 1;
  }
  else {
  
    while(Num_tag>0) {
      Num_tag /= 10;
      NumOfDigits++;
    }
  }
  
  if(NumOfDigits >= NumOfDigitsInPaddingFormat)
    return QIntToStr(Num);
	
  QString result = "";
  
  for(int i=0; i < (NumOfDigitsInPaddingFormat-NumOfDigits) ; i++)
  {
    result = result + QString("0");
  }
  
  result = result + QIntToStr(Num);
  
  return result;
}
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox;
//---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TForm(AOwner)
{
	m_MC = CMaintenanceCounters::GetInstance();
	m_AppParams = CAppParams::Instance();
	m_BackEnd = CBackEndInterface::Instance();
	m_MaintenanceMode = false;
	UpdatePMStrings();
}
//---------------------------------------------------------------------

void __fastcall TAboutBox::OKButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
/* returns the version of the machine according to the format tha the IS writes in the registry (XX.XX.XXX) */
AnsiString TAboutBox::GetVersionStringFormatted(AnsiString version)
{
  TQStringVector tokens; 
  const QString DELIMITDOT = ".";
  QString ZERO = "0";
  AnsiString result ; 
  
  Tokenize(version.c_str(), tokens, DELIMITDOT);

  tokens[0] = PadNumber( QStrToInt(tokens[0]), 2);
  tokens[1] = PadNumber( QStrToInt(tokens[1]), 2);
  tokens[2] = PadNumber( QStrToInt(tokens[2]), 3);
  
  result = (tokens[0]+ DELIMITDOT + tokens[1] + DELIMITDOT + tokens[2]).c_str();
  
  return result;
}
AnsiString TAboutBox::ReadKey(AnsiString Key, AnsiString Value)
{
  AnsiString temp;

  TRegistry *Reg = new TRegistry();
  Reg->RootKey = HKEY_LOCAL_MACHINE;
  if(Reg->KeyExists(Key)) {
    try{
      if(Reg->OpenKey(Key ,FALSE)) {
        temp = Reg->ReadString(Value);
      }
      Reg->CloseKey();
    }

    catch(ERegistryException &E) {
      delete Reg;
      return "";
    }
  }

  delete Reg;

  return temp;
}

void TAboutBox::ReadPatchesFromRegistry()
{
  AnsiString path;

  TRegistry *Reg = new TRegistry();
  Reg->RootKey = HKEY_LOCAL_MACHINE;

// we have to set the machine name specifically to each machine
// and not use 'APPLICATION_NAME' because it has to be the same string 
// that the installation puts in the registry 
//
// For example 'Connex500' and NOT 'Connex 500' (without a space)
//
  //m_sMachineType = MachineTypeArr[CAppParams::Instance()->MachineType].c_str(); //runtime objet

#if defined(OBJET_1000)
  m_sMachineType = "Objet1000";
#elif defined(OBJET_500)
  m_sMachineType = "Objet500";
#elif defined(OBJET_350)
  m_sMachineType = "Objet350";
#elif defined(OBJET_260)
  m_sMachineType = "Objet260";
#endif

  Reg->OpenKey( "SOFTWARE\\Stratasys Ltd.\\" + m_sMachineType + "\\" + GetVersionStringFormatted(VersionNum->Caption), false);


  TStringList* PatchStrings = new TStringList();
  Reg->GetKeyNames( PatchStrings );

  PatchesList->Clear();

  for(int i = 0; i < PatchStrings->Count; i++)
  {
    PatchesList->AddItem( PatchStrings->Strings[i] , &TObject());
  }

  if( PatchStrings->Count == 0 ) { // no entries
    PatchesList->AddItem( "None" , &TObject());
  }

  PatchesList->ItemIndex = 0;

  PatchesListClick( PatchesList );

  delete PatchStrings;
  delete Reg;
}

//---------------------------------------------------------------------------

void __fastcall TAboutBox::PatchesListClick(TObject *Sender)
{
  AnsiString path;
  TListBox* list = (TListBox*)Sender;

  if( list ) {
    path = "SOFTWARE\\Objet Geometries Ltd.\\" + m_sMachineType + "\\" + GetVersionStringFormatted(VersionNum->Caption) + "\\" +  (AnsiString)(list->Items->Strings[ list->ItemIndex ]);
    PatchVersion->Caption = ReadKey(path, "Version");
    PatchBuildDate->Caption = ReadKey(path, "BuildDate");
    PatchDescription->Caption = ReadKey(path, "Description");
  }
}


void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
  BuildDateLabel->Caption = __DATE__;
  BuildTimeLabel->Caption = __TIME__;

#ifdef _DEBUG
   Label12->Caption = "DEBUG Ver.";
#else
   Label12->Caption = "";
#endif

  ProductNameLabel->Caption = APPLICATION_NAME;
  ComputerNameLabel->Caption = Q2RTApplication->ComputerName.Value().c_str();

  AnsiString AppFilePath;

  // Get the file path (including the last '\')
  AppFilePath = ExtractFilePath(Application->ExeName).c_str();

  Graphics::TBitmap *TmpImage;

  try
  {
	TmpImage = new Graphics::TBitmap;
	FrontEndInterface->LoadBitmap32(TmpImage, IDB_HELP_ABOUT_IMAGE, this->Color);
	ProgramIcon->Picture->Assign(TmpImage);
  }
  catch(...)
  {
  }

  delete TmpImage;
}

void TAboutBox::UpdateVersions(void)
{
  // force MCB version update.
  m_BackEnd->GetGeneralInformation();

  VersionNum->Caption   = m_BackEnd->GetVersion(VERSION_APP).c_str();
  ReleaseNumLabel->Caption = GetReleaseNumber().c_str();
  OCBSWLabel->Caption   = m_BackEnd->GetVersion(VERSION_OCB).c_str();
  OCBA2DLabel->Caption  = m_BackEnd->GetVersion(VERSION_OCB_A2D).c_str();
  OHDBSWLabel->Caption  = m_BackEnd->GetVersion(VERSION_OHDB_SW).c_str();
  OHDBHWLabel->Caption  = m_BackEnd->GetVersion(VERSION_OHDB_HW).c_str();
  OHDBA2DLabel->Caption = m_BackEnd->GetVersion(VERSION_OHDB_A2D).c_str();
  MCBSWLabel->Caption   = m_BackEnd->GetVersion(VERSION_MCB_SW).c_str();
  MCBHWLabel->Caption   = m_BackEnd->GetVersion(VERSION_MCB_HW).c_str();
  EdenPCILabel->Caption = m_BackEnd->GetVersion(VERSION_EDEN_PCI).c_str();
  HaspAPILabel->Caption = m_BackEnd->GetVersion(VERSION_HASP_API).c_str();
  QString Label, Hint;
  m_BackEnd->GetPCIDriverVersion(Label, Hint);
  PCIDriverLabel->Caption = Label.c_str();
  PCIDriverLabel->Hint    = Hint.c_str();
  Label18->Hint           = Hint.c_str();
  ElapsedPrintTimeLabel->Caption = IntToStr(m_BackEnd->GetMaintenanceCounterElapsedSeconds(TOTAL_PRINTING_TIME_COUNTER_ID) / 3600) + " hours" ;
  MSC1HWLabel->Caption   = m_BackEnd->GetVersion(HW_FW_VERSION_MSC1).c_str(); //Elad added , I2C HW+FW Version.
  MSC2HWLabel->Caption   = m_BackEnd->GetVersion(HW_FW_VERSION_MSC2).c_str();
  }

//---------------------------------------------------------------------------

QString GetVersionDate(void)
{
   return __DATE__;
}
//---------------------------------------------------

QString GetVersionTime(void)
{
   return __TIME__;
}


void __fastcall TAboutBox::FormShow(TObject *Sender)
{
  UpdateVersions(); 

  ReadPatchesFromRegistry();
  
  UpdatePMStrings();
}
//---------------------------------------------------------------------------

void TAboutBox::UpdatePMStrings(void)
{
	ResetPM->Visible = m_MaintenanceMode;

	int SmallPM = m_AppParams->PMIntervalHours;
	int LargePM = SmallPM * 2;

	// update the PM is due label
	int PMCounter = m_BackEnd->GetMaintenanceCounterElapsedSeconds(SERVICE_COUNTER_ID) / 3600;
	if( PMCounter >= LargePM ) { //if the counter exceeded the large PM, update the PM as the large no matter what...
		SetNextPMType(false);
	}

	int PMDue = ( m_AppParams->NextPMTypeIsSmall ) ? SmallPM - PMCounter : LargePM - PMCounter;
	PMDueIn->Caption = QIntToStr( PMDue ).c_str();
	PMDueIn->Font->Color = ( PMDue < 0 ) ? clMaroon : clNavy;

	// update the PM type label
	PMType->Caption = ( m_AppParams->NextPMTypeIsSmall ) ? QIntToStr(SmallPM).c_str() : QIntToStr(LargePM).c_str();
}

QString TAboutBox::GetPMType(void)
{
   return PMType->Caption.c_str();
}

QString TAboutBox::GetPMDueIn(void)
{   
   return PMDueIn->Caption.c_str();
}
//---------------------------------------------------------------------------

void TAboutBox::SetNextPMType(bool a_PMTypeIsSmall)
{
	// set the next PM parameter
	m_AppParams->NextPMTypeIsSmall = a_PMTypeIsSmall;
	m_AppParams->SaveSingleParameter( &(m_AppParams->NextPMTypeIsSmall) );

	// set the PM warning according to the PM type
	int PMInterval = m_AppParams->PMIntervalHours;
	int PMWarning = a_PMTypeIsSmall ? PMInterval : ( PMInterval * 2 );
	m_MC->SetWarningTimeInSecs(SERVICE_COUNTER_ID, PMWarning * 3600);
	m_MC->SetUserWarning(SERVICE_COUNTER_ID, PMWarning * 3600);
	m_MC->SaveToFile();
}

void TAboutBox::SetMaintenanceMode(bool a_Mode)
{
	m_MaintenanceMode = a_Mode;
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::ResetPMMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ResetPM->BevelOuter = bvLowered;
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::ResetPMMouseUp(TObject *Sender, TMouseButton Button,
	  TShiftState Shift, int X, int Y)
{
	ResetPM->BevelOuter = bvRaised;
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::ResetPMClick(TObject *Sender)
{
	if ( m_AppParams->NextPMTypeIsSmall ) {  // if we just did a small PM
		SetNextPMType(false); // set the next PM type to be large
	}
	else {
		// if we just did a large PM
		m_MC->ResetCounter(SERVICE_COUNTER_ID); // reset the PM maintenance counter
		SetNextPMType(true); // set the next PM type to be small (this will also save the changes to file)
	}

	FrontEndInterface->UpdateStatus(FE_CLEAR_SERVICE_ALERT,0,true); // ask to turn off the maintenance warning (if it's not on then it will do nothing)

	UpdatePMStrings();
}
//---------------------------------------------------------------------------

QString TAboutBox::GetReleaseNumber()
{
	ifstream ReleaseFile;
	QString releaseNumber = "???";

	try
	{
		ReleaseFile.open("ReadMe.txt");
		getline(ReleaseFile,releaseNumber); 
	}
	catch (...)
	{
	   releaseNumber = "???";
	}
	return releaseNumber;
}
