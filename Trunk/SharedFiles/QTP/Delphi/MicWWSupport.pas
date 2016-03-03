{$A8,B-,C+,D+,E-,F-,G+,H+,I+,J+,K-,L+,M+,N+,O-,P+,Q-,R-,S-,T-,U-,V+,W-,X+,Y+,Z1}
{$ifdef VER150}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$endif}

unit MicWWSupport;

interface
uses
  AgentExtensibilitySDK,  DlphVDEPAgentImpl,
  Windows, Messages, SysUtils, Classes, DBTables, Db,
  Wwdbigrd, Wwdbgrid,  Wwdatsrc,  Wwtable,
  Forms, Controls, Graphics, Dialogs, StdCtrls, Grids;


type
  TMicAOWwDBGrid = class (TCustomGridAOBase)
    public
      function ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean; override;
      function GetCellDataEx(row, column: Longint):string ;override;
      procedure SetCellDataEx(row, column: Longint; Value: String); override;
      function RowCountEx :Integer ; override;
      function ColCountEx :Integer ; override;
      function GridIdxToVisibleRowNum( row_str:string ): Longint; override;
      function GetSelectedRowEx:Integer ; override;
      function CaptureTableEx(filename:string): boolean; override;
  end;



implementation
const
  MAXROWS = 2147483647;



//////////////////////////////////////////////////////
//   implementation for TwwDBGrid
/////////////////////////////////////////////////////
var
lest_click: DWORD;
IsDbClick, IsClientClick :BOOL;
LastCol, LastRow: Integer;
LastCellText:string;
//Editing: Boolean;
function  TMicAOWwDBGrid.ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean;


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
  RecLine.Params[0]:= row_str;
  RecLine.Params[1]:= col_str;
  RecLine.SendMode:= Mode;
end;

// can't use TDrawGrid(FObject).Col since it returns the number of
// row out of visible rows, and not out of whole set of grid rows.
function GetCurDBRow(): integer;
begin
    if ( (not Assigned(TwwDBGrid(FObject).DataSource)) or
         (not Assigned(TwwDBGrid(FObject).DataSource.DataSet)) ) then
         Result := -1
    else
         Result :=  TwwDBGrid(FObject).DataSource.DataSet.RecNo;
end;

var
  row,Column: Integer;
  row_str,col_str:string;
  CellStr:String;
  var Control: TObject;
begin
    Result := false;

    // don't want redundant Type or Click operations to be recorded
    if (Msg.Msg=WM_LBUTTONDOWN) or (Msg.Msg=WM_LBUTTONUP) or (Msg.Msg=WM_LBUTTONDBLCLK)
        or (Msg.Msg = WM_KEYDOWN) or (Msg.Msg = WM_KEYUP) then
    begin
      Result := true;
      SetLength(RecLine.Params, 0);
      RecLine.SendMode:= NO_PROCESS;
    end;

    // record SetCellData on messages intended for child TwwInplaceEdit
    if   (GetParent(handle) = TWinControl(FObject).Handle) then
    begin

      control := findProcessControl(handle);
      if (Control <> nil) and IsInheritFrom(control,'TwwInplaceEdit') then
      begin

        // if WM_SETFOCUS, store LastCellText
        if ( Msg.Msg=WM_SETFOCUS) then
           LastCellText:=TwwInplaceEdit(control).Text;

        // if WM_KILLFOCUS, might have to record SetCellData
        if ( Msg.Msg=WM_KILLFOCUS) then
        begin

           // check if text has changed
           CellStr:=TwwInplaceEdit(control).Text;
           if (CellStr<>LastCellText) then
           begin

              ColRowToStr(GetCurDBRow(), TDrawGrid(FObject).Col, row_str, col_str);
              RecLine.Cmd:= 'SetCellData';
              SetLength(RecLine.Params, 3);
              RecLine.Params[0]:= row_str;
              RecLine.Params[1]:= col_str;
              RecLine.Params[2]:= cellStr;
              RecLine.SendMode:= SEND_LINE;
              result := true;
          end;
       end;
      end;
    end;

    // from here on we have no interest in messages not intended for grid
    if handle <> TWinControl(FObject).Handle then exit;

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

     end;

end;

function  TMicAOWwDBGrid.RowCountEx() :Integer ;
var
  bookmark: TBookmark;
begin
   bookmark := TwwDBGrid(FObject).DataSource.Dataset.GetBookmark;
   TwwDBGrid(FObject).DataSource.Dataset.DisableControls;
   TwwDBGrid(FObject).DataSource.Dataset.First;
   if  (TwwDBGrid(FObject).DataSource.Dataset.EOF) then
         Result:=0
    else
    begin
    Result := Abs(TwwDBGrid(FObject).DataSource.Dataset.MoveBy(MAXROWS))+1;
    TwwDBGrid(FObject).DataSource.Dataset.EnableControls;
    TwwDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
    TwwDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
    end;
end;

function  TMicAOWwDBGrid.ColCountEx() :Integer ;
begin
    Result :=TwwDBGrid(FObject).FieldCount;
end;


procedure TMicAOWwDBGrid.SetCellDataEx(row, column: Longint; Value: String);
  var
  T: TField;
  bookmark: TBookmark;
  begin
     dec (column);       // the first colume is #1
     if (column<0) then exit;
     try
        begin
      if Assigned(TwwDBGrid(FObject).DataSource) then
       if Assigned(TwwDBGrid(FObject).DataSource.Dataset) then
       begin
       // if ( dgIndicator in TwwDBGrid(FObject).Options ) then
