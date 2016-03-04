//---------------------------------------------------------------------------

#ifndef OCBSimulatorDlgH
#define OCBSimulatorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>

#include "EdenProtocolEngine.h"
#include "QSimpleQueue.h"
#include "OCBSimulator.h"
#include "OCBSimProtocolClient.h"
#include "QSafeList.h"
#include "NotificationsThread.h"
#include <vector>

#define WM_OCB_SIM_MESSAGE_IN_Q  WM_USER+1
#define WM_OCB_SIM_MSG_LOG       WM_USER+3

const int MAX_MSG_LENGTH       = 1024;
const int MAX_NUM_0F_MESSAGES  = 100;
const int MAX_NUM_OF_OCB_MSGS  = 40;
const int MIN_OCB_MSG_NUMBER  = 100;
const int MAX_OCB_MSG_NUMBER  = 140;
const int OFF                  = 0;
const int ON                   = 1;
const int MODEL                = 0;
const int SUPPORT              = 1;
const int RECEIVED             = 0;
const int TRANSMITED           = 1;
const AnsiString DEFAULT_REPLY = "";

typedef std::vector<BYTE> TNotificationMessage;
typedef std::vector<TNotificationMessage> TNotificationMessageList;

struct TNotificationMessageInfo{
  TComboBox* ComboBox;
  TUpDown* UpDown;
  TNotificationMessageList MessageList;
  };


