// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Varpyth.pas' rev: 11.00

#ifndef VarpythHPP
#define VarpythHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Variants.hpp>	// Pascal unit
#include <Pythonengine.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Varpyth
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
enum TSequenceType { stTuple, stList };
#pragma option pop

//-- var, const, procedure ---------------------------------------------------
extern PACKAGE Variant __fastcall VarPythonCreate(Pythonengine::PPyObject AObject)/* overload */;
extern PACKAGE Variant __fastcall VarPythonCreate(const Variant &AValue)/* overload */;
extern PACKAGE Variant __fastcall VarPythonCreate(System::TVarRec const * AValues, const int AValues_Size, TSequenceType ASequenceType = (TSequenceType)(0x1))/* overload */;
extern PACKAGE Variant __fastcall VarPythonEval(const AnsiString APythonExpression);
extern PACKAGE Variant __fastcall GetAtom(Pythonengine::PPyObject AObject);
extern PACKAGE Word __fastcall VarPython(void);
extern PACKAGE bool __fastcall VarIsPython(const Variant &AValue);
extern PACKAGE Variant __fastcall VarAsPython(const Variant &AValue);
extern PACKAGE Pythonengine::PPyObject __fastcall ExtractPythonObjectFrom(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsSame(const Variant &A, const Variant &B);
extern PACKAGE bool __fastcall VarIsSameType(const Variant &A, const Variant &B);
extern PACKAGE bool __fastcall VarIsPythonSequence(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonMapping(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonNumber(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonString(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonInteger(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonFloat(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonTuple(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonList(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonDict(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonClass(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonInstance(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonMethod(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonFunction(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonModule(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonCallable(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonUnicode(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonDateTime(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonDate(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonTime(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonDateTimeDelta(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsPythonTZInfo(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsBool(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsEnum(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsInstanceOf(const Variant &AInstance, const Variant &AClass);
extern PACKAGE bool __fastcall VarIsSubclassOf(const Variant &ADerived, const Variant &AClass);
extern PACKAGE bool __fastcall VarIsSubtypeOf(const Variant &ADerived, const Variant &AType);
extern PACKAGE bool __fastcall VarIsNone(const Variant &AValue);
extern PACKAGE bool __fastcall VarIsTrue(const Variant &AValue);
extern PACKAGE Variant __fastcall NewPythonList(const int ASize = 0x0);
extern PACKAGE Variant __fastcall NewPythonTuple(const int ASize);
extern PACKAGE Variant __fastcall NewPythonDict();
extern PACKAGE Variant __fastcall None();
extern PACKAGE Variant __fastcall Ellipsis();
extern PACKAGE Variant __fastcall MainModule();
extern PACKAGE Variant __fastcall BuiltinModule();
extern PACKAGE Variant __fastcall SysModule();
extern PACKAGE Variant __fastcall DatetimeModule();
extern PACKAGE Variant __fastcall Import(const AnsiString AModule);
extern PACKAGE int __fastcall len(const Variant &AValue);
extern PACKAGE Variant __fastcall _type(const Variant &AValue);

}	/* namespace Varpyth */
using namespace Varpyth;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Varpyth
