{$A8,B-,C+,D+,E-,F-,G+,H+,I+,J+,K-,L+,M+,N+,O-,P+,Q-,R-,S-,T-,U-,V+,W-,X+,Y+,Z1}
{$ifdef VER150}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$endif}

unit DlphVDEPAgentImpl;

interface

uses
  ActiveX,
  Math,
  Windows;

function GetProperty(hWnd:Integer; PropName: PWideChar; saParams: PSafeArray;  var vtRetVal: OleVariant): HRESULT;  stdcall;
function SetProperty(hWnd:Integer; PropName: PWideChar; saParams: PSafeArray;  vtVal: OleVariant): HRESULT; stdcall;
function GetAllPropertyNames(hWnd: Integer; var psaPropNames: PSafeArray): HRESULT; stdcall;
function GetAOData(hWnd: Integer; var psaData: PSafeArray): HRESULT; stdcall;
function ResetAO(hWnd: Integer): HRESULT; stdcall;
function CustomAgentService(szDllPath: PWideChar; szMethod: PWideChar; vtParamIn: OleVariant; var vtParamOut: OleVariant): HRESULT; stdcall;
function _PackRecordLine(Cmd: String; Params: array of OleVariant; SendMode: Integer): OleVariant;
function findProcessControl( wnd: HWND ): TObject;

implementation

uses
  Controls,
  SysUtils,
  StrUtils,
  Variants,
  Forms,
  MicAOFactoryMgr;

function findProcessControl( wnd: HWND ): TObject;
const
  TAppClass:string= 'TApplication';
var
  atom: TAtom;
  AtomText: array[0..31] of Char;
  ClassName: array [0..32] of Char;
begin
  Assert( wnd <> 0 );
  atom := GlobalAddAtom(StrFmt(AtomText, 'Delphi%.8X',
            [GetCurrentProcessID]));
  try
    result := Pointer( GetProp( wnd, MakeIntAtom(atom )));
  finally
    GlobalDeleteAtom( atom );
  end;

  if result = nil then
  begin
    ClassName[0]:= #0;
    GetClassName(Wnd, ClassName, 32);
    if TAppClass = ClassName then
       Result:= Application;
  end
end;

procedure FillWrapperParameters(saParams: PSafeArray; var AOWrp: TMicAOWrp);
var
    ix : array [0..1] of Integer;
    i, Count: Integer;
begin
    if saParams = Nil then
    begin
       AOWrp.FParam.NumOfParams:= 0;
       exit;
    end;

    SafeArrayLock(saParams);
    ix[0]:=0;

    Count:= saParams^.rgsabound[0].cElements;
    for i:=0 to Count - 1 do
    begin
        ix[1]:=i;
        SafeArrayGetElement(saParams, ix, AOWrp.FParam.Parameters[i]);
    end;
    SafeArrayUnlock(saParams);
    AOWrp.FParam.NumOfParams := Count;
end;


function GetProperty(hWnd:Integer; PropName: PWideChar; saParams: PSafeArray;  var vtRetVal: OleVariant): HRESULT;  stdcall;
var Control: TObject;
    AOWrp: TMicAOWrp;
begin
  Result:= E_FAIL;
  Control:= findProcessControl(hWnd);
  if Control <> nil then
  begin
     GetAOFactoryMgr.CreateAOFromHandle(Control, AOWrp);
     if AOWrp = nil then exit;
     FillWrapperParameters(saParams, AOWrp);
     try
        Result:= AOWrp.GetProperty(WideCharToString(PropName), vtRetVal);
     except
        On E : EInvalidArgument do
           Result := E_INVALIDARG;
     end;
  end
  else
    Result:= E_FAIL;
end;


function SetProperty(hWnd:Integer; PropName: PWideChar; saParams: PSafeArray; vtVal: OleVariant): HRESULT; stdcall;
var Control: TObject;
    AOWrp: TMicAOWrp;
    StrParamName: String;
begin
  Result:= E_FAIL;
  Control:= findProcessControl(hWnd);
  if Control <> nil then
  begin
    try
     GetAOFactoryMgr.CreateAOFromHandle(Control, AOWrp);
     if AOWrp = nil then exit;
     if PropName = nil then
     begin
       FillWrapperParameters(saParams, AOWrp);
       StrParamName:= AOWrp.FParam.Parameters[AOWrp.FParam.NumOfParams - 1];
       Dec(AOWrp.FParam.NumOfParams);
       Result:= AOWrp.SetProperty(StrParamName, vtVal);
     end
     else
       Result:= AOWrp.SetProperty(WideCharToString(PropName), vtVal);
    except
      On E : EInvalidArgument do
        Result := E_INVALIDARG;
    end
  end
  else
    Result:= E_FAIL;
