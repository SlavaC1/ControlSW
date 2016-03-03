
{$A8,B-,C+,D+,E-,F-,G+,H+,I+,J+,K-,L+,M+,N+,O-,P+,Q-,R-,S-,T-,U-,V+,W-,X+,Y+,Z1}
{$ifdef VER150}
{$WARN UNSAFE_TYPE OFF}
{$WARN UNSAFE_CODE OFF}
{$WARN UNSAFE_CAST OFF}
{$endif}

///////////////////////////////////////////////////////////////////////////////
//   This unit contains the base classes that you need to inherit from when  //
//   you develop your QuickTest Professional Delphi Add-in extensibility     //
//   Agent Objects, and utility functions that you can call in your          //
//   extensibility code.                                                      //
///////////////////////////////////////////////////////////////////////////////

unit AgentExtensibilitySDK;

interface
uses TypInfo, Controls, Messages, Windows, SysUtils, DB, Math;

const
  MAX_PARAM=32; // The maximum number of parameters for a property query.
                // (See TServiceParam definition)
  ERR_INT=-999; // The value returned by MicStrToInt when it fails.
  MIC_MAX_HEADER_LENGTH = 25;  // Maximum length of table column header.
  FAIL_TO_GET_CELL_VALUE ='Fail_TO_GET_CELL_VALUE'; // The value returned by 
                                                    // TMicGridBase.GetStringFromTField
                                                    // when it fails. 


///////////////////////////////////////////////////////////////////////////////
// Recording Mode Constants                                                  //
// These constants can be used by the Agent Object to control how QuickTest  //
// adds a recorded step to the test.                                         //
///////////////////////////////////////////////////////////////////////////////
  SEND_LINE = 0; // Add step to test immediately
  KEEP_LINE = SEND_LINE + 1; // Store step in buffer and delay adding it to test. 
                             // Step is added to test when the next recorded step 
                             // is sent or when timeout expires, unless step is retracted.
  KEEP_LINE_NO_TIMEOUT	= KEEP_LINE + 1; // Same as KEEP_LINE, without timeout.
  REPLACE_AND_KEEP_LINE_NO_TIMEOUT = KEEP_LINE_NO_TIMEOUT + 1; // Replace the step in the buffer (if exists)
                                                               // with a new step, and delay adding 
                                                               // the step to the test (without timeout). 
  REPLACE_AND_SEND_LINE = REPLACE_AND_KEEP_LINE_NO_TIMEOUT + 1; //Discard the step in the buffer (if exists)  
                                                                // and add the new step to the test.
  REPLACE_AND_KEEP_LINE = REPLACE_AND_SEND_LINE + 1; // Same as REPLACE_AND_KEEP_LINE_NO_TIMEOUT but with timeout.
  CLEAN_UP	= REPLACE_AND_KEEP_LINE + 1;         // Discard the step in the buffer (if exists).
  STORED_LINE = CLEAN_UP + 1;                        // Add the step stored in the buffer to the test.
  NO_PROCESS = STORED_LINE + 1;                      // stop message from being passed on. not part of original enum.
   


type

// TServiceParam is a container for parameters passed from QuickTest.
// For example, when an event occurs during a recording session, QuickTest
// passes the relevant Windows message information to the Agent Object.
   TServiceParam = record
     Parameters: array[0..MAX_PARAM] of OleVariant;
     NumOfParams: Integer;
   End;

