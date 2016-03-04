//---------------------------------------------------------------------------

#ifndef MCBSimulatorDlgH
#define MCBSimulatorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "EdenProtocolEngine.h"
#include "EdenProtocolClient.h"

//---------------------------------------------------------------------------
class TMCBSimulatorForm : public TForm
{
__published:	// IDE-managed Components
  TLabel *Label1;
  void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
  // the protocol engine
  CEdenProtocolEngine *m_ProtocolEngine;

  // the protocol client
  CEdenProtocolClient *m_ProtocolClient;

public:		// User declarations
  __fastcall TMCBSimulatorForm(TComponent* Owner);

  void Open(CEdenProtocolEngine *ProtocolEngine);

  void SendPing(int TransactionId);

  static void PingHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);
};
//---------------------------------------------------------------------------
extern PACKAGE TMCBSimulatorForm *MCBSimulatorForm;
//---------------------------------------------------------------------------
#endif
