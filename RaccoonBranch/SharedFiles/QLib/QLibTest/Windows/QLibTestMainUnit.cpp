 //---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "QLibTestMainUnit.h"
#include "AllQLib.h"
#include "MiniDebugger.h"
#include "QMonitorDlg.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;


int GlobalA = 0;
int GlobalB = 0;
int GlobalC = 0;


class CMyCondition1 : public CQCondition {
  public:
    bool Condition(void) {
      return (GlobalA > GlobalB);
    }
};

CQConditionMonitor MyMonitor;


class CMyComponent : public CQComponent {
  public:

    DEFINE_VAR_PROPERTY(int,Ran);
    DEFINE_METHOD_1(CMyComponent,int,Func,int);
    DEFINE_METHOD_2(CMyComponent,QString,DoIt,QString,float);

    CMyComponent(const QString& Name) : CQComponent(Name) {
      INIT_VAR_PROPERTY(Ran,15);
      INIT_METHOD(CMyComponent,Func);
      INIT_METHOD(CMyComponent,DoIt);
    }
};

int CMyComponent::Func(int p)
{
  return (p * 2);
}

QString CMyComponent::DoIt(QString s,float f)
{
  ShowMessage(("Str = " + s).c_str());
  return FloatToStr(f * 2).c_str();
}

CMyComponent AObject("AObject"),BObject("BObject");



class CMyThread : public CQThread {
  private:
    void Execute(void) {
      while(!Terminated) {
        Sleep(BeepInterval);

        if(m_Mode) {
          Beep();

          throw 5;
        }
      }
    }

  public:
    bool m_Mode;

    DEFINE_VAR_PROPERTY(int,BeepInterval);
    DEFINE_METHOD_1(CMyThread,int,DoBeeps,bool);

    CMyThread(const QString& Name) : CQThread(false,Name) {
      INIT_METHOD(CMyThread,DoBeeps);
      INIT_VAR_PROPERTY(BeepInterval,1000);
      m_Mode = false;
    }
};

int CMyThread::DoBeeps(bool Mode)
{
  m_Mode = Mode;
  return 0;
}

CMyThread Beeper("Beeper");


class CMsgEaterThread : public CQThread {
  private:
    int i;

    TMyMessageQueue *m_q;

    void Execute(void) {
      for(;;) {
        if((m_q->Receive(i) == wrReleased) || Terminated)
          break;

        QSleep(1000);

        MainForm->SetLabelCaption(IntToStr(i));
      }
    }

  public:
    CMsgEaterThread(const QString& Name) : CQThread(false,Name) {
      i = 0;
    }

    void SetQ(TMyMessageQueue *q) {
      m_q = q;
    }
};


class CAppParams : public CQParamsContainer {
  public:
    DEFINE_PARAM(int,Temperature);
    DEFINE_PARAM(float,ModelHeight);
    DEFINE_PARAM(bool,OffLineMode);
    DEFINE_PARAM(QString,Test);
    DEFINE_ARRAY_PARAM(int,4,MyArray);

    CAppParams(CQParamsStream *ParamsStream,const QString& Name) : CQParamsContainer(ParamsStream,Name) {

      INIT_PARAM(Temperature,85,"Temperature");

      INIT_PARAM_WITH_LIMITS(ModelHeight,10.6f,5.0f,20.0f,"");
      ModelHeight.SetAttributes(paLimitsException);

      INIT_PARAM(OffLineMode,false,"");
      INIT_PARAM(Test,"Ran","");

      INIT_PARAM(MyArray,"2,5,10,8","Test");
    }

    ~CAppParams() {
      SaveAll();
    }
};


CQParamsFileStream CfgFile("Test1.cfg");
CAppParams MyParams(&CfgFile,"ConfigParams");


class CMonitorTestThread : public CQThread {
  public:
     CMonitorTestThread(QString Name) : CQThread(false,Name) {}

  void Execute() {
    for(int i=0; i < 1000; i++)
    {
      QMonitor.Printf("i = %d",i);
      QSleep(10);
    }
  }
};


//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void Action1(TGenericCockie)
{
  QMonitor.Print("Action 1");
}

void Action2(TGenericCockie)
{
  QMonitor.Print("Action 2");
}

void Action3(TGenericCockie)
{
  QMonitor.Print("Action 3");
}