//          if ( row = 0 ) then
//             begin
//              TwwCustomDBGrid(FObject).Columns[Integer(column)];
//              end
 //         else
            begin
            bookmark := TwwDBGrid(FObject).DataSource.Dataset.GetBookmark;
            TwwDBGrid(FObject).DataSource.Dataset.DisableControls;
            TwwDBGrid(FObject).DataSource.Dataset.First;
            TwwDBGrid(FObject).DataSource.Dataset.MoveBy(Integer(row - 1));
            T := TwwCustomDBGrid(FObject).Fields[Integer(column)];
            TwwDBGrid(FObject).DataSource.Dataset.Edit;
  		      SetStringToTField(T, Value);
            TwwDBGrid(FObject).DataSource.Dataset.EnableControls;
            TwwDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
            TwwDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
         end;
        end; // if Assigned(TwwDBGrid(obj).DataSource.Dataset)

  end; // try
  except
  end;
end;


function TMicAOWwDBGrid.GetCellDataEx(row, column: Longint):string ;
  var
  T: TField;
  s: string;
  bookmark: TBookmark;

  begin
     dec (column);       // the first colume is #1
     if (column<0) then exit;
     try
        begin
      if Assigned(TwwDBGrid(FObject).DataSource) then
       if Assigned(TwwDBGrid(FObject).DataSource.Dataset) then
       begin
       // if ( dgIndicator in TwwDBGrid(FObject).Options ) then
          if ( row = 0 ) then
             begin


             s :=TwwCustomDBGrid(FObject).Fields[Integer(column)].FieldName;
             Result := ReplaceIllegalCharacters(s);
             end
          else
            begin
            bookmark := TwwDBGrid(FObject).DataSource.Dataset.GetBookmark;
            TwwDBGrid(FObject).DataSource.Dataset.DisableControls;
            TwwDBGrid(FObject).DataSource.Dataset.First;
            TwwDBGrid(FObject).DataSource.Dataset.MoveBy(Integer(row - 1));
            T := TwwCustomDBGrid(FObject).Fields[Integer(column)];
            Result := GetStringFromTField(T);
            TwwDBGrid(FObject).DataSource.Dataset.EnableControls;
            TwwDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
            TwwDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
         end;
        end; // if Assigned(TwwDBGrid(obj).DataSource.Dataset)

  end; // try
  except
  end;
  end;


function TMicAOWwDBGrid.GridIdxToVisibleRowNum( row_str:string ): Longint;
var
row:Longint;
begin
 Result :=-1;
 if (row_str[1]<>'#') then exit;
 row_str[1]:=' ';
 row :=MicStrToInt(row_str);
 if (row<0) then exit;

 //TwwDBGrid(FObject).DataSource.Dataset.DisableControls;
 TwwDBGrid(FObject).DataSource.Dataset.First;
 TwwDBGrid(FObject).DataSource.Dataset.MoveBy(row - 1);
 //TwwDBGrid(FObject).DataSource.Dataset.EnableControls;
 Result :=TDrawGrid(FObject).row;
end;

function TMicAOWwDBGrid.GetSelectedRowEx():Integer ;
var
bookmark: TBookmark;
begin
  bookmark := TwwDBGrid(FObject).DataSource.Dataset.GetBookmark;
  TwwDBGrid(FObject).DataSource.Dataset.DisableControls;
  Result := Abs(TwwDBGrid(FObject).DataSource.Dataset.MoveBy(-MAXROWS)) + 1;
  TwwDBGrid(FObject).DataSource.Dataset.EnableControls;
  TwwDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
  TwwDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
end;

function TMicAOWwDBGrid.CaptureTableEx(filename:string): boolean;
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
   if Assigned(TwwDBGrid(FObject).DataSource) then
     if Assigned(TwwDBGrid(FObject).DataSource.Dataset) then
       begin
            num_cols := TwwCustomDBGrid(FObject).FieldCount;
            line:='';
            for i:=1 to num_cols do
                 line := line+ GetCellDataEx(0,i) + #9;
            line:=copy (line,1,Length(line)-1);  // remove the lest #9
            Writeln(F, line);

            bookmark := TwwDBGrid(FObject).DataSource.Dataset.GetBookmark;
            TwwDBGrid(FObject).DataSource.Dataset.DisableControls;
            TwwDBGrid(FObject).DataSource.Dataset.First;
            while ( (not TwwDBGrid(FObject).DataSource.Dataset.EOF)) do
              begin
              line:='';
                  for i:=0 to num_cols-1 do
                    begin
                      T := TwwCustomDBGrid(FObject).Fields[i];
                      line := line + GetStringFromTField(T)+ #9;
                      end;
              line:=copy (line,1,Length(line)-1);  // remove the lest #9
              Writeln(F, line);
              TwwDBGrid(FObject).DataSource.Dataset.Next;
              end;
            TwwDBGrid(FObject).DataSource.Dataset.EnableControls;
            TwwDBGrid(FObject).DataSource.Dataset.GotoBookmark( bookmark );
            TwwDBGrid(FObject).DataSource.Dataset.FreeBookmark( bookmark );
     end; // if Assigned(TwwDBGrid(obj).DataSource.Dataset)
finally
CloseFile(F);
end; // try
end;

function GridVCLObj2AO(obj:TObject;var AgentObject :TMicAO):HRESULT; stdcall;
begin
     Result:= S_OK;

     if (IsInheritFrom(Obj,'TwwDBGrid')) then
       AgentObject:=TMicAOWwDBGrid.Create
     else
       Result:= S_FALSE;
end;

begin
  AddExtensibilityServer(@GridVCLObj2AO);
end.
