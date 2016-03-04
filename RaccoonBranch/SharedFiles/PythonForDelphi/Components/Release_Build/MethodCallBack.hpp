// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Methodcallback.pas' rev: 11.00

#ifndef MethodcallbackHPP
#define MethodcallbackHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Methodcallback
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
enum TCallType { ctSTDCALL, ctCDECL };
#pragma option pop

typedef void __fastcall (__closure *TCallBack)(void);

typedef int __stdcall (__closure *TDDEAPIfunc)(int CallType, int Fmt, int Conv, int hsz1, int hsz2, int Data, int Data1, int Data2);

//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void * __fastcall GetDDECallBack(TDDEAPIfunc method);
extern PACKAGE void * __fastcall GetOfObjectCallBack(TCallBack CallBack, int argnum, TCallType calltype);
extern PACKAGE void * __fastcall GetCallBack(System::TObject* self, void * method, int argnum, TCallType calltype);
extern PACKAGE void __fastcall DeleteCallBack(void * Proc);

}	/* namespace Methodcallback */
using namespace Methodcallback;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Methodcallback