void Action4(TGenericCockie)
{
  QMonitor.Print("Action 4");
}

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  m_MsgQ = new TMyMessageQueue(4);

  m_MyThread = new CMsgEaterThread("MessageEater");

  m_MyThread->SetQ(m_MsgQ);

  m_QApplication = new CQApplication;
  m_QApplication->Init();

  m_BaseMenu = new CQSubMenu("QMonitor test menu");
  m_DebugMenu = new CQSubMenu("Debug menu",'D');
  m_OptionsMenu = new CQSubMenu("Options",'O');

  m_BaseMenu->AddSubMenu(m_DebugMenu);
  m_BaseMenu->AddSeperator();
  m_BaseMenu->AddSubMenu(m_OptionsMenu);
  m_BaseMenu->AddAction(Action1,"Action 1",'1');
  m_DebugMenu->AddAction(Action2,"Action 2",'2');
  m_OptionsMenu->AddAction(Action3,"Action 3",'3');
  m_OptionsMenu->AddSeperator();
  m_OptionsMenu->AddAction(Action4,"Action 4",'4');

  CQMenusHandler::Init(m_BaseMenu);

  m_TcpIpClient = new CQTcpIpClient("10.10.10.186",1024);

  m_TcpIpTestThread = new CTcpIpTestThread("TcpIpTestThread",m_TcpIpClient);

  m_TcpIpServer =  new CQTcpIpServer(1024);
  m_TcpIpServerThread = new CTcpIpServerTestThread("TcpIpServerTestThread",m_TcpIpServer);
  m_TcpIpServerThread->Resume();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
  delete m_QApplication;
  CQMenusHandler::DeInit();

  m_MsgQ->Release();
  delete m_MyThread;
  delete m_MsgQ;

  delete m_TcpIpTestThread;
  delete m_TcpIpClient;

  delete m_TcpIpServer;
  delete m_TcpIpServerThread;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button3Click(TObject *Sender)
{
  MiniDebuggerForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button1Click(TObject *Sender)
{
  m_MsgQ->Send(Button1->Tag);
  Button1->Tag = Button1->Tag + 1;
}
//---------------------------------------------------------------------------

void TMainForm::HandleUpdateLabel(TMessage &Message)
{
  Label1->Caption = m_NewCaption;
}

void __fastcall TMainForm::Button2Click(TObject *Sender)
{
  CMonitorTestThread *t = new CMonitorTestThread("Thread_1");
  t->FreeOnTerminate(true);

  CMonitorTestThread *q = new CMonitorTestThread("Thread_2");
  q->FreeOnTerminate(true);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SListWriteClick(TObject *Sender)
{
  CQStringList SList;

  SList.Add("ran");
  SList.Add("peleg");
  SList.Add("---------");
  SList.Add("QLib");

  SList.SaveToFile("test1.txt");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SListReadClick(TObject *Sender)
{
  CQStringList SList;

  SList.LoadFromFile("test1.txt");

  for(unsigned i=0; i < SList.Count(); i++)
    ShowMessage(SList[i].c_str());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button4Click(TObject *Sender)
{
  time_t t = QGetCurrentTime();

  QString s = QTimeToStr(t);

  Label2->Caption = s.c_str();

  t = QStrToTime(s);
  s = QTimeToStr(t);

  Label3->Caption = s.c_str();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button5Click(TObject *Sender)
{
  for (int i=0; i < 10; i++)
    m_SafeList.push_back(i);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button6Click(TObject *Sender)
{
  m_SafeList.StartProtect();
  for (CQSafeListAdaptor<vector<int> >::iterator i=m_SafeList.begin(); i != m_SafeList.end(); i++)
    QMonitor.Printf(IntToStr(*i).c_str());
  m_SafeList.StopProtect();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button7Click(TObject *Sender)
{
  m_TcpIpClient->Connect();

  if(m_TcpIpClient->IsConnected())
   m_TcpIpTestThread->Resume();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button8Click(TObject *Sender)
{
  m_TcpIpClient->Disconnect();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button9Click(TObject *Sender)
{
  m_TcpIpClient->Write("Hello",6);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Button11Click(TObject *Sender)
{
  m_TcpIpServer->Disconnect();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button12Click(TObject *Sender)
{
  m_TcpIpServer->Write("Server says hello",sizeof("Server says hello"));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button13Click(TObject *Sender)
{
  if (m_TcpIpServer->IsConnected())
    QMonitor.Printf("Status - connceted");
  else
    QMonitor.Printf("Status - disconnceted");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button10Click(TObject *Sender)
{
  MyParams.Temperature.Push();
  MyParams.Temperature = StrToInt(Edit1->Text);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button14Click(TObject *Sender)
{
  MyParams.Temperature.Pop();
  Edit1->Text = IntToStr((int)MyParams.Temperature);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button15Click(TObject *Sender)
{
  CQStringList SList;

//  SList.LoadFromString("This,is,comma,seperated,string");

  SList.LoadFromString("a,b,c");
  SList.SaveToFile("test1.txt");
}
//---------------------------------------------------------------------------

