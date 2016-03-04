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

const int DESIGNED_FORM_HEIGHT = INT_FROM_RESOURCE(IDN_DESIGNED_FORM_HEIGHT);

//---------------------------------------------------------------------------
__fastcall TActuatorsControlForm::TActuatorsControlForm(TComponent* Owner)
	: TForm(Owner)
{
	m_ParamMgr           = CAppParams::Instance();
	m_ActuatorsStatus    = 0;
	m_InputsStatus       = 0;

#ifdef OBJET_MACHINE
	memset(m_PumpTankArr,-1,MAX_ACTUATOR_ID);

	Model1LeftPumpLbl->Tag  = ACTUATOR_ID_MODEL1_H0_1_PUMP;
	Model1LeftPumpBtn->Tag = ACTUATOR_ID_MODEL1_H0_1_PUMP;
	Model1LeftPumpImg->Tag  = ACTUATOR_ID_MODEL1_H0_1_PUMP;
	Model1LeftPumpLbl->Caption = "Model 1 Left";
	m_PumpTankArr[Model1LeftPumpBtn->Tag] = TYPE_TANK_MODEL1;

	Model1RightPumpLbl->Tag  = ACTUATOR_ID_MODEL2_H0_1_PUMP;
	Model1RightPumpBtn->Tag = ACTUATOR_ID_MODEL2_H0_1_PUMP;
	Model1RightPumpImg->Tag  = ACTUATOR_ID_MODEL2_H0_1_PUMP;
	Model1RightPumpLbl->Caption = "Model 1 Right";
	m_PumpTankArr[Model1RightPumpBtn->Tag] = TYPE_TANK_MODEL2;

	Model2LeftPumpLbl->Tag  = ACTUATOR_ID_MODEL3_H2_3_PUMP;
	Model2LeftPumpBtn->Tag = ACTUATOR_ID_MODEL3_H2_3_PUMP;
	Model2LeftPumpImg->Tag  = ACTUATOR_ID_MODEL3_H2_3_PUMP;
	Model2LeftPumpLbl->Caption = "Model 2 Left";
	m_PumpTankArr[Model2LeftPumpBtn->Tag] = TYPE_TANK_MODEL3;

	Model2RightPumpLbl->Tag  = ACTUATOR_ID_MODEL4_H2_3_PUMP;
	Model2RightPumpBtn->Tag = ACTUATOR_ID_MODEL4_H2_3_PUMP;
	Model2RightPumpImg->Tag  = ACTUATOR_ID_MODEL4_H2_3_PUMP;
	Model2RightPumpLbl->Caption = "Model 2 Right";
	m_PumpTankArr[Model2RightPumpBtn->Tag] = TYPE_TANK_MODEL4;

	Model3LeftPumpLbl->Tag  = ACTUATOR_ID_MODEL5_H4_5_PUMP;
	Model3LeftPumpBtn->Tag = ACTUATOR_ID_MODEL5_H4_5_PUMP;
	Model3LeftPumpImg->Tag  = ACTUATOR_ID_MODEL5_H4_5_PUMP;
	Model3LeftPumpLbl->Caption = "Model 3 Left";
	m_PumpTankArr[Model3LeftPumpBtn->Tag] = TYPE_TANK_MODEL5;

	Model3RightPumpLbl->Tag  = ACTUATOR_ID_MODEL6_H4_5_PUMP;
	Model3RightPumpBtn->Tag = ACTUATOR_ID_MODEL6_H4_5_PUMP;
	Model3RightPumpImg->Tag  = ACTUATOR_ID_MODEL6_H4_5_PUMP;
	Model3RightPumpLbl->Caption = "Model 3 Right";
	m_PumpTankArr[Model3RightPumpBtn->Tag] = TYPE_TANK_MODEL6;

	m_PumpTankArr[S1PumpBtn->Tag] = TYPE_TANK_SUPPORT1;
	m_PumpTankArr[S2PumpBtn->Tag] = TYPE_TANK_SUPPORT2;

	//arrange M3 and Support controls
	Model3LeftPumpLbl->Top  = Model2RightPumpLbl->Top+21;
	Model3LeftPumpBtn->Top = Model2RightPumpBtn->Top+21;
	Model3LeftPumpImg->Top  = Model2RightPumpImg->Top+21;

	Model3RightPumpLbl->Top  = Model2RightPumpLbl->Top+42;
	Model3RightPumpBtn->Top = Model2RightPumpBtn->Top+42;
	Model3RightPumpImg->Top  = Model2RightPumpImg->Top+42;

	S1PumpLabel->Top = Model2RightPumpLbl->Top+63;
	S1PumpBtn->Top = Model2RightPumpBtn->Top+63;
	S1PumpImage->Top = Model2RightPumpImg->Top+63;

	S2PumpLabel->Top = Model2RightPumpLbl->Top+84;
	S2PumpBtn->Top = Model2RightPumpBtn->Top+84;
	S2PumpImage->Top = Model2RightPumpImg->Top+84;

	//Drain pumps not available
	M1DrainPumpBtn->Top = DESIGNED_FORM_HEIGHT+10;  //hide button
	M2DrainPumpBtn->Top = DESIGNED_FORM_HEIGHT+10;  //hide button
	S1DrainPumpBtn->Top = DESIGNED_FORM_HEIGHT+10;  //hide button
	S2DrainPumpBtn->Top = DESIGNED_FORM_HEIGHT+10;  //hide button
	M1DrainPumpLabel->Visible = false;
	M2DrainPumpLabel->Visible = false;
	M1DrainPumpImage->Visible = false;
	M2DrainPumpImage->Visible = false;
	S1DrainPumpLabel->Visible = false;
	S2DrainPumpLabel->Visible = false;
	S1DrainPumpImage->Visible = false;
	S2DrainPumpImage->Visible = false;
	DrainPumpsBevel->Visible = false;
	DrainPumpsLabel->Visible = false;

#ifdef LIMIT_TO_DM2
	Model1LeftMSImg->Tag 	= SENSOR_ID_20;
	Model1RightMSImg->Tag 	= SENSOR_ID_21;
	Model2LeftMSImg->Tag 	= SENSOR_ID_18;
	Model2RightMSImg->Tag 	= SENSOR_ID_19;

	Model2LeftPumpImg->Tag	= 34;
	S1PumpImage->Tag		= 40;
	S2PumpImage->Tag		= 41;
	Model2RightPumpImg->Tag	= 35;

	Model2LeftPumpBtn->Tag	= 34;
	Model2RightPumpBtn->Tag = 35;
	S1PumpBtn->Tag			= 40;
	S2PumpBtn->Tag			= 41;
#else

	SupportLeftMSImg->Tag 	= SENSOR_ID_16;
	SupportRightMSImg->Tag 	= SENSOR_ID_17;
	Model1LeftMSImg->Tag    = SENSOR_ID_18;
	Model1RightMSImg->Tag   = SENSOR_ID_19;
	Model2LeftMSImg->Tag 	= SENSOR_ID_20;
	Model2RightMSImg->Tag 	= SENSOR_ID_21;

	Model2LeftPumpImg->Tag	= 40;
	S1PumpImage->Tag		= 32;
	S2PumpImage->Tag		= 33;
	Model2RightPumpImg->Tag	= 41;

	Model2LeftPumpBtn->Tag	= 40;
	Model2RightPumpBtn->Tag = 41;
	S1PumpBtn->Tag			= 32;
	S2PumpBtn->Tag			= 33;
#endif

	Model3LeftMSImg->Tag 	= SENSOR_ID_22;
	Model3RightMSImg->Tag 	= SENSOR_ID_23;

#else
	Model1LeftPumpLbl->Tag  = ACTUATOR_ID_MODEL1_H0_1_PUMP;
	Model1LeftPumpBtn->Tag = ACTUATOR_ID_MODEL1_H0_1_PUMP;
	Model1LeftPumpImg->Tag  = ACTUATOR_ID_MODEL1_H0_1_PUMP;

	Model1RightPumpLbl->Tag  = ACTUATOR_ID_MODEL1_H2_3_PUMP;
	Model1RightPumpBtn->Tag = ACTUATOR_ID_MODEL1_H2_3_PUMP;
	Model1RightPumpImg->Tag  = ACTUATOR_ID_MODEL1_H2_3_PUMP;

	Model2LeftPumpLbl->Tag  = ACTUATOR_ID_MODEL2_H0_1_PUMP;
	Model2LeftPumpBtn->Tag = ACTUATOR_ID_MODEL2_H0_1_PUMP;
	Model2LeftPumpImg->Tag  = ACTUATOR_ID_MODEL2_H0_1_PUMP;

	Model2RightPumpLbl->Tag  = ACTUATOR_ID_MODEL2_H2_3_PUMP;
	Model2RightPumpBtn->Tag = ACTUATOR_ID_MODEL2_H2_3_PUMP;
	Model2RightPumpImg->Tag  = ACTUATOR_ID_MODEL2_H2_3_PUMP;
	Model3InsertedLabel->Visible = false;
	Model2LeftMSImg->Visible = false;
	Model4InsertedLabel->Visible = false;
	Model2RightMSImg->Visible = false;
	Model5InsertedLabel->Visible = false;
	Model3LefttMSImg->Visible = false;
	Model6InsertedLabel->Visible = false;
	Model3RightMSImg->Visible = false;

#endif

    // Init container in-place leds to Red
	for(int i = 0; i < SensorsGroupBox->ControlCount; i++)
	{
		TImage *Image  = dynamic_cast<TImage *>(SensorsGroupBox->Controls[i]);
		if(Image)
		{
			if(Image->Tag >= SENSOR_ID_16 && Image->Tag <= SENSOR_ID_23)
				LOAD_BITMAP(Image->Picture->Bitmap, IDB_RED_LED);
        }
	}

	if(m_ParamMgr->RemovableTray == false)
	{
//#ifndef CONNEX_260  // hide the Tray Inserted sensor indication
		TrayHeaterLabel->Visible  = false;
		TrayHeaterButton->Visible = false;
		TrayHeaterImage->Visible  = false;

		YZTMotorsPowerLabel->Top  = TrayHeaterLabel->Top;
		YZTMotorsPowerButton->Top = TrayHeaterButton->Top;
		YZTMotorsPowerImage->Top  = TrayHeaterImage->Top;
//#else
	}
	else
	{
		TrayHeaterLabel->Visible  = true;
		TrayHeaterButton->Visible = true;
		TrayHeaterImage->Visible  = true;
//#endif
	}


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

		//reorganize the support drain pumps
		S1DrainPumpLabel->Top = M1DrainPumpLabel->Top;
		S2DrainPumpLabel->Top = M2DrainPumpLabel->Top;
		S1DrainPumpBtn->Top = M1DrainPumpBtn->Top;
		S2DrainPumpBtn->Top = M2DrainPumpBtn->Top;
		S1DrainPumpImage->Top = M1DrainPumpImage->Top;
		S2DrainPumpImage->Top = M2DrainPumpImage->Top;

		//prevent collision between the drain pumps and the roller suction system
		M1DrainPumpBtn->Tag = -1;
		M2DrainPumpBtn->Tag = -1;
		M1DrainPumpImage->Tag = -1;
		M2DrainPumpImage->Tag = -1;

		M1DrainPumpBtn->Top = DESIGNED_FORM_HEIGHT+10;  //hide button
		M2DrainPumpBtn->Top = DESIGNED_FORM_HEIGHT+10;  //hide button
	}
	else
	{
#ifndef OBJET_MACHINE //drain pumps not available in objet
		M1DrainPumpLabel->Visible = true;
		M2DrainPumpLabel->Visible = true;

		M1DrainPumpBtn->Visible = true;
		M2DrainPumpBtn->Visible = true;

		M1DrainPumpImage->Visible = true;
		M2DrainPumpImage->Visible = true;
#endif

		RollerSuctionPumpButton->Top = DESIGNED_FORM_HEIGHT+10; //hide button
		RollerSuctionValveButton->Top = DESIGNED_FORM_HEIGHT+10; //hide button
	}
}
//---------------------------------------------------------------------------



