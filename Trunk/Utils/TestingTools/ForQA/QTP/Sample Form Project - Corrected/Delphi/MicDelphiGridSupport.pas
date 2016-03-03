{$A8,B-,C+,D+,E-,F-,G+,H+,I+,J+,K-,L+,M+,N+,O-,P+,Q-,R-,S-,T-,U-,V+,W-,X+,Y+,Z1}
{$ifdef VER150}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$endif}

unit MicDelphiGridSupport;               

interface
uses
  AgentExtensibilitySDK,
  Windows, Messages, SysUtils, Db, DBTables,
  Forms, Controls, Graphics, Grids, DBGrids, DBCtrls, StdCtrls, Mask;

type

TMicAODBGrid = class (TCustomGridAOBase)
  protected
    function ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean; override;
    function GetCellDataEx(row, column: Longint):string ;override;
    procedure SetCellDataEx(row, column: Longint; Value: String); override;
    function RowCountEx :Integer ; override;
    function ColCountEx :Integer ; override;
    function GridIdxToVisibleRowNum( row_str:string ): Longint; override;
    function GetSelectedRowEx:Integer ; override;
    function CaptureTableEx(filename:string): boolean; override;
  public
    function GetSubObjectEx(propname:string; var SubObjectProp: string): TObject; override;
end;


TMicAODBGridInplaceEdit = class (TMicWinControlAO)
  protected
    function  Get_Recording :string ;

  published
    property __QTPRecording: string read Get_Recording;
end;

TMicAOPopupListbox = class (TMicWinControlAO)
  private
    function GetItemFromPoint(p:TPoint) :string ;
    function ItemToIndex(item:string):Integer;
  protected
    function  Get_Recording :string ;
    function  Get_MercHandle:HWND ;
    function  Get_ItemPoint :string ;
  published
    property __QTPRecording:string  read Get_Recording;
    property __MercHandle :HWND  read Get_MercHandle;
    property __ItemPoint: string read Get_ItemPoint;
end;

TMicAOStringGrid = class (TCustomGridAOBase)
  protected
    function GetCellDataEx(row, column: Longint):string ;override;
    procedure SetCellDataEx(row, column: Longint; Value: String); override;
    function GridIdxToVisibleRowNum( row_str:string ): Longint; override;

end;

TMicAODBLookupComboBox = class (TMicWinControlAO)
  protected
    function  Get_Text :string ;
    function  Get_MercEOF :Boolean;
  published
    property Text: string read Get_Text;
    property __MercEOF: Boolean read Get_MercEOF;
end;

TMicAODBLookupListBox = class (TMicWinControlAO)
  protected
    function  Get_Recording :string ;
    function  Get_MercHandle:HWND ;
    function  Get_Text :string ;

  published
    property __QTPRecording: string read Get_Recording;
    property __MercHandle :HWND  read Get_MercHandle;
    property Text: string read Get_Text;

end;


implementation
  
const
  MAXROWS = 2147483647;

//////////////////////////////////////////////////////
//   implementation for TDbGrid
/////////////////////////////////////////////////////

var
lest_click: DWORD;
IsDbClick, IsClientClick :BOOL;
LastCol, LastRow: Integer;
LastCellText:string;
Editing: Boolean;
function  TMicAODBGrid.ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean;

procedure ColRowToStr(Row, Column: Integer; var row_str,col_str:string);
begin
  row_str:='#'+inttostr(row);
  col_str:=GetCellDataEx(0,Column);
  if ((Length(col_str)=0) or (Length(col_str) > MIC_MAX_HEADER_LENGTH)) then
     col_str:='#'+inttostr(Column);

end;

procedure PrepareCellRecordLine(Operation: String; Row, Column, Mode: Integer; var RecLine:TRecordLine);
var
  row_str,col_str:string;
begin
  ColRowToStr(Row, Column, row_str, col_str);
  RecLine.Cmd:= Operation;
  SetLength(RecLine.Params, 2);
  RecLine.Params[0]:= Row;
  RecLine.Params[1]:= col_str;
  RecLine.SendMode:= Mode;
