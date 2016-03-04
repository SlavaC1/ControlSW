//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainStatusScreen.h"
#include "MainUnit.h"
#include "FrontEndControlIDs.h"
#include "BackEndInterface.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LCDBaseScreen"
#pragma link "EnhancedLabel"
#pragma link "EnhancedProgressBar"
#pragma link "ThreeLayersGraphic"
#pragma link "EnhancedMenu"

#pragma resource "*.dfm"
#include "FEResources.h"

const TColor ACTIVE_CONTAINER_COLOR          = LOAD_COLOR(IDC_ACTIVE_CONTAINER);
const TColor INACTIVE_CONTAINER_COLOR        = LOAD_COLOR(IDC_INACTIVE_CONTAINER);
const TColor DEFAULT_BACKGROUND_COLOR        = LOAD_COLOR(IDC_DEFAULT_BACKGROUND_CONTAINER);
const TColor INACTIVE_BACKGROUND_COLOR       = LOAD_COLOR(IDC_DEFAULT_INACTIVE_CONTAINER);
const TColor CONTAINER_OUT_BACKGROUND_COLOR  = TRANSPARENT_COLOR;
const TColor APPLICATION_RED_COLOR           = LOAD_COLOR(IDC_APPLICATION_RED);

//---------------------------------------------------------------------------
__fastcall TMainStatusFrame::TMainStatusFrame(TComponent* Owner)
        : TLCDBaseFrame(Owner)
{
  m_DrainTimers[0] = S2DrainTimer;
  m_DrainTimers[1] = S1DrainTimer;
  m_DrainTimers[2] = M2DrainTimer;
  m_DrainTimers[3] = M1DrainTimer;

  TWinControl* ParentPtr = PrintProgressBar->Parent;
  int          TankWidth = INT_FROM_RESOURCE(IDN_GRAPHIC_TANK_WIDTH);
  int          TankLeft, TankTop;

  Width  = INT_FROM_RESOURCE(IDN_MAIN_STATUS_SCREEN_WIDTH);
  Height = INT_FROM_RESOURCE(IDN_MAIN_STATUS_SCREEN_HEIGHT);

  Panel1->Width  = INT_FROM_RESOURCE(IDN_MAIN_STATUS_SCREEN_WIDTH);
  Panel1->Height = INT_FROM_RESOURCE(IDN_MAIN_STATUS_SCREEN_HEIGHT);

  PaintBox1->Width  = INT_FROM_RESOURCE(IDN_MAIN_STATUS_SCREEN_WIDTH);
  PaintBox1->Height = INT_FROM_RESOURCE(IDN_MAIN_STATUS_SCREEN_HEIGHT);

  for(int i = FIRST_TANK_TYPE; i < LAST_TANK_TYPE; i++)
  {
	 TankLeft = INT_FROM_RESOURCE_ARRAY(i,IDN_GRAPHIC_TANK_LEFT);
	 TankTop = INT_FROM_RESOURCE_ARRAY(i,IDN_GRAPHIC_TANK_TOP);
     
     m_TankRectHeightArray[i]                   = 0;
     m_TankStatusArray[i]                       = false;
     m_TankVisibleArray[i]                      = true;

	 m_TankGraphicsArray[i]                     = new TThreeLayersGraphic(this);
     m_TankGraphicsArray[i]->Parent             = ParentPtr;
     m_TankGraphicsArray[i]->Name               = ("ThreeLayersGraphic" + QIntToStr(i)).c_str();
     m_TankGraphicsArray[i]->Tag                = i;
     m_TankGraphicsArray[i]->Layer2DrawEvent    = TankGraphicsLayer2DrawEvent;
	 m_TankGraphicsArray[i]->Height             = INT_FROM_RESOURCE(IDN_GRAPHIC_TANK_HEIGHT);
     m_TankGraphicsArray[i]->Left               = TankLeft;
	 m_TankGraphicsArray[i]->Top                = TankTop;
     m_TankGraphicsArray[i]->Width              = TankWidth;
     LOAD_BITMAP(m_TankGraphicsArray[i]->AlphaLayer, IDB_ALPHA_LAYER_GRAPHICS);
     LOAD_BITMAP(m_TankGraphicsArray[i]->Layer3,     IDB_LAYER3_GRAPHICS);
     m_TankGraphicsArray[i]->Layer3XOffset      = 0;
     m_TankGraphicsArray[i]->Layer3YOffset      = 0;
     m_TankGraphicsArray[i]->Layer1Transparent  = true;
     m_TankGraphicsArray[i]->Layer3Transparent  = true;
     m_TankGraphicsArray[i]->VScreenTransparent = true;
     m_TankGraphicsArray[i]->TransparentMode    = tmFixed;
     m_TankGraphicsArray[i]->TransparentColor   = TRANSPARENT_COLOR;
     m_TankGraphicsArray[i]->BackgroundColor    = INACTIVE_BACKGROUND_COLOR;
     m_TankGraphicsArray[i]->UseBackgroundColor = true;
     m_TankGraphicsArray[i]->AlphaBlendMode     = Blend8Bit;

     m_ResinNameLabelArray[i]                   = new TLabel(this);
     m_ResinNameLabelArray[i]->Parent           = ParentPtr;
     m_ResinNameLabelArray[i]->AutoSize         = false;
     m_ResinNameLabelArray[i]->Left             = TankLeft-TankWidth/2;
	 m_ResinNameLabelArray[i]->Top              = TankTop-15;
	 m_ResinNameLabelArray[i]->Height           = INT_FROM_RESOURCE(IDN_GRAPHIC_NAME_HEIGHT);
     m_ResinNameLabelArray[i]->Width            = 2*TankWidth;
     m_ResinNameLabelArray[i]->Transparent      = true;
     m_ResinNameLabelArray[i]->Font->Color      = clWhite;
	 m_ResinNameLabelArray[i]->Font->Height     = -15;
     m_ResinNameLabelArray[i]->Font->Size       = 8;
     m_ResinNameLabelArray[i]->Font->Style      = TFontStyles()<< fsBold;
     m_ResinNameLabelArray[i]->Alignment        = taCenter;
     m_ResinNameLabelArray[i]->Caption          = "";

	 m_ResinWeightLabelArray[i]                 = new TLabel(this);
	 m_ResinWeightLabelArray[i]->Parent         = ParentPtr;
	 m_ResinWeightLabelArray[i]->AutoSize       = false;
	 m_ResinWeightLabelArray[i]->Left           = TankLeft;
	 m_ResinWeightLabelArray[i]->Top            = TankTop+m_TankGraphicsArray[i]->Height-17;
	 m_ResinWeightLabelArray[i]->Height         = 18;
	 m_ResinWeightLabelArray[i]->Width          = TankWidth;
	 m_ResinWeightLabelArray[i]->Transparent    = true;
	 m_ResinWeightLabelArray[i]->Font->Color    = clWhite;
	 m_ResinWeightLabelArray[i]->Font->Height   = -15;
	 m_ResinWeightLabelArray[i]->Font->Size     = 8;
	 m_ResinWeightLabelArray[i]->Font->Style    = TFontStyles()<< fsBold;
	 m_ResinWeightLabelArray[i]->Alignment      = taCenter;
	 m_ResinWeightLabelArray[i]->Caption        = "???";

	 m_ResinTypeLabelArray[i]                   = new TLabel(this);
     m_ResinTypeLabelArray[i]->Parent           = ParentPtr;
     m_ResinTypeLabelArray[i]->AutoSize         = false;
     m_ResinTypeLabelArray[i]->Left             = TankLeft;
	 m_ResinTypeLabelArray[i]->Top              = TankTop+(m_TankGraphicsArray[i]->Height/4);
     m_ResinTypeLabelArray[i]->Height           = INT_FROM_RESOURCE(IDN_GRAPHIC_TYPE_HEIGHT);
     m_ResinTypeLabelArray[i]->Width            = TankWidth;
	 m_ResinTypeLabelArray[i]->Transparent      = true;
	 m_ResinTypeLabelArray[i]->Font->Color      = clWhite;
	 m_ResinTypeLabelArray[i]->Font->Height     = -3;
	 m_ResinTypeLabelArray[i]->Font->Size       = 10;
     m_ResinTypeLabelArray[i]->Font->Style      = TFontStyles()<< fsBold;
     m_ResinTypeLabelArray[i]->Alignment        = taCenter;
	 m_ResinTypeLabelArray[i]->Caption          = QStrToUpper(TankToStr((TTankIndex)i)).c_str();//QStrToUpper(LOAD_QSTRING_ARRAY(Tank,IDS_TANK_STRINGS)).c_str();

	 m_DrainImageArray[i]                       = new TImage(this);
     m_DrainImageArray[i]->Visible              = false;
     m_DrainImageArray[i]->Parent               = ParentPtr;
     m_DrainImageArray[i]->AutoSize             = false;
     m_DrainImageArray[i]->Left                 = TankLeft;
     m_DrainImageArray[i]->Top                  = INT_FROM_RESOURCE(IDN_DRAIN_IMG_TOP);
     m_DrainImageArray[i]->Height               = INT_FROM_RESOURCE(IDN_GRAPHIC_TYPE_HEIGHT);
     m_DrainImageArray[i]->Width                = TankWidth;
     m_DrainImageArray[i]->Transparent          = true;
     m_DrainImageArray[i]->Center               = true;
     LOAD_BITMAP(m_DrainImageArray[i]->Picture->Bitmap, IDB_DRAIN);

     m_TankEnableArray[i]                = new TImage(this);
     m_TankEnableArray[i]->Visible       = false;
     m_TankEnableArray[i]->Parent        = ParentPtr;
     m_TankEnableArray[i]->AutoSize      = false;
     m_TankEnableArray[i]->Top           = TankTop;
     m_TankEnableArray[i]->Height        = INT_FROM_RESOURCE(IDN_RR_INPROGRESS_IMG_SIDE);
     m_TankEnableArray[i]->Width         = INT_FROM_RESOURCE(IDN_RR_INPROGRESS_IMG_SIDE);
     m_TankEnableArray[i]->Left          = TankLeft + TankWidth - m_TankEnableArray[i]->Width;
     m_TankEnableArray[i]->Transparent   = true;
     m_TankEnableArray[i]->Center        = true;
     m_TankEnableArray[i]->Hint          = "";
     m_TankEnableArray[i]->ShowHint      = true;
     m_TankEnableArray[i]->Tag           = i;
     m_TankEnableArray[i]->OnDblClick    = DynamicOnDblClick;
     LOAD_BITMAP(m_TankEnableArray[i]->Picture->Bitmap, IDB_INFO);
  }

  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
     m_CurrentActiveTankArray[i] = NO_TANK;
     m_ActiveTankArray[i]        = NO_TANK;
  }

  LOAD_BITMAP(CurrentStateLabel->LeftPart,     IDB_STATUS_PANEL_LEFT);
  LOAD_BITMAP(CurrentStateLabel->MiddlePart,   IDB_STATUS_PANEL_MIDDLE);
  LOAD_BITMAP(CurrentStateLabel->RightPart,    IDB_STATUS_PANEL_RIGHT);
  CurrentStateLabel->MiddlePartsNum          = INT_FROM_RESOURCE(IDN_STATUS_PANEL_MIDDLE_PARTS_NUM);
  CurrentStateLabel->TransparentColor        = TRANSPARENT_COLOR;

  LOAD_BITMAP(SubStatusLabel->LeftPart,        IDB_STATUS_PANEL_LEFT);
  LOAD_BITMAP(SubStatusLabel->MiddlePart,      IDB_STATUS_PANEL_MIDDLE);
  LOAD_BITMAP(SubStatusLabel->RightPart,       IDB_STATUS_PANEL_RIGHT);
  SubStatusLabel->MiddlePartsNum             = INT_FROM_RESOURCE(IDN_STATUS_PANEL_MIDDLE_PARTS_NUM);
  SubStatusLabel->TransparentColor           = TRANSPARENT_COLOR;

  LOAD_BITMAP(PrintTimeLabel->LeftPart,        IDB_PROGRESS_TIME_PANEL_LEFT);
  LOAD_BITMAP(PrintTimeLabel->MiddlePart,      IDB_PROGRESS_PANEL_MIDDLE);
  LOAD_BITMAP(PrintTimeLabel->RightPart,       IDB_PROGRESS_PANEL_RIGHT);
  PrintTimeLabel->MiddlePartsNum             = INT_FROM_RESOURCE(IDN_PROGRESS_PANEL_MIDDLE_PARTS_NUM);
  PrintTimeLabel->TransparentColor           = TRANSPARENT_COLOR;

  LOAD_BITMAP(HeightProgressLabel->LeftPart,   IDB_PROGRESS_PANEL_LEFT);
  LOAD_BITMAP(HeightProgressLabel->MiddlePart, IDB_PROGRESS_PANEL_MIDDLE);
  LOAD_BITMAP(HeightProgressLabel->RightPart,  IDB_PROGRESS_PANEL_RIGHT);
  HeightProgressLabel->MiddlePartsNum        = INT_FROM_RESOURCE(IDN_PROGRESS_PANEL_MIDDLE_PARTS_NUM);
  HeightProgressLabel->TransparentColor      = TRANSPARENT_COLOR;

  LOAD_BITMAP(SliceProgressLabel->LeftPart,    IDB_PROGRESS_PANEL_LEFT);
  LOAD_BITMAP(SliceProgressLabel->MiddlePart,  IDB_PROGRESS_PANEL_MIDDLE);
  LOAD_BITMAP(SliceProgressLabel->RightPart,   IDB_PROGRESS_PANEL_RIGHT);
  SliceProgressLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_PROGRESS_PANEL_MIDDLE_PARTS_NUM);
  SliceProgressLabel->TransparentColor       = TRANSPARENT_COLOR;

  LOAD_BITMAP(PreviousJobLabel->LeftPart,      IDB_PROGRESS_PANEL_LEFT);
  LOAD_BITMAP(PreviousJobLabel->MiddlePart,    IDB_PROGRESS_PANEL_MIDDLE);
  LOAD_BITMAP(PreviousJobLabel->RightPart,     IDB_PROGRESS_PANEL_RIGHT);
  PreviousJobLabel->MiddlePartsNum           = INT_FROM_RESOURCE(IDN_PROGRESS_PANEL_MIDDLE_PARTS_NUM);
  PreviousJobLabel->TransparentColor         = TRANSPARENT_COLOR;

  LOAD_BITMAP(ServiceAlertLabel->LeftPart,     IDB_SERVICE_ALERT_PANEL_LEFT);
  LOAD_BITMAP(ServiceAlertLabel->MiddlePart,   IDB_SERVICE_ALERT_PANEL_MIDDLE);
  LOAD_BITMAP(ServiceAlertLabel->RightPart,    IDB_SERVICE_ALERT_PANEL_RIGHT);
  ServiceAlertLabel->MiddlePartsNum          = INT_FROM_RESOURCE(IDN_SERVICE_ALERT_MIDDLE_PARTS_NUM);
  ServiceAlertLabel->TransparentColor        = TRANSPARENT_COLOR;

  LOAD_BITMAP(PrintProgressBar->ProgressBitmap,IDB_PRINT_PROGRESS_BITMAP);
  LOAD_BITMAP(PrintProgressBar->BodyBitmap,    IDB_PRINT_PROGRESS_BODY_BITMAP);
  //PrintProgressBar->Height = 385;
  PrintProgressBar->TransparentColor         = TRANSPARENT_COLOR;
  PrintProgressBar->Top                      = Panel1->Top;
}