///////////////////////////////////////////////////////////////////////////////
// This API provides the following classes, which you can extend to create   //
// your Delphi Add-in Extensibility Agent Objects. Decide which base class   //
// to inherit according to the type of Delphi object you are supporting, and //
// whether you want to support recording on the object.                      //
// TMicAO: Base class for all Agent Objects                                  //
// TMicControlAO     (extends TMicAO)                                        //
// TMicWinControlAO  (extends TMicControlAO)                                 //
// TMicRecordableAO  (extends TMicWinControlAO)                              //
// TMicGridAOBase    (extends TMicRecordableAO)                              //
// TCustomGridAOBase (extends TMicGridAOBase)                                //
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// TMicAO is the base class for all Agent Objects                            //
// The SDK provides a basic implementation for the class's procedures and    //
// functions, which you can override with an implementation more specific to //
// your control's design if necessary.                                       //
//                                                                           // 
// The published properties defined in the Agent Object class provide the    //
// interface between QuickTest and the Delphi object in your application.    //
// QuickTest accesses the published properties of the Agent Object and then, //
// if necessary, the published properties of the Delphi object.              //
//                                                                           //
// The TMicAO class provides an empty implementation for the  GetSubObjectEx //
// and GetPropertyEx functions. If your Agent Object directly inherits from  //
// it is recommended that you override these functions.                      //
///////////////////////////////////////////////////////////////////////////////
  
  TMicAO = class(TObject)
    public
      // Init initializes the object member variables.
      
      procedure Init(obj: TObject; var Param: TServiceParam);  virtual;
      
      // The GetSubObjectEx function receives a string that represents a property  
      // that QuickTest could not retrieve directly as a published property 
      // from the object being tested, on the assumption that the string 
      // represents a property of an object contained within this object. 
      // For example: "controls[10].Name"
      // GetSubObjectEx is supposed to return the relevant sub-object (TObject)
      // if it exists, and the remaining part of the passed string, representing
      // the property within that item.
      // In the example above - the object that GetSubObjectEx needs to return is the
      // 10th item in the controls array, and the SubObjectProp argument should 
      // contain "Name".
      
      // QuickTest then creates the Agent Object that supports the object that 
      // GetSubObjectEx returned, and retrieves the SubObjectProp from that object, 
      // calling GetSubObject recursively if necessary (for example to retrieve 
      // "controls[10].controls[5].Font.Size")     
      function GetSubObjectEx(propname:string; var SubObjectProp: string): TObject; virtual;
      
      // The GetPropertyEx function receives a property name and must return the 
      // value of that property.
      // QuickTest calls GetPropertyEx (when GetSubObjectEx fails to find 
      // a relevant sub-item in the propname string) to retrieve unpublished 
      // properties or properties with a complex syntax. 
      // For example, GetPropertyEx can provide access to members of an array.
      function GetPropertyEx(propname:string; var Value: OleVariant): HRESULT; virtual;

    protected
      FObject: TObject;  // UI control object
      pParam: ^TServiceParam;  // Parameters passed from QTP
  end;

  /////////////////////////////////////////////////////////////////////////////
  //  Use TMicTControlAO as the base class for your Agent Object if you are  //
  //  creating support for Delphi objects that inherit from the              //
  //  TControl class.                                                        //
  //  The TMicTControlAO class provides access to the public                 //
  //  (but not published) 'Parent' member variable of TControl.              //
  /////////////////////////////////////////////////////////////////////////////
  
  TMicControlAO= class(TMicAO)
      protected
        function Get_Parent: TWinControl;
      published
        property Parent: TWinControl read Get_Parent;
  end;

  /////////////////////////////////////////////////////////////////////////////
  //  Use TMicTWinControlAO as the base class for your Agent Object if you   //
  //  are creating support for Delphi objects that inherit from the          //
  //  TWinControl class.                                                     //
  //  The TMicTWinControlAO class provides access to the public              //
  //  (but not published) 'Controls' collection of TWinControl.              //
  //                                                                         //
  //  The TMicTWinControlAO class provides a basic implementation for the    //
  //  GetSubObjectEx function, retrieving sub-items described in the format   //
  //  "Object.SubItem" or "ItemsArray[]". If your control uses a different   //
  //  syntax for collections of items, override GetSubObjectEx with an       //
  //  implementation more specific to your control's design.                 //
  /////////////////////////////////////////////////////////////////////////////   

  TMicWinControlAO= class(TMicControlAO)
      public
        function GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;  override;
      protected
        function GetControlCount: Integer;
        function Get_Handle:HWND;
        function Get_ParentWindow:HWND;
        function Get_Showing: Boolean;
        function Get_TabOrder: TTabOrder;
        function Get_TabStop: Boolean;
      published
        property ControlCount: Integer read GetControlCount;
        property Handle: HWnd read Get_Handle;
        property ParentWindow: HWnd read Get_ParentWindow;
        property Showing: Boolean read Get_Showing;
        property TabOrder: TTabOrder read Get_TabOrder;
        property TabStop: Boolean read Get_TabStop;
  end;


  // This structure is returned by ProcessMessage (the Agent Object's recording function)
  // and contains the operation and argument values for the recorded line
  // that needs to be added to the test or component, as well as the send mode for this line.
  TRecordLine= record
    Cmd: String;
    Params: array of OleVariant;
    SendMode: Integer;
  end;

  /////////////////////////////////////////////////////////////////////////////
  //  Use TMicRecordableAO as the base class for your Agent Object if you    //
  //  are creating support for Delphi objects that inherit from the          //
  //  TWinControl class, and you want to support the QuickTest recording     //
  //  capability on these objects.                                           //
  //  If you extend TMicRecordableAO, you must implement the ProcessMessage  //
  //  function. In this function, you must specify the step that QuickTest   //
  //  should add to the test or component for each event that can occur on   //
  //  the object. 
  /////////////////////////////////////////////////////////////////////////////

  TMicRecordableAO= class(TMicWinControlAO)
    protected
      // This is the Agent Object's recording function. QuickTest calls this method 
      // (via the __QTPRecording property defined below) when a recording session is active and 
      // an event occurs on the Delphi object supported by this Agent Object.
      // The ProcessMessage function receives a window message and must return the recorded line 
      // for QuickTest to add to the test or component and the relevant recording mode
      // (see Recording Mode Constants defined above).
      function ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean; virtual; abstract;
 
    private
      // Utilities (implemented by the TMicRecordableAO class)
      // GetMessageParm processes the event parameters passed by QuickTest.
      function GetMessageParm( var Msg: TMessage): Boolean; overload;
      function GetMessageParm( var handle: Cardinal; var Msg: TMessage): Boolean; overload;

      // PackRecordLineto converts the recording line you provide in ProcessMessage to the
      // format required by QuickTest.
      function PackRecordLine(Cmd: String; Params: array of OleVariant; SendMode: Integer): OleVariant;

      // Get_Recording is the function that handles the __QTPRecording published property.
      // It calls GetMessageParm to process the event parameters that QuickTest passes, 
      // calls ProcessMessage to determine the recorded line that needs to be returned 
      // to QuickTest and calls PackRecordLine (if relevant) to convert the recording 
      // line to the format required by QuickTest. 
      function Get_Recording :OleVariant ; virtual;
    
    published
      // The __QTPRecording property is used to support the QuickTest recording capability.
      // The property name is prefixed by a '__' to prevent the QuickTest Object Spy 
      // and Checkpoint Property dialog box from displaying it as a regular identification property.

      // When a recording session is active and an event occurs on the Delphi object supported 
      // by this Agent Object, QuickTest passes a window message to the Agent Object (in the 
      // TServiceParam format defined above) and retrieves line to record (and recording mode)
      // from the __QTPRecording published property (handled by Get_Recording described above.
      property __QTPRecording: OleVariant read Get_Recording;
  end;


  /////////////////////////////////////////////////////////////////////////////
  //  Use TMicGridAOBase as the base class for your Agent Object if you      //
  //  are creating support for a Delphi Grid object that does not inherit    //
  //  from the TCustomGrid class.                                            //
  //  If your Agent Object inherits from TMicGridAOBase, you must implement  //
  //  all of the abstract methods to perform the relevant operations on your //
  //  control.                                                               //
  /////////////////////////////////////////////////////////////////////////////

  TMicGridAOBase = class (TMicRecordableAO)
    protected
      // QuickTest calls these functions to perform operations on the Delphi 
      // object being tested.
      
      // Gets the value from the specified cell.
      function GetCellDataEx(row, col: Longint):string ; virtual; abstract; 
      
      // Sets the value in the specified cell.
      procedure SetCellDataEx(row, col: Longint; Value: String); virtual; abstract; 
      
      // Dumps grid content to the specified file.
      function CaptureTableEx(filename:string): boolean; virtual; abstract;
      
      // Returns the number of rows in the grid.
      function RowCountEx :Integer ; virtual; abstract;

      // Returns the number of columns in the grid.
      function ColCountEx :Integer ; virtual; abstract;
      
      // Returns the number of the row currently selected in the grid.
      function GetSelectedRowEx:Integer ; virtual; abstract;
      
      // Scrolls to the relevant area of the grid to make the specified cell visible.
      function MakeCellVisibleEx(row, col: Longint): bool; virtual; abstract; 
      
      // Receives the number of the row within the whole grid and returns 
      // the index of that row within the currently visible part of the grid.
      function GridIdxToVisibleRowNum( row_str:string ): Longint; virtual; abstract;
      
      // Gets the rectangle coordinates for the specified cell
      function GetCellRectEx(Row, Column: Integer; var rect: TRect): Boolean; virtual; abstract;

    protected
      // Utility functions that the Agent Object can use to parse the parameters 
      // that QuickTest passes to the Agent Object.
      
      // Parses cell information passed by QuickTest into two simple 
      // strings containing the row number and column number.
      function GetCellParm( var row_str,col_str:string ): bool;
      
      // Converts the row number string provided by QuickTest into an integer
      function RowStrToInt(row_str:string ): Longint;
      // Converts the column number string provided by QuickTest into an integer
      function ColumnStrToInt(col_str:string ): Longint;

      // Use this function to replace characters used in the grid content for text formatting  
      // before returning information to QuickTest
      function ReplaceIllegalCharacters (S:string):String;

      // Use these methods to access the string value contained in the TField object
      function GetStringFromTField(Field: TField):String;
      procedure SetStringToTField(Field: TField; Value: String);

    private
      // Functions that support the published properties
      function Get_CellRect :string ; virtual;
      function Get_CellData :string ;
      procedure Set_CellData(Value: string);
      function Get_RowCount :Integer ;
      function Get_ColCount :Integer ;
      function Get_SelectedCell :string ;
      function Get_TableContent:boolean;

    published
      // The following properties are used to support extensibility implementation.
      // The property names are prefixed by a '__' to hide them from the QuickTest  
      // Object Spy, checkpoints and output values. 
      
      // Returns the rectangle at which the cell is located, in the format: 
      // "x;y;width;height;;"  where x and y are the coordinates of the top left 
      // corner of the rectangle.
      property __CellRect: string read Get_CellRect;

      // Used for setting and retrieving the value contained in the cell.
      property __CellData: string  read Get_CellData write Set_CellData;

      // Dumps the content (data) of the whole table to the specified file.
      // - Returns true or false indicating success or failure. 
      // - File is specified in the parameter passed to the Agent Object.
      // - Table content is written to the file in string format, with 
      //   tabs separating cell data and new-line characters separating rows. 
      property __TableContent: boolean read Get_TableContent;

      // These properties are visible as Run-time object properties
      property RowCount: Integer read Get_RowCount;
      property ColCount: Integer read Get_ColCount;
      // Returns the content of the currently selected cell. 
      property SelectedCell: string read Get_SelectedCell;
  end;

  
  /////////////////////////////////////////////////////////////////////////////
  //  Use TCustomGridAOBase as the base class for your Agent Object if you   //
  //  are creating support for Delphi grid objects that inherit from the     //
  //  TCustomGrid class.                                                     //
  //  This class provides a basic implementation for several of the functions //
  //  required to support testing on grid objects (including support for the //
  //  recording capability).                                                 //
  //  If your Agent Object inherits from TCustomGridAOBase, you must         //
  //  implement the following abstract methods (defined in TMicGridAOBase)   //
  //  to perform the relevant operations on your control:                    //
  //  SetCellDataExt, GetCellDataExt, GridIdxToVisibleRowNum.                      //
  /////////////////////////////////////////////////////////////////////////////
  
  TCustomGridAOBase = class (TMicGridAOBase)
    protected
      // Recording function
      function ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean; override;

      function RowCountEx :Integer ; override;
      function ColCountEx :Integer ; override;
      function GetSelectedRowEx:Integer ; override;
      function MakeCellVisibleEx(row, col: Longint): bool; override;
      function GetCellRectEx(Row, Column: Integer; var rect: TRect): Boolean; override;
      function CaptureTableEx(filename:string): boolean; override;

      // GetCellFromPoint retrieves the column number and row number of the cell 
      // located at the specified point.
      function GetCellFromPoint( obj: TObject; p:Tpoint; var row, col: Longint ): Boolean;   
  end;

  /////////////////////////////////////////////////////////////////////////////
  // This is the format for an Agent Object factory function.                // 
  // The function receives a VCL UI control and decides what type of Agent   //
  // Object is required to support the control. The factory function then    //
  // creates the Agent Object and returns it.                                //
  // The Agent Object must be a TMicAO descendant.                           //
  // QuickTest calls the factory function to supply the necessary Agent      //
  // Object when it encounters a Delphi object not supported by the Delphi   //
  // Add-in.                                                                 //
  /////////////////////////////////////////////////////////////////////////////
  TCreateAOFromHandle = function(obj:TObject;var AgentObject :TMicAO):HRESULT; stdcall;
  
  
  /////////////////////////////////////////////////////////////////////////////
  // You must use this API function to register your factory function to the //
  // Delphi Add-in.                                                          //   
  // It is recommended that you call this function in the initialization     //
  // section of your Extensibility unit.                                     //
  /////////////////////////////////////////////////////////////////////////////
  procedure AddExtensibilityServer(VCLObj2AO: TCreateAOFromHandle);


  /////////////////////////////////////////////////////////////////////////////
  //  The functions below are utility functions that you can call in your    //
  //  extensibility code.                                                    //
  /////////////////////////////////////////////////////////////////////////////

  // The IsInheritFrom function checks whether the class inheritance branch of the  
  // specified object contains the specified class.
  function IsInheritFrom(Obj:TObject;AClass: String): WordBool;
 
  // The MicStrToInt function wraps the StrToInt function and handles exceptions thrown 
  // by StrToint upon failure.
  function MicStrToInt(S:string):Longint;

  // The TypeKind2VarType function converts Delphi Run-Time Type Information (RTTI) type to Variant type.
  function TypeKind2VarType(Kind: TTypeKind): TVarType;

  // The ParseIndexedProperty function receives a property in the format: Property[Index] 
  // and returns the relevant Property and Index
  function ParseIndexedProperty(RawProp:string; var PropName: string; var Index: Integer): Boolean;
  
  
  // The ParseSubProperty function receives a property in the format: Property.SubProperty 
  // and returns the relevant Property and SubProperty
  function ParseSubProperty( RawProp: string; var PropertyName: string; var SubPropertyName: string): Boolean;


