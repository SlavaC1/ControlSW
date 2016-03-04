/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Cross-Platform application version           *
 *                     information service.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 23/04/2002                                           *
 * Last upate: 23/04/2002                                           *
 ********************************************************************/

#include <stdio.h>
#include "QTypes.h"
#include "Windows.h"


// Get application version string (long format, e.g. 2.5.0.1)
QString QGetAppVersionStr(void)
{
  char  AppFullPath[MAX_PATH];
  DWORD dwVerHnd;
  DWORD dwVerInfoSize;
  UINT  uiVerSize;
  DWORD *lpBuffer;
  QString sVer;

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

// Get application version string (short format, e.g. 2.5)
QString QGetShortAppVersionStr(void)
{
  QString FullVersion = QGetAppVersionStr();

  // Find the second '.' character...
  int i = FullVersion.find_first_of('.');
  i = FullVersion.find_first_of('.',i + 1);

  return FullVersion.substr(0,i);
}