//---------------------------------------------------------------------------
void TMainStatusFrame::SetContainerData(TTankIndex Tank, float FullPercent)
{
  if(VALIDATE_TANK(Tank))
  {
    m_TankRectHeightArray[Tank] = (FullPercent / 100.0f) * INT_FROM_RESOURCE(IDN_CONTAINER_RECT_HEIGHT);

    // Don't let the display height be too low...
    if ((m_TankRectHeightArray[Tank] < INT_FROM_RESOURCE(IDN_MINIMUM_DISPLAY_HEIGHT)) &&
        (m_TankRectHeightArray[Tank] >= 0))
      m_TankRectHeightArray[Tank] = INT_FROM_RESOURCE(IDN_MINIMUM_DISPLAY_HEIGHT);

    RepaintContainer(Tank);
  }
}//SetContainerData

bool TMainStatusFrame::IsActive(TTankIndex Tank)
{
  bool ret = false;
  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE && !ret; i++)
     ret = (Tank == m_ActiveTankArray[i]);
  return ret;
}//IsActive


void __fastcall TMainStatusFrame::TankGraphicsLayer2DrawEvent(TObject *Sender, TCanvas *DrawCanvas)
{
  TThreeLayersGraphic* Graphic = dynamic_cast<TThreeLayersGraphic*>(Sender);
  if(!VALIDATE_TANK(Graphic->Tag))
    return;
  if (Graphic && !m_TankVisibleArray[Graphic->Tag])
    return;

  if (m_TankStatusArray[Graphic->Tag])
    if (IsActive(static_cast<TTankIndex>(Graphic->Tag)))
      DrawCanvas->Brush->Color = ACTIVE_CONTAINER_COLOR;
    else
      DrawCanvas->Brush->Color = INACTIVE_CONTAINER_COLOR;
  else
    DrawCanvas->Brush->Color = clRed;

  DrawCanvas->Pen->Style = psClear;
  DrawCanvas->FillRect(Rect(0,INT_FROM_RESOURCE(IDN_CONTAINER_RECT_LOW_Y) - m_TankRectHeightArray[Graphic->Tag],
                            m_TankGraphicsArray[Graphic->Tag]->Width,
                            INT_FROM_RESOURCE(IDN_CONTAINER_RECT_LOW_Y)));
}//TankGraphicsLayer2DrawEvent
//---------------------------------------------------------------------------

