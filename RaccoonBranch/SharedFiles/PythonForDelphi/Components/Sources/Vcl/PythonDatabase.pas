unit PythonDatabase;

{$I Definition.Inc}

interface
uses Classes, PythonEngine;
type
  TPythonDatabase = class(TEngineClient)
    protected
      procedure SetEngine( val : TPythonEngine ); override;
    public
      constructor Create( AOwner : TComponent ); override;
  end;

  procedure Register;

implementation
uses pyDB, pyDBTables;

procedure Register;
begin
  RegisterComponents('Python',[TPythonDatabase]);
end;

constructor TPythonDatabase.Create( AOwner : TComponent );
begin
  inherited;
  if not (csDesigning in ComponentState) then
    begin
      pyDB.CreateComponents(Self);
      pyDBTables.CreateComponents(Self);
    end;
end;

procedure TPythonDatabase.SetEngine( val : TPythonEngine );
var
  i : Integer;
begin
  inherited;
  for i := 0 to ComponentCount-1 do
    if Components[i] is TEngineClient then
      with TEngineClient(Components[i]) do
        Engine := val;
end;

end.