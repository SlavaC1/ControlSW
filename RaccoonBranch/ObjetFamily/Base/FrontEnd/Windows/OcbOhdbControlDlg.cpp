//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BackEndInterface.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"
#include "FrontEndControlIDs.h"
#include "FrontEnd.h"
#include "AppParams.h"
#include "FEResources.h"
#include "FrontEndParams.h"
//#include "ThemeMgr.hpp"
#include "OcbOhdbControlDlg.h"
#include "Power.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const char  VOLTAGES_HELP_FILE_NAME[] = "Voltages.hlp";

TOcbOhdbControlForm *OcbOhdbControlForm;
//---------------------------------------------------------------------------
__fastcall TOcbOhdbControlForm::TOcbOhdbControlForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TOcbOhdbControlForm::FormClose(TObject *Sender,TCloseAction &Action)
{
  CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
  
  FrontEndParams->OcbOhdbDlgLeftPosition = Left;
  FrontEndParams->OcbOhdbDlgTopPosition = Top;
  FrontEndParams->OcbOhdbDlgAlwaysOnTop = (FormStyle == fsStayOnTop);

  CBackEndInterface* BackEnd = CBackEndInterface::Instance();
}
//---------------------------------------------------------------------------

void __fastcall TOcbOhdbControlForm::FormShow(TObject *Sender)
{
 /* // To show the components properly themed on windows XP
  for (int i = 0 ; dynamic_cast<TForm *>(Application->MainForm)->ComponentCount - 1 ; i++)
  {
	if (dynamic_cast<TThemeManager *>(Application->MainForm->Components[i]))
	{
		if (dynamic_cast<TThemeManager *>(dynamic_cast<TForm *>(Application->MainForm)->Components[i])->IsMainManager)
		{
			dynamic_cast<TThemeManager *>(dynamic_cast<TForm *>(Application->MainForm)->Components[i])->CollectForms();

			for (int j = 0 ; j < dynamic_cast<TForm *>(Sender)->ComponentCount - 1; j++)
			{
				if (dynamic_cast<TWinControl *>(dynamic_cast<TForm *>(Sender)->Components[j]))
					dynamic_cast<TWinControl *>(dynamic_cast<TForm *>(Sender)->Components[j])->Repaint();
			}

			break;
		}
	}
  } */
  //-------------------------------------------

   CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();
   CBackEndInterface* BackEnd = CBackEndInterface::Instance();

  AlwaysOnTopAction->Checked = (FormStyle == fsStayOnTop);

  // Set from position
  if(FrontEndParams->OcbOhdbDlgLeftPosition != -1)
    Left = FrontEndParams->OcbOhdbDlgLeftPosition;
  else
    Left = (Screen->Width - Width) / 2;

  if (FrontEndParams->OcbOhdbDlgTopPosition != -1)
    Top = FrontEndParams->OcbOhdbDlgTopPosition;
  else
    Top = (Screen->Height - Height) / 2;
  FrontEndInterface->RefreshStatus();
 }
 
 void __fastcall TOcbOhdbControlForm::AlwaysOnTopActionExecute(TObject *Sender)
{
  AlwaysOnTopAction->Checked = !AlwaysOnTopAction->Checked;

  // Temporarly disable the OnShow event
  TNotifyEvent TmpEvent = OnShow;
  OnShow = NULL;
  FormStyle = AlwaysOnTopAction->Checked ? fsStayOnTop : fsNormal;
  OnShow = TmpEvent;
}
//---------------------------------------------------------------------------

void __fastcall TOcbOhdbControlForm::CloseDialogActionExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
   
 // Update status on the frame (integer version)
void TOcbOhdbControlForm::UpdateStatus(int ControlID,int Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,Status);
}

// Update status on the frame (float version)
void TOcbOhdbControlForm::UpdateStatus(int ControlID,float Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,*((int *)&Status));
}

// Update status on the frame (string version)
void TOcbOhdbControlForm::UpdateStatus(int ControlID,QString Status)
{
  PostMessage(Handle,WM_STATUS_UPDATE,ControlID,reinterpret_cast<int>(strdup(Status.c_str())));
}


