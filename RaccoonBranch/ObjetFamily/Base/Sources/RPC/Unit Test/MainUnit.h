//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "AllQLib.h"
#include "RPCServer.h"
#include "RPCClient.h"
#include <ComCtrls.hpp>


class CTestObject;


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TRadioGroup *OpKindRadioGroup;
        TEdit *ObjectNameEdit;
        TEdit *RPCItemEdit;
        TLabel *Label1;
        TLabel *Label2;
        TButton *ExecButton;
        TLabel *Label3;
        TEdit *ValueEdit;
        TButton *Button2;
        TLabel *Label4;
        TEdit *ResultEdit;
        TEdit *Edit1;
        TLabel *Label5;
        TUpDown *TimeoutUpDown;
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ExecButtonClick(TObject *Sender);
private:	// User declarations

  CQApplication *m_TesterApplication;

  CEdenProtocolEngine *m_TesterProtocolEngine;
  CRPCServer *m_RPCServer;
  CRPCClient *m_RPCClient;

  CTestObject *m_TestObject;  

  int ExtractMethodArguments(QString ArgsLine,QString *Args);  

public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