end;

// can't use TDrawGrid(FObject).Col since it returns the number of
// row out of visible rows, and not out of whole set of grid rows.
function GetCurDBRow(): integer;
begin
    if ( (not Assigned(TDBGrid(FObject).DataSource)) or
         (not Assigned(TDBGrid(FObject).DataSource.DataSet)) ) then
         Result := -1
    else
         Result :=  TDBGrid(FObject).DataSource.DataSet.RecNo;
end;

var
  row,Column: Integer;
  row_str,col_str:string;
  CellStr:String;
begin
    Result := false;

    if handle <> TWinControl(FObject).Handle then exit;

    // don't want redundant Type or Click operations to be recorded
    if (Msg.Msg=WM_LBUTTONDOWN) or (Msg.Msg=WM_LBUTTONUP) or (Msg.Msg=WM_LBUTTONDBLCLK)
        or (Msg.Msg = WM_KEYDOWN) or (Msg.Msg = WM_KEYUP) then
    begin
      Result := true;
      SetLength(RecLine.Params, 0);
      RecLine.SendMode:= NO_PROCESS;
    end;

    // case WM_LBUTTONDOWN
    if (Msg.Msg=WM_LBUTTONDOWN) then
    begin

         IsDbClick:= FALSE;
         // don't want to record SelectCell or ActivateCell on clicks made
         // on scrollbar. for clicks on scrollbar, a WM_NCLBUTTONDOWN
         // is sent for button down. for some reason WM_LBUTTONUP and not
         // WM_NCLBUTTONUP is sent for button up. so we need to know not
         // to record those
         IsClientClick := TRUE;
    end

    // case WM_LBUTTONUP or WM_LBUTTONDBLCLK
    else
    if ( (Msg.Msg=WM_LBUTTONUP) or
       (Msg.Msg=WM_LBUTTONDBLCLK) ) then
    begin

      // can't use coordinate to calculate row and col clicked since the
      // grid may move with the click and we'll get the current cell under
      // the cursor (even when calculating on WM_LBUTTONDOWN)
      column := TDrawGrid(FObject).Col;
      row := GetCurDBRow();
      if (row = -1) then exit;
      LastCol:= Column;
      LastRow:= row;

      if ( (Msg.Msg=WM_LBUTTONDBLCLK)
          or (((GetTickCount()-lest_click)<500) and IsClientClick) )then
      begin
         PrepareCellRecordLine('ActivateCell', Row, Column, REPLACE_AND_SEND_LINE, RecLine);
         Result:= true;
         IsDbClick:= TRUE;
	 if (Msg.Msg = WM_LBUTTONUP) then IsClientClick := FALSE; // reset IsClientClick
      end
      else if IsClientClick then  // case WM_LBUTTONUP
      begin
        if (not (IsDbClick)) then // need to record only if haven't already recorded on WM_LBUTTONDBLCLK
        begin
          PrepareCellRecordLine('SelectCell', Row, Column, KEEP_LINE, RecLine);
          Result:= true;
        end;
        IsClientClick := FALSE; // reset IsClientClick
      end;

      lest_click:= GetTickCount();

    end

    // case WM_KEYDOWN
     else
     if Msg.Msg = WM_KEYDOWN then
     begin

       LastCol:=  TDrawGrid(FObject).Col;
       LastRow:=  GetCurDBRow();
       if (LastRow = -1) then exit;

       if Msg.WParam = VK_RETURN then
       begin
         PrepareCellRecordLine('ActivateCell', LastRow, LastCol, SEND_LINE, RecLine);
         Result:= true;
       end;

     end

     // case WM_KEYUP
     else
     if Msg.Msg = WM_KEYUP then
     begin

       row := GetCurDBRow();
       if (row = -1) then exit;
       if (LastCol<>TDrawGrid(FObject).Col) or (LastRow<>row) then
       begin
          PrepareCellRecordLine('SelectCell', row,
                                      TDrawGrid(FObject).Col, SEND_LINE, RecLine);
          Result:= true;
       end
     end

     // case WM_KILLFOCUS
     else
     if (Msg.Msg = WM_KILLFOCUS) and (GetParent(Msg.WParam) = TWinControl(FObject).Handle) then
     begin // Editor activated
       LastCol:=  TDrawGrid(FObject).Col;
       LastRow:=  GetCurDBRow();
       if (LastRow = -1) then exit;
       LastCellText:= GetCellDataEx(LastRow, LastCol);
       Editing:= true;
     end

     // case WM_SETFOCUS
     else
     if Msg.Msg = WM_SETFOCUS then
     begin
        if Editing then
        begin
          CellStr:= GetCellDataEx(LastRow, LastCol);
          if LastCellText <> CellStr then
          begin
            ColRowToStr(LastRow, LastCol, row_str, col_str);
            RecLine.Cmd:= 'SetCellData';
            SetLength(RecLine.Params, 3);
            RecLine.Params[0]:= LastRow;
            RecLine.Params[1]:= col_str;
            RecLine.Params[2]:= CellStr;
            RecLine.SendMode:= SEND_LINE;
            Editing:= false;
            LastCol:=  TDrawGrid(FObject).Col;
            LastRow:=  GetCurDBRow();
            if (LastRow = -1) then exit;
            Result:= true;
          end;
        end;

     end;

