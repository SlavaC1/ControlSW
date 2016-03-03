//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "stdio.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
        : TForm(Owner)
{
  VersionLabel->Caption = GetAppVersionStr();
}
//---------------------------------------------------------------------------

AnsiString TAboutForm::GetAppVersionStr(void)
{
  char  AppFullPath[MAX_PATH];
  DWORD dwVerHnd;
  DWORD dwVerInfoSize;
  UINT  uiVerSize;
  DWORD *lpBuffer;
  AnsiString sVer;

  GetModuleFileName(NULL, AppFullPath, sizeof(AppFullPath));
  dwVerInfoSize = GetFileVersionInfoSize(AppFullPath, &dwVerHnd);

  if(dwVerInfoSize)
  {
    // If we were able to get the information, process it:
    HANDLE hMem;
    LPSTR lpvMem;
    char szGetName[MAX_PATH];

    lpvMem = new char[dwVerInfoSize];
    ::GetFileVersionInfo(AppFullPath, dwVerHnd, dwVerInfoSize, lpvMem);

    // Get the translation information.
    ::VerQueryValue(lpvMem,"\\VarFileInfo\\Translation",(void**)&lpBuffer,&uiVerSize);

    sprintf(szGetName,"\\StringFileInfo\\%04hX%04hX\\FileVersion",LOWORD(*lpBuffer),HIWORD(*lpBuffer));

    BOOL fRet;
    UINT cchVer = 0;
    LPSTR lszVer = NULL;
    char szResult[256];

    fRet = ::VerQueryValue(lpvMem, szGetName, (LPVOID*)&lszVer, &cchVer);

    if(fRet && (cchVer > 0) && (lszVer != NULL))
      sVer = lszVer;
    else
      sVer = "";

    delete []lpvMem;
  }

  return sVer;
}
void __fastcall TAboutForm::Button1Click(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------
