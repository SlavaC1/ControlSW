/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2013. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#ifndef ___VLTAARDVARK__PERIPHERAL___
#define ___VLTAARDVARK__PERIPHERAL___

#include "vaultic_common.h"
#include "aardvark.h"

/*
* Error Codes
*/
#define EAARINITOPEN         VLT_ERROR( VLT_AARDVARK, 0 )
#define EAARINITCONFIG       VLT_ERROR( VLT_AARDVARK, 1 )
#define EAARINITBITRATE      VLT_ERROR( VLT_AARDVARK, 2 )
#define EAARNOAARDVARK       VLT_ERROR( VLT_AARDVARK, 3 )
#define EAARSLAVEDISABLE     VLT_ERROR( VLT_AARDVARK, 4 )
#define EAARINITTGTPWRBOTH   VLT_ERROR( VLT_AARDVARK, 5 )
#define EAARCLOSEPWRNONE     VLT_ERROR( VLT_AARDVARK, 6 )
#define EAARCLOSEAARDCLS     VLT_ERROR( VLT_AARDVARK, 7 )
#define EAARDVARKINUSE       VLT_ERROR( VLT_AARDVARK, 8 )
#define EAARSNDNULLPARAMS    VLT_ERROR( VLT_AARDVARK, 9 )
#define EAARRCVNULLPARAMS    VLT_ERROR( VLT_AARDVARK, 10 )
#define EAARSNDFAIL          VLT_ERROR( VLT_AARDVARK, 11 )
#define EAARRCVFAIL          VLT_ERROR( VLT_AARDVARK, 12 )
#define EAARINITCONFIGSPI    VLT_ERROR( VLT_AARDVARK, 13 )
#define EAARREADATTMPTS      VLT_ERROR( VLT_AARDVARK, 14 )
#define EAARRCVCAPTOOLOW     VLT_ERROR( VLT_AARDVARK, 15 )
#define EAARINITNULLPARAMS   VLT_ERROR( VLT_AARDVARK, 16 )
#define EAARUNSUPPIOCTLID    VLT_ERROR( VLT_AARDVARK, 17 )
#define EAARINVLDSERNUM      VLT_ERROR( VLT_AARDVARK, 18 )
#define EAARUPDBITLNULLPTR   VLT_ERROR( VLT_AARDVARK, 19 )
#define EAARTOOMANYAARDV     VLT_ERROR( VLT_AARDVARK, 20 )
#define EAADSTBTRTNULLPARAMS VLT_ERROR( VLT_AARDVARK, 21 )
#define EAARDSPIWRITE        VLT_ERROR( VLT_AARDVARK, 22 )
#define EAARSNDDELARRYNULL   VLT_ERROR( VLT_AARDVARK, 23 )
#define EDTAADRSIVLDPARAM    VLT_ERROR( VLT_AARDVARK, 25 )
#define EDTAADRSIVLDSTR      VLT_ERROR( VLT_AARDVARK, 26 )
#define EDTAADRSEXDMXM       VLT_ERROR( VLT_AARDVARK, 27 )
#define EDTAADRSINSFLEN      VLT_ERROR( VLT_AARDVARK, 28 )


/*
* Defines
*/
#define DEFAULT_BIT_RATE      (VLT_U8)125
#define MAX_NO_OF_AARDVARKS    (VLT_U8)0x0A
#define XML_AA_HEADER_SZ      (VLT_U8)27
#define XML_AA_DATA_SZ        (VLT_U8)34
#define XML_AA_FOOTER_SZ      (VLT_U8)12

VLT_STS VltAardvarkDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString );

#endif //___VLTAARDVARK__PERIPHERAL___