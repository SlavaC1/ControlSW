{ *********************************************************************** }
{ SystemCppException.pas                                                  }
{                                                                         }
{ Enables the Delphi RTL to catch, dispatch and cleanup C++ exceptions    }
{ and to handle exceptions derived from std::exception.                   }
{                                                                         }
{ Author:               Moritz Beutel                                     }
{ Initial release date: 2008/09/24                                        }
{ Date:                 2010/02/26                                        }
{ Version:              1.05                                              }
{ License:              see below                                         }
{ Copyright:            (c) 2008, 2010 AUDACIA Software                   } 
{                                                                         }
{ Supported Delphi/C++Builder versions:                                   }
{   Delphi 6, 7, 2005 through 2010                                        }
{   C++Builder 6 through 2010                                             }
{ Tested with C++Builder 6, 2009 and 2010.                                }
{ *********************************************************************** }


unit SystemCppException;

interface

uses
  SysUtils;

type
  PCppStdException = type Pointer; { mapped to std::exception* via $HPPEMIT }
  {$EXTERNALSYM PCppStdException}

  { C++ exception of any type }
  ECppException = class (Exception)
  private
    FTypeName: AnsiString;
    FExcDesc:  Pointer;

    constructor CreateTypeNamed (_TypeName: PAnsiChar; ExcDesc: Pointer); overload;
    function GetCppExceptionObject: Pointer;
    function GetThrowLine: Integer;
    function GetThrowFile: AnsiString;
  public
    property CppExceptionObject: Pointer read GetCppExceptionObject;
    property ThrowLine: Integer read GetThrowLine;
    property ThrowFile: AnsiString read GetThrowFile;
    property TypeName: AnsiString read FTypeName;

    function IsCppClass: Boolean;
    function AsCppClass (CppClassName: AnsiString): Pointer;

    destructor Destroy; override;
  end;

  { C++ exception derived from std::exception }
  ECppStdException = class (ECppException)
  private
    FExcObj: PCppStdException;

    constructor Create (AExcObj: PCppStdException; Msg: String;
      _TypeName: PAnsiChar; ExcDesc: Pointer); overload;
    function GetStdException: PCppStdException;
  public
    { This property returns a pointer to the wrapped exception. }
    property StdException: PCppStdException read GetStdException;
  end;

  (*$HPPEMIT '#include <typeinfo>'*)
  (*$HPPEMIT '#include <exception>'*)
  (*$HPPEMIT ''*)
  (*$HPPEMIT 'namespace Systemcppexception'*)
  (*$HPPEMIT '{'*)
  (*$HPPEMIT ''*)
  (*$HPPEMIT 'class DELPHICLASS ECppException;'*)
  (*$HPPEMIT ''*)
  (*$HPPEMIT 'template <class E>'*)
  (*$HPPEMIT '    class CppExceptionWrapper'*)
  (*$HPPEMIT '{'*)
  (*$HPPEMIT 'private:'*)
  (*$HPPEMIT '    E* internalClass;'*)
  (*$HPPEMIT '    struct _safebool_t;'*)
  (*$HPPEMIT ''*)
  (*$HPPEMIT 'public:'*)
  (*$HPPEMIT '    CppExceptionWrapper (ECppException* exception)'*)
  (*$HPPEMIT '     : internalClass (static_cast <E*> (exception->AsCppClass (typeid (E).name ())))'*)
  (*$HPPEMIT '    {}'*)
  (*$HPPEMIT '    operator _safebool_t* (void) const'*)
  (*$HPPEMIT '    { return (_safebool_t* ) internalClass; }'*)
  (*$HPPEMIT '    E& operator * (void)'*)
  (*$HPPEMIT '    { return *internalClass; }'*)
  (*$HPPEMIT '    E* operator -> (void)'*)
  (*$HPPEMIT '    { return internalClass; }'*)
  (*$HPPEMIT '};'*)
  (*$HPPEMIT ''*)
  (*$HPPEMIT 'typedef std::exception* PCppStdException;'*)
  (*$HPPEMIT ''*)
  (*$HPPEMIT '}	/* namespace Systemcppexception */'*)


procedure InstallCppExceptionFilter;
procedure UninstallCppExceptionFilter;

implementation

