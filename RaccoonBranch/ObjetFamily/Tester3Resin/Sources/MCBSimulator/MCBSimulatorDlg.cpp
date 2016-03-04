//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MCBSimulatorDlg.h"
#include "OCBCommDefs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMCBSimulatorForm *MCBSimulatorForm;
//---------------------------------------------------------------------------
__fastcall TMCBSimulatorForm::TMCBSimulatorForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void TMCBSimulatorForm::Open(CEdenProtocolEngine *ProtocolEngine)
{
  if (m_ProtocolClient == NULL)
  {
    m_ProtocolEngine = ProtocolEngine;
    m_ProtocolClient = new CEdenProtocolClient(m_ProtocolEngine,2,1);
    m_ProtocolClient->InstallReceiveHandler(0, 255, PingHandler, reinterpret_cast<TGenericCockie>(this));
  }

  Show();
}
//---------------------------------------------------------------------------


void TMCBSimulatorForm::PingHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{

  TOCBPingMessage *Message = static_cast<TOCBPingMessage *>(Data);
  TMCBSimulatorForm *InstancePtr = reinterpret_cast<TMCBSimulatorForm *>(Cockie);

  if (Message->MessageID == OCB_PING)
     InstancePtr->SendPing(TransactionId);

}

//---------------------------------------------------------------------------

void TMCBSimulatorForm::SendPing(int TransactionId)
{
  // Build the ping message
  TOCBPingMessage PingMessage;
  PingMessage.MessageID = OCB_PING;

  // Send a ping reply
  m_ProtocolClient->Send(&PingMessage,sizeof(TOCBPingMessage),TransactionId);

}
//---------------------------------------------------------------------------

void __fastcall TMCBSimulatorForm::FormDestroy(TObject *Sender)
{
  if (m_ProtocolClient)
    Q_SAFE_DELETE(m_ProtocolClient);

}
//---------------------------------------------------------------------------

