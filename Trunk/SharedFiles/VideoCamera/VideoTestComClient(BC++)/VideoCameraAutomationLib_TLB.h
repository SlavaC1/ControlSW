// ************************************************************************ //
// WARNING
// -------
// The types declared in this file were generated from data read from a
// Type Library. If this type library is explicitly or indirectly (via
// another type library referring to this type library) re-imported, or the
// 'Refresh' command of the Type Library Editor activated while editing the
// Type Library, the contents of this file will be regenerated and all
// manual modifications will be lost.
// ************************************************************************ //

// C++ TLBWRTR : $Revision:   1.151.1.0.1.27  $
// File generated on 9/19/2007 17:03:25 from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\SVN\EdenCapture\trunk\VideoCameraAutomation\VideoCameraAutomation\Debug\VideoCameraAutomation.dll (1)
// LIBID: {8C0F7857-3DB5-49A9-A4D7-0DE16C5A8061}
// LCID: 0
// Helpfile:
// HelpString: VideoCameraAutomation 1.0 Type Library
// DepndLst:
//   (1) v2.0 stdole, (C:\WINDOWS\system32\STDOLE2.TLB)
// ************************************************************************ //
#ifndef   VideoCameraAutomationLib_TLBH
#define   VideoCameraAutomationLib_TLBH

#pragma option push -b -w-inl

#include <ComObj.HPP> // define ComObjHPP affects utilcls.h
#include <utilcls.h>
#if !defined(__UTILCLS_H_VERSION) || (__UTILCLS_H_VERSION < 0x0600)
//
// The code generated by the TLIBIMP utility or the Import|TypeLibrary 
// and Import|ActiveX feature of C++Builder rely on specific versions of
// the header file UTILCLS.H found in the INCLUDE\VCL directory. If an 
// older version of the file is detected, you probably need an update/patch.
//
#error "This file requires a newer version of the header UTILCLS.H" \
       "You need to apply an update/patch to your copy of C++Builder"
#endif
#include <olectl.h>
#include <ocidl.h>
#if defined(USING_ATLVCL) || defined(USING_ATL)
#if !defined(__TLB_NO_EVENT_WRAPPERS)
#include <atl/atlmod.h>
#endif
#endif


// *********************************************************************//
// Forward reference of some VCL types (to avoid including STDVCL.HPP)    
// *********************************************************************//
namespace Stdvcl {class IStrings; class IStringsDisp;}
using namespace Stdvcl;
typedef TComInterface<IStrings> IStringsPtr;
typedef TComInterface<IStringsDisp> IStringsDispPtr;

