{$A8,B-,C+,D+,E-,F-,G+,H+,I+,J+,K-,L+,M+,N+,O-,P+,Q-,R-,S-,T-,U-,V+,W-,X+,Y+,Z1}

{$ifdef VER150}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$endif}



unit MicDelphiAgent;

interface

implementation
uses  Windows,
      Registry,
      SysUtils,
      DlphVDEPAgentImpl;


function GetGenericAgentPath:String;
var Registry: TRegistry;
begin
  Result:= '';
  Registry := TRegistry.Create(KEY_READ);
  try
    Registry.RootKey:= HKEY_CURRENT_USER;
    // False because we do not want to create it if it doesn't exist
    Registry.OpenKey('Software\Mercury Interactive\QuickTest Professional\MicTest\Packages\DelphiPackage\Settings', False);
    Result := Registry.ReadString('GenericAgentPath');
  finally
    Registry.Free;
  end;
end;

var
   g_hAgent: DWORD;
   g_hMMF:DWORD;


type
      TSetPrecompiledAgentCallbacks = procedure(
                PGetProperty: Pointer;
                PSetProperty: Pointer;
                PGetAllPropertyNames: Pointer;
                PGetAoData: Pointer;
                PResetAO: Pointer;
                PCustomAgentService: Pointer ); stdcall;



procedure SetBindingInfo;
const
   BindingInfoMMFFmt: string = 'HPQTPDELPHIAGENTBINDINGINFO_95C8BC62-8C32-4409-AB2E-CB0654106898_%x';
type
      TBindingInfo = record
          PGetProperty: Pointer;
          PSetProperty: Pointer;
          PGetAllPropertyNames: Pointer;
          PGetAoData: Pointer;
          PResetAO: Pointer;
          PCustomAgentService: Pointer
      end;

      PTBindingInfo = ^TBindingInfo;
var
    MMFName: string;
    ProcessId:DWORD;
    pvMMF: PTBindingInfo;
begin
  ProcessId:=GetCurrentProcessId();
  FmtStr(MMFName, BindingInfoMMFFmt, [ProcessId]);

  pvMMF:= nil;
  try
    g_hMMF:= CreateFileMapping(INVALID_HANDLE_VALUE, nil, PAGE_READWRITE,	0, sizeof(TBindingInfo), PAnsiChar(MMFName));
    if g_hMMF <> 0 then
    begin
      pvMMF:= MapViewOfFile(g_hMMF, FILE_MAP_WRITE, 0, 0, 0);
      if pvMMF<> nil then
      begin
        pvMMF^.PGetProperty:= @GetProperty;
        pvMMF^.PSetProperty:= @SetProperty;
        pvMMF^.PGetAllPropertyNames:= @GetAllPropertyNames;
        pvMMF^.PGetAoData:= @GetAoData;
        pvMMF^.PResetAO:= @ResetAO;
        pvMMF^.PCustomAgentService:= @CustomAgentService;
      end;
    end;
  finally
    if pvMMF <> nil then
      UnMapViewOfFile(pvMMF);
  end;
end;


procedure BindToGenericAgent;
var
    AgentPath: String;
    SetCallBacks: TSetPrecompiledAgentCallbacks;
begin
    g_hAgent:= 0;
    AgentPath:= GetGenericAgentPath + '\QTPDelphiAgent.dll';
    g_hAgent:= LoadLibrary(PAnsiChar(AgentPath));
    if g_hAgent = 0 then exit;

    SetCallBacks:= TSetPrecompiledAgentCallbacks(GetProcAddress(g_hAgent, 'SetPrecompiledAgentCallbacks'));
    if @SetCallBacks = nil then exit;

    SetCallBacks(@GetProperty, @SetProperty,
                 @GetAllPropertyNames, @GetAoData,
                 @ResetAO, @CustomAgentService);
end;

procedure FreeBindingInfo;
begin
  if g_hMMF <> 0 then
    CloseHandle(g_hMMF);
end;

procedure ReleaseGenericAgent;
begin
  FreeLibrary(g_hAgent);
  g_hAgent:= 0;
end;

initialization
//  BindToGenericAgent;
    SetBindingInfo;
finalization
//  ReleaseGenericAgent;
    FreeBindingInfo;

end.
