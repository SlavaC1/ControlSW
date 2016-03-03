//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ActuatorsControlDlg.h"
#include "QMonitor.h"
#include "FrontEndControlIDs.h"
#include "FrontEndParams.h"
#include "FrontEnd.h"
#include "AppParams.h"
#include "FEResources.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TActuatorsControlForm* ActuatorsControlForm;

const int DESIGNED_FORM_HEIGHT = 550;//INT_FROM_RESOURCE(IDN_DESIGNED_FORM_HEIGHT);
const int CONTROL_TOP_DELTA = 21;
const int HIDE_BUTTON_DELTA = 100;

//---------------------------------------------------------------------------
__fastcall TActuatorsControlForm::TActuatorsControlForm(TComponent* Owner)
	: TForm(Owner)
{
	m_ParamMgr           = CAppParams::Instance();
	m_ActuatorsStatus    = 0;
	m_InputsStatus       = 0;

	memset(m_PumpTankArr, -1, MAX_ACTUATOR_ID);
    InitPumps();

	InitTrayHeater();
	InitSignalTower();
	InitRSS();
	InitAmbientHeater();
	InitDualWaste();
	InitRemovableTray();
	InitDoorGuiParams(); //init the door sensor params
    InitPurgeBath();

	ActuatorsGroupBox->Height = DESIGNED_FORM_HEIGHT - 43;
	SensorsGroupBox->Height   = DESIGNED_FORM_HEIGHT - 43;
}
//---------------------------------------------------------------------------

void TActuatorsControlForm::InitPumps()
{
   for(int i = 0; i < NUMBER_OF_PUMPS; i++)
	{
		ModelPumpLbl[i]                  = new TLabel(this);
		ModelPumpLbl[i]->Parent          = ActuatorsGroupBox;
		ModelPumpLbl[i]->AutoSize        = true;
		ModelPumpLbl[i]->Height          = 13;
		ModelPumpLbl[i]->Width           = 70;
		ModelPumpLbl[i]->Left            = 11;
		ModelPumpLbl[i]->Top             = 44 + CONTROL_TOP_DELTA * i;
		ModelPumpLbl[i]->Caption         = TankToStr((TTankIndex)i).c_str();
		ModelPumpLbl[i]->Alignment       = taLeftJustify;
		ModelPumpLbl[i]->Layout          = tlTop;
		ModelPumpLbl[i]->Visible         = true;
		//ModelPumpLbl[i]->Tag 			 = PumpsActuatorID[i];

		ModelPumpBtn[i]                = new TButton(this);
		ModelPumpBtn[i]->Caption       = "On";
		ModelPumpBtn[i]->Parent        = ActuatorsGroupBox;
		ModelPumpBtn[i]->Left          = 121;
        ModelPumpBtn[i]->Top           = 44 + CONTROL_TOP_DELTA * i;
		ModelPumpBtn[i]->Width         = 25;
		ModelPumpBtn[i]->Height         = 18;
        ModelPumpBtn[i]->OnClick       = ActuatorOnActionExecute;
		ModelPumpBtn[i]->Action        = ActuatorOnAction;
		ModelPumpBtn[i]->Visible       = true;
		ModelPumpBtn[i]->Tag 		   = PumpsActuatorID[i];
		m_PumpTankArr[ModelPumpBtn[i]->Tag] = i;

		ModelPumpImg[i]                = new TImage(this);
		ModelPumpImg[i]->Visible       = true;
		ModelPumpImg[i]->Parent        = ActuatorsGroupBox;
		ModelPumpImg[i]->AutoSize      = true;
		ModelPumpImg[i]->Top           = 44 + CONTROL_TOP_DELTA * i;
		ModelPumpImg[i]->Height        = 16;
		ModelPumpImg[i]->Width         = 16;
		ModelPumpImg[i]->Left          = 152;
		ModelPumpImg[i]->Transparent   = true;
		ModelPumpImg[i]->Center        = true;
		ModelPumpImg[i]->Hint          = "";
		ModelPumpImg[i]->ShowHint      = true;
		ModelPumpImg[i]->Tag           = PumpsActuatorID[i];
	}
	int num_of_containers = 0 ;
	if(m_ParamMgr->DualWasteEnabled)
		num_of_containers = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;
	else
		num_of_containers = TOTAL_NUMBER_OF_CONTAINERS;

	for (int i = 0; i < num_of_containers; i++)
	{
		TankMSLbl[i]                  = new TLabel(this);
		TankMSLbl[i]->Parent          = SensorsGroupBox;
		TankMSLbl[i]->AutoSize        = true;
		TankMSLbl[i]->Height          = 13;
		TankMSLbl[i]->Width           = 70;
		TankMSLbl[i]->Left            = 11;
		TankMSLbl[i]->Top             = 117 + CONTROL_TOP_DELTA * i;
		TankMSLbl[i]->Caption         = TankToStr((TTankIndex)i).c_str();
		TankMSLbl[i]->Alignment       = taLeftJustify;
		TankMSLbl[i]->Layout          = tlTop;
		TankMSLbl[i]->Visible         = true;

		TankMSImg[i]                = new TImage(this);
		TankMSImg[i]->Visible       = true;
		TankMSImg[i]->Parent        = SensorsGroupBox;
		TankMSImg[i]->AutoSize      = true;
		TankMSImg[i]->Top           = 117 + CONTROL_TOP_DELTA * i;
		TankMSImg[i]->Height        = 16;
		TankMSImg[i]->Width         = 16;
		TankMSImg[i]->Left          = 107;
		TankMSImg[i]->Transparent   = true;
		TankMSImg[i]->Center        = true;
		TankMSImg[i]->Hint          = "";
		TankMSImg[i]->ShowHint      = true;
		TankMSImg[i]->Tag           = 16+i;
	}
}

