/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_comms.h"
#include "vaultic_utils.h"
#include "vaultic_mem.h"
#include "vaultic_cipher.h"
#include "vaultic_cipher_tests.h"
#include "vaultic_apdu.h"
#include "vaultic_putkey_aux.h"
#include "vaultic_readkey_aux.h"
#include "vaultic_command.h"
#include "vaultic_api.h"
#include "vaultic_version.h"

/*
 * Local consts 
 */
#define VLT_PASS_MIN_LEN            (VLT_U8)0x04
#define VLT_PASS_MAX_LEN            (VLT_U8)0x20
#define VLT_SCRTKEY_HDR_LEN         (VLT_U8)0x04
#define VLT_FILENAME_MIN_LEN        (VLT_U8)0x02
#define VLT_FILENAME_MAX_LEN        (VLT_U8)0x09

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

VLT_STS VltSubmitPassword(VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
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
     * A password can only be between 0x04 and 0x20 bytes long
     */
    if( ( VLT_PASS_MAX_LEN < u8PasswordLength ) || 
        ( VLT_PASS_MIN_LEN > u8PasswordLength ) )
    {
        return( ESPPASSLENIVLD );
    }

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SUBMIT_PASSWORD;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8UserID;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8RoleID;
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

VLT_STS VltInitializeUpdate(VLT_U8 u8UserID, 
    VLT_U8 u8RoleID,
    VLT_U8 u8HostChallengeLength,
    const VLT_U8 *pu8HostChallenge,
    VLT_INIT_UPDATE *pRespData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;

    VLT_STS  status = VLT_FAIL;
    VLT_BOOL bSCPMode; /* indicates SCP mode or MS mode */    
    VLT_U16  pu16Require;
    idx = VLT_APDU_DATA_OFFSET;

    /*
    * Check the Repsonse Data parameter is not NULL
    */
    if( NULL == pRespData )
    {
        return( EIUNULLPARA ); 
    }

    /* If the host challenge is specified then the length must also be
     * specified, otherwise if the challenge is NULL then the length must be
     * zero. */

    if( ( ( 0 == u8HostChallengeLength ) && ( NULL != pu8HostChallenge ) )  ||
        ( ( 0 != u8HostChallengeLength ) && ( NULL == pu8HostChallenge ) ) )
    {
        return( EIUINVLDPARAM );
    }
    
    /* Are we in SCP mode, or MS mode?
     * Use the length of the host challenge to find out. */

    bSCPMode = (0 != u8HostChallengeLength);

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_INITIALIZE_UPDATE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8UserID;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8RoleID;
    if (bSCPMode)
    {
        /* This is a case #4 (case #3 + case #2) command: P3 is LIN. */
        Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(u8HostChallengeLength);

        /* Build Data In */
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &Command.pu8Data[idx], pu8HostChallenge, u8HostChallengeLength);
        idx += u8HostChallengeLength;

        pu16Require = VLT_INITIALIZE_UPDATE_SCP_RSP_LENGTH;
    }
    else
    {
        /* This is a case #2 command: P3 is LEXP. */
        Command.pu8Data[VLT_APDU_P3_OFFSET] = 
            LEXP(VLT_INITIALIZE_UPDATE_MS_RSP_LENGTH);

        pu16Require = VLT_INITIALIZE_UPDATE_MS_RSP_LENGTH;
    }

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, pu16Require, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if(  VLT_OK != status )
    {
        return( status );
    }
    
    idx = 0;
    
    if( bSCPMode ) /* we're expecting an SCP02 or SCP03 response */
    {
        /* Unpack the header common to both SCP02 and SCP03. */

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(pRespData->data.Scp02.au8SerialNumber, &Response.pu8Data[idx], VLT_SCP_CHIP_SERIAL_LENGTH);
        idx += VLT_SCP_CHIP_SERIAL_LENGTH;
        idx += VLT_SCP_RFU_LENGTH;
        pRespData->data.Scp02.u8KeySetIndex = Response.pu8Data[idx++];
        pRespData->data.Scp02.u8ScpID = Response.pu8Data[idx++];

        /* Unpack the remainder of the structures. */

        switch (pRespData->data.Scp02.u8ScpID)
        {
            case VLT_SCP_ID_SCP02:

                pRespData->u8LoginMethodID = VLT_LOGIN_SCP02;

                pRespData->data.Scp02.u16SequenceCounter = VltEndianReadPU16(&Response.pu8Data[idx]);
                idx += VLT_SCP02_SEQUENCE_CTR_LENGTH;
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy(pRespData->data.Scp02.au8DeviceChallenge, &Response.pu8Data[idx],
                            VLT_SCP02_DEVICE_CHALLENGE_LENGTH);
                idx += VLT_SCP02_DEVICE_CHALLENGE_LENGTH;
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy(pRespData->data.Scp02.au8Cryptogram, &Response.pu8Data[idx],
                            VLT_SCP_CRYPTOGRAM_LENGTH);
                idx += VLT_SCP_CRYPTOGRAM_LENGTH;
                break;

            case VLT_SCP_ID_SCP03:

                pRespData->u8LoginMethodID = VLT_LOGIN_SCP03;

                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy(pRespData->data.Scp03.u8DeviceChallenge, &Response.pu8Data[idx],
                            VLT_SCP03_DEVICE_CHALLENGE_LENGTH);
                idx += VLT_SCP03_DEVICE_CHALLENGE_LENGTH;
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy(pRespData->data.Scp03.u8Cryptogram, &Response.pu8Data[idx],
                            VLT_SCP_CRYPTOGRAM_LENGTH);
                idx += VLT_SCP_CRYPTOGRAM_LENGTH;
                break;

            default:
                return( EIUBADSCP ); /* unrecognised SCP identifier */
        }        
    }
    else /* we're expecting an MS response */
    {
        pRespData->u8LoginMethodID = VLT_LOGIN_MS;

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(pRespData->data.MS.u8DeviceChallenge, &Response.pu8Data[idx],
                    VLT_MS_DEVICE_CHALLENGE_LENGTH);
        idx += VLT_MS_DEVICE_CHALLENGE_LENGTH;
    }

    return( status );
}


VLT_STS VltExternalAuthenticate(VLT_U8 u8AuthLevel,
    VLT_U8 u8ChannelLevel,
    VLT_U8 u8CryptogramLength,
    const VLT_U8 *pu8Cryptogram )
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;
    idx = VLT_APDU_DATA_OFFSET;

    /*
     * Validate critical input parameters 
     */
    if( NULL == pu8Cryptogram )
    {
        return( EEANULLPARA );
    }

    if( 0 == u8CryptogramLength )
    {
        return( EEAIVLDCRPTLEN );
    }

    switch( u8AuthLevel )
    {
        case VLT_LOGIN_SCP02:
        case VLT_LOGIN_SCP03:
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_CHANNEL;
            Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_EXTERNAL_AUTHENTICATE_SCP;
            Command.pu8Data[VLT_APDU_P1_OFFSET] = u8ChannelLevel;
            break;

        case VLT_LOGIN_MS:
            Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
            Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_EXTERNAL_AUTHENTICATE_MS;
            Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
            break;

        default:
            return( EEABADCHAN );
    }
    
    /* Build APDU */
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(u8CryptogramLength);

    /* Build Data In */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8Cryptogram, u8CryptogramLength);
    idx += u8CryptogramLength;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

/* Note: Manage Authentication Data is documented as permitting command
 * chaining but it never should need to use it as the P3 length field should
 * not exceed 0x4C. */

