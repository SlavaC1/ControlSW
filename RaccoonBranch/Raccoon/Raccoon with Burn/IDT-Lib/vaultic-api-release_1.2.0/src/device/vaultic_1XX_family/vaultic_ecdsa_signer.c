/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_mem.h"
#include "vaultic_utils.h"
#include "vaultic_ecdsa_signer.h"
#include "vaultic_elliptic-ff2n.h"
#include "vaultic_ff2n.h"
#include "vaultic_bigdigits.h"
#include "vaultic_sha256.h"
#include <time.h>
#include <stdlib.h>

/**
 * Private Defs
 */
#define ST_UNKNOWN				0x00
#define ST_INITIALISED_SIGN		0x10
#define ST_INITIALISED_VERIFY	0x20
#define ST_UPDATED				0x30
#define ST_FINALISED			0x40

#define MAX_BITS			303			/* maximum field order in bits */
#define MAX_BYTES			38			/* (MAX_BITS / 8) + 1  */
#define MAX_DIGITS			10			/* (MAX_BITS / 32) + 1 */
#define MAX_SIZE			MAX_DIGITS	
#define HASH_SIZE			256			/* SHA-256 bit size */
#define HASH_DIGIT_SIZE		(HASH_SIZE / (sizeof(DIGIT_T) * 8))	/* number of big digits in a hash */
#define HASH_BYTE_SIZE		(HASH_SIZE / (sizeof(VLT_U8) * 8))	/* number of bytes in a hash */

#define NUM_DIGITS(n)	((n) % sizeof(DIGIT_T) ? ((n) / sizeof(DIGIT_T)) + 1 : ((n) / sizeof(DIGIT_T)))

#define	MAX_RANDOM_BYTES	(255)	/* maximun return from Generate Random command */

/**
 * Private Data
 */
static VLT_U8 signerState = ST_UNKNOWN;

/* EC domain parameter object */
static E2n_Curve E;
static FF2N_ELT E_n[MAX_DIGITS];
static FF2N_ELT E_a[MAX_DIGITS];
static FF2N_ELT E_b[MAX_DIGITS];
static FF2N_ELT E_Gx[MAX_DIGITS];
static FF2N_ELT E_Gy[MAX_DIGITS];
static DIGIT_T E_r[MAX_DIGITS];


/* key storage in big digits format */
static FF2N_ELT sPublicKeyQx[MAX_DIGITS];
static FF2N_ELT sPublicKeyQy[MAX_DIGITS];
static DIGIT_T sPrivateKey[MAX_DIGITS];

/* number of big digits */
static VLT_U8 sNumFieldDigits = 0;
static VLT_U8 sNumFieldBytes = 0;

static VLT_U8 sNumBpOrderDigits = 0;
static VLT_U8 sNumBpOrderBytes = 0;

/* private helper function prototypes */
static VLT_STS GenerateRandomDigits(DIGIT_T *pBuffer, VLT_U16 numDigits);


/* --------------------------------------------------------------------------
 * EcdsaSignerInit
 * -------------------------------------------------------------------------- */