void TActuatorsControlForm::InitRSS()
{
   //RSS, itamar added
	if(m_ParamMgr->IsSuctionSystemExist)
	{
		RollerSuctionPumpLabel->Visible = true;
		RollerSuctionValveLabel->Visible = true;

		RollerSuctionPumpButton->Visible = true;
		RollerSuctionValveButton->Visible = true;

		RollerSuctionSystemLabel->Visible = true;
		RollerSuctionSystemBevel->Visible = true;

		RollerSuctionPumpImage->Visible = true;
		RollerSuctionValveImage->Visible = true;
	}
	else
	{
		RollerSuctionPumpButton->Top  = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		RollerSuctionValveButton->Top = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA; 
	}
}

void TActuatorsControlForm::InitSignalTower()
{
    RedLightButton->Tag    = ACTUATOR_ID_SIGNAL_TOWER_RED;
	RedLightImage->Tag     = ACTUATOR_ID_SIGNAL_TOWER_RED;

	GreenLightButton->Tag  = ACTUATOR_ID_SIGNAL_TOWER_GREEN;
	GreenLightImage->Tag   = ACTUATOR_ID_SIGNAL_TOWER_GREEN;
	
	YellowLightButton->Tag = ACTUATOR_ID_SIGNAL_TOWER_YELLOW;
	YellowLightImage->Tag  = ACTUATOR_ID_SIGNAL_TOWER_YELLOW;

	if(m_ParamMgr->SignalTowerEnabled)
	{
		SignalTowerLabel->Visible  = true;
		SignalTowerBevel->Visible  = true;
		RedLightButton->Visible    = true;
		RedLightLabel->Visible     = true;
		RedLightImage->Visible     = true;
		GreenLightButton->Visible  = true;
		GreenLightLabel->Visible   = true;
		GreenLightImage->Visible   = true;
		YellowLightButton->Visible = true;
		YellowLightLabel->Visible  = true;
		YellowLightImage->Visible  = true;

		SignalTowerLabel->Top  = 90;
		SignalTowerBevel->Top  = 106;
		SignalTowerLabel->Left  = 200;
		SignalTowerBevel->Left  = 200;

		RedLightButton->Top        = 112;
		RedLightLabel->Top         = 112;
		RedLightImage->Top         = 112;

		GreenLightButton->Top      = 112 + CONTROL_TOP_DELTA;
		GreenLightLabel->Top       = 112 + CONTROL_TOP_DELTA;
		GreenLightImage->Top       = 112 + CONTROL_TOP_DELTA;
		
		YellowLightButton->Top     = 112 + CONTROL_TOP_DELTA * 2;
		YellowLightLabel->Top      = 112 + CONTROL_TOP_DELTA * 2;
		YellowLightImage->Top      = 112 + CONTROL_TOP_DELTA * 2;

		RedLightButton->Left       = 314;
		GreenLightButton->Left     = 314;
		YellowLightButton->Left    = 314;

		RedLightLabel->Left        = 203;
		GreenLightLabel->Left      = 203;
		YellowLightLabel->Left     = 203;

		RedLightImage->Left        = 343;
		GreenLightImage->Left      = 343;
		YellowLightImage->Left     = 343;
	}
	else
	{
		SignalTowerLabel->Visible  = false;
		SignalTowerBevel->Visible  = false;
		RedLightButton->Top        = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		RedLightLabel->Visible     = false;
		RedLightImage->Visible     = false;
		GreenLightButton->Top      = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		GreenLightLabel->Visible   = false;
		GreenLightImage->Visible   = false;
		YellowLightButton->Top     = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		YellowLightLabel->Visible  = false;
		YellowLightImage->Visible  = false;
	}
}

