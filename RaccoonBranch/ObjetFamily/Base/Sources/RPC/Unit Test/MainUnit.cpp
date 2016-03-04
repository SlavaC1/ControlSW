//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "QTcpIpClientComPort.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


class CTestObject : public CQComponent {
  private:
    DEFINE_METHOD_1(CTestObject,int,ShowString,QString);

  public:
    CTestObject() : CQComponent("TestObject") {
      INIT_METHOD(CTestObject,ShowString);
    }

};

int CTestObject::ShowString(QString Str)
{
  QMonitor.NotificationMessage(Str);
  return 0;
}


TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button2Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  m_TesterApplication = new CQApplication;
  m_TesterApplication->Init();

  AnsiString s = InputBox("Tester RPC","Enter com port number (0 for TCP/IP com port)","1");

  if(s == "")
    s = "1";

  CQBaseComPort *ComPort;

  if(s != "0")
    // Create serial com port
    ComPort = new CQStdComPort(StrToInt(s));
  else
  {
    AnsiString s = InputBox("Tester RPC","Enter TCP/IP address","");

    if(s == "")
      return;

    // Create TCP/IP com port
    ComPort = new CQTcpIpClientComPort(s.c_str());
  }

  m_TesterProtocolEngine = new CEdenProtocolEngine(ComPort);
  m_RPCClient = new CRPCClient(m_TesterProtocolEngine);
  m_RPCServer = new CRPCServer(m_TesterProtocolEngine);

  // For testing
  m_TestObject = new CTestObject;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  delete m_TestObject;

  delete m_RPCClient;
  delete m_RPCServer;
  delete m_TesterProtocolEngine;

  delete m_TesterApplication;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
  m_TesterApplication->Start();
  m_TesterProtocolEngine->Start();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  m_TesterProtocolEngine->Stop();
  m_TesterApplication->Stop();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExecButtonClick(TObject *Sender)
{
  QString Result;

  try
  {
    switch(OpKindRadioGroup->ItemIndex)
    {
      case 0:
        Result = m_RPCClient->GetProperty(ObjectNameEdit->Text.c_str(),RPCItemEdit->Text.c_str(),
                                          TimeoutUpDown->Position * 1000);
        break;

      case 1:
        Result = m_RPCClient->SetProperty(ObjectNameEdit->Text.c_str(),RPCItemEdit->Text.c_str(),ValueEdit->Text.c_str(),
                                          TimeoutUpDown->Position * 1000);
        break;

      case 2:
        {
          QString Args[MAX_QMETHOD_ARGUMENTS];

          int ArgsNum = ExtractMethodArguments(ValueEdit->Text.c_str(),Args);

          Result = m_RPCClient->InvokeMethod(ObjectNameEdit->Text.c_str(),RPCItemEdit->Text.c_str(),
                                             Args,ArgsNum,TimeoutUpDown->Position * 1000);
        }
        break;
    }

    ResultEdit->Text = Result.c_str();

  } catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }
}
//---------------------------------------------------------------------------

int TMainForm::ExtractMethodArguments(QString ArgsLine,QString *Args)
{
  // If empty line, arguments mumber = 0 (this can be a problem with a function required
  // 1 empty argument)
  if(ArgsLine == "")
    return 0;

  int Prev = 0;
  int Next = 0;

  int i;

  for(i = 0; ;i++)
  {
    Next = ArgsLine.find_first_of(",",Prev);

    if(Next != -1)
      Args[i] = ArgsLine.substr(Prev,Next-Prev);
    else
    {
      Args[i] = ArgsLine.substr(Prev,ArgsLine.length() - Prev);
      break;
    }

    Prev = Next + 1;
  }

  return i + 1;
}