void __fastcall TMainStatusFrame::SelectSoftButtonClick(TObject *Sender)
{
  SelectSoftButton->ItemIndex = -1;
  MainForm->GetUIFrame()->NavigateForward(MAIN_MAINTENANCE_STATUS_SCREEN_ID);
}
//---------------------------------------------------------------------------

void TMainStatusFrame::SetContainerStatus(TTankIndex Tank, bool Status)
{
  if(VALIDATE_TANK_INCLUDING_WASTE(Tank))
     m_TankStatusArray[Tank] = Status;
}//SetContainerStatus

void TMainStatusFrame::UpdateTankEnabledStatus(TTankIndex Tank, bool Enabled)
{
  if(!VALIDATE_TANK(Tank))
     return;

  m_TankEnableArray[Tank]->Visible = !(Enabled);
}

void TMainStatusFrame::SetDisabledIconHint(TTankIndex Tank,  AnsiString Hint)
{
  if(!VALIDATE_TANK(Tank))
     return;

  m_TankEnableArray[Tank]->Hint = Hint;
}

void TMainStatusFrame::SetContainerVisible(TTankIndex Tank, bool Visible)
{
  if(!VALIDATE_TANK(Tank))
     return;

  m_TankVisibleArray[Tank]               = Visible;
  m_ResinWeightLabelArray[Tank]->Visible = Visible;

  if (!Visible)
     m_TankGraphicsArray[Tank]->BackgroundColor = CONTAINER_OUT_BACKGROUND_COLOR;
  else if(IsActive(Tank))
     m_TankGraphicsArray[Tank]->BackgroundColor = DEFAULT_BACKGROUND_COLOR;
  else
     m_TankGraphicsArray[Tank]->BackgroundColor = INACTIVE_BACKGROUND_COLOR;

  RepaintContainer(Tank);
}//SetContainerVisible


