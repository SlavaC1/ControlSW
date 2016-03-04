// QLib simple test application
// Date: 12/8/01
// Author: Ran Peleg

#include <stdio.h>
#include "AllQLib.h"


// Define a costum component class
class CTestClass : public CQComponent {
  private:
    int m_a,m_b;

    // Define two properties (set and get functions)
    DEFINE_PROPERTY(CTestClass,int,a);
    DEFINE_PROPERTY(CTestClass,int,b);

    // Define a variable property
    DEFINE_VAR_PROPERTY(float,v);

    int GetA(void) {
      return m_a;
    }
    void SetA(int NewA) {
      m_a = NewA;
      QMonitor.Print("a has been set\n");
    };

    int GetB(void) {
      return m_b;
    }
    void SetB(int NewB) {
      m_b = NewB;
      QMonitor.Print("b has been set\n");
    };


  public:
    // Constructor
    CTestClass(const QString& Name) : CQComponent(Name) {
      INIT_METHOD(CTestClass,SetNumbers);
      INIT_METHOD(CTestClass,AddNumbers);
      INIT_PROPERTY(CTestClass,a,SetA,GetA);
      INIT_PROPERTY(CTestClass,b,SetB,GetB);
      INIT_VAR_PROPERTY(v,0);

      m_a = 10;
      m_b = 20;
    }

    DEFINE_METHOD_2(CTestClass,int,SetNumbers,int,int);
    DEFINE_METHOD(CTestClass,int,AddNumbers);
};

int CTestClass::SetNumbers(int a,int b)
{
  m_a = a;
  m_b = b;
  return 0;
}

int CTestClass::AddNumbers(void)
{
  return (m_a + m_b);
}



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
        QMonitor.Print("Disconnected");
        Suspend();
      }
      else
      if(Status == tcAbort)
      {
        QMonitor.Print("Aborted");
        Suspend();
      }
      else
        QMonitor.Printf("%c",Buffer[0]);
    }
  }
};

class CTcpIpTestServerThread : public CQThread {
  private:
    CQTcpIpServer *m_TcpIpServer;

  public:
     CTcpIpTestServerThread(QString Name,CQTcpIpServer *TcpIpServer) : CQThread(false,Name) {
       m_TcpIpServer = TcpIpServer;
     }

  void Execute(void) {
    char Buffer[10];

    m_TcpIpServer->Connect();
    if ( m_TcpIpServer->IsConnected())
      QMonitor.Printf("Client connected to server");

    while(!Terminated)
    {
     if (m_TcpIpServer->IsConnected())
      {
       TQTcpIpServerStatus Status = m_TcpIpServer->Read(Buffer,1);

       if(Status == tsDisconnect)
       {
         QMonitor.Print("Disconnected");
         
       }
       else
       if(Status == tsAbort)
       {
        QMonitor.Print("Aborted");
       
       }
       else
        QMonitor.Printf("%c",Buffer[0]);
      }
     else
     {
      m_TcpIpServer->Reconnect();

     }
   }
  }
};


CQTcpIpClient *TcpIpClient;
CTcpIpTestThread *TcpIpTestThread; 
CQTcpIpServer *TcpIpServer;
CTcpIpTestServerThread *TcpIpTestServerThread; 


// Menu functions
// ------------------------------------------------------------------


void TcpIpConnect(TGenericCockie)
{
  TcpIpClient->Connect();

  if(TcpIpClient->IsConnected())
    TcpIpTestThread->Resume();  
}

void TcpIpDisconnect(TGenericCockie)
{
  TcpIpClient->Disconnect();
}

void TcpIpSend(TGenericCockie)
{
  TcpIpClient->Write("Hello",6);
}


void TcpIpServerDisconnect(TGenericCockie)
{
  TcpIpServer->Disconnect();
}

void TcpIpServerSend(TGenericCockie)
{
  TcpIpServer->Write("Hello",6);
}

void TcpIpServerStatus(TGenericCockie)
{
  if (TcpIpServer->IsConnected())
    QMonitor.Printf("status - connected");
  else
    QMonitor.Printf("status - disconnected");
}
// Enable monitor messages
void EnableMonitorMessages(TGenericCockie)
{
  QMonitor.Print("Messages enabled",false);
  QMonitor.Mask = false;
}

// Disable monitor messages
void DisableMonitorMessages(TGenericCockie)
{
  QMonitor.Print("Messages disabled",false);
  QMonitor.Mask = true;
}

// List all objects in the program
void ListAllObjects(TGenericCockie)
{
  // Get a pointer to the objects roster (we will need it later)
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  // If the roster is NULL, no objects are registered
  if(Roster == NULL)
    QMonitor.Print("No objects found.",false);
  else
  {
    // Get a pointer to the list of the components
    TQComponentList *CompList = Roster->GetComponentList();

    // Print the name of all the components in the list
    for(TQComponentList::iterator i = CompList->begin(); i != CompList->end(); i++)
      QMonitor.PrintfNonMaskable("%s",(*i)->Name().c_str());
  }
}