//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //   
//                        END OF API DECLARATIONS AND DOCUMENTATION                     //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


implementation
uses
  ComCtrls, Grids,     
  DlphVDEPAgentImpl,
  MicAOFactoryMgr;


procedure AddExtensibilityServer(VCLObj2AO: TCreateAOFromHandle);
begin
  GetAOFactoryMgr.AddExtensibilityServer(VCLObj2AO);
end;

///////////////////////////////////////////////////////////////////////////////////////
//  TMicAO Implementation
procedure TMicAO.Init(obj: TObject; var Param: TServiceParam);
begin
  FObject:= obj;
  pParam:= @Param;
end;

function TMicAO.GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;
begin
    Result:= Nil;
end;

function TMicAO.GetPropertyEx(propname:string; var Value: OleVariant): HRESULT;
begin
    Result:= E_NOTIMPL;
end;

///////////////////////////////////////////////////////////////////////////////////////
//  TMicRecordableAO Implementation
function TMicRecordableAO.GetMessageParm( var Msg: TMessage): Boolean;
var handle: Cardinal;
begin
 Result:=  GetMessageParm(handle, Msg);
end;

function TMicRecordableAO.GetMessageParm( var handle: Cardinal; var Msg: TMessage): Boolean;
begin
  Result := False;
  if (pParam = nil) or (pParam^.NumOfParams <> 4) then exit;
  handle:=  pParam^.Parameters[0];
  Msg.Msg:= pParam^.Parameters[1];
  Msg.wParam:=pParam^.Parameters[2];
  Msg.lParam:=pParam^.Parameters[3];
  Result := True;
