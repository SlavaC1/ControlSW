/* --------------------------------------------------------------------------
 * VaultIC API for the VaultIC 100 family of devices.
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_comms.h"
#include "vaultic_utils.h"
#include "vaultic_mem.h"
#include "vaultic_apdu.h"
#include "vaultic_key_aux.h"
#include "vaultic_raw_key_aux.h"
#include "vaultic_ecdsa_key_aux.h"
#include "vaultic_id_key_aux.h"
#include "vaultic_command.h"
#include "vaultic_api.h"
#include "vaultic_version.h"
#include "vaultic_sha256.h"

/*
 * Local consts 
 */
#define VLT_STATE_SEND_MESSAGE      (VLT_U8)0x00
#define VLT_STATE_SEND_SIGNATURE    (VLT_U8)0x01
#define VLT_STATE_SEND_COMPLETE     (VLT_U8)0x02


static VLT_U8 vltApiVersion[] = {VERSION};

/*
 * Local variables
 */
VLT_MEM_BLOB Command;
VLT_MEM_BLOB Response;
VLT_U8 apduHeader[VLT_APDU_TYPICAL_HEADER_SZ];
VLT_U16 idx;

VLT_STS VltApiInit(VLT_INIT_COMMS_PARAMS* pInitCommsParams)
{           
/* Only perform the cipher tests if required. */
#if ( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )
    DoCipherTests(); 
#endif

    return( VltCommsInit( pInitCommsParams, &Command, &Response ) );
}

VLT_STS VltApiClose( void )
{
    return( VltCommsClose( ) );
}

/* --------------------------------------------------------------------------
 * IDENTITY AUTHENTICATION COMMANDS
 * -------------------------------------------------------------------------- */

VLT_STS VltSubmitPassword(
    VLT_U8 u8PasswordLength,
    const VLT_U8 *pu8Password)
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;    
    idx = VLT_APDU_DATA_OFFSET;

    /*
     * Validate all input parameters.
     */                   
    if( NULL == pu8Password )
    {
        return( ESPNULLPARA );
    }

    /*
     * A password must VLT_PASSWORD_REQUIRED_LEN bytes in length
     */
    if( VLT_PASSWORD_REQUIRED_LEN != u8PasswordLength )
    {
        return( ESPPASSLENIVLD );
    }

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SUBMIT_PASSWORD;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = VLT_CREATOR;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = VLT_MANUFACTURER;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(u8PasswordLength);

    /* Build Data In */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8Password, u8PasswordLength);
    idx += u8PasswordLength;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw);

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status ); 
}


/* Note: Manage Authentication Data is documented as permitting command
 * chaining but it never should need to use it as the P3 length field should
 * not exceed 0x27 bytes. */

VLT_STS VltManageAuthenticationData( const VLT_MANAGE_AUTH_DATA *pAuthSetup )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    VLT_U16 u16Len = 0;    
    idx = VLT_APDU_DATA_OFFSET;

    if( NULL == pAuthSetup )
    {
        return( EMADNULLPARA );
    }

    /* Build APDU */        
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_MANAGE_AUTHENTICATION_DATA;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = pAuthSetup->u8UserID;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = VLT_UPDATE_USER;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;


    /* Build Data In */
    Command.pu8Data[idx++] = pAuthSetup->u8Method;
    Command.pu8Data[idx++] = pAuthSetup->u8RoleID;
    Command.pu8Data[idx++] = 0;
    Command.pu8Data[idx++] = 0;
    Command.pu8Data[idx++] = pAuthSetup->u8TryCount;

    switch( pAuthSetup->u8Method )
    {
        case VLT_LOGIN_PASSWORD:
            /*
             * A password must be VLT_PASSWORD_REQUIRED_LEN bytes in length
             */
            if( VLT_PASSWORD_REQUIRED_LEN != pAuthSetup->cleartext.u8PasswordLength )
            {
                return( EMADPASSLENIVLD );
            }

            u16Len = pAuthSetup->cleartext.u8PasswordLength;

            Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 0) & 0xFF);

            /*
             * No need to check the return type as pointer has been validated
             */
            (void)host_memcpy( &Command.pu8Data[idx], 
                            pAuthSetup->cleartext.u8Password,
                pAuthSetup->cleartext.u8PasswordLength );

            idx += pAuthSetup->cleartext.u8PasswordLength;
            break;
        
        case VLT_LOGIN_STRONG:
            /* Only the length argument of 0 is required for Strong Authentication .  */
            Command.pu8Data[idx++] = 0;
            Command.pu8Data[idx++] = 0;

            break;

        default:
            return( EMADBADOPER );
    }

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return status;
}

VLT_STS VltGetAuthenticationInfo( VLT_U8 u8UserID, 
    VLT_AUTH_INFO *pRespData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;
    
    if( NULL == pRespData )
    {
        return( EGAINULLPARA );
    }    

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GET_AUTHENTICATION_INFO;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8UserID;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LEXP(VLT_GET_AUTH_INFO_P3);
    /* Send the command */
    status = VltCommand( &Command, &Response, idx, VLT_GET_AUTH_INFO_P3, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return( status );
    }

    /* Unpack the response */
    idx = 0;    
    pRespData->u8AuthMethod = Response.pu8Data[idx++];
    pRespData->u8Roles = Response.pu8Data[idx++];
    idx++; // Skip security level
    pRespData->u8RemainingTryCount = Response.pu8Data[idx++];
    pRespData->u8MaxTries = Response.pu8Data[idx++];
    pRespData->u16SequenceCount = VltEndianReadPU16( &Response.pu8Data[idx] );

    return( status );
}

VLT_STS VltCancelAuthentication( void )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_CANCEL_AUTHENTICATION;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltInternalAuthenticate(VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8ChallengeLengths,
    const VLT_U8 *pu8HostChallenge,
    VLT_PU8 pu8DeviceChallenge,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE; 
    VLT_U16 count = 0;   
    VLT_BOOL bSgnRcvd = FALSE ;
    idx = VLT_APDU_DATA_OFFSET;

    /*
     * Validate critical input parameters.
     */
    if( ( NULL == pu8HostChallenge ) ||
        ( NULL == pu8DeviceChallenge ) ||
        ( NULL == pu16SignatureLength )  ||        
        ( NULL == pu8Signature ) )
    {
        return( EGIANULLPARA );
    }

    if( ( 0 == *pu16SignatureLength ) || 
        ( u8ChallengeLengths != VLT_INTERNAL_AUTH_HOST_CHALLENGE_LENGTH ) )
    {
        return( EGIAIVLDSIGLEN );
    }

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GEN_INTERNAL_AUTHENTICATE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8UserID;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8RoleID;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( apduHeader, Command.pu8Data, VLT_APDU_TYPICAL_HEADER_SZ );

    /* Append the host challenge. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], 
        pu8HostChallenge, 
        VLT_INTERNAL_AUTH_HOST_CHALLENGE_LENGTH );

    idx += VLT_INTERNAL_AUTH_HOST_CHALLENGE_LENGTH;

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN( 
        WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /* Check the status word */
    if( ( Sw != VLT_STATUS_NONE ) && 
        ( Sw != VLT_STATUS_SUCCESS ) && 
        ( Sw != VLT_STATUS_RESPONDING ) )
    {
        return( Sw );
    }
    /* Ensure the command didn't just failed */
    if( VLT_OK != status )
    {
        return( status );
    }

    /* Remove the status word size from the response length */
    Response.u16Len -= VLT_SW_SIZE ;

    /* Unpack the response */
    idx = 0;

    /* Unpack the device challenge */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8DeviceChallenge, 
        &Response.pu8Data[idx], 
        VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH );

    idx += VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH;
    Response.u16Len -= VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH;

    /* Unpack the signature which may span more than one transaction */
    do
    {
        /* Check to see if we have received the whole signature */
        if( Sw == VLT_STATUS_RESPONDING )
        {
            bSgnRcvd = FALSE;
        }
        else
        {
            bSgnRcvd = TRUE;
        }

        /* Do we have enough space to copy the signature out to the caller? */
        if( *pu16SignatureLength < ( Response.u16Len + count ) )
        {
            return( EGIANOROOM );
        }
        else
        {
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &pu8Signature[ count ], 
                &Response.pu8Data[ idx ], 
                Response.u16Len );

            count += Response.u16Len;
        }

        if( FALSE == bSgnRcvd )
        {
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( Command.pu8Data, apduHeader, VLT_APDU_TYPICAL_HEADER_SZ );

            /* Send the command */
            status = VltCommand( &Command, 
                &Response, 
                VLT_APDU_TYPICAL_HEADER_SZ,
                0, 
                &Sw );

            if( ( Sw != VLT_STATUS_NONE ) && 
                ( Sw != VLT_STATUS_SUCCESS ) && 
                ( Sw != VLT_STATUS_RESPONDING ) )
            {
                return( Sw );
            }

            if( VLT_OK != status )
            {
                return( status );
            } 

            /* Remove the status word size from the response length */
            Response.u16Len -= VLT_SW_SIZE ;
            /* Get the pointer to the response data */
            idx = 0;
        }
    }
    while( FALSE == bSgnRcvd );
    
    /* Let the caller know the size of the signature received */
    *pu16SignatureLength = count;

    return( status );
}