// List all the methods in an object
void ListAllMethods(TGenericCockie)
{
  // Get a pointer to the objects roster (we will need it later)
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  // If the roster is NULL, no objects are registered
  if(Roster == NULL)
    QMonitor.Print("No objects found.",false);
  else
  {
    QString ObjName;

    ObjName = QMonitor.GetString("Enter object name: ");

    // Search for the object in the roster
    CQComponent *Component = Roster->FindComponent(ObjName);

    if(Component == NULL)
      QMonitor.Print("Object not found.",false);
    else
    {
      for(int i = 0; i < Component->MethodCount(); i++)
        QMonitor.PrintfNonMaskable("%s   (%d arguments)",Component->Methods[i]->Name().c_str(),
                                   Component->Methods[i]->GetMethodArgumentsCount());
    }
  }
}

// List all the properties in an object
void ListAllProperties(TGenericCockie)
{
  // Get a pointer to the objects roster (we will need it later)
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  // If the roster is NULL, no objects are registered
  if(Roster == NULL)
    QMonitor.Print("No objects found.",false);
  else
  {
    QString ObjName;

    ObjName = QMonitor.GetString("Enter object name: ");

    // Search for the object in the roster
    CQComponent *Component = Roster->FindComponent(ObjName);

    if(Component == NULL)
      QMonitor.Print("Object not found.",false);
    else
      for(int i = 0; i < Component->PropertyCount(); i++)
        QMonitor.PrintfNonMaskable("%s  = %s",Component->Properties[i]->Name().c_str(),
                                   Component->Properties[i]->ValueAsString().c_str());
  }
}

class CTestThread : public CQThread {
  private:
    CQSimpleQueue m_Queue;

  public:
    CTestThread() : m_Queue(3,4) {}

    void Execute(void) {
      int i;

      for(;;)
      {
        m_Queue.Receive(&i,4);
        QMonitor.Printf("%d",i);
      } 
    } 

    void Send(int i)
    {
      m_Queue.Send(&i,4);
    }
};


class CSenderThread : public CQThread {
  private:
    CTestThread *m_Test;

  public:
    CSenderThread(CTestThread *Test) {
      m_Test = Test;
    }

    void Execute(void) {
      for(int i = 0 ;; i++)
      {
        m_Test->Send(i);
        QSleepTicks(60);
      } 
    } 
};


int main(void)
{
  TcpIpClient = new CQTcpIpClient("10.10.10.186",1024);
  TcpIpTestThread = new CTcpIpTestThread("TcpIpTestThread",TcpIpClient);
  

  TcpIpServer = new CQTcpIpServer(1024);
  TcpIpTestServerThread = new CTcpIpTestServerThread("TcpIpTestServerThread",TcpIpServer);

/*
  CQStdComPort c(2);

  c.Write("Ran Peleg",10);

  char Buffer[20];
  int l = c.Read(Buffer,10);

  for(int i=0; i < l; i++)
    QMonitor.Printf("%c",Buffer[i]);
*/

/*
  CTestThread c1;
  CSenderThread s1(&c1);
*/

  // Create two dummy instances of CTestClass
  CTestClass Obj1("Object1"),Obj2("Object2");

  // Define the submenu objects
  CQSubMenu RootMenu("QLib test application"),OptionsMenu("run-time Objects menu",'O'),TestMenu("Test menu",'T');
  CQSubMenu TcpIpMenu("TCP/IP test options",'P');

  // Build the menus structure
  RootMenu.AddAction(EnableMonitorMessages,"Enable monitor messages",'E');
  RootMenu.AddAction(DisableMonitorMessages,"Disable monitor messages",'D');
  RootMenu.AddSeperator();

  RootMenu.AddSubMenu(&OptionsMenu);
  RootMenu.AddSubMenu(&TestMenu);
  RootMenu.AddSeperator();
  RootMenu.AddSubMenu(&TcpIpMenu);

  OptionsMenu.AddAction(ListAllObjects,"List all objects",'L');
  OptionsMenu.AddAction(ListAllMethods,"List all the methods of an object",'M');
  OptionsMenu.AddAction(ListAllProperties,"List all the properties of an object",'P');

  TcpIpMenu.AddAction(TcpIpConnect,"Connect",'C');
  TcpIpMenu.AddAction(TcpIpDisconnect,"Disconnect",'D');
  TcpIpMenu.AddAction(TcpIpSend,"Send",'S');
  TcpIpMenu.AddSeperator();
  TcpIpMenu.AddAction(TcpIpServerDisconnect,"Server Disconnect",'I');
  TcpIpMenu.AddAction(TcpIpServerSend,"Server Send",'E');
  TcpIpMenu.AddAction(TcpIpServerStatus,"Server Status",'T');

  // Init the menus handler
  CQMenusHandler::Init(&RootMenu);

  // Block until the menus handler thread will exit
  CQMenusHandler::WaitForCompletion();

  CQMenusHandler::DeInit();
  QMonitor.Print("Program done",false);
}

