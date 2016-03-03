//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <iostream.h>
#include "Resources.h"
//---------------------------------------------------------------------------

#define BUFSIZE 512

#pragma argsused
int main()
{
   std::string ApplicationName    = LOAD_STRING(IDS_APPLICATION_NAME);
   std::string ApplicationVersion = LOAD_STRING(IDS_APPLICATION_VERSION);
   std::string RequestMessage     = LOAD_STRING(IDS_REQUEST_MESSAGE);
   std::string ResponseMessage    = LOAD_STRING(IDS_RESPONSE_MESSAGE);
   std::string PipeName           = LOAD_STRING(IDS_PIPE_NAME);

   HANDLE hPipe;
   TCHAR  chBuf[BUFSIZE];
   BOOL   fSuccess;
   DWORD  cbRead, cbWritten, dwMode;

// Try to open a named pipe; wait for it, if necessary.

   while (1) 
   { 
      hPipe = CreateFile( 
         PipeName.c_str(), // pipe name
         GENERIC_READ |    // read and write access
         GENERIC_WRITE,
         0,                // no sharing
         NULL,             // default security attributes
         OPEN_EXISTING,    // opens existing pipe
         0,                // default attributes
         NULL);            // no template file
 
   // Break if the pipe handle is valid. 
 
      if (hPipe != INVALID_HANDLE_VALUE) 
         break; 
 
      // Exit if an error other than ERROR_PIPE_BUSY occurs.
 
      if (GetLastError() != ERROR_PIPE_BUSY)
      {
        cout << "Error. This PowerFailHandler utility cannot find a running " << ApplicationName.c_str() << " application," << endl
           << "and will be closed." << endl;
        Sleep(3000);

        return 0;
      }
 
      // All pipe instances are busy, so wait for 20 seconds. 
 
      if (!WaitNamedPipe(PipeName.c_str(), 20000))
      { 
        cout << "Error. Could not open pipe." << endl;
        Sleep(3000);

        return 0;
      }
   } 
 
// The pipe connected; change to message-read mode. 
 
   dwMode = PIPE_READMODE_MESSAGE; 
   fSuccess = SetNamedPipeHandleState( 
      hPipe,    // pipe handle 
      &dwMode,  // new pipe mode
      NULL,     // don't set maximum bytes 
      NULL);    // don't set maximum time 
   if (!fSuccess) 
   {
      cout << "Error. SetNamedPipeHandleState failed" << endl;
      Sleep(3000);

      return 0;
   }

// Send a message to the pipe server.

   fSuccess = WriteFile( 
      hPipe,                  // pipe handle 
      RequestMessage.c_str(), // message 
      RequestMessage.size(),  // message length
      &cbWritten,             // bytes written 
      NULL);                  // not overlapped 
   if (!fSuccess) 
   {
      cout << "Error. WriteFile failed" << endl;
      Sleep(3000);

      return 0;
   }
 
   do 
   { 
   // Read from the pipe. 
 
      fSuccess = ReadFile( 
         hPipe,    // pipe handle 
         chBuf,    // buffer to receive reply
         BUFSIZE*sizeof(TCHAR),  // size of buffer 
         &cbRead,  // number of bytes read 
         NULL);    // not overlapped 
 
      if (! fSuccess && GetLastError() != ERROR_MORE_DATA) 
         break; 
 
//      _tprintf( TEXT("%s\n"), chBuf ); 
   } while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

   CloseHandle(hPipe);

   if (ResponseMessage.compare(chBuf) == 0)
   {
      cout << "Success. " << ApplicationName.c_str() << " application successfully received a PowerFail message." << endl
           << "exiting."  << endl;
      Sleep(3000);

      return 1;
   } else
   {
      cout << ApplicationName.c_str() << " application did not respond as expected to a PowerFail message." << endl
           << "exiting."  << endl;
      Sleep(3000);

      return 0;
   }
}