end;

function  TMicAODBGrid.RowCountEx() :Integer ;
var
  bookmark: TBookmark;
begin
   Result:= TDBGrid(FObject).DataSource.Dataset.RecordCount;
   if Result <= 0 then
   begin
      bookmark := TDBGrid(FObject).DataSource.Dataset.GetBookmark;
      TDBGrid(FObject).DataSource.Dataset.DisableControls;
      TDBGrid(FObject).DataSource.Dataset.First;
      if  (TDBGrid(FObject).DataSource.Dataset.EOF) then
          Result:=0
      else
      begin
        Result := Abs(TDBGrid(FObject).DataSource.Dataset.MoveBy(MAXROWS))+1;
        TDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
        TDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
        TDBGrid(FObject).DataSource.Dataset.EnableControls
      end
   end
end;

function  TMicAODBGrid.ColCountEx() :Integer ;
begin
    Result :=TDBGrid(FObject).FieldCount;
end;



function TMicAODBGrid.GetCellDataEx(row, column: Longint):string ;
  var
  T: TField;
  clmn: TColumn;
  bookmark: TBookmark;
  iRes: Integer;
  begin
     dec (column);       // the first colume is #1
     if (column<0) then exit;
     try
        begin
      if Assigned(TDBGrid(FObject).DataSource) then
       if Assigned(TDBGrid(FObject).DataSource.Dataset) then
       begin
       // if ( dgIndicator in TDBGrid(FObject).Options ) then
          if ( row = 0 ) then
             begin
              clmn := TDBGrid(FObject).Columns[Integer(column)];
              Result := clmn.Title.Caption;
             end
          else
            begin
            bookmark := TDBGrid(FObject).DataSource.Dataset.GetBookmark;
            TDBGrid(FObject).DataSource.Dataset.DisableControls;
            TDBGrid(FObject).DataSource.Dataset.First;
 			iRes:= TDBGrid(FObject).DataSource.Dataset.MoveBy(Integer(row - 1));

            // Check the range of rows and cols return the empty string, if the cell doesn't exist
            if((iRes = Integer(row - 1)) and
            	(TCustomDBGrid(FObject).FieldCount > Integer(column))) then
 	           begin
		            T := TCustomDBGrid(FObject).Fields[Integer(column)];
		            Result := GetStringFromTField(T);
	            end
            else
            	Result:= '';

            TDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
            TDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
            TDBGrid(FObject).DataSource.Dataset.EnableControls;
         end;
        end; // if Assigned(TDBGrid(obj).DataSource.Dataset)

  end; // try
  except
  end;
  end;


