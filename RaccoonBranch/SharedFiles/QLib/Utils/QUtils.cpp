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
 * Last upate: 14/04/2003                                           *
 ********************************************************************/

#include <iomanip.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "QUtils.h"
#include <cctype> // for toupper
#include <algorithm>
#include "QFile.h"

using namespace std;



// File path delimeters
#ifdef OS_WINDOWS
  const char FILE_PATH_DELIM[] = "\\:";
  const char FILE_PATH_SLASH = '\\';
#else
  const char FILE_PATH_DELIM[] = "/";
  const char FILE_PATH_SLASH = '/';  
#endif

const int MAX_FORMATTED_STRING_SIZE = 256;

QString QGenerateRandomString(int len)
{
    QString string = "";
    
    const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i)
        string += alphanum[rand() % (sizeof(alphanum) - 1)];

    return string;
}


void Tokenize(const QString str, TQStringVector& tokens, const QString delimiters, bool PushEmpty)
{
    // Skip delimiters at beginning.
    QString::size_type lastPos = str.find_first_not_of(delimiters, 0);
    if (PushEmpty && lastPos > 0)
        for (unsigned i = 0; i < lastPos; i++)
           tokens.push_back("");
            
    // Find first "non-delimiter".
    QString::size_type pos = str.find_first_of(delimiters, lastPos);

    while (QString::npos != pos || QString::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        if (PushEmpty)
        {
           int lastRealPos = (QString::npos == lastPos && QString::npos != pos) ? str.size()+1 : lastPos;
           if (lastRealPos > static_cast<int>(pos+1))
               for (int i = pos+1; i < lastRealPos; i++)
                  tokens.push_back("");
        }
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void JoinTokens(QString& str, TQStringVector tokens, const QString delimiters)
{
  str = "";
  if (tokens.size() > 0)
  {
    TQStringVectorIter Iter = tokens.begin();
    str = *Iter;
    Iter++;
    for (; Iter != tokens.end(); Iter++)
        str += delimiters + *Iter;
  }
}


// Remove leading and trailing space characters from a string (\n,\r,\t,' ')
QString QTrimString(const QString Str)
{
  int StartChar = Str.find_first_not_of(" \n\r\t");
  int EndChar = Str.find_last_not_of(" \n\r\t");

  if(EndChar == -1)
    return "";

  return Str.substr(StartChar,EndChar - StartChar + 1);
}

// Format a string (printf style) and return a QString object
QString QFormatStr(const char *FormatStr,...)
{
  va_list ArgPtr;
  va_start(ArgPtr,FormatStr);

  // Print the formatted string to a temporary buffer
  char Buffer[MAX_FORMATTED_STRING_SIZE];

  // Prepare the formatted string
  vsnprintf(Buffer,MAX_FORMATTED_STRING_SIZE - 1,FormatStr,ArgPtr);
  Buffer[MAX_FORMATTED_STRING_SIZE - 1] = NULL;

  va_end(ArgPtr);

  // Convert to QString
  QString QStr(Buffer);

  return QStr;
}

// Convert a QString to integer
int QStrToInt(const QString Str)
{
  TStrStream StrStream;
  int v;

  StrStream << Str;

  // Convert the string to integer
  StrStream >> v;

  if(StrStream.fail())
    throw EQUtils("Can not convert string (" + Str + ") to integer");

  return v;
}

QString QBoolToStr(const bool v, bool UseBoolStrs)
{
  if (UseBoolStrs)
     return (v ? "true" : "false"); 
  return QIntToStr(v);
}

// Convert an integer to QString
QString QIntToStr(const int v)
{
  TStrStream StrStream;

#ifdef OS_WINDOWS
  StrStream << v;
#elif defined(OS_VXWORKS)
  StrStream << v << ends;
#endif

  return StrStream.str();
}

// Convert a QString to float
double QStrToFloat(const QString Str)
{
  TStrStream StrStream;
  double v;

  StrStream << Str;

  // Convert the string to float number
  StrStream >> v;

  if(StrStream.fail())
    throw EQUtils("Can not convert string (" + Str + ") to float");

  return v;
}

// Convert an integer to QString
QString QFloatToStr(const double v)
{
  TStrStream StrStream;

#ifdef OS_WINDOWS
  StrStream << v;
#elif defined(OS_VXWORKS)
  StrStream << v << ends;
#endif

  return StrStream.str();
}

// Return the current local time_t
time_t QGetCurrentTime(void)
{
  return time(NULL);
}

QString QReplaceSubStr(const QString Str, const QString SubFromStr, const QString SubToStr)
{
   size_t    ip       = 0;
   QString   StrClone = Str;

   while ( (ip = StrClone.find(SubFromStr.c_str(), ip)) != string::npos )
   {
      StrClone.replace( ip, SubFromStr.size(), SubToStr.c_str() );
      ip += SubToStr.size();
   }
   return StrClone;
}

QString QStrToUpper(const QString Str)
{
   QString S = Str;
   std::transform(Str.begin(), Str.end(), S.begin(),
                 (int(*)(int)) toupper);
   return S;
}

QString QStrToLower(const QString Str)
{
   QString S = Str;
   std::transform(Str.begin(), Str.end(), S.begin(),
                 (int(*)(int)) tolower);
   return S;
}

// Convert time_t to string representation
QString QDateTimeToStr(const time_t TimeToConvert)
{
  char Buffer[32];
  struct tm *LocalTimeTM = localtime(&TimeToConvert);

  if(!strftime(Buffer,32,"%d/%m/%y %X",LocalTimeTM))
    throw EQUtils("Can not format date/time string");

  return QString(Buffer);
}

// Convert time_t to string representation (time only)
QString QTimeToStr(const time_t TimeToConvert)
{
  char Buffer[32];
  struct tm *LocalTimeTM = localtime(&TimeToConvert);

  if(!strftime(Buffer,32,"%X",LocalTimeTM))
    throw EQUtils("Can not format time string");

  return QString(Buffer);
}

// Convert time_t to string representation (date only)
QString QDateToStr(const time_t TimeToConvert)
{
  char Buffer[32];
  struct tm *LocalTimeTM = localtime(&TimeToConvert);

  if(!strftime(Buffer,32,"%d/%m/%y",LocalTimeTM))
    throw EQUtils("Can not format date string");

  return QString(Buffer);
}

// Convert a string representing time to time_t type (e.g. 2/3/03 21:05)
time_t QStrToDateTime(const QString TimeString)
{
  struct tm stm;

  if(sscanf(TimeString.c_str(),"%d/%d/%d %d:%d:%d",&stm.tm_mday,&stm.tm_mon,&stm.tm_year,&stm.tm_hour,
                                  &stm.tm_min,&stm.tm_sec) != 6)
    throw EQUtils("Can not convert string to time");

  // Fix the time struct fields
  stm.tm_year += 100;
  stm.tm_mon--;
  stm.tm_isdst = -1;

  time_t RetTime;

  // Convert to time_t and return
  if((RetTime = mktime(&stm)) == -1)
    throw EQUtils("Can not make time from string");

  return RetTime;
}

// Convert a string representing time to time_t type (e.g. 21:05)
time_t QStrToTime(const QString TimeString)
{
  struct tm stm;

  if(sscanf(TimeString.c_str(),"%d:%d:%d",&stm.tm_hour,&stm.tm_min,&stm.tm_sec) != 3)
    throw EQUtils("Can not convert string to time");

  stm.tm_mday = stm.tm_mon = stm.tm_year = 0;

  time_t RetTime;

  // Convert to time_t and return
  if((RetTime = mktime(&stm)) == -1)
    throw EQUtils("Can not make time from string");

  return RetTime;
}

// Convert a string representing time to time_t type (e.g. 2/3/03)
time_t QStrToDate(const QString TimeString)
{
  struct tm stm;

  if(sscanf(TimeString.c_str(),"%d/%d/%d",&stm.tm_mday,&stm.tm_mon,&stm.tm_year) != 3)
    throw EQUtils("Can not convert string to time");

  stm.tm_hour = stm.tm_min = stm.tm_sec = 0;

  // Fix the time struct fields
  stm.tm_year += 100;
  stm.tm_mon--;
  stm.tm_isdst = -1;

  time_t RetTime;

  // Convert to time_t and return
  if((RetTime = mktime(&stm)) == -1)
    throw EQUtils("Can not make time from string");

  return RetTime;
}

// Convert a number to hex
QString QIntToHex(const int Number,const int Digits)
{
  QString FormatStr = "%." + QIntToStr(Digits) + "X";
  return QFormatStr(FormatStr.c_str(),Number);
}

// Convert a hex string number to integer
int QHexToInt(const QString HexStr)
{
  int i;

  if(sscanf(HexStr.c_str(),"%x",&i) == 0)
    throw EQUtils("Can not convert hex string to number");

  return i;
}

// Extarct the file name include the extension
QString QExtractFileName(const QString Str)
{
  int i = Str.find_last_of(FILE_PATH_DELIM);

  return Str.substr(i + 1,Str.length() - i - 1);
}

size_t QDumpBufferToFile(QString FileName, PBYTE Buffer, unsigned BufferLength)
{
  QOSFileCreator FileCreator;

  // Try to open the file
  //FileCreator.CreateFile(FileName,"wb");

	try{
		// Open the file for write
		FileCreator.CreateFile(FileName,"wb");
	}
	catch(EQOSFileCreator& Err)
	{
		throw Err;
	}

  FILE *out = FileCreator.toSTDIO();

  size_t ret = 0;
  if (out)
  {
    ret = fwrite(Buffer, sizeof(BYTE), BufferLength, out);
    fclose(out);
  }
  return ret;
}//QDumpBufferToFile

// Extarct the file name without the extension
QString QExtractFileNameWithoutExt(const QString Str)
{
  // Search for the file name start
  int FNameStart = Str.find_last_of(FILE_PATH_DELIM) + 1;

  // Search for the last '.' character
  int DotPos = Str.find_last_of(".");
  return Str.substr(FNameStart,DotPos - FNameStart);
}

// Extarct file path include the extension
QString QExtractFilePath(const QString Str)
{
  int i = Str.find_last_of(FILE_PATH_DELIM);

  return Str.substr(0,i + 1);
}

// Extarct only the file extension
QString QExtractFileExt(const QString Str)
{
  int i = Str.find_last_of(".");

  if(i != -1)
    return Str.substr(i + 1,Str.length() - i);

  return "";
}

// Extarct only the file drive name not including the ':' character (if applicable for the specific OS)
// Note: If it is a network drive, the return value will be '\\'
QString QExtractFileDrive(const QString Str)
{
  int i = Str.find_first_of(":");

  // It is a normal path
  if(i != -1)
    return Str.substr(0,1);

  if(Str.length() > 2)
    if((Str[0] == '\\') && (Str[1] == '\\'))
      return "\\\\";

  return "";
}

// Add a path delimeter at the end of a file name if neccessery
QString QAddPathDelimeter(const QString Str)
{
  QString Result = Str;

  // Check if the file has already a path delimeter at its end
  if(Str[Str.length() - 1] != FILE_PATH_SLASH)
    Result.append(1,FILE_PATH_SLASH);

  return Result;
}

// Break a key/value combination (in the form key=value)
// Return false if the seperating '=' character can not be found
bool QBreakKeyAndValueStr(const QString Str,QString& Key,QString& Value,char BreakChar)
{
  // Search for the '=' character
  int i = Str.find_first_of(BreakChar);

  if(i != -1)
  {
    // The key part is trimmed
    Key = QTrimString(Str.substr(0,i));
    Value = Str.substr(i + 1,Str.length() - i);
  } else
      return false;

  return true;
}

// Make sure that the line break characters match the OS specific characters
QString QFixLineBreaks(const QString Str)
{
#ifdef OS_WINDOWS
  QString ResultStr;

  ResultStr.reserve(Str.length());

  for(unsigned i = 0; i < Str.length(); i++)
  {
    if(Str[i] == '\n')
    {
      if(i > 0)
        if(Str[i - 1] != '\r')
        {
          ResultStr.append(1,'\r');
          ResultStr.append(1,'\n');
        }
    } else
        ResultStr.append(1,Str[i]);
  }

  return ResultStr;
#else

  return Str;
#endif
}

// Simple round operation
long QSimpleRound(float x)
{
  if(x > 0.0f)
    return (long)(x + 0.5f);

  return (long)(x - 0.5f);
}

// Specialization for bool
template<>
bool QStrToValue<bool>(const QString Str)
{
  // Check if the string is the word "false" or "true" (not case sensitive)
  if(!stricmp(Str.c_str(),"true"))
    return true;

  if(!stricmp(Str.c_str(),"false"))
    return false;

  TStrStream StrStream;
  bool v;

  StrStream << Str;

  // Convert the string to integer
  StrStream >> v;

  if(StrStream.fail())
    throw EQUtils("Can not convert string (" + Str + ") to boolean");

  return v;
}

// Utility function for conversion of unsigned types
template <class T>
T UnsignedStrToValue(const QString& Str)
{
  T r = ConvertStrToValue<T>(Str);

  // Ignore signed numbers
  if(Str[0] == '-')
    throw EQUtils("Can not convert string (" + Str + ") to value");

  return r;
}

// Specialization for all the unsigned types
template<>
unsigned QStrToValue<unsigned>(const QString Str)
{
  return UnsignedStrToValue<unsigned>(Str);
}

template<>
unsigned char QStrToValue<unsigned char>(const QString Str)
{
 unsigned int UnsignedIntValue;

 UnsignedIntValue = (UnsignedStrToValue<int>(Str));

 //If the int value is larger then one BYTE value
 //throw exception and not cast to char
 if(UnsignedIntValue > 255)
	throw EQUtils("Trying to convert ""int"" type larger then BYTE value to char");
 else
	return (unsigned char)UnsignedIntValue;
}

template<>
unsigned short QStrToValue<unsigned short>(const QString Str)
{
  return UnsignedStrToValue<unsigned short>(Str);
}

template<>
unsigned long QStrToValue<unsigned long>(const QString Str)
{
  return UnsignedStrToValue<unsigned long>(Str);
}