VLT_STS VltExternalAuthenticate( VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8HostChallengeLength,
    const VLT_U8 *pu8HostChallenge,
    VLT_U16 u16HostSignatureLength,
    const VLT_U8 *pu8HostSignature )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    VLT_U16 size = 0;
    VLT_U16 count = 0;
    VLT_U16 chunk = 0;
    idx = 0;

    if( ( NULL == pu8HostChallenge ) ||        
        ( NULL == pu8HostSignature ) )
    {
        return( EGEANULLPARA );
    }    

    if( VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH != u8HostChallengeLength )
    {
        return( EGEAINVLDHOSTCHLEN );
    }

    if( ( 0 == u16HostSignatureLength )||
        ( u16HostSignatureLength > ( VltCommsGetMaxSendSize() - 
        VLT_INTERNAL_AUTH_DEV_CHALLENGE_LENGTH) ) )
    {
        return( EGEAINVLDHOSTSIGLEN );
    }

    /* Total size of the payload */
    size = ( u16HostSignatureLength + u8HostChallengeLength );

    do 
    {
        /* Remaining data to send */
        chunk = (size - count);
        
        /*
         * If the remaining data to send is larger than our 
         * maximum buffer size return an error as chaining 
         * is not supported.
         */
        if( chunk > VltCommsGetMaxSendSize() )
        {
            return( EGEAINVLDHOSTSIGLEN );
        }
        else
        {            
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL; 
        }

        /* Set up the apdu */
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_GEN_EXTERNAL_AUTHENTICATE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = VLT_USER;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = VLT_APPROVED_USER;        
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = LIN(WRAPPED_BYTE(chunk));

        /*        
         * If this is the first time we transfer data then we need to 
         * account for the challenge part of the message. Otherwise
         * index in the right place of the signature and send the rest
         * of the data.
         */
        if( 0 == count )
        {
            /* Copy in the host challenge*/
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[ VLT_APDU_DATA_OFFSET ], 
                pu8HostChallenge,             
                u8HostChallengeLength);            

            /* Update the index in the signature */
            idx = chunk - u8HostChallengeLength;

            /* Copy in part of the signature */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[ ( VLT_APDU_DATA_OFFSET + u8HostChallengeLength ) ], 
                pu8HostSignature, 
                idx);            
        }
        else
        {
            /* Copy in part of the signature */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[ VLT_APDU_DATA_OFFSET ], 
                &pu8HostSignature[idx], 
                chunk ); 

             /* Update the index in the signature */
            idx += chunk;
        }   

        /* Send the command */
        status = VltCommand( &Command, 
            &Response, 
            ( chunk + VLT_APDU_DATA_OFFSET ), 
            0, 
            &Sw );

        /* React to the status word */
        switch( Sw )
        {
            case VLT_STATUS_COMPLETED:
            case VLT_STATUS_SUCCESS:
                break;
            case VLT_STATUS_NONE:                
                return( status );
            default:
                return( Sw ); /* unexpected status word */
        }

        if (VLT_OK != status)
        {
            return status;
        }       
        
        /* Update the transfer progress */
        count += chunk;
    }
    while( count <  size );

    return( status );
}


/* --------------------------------------------------------------------------
 * CRYPTO SERVICES
 * -------------------------------------------------------------------------- */

VLT_STS VltInitializeAlgorithm(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_U8 u8Mode,
    const VLT_ALGO_PARAMS *pAlgorithm )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    idx = VLT_APDU_DATA_OFFSET;

    /* Check all pointers are valid pointer in the input arguments */
    if ( NULL == pAlgorithm )
    {
        return( EIANULLPARA );
    }

    
    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_INITIALIZE_ALGORITHM;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
    /* P3 is filled out once the data has been built */

    /* Build Data In */

    Command.pu8Data[idx++] = u8Mode;
    Command.pu8Data[idx++] = pAlgorithm->u8AlgoID;

    switch( pAlgorithm->u8AlgoID )
    {
        case VLT_ALG_SIG_ECDSA_GF2M: /* unsure about this one */
            Command.pu8Data[idx++] = pAlgorithm->EcdsaDsa.u8Digest;
            break;

        case VLT_ALG_DIG_SHA256:
            /* no algorithm parameters */
            break;

        default:
            return( EIABADALGO ); /* unrecognised algorithm */
    }

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] =
        LIN(WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}



VLT_STS VltPutKey(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
    const VLT_KEY_OBJECT *pKeyObj )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    if( ( NULL == pKeyFilePrivileges ) ||
        ( NULL == pKeyObj ) )
    {
        return( EPKNULLPARA );
    }

    switch( pKeyObj->u8KeyID )
    {
        case VLT_KEY_RAW:
            status = VltPutKey_Raw(u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                &(pKeyObj->data.RawKey),
                &Sw );
            break;
        case VLT_KEY_ECDSA_GF2M_PUB: /* ECDSA public key object */
            status = VltPutKey_EcdsaPublic( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.EcdsaPubKey),
                &Sw );
            break;

        case VLT_KEY_ECDSA_GF2M_PRIV: /* ECDSA private key object */
            status = VltPutKey_EcdsaPrivate( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.EcdsaPrivKey),
                &Sw );
            break;
        case VLT_KEY_ECDSA_DOMAIN_PARMS: /* ECDSA Domain key object */
            status = VltPutKey_EcdsaParams( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.EcdsaParamsKey),
                &Sw );
            break;
        case VLT_KEY_HOST_DEVICE_ID: /* Host/Device ID key object */
            status = VltPutKey_IdKey( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.HostDeviceIdKey ),
                &Sw );
            break;


        default:
            return( VLT_FAIL );
    }

    if( ( VLT_OK == status ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltReadKey( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_KEY_OBJECT *pKeyObj )
{
    VLT_SW Sw = VLT_STATUS_NONE;    
    VLT_STS status = VLT_FAIL;  
    

    if( NULL == pKeyObj )
    {
        return( ERKNULLPARA );
    }

    /* initialise the crc for the read key*/
    ReadKeyInitCrc();

    /* read the first part of the key */
    status = VltReadKeyCommand( &Command, &Response, u8KeyGroup, u8KeyIndex, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && 
        ( Sw != VLT_STATUS_SUCCESS ) && 
        ( Sw != VLT_STATUS_RESPONDING ) &&
        ( Sw != VLT_STATUS_EOF )   )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return( status );
    }

    /* We're going to rely on the client to expect the right sort of key, since
     * they have to allocate all of the data buffers required before we're
     * called. However, we need some level of safety so we require that the
     * client fill out the pKeyObj->u8KeyID with the type of key they're
     * expecting. If when we retrieve the key it's not the expected type then we
     * terminate early with an error. */

    if( VLT_KEY_RAW != pKeyObj->u8KeyID )
    {
        if( Response.pu8Data[0] != pKeyObj->u8KeyID )
        {
            return( ERKMISMATCH );
        }
    }
    
    switch( pKeyObj->u8KeyID )
    {
        case VLT_KEY_RAW:
            return( VltReadKey_Raw( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.RawKey, &Sw ) );

        /* ECDSA public key object */
        case VLT_KEY_ECDSA_GF2M_PUB:             
            return( VltReadKey_EcdsaPublic( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaPubKey, &Sw ) );
        /* ECDSA private key object */
        case VLT_KEY_ECDSA_GF2M_PRIV:             
            return( VltReadKey_EcdsaPrivate( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaPrivKey, &Sw ) );

        
        /* ECDSA Params key object */
        case VLT_KEY_ECDSA_DOMAIN_PARMS:
            return( VltReadKey_EcdsaParams( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaParamsKey, &Sw ) );

        /* Host/Device ID key object */
        case VLT_KEY_HOST_DEVICE_ID:
            return( VltReadKey_IdKey( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.HostDeviceIdKey, &Sw ) );

        default:
            return( ERKUNSUPPKEY );
    }
}

VLT_STS VltGenerateAssuranceMessage( VLT_PU8 pu8SignerIdLength,
    VLT_PU8 pu8SignerID,
    VLT_ASSURANCE_MESSAGE* pAssuranceMsg  )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;   

    
    /* validate critical parameters */
    if( ( NULL == pAssuranceMsg ) || 
        ( NULL == pu8SignerID ) || 
        ( NULL == pu8SignerIdLength )||
        ( NULL == pAssuranceMsg->pu8AssuranceMessage ) ||
        ( NULL == pAssuranceMsg->pu8VerifierID ) )
    {
        return( EGASNULLPARA );
    }

    /** 
     * validate the signer ID, Verifier ID, and 
     * Assurance Message buffer lengths
     * match the required length 
     */
    if ( ( VLT_GA_SIGNER_ID_LENGTH != *pu8SignerIdLength ) ||
         ( pAssuranceMsg->u8VerifierIdLength < VLT_GA_VERIFIER_ID_LENGTH ) ||
         ( pAssuranceMsg->u8AssuranceMessageLength < VLT_GA_MESSAGE_LENGTH ))
    {
        return ( EGASINVLDLEN );
    }

    /* set the index offset */
    idx = VLT_APDU_DATA_OFFSET;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GENERATE_ASSURANCE_MESSAGE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LEXP( *pu8SignerIdLength );

    /* copy the Signer ID into the command buffer */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8SignerID, *pu8SignerIdLength );
    idx += *pu8SignerIdLength;

    /* send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if (VLT_OK != status)
    {
        return status;
    }

    Response.u16Len -= VLT_SW_SIZE;

    /**
     * Check the length of the response is equal to the expected length
    */
    if ( VLT_GA_MESSAGE_LENGTH != Response.u16Len )
    {
        return ( EGAINVLDRECLEN );
    }
    

    /* Unpack the response */    
    idx = 0;
    
    
    
    /* Copy out the assurance message byte stream. */
    host_memcpy( pAssuranceMsg->pu8AssuranceMessage, &Response.pu8Data[idx], VLT_GA_MESSAGE_LENGTH );
    pAssuranceMsg->u8AssuranceMessageLength = VLT_GA_MESSAGE_LENGTH;
    idx += VLT_GA_MESSAGE_LENGTH;
 
    return( status );    
}


