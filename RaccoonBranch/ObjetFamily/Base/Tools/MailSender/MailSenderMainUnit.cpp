//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <NMsmtp.hpp>
#include <Psock.hpp>


//---------------------------------------------------------------------------

#pragma argsused
int main(int argc, char* argv[])
{
  if(AnsiString(argv[1]) == "")
    return -1;

  if(argc != 3)
    return -1;

  TNMSMTP *NMSMTP1 = new TNMSMTP(NULL);

  NMSMTP1->Host = "MAILSRV";
  NMSMTP1->PostMessage->ToAddress->Add(argv[2]);
  NMSMTP1->PostMessage->FromAddress = "Builder6@objet.co.il";

  char ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
  unsigned long ComputerNameLength = MAX_COMPUTERNAME_LENGTH;
  GetComputerName(ComputerName,&ComputerNameLength);

  NMSMTP1->PostMessage->FromName = AnsiString(ComputerName);

  NMSMTP1->PostMessage->Subject = "Compilation status report";
  NMSMTP1->PostMessage->Body->Add(argv[1]);
  NMSMTP1->Connect();
  NMSMTP1->SendMail();
  NMSMTP1->Disconnect();

  delete NMSMTP1;
  return 0;
}
//---------------------------------------------------------------------------