namespace Videocameraautomationlib_tlb
{

// *********************************************************************//
// HelpString: VideoCameraAutomation 1.0 Type Library
// Version:    1.0
// *********************************************************************//


// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:        
//   Type Libraries     : LIBID_xxxx                                      
//   CoClasses          : CLSID_xxxx                                      
//   DISPInterfaces     : DIID_xxxx                                       
//   Non-DISP interfaces: IID_xxxx                                        
// *********************************************************************//
extern __declspec (package) const GUID LIBID_VideoCameraAutomationLib;
extern __declspec (package) const GUID IID_IVideoCamera;
extern __declspec (package) const GUID CLSID_VideoCameraComWrapper;
extern __declspec (package) const GUID GUID_wireHBITMAP;
extern __declspec (package) const GUID GUID__userBITMAP;
extern __declspec (package) const GUID GUID___MIDL_IWinTypes_0007;
extern __declspec (package) const GUID GUID__userHBITMAP;
extern __declspec (package) const GUID GUID_wireHWND;
extern __declspec (package) const GUID GUID___MIDL_IWinTypes_0009;
extern __declspec (package) const GUID GUID__RemotableHandle;

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
struct    tag_userBITMAP;
typedef   tag_userBITMAP _userBITMAP;

union     __MIDL_IWinTypes_0007;
struct    tag_userHBITMAP;
typedef   tag_userHBITMAP _userHBITMAP;

union     __MIDL_IWinTypes_0009;
struct    tag_RemotableHandle;
typedef   tag_RemotableHandle _RemotableHandle;

interface DECLSPEC_UUID("{C881069E-1B80-46AA-9C0E-391E5AA52591}") IVideoCamera;
typedef TComInterface<IVideoCamera, &IID_IVideoCamera> IVideoCameraPtr;


// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
//                                                                        
// The LIBID_OF_ macro(s) map a LIBID_OF_CoClassName to the GUID of this  
// TypeLibrary. It simplifies the updating of macros when CoClass name    
// change.                                                                
// *********************************************************************//
typedef IVideoCamera VideoCameraComWrapper;
typedef IVideoCameraPtr VideoCameraComWrapperPtr;

#define LIBID_OF_VideoCameraComWrapper (&LIBID_VideoCameraAutomationLib)

// *********************************************************************//
// Declaration of Aliases defined in Type Library                         
// *********************************************************************//
typedef Videocameraautomationlib_tlb::_userHBITMAP* wireHBITMAP;
typedef Videocameraautomationlib_tlb::_RemotableHandle* wireHWND;

// *********************************************************************//
// Declaration of Structures and Unions defined in Type Library           
// *********************************************************************//
struct tag_userBITMAP
{
  long bmType;
  long bmWidth;
  long bmHeight;
  long bmWidthBytes;
  unsigned_short bmPlanes;
  unsigned_short bmBitsPixel;
  unsigned_long cbSize;
  unsigned_char* pBuffer;
};

union  __MIDL_IWinTypes_0007
{
  long hInproc;
  Videocameraautomationlib_tlb::_userBITMAP* hRemote;
  __int64 hInproc64;
};

struct tag_userHBITMAP
{
  long fContext;
  Videocameraautomationlib_tlb::__MIDL_IWinTypes_0007 u;
};

union  __MIDL_IWinTypes_0009
{
  long hInproc;
  long hRemote;
};

struct tag_RemotableHandle
{
  long fContext;
  Videocameraautomationlib_tlb::__MIDL_IWinTypes_0009 u;
};

// *********************************************************************//
// Interface: IVideoCamera
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {C881069E-1B80-46AA-9C0E-391E5AA52591}
// *********************************************************************//
interface IVideoCamera  : public IDispatch
{
public:
  // [1] Samples a frame from the video camera stores it in a HBITMAP in memory and returns the HBITMAP.
  virtual HRESULT STDMETHODCALLTYPE sampleToBitmap(Videocameraautomationlib_tlb::wireHBITMAP* phbitmap/*[out,retval]*/) = 0;
  // [2] Set/Get Gamma
  virtual HRESULT STDMETHODCALLTYPE get_Gamma(double* pVal/*[out,retval]*/) = 0;
  // [2] Set/Get Gamma
  virtual HRESULT STDMETHODCALLTYPE set_Gamma(double pVal/*[in]*/) = 0;
  // [3] Opens the property pages (settings dialog).
  virtual HRESULT STDMETHODCALLTYPE openSettingsDialog(Videocameraautomationlib_tlb::wireHWND hwndOwner/*[in]*/) = 0;

#if !defined(__TLB_NO_INTERFACE_WRAPPERS)

  Videocameraautomationlib_tlb::wireHBITMAP __fastcall sampleToBitmap(void)
  {
    Videocameraautomationlib_tlb::wireHBITMAP phbitmap = 0;
    OLECHECK(this->sampleToBitmap((Videocameraautomationlib_tlb::wireHBITMAP*)&phbitmap));
    return phbitmap;
  }

  double __fastcall get_Gamma(void)
  {
    double pVal;
    OLECHECK(this->get_Gamma((double*)&pVal));
    return pVal;
  }


  __property   double          Gamma = {read = get_Gamma, write = set_Gamma};

#endif //   __TLB_NO_INTERFACE_WRAPPERS

};

#if !defined(__TLB_NO_INTERFACE_WRAPPERS)
// *********************************************************************//
// SmartIntf: TCOMIVideoCamera
// Interface: IVideoCamera
// *********************************************************************//
template <class T /* IVideoCamera */ >
class TCOMIVideoCameraT : public TComInterface<IVideoCamera>, public TComInterfaceBase<IUnknown>
{
public:
  TCOMIVideoCameraT() {}
  TCOMIVideoCameraT(IVideoCamera *intf, bool addRef = false) : TComInterface<IVideoCamera>(intf, addRef) {}
  TCOMIVideoCameraT(const TCOMIVideoCameraT& src) : TComInterface<IVideoCamera>(src) {}
  TCOMIVideoCameraT& operator=(const TCOMIVideoCameraT& src) { Bind(src, true); return *this;}