VLT_STS VltVerifyAssuranceMessage(VLT_U8 u8SignedAssuranceMsgLength,
    VLT_PU8 pu8SignedAssuranceMsg )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;   

    
    /* validate critical parameters */
    if ( NULL == pu8SignedAssuranceMsg ) 
    {
        return( EGASNULLPARA );
    }

    

    
    /* set the index offset */
    idx = VLT_APDU_DATA_OFFSET;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GENERATE_ASSURANCE_MESSAGE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LEXP( u8SignedAssuranceMsgLength );

    /* copy the Signer ID into the command buffer */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], 
        pu8SignedAssuranceMsg, 
        u8SignedAssuranceMsgLength );
	idx += u8SignedAssuranceMsgLength;

    /* send the command */
    status = VltCommand( &Command, &Response, idx , 0, &Sw );

    
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }
    
    return( status );    
}


VLT_STS VltGenerateSignature(VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32SignatureLength;
    VLT_SW Sw = VLT_STATUS_NONE;

    /* Ensure that the signature length is specified as we use it here. The
     * remainder of the parameter checking happens inside VltCase4. */
    if (NULL == pu16SignatureLength)
    {
        return( EGSNULLPARA );
    }

    u32SignatureLength = *pu16SignatureLength;

    status = VltCase4( VLT_INS_GENERATE_VERIFY_SIGNATURE,
        0,
        u32MessageLength,
        pu8Message,
        &u32SignatureLength,
        pu8Signature,
        &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    *pu16SignatureLength = (VLT_U16) u32SignatureLength;

    return( status );
}


VLT_STS VltUpdateSignature(VLT_U32 u32MessagePartLength,
						   const VLT_U8 *pu8MessagePart)
{
	VLT_SW Sw = VLT_STATUS_NONE;
	VLT_STS status = VLT_FAIL;

    VLT_U16 u16Idx; 
    VLT_U16 u16MaxChunk;
    VLT_U32 u32Remaining;
  
	if (0 == u32MessagePartLength)
	{
		return( EGSNULLPARA );
	}

	u16MaxChunk  = VltCommsGetMaxSendSize();
	u32Remaining = u32MessagePartLength;

	do
    {
        VLT_U16 u16Chunk;

        /* Build APDU. We have to do this on every iteration as the output of
         * the previous iteration will have overwritten it (assuming a shared
         * buffer). */
        u16Idx = VLT_APDU_DATA_OFFSET ;

        u16Chunk = (u32Remaining > u16MaxChunk) ? u16MaxChunk : (VLT_U16) u32Remaining;
		
        Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_CHAINING;
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_GENERATE_VERIFY_SIGNATURE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = LIN(WRAPPED_BYTE(u16Chunk));

        if( 0 != u16Chunk )
        {
            /* Build Data In */

			host_memcpy( &Command.pu8Data[u16Idx], pu8MessagePart, u16Chunk );
            u16Idx += u16Chunk;
            pu8MessagePart += u16Chunk;
        }

        /* Send the command */

        status = VltCommand( &Command, &Response, u16Idx, 0, &Sw );

        if (VLT_OK != status)
        {
            return status;
        }

        u32Remaining -= u16Chunk;
    }
    while (u32Remaining && Sw == VLT_STATUS_NEXT_MESSAGE_PART_EXPECTED);

	if( ( Sw != VLT_STATUS_NONE ) && 
		( Sw != VLT_STATUS_SUCCESS ) && 
		( Sw != VLT_STATUS_COMPLETED  ) )
	{
		return( Sw );
	}

	return( status );
}

VLT_STS VltComputeSignatureFinal( VLT_PU16 pu16SignatureLength,
    VLT_PU8 pu8Signature )
{
	VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
	VLT_U32 u32SignatureLength;
    VLT_U16 u16Idx; 
	
    VLT_PU8 pu8Out;
    VLT_PU8 pu8OutEnd;

	if (NULL == pu16SignatureLength)
    {
        return( EGSNULLPARA );
    }

	u32SignatureLength = *pu16SignatureLength;

    /* We need to split the data up into chunks, the size of which the comms
     * layer tells us. */
    pu8Out = pu8Signature;
    pu8OutEnd = pu8Signature + u32SignatureLength;

    do
    {
        /* Build APDU. We have to do this on every iteration as the output of
         * the previous iteration will have overwritten it (assuming a shared
         * buffer). */
        u16Idx = VLT_APDU_DATA_OFFSET ;

        Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL; 
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_GENERATE_VERIFY_SIGNATURE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 0;

        /* Send the command */

        status = VltCommand( &Command, &Response, u16Idx, 0, &Sw );

        if (VLT_OK != status)
        {
            return status;
        }

        /* How big is the response? */
        Response.u16Len -= VLT_SW_SIZE;

        /* Copy */
        if( ( pu8Out + Response.u16Len ) > pu8OutEnd )
        {
            /* ran out of output buffer space */
			*pu16SignatureLength = Response.u16Len;
            return( EC4NOROOM ); 
        }

        host_memcpy( pu8Out, Response.pu8Data, Response.u16Len );
        pu8Out += Response.u16Len;

        /* Check response code */
        switch( Sw )
        {
            case VLT_STATUS_COMPLETED:
            case VLT_STATUS_RESPONDING:
            case VLT_STATUS_SUCCESS:
                break;
            case VLT_STATUS_NONE: 
                return( status );
            default:
                return VLT_OK; /* unexpected status word */
        }
    }
    while (Sw == VLT_STATUS_RESPONDING);

    /* Report the final amount of data produced */
    u32SignatureLength = (VLT_U32)(pu8Out - pu8Signature);

	if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    *pu16SignatureLength = (VLT_U16) u32SignatureLength;

    return( status );
}

VLT_STS VltVerifySignature( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_U16 u16SignatureLength,
    const VLT_U8 *pu8Signature )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    VLT_U8 state = VLT_STATE_SEND_MESSAGE;
    VLT_U16 u16Chunk = 0;
    VLT_U32 u32Remaining;
    const VLT_U8* pu8In ;

    /* validate critical parameters */
    if( ( NULL == pu8Message ) ||
        ( NULL == pu8Signature ) )
    {
        return( EVSNULLPARA );
    }

    if( 0 == u32MessageLength )
    {
        return( EVSINVLDMSGLEN );
    }

    if( 0 == u16SignatureLength )
    {
        return( EVSINVLDSIGLEN );
    }

    /* 
     * Cycle through the send states:
     *  o - VLT_STATE_SEND_MESSAGE  
     *  o - VLT_STATE_SEND_SIGNATURE 
     */
    while( state < VLT_STATE_SEND_COMPLETE )
    {
        /*
         * set up state configuration data
         */
        switch( state )
        {
            case VLT_STATE_SEND_MESSAGE:
                u32Remaining = u32MessageLength;
                pu8In = pu8Message;
                break;
            case VLT_STATE_SEND_SIGNATURE:
                u32Remaining = u16SignatureLength;
                pu8In = pu8Signature;
                break;
            default:
                return( EVSINVLDSTATECFG );
        }

        /*
         * transfer data 
         */
        while( u32Remaining )
        {
            /* set the index offset */
            idx = VLT_APDU_DATA_OFFSET;

            /* determine the size of the transfer */
            if( VltCommsGetMaxSendSize() < u32Remaining )
            {
                u16Chunk = VltCommsGetMaxSendSize();
                    Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = 
                    VLT_CLA_CHAINING;
            }
            else
            {
                u16Chunk = (VLT_U16)u32Remaining;
                Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = 
                    VLT_CLA_NO_CHANNEL;
            }

            /* build apdu */
            Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GENERATE_VERIFY_SIGNATURE;
            Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
            Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
            Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN( WRAPPED_BYTE( u16Chunk ) );
            /* copy the data */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[idx], pu8In, u16Chunk );
            idx += u16Chunk;
            pu8In += u16Chunk;
            /* send the command */
            status = VltCommand( &Command, &Response, idx, 0, &Sw );

            /*
             * validate the status word based on the current state
             */
            if( VLT_STATE_SEND_MESSAGE == state )
            {
               if( ( Sw != VLT_STATUS_NEXT_MESSAGE_PART_EXPECTED ) &&
                   ( Sw != VLT_STATUS_NEXT_SIGNATURE_PART_EXPECTED )&& 
                   ( Sw != VLT_STATUS_NONE ) )
               {
                   return( Sw );
               }
            }
            else if( VLT_STATE_SEND_SIGNATURE == state )
            {    
               if( ( Sw != VLT_STATUS_NEXT_SIGNATURE_PART_EXPECTED ) &&
                   ( Sw != VLT_STATUS_SUCCESS )&& 
                   ( Sw != VLT_STATUS_NONE ) )
               {
                   return( Sw );
               }
            }
            else
            {
                return( EVSINVLDSTATESND );
            }

            /* validate the transfer status */
            if( VLT_OK != status ) 
            {
                return( status );
            }
            
            /* update the transfer progress */
            u32Remaining -= u16Chunk;            
        }

        /* transition to the next state */
        ++state;
    }

    return( status );
}

