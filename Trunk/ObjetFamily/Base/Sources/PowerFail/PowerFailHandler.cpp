/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module:  Power Fail Handler Thread                               *
 * Module Description: Responsible for receiving a PowerFail        *
 *                     Notification and manage the ShutDown         *
 *                     sequence.                                    *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Shahar Behagen                                           *
 * Start date: 23/12/2008                                           *
 * Last upate: 23/12/2008                                           *
 ********************************************************************/

#include "PowerFailHandler.h"
#include "Q2RTApplication.h"
#include "MachineManager.h"
#include "AppLogFile.h"

#define BUFSIZE 4096

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

// Note: The CPowerFail code is based on the "Multithreaded Pipe Server example" taken from: http://msdn.microsoft.com/en-us/library/aa365588(VS.85).aspx
CPowerFail::CPowerFail(void)
  : CQThread(true,"PowerFail")
{
   m_lpszPipename          = TEXT("\\\\.\\pipe\\powerfailmessagepipe"); // see: ...\Tools\PowerFailHandler\ApplicationRelated.rc for a corresponding Client pipename-string declaration.
   m_lpszRequestMessage    = TEXT("DO_POWERFAIL_PROCEDURE");            // see: ...\Tools\PowerFailHandler\ApplicationRelated.rc for a corresponding Client request-string declaration.
   m_lpszResponseMessage   = TEXT("DOING_POWERFAIL_PROCEDURE");         // see: ...\Tools\PowerFailHandler\ApplicationRelated.rc for a corresponding Client response-string declaration.
   m_lpszUnexpectedMessage = TEXT("RECEIVED_UNEXPECTED_MESSAGE");

// Create an instance of the named pipe and
// then wait for the client to connect to it. When the client
// connects, a thread is created to handle communications
// with that client. 

   m_hPipe = CreateNamedPipe(
       m_lpszPipename,           // pipe name
       PIPE_ACCESS_DUPLEX,       // read/write access
       PIPE_TYPE_MESSAGE |       // message type pipe
       PIPE_READMODE_MESSAGE |   // message-read mode
       PIPE_WAIT,                // blocking mode
       PIPE_UNLIMITED_INSTANCES, // max. instances
       BUFSIZE,                  // output buffer size
       BUFSIZE,                  // input buffer size
       0,                        // client time-out
       NULL);                    // default security attribute

   if (m_hPipe == INVALID_HANDLE_VALUE)
   {
       throw EQException("CreatePipe failed");
   }
}

// Thread execute function (override)
void CPowerFail::Execute(void)
{
   TCHAR chRequest[BUFSIZE];
   TCHAR chReply[BUFSIZE];
   DWORD cbBytesRead, cbReplyBytes, cbWritten;
   BOOL  fSuccess;
   BOOL  fConnected;

   try {
   while(!Terminated)
   {
   // Wait for the client to connect; if it succeeds,
   // the function returns a nonzero value. If the function
   // returns zero, GetLastError() returns ERROR_PIPE_CONNECTED.

      fConnected = ConnectNamedPipe(m_hPipe, NULL) ?
          TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

      if (QLib::wrSignaled == m_StopWaitingForConnectionEvent.WaitFor(0))
      {
          CloseHandle(m_hPipe);
          break;
      }

      if (!fConnected)
      {
      // The client could not connect, so close the pipe.
         throw EQException("Client could not connect.");
      }

   // Read client requests from the pipe.
      fSuccess = ReadFile(
         m_hPipe,      // handle to pipe
         chRequest,    // buffer to receive data
         BUFSIZE*sizeof(TCHAR), // size of buffer
         &cbBytesRead, // number of bytes read
         NULL);        // not overlapped I/O

      if (! fSuccess || cbBytesRead == 0)
         throw EQException("Failed to read message.");
      GetAnswerToRequest(chRequest, chReply, &cbReplyBytes);

    	CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
      Mgr->PowerFailShutdown();

   // Write the reply to the pipe.
      fSuccess = WriteFile(
         m_hPipe,      // handle to pipe
         chReply,      // buffer to write from
         cbReplyBytes, // number of bytes to write
         &cbWritten,   // number of bytes written
         NULL);        // not overlapped I/O

      if (! fSuccess || cbReplyBytes != cbWritten)
         throw EQException("Failed to write message.");

  // Flush the pipe to allow the client to read the pipe's contents
  // before disconnecting. Then disconnect the pipe, and allow another client to connect.

      FlushFileBuffers(m_hPipe);
      DisconnectNamedPipe(m_hPipe);
  } // while(!Terminated)
  } catch(...) {
    CQLog::Write(LOG_TAG_GENERAL, "CPowerFail::Execute - unexpected error");
    if (!Terminated)
      throw EQException("CPowerFail::Execute - unexpected error");
  }
}

void CPowerFail::GetAnswerToRequest(LPTSTR chRequest, LPTSTR chReply, LPDWORD pchBytes)
{
  if (0 == strcmp(m_lpszRequestMessage, chRequest))
    lstrcpy (chReply, m_lpszResponseMessage);
  else
    lstrcpy (chReply, m_lpszUnexpectedMessage);

  *pchBytes = (lstrlen(chReply)+1)*sizeof(TCHAR);
}

CPowerFail::~CPowerFail()
{}

void CPowerFail::StopWaitingForConnection()
{
  HANDLE hPipe;

  m_StopWaitingForConnectionEvent.SetEvent();

// Connect as a client to force ConnectNamedPipe() to return
  hPipe = CreateFile(
     m_lpszPipename,   // pipe name
     GENERIC_READ |    // read and write access
     GENERIC_WRITE,
     0,                // no sharing
     NULL,             // default security attributes
     OPEN_EXISTING,    // opens existing pipe
     0,                // default attributes
     NULL);            // no template file

  if (hPipe == INVALID_HANDLE_VALUE)
    throw EQException("Failed to connect to pipe");
}

void CPowerFail::Shutdown()
{
  FreeOnTerminate(true);      // Causes the thread to self destruct *after* execute function returns. (Don't leave the D'tor before exiting the thread executing function)
  Terminate();                // Just sets the Trerminate flag.
  StopWaitingForConnection(); // Closes the waiting object from its wait.
}