void TActuatorsControlForm::InitTrayHeater()
{
    if(! m_ParamMgr->TrayHeaterEnabled)
	{
		TrayHeaterLabel->Visible  = false;
		TrayHeaterButton->Top   = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		TrayHeaterImage->Visible  = false;
	}
	else
	{
		TrayHeaterLabel->Visible  = true;
		TrayHeaterImage->Visible  = true;
	}
}

void TActuatorsControlForm::InitAmbientHeater()
{
	AmbientHeater1Button->Tag = ACTUATOR_ID_AMBIENT_HEATER_1;
	AmbientHeater1Image->Tag  = ACTUATOR_ID_AMBIENT_HEATER_1;
	AmbientHeater2Button->Tag = ACTUATOR_ID_AMBIENT_HEATER_2;
	AmbientHeater2Image->Tag  = ACTUATOR_ID_AMBIENT_HEATER_2;

	if(m_ParamMgr->AmbientHeaterEnabled)
	{
		AmbientHeater1Button->Visible = true;
		AmbientHeater1Label->Visible  = true;
		AmbientHeater1Image->Visible  = true;
		AmbientHeater2Button->Visible = true;
		AmbientHeater2Label->Visible  = true;
		AmbientHeater2Image->Visible  = true;
	}
	else
	{
		AmbientHeater1Button->Top     = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		AmbientHeater1Label->Visible  = false;
		AmbientHeater1Image->Visible  = false;
		AmbientHeater2Button->Top     = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
		AmbientHeater2Label->Visible  = false;
		AmbientHeater2Image->Visible  = false;
	}

}