void __fastcall TMainStatusFrame::ServiceAlertLabelClick(TObject *Sender)
{
  MainForm->GetUIFrame()->MaintenanceCountersActionExecute(Sender);
}
//---------------------------------------------------------------------------

void TMainStatusFrame::SetActiveContainer(TChamberIndex Chamber, TTankIndex ActiveTank)
{
  if (!VALIDATE_CHAMBER(Chamber) || !VALIDATE_TANK(ActiveTank))
     return;

  TTankIndex PrevActiveTank  = m_ActiveTankArray[Chamber];
  m_ActiveTankArray[Chamber] = ActiveTank;
  
  if (VALIDATE_TANK(PrevActiveTank))
  {
     if(!IsActive(PrevActiveTank) && m_TankGraphicsArray[PrevActiveTank])
     {
        m_TankGraphicsArray[PrevActiveTank]->BackgroundColor =
            (m_TankVisibleArray[PrevActiveTank] ? INACTIVE_BACKGROUND_COLOR : CONTAINER_OUT_BACKGROUND_COLOR);
        RepaintContainer(PrevActiveTank);
     }
  }

  m_TankGraphicsArray[ActiveTank]->BackgroundColor =
         (m_TankVisibleArray[ActiveTank] ? DEFAULT_BACKGROUND_COLOR : CONTAINER_OUT_BACKGROUND_COLOR);
  RepaintContainer(ActiveTank);
}