end;

function TMicRecordableAO.Get_Recording :OleVariant;
var Msg: TMessage;
    Handle: Cardinal;
    RecLine: TRecordLine;
begin
  Result:= S_FALSE;
  if (not (GetMessageParm(Handle, Msg))) then exit;
  if ProcessMessage(Handle, Msg, RecLine) then
      Result:= PackRecordLine(RecLine.Cmd, Recline.Params, RecLine.SendMode);
end;

function TMicRecordableAO.PackRecordLine(Cmd: String; Params: array of OleVariant; SendMode: Integer): OleVariant;
begin
  Result:= _PackRecordLine(Cmd, Params, SendMode);
end;

///////////////////////////////////////////////////////////////////////////////////////
//  TMicControlAO Implementation
function TMicControlAO.Get_Parent: TWinControl;
begin
  Result:= TControl(FObject).parent;
end;

///////////////////////////////////////////////////////////////////////////////////////
//  TMicWinControlAO Implementation
function TMicWinControlAO.GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;
    var
      SubObject: String;
      Index: Integer;
begin
  Result:= inherited GetSubObjectEx(propname, SubObjectProp);
  if Result <> nil then exit;
  if not ParseSubProperty(propname, SubObject, SubObjectProp) then exit;
  if not ParseIndexedProperty(SubObject, SubObject, Index) then exit;

  if(UpperCase(SubObject)='CONTROLS') and
    (Index >= 0) and (Index < TToolBar(FObject).ButtonCount) then
	  Result:= TWinControl(FObject).Controls[Index];