VLT_STS VltUpdateVerify(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message)
{
	VLT_SW Sw = VLT_STATUS_NONE;
	VLT_STS status = VLT_FAIL;
	
    VLT_U16 u16Idx; 
    VLT_U16 u16MaxChunk;
    VLT_U32 u32Remaining;
  
	if (0 == u32MessageLength)
	{
		return( EGSNULLPARA );
	}

	u16MaxChunk  = VltCommsGetMaxSendSize();
	u32Remaining = u32MessageLength;
	
	do
    {
		VLT_U16 u16Chunk;
 		 if( u16MaxChunk < u32Remaining )
            {
                u16Chunk = u16MaxChunk;
                    Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = 
                    VLT_CLA_CHAINING;
            }
            else
            {
                u16Chunk = (VLT_U16)u32Remaining;
                Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = 
                    VLT_CLA_NO_CHANNEL;
            }
        /* Build APDU. We have to do this on every iteration as the output of
         * the previous iteration will have overwritten it (assuming a shared
         * buffer). */
        u16Idx = VLT_APDU_DATA_OFFSET ;
		
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_GENERATE_VERIFY_SIGNATURE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = LIN(WRAPPED_BYTE(u16Chunk));

        if( 0 != u16Chunk )
        {
            /* Build Data In */

            host_memcpy( &Command.pu8Data[u16Idx], pu8Message, u16Chunk );
            u16Idx += u16Chunk;
            pu8Message += u16Chunk;
        }

        /* Send the command */

        status = VltCommand( &Command, &Response, u16Idx, 0, &Sw );

        if (VLT_OK != status)
        {
            return status;
        }

        u32Remaining -= u16Chunk;
    }
    while (u32Remaining && (Sw == VLT_STATUS_NEXT_MESSAGE_PART_EXPECTED || Sw == VLT_STATUS_NEXT_SIGNATURE_PART_EXPECTED));

	if ( ( Sw != VLT_STATUS_SUCCESS )
		&& ( Sw != VLT_STATUS_NONE ) )
	{
		return( Sw );
	}

	return( status );
}

VLT_STS VltComputeVerifyFinal(VLT_U32 u32SignatureLength,
						   const VLT_U8 *pu8Signature)
{
    VLT_SW Sw = VLT_STATUS_NONE;
	VLT_STS status = VLT_FAIL;
	
    VLT_U16 u16Idx; 
    VLT_U16 u16MaxChunk;
    VLT_U32 u32Remaining;
  
	if (0 == u32SignatureLength)
	{
		return( EGSNULLPARA );
	}

	u16MaxChunk  = VltCommsGetMaxSendSize();
	u32Remaining = u32SignatureLength;
	
	do
	{
		VLT_U16 u16Chunk;
		if( u16MaxChunk < u32Remaining )
		{
			u16Chunk = u16MaxChunk;
			Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_CHAINING;
		}
		else
		{
			u16Chunk = (VLT_U16)u32Remaining;
			Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL;
		}
        /* Build APDU. We have to do this on every iteration as the output of
         * the previous iteration will have overwritten it (assuming a shared
         * buffer). */
        u16Idx = VLT_APDU_DATA_OFFSET ;
		
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_GENERATE_VERIFY_SIGNATURE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = LIN(WRAPPED_BYTE(u16Chunk));

        if( 0 != u16Chunk )
        {
            /* Build Data In */
			host_memcpy( &Command.pu8Data[u16Idx], pu8Signature, u16Chunk );
            u16Idx += u16Chunk;
            pu8Signature += u16Chunk;
        }

        /* Send the command */
        status = VltCommand( &Command, &Response, u16Idx, 0, &Sw );

        if (VLT_OK != status)
        {
            return status;
        }

        u32Remaining -= u16Chunk;
    }
    while (u32Remaining && Sw == VLT_STATUS_NEXT_SIGNATURE_PART_EXPECTED);

	if ( ( Sw != VLT_STATUS_SUCCESS )
		&& ( Sw != VLT_STATUS_NONE ) )
	{
		return( Sw );
	}

	return( status );
}

VLT_STS VltComputeMessageDigest( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message,
    VLT_PU8 pu8DigestLength,
    VLT_PU8 pu8Digest )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32MsgIdx = 0;
    VLT_U16 u16Chunk = 0;

    if( 
        ( NULL == pu8Message ) ||
        ( NULL == pu8DigestLength ) ||       
        ( NULL == pu8Digest ) )
    {
        return( EMDNULLPARA );
    }   

    if( 0 == u32MessageLength )
    {
        return( EMDINVLDMSGLEN );
    }

    if( 0 == *pu8DigestLength )
    {
        return( EMDINVLDSGSTLEN );
    }

    while(  u32MsgIdx < u32MessageLength )
    {

        idx = VLT_APDU_DATA_OFFSET;

        /* determine the size of the transfer */
        if( VltCommsGetMaxSendSize() < ( u32MessageLength - u32MsgIdx ) )
        {            
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = 
            VLT_CLA_CHAINING;

            u16Chunk = VltCommsGetMaxSendSize();
        }
        else
        {         
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = 
                VLT_CLA_NO_CHANNEL;

            u16Chunk = (VLT_U16)( u32MessageLength - u32MsgIdx );
        }

        /* build apdu */
        Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_COMPUTE_MESSAGE_DIGEST;
        Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
        Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
        Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(u16Chunk));

        /* Build Data In */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &Command.pu8Data[idx], &pu8Message[u32MsgIdx], u16Chunk );
        idx += u16Chunk;
        u32MsgIdx += u16Chunk;

        /* Send the command */
        status = VltCommand( &Command, &Response, idx, 0, &Sw );

        /* adjust the length */
        Response.u16Len -= VLT_SW_SIZE;

        if( ( Sw != VLT_STATUS_NONE ) && 
            ( Sw != VLT_STATUS_SUCCESS ) && 
            ( Sw != VLT_STATUS_COMPLETED  ) )
        {
            return( Sw );
        }

        if (VLT_OK != status)
        {
            return status;
        }
    }
    /* ensure we have come out of the sending loop at the right time */
    if( ( 0 == Response.u16Len ) || 
        ( u32MsgIdx != u32MessageLength ) )
    {
        return( EMDIVLDSTATE );
    }
    /* ensure we have enough space to copy out the digest */
    if( *pu8DigestLength < Response.u16Len )
    {
        return( EMDNOROOM );
    }

    /* copy the digest */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8Digest, Response.pu8Data, Response.u16Len );

    /* copy the digest size */
    *pu8DigestLength = (VLT_U8)Response.u16Len;

    return( status );
}

VLT_STS VltUpdateMessageDigest( VLT_U32 u32MessageLength,
    const VLT_U8 *pu8Message)
{
	VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32MsgIdx = 0;
    VLT_U16 u16Chunk = 0;

	if (u32MessageLength ==0 && pu8Message == NULL)
	{
		Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_CHAINING;
		Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_COMPUTE_MESSAGE_DIGEST;
		Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
		Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
		Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

		status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

		 /* adjust the length */
        Response.u16Len -= VLT_SW_SIZE;

        if( ( Sw != VLT_STATUS_NONE ) && 
            ( Sw != VLT_STATUS_SUCCESS ) && 
            ( Sw != VLT_STATUS_COMPLETED  ) )
        {
            return( Sw );
        }

        if (VLT_OK != status)
        {
            return status;
        }
	}

	while (u32MsgIdx < u32MessageLength)
    {
		idx = VLT_APDU_DATA_OFFSET;

		Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_CHAINING;
		Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_COMPUTE_MESSAGE_DIGEST;
		Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
		Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;

        /* determine the size of the transfer */
		u16Chunk = ( VltCommsGetMaxSendSize() < ( u32MessageLength - u32MsgIdx ) ) ? VltCommsGetMaxSendSize() : (VLT_U16)( u32MessageLength - u32MsgIdx );

        /* build apdu */
        Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(u16Chunk));

        /* Build Data In */
        host_memcpy( &Command.pu8Data[idx], &pu8Message[u32MsgIdx], u16Chunk );
        idx += u16Chunk;
        u32MsgIdx += u16Chunk;

		status = VltCommand( &Command, &Response, idx, 0, &Sw );

		 /* adjust the length */
        Response.u16Len -= VLT_SW_SIZE;

        if( ( Sw != VLT_STATUS_NONE ) && 
            ( Sw != VLT_STATUS_SUCCESS ) && 
            ( Sw != VLT_STATUS_COMPLETED  ) )
        {
            return( Sw );
        }

        if (VLT_OK != status)
        {
            return status;
        }
	}

	return status;
}

