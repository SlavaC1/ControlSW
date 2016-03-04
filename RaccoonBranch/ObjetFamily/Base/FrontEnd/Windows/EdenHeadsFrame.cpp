//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EdenHeadsFrame.h"
#include "BackEndInterface.h"
#include "QMonitor.h"
#include "FrontEndControlIDs.h"
#include "AppParams.h"
#include "FEResources.h"
#include <algorithm>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THeadsFrame *HeadsFrame;

const int   MAX_A2D_TEMP = 4095; 
const int   MIN_A2D_TEMP = 500; 


void __fastcall THeadsFrame::ReqTempOnChange(TObject *Sender)
{
   TEdit* Edit = dynamic_cast<TEdit *>(Sender);
   int i  = Edit->Tag;
   if ( Edit->Text == "" )
      return;

   int Text  = QStrToInt(Edit->Text.c_str());
   Edit->Font->Color = ((Text != HeadReqTempEditsSaved[i]) ? clRed : clBlack);
}

void __fastcall THeadsFrame::ReqTempOnExit  (TObject *Sender)
{
   TEdit* Edit = dynamic_cast<TEdit *>(Sender);
   int i = Edit->Tag;
   int Text = ((Edit->Text == "") ? HeadReqTempEditsSaved[i] : QStrToInt(Edit->Text.c_str()));
   if(Text>=MIN_A2D_TEMP && Text<=MAX_A2D_TEMP)
	 Edit->Text = QIntToStr(Text).c_str();
   else
	 Edit->Text = QIntToStr(HeadReqTempEditsSaved[i]).c_str();
}

