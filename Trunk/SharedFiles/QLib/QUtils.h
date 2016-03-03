/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Module Description: Collection of various utility functions and  *
 *                     classes.                                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 10/07/2001                                           *
 * Last upate: 27/08/2003                                           *
 ********************************************************************/

#ifndef _Q_UTILS_H_
#define _Q_UTILS_H_

#include <time.h>
#include "QTypes.h"
#include "QException.h"


// Exception class for all the QLib RTTI elements
class EQUtils : public EQException {
  public:
    EQUtils(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

void Tokenize(const QString str, TQStringVector& tokens, const QString delimiters = ",", bool PushEmpty=false);
void JoinTokens(QString& str, TQStringVector tokens, const QString delimiters = ",");

QString QGenerateRandomString(int len);

// Remove leading and trailing space characters from a string (\n,\r,\t,' ')
QString QTrimString(const QString Str);

// Format a string (printf style) and return a QString object
QString QFormatStr(const char *FormatStr,...);

// Convert a QString to integer
int QStrToInt(const QString Str);

QString QBoolToStr(const bool v, bool UseBoolStrs = false);

// Convert an integer to QString
QString QIntToStr(const int i);

// Convert a QString to float
double QStrToFloat(const QString Str);

// Convert an integer to QString
QString QFloatToStr(const double v);

// Return the current local time_t
time_t QGetCurrentTime(void);

// Convert time_t to string representation (time only)
QString QTimeToStr(const time_t TimeToConvert);

// Convert time_t to string representation (date only)
QString QDateToStr(const time_t TimeToConvert);

// Convert time_t to string representation (date/time)
QString QDateTimeToStr(const time_t TimeToConvert);

// Convert a string representing time to time_t type (e.g. 21:05)
time_t QStrToTime(const QString TimeString);

// Convert a string representing time to time_t type (e.g. 2/3/03)
time_t QStrToDate(const QString TimeString);

// Convert a string representing time to time_t type (e.g. 2/3/03 21:05)
time_t QStrToDateTime(const QString TimeString);

// Convert a number to hex
QString QIntToHex(const int Number,const int Digits=1);

// Break a key/value combination (in the form key=value)
// Return false if the seperating '=' character can not be found
bool QBreakKeyAndValueStr(const QString Str,QString& Key,QString& Value,char BreakChar = '=');

// Convert a hex string number to integer
int QHexToInt(const QString HexStr);

// Extarct the file name include the extension
QString QExtractFileName(const QString Str);

// Extarct the file name without the extension
QString QExtractFileNameWithoutExt(const QString Str);

// Extarct file path include the extension
QString QExtractFilePath(const QString Str);

// Extarct only the file extension
QString QExtractFileExt(const QString Str);

// Extarct only the file drive name not including the ':' character (if applicable for the specific OS)
QString QExtractFileDrive(const QString Str);

// Add a path delimeter at the end of a file name if neccessery
QString QAddPathDelimeter(const QString Str);

// Make sure that the line break characters match the OS specific characters
QString QFixLineBreaks(const QString Str);

QString QReplaceSubStr(const QString Str, const QString SubFromStr, const QString SubToStr);
QString QStrToUpper(const QString S);
QString QStrToLower(const QString S);
QString QFixLineBreaks(const QString Str);

size_t QDumpBufferToFile(QString FileName, PBYTE Buffer, unsigned BufferLength);

// Simple round operation
long QSimpleRound(float x);


/*
WoW64 (Windows 32-bit on Windows 64-bit) is a subsystem of the Windows operating system capable of running 32-bit applications
and is included on all 64-bit versions of Windows—including Windows XP Professional x64 Edition, IA-64 and x64 versions of Windows Server 2003,
as well as 64-bit versions of Windows Vista, Windows Server 2008, Windows 7 and Windows 8. In Windows Server 2008 R2 Server Core,
it is an optional component. WoW64 is designed to take care of many of the differences between 32-bit Windows and 64-bit Windows,
particularly involving structural changes to Windows itself.
*/
bool IsWow64();

bool IsRunningOnWindows64Bit();

// Generic version of type to string conversion routines

// Do not use this function directly! Please use the QStrToValue function for conversion
// from string to some arbitrary type.
template<class T>
T ConvertStrToValue(const QString& Str)
{
  TStrStream StrStream;
  T v;

  StrStream << Str;

  // Convert string to value of type T
  StrStream >> v;

  if(StrStream.fail() ||
     ((unsigned)StrStream.tellg() != Str.length()))
    throw EQUtils("Can not convert string (" + Str + ") to value");

  return v;
}

template<> QString ConvertStrToValue<QString>(const QString& Str)
{
  TStrStream StrStream;
  QString v;

  StrStream << Str;

  // Convert string to value of type T
  StrStream >> v;

  return v;
}


// Generic version
template<class T>
T QStrToValue(const QString Str)
{
  return ConvertStrToValue<T>(Str);
}

// Specialization for bool
template<>
bool QStrToValue<bool>(const QString Str);

// Specialization for all the unsigned types
template<>
unsigned QStrToValue<unsigned>(const QString Str);

template<>
unsigned char QStrToValue<unsigned char>(const QString Str);

template<>
unsigned short QStrToValue<unsigned short>(const QString Str);

template<>
unsigned long QStrToValue<unsigned long>(const QString Str);

template<class T>
QString QValueToStr(T v)
{
  TStrStream StrStream;

#ifdef OS_WINDOWS
  StrStream << v;
#elif defined(OS_VXWORKS)
  StrStream << v << ends;
#endif

  return StrStream.str();
}

// These macros prevnt stepping into invalidated pointers.  p is set to null even if p's destructor throws exception
#define Q_SAFE_DELETE(p) {try{if (p) delete p;}catch(...){p = 0; throw;}p = 0;}
#define Q_SAFE_DELETE_ARRAY(p) {try{if (p) delete[] p;}catch(...){p = 0; throw;}p = 0;} // use this when p was allocated in a way similar to this: p = new char[90];


#endif