VLT_STS VltComputeMessageDigestFinal( VLT_PU8 pu8DigestLength,
    VLT_PU8 pu8Digest )
{
	VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

	if (NULL == pu8Digest) 
    {
        return( EMDNULLPARA );
    }   

	if (0 == pu8DigestLength)
    {
        return( EMDINVLDMSGLEN );
    }

	/* build apdu */
	Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_COMPUTE_MESSAGE_DIGEST;
	Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
	Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
	Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

	idx = VLT_APDU_DATA_OFFSET;

	/* Send the command */
	status = VltCommand( &Command, &Response, idx, 0, &Sw );

	/* adjust the length */
	Response.u16Len -= VLT_SW_SIZE;

	if( ( Sw != VLT_STATUS_NONE ) && 
		( Sw != VLT_STATUS_SUCCESS ) && 
		( Sw != VLT_STATUS_COMPLETED  ) )
	{
		return( Sw );
	}

	if (VLT_OK != status)
	{
		return status;
	}

	/* copy the digest */
    host_memcpy( pu8Digest, Response.pu8Data, Response.u16Len );

    /* copy the digest size */
    *pu8DigestLength = (VLT_U8)Response.u16Len;

    return( status );
}

VLT_STS VltGenerateRandom(VLT_U8 u8NumberOfCharacters, 
    VLT_PU8 pu8RandomCharacters )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    /* validate critical parameters */
    if( NULL == pu8RandomCharacters )
    {
        return( EGRNULLPARAM );
    }

    if( u8NumberOfCharacters > VltCommsGetMaxReceiveSize() )
    {
        return( EGRNOROOM );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GENERATE_RANDOM;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = u8NumberOfCharacters;
    /* send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );
    /* Adjust the size */
    Response.u16Len -= VLT_SW_SIZE;

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }
    
    if ( VLT_OK != status )
    {
        return status;
    }

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8RandomCharacters, Response.pu8Data, Response.u16Len );

    return( status );
}


VLT_STS VltGenerateKeyPair(VLT_U8 u8PublicKeyGroup,
    VLT_U8 u8PublicKeyIndex,
    VLT_U8 u8PrivateKeyGroup,
    VLT_U8 u8PrivateKeyIndex,
    const VLT_KEY_GEN_DATA *pKeyGenData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;
  

    if( NULL == pKeyGenData ) 
    {
        return( EGKPNULLPARA );
    }    

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GENERATE_KEY_PAIR;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;


    /* Build Data In */

    /* bAlgoID */
    Command.pu8Data[idx++] = pKeyGenData->u8AlgoID;
    /* iPubGroup */
    Command.pu8Data[idx++] = u8PublicKeyGroup;
    /* iPub */
    Command.pu8Data[idx++] = u8PublicKeyIndex;
    /* bmPubAccess fixed to 3 on vaultic 100. 
     * only 2 users - 0 & 1. */
    Command.pu8Data[idx++] = 3; 
    Command.pu8Data[idx++] = 3;
    Command.pu8Data[idx++] = 0;
    Command.pu8Data[idx++] = 3;
    /* iPrivGroup */
    Command.pu8Data[idx++] = u8PrivateKeyGroup;
    /* iPriv */
    Command.pu8Data[idx++] = u8PrivateKeyIndex;
    /* bmPubAccess fixed to 3 on vaultic 100. 
     * only 2 users - 0 & 1. */
    Command.pu8Data[idx++] = 3;
    Command.pu8Data[idx++] = 3;
    Command.pu8Data[idx++] = 0;
    Command.pu8Data[idx++] = 3;

    /* abParams */
    switch( pKeyGenData->u8AlgoID )
    {
        case VLT_ALG_KPG_ECDSA_GF2M:
            {
                const VLT_KEY_GEN_ECDSA_DATA *d = &pKeyGenData->EcdsaKeyGenObj;

                Command.pu8Data[idx++] = d->u8DomainParamsGroup;
                Command.pu8Data[idx++] = d->u8DomainParamsIndex;
            }
            break;

        default:
            return EGKPBADKPG;
    }

    /* Update P3 now that we know the correct length. */

    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */

    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}



/* --------------------------------------------------------------------------
 * FILE SYSTEM SERVICES
 * -------------------------------------------------------------------------- */


VLT_STS VltSelectFile(const VLT_U8 *pu8Path, 
    VLT_U8 u8PathLength, 
    VLT_SELECT *pRespData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;


    if( ( NULL == pu8Path ) ||        
        ( NULL == pRespData ) )
    {
        return( ESFNULLPARA );
    }
    
    if( ( 0 == u8PathLength ) ||
        ( VltCommsGetMaxSendSize() < u8PathLength ) )
    {
        return( ESFDINVLDPATHLEN );
    }
    
    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SELECT;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Build Data In */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8Path, u8PathLength );
    idx += u8PathLength;

    /* Check that a NULL terminator is there, if not add one */
    if( ( '\0' != Command.pu8Data[idx - 1] ) && 
        ( VltCommsGetMaxSendSize() != u8PathLength ) )
    {
        Command.pu8Data[idx++] = '\0';
    }

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 
        LIN(WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, VLT_SF_RESPONSE_LENGTH, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return status;
    }

    /* Unpack the response */
    idx = 0;    

    pRespData->u32FileSize = VltEndianReadPU32( &Response.pu8Data[idx] );
    idx += 4;
    pRespData->FileAccess.u8Read    = Response.pu8Data[idx++];
    pRespData->FileAccess.u8Write   = Response.pu8Data[idx++];
    pRespData->FileAccess.u8Delete  = Response.pu8Data[idx++];
    pRespData->FileAccess.u8Execute = Response.pu8Data[idx++];
    pRespData->u8FileAttribute      = Response.pu8Data[idx++];

    return( status );
}


VLT_STS VltWriteFile( const VLT_U8 *pu8DataIn,
    VLT_U8 u8DataLength )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;

    if( NULL == pu8DataIn )
    {
        return( EWFNULLPARA );
    }    

    if( 0 == u8DataLength )
    {
        return( EWFIVLDLEN );
    }

    /* Reject the request if it's larger than the maximum chunk size. */
    if( u8DataLength > VltCommsGetMaxSendSize() )
    {
        return( EWFTOOBIG );
    }       

    /* Build APDU */    
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_WRITE_FILE; 
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(u8DataLength);

    /* Build Data In */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8DataIn, u8DataLength );
    idx += u8DataLength;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}


VLT_STS VltReadFile( VLT_PU16 pu16ReadLength,
    VLT_PU8 pu8RespData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16MemCopySize = 0;

    idx = 0;

    if( ( NULL == pu16ReadLength ) ||
        ( NULL == pu8RespData ) )
    {
        return( ERFNULLPARAM );
    }

    /*
    * SDVAULTICWRAP-44: Check for 0 bytes 
    */
    if( 0 == *pu16ReadLength )
    {
        return( ERFZEROBYTES );
    }

    if( ( 0 == *pu16ReadLength ) ||
        ( *pu16ReadLength > VLT_MAX_APDU_RCV_DATA_SZ ) )
    {
        return( ERFINVLDLEN );
    }

    do
    {
        /* Set up the apdu */
        Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL;
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_READ_FILE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;        
        
        if ( VLT_STATUS_RESPONDING == Sw )
        {
            Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 0;
        }
        else
        {
            /*
             * When the value is 256, the cast to a VLT_U8 will 
             * result in a P3 of 0. P3 = 0 will result in 256 bytes 
             * being received.
             */
            Command.pu8Data[ VLT_APDU_P3_OFFSET ] = (VLT_U8)*pu16ReadLength;
        }


        /* Send in the command*/
        status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );
        /* Adjust the length received */
        Response.u16Len -= VLT_SW_SIZE;

        /* Check status and status word */
        if( VLT_OK != status )
        {
            return( status );
        }
        

        /*
         * Check there is enough room in the read buffer.
         */
        if ( ( idx + Response.u16Len ) <= *pu16ReadLength )
        {
            /* The received size is less or equal the number requested. */
            u16MemCopySize = Response.u16Len;            
        }
        else if ( ( idx < *pu16ReadLength ) &&
                  ( Response.u16Len > ( *pu16ReadLength - idx ) ) )
        {
            /* The received size is greater than the number requested
             * only copy the bytes there is available space to store. */
            u16MemCopySize = *pu16ReadLength - idx;            
        }
        else
        {
            /* Run out of room, don't copy the rest. */
            u16MemCopySize = 0;
        }

        
        /*
         * Copy the data out if we have been given enough space.
         */
        if ( 0 != u16MemCopySize )
        {
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &pu8RespData[idx], Response.pu8Data, u16MemCopySize );
            idx += u16MemCopySize;
        }
        
        
        /* Check response code */
        switch( Sw )
        {
            case VLT_STATUS_COMPLETED:
            case VLT_STATUS_RESPONDING:
            case VLT_STATUS_SUCCESS:
                break;
            case VLT_STATUS_NONE: 
                return( status );
            case VLT_STATUS_EOF:
                status = VLT_EOF;
                break;
            default:
                return Sw; /* unexpected status word */
        }

    } while ( Sw == VLT_STATUS_RESPONDING );

    *pu16ReadLength = idx;
    
    return( status );     
}

