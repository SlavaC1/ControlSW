//---------------------------------------------------------------------------


#ifndef EdenHeadsFrameH
#define EdenHeadsFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include <ActnList.hpp>
#include "QTypes.h"
#include "HeatersDefs.h"
#include "GlobalDefs.h"

// Define private windows message for status updates
#define WM_STATUS_UPDATE WM_USER

//---------------------------------------------------------------------------
class THeadsFrame : public TFrame
{
__published:	// IDE-managed Components
  TActionList *ActionList1;
  TAction *HeadHeatersOnAction;
  TAction *HeadHeatersOffAction;
  TAction *HeadFillingOnAction;
  TAction *HeadFillingOffAction;
	TLabel *ReqTempADUHeads;
	TLabel *CurTempADUHeads;
	TLabel *CurTempCelHeads;
	TGroupBox *HeadsFillingGroupBox;
        TButton *SaveParameterButton;
	TBitBtn *TurnHeadHeatersButton;
	TBevel *UpperThermistorsBevel;
	TBevel *LowerThermistorsBevel;
	TLabel *LowerThermistorsLabel;
	TLabel *UpperThermistorsLabel;
	TBitBtn *TurnHeadFillingButton;
	TLabel *CurTempCelFront;
	TLabel *CurTempADUFront;
	TLabel *ReqTempADUFront;
	TLabel *ReqTempADUHeadsRear;
	TLabel *CurTempADURear;
	TLabel *CurTempCelRear;
  void __fastcall HeadHeatersOnActionExecute(TObject *Sender);
  void __fastcall HeadHeatersOffActionExecute(TObject *Sender);
  void __fastcall HeadFillingOnActionExecute(TObject *Sender);
  void __fastcall HeadFillingOffActionExecute(TObject *Sender);
  void __fastcall SaveParameterButtonClick(TObject *Sender);

  void __fastcall ReqTempOnChange(TObject *Sender);
  void __fastcall ReqTempOnExit  (TObject *Sender);

private:	// User declarations

  TPanel*  HeadHeatersA2DTPanels    [TOTAL_NUMBER_OF_HEATERS];
  TPanel*  HeadHeatersCelsiusTPanels[TOTAL_NUMBER_OF_HEATERS];
  TLabel*  HeadHeatersLabels        [TOTAL_NUMBER_OF_HEATERS];
  TEdit*   HeadReqTempEdits         [TOTAL_NUMBER_OF_HEATERS];
  TUpDown* HeadReqTempUpDown        [TOTAL_NUMBER_OF_HEATERS];
  int      HeadReqTempEditsSaved    [TOTAL_NUMBER_OF_HEATERS];
  
 // Message handler for the WM_STATUS_UPDATE message
  void HandleStatusUpdateMessage(TMessage &Message);

  // Is purge active
  bool m_PurgeActive;
  TLabel* MaterialLevelLabelArray   [NUM_OF_CHAMBERS_THERMISTORS];

  TPanel* ActivePumpStatusPanelArray[NUM_OF_CHAMBERS_THERMISTORS];
  TPanel* MaterialLevelPanelArray   [NUM_OF_CHAMBERS_THERMISTORS];
public:		// User declarations
  __fastcall THeadsFrame(TComponent* Owner);

  // Display the current parameters for the head heaters temperature
  void DisplayCurrentParameters();

  // Update the current devices status
  void UpdateDevicesStatus();

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
extern PACKAGE THeadsFrame *HeadsFrame;
//---------------------------------------------------------------------------
#endif
