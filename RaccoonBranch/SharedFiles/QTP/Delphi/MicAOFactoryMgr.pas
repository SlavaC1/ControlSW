{$A8,B-,C+,D+,E-,F-,G+,H+,I+,J+,K-,L+,M+,N+,O-,P+,Q-,R-,S-,T-,U-,V+,W-,X+,Y+,Z1}
{$ifdef VER150}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$endif}

unit MicAOFactoryMgr;

interface
uses
  SysUtils, Windows, Messages, TypInfo, Menus,
  Forms, Controls, Graphics, ComCtrls, CheckLst,
  AgentExtensibilitySDK,
  MicAOFactory,
  MicDelphiGridSupport;

  const
   MAX_PARAM=32;
   MAX_EXT_FACTORIES=1024;

type
  TMicAOWrp = class
     public
        FObject:  TObject;
        FAO: TMicAO;
        FParam: TServiceParam;

        procedure Init(obj: TObject; AO: TMicAO);
        function  GetProperty(propname:string; var value: OleVariant): HRESULT;
        function  SetProperty(propname:string; value: Variant): HRESULT;
        function  GetAllPropertyNames(var PropNames: String; var Count: Integer): HRESULT; overload;
        function  GetAllPropertyNames(Container: string; var PropNames: String; var Count: Integer): HRESULT; overload;

     protected
        function  GetObjectProperty(obj:TObject; propname:string; var value: OleVariant): HRESULT;
        function  SetObjectProperty(obj:TObject; propname:string; value: Variant): HRESULT;
        function  GetObjectAllPropertyNames(Obj:TObject; var PropNames: String; var Count: Integer): HRESULT;  overload;
        function  GetObjectAllPropertyNames(obj:TObject; Container: string; var PropNames:String; var Count: Integer): HRESULT; overload;
        function  GetObjectPropertyType(obj:TObject;propname:string): TVarType;

     private
        function  IsBitOn (Value: Integer; Bit: Byte): Boolean;
        function  GetValOfSet(obj:TObject;PropInfo: PPropInfo):String;
  end;


  TExtensibilityFactory=record
    hLib:Integer;
    szLibPath: String;
    VCLObj2AO: TCreateAOFromHandle
 end;


  TAOFactoryMgr=class
      public
        Constructor Create;
        Destructor Destroy; override;
        procedure CreateAOFromHandle(obj:TObject; var AgentObject :TMicAOWrp);
        procedure AddExtensibilityServer(szLibPath: String); overload;
        procedure AddExtensibilityServer(VCLObj2AO: TCreateAOFromHandle); overload;
      private
         ExtensibilityFactories: array [0..MAX_EXT_FACTORIES] of TExtensibilityFactory;
         FactoriesCount: Integer;

  end;

  function GetAOFactoryMgr:TAOFactoryMgr;


implementation


////////////////////////////////////////////////////////////////
// TMicAOWrp
//////////////////////////////////////////////////////////////// 
procedure TMicAOWrp.Init(obj: TObject; AO: TMIcAO);
begin
     FObject:=obj;
     FAO:= AO;
     FAO.Init(obj, FParam);
end;


function TMicAOWrp.IsBitOn (Value: Integer; Bit: Byte): Boolean;
begin
  Result := (Value and (1 shl Bit)) <> 0;
end;


///  Get value of Set type
function TMicAOWrp.GetValOfSet(obj:TObject;PropInfo: PPropInfo):String;
var
  TypeInfo: PTypeInfo;
  TypeData: PTypeData;
  W: Cardinal;
  i:integer;
begin
   Result :='';
   TypeData := GetTypeData(PropInfo.PropType^);
   if TypeData = nil then exit;

   TypeInfo := TypeData.CompType^;
   if TypeInfo = nil then exit;

   W := GetOrdProp(obj, PropInfo);

   for i:= GetTypeData (TypeInfo).MinValue to GetTypeData (TypeInfo).MaxValue do
     if IsBitOn (W, i) then
     begin
        if Length(Result) > 0 then
          Result := Result + '|' + GetEnumName (TypeInfo, i)
        else
          Result :=  GetEnumName (TypeInfo, i);
     end;