type
  TExceptObjProc = function (P: PExceptionRecord): Exception;

  PCppTypeId = ^TCppTypeId;

  TCppBaseList = packed record { Delphi equivalent of struct baseList in xx.h }
    blType  : PCppTypeId; // type   of this base
    blOffs  : Cardinal;   // offset of this base
    blFlags : Cardinal;   // flags
  end;

  TCppTypeId = packed record { incomplete Delphi equivalent of struct tpid in xx.h }
    tpSize        : Cardinal; // size of type in bytes
    tpMask        : Word;     // attribute bits
    tpName        : Word;     // offset of start of the zero terminated name
                              // where offset is relative from tpid base

    tpcVptrOffs   : Cardinal; // offset of vtable pointer
    tpcFlags      : Cardinal; // more flags

    { Only valid if (tpMask & TM_IS_CLASS) }
    tpcBaseList   : Word;     // offset of non-virt base list,
                              // where offset is relative from tpid base
    tpcVbasList   : Word;     // offset of virtual  base list
                              // where offset is relative from tpid base

    tpcDlOpAddr   : Pointer;  // operator delete   addr
    tpcDlOpMask   : Word;     // operator delete   convention

    tpcDaOpMask   : Word;     // operator delete[] convention
    tpcDaOpAddr   : Pointer;  // operator delete[] addr

    { Only valid if (tpcFlags & CF_HAS_DTOR), implies also (tpMask & TM_IS_CLASS) }
    tpcDtorCount  : Cardinal; // dtor count - total
    tpcNVdtCount  : Cardinal; // dtor count - non-virtual

    tpcDtorAddr   : Pointer;  // destructor addr
    tpcDtorMask   : Word;     // destructor convention

    tpcDtMembers  : Word;     // offset of list of destructible members
                              // where offset is relative from tpid base

    { ... }

    { Following is the zero terminated name, padded with zero's to
      the next dword boundary.

      Optionally (if tpMask & TM_IS_CLASS), we have next:
       - non-virtual base list, terminated by a null pointer
       -     virtual base list, terminated by a null pointer

      Optionally (if tpcFlags & CF_HAS_DTOR), we have next:
       - list of destructible members, terminated by a null pointer }
  end;

const

  { Flags for TCppTypeId.tpMask }
  TM_IS_STRUCT   = $0001;
  TM_IS_CLASS    = $0002;
  TM_IS_PTR      = $0010;
  TM_IS_REF      = $0020;
  TM_IS_VOIDPTR  = $0040;
  TM_LOCALTYPE   = $0080;
  TM_IS_CONST    = $0100;
  TM_IS_VOLATILE = $0200;
  TM_IS_ARRAY    = $0400;

  { Flags for TCppTypeId.tpcFlags }
  CF_HAS_CTOR    = $00000001;
  CF_HAS_DTOR    = $00000002;
  CF_HAS_BASES   = $00000004;
  CF_HAS_VBASES  = $00000008;
  CF_HAS_VTABPTR = $00000010;
  CF_HAS_VIRTDT  = $00000020;
  CF_HAS_RTTI    = $00000040;
  CF_DELPHICLASS = $00000080;
  CF_HAS_FARVPTR = $00001000;
  CF_HAS_GUID    = $00002000;

  { Flags for TCppExceptDesc.xdFlags }
  XDF_ISDELPHIEXCEPTION   = $00000004;
  XDF_RETHROWN            = $00000008;

type
  PCppExceptDesc = ^TCppExceptDesc;
  TCppFreeMemFP = procedure (P: PCppExceptDesc); cdecl;

  TCppExceptDesc = packed record { Delphi equivalent of struct _exceptDesc in xx.h }
    xdPrevious    : Pointer;        // previous exception or 0

    xdTypeID      : PCppTypeId;     // addr of type-id for thrown type
    xdFriendList  : PCppTypeId;     // friend list supplied to _ThrowExcept
    xdFlags       : Cardinal;       // flags passed to _ThrowExcept
    xdSize        : Cardinal;       // size of thrown value
    xdBase        : PCppTypeId;     // type-id of base type
    xdMask        : Word;           // type-id mask
    xdCflg        : Word;           // type-id class flags (or 0)

    xdFreeFunc    : TCppFreeMemFP;  // function to free memory

    xdCCaddr      : Pointer;        // copy-ctor addr
    xdCCmask      : Cardinal;       // copy-ctor mask

    xdERRaddr     : Pointer;        // address of matching ERR (when found)
    xdHtabAdr     : Pointer;        // address of matching handler

    xdContext     : Cardinal;       // context of 'catch' block

    xdThrowLine   : Cardinal;       // source line no.  of throw statement
    xdThrowFile   : PAnsiChar;      // source file name of throw statement

    xdArgType     : PCppTypeId;     // address of arg type descriptor
    xdArgAddr     : Pointer;        // address of arg copy on stack
    xdArgBuff     : AnsiChar;       // arg stored in buffer
    xdArgCopy     : AnsiChar;       // arg copied to catch arg

    xdOSESP       : LongWord;       // esp of main OS exception handler
    xdOSERR       : LongWord;       // addr of the OS ERR on entry to _ExceptionHandler
    xdOSContext   : {PContext} Pointer;       // CPU Context for an OS exception
    xdValue       : array[0..0] of AnsiChar;  // copy of thrown value
  end;

  PCppBaseList = ^TCppBaseList;