VLT_STS VltManageAuthenticationData( const VLT_MANAGE_AUTH_DATA *pAuthSetup )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    VLT_U16 u16Len = 0;    
    VLT_U8 i = 0;
    idx = VLT_APDU_DATA_OFFSET;

    if( NULL == pAuthSetup )
    {
        return( EMADNULLPARA );
    }

    /* Build APDU */        
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_MANAGE_AUTHENTICATION_DATA;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = pAuthSetup->u8UserID;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = pAuthSetup->u8OperationID;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;


    /* Build Data In */
    /* The authentication data is sent only in the case of the create and
     * update operations. */
    if( ( pAuthSetup->u8OperationID == VLT_CREATE_USER ) ||
        ( pAuthSetup->u8OperationID == VLT_UPDATE_USER ) )
    {
        Command.pu8Data[idx++] = pAuthSetup->u8Method;
        Command.pu8Data[idx++] = pAuthSetup->u8RoleID;
        Command.pu8Data[idx++] = pAuthSetup->u8ChannelLevel;
        Command.pu8Data[idx++] = pAuthSetup->u8SecurityOption;
        Command.pu8Data[idx++] = pAuthSetup->u8TryCount;

        switch( pAuthSetup->u8Method )
        {
            case VLT_LOGIN_PASSWORD:
                /*
                 * A password can only be between 0x04 and 0x20 bytes long
                 */
                if( ( VLT_PASS_MAX_LEN < pAuthSetup->data.cleartext.u8PasswordLength ) || 
                    ( VLT_PASS_MIN_LEN > pAuthSetup->data.cleartext.u8PasswordLength ) )
                {
                    return( EMADPASSLENIVLD );
                }
                u16Len = pAuthSetup->data.cleartext.u8PasswordLength;

                Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 0) & 0xFF);

                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &Command.pu8Data[idx], 
                                pAuthSetup->data.cleartext.u8Password,
                    pAuthSetup->data.cleartext.u8PasswordLength );

                idx += pAuthSetup->data.cleartext.u8PasswordLength;
                break;
            case VLT_LOGIN_MS:                
            case VLT_LOGIN_SCP02:
            case VLT_LOGIN_SCP03:

                if( VLT_LOGIN_MS == pAuthSetup->u8Method )
                {
                    /*
                     * MSAuthentication need an 3DES key that is 1 key.
                     */
                    if( 1 != pAuthSetup->data.secret.u8NumberOfKeys ) 
                    {
                        return( EMADMSPKEYSINVLD );
                    }
                }
                else
                {
                    /*
                     * SCP02 and SCP03 need an SMAC and SENC that is 2 keys.
                     */
                    if( 2 != pAuthSetup->data.secret.u8NumberOfKeys )
                    {
                        return( EMADSCPKEYSINVLD );
                    }   
                }
                
                /*
                 * Calculate the key length 
                 */
                /* Add secret key header first */
                u16Len = pAuthSetup->data.secret.u8NumberOfKeys * VLT_SCRTKEY_HDR_LEN;
                for( i = 0; i < pAuthSetup->data.secret.u8NumberOfKeys; i++ )
                {
                    u16Len +=  pAuthSetup->data.secret.aKeys[i].u16KeyLength ;
                }

                Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8)((u16Len >> 0) & 0xFF);

                /*
                 *  Set up the keys
                 */
                for( i = 0; i < pAuthSetup->data.secret.u8NumberOfKeys; i++ )
                {

                    Command.pu8Data[idx++] = pAuthSetup->data.secret.aKeys[i].u8KeyID;
                    Command.pu8Data[idx++] = pAuthSetup->data.secret.aKeys[i].u8Mask;                   
                    Command.pu8Data[idx++] = (VLT_U8)( ( pAuthSetup->data.secret.aKeys[i].u16KeyLength >> 8 ) & 0xFF );
                    Command.pu8Data[idx++] = (VLT_U8)( ( pAuthSetup->data.secret.aKeys[i].u16KeyLength >> 0 ) & 0xFF );

                    /*
                    * No need to check the return type as pointer has been validated
                    */
                    (void)host_memcpyxor( &Command.pu8Data[idx],
                        pAuthSetup->data.secret.aKeys[i].pu8Key, 
                        pAuthSetup->data.secret.aKeys[i].u16KeyLength, 
                        pAuthSetup->data.secret.aKeys[i].u8Mask );

                    idx += pAuthSetup->data.secret.aKeys[i].u16KeyLength;
                }                   
                break;
            default:
                return( EMADBADOPER );
        }

        /* Update P3 now that we know the correct length. */
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 
            LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );
    }

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
    pRespData->u8MinSecurityLevel = Response.pu8Data[idx++];
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

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltGetChallenge(const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters, 
    VLT_PU8 pu8DeviceChallenge )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    VLT_U32 u32SendSize = 0;
    idx = VLT_APDU_DATA_OFFSET;

    /*
    * Check the input pointers
    */
    if( ( NULL == pAuthParameters ) || ( NULL == pu8DeviceChallenge ) )
    {
        return( EGCNULLPARA );
    }

    /*
    * Check the validity of the data contained within the Generic String 
    * Authentication data structure 
    */
    if( VLT_GEN_AUTH_USE_IDENTIFIERS == pAuthParameters->u8Option )
    {
        if( NULL == pAuthParameters->pu8DeviceIDPath )
        {
            return( EGCINVLDDEVIDPTH);
        }

        if( NULL == pAuthParameters->pu8HostIDPath )
        {
            return( EGCINVLDHSTIDPTH);
        }

        u32SendSize =  sizeof( pAuthParameters->u8ChallengeSize ) + 
            sizeof( pAuthParameters->u8Option) + 
            sizeof( pAuthParameters->u16DeviceIDPathLength ) +
            pAuthParameters->u16DeviceIDPathLength +
            sizeof( pAuthParameters->u16HostIDPathLength ) +
            pAuthParameters->u16HostIDPathLength ;
    }
    else
    {
        u32SendSize =  sizeof( pAuthParameters->u8ChallengeSize ) + 
            sizeof( pAuthParameters->u8Option);
    }
       
    if( VltCommsGetMaxSendSize() < u32SendSize )
    {
        return( EGCDATATOOLRG );
    }

    /* Pack the command data and the fixed initial part of the Generic Strong
     * Authentication parameters. */

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GET_CHALLENGE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    /* P3 is filled out once the data has been built */

    /* Build Data In */

    Command.pu8Data[idx++] = pAuthParameters->u8ChallengeSize;
    Command.pu8Data[idx++] = pAuthParameters->u8Option;

    /* Append the variable sized identifiers portion of the GSA parameters only.
     * if it is required. */

    if( pAuthParameters->u8Option == VLT_GEN_AUTH_USE_IDENTIFIERS )
    {
        /* Data In (continued) */
        Command.pu8Data[idx++] = (VLT_U8) ((pAuthParameters->u16DeviceIDPathLength >> 8) & 0xFF);
        Command.pu8Data[idx++] = (VLT_U8) ((pAuthParameters->u16DeviceIDPathLength >> 0) & 0xFF);

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &Command.pu8Data[idx], 
            pAuthParameters->pu8DeviceIDPath,
            pAuthParameters->u16DeviceIDPathLength);

        idx += pAuthParameters->u16DeviceIDPathLength;

        Command.pu8Data[idx++] = (VLT_U8) ((pAuthParameters->u16HostIDPathLength >> 8) & 0xFF);
        Command.pu8Data[idx++] = (VLT_U8) ((pAuthParameters->u16HostIDPathLength >> 0) & 0xFF);

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &Command.pu8Data[idx], 
            pAuthParameters->pu8HostIDPath,
            pAuthParameters->u16HostIDPathLength );

        idx += pAuthParameters->u16HostIDPathLength;
    }

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 
        pAuthParameters->u8ChallengeSize, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if (VLT_OK != status)
    {
        return( status );
    }

    /* Unpack the response */
    idx = 0;    

    /* Unpack the challenge into the output buffer. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8DeviceChallenge, &Response.pu8Data[idx], 
        pAuthParameters->u8ChallengeSize );

    return( status );
}
#endif /*( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )*/

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltGetChallenge(const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters, 
    VLT_PU8 pu8DeviceChallenge )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    idx = VLT_APDU_DATA_OFFSET;

    /*
    * Check the input pointers
    */
    if( ( NULL == pAuthParameters ) || ( NULL == pu8DeviceChallenge ) )
    {
        return( EGCNULLPARA );
    }

    /* Pack the command data and the fixed initial part of the Generic Strong
     * Authentication parameters. */

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GET_CHALLENGE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    /* P3 is filled out once the data has been built */

    /* Build Data In */

    Command.pu8Data[idx++] = pAuthParameters->u8ChallengeSize;
    Command.pu8Data[idx++] = pAuthParameters->u8Option;

    /* Append the variable sized identifiers portion of the GSA parameters only.
     * if it is required. */

    if( pAuthParameters->u8Option == VLT_GEN_AUTH_USE_IDENTIFIERS )
    {
        Command.pu8Data[idx++] = pAuthParameters->u8DeviceIdGroup;
        Command.pu8Data[idx++] = pAuthParameters->u8DeviceIdIndex;
        Command.pu8Data[idx++] = pAuthParameters->u8HostIdGroup;
        Command.pu8Data[idx++] = pAuthParameters->u8HostIdIndex;
    }

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 
        pAuthParameters->u8ChallengeSize, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if (VLT_OK != status)
    {
        return( status );
    }

    /* Unpack the response */
    idx = 0;    

    /* Unpack the challenge into the output buffer. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8DeviceChallenge, &Response.pu8Data[idx], 
        pAuthParameters->u8ChallengeSize );

    return( status );
}
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltGenericInternalAuthenticate( 
    const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters,
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
    if( ( NULL == pAuthParameters ) ||
        ( NULL == pu8HostChallenge ) ||
        ( NULL == pu8DeviceChallenge ) ||
        ( NULL == pu16SignatureLength )  ||        
        ( NULL == pu8Signature ) )
    {
        return( EGIANULLPARA );
    }

    if( 0 == *pu16SignatureLength ) 
    {
        return( EGIAIVLDSIGLEN );
    }

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GEN_INTERNAL_AUTHENTICATE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Cache the data in case we need to resend it */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( apduHeader, Command.pu8Data, VLT_APDU_TYPICAL_HEADER_SZ );

    /* Set up the bChallengeSize & bOption */
    Command.pu8Data[idx++] = pAuthParameters->u8ChallengeSize;
    Command.pu8Data[idx++] = pAuthParameters->u8Option;
    /*
     * If the bOption is set to use identifiers in the authentication protocol 
     * add them to the APDU, otherwise just add the Host Challenge
     */
    if (pAuthParameters->u8Option == VLT_GEN_AUTH_USE_IDENTIFIERS)
    {
        Command.pu8Data[idx++] = (VLT_U8)((pAuthParameters->u16DeviceIDPathLength >> 8) & 0xFF);
        Command.pu8Data[idx++] = (VLT_U8)((pAuthParameters->u16DeviceIDPathLength >> 0) & 0xFF);

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &Command.pu8Data[idx], pAuthParameters->pu8DeviceIDPath, 
            pAuthParameters->u16DeviceIDPathLength );

        idx += pAuthParameters->u16DeviceIDPathLength;

        Command.pu8Data[idx++] = (VLT_U8)((pAuthParameters->u16HostIDPathLength >> 8) & 0xFF);
        Command.pu8Data[idx++] = (VLT_U8)((pAuthParameters->u16HostIDPathLength >> 0) & 0xFF);

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( &Command.pu8Data[idx], pAuthParameters->pu8HostIDPath, 
            pAuthParameters->u16HostIDPathLength );

        idx += pAuthParameters->u16HostIDPathLength;
    }

    /* Append the host challenge. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], 
        pu8HostChallenge, 
        pAuthParameters->u8ChallengeSize );

    idx += pAuthParameters->u8ChallengeSize;

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
    (void)host_memcpy( pu8DeviceChallenge, &Response.pu8Data[idx], pAuthParameters->u8ChallengeSize );
    idx += pAuthParameters->u8ChallengeSize;
    Response.u16Len -= pAuthParameters->u8ChallengeSize;

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
#endif /*( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltGenericInternalAuthenticate( 
    const VLT_GENERIC_AUTH_SETUP_DATA *pAuthParameters,
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
    if( ( NULL == pAuthParameters ) ||
        ( NULL == pu8HostChallenge ) ||
        ( NULL == pu8DeviceChallenge ) ||
        ( NULL == pu16SignatureLength )  ||        
        ( NULL == pu8Signature ) )
    {
        return( EGIANULLPARA );
    }

    if( 0 == *pu16SignatureLength ) 
    {
        return( EGIAIVLDSIGLEN );
    }

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GEN_INTERNAL_AUTHENTICATE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Cache the data in case we need to resend it */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( apduHeader, Command.pu8Data, VLT_APDU_TYPICAL_HEADER_SZ );

    /* Set up the bChallengeSize & bOption */
    Command.pu8Data[idx++] = pAuthParameters->u8ChallengeSize;
    Command.pu8Data[idx++] = pAuthParameters->u8Option;
    /*
     * If the bOption is set to use identifiers in the authentication protocol 
     * add them to the APDU, otherwise just add the Host Challenge
     */
    if (pAuthParameters->u8Option == VLT_GEN_AUTH_USE_IDENTIFIERS)
    {
        Command.pu8Data[idx++] = pAuthParameters->u8DeviceIdGroup;
        Command.pu8Data[idx++] = pAuthParameters->u8DeviceIdIndex;
        Command.pu8Data[idx++] = pAuthParameters->u8HostIdGroup;
        Command.pu8Data[idx++] = pAuthParameters->u8HostIdIndex;
    }

    /* Append the host challenge. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], 
        pu8HostChallenge, 
        pAuthParameters->u8ChallengeSize );

    idx += pAuthParameters->u8ChallengeSize;

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
    (void)host_memcpy( pu8DeviceChallenge, &Response.pu8Data[idx], pAuthParameters->u8ChallengeSize );
    idx += pAuthParameters->u8ChallengeSize;
    Response.u16Len -= pAuthParameters->u8ChallengeSize;

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
#endif /*( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

VLT_STS VltGenericExternalAuthenticate(VLT_U8 u8HostChallengeLength,
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

    if( 0 == u8HostChallengeLength )
    {
        return( EGEAINVLDHOSTCHLEN );
    }

    if( 0 == u16HostSignatureLength )
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
         * maximum buffer size chunk it down to the buffer size, 
         * otherwise just accept the size as is.
         */
        if( chunk > VltCommsGetMaxSendSize() )
        {
            chunk = VltCommsGetMaxSendSize();
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_CHAINING;
        }
        else
        {            
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL; 
        }

        /* Set up the apdu */
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_GEN_EXTERNAL_AUTHENTICATE;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;        
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

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltInitializeAlgorithm(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_U8 u8Mode,
    const VLT_ALGO_PARAMS *pAlgorithm )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    VLT_U32 u32SendLen = 0;
    idx = VLT_APDU_DATA_OFFSET;

    /* Check for a valid pointer in the input arguments */
    if( NULL == pAlgorithm )
    {
        return( EIANULLPARA );
    }

    /*
    * If the Algo ID specifies that a label is specified check that the length
    * of this can fit into the available buffer space
    */
    if( ( VLT_ALG_CIP_RSAES_PKCS_OAEP == pAlgorithm->u8AlgoID ) ||
        ( VLT_ALG_KTS_RSA_OAEP_BASIC == pAlgorithm->u8AlgoID ) )
    {
        u32SendLen = pAlgorithm->data.RsaesOaep.u16LLen + sizeof( u8Mode ) +
            sizeof( pAlgorithm->u8AlgoID );

        if( VltCommsGetMaxSendSize() < u32SendLen )
        {
            return( EIADATATOOLRG );
        }
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
        case VLT_ALG_SIG_MAC_ISO9797_ALG1:
        case VLT_ALG_SIG_MAC_ISO9797_ALG3:
        case VLT_ALG_SIG_CMAC_AES:
            Command.pu8Data[idx++] = pAlgorithm->data.Cmac.u8Padding;
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[idx], pAlgorithm->data.Cmac.u8Iv, pAlgorithm->data.Cmac.u8IvLength);
            idx += pAlgorithm->data.Cmac.u8IvLength;
            break;

        case VLT_ALG_SIG_HMAC:
            Command.pu8Data[idx++] = pAlgorithm->data.Hmac.u8Digest;
            Command.pu8Data[idx++] = pAlgorithm->data.Hmac.u8Output;
            break;

        case VLT_ALG_SIG_HOTP:
        case VLT_ALG_SIG_TOTP:
            Command.pu8Data[idx++] = pAlgorithm->data.Otp.u8Output;
            break;

        case VLT_ALG_SIG_RSASSA_PKCS_PSS:
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPss.u8DigestPss;
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPss.u8DigestMgf1;
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPss.u8SaltLength;
            break;

        case VLT_ALG_SIG_RSASSA_PKCS:
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPkcs.u8Digest;
            break;

        case VLT_ALG_SIG_DSA:
        case VLT_ALG_SIG_ECDSA_GFP:
        case VLT_ALG_SIG_ECDSA_GF2M: /* unsure about this one */
            Command.pu8Data[idx++] = pAlgorithm->data.EcdsaDsa.u8Digest;
            break;

        case VLT_ALG_DIG_SHA1:
        case VLT_ALG_DIG_SHA224:
        case VLT_ALG_DIG_SHA256:
        case VLT_ALG_DIG_SHA384:
        case VLT_ALG_DIG_SHA512:
            /* no algorithm parameters */
            break;

        case VLT_ALG_CIP_DES:
        case VLT_ALG_CIP_TDES_2K_EDE:
        case VLT_ALG_CIP_TDES_3K_EDE:     
        case VLT_ALG_CIP_TDES_3K_EEE:     
        case VLT_ALG_CIP_AES:          
        case VLT_ALG_KTS_AES:       
        case VLT_ALG_KTS_TDES_3K_EEE:
        case VLT_ALG_KTS_TDES_3K_EDE:
            Command.pu8Data[idx++] = pAlgorithm->data.SymCipher.u8Mode;
            Command.pu8Data[idx++] = pAlgorithm->data.SymCipher.u8Padding;
            
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[idx], 
                pAlgorithm->data.SymCipher.u8Iv, 
                pAlgorithm->data.SymCipher.u8IvLength);

            idx += pAlgorithm->data.SymCipher.u8IvLength;
            break;

        case VLT_ALG_CIP_RSAES_PKCS_OAEP:
        case VLT_ALG_KTS_RSA_OAEP_BASIC:
            Command.pu8Data[idx++] = pAlgorithm->data.RsaesOaep.u8DigestOaep;
            Command.pu8Data[idx++] = pAlgorithm->data.RsaesOaep.u8DigestMgf1;
            Command.pu8Data[idx++] = (VLT_U8) ((pAlgorithm->data.RsaesOaep.u16LLen >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((pAlgorithm->data.RsaesOaep.u16LLen >> 0) & 0xFF);
            /* FIXME: There's an assumption here that the label will be small enough
             *        to fit in the buffer. The label length is a U16 so could well
             *        need to span multiple chunks. */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[idx], 
                pAlgorithm->data.RsaesOaep.pu8Label,
                pAlgorithm->data.RsaesOaep.u16LLen);

            idx += pAlgorithm->data.RsaesOaep.u16LLen;
            break;

        case VLT_ALG_SIG_RSASSA_X509:/* "strongly discouraged" */
        case VLT_ALG_CIP_RSAES_PKCS:
        case VLT_ALG_CIP_RSAES_X509: /* "strongly discouraged" */
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
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltInitializeAlgorithm(VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_U8 u8Mode,
    const VLT_ALGO_PARAMS *pAlgorithm )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    
    VLT_U32 u32SendLen = 0;
    idx = VLT_APDU_DATA_OFFSET;

    /* Check all pointers are valid pointer in the input arguments */
    if ( NULL == pAlgorithm )
    {
        return( EIANULLPARA );
    }

    /*
    * If the Algo ID specifies that a label is specified check that the length
    * of this can fit into the available buffer space
    */
    if( ( VLT_ALG_CIP_RSAES_PKCS_OAEP == pAlgorithm->u8AlgoID ) ||
        ( VLT_ALG_KTS_RSA_OAEP_BASIC == pAlgorithm->u8AlgoID ) )
    {
        u32SendLen = pAlgorithm->data.RsaesOaep.u16LLen + sizeof( u8Mode ) +
            sizeof( pAlgorithm->u8AlgoID );

        if( VltCommsGetMaxSendSize() < u32SendLen )
        {
            return( EIADATATOOLRG );
        }
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
        case VLT_ALG_SIG_MAC_ISO9797_ALG1:
        case VLT_ALG_SIG_MAC_ISO9797_ALG3:
            Command.pu8Data[idx++] = pAlgorithm->data.Cmac.u8Padding;
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[idx], pAlgorithm->data.Cmac.u8Iv, pAlgorithm->data.Cmac.u8IvLength);
            idx += pAlgorithm->data.Cmac.u8IvLength;
            break;

        case VLT_ALG_SIG_CMAC_AES:
            /* The Vault IC expects Padding method 2 and an all zero IV for
            * CMAC so no parameters are passed down 
            */
            break;

        case VLT_ALG_SIG_HMAC:
            Command.pu8Data[idx++] = pAlgorithm->data.Hmac.u8Digest;
            Command.pu8Data[idx++] = pAlgorithm->data.Hmac.u8Output;
            break;

        case VLT_ALG_SIG_HOTP:
        case VLT_ALG_SIG_TOTP:
            Command.pu8Data[idx++] = pAlgorithm->data.Otp.u8Output;
            break;

        case VLT_ALG_SIG_RSASSA_PKCS_PSS:
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPss.u8DigestPss;
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPss.u8DigestMgf1;
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPss.u8SaltLength;
            break;

        case VLT_ALG_SIG_RSASSA_PKCS:
            Command.pu8Data[idx++] = pAlgorithm->data.RsassaPkcs.u8Digest;
            break;

        case VLT_ALG_SIG_DSA:
        case VLT_ALG_SIG_ECDSA_GFP:
        case VLT_ALG_SIG_ECDSA_GF2M: /* unsure about this one */
            Command.pu8Data[idx++] = pAlgorithm->data.EcdsaDsa.u8Digest;
            break;

        case VLT_ALG_DIG_SHA1:
        case VLT_ALG_DIG_SHA224:
        case VLT_ALG_DIG_SHA256:
        case VLT_ALG_DIG_SHA384:
        case VLT_ALG_DIG_SHA512:
            /* no algorithm parameters */
            break;

        case VLT_ALG_CIP_DES:
        case VLT_ALG_CIP_TDES_2K_EDE:
        case VLT_ALG_CIP_TDES_3K_EDE:     
        case VLT_ALG_CIP_TDES_3K_EEE:     
        case VLT_ALG_CIP_AES:          
        case VLT_ALG_KTS_AES:       
        case VLT_ALG_KTS_TDES_3K_EEE:
        case VLT_ALG_KTS_TDES_3K_EDE:
            Command.pu8Data[idx++] = pAlgorithm->data.SymCipher.u8Mode;
            Command.pu8Data[idx++] = pAlgorithm->data.SymCipher.u8Padding;
            
            host_memcpy( &Command.pu8Data[idx], 
                pAlgorithm->data.SymCipher.u8Iv, 
                pAlgorithm->data.SymCipher.u8IvLength);

            idx += pAlgorithm->data.SymCipher.u8IvLength;
            break;

        case VLT_ALG_CIP_RSAES_PKCS_OAEP:
        case VLT_ALG_KTS_RSA_OAEP_BASIC:
            /* Check the pointers are valid before trying to use them */
            if ( ( NULL == pAlgorithm->data.RsaesOaep.pu8Label )&&
               ( 0 != pAlgorithm->data.RsaesOaep.u16LLen ) )
            {
                return ( EIAOAEPNULLPARA );
            }

            Command.pu8Data[idx++] = pAlgorithm->data.RsaesOaep.u8DigestOaep;
            Command.pu8Data[idx++] = pAlgorithm->data.RsaesOaep.u8DigestMgf1;
            Command.pu8Data[idx++] = (VLT_U8) ((pAlgorithm->data.RsaesOaep.u16LLen >> 8) & 0xFF);
            Command.pu8Data[idx++] = (VLT_U8) ((pAlgorithm->data.RsaesOaep.u16LLen >> 0) & 0xFF);
            /* FIXME: There's an assumption here that the label will be small enough
             *        to fit in the buffer. The label length is a U16 so could well
             *        need to span multiple chunks. */
            
            /* don't copy the label if the ptr is NULL. */            
            if ( NULL != pAlgorithm->data.RsaesOaep.pu8Label )
            {
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &Command.pu8Data[idx], 
                    pAlgorithm->data.RsaesOaep.pu8Label,
                    pAlgorithm->data.RsaesOaep.u16LLen);
            }

            idx += pAlgorithm->data.RsaesOaep.u16LLen;
            break;

        case VLT_ALG_SIG_RSASSA_X509:/* "strongly discouraged" */
        case VLT_ALG_CIP_RSAES_PKCS:
        case VLT_ALG_CIP_RSAES_X509: /* "strongly discouraged" */
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
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