VLT_STS EcdsaSignerInit(
	const VLT_ECDSA_DOMAIN_PARAMS* pDomainParams, 
	const VLT_ECDSA_PRIVATE_KEY* pPrivateKey, 
	const VLT_ECDSA_PUBLIC_KEY* pPublicKey, 
    VLT_U8 u8OpMode)
{   
    UINT len;

    if ((NULL == pDomainParams ))
    {
        return (EECDSAINITNULLPARAM);
    }

    /* Check the operation mode is supported */
    if (VLT_SIGN_MODE == u8OpMode)   
	{
		/* SIGN needs a valid private key */
		if (NULL == pPrivateKey)
	        return (EECDSAINITNULLPARAM);

		if (NULL == pPrivateKey->pu8D)
		{
	        return EECDSAINITNULLPARAM;
		}
		
		if ((pPrivateKey->u16DLen == 0) ||
            (pPrivateKey->u16DLen > MAX_BYTES))
	        return EECDSAINVALIDPARAM;
    }
	else if (VLT_VERIFY_MODE == u8OpMode)
    {
		/* VERIFY needs a public private key */
		if (NULL == pPublicKey )
	        return (EECDSAINITNULLPARAM);

		if ((NULL == pPublicKey->pu8Qx) ||
			(NULL == pPublicKey->pu8Qy))
		{
	        return EECDSAINITNULLPARAM;
		}
		
		if ((pPublicKey->u16QLen == 0) ||
            (pPublicKey->u16QLen > MAX_BYTES))
	        return EECDSAINVALIDPARAM;
    }
	else 
    {
		/* invalid mode */
        return (EECDSAOPMODENOTSUPP);
    }

	/* validate domain params */
	if ((NULL == pDomainParams->pu8A) ||
		(NULL == pDomainParams->pu8B) ||
		(NULL == pDomainParams->pu8Gx) ||
		(NULL == pDomainParams->pu8Gy) ||
		(NULL == pDomainParams->pu8Gz) ||
		(NULL == pDomainParams->pu8N) ||
		(NULL == pDomainParams->pu8Q))
	        return (EECDSAINITNULLPARAM);

	if ( (pDomainParams->u16QLen == 0) ||
         (pDomainParams->u16QLen > MAX_BYTES))
	        return EECDSAINVALIDPARAM;

	if ((pDomainParams->u16NLen == 0) ||
        (pDomainParams->u16NLen > MAX_BYTES))
	        return EECDSAINVALIDPARAM;

	/* set-up number of big digits and bytes required to represent field elements */
	sNumFieldBytes = (VLT_U8)pDomainParams->u16QLen; 
	sNumFieldDigits = (VLT_U8)NUM_DIGITS(pDomainParams->u16QLen);

	/* base point order length may be significantly smaller than field size */
	sNumBpOrderBytes = (VLT_U8)pDomainParams->u16NLen; 
	sNumBpOrderDigits = (VLT_U8)NUM_DIGITS(pDomainParams->u16NLen);

	/* 
	 * set-up EC library domain parameter object. This requires
	 * type conversions and coercions from the VaultIC domain
	 * type defintions. In general we need to convert from 
	 * BYTE arrays in MSB to LSB order to big digit library arrays,  
	 * which are 32-bit integer arrays in LSB to MSB order.
	*/
	mpConvFromOctets(E_a, sNumFieldDigits, pDomainParams->pu8A, sNumFieldBytes);
	mpConvFromOctets(E_b, sNumFieldDigits, pDomainParams->pu8B, sNumFieldBytes);
	mpConvFromOctets(E_Gx, sNumFieldDigits, pDomainParams->pu8Gx, sNumFieldBytes);
	mpConvFromOctets(E_Gy, sNumFieldDigits, pDomainParams->pu8Gy, sNumFieldBytes);
	mpConvFromOctets(E_n, sNumFieldDigits, pDomainParams->pu8Q, sNumFieldBytes);
	mpConvFromOctets(E_r, sNumFieldDigits, pDomainParams->pu8N, sNumFieldBytes);

	/* set-up curve data structure */

	E.G.x =	E_Gx;				/* base generator point X co-ordinate */
	E.G.y =	E_Gy;				/* base generator point Y co-ordinate */
	E.a = E_a;  				/* curve equation co-efficient a */
	E.b = E_b;					/* curve equation co-efficient b */
	E.h = pDomainParams->u32H; 	/* co-factor */
	E.len = sNumFieldDigits; 	/* size of field in big digits	*/
	E.n = E_n;					/* reduction polynomial */
	E.r = E_r;					/* base point order */
	E.rlen = sNumFieldDigits;   /* base point order length in big digits */

	/* check field size is within bounds */
	len = mpBitLength(E.n, E.len);
	if (len > (MAX_BITS + 1)) 
	{
		return EECDSAINVALIDPARAM;
	}

	/* set-up key storage */
	if (VLT_VERIFY_MODE == u8OpMode)
    {
		mpConvFromOctets(sPublicKeyQx, sNumFieldDigits, 
			pPublicKey->pu8Qx, pPublicKey->u16QLen);
		mpConvFromOctets(sPublicKeyQy, sNumFieldDigits, 
			pPublicKey->pu8Qy, pPublicKey->u16QLen);
  	    signerState = ST_INITIALISED_VERIFY;
	}

	if (VLT_SIGN_MODE == u8OpMode)
    {
		mpConvFromOctets(sPrivateKey, sNumBpOrderDigits, 
			pPrivateKey->pu8D, pPrivateKey->u16DLen);
        signerState = ST_INITIALISED_SIGN;
	}

   /* Seed the random-number generator with current time so that
    * the numbers will be different every time we run.
    */
   srand( (unsigned)time( NULL ) );

   return VLT_OK;
}

/* --------------------------------------------------------------------------
 * EcdsaSignerClose
 * -------------------------------------------------------------------------- */