{ pre-Tiburon workaround }
{$IFNDEF VER200}
  PByte = PAnsiChar;
{$ENDIF}

var
  OldExceptObjProc: TExceptObjProc;

resourcestring
  SCppExceptionMsg = 'Unhandled C++ Exception of type ''%s'' occurred.';

procedure ExceptionHandled (ExcDesc: PCppExceptDesc); forward;
function CppGetBase (var Obj: Pointer; TypeDesc: PCppTypeId;
  BaseName: PAnsiChar): Boolean; forward;


constructor ECppException.CreateTypeNamed (_TypeName: PAnsiChar; ExcDesc: Pointer);
begin
  inherited CreateFmt (SCppExceptionMsg, [_TypeName]);
  FTypeName := _TypeName;
  FExcDesc := ExcDesc;
end;

constructor ECppStdException.Create (AExcObj: PCppStdException; Msg: String;
  _TypeName: PAnsiChar; ExcDesc: Pointer);
begin
{$IFDEF DEBUG}
  inherited CreateFmt ('[%s] %s', [_TypeName, Msg]);
{$ELSE}
  inherited Create (Msg);
{$ENDIF}
  FExcObj := AExcObj;
  FTypeName := _TypeName;
  FExcDesc := ExcDesc;
end;

function ECppException.IsCppClass: Boolean;
var
  ExcDesc: PCppExceptDesc;
begin
  ExcDesc := PCppExceptDesc (FExcDesc);
  Result := (ExcDesc.xdTypeID.tpMask and TM_IS_CLASS) <> 0;
end;

function ECppException.GetCppExceptionObject: Pointer;
var
  ExcDesc: PCppExceptDesc;
begin
  ExcDesc := PCppExceptDesc (FExcDesc);
  Result := @ExcDesc.xdValue;
end;

function ECppException.GetThrowFile: AnsiString;
var
  ExcDesc: PCppExceptDesc;
begin
  ExcDesc := PCppExceptDesc (FExcDesc);
  if ExcDesc.xdThrowFile <> nil then
    Result := ExcDesc.xdThrowFile
  else
    Result := '';
end;

function ECppException.GetThrowLine: Integer;
var
  ExcDesc: PCppExceptDesc;
begin
  ExcDesc := PCppExceptDesc (FExcDesc);
  Result := ExcDesc.xdThrowLine;
end;

function ECppException.AsCppClass (CppClassName: AnsiString): Pointer;
var
  ExcDesc: PCppExceptDesc;
  Obj: Pointer;
begin
  ExcDesc := PCppExceptDesc (FExcDesc);
  Obj := @ExcDesc.xdValue;
  if (CppGetBase (Obj, ExcDesc.xdTypeID, PAnsiChar (CppClassName))) then
    Result := Obj
  else
    Result := nil;
end;

destructor ECppException.Destroy;
var
  ExcDesc: PCppExceptDesc;
begin
  ExcDesc := PCppExceptDesc (FExcDesc);

  { call the exception object's destructor and free the memory as it
    is done in _CatchCleanup() in xx.cpp. }
  ExceptionHandled (ExcDesc);

  { Free the memory taken up by the exception descriptor }
  ExcDesc.xdFreeFunc (ExcDesc);
end;


function ECppStdException.GetStdException: PCppStdException;
begin
  Result := FExcObj;
end;

{ This function should basically work like destThrownValue()/callDestructor()
  in xx.cpp }
procedure _DestroyThrownValue (Obj: Pointer; ObjType: PCppTypeId);
type
  TCdeclDestructor    = procedure (Obj: Pointer; Flags: Integer); cdecl;
  TPascalDestructor   = procedure (Flags: Integer; Obj: Pointer); pascal;
  TFastcallDestructor = procedure (Obj: Pointer; Flags: Integer);
  TStdcallDestructor  = procedure (Obj: Pointer; Flags: Integer); stdcall;
var
  Flags: Integer;
  CdeclDestructor: TCdeclDestructor;
  PascalDestructor: TPascalDestructor;
  FastcallDestructor: TFastcallDestructor;
  StdcallDestructor: TStdcallDestructor;
