//---------------------------------------------------------------------------

#ifndef TankIdentificationNoticeH
#define TankIdentificationNoticeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Q2RTErrors.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "QMutex.h"
#include "QComponent.h"
#include "GlobalDefs.h"


#define WM_SHOW_DIALOG             WM_USER
#define WM_HIDE_DIALOG             WM_USER + 1
#define WM_SUPPRESS_DIALOG_POPUPS  WM_USER + 2
#define WM_ALLOW_DIALOG_POPUPS     WM_USER + 3


class CBackEndInterface;
class TTankIDNoticeTester;

//---------------------------------------------------------------------------
class TTankIDNotice : public TForm
{
__published:	// IDE-managed Components
        TImage *Image2;
        TLabel *Notice;
        TButton *OKButton;
        TLabel *CartridgeLabel;
        TLabel *CartridgeTypeLabel;
        TGroupBox *OptionsGroupBox;
        TRadioButton *IAcceptRadio;
        TRadioButton *IDontAcceptRadio;
        TLabel *TagIdLabel;
        TLabel *TagIDValueLabel;
        TMemo *WarningTextMemo;
        TGroupBox *InsertedResinGroupBox;
        TLabel *InsertedResinTypeLabel;
        TLabel *InsertedResinLabel;
        TGroupBox *MachineResinTypeGroupBox;
        TLabel *MachineResinLabel;
        TLabel *MachineResinTypeLabel;
        TGroupBox *ExpirationDateGroupBox;
        TLabel *ExpirationDateLabel;
        TLabel *ExpirationDateValueLabel;
        TLabel *NotificationTitleLabel;
        TButton *HelpButton;
        TRadioButton *RunResinReplacementRadio;
        TLabel *NotRecommendedLabel;
        TLabel *PipeResinTypeLabel;
  TLabel *Label1;
        void __fastcall OKButtonClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall IDontAcceptRadioClick(TObject *Sender);
        void __fastcall IAcceptRadioClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall HelpButtonClick(TObject *Sender);
  void __fastcall FormActivate(TObject *Sender);
private:	// User declarations

  DWORD m_GUIMainThread;
  QString m_ContainerName;
  
  CBackEndInterface *m_BackEndInterface;

  // Is the dialog modal
  bool m_Modal;
  bool m_ShowDialogPending;

  int  m_DlgType;
  TTankIndex  m_Cartridge;
  bool m_SuppressDialogPopups;

  // Pointers to the forms:
  static TTankIDNotice *m_TankIDNoticeArray[TOTAL_NUMBER_OF_CONTAINERS];

  static CQMutex m_MutexFormShowSync;
  static TTankIDNoticeTester* m_Tester;
  QString m_dlgText;

  TModalResult m_ChooseModeModalResult; 

public:		// User declarations

  __fastcall TTankIDNotice(TComponent* Owner);

  // Creates the Form instances
  static TQErrCode CreateTankIDNoticeForms();

  // Destroy the Form instances
  static TQErrCode DestroyTankIDNoticeForms();

  // Send a message to show the dialog (blocking)
  static TQErrCode ShowDialog(int DialogType, TTankIndex Cartridge, bool Modal);

  // General messages sent to TTankIDNotice dialogs
  static void DlgSendMessage(int Cartridge, int Msg, bool Blocking);

  static TQErrCode GetDialog(int Cartridge, TTankIDNotice** CurrentForm, HANDLE* hdl);

  static void HideDialog(int Cartridge);

  static void SuppressDialogPopups(int Cartridge);

  static void AllowDialogPopups(int Cartridge);

  static QString GetNoticeText(int Cartridge);

  int OpenDialog(int DialogType, int Cartridge);

  void FormUILogic();

  void SetContainerName (QString name);

  // Message handler for the WM_SHOW_DIALOG message
  MESSAGE void HandleShowDialogMessage(TMessage &Message);
  MESSAGE void HandleHideDialogMessage(TMessage &Message);
  MESSAGE void HandleSuppressDialogPopupsMessage(TMessage &Message);
  MESSAGE void HandleAllowDialogPopupsMessage(TMessage &Message);

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_SHOW_DIALOG,TMessage,HandleShowDialogMessage);
  MESSAGE_HANDLER(WM_HIDE_DIALOG,TMessage,HandleHideDialogMessage);
  MESSAGE_HANDLER(WM_SUPPRESS_DIALOG_POPUPS,TMessage,HandleSuppressDialogPopupsMessage);
  MESSAGE_HANDLER(WM_ALLOW_DIALOG_POPUPS,TMessage,HandleAllowDialogPopupsMessage);
END_MESSAGE_MAP(TForm);

};
//---------------------------------------------------------------------------
extern PACKAGE TTankIDNotice *TankIDNotice;
extern PACKAGE TTankIDNotice *TankIDNoticeModel1;
extern PACKAGE TTankIDNotice *TankIDNoticeModel2;
extern PACKAGE TTankIDNotice *TankIDNoticeSupport1;
extern PACKAGE TTankIDNotice *TankIDNoticeSupport1;
//---------------------------------------------------------------------------

class TTankIDNoticeTester : public CQComponent
{
public:
  TTankIDNoticeTester();
  DEFINE_V_METHOD_2(TTankIDNoticeTester,int,TankIDNoticeOpenDialog,int/*DialogType*/,int /* TTankIndex - Cartridge*/);
};

#endif
