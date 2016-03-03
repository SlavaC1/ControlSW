// This unit is a template for the Delphi Extensibility code that you develop.
// For more information, see <QuickTest Professional Delphi Add-in Extensibility 
// installation folder>\dat\Extensibility\Delphi\AgentExtensibilitySDK.pas. 


unit ExtensibilityImplementationTemplate;
{TODO: Change the name of the unit according to the set of 
custom controls for which you are creating support}

interface
  uses AgentExtensibilitySDK;

type
  // TODO: In this section, define your own Agent Objects. Create an Agent Object for
  // each type of control that you want to support. 
  // Each Agent Object must inherit from TMicAO or one of its descendants.
  // The AgentExtensibilitySDK contains some useful base classes for your Agent Object:  
  // TMicControlAO:    Provides access to the 'Parent' property. 
  //                   Use this base class for Agent Objects that support controls
  //                   that inherit from TControl but not TWinControl, like TImage, 
  //                   TSpeedButton, and TLabel.
  // TMicWinControlAO: Provides access to the 'Controls' collection and the 
  //                   'ControlCount' and 'Parent' properties. 
  //                   Use this base class for Agent Objects that support controls
  //                   that inherit from TWinControl, like most of the VCL controls.
  // TMicRecordableAO: Inherits from TMicWinControlAO and provides much of the  
  //                   implementation required to support recording. 
  //                   To complete the recording support, you need to override only
  //                   the ProcessMessage function. 
  //                   Use this base class for Agent Objects that support 
  //                   controls that inherit from TWinControl, and must support
  //                   custom recording.
  // TMicGridAOBase:   Use this base class for Agent Objects that support grid 
  //                   controls that do not inherit from TCustomGrid.
  //                   This class implements basic grid properties used by the 
  //                   Mercury.DelphiTableSrv test object extension that QuickTest  
  //                   uses internally to support Delphi Table test objects 
  //                   (for more information see the Delphi Extensibility Developer's Guide).
  // TCustomGridAOBase: Inherits from TMicGridAOBase and provides much of the  
  //                    implementation required to support basic grid 
  //                    properties and operations.
  //                    Use this base class for Agent Objects that support grid 
  //                    controls that inherit from TCustomGrid.
  //                    To support simple custom grid controls (similar to 
  //                    TStringGrid, for example) you need only design an Agent   
  //                    Object that inherits from TCustomGridAOBase and implement
  //                    the GetCellData, SetCellData, and GridIdxToVisibleRowNum functions.
  TUserControl1AO=class(TMicRecordableAO)
    protected
    // Recording function
      function ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean; override;

    // Run functions - the functions that support test object methods.
      function RunMethod1:HRESULT;
      function RunMethod2:HRESULT;

    // Getters and setters for published properties
      function GetMyVisibleProperty:Integer;
      function GetMyHiddenPropery:OleVariant;
      procedure SetMyHiddenPropery(Param: OleVariant);

    // Provide access to properties with custom syntax (if necessary)
      function GetSubObjectEx(propname:string; var SubObjectProp: string): TObject;  override;
      function GetPropertyEx(propname:string; var Value: OleVariant): HRESULT; override;

    published
    // QuickTest uses published properties that begin with prefix __QTPReplayMtd_ to 
    // access the functions that support test object methods.
    // QuickTest passes the test object method arguments using the pParams member
    // variable of the TMicAO base class. (See TMicAO definition in <QuickTest 
    // Professional Delphi Add-in Extensibility installation folder>\dat\
    // Extensibility\Delphi\AgentExtensibilitySDK.pas for more detail.)
      property __QTPReplayMtd_Method1:HRESULT read RunMethod1;
      property __QTPReplayMtd_Method2:HRESULT read RunMethod2;

    // Use a double underscore (’__’) prefix for names of properties that you do 
    // not want displayed in the Object Spy or accessed by checkpoints and
    // output values. Such hidden properties can be accessed by the GetROProperty
    // and SetROProperty test object methods, and can be accessed directly  
    // in user defined functions created in QuickTest.
      property __MyHiddenPropery: OleVariant read GetMyHiddenPropery write SetMyHiddenPropery;

    // All (non-hidden) published properties of the Agent Object and the VCL 
    // control that it supports are displayed in the QuickTest Object Spy 
    // and can be accessed by checkpoints, output values, and so forth. 
    // You can add properties in this section to enable access to (public) 
    // unpublished member variables of the control or for any other purpose.
      property MyVisibleProperty: Integer read GetMyVisibleProperty;
  end;



implementation
uses Windows; {TODO: Add here the unit in which the control is defined
               and any other units that you need}


var g_bLeftButtonPressed: Boolean;

