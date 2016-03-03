/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: Basic types and macros.                                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/07/2001                                           *
 * Last upate: 26/09/2001                                           *
 ********************************************************************/

#ifndef _Q_TYPES_H_
#define _Q_TYPES_H_

/* The following macros contorl the platform / OS specifc compilation:

Operatin system macros
----------------------
(One of the following macros must be defined for proper compilation)
OS_WINDOWS  - Windows (win32)
OS_VXWORKS  - VxWorks

*/

// The following macros check the validity of the OS macro defintion

#if !defined(OS_WINDOWS) && !defined(OS_VXWORKS)
#error QLib: OS specific compilation switch must be defined (OS_WINDOWS or OS_VXWORKS)
#endif

#if defined(OS_WINDOWS) && defined(OS_VXWORKS)
#error QLib: Contention between OS specific compilation switches (OS_WINDOWS & OS_VXWORKS)
#endif

// We need C++ string
#include <string>
#include <vector>
#include <map>

// Choose between the old and new string stream classes according to the selected OS
#ifdef OS_WINDOWS
// Use the new ANSI string stream
#include <sstream>
typedef std::stringstream TStrStream;
#elif defined(OS_VXWORKS)
// Use old ANSI string stream class
#include <strstream>
typedef std::strstream TStrStream;
#endif

// Basic types
//*******************************************************************

#ifdef OS_WINDOWS

// Types required for windows compilation
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef long           *PLONG;
typedef unsigned long  *PULONG;

#elif defined(OS_VXWORKS)
// Types required for VxWorks compilation

typedef int            SINT;        // 32 bit signed int  
typedef unsigned long  DWORD;       // 32 bit unsigned int
typedef unsigned short WORD;       // 16 bit unsigned int
typedef unsigned char  BYTE;       // 8 bit unsigned int
typedef short          SWORD;       // 16 bit signed int
typedef BYTE          *BYTE_PTR ;  // BYTE POINTER

#endif

// Declarations required for all platforms
typedef unsigned char   BYTE;
typedef unsigned char  *PBYTE;
typedef short          *PSHORT;
typedef long           *PLONG;
typedef unsigned long  *PULONG;
typedef char           *PCHAR;
typedef void           *PVOID;
typedef unsigned short *PUSHORT;


// Additional usefull types
//*******************************************************************

// Std C++ library compliant string
typedef std::string QString;
typedef QString TQStringArray[];

typedef std::vector<QString> TQStringVector;
typedef TQStringVector::iterator TQStringVectorIter;

typedef std::map<QString,QString> TQStringMap;
typedef TQStringMap::iterator TQStringMapIter;

// Generic cockie and callback types
typedef ULONG TGenericCockie;
typedef void (*TGenericCallback)(TGenericCockie Cockie);

// Standard error code type
typedef int TQErrCode;
typedef int TQActionErrCode;

typedef float TQPercentage;
typedef long int TQMilliseconds;
typedef long int TQSeconds;
typedef unsigned long TQTicks; // todo -oNobody -cNone: why can't I make it DWORD ?!?!?!

// Disable the C++ builder yells about functions that can not be inlined and
// about code in header files.
#ifdef __BCPLUSPLUS__
#pragma option -w-inl
#pragma option -w-pch
#endif

#endif

