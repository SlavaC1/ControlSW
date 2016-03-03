

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sun Sep 23 19:34:39 2007
 */
/* Compiler settings for .\VideoCameraAutomation.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __VideoCameraAutomation_h__
#define __VideoCameraAutomation_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVideoCamera_FWD_DEFINED__
#define __IVideoCamera_FWD_DEFINED__
typedef interface IVideoCamera IVideoCamera;
#endif 	/* __IVideoCamera_FWD_DEFINED__ */


#ifndef __VideoCameraComWrapper_FWD_DEFINED__
#define __VideoCameraComWrapper_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoCameraComWrapper VideoCameraComWrapper;
#else
typedef struct VideoCameraComWrapper VideoCameraComWrapper;
#endif /* __cplusplus */

#endif 	/* __VideoCameraComWrapper_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IVideoCamera_INTERFACE_DEFINED__
#define __IVideoCamera_INTERFACE_DEFINED__

/* interface IVideoCamera */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVideoCamera;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C881069E-1B80-46AA-9C0E-391E5AA52591")
    IVideoCamera : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE sampleToBitmap( 
            /* [retval][out] */ HBITMAP *phbitmap) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gamma( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Gamma( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE openSettingsDialog( 
            /* [in] */ HWND hwndOwner) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVideoCameraVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVideoCamera * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVideoCamera * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVideoCamera * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVideoCamera * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVideoCamera * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVideoCamera * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVideoCamera * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *sampleToBitmap )( 
            IVideoCamera * This,
            /* [retval][out] */ HBITMAP *phbitmap);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Gamma )( 
            IVideoCamera * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Gamma )( 
            IVideoCamera * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *openSettingsDialog )( 
            IVideoCamera * This,
            /* [in] */ HWND hwndOwner);
        
        END_INTERFACE
    } IVideoCameraVtbl;

    interface IVideoCamera
    {
        CONST_VTBL struct IVideoCameraVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoCamera_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVideoCamera_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVideoCamera_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVideoCamera_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVideoCamera_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVideoCamera_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVideoCamera_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVideoCamera_sampleToBitmap(This,phbitmap)	\
    (This)->lpVtbl -> sampleToBitmap(This,phbitmap)

#define IVideoCamera_get_Gamma(This,pVal)	\
    (This)->lpVtbl -> get_Gamma(This,pVal)

#define IVideoCamera_put_Gamma(This,newVal)	\
    (This)->lpVtbl -> put_Gamma(This,newVal)

#define IVideoCamera_openSettingsDialog(This,hwndOwner)	\
    (This)->lpVtbl -> openSettingsDialog(This,hwndOwner)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVideoCamera_sampleToBitmap_Proxy( 
    IVideoCamera * This,
    /* [retval][out] */ HBITMAP *phbitmap);


void __RPC_STUB IVideoCamera_sampleToBitmap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IVideoCamera_get_Gamma_Proxy( 
    IVideoCamera * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IVideoCamera_get_Gamma_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IVideoCamera_put_Gamma_Proxy( 
    IVideoCamera * This,
    /* [in] */ double newVal);


void __RPC_STUB IVideoCamera_put_Gamma_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVideoCamera_openSettingsDialog_Proxy( 
    IVideoCamera * This,
    /* [in] */ HWND hwndOwner);


void __RPC_STUB IVideoCamera_openSettingsDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVideoCamera_INTERFACE_DEFINED__ */



#ifndef __VideoCameraAutomationLib_LIBRARY_DEFINED__
#define __VideoCameraAutomationLib_LIBRARY_DEFINED__

/* library VideoCameraAutomationLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VideoCameraAutomationLib;

EXTERN_C const CLSID CLSID_VideoCameraComWrapper;

#ifdef __cplusplus

class DECLSPEC_UUID("999F1681-6DE1-4249-8334-52184982A842")
VideoCameraComWrapper;
#endif
#endif /* __VideoCameraAutomationLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HBITMAP_UserSize(     unsigned long *, unsigned long            , HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal(  unsigned long *, unsigned char *, HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal(unsigned long *, unsigned char *, HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree(     unsigned long *, HBITMAP * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