end;

function TMicWinControlAO.GetControlCount: Integer;
begin
  Result:= TWinControl(FObject).ControlCount;
end;

function TMicWinControlAO.Get_Handle:HWND;
begin
  Result:= TWinControl(FObject).Handle;
end;

function TMicWinControlAO.Get_ParentWindow:HWND;
begin
  Result:= TWinControl(FObject).ParentWindow;
end;

function TMicWinControlAO.Get_Showing: Boolean;
begin
  Result:= TWinControl(FObject).Showing;
end;

function TMicWinControlAO.Get_TabOrder: TTabOrder;
begin
  Result:= TWinControl(FObject).TabOrder;
end;

function TMicWinControlAO.Get_TabStop: Boolean;
begin
  Result:= TWinControl(FObject).TabStop;
end;

///////////////////////////////////////////////////////////////////////////////////////
//  Utilities
function IsInheritFrom(Obj:TObject;AClass: String): WordBool;
var
  P: TClass;
begin
  Result := true;
  P := Obj.ClassType;
  repeat
    if (CompareText(UpperCase(P.ClassName), UpperCase(AClass)) = 0) then exit;
    P := P.ClassParent;
  until (P = nil);
  Result := false;
end;

function MicStrToInt(S:string):Longint;
var E:Integer;
begin
  Result :=ERR_INT;
  try
    Val(S, Result, E);
    if E<>0 then Result :=ERR_INT;