VLT_STS VltUnInitializeAlgorithm()
{
	VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;    

	 /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_INITIALIZE_ALGORITHM;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0x00;;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0x00;
    /* P3 is filled out once the data has been built */

    /* Build Data In */

    Command.pu8Data[idx++] = 0x00;

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

    #if( VLT_ENABLE_KEY_SECRET == VLT_ENABLE )
        case VLT_KEY_DES: /* Secret key object */
        case VLT_KEY_TDES_2K:
        case VLT_KEY_TDES_3K:
        case VLT_KEY_AES_128:
        case VLT_KEY_AES_192:
        case VLT_KEY_AES_256:
        case VLT_KEY_HMAC:
            status = VltPutKey_Secret( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.SecretKey),
                &Sw );
            break;
    #endif /* ( VLT_ENABLE_KEY_SECRET == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_HOTP == VLT_ENABLE )
        case VLT_KEY_HOTP: /* HOTP key object */
            status = VltPutKey_Hotp( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.HotpKey),
                &Sw );
            break;
    #endif /* ( VLT_ENABLE_KEY_HOTP == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_TOTP == VLT_ENABLE )
        case VLT_KEY_TOTP: /* TOTP key object */
            status = VltPutKey_Totp( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.TotpKey),
                &Sw );
            break;
    #endif /* ( VLT_ENABLE_KEY_TOTP == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_RSA == VLT_ENABLE )
        case VLT_KEY_RSASSA_PUB: /* RSA public key object */
        case VLT_KEY_RSAES_PUB:
            status = VltPutKey_RsaPublic( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.RsaPubKey),
                &Sw );
            break;

        case VLT_KEY_RSASSA_PRIV: /* RSA private key object */
        case VLT_KEY_RSAES_PRIV:
            status = VltPutKey_RsaPrivate( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.RsaPrivKey),
                &Sw );
            break;

        case VLT_KEY_RSASSA_PRIV_CRT: /* RSA CRT private key object */
        case VLT_KEY_RSAES_PRIV_CRT:
            status = VltPutKey_RsaPrivateCrt( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.RsaPrivCrtKey),
                &Sw );
            break;
    #endif /* ( VLT_ENABLE_KEY_RSA == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_DSA == VLT_ENABLE )
        case VLT_KEY_DSA_PUB: /* DSA public key object */
            status = VltPutKey_DsaPublic( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                (&pKeyObj->data.DsaPubKey),
                &Sw );
            break;

        case VLT_KEY_DSA_PRIV: /* DSA private key object */
            status = VltPutKey_DsaPrivate( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.DsaPrivKey),
                &Sw );
            break;

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
        case VLT_KEY_DSA_DOMAIN_PARAMS:
            status = VltPutKey_DsaParams( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.DsaParamsKey),
                &Sw );
            break;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