begin
  // 		callDestructor(objAddr, objType, 0, dtorAddr, dtorMask, 1);
  Flags := 2;

  Assert ((ObjType.tpcDtorMask and $0080) = 0, 'fastthis (-po) not supported');

  case ObjType.tpcDtorMask and $0007 of
  1: // __cdecl
    begin
      CdeclDestructor := TCdeclDestructor (ObjType.tpcDtorAddr);
      CdeclDestructor (Obj, Flags);
    end;
  2: // __pascal
    begin
      PascalDestructor := TPascalDestructor (ObjType.tpcDtorAddr);
      PascalDestructor (Flags, Obj);
    end;
  3: // __fastcall
    begin
      FastcallDestructor := TFastcallDestructor (ObjType.tpcDtorAddr);
      FastcallDestructor (Obj, Flags);
    end;
  5: // __stdcall
    begin
      StdcallDestructor := TStdcallDestructor (ObjType.tpcDtorAddr);
      StdcallDestructor (Obj, Flags);
    end;
  else
    Assert (False, 'Unsupported calling convention!');
  end;
end;

{ This function should basically work like exceptionHandled() in xx.cpp }
procedure ExceptionHandled (ExcDesc: PCppExceptDesc);
{type
  T = Integer;}
var
  TypeDesc: PCppTypeId;
  Obj: Pointer;
