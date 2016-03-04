//---------------------------------------------------------------------------


#ifndef EdenGeneralDeviceFrameH
#define EdenGeneralDeviceFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include "QTypes.h"
#include <ComCtrls.hpp>
#include "AppParams.h"

// Define private windows message for status updates
#define WM_STATUS_UPDATE WM_USER
//---------------------------------------------------------------------------
class TGeneralDeviceFrame : public TFrame
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TGroupBox *GroupBox2;
        TGroupBox *UVLampsGroupBox;
  TGroupBox *GroupBox4;
  TSpeedButton *PowerButton;
  TSpeedButton *LockDoorButton;
  TButton *OCBResetButton;
  TButton *OHDBResetButton;
        TPanel *A2D_CurrAmbientTempPanel;
  TLabel *Label5;
  TSpeedButton *TurnUVLampsButton;
  TSpeedButton *TurnRollerButton;
  TLabel *Label9;
  TEdit *ReqRollerSpeedEdit;
  TLabel *Label11;
  TPanel *RollerStatusPanel;
  TGroupBox *GroupBox5;
  TButton *MCBResetButton;
  TLabel *Label1;
        TPanel *A2D_TrayTemperaturePanel;
  TLabel *Label2;
  TEdit *TrayTemperatureEdit;
  TSpeedButton *TurnTrayButton;
  TPanel *PowerStatusPanel;
  TPanel *DoorStatusPanel;
  TLabel *Label3;
  TPanel *FanStatusPanel;
  TGroupBox *GroupBox6;
  TLabel *Label4;
        TPanel *A2D_VacuumSensorAveragePanel;
  TActionList *ActionList1;
  TAction *PowerOnAction;
  TAction *PowerOffAction;
  TAction *LockDoorAction;
  TAction *UnlockDoorAction;
  TAction *MCBResetAction;
  TAction *TrayOnAction;
  TAction *TrayOffAction;
  TAction *AmbientTempOnAction;
  TAction *AmbientTempOffAction;
  TAction *RollerOnAction;
  TAction *RollerOffAction;
  TAction *UVLampsOnAction;
  TAction *UVLampsOffAction;
  TUpDown *ReqTrayTempUpDown;
  TUpDown *ReqRollerSpeedUpDown;
  TGroupBox *GroupBox7;
  TSpeedButton *CoolingFansSpeedButton;
  TAction *CoolingFansOnAction;
  TAction *CoolingFansOffAction;
  TLabel *Label7;
  TEdit *CoolingFansDutyCycleEdit;
  TUpDown *CoolingFansDutyCycleUpDown;
        TPanel *A2D_VacuumSensorLastValuePanel;
        TLabel *Label12;
        TSpeedButton *OdourFanButton;
        TAction *AmbientOdourFanOnAction;
        TAction *AmbientOdourFanOffAction;
        TEdit *OdourFanValueEdit;
        TUpDown *OdourFanValueUpDown;
        TPanel *UVSensor;
        TLabel *Label8;
        TPanel *C_TrayTemperaturePanel;
        TLabel *Label15;
        TLabel *Label16;
        TPanel *C_CurrAmbientTempPanel;
        TLabel *Label17;
        TLabel *Label18;
        TPanel *H2O_VacuumSensorAveragePanel;
        TPanel *H2O_VacuumSensorLastValuePanel;
        TLabel *Label19;
        TLabel *Label20;
        TCheckBox *DisplayValuesModeCheckBox;
        TPanel *A2DTrayTemperaturePanel;
        TButton *RestartAveragingButton;
        TPanel *SensorSyncAveragePanel;
        TLabel *Label21;
        TTimer *Timer1;
        TGroupBox *UVDimmingGroupBox;
        TButton *SetUVDimmingButton;
    TGroupBox *UVOnOffGroupBox;
        TPanel *RSSPumpOnValuePanel;
        TPanel *RSSPumpOffValuePanel;
        TLabel *SuctionValveOnLabel;
        TLabel *SuctionValveOffLabel;
  void __fastcall PowerOnActionExecute(TObject *Sender);
  void __fastcall PowerOffActionExecute(TObject *Sender);
  void __fastcall LockDoorActionExecute(TObject *Sender);
  void __fastcall UnlockDoorActionExecute(TObject *Sender);
  void __fastcall MCBResetActionExecute(TObject *Sender);
  void __fastcall TrayOnActionExecute(TObject *Sender);
  void __fastcall TrayOffActionExecute(TObject *Sender);
  void __fastcall RollerOnActionExecute(TObject *Sender);
  void __fastcall RollerOffActionExecute(TObject *Sender);
  void __fastcall UVLampsOnActionExecute(TObject *Sender);
  void __fastcall UVLampsOffActionExecute(TObject *Sender);
  void __fastcall CoolingFansOnActionExecute(TObject *Sender);
  void __fastcall CoolingFansOffActionExecute(TObject *Sender);
        void __fastcall AmbientOdourFanOffActionExecute(TObject *Sender);
        void __fastcall AmbientOdourFanOnActionExecute(TObject *Sender);
        void __fastcall DisplayValuesModeCheckBoxClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall RestartAveragingButtonClick(TObject *Sender);
        void __fastcall SetUVDimmingButtonClick(TObject *Sender);
        //void __fastcall UVDimmingChangActionExecute(TObject *Sender);
    void __fastcall UVTrackBarChange(TObject *Sender);

        void __fastcall UVLampsCheckBoxClick(TObject *Sender);
private:	// User declarations


  TGroupBox*    m_UVGroupBox        [NUMBER_OF_UV_LAMPS];
  TPanel*       m_UVPanelArray      [NUMBER_OF_UV_LAMPS];
  TPanel*       m_UVLampPanelArray  [NUMBER_OF_UV_LAMPS];
  TTrackBar*    m_UVTrackBarArray   [NUMBER_OF_UV_LAMPS];
  int           m_SetUVDimming      [NUMBER_OF_UV_LAMPS];
  TCheckBox*    m_UVLampsChecks     [NUMBER_OF_UV_LAMPS];
  TLabel*       m_UVLampLabelArray  [NUMBER_OF_UV_LAMPS];
  BYTE          m_UvLampsMask;


  // Message handler for the WM_STATUS_UPDATE message
  void HandleStatusUpdateMessage(TMessage &Message);

public:		// User declarations

  // Display the current parameters for the tray temperature, ambient temperature and roller speed
  void DisplayCurrentParameters();

  // Update the current UV Dimming
  void UpdateUVDimming();

  // Update the current devices status
  void UpdateDevicesStatus();

  __fastcall TGeneralDeviceFrame(TComponent* Owner);

  // Update status on the frame (integer version)
  void UpdateStatus(int ControlID,int Status);

  // Update status on the frame (float version)
  void UpdateStatus(int ControlID,float Status);

  // Update status on the frame (string version)
  void UpdateStatus(int ControlID,QString Status);

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_STATUS_UPDATE,TMessage,HandleStatusUpdateMessage);
END_MESSAGE_MAP(TFrame);


};
//---------------------------------------------------------------------------
#endif