#endif /* ( VLT_ENABLE_KEY_DSA == VLT_ENABLE ) */
#if( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE )
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
		case VLT_KEY_ECC_PUB: /* ECC public key object */
			status = VltPutKey_EcdsaPublic( u8KeyGroup,
				u8KeyIndex,
				pKeyFilePrivileges,
				pKeyObj->u8KeyID,
				&(pKeyObj->data.EcdsaPubKey),
				&Sw );
			break;

		case VLT_KEY_ECC_PRIV: /* ECC private key object */
			status = VltPutKey_EcdsaPrivate( u8KeyGroup,
				u8KeyIndex,
				pKeyFilePrivileges,
				pKeyObj->u8KeyID,
				&(pKeyObj->data.EcdsaPrivKey),
				&Sw );
			break;

#else
		case VLT_KEY_ECDSA_PUB: /* ECDSA public key object */
			status = VltPutKey_EcdsaPublic( u8KeyGroup,
				u8KeyIndex,
				pKeyFilePrivileges,
				pKeyObj->u8KeyID,
				&(pKeyObj->data.EcdsaPubKey),
				&Sw );
			break;

		case VLT_KEY_ECDSA_PRIV: /* ECDSA private key object */
			status = VltPutKey_EcdsaPrivate( u8KeyGroup,
				u8KeyIndex,
				pKeyFilePrivileges,
				pKeyObj->u8KeyID,
				&(pKeyObj->data.EcdsaPrivKey),
				&Sw );
			break;
#endif

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

        case VLT_KEY_ECDSA_DOMAIN_PARMS: /* ECDSA Domain key object */
            status = VltPutKey_EcdsaParams( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.EcdsaParamsKey),
                &Sw );
            break;

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

   #endif /* ( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)

#if( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE )

        case VLT_KEY_HOST_DEVICE_ID: /* Host/Device ID key object */
            status = VltPutKey_IdKey( u8KeyGroup,
                u8KeyIndex,
                pKeyFilePrivileges,
                pKeyObj->u8KeyID,
                &(pKeyObj->data.HostDeviceIdKey ),
                &Sw );
            break;