begin
  { Is the thrown value still present in the descriptor? }
  if Ord (ExcDesc.xdArgBuff) <> 0 then
  begin

    { Destroy the thrown value if necessary }
    if (ExcDesc.xdCflg and CF_HAS_DTOR) <> 0 then
    begin
      TypeDesc := ExcDesc.xdBase;
      Obj := Pointer (@ExcDesc.xdValue);

      { All delphi class objects are thrown by pointer, sort of.
        However, we should not meet a Delphi class here! }
      if (TypeDesc.tpcFlags and CF_DELPHICLASS) <> 0 then
        Obj := Pointer ((PCardinal (Obj))^); { dereference }

      { We can't do anything about the _DestructorCount variable here, but
        this is a legacy feature anyway. }

      { Don't destroy it if it's a delphi class and it's being rethrown. }
      if (ExcDesc.xdFlags and (XDF_ISDELPHIEXCEPTION or XDF_RETHROWN))
        <> (XDF_ISDELPHIEXCEPTION or XDF_RETHROWN) then
        _DestroyThrownValue (Obj, TypeDesc);
    end;

    { Mark the fact that the arg is gone }
    ExcDesc.xdArgBuff := AnsiChar (0);
  end;

  { Did we make a copy of the argument? }
  if Ord (ExcDesc.xdArgCopy) <> 0 then
  begin
    TypeDesc := ExcDesc.xdArgType;

    { Destroy the thrown value if necessary }
    if ((TypeDesc.tpMask and TM_IS_CLASS) <> 0)
      and ((TypeDesc.tpcFlags and CF_HAS_DTOR) <> 0) then
      _DestroyThrownValue (ExcDesc.xdArgAddr, TypeDesc);

    { Mark the fact that the arg is gone }
    ExcDesc.xdArgCopy := AnsiChar (0);
  end;
end;

{ It is not easy to get the C++ type description for std::exception,
  therefore we identify the base class by name.

  This function should basically work like locateBaseClass() in xxtype.cpp }
function _LocateCppBaseClass (BaseList: PCppBaseList; VBase: Boolean;
  BaseName: PAnsiChar; var Addr: Pointer) : Boolean;
var
  Ptr: Pointer;
  BaseBaseList: PCppBaseList;
  BaseType: PCppTypeId;
begin
  { Check for end of base list }
  Result := False;
  while BaseList.blType <> nil do
  begin
    BaseType := BaseList.blType;

    Ptr := Pointer (PByte (Addr) + BaseList.blOffs);
    Inc (BaseList);

    if VBase then
      Ptr := Pointer ((PCardinal (Ptr))^); { dereference }

    { Is this the right base class? }
    if StrComp (PAnsiChar (PByte (BaseType) + BaseType.tpName),
                BaseName) = 0 then
    begin
      Addr := Ptr;    { Match --> return the adjusted pointer to the caller }
      Result := True;
      Exit;
    end;

    { Does this base class have any base classes? }
    { Annotation: Would BaseType.tpcFlags & CF_HAS_BASES be a better match for this? }
    if (BaseType.tpMask and TM_IS_CLASS) = 0 then
      Continue;

    { Get the list of non-virtual bases for this base class }
    BaseBaseList := PCppBaseList (PByte (BaseType) + BaseType.tpcBaseList);

    { Give up on this base if it has no non-virtual bases (Ann.: why?) }
    if BaseBaseList = nil then
      Continue;

    { Search the base classes of this base recursively }
    if _LocateCppBaseClass (BaseBaseList, False, BaseName, Ptr) then
    begin
      Addr := Ptr;    { Match --> return the adjusted pointer to the caller }
      Result := True;
      Exit;
    end;
  end;
end;

function CppGetBase (var Obj: Pointer; TypeDesc: PCppTypeId;
  BaseName: PAnsiChar) : Boolean;
var
  BaseList, VBaseList: PCppBaseList;
begin
  if StrComp (PAnsiChar (PByte (TypeDesc) + TypeDesc.tpName), BaseName) = 0 then
    { a class can be considered its own base }
    Result := True
  else if (TypeDesc.tpMask and TM_IS_CLASS) <> 0 then
  begin
    { iterate through the base classes }
    BaseList  := PCppBaseList (PByte (TypeDesc) + TypeDesc.tpcBaseList);
    VBaseList := PCppBaseList (PByte (TypeDesc) + TypeDesc.tpcVbasList);
    Result := _LocateCppBaseClass (BaseList,  False, BaseName, Obj)
           or _LocateCppBaseClass (VBaseList, False, BaseName, Obj);
  end
  else
    Result := False; { Don't marvel. C++ permits to throw _every_ type. } 
end;

function CppExceptObjProc (P : PExceptionRecord) : Exception;
type
  { Function pointer to std::type_info::what().
    In the Dinkumware library, std::exception::what() always has the
    __cdecl calling convention. }
  TCppTypeInfoWhatMethod = function (This: Pointer): PAnsiChar; cdecl;
var
  ExcTypeName: PAnsiChar;
  ExcDesc: PCppExceptDesc;
  ExcObject: Pointer;
  ExcObjectVTbl: Pointer;

  WhatMethod: TCppTypeInfoWhatMethod;
begin
  case P.ExceptionCode of
  $0EEFFACE: { C++ exception }
    begin
      { When a C++ exception is thrown, the C++ compiler (indirectly) calls the
        tossAnException function defined in xx.cpp. The Win32 exception argument
        table is filled in line 1096ff as follows:

          argTable[0] = (unsigned long)(void __far *)__throwExceptionName;
          argTable[1] = (unsigned long)(void __far *)throwPC;
          argTable[2] = (unsigned long)(void __far *)xdp; // : PCppExceptDesc }

      ExcTypeName := PAnsiChar (P.ExceptionInformation[0]);
      ExcDesc := PCppExceptDesc (P.ExceptionInformation[2]);
      ExcObject := Pointer (@ExcDesc.xdValue);

      if CppGetBase (ExcObject, ExcDesc.xdTypeID, 'std::exception') then
      begin
        { The exception object is a std::exception derivate and implements
          the virtual member function what(). }
  
        ExcObjectVTbl := Pointer (PCardinal (ExcObject)^);
        WhatMethod := TCppTypeInfoWhatMethod (PCardinal (
          Cardinal (ExcObjectVTbl) + $04)^);
        Result := ECppStdException.Create (ExcObject, String (WhatMethod (ExcObject)),
          PAnsiChar (ExcTypeName), Pointer (ExcDesc));
      end
      else
        { The exception object has some other type. We cannot extract an
          exception message with reasonable efforts. }
        Result := ECppException.CreateTypeNamed (PAnsiChar (ExcTypeName),
          Pointer (ExcDesc));
    end;

  else { some other uncaught exception }
    if Assigned (OldExceptObjProc) then
      Result := OldExceptObjProc (P)
    else
      Result := nil;
  end;
end;


procedure InstallCppExceptionFilter;
begin
  OldExceptObjProc := TExceptObjProc (System.ExceptObjProc);
  System.ExceptObjProc := Pointer (@CppExceptObjProc);
end;

procedure UninstallCppExceptionFilter;
begin
  System.ExceptObjProc := Pointer (@OldExceptObjProc);
end;

{$IFNDEF SYSTEMCPPEXCEPTION_NO_IMPLICIT_INSTALL}
initialization
  InstallCppExceptionFilter;

finalization
  UninstallCppExceptionFilter;
{$ENDIF SYSTEMCPPEXCEPTION_NO_IMPLICIT_INSTALL}



{ Copyright (c) 2008, 2010 Moritz Beutel. All rights reserved.

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following
conditions are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS “AS IS” AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. }

end.