end;

function GetAllPropertyNames(hWnd: Integer; var psaPropNames: PSafeArray): HRESULT; stdcall;
var Control: TObject;
    AOWrp: TMicAOWrp;
    Count, idx, idx2: Integer;
    PropNames: String;

    rgsabound: array [0..1] of SAFEARRAYBOUND;
    Value: OleVariant;
    ix : array [0..1] of Integer;

function PosEx(const SubStr, S: string; Offset: Cardinal = 1): Integer;
var
  I,X: Integer;
  Len, LenSubStr: Integer;
begin
  if Offset = 1 then
    Result := Pos(SubStr, S)
  else
  begin
    I := Offset;
    LenSubStr := Length(SubStr);
    Len := Length(S) - LenSubStr + 1;
    while I <= Len do
    begin
      if S[I] = SubStr[1] then
      begin
        X := 1;
        while (X < LenSubStr) and (S[I + X] = SubStr[X + 1]) do
          Inc(X);
        if (X = LenSubStr) then
        begin
          Result := I;
          exit;
        end;
      end;
      Inc(I);
    end;
    Result := 0;
  end;
end;

begin
  Result:= E_FAIL;
  Control:= findProcessControl(hWnd);
  if Control <> nil then
  begin
     GetAOFactoryMgr.CreateAOFromHandle(Control, AOWrp);
     if AOWrp = nil then exit;
     Result:= AOWrp.GetAllPropertyNames(PropNames, Count);

     if Result <> S_OK then exit;
     
     rgsabound[1].lLbound:= 0;
     rgsabound[1].cElements:= Count;
     rgsabound[0].lLbound:= 0;
     rgsabound[0].cElements:= 2;
     psaPropNames:= SafeArrayCreate( varVariant, 2, rgsabound);
     SafeArrayLock(psaPropNames);

     idx:= 1;
     ix[0]:= 0; ix[1]:= 0;
     while (ix[1] < Count) and (idx > 0) do
     begin
       idx2:= PosEx(';', PropNames, idx);
       if idx2 = 0 then
          Value:= Copy(PropNames,idx,Length(PropNames) - idx) // Handle last item
       else
         Value:= Copy(PropNames,idx,idx2-idx);

       ix[0]:= 0;
       SafeArrayPutElement(psaPropNames, ix, Value);
       ix[0]:= 1; Value:= False;
       SafeArrayPutElement(psaPropNames, ix, Value);


       idx:= idx2 + 1;
       Inc(ix[1]);
     end;

     SafeArrayUnLock(psaPropNames);
  end
  else
    Result:= E_FAIL;
end;

function GetAOData(hWnd: Integer; var psaData: PSafeArray): HRESULT; stdcall;
var pControl: TObject;
begin
  pControl:= findProcessControl(hWnd);
  if pControl <> nil then
  begin

     Result:= S_OK;
  end
  else
    Result:= E_FAIL;
end;

function ResetAO(hWnd: Integer): HRESULT; stdcall;
begin
   Result:= S_OK;
end;

function CustomAgentService(szDllPath: PWideChar; szMethod: PWideChar; vtParamIn: OleVariant; var vtParamOut: OleVariant): HRESULT; stdcall;
begin
   Result:= S_OK;
end;

function _PackRecordLine(Cmd: String; Params: array of OleVariant; SendMode: Integer): OleVariant;
type OleVariantArray  = array of OleVariant;

var i, SaSize: Integer;
    LineContent: PSafeArray;
    rgsabound: array [0..1] of SAFEARRAYBOUND;
    Value: OleVariant;
    ix : array [0..1] of Integer;
begin
  SaSize:= 3 + High(Params) - Low(Params);

  rgsabound[1].lLbound:= 0;
  rgsabound[1].cElements:= SaSize;
  rgsabound[0].lLbound:= 0;
  rgsabound[0].cElements:= 1;
  LineContent:= SafeArrayCreate( varVariant, 2, rgsabound);
  SafeArrayLock(LineContent);

  ix[0]:=0;
  Value:= SendMode; ix[1]:= 0;
  SafeArrayPutElement(LineContent, ix, Value);

  Value:= Cmd; ix[1]:= 1;
  SafeArrayPutElement(LineContent, ix, Value);

  // Add parameters
  for i:= Low(Params) to High(Params) do
  begin
    ix[1]:= i + 2 - Low(Params);
    SafeArrayPutElement(LineContent, ix, Params[i]);
  end;

  SafeArrayUnLock(LineContent);
  TVarData(Result).VType  := varVariant or varArray;
  TVarData(Result).VArray := PVarArray(LineContent);
end;


end.
