

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "VideoCameraAutomation.h"

#define TYPE_FORMAT_STRING_SIZE   113                               
#define PROC_FORMAT_STRING_SIZE   113                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IVideoCamera_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IVideoCamera_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, float, double or hyper in -Oif or -Oicf.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure sampleToBitmap */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter phbitmap */

/* 16 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	NdrFcShort( 0x40 ),	/* Type Offset=64 */

	/* Return value */

/* 22 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Gamma */

/* 28 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 30 */	NdrFcLong( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 38 */	NdrFcShort( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x2c ),	/* 44 */
/* 42 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 44 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 46 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 48 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Gamma */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x9 ),	/* 9 */
/* 64 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 66 */	NdrFcShort( 0x10 ),	/* 16 */
/* 68 */	NdrFcShort( 0x8 ),	/* 8 */
/* 70 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter newVal */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Return value */

/* 78 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure openSettingsDialog */

/* 84 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 86 */	NdrFcLong( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0xa ),	/* 10 */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x8 ),	/* 8 */
/* 98 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter hwndOwner */

/* 100 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 102 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 104 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Return value */

/* 106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 110 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  4 */	NdrFcShort( 0x3c ),	/* Offset= 60 (64) */
/*  6 */	
			0x13, 0x0,	/* FC_OP */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x88,		/* 136 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	NdrFcShort( 0x3 ),	/* 3 */
/* 16 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 20 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 22 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 26 */	NdrFcShort( 0xa ),	/* Offset= 10 (36) */
/* 28 */	NdrFcLong( 0x50746457 ),	/* 1349805143 */
/* 32 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 34 */	NdrFcShort( 0xffff ),	/* Offset= -1 (33) */
/* 36 */	
			0x13, 0x0,	/* FC_OP */
/* 38 */	NdrFcShort( 0xc ),	/* Offset= 12 (50) */
/* 40 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 42 */	NdrFcShort( 0x1 ),	/* 1 */
/* 44 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 46 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 48 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 50 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 52 */	NdrFcShort( 0x18 ),	/* 24 */
/* 54 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (40) */
/* 56 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 58 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 60 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 62 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 64 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x4 ),	/* 4 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0xffbe ),	/* Offset= -66 (6) */
/* 74 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 76 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 78 */	
			0x12, 0x0,	/* FC_UP */
/* 80 */	NdrFcShort( 0x2 ),	/* Offset= 2 (82) */
/* 82 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 84 */	NdrFcShort( 0x4 ),	/* 4 */
/* 86 */	NdrFcShort( 0x2 ),	/* 2 */
/* 88 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 92 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 94 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 98 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 100 */	NdrFcShort( 0xffff ),	/* Offset= -1 (99) */
/* 102 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 104 */	NdrFcShort( 0x1 ),	/* 1 */
/* 106 */	NdrFcShort( 0x4 ),	/* 4 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (78) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            HBITMAP_UserSize
            ,HBITMAP_UserMarshal
            ,HBITMAP_UserUnmarshal
            ,HBITMAP_UserFree
            },
            {
            HWND_UserSize
            ,HWND_UserMarshal
            ,HWND_UserUnmarshal
            ,HWND_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IVideoCamera, ver. 0.0,
   GUID={0xC881069E,0x1B80,0x46AA,{0x9C,0x0E,0x39,0x1E,0x5A,0xA5,0x25,0x91}} */

#pragma code_seg(".orpc")
static const unsigned short IVideoCamera_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    28,
    56,
    84
    };

static const MIDL_STUBLESS_PROXY_INFO IVideoCamera_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IVideoCamera_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IVideoCamera_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IVideoCamera_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _IVideoCameraProxyVtbl = 
{
    &IVideoCamera_ProxyInfo,
    &IID_IVideoCamera,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IVideoCamera::sampleToBitmap */ ,
    (void *) (INT_PTR) -1 /* IVideoCamera::get_Gamma */ ,
    (void *) (INT_PTR) -1 /* IVideoCamera::put_Gamma */ ,
    (void *) (INT_PTR) -1 /* IVideoCamera::openSettingsDialog */
};


static const PRPC_STUB_FUNCTION IVideoCamera_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IVideoCameraStubVtbl =
{
    &IID_IVideoCamera,
    &IVideoCamera_ServerInfo,
    11,
    &IVideoCamera_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x600016e, /* MIDL Version 6.0.366 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _VideoCameraAutomation_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IVideoCameraProxyVtbl,
    0
};

const CInterfaceStubVtbl * _VideoCameraAutomation_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IVideoCameraStubVtbl,
    0
};

PCInterfaceName const _VideoCameraAutomation_InterfaceNamesList[] = 
{
    "IVideoCamera",
    0
};

const IID *  _VideoCameraAutomation_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _VideoCameraAutomation_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _VideoCameraAutomation, pIID, n)

int __stdcall _VideoCameraAutomation_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_VideoCameraAutomation_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo VideoCameraAutomation_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _VideoCameraAutomation_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _VideoCameraAutomation_StubVtblList,
    (const PCInterfaceName * ) & _VideoCameraAutomation_InterfaceNamesList,
    (const IID ** ) & _VideoCameraAutomation_BaseIIDList,
    & _VideoCameraAutomation_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