// Recording function
function TUserControl1AO.ProcessMessage(handle: Cardinal; Msg: TMessage; var RecLine:TRecordLine):Boolean;
begin
  Result:= False;

  // The code below implements the recording of a simple Click operation, 
  // demonstrating how to recognize messages and implement recording. 
  // TODO: Replace the code below with your own implementation, to record 
  // events that might occur on your control.
  if Msg.Msg=WM_LBUTTONDOWN then
     g_bLeftButtonPressed:= true;
  else if (Msg.Msg=WM_LBUTTONUP) and g_bLeftButtonPressed then
  begin
    g_bLeftButtonPressed:= false;
    RecLine.Cmd:= 'Click'; // Test object method name
    SetLength(RecLine.Params, 0); // Initialize parameters array - no parameters
                                  // for the click test object method
    RecLine.SendMode:= SEND_LINE; // Specify the Recording Mode
    Result:= true; // Return true to instruct QuickTest to record the step
  end;
end;

// Run functions
function TUserControl1AO.RunMethod1:HRESULT;
var Param1: Integer;
begin
  Result:= E_FAIL;

  // Check the number of parameters and their values
  if pParam^.NumOfParams <> 1 then exit;
  Param1:= pParam^.Parameters[0];

  // TODO: Run the operation on the control
  // Use <TUserControlName>(FObject) to access the control's member variables, properties and methods.
  // (Replace <TUserControlName> with the name of the control type you are supporting.)

  // Return success
  Result:= S_OK;
end;

function TUserControl1AO.GetMyHiddenPropery:OleVariant;
begin
  // TODO: Implement the property query
  // Use <TUserControlName>(FObject) to access the control's member variables, properties and methods.
  // (Replace <TUserControlName> with the name of the control type you are supporting.)

  Result:= { TODO: Value retrieved from the control};
end;

// TODO: Implement functions to run all of the necessary test object methods
//       and to retrieve all necessary property values.



// For more information on the GetSubObjectEx and GetPropertyEx functions, see  
// <QuickTest Professional Delphi Add-in Extensibility installation folder>\dat\
// Extensibility\Delphi\AgentExtensibilitySDK.pas.

// The GetSubObjectEx function receives a string that represents a property  
// that might belong to an object contained within the control being handled. 
// For example: "controls[10].Name"
// GetSubObjectEx is supposed to return the relevant sub-object (TObject)
// if it exists, and the remaining part of the passed string, representing
// the property within that item (in the example above, the 10th control and "Name").

function TUserControl1AO.GetSubObjectEX(propname:string; var SubObjectProp: string): TObject;
    var
      PropertyName: String;
      Index: Integer;
begin
  Result:= inherited GetSubObjectEx(propname, SubObjectProp); // Do not remove this line!
  if Result <> nil then exit;

  // TODO: You can create your own code to parse the 'propname' argument. 
  // The code below processes the Array[Index].SubProp syntax, where the Array 
  // is a member variable, an array of objects.
  if not ParseSubProperty(propname, PropertyName, SubObjectProp) then exit;
  if not ParseIndexedProperty(PropertyName, PropertyName, Index) then exit;

  // Return the appropriate item from the array
  if UpperCase(PropertyName)=UpperCase('UserControlArrayName') then
	  Result:= TUserControl(FObject).UserControlArrayName[Index];
end;

// The GetPropertyEx function receives a complex property name and must return the 
// value of that property.
// For example, GetPropertyEx can provide access to members of an array.
function TUserControl1AO.GetPropertyEx(propname:string; var Value: OleVariant): HRESULT;
begin
   // TODO: Parse the 'propname' parameter and return the value of the property. 
end;



// Factory function. QuickTest calls the factory function to recognize the 
// control and create the necessary Agent Object when it encounters a Delphi 
// object not supported by the Delphi Add-in. 
function MicVCLObj2AO(obj:TObject;var ao :TMicAO):HRESULT; stdcall;

begin
// TODO: In most cases, using the IsInheritFrom API function to check which class 
//       the control inherits is sufficient to recognize the control. 
//       However, you can design your factory function to recognize your controls 
//       and decide which Agent Object to create to support them in any way you see fit.        

if IsInheritFrom(obj,'TUserControl') then   // TODO: replace TUserControl with the 
                                             //       type of control you are supporting 
  begin
     ao:=  TUserControl1AO.Create; // Create the new Agent Object
     Result:= S_OK;
  end
  else
     Result:= S_FALSE;  // Return S_FALSE if the factory agent does not recognize
                        // control as a control for which you developed support.
end;

begin
  // Initialize global variables
  g_bLeftButtonPressed:= false;

  // Register your factory function to the QuickTest Delphi Add-in.
  AddExtensibilityServer(@MicVCLObj2AO);
end.