#endif /* ( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE ) */

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

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
        ( Sw != VLT_STATUS_RESPONDING ) )
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

    /* Secret key object */
    #if( VLT_ENABLE_KEY_SECRET == VLT_ENABLE )
        case VLT_KEY_DES: 
        case VLT_KEY_TDES_2K:
        case VLT_KEY_TDES_3K:
        case VLT_KEY_AES_128:
        case VLT_KEY_AES_192:
        case VLT_KEY_AES_256:
		case VLT_KEY_SECRET_VALUE:
        case VLT_KEY_HMAC:            
            return( VltReadKey_Secret( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.SecretKey, &Sw ) );          
    #endif /* ( VLT_ENABLE_KEY_SECRET == VLT_ENABLE ) */

    /* HOTP key object */
    #if( VLT_ENABLE_KEY_HOTP == VLT_ENABLE )
        case VLT_KEY_HOTP:             
            return( VltReadKey_Hotp( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.HotpKey, &Sw ) );                      
    #endif /* ( VLT_ENABLE_KEY_HOTP == VLT_ENABLE ) */

    /* TOTP key object */
    #if( VLT_ENABLE_KEY_TOTP == VLT_ENABLE )
        case VLT_KEY_TOTP: 
            return( VltReadKey_Totp( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.TotpKey, &Sw ) );  
    #endif /* ( VLT_ENABLE_KEY_TOTP == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_RSA == VLT_ENABLE )
        /* RSA public key object */
        case VLT_KEY_RSASSA_PUB: 
        case VLT_KEY_RSAES_PUB:            
            return( VltReadKey_RsaPublic( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.RsaPubKey, &Sw ) );  

        /* RSA private key object */
        case VLT_KEY_RSASSA_PRIV: 
        case VLT_KEY_RSAES_PRIV:            
            return( VltReadKey_RsaPrivate( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.RsaPrivKey, &Sw ) ); 

        /* RSA CRT private key object */
        case VLT_KEY_RSASSA_PRIV_CRT: 
        case VLT_KEY_RSAES_PRIV_CRT:
            return( VltReadKey_RsaPrivateCrt( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.RsaPrivCrtKey, &Sw ) );
    #endif /* ( VLT_ENABLE_KEY_RSA == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_DSA == VLT_ENABLE )
        /* DSA public key object */
        case VLT_KEY_DSA_PUB:             
            return( VltReadKey_DsaPublic( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.DsaPubKey, &Sw ) );            

        /* DSA private key object */
        case VLT_KEY_DSA_PRIV:             
            return( VltReadKey_DsaPrivate( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.DsaPrivKey, &Sw ) );

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
        
        /* DSA Params key object */
        case VLT_KEY_DSA_DOMAIN_PARAMS:
            return( VltReadKey_DsaParams( u8KeyGroup, u8KeyIndex,
            &pKeyObj->data.DsaParamsKey, &Sw ) );

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

    #endif /* ( VLT_ENABLE_KEY_DSA == VLT_ENABLE ) */

    #if( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE )
			#if( VAULT_IC_VERSION  == VAULTIC_VERSION_1_2_1)
        /* ECDSA public key object */
        case VLT_KEY_ECC_PUB:             
            return( VltReadKey_EcdsaPublic( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaPubKey, &Sw ) );
        /* ECDSA private key object */
        case VLT_KEY_ECC_PRIV:             
            return( VltReadKey_EcdsaPrivate( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaPrivKey, &Sw ) );
#else
			 /* ECDSA public key object */
        case VLT_KEY_ECDSA_PUB:             
            return( VltReadKey_EcdsaPublic( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaPubKey, &Sw ) );
        /* ECDSA private key object */
        case VLT_KEY_ECDSA_PRIV:             
            return( VltReadKey_EcdsaPrivate( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaPrivKey, &Sw ) );
#endif

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
        
        /* DSA Params key object */
        case VLT_KEY_ECDSA_DOMAIN_PARMS:
            return( VltReadKey_EcdsaParams( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.EcdsaParamsKey, &Sw ) );

#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

    #endif /* ( VLT_ENABLE_KEY_ECDSA == VLT_ENABLE ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    
    #if( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE )

        /* Host/Device ID key object */
        case VLT_KEY_HOST_DEVICE_ID:
            return( VltReadKey_IdKey( u8KeyGroup, u8KeyIndex,
                &pKeyObj->data.HostDeviceIdKey, &Sw ) );

    #endif /* ( VLT_ENABLE_KEY_HOST_DEVICE_ID == VLT_ENABLE ) */

    #endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */ 

        default:
            return( ERKUNSUPPKEY );
    }
}