procedure TMicAODBGrid.SetCellDataEx(row, column: Longint; Value: String);
  var
  T: TField;
  bookmark: TBookmark;
  iRes: Integer;
  begin
     dec (column);       // the first colume is #1
     if (column<0) then exit;
     try
        begin
      if Assigned(TDBGrid(FObject).DataSource) then
       if Assigned(TDBGrid(FObject).DataSource.Dataset) then
       begin
       // if ( dgIndicator in TDBGrid(FObject).Options ) then
          if ( row = 0 ) then
             begin
              TDBGrid(FObject).Columns[Integer(column)];
              end
          else
            begin
            bookmark := TDBGrid(FObject).DataSource.Dataset.GetBookmark;
            TDBGrid(FObject).DataSource.Dataset.DisableControls;
            TDBGrid(FObject).DataSource.Dataset.First;
 	      		iRes:= TDBGrid(FObject).DataSource.Dataset.MoveBy(Integer(row - 1));
            if not (dgEditing in (TDBGrid(FObject).Options)) then exit;

            // Check the range of rows and cols return the empty string, if the cell doesn't exist
            if((iRes = Integer(row - 1)) and
            	(TCustomDBGrid(FObject).FieldCount > Integer(column))) then
 	           begin
		            T := TCustomDBGrid(FObject).Fields[Integer(column)];
                TDBGrid(FObject).DataSource.Dataset.Edit;
  		          SetStringToTField(T, Value);
	           end;

            TDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
            TDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
            TDBGrid(FObject).DataSource.Dataset.EnableControls;
         end;
        end; // if Assigned(TDBGrid(obj).DataSource.Dataset)

  end; // try
  except
  end;
end;



function TMicAODBGrid.GridIdxToVisibleRowNum( row_str:string ): Longint;
var
row:Longint;
begin
 Result :=-1;
 if (row_str[1]<>'#') then exit;
 row_str[1]:=' ';
 row :=MicStrToInt(row_str);
 if (row<0) then exit;

 //TDBGrid(FObject).DataSource.Dataset.DisableControls;
 TDBGrid(FObject).DataSource.Dataset.First;
 TDBGrid(FObject).DataSource.Dataset.MoveBy(row - 1);
 //TDBGrid(FObject).DataSource.Dataset.EnableControls;
 Result :=TDrawGrid(FObject).row;
end;

function TMicAODBGrid.GetSelectedRowEx():Integer ;
var
bookmark: TBookmark;
begin
  bookmark := TDBGrid(FObject).DataSource.Dataset.GetBookmark;
  TDBGrid(FObject).DataSource.Dataset.DisableControls;
  Result := Abs(TDBGrid(FObject).DataSource.Dataset.MoveBy(-MAXROWS)) + 1;
  TDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
  TDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
  TDBGrid(FObject).DataSource.Dataset.EnableControls;
end;

function TMicAODBGrid.CaptureTableEx(filename:string): boolean;
var
F : TextFile;
num_cols,i: Integer;
line: string;
bookmark: TBookmark;
T: TField;
begin
Result :=True;
if (Length(filename)=0) then  exit;
AssignFile(F, fileName);
Rewrite(F);
try
   if Assigned(TDBGrid(FObject).DataSource) then
     if Assigned(TDBGrid(FObject).DataSource.Dataset) then
       begin
            num_cols := TCustomDBGrid(FObject).FieldCount;
            line:='';
            for i:=1 to num_cols do
                 line := line+ GetCellDataEx(0,i) + #9;
            line:=copy (line,1,Length(line)-1);  // remove the lest #9
            Writeln(F, line);

            bookmark := TDBGrid(FObject).DataSource.Dataset.GetBookmark;
            TDBGrid(FObject).DataSource.Dataset.DisableControls;
            TDBGrid(FObject).DataSource.Dataset.First;
            while ( (not TDBGrid(FObject).DataSource.Dataset.EOF)) do
              begin
              line:='';
                  for i:=0 to num_cols-1 do
                  begin
                   T := TCustomDBGrid(FObject).Fields[i];
                   line := line + ReplaceIllegalCharacters(GetStringFromTField(T))+ #9;
                  end;
              line:=copy (line,1,Length(line)-1);  // remove the lest #9
              Writeln(F, line);
              TDBGrid(FObject).DataSource.Dataset.Next;
              end;
            TDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
            TDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
            TDBGrid(FObject).DataSource.Dataset.EnableControls;
     end; // if Assigned(TDBGrid(obj).DataSource.Dataset)
