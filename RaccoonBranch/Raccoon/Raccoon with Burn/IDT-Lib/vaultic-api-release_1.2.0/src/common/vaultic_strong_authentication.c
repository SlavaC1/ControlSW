/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_strong_authentication.h
 * 
 * \brief Interface to strong authentication.
 * 
 * \par Description:
 * TBD.
 */

#include "vaultic_common.h"
#include "vaultic_strong_authentication.h"
#include "vaultic_api.h"
#include "vaultic_mem.h"
#include "vaultic_crypto.h"



/**
 * Local definitions
 */
#define HOST_CHALLENGE_LEN              (VLT_U8)8
#define MAX_SIGNATURE_SIZE              (VLT_U8)76
#define HOST_ID_LENGTH                  (VLT_U8)16
#define DEVICE_ID_LENGTH                (VLT_U8)16



#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    /*
    * Local Variables used by Strong Authentication
    */
    VLT_U8 u8StrongAuthState = VLT_USER_NOT_AUTHENTICATED;
    VLT_U8 au8Signature[MAX_SIGNATURE_SIZE];
    VLT_U8 au8Message[VLT_GA_MESSAGE_LENGTH];

#endif // #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )


VLT_STS VltStrongInit( VLT_STRONG_AUTH_PARAMS* pAuthParams )
{
#if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
    
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16SignatureLength = MAX_SIGNATURE_SIZE;
    VLT_U8 au8HostChallenge[HOST_CHALLENGE_LEN];
    VLT_U8 au8DeviceChallenge[HOST_CHALLENGE_LEN];
    
    VLT_U8 au8SignerID[VLT_GA_SIGNER_ID_LENGTH];
    

    /* Generate Assurance Message Structure Setup. */
    VLT_ASSURANCE_MESSAGE AssuranceMessage;
    AssuranceMessage.pu8AssuranceMessage = au8Message;
    AssuranceMessage.u8AssuranceMessageLength = VLT_GA_MESSAGE_LENGTH;
    

    
    /*
    * Check that the input parameters are valid
    */
    if ( ( NULL == pAuthParams ) ||
         ( NULL == pAuthParams->pu8HostId ) || 
         ( NULL == pAuthParams->pu8DeviceId ) )
    {
        return ESTRONGINITNULLPARAM;
    }


    /*
    * Generate Host Challenge ( string of random characters usually 8 bytes )
    */
    status = VltGenerateRandom(
        HOST_CHALLENGE_LEN, 
        &au8HostChallenge[0] );

   
    /*
    * Check that the call to Generate Random was successful
    */
    if( VLT_OK != status )
    {
        status = ESTRONGGENRANDFAIL;
    }

    
    /*
     * Generate a unique Signer ID for the session ( 8 byte number ).
     */
    if ( VLT_OK ==  status )
    {
        status = VltGenerateRandom(
            HOST_CHALLENGE_LEN, 
            &au8SignerID[0] );

        /*
        * Check that the call to Generate Random was successful
        */
        if( VLT_OK != status )
        {
            status = ESTRONGGENSIGNIDFAIL;
        }
    }
    

    
    /*
     * Only call Internal Authenticate if Generate Random passed.
     */
    if ( status == VLT_OK)
    {
        /*
         * Generate random for the host challenge was successful
         * call VltInternalAuthenticate to start the auth process.
         */
        status = VltInternalAuthenticate( pAuthParams->u8UserID,
            pAuthParams->u8RoleID,
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
        status = CryptoInit( &pAuthParams->VerifyingKey,
            VLT_VERIFY_MODE,
            &pAuthParams->AlgoParams );

        /* Check the crypto service was constructed correctly */
        if ( VLT_OK == status )
        {
            /*
             * Pass the signature to the crypto to be verified.
             * There will be no output (verify) so it is ok to pass
             * the signature buffer for the output.
             */
            status = CryptoDoFinal( au8Signature, 
                u16SignatureLength, 
                u16SignatureLength,
                au8Signature, 
                (VLT_PU32)&u16SignatureLength, 
                u16SignatureLength );

            if ( VLT_OK != status )
            {
                /* Failed the verify operation, don't continue, return an error. */
                status = EINTERNALAUTHVERFAILED;
            }

            status = CryptoClose( );
        }
    }






    /*
     * If the signature was verified, continue and call 
     * generate assurance message.
     */
    if ( VLT_OK == status )
    {
        u16SignatureLength = VLT_GA_SIGNER_ID_LENGTH;
        
        status = VltGenerateAssuranceMessage( (VLT_PU8)&u16SignatureLength,
            &au8SignerID[0],
            &AssuranceMessage );

        /*
        * Check that the call to Generate Assurance Message was successful
        */
        if( VLT_OK != status )
        {
            status = ESTRONGGENASSURANCEFAIL;
        }
    }


    /*
     * Sign the Assurance Message to pass back for assurance validation.
     */
    if ( VLT_OK == status )
    {
        /*
         * Calculate the Signed assurance message
         */
        status = CryptoInit( &pAuthParams->SigningKey,
            VLT_SIGN_MODE,
            &pAuthParams->AlgoParams );

        /* Check the crypto service was constructed correctly. */
        if ( VLT_OK == status )
        {
            /* Sign the hosts private key AssuranceMessage.pu8AssuranceMessage. */
            status = CryptoDoFinal( AssuranceMessage.pu8AssuranceMessage,
                AssuranceMessage.u8AssuranceMessageLength,
                AssuranceMessage.u8AssuranceMessageLength,
                au8Signature, 
                (VLT_PU32)&u16SignatureLength, 
                u16SignatureLength );

            /* Check the status */
            if ( VLT_OK != status )
            {
                status = EASSURANCEMSGSIGNFAILED;
            }

            status = CryptoClose( );
        }
    }



    /*
     * Call verify assurance message to pass the signed assurance message
     * to the vaultic for verification.
     */
    if ( VLT_OK == status )
    {
        status = VltVerifyAssuranceMessage( (VLT_U8)&u16SignatureLength, 
            au8Signature );

        /* Check the vault has reported the signed assurance was verified. */
        if ( VLT_OK != status )
        {
            status = ESTRONGVERASSMSGFAIL;
        }
    }       


    /*
     * If the signed assurance message was verified, build a message to sign,
     * and have the device verify it.  This will authenticate the host (user)
     to the vaultic.  The structure of the message:
     * SIGN ( Ch || Cd || [DevID] )
     */
    if ( VLT_OK == status )
    {
        /* Construct a message to SIGN ( Cd || Ch || [ HostID ] ) */
        (void)host_memcpy( &au8Message[0], 
            au8DeviceChallenge, 
            HOST_CHALLENGE_LEN );

        (void)host_memcpy( &au8Message[HOST_CHALLENGE_LEN], 
            au8HostChallenge, 
            HOST_CHALLENGE_LEN );

        (void)host_memcpy( &au8Message[HOST_ID_LENGTH], 
            pAuthParams->pu8DeviceId,  
            HOST_ID_LENGTH);


        
        /*
         * Calculate the Signed assurance message
         */
        status = CryptoInit( &pAuthParams->SigningKey,
            VLT_SIGN_MODE,
            &pAuthParams->AlgoParams );


        /* Check the crypto service was constructed correctly*/
        if ( VLT_OK == status )
        {
            u16SignatureLength = sizeof( au8Signature );
            /* 
             * Sign the message using the hosts private key. 
             */
            status = CryptoDoFinal( au8Message,
                VLT_GA_MESSAGE_LENGTH,
                VLT_GA_MESSAGE_LENGTH,
                au8Signature, 
                (VLT_PU32)&u16SignatureLength, 
                u16SignatureLength );

            /* Check the status */
            if ( VLT_OK != status )
            {
                status = EEXTERNALAUTHSIGNFAILED;
            }

            status = CryptoClose( );
        }
    }

    /*
     * Ask the device to verify the SIGN ( Cd || Ch || [ HostID ] )
     * send the Host Challenge - Ch, and the signature to the 
     * device using the External Authenticate Command.
     */
    if ( VLT_OK == status )
    {
        status = VltExternalAuthenticate( pAuthParams->u8UserID,
            pAuthParams->u8RoleID,
            HOST_CHALLENGE_LEN,
            au8HostChallenge,
            u16SignatureLength,
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



VLT_STS VltStrongClose( void )
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

VLT_STS VltStrongGetState( VLT_PU8 pu8State )
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


