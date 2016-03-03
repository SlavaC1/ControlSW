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
#include "OcbOhdbControlDlg.h"
#include "Power.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

const char  VOLTAGES_HELP_FILE_NAME[] = "Voltages.hlp";
TOcbOhdbControlForm *OcbOhdbControlForm;

//! OFFSET based on offsetof() in _stddef.h, only that it keeps the type so we can check it
#define OFFSET(class_,member_) (&(((class_*)(0))->member_))

TOcbOhdbControlForm::PanelUpdateHelperEntry TOcbOhdbControlForm::PanelUpdateHelperTable[] =
//                  a2dMsgId                                         valMsgId	                                           offsetA2dPanel                                                offsetValPanel                                           minVal             maxVal
{
	{ FE_CURRENT_A2D_VS_POWER_SUPPLIERS_STATUS,         FE_CURRENT_VS_POWER_SUPPLIERS_STATUS,          OFFSET(TOcbOhdbControlForm, A2D_VS_OCB_PowerSupplyStatusPanel),    OFFSET(TOcbOhdbControlForm, VS_OCB_PowerSupplyStatusPanel),          VS_MIN_VALUE,       VS_MAX_VALUE      },
	{ FE_CURRENT_A2D_VCC_POWER_SUPPLIERS_STATUS,        FE_CURRENT_VCC_POWER_SUPPLIERS_STATUS,         OFFSET(TOcbOhdbControlForm, A2D_VCC_OCB_PowerSupplyStatusPanel),   OFFSET(TOcbOhdbControlForm, VCC_OCB_PowerSupplyStatusPanel),         VCC_MIN_VALUE,      VCC_MAX_VALUE     },
	{ FE_CURRENT_A2D_12V_POWER_SUPPLIERS_STATUS,        FE_CURRENT_12V_POWER_SUPPLIERS_STATUS,         OFFSET(TOcbOhdbControlForm, A2D_V_12V_OCB_PowerSupplyStatusPanel), OFFSET(TOcbOhdbControlForm, V_12V_OCB_PowerSupplyStatusPanel),       _12V_MIN_VALUE,     _12V_MAX_VALUE    },
	{ FE_CURRENT_A2D_24V_POWER_SUPPLIERS_STATUS,        FE_CURRENT_24V_POWER_SUPPLIERS_STATUS,         OFFSET(TOcbOhdbControlForm, A2D_V_24V_OCB_PowerSupplyStatusPanel), OFFSET(TOcbOhdbControlForm, V_24V_OCB_PowerSupplyStatusPanel),       _24V_MIN_VALUE,     _24V_MAX_VALUE    },
	{ FE_CURRENT_OHDB_A2D_VPP_POWER_SUPPLIERS_STATUS,   FE_CURRENT_OHDB_VPP_POWER_SUPPLIERS_STATUS,    OFFSET(TOcbOhdbControlForm, A2D_VPP_OHDB_PowerSupplyStatusPanel),  OFFSET(TOcbOhdbControlForm, VPP_OHDB_PowerSupplyStatusPanel),        VPP_MIN_VALUE,      VPP_MAX_VALUE     },
	{ FE_CURRENT_OHDB_A2D_VDD_POWER_SUPPLIERS_STATUS,   FE_CURRENT_OHDB_VDD_POWER_SUPPLIERS_STATUS,    OFFSET(TOcbOhdbControlForm, A2D_VDD_OHDB_PowerSupplyStatusPanel),  OFFSET(TOcbOhdbControlForm, VDD_OHDB_PowerSupplyStatusPanel),  	   VDD_MIN_VALUE,      VDD_MAX_VALUE     },
	{ FE_CURRENT_OHDB_A2D_VCC_POWER_SUPPLIERS_STATUS,   FE_CURRENT_OHDB_VCC_POWER_SUPPLIERS_STATUS,    OFFSET(TOcbOhdbControlForm, A2D_VCC_OHDB_PowerSupplyStatusPanel),  OFFSET(TOcbOhdbControlForm, VCC_OHDB_PowerSupplyStatusPanel),  	   VCC_MIN_VALUE,      VCC_MAX_VALUE     },
	{ FE_CURRENT_OHDB_A2D_3_3_V_POWER_SUPPLIERS_STATUS, FE_CURRENT_OHDB_3_3_V_POWER_SUPPLIERS_STATUS,  OFFSET(TOcbOhdbControlForm, A2D_3_3V_OHDB_PowerSupplyStatusPanel), OFFSET(TOcbOhdbControlForm, Volts_3_3V_OHDB_PowerSupplyStatusPanel), V_3_3_MIN_VALUE,    V_3_3_MAX_VALUE   },
	{ FE_CURRENT_OHDB_A2D_1_2_V_POWER_SUPPLIERS_STATUS, FE_CURRENT_OHDB_1_2_V_POWER_SUPPLIERS_STATUS,  OFFSET(TOcbOhdbControlForm, A2D_1_2V_OHDB_PowerSupplyStatusPanel), OFFSET(TOcbOhdbControlForm, Volts_1_2V_OHDB_PowerSupplyStatusPanel), V_1_2_MIN_VALUE,    V_1_2_MAX_VALUE   },
	{ FE_CURRENT_MSC1_I2C_ANALOG_IN_24_STATUS,          FE_CURRENT_MSC1_I2C_ANALOG_IN_24V_STATUS,      OFFSET(TOcbOhdbControlForm, I2C_1_24_A2D_StatusPanel),             OFFSET(TOcbOhdbControlForm, I2C_1_24V_StatusPanel),                  MSC_24V_MIN_VALUE,  MSC_24V_MAX_VALUE },
	{ FE_CURRENT_MSC1_I2C_ANALOG_IN_7_STATUS,           FE_CURRENT_MSC1_I2C_ANALOG_IN_7V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_1_7_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_1_7V_StatusPanel),                   MSC_7V_MIN_VALUE,   MSC_7V_MAX_VALUE  },
	{ FE_CURRENT_MSC1_I2C_ANALOG_IN_5_STATUS,           FE_CURRENT_MSC1_I2C_ANALOG_IN_5V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_1_5_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_1_5V_StatusPanel),                   MSC_5V_MIN_VALUE,   MSC_5V_MAX_VALUE  },
	{ FE_CURRENT_MSC1_I2C_ANALOG_IN_3_STATUS,           FE_CURRENT_MSC1_I2C_ANALOG_IN_3V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_1_3_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_1_3V_StatusPanel),                   MSC_3_3V_MIN_VALUE, MSC_3_3V_MAX_VALUE},
	{ FE_CURRENT_MSC2_I2C_ANALOG_IN_24_STATUS,          FE_CURRENT_MSC2_I2C_ANALOG_IN_24V_STATUS,      OFFSET(TOcbOhdbControlForm, I2C_2_24_A2D_StatusPanel),             OFFSET(TOcbOhdbControlForm, I2C_2_24V_StatusPanel),                  MSC_24V_MIN_VALUE,  MSC_24V_MAX_VALUE },
	{ FE_CURRENT_MSC2_I2C_ANALOG_IN_7_STATUS,           FE_CURRENT_MSC2_I2C_ANALOG_IN_7V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_2_7_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_2_7V_StatusPanel),                   MSC_7V_MIN_VALUE,   MSC_7V_MAX_VALUE  },
	{ FE_CURRENT_MSC2_I2C_ANALOG_IN_5_STATUS,           FE_CURRENT_MSC2_I2C_ANALOG_IN_5V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_2_5_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_2_5V_StatusPanel),                   MSC_5V_MIN_VALUE,   MSC_5V_MAX_VALUE  },
	{ FE_CURRENT_MSC2_I2C_ANALOG_IN_3_STATUS,           FE_CURRENT_MSC2_I2C_ANALOG_IN_3V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_2_3_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_2_3V_StatusPanel),                   MSC_3_3V_MIN_VALUE, MSC_3_3V_MAX_VALUE},
	{ FE_CURRENT_MSC3_I2C_ANALOG_IN_24_STATUS,          FE_CURRENT_MSC3_I2C_ANALOG_IN_24V_STATUS,      OFFSET(TOcbOhdbControlForm, I2C_3_24_A2D_StatusPanel),             OFFSET(TOcbOhdbControlForm, I2C_3_24V_StatusPanel),                  MSC_24V_MIN_VALUE,  MSC_24V_MAX_VALUE },
	{ FE_CURRENT_MSC3_I2C_ANALOG_IN_7_STATUS,           FE_CURRENT_MSC3_I2C_ANALOG_IN_7V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_3_7_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_3_7V_StatusPanel),                   MSC_7V_MIN_VALUE,   MSC_7V_MAX_VALUE  },
	{ FE_CURRENT_MSC3_I2C_ANALOG_IN_5_STATUS,           FE_CURRENT_MSC3_I2C_ANALOG_IN_5V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_3_5_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_3_5V_StatusPanel),                   MSC_5V_MIN_VALUE,   MSC_5V_MAX_VALUE  },
	{ FE_CURRENT_MSC3_I2C_ANALOG_IN_3_STATUS,           FE_CURRENT_MSC3_I2C_ANALOG_IN_3V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_3_3_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_3_3V_StatusPanel),                   MSC_3_3V_MIN_VALUE, MSC_3_3V_MAX_VALUE},
	{ FE_CURRENT_MSC4_I2C_ANALOG_IN_24_STATUS,          FE_CURRENT_MSC4_I2C_ANALOG_IN_24V_STATUS,      OFFSET(TOcbOhdbControlForm, I2C_4_24_A2D_StatusPanel),             OFFSET(TOcbOhdbControlForm, I2C_4_24V_StatusPanel),                  MSC_24V_MIN_VALUE,  MSC_24V_MAX_VALUE },
	{ FE_CURRENT_MSC4_I2C_ANALOG_IN_7_STATUS,           FE_CURRENT_MSC4_I2C_ANALOG_IN_7V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_4_7_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_4_7V_StatusPanel),                   MSC_7V_MIN_VALUE,   MSC_7V_MAX_VALUE  },
	{ FE_CURRENT_MSC4_I2C_ANALOG_IN_5_STATUS,           FE_CURRENT_MSC4_I2C_ANALOG_IN_5V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_4_5_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_4_5V_StatusPanel),                   MSC_5V_MIN_VALUE,   MSC_5V_MAX_VALUE  },
	{ FE_CURRENT_MSC4_I2C_ANALOG_IN_3_STATUS,           FE_CURRENT_MSC4_I2C_ANALOG_IN_3V_STATUS,       OFFSET(TOcbOhdbControlForm, I2C_4_3_A2D_StatusPanel),              OFFSET(TOcbOhdbControlForm, I2C_4_3V_StatusPanel),                   MSC_3_3V_MIN_VALUE, MSC_3_3V_MAX_VALUE}
};

