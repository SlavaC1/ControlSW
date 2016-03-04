program TestProject;

uses
  Forms,
  Test in 'C:\DevTools\Borland\Delphi7\Projects\Test.pas' {Form1};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