void TMainStatusFrame::UpdateResinType(TTankIndex Tank, AnsiString ResinType)
{
  if (VALIDATE_TANK(Tank))
      m_ResinNameLabelArray[Tank]->Caption = ResinType;
  else
      MessageDlg("UpdateResinType Error: Illegal container number",mtError,TMsgDlgButtons() << mbOK,0);
}//UpdateResinType

void TMainStatusFrame::UpdateDrainPumpsStatus(TChamberIndex Chamber, TTankIndex Tank)
{
  if(!VALIDATE_CHAMBER(Chamber))
     return;

  if(VALIDATE_TANK(m_CurrentActiveTankArray[Chamber]))
     m_DrainImageArray[m_CurrentActiveTankArray[Chamber]]->Visible = false;

  m_CurrentActiveTankArray[Chamber] = Tank;

  if(VALIDATE_TANK(Tank))
  {
    m_DrainImageArray[Tank]->Visible = true;
    m_DrainTimers[Tank]->Enabled     = true;
	m_DrainTimers[GetSiblingTank(Tank)]->Enabled     = false;
	m_DrainImageArray[GetSiblingTank(Tank)]->Visible = false;
  }
  else // If the tank is NO_TANK
  {
   switch (Chamber)
   {
   case TYPE_CHAMBER_SUPPORT:
      m_DrainTimers[TYPE_TANK_SUPPORT2]->Enabled     = false;
      m_DrainTimers[TYPE_TANK_SUPPORT1]->Enabled     = false;
      m_DrainImageArray[TYPE_TANK_SUPPORT2]->Visible = false;
      m_DrainImageArray[TYPE_TANK_SUPPORT1]->Visible = false;
      break;

   case TYPE_CHAMBER_MODEL1:
      m_DrainTimers[TYPE_TANK_MODEL1]->Enabled      = false;
      m_DrainImageArray[TYPE_TANK_MODEL1]->Visible  = false;
#ifdef OBJET_MACHINE
     m_DrainTimers[TYPE_TANK_MODEL2]->Enabled      = false;
     m_DrainImageArray[TYPE_TANK_MODEL2]->Visible  = false; 
#endif	 	  
      break;
   case TYPE_CHAMBER_MODEL2:
   #ifdef OBJET_MACHINE
     m_DrainTimers[TYPE_TANK_MODEL3]->Enabled      = false;
     m_DrainImageArray[TYPE_TANK_MODEL3]->Visible  = false; 
	 m_DrainTimers[TYPE_TANK_MODEL4]->Enabled      = false;
     m_DrainImageArray[TYPE_TANK_MODEL4]->Visible  = false; 
   #else
      m_DrainTimers[TYPE_TANK_MODEL2]->Enabled      = false;
      m_DrainImageArray[TYPE_TANK_MODEL2]->Visible  = false; 
   #endif
      break;
#ifdef OBJET_MACHINE
   case TYPE_CHAMBER_MODEL3:
      m_DrainTimers[TYPE_TANK_MODEL5]->Enabled      = false;
      m_DrainImageArray[TYPE_TANK_MODEL5]->Visible  = false;
	  m_DrainTimers[TYPE_TANK_MODEL6]->Enabled      = false;
      m_DrainImageArray[TYPE_TANK_MODEL6]->Visible  = false;
      break;
#endif
   default:
      throw EQException ("UpdateDrainPumpsStatus function recieved invalid chamber index argument");
    }
  }
}

