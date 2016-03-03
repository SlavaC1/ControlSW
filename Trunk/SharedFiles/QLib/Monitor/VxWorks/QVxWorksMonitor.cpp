/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Monitor)                               *
 * Module Description: VxWorks implementation for the CQMonitor     *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 20/09/2001                                           *
 ********************************************************************/

#include <stdarg.h>              
#include <stdio.h>
#include "QMonitor.h"
#include "QUtils.h"
#include "QErrors.h"


// The monitor implemented as a global object
CQMonitor QMonitor;

const int MAX_PRINT_BUFFER = 1024;


// Constructor
CQMonitor::CQMonitor(void) : CQComponent("Monitor")
{
  // Initialize properties and methods
  INIT_METHOD(CQMonitor,ErrorMessage);
  INIT_METHOD(CQMonitor,WarningMessage);
  INIT_METHOD(CQMonitor,NotificationMessage);
  INIT_PROPERTY(CQMonitor,Mask,SetMask,GetMask);
  INIT_PROPERTY(CQMonitor,StrOut,SetStrOut,GetStrOut);

  m_Mask = false;
}

// Simple output to the monitor (non-formatted)
void CQMonitor::Print(const QString& Str,bool Maskable)
{
  // If the print request is non-maskable, always print
  if(!Maskable)
    printf("%s\n",Str.c_str());
  else
    if(!m_Mask)
      printf("%s\n",Str.c_str());
}

// Simple output to the monitor (non-formatted)
void CQMonitor::Print(const char *Str,bool Maskable)
{
  // If the print request is non-maskable, always print
  if(!Maskable)
    printf("%s\n",Str);
  else
    if(!m_Mask)
      printf("%s\n",Str);
}

// Formatted output to the monitor
void CQMonitor::Printf(const char *FormatStr,...)
{
  char Buffer[MAX_PRINT_BUFFER];

  va_list ArgPtr;
  va_start(ArgPtr,FormatStr);

  if(!m_Mask)
  {
    vsprintf(Buffer,FormatStr,ArgPtr);

    // New line is added for every print
    printf("%s\n",Buffer);
  }

  va_end(ArgPtr);
}

// Formatted output to the monitor (non maskable)
void CQMonitor::PrintfNonMaskable(const char *FormatStr,...)
{
  char Buffer[MAX_PRINT_BUFFER];

  va_list ArgPtr;
  va_start(ArgPtr,FormatStr);

  vsprintf(Buffer,FormatStr,ArgPtr);

  // New line is added for every print
  printf("%s\n",Buffer);

  va_end(ArgPtr);
}

// Show an error message (formatted)
TQErrCode CQMonitor::ErrorMessage(QString Str)
{
  printf("Error: %s\n",Str.c_str());
  return Q_NO_ERROR;
}

// Print a warning message (formatted)
TQErrCode CQMonitor::WarningMessage(QString Str)
{
  printf("Warning: %s\n",Str.c_str());
  return Q_NO_ERROR;
}

// Print a notification message (formatted)
TQErrCode CQMonitor::NotificationMessage(QString Str)
{
  printf("%s\n",Str.c_str());
  return Q_NO_ERROR;
}

// Get a single character from the monitor
char CQMonitor::GetChar(void)
{
  return getchar();
}

// Request a number from the user (integer version)
bool CQMonitor::GetNumber(const char *Prompt,int& Number)
{
  bool RetFlag = true;
  printf("%s\n",Prompt);

  try
  {
    char InputBuffer[80];
    for ( int i = 0; i < 80 ; ++i)
      InputBuffer[i] = '\0';

    sscanf("%80s",InputBuffer);
    Number = QStrToInt(InputBuffer);
  } catch(...)
    {
      RetFlag = false;
    }

  return RetFlag;
}

// Request a number from the user (float version)
bool CQMonitor::GetNumber(const char *Prompt,double& Number)
{
  bool RetFlag = true;
  printf("%s\n",Prompt);

  try
  {
    char InputBuffer[80];
    for ( int i = 0; i < 80 ; ++i)
      InputBuffer[i] = '\0';
	  
    sscanf("%80s",InputBuffer);
    Number = QStrToFloat(InputBuffer);
  } catch(...)
    {
      RetFlag = false;
    }

  return RetFlag;
}

// Ask the user a yes/no question (retrun true if yes)
bool CQMonitor::AskYesNo(const char *Prompt)
{
  bool RetFlag;
  char Input;

  printf("%s\n",Prompt);

  for(;;)
  {
    // Get a character from the user
    Input = GetChar();

    switch(Input)
    {
      case 'Y':
      case 'y':
        return true;

      case 'N':
      case 'n':
        return false;

      default:
        printf("Invalid option (press Y or N)");
    }
  }
}

// Ask the user a yes/no question (retrun true if yes)
bool CQMonitor::AskYesNo(const QString Prompt)
{
  bool RetFlag;
  char Input;

  printf("%s\n",Prompt);

  for(;;)
  {
    // Get a character from the user
    Input = GetChar();

    switch(Input)
    {
      case 'Y':
      case 'y':
        return true;

      case 'N':
      case 'n':
        return false;

      default:
        printf("Invalid option (press Y or N)");
    }
  }
}

// Read a generic string
QString CQMonitor::GetString(const char *Prompt)
{
  char Buffer[101];

  printf("%s\n",Prompt);

  scanf("%100s",Buffer);

  return QString(Buffer);
}

