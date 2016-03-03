// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'DlphVDEPAgentImpl.pas' rev: 6.00

#ifndef DlphVDEPAgentImplHPP
#define DlphVDEPAgentImplHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Windows.hpp>	// Pascal unit
#include <Math.hpp>	// Pascal unit
#include <ActiveX.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Dlphvdepagentimpl
{
//-- type declarations -------------------------------------------------------
//-- var, const, procedure ---------------------------------------------------
extern PACKAGE System::TObject* __fastcall findProcessControl(HWND wnd);
extern PACKAGE HRESULT __stdcall GetProperty(int hWnd, wchar_t * PropName, Activex::PSafeArray saParams, OleVariant &vtRetVal);
extern PACKAGE HRESULT __stdcall SetProperty(int hWnd, wchar_t * PropName, Activex::PSafeArray saParams, const OleVariant vtVal);
extern PACKAGE HRESULT __stdcall GetAllPropertyNames(int hWnd, Activex::PSafeArray &psaPropNames);
extern PACKAGE HRESULT __stdcall GetAOData(int hWnd, Activex::PSafeArray &psaData);
extern PACKAGE HRESULT __stdcall ResetAO(int hWnd);
extern PACKAGE HRESULT __stdcall CustomAgentService(wchar_t * szDllPath, wchar_t * szMethod, const OleVariant vtParamIn, OleVariant &vtParamOut);
extern PACKAGE OleVariant __fastcall _PackRecordLine(AnsiString Cmd, const OleVariant * Params, const int Params_Size, int SendMode);

}	/* namespace Dlphvdepagentimpl */
using namespace Dlphvdepagentimpl;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// DlphVDEPAgentImpl
