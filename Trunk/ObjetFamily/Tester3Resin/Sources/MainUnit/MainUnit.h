//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include "TesterApplication.h"
#include "HeadTypeSelector.h"
#include <ActnList.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------



class TMainForm : public TForm
{
__published:	// IDE-managed Components
  TMainMenu *MainMenu;
  TMenuItem *Setup;
  TActionList *ActionList1;
  TAction *ActivateParamsDialogAction;
  TAction *OpenSetupAction;
  TMenuItem *OCBsimulator;
  TAction *OCBSimulatorAction;
  TMenuItem *OHDBTester;
  TMenuItem *OCBTester;
  TMenuItem *MCBSimulator;
  TMenuItem *Aplications1;
  TMenuItem *HeadsTester;
  TMenuItem *ReliabilityTest;
        TMenuItem *RFID;
	TMenuItem *ChecksumGenMenuItem;
	TOpenDialog *OpenFileDialog;
	TStatusBar *StatusBar;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall ActivateParamsDialogActionExecute(TObject *Sender);
  void __fastcall OpenSetupActionExecute(TObject *Sender);
  void __fastcall OCBSimulatorActionExecute(TObject *Sender);
  void __fastcall OHDBTesterClick(TObject *Sender);
  void __fastcall OCBTesterClick(TObject *Sender);
  void __fastcall MCBSimulatorClick(TObject *Sender);
  void __fastcall HeadsTesterClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall ReliabilityTestClick(TObject *Sender);
        void __fastcall RFIDClick(TObject *Sender);
	void __fastcall ChecksumGenMenuItemClick(TObject *Sender);
private:	// User declarations

  
  // pointer to the system parametres manager instance
  CSystemParams* m_SystemParam;

  // pointer to the application parametrs manager
  CAppParams* m_AppParams;

  // pointer to the tester application singelton instance
  CTesterApplication *m_TesterApplication;

  THeadTypeSelectionResult m_HeadTypeSelectionResult;

  CEdenProtocolEngine* GetOHDBProtocolEngine();
  CEdenProtocolEngine* GetOCBProtocolEngine();
public:		// User declarations
  __fastcall TMainForm(TComponent* Owner);

  // Get the instance of the system parametres manager
  CSystemParams* GetSystemParamsIntance();
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