//    Result :=strtoint(S);
  except
  end;
end;

function TypeKind2VarType(Kind: TTypeKind): TVarType;
const
    VT_EMPTY           = 0;
    VT_I2              = 2;   { [V][T][P]  2 byte signed int           }
    VT_I4              = 3;   { [V][T][P]  4 byte signed int           }
    VT_R4              = 4;   { [V][T][P]  4 byte real                 }
    VT_BSTR            = 8;   { [V][T][P]  binary string               }
    VT_VARIANT         = 12;  { [V][T][P]  VARIANT FAR*                }
    VT_UNKNOWN         = 13;  { [V][T]     IUnknown FAR*               }
    VT_I1              = 16;  {    [T]     signed char                 }
    VT_UI1             = 17;  {    [T]     unsigned char               }
    VT_HRESULT         = 25;  {    [T]                                 }
    VT_PTR             = 26;  {    [T]     pointer type                }
    VT_SAFEARRAY       = 27;  {    [T]     (use VT_ARRAY in VARIANT)   }
    VT_I8              = 20;  {    [T][P]  signed 64-bit int           }
begin
  case Kind of
     tkUnknown:
        Result:= VT_EMPTY;
     tkInteger:
        Result:= VT_I4;
     tkChar:
        Result:= VT_UI1;
     tkEnumeration:
        Result:= VT_I2;
     tkFloat:
        Result:= VT_R4;
     tkClass, tkMethod, tkRecord:
        Result:= VT_PTR;
     tkString, tkSet, tkWChar, tkLString, tkWString:
        Result:= VT_BSTR;
     tkVariant:
        Result:= VT_VARIANT;
     tkArray, tkDynArray:
        Result:= VT_SAFEARRAY;
     tkInterface:
        Result:= VT_UNKNOWN;
     tkInt64:
        Result:= VT_I8
     else
        Result:= VT_EMPTY;
  end;
end;


function ParseIndexedProperty(RawProp:string; var PropName: string; var Index: Integer): Boolean;
var
  idx,idx2:Integer;
begin
  Result:= False;
  Index:=-1;
  PropName:= '';

  idx:=pos('[',RawProp);
  if (idx>0) then
    begin
      idx2:=pos(']',RawProp);
      PropName:=Copy(RawProp,idx+1,idx2-idx-1);
      Delete(RawProp,idx,Length(RawProp));
      Index:=MicStrToInt(RawProp);
    end;
  if (Index < 0) or (PropName = propname) then exit;

  Result:= True;
end;

function ParseSubProperty( RawProp: string; var PropertyName: string; var SubPropertyName: string): Boolean;
var idx: Integer;
begin
  Result:= False;
  idx:=pos('.',RawProp);
  if (idx>0) then
  begin
    SubPropertyName:=Copy(RawProp,idx+1,Length(RawProp));
    Delete(RawProp,idx,Length(RawProp));
    PropertyName:= RawProp;
    Result:= True;
  end;
end;

///////////////////////////////////////////////////////////////////////////////////////
//  TMicGridAOBase
function TMicGridAOBase.GetCellParm( var row_str,col_str:string ): bool;

function Var2CellStr(v:OleVAriant): String;
begin
  Result:=  v;
  if MicStrToInt(Result) <> ERR_INT then
    Result:= '#' + Result;
end;

begin
  Result:= false;
  if pParam^.NumOfParams < 2 then exit;
  row_str:=Var2CellStr(pParam^.Parameters[0]);
  col_str:=Var2CellStr(pParam^.Parameters[1]);
  Result := True;
end;

function TMicGridAOBase.RowStrToInt(row_str:string ): Longint;
begin
Result :=-1;

if (Length(row_str)=0) then
  exit;