__fastcall THeadsFrame::THeadsFrame(TComponent* Owner)
  : TFrame(Owner)
{
  m_PurgeActive = false;
  TWinControl* ParentPtr = TurnHeadHeatersButton->Parent;

  int          sortTops[TOTAL_NUMBER_OF_HEATERS];
  
  HeadsFillingGroupBox->Top  = INT_FROM_RESOURCE(IDN_HEAD_FRAME_START);
  HeadsFillingGroupBox->Font->Color  = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);

  for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
  {
     HeadHeatersA2DTPanels[i]                 = new TPanel(this);
     HeadHeatersA2DTPanels[i]->Parent         = ParentPtr;
	 HeadHeatersA2DTPanels[i]->Left           = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_LEFT);
	 HeadHeatersA2DTPanels[i]->Width          = INT_FROM_RESOURCE(IDN_HEAD_HEATERS_WIDTH);
	 HeadHeatersA2DTPanels[i]->Top            = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_TOP)+30;
     HeadHeatersA2DTPanels[i]->Height         = INT_FROM_RESOURCE(IDN_HEAD_HEATER_PANEL_HEIGHT);
     HeadHeatersA2DTPanels[i]->BevelOuter     = bvLowered;
	 HeadHeatersA2DTPanels[i]->Tag            = i;
	 HeadHeatersA2DTPanels[i]->ParentBackground = false;	

     HeadHeatersCelsiusTPanels[i]             = new TPanel(this);
     HeadHeatersCelsiusTPanels[i]->Parent     = ParentPtr;
	 HeadHeatersCelsiusTPanels[i]->Left       = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_LEFT);
     HeadHeatersCelsiusTPanels[i]->Width      = INT_FROM_RESOURCE(IDN_HEAD_HEATERS_WIDTH);
	 HeadHeatersCelsiusTPanels[i]->Top        = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_TOP);
     HeadHeatersCelsiusTPanels[i]->Height     = INT_FROM_RESOURCE(IDN_HEAD_HEATER_PANEL_HEIGHT);
     HeadHeatersCelsiusTPanels[i]->BevelOuter = bvLowered;
	 HeadHeatersCelsiusTPanels[i]->Tag        = i;
	 HeadHeatersCelsiusTPanels[i]->ParentBackground = false;

     HeadHeatersLabels[i]                     = new TLabel(this);
     HeadHeatersLabels[i]->Parent             = ParentPtr;
	 HeadHeatersLabels[i]->Left               = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_LEFT);
	 HeadHeatersLabels[i]->Top                = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_TOP)+90;
     HeadHeatersLabels[i]->Caption            = GetHeaterStr(i).c_str();
     HeadHeatersLabels[i]->Transparent        = true;
     HeadHeatersLabels[i]->Font->Color        = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);
     HeadHeatersLabels[i]->AutoSize           = true;
     HeadHeatersLabels[i]->Tag                = i;

     HeadReqTempEdits[i]                      = new TEdit(this);
     HeadReqTempEdits[i]->Parent              = ParentPtr;
     HeadReqTempEdits[i]->Tag                 = i;
	 HeadReqTempEdits[i]->Left                = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_LEFT);
	 HeadReqTempEdits[i]->Top                 = INT_FROM_RESOURCE_ARRAY(i,IDN_HEAD_HEATERS_TOP)+60;
     HeadReqTempEdits[i]->Width               = INT_FROM_RESOURCE(IDN_HEAD_HEATERS_WIDTH);
     HeadReqTempEdits[i]->OnChange            = ReqTempOnChange;
     HeadReqTempEdits[i]->OnExit              = ReqTempOnExit;
     sortTops[i]                              = HeadReqTempEdits[i]->Top; 
     
  }

  sort(sortTops, sortTops + TOTAL_NUMBER_OF_HEATERS);

  for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
  {
	  for(int j = 0; j < TOTAL_NUMBER_OF_HEATERS; j++)
      {
          if (sortTops[i] == HeadReqTempEdits[j]->Top)
          {
              HeadReqTempEdits[j]->TabOrder = i;
              break;
          }

      }
  }

  int PanelHeight=20,SpaceY = 10;
  int SpaceX      = 10;
  int PanelWidth  = (HeadsFillingGroupBox->Width- (NUM_OF_DM_CHAMBERS_THERMISTORS+1)*SpaceX)   / NUM_OF_DM_CHAMBERS_THERMISTORS;

  UpperThermistorsBevel->Parent = HeadsFillingGroupBox;
  UpperThermistorsBevel->Top = HeadsFillingGroupBox->Height/10;
  UpperThermistorsLabel->Parent = HeadsFillingGroupBox;
  UpperThermistorsLabel->Top = UpperThermistorsBevel->Top - UpperThermistorsLabel->Height-1;

  LowerThermistorsBevel->Parent = HeadsFillingGroupBox;
  LowerThermistorsBevel->Top = HeadsFillingGroupBox->Height/2;
  LowerThermistorsLabel->Parent = HeadsFillingGroupBox;
  LowerThermistorsLabel->Top = LowerThermistorsBevel->Top - LowerThermistorsLabel->Height-1;

  #ifdef OBJET_MACHINE
  const int ChamberPhisicalOffsetArray[NUM_OF_DM_CHAMBERS_THERMISTORS] = {TYPE_CHAMBER_SUPPORT, TYPE_CHAMBER_MODEL3, TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL1};
  #else
  const int ChamberPhisicalOffsetArray[NUM_OF_CHAMBERS_THERMISTORS] = {TYPE_CHAMBER_SUPPORT, TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL1};
  #endif

  for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
  {
  //Labels
     MaterialLevelLabelArray[i]                  = new TLabel(this);
	 MaterialLevelLabelArray[i]->Parent          = HeadsFillingGroupBox;
	 MaterialLevelLabelArray[i]->AutoSize        = false;
	 MaterialLevelLabelArray[i]->Height          = PanelHeight;
	 MaterialLevelLabelArray[i]->Width           = PanelWidth;
	 if(i<NUM_OF_DM_CHAMBERS_THERMISTORS)
	 {
	   MaterialLevelLabelArray[i]->Left            = SpaceX + (SpaceX + PanelWidth)*ChamberPhisicalOffsetArray[i];
	   MaterialLevelLabelArray[i]->Top             = LowerThermistorsBevel->Top+SpaceY;
	 }
	 else
	 {
	   MaterialLevelLabelArray[i]->Left            = SpaceX + (SpaceX + PanelWidth)*(i-(NUM_OF_DM_CHAMBERS_THERMISTORS-1));
	   MaterialLevelLabelArray[i]->Top             = UpperThermistorsBevel->Top+SpaceY;
	 }
	 MaterialLevelLabelArray[i]->Caption         = ChamberThermistorToStr(i).c_str();
	 MaterialLevelLabelArray[i]->Alignment       = taCenter;
	 MaterialLevelLabelArray[i]->Layout          = tlCenter;

   //Thermistors level Panel
     MaterialLevelPanelArray[i]                  = new TPanel(this);
     MaterialLevelPanelArray[i]->Parent          = HeadsFillingGroupBox;
     MaterialLevelPanelArray[i]->Left            = MaterialLevelLabelArray[i]->Left;
     MaterialLevelPanelArray[i]->Width           = MaterialLevelLabelArray[i]->Width;
	 MaterialLevelPanelArray[i]->Height          = MaterialLevelLabelArray[i]->Height;
	 MaterialLevelPanelArray[i]->Top             = MaterialLevelLabelArray[i]->Top+MaterialLevelLabelArray[i]->Height;
     MaterialLevelPanelArray[i]->BevelOuter      = bvLowered;
	 MaterialLevelPanelArray[i]->Caption         = "???";
	 MaterialLevelPanelArray[i]->ParentBackground = false;
   //Pump activation panel
     ActivePumpStatusPanelArray[i]               = new TPanel(this);
     ActivePumpStatusPanelArray[i]->Parent       = HeadsFillingGroupBox;
	 ActivePumpStatusPanelArray[i]->Left         = MaterialLevelPanelArray[i]->Left;
     ActivePumpStatusPanelArray[i]->Width        = MaterialLevelPanelArray[i]->Width;
     ActivePumpStatusPanelArray[i]->Height       = MaterialLevelPanelArray[i]->Height;
     ActivePumpStatusPanelArray[i]->Top          = MaterialLevelPanelArray[i]->Top+SpaceY+MaterialLevelPanelArray[i]->Height;
	 ActivePumpStatusPanelArray[i]->BevelOuter   = bvLowered;
     ActivePumpStatusPanelArray[i]->Caption      = "Off";
	 ActivePumpStatusPanelArray[i]->Color        = clRed;
	 ActivePumpStatusPanelArray[i]->ParentBackground = false;
  }

  LOAD_BITMAP(TurnHeadHeatersButton->Glyph, IDB_RED_LED);
  LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_RED_LED);

}
//---------------------------------------------------------------------------