void TActuatorsControlForm::InitDualWaste()
{
	 if(m_ParamMgr->DualWasteEnabled)
	 {
	 	Label63->Caption = "Waste Left";

	 	WasteRightLabel->Visible = true;
	 	WasteRightBtn->Visible   = true;
	 	WasteRightImg->Visible   = true;

	 	Button39->Tag      = ACTUATOR_ID_WASTE_PUMP_LEFT;
	 	Image29->Tag       = ACTUATOR_ID_WASTE_PUMP_LEFT;
	 	WasteRightBtn->Tag = ACTUATOR_ID_WASTE_PUMP_RIGHT;
	 	WasteRightImg->Tag = ACTUATOR_ID_WASTE_PUMP_RIGHT;

		//needs refactorring
		TankMSImg[TYPE_TANK_WASTE_LEFT]->Tag  = SENSOR_ID_22;
		TankMSImg[TYPE_TANK_WASTE_RIGHT]->Tag = SENSOR_ID_23;

	 	// Arrange the controls on form
	 	WasteRightLabel->Top = Label63->Top  + CONTROL_TOP_DELTA;
	 	WasteRightBtn->Top   = Button39->Top + CONTROL_TOP_DELTA; // under Waste Left
	 	WasteRightImg->Top   = Image29->Top  + CONTROL_TOP_DELTA;

	 	Label64->Top  = WasteRightLabel->Top + CONTROL_TOP_DELTA; // Roller Pump
	 	Button42->Top = WasteRightBtn->Top   + CONTROL_TOP_DELTA;
	 	Image30->Top  = WasteRightImg->Top   + CONTROL_TOP_DELTA;

	 	Label58->Top  = Label64->Top  + CONTROL_TOP_DELTA; // Vacuum
	 	Button38->Top = Button42->Top + CONTROL_TOP_DELTA;
	 	Image28->Top  = Image30->Top  + CONTROL_TOP_DELTA;

	 	Label41->Top  = Label58->Top  + CONTROL_TOP_DELTA; // Air Valve
	 	Button25->Top = Button38->Top + CONTROL_TOP_DELTA;
	 	Image14->Top  = Image28->Top  + CONTROL_TOP_DELTA;

		TankMSLbl[TYPE_TANK_WASTE_LEFT]->Visible  = true;
		TankMSLbl[TYPE_TANK_WASTE_LEFT]->Top  = InterlockLbl->Top + CONTROL_TOP_DELTA;;
		TankMSImg[TYPE_TANK_WASTE_LEFT]->Visible  = true;
		TankMSImg[TYPE_TANK_WASTE_LEFT]->Top  = InterlockImg->Top + CONTROL_TOP_DELTA;

		TankMSLbl[TYPE_TANK_WASTE_RIGHT]->Visible  = true;
		TankMSLbl[TYPE_TANK_WASTE_RIGHT]->Top  = TankMSLbl[TYPE_TANK_WASTE_LEFT]->Top + CONTROL_TOP_DELTA;;
		TankMSImg[TYPE_TANK_WASTE_RIGHT]->Visible  = true;
		TankMSImg[TYPE_TANK_WASTE_RIGHT]->Top  = TankMSImg[TYPE_TANK_WASTE_LEFT]->Top + CONTROL_TOP_DELTA;
	 }
	 else
	 {
	 	WasteRightBtn->Top               = DESIGNED_FORM_HEIGHT + HIDE_BUTTON_DELTA;
	 }
}