if (row_str[1]='#') then
   begin
      row_str[1]:=' ';
      Result :=MicStrToInt(row_str);
      exit;
   end;
end;

function TMicGridAOBase.ColumnStrToInt(col_str:string ): Longint;
var
i:Integer;
begin
Result :=-1;

if (Length(col_str)=0) then
  exit;

if (col_str[1]='#') then
   begin
      col_str[1]:=' ';
      Result :=MicStrToInt(col_str);
      exit;
   end;

for i:=0 to ColCountEx() do
 if (GetCellDataEx(0,i)=col_str) then
    begin
         Result :=i;
    end;
end;

function TMicGridAOBase.Get_CellRect() :string ;
var
row_str,col_str:string;
row,column:Longint;
rect: TRect;
begin
 Result := '';
 GetCellParm(row_str,col_str);
 row:=GridIdxToVisibleRowNum(row_str);
 column:=ColumnStrToInt(col_str);

 if GetCellRectEx(row, column, rect) then
   Result:= inttostr(rect.Left)+';'+ inttostr(rect.Top)+';'+inttostr(rect.Right)+';'+inttostr(rect.Bottom)+';;';
end;


function TMicGridAOBase.Get_CellData() :string ;
var
row_str,col_str:string;
row,column:Longint;
begin
 Result := ' ';
 GetCellParm(row_str,col_str);
 row:=RowStrToInt(row_str);
 column:=ColumnStrToInt(col_str);
 if ((row<0) or (column<0)) then  raise EInvalidArgument.Create(' ');
 Result:= GetCellDataEx(row,column);
end;

procedure TMicGridAOBase.Set_CellData(Value: string);
var
row_str,col_str:string;
row,column:Longint;
begin
 GetCellParm(row_str,col_str);
 row:=RowStrToInt(row_str);
 column:=ColumnStrToInt(col_str);
 if ((row<0) or (column<0)) then raise EInvalidArgument.Create(' ');
 SetCellDataEx(row, column, Value);

end;

function  TMicGridAOBase.Get_RowCount() :Integer ;
begin
     Result :=RowCountEx();
end;

function  TMicGridAOBase.Get_ColCount() :Integer ;
begin
     Result :=ColCountEx();
end;

function  TMicGridAOBase.Get_SelectedCell() :string ;
var
col_str,row_str:string;
begin
 row_str:='#'+inttostr(GetSelectedRowEx());
 col_str:= GetCellDataEx(0,TDrawGrid(FObject).Col);
 if ((Length(col_str)=0) or (Length(col_str) > MIC_MAX_HEADER_LENGTH)) then
     col_str:='#'+inttostr(TDrawGrid(FObject).Col);

 Result :=row_str+';'+col_str+';;';
end;

function TMicGridAOBase.Get_TableContent(): boolean;
var
filename:string;
begin
  Result:= false;
  if pParam^.NumOfParams < 1 then exit;
  filename:=pParam^.Parameters[0];
  Result :=CaptureTableEx(filename);
end;