VLT_STS VltDeleteKey( VLT_U8 u8KeyGroup, VLT_U8 u8KeyIndex )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DELETE_KEY;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltEncrypt( VLT_U32 u32PlainTextLength,
    const VLT_U8 *pu8PlainText,
    VLT_PU32 pu32CipherTextLength,
    VLT_PU8 pu8CipherText )
{
    /* Ensure that the input buffer is specified. The remainder of the parameter
     * checking happens inside VltCase4. */
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    if( NULL == pu8PlainText )
    {
        return( EENULLPARA );
    }

    if ( 0 == u32PlainTextLength  )
    {
        return( EENINVLDPLTXTLEN );
    }

    status = VltCase4(VLT_INS_ENCRYPT_DECRYPT, 
        0,
        u32PlainTextLength, 
        pu8PlainText,
        pu32CipherTextLength, 
        pu8CipherText, 
        &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltDecrypt( VLT_U32 u32CipherTextLength,
    const VLT_U8 *pu8CipherText,
    VLT_PU32 pu32PlainTextLength,
    VLT_PU8 pu8PlainText )
{ 
    /* Ensure that the input buffer is specified. The remainder of the parameter
     * checking happens inside VltCase4. */
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    if( NULL == pu8CipherText )
    {
        return( EDNULLPARA );
    }

    if( 0 == u32CipherTextLength )
    {
        return( EDCINVLDCHPTXTLEN );
    }

    status = VltCase4( VLT_INS_ENCRYPT_DECRYPT, 
        0,
        u32CipherTextLength, 
        pu8CipherText,
        pu32PlainTextLength, 
        pu8PlainText, 
        &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltGenerateAssuranceMessage( VLT_U8 u8KeyGroup,
    VLT_U8 u8KeyIndex,
    VLT_PU8 pu8SignerIdLength,
    VLT_PU8 pu8SignerID,
    VLT_ASSURANCE_MESSAGE* pAssuranceMsg  )
{ 
#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;   

    
    /* validate critical parameters */
    if( ( NULL == pAssuranceMsg ) || 
        ( NULL == pu8SignerID ) || 
        ( NULL == pu8SignerIdLength ) )
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
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8KeyGroup;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8KeyIndex;
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
    if ( VLT_GA_SIGNER_ID_LENGTH + 
         VLT_GA_VERIFIER_ID_LENGTH + 
         VLT_GA_MESSAGE_LENGTH != Response.u16Len )
    {
        return ( EGAINVLDRECLEN );
    }
    

    /* Unpack the response */    
    idx = 0;
    
    /* Copy out the Signer ID byte stream. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pu8SignerID, &Response.pu8Data[idx], VLT_GA_SIGNER_ID_LENGTH );
    *pu8SignerIdLength = VLT_GA_SIGNER_ID_LENGTH;
    idx += VLT_GA_SIGNER_ID_LENGTH;
    
    /* Copy out the Verifier ID byte stream. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( pAssuranceMsg->pu8VerifierID, &Response.pu8Data[idx], VLT_GA_VERIFIER_ID_LENGTH );
    pAssuranceMsg->u8VerifierIdLength = VLT_GA_VERIFIER_ID_LENGTH;
    idx += VLT_GA_VERIFIER_ID_LENGTH;
    
    /* Copy out the assurance message byte stream. */
    host_memcpy( pAssuranceMsg->pu8AssuranceMessage, &Response.pu8Data[idx], VLT_GA_MESSAGE_LENGTH );
    pAssuranceMsg->u8AssuranceMessageLength = VLT_GA_MESSAGE_LENGTH;
    idx += VLT_GA_MESSAGE_LENGTH;
 
    return( status );    
#else
    return( EMETHODNOTSUPPORTED );
#endif 
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

VLT_STS VltUpdateSignature(VLT_U32 u32MessageLength,
						   const VLT_U8 *pu8Message)
{
	VLT_SW Sw = VLT_STATUS_NONE;
	VLT_STS status = VLT_FAIL;

    VLT_U16 u16Idx; 
    VLT_U16 u16MaxChunk;
    VLT_U32 u32Remaining;
  
	u16MaxChunk  = VltCommsGetMaxSendSize();
	u32Remaining = u32MessageLength;

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
    while (u32Remaining && Sw == VLT_STATUS_NEXT_MESSAGE_PART_EXPECTED);

	if ( ( Sw != VLT_STATUS_SUCCESS )
		&& ( Sw != VLT_STATUS_NONE ) )
	{
		return( Sw );
	}

	return( status );
}

VLT_STS VltComputeVerifyFinal(VLT_U16 u32SignatureLength,
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
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_2_1 )
VLT_STS VltDeriveKey(VLT_U8 u8keyGroup,
    VLT_U8 u8keyIndex,
	const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
	VLT_U8 u8DerivatedKeyType,
	VLT_U16 u16WDerivatedKeyLen,
	const VLT_KEY_DERIVATION *pKeyDerivation )
{
	VLT_SW Sw = VLT_STATUS_NONE;
	VLT_STS status = VLT_FAIL;
	idx = VLT_APDU_DATA_OFFSET;

	/* Check for a valid pointer in the input arguments */
	if( ( NULL == pKeyFilePrivileges ) ||
        ( NULL == pKeyDerivation ) )
    {
        return( EPKNULLPARA );
    }

	/* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DERIVE_KEY;
	Command.pu8Data[VLT_APDU_P1_OFFSET] = pKeyDerivation->u8SecretKeyGroup;
	Command.pu8Data[VLT_APDU_P2_OFFSET] = pKeyDerivation->u8SecretKeyIndex;
    /* P3 is filled out once the data has been built */

	Command.pu8Data[idx++] = pKeyDerivation->u8AlgoID;

	switch (pKeyDerivation->u8AlgoID )
	{
	case VLT_ALG_KDF_HASH_MODE:
		if ((pKeyDerivation->data.hashMode.u16prependLen > 0 && pKeyDerivation->data.hashMode.pu8prependData == NULL )
			||(pKeyDerivation->data.hashMode.u16appendLen > 0 && pKeyDerivation->data.hashMode.pu8appendData == NULL ))
		{
			return( EPKNULLPARA );
		}

		Command.pu8Data[idx++] = (VLT_U8) (pKeyDerivation->data.hashMode.u8digest);
		Command.pu8Data[idx++] = (VLT_U8) (u8DerivatedKeyType);
		Command.pu8Data[idx++] = (VLT_U8) ((u16WDerivatedKeyLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((u16WDerivatedKeyLen >> 0) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) (u8keyGroup);
		Command.pu8Data[idx++] = (VLT_U8) (u8keyIndex);
		/* bmPubAccess */
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

		//Optional param
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.hashMode.u16prependLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.hashMode.u16prependLen >> 0) & 0xFF);
		if (pKeyDerivation->data.hashMode.u16prependLen > 0)
		{
			//Prepend
			(void)host_memcpy( &Command.pu8Data[idx], 
				pKeyDerivation->data.hashMode.pu8prependData,
				pKeyDerivation->data.hashMode.u16prependLen);
			idx+= pKeyDerivation->data.hashMode.u16prependLen;
		}

		//Optional param
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.hashMode.u16appendLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.hashMode.u16appendLen >> 0) & 0xFF);
		if (pKeyDerivation->data.hashMode.u16appendLen > 0)
		{
			//Append
			(void)host_memcpy( &Command.pu8Data[idx], 
				pKeyDerivation->data.hashMode.pu8appendData,
				pKeyDerivation->data.hashMode.u16appendLen);
			idx+= pKeyDerivation->data.hashMode.u16appendLen;
		}

		break;
	case VLT_ALG_KDF_CONCATENATION_NIST:
	case VLT_ALG_KDF_CONCATENATION_X963:
		if ((pKeyDerivation->data.concatenation_SP800_56A.u16suppPubInfoLen > 0 && pKeyDerivation->data.concatenation_SP800_56A.pu8suppPubInfo == NULL)
			|| (pKeyDerivation->data.concatenation_SP800_56A.u16suppPrivInfoLen > 0 && pKeyDerivation->data.concatenation_SP800_56A.pu8suppPrivInfo == NULL)
			|| (pKeyDerivation->data.concatenation_SP800_56A.pu8algoId == NULL)
			|| (pKeyDerivation->data.concatenation_SP800_56A.pu8UInfo == NULL)
			|| (pKeyDerivation->data.concatenation_SP800_56A.pu8VInfo == NULL)
			)
			return( EPKNULLPARA );

		Command.pu8Data[idx++] = (VLT_U8) (pKeyDerivation->data.concatenation_SP800_56A.u8digest);
		Command.pu8Data[idx++] = (VLT_U8) (u8DerivatedKeyType);
		Command.pu8Data[idx++] = (VLT_U8) ((u16WDerivatedKeyLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((u16WDerivatedKeyLen >> 0) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) (u8keyGroup);
		Command.pu8Data[idx++] = (VLT_U8) (u8keyIndex);
		/* bmPubAccess */
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

		//Algo ID
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16algoIdLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16algoIdLen >> 0) & 0xFF);
		(void)host_memcpy( &Command.pu8Data[idx], 
			pKeyDerivation->data.concatenation_SP800_56A.pu8algoId,
			pKeyDerivation->data.concatenation_SP800_56A.u16algoIdLen);
		idx+= pKeyDerivation->data.concatenation_SP800_56A.u16algoIdLen;

		//Party U info
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16UInfoLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16UInfoLen >> 0) & 0xFF);
		(void)host_memcpy( &Command.pu8Data[idx], 
			pKeyDerivation->data.concatenation_SP800_56A.pu8UInfo,
			pKeyDerivation->data.concatenation_SP800_56A.u16UInfoLen);
		idx+= pKeyDerivation->data.concatenation_SP800_56A.u16UInfoLen;

		//Party V info
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16VInfoLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16VInfoLen >> 0) & 0xFF);
		(void)host_memcpy( &Command.pu8Data[idx], 
			pKeyDerivation->data.concatenation_SP800_56A.pu8VInfo,
			pKeyDerivation->data.concatenation_SP800_56A.u16VInfoLen);
		idx+= pKeyDerivation->data.concatenation_SP800_56A.u16VInfoLen;

		//Optional Party pub supp info
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16suppPubInfoLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16suppPubInfoLen >> 0) & 0xFF);
		if (pKeyDerivation->data.concatenation_SP800_56A.u16suppPubInfoLen > 0)
		{	
			(void)host_memcpy( &Command.pu8Data[idx], 
				pKeyDerivation->data.concatenation_SP800_56A.pu8suppPubInfo,
				pKeyDerivation->data.concatenation_SP800_56A.u16suppPubInfoLen);

			idx+= pKeyDerivation->data.concatenation_SP800_56A.u16suppPubInfoLen;
		}

		//Optional Party pub supp info
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16suppPrivInfoLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.concatenation_SP800_56A.u16suppPrivInfoLen >> 0) & 0xFF);
		if (pKeyDerivation->data.concatenation_SP800_56A.u16suppPrivInfoLen > 0)
		{	
			(void)host_memcpy( &Command.pu8Data[idx], 
				pKeyDerivation->data.concatenation_SP800_56A.pu8suppPrivInfo,
				pKeyDerivation->data.concatenation_SP800_56A.u16suppPrivInfoLen);

			idx+= pKeyDerivation->data.concatenation_SP800_56A.u16suppPrivInfoLen;
		}
		break;
	case VLT_ALG_KDF_COUNTER_MODE:
		Command.pu8Data[idx++] = (VLT_U8) (pKeyDerivation->data.counterMode_SP800_108.u8PRF);
		Command.pu8Data[idx++] = (VLT_U8) (u8DerivatedKeyType);
		Command.pu8Data[idx++] = (VLT_U8) ((u16WDerivatedKeyLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((u16WDerivatedKeyLen >> 0) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) (u8keyGroup);
		Command.pu8Data[idx++] = (VLT_U8) (u8keyIndex);
		/* bmPubAccess */
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
		Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;

		/* Context */
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.counterMode_SP800_108.u16wContextLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.counterMode_SP800_108.u16wContextLen >> 0) & 0xFF);

		(void)host_memcpy( &Command.pu8Data[idx], 
			pKeyDerivation->data.counterMode_SP800_108.pu8sContext,
			pKeyDerivation->data.counterMode_SP800_108.u16wContextLen);

		/* Label */
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.counterMode_SP800_108.u16wLabelLen >> 8) & 0xFF);
		Command.pu8Data[idx++] = (VLT_U8) ((pKeyDerivation->data.counterMode_SP800_108.u16wLabelLen >> 0) & 0xFF);

		(void)host_memcpy( &Command.pu8Data[idx], 
			pKeyDerivation->data.counterMode_SP800_108.pu8sLabel,
			pKeyDerivation->data.counterMode_SP800_108.u16wLabelLen);

		idx+= pKeyDerivation->data.counterMode_SP800_108.u16wLabelLen;

		break;
	default:
		return( EIABADALGO ); /* unrecognised algorithm */
	}

	 /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] =
        LIN(WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

	return(status);
}

VLT_STS VltConstructDHAgreement(
	VLT_U8 u8resultKeyGroup,
    VLT_U8 u8resultKeyIndex,
    const VLT_FILE_PRIVILEGES *pKeyFilePrivileges,
	const VLT_KEY_MATERIAL *pKeyMaterial )
{
	VLT_SW Sw = VLT_STATUS_NONE;
	VLT_STS status = VLT_FAIL;
	idx = VLT_APDU_DATA_OFFSET;

	 /* Check for a valid pointer in the input arguments */
	if( ( NULL == pKeyFilePrivileges ) ||
        ( NULL == pKeyMaterial ) )
    {
        return( EPKNULLPARA );
    }

	/* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_ESTABLISH_KEY_MATERIAL;
	Command.pu8Data[VLT_APDU_P1_OFFSET] = pKeyMaterial->u8StaticKeyGroup;
	Command.pu8Data[VLT_APDU_P2_OFFSET] = pKeyMaterial->u8StaticKeyIndex;
    /* P3 is filled out once the data has been built */

	/* Build Data In common data*/
	Command.pu8Data[idx++] = pKeyMaterial->u8AlgoID;
	Command.pu8Data[idx++] = (VLT_U8)u8resultKeyGroup;
	Command.pu8Data[idx++] = (VLT_U8)u8resultKeyIndex;
	/* bmAccess */
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Read;
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Write;
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Delete;
    Command.pu8Data[idx++] = pKeyFilePrivileges->u8Execute;
	
	/* Specific data */
	switch( pKeyMaterial->u8AlgoID )
	{
	case VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GFp:
	case VLT_ALG_KAS_ONEPASS_BSI_ECC_DH_GF2m:
		{
			Command.pu8Data[idx++] = (VLT_U8) ((pKeyMaterial->data.onePass.u8KLen >> 8) & 0xFF);
			Command.pu8Data[idx++] = (VLT_U8) ((pKeyMaterial->data.onePass.u8KLen >> 0) & 0xFF);

			/* don't copy the key object if the ptr is NULL. */            
			if ( NULL != pKeyMaterial->data.onePass.pu8keyObject )
			{
				/*
				* No need to check the return type as pointer has been validated
				*/
				(void)host_memcpy( &Command.pu8Data[idx], 
					pKeyMaterial->data.onePass.pu8keyObject,
					pKeyMaterial->data.onePass.u8KLen);
			}

			idx += pKeyMaterial->data.onePass.u8KLen;
		}
		break;

	case VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GFp:
	case VLT_ALG_KAS_STATIC_UNIFIED_BSI_ECC_DH_GF2m:
		Command.pu8Data[idx++] = pKeyMaterial->data.staticUnified.u8PubKeyGroup;
		Command.pu8Data[idx++] = pKeyMaterial->data.staticUnified.u8PubKeyIndex;
		break;
	default:
		return( EIABADALGO ); /* unrecognised algorithm */
	}

	  /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] =
        LIN(WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

	return(status);
}
#endif