//---------------------------------------------------------------------------
class TOCBSimulatorForm : public TForm
{
__published:	// IDE-managed Components
  TMemo *MessageLogMemo;
  TTabSheet *TabSheet1;
  TTabSheet *TabSheet2;
  TTabSheet *TabSheet3;
  TBitBtn *ResetLogBitBtn;
  TEdit *Edit5;
  TEdit *Edit6;
  TEdit *Edit7;
  TEdit *Edit11;
  TEdit *Edit12;
  TEdit *Edit13;
  TEdit *Edit14;
  TEdit *Edit21;
  TEdit *Edit22;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TLabel *Label4;
  TLabel *Label5;
  TLabel *Label6;
  TBitBtn *DefaultsBitBtn;
  TBitBtn *BitBtn1;
  TBitBtn *BitBtn2;
  TCheckBox *CheckBox5;
  TCheckBox *CheckBox6;
  TCheckBox *CheckBox7;
  TCheckBox *CheckBox11;
  TCheckBox *CheckBox12;
  TCheckBox *CheckBox13;
  TCheckBox *CheckBox14;
  TCheckBox *CheckBox21;
  TCheckBox *CheckBox22;
  TLabel *Label11;
  TLabel *Label12;
  TLabel *Label13;
  TLabel *Label17;
  TLabel *Label18;
  TLabel *Label19;
  TLabel *Label20;
  TLabel *Label27;
  TLabel *Label28;
  TBitBtn *BitBtn3;
  TTabSheet *TabSheet4;
  TCheckBox *CheckBox1;
  TCheckBox *CheckBox31;
  TCheckBox *CheckBox32;
  TCheckBox *CheckBox33;
  TComboBox *ComboBox5;
  TComboBox *ComboBox6;
  TComboBox *ComboBox7;
  TComboBox *ComboBox11;
  TComboBox *ComboBox12;
  TComboBox *ComboBox13;
  TComboBox *ComboBox14;
  TComboBox *ComboBox21;
  TComboBox *ComboBox22;
  TComboBox *ComboBox31;
  TComboBox *ComboBox32;
  TComboBox *ComboBox33;
  TComboBox *ComboBox34;
  TLabel *Label37;
  TLabel *Label38;
  TEdit *Edit31;
  TEdit *Edit32;
  TEdit *Edit33;
  TEdit *Edit34;
  TLabel *Label39;
  TLabel *Label40;
  TLabel *Label41;
  TLabel *Label42;
  TBitBtn *BitBtn4;
  TLabel *Label43;
  TEdit *Edit39;
  TComboBox *ComboBox35;
  TCheckBox *CheckBox35;
  TCheckBox *CheckBox36;
  TComboBox *ComboBox36;
  TEdit *Edit40;
  TLabel *Label44;
  TLabel *Label45;
  TEdit *Edit41;
  TComboBox *ComboBox37;
  TCheckBox *CheckBox37;
  TLabel *Label46;
  TEdit *Edit42;
  TComboBox *ComboBox38;
  TCheckBox *CheckBox38;
  TLabel *Label50;
  TTabSheet *TabSheet5;
  TLabel *Label51;
  TLabel *Label52;
  TLabel *Label54;
  TEdit *Edit43;
  TComboBox *ComboBox40;
  TCheckBox *CheckBox40;
  TLabel *Label55;
  TEdit *Edit44;
  TComboBox *ComboBox41;
  TCheckBox *CheckBox41;
  TLabel *Label56;
  TEdit *Edit45;
  TComboBox *ComboBox42;
  TCheckBox *CheckBox42;
  TBitBtn *BitBtn5;
  TLabel *Label14;
  TLabel *Label21;
  TComboBox *ComboBox15;
  TEdit *Edit15;
  TUpDown *UpDown2;
  TLabel *Label22;
  TLabel *Label23;
  TEdit *Edit16;
  TUpDown *UpDown3;
  TComboBox *ComboBox16;
  TLabel *Label24;
  TLabel *Label25;
  TLabel *Label26;
  TLabel *Label29;
  TLabel *Label30;
  TLabel *Label31;
  TEdit *Edit17;
  TUpDown *UpDown4;
  TComboBox *ComboBox17;
  TTabSheet *TabSheet6;
  TLabel *Label9;
  TEdit *Edit3;
  TComboBox *ComboBox3;
  TCheckBox *CheckBox3;
  TEdit *Edit20;
  TComboBox *ComboBox20;
  TLabel *Label10;
  TEdit *Edit4;
  TComboBox *ComboBox4;
  TCheckBox *CheckBox4;
  TUpDown *UpDown7;
  TLabel *Label32;
  TLabel *Label33;
  TLabel *Label34;
  TLabel *Label35;
  TBitBtn *BitBtn6;
  TTabSheet *TabSheet7;
  TLabel *Label36;
  TLabel *Label59;
  TLabel *Label60;
  TLabel *Label65;
  TBitBtn *BitBtn7;
  TLabel *Label7;
  TEdit *Edit1;
  TComboBox *ComboBox1;
  TCheckBox *CheckBox34;
  TEdit *Edit8;
  TComboBox *ComboBox8;
  TLabel *Label53;
  TEdit *Edit38;
  TComboBox *ComboBox39;
  TCheckBox *CheckBox39;
  TLabel *Label8;
  TEdit *Edit2;
  TComboBox *ComboBox2;
  TCheckBox *CheckBox2;
  TTabSheet *TabSheet8;
  TLabel *Label62;
  TEdit *Edit51;
  TComboBox *ComboBox51;
  TCheckBox *CheckBox51;
  TEdit *Edit18;
  TComboBox *ComboBox18;
  TLabel *Label63;
  TEdit *Edit52;
  TComboBox *ComboBox52;
  TCheckBox *CheckBox52;
  TEdit *Edit19;
  TComboBox *ComboBox19;
  TLabel *Label64;
  TEdit *Edit53;
  TComboBox *ComboBox53;
  TCheckBox *CheckBox53;
  TLabel *Label66;
  TLabel *Label67;
  TLabel *Label68;
  TLabel *Label69;
  TBitBtn *BitBtn8;
  TUpDown *UpDown5;
  TUpDown *UpDown6;
  TUpDown *UpDown1;
  TTabSheet *TabSheet9;
  TPageControl *PageControl1;
  TLabel *Label15;
  TEdit *Edit9;
  TComboBox *ComboBox9;
  TCheckBox *CheckBox9;
  TLabel *Label16;
  TEdit *Edit10;
  TComboBox *ComboBox10;
  TCheckBox *CheckBox10;
  TLabel *Label47;
  TEdit *Edit35;
  TComboBox *ComboBox47;
  TCheckBox *CheckBox47;
  TLabel *Label70;
  TLabel *Label71;
  TLabel *Label72;
  TLabel *Label73;
  TBitBtn *BitBtn9;
  TTabSheet *TabSheet10;
  TLabel *Label74;
  TLabel *Label75;
  TLabel *Label76;
  TLabel *Label77;
  TBitBtn *BitBtn10;
  TLabel *Label48;
  TEdit *Edit36;
  TComboBox *ComboBox48;
  TCheckBox *CheckBox48;
  TCheckBox *CheckBox49;
  TComboBox *ComboBox49;
  TEdit *Edit37;
  TLabel *Label49;
  TLabel *Label61;
  TEdit *Edit50;
  TComboBox *ComboBox50;
  TCheckBox *CheckBox50;
  TLabel *Label57;
  TEdit *Edit46;
  TComboBox *ComboBox43;
  TCheckBox *CheckBox43;
  TLabel *Label58;
  TEdit *Edit47;
  TComboBox *ComboBox44;
  TCheckBox *CheckBox44;
  TLabel *Label78;
  TEdit *Edit23;
  TComboBox *ComboBox23;
  TCheckBox *CheckBox8;
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TButton *Button4;
  TButton *Button5;
  TButton *Button6;
  TButton *Button7;
  TLabel *Label79;
  TEdit *Edit24;
  TComboBox *ComboBox24;
  TCheckBox *CheckBox15;
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);

  // sets the combo boxes and check boxes on a tab sheet to their default value
  void __fastcall DefaultsBitBtnClick(TObject *Sender);

  // clear the message log
  void __fastcall ResetLogBitBtnClick(TObject *Sender);
  void __fastcall SendNotificationButtonClick(TObject *Sender);