void __fastcall TMainStatusFrame::DrainTimerTimer(TObject *Sender)
{
  TTimer *Timer = dynamic_cast<TTimer *>(Sender);

  if (! m_DrainImageArray[Timer->Tag]->Visible)
  {
      m_DrainImageArray[Timer->Tag]->Visible = true;
      Timer->Interval = 1000;
  }
  else
  {
      m_DrainImageArray[Timer->Tag]->Visible = false;
      Timer->Interval = 250;
  }
}

//---------------------------------------------------------------------------

void TMainStatusFrame::RepaintContainer(TTankIndex Tank)
{
  if (!VALIDATE_TANK(Tank))
     return;

//  m_TankGraphicsArray[Tank]->Redraw();
  m_TankGraphicsArray[Tank]->Repaint();
  m_ResinTypeLabelArray[Tank]->Repaint();
  m_ResinWeightLabelArray[Tank]->Repaint();
  m_ResinNameLabelArray[Tank]->Repaint();
  m_DrainImageArray[Tank]->Repaint();
  m_TankEnableArray[Tank]->Repaint();
}

void __fastcall TMainStatusFrame::DynamicOnDblClick (TObject *Sender)
{
  TImage *Image = NULL;
  Image = dynamic_cast<TImage *>(Sender);
  if (NULL == Image)
    return;

  CBackEndInterface::Instance()->RemountSingleTankStatus((TTankIndex)Image->Tag);
}