finally
CloseFile(F);
end; // try
end;

function  TMicAODBGrid.GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;
var
  SubObject: String;
  Index: Integer;
begin
  Result:= inherited GetSubObjectEx(propname, SubObjectProp);
  if Result <> nil then exit;
  if not ParseSubProperty(propname, SubObject, SubObjectProp) then exit;
  if not ParseIndexedProperty(SubObject, SubObject, Index) then exit;
  if (UpperCase(propname)='ITEMS') then
  begin
    try
      Result:=TDBGridColumns(Self.FObject).Items[Index]
    except
      Result := nil
    end
  end;
end;

//////////////////////////////////////////////////////
//   implementation for TDBGridInplaceEdit
/////////////////////////////////////////////////////
var
GLestMecrString:string;
function  TMicAODBGridInplaceEdit.Get_Recording() :string ;
  var
  Msg: TMessage;
  S:string;
  p:TPoint;
  begin
  Result :='';// 'TMicAODBGridInplaceEdit '+FParam[1]+'; Msg'+FParam[2]+';'+FParam[3]+';'+FParam[4] ;
//  if (not (GetMessageParm(Msg))) then exit;

  if ( Msg.Msg=WM_SETFOCUS) then
  begin
     GLestMecrString:=TCustomMaskEdit(FObject).Text;
     exit;
  end;


  if ( Msg.Msg=WM_KILLFOCUS) then
  begin
  s:=TCustomMaskEdit(FObject).Text;
  if (s<>GLestMecrString) then
     begin
     Result := 'dlph_edit_set ("%m","'+s+'")';
     end;
  end;

 if ( Msg.Msg=WM_LBUTTONUP) then
     begin
     p.x:=LOWORD(Msg.lParam);
     p.y:=HIWORD(Msg.lParam);
     s:=inttostr(p.x)+', '+ inttostr(p.y);
     Result := 'obj_mouse_click ("%m", '+s+')';
     end;
end;


//////////////////////////////////////////////////////
//   implementation for TPopupListbox
/////////////////////////////////////////////////////

function  TMicAOPopupListbox.GetItemFromPoint(p:TPoint) :string ;
var
ItemNo : Integer;
begin
Result:='';
ItemNo := TCustomListBox(FObject).ItemAtPos(p, True);
if (ItemNo >= 0) then
    begin
    Result:=TCustomListBox(FObject).Items[ItemNo];
    end;
end;
function  TMicAOPopupListbox.Get_Recording() :string ;
var
Msg: TMessage;
p:TPoint;
s:string;
begin
//  if (not (GetMessageParm(Msg))) then exit;
  Result :='';// 'TMicAOPopupListbox-> '+FParam[1]+'; Msg'+FParam[2]+';'+FParam[3]+';'+FParam[4] ;
  if ( Msg.Msg=WM_LBUTTONDOWN) then
     begin
     p.x:=LOWORD(Msg.lParam);
     p.y:=HIWORD(Msg.lParam);
     s:=GetItemFromPoint(p);
     Result := 'dlph_list_select_item ("%m","'+s+'")';
     end;
end;

function  TMicAOPopupListbox.Get_MercHandle():HWND ;
var
obj:TWinControl;
begin
obj:=TWinControl( TWinControl(FObject).Owner);
Result:= obj.handle;
end;

function TMicAOPopupListbox.ItemToIndex(item:string):Integer;
var
Count,i:Integer;
begin
Result:=-1;
if (item[1]='#') then
   begin
      item[1]:=' ';
      Result :=MicStrToInt(item)-1;
      exit;
   end;

Count := TCustomListBox(FObject).Items.Count;
for i:=0 to count do
    if (TCustomListBox(FObject).Items[i]=item) then
        begin
            Result:=i;
            Exit;
        end;

end;

function TMicAOPopupListbox.Get_ItemPoint() :string ;
var
item:string;
R:TRect;
ItemNo : Integer;
p1,p2:TPOINT;
begin
Result:='0;0';

