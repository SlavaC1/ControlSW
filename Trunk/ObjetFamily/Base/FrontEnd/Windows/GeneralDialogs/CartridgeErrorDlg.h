//---------------------------------------------------------------------------

#ifndef CartridgeErrorDlgH
#define CartridgeErrorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "Q2RTErrors.h"
#include <ActnList.hpp>

#include "GlobalDefs.h"
#include <map>

#define WM_SHOW_DIALOG          WM_USER
#define WM_INFORM_PRINTING_STOP WM_USER + 1

class CBackEndInterface;

typedef enum  {TANK_IN,TANK_OUT,TANK_IN_EMPTY} TTankStatus;

typedef map<TTankIndex, TTankStatus>           TTankIndex2TTankStatusMap;
typedef TTankIndex2TTankStatusMap::iterator    TTankIndex2TTankStatusMapIter;

//---------------------------------------------------------------------------
class TCartridgeErrorForm : public TForm
{
__published:	// IDE-managed Components
  TLabel *ErrorLabel;
  TButton *StopButton;
  TTimer *CartridgeStatusTimer;
        TImage *IconImage;
  TLabel *InstructionsLabel;
        TTimer *ElapsedTimeTimer;
        TActionList *ActionList;
        TAction *StopAndCloseAction;        
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall StopButtonClick(TObject *Sender);
  void __fastcall CartridgeStatusTimerTimer(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
        void __fastcall ElapsedTimeTimerTimer(TObject *Sender);
private:	// User declarations

  TChamberIndex m_Chamber; // Arcady: Is this the chamber where the problem occurred ?

  int  m_SecondsCounter;
  int  m_MinutesCounter;
  int  m_HoursCounter;

  int  m_DlgType;
  bool m_WasteRemovalDetected;
  bool m_HandleStopAndResumePrinting;
  bool m_HandleStopPrinting;

  bool m_WastePrevOnOff;

  CBackEndInterface *m_BackEndInterface;

  // Is the dialog modal
  bool m_Modal;

  void __fastcall WasteStatusTimerTimer(TObject *Sender);




  // Message handler for the WM_SHOW_DIALOG message
  MESSAGE void HandleShowDialogMessage(TMessage &Message);

  // Message handler for the WM_INFORM_PRINTING_STOP message
  MESSAGE void HandleInformStopToDlg       (TMessage &Message);

  void UpdateTanksState(void);
  int  CountTanksInNum(void);
  int  CountTanksEmptyNum(void);
  TModalResult ReturnValue(TModalResult aModalResult);

  TTankIndex2TTankStatusMap m_TanksStatus;

  bool m_ReadyForPrint;


public:		// User declarations
  __fastcall TCartridgeErrorForm(TComponent* Owner);
  int OpenCartridgeErrorDialog(int DlgType, TChamberIndex Cartridge);

  // Send a message to show the dialog (blocking)
  TQErrCode ShowDialog(int DialogType, int Cartridge, bool Modal);
  TQErrCode InformStopToDlg();

  void SetHandleStopAndResume(bool HandleStopAndResume);


BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_SHOW_DIALOG,TMessage,HandleShowDialogMessage);
  MESSAGE_HANDLER(WM_INFORM_PRINTING_STOP,TMessage,HandleInformStopToDlg);
END_MESSAGE_MAP(TForm);


};
//---------------------------------------------------------------------------
extern PACKAGE TCartridgeErrorForm *CartridgeErrorForm;
//---------------------------------------------------------------------------
#endif