VLT_STS EcdsaSignerClose( void )
{
    VLT_STS status = VLT_OK;

	/* zeroise key storage */
	mpSetZero(sPublicKeyQx, sizeof(sPublicKeyQx) / sizeof(DIGIT_T));
	mpSetZero(sPublicKeyQy, sizeof(sPublicKeyQx) / sizeof(DIGIT_T));
	mpSetZero(sPrivateKey, sizeof(sPublicKeyQx) / sizeof(DIGIT_T));

	/* clear curve definition */
	host_memset((VLT_PU8)&E, 0, sizeof(E));

    signerState = ST_UNKNOWN;

    return status;
}


/* --------------------------------------------------------------------------
 * EcdsaSignerDoFinal
 * -------------------------------------------------------------------------- */
VLT_STS EcdsaSignerDoFinal(
    VLT_PU8 pu8Message, 
    VLT_U32 u32MessageLen, 
    VLT_U32 u32MessageCapacity, 
    VLT_PU8 pu8Signature, 
    VLT_PU32 pu32SignatureLen, 
    VLT_U32 u32SignatureCapacity )
{
	E2n_Point P;
	E2n_Point R;
	E2n_Point Q;

	/* intermediate calculation storage */
	DIGIT_T k[MAX_DIGITS];
	DIGIT_T k1[MAX_DIGITS]; 
	DIGIT_T	tmp[MAX_DIGITS];
	DIGIT_T r[MAX_DIGITS]; 
	DIGIT_T s[MAX_DIGITS]; 
	DIGIT_T u1[MAX_DIGITS];
	DIGIT_T u2[MAX_DIGITS]; 
	DIGIT_T v[MAX_DIGITS]; 
	DIGIT_T yy[MAX_DIGITS];

	DIGIT_T Px[MAX_DIGITS];
	DIGIT_T Py[MAX_DIGITS];

	DIGIT_T Rx[MAX_DIGITS];
	DIGIT_T Ry[MAX_DIGITS];

	DIGIT_T Qx[MAX_DIGITS];
	DIGIT_T Qy[MAX_DIGITS];

	/* SHA-256 storage */
	DIGIT_T	bdHash[MAX_DIGITS];
	VLT_U8 bHash[HASH_BYTE_SIZE];

	UINT len;
	UINT hashLen;

	sha256_ctx ctx; // context holder

    VLT_STS status = VLT_FAIL;

	if((ST_INITIALISED_SIGN != signerState) &&
	   (ST_INITIALISED_VERIFY != signerState))
	{
		/* not initialised */
		return EECDSAEXECUTIONERROR;
	}

	/* Initialise Point variables */
	P.x = Px;
	P.y = Py;
	R.x = Rx;
	R.y = Ry;
	Q.x = Qx;
	Q.y = Qy;

    if ( ( NULL == pu8Message ) ||
         ( NULL == pu8Signature ) ||
         ( NULL == pu32SignatureLen ) )
    {
        return ( EECDSAINUPNULLPARAM );
    }

	/* hash of message used by both signing and verify */

	/* e or e1 = SHA-256(M) */
	sha256_begin(&ctx);
    sha256_hash(pu8Message, u32MessageLen, &ctx);
    sha256_end(bHash, &ctx);

	/* convert hash to big digits, 
	same size as base point order if > hash size */
	if (sNumBpOrderDigits > HASH_DIGIT_SIZE)
		hashLen = sNumBpOrderDigits;
	else
		hashLen = HASH_DIGIT_SIZE;
	mpConvFromOctets(bdHash, hashLen, bHash, HASH_BYTE_SIZE);

	/* ANS X9.62-2005 7.3.e
	// if bit length of hash is > bit length of base point order
	// then truncate hash by removing LSBs until bit length
	// equals the length of the base point order
	*/
	len = mpBitLength(E.r, E.rlen);
	if (len < HASH_SIZE)
	{	
		/* take leftmost bits of message by shifting right */
		mpShiftRight(tmp, bdHash, HASH_SIZE - len, hashLen);
		/* truncate to base point order size */
		mpSetEqual(bdHash, tmp, E.rlen);
	}

	if (ST_INITIALISED_SIGN == signerState)
	{
		/* signing process as per ANS X9.62 Section 7.3 */
		*pu32SignatureLen = 0;

		/* generate ephemeral private key k such that 0 < k < n */			 
		if (VLT_OK != GenerateRandomDigits(tmp, E.rlen))
			return EECDSAEXECUTIONERROR;
		mpModulo(k, tmp, E.rlen, E.r, E.rlen);
		if (mpIsZero(k, E.rlen))
		{
			/* probability of a zero is 1/n */
			if (VLT_OK != GenerateRandomDigits(tmp, E.rlen))
				return EECDSAEXECUTIONERROR;
			mpModulo(k, tmp, E.rlen, E.r, E.rlen);
			if (mpIsZero(k, E.rlen))
			{
				return EECDSAEXECUTIONERROR;
			}
		}

		/* generate ephemeral public key: P = kG */
		e2n_point_mul(&E, &P, &E.G, k, E.rlen);

		/* convert P.x to integer j	*/
		/* conversion is implicit for polynomial basis */

		/*
		// r = j mod n, n = base point oder (E.r)
		*/
		mpModulo(r, P.x, E.rlen, E.r, E.rlen);

		/*
		// calculate s = k^-1 (e + dr) mod n
		*/

		/* Compute k' = k^-1 mod n */
		mpModInv(k1, k, E.r, E.rlen);

		/* Compute s = (k^-1(SHA-xxx(M) + dr)) mod n */

		/* d * r */
		mpModMult(tmp, sPrivateKey, r, E.r, E.rlen);
		/* M + d * r */
		mpModAdd(yy, tmp, bdHash, E.r, E.rlen);
		/* s = (k^-1)(M + dr) */
		mpModMult(s, k1, yy, E.r, E.rlen);
	
		/* signing: convert back to byte format and construct r || s */
		mpConvToOctets(r, sNumBpOrderDigits, pu8Signature, sNumBpOrderBytes);
		mpConvToOctets(s, sNumBpOrderDigits, pu8Signature + sNumBpOrderBytes, 
			sNumBpOrderBytes);

		/* set the byte length of the output signature */
		*pu32SignatureLen = sNumBpOrderBytes * 2;

		status = VLT_OK;
	}
	else
	{
	    /* ANS X9.62-2005 Section 7.4.1: Verification with Public Key */;

		/* extract r & s and format as big digits */
		mpConvFromOctets(r, E.rlen, pu8Signature, (*pu32SignatureLen) / 2);
		mpConvFromOctets(s, E.rlen, pu8Signature + (*pu32SignatureLen / 2), 
			(*pu32SignatureLen) / 2);

		/* Compute u1 = e1(s1^-1) mod n */
		mpModInv(tmp, s, E.r, E.rlen);
		mpModMult(u1, tmp, bdHash, E.r, E.rlen);

		/* Compute u2 = r1(s1^-1) mod n */
		mpModMult(u2, tmp, r, E.r, E.rlen);

		/* use supplied public key */
		mpSetEqual(Q.x, sPublicKeyQx, E.len);
		mpSetEqual(Q.y, sPublicKeyQy, E.len);

		/* compute R = u1G */
		e2n_point_mul(&E, &R, &E.G, u1, E.rlen);

		/* P = u2Q */
		e2n_point_mul(&E, &P, &Q, u2, E.rlen);

		/* R = R + P */
		e2n_point_add(&E, &R, &R, &P);

		/* compute v = j mod n */
		mpModulo(v, R.x, E.rlen, E.r, E.rlen);

		/* verify v == r */
		if (mpEqual(v, r, E.rlen))
		{	
			status = VLT_OK;
		}
		else
		{
			status = VLT_FAIL;
		}
	}

    return ( status );
}

/* --------------------------------------------------------------------------
 * EcdsaSignerUpdate - not required at the moment
 * -------------------------------------------------------------------------- */
VLT_STS EcdsaSignerUpdate( VLT_PU8 pu8Message, 
    VLT_U32 u32MessageLen, 
    VLT_U32 u32MessageCapacity )
{
    VLT_STS status = VLT_FAIL;

    
    if ( NULL == pu8Message )
    {
        return ( EECDSAINUPNULLPARAM );
    }

    return( status );
}


/* pseudorandom digits using CRT library */
VLT_STS GenerateRandomDigits(DIGIT_T *pBuffer, VLT_U16 numDigits)
{
	UINT i;

	if (NULL == pBuffer)
		return VLT_FAIL;

	if (numDigits > MAX_DIGITS)
		return VLT_FAIL;

	for (i = 0; i < numDigits; i++)
	{
		*pBuffer++ = (DIGIT_T)rand();
		if (i != 0  && (*(pBuffer - 1 ) == *(pBuffer - 2)))
			/* PRNG stuck fault */
			return VLT_FAIL;
	}		
	return VLT_OK;
}

