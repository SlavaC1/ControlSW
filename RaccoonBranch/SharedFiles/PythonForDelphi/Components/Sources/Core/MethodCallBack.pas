(**************************************************************************)
(*                                                                        *)
(* Module:  Unit 'MethodCallback'      Copyright (c) 1998                 *)
(*                                                                        *)
(* Version: 0.0                        Dr. Dietmar Budelsky               *)
(* Sub-Version: 0.2                    dbudelsky@web.de                   *)
(*                                     Germany                            *)
(*                                                                        *)
(**************************************************************************)
(*  Functionality: Generates synthetic callback functions which calls     *)
(*  DELPHI Class Methods. A callback mechanism (DDE, PYTHON, TCL) can now *)
(*  use DELPHI objects.                                                   *)
(*                                                                        *)
(**************************************************************************)
(*  Contributors:                                                         *)
(*      Grzegorz Makarewicz (mak@mikroplan.com.pl)                        *)
(*      Morgan Martinet     (mmm@free.fr)                                 *)
(**************************************************************************)
(* This source code is distributed with no WARRANTY, for no reason or use.*)
(* Everyone is allowed to use and change this code free, as long as this  *)
(* header and its copyright text is intact.                               *)
(* Dr. Dietmar Budelsky, 1998-01-07                                       *)
(**************************************************************************)
unit MethodCallBack;

{$I Definition.Inc}

interface
uses SysUtils;

type
  TCallType = (ctSTDCALL, ctCDECL);
  TCallBack = procedure of object;
  TDDEAPIfunc = function( CallType, Fmt: Integer; Conv: longint;
                          hsz1, hsz2: longint;
                          Data: longint; Data1, Data2: integer):
                          longint of object; stdcall;
// Method declaration for DDE(ML) interface. Callbackmethods for DDE(ML) have
// to be declared according to this.

function GetDDECallBack(method: TDDEAPIfunc): Pointer;
// Call for example with
// GetDDECallBack(DDECallBackMethod);

function  GetCallBack( self: TObject; method: Pointer;
                       argnum: Integer; calltype: tcalltype): Pointer;
// Call for example with
// CallBackProc := GetCallBack( self, @TSelfObject.Method, 2, ctSTDCALL);
//
// "self" is a valid TSelfObject,
// "Method" is a pointer to the class method, which should be triggered,
// when CallBackProc is called. It has to be declared according to the
// calltype!
// argnum is the number of callback parameters. There are the following
// exceptions: Double and Currency count for two. (sure)
//             Float counts for two               (not tested yet)
//             Extended counts for three          (not tested yet)
//             Records count for SizeOf(record)/4 rounded up.
// calltype is the calling convention of the callback function.

function  GetOfObjectCallBack( CallBack: TCallBack;
                               argnum: Integer; calltype: TCallType): Pointer;
// More sophisticated interface for standardized callback mechanisms.
// Usage for example:
// type
// TMyCallBack = function(x: Integer):Integer of object; cdecl;
// TMyClass = Class
//   CallBackProc: Pointer;
//   function y(x: Integer):Integer; cdecl;
//   procedure Init;
// end;
// ...
// function SetCallBack(f: TMyCallBack): Pointer;
// begin
//   result := GetOfObjectCallBack( TCallBack(f), 1, ctCDECL);
// end;
// procedure TMyClass.Init;
// begin
//   CallBackProc := SetCallBack(y);
// end;

procedure DeleteCallBack( Proc: Pointer );
// frees the memory used for Proc. Call with
// DeleteCallBack( CallBackProc);

implementation

uses Classes;
type
  PByte = ^Byte;
var
  CallBackList: TList;

function GetDDECallBack(method: TDDEAPIfunc): Pointer;
begin
  result := GetOfObjectCallBack(TCallBack(method),8,ctSTDCALL);
end;

function  GetOfObjectCallBack( CallBack: TCallBack;
                               argnum: Integer; calltype: TCallType): Pointer;
begin
  result := GetCallBack( TMethod(CallBack).Data,
                         TMethod(CallBack).Code,
                         argnum, calltype);
end;

function  GetCallBack( self: TObject; method: Pointer;
                       argnum: Integer; calltype: tcalltype): Pointer;
const
// Short handling of stdcalls:
S1: array [0..14] of byte = (
$5A,            //00  pop  edx  // pop return address
$B8,0,0,0,0,    //01  mov  eax, self
$50,            //06  push eax
$52,            //07  push edx // now push return address
// call the real callback
$B8,0,0,0,0,    //08  mov  eax, Method
$FF,$E0);       //13  jmp  eax

//Handling for ctCDECL:
C1: array [0..2] of byte = (
// begin of call
$55,            //00      push ebp
$8B,$EC);       //01      mov  ebp, esp

// push arguments
//  for i:= argnum-1 downto 0 do begin
C2: array [0..3] of byte = (
$8B,$45,0,      //03+4*s  mov eax,[ebp+8+4*i]
$50);           //06+4*s  push eax
//  end;

// self parameter
C3: array [0..17] of byte = (
$B8,0,0,0,0,    //03+4*s  mov eax, self
$50,            //08+4*s  push eax
// call the real callback
$B8,0,0,0,0,    //09+4*s  mov  eax,Method
$FF,$D0,        //14+4*s  call eax
// clear stack
$83,$C4,0,      //16+4*s  add esp, 4+bytes
$5D,            //19+4*s  pop  ebp
$C3);           //20+4*s  ret
var
  bytes: Word;
  i: Integer;
  P,Q: PByte;
begin
  if calltype = ctSTDCALL then begin
    GetMem(Q,15);
    CallBackList.Add(Q);
    P := Q;
    move(S1,P^,SizeOf(S1));
    Inc(P,2);
    move(self,P^,SizeOf(self));
    Inc(P,7);
    move(method,P^,SizeOf(method));
    {Inc(P,6); End of proc}
  end else begin  {ctCDECL}
    bytes := argnum * 4;
    GetMem(Q,21+4*argnum);
    CallBackList.Add(Q);
    P := Q;
    move(C1,P^,SizeOf(C1));
    Inc(P,SizeOf(C1));
    for i:=argnum-1 downto 0 do begin
      move(C2,P^,SizeOf(C2));
      Inc(P,2);
      P^:=8+4*i;
      Inc(P,2);
    end;
    move(C3,P^,SizeOf(C3));
    Inc(P,1);
    move(self,P^,SizeOf(self));
    Inc(P,6);
    move(method,P^,SizeOf(method));
    Inc(P,8);
    P^ := 4+bytes;
    {Inc(P,3); End of proc}
  end;
  result := Q;
end;

procedure DeleteCallBack( Proc: Pointer);
begin
  if CallBackList.Remove(Proc) <> -1 then
    FreeMem(Proc);
end;

procedure FreeCallBacks;
var i: Integer;
    P: PByte;
begin
  with CallBackList do begin
    for i := 0 to Count - 1 do begin
      P :=  Items[i];
      if P <> nil then FreeMem(P);
    end;
    Clear;
    Free;
  end;
end;

initialization
  CallBackList := TList.Create;
finalization
  FreeCallBacks;
end.