VLT_STS VltSeekFile( VLT_U32 u32SeekLength )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;

    /* Build APDU */ 
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SEEK_FILE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(4);


    /* Build Data In */
    Command.pu8Data[idx++] = (VLT_U8) ((u32SeekLength >> 24) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u32SeekLength >> 16) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u32SeekLength >>  8) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u32SeekLength >>  0) & 0xFF);

    /* Send the command */

    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && 
        ( Sw != VLT_STATUS_SUCCESS ) && 
        ( Sw != VLT_STATUS_EOF ) )
    {
        return( Sw );
    }

    if( VLT_STATUS_EOF == Sw )
    {
        return( VLT_EOF );
    }

    return( status );
}


/* --------------------------------------------------------------------------
 * MANUFACTURING COMMANDS
 * -------------------------------------------------------------------------- */

VLT_STS VltGetInfo(VLT_TARGET_INFO *pRespData )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    if( NULL == pRespData )
    {
        return( EGINULLPARA );
    }

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GET_INFO;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LEXP(VLT_TARGET_INFO_LENGTH);

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return ( status );
    }
   
    /* 
     * Remove the status word from the length of the 
     * data received.
     */
    Response.u16Len -= VLT_SW_SIZE;  

    /*
     * The length of the data returned must much either 
     * the get info data with the Date or without, any other
     * length values can be considered invalid.
     */
    if ( VLT_TARGET_INFO_LENGTH != Response.u16Len ) 
        
    {
        return( EGTINFOIVLDRESPLEN );
    }

    /* Unpack the response */
    idx = 0;    

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pRespData->au8Firmware, &Response.pu8Data[idx], VLT_FIRMWARE_VERSION_LENGTH );
    idx += VLT_FIRMWARE_VERSION_LENGTH;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy(pRespData->au8Serial, &Response.pu8Data[idx], VLT_CHIP_SERIAL_NUMBER_LENGTH);
    idx += VLT_CHIP_SERIAL_NUMBER_LENGTH;

    pRespData->u8State = Response.pu8Data[idx++];
    pRespData->u8SelfTests = Response.pu8Data[idx++];
    pRespData->u8Role = Response.pu8Data[idx++];
    pRespData->u8Mode = Response.pu8Data[idx++];
    pRespData->u32Space = VltEndianReadPU32(&Response.pu8Data[idx]);
    idx += 6;    
    pRespData->u8Attack = Response.pu8Data[idx++];
    pRespData->u16AttackCounter = VltEndianReadPU16(&Response.pu8Data[idx]);
    idx += 2;
    
    
    
    return( status );
}

VLT_STS VltSelfTest( void )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SELF_TEST;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltSetStatus( VLT_U8 u8State )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_STATUS;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8State;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );    
}

VLT_STS VltSetConfig( VLT_U8 u8ConfigItem, VLT_U8 u8DataLength, VLT_PU8 pu8ConfigData )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( NULL == pu8ConfigData )
    {
        return( ESETCNFGNULLDATA );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_CONFIG;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8ConfigItem;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = u8DataLength;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8ConfigData, u8DataLength );
    idx += u8DataLength;

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status ); 
}

#if (VLT_ENABLE_SECURE_COUNTERS == VLT_ENABLE)	
#if (VAULT_IC_TARGET == VAULTIC100)
VLT_STS VltSetSecureCounters(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup,VLT_U8 u8CounterValueLen, VLT_PU8 pu8CounterValue)
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( NULL == pu8CounterValue )
    {
		return( ESETCNTNULLDATA );
    }

	/* 
     * Ensure pu8CounterValue is 4-bytes length
     */
	if( VLT_COUNTER_LENGTH != u8CounterValueLen )
    {
		return( ESETCNTINVLDLEN );
    }

	/* 
     * Ensure u8CounterNumber and u8CounterGroup are valid
     */
	if (((u8CounterNumber < 0x01) && (u8CounterNumber > 0x08))
		|| ((u8CounterGroup < 0x01) && (u8CounterGroup > 0x02)))
    {
		return( ESETCNTINVLDVAL );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_COUNTER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8CounterNumber;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8CounterGroup;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = u8CounterValueLen;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8CounterValue, u8CounterValueLen );
    idx += u8CounterValueLen;

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status ); 
}