private:	// User declarations

   // type for the components used
   typedef enum {COMBO_BOX, CHECK_BOX, EDIT_BOX, UP_DOWN} TComponentsType;

   // type for the components sub tag
   typedef enum {RECIEVED_SUB_TAG = 0, TRANSMIT_SUB_TAG = 0, CHANGE_SUB_TAG = 0, NOTIFICATION_TIME_SUB_TAG = 0, NOTIFICATION_SUB_TAG = 1} TSubTagType;

   // an array of the notification message info struct that include the components and the messages list
   TNotificationMessageInfo m_NotificationMessageInfo[MAX_NUM_OF_OCB_MSGS];

   // Initialization for the array of NotificationMessageInfo from the ini file
   void InitializeNotificationMessageInfo();

   // convert a message string to a vector of bytes representing the message
  TNotificationMessage ConvertMessageToVector(QString MessageString);


   // the protocol engine
   CEdenProtocolEngine *m_ProtocolEngine;

   // the protocol client
   COCBSimProtocolClient *m_ProtocolClient;

   // the OCB simulator instance
   COCBSimulator *m_OCBSimulator;


  // simple message queue for the recieved messages
  CQSimpleQueue *m_MessageQueue;

  // message queue for thread safe displaying the messages in the message log
  CQSimpleQueue *m_MsgLogQueue;

  // list of notfifcation to be sent
  TNotificationsList m_NotificationList;

  // the thread the send the notification on their due time
  CNotificationsThread *m_NotificationsThread;

  // send the notification the user selected at the selected time
  void SendNotification(int MessageId);
  
   // decode the message and display it in the right edit box
  void DecodeAndDisplayMessage(PVOID Data);

  // reply to the message with the default reply for this message or
  // with the reply the user choosed
  void ReplyToMessage(PVOID Data, int TransactionID);

  // send a reply according to the message id of the recieved message, and the reply the user choosed
  void SendReplyMessage(BYTE MessageId,PVOID Data, AnsiString Reply, int TransactionID);



  // find a componet by its type, tag and subtag
  TComponent* FindComponent(TComponentsType Type, int Tag, TSubTagType SubTag);

public:		// User declarations
  __fastcall TOCBSimulatorForm(TComponent* Owner);
  void Open(CEdenProtocolEngine *ProtocolEngine);

  static void AllMessageCallBack(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie InstancePtr);
 
  // insert a new message in the message queue and post a message to the dialog to inform about the new nessage
  void HandleMessage(PVOID Data, unsigned DataLength, int TransactionID);

  // Display the received/transmited messages in the message log
  void DisplayMessageInMessageLog(PVOID Message, unsigned Length, int Direction);

  // Handle the new message, and update the relevant fields in the dialog
  MESSAGE void DoHandleMessage(TMessage& Message);

  // display a message in the message log
  MESSAGE void DoDisplayMessageInMessageLog(TMessage& Message);

  BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_OCB_SIM_MESSAGE_IN_Q,TMessage ,DoHandleMessage);
  MESSAGE_HANDLER(WM_OCB_SIM_MSG_LOG,TMessage ,DoDisplayMessageInMessageLog);
  END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TOCBSimulatorForm *OCBSimulatorForm;
//---------------------------------------------------------------------------
#endif