if pParam^.NumOfParams < 1 then exit;
item:=pParam^.Parameters[0];
ItemNo:=ItemToIndex(item);
if (ItemNo<0) then exit;
r:=TCustomListBox(FObject).ItemRect(ItemNo);
if ( (r.Left=0) and (r.Top=0)and (r.Right=0) and (r.Bottom=0)) then exit;
p1.x:=r.Left;
p1.y:=r.Top;
p2:=TControl(FObject).ClientToScreen(p1);
Result:= inttostr(p2.x)+';'+ inttostr(p2.y)
end;

//////////////////////////////////////////////////////
//   implementation for TStringGrid
/////////////////////////////////////////////////////
function TMicAOStringGrid.GetCellDataEx(row, column: Longint):string ;
begin
  Result :=TStringGrid(FObject).Cells[column,row];
end;

procedure TMicAOStringGrid.SetCellDataEx(row, column: Longint; Value: String);
begin
   TStringGrid(FObject).Cells[column,row]:= Value;
end;

function TMicAOStringGrid.GridIdxToVisibleRowNum( row_str:string ): Longint;
begin
  Result:= RowStrToInt(row_str);
end;

//////////////////////////////////////////////////////
//   implementation for TDBLookupComboBox
/////////////////////////////////////////////////////

function TMicAODBLookupComboBox.Get_Text() :string ;
begin
Result :=TDBLookupComboBox(FObject).Text;
end;

function TMicAODBLookupComboBox.Get_MercEOF() :Boolean ;
begin
try
 begin
 Result :=TDBLookupComboBox(FObject).ListSource.Dataset.EOF
 end; // try
 except
 Result :=FALSE;
 end;

end;


//////////////////////////////////////////////////////
//   implementation for TDBLookupListBox
/////////////////////////////////////////////////////
function  TMicAODBLookupListBox.Get_Recording() :string ;
  var
  Msg: TMessage;
  S:string;
  begin
  Result :='';//'TMicAODBLookupListBox '+FParam[1]+'; Msg'+FParam[2]+';'+FParam[3]+';'+FParam[4] ;
//  if (not (GetMessageParm(Msg))) then exit;
  if ( Msg.Msg=WM_LBUTTONUP) then
     begin
     S:=TDBLookupListBox(FObject).SelectedItem;
     Result := 'dlph_list_select_item ("%m","'+s+'")';
     end;

end;

function  TMicAODBLookupListBox.Get_MercHandle():HWND ;
var
obj:TWinControl;
begin
if (FObject.InheritsFrom(TPopupDataList)) then  //TPopupDataList
   obj:=TWinControl( TWinControl(FObject).Owner)
else
   obj:=TWinControl(FObject);
   Result:= obj.handle;
end;

function TMicAODBLookupListBox.Get_Text() :string ;
begin
Result :=TDBLookupListBox(FObject).SelectedItem;
end;


function GridVCLObj2AO(obj:TObject;var AgentObject :TMicAO):HRESULT; stdcall;
begin
     Result:= S_OK;

     if (IsInheritFrom(Obj,'TDBGrid')) then
        AgentObject:=TMicAODBGrid.Create
     //else if (IsInheritFrom(Obj,'TDBGridInplaceEdit')) then
       //AgentObject:=TMicAODBGridInplaceEdit.Create
     else if (IsInheritFrom(Obj,'TPopupListbox')) then
       AgentObject:=TMicAOPopupListbox.Create
     else if (IsInheritFrom(Obj,'TStringGrid')) then
       AgentObject:=TMicAOStringGrid.Create
     else if (IsInheritFrom(Obj,'TDBLookupComboBox')) then
       AgentObject:=TMicAODBLookupComboBox.Create
     else if (IsInheritFrom(Obj,'TDBLookupListBox')) then  //TDBLookupListBox   TPopupDataList
       AgentObject:=TMicAODBLookupListBox.Create
     else
       Result:= S_FALSE;
end;


begin
  AddExtensibilityServer(@GridVCLObj2AO);

end.