function TMicGridAOBase.ReplaceIllegalCharacters (S:string):String;
begin
	while Pos(#13, S) > 0 do
	    S[Pos(#13, S)] := ' ';
	while Pos(#10, S) > 0 do
	    S[Pos(#10, S)] := ' ';
	while Pos(#9, S) > 0 do
	    S[Pos(#9, S)] := ' ';
   	while Pos(#0, S) > 0 do
   		S[Pos(#0, S)] := ' ';
	Result :=S;
end;

function TMicGridAOBase.GetStringFromTField(Field: TField):String;
begin
  try
   case Field.DataType  of
       ftMemo: Result := Field.AsString;
   else
       Result := Field.DisplayText;
  end;
  except
   Result := FAIL_TO_GET_CELL_VALUE;
   end;
end;

Procedure TMicGridAOBase.SetStringToTField(Field: TField; Value: String);
begin
  try
    Field.Value:=  Value;
  except
  end;
end;
//  end of TMicGridAOBase implementation
///////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////
//  TCustomGridAOBase implementation
function TCustomGridAOBase.GetCellFromPoint( obj: TObject; p:Tpoint; var row, col: Longint ): Boolean;
var
  rect: TRect;
  i, j: Integer;
begin
  Result := False;
  col :=-1;
  row :=-1;
   for i := 0 to TDrawGrid(obj).ColCount - 1 do
      begin
        for j := 0 to TDrawGrid(obj).RowCount - 1 do
        begin
          rect := TDrawGrid(obj).CellRect(i, j);
          if  ((rect.Top <= p.y) and (rect.Left <= p.x) and
              (rect.Bottom > p.y) and (rect.Right > p.x) ) then
               begin
                 col := i;
                 row := j;
                 Result := TRUE;
                 exit;
               end;

        end; // for j
    end; // for i
end;

function TCustomGridAOBase.RowCountEx() :Integer ;
begin
Result := TDrawGrid(FObject).RowCount;
end;

function TCustomGridAOBase.ColCountEx() :Integer ;
begin
Result := TDrawGrid(FObject).ColCount;
end;

function TCustomGridAOBase.GetSelectedRowEx():Integer ;
begin
Result := TDrawGrid(FObject).row;
end;

function TCustomGridAOBase.MakeCellVisibleEx(row, col: Longint): bool;
begin
     TDrawGrid(FObject).LeftCol :=  col ;
     Result:=True;
end;


function TCustomGridAOBase.GetCellRectEx(Row, Column: Integer; var rect: TRect): Boolean;
begin
  Result:= False;
  if ((Row<0) or (Column<0)) then exit;
    rect:=TDrawGrid(FObject).CellRect(Column,Row);
  if ((rect.Left=0) and (rect.Top=0)) then
    begin
      MakeCellVisibleEx(Row,Column);
      rect:=TDrawGrid(FObject).CellRect(Column,Row);
    end;

  Result:= True;
end;

var
lest_click: DWORD;
IsDbClick :BOOL;
LastCol, LastRow: Integer;
LastCellText:string;
Editing: Boolean;
function  TCustomGridAOBase.ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean;

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

var
  p:TPoint;
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

    if (Msg.Msg=WM_LBUTTONDOWN) then
         IsDbClick:= FALSE
    else
    if ( (Msg.Msg=WM_LBUTTONUP) or
       (Msg.Msg=WM_LBUTTONDBLCLK) ) then
    begin
      p.x:=LOWORD(Msg.lParam);
      p.y:=HIWORD(Msg.lParam);
      if (not (GetCellFromPoint(FObject,p,row,Column))) then exit;

      LastCol:= Column;
      LastRow:= Row;

      if ((Msg.Msg=WM_LBUTTONDBLCLK) or
         ((GetTickCount()-lest_click)<500)) then
      begin
         PrepareCellRecordLine('ActivateCell', Row, Column, SEND_LINE, RecLine);
         Result:= true;
         IsDbClick:= TRUE;
      end
      else
        if not (IsDbClick) then
        begin
          PrepareCellRecordLine('SelectCell', Row, Column, SEND_LINE, RecLine);
          Result:= true;
        end;
        lest_click:= GetTickCount();

     end
     else
     if Msg.Msg = WM_KEYDOWN then
     begin
       LastCol:=  TDrawGrid(FObject).Col;
       LastRow:=  TDrawGrid(FObject).Row;

       if Msg.WParam = VK_RETURN then
       begin
         PrepareCellRecordLine('ActivateCell', LastRow, LastCol, SEND_LINE, RecLine);
         Result:= true;
       end;

     end
     else
     if Msg.Msg = WM_KEYUP then
     begin
       if (LastCol<>TDrawGrid(FObject).Col) or (LastRow<>TDrawGrid(FObject).Row) then
       begin
          PrepareCellRecordLine('SelectCell', TDrawGrid(FObject).Row,
                                      TDrawGrid(FObject).Col, SEND_LINE, RecLine);
          Result:= true;
       end
     end
     else
     if (Msg.Msg = WM_KILLFOCUS) and (GetParent(Msg.WParam) = TWinControl(FObject).Handle) then
     begin // Editor activated
       LastCol:=  TDrawGrid(FObject).Col;
       LastRow:=  TDrawGrid(FObject).Row;
       LastCellText:= GetCellDataEx(LastRow, LastCol);
       Editing:= true;
     end
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
            Result:= true;
            Editing:= false;
            LastCol:=  TDrawGrid(FObject).Col;
            LastRow:=  TDrawGrid(FObject).Row;
          end;
        end;

     end;

end;


function  TCustomGridAOBase.CaptureTableEx(filename:string): boolean;
var
F : TextFile;
i,j: Integer;
line: string;
begin
Result :=True;
if (Length(filename)=0) then  exit;
AssignFile(F, fileName);
Rewrite(F);
for i:=0 to Get_RowCount() do
 begin
    line:='';
    for j:=0 to Get_ColCount() do
       line := line+ ReplaceIllegalCharacters(GetCellDataEx(i,j)) + #9;

    line:=copy (line,1,Length(line)-2);  // remove the lest #9
    Writeln(F, line);
 end;
 CloseFile(F);
end;

end.
