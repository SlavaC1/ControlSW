unit MicAOFactory;

interface
uses
  AgentExtensibilitySDK;

  function MicVCLObj2AO(obj:TObject;var AgentObject :TMicAO):HRESULT; stdcall;

type

  TMicMenuItem=class(TMicAO)
    public
      function GetCount: Integer;
      function GetSubObjectEx(propname:string; var SubObjectProp: string): TObject; override;

    published
      property Count: Integer read GetCount;
  end;

  TMicToolBar=class(TMicWinControlAO)
    public
      function GetSubObjectEx(propname:string; var SubObjectProp: string): TObject; override;
      function GetButtonCount: Integer;
    published
      property ButtonCount: Integer read GetButtonCount;
  end;

  TMicCheckListBox=class(TMicWinControlAO)
    public
       function GetPropertyEx(propname:string; var Value: OleVariant): HRESULT; override;
  end;

  TMicApplication=class(TMicAO)
    protected
      function GetMainForm:TObject;
    published
      property MainForm:TObject read GetMainForm;
  end;

  TMicForm=class(TMicWinControlAO)
    protected
      function ReplayRestore: Integer;
      function ReplayMinimize: Integer;
    published
      property __QTPReplayMtd_Restore: Integer read ReplayRestore;
      property __QTPReplayMtd_Minimize: Integer read ReplayMinimize;
  end;



implementation
uses
  SysUtils, Windows, Messages, TypInfo, Menus,
  Forms, Controls, Graphics, ComCtrls, CheckLst;


  function MicVCLObj2AO(obj:TObject;var AgentObject :TMicAO):HRESULT; stdcall;
  begin
      if IsInheritFrom(obj,'TMenuItem') then
        begin
          AgentObject:=  TMicMenuItem.Create;
          Result:= S_OK;
        end
      else if IsInheritFrom(obj,'TToolBar') then
        begin
          AgentObject:=  TMicToolBar.Create;
          Result:= S_OK;
        end
      else if IsInheritFrom(obj,'TCheckListBox') then
        begin
          AgentObject:=  TMicCheckListBox.Create;
          Result:= S_OK;
        end
      else if IsInheritFrom(obj,'TForm') then
        begin
          AgentObject:=  TMicForm.Create;
          Result:= S_OK;
        end
      else if IsInheritFrom(obj,'TApplication') then
        begin
          AgentObject:=  TMicApplication.Create;
          Result:= S_OK;
        end
      // TODO: Enhance MicVCLObj2AO here.
      // Code below creates default AOs.
      else if IsInheritFrom(obj,'TWinControl') then
        begin
          AgentObject:=  TMicWinControlAO.Create;
          Result:= S_OK;
        end
      else if IsInheritFrom(obj,'TControl') then
        begin
          AgentObject:=  TMicControlAO.Create;
          Result:= S_OK;
        end
      else
        begin
          AgentObject:= TMicAO.Create;
          Result:= S_OK;
        end;
  end;




///////////////////////////////////////////////////////////////////
//                TMicToolBar
/////////////////////////////////////////////////////////////////// 
  function TMicToolBar.GetButtonCount: Integer;
  begin
     Result:= TToolBar(FObject).ButtonCount;
  end;

  function TMicToolBar.GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;
    var
      SubObject: String;
      Index: Integer;
  begin
    Result:= inherited GetSubObjectEx(propname, SubObjectProp);
    if Result <> nil then exit;
    if not ParseSubProperty(propname, SubObject, SubObjectProp) then exit;
    if not ParseIndexedProperty(SubObject, SubObject, Index) then exit;

   	if(UpperCase(SubObject)='BUTTONS') and
      (Index >= 0) and (Index < TToolBar(FObject).ButtonCount) then
	   	    Result:= TToolBar(FObject).Buttons[Index];
  end;


///////////////////////////////////////////////////////////////////
//                TMicMenuItem
///////////////////////////////////////////////////////////////////
  function TMicMenuItem.GetCount: Integer;
  begin
    Result:= TMenuItem(FObject).Count;
  end;

  function TMicMenuItem.GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;
  var
    SubObject: String;
    Index: Integer;
  begin
    Result:= nil;
    if not ParseSubProperty(propname, SubObject, SubObjectProp) then exit;
    if not ParseIndexedProperty(SubObject, SubObject, Index) then exit;

   	if(UpperCase(SubObject)='ITEMS') and
      (Index >= 0) and (Index < TMenuItem(FObject).Count) then
	   	    Result:= TMenuItem(FObject).Items[Index];
  end;


  function TMicCheckListBox.GetPropertyEx(propname:string; var Value: OleVariant): HRESULT;
  var
    Index: Integer;
  begin
    Result:= inherited GetPropertyEx(propname, Value);
    if Result = S_OK then exit;
    Result:= E_INVALIDARG;
    if not ParseIndexedProperty(propname, propname, Index) then exit;
    if  (UpperCase(propname)='CHECKED') and (Index >= 0) then
      begin
        if(Index < TCheckListBox(FObject).Count) then
        begin
          Value:= TCheckListBox(FObject).Checked[Index];
          Result:= S_OK;
        end
        else
          Result:= E_INVALIDARG;
      end;
  end;

 function TMicApplication.GetMainForm:TObject;
 begin
    Result:= TApplication(FObject).MainForm;
 end;

 function TMicForm.ReplayRestore:Integer;
 begin
    if IsWindowVisible(TForm(FObject).Handle) then
       ShowWindow(TForm(FObject).Handle, SW_RESTORE)
    else
       Application.Restore;

    Result:= S_OK;
 end;

 function TMicForm.ReplayMinimize:Integer;
 begin
    if (GetWindowLong(TForm(FObject).Handle, GWL_STYLE) or WS_MINIMIZEBOX) = 0 then
    begin
      Result:= E_FAIL;
      exit;
    end;

    if TForm(FObject).Handle = Application.MainForm.Handle then
       Application.Minimize
     else
       ShowWindow(TForm(FObject).Handle, SW_MINIMIZE);

    Result:= S_OK;
 end;

end.