VLT_STS VltGenerateRandom(VLT_U8 u8NumberOfCharacters, 
    VLT_PU16 pu16RandomCharacters )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    /* validate critical parameters */
    if( NULL == pu16RandomCharacters )
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
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8NumberOfCharacters;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;
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
    (void)host_memcpy( (VLT_PU8)pu16RandomCharacters, Response.pu8Data, Response.u16Len );

    return( VLT_OK );


    return( status );
}

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
VLT_STS VltGenerateKeyPair(VLT_U8 u8PublicKeyGroup,
    VLT_U8 u8PublicKeyIndex,
    const VLT_FILE_PRIVILEGES *pPublicKeyFilePrivileges,
    VLT_U8 u8PrivateKeyGroup,
    VLT_U8 u8PrivateKeyIndex,
    const VLT_FILE_PRIVILEGES *pPrivateKeyFilePrivileges,
    const VLT_KEY_GEN_DATA *pKeyGenData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;
  

    if( ( NULL == pPublicKeyFilePrivileges ) ||
        ( NULL == pPrivateKeyFilePrivileges ) ||
        ( NULL == pKeyGenData ) )
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
    /* bmPubAccess */
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Read;
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Write;
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Delete;
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Execute;
    /* iPrivGroup */
    Command.pu8Data[idx++] = u8PrivateKeyGroup;
    /* iPriv */
    Command.pu8Data[idx++] = u8PrivateKeyIndex;
    /* bmPubAccess */
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Read;
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Write;
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Delete;
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Execute;

    /* abParams */
    switch( pKeyGenData->u8AlgoID )
    {
        case VLT_ALG_KPG_RSASSA:
        case VLT_ALG_KPG_RSAES:
            {
                const VLT_KEY_GEN_RSA_DATA *d = &pKeyGenData->data.RsaKeyGenObj;

                Command.pu8Data[idx++] = d->u8Option;
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16Length >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16Length >> 0) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16ELen >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16ELen >> 0) & 0xFF);
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &Command.pu8Data[idx], d->pu8e, d->u16ELen);
                idx += d->u16ELen;
            }
            break;

        case VLT_ALG_KPG_DSA:
        case VLT_ALG_KPG_ECDSA_GFP:
        case VLT_ALG_KPG_ECDSA_GF2M:
            {
                const VLT_KEY_GEN_ECDSA_DSA_DATA *d = &pKeyGenData->data.EcdsaDsaKeyGenObj;

                Command.pu8Data[idx++] = (VLT_U8) ((d->u16PathLength >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16PathLength >> 0) & 0xFF);
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &Command.pu8Data[idx], d->pu8DomainPath, d->u16PathLength);
                            idx += d->u16PathLength;
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
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
VLT_STS VltGenerateKeyPair(VLT_U8 u8PublicKeyGroup,
    VLT_U8 u8PublicKeyIndex,
    const VLT_FILE_PRIVILEGES *pPublicKeyFilePrivileges,
    VLT_U8 u8PrivateKeyGroup,
    VLT_U8 u8PrivateKeyIndex,
    const VLT_FILE_PRIVILEGES *pPrivateKeyFilePrivileges,
    const VLT_KEY_GEN_DATA *pKeyGenData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;
  

    if( ( NULL == pPublicKeyFilePrivileges ) ||
        ( NULL == pPrivateKeyFilePrivileges ) ||
        ( NULL == pKeyGenData ) )
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
    /* bmPubAccess */
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Read;
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Write;
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Delete;
    Command.pu8Data[idx++] = pPublicKeyFilePrivileges->u8Execute;
    /* iPrivGroup */
    Command.pu8Data[idx++] = u8PrivateKeyGroup;
    /* iPriv */
    Command.pu8Data[idx++] = u8PrivateKeyIndex;
    /* bmPubAccess */
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Read;
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Write;
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Delete;
    Command.pu8Data[idx++] = pPrivateKeyFilePrivileges->u8Execute;

    /* abParams */
    switch( pKeyGenData->u8AlgoID )
    {
        case VLT_ALG_KPG_RSASSA:
        case VLT_ALG_KPG_RSAES:
            {
                const VLT_KEY_GEN_RSA_DATA *d = &pKeyGenData->data.RsaKeyGenObj;

                Command.pu8Data[idx++] = d->u8Option;
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16Length >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16Length >> 0) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16ELen >> 8) & 0xFF);
                Command.pu8Data[idx++] = (VLT_U8) ((d->u16ELen >> 0) & 0xFF);
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &Command.pu8Data[idx], d->pu8e, d->u16ELen);
                idx += d->u16ELen;
            }
            break;

        case VLT_ALG_KPG_DSA:
        case VLT_ALG_KPG_ECDSA_GFP:
        case VLT_ALG_KPG_ECDSA_GF2M:
            {
                const VLT_KEY_GEN_ECDSA_DSA_DATA *d = &pKeyGenData->data.EcdsaDsaKeyGenObj;

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
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

/* --------------------------------------------------------------------------
 * FILE SYSTEM SERVICES
 * -------------------------------------------------------------------------- */

VLT_STS VltBeginTransaction( void )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_BEGIN_TRANSACTION;
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

VLT_STS VltEndTransaction( void )
{    
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_END_TRANSACTION;
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

VLT_STS VltSelectFileOrDirectory(const VLT_U8 *pu8Path, 
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

VLT_STS VltListFiles(VLT_PU16 pu16ListRespLength,
     VLT_PU8 pu8RespData )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = 0;

    /* validate critical parameters */
    if( ( NULL == pu16ListRespLength) ||
        ( NULL == pu8RespData ) )
    {
        return( ELFNULLPARAM );
    }
    
    if( 0 == *pu16ListRespLength )
    {
        return( ELFIVLDRESPLEN );
    }

    do
    {
        /* Set up the apdu */
        Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL;
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = VLT_INS_LIST_FILES;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;        
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 0;
        /* Fire in the command*/
        status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

        /* Check status and status word */
        if( VLT_OK != status )
        {
            return( status );
        }
        
        if( ( Sw != VLT_STATUS_NONE ) && 
            ( Sw != VLT_STATUS_SUCCESS ) && 
            ( Sw != VLT_STATUS_EOF ) )
        {
            return( Sw );
        }

        /* Adjust the response length */
        Response.u16Len -= VLT_SW_SIZE ; 

        /*
         * Copy the data if we have enough space in the buffer, 
         * otherwise keep accumulating the size of the listing so 
         * it can be reported back to the caller.
         */
        if( ( idx + Response.u16Len ) <= *pu16ListRespLength )
        {
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &pu8RespData[idx], Response.pu8Data, Response.u16Len );
            idx += Response.u16Len;
        }
        else
        {
            idx += Response.u16Len;
        }
    }
    while( Sw != VLT_STATUS_EOF );

    
    /*
     * The size of the listing is larger than the buffer
     * available, return an error code and let the caller
     * know the actual size of the listing.
     */
    if( idx > *pu16ListRespLength )
    {
        status = ELFNOROOM;
    }


    /* 
     * SDVAULTICWRAP-55:
     * Set the size regardless. 
     */
    *pu16ListRespLength = idx;

    return( status );    
}

VLT_STS VltCreateFile(VLT_U8 u8UserID,
    VLT_U32 u32FileSize, 
    const VLT_FILE_PRIVILEGES *pFilePriv,
    VLT_U8 u8FileAttribute,
    VLT_U16 u16FileNameLength,
    const VLT_U8 *pu8FileName )
{ 
    VLT_STS status = VLT_FAIL;        
    VLT_SW Sw = VLT_STATUS_NONE;

    idx = VLT_APDU_DATA_OFFSET;

    /*
    * Check the input paramters for valid pointers
    */
    if( ( NULL == pFilePriv ) ||
        ( NULL == pu8FileName ) )
    {
        return( ECFNULLPARA );
    }

    if( ( VLT_FILENAME_MIN_LEN > u16FileNameLength ) || 
        ( VLT_FILENAME_MAX_LEN < u16FileNameLength ) )
    {
        return( ECFINVLDLEN );
    }

    /*
    * Check that the name passed will fit in the buffer
    */
    if( VltCommsGetMaxSendSize() < u16FileNameLength )
    {
        return( ECFILNMLENTOOBIG );
    }    

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_CREATE_FILE;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8UserID;
    /* P3 is filled out once the data has been built */

    /* dwSize */
    Command.pu8Data[idx++] = (VLT_U8) ((u32FileSize >> 24) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u32FileSize >> 16) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u32FileSize >>  8) & 0xFF);
    Command.pu8Data[idx++] = (VLT_U8) ((u32FileSize >>  0) & 0xFF);
    /* bmAccess */
    Command.pu8Data[idx++] = pFilePriv->u8Read;
    Command.pu8Data[idx++] = pFilePriv->u8Write;
    Command.pu8Data[idx++] = pFilePriv->u8Delete;
    Command.pu8Data[idx++] = pFilePriv->u8Execute;
    /* bmAttributes */
    Command.pu8Data[idx++] = u8FileAttribute;
    /* wNameLength */
    Command.pu8Data[idx++] = (VLT_U8)( ( ( u16FileNameLength ) >> 8 ) & 0xFF );
    Command.pu8Data[idx++] = (VLT_U8)( ( ( u16FileNameLength ) >> 0 ) & 0xFF );
    /* sName */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8FileName, u16FileNameLength );
    idx += u16FileNameLength;

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