end;

function  TMicAOWrp.GetObjectProperty(obj:TObject;propname:string; var value: OleVariant): HRESULT;

function My_GetEnumName(TypeInfo: PTypeInfo; Value: Integer): string;
const
  BooleanIdents: array [Boolean] of string = ('False', 'True');
var
  P: ^ShortString;
  T: PTypeData;
begin
  if TypeInfo^.Kind = tkInteger then
  begin
    Result := IntToStr(Value);
    Exit;
  end;
  T := GetTypeData(GetTypeData(TypeInfo)^.BaseType^);
  if (TypeInfo = System.TypeInfo(Boolean)) or (T^.MinValue < 0) then
  begin
    if TypeInfo = System.TypeInfo(Boolean) then
      Result := BooleanIdents[Boolean(Value <> 0)]
    else                                         // LongBool/WordBool/ByteBool
      Result := BooleanIdents[Ord(Value) <> 0];  // map non-zero to true
  end
  else
  begin
    P := @T^.NameList;
    while Value <> 0 do
    begin
      Inc(Integer(P), Length(P^) + 1);
      Dec(Value);
    end;
    Result := P^;
  end;
end;

var
PropInfo: PPropInfo;
ObjSub:TObject;
SubAOWrp: TMicAOWrp;
PropSubName:string;
begin
    Result := E_FAIL;
    PropSubName:= '';
    ParseSubProperty( PropName, PropName, PropSubName);

    PropInfo:= GetPropInfo(obj.ClassInfo,PropName);

    if (PropInfo = nil) then
        begin
             ObjSub:=FAO.GetSubObjectEx(propname, PropSubName);
             if (ObjSub = nil) then
             begin
               Result:= FAO.GetPropertyEx(propname, value);
               exit;
             end;

             GetAOFactoryMgr.CreateAOFromHandle(ObjSub, SubAOWrp);
             if SubAOWrp <> nil then
             begin
               SubAOWrp.FParam:= FParam;
               Result :=SubAOWrp.GetProperty(PropSubName, value);
             end;
             exit;
        end;

    case PropInfo^.PropType^.Kind of
        tkInteger:
	          Value := GetOrdProp(Obj, PropInfo);
        tkEnumeration:
    	      Value :=
	            {$ifdef ver90}
    	        GetEnumName(PropInfo^.PropType,GetOrdProp(Obj, PropInfo));
            	{$else}
		          My_GetEnumName((PTypeinfo(PropInfo^.Proptype^)),GetOrdProp(Obj, PropInfo));
        	    {$endif}
	      tkFloat:
	          Value := GetFloatProp(Obj, PropInfo);
	      tkString,
	      tkLString:
            Value :=  GetStrProp(Obj, PropInfo);
 	      tkSet:
            if Length(PropSubName) = 0 then
            	Value:= GetValOfSet(obj,PropInfo)
            else
              Value:= Pos(UpperCase(PropSubName), UpperCase(GetValOfSet(obj,PropInfo))) <> 0;
        tkClass:
            begin
	              ObjSub:= TObject(GetOrdProp(Obj, PropInfo));
                SubAOWrp:= nil;
                if ObjSub <> nil then
                GetAOFactoryMgr.CreateAOFromHandle(ObjSub, SubAOWrp);
                if SubAOWrp <> nil then
                begin
                  SubAOWrp.FParam:= FParam;
                  Result :=SubAOWrp.GetProperty(PropSubName, value);
                end
                else Result:= E_FAIL;
                exit;
            end;
        tkVariant:
             Value := GetVariantProp(Obj, PropInfo);
     end;
    // Set the success flag
   Result := S_OK;

end;


function TMicAOWrp.GetProperty(propname:string; var Value: OleVariant): HRESULT;
begin
  Result:= GetObjectProperty(FAO, propname, Value);
  if Result = S_OK then exit;

  Result :=GetObjectProperty(FObject, propname, Value);
end;

