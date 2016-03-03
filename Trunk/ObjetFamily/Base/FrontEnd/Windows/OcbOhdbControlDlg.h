//---------------------------------------------------------------------------

#ifndef OcbOhdbControlDlgH
#define OcbOhdbControlDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#define WM_STATUS_UPDATE WM_USER

//---------------------------------------------------------------------------
class TOcbOhdbControlForm : public TForm
{
__published:	// IDE-managed Components
       
	//TImage *OcbOhdbControlImage;
	TGroupBox *PowerSupply;
	TLabel *Label6;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TPanel *VS_OCB_PowerSupplyStatusPanel;
	TPanel *VCC_OCB_PowerSupplyStatusPanel;
	TPanel *V_12V_OCB_PowerSupplyStatusPanel;
	TPanel *V_24V_OCB_PowerSupplyStatusPanel;
	TPanel *A2D_VS_OCB_PowerSupplyStatusPanel;
	TPanel *A2D_VCC_OCB_PowerSupplyStatusPanel;
	TPanel *A2D_V_12V_OCB_PowerSupplyStatusPanel;
	TPanel *A2D_V_24V_OCB_PowerSupplyStatusPanel;
	TGroupBox *GroupBox3;
	TLabel *Label27;
	TLabel *Label29;
	TLabel *Label31;
	TLabel *Label32;
	TPanel *VPP_OHDB_PowerSupplyStatusPanel;
	TPanel *VDD_OHDB_PowerSupplyStatusPanel;
	TPanel *A2D_VPP_OHDB_PowerSupplyStatusPanel;
	TPanel *A2D_VDD_OHDB_PowerSupplyStatusPanel;
	TAction *AlwaysOnTopAction;
	TPopupMenu *PopupMenu1;
	TGroupBox *GroupBox1;
	TLabel *Label9;
	TLabel *Label12;
	TPanel *I2C_1_24_A2D_StatusPanel;
	TPanel *I2C_1_7_A2D_StatusPanel;
	TPanel *I2C_1_5_A2D_StatusPanel;
	TPanel *I2C_1_3_A2D_StatusPanel;
	TPanel *I2C_A_8_StatusPanel;
	TPanel *I2C_1_24V_StatusPanel;
	TPanel *I2C_1_5V_StatusPanel;
	TPanel *I2C_1_3V_StatusPanel;
	TPanel *I2C_A_8V_StatusPanel;
	TPanel *I2C_1_7V_StatusPanel;
	TGroupBox *GroupBox2;
	TLabel *Label1;
	TLabel *Label2;
	TPanel *I2C_2_24_A2D_StatusPanel;
	TPanel *I2C_2_7_A2D_StatusPanel;
	TPanel *I2C_2_5_A2D_StatusPanel;
	TPanel *I2C_2_3_A2D_StatusPanel;
	TPanel *Panel5;
	TPanel *I2C_2_24V_StatusPanel;
	TPanel *I2C_2_5V_StatusPanel;
	TPanel *I2C_2_3V_StatusPanel;
	TPanel *Panel9;
	TPanel *I2C_2_7V_StatusPanel;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label5;
	TLabel *Label7;
	TLabel *Label4;
	TLabel *Label8;
	TLabel *Label10;
	TLabel *Label3;
	TLabel *Label11;
	TLabel *Label15;
	TGroupBox *GroupBox5;
	TLabel *Label50;
	TLabel *Label51;
	TLabel *Label52;
	TLabel *Label53;
	TLabel *Label54;
	TLabel *Label55;
	TLabel *Label56;
	TLabel *Label57;
	TPanel *I2C_3_24_A2D_StatusPanel;
	TPanel *I2C_3_7_A2D_StatusPanel;
	TPanel *I2C_3_5_A2D_StatusPanel;
	TPanel *I2C_3_3_A2D_StatusPanel;
	TPanel *I2C_3_24V_StatusPanel;
	TPanel *I2C_3_7V_StatusPanel;
	TPanel *I2C_3_5V_StatusPanel;
	TPanel *I2C_3_3V_StatusPanel;
	TPanel *Panel54;
	TPanel *Panel58;
	TGroupBox *GroupBox6;
	TLabel *Label60;
	TLabel *Label61;
	TLabel *Label62;
	TLabel *Label63;
	TLabel *Label64;
	TLabel *Label65;
	TLabel *LAbel66;
	TLabel *LAbel67;
	TPanel *I2C_4_24_A2D_StatusPanel;
	TPanel *I2C_4_7_A2D_StatusPanel;
	TPanel *I2C_4_5_A2D_StatusPanel;
	TPanel *I2C_4_3_A2D_StatusPanel;
	TPanel *I2C_4_24V_StatusPanel;
	TPanel *I2C_4_7V_StatusPanel;
	TPanel *I2C_4_5V_StatusPanel;
	TPanel *I2C_4_3V_StatusPanel;
	TPanel *Panel64;
	TPanel *Panel68;
	TPanel *A2D_VCC_OHDB_PowerSupplyStatusPanel;
	TPanel *VCC_OHDB_PowerSupplyStatusPanel;
	TLabel *Label16;
	TPanel *A2D_3_3V_OHDB_PowerSupplyStatusPanel;
	TPanel *Volts_3_3V_OHDB_PowerSupplyStatusPanel;
	TLabel *Label17;
	TPanel *A2D_1_2V_OHDB_PowerSupplyStatusPanel;
	TPanel *Volts_1_2V_OHDB_PowerSupplyStatusPanel;
	TLabel *Label25;
	//void __fastcall OpenOcbOhdbControlDlgActionExecute(TObject *Sender);
	void __fastcall CloseDialogActionExecute(TObject *Sender);
	void __fastcall AlwaysOnTopActionExecute(TObject *Sender);
	//void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall HelpButtonClick(TObject *Sender);

private:	// User declarations
    typedef struct { int       a2dMsgId;
                     int       valMsgId;
                     TPanel ** offsetA2dPanel;
                     TPanel ** offsetValPanel;
                     float     minVal;
                     float     maxVal;	} PanelUpdateHelperEntry;

    static PanelUpdateHelperEntry PanelUpdateHelperTable[];

    void     HandleStatusUpdateMessage(TMessage &Message);
    TPanel * FormOffset2TPanelPtr(TPanel ** appTPanel)
    {
        return *((TPanel**)(((int)this) + (int)appTPanel));
    }
    

public:		// User declarations
  __fastcall TOcbOhdbControlForm(TComponent* Owner);
  // Update status on the frame (integer version)
  void UpdateStatus(int ControlID,int Status);
  // Update status on the frame (float version)
  void UpdateStatus(int ControlID,float Status);
  // Update status on the frame (string version)
  void UpdateStatus(int ControlID,QString Status);
  

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_STATUS_UPDATE,TMessage,HandleStatusUpdateMessage);
END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TOcbOhdbControlForm *OcbOhdbControlForm;
//---------------------------------------------------------------------------
#endif