VLT_STS VltIncrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
	VLT_COUNTER_DATA* pCounterData,
	VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature,
	VLT_COUNTER_RESPONSE* pCounterResponse )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( (NULL == pu8Signature) 
		|| (NULL == pCounterData)
		|| (NULL == pCounterResponse))
    {
        return( EICNULLPARAM );
    }
 
	if ((pCounterData->nounce.u8NounceLength != VLT_NOUNCE_LENGTH) 
		|| (pCounterData->amount.u8AmountLength != VLT_COUNTER_LENGTH))
	{
		 return( EICINVLDLEN );
	}

	/* 
     * Ensure u8CounterNumber and u8CounterGroup are valid
     */
	if (((u8CounterNumber < 0x01) && (u8CounterNumber > 0x08))
		|| ((u8CounterGroup < 0x01) && (u8CounterGroup > 0x02)))
    {
		return( EICINVLDVAL );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_INCREMENT_COUNTER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8CounterNumber;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8CounterGroup;

    /*
    * Copy nounce value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->nounce.u8Nounce , pCounterData->nounce.u8NounceLength );
    idx += pCounterData->nounce.u8NounceLength;

	/*
    * Copy amount increment value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->amount.u8Amount, pCounterData->amount.u8AmountLength );
    idx += pCounterData->amount.u8AmountLength;

	/*
    * Copy signature value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pu8Signature,u16Signaturelen );
    idx += u16Signaturelen;

	/* Update P3 now that we know the correct length. */
    Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
     if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return ( status );
    } 

	 /* 
     * Remove the status word from the length of the 
     * data received.
     */
    Response.u16Len -= VLT_SW_SIZE;  

	/*
     * The length of the data returned must much either 
     * the get info data with the Date or without, any other
     * length values can be considered invalid.
     */
    if ( (VLT_COUNTER_LENGTH + VLT_RND_LENGTH + u16Signaturelen)!= Response.u16Len ) 
    {
        return( EICIVLDRESPLEN );
    }

    /* Unpack the response */
    idx = 0;    

    /*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy( pCounterResponse->u8CntValue, &Response.pu8Data[idx], VLT_COUNTER_LENGTH );
    idx += VLT_COUNTER_LENGTH;

    /*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy(pCounterResponse->u8Rnd, &Response.pu8Data[idx], VLT_RND_LENGTH);
    idx += VLT_RND_LENGTH;

	pCounterResponse->u16SignatureLength = u16Signaturelen;

	/*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy(pCounterResponse->pu8Signature, &Response.pu8Data[idx], u16Signaturelen);
    idx += u16Signaturelen;
    
    return( status );
}

VLT_STS VltDecrementCounter(VLT_U8 u8CounterNumber, VLT_U8 u8CounterGroup, 
	VLT_COUNTER_DATA* pCounterData,
	VLT_COUNTER_RESPONSE* pCounterResponse )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( (NULL == pCounterData)
		|| (NULL == pCounterResponse))
    {
        return( EDCNULLPARAM );
    }
 
	if ((pCounterData->nounce.u8NounceLength != VLT_NOUNCE_LENGTH) 
		|| (pCounterData->amount.u8AmountLength != VLT_COUNTER_LENGTH))
	{
		 return( EDCINVLDLEN );
	}

	/* 
     * Ensure u8CounterNumber and u8CounterGroup are valid
     */
	if (((u8CounterNumber < 0x01) && (u8CounterNumber > 0x08))
		|| ((u8CounterGroup < 0x01) && (u8CounterGroup > 0x02)))
    {
		return( EDCINVLDVAL );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DECREMENT_COUNTER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8CounterNumber;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8CounterGroup;

    /*
    * Copy nounce value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->nounce.u8Nounce , pCounterData->nounce.u8NounceLength );
    idx += pCounterData->nounce.u8NounceLength;

	/*
    * Copy amount increment value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->amount.u8Amount, pCounterData->amount.u8AmountLength );
    idx += pCounterData->amount.u8AmountLength;

	/* Update P3 now that we know the correct length. */
    Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
     if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return ( status );
    } 

	 /* 
     * Remove the status word from the length of the 
     * data received.
     */
    Response.u16Len -= VLT_SW_SIZE;  

    /* Unpack the response */
    idx = 0;    

    /*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy( pCounterResponse->u8CntValue, &Response.pu8Data[idx], VLT_COUNTER_LENGTH );
    idx += VLT_COUNTER_LENGTH;

    /*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy(pCounterResponse->u8Rnd, &Response.pu8Data[idx], VLT_RND_LENGTH);
    idx += VLT_RND_LENGTH;
    
	pCounterResponse->u16SignatureLength = Response.u16Len - VLT_COUNTER_LENGTH - VLT_RND_LENGTH;

	/*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy(pCounterResponse->pu8Signature, &Response.pu8Data[idx], pCounterResponse->u16SignatureLength);

    return( status );
}
#else
VLT_STS VltIncrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,
		VLT_U16 u16Signaturelen, VLT_PU8 pu8Signature,VLT_COUNTER_RESPONSE* pCounterResponse )
{
	VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
	VLT_U8 i;
	VLT_U8 nbCounterToSet = 0;

    idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( (NULL == pu8Signature) 
		|| (NULL == pCounterData)
		|| (NULL == pCounterResponse))
    {
        return( EICNULLPARAM );
    }
 
	if ((pCounterData->nounce.u8NounceLength != VLT_NOUNCE_LENGTH) 
		|| (pCounterData->amount.u8AmountLength != VLT_COUNTER_LENGTH))
	{
		 return( EICINVLDLEN );
	}

	/* 
     * Ensure u8CounterNumber and u8CounterGroup are valid
     */
	if ((u8CounterGroup < 0x01) && (u8CounterGroup > 0x02))
    {
		return( EICINVLDVAL );
    }

	/* Get number of counter to set */
	for (i=CHAR_BIT-1;i>=0;i--) {
		if (u8CounterMask&(1<<i))
			nbCounterToSet++;

	}
    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_INCREMENT_COUNTER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8CounterMask;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8CounterGroup;

    /*
    * Copy nounce value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->nounce.u8Nounce , pCounterData->nounce.u8NounceLength );
    idx += pCounterData->nounce.u8NounceLength;

	/*
    * Copy amount increment value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->amount.u8Amount, pCounterData->amount.u8AmountLength );
    idx += pCounterData->amount.u8AmountLength;

	/*
    * Copy signature value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pu8Signature,u16Signaturelen );
    idx += u16Signaturelen;

	/* Update P3 now that we know the correct length. */
    Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return ( status );
    } 
	 /* 
     * Remove the status word from the length of the 
     * data received.
     */
    Response.u16Len -= VLT_SW_SIZE;  

    /* Unpack the response */
    idx = 0;    
	(void)host_memset(pCounterResponse->u8CntValue,0x00,VLT_FULL_COUNTERS_LENGTH);

	for(i=0;i<nbCounterToSet;i++)
	{
		/*
		* No need to check the return type as pointer has been validated
		*/
		(void)host_memcpy( pCounterResponse->u8CntValue, &Response.pu8Data[idx], VLT_COUNTER_LENGTH );
		idx += VLT_COUNTER_LENGTH;
	}
	pCounterResponse->u8NbCount = nbCounterToSet;
	/*
	* No need to check the return type as pointer has been validated
	*/
	(void)host_memcpy(pCounterResponse->u8Rnd, &Response.pu8Data[idx], VLT_RND_LENGTH);
	idx += VLT_RND_LENGTH;

	pCounterResponse->u16SignatureLength = Response.u16Len - VLT_RND_LENGTH - (nbCounterToSet*VLT_COUNTER_LENGTH);

	/*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy(pCounterResponse->pu8Signature, &Response.pu8Data[idx], pCounterResponse->u16SignatureLength);

	return( status );
}

VLT_STS VltDecrementCounter(VLT_U8 u8CounterMask, VLT_U8 u8CounterGroup, 
		VLT_COUNTER_DATA* pCounterData,VLT_COUNTER_RESPONSE* pCounterResponse)
{
	VLT_STS status = VLT_FAIL;
	VLT_SW Sw = VLT_STATUS_NONE;
	VLT_U8 i;
	VLT_U8 nbCounterToSet = 0;
	idx = VLT_APDU_DATA_OFFSET;
	
    /* 
     * Ensure we have a non null data pointer
     */
    if( (NULL == pCounterData)
		|| (NULL == pCounterResponse))
    {
        return( EDCNULLPARAM );
    }
 
	if ((pCounterData->nounce.u8NounceLength != VLT_NOUNCE_LENGTH) 
		|| (pCounterData->amount.u8AmountLength != VLT_COUNTER_LENGTH))
	{
		 return( EDCINVLDLEN );
	}

	/* 
     * Ensure u8CounterNumber and u8CounterGroup are valid
     */
	if ((u8CounterGroup < 0x01) && (u8CounterGroup > 0x02))
    {
		return( EDCINVLDVAL );
    }

	/* Get number of counter to set */
	for (i=CHAR_BIT-1;i>=0;i--) {
		if (u8CounterMask&(1<<i))
			nbCounterToSet++;
	}

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DECREMENT_COUNTER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8CounterMask;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8CounterGroup;

    /*
    * Copy nounce value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->nounce.u8Nounce , pCounterData->nounce.u8NounceLength );
    idx += pCounterData->nounce.u8NounceLength;

	/*
    * Copy amount increment value
    */
	(void)host_memcpy( &Command.pu8Data[idx], pCounterData->amount.u8Amount, pCounterData->amount.u8AmountLength );
    idx += pCounterData->amount.u8AmountLength;

	/* Update P3 now that we know the correct length. */
    Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
     if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return ( status );
    } 

	 /* 
     * Remove the status word from the length of the 
     * data received.
     */
    Response.u16Len -= VLT_SW_SIZE;  

    /* Unpack the response */
    idx = 0;    
	(void)host_memset(pCounterResponse->u8CntValue,0x00,VLT_FULL_COUNTERS_LENGTH);

	for(i=0;i<nbCounterToSet;i++)
	{
		/*
		* No need to check the return type as pointer has been validated
		*/
		(void)host_memcpy( pCounterResponse->u8CntValue, &Response.pu8Data[idx], VLT_COUNTER_LENGTH );
		idx += VLT_COUNTER_LENGTH;
	}
	pCounterResponse->u8NbCount = nbCounterToSet;
	/*
	* No need to check the return type as pointer has been validated
	*/
	(void)host_memcpy(pCounterResponse->u8Rnd, &Response.pu8Data[idx], VLT_RND_LENGTH);
	idx += VLT_RND_LENGTH;

	pCounterResponse->u16SignatureLength = Response.u16Len - VLT_RND_LENGTH - (nbCounterToSet*VLT_COUNTER_LENGTH);

	/*
    * No need to check the return type as pointer has been validated
    */
	(void)host_memcpy(pCounterResponse->pu8Signature, &Response.pu8Data[idx], pCounterResponse->u16SignatureLength);

	return( status );
}

VLT_STS VltSetSecureCounters(VLT_U8 u8CounterGroup,
							 VLT_U8 u8CounterValueLen, 
							 VLT_PU8 pu8CounterValue)
{
	 VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( NULL == pu8CounterValue )
    {
		return( ESETCNTNULLDATA );
    }

	/* 
     * Ensure pu8CounterValue is 4-bytes length
     */
	if( VLT_FULL_COUNTERS_LENGTH != u8CounterValueLen )
    {
		return( ESETCNTINVLDLEN );
    }

	/* 
     * Ensure u8CounterNumber and u8CounterGroup are valid
     */
	if ((u8CounterGroup < 0x01) && (u8CounterGroup > 0x02))
    {
		return( ESETCNTINVLDVAL );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_COUNTER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0x00;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8CounterGroup;
	Command.pu8Data[VLT_APDU_P3_OFFSET] = VLT_FULL_COUNTERS_LENGTH;

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8CounterValue, u8CounterValueLen );
    idx += u8CounterValueLen;

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status ); 
}
#endif
#endif

#if (VLT_ENABLE_CHIP_DEACTIVATION == VLT_ENABLE)	
VLT_STS VltDeactivate(VLT_PU8 pu8KeyId,VLT_U8 u8KeyIdLen,
    VLT_PU8 pu8Keydata,
    VLT_U8 u8KeyDataLen)
{
	VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
	idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( NULL == pu8KeyId || NULL == pu8Keydata)
    {
        return( EDEACTIVATENULLDATA );
    }
 
	/* 
     * Ensure we have good length values
     */
	if (u8KeyIdLen != KEYID_LEN || u8KeyDataLen != KEYDATA_LEN)
	{
		 return( EDEACTIVATEINVLENGTH );
	}

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DEACTIVATE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0x22;

	/*
    * Copy keyId
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8KeyId, u8KeyIdLen );
    idx += u8KeyIdLen;

	/*
    * Copy key data
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8Keydata, u8KeyDataLen );
    idx += u8KeyDataLen;

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status ); 
}

VLT_STS VltGetChallenge(VLT_PU8 pu8Challenge,VLT_PU8 pu8ChallengeLen)
{
	VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;

    /* 
     * Ensure we have a non null data pointer
     */
    if( NULL == pu8Challenge || pu8ChallengeLen == NULL)
    {
        return( EGETCHALLENGENULLDATA );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GETCHALLENGE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = CHALLENGE_SIZE;

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

	(void)host_memcpy( pu8Challenge, &Response.pu8Data[0], 
        CHALLENGE_SIZE );
	*pu8ChallengeLen = CHALLENGE_SIZE;

    return( status ); 
}

VLT_STS VltActivate(VLT_PU8 pu8Challenge,VLT_U8 u8ChallengeLen,
    VLT_PU8 pu8Keydata,
    VLT_U8 u8KeyDataLen)
{
	VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
	sha256_ctx ctx; // context holder
	VLT_U8 bHash[HASH_CHALLENGE_SIZE];
	VLT_U8 dataToHash[HASH_CHALLENGE_SIZE];
	//VLT_U8 fake[] = {0x48,0xCC,0x36,0x27,0x7F,0xE4,0xDE,0x66,0x92,0x42,0x3C,0x42,0x80,0x34,0xBE,0x42};
	int kactIdx;

	idx = VLT_APDU_DATA_OFFSET;

    /* 
     * Ensure we have a non null data pointer
     */
    if( NULL == pu8Challenge || NULL == pu8Keydata)
    {
        return( EDEACTIVATENULLDATA );
    }
 
	/* 
     * Ensure we have good length values
     */
	if (u8ChallengeLen != CHALLENGE_SIZE || u8KeyDataLen != KEYDATA_LEN)
	{
		 return( EDEACTIVATEINVLENGTH );
	}

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
	Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_ACTIVATE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0x20;

	//Calculate Kact XOR Sha256(Challenge)
	
	//(void)host_memcpy( dataToHash, fake, u8ChallengeLen );
	(void)host_memcpy( dataToHash, pu8Challenge, u8ChallengeLen );
	(void)host_memset(dataToHash+u8ChallengeLen,0x00,HASH_CHALLENGE_SIZE-u8ChallengeLen);

	sha256_begin(&ctx);
    sha256_hash(dataToHash, HASH_CHALLENGE_SIZE, &ctx);
    sha256_end(bHash, &ctx);

	for (kactIdx=0;kactIdx<HASH_CHALLENGE_SIZE;kactIdx++)
	{
		pu8Keydata[kactIdx] = pu8Keydata[kactIdx] ^ bHash[kactIdx];
	}

	/*
    * Copy key activation data
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8Keydata, u8KeyDataLen );
    idx += u8KeyDataLen;

    /* 
     * Send the command 
     */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    /*
     * Let the caller know that the command transport 
     * succeeded but the Vault IC responded with status word
     * other than 0x9000.
     */
    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status ); 
}
#endif