function  TMicAOWrp.SetObjectProperty(obj:TObject; propname:string; value: Variant): HRESULT;
var
PropInfo: PPropInfo;
idx:Integer;
PropSubName:string;
LongValue: Longint;
begin
    Result := E_FAIL;
    idx:=pos('.',propname);
    if (idx>0) then
    begin
       PropSubName:=Copy(propname,idx+1,Length(propname));
       Delete(propname,idx,Length(propname));
    end;
     PropInfo:=GetPropInfo(obj.ClassInfo,PropName);
     if (PropInfo= nil) then
        begin
            { ObjSub:=GetNotSupportObject(obj,propname);
             if (ObjSub=nil) then exit;
             Result :=GetObjectProperty(ObjSub,PropSubName);}
             exit;
        end;

    if PropInfo^.SetProc = nil then exit;

    Result:= S_OK;
    case PropInfo^.PropType^.Kind of
         tkString:  SetStrProp(obj,PropInfo,value);
         tkLString: SetStrProp(obj,PropInfo,value);
         tkWString: SetStrProp(obj,PropInfo,value);
         tkInteger,tkEnumeration:
         begin
           LongValue:=MicStrToInt(value);
           if (LongValue=ERR_INT) then
           begin
              Result:= E_FAIL;
           		exit;
           end;

           SetOrdProp(Obj, PropInfo, LongValue);
         end;
         else
         Result:= E_FAIL;
    end;
end;

function TMicAOWrp.SetProperty(propname:string; value:Variant): HRESULT;
begin
  Result:=SetObjectProperty(FAO,propname,value);
  if Result = S_OK then exit;
  Result :=SetObjectProperty(FObject,propname,value);
end;

function TMicAOWrp.GetObjectPropertyType(obj:TObject;propname:string): TVarType;
var
PropInfo: PPropInfo;
ObjSub:TObject;
idx:Integer;
PropSubName:string;
begin
   Result := 0;
   idx:=pos('.',propname);
    if (idx>0) then
    begin
       PropSubName:=Copy(propname,idx+1,Length(propname));
       Delete(propname,idx,Length(propname));
       PropInfo:=GetPropInfo(obj.ClassInfo,propname);
       if (PropInfo= nil) then  exit;
       if  (PropInfo^.PropType^.Kind<>tkClass) then exit;
       ObjSub:=TObject(GetOrdProp(Obj, PropInfo));
       Result := GetObjectPropertyType(ObjSub,PropSubName);
       exit;
    end;
     PropInfo:=GetPropInfo(obj.ClassInfo,PropName);
     if (PropInfo= nil) then exit;

     result:= TypeKind2VarType(PropInfo^.PropType^.Kind);
end;

function TMicAOWrp.GetAllPropertyNames(var PropNames: String; var Count: Integer): HRESULT;
begin
  Result:= GetAllPropertyNames('', PropNames, Count);
end;

function TMicAOWrp.GetAllPropertyNames(Container: string; var PropNames: String; var Count: Integer): HRESULT;
var
  Count1,Count2: Integer;
  s1,s2: String;
begin
  s2:='';
  Count1:=0;
  Count2:=0;

  Result:= GetObjectAllPropertyNames(FObject, Container, s1, Count1);
  if Result <> S_OK then exit;
  Result:= GetObjectAllPropertyNames(FAO, Container, s2, Count2);

  Count:= Count1 + Count2;
  PropNames:= s1 + s2;
end;

function  TMicAOWrp.GetObjectAllPropertyNames(obj:TObject; var PropNames:String; var Count: Integer): HRESULT;
begin
  Result:= GetObjectAllPropertyNames(obj, '', PropNames, Count);
end;

function  TMicAOWrp.GetObjectAllPropertyNames(obj:TObject; Container: string; var PropNames:String; var Count: Integer): HRESULT;

function SubObjProp(Container: string; prop: String):string;
begin
    if Length(Container) > 0 then
      Result:= Container+ '.' + prop
    else
      Result:= prop;
end;

var
  I: Integer;
  PropInfo: PPropInfo;
  TempList: PPropList;
  TypeInfo: PTypeInfo;
  PropCount: integer;
  SubObj: TObject;
  SubAOWrp: TMicAOWrp;
  SubPropList: String;
  SubPropCount: integer;
begin
  Result := S_OK;
  PropNames:= '';
  Count:= 0;

  TypeInfo:=obj.ClassInfo;
  if TypeInfo = nil then exit;
  PropCount := GetTypeData(TypeInfo)^.PropCount;
  if PropCount > 0 then
  begin
    GetMem(TempList, PropCount * SizeOf(Pointer));
    try
      GetPropInfos(TypeInfo, TempList);
      Count:= 0;
      for I := 0 to PropCount - 1 do
      begin
        PropInfo := TempList^[I];
        case PropInfo^.PropType^.Kind of
           tkInteger, tkEnumeration,
           tkFloat, tkString, tkLString, tkSet, tkVariant:
              begin
                PropNames:=PropNames + SubObjProp(Container, PropInfo.name) +';';
                inc(Count);
              end;
           tkClass:
              if UpperCase(PropInfo.name) <> 'PARENT' then
              begin
	              SubObj:= TObject(GetOrdProp(Obj, PropInfo));
                if SubObj <> nil then
                begin
                    SubAOWrp:= nil;
                    GetAOFactoryMgr.CreateAOFromHandle(SubObj, SubAOWrp);
                    if SubAOWrp <> nil then
                    begin
                      Result :=SubAOWrp.GetAllPropertyNames(SubObjProp(Container, PropInfo.name), SubPropList, SubPropCount);
                      if Result = S_OK then
                      begin
                        PropNames:= PropNames + SubPropList;
                        Count:= Count + SubPropCount;
                      end;
                    end;
                end
              end;
        end

      end;
    finally
      FreeMem(TempList, Count * SizeOf(Pointer));
    end;
  end;
end;



////////////////////////////////////////////////////////////////
// TAOFactoryMgr
////////////////////////////////////////////////////////////////
Constructor TAOFactoryMgr.Create;
begin
   FactoriesCount:= 0;
end;

Destructor TAOFactoryMgr.Destroy;
var i: integer;
begin
  for i:=0 to FactoriesCount-1 do
  begin
      FreeLibrary(ExtensibilityFactories[i].hLib);
      ExtensibilityFactories[i].hLib:= 0;
  end;
  FactoriesCount:= 0;

  inherited Destroy;
end;

procedure TAOFactoryMgr.CreateAOFromHandle(obj:TObject; var AgentObject :TMicAOWrp);
var i: integer;
    AO:TMicAO;
begin
  AO:=nil;
// Try create AO using external extensibility factories
  for i:=0 to FactoriesCount-1 do
    if ExtensibilityFactories[i].VCLObj2AO(obj, AO) = S_OK then
      if AO <> nil then
         break;

  if AO = nil then
      MicVCLObj2AO(obj, AO);  // Try create AO using built in AO factory

  if AO <> nil then
    begin
      AgentObject:= TMicAOWrp.Create;
      AgentObject.Init(obj, AO);
      exit;
    end;
end;

procedure TAOFactoryMgr.AddExtensibilityServer(szLibPath: string);
var  Factory: TExtensibilityFactory;
begin
   Factory.hLib:= LoadLibrary(PChar(szLibPath));
   if Factory.hLib = 0 then exit;
   Factory.VCLObj2AO:= GetProcAddress(Factory.hLib, 'VCLObj2AO');
   if @Factory.VCLObj2AO = nil then exit;
   Factory.szLibPath:= szLibPath;

   ExtensibilityFactories[FactoriesCount]:= Factory;
   Inc(FactoriesCount);
end;

procedure TAOFactoryMgr.AddExtensibilityServer(VCLObj2AO: TCreateAOFromHandle);
begin
   ExtensibilityFactories[FactoriesCount].hLib := 0;
   ExtensibilityFactories[FactoriesCount].szLibPath:= '';
   ExtensibilityFactories[FactoriesCount].VCLObj2AO := VCLObj2AO;
   Inc(FactoriesCount);
end;



var  g_AOFactoryMgr: TAOFactoryMgr;

function GetAOFactoryMgr:TAOFactoryMgr;
begin
    if g_AOFactoryMgr = nil then
       g_AOFactoryMgr:= TAOFactoryMgr.Create;
    Result:= g_AOFactoryMgr;
end;

initialization
 
finalization
  g_AOFactoryMgr.Destroy;

end.