  HRESULT         __fastcall sampleToBitmap(Videocameraautomationlib_tlb::wireHBITMAP* phbitmap/*[out,retval]*/);
  Videocameraautomationlib_tlb::wireHBITMAP __fastcall sampleToBitmap(void);
  HRESULT         __fastcall get_Gamma(double* pVal/*[out,retval]*/);
  double          __fastcall get_Gamma(void);
  HRESULT         __fastcall set_Gamma(double pVal/*[in]*/);
  HRESULT         __fastcall openSettingsDialog(Videocameraautomationlib_tlb::wireHWND hwndOwner/*[in]*/);

  __property   double          Gamma = {read = get_Gamma, write = set_Gamma};
};
typedef TCOMIVideoCameraT<IVideoCamera> TCOMIVideoCamera;

// *********************************************************************//
// DispIntf:  IVideoCamera
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {C881069E-1B80-46AA-9C0E-391E5AA52591}
// *********************************************************************//
template<class T>
class IVideoCameraDispT : public TAutoDriver<IVideoCamera>
{
public:
  IVideoCameraDispT(){}

  IVideoCameraDispT(IVideoCamera *pintf)
  {
    TAutoDriver<IVideoCamera>::Bind(pintf, false);
  }

  IVideoCameraDispT(IVideoCameraPtr pintf)
  {
    TAutoDriver<IVideoCamera>::Bind(pintf, true);
  }

  IVideoCameraDispT& operator=(IVideoCamera *pintf)
  {
    TAutoDriver<IVideoCamera>::Bind(pintf, false);
    return *this;
  }

  IVideoCameraDispT& operator=(IVideoCameraPtr pintf)
  {
    TAutoDriver<IVideoCamera>::Bind(pintf, true);
    return *this;
  }

  HRESULT BindDefault()
  {
    return OLECHECK(Bind(CLSID_VideoCameraComWrapper));
  }

  HRESULT BindRunning()
  {
    return BindToActive(CLSID_VideoCameraComWrapper);
  }

  HRESULT         __fastcall sampleToBitmap(/*AUTO_PARAM_ERROR(Videocameraautomationlib_tlb::wireHBITMAP* phbitmap)*/ TVariant*  phbitmap);
  HRESULT         __fastcall get_Gamma(double* pVal/*[out,retval]*/);
  double          __fastcall get_Gamma(void);
  HRESULT         __fastcall set_Gamma(double pVal/*[in]*/);
  HRESULT         __fastcall openSettingsDialog(/*AUTO_PARAM_ERROR(Videocameraautomationlib_tlb::wireHWND hwndOwner)*/ TVariant&  hwndOwner);