//---------------------------------------------------------------------------
__fastcall TOcbOhdbControlForm::TOcbOhdbControlForm(TComponent* Owner)
    : TForm(Owner)
{
}

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
    for (unsigned int i=0; i < ARRAYSIZE(PanelUpdateHelperTable); ++i)
    {
        if (Message.WParam == PanelUpdateHelperTable[i].a2dMsgId)
        {
            FormOffset2TPanelPtr(PanelUpdateHelperTable[i].offsetA2dPanel)->Caption = IntToStr(Message.LParam);
        }
        else if (Message.WParam == PanelUpdateHelperTable[i].valMsgId)
        {
            TPanel * pValPanel = FormOffset2TPanelPtr(PanelUpdateHelperTable[i].offsetValPanel);
            float    NewValue  = *((float *)& Message.LParam);

            pValPanel->Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
            if ((NewValue < PanelUpdateHelperTable[i].minVal) || (NewValue > PanelUpdateHelperTable[i].maxVal))
			{
				FormOffset2TPanelPtr(PanelUpdateHelperTable[i].offsetA2dPanel)->Color       = clRed;
				FormOffset2TPanelPtr(PanelUpdateHelperTable[i].offsetA2dPanel)->Font->Color = clWhite;
				pValPanel->Color       = clRed;
				pValPanel->Font->Color = clWhite;
			}
			else
			{
				FormOffset2TPanelPtr(PanelUpdateHelperTable[i].offsetA2dPanel)->Color       = clBtnFace;
				FormOffset2TPanelPtr(PanelUpdateHelperTable[i].offsetA2dPanel)->Font->Color = clBlack;
				pValPanel->Color       = clBtnFace;
				pValPanel->Font->Color = clBlack;
            }
        }
    }
}    

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TOcbOhdbControlForm::FormClose(TObject *Sender,TCloseAction &Action)
{
    CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

    FrontEndParams->OcbOhdbDlgLeftPosition = Left;
    FrontEndParams->OcbOhdbDlgTopPosition = Top;
	FrontEndParams->OcbOhdbDlgAlwaysOnTop = (FormStyle == fsStayOnTop);
}

void __fastcall TOcbOhdbControlForm::FormShow(TObject *Sender)
{
	CFrontEndParams *FrontEndParams = CFrontEndParams::Instance();

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
//---------------------------------------------------------------------------
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
void __fastcall TOcbOhdbControlForm::HelpButtonClick(TObject *Sender)
{
    Application->HelpFile = (Q2RTApplication->AppFilePath.Value() + LOAD_QSTRING(IDS_HELP_DIR) + VOLTAGES_HELP_FILE_NAME).c_str();
    Application->HelpContext(10);
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