void TActuatorsControlForm::InitRemovableTray()
{
    if(! m_ParamMgr->RemovableTray)
	{
		TrayInsertedImg->Visible   = false;
		TrayInsertedLabel->Visible = false;
	}
	else
	{
		TrayInsertedImg->Visible   = true;
		TrayInsertedLabel->Visible = true;
	}
}
// Update status on the frame (integer version)
void TActuatorsControlForm::UpdateStatus(int ControlID, int Status)
{
	PostMessage(Handle, WM_STATUS_UPDATE, ControlID, Status);
}
//---------------------------------------------------------------------------
// Update status on the frame (float version)
void TActuatorsControlForm::UpdateStatus(int ControlID, float Status)
{
	PostMessage(Handle, WM_STATUS_UPDATE, ControlID, *((int *)&Status));
}
//---------------------------------------------------------------------------
// Update status on the frame (string version)
void TActuatorsControlForm::UpdateStatus(int ControlID, QString Status)
{
	PostMessage(Handle, WM_STATUS_UPDATE, ControlID, reinterpret_cast<int>(strdup(Status.c_str())));
}
//---------------------------------------------------------------------------
// Message handler for the WM_STATUS_UPDATE message
void TActuatorsControlForm::HandleStatusUpdateMessage(TMessage &Message)
{
	// find the image to update
	// Handle only the actuator and input messages
	if (Message.WParam >= FE_CURRENT_ACTUATOR_STATUS_START && Message.WParam < FE_CURRENT_ACTUATOR_STATUS_START + MAX_ACTUATOR_ID )
	{
		int ActuatorNum = Message.WParam - FE_CURRENT_ACTUATOR_STATUS_START;
		TComponent *MyComponent;
		TButton *Button;
		// Updating the Button's Action
		for (int i = 0; i < ComponentCount; i++)
		{
			MyComponent = Components[i];
			Button  = dynamic_cast<TButton *>(MyComponent);

			if (Button != NULL && Button->Tag  == ActuatorNum)
			{
				if (Message.LParam)
					Button->Action = ActuatorOffAction;
				else
					Button->Action = ActuatorOnAction;

				break;
			}
		}
		TImage  *Image;
		// Updating the Led
		for (int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
		{
			Image  = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
			if (Image != NULL && Image->Tag  == ActuatorNum)
			{
				// check if it is the first update of this control
				if (Image->Picture->Bitmap->Width != 0)
				{
					// check if the status has changed
					if (Message.LParam == (GetActuatorStatus((UINT64)ActuatorNum) ? 1 : 0))
						return;
				}

				LOAD_BITMAP(Image->Picture->Bitmap, ((Message.LParam) ? IDB_GREEN_LED : IDB_RED_LED));
				SetActuatorStatus((UINT64)ActuatorNum, (bool)Message.LParam);
				break;
			}
		}
	}

	if (Message.WParam >= FE_CURRENT_OCB_SENSOR_STATUS_START && (Message.WParam < FE_CURRENT_OCB_SENSOR_STATUS_START + MAX_SENSOR_ID))
	{
		LEDIndicator(SensorsGroupBox, Message.WParam - FE_CURRENT_OCB_SENSOR_STATUS_START, (bool)Message.LParam);
	}
}
//---------------------------------------------------------------------------
bool TActuatorsControlForm::GetActuatorStatus(UINT64 ActuatorNum)
{

	if (m_ActuatorsStatus & ((UINT64)1 << ActuatorNum))
		return true;

	return false;
}
//---------------------------------------------------------------------------
void TActuatorsControlForm::SetActuatorStatus( UINT64 ActuatorNum, bool Status)
{
	if (Status)
		m_ActuatorsStatus |= ((UINT64)1 << ActuatorNum);
	else
		m_ActuatorsStatus &= ~((UINT64)1 << ActuatorNum);

}
//---------------------------------------------------------------------------
bool TActuatorsControlForm::GetInputStatus(int InputNum)
{
	if (m_InputsStatus & ((unsigned long)1 << InputNum))
		return true;

	return false;
}
//---------------------------------------------------------------------------
void TActuatorsControlForm::SetInputStatus(int InputNum, bool Status)
{
	if (Status)
		m_InputsStatus |= ((unsigned long)1 << InputNum);
	else
		m_InputsStatus &= ~((unsigned long)1 << InputNum);
}
//---------------------------------------------------------------------------
void TActuatorsControlForm::LEDIndicator(TWinControl *Control, int InputNum, bool OnOff)
{
    if ((InputNum < 0) || (InputNum > MAX_SENSOR_ID))
    {
        return; // bad sensor ID
    }

    bool isValueUpdated = (OnOff != GetInputStatus(InputNum));

    // check if the status has changed
    if (isValueUpdated)
    {
        SetInputStatus(InputNum, OnOff); // update changed
    }

    // find the image to update
    TImage * Image;
    for (int i = 0; (i < Control->ControlCount); ++i)
    {
        Image = dynamic_cast<TImage *>(Control->Controls[i]);
        if ((Image != NULL) && (Image->Tag  == InputNum) && (Image->Visible) &&
            (isValueUpdated || (Image->Picture->Bitmap->Width == 0)/* 1st update*/))
        {
            LOAD_BITMAP(Image->Picture->Bitmap, ((OnOff) ? IDB_GREEN_LED : IDB_RED_LED));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void TActuatorsControlForm::DisplayEmulationState()
{
	// find the image to update
	TComponent *MyComponent;
	TImage  *Image;
	TButton *Button;

	// Updating the Button's Action
	for (int i = 0; i < ComponentCount; i++)
	{
		MyComponent = Components[i];
		Button  = dynamic_cast<TButton *>(MyComponent);

		if (Button != NULL)
			Button->Action = ActuatorOnAction;
	}

	// Updating the Actuators Leds
	for (int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
	{
		Image  = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
		if (Image != NULL)
			LOAD_BITMAP(Image->Picture->Bitmap, IDB_RED_LED);
	}

	// Updating the Sensors Leds
	for (int i = 0; i < SensorsGroupBox->ControlCount; i++)
	{
		Image  = dynamic_cast<TImage *>(SensorsGroupBox->Controls[i]);
		if (Image != NULL)
		{

#ifdef _DEBUG

			if(Image->Tag == 8  &&  FindWindow(0, "TrayIsOut.txt - Notepad")) // tray inserted sensor indication + Tray inserted file is open
				LOAD_BITMAP(Image->Picture->Bitmap, IDB_RED_LED);
			else
				LOAD_BITMAP(Image->Picture->Bitmap, IDB_GREEN_LED);

#else

			LOAD_BITMAP(Image->Picture->Bitmap, IDB_GREEN_LED);

#endif
		}
	}
}
//---------------------------------------------------------------------------
void TActuatorsControlForm::ChangeLedsEmulation(bool OnOff, int LedID)
{
	TImage* Image = NULL;
	// Updating the Actuator Led
	for (int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
	{
		Image  = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
		if (Image != NULL && Image->Tag == LedID)
		{
			LOAD_BITMAP(Image->Picture->Bitmap, ((OnOff) ? IDB_GREEN_LED : IDB_RED_LED));
			break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TActuatorsControlForm::ActuatorOffActionExecute(TObject *Sender)
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	TAction *ActionItem = dynamic_cast<TAction *>(Sender);
	TButton *Button = dynamic_cast<TButton *>(ActionItem->ActionComponent);

    // No valid Actuator ID assigned to the button
	if(Button->Tag == -1)
		return;

	if (CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
		ChangeLedsEmulation(false, Button->Tag);

	try
	{
		Button->Action = ActuatorOnAction;
		BackEnd->SetActuatorState(Button->Tag, false);
	   	if(Button->Tag == 17) //In order to update the interlock (General devices window)
		{
			FrontEndInterface->UpdateStatus(FE_CURRENT_DOOR_STATUS, 0);
		}  
		//update only if pumps were activated
		if(m_PumpTankArr[Button->Tag] != -1)
			BackEnd->UpdateWeightAfterMaterialConsumption(static_cast<TTankIndex>(m_PumpTankArr[Button->Tag]));
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------
void TActuatorsControlForm::InitDoorGuiParams()
{


	CBackEndInterface* BackEnd = CBackEndInterface::Instance();
	if(BackEnd!=NULL)
	{
		bool  bIsFourDoor = false ;
		if( BackEnd->GetNumOfDoor() > 1)
		{
		  bIsFourDoor = true;


		  EmergencyRearImg ->Top  = TrayInsertedLabel->Top + CONTROL_TOP_DELTA;
		  EmergencyRearlbl->Top   = EmergencyRearImg->Top;

		  EmergencyFrontImg->Top  = EmergencyRearlbl->Top + CONTROL_TOP_DELTA;
		  EmergencyFrontlbl->Top  = EmergencyFrontImg->Top;

		  ServiceKeyDoorImg->Top  = EmergencyFrontlbl->Top + CONTROL_TOP_DELTA;
		  ServiceKeyDoorLabel->Top  = ServiceKeyDoorImg->Top;

		  serviceDoorImg->Top  = ServiceKeyDoorLabel->Top + 1.5 * CONTROL_TOP_DELTA;
		  lblServiceDoorLabel->Top  = serviceDoorImg->Top;

		  FrontRightDoorImg->Top  = lblServiceDoorLabel->Top + CONTROL_TOP_DELTA;
		  FrontRightDoorLabel->Top  = FrontRightDoorImg->Top;

		  FrontLeftDoorImg->Top  = FrontRightDoorLabel->Top + CONTROL_TOP_DELTA;
		  FrontLeftDoorLabel->Top  = FrontLeftDoorImg->Top;
        }

		//elments for four doors
		  lblServiceDoorLabel->Visible  = bIsFourDoor;
		  FrontRightDoorLabel->Visible  = bIsFourDoor;
		  FrontLeftDoorLabel ->Visible  = bIsFourDoor;
		  ServiceKeyDoorLabel->Visible  = bIsFourDoor;

		  serviceDoorImg->Visible  = bIsFourDoor;
		  FrontRightDoorImg->Visible  = bIsFourDoor;
		  FrontLeftDoorImg ->Visible  = bIsFourDoor;
		  ServiceKeyDoorImg->Visible  = bIsFourDoor;
		  EmergencyFrontImg->Visible  = bIsFourDoor;
		  EmergencyFrontlbl->Visible  = bIsFourDoor;
		  EmergencyRearImg ->Visible  = bIsFourDoor;
		  EmergencyRearlbl->Visible   = bIsFourDoor;

		  //elements for one door
		  InterlockLbl ->Visible  = (!bIsFourDoor);
		  InterlockImg ->Visible = (!bIsFourDoor);
		}
}


//---------------------------------------------------------------------------
//! Initialize the indicator for the purge bath overflow sensor indicator
void TActuatorsControlForm::InitPurgeBath()
{
    bool doesPurgeBathExist = m_ParamMgr->PurgeBathEnabled;

    PurgeBathLabel->Visible  = doesPurgeBathExist;
    PurgeBathImg->Visible    = doesPurgeBathExist;

    if (doesPurgeBathExist)
    {
        PurgeBathImg->Tag = SENSOR_ID_0;
        LOAD_BITMAP(PurgeBathImg->Picture->Bitmap, ((GetInputStatus(SENSOR_ID_0)) ? IDB_GREEN_LED : IDB_RED_LED));
    }
}


//---------------------------------------------------------------------------
void __fastcall TActuatorsControlForm::ActuatorOnActionExecute(TObject *Sender)
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	TAction *ActionItem = dynamic_cast<TAction *>(Sender);
	TButton *Button = dynamic_cast<TButton *>(ActionItem->ActionComponent);

    // No valid Actuator ID assigned to the button
	if(Button->Tag == -1)
		return;

	if (CHECK_EMULATION(m_ParamMgr->OCB_Emulation == true))
		ChangeLedsEmulation(true, Button->Tag);

	try
	{
		Button->Action = ActuatorOffAction;
		BackEnd->SetActuatorState(Button->Tag, true);
	   if(Button->Tag == 17) //In order to update the interlock (General devices window)
		{
			FrontEndInterface->UpdateStatus(FE_CURRENT_DOOR_STATUS, 1);
		}
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
	}
}/*******************************************************************************************************************/


#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TActuatorsControlForm::FormShow(TObject *  Sender)
{
	CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);



	//dual waste moved

	//signal tower moved

	//ambient heater moved
	//removable tray moved

	if (CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
	{
		Caption += " (Emulation)";
		DisplayEmulationState();
	}

	try
	{
		BackEnd->MarkIfPollingControlIsOn(true);
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
	}

	if (FrontEndParams->ActuatorsDlgLeftPosition != -1)
		Left = FrontEndParams->ActuatorsDlgLeftPosition;
	else
		Left = (Screen->Width - Width) / 2;

	if (FrontEndParams->ActuatorsDlgTopPosition != -1)
		Top = FrontEndParams->ActuatorsDlgTopPosition;
	else
		Top = (Screen->Height - Height) / 2;

	FrontEndInterface->RefreshStatus();
}
//---------------------------------------------------------------------------
void __fastcall TActuatorsControlForm::FormClose(TObject *Sender,
		TCloseAction &Action)
{
	CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

	FrontEndParams->ActuatorsDlgLeftPosition = Left;
	FrontEndParams->ActuatorsDlgTopPosition = Top;
	FrontEndParams->ActuatorsDlgAlwaysOnTop = (FormStyle == fsStayOnTop);

	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	try
	{
		BackEnd->MarkIfPollingControlIsOn(false);
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
	}

}
//---------------------------------------------------------------------------
void __fastcall TActuatorsControlForm::AlwaysOnTopActionExecute(TObject *Sender)
{
	AlwaysOnTopAction->Checked = !AlwaysOnTopAction->Checked;

	// Temporarly disable the OnShow event
	TNotifyEvent TmpEvent = OnShow;
	OnShow = NULL;
	FormStyle = AlwaysOnTopAction->Checked ? fsStayOnTop : fsNormal;
	OnShow = TmpEvent;
}
//---------------------------------------------------------------------------
void __fastcall TActuatorsControlForm::CloseDialogActionExecute(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TActuatorsControlForm::FormCreate(TObject *Sender)
{
	Height = DESIGNED_FORM_HEIGHT;
}


//---------------------------------------------------------------------------
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