  __property   double          Gamma = {read = get_Gamma, write = set_Gamma};
};
typedef IVideoCameraDispT<IVideoCamera> IVideoCameraDisp;

// *********************************************************************//
// SmartIntf: TCOMIVideoCamera
// Interface: IVideoCamera
// *********************************************************************//
template <class T> HRESULT __fastcall
TCOMIVideoCameraT<T>::sampleToBitmap(Videocameraautomationlib_tlb::wireHBITMAP* phbitmap/*[out,retval]*/)
{
  return (*this)->sampleToBitmap(phbitmap);
}

template <class T> Videocameraautomationlib_tlb::wireHBITMAP __fastcall
TCOMIVideoCameraT<T>::sampleToBitmap(void)
{
  Videocameraautomationlib_tlb::wireHBITMAP phbitmap = 0;
  OLECHECK(this->sampleToBitmap((Videocameraautomationlib_tlb::wireHBITMAP*)&phbitmap));
  return phbitmap;
}

template <class T> HRESULT __fastcall
TCOMIVideoCameraT<T>::get_Gamma(double* pVal/*[out,retval]*/)
{
  return (*this)->get_Gamma(pVal);
}

template <class T> double __fastcall
TCOMIVideoCameraT<T>::get_Gamma(void)
{
  double pVal;
  OLECHECK(this->get_Gamma((double*)&pVal));
  return pVal;
}

template <class T> HRESULT __fastcall
TCOMIVideoCameraT<T>::set_Gamma(double pVal/*[in]*/)
{
  return (*this)->set_Gamma(pVal);
}

template <class T> HRESULT __fastcall
TCOMIVideoCameraT<T>::openSettingsDialog(Videocameraautomationlib_tlb::wireHWND hwndOwner/*[in]*/)
{
  return (*this)->openSettingsDialog(hwndOwner);
}

// *********************************************************************//
// DispIntf:  IVideoCamera
// Flags:     (4544) Dual NonExtensible OleAutomation Dispatchable
// GUID:      {C881069E-1B80-46AA-9C0E-391E5AA52591}
// *********************************************************************//
template <class T> HRESULT __fastcall
IVideoCameraDispT<T>::sampleToBitmap(/*AUTO_PARAM_ERROR(Videocameraautomationlib_tlb::wireHBITMAP* phbitmap)*/ TVariant*  phbitmap)
{
  _TDispID _dispid(*this, OLETEXT("sampleToBitmap"), DISPID(1));
  TAutoArgs<0> _args;
  return OutRetValSetterPtr(phbitmap /*[VT_USERDEFINED:2]*/, _args, OleFunction(_dispid, _args));
}

template <class T> HRESULT __fastcall
IVideoCameraDispT<T>::get_Gamma(double* pVal/*[out,retval]*/)
{
  _TDispID _dispid(*this, OLETEXT("Gamma"), DISPID(2));
  TAutoArgs<0> _args;
  return OutRetValSetterPtr(pVal /*[VT_R8:1]*/, _args, OlePropertyGet(_dispid, _args));
}

template <class T> double __fastcall
IVideoCameraDispT<T>::get_Gamma(void)
{
  double pVal;
  this->get_Gamma((double*)&pVal);
  return pVal;
}

template <class T> HRESULT __fastcall
IVideoCameraDispT<T>::set_Gamma(double pVal/*[in]*/)
{
  _TDispID _dispid(*this, OLETEXT("Gamma"), DISPID(2));
  TAutoArgs<1> _args;
  _args[1] = pVal /*[VT_R8:0]*/;
  return OlePropertyPut(_dispid, _args);
}

template <class T> HRESULT __fastcall
IVideoCameraDispT<T>::openSettingsDialog(/*AUTO_PARAM_ERROR(Videocameraautomationlib_tlb::wireHWND hwndOwner)*/ TVariant&  hwndOwner)
{
  _TDispID _dispid(*this, OLETEXT("openSettingsDialog"), DISPID(3));
  TAutoArgs<1> _args;
  _args[1] = hwndOwner /*[VT_USERDEFINED:1]*/;
  return OleFunction(_dispid, _args);
}

// *********************************************************************//
// The following typedefs expose classes (named CoCoClassName) that       
// provide static Create() and CreateRemote(LPWSTR machineName) methods   
// for creating an instance of an exposed object. These functions can     
// be used by client wishing to automate CoClasses exposed by this        
// typelibrary.                                                           
// *********************************************************************//

// *********************************************************************//
// COCLASS DEFAULT INTERFACE CREATOR
// CoClass  : VideoCameraComWrapper
// Interface: TCOMIVideoCamera
// *********************************************************************//
typedef TCoClassCreatorT<TCOMIVideoCamera, IVideoCamera, &CLSID_VideoCameraComWrapper, &IID_IVideoCamera> CoVideoCameraComWrapper;
#endif  //   __TLB_NO_INTERFACE_WRAPPERS


};     // namespace Videocameraautomationlib_tlb

#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using  namespace Videocameraautomationlib_tlb;
#endif

#pragma option pop

#endif // VideoCameraAutomationLib_TLBH