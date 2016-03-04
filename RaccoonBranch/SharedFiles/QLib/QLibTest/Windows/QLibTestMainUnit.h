//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "QMessageQueue.h"
#include "QMenu.h"
#include <Menus.hpp>

#include "QApplication.h"
#include "QSafeList.h"
#include "QTcpIpClient.h"
#include "QTcpIpServer.h"
#include "QThread.h"
#include "QParameter.h"

#define WM_UPDATE_LABEL WM_USER


class CMsgEaterThread;

typedef CQMessageQueue<int> TMyMessageQueue;


class CTcpIpTestThread : public CQThread {
  private:
    CQTcpIpClient *m_TcpIpClient;

  public:
     CTcpIpTestThread(QString Name,CQTcpIpClient *TcpIpClient) : CQThread(true,Name) {
       m_TcpIpClient = TcpIpClient;
     }

  void Execute(void) {
    char Buffer[10];

    while(!Terminated)
    {
      TQTcpIpClientStatus Status = m_TcpIpClient->Read(Buffer,1);

      if(Status == tcDisconnect)
      {
        QMonitor.Print("Client Disconnected");
        Suspend();
      }
      else
      if(Status == tcAbort)
      {
        QMonitor.Print("Client Aborted");
        Suspend();
      }
      else
        QMonitor.Printf("%c",Buffer[0]);
    }
  }
};

class CTcpIpServerTestThread : public CQThread {
  private:
    CQTcpIpServer *m_TcpIpServer;

  public:
     CTcpIpServerTestThread(QString Name,CQTcpIpServer *TcpIpServer) : CQThread(true,Name) {
       m_TcpIpServer = TcpIpServer;
     }

  void Execute(void) {
    char Buffer[10];

    if (m_TcpIpServer->Connect())
     QMonitor.Printf("Client connected to server");

    while(!Terminated)
    {
      if (m_TcpIpServer->IsConnected())
      {
        TQTcpIpServerStatus Status = m_TcpIpServer->Read(Buffer,1);

        if(Status == tcDisconnect)
        {
          QMonitor.Print("Server Disconnected");

        }
        else
        if(Status == tcAbort)
        {
          QMonitor.Print("Serevr Aborted");

        }
        else
          QMonitor.Printf("%c",Buffer[0]);
      }
      else
        m_TcpIpServer->Connect();
    }
  }
};


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TButton *Button3;
        TLabel *Label1;
        TButton *Button1;
        TButton *Button2;
        TMainMenu *s;
        TMenuItem *MonitorBase;
        TButton *SListWrite;
        TButton *SListRead;
        TButton *Button4;
        TLabel *Label2;
        TLabel *Label3;
  TButton *Button5;
  TButton *Button6;
        TGroupBox *GroupBox1;
        TButton *Button7;
        TButton *Button8;
        TButton *Button9;
  TGroupBox *GroupBox2;
  TButton *Button11;
  TButton *Button12;
  TButton *Button13;
  TEdit *Edit1;
  TButton *Button10;
  TButton *Button14;
        TButton *Button15;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall SListWriteClick(TObject *Sender);
        void __fastcall SListReadClick(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
  void __fastcall Button5Click(TObject *Sender);
  void __fastcall Button6Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
        void __fastcall Button8Click(TObject *Sender);
        void __fastcall Button9Click(TObject *Sender);
  void __fastcall Button11Click(TObject *Sender);
  void __fastcall Button12Click(TObject *Sender);
  void __fastcall Button13Click(TObject *Sender);
  void __fastcall Button10Click(TObject *Sender);
  void __fastcall Button14Click(TObject *Sender);
        void __fastcall Button15Click(TObject *Sender);
private:	// User declarations

  TMyMessageQueue *m_MsgQ;

  AnsiString m_NewCaption;

  CMsgEaterThread *m_MyThread;

  CQSubMenu *m_BaseMenu;
  CQSubMenu *m_DebugMenu;
  CQSubMenu *m_OptionsMenu;

  CQApplication *m_QApplication;

  MESSAGE void HandleUpdateLabel(TMessage &Message);

  CQSafeListAdaptor<vector<int> > m_SafeList;

  CQTcpIpClient *m_TcpIpClient;
  CTcpIpTestThread *m_TcpIpTestThread;

  CQTcpIpServer *m_TcpIpServer;
  CTcpIpServerTestThread *m_TcpIpServerThread;

public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);

  void SetLabelCaption(const AnsiString& NewCaption) {
    m_NewCaption = NewCaption;
    PostMessage(Handle,WM_UPDATE_LABEL,0,0);
  }

BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_UPDATE_LABEL,TMessage,HandleUpdateLabel);
END_MESSAGE_MAP(TForm);

};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