VLT_STS VltTestCase1( void )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_TEST_CASE_1;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltTestCase2(VLT_U8 u8RequestedDataLength, VLT_PU8 pu8RespData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* validate critical parameters */
    if( NULL == pu8RespData )
    {
        return( ETC2NULLPARAM );
    }

    /*
    * SDVAULTICWRAP-44: Check for 0 bytes 
    */
    if( 0 == u8RequestedDataLength )
    {
        return( ETC2ZEROBYTES );
    }

    if( u8RequestedDataLength > VltCommsGetMaxReceiveSize() )
    {
        return( ETC2NOROOM );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_TEST_CASE_2;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8RequestedDataLength;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LEXP(u8RequestedDataLength);

    /* send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );
    /* Adjust the size */
    Response.u16Len -= VLT_SW_SIZE;

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8RespData, Response.pu8Data, Response.u16Len );

    return( status );       
}

VLT_STS VltTestCase3( VLT_U8 u8DataLength, const VLT_U8 *pu8DataIn )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    if( NULL == pu8DataIn )
    {
        return( ETC3NULLPARA );
    }

    /* Reject the request if it's larger than the maximum chunk size. */
    if( ( VltCommsGetMaxSendSize() < u8DataLength ) ||
        ( 0 == u8DataLength ) )
    {
        return( ETC3INVLDLEN );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_TEST_CASE_3;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(u8DataLength);

    /* Build Data In */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8DataIn, u8DataLength );
    idx += u8DataLength;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltTestCase4( VLT_U8 u8DataLength,
    const VLT_U8 *pu8DataIn, 
    VLT_U8 u8RequestedDataLength,
    VLT_PU8 pu8RespData )
{
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;

    idx = VLT_APDU_DATA_OFFSET;

    /* validate critical parameters */
    if( ( NULL == pu8DataIn ) ||
        ( NULL == pu8RespData ) )
    {
        return( ETC4NULLPARA );
    }

    if( ( VltCommsGetMaxSendSize() < u8DataLength ) ||
        ( 0 == u8DataLength ) )
    {
        return( ETC4IVLDSENDLEN );
    }

    if( ( VltCommsGetMaxReceiveSize() < u8RequestedDataLength ) ||
        ( 0 == u8RequestedDataLength ) )
    {
        return( ETC4IVLDRECVLEN );
    }
    
    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_TEST_CASE_4;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] =  u8RequestedDataLength;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(WRAPPED_BYTE(u8DataLength));

    /* Build Data In */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8DataIn, u8DataLength );
    idx += u8DataLength;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if (VLT_OK != status)
    {
        return status;
    }

    /* adjust the response size */
    Response.u16Len -= VLT_SW_SIZE;

    /* 
     * ensure that the received data don't exceed
     * the requested size 
     */
    if( u8RequestedDataLength < Response.u16Len )
    {
        return( ETC4NOROOM ); 
    }

    /* copy the data */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8RespData, Response.pu8Data, Response.u16Len );


    return( status );
}

VLT_STS VltGetLibInfo( VLT_LIBRARY_INFO* pLibraryInfo )
{
    /*
    * Ensure we have been passed a non null pointer.
    */
    if( NULL == pLibraryInfo )
    {
        return( EGTLBINFIVLDPTR );
    }

    /*
    * Set the library version.
    */
    pLibraryInfo->pVersion = vltApiVersion;

    /*
    * Reset the capabilities.
    */
    pLibraryInfo->capabilities = (VLT_U32)0;


    /*
    * Set the capabilities.
    */
   #if( VLT_ENABLE_KEY_SECRET == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_SECRET;
    #endif

    #if( VLT_ENABLE_KEY_HOTP == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_HOTP;
    #endif

    #if( VLT_ENABLE_KEY_TOTP == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_TOTP;
    #endif

    #if( VLT_ENABLE_KEY_RSA == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_RSA;
    #endif

    #if( VLT_ENABLE_KEY_DSA == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_DSA;
    #endif

    #if( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_ECDSA;
    #endif

    #if( VLT_ENABLE_CIPHER_TESTS == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_CIPHER_TESTS;
    #endif

    #if( VLT_ENABLE_SCP02 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_SCP02;
    #endif

    #if( VLT_ENABLE_SCP03 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_SCP03;
    #endif

    #if( VLT_ENABLE_BLOCK_PROTOCOL == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_BLOCK_PROTOCOL;
    #endif

    #if( VLT_ENABLE_ISO7816 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_ISO7816;
    #endif

    #if( VLT_ENABLE_TWI == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_TWI;
    #endif

    #if( VLT_ENABLE_SPI == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_SPI;
    #endif

    #if( VLT_ENABLE_CIPHER_AES == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_CIPHER_AES;
    #endif

    #if( VLT_ENABLE_CIPHER_TDES == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_CIPHER_TDES;
    #endif

    #if( VLT_ENABLE_CIPHER_DES == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_CIPHER_DES;
    #endif

    #if( VLT_ENABLE_FAST_CRC16CCIT == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_FAST_CRC16CCIT;
    #endif

    #if( VLT_ENABLE_KEY_WRAPPING == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_WRAPPING;
    #endif

    #if( VLT_ENABLE_MS_AUTH == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_MS_AUTH;
    #endif

    #if( VLT_ENABLE_IDENTITY_AUTH == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_IDENTITY_AUTH;
    #endif

    #if( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_KEY_HOST_DEVICE_ID;
    #endif

    #if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_FILE_SYSTEM;
    #endif

    #if( VLT_ENABLE_AARDVK_SPPRSS_ERR == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_AARDVRKSUPP_ERR;
    #endif

    #if ( VLT_ENABLE_IDENTITY_AUTH_V2 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_IDENTITY_AUTH_V2;
    #endif

    #if ( VLT_ENABLE_STRONG_AUTH == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_STRONG_AUTH;
    #endif

    #if ( VLT_ENABLE_ECDSA_SIGNER == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_ECDSA_SIGNER;
    #endif

    #if ( VLT_ENABLE_ECDSA_K233 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_ECDSA_K233;
    #endif

    #if ( VLT_ENABLE_ECDSA_K283 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_ECDSA_K283;
    #endif

    #if ( VLT_ENABLE_ECDSA_B233 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_ECDSA_B233;
    #endif

    #if ( VLT_ENABLE_ECDSA_B283 == VLT_ENABLE )
        pLibraryInfo->capabilities |= VLT_CPB_ENABLE_ECDSA_B283;
    #endif



    /*
    * Reset the Reserved
    */
    pLibraryInfo->reserved = (VLT_U32)0;

    /*
    * This api is compatible with the specified VaultIC Version.
    */
    pLibraryInfo->fwCompatibilityVersionId = VAULT_IC_VERSION;

    return( VLT_OK );
}

#if(VLT_ENABLE_ISO7816 == VLT_ENABLE )
VLT_STS VltCardEvent(VLT_PU8 pu8ReaderName, DWORD dwTimeout,PDWORD pdwEventState)
{
    return( VltCommsCardEvent(pu8ReaderName,dwTimeout,pdwEventState) );
}

VLT_STS VltSelectCard(SCARDHANDLE hScard, SCARDCONTEXT hCxt, DWORD dwProtocol)
{
	return( VltCommsSelectCard(hScard,hCxt,dwProtocol) );
}
#endif