// Message handler for the WM_STATUS_UPDATE message
void TOcbOhdbControlForm::HandleStatusUpdateMessage(TMessage &Message)
{

  switch(Message.WParam)
  {

    case FE_CURRENT_VS_POWER_SUPPLIERS_STATUS://Elad	 
         {
		     float NewValue = *((float *)& Message.LParam);
		     VS_OCB_PowerSupplyStatusPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			 if( NewValue < VS_MIN_VALUE || NewValue> VS_MAX_VALUE )
			    {
				  VS_OCB_PowerSupplyStatusPanel->Color = clRed;
				  A2D_VS_OCB_PowerSupplyStatusPanel->Color = clRed;
				}
			 else
                {			
     				VS_OCB_PowerSupplyStatusPanel->Color =clBtnFace;
					A2D_VS_OCB_PowerSupplyStatusPanel->Color = clBtnFace;
				}
			 break;
	     } 
	
    case FE_CURRENT_A2D_VS_POWER_SUPPLIERS_STATUS://Elad
          A2D_VS_OCB_PowerSupplyStatusPanel->Caption = IntToStr(Message.LParam);
		break;

		
    case FE_CURRENT_VCC_POWER_SUPPLIERS_STATUS://Elad	 
         {
		     float NewValue = *((float *)& Message.LParam);
			 VCC_OCB_PowerSupplyStatusPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			 if( NewValue < VCC_MIN_VALUE || NewValue > VCC_MAX_VALUE ) 
			    {
				 VCC_OCB_PowerSupplyStatusPanel->Color = clRed;
				 A2D_VCC_OCB_PowerSupplyStatusPanel->Color = clRed;
				}
			 else
			    {
        			 VCC_OCB_PowerSupplyStatusPanel->Color =clBtnFace;
					 A2D_VCC_OCB_PowerSupplyStatusPanel->Color = clBtnFace;
				}
			 break;
	     }
	
    case FE_CURRENT_A2D_VCC_POWER_SUPPLIERS_STATUS://Elad	
          A2D_VCC_OCB_PowerSupplyStatusPanel->Caption = IntToStr(Message.LParam);
		  break;
	
    case FE_CURRENT_12V_POWER_SUPPLIERS_STATUS://Elad	 
         {
		     float NewValue = *((float *)& Message.LParam);
			 V_12V_OCB_PowerSupplyStatusPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			 if( NewValue < _12V_MIN_VALUE || NewValue > _12V_MAX_VALUE )
			   {
				 V_12V_OCB_PowerSupplyStatusPanel->Color = clRed;
				 A2D_V_12V_OCB_PowerSupplyStatusPanel->Color = clRed;
			   }
			 else 
			     {
				   V_12V_OCB_PowerSupplyStatusPanel->Color =clBtnFace;
				   A2D_V_12V_OCB_PowerSupplyStatusPanel->Color =clBtnFace;
				   }
				  
			 break;
	     }
	
	
	case FE_CURRENT_A2D_12V_POWER_SUPPLIERS_STATUS://Elad	 
	      A2D_V_12V_OCB_PowerSupplyStatusPanel->Caption =IntToStr(Message.LParam);
		  break;
	
	
	case FE_CURRENT_24V_POWER_SUPPLIERS_STATUS://Elad	 
         {
		     float NewValue = *((float *)& Message.LParam);
			 V_24V_OCB_PowerSupplyStatusPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			 if( NewValue < _24V_MIN_VALUE || NewValue > _24V_MAX_VALUE )
     			{
					V_24V_OCB_PowerSupplyStatusPanel->Color = clRed;
					A2D_V_24V_OCB_PowerSupplyStatusPanel->Color = clRed;
				}
			 else
                {  
    				V_24V_OCB_PowerSupplyStatusPanel->Color =clBtnFace;
					A2D_V_24V_OCB_PowerSupplyStatusPanel->Color =clBtnFace;
                } 					
			 break;
	     }

	case FE_CURRENT_A2D_24V_POWER_SUPPLIERS_STATUS://Elad	 
	       A2D_V_24V_OCB_PowerSupplyStatusPanel->Caption = IntToStr(Message.LParam);
		   break;
		   
	case FE_CURRENT_OHDB_VPP_POWER_SUPPLIERS_STATUS://Elad	 
         {
		     float NewValue = *((float *)& Message.LParam);
			 VPP_OHDB_PowerSupplyStatusPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			 if( NewValue < VPP_MIN_VALUE || NewValue > VPP_MAX_VALUE ) 
			    {
				 VPP_OHDB_PowerSupplyStatusPanel->Color = clRed;
				 A2D_VPP_OHDB_PowerSupplyStatusPanel->Color = clRed;
				}
			 else
			    {
					 VPP_OHDB_PowerSupplyStatusPanel->Color =clBtnFace;
					 A2D_VPP_OHDB_PowerSupplyStatusPanel->Color = clBtnFace;
				}
			 break;
	     }
	
    case FE_CURRENT_OHDB_A2D_VPP_POWER_SUPPLIERS_STATUS://Elad	
          A2D_VPP_OHDB_PowerSupplyStatusPanel->Caption = IntToStr(Message.LParam);
		  break;	   
    


    case FE_CURRENT_OHDB_VDD_POWER_SUPPLIERS_STATUS://Elad	 
         {
		     float NewValue = *((float *)& Message.LParam);
			 VDD_OHDB_PowerSupplyStatusPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
			 if( NewValue < VDD_MIN_VALUE || NewValue > VDD_MAX_VALUE ) 
			    {
				 VDD_OHDB_PowerSupplyStatusPanel->Color = clRed;
				 A2D_VDD_OHDB_PowerSupplyStatusPanel->Color = clRed;
				}
			 else
			    {
        			 VDD_OHDB_PowerSupplyStatusPanel->Color =clBtnFace;
					 A2D_VDD_OHDB_PowerSupplyStatusPanel->Color = clBtnFace;
				}
			 break;
	     }
	
    case FE_CURRENT_OHDB_A2D_VDD_POWER_SUPPLIERS_STATUS://Elad	
          A2D_VDD_OHDB_PowerSupplyStatusPanel->Caption = IntToStr(Message.LParam);
		  break;

		  
		  
	case FE_CURRENT_MSC1_I2C_ANALOG_IN_7_STATUS://Elad
		  I2C_1_24_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC1_I2C_ANALOG_IN_7V_STATUS://Elad
		{
		  float NewValue = *((float *)& Message.LParam);
		  I2C_1_24V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		  if( NewValue < MSC_24V_MIN_VALUE || NewValue > MSC_24V_MAX_VALUE )
			{
			   I2C_1_24V_StatusPanel->Color = clRed;
			   I2C_1_24_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_1_24V_StatusPanel->Color = clBtnFace;
			  I2C_1_24_A2D_StatusPanel->Color = clBtnFace;
			}

		}
		break;


	case FE_CURRENT_MSC1_I2C_ANALOG_IN_8_STATUS://Elad
		  I2C_1_7_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;
    
	case FE_CURRENT_MSC1_I2C_ANALOG_IN_8V_STATUS://Elad
		 {
		   float NewValue = *((float *)& Message.LParam);
		   I2C_1_7V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		   if( NewValue < MSC_7V_MIN_VALUE || NewValue> MSC_7V_MAX_VALUE )
			{
			   I2C_1_7V_StatusPanel->Color = clRed;
			   I2C_1_7_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_1_7V_StatusPanel->Color = clBtnFace;
			  I2C_1_7_A2D_StatusPanel->Color = clBtnFace;
			}
		 }
		break;

	case FE_CURRENT_MSC1_I2C_ANALOG_IN_9_STATUS://Elad
		  I2C_1_5_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC1_I2C_ANALOG_IN_9V_STATUS://Elad
		 {
		  float NewValue = *((float *)& Message.LParam);
		  I2C_1_5V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		  if( NewValue < MSC_5V_MIN_VALUE || NewValue> MSC_5V_MAX_VALUE )
			{
			   I2C_1_5V_StatusPanel->Color = clRed;
			   I2C_1_5_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_1_5V_StatusPanel->Color = clBtnFace;
			  I2C_1_5_A2D_StatusPanel->Color = clBtnFace;
			}
		 }
		break;

	case FE_CURRENT_MSC1_I2C_ANALOG_IN_10_STATUS://Elad
		  I2C_1_3_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC1_I2C_ANALOG_IN_10V_STATUS://Elad
		 {
		  float NewValue = *((float *)& Message.LParam);
		  I2C_1_3V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		  if( NewValue < MSC_3_3V_MIN_VALUE || NewValue> MSC_3_3V_MAX_VALUE )
			{
			   I2C_1_3V_StatusPanel->Color = clRed;
			   I2C_1_3_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_1_3V_StatusPanel->Color = clBtnFace;
			  I2C_1_3_A2D_StatusPanel->Color = clBtnFace;
			}
		 }
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_7_STATUS://Elad
		  I2C_2_24_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_7V_STATUS://Elad
		{
		  float NewValue = *((float *)& Message.LParam);
		  I2C_2_24V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		  if( NewValue < MSC_24V_MIN_VALUE || NewValue> MSC_24V_MAX_VALUE )
			{
			   I2C_2_24V_StatusPanel->Color = clRed;
			   I2C_2_24_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_2_24V_StatusPanel->Color = clBtnFace;
			  I2C_2_24_A2D_StatusPanel->Color = clBtnFace;
			}

		}
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_8_STATUS://Elad
		  I2C_2_7_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_8V_STATUS://Elad
		 {
		   float NewValue = *((float *)& Message.LParam);
		   I2C_2_7V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		   if( NewValue < MSC_7V_MIN_VALUE || NewValue> MSC_7V_MAX_VALUE )
			{
			   I2C_2_7V_StatusPanel->Color = clRed;
			   I2C_2_7_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_2_7V_StatusPanel->Color = clBtnFace;
			  I2C_2_7_A2D_StatusPanel->Color = clBtnFace;
			}
		 }
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_9_STATUS://Elad
		  I2C_2_5_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_9V_STATUS://Elad
		 {
		  float NewValue = *((float *)& Message.LParam);
		  I2C_2_5V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		  if( NewValue < MSC_5V_MIN_VALUE || NewValue> MSC_5V_MAX_VALUE )
			{
			   I2C_2_5V_StatusPanel->Color = clRed;
			   I2C_2_5_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_2_5V_StatusPanel->Color = clBtnFace;
			  I2C_2_5_A2D_StatusPanel->Color = clBtnFace;
			}
		 }
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_10_STATUS://Elad
		  I2C_2_3_A2D_StatusPanel->Caption = IntToStr(Message.LParam);
		break;

	case FE_CURRENT_MSC2_I2C_ANALOG_IN_10V_STATUS://Elad
		 {
		  float NewValue = *((float *)& Message.LParam);
		  I2C_2_3V_StatusPanel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));

		 if( NewValue < MSC_3_3V_MIN_VALUE || NewValue> MSC_3_3V_MAX_VALUE )
			{
			   I2C_2_3V_StatusPanel->Color = clRed;
			   I2C_2_3_A2D_StatusPanel->Color = clRed;
			}

			else
			{
			  I2C_2_3V_StatusPanel->Color = clBtnFace;
			  I2C_2_3_A2D_StatusPanel->Color = clBtnFace;
			}
		 }
		break;


   default:
		 break;
	}


}

void __fastcall TOcbOhdbControlForm::HelpButtonClick(TObject *Sender)
{
  Application->HelpFile = (Q2RTApplication->AppFilePath.Value() + LOAD_QSTRING(IDS_HELP_DIR) + VOLTAGES_HELP_FILE_NAME).c_str();
  Application->HelpContext(10);
}
//---------------------------------------------------------------------------