// Update status on the frame (integer version)
void TActuatorsControlForm::UpdateStatus(int ControlID,int Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}

// Update status on the frame (float version)
void TActuatorsControlForm::UpdateStatus(int ControlID,float Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}

// Update status on the frame (string version)
void TActuatorsControlForm::UpdateStatus(int ControlID,QString Status)
{
	PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}

// Message handler for the WM_STATUS_UPDATE message
void TActuatorsControlForm::HandleStatusUpdateMessage(TMessage &Message)
{
	// find the image to update
	TComponent *MyComponent;
	TImage  *Image;
	TButton *Button;

	// Handle only the actuator and input messages
	if(Message.WParam >= FE_CURRENT_ACTUATOR_STATUS_START && Message.WParam < FE_CURRENT_ACTUATOR_STATUS_START + MAX_ACTUATOR_ID)
	{
		int ActuatorNum = Message.WParam - FE_CURRENT_ACTUATOR_STATUS_START;

		// Updating the Button's Action
		for(int i = 0; i < ComponentCount; i++)
		{
			MyComponent = Components[i];
			Button  = dynamic_cast<TButton *>(MyComponent);

			if(Button != NULL && Button->Tag  == ActuatorNum)
			{
				if(Message.LParam)
					Button->Action = ActuatorOffAction;
				else
					Button->Action = ActuatorOnAction;

				break;
			}
		}

		// Updating the Led
		for(int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
		{
			Image  = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
			if(Image != NULL && Image->Tag  == ActuatorNum)
			{
				// check if it is the first update of this control
				if(Image->Picture->Bitmap->Width != 0)
				{
					// check if the status has changed
					if(Message.LParam == (GetActuatorStatus((UINT64)ActuatorNum)? 1 : 0))
						return;
				}

				LOAD_BITMAP(Image->Picture->Bitmap, ((Message.LParam) ? IDB_GREEN_LED : IDB_RED_LED));
				SetActuatorStatus((UINT64)ActuatorNum, (bool)Message.LParam);
				break;
			}
		}
	}

	if(Message.WParam >= FE_CURRENT_OCB_SENSOR_STATUS_START && (Message.WParam < FE_CURRENT_OCB_SENSOR_STATUS_START + MAX_SENSOR_ID))
	{
		LEDIndicator(SensorsGroupBox,Message.WParam - FE_CURRENT_OCB_SENSOR_STATUS_START,(bool)Message.LParam);
	}
}
//---------------------------------------------------------------------------

void __fastcall TActuatorsControlForm::FormShow(TObject *Sender)
{
	CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);

	if(CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
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

	if(FrontEndParams->ActuatorsDlgLeftPosition != -1)
		Left = FrontEndParams->ActuatorsDlgLeftPosition;
	else
		Left = (Screen->Width - Width) / 2;

	if(FrontEndParams->ActuatorsDlgTopPosition != -1)
		Top = FrontEndParams->ActuatorsDlgTopPosition;
	else
		Top = (Screen->Height - Height) / 2;

	if(!m_ParamMgr->RemovableTray)
	{
		Label8->Visible = false;
		Image4->Visible = false;
	}
	/*
	#if !defined (CONNEX_260) && !defined (OBJET_260)  // hide the Tray Inserted sensor indication
	Label8->Visible = false;
	Image4->Visible = false;
	#endif
	*/ //runtime objet
	FrontEndInterface->RefreshStatus(); //this line makes all the right tanks to be the active tanks

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

bool TActuatorsControlForm::GetActuatorStatus(UINT64 ActuatorNum)
{

	if(m_ActuatorsStatus & ((UINT64)1 << ActuatorNum))
		return true;

	return false;
}

void TActuatorsControlForm::SetActuatorStatus(UINT64 ActuatorNum, bool Status)
{
	if(Status)
		m_ActuatorsStatus |= ((UINT64)1 << ActuatorNum);
	else
		m_ActuatorsStatus &= ~((UINT64)1 << ActuatorNum);

}


bool TActuatorsControlForm::GetInputStatus(int InputNum)
{
	if(m_InputsStatus & ((unsigned long)1 << InputNum))
		return true;

	return false;
}

void TActuatorsControlForm::SetInputStatus(int InputNum, bool Status)
{
	if(Status)
		m_InputsStatus |= ((unsigned long)1 << InputNum);
	else
		m_InputsStatus &= ~((unsigned long)1 << InputNum);
}

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


void TActuatorsControlForm::LEDIndicator(TWinControl *Control,int InputNum,bool OnOff)
{
	// find the image to update
	TImage *Image;

	for(int i = 0; i < Control->ControlCount; i++)
	{
		Image = dynamic_cast<TImage *>(Control->Controls[i]);

		if(Image != NULL)
			if(Image->Tag  == InputNum)
			{
				// check if it is the first update of this control
				if(Image->Picture->Bitmap->Width != 0)
				{
					// check if the status has changed
					if(OnOff == (GetInputStatus(InputNum)? 1 : 0))
						return;
				}
				LOAD_BITMAP(Image->Picture->Bitmap, ((OnOff) ? IDB_GREEN_LED : IDB_RED_LED));
				SetInputStatus(InputNum,OnOff);
				break;
			}
	}
}
void __fastcall TActuatorsControlForm::FormCreate(TObject *Sender)
{
	Height = DESIGNED_FORM_HEIGHT;
}
//---------------------------------------------------------------------------

void __fastcall TActuatorsControlForm::ActuatorOffActionExecute(TObject *Sender)
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	TAction *ActionItem = dynamic_cast<TAction *>(Sender);
	TButton *Button = dynamic_cast<TButton *>(ActionItem->ActionComponent);

	if(CHECK_EMULATION(m_ParamMgr->OCB_Emulation))
		ChangeLedsEmulation(false, Button->Tag);

	try
	{
		Button->Action = ActuatorOnAction;
		BackEnd->SetActuatorState(Button->Tag, false);
		//update only if pumps were activated
		if(m_PumpTankArr[Button->Tag]!=-1)
			BackEnd->UpdateWeightAfterMaterialConsumption(static_cast<TTankIndex>(m_PumpTankArr[Button->Tag]));
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
	}
}
//---------------------------------------------------------------------------

void __fastcall TActuatorsControlForm::ActuatorOnActionExecute(TObject *Sender)
{
	CBackEndInterface* BackEnd = CBackEndInterface::Instance();

	TAction *ActionItem = dynamic_cast<TAction *>(Sender);
	TButton *Button = dynamic_cast<TButton *>(ActionItem->ActionComponent);

	if(CHECK_EMULATION(m_ParamMgr->OCB_Emulation == true))
		ChangeLedsEmulation(true, Button->Tag);

	try
	{
		Button->Action = ActuatorOffAction;
		BackEnd->SetActuatorState(Button->Tag, true);
	}
	catch(EQException& Exception)
	{
		QMonitor.ErrorMessage(Exception.GetErrorMsg());
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
	for(int i = 0; i < ComponentCount; i++)
	{
		MyComponent = Components[i];
		Button  = dynamic_cast<TButton *>(MyComponent);

		if(Button != NULL)
			Button->Action = ActuatorOnAction;
	}

	// Updating the Actuators Leds
	for(int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
	{
		Image  = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
		if(Image != NULL)
			LOAD_BITMAP(Image->Picture->Bitmap, IDB_RED_LED);
	}

	// Updating the Sensors Leds
	for(int i = 0; i < SensorsGroupBox->ControlCount; i++)
	{
		Image  = dynamic_cast<TImage *>(SensorsGroupBox->Controls[i]);
		if(Image != NULL)
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

void TActuatorsControlForm::ChangeLedsEmulation(bool OnOff, int LedID, int SensorID /* = -1 by default */)
{
	TImage* Image = NULL;
	// Updating the Actuator Led
	for(int i = 0; i < ActuatorsGroupBox->ControlCount; i++)
	{
		Image  = dynamic_cast<TImage *>(ActuatorsGroupBox->Controls[i]);
		if(Image != NULL && Image->Tag == LedID)
		{
			LOAD_BITMAP(Image->Picture->Bitmap, ((OnOff) ? IDB_GREEN_LED : IDB_RED_LED));
			break;
		}
	}
}