// Display the current parameters for the head heaters temperature
void THeadsFrame::DisplayCurrentParameters()
{
  CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  int CurrentTempParam[TOTAL_NUMBER_OF_HEATERS];

  BackEnd->GetCurrentHeadHeatersTempParameters(CurrentTempParam, true);

  for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
  {
	 HeadReqTempEdits[i]->Text   = CurrentTempParam[i]; //need to assign to HeadReqTempEdits?
	 HeadReqTempEditsSaved[i] = QStrToInt(HeadReqTempEdits[i]->Text.c_str());
     HeadReqTempEdits[i]->Font->Color = clBlack;
  }

}
//---------------------------------------------------------------------------

void __fastcall THeadsFrame::HeadHeatersOnActionExecute(TObject *Sender)
{
  try
  {
  int RequestedTemp[TOTAL_NUMBER_OF_HEATERS];
  for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
	RequestedTemp[i] = QStrToInt(HeadReqTempEdits[i]->Text.c_str());
#ifndef OBJET_MACHINE
	RequestedTemp[i] = HeadReqTempUpDown[i]->Position;
#endif
  CBackEndInterface::Instance()->TurnHeadHeaters(true, RequestedTemp, TOTAL_NUMBER_OF_HEATERS);

	TurnHeadHeatersButton->Glyph  = NULL;
	TurnHeadHeatersButton->Action = HeadHeatersOffAction;
	LOAD_BITMAP(TurnHeadHeatersButton->Glyph, IDB_GREEN_LED);

  }
  catch(EQException& Exception)
  {    
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------

void __fastcall THeadsFrame::HeadHeatersOffActionExecute(TObject *Sender)
{
  try
  {
	CBackEndInterface::Instance()->TurnHeadHeaters(false);

	
	TurnHeadHeatersButton->Glyph = NULL;
    TurnHeadHeatersButton->Action = HeadHeatersOnAction;
	LOAD_BITMAP(TurnHeadHeatersButton->Glyph, IDB_RED_LED);

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------


void __fastcall THeadsFrame::HeadFillingOnActionExecute(TObject *Sender)
{
  try
  {
	CBackEndInterface::Instance()->TurnHeadFilling(true);

	
	TurnHeadFillingButton->Glyph = NULL;
	TurnHeadFillingButton->Action = HeadFillingOffAction;
	LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_GREEN_LED);

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}
//---------------------------------------------------------------------------

void __fastcall THeadsFrame::HeadFillingOffActionExecute(TObject *Sender)
{
  try
  {
	CBackEndInterface::Instance()->TurnHeadFilling(false);

	
	TurnHeadFillingButton->Glyph = NULL;
	TurnHeadFillingButton->Action = HeadFillingOnAction;
	LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_RED_LED);

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------

// Update status on the frame (integer version)
void THeadsFrame::UpdateStatus(int ControlID,int Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}

// Update status on the frame (float version)
void THeadsFrame::UpdateStatus(int ControlID,float Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}

// Update status on the frame (string version)
void THeadsFrame::UpdateStatus(int ControlID,QString Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}


// Message handler for the WM_STATUS_UPDATE message
void THeadsFrame::HandleStatusUpdateMessage(TMessage &Message)
{
  int Tmp;
  if (VALIDATE_HEATER_INCLUDING_PREHEATER(Tmp = (Message.WParam - FE_HEAD_HEATER_A2D_BASE)))
    HeadHeatersA2DTPanels[Tmp]->Caption = IntToStr(Message.LParam);
  else if (VALIDATE_HEATER_INCLUDING_PREHEATER(Tmp = (Message.WParam - FE_HEAD_HEATER_CELSIUS_BASE)))
    HeadHeatersCelsiusTPanels[Tmp]->Caption = IntToStr(Message.LParam);
  else if (VALIDATE_CHAMBER_THERMISTOR(Tmp = (Message.WParam - FE_CURRENT_HEAD_FILLING_BASE)))
    MaterialLevelPanelArray[Tmp]->Caption = IntToStr(Message.LParam);
  else if (VALIDATE_CHAMBER_THERMISTOR(Tmp = (Message.WParam - FE_CURRENT_HEAD_FILLING_IS_FILLED_BASE)))
  {
	if(Message.LParam==0)
	  MaterialLevelPanelArray[Tmp]->Color = clRed;
	else if(Message.LParam==1)
	  MaterialLevelPanelArray[Tmp]->Color = clLime;
	else //Thermistor is not active
	  MaterialLevelPanelArray[Tmp]->Color = clBtnFace;
  }
  else if (VALIDATE_CHAMBER_THERMISTOR(Tmp = (Message.WParam - FE_ACTIVE_PUMP_BASE)))
  {
    ActivePumpStatusPanelArray[Tmp]->Caption = ((Message.LParam) ? "On" : "Off");
	ActivePumpStatusPanelArray[Tmp]->Color   = ((Message.LParam) ? clLime : clRed);
  }
  else if (Message.WParam == FE_HEAD_FILLING_MONITOR_STATUS)
  {
      if (Message.LParam)
	  {
		
		TurnHeadFillingButton->Glyph = NULL;
		TurnHeadFillingButton->Action = HeadFillingOffAction;
		LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_GREEN_LED);
		//Repaint();
	  }
	  else
	  {
        
		TurnHeadFillingButton->Glyph = NULL;
        TurnHeadFillingButton->Action = HeadFillingOnAction;
        LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_RED_LED);
      }
  }
}
//---------------------------------------------------------------------------


// Update the current devices status
void THeadsFrame::UpdateDevicesStatus()
{
  try
  {
    CBackEndInterface* BackEnd = CBackEndInterface::Instance();
    if (BackEnd->IsHeadFillingOn())
	{
	  
	  TurnHeadFillingButton->Glyph = NULL;
	  TurnHeadFillingButton->Action = HeadFillingOffAction;
	  LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_GREEN_LED);
	}
	else
	{
	  
	  TurnHeadFillingButton->Glyph = NULL;
	  TurnHeadFillingButton->Action = HeadFillingOnAction;
      LOAD_BITMAP(TurnHeadFillingButton->Glyph, IDB_RED_LED);
    }

    if (BackEnd->IsHeadHeaterOn())
	{
	  
	  TurnHeadHeatersButton->Glyph = NULL;
	  TurnHeadHeatersButton->Action = HeadHeatersOffAction;
	  LOAD_BITMAP(TurnHeadHeatersButton->Glyph, IDB_GREEN_LED);
	}
	else
	{
	  
      TurnHeadHeatersButton->Glyph = NULL;
	  TurnHeadHeatersButton->Action = HeadHeatersOnAction;
	  LOAD_BITMAP(TurnHeadHeatersButton->Glyph, IDB_RED_LED);
    }

  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }

}

void __fastcall THeadsFrame::SaveParameterButtonClick(TObject *Sender)
{
  try
  {
	for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
	  CBackEndInterface::Instance()->SaveHeadHeaterParameter(i,QStrToInt(HeadReqTempEdits[i]->Text.c_str()));
    DisplayCurrentParameters();    
	for(int i = 0; i < TOTAL_NUMBER_OF_HEATERS; i++)
        HeadReqTempEdits[i]->Font->Color = clBlack;


  }
  catch(EQException& Exception)
  {    
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}
//---------------------------------------------------------------------------




