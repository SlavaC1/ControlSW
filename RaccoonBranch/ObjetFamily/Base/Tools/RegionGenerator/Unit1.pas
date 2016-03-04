unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ExtDlgs;

type
  TForm1 = class(TForm)
    Button1: TButton;
    SaveDialog1: TSaveDialog;
    OpenDialog1: TOpenPictureDialog;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
    Bmp : TBitmap;

    function IsTransparent(x,y : integer) : boolean;

    function GenerateRegion : HRGN;

    procedure SaveRegionToFile(Rgn : HRGN; FileName : string);
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

const
  LINE_WIDTH = 14;

procedure TForm1.Button1Click(Sender: TObject);
var
  Rgn : HRGN;
begin
  Bmp := TBitmap.Create;

  if OpenDialog1.Execute then
  begin
    Bmp.LoadFromFile(OpenDialog1.FileName);

    if SaveDialog1.Execute then
    begin
      Rgn := GenerateRegion;
      SaveRegionToFile(Rgn,SaveDialog1.FileName);
      DeleteObject(Rgn);
    end;
  end;
end;

function TForm1.IsTransparent(x,y : integer) : boolean;
begin
  // Color Changed to Purple (Eden500) 
  Result := Bmp.Canvas.Pixels[x,y] = $00960096;//clWhite;
//  Result := Bmp.Canvas.Pixels[x,y] = clLime;//clWhite;
end;

function TForm1.GenerateRegion : HRGN;
var
  RGN: HRGN;
  X,XStart,Y: Integer;
  TRP: Boolean;
begin
  Result:=CreateRectRgn(0,0,0,0);
  for Y:=0 to Bmp.Height - 1 do
  begin
    XStart:=0;
    TRP:=IsTransparent(0,Y);
    for X:=0 to Bmp.Width - 1 do
    begin
      if IsTransparent(X,Y)<>TRP then
      begin
        if TRP then XStart:=X
        else
        begin
          RGN:=CreateRectRgn(XStart,Y,X,Succ(Y));
          try
            //with TForm(Owner),ClientOrigin do OffsetRgn(RGN,X-Left,Y-Top);
            CombineRgn(Result,Result,RGN,RGN_OR);
          finally
            DeleteObject(RGN);
          end;
        end;
        TRP:=IsTransparent(X,Y);
      end;
    end;
    if not TRP then
    begin
      RGN:=CreateRectRgn(XStart,Y,Bmp.Width,Succ(Y));
      try
//        OffsetRgn(RGN,X-Left,Y-Top);
        CombineRgn(Result,Result,RGN,RGN_OR);
      finally
        DeleteObject(RGN);
      end;
    end;
  end;
end;

procedure TForm1.SaveRegionToFile(Rgn : HRGN; FileName : string);
var
  StrList : TStringList;
  BufferSize : longint;
  r,Buf : pByte;
  i,p : integer;
  s : string;
begin
  StrList := TStringList.Create;

  BufferSize := GetRegionData(Rgn,0,nil);
  GetMem(Buf,BufferSize);
  GetRegionData(Rgn,BufferSize,PRgnData(Buf));

  r := Buf;

  try
    StrList.Clear;

    StrList.Add('// Automatically generated by RegionGenerator');
    StrList.Add('#ifndef _REGION_DEF_H_');
    StrList.Add('#define _REGION_DEF_H_');
    StrList.Add('');
    StrList.Add('const unsigned REGION_BUFFER_SIZE = ' + IntToStr(BufferSize));
    StrList.Add('');

    s := 'unsigned char Region[REGION_BUFFER_SIZE] = {';
    StrList.Add(s);

    s := '';
    p := 0;

    for i := 0 to BufferSize - 1 do
    begin
      s := s + '0x' + IntToHex(r^,2);
      Inc(r);

      if i < BufferSize - 1 then
        s := s + ',';

      Inc(p);

      if p = LINE_WIDTH then
      begin
        p := 0;
        StrList.Add(s);
        s := '';
      end;
    end;

    if p > 0 then
      StrList.Add(s + '};')
    else
    begin
      StrList.Strings[StrList.Count - 1] := StrList.Strings[StrList.Count - 1] + '};';
    end;

    StrList.Add('');
    StrList.Add('#endif');
    StrList.Add('');


    StrList.SaveToFile(FileName);
  finally
    StrList.Free;

    if BufferSize > 0 then
      Dispose(Buf);
  end;
end;

end.