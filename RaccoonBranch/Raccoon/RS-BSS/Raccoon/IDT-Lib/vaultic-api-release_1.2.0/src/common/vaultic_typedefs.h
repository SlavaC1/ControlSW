/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_typedefs.h
 * 
 * \brief Types used by the VaultIC API.
 * 
 * \par Description:
 * This file declares the data types used by the VaultIC API.
 */

#ifndef VAULTIC_TYPEDEFS_H
#define VAULTIC_TYPEDEFS_H

#include "vaultic_config.h"

#if( VLT_PLATFORM == VLT_WINDOWS )
    #include <windows.h>
#endif


#ifndef NULL
  #ifdef __cplusplus
    #define NULL    0
  #else
    #define NULL    ((void *)0)
  #endif
#endif

#ifndef FALSE
  #define FALSE               0
#endif

#ifndef TRUE
  #define TRUE                1
#endif

#if( VLT_PLATFORM == VLT_WINDOWS )

    typedef WORD    VLT_STS;
    typedef WORD    VLT_SW; 
    typedef PWORD   VLT_PSW;    
    typedef BYTE    VLT_U8; 
    typedef PBYTE   VLT_PU8;    
    typedef WORD    VLT_U16;    
    typedef PWORD   VLT_PU16;   
    typedef DWORD   VLT_U32;
    typedef PDWORD  VLT_PU32;   
    typedef BOOL    VLT_BOOL;
    typedef UINT    VLT_UINT;

#elif( VLT_PLATFORM == VLT_EMBEDDED )

    typedef unsigned short  VLT_STS;
    typedef unsigned short  VLT_SW; 
    typedef unsigned short* VLT_PSW; 
    typedef unsigned char   VLT_U8; 
    typedef unsigned char*  VLT_PU8;
    typedef unsigned short  VLT_U16;
    typedef unsigned short* VLT_PU16;
    typedef unsigned long   VLT_U32;
    typedef unsigned long*  VLT_PU32;   
    typedef unsigned char   VLT_BOOL;
    typedef unsigned int    VLT_UINT;
        
#else

    typedef unsigned short  VLT_STS;
    typedef unsigned short  VLT_SW; 
    typedef unsigned short* VLT_PSW; 
    typedef unsigned char   VLT_U8; 
    typedef unsigned char*  VLT_PU8;
    typedef unsigned short  VLT_U16;
    typedef unsigned short* VLT_PU16;
    typedef unsigned long   VLT_U32;
    typedef unsigned long*  VLT_PU32;  
    typedef unsigned char   VLT_BOOL;
    typedef unsigned int    VLT_UINT;

#endif

#endif /*VAULTIC_TYPEDEFS_H*/