VLT_STS VltCreateFolder(VLT_U8 u8UserID,
    const VLT_FILE_PRIVILEGES *pFilePriv,                                   
    VLT_U8 u8FolderAttribute,
    VLT_U16 u16FolderNameLength,
    const VLT_U8 *pu8FolderName)
{ 
    VLT_STS status = VLT_FAIL;
    VLT_SW Sw = VLT_STATUS_NONE;

    idx = VLT_APDU_DATA_OFFSET;

    /*
    * Check the input paramters for valid pointers
    */
    if( ( NULL == pFilePriv ) ||
        ( NULL == pu8FolderName ) )
    {
        return( ECFLNULLPARA );
    }

    if( ( VLT_FILENAME_MIN_LEN > u16FolderNameLength ) || 
        ( VLT_FILENAME_MAX_LEN < u16FolderNameLength ) )
    {
        return( ECFLINVLDLEN );
    }
    
    /*
    * Check that the name passed will fit in the buffer
    */
    if( VltCommsGetMaxSendSize() < u16FolderNameLength )
    {
        return( ECFOLNMLENTOOBIG );
    }

    /* Build APDU */    
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_CREATE_FOLDER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8UserID;
    /* P3 is filled out once the data has been built */

    /* bmAccess */
    Command.pu8Data[idx++] = pFilePriv->u8Read;
    Command.pu8Data[idx++] = pFilePriv->u8Write;
    Command.pu8Data[idx++] = pFilePriv->u8Delete;
    Command.pu8Data[idx++] = pFilePriv->u8Execute;
    /* bmAttributes */
    Command.pu8Data[idx++] = u8FolderAttribute;
    /* wNameLength */
    Command.pu8Data[idx++] = (VLT_U8)( ( ( u16FolderNameLength ) >> 8 ) & 0xFF );
    Command.pu8Data[idx++] = (VLT_U8)( ( ( u16FolderNameLength ) >> 0)  & 0xFF );
    /* sName */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( &Command.pu8Data[idx], pu8FolderName, u16FolderNameLength);
    idx += u16FolderNameLength;

    /* Update P3 now that we know the correct length. */
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 
        LIN( WRAPPED_BYTE( idx - VLT_APDU_TYPICAL_HEADER_SZ ) );

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}


VLT_STS VltDeleteFile( void )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DELETE_FILE;
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

VLT_STS VltDeleteFolder( VLT_U8 u8Recursion )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_DELETE_FOLDER;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8Recursion;
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

VLT_STS VltWriteFile( const VLT_U8 *pu8DataIn,
    VLT_U8 u8DataLength,
    VLT_U8 u8ReclaimSpace )
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
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8ReclaimSpace;
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

#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
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
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
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
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = 0;        
        
        if ( VLT_STATUS_RESPONDING == Sw )
        {
            Command.pu8Data[ VLT_APDU_P2_OFFSET ] = 0;
        }
        else
        {
            /*
             * When the value is 256, the cast to a VLT_U8 will 
             * result in a P2 of 0. P2 = 0 will result in 256 bytes 
             * being received.
             */
            Command.pu8Data[ VLT_APDU_P2_OFFSET ] = (VLT_U8)*pu16ReadLength;
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
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

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

VLT_STS VltSetPrivileges( const VLT_FILE_PRIVILEGES *pFilePriv )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;

    if( NULL == pFilePriv )
    {
        return( ESPVNULLPARA );
    }

    /* Build APDU */    
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL; 
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_PRIVILEGES;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(4);


    /* Build Data In */

    Command.pu8Data[idx++] = pFilePriv->u8Read;
    Command.pu8Data[idx++] = pFilePriv->u8Write;
    Command.pu8Data[idx++] = pFilePriv->u8Delete;
    Command.pu8Data[idx++] = pFilePriv->u8Execute;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltSetAttributes( VLT_U8 u8Attribute )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
    idx = VLT_APDU_DATA_OFFSET;

    /* Build APDU */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_ATTRIBUTES;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LIN(1);

    /* Build Data In */
    Command.pu8Data[idx++] = u8Attribute;

    /* Send the command */
    status = VltCommand( &Command, &Response, idx, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
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
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_GET_INFO;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = LEXP(VLT_TARGET_INFO_LENGTH_WDATE);

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
    if( ( VLT_TARGET_INFO_LENGTH_WDATE != Response.u16Len ) && 
        ( VLT_TARGET_INFO_LENGTH_WODATE != Response.u16Len ) )
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

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    pRespData->u8Mode = Response.pu8Data[idx++];
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

    pRespData->u32Space = VltEndianReadPU32(&Response.pu8Data[idx]);
    idx += 6;    
    pRespData->u8Attack = Response.pu8Data[idx++];
    pRespData->u16AttackCounter = VltEndianReadPU16(&Response.pu8Data[idx]);
    idx += 2;
    
    /*
     * Unpack the date if the date has been returned. Otherwise
     * zero it and return gracefully.
     */
    if( VLT_TARGET_INFO_LENGTH_WDATE == Response.u16Len )
    {
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( pRespData->au8Date, &Response.pu8Data[idx], VLT_FIRMWARE_DATE_LENGTH );
    }
    else
    {
        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memset( pRespData->au8Date, 0x00, VLT_FIRMWARE_DATE_LENGTH );
    }
    
    return( status );
}

VLT_STS VltSelfTest( void )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)*/

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
 
    /*
     * Ensure we have been passed a supported config item.
     */
    if( VLT_ISO8601_DATE_TIME == u8ConfigItem )
    {
        /*
         * Validate the length of the time and date config item
         * the size should also include the null string terminator. 
         */
        if( VLT_ISO8601_DATE_TIME_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDDTLEN );
        }
    }
    else if( VLT_I2C_ADDRESS == u8ConfigItem )
    {
        /*
         * Validate the length of the time and date config item
         * the size should also include the null string terminator. 
         */
        if( VLT_I2C_ADDRESS_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDI2CLEN );
        }
    }
    else if( VLT_USE_AARDVARK_TWI_TIMEOUT == u8ConfigItem )
    {
        /*
         * Validate the length of the Aardvark TWI timeout parameter
         */
        if( VLT_AARDVARK_TWI_TIMEOUT_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDAARLEN );
        }     
    }
    else if( VLT_SPI_POWER_SAVING_MODE == u8ConfigItem )
    {
        /*
         * Validate the length of the Power Saving Mode parameter
         */
        if( VLT_SPI_POWER_SAVING_MODE_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDSPILEN );
        }
    }
	else if( VLT_COMMUNICATION_CHANNEL == u8ConfigItem )
    {
        /*
         * Validate the length of the communication channel parameter
         */
		if( VLT_COMMUNICATION_CHANNEL_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDCOMMLEN );
        }
    }
	else if( VLT_GPIO_ACCESS_MODE == u8ConfigItem )
    {
        /*
         * Validate the length of the GPIO access Mode parameter
         */
		if( VLT_GPIO_ACCESS_MODE_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDGPIOLEN );
        }
    }
	else  if( VLT_USB_MAX_POWER == u8ConfigItem )
	{
		/*
         * Validate the length of the USB max power parameter
         */
		if( VLT_USB_MAX_POWER_SZ != u8DataLength )
        {
            return( ESETCNFGIVLDGPIOLEN );
        }
	}
	else if( (VLT_USB_VENDORID_PRODUCTID_DEVICEID != u8ConfigItem ) 
		&& (VLT_USB_ATTRIBUTES != u8ConfigItem ) 
		&& (VLT_USB_MANUFACTURER_STRING != u8ConfigItem ) 
		&& (VLT_USB_PRODUCT_STRING != u8ConfigItem ) 
		&& (VLT_USB_CONFIG_STRING != u8ConfigItem ) 
		&& (VLT_USB_INTERFACE_STRING != u8ConfigItem ) 
		&& (VLT_USB_SERIAL_NUMBER_STRING != u8ConfigItem ) )
    {
        return( ESETCNFGIVLDITEM );
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

VLT_STS VltSetGpioDirection( VLT_U8 u8GpioDirMask, VLT_U8 u8GpioMode )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_SET_GPIO;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8GpioDirMask;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = u8GpioMode;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 0;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    return( status );
}

VLT_STS VltWriteGpio( VLT_U8 u8GpioValue )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_WRITE_GPIO;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = u8GpioValue;
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

VLT_STS VltReadGpio( VLT_PU8 pu8GpioValue )
{
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;
     
    /* validate critical parameters*/
    if( NULL == pu8GpioValue )
    {
        return( ERDGPIONULLPARAM );
    }

    /* build the apdu */
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
    Command.pu8Data[VLT_APDU_INS_OFFSET] = VLT_INS_READ_GPIO;
    Command.pu8Data[VLT_APDU_P1_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P2_OFFSET] = 0;
    Command.pu8Data[VLT_APDU_P3_OFFSET] = 1;

    /* Send the command */
    status = VltCommand( &Command, &Response, VLT_APDU_DATA_OFFSET, 0, &Sw );

    if( ( Sw != VLT_STATUS_NONE ) && ( Sw != VLT_STATUS_SUCCESS ) )
    {
        return( Sw );
    }

    if( VLT_OK != status )
    {
        return( status );
    }

    *pu8GpioValue = Response.pu8Data[0];

    return( status );
}

VLT_STS VltTestCase1( void )
{ 
    VLT_SW Sw = VLT_STATUS_NONE;
    VLT_STS status = VLT_FAIL;

    /* build the apdu */
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */
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
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
#endif /*( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

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
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X ) */

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

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
#if( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_LEGACY;
#endif /* ( VAULT_IC_VERSION == VAULTIC_VERSION_1_0_X )*/ 

#if( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X)
    Command.pu8Data[VLT_APDU_CLASS_OFFSET] = VLT_CLA_NO_CHANNEL;
#endif /* ( (VAULT_IC_VERSION & VAULTIC_VERSION_1_2_X) == VAULTIC_VERSION_1_2_X) */

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
VLT_STS VltSelectCard( SCARDHANDLE hScard,  SCARDCONTEXT hCxt, DWORD dwProtocol)
{
	return( VltCommsSelectCard(hScard,hCxt,dwProtocol) );
}
#endif