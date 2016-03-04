/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_ecdsa_strong_authentication.h
 * 
 * \brief implementation of ECDSA strong authentication
 * 
 * \par Description:
 * Implementation ECDSA strong authentication for VaultIC100 device
 */

#include "vaultic_common.h"
#include "vaultic_ecdsa_strong_authentication.h"
#include "vaultic_api.h"
#include "vaultic_mem.h"
#include "vaultic_ecdsa_signer.h"

/**
 * Local definitions
 */
#define HOST_CHALLENGE_LEN		(VLT_U8)8
#define DEVICE_CHALLENGE_LEN	(VLT_U8)8
#define MAX_SIGNATURE_SIZE		(VLT_U8)76
#define HOST_ID_LENGTH			(VLT_U8)16
#define DEVICE_ID_LENGTH		(VLT_U8)16
#define MESSAGE_LENGTH			(HOST_CHALLENGE_LEN + DEVICE_CHALLENGE_LEN + HOST_ID_LENGTH)

#define MAX_GF2M_ORDER		(VLT_U16)303	/* maximum of 303-bit fields supported */
#define MAX_ORDER_BYTES		(VLT_U16)40		/* for 303-bit element */
#define MAX_KEY_SIZE		MAX_ORDER_BYTES	

#define DEVICE_PUBLIC_KEY_GROUP		(0x01)
#define DEVICE_PUBLIC_KEY_INDEX		(0x01) 

/*
* Local helper function prototypes
*/
static VLT_STS VltEcdsaAuthenticateDevice(								   
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams,
	VLT_U8 au8DeviceChallenge[DEVICE_CHALLENGE_LEN]);

static VLT_STS VltEcdsaAuthenticateHost(								   
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams,
	VLT_U8 au8DeviceChallenge[DEVICE_CHALLENGE_LEN]);

static VLT_STS VltEcdsaHostAssurance(								   
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams);


#if (VLT_ENABLE_ECDSA_K233 == VLT_ENABLE)

/*
// Koblitz K-233 curve from ANS X9.62
*/ 

static VLT_U8 sK233_Q[] = {
	0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01 };

static VLT_U8 sK233_Gx[] = {
	0x00,0x00,0x01,0x72,0x32,0xba,0x85,0x3a,
	0x7e,0x73,0x1a,0xf1,0x29,0xf2,0x2f,0xf4,
	0x14,0x95,0x63,0xa4,0x19,0xc2,0x6b,0xf5,
	0x0a,0x4c,0x9d,0x6e,0xef,0xad,0x61,0x26 };

static VLT_U8 sK233_Gy[] = {
	0x00,0x00,0x01,0xdb,0x53,0x7d,0xec,0xe8,
	0x19,0xb7,0xf7,0x0f,0x55,0x5a,0x67,0xc4,
	0x27,0xa8,0xcd,0x9b,0xf1,0x8a,0xeb,0x9b,
	0x56,0xe0,0xc1,0x10,0x56,0xfa,0xe6,0xa3 };

static VLT_U8 sK233_Gz[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01 };

static VLT_U8 sK233_A[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

static VLT_U8 sK233_B[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01 };

static VLT_U8 sK233_N[] = {
	0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x06,0x9d,0x5b,0xb9,0x15,0xbc,0xd4,
	0x6e,0xfb,0x1a,0xd5,0xf1,0x73,0xab,0xdf };

static VLT_U32 sK233_H = 0x04;

#endif /* VLT_ENABLE_ECDSA_K233 == VLT_ENABLE */

#if (VLT_ENABLE_ECDSA_B233 == VLT_ENABLE)

/*
// B-233 Curve Values.
*/
static VLT_U8 sB233_Q[] =
{
    0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};

static VLT_U8 sB233_Gx[] =
{
    0x00,0x00,0x00,0xfa,0xc9,0xdf,0xcb,0xac,
	0x83,0x13,0xbb,0x21,0x39,0xf1,0xbb,0x75,
    0x5f,0xef,0x65,0xbc,0x39,0x1f,0x8b,0x36,
	0xf8,0xf8,0xeb,0x73,0x71,0xfd,0x55,0x8b    
};

static VLT_U8 sB233_Gy[] =
{
   0x00,0x00,0x01,0x00,0x6a,0x08,0xa4,0x19,
   0x03,0x35,0x06,0x78,0xe5,0x85,0x28,0xbe,
   0xbf,0x8a,0x0b,0xef,0xf8,0x67,0xa7,0xca,
   0x36,0x71,0x6f,0x7e,0x01,0xf8,0x10,0x52
};

static VLT_U8 sB233_Gz[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};

static VLT_U8 sB233_A[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};

static VLT_U8 sB233_B[] =
{
    0x00,0x00,0x00,0x66,0x64,0x7e,0xde,0x6c,
	0x33,0x2c,0x7f,0x8c,0x09,0x23,0xbb,0x58,
    0x21,0x3b,0x33,0x3b,0x20,0xe9,0xce,0x42,
	0x81,0xfe,0x11,0x5f,0x7d,0x8f,0x90,0xad
};

static VLT_U8 sB233_N[] =
{
    0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x13,0xe9,0x74,0xe7,0x2f,0x8a,0x69,
	0x22,0x03,0x1d,0x26,0x03,0xcf,0xe0,0xd7
};

static VLT_U16 sB233_H = 2;

#endif

#if (VLT_ENABLE_ECDSA_B283 == VLT_ENABLE)

/*
// B-283 Curve Values.
*/
static VLT_U8 sB283_Q[] =
{
    0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0xa1
};

static VLT_U8 sB283_Gx[] =
{
    0x05,0xf9,0x39,0x25,0x8d,0xb7,0xdd,0x90,
	0xe1,0x93,0x4f,0x8c,0x70,0xb0,0xdf,0xec,
    0x2e,0xed,0x25,0xb8,0x55,0x7e,0xac,0x9c,
	0x80,0xe2,0xe1,0x98,0xf8,0xcd,0xbe,0xcd,
    0x86,0xb1,0x20,0x53
};


static VLT_U8 sB283_Gy[] =
{
   0x03,0x67,0x68,0x54,0xfe,0x24,0x14,0x1c,
   0xb9,0x8f,0xe6,0xd4,0xb2,0x0d,0x02,0xb4,
   0x51,0x6f,0xf7,0x02,0x35,0x0e,0xdd,0xb0,
   0x82,0x67,0x79,0xc8,0x13,0xf0,0xdf,0x45,
   0xbe,0x81,0x12,0xf4
};

static VLT_U8 sB283_Gz[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01
};

static VLT_U8 sB283_A[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01
};

static VLT_U8 sB283_B[] =
{
    0x02,0x7b,0x68,0x0a,0xc8,0xb8,0x59,0x6d,
	0xa5,0xa4,0xaf,0x8a,0x19,0xa0,0x30,0x3f,
    0xca,0x97,0xfd,0x76,0x45,0x30,0x9f,0xa2,
	0xa5,0x81,0x48,0x5a,0xf6,0x26,0x3e,0x31,
    0x3b,0x79,0xa2,0xf5
};

static VLT_U8 sB283_N[] =
{
    0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xef,0x90,0x39,0x96,0x60,0xfc,
	0x93,0x8a,0x90,0x16,0x5b,0x04,0x2a,0x7c,
    0xef,0xad,0xb3,0x07
};

static VLT_U16 sB283_H = 2;

#endif /* VLT_ENABLE_ECDSA_B283 == VLT_ENABLE */


#if (VLT_ENABLE_ECDSA_K283 == VLT_ENABLE)

/*
// K-283 Curve Values.
*/
static VLT_U8 sK283_Q[] =
{
    0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0xa1
};

static VLT_U8 sK283_Gx[] =
{
    0x05,0x03,0x21,0x3f,0x78,0xca,0x44,0x88,
	0x3f,0x1a,0x3b,0x81,0x62,0xf1,0x88,0xe5,
    0x53,0xcd,0x26,0x5f,0x23,0xc1,0x56,0x7a,
	0x16,0x87,0x69,0x13,0xb0,0xc2,0xac,0x24,
    0x58,0x49,0x28,0x36
};

static VLT_U8 sK283_Gy[] =
{
    0x01,0xcc,0xda,0x38,0x0f,0x1c,0x9e,0x31,
	0x8d,0x90,0xf9,0x5d,0x07,0xe5,0x42,0x6f,
    0xe8,0x7e,0x45,0xc0,0xe8,0x18,0x46,0x98,
	0xe4,0x59,0x62,0x36,0x4e,0x34,0x11,0x61,
    0x77,0xdd,0x22,0x59
};

static VLT_U8 sK283_Gz[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01
};

static VLT_U8 sK283_A[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

static VLT_U8 sK283_B[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01
};

static VLT_U8 sK283_N[] =
{
    0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xe9,0xae,0x2e,0xd0,0x75,0x77,
	0x26,0x5d,0xff,0x7f,0x94,0x45,0x1e,0x06,
    0x1e,0x16,0x3c,0x61
};

static VLT_U16 sK283_H = 4;

#endif /* (VLT_ENABLE_ECDSA_K283 == VLT_ENABLE) */



#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    /*
    * Local Variables used by Strong Authentication
    */
	static VLT_U8 u8StrongAuthState = VLT_USER_NOT_AUTHENTICATED;
	static VLT_U8 au8Signature[MAX_SIGNATURE_SIZE];
	static VLT_U8 au8Message[VLT_GA_MESSAGE_LENGTH];

#endif // #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )

/**
 * \fn VltEcdsaStrongSetCryptoParams(VLT_U8 paramId, VLT_SA_CRYPTO_PARAMS* pCrytoParams)
 * \brief Sets up the crypto parameter data for the default curves supported by VaultIC100
 * \return Status.
 */
VLT_STS VltEcdsaStrongSetCryptoParams(VLT_U8 paramId, VLT_SA_CRYPTO_PARAMS* pCryptoParams)
{
	VLT_STS status = VLT_FAIL;

    if (NULL == pCryptoParams)
        return ESTRONGINITNULLPARAM;

	pCryptoParams->u8AlgID = VLT_ALG_SIG_ECDSA_GF2M;

	switch (paramId) {
	case VLT_ECDSA_CURVE_B233:
		#if (VLT_ENABLE_ECDSA_B233 == VLT_ENABLE)
		pCryptoParams->params.ecdsa.domainParams.u16QLen = sizeof(sB233_Q);
		pCryptoParams->params.ecdsa.domainParams.u16NLen = sizeof(sB233_N);
		pCryptoParams->params.ecdsa.domainParams.pu8Q = sB233_Q;
		pCryptoParams->params.ecdsa.domainParams.pu8N = sB233_N;
		pCryptoParams->params.ecdsa.domainParams.pu8Gx = sB233_Gx;
		pCryptoParams->params.ecdsa.domainParams.pu8Gy = sB233_Gy;
		pCryptoParams->params.ecdsa.domainParams.pu8Gz = sB233_Gz;
		pCryptoParams->params.ecdsa.domainParams.pu8A = sB233_A;
		pCryptoParams->params.ecdsa.domainParams.pu8B = sB233_B;
		pCryptoParams->params.ecdsa.domainParams.u32H = sB233_H;
		status = VLT_OK;
		#endif
		break;
	case VLT_ECDSA_CURVE_K233:
		#if (VLT_ENABLE_ECDSA_K233 == VLT_ENABLE)
		pCryptoParams->params.ecdsa.domainParams.u16QLen = sizeof(sK233_Q);
		pCryptoParams->params.ecdsa.domainParams.u16NLen = sizeof(sK233_N);
		pCryptoParams->params.ecdsa.domainParams.pu8Q = sK233_Q;
		pCryptoParams->params.ecdsa.domainParams.pu8N = sK233_N;
		pCryptoParams->params.ecdsa.domainParams.pu8Gx = sK233_Gx;
		pCryptoParams->params.ecdsa.domainParams.pu8Gy = sK233_Gy;
		pCryptoParams->params.ecdsa.domainParams.pu8Gz = sK233_Gz;
		pCryptoParams->params.ecdsa.domainParams.pu8A = sK233_A;
		pCryptoParams->params.ecdsa.domainParams.pu8B = sK233_B;
		pCryptoParams->params.ecdsa.domainParams.u32H = sK233_H;
		status = VLT_OK;
		#endif
		break;
	case VLT_ECDSA_CURVE_B283:
		#if (VLT_ENABLE_ECDSA_B283 == VLT_ENABLE)
		pCryptoParams->params.ecdsa.domainParams.u16QLen = sizeof(sB283_Q);
		pCryptoParams->params.ecdsa.domainParams.u16NLen = sizeof(sB283_N);
		pCryptoParams->params.ecdsa.domainParams.pu8Q = sB283_Q;
		pCryptoParams->params.ecdsa.domainParams.pu8N = sB283_N;
		pCryptoParams->params.ecdsa.domainParams.pu8Gx = sB283_Gx;
		pCryptoParams->params.ecdsa.domainParams.pu8Gy = sB283_Gy;
		pCryptoParams->params.ecdsa.domainParams.pu8Gz = sB283_Gz;
		pCryptoParams->params.ecdsa.domainParams.pu8A = sB283_A;
		pCryptoParams->params.ecdsa.domainParams.pu8B = sB283_B;
		pCryptoParams->params.ecdsa.domainParams.u32H = sB283_H;
		status = VLT_OK;
		#endif
		break;
	case VLT_ECDSA_CURVE_K283:
		#if (VLT_ENABLE_ECDSA_K283 == VLT_ENABLE)
		pCryptoParams->params.ecdsa.domainParams.u16QLen = sizeof(sK283_Q);
		pCryptoParams->params.ecdsa.domainParams.u16NLen = sizeof(sK283_N);
		pCryptoParams->params.ecdsa.domainParams.pu8Q = sK283_Q;
		pCryptoParams->params.ecdsa.domainParams.pu8N = sK283_N;
		pCryptoParams->params.ecdsa.domainParams.pu8Gx = sK283_Gx;
		pCryptoParams->params.ecdsa.domainParams.pu8Gy = sK283_Gy;
		pCryptoParams->params.ecdsa.domainParams.pu8Gz = sK283_Gz;
		pCryptoParams->params.ecdsa.domainParams.pu8A = sK283_A;
		pCryptoParams->params.ecdsa.domainParams.pu8B = sK283_B;
		pCryptoParams->params.ecdsa.domainParams.u32H = sK283_H;
		status = VLT_OK;
		#endif
		break;
	}
	return status;
}

/**
 * \fn VltEcdsaStrongAuthenticate(
 *		VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
 *		VLT_SA_PROTOCOL_PARAMS* pProtocolParams)* 
 * \brief Perfoms ECDSA based strong authentication
 *
 * \return Status.
 */

VLT_STS VltEcdsaStrongAuthenticate(
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams)
{
	VLT_STS status = VLT_FAIL;
	VLT_U8 u8UserId, u8RoleId;
    VLT_U8 au8DeviceChallenge[DEVICE_CHALLENGE_LEN];


    if ( ( NULL == pCryptoParams ) ||
         ( NULL == pProtocolParams ) )
    {
        return ESTRONGINITNULLPARAM;
    }

	/*
	*	Check requested algorithm is ECDSA 
	*/
	if(VLT_ALG_SIG_ECDSA_GF2M != pCryptoParams->u8AlgID)
    {
        return ESTRONGINITNULLPARAM;
    }

	if ((VLT_AUTH_MUTUAL_FIPS == pProtocolParams->u8AuthModeID) ||
		(VLT_AUTH_MUTUAL_NON_FIPS == pProtocolParams->u8AuthModeID))
	{
		/* mutual authentication requires role and user IDs to be
		   zero for the device authentication phase */
		u8UserId = pProtocolParams->u8UserID;
		u8RoleId = pProtocolParams->u8RoleID;
		pProtocolParams->u8UserID = 0;
		pProtocolParams->u8RoleID = 0;
	}

	/* all protocols do a unilateral (device to host) authentication first */
	status = VltEcdsaAuthenticateDevice(
		pCryptoParams, pProtocolParams, au8DeviceChallenge);

	if ((VLT_OK == status) && 
		(VLT_AUTH_MUTUAL_FIPS == pProtocolParams->u8AuthModeID))
	{
		/* 
		 * FIPS mode requires assurance of host possession of private key 
		*/
		status = VltEcdsaHostAssurance(pCryptoParams, pProtocolParams);
	}		

	if ( VLT_OK == status )
	{
		if ((VLT_AUTH_MUTUAL_FIPS == pProtocolParams->u8AuthModeID) ||
			(VLT_AUTH_MUTUAL_NON_FIPS == pProtocolParams->u8AuthModeID))
		{
			/* restore user and role IDs */
			pProtocolParams->u8UserID = u8UserId;;
			pProtocolParams->u8RoleID = u8RoleId;
			/* authenticate host to device */
			status = VltEcdsaAuthenticateHost(
				pCryptoParams, pProtocolParams, au8DeviceChallenge);
		}
	}

	return status;

}

/**
 * \fn VltEcdsaAuthenticateUnilateral ( VLT_IDENTITY_AUTH* pAuthParams )
 * \brief Perfoms the FIPS186 Unilateral athentication procedure
 * \return Status.
 */

VLT_STS VltEcdsaAuthenticateDevice(								   
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams,
	VLT_U8 au8DeviceChallenge[DEVICE_CHALLENGE_LEN]
)
{

#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16SignatureLength = MAX_SIGNATURE_SIZE;
    VLT_U32 u32SignatureLength = MAX_SIGNATURE_SIZE;
    VLT_U8 au8HostChallenge[HOST_CHALLENGE_LEN];
    VLT_U8 au8Message[MESSAGE_LENGTH];
 	
    /*
    * Generate Host Challenge ( string of random characters usually 8 bytes )
    */
    if (VLT_OK != (status = VltGenerateRandom(
		HOST_CHALLENGE_LEN, &au8HostChallenge[0] )))
    {
        status = ESTRONGGENRANDFAIL;
    }
       
    if ( status == VLT_OK)
    {
        /*
         * call VltInternalAuthenticate to start the auth process.
         */
        status = VltInternalAuthenticate(
			pProtocolParams->u8UserID,
            pProtocolParams->u8RoleID,
            HOST_CHALLENGE_LEN,
            au8HostChallenge,
            au8DeviceChallenge,
            &u16SignatureLength,
            au8Signature);
    
        /*
         * Check the call to Internal Authenticate was successful.
         */
        if( VLT_OK != status )
        {
            status = ESTRONGINTAUTHFAIL;
        }
    }
	
    if ( VLT_OK == status )
    {
		/*
		 * Construct the message to verify the device cryptogram:
		 * SIGN ( Cd || Ch || HostID )
		 */
        (void)host_memcpy( 
			&au8Message[0], 
            au8DeviceChallenge, 
            DEVICE_CHALLENGE_LEN );

        (void)host_memcpy( 
			&au8Message[DEVICE_CHALLENGE_LEN], 
            au8HostChallenge, 
            HOST_CHALLENGE_LEN );

        (void)host_memcpy( 
			&au8Message[DEVICE_CHALLENGE_LEN + HOST_CHALLENGE_LEN], 
            pProtocolParams->pu8HostId,  
            HOST_ID_LENGTH);
	}

    /*
     * Take the signature returned by the vaultic  and verify the signature 
     * using the vaultic's public key.  Only perform this action if the status
     * value is VLT_OK.
     */
    if ( VLT_OK == status )
    {
        /*
         * Perform the verify operation using the vaultic's public key.
         */
        status = EcdsaSignerInit( 
					&pCryptoParams->params.ecdsa.domainParams,
					NULL,
					&pCryptoParams->params.ecdsa.devicePublicKey, 
					VLT_VERIFY_MODE);

        /* Check the crypto service was constructed correctly */
        if ( VLT_OK == status )
        {
            /*
             * Pass the message to the crypto to be signed and verified.
             * There will be no output (verify) so it is ok to pass
             * the signature buffer for the output.
             */
			u32SignatureLength = u16SignatureLength;
            status = EcdsaSignerDoFinal(
						au8Message, 
						MESSAGE_LENGTH, 
						MESSAGE_LENGTH,
						au8Signature, 
						&u32SignatureLength, 
						u32SignatureLength );

            if ( VLT_OK != status )
            {
                /* Failed the verify operation, don't continue, return an error. */
                status = EINTERNALAUTHVERFAILED;
            }
        }
		/* clear down and free signer resources */
		EcdsaSignerClose( );
    }
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */
}


VLT_STS VltEcdsaAuthenticateHost(								   
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams,
	VLT_U8 au8DeviceChallenge[DEVICE_CHALLENGE_LEN])
{
#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16SignatureLength = MAX_SIGNATURE_SIZE;
    VLT_U32 u32SignatureLength = MAX_SIGNATURE_SIZE;
    VLT_U8 au8HostChallenge[HOST_CHALLENGE_LEN];
    VLT_U8 au8Message[MESSAGE_LENGTH];
        
    /*
    * Check that the input parameters are valid
    */
    if ( ( NULL == pCryptoParams ) ||
         ( NULL == pProtocolParams ) )
    {
        return ESTRONGINITNULLPARAM;
    }

    /*
    * Generate Host Challenge 
    */
    status = VltGenerateRandom(HOST_CHALLENGE_LEN, &au8HostChallenge[0]);
    if( VLT_OK != status )
    {
        status = ESTRONGGENRANDFAIL;
    }
    
    if ( VLT_OK == status )
    {
		/*
		 * Construct host cryptogram: SIGN ( Ch || Cd || DeviceID )
		 */
        (void)host_memcpy( 
			&au8Message[0], 
            au8HostChallenge, 
            HOST_CHALLENGE_LEN );

        (void)host_memcpy( 
			&au8Message[HOST_CHALLENGE_LEN], 
            au8DeviceChallenge, 
            DEVICE_CHALLENGE_LEN );

        (void)host_memcpy( 
			&au8Message[DEVICE_CHALLENGE_LEN + HOST_CHALLENGE_LEN], 
            pProtocolParams->pu8DeviceId,  
            DEVICE_ID_LENGTH);
	}

    if ( VLT_OK == status )
    {
        /*
         * Perform the sign operation using the host's private key.
         */
        status = EcdsaSignerInit( 
					&pCryptoParams->params.ecdsa.domainParams,
					&pCryptoParams->params.ecdsa.hostPrivateKey,
					NULL, 
					VLT_SIGN_MODE);

        if ( VLT_OK == status )
        {
            /*
             * Pass the message to the crypto to be signed .
             */
			u32SignatureLength = u16SignatureLength;
            status = EcdsaSignerDoFinal(
						au8Message, 
						MESSAGE_LENGTH, 
						MESSAGE_LENGTH,
						au8Signature, 
						&u32SignatureLength, 
						u32SignatureLength );

            if ( VLT_OK != status )
            {
                status = EINTERNALAUTHVERFAILED;
            }
        }
		/* clear down and free signer resources */
        EcdsaSignerClose( );
    }

    if ( VLT_OK == status )
    {
        status = VltExternalAuthenticate( 
			pProtocolParams->u8UserID,
            pProtocolParams->u8RoleID,
            HOST_CHALLENGE_LEN,
            au8HostChallenge,
            (VLT_U16)u32SignatureLength,
            au8Signature);

        if ( VLT_OK != status )
        {
            status = ESTRONGAUTHFAIL;
        }
        else
        {
            /* 
             * Set a flag to indicate the user is logged in. 
             */
            u8StrongAuthState = VLT_USER_AUTHENTICATED;
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */
}


VLT_STS VltEcdsaHostAssurance(								   
	VLT_SA_CRYPTO_PARAMS* pCryptoParams, 
	VLT_SA_PROTOCOL_PARAMS* pProtocolParams)
{
#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32SignatureLength = MAX_SIGNATURE_SIZE;
    VLT_U8 au8SignerID[VLT_GA_SIGNER_ID_LENGTH];
    VLT_U8 u8SignatureLength = VLT_GA_SIGNER_ID_LENGTH;
    
    /* Generate Assurance Message Structure Setup. */
    VLT_ASSURANCE_MESSAGE AssuranceMessage;
    AssuranceMessage.pu8AssuranceMessage = au8Message;
    AssuranceMessage.u8AssuranceMessageLength = VLT_GA_MESSAGE_LENGTH;
    
    /*
    * Check that the input parameters are valid
    */
    if ( ( NULL == pCryptoParams ) ||
         ( NULL == pProtocolParams ) )
    {
        return ESTRONGINITNULLPARAM;
    }
    
    /*
     * Generate a unique Signer ID for the session ( 8 byte number ).
     */
    status = VltGenerateRandom(VLT_GA_SIGNER_ID_LENGTH, &au8SignerID[0] );
    if( VLT_OK != status )
    {
        status = ESTRONGGENSIGNIDFAIL;
    }
   
    if ( VLT_OK == status )
    {
        /*
        * Have the device generate an assurance message 
        */        
        status = VltGenerateAssuranceMessage( 
			&u8SignatureLength,
            au8SignerID,
            &AssuranceMessage );

        if( VLT_OK != status )
        {
            status = ESTRONGGENASSURANCEFAIL;
        }
    }

    if ( VLT_OK == status )
    {
        /*
        * Sign the assurance message using the host's private key
        */        
        status = EcdsaSignerInit( 
					&pCryptoParams->params.ecdsa.domainParams,
					&pCryptoParams->params.ecdsa.hostPrivateKey,
					NULL, 
					VLT_SIGN_MODE);

        if ( VLT_OK == status )
        {           
            status = EcdsaSignerDoFinal(
				AssuranceMessage.pu8AssuranceMessage,
                AssuranceMessage.u8AssuranceMessageLength,
                AssuranceMessage.u8AssuranceMessageLength,
                au8Signature, 
                &u32SignatureLength, 
                u32SignatureLength);

        }
		/* clear down and free signer resources */
        EcdsaSignerClose( );
    }

    /*
     * Call verify assurance message to pass the signed assurance message
     * to the vaultic for verification.
     */
    if ( VLT_OK == status )
    {
		u8SignatureLength = (VLT_U8)u32SignatureLength; 
        status = VltVerifyAssuranceMessage( 
			u8SignatureLength, 
            au8Signature );

        /* Check the vault has reported the signed assurance was verified. */
        if ( VLT_OK != status )
        {
            status = ESTRONGVERASSMSGFAIL;
        }
    }       

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */
}


VLT_STS VltEcdsaStrongClose( void )
{
#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    /*
     * Changed the state variable to indicate the 
     * user is no longer authenticated.
     */
    u8StrongAuthState = VLT_USER_NOT_AUTHENTICATED;

    /*
     * Call cancel authentication to log the user out.
     */
    return VltCancelAuthentication( );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */
}

VLT_STS VltEcdsaStrongGetState( VLT_PU8 pu8State )
{
#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    /*
    * Check the validity of the input parameter
    */
    if( NULL == pu8State)
    {
        return SCPGETSTATENULLPARAM;
    }

    *pu8State = u8StrongAuthState;

    return( VLT_OK );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE ) */    
}


