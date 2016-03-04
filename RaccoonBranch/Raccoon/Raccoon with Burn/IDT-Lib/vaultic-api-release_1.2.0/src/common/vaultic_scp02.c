/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_scp02.c
 * 
 * \brief Implemetation of SCP02.
 * 
 * \par Description:
 * TBD.
 */

#include "vaultic_common.h"
#include "vaultic_secure_channel.h"
#include "vaultic_api.h"
#include "vaultic_mem.h"
#include "vaultic_cipher.h"
#include "vaultic_symmetric_signer.h"
#include "vaultic_apdu.h"
#include "vaultic_scp02.h"


/*
* Defines
*/

#define SCP02_SESSION_KEY_LEN (VLT_U8)0x10
#define DES_INIT_VECT_LEN     (VLT_U8)0x08
#define CRYPTOGRAM_MESS_LEN   (VLT_U8)0x10
#define SCP02_CMAC_RMAC_LEN   (VLT_U8)0x08
#define SCP02_CENC_LEN        (VLT_U8)0x08

#define CALC_CMAC             (VLT_U8)0x00
#define CALC_RMAC             (VLT_U8)0x01

#define SW_INVALID_MAC_HIGH   (VLT_U8)0x69
#define SW_INVALID_MAC_LOW    (VLT_U8)0x88

#define SCP02_ENC_MAC_OVHD    (VLT_U8)0x18

#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    /*
    * External Variables
    */
    extern VLT_U8 u8SecureChannelLevel; /* Declared in vaultic_secure_channel.c */
    extern VLT_U8 u8SecureChannelState; /* Declared in vaultic_secure_channel.c */

    extern VLT_U8 au8CMacKey[SCPXX_MAX_SESSION_KEY_LEN]; /* Declared in vaultic_secure_channel.c */
    extern VLT_U8 au8RMacKey[SCPXX_MAX_SESSION_KEY_LEN]; /* Declared in vaultic_secure_channel.c */
    extern VLT_U8 au8CEncKey[SCPXX_MAX_SESSION_KEY_LEN]; /* Declared in vaultic_secure_channel.c */
    
    extern VLT_U8 au8CMac[SCPXX_MAX_CMAC_LEN]; /* Declared in vaultic_secure_channel.c */
    extern VLT_U8 au8RMac[SCPXX_MAX_RMAC_LEN]; /* Declared in vaultic_secure_channel.c */

    /*
    * Private Data
    */
    static VLT_U8 au8CMacIV[DES_INIT_VECT_LEN];
    static VLT_U8 au8RMacIV[DES_INIT_VECT_LEN];

    /*
    * Static methods
    */
    static void ResetChannel( void );

    static VLT_STS DeriveSessionKeys(VLT_U16 u16SeqCnt, 
        KEY_BLOB* pSMac, KEY_BLOB* pSEnc );

    static VLT_STS TDesEdeCbc(VLT_PU8 pu8SessionKey, 
        VLT_U16 initVal, 
        VLT_U16 u16SeqCnt, 
        KEY_BLOB* pStaticKey);

    static VLT_STS Iso9797MacAlgo1(VLT_PU8 pu8Message, VLT_PU8 pu8AuthCrypto);

    static VLT_STS UpdateCMacIV(VLT_PU8 pu8Mac, VLT_PU8 pu8MacIV);

    static VLT_STS CalculateMac( VLT_MEM_BLOB *pCmd, VLT_U8 u8MacMode );

    static VLT_STS EncryptCommandData(VLT_MEM_BLOB* pCmd);

    static VLT_STS DecryptResponseData(VLT_MEM_BLOB* pRsp);
#endif

VLT_STS VltScp02Init( VLT_U8 u8ChannelLevel, 
    KEY_BLOB* pSMac, 
    KEY_BLOB* pSEnc, 
    VLT_PU8 pu8HostChal, 
    VLT_U8 u8HostChalLen, 
    VLT_INIT_UPDATE* pInitUpRsp )
{
#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U8 au8AuthCryptoDevice[VLT_SCPXX_CRYPTOGRAM_LENGTH];
    VLT_U8 au8AuthCryptoHost[VLT_SCPXX_CRYPTOGRAM_LENGTH];
    VLT_U8 au8MessageBuf[CRYPTOGRAM_MESS_LEN];
    VLT_PU8 pMessagePos = 0;

    /*
    * Check that the input parameters are valid
    */
    if( ( NULL == pSMac ) || 
        ( NULL == pSEnc ) || 
        ( NULL == pSMac->keyValue) || 
        ( NULL == pSEnc->keyValue) ||
        ( NULL == pu8HostChal ) ||
        ( NULL == pInitUpRsp ) )
    {
        return ESCP02INITNULLPARAM;
    }

    /*
    * Check the validity of the SMAC Key
    */
    if( ( VLT_KEY_TDES_2K != pSMac->keyType ) ||
        ( TDES_KEY_SIZE != pSMac->keySize) )
    {
        return ESCP02INITINVLDSMAC;
    }

    /*
    * Check the validity of the SENC Key
    */
    if( ( VLT_KEY_TDES_2K != pSEnc->keyType ) ||
        ( TDES_KEY_SIZE != pSEnc->keySize) )
    {
        return ESCP02INITINVLDSENC;
    }

    /*
    * Reset the channel 
    */
    ResetChannel( );

    /*
    * Derive Session Keys
    */
    status = DeriveSessionKeys(pInitUpRsp->data.Scp02.u16SequenceCounter, pSMac, pSEnc );

    if( VLT_OK == status )
    {
        /*
        * Build the message to calculate the Authentication Crytopgram for the
        * Device - Format Ch || SC || Cd
        */
        pMessagePos = &au8MessageBuf[0];

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(pMessagePos, pu8HostChal, u8HostChalLen);

        pMessagePos += u8HostChalLen;

        *(pMessagePos++) = 
            (VLT_U8)( ( pInitUpRsp->data.Scp02.u16SequenceCounter >> 8 ) & 0xFF );

        *(pMessagePos++) = 
            (VLT_U8)( ( pInitUpRsp->data.Scp02.u16SequenceCounter ) & 0xFF );

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(pMessagePos, 
            &(pInitUpRsp->data.Scp02.au8DeviceChallenge[0]), 
            sizeof( pInitUpRsp->data.Scp02.au8DeviceChallenge ) );  

        /*
        * Compute Device Cryptogram ACd
        */
        status = Iso9797MacAlgo1(&au8MessageBuf[0], &au8AuthCryptoDevice[0]);       
    }

    if( VLT_OK == status )
    {
        /*
        * Compare the computed and recieved values
        */
        if( 0 != host_memcmp( au8AuthCryptoDevice, 
            pInitUpRsp->data.Scp02.au8Cryptogram, 
            VLT_SCPXX_CRYPTOGRAM_LENGTH ) )
        {
            /*
            * Invalid Device Authentication Cryptogram
            */
            status = ESCP02INVLDDEVCRYPTO;
        }
    }

    if( VLT_OK == status )
    {
        /*
        * Build the message to calculate the Authentication Crytopgram for the
        * Host - Format SC || Cd || Ch
        */
        pMessagePos = &au8MessageBuf[0]; 

        *(pMessagePos++) = 
            (VLT_U8)( ( pInitUpRsp->data.Scp02.u16SequenceCounter >> 8 ) & 0xFF );

        *(pMessagePos++) = 
            (VLT_U8)( ( pInitUpRsp->data.Scp02.u16SequenceCounter ) & 0xFF );

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(pMessagePos, 
            &(pInitUpRsp->data.Scp02.au8DeviceChallenge[0]), 
            sizeof( pInitUpRsp->data.Scp02.au8DeviceChallenge ) );

        pMessagePos += sizeof( pInitUpRsp->data.Scp02.au8DeviceChallenge );

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy(pMessagePos, pu8HostChal, u8HostChalLen); 

        /*
        * Compute Host Cryptogram ACh
        */
        status = Iso9797MacAlgo1(&au8MessageBuf[0], &au8AuthCryptoHost[0]);

    }

    if(VLT_OK == status)
    {
        /*
        * External Authenticate
        */
        status = VltExternalAuthenticate(VLT_LOGIN_SCP02,
            u8ChannelLevel,
            VLT_SCPXX_CRYPTOGRAM_LENGTH,
            &au8AuthCryptoHost[0] );
    
        /*
        * Check that the call to External Authenticate was successful
        */
        if( VLT_OK != status )
        {
            status = ESCP02EXTAUTHFAIL;
        }    
    }

    /*
    * If External Authenticate sent back status success update the channel
    * level as it has now been established
    */
    if ( VLT_OK == status )
    {
        u8SecureChannelLevel = u8ChannelLevel;
        u8SecureChannelState = VLT_USER_AUTHENTICATED;
    }


    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltScp02Close( void )
{
#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    status = VltCancelAuthentication();

    ResetChannel();

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltScp02Wrap( VLT_MEM_BLOB *pCmd )
{
#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Check that we were passed a valid command buffer
    */
    if( ( NULL == pCmd ) || ( NULL == pCmd->pu8Data ) )
    {
        return ESCP02WRAPNULLPARAM;
    }

    /*
    * If no channel is established don't add anything
    */

    if( VLT_NO_CHANNEL == u8SecureChannelLevel)
    {
        /*
        * The exception to this rule is if External Authenticate is being
        * sent to setup a Secure Channel.  If so it needs to have a C-Mac
        * appended
        */
        if( VLT_INS_EXTERNAL_AUTHENTICATE_SCP != 
            pCmd->pu8Data[VLT_APDU_INS_OFFSET] )
        {
            /*
            * Any other command should be sent as is
            */
            status = VLT_OK;
        }
    }
    else
    {
        /*
        * A secure channel has been established.  Check if the command is 
        * Get Response.  If it is don't add any C-MAC or C-ENC
        */
        if( VLT_INS_GET_RESPONSE == pCmd->pu8Data[VLT_APDU_INS_OFFSET])
        {
            status = VLT_OK;
        }
    }
    /*
    * If the status value still has the initial value of VLT_FAIL
    * add the appropriate secure channel data
    */
    if ( VLT_FAIL == status )
    {
        /*
        * If Secure Channel is established with C-MAC or above change the class
        * byte
        */
        pCmd->pu8Data[VLT_APDU_CLASS_OFFSET] |= VLT_CLA_SCP_BIT; 

        /*
        * All channel levels other than NO CHANNEL add a CMAC
        */
        status = CalculateMac( pCmd, CALC_CMAC );

        if( VLT_OK == status )
        {
            /*
            * Check if the channel level requires the data to be encrypted
            */
            if( ( VLT_CMAC_CENC == u8SecureChannelLevel ) ||
                ( VLT_CMAC_CENC_RMAC == u8SecureChannelLevel ) || 
                ( VLT_CMAC_CENC_RMAC_RENC == u8SecureChannelLevel ) )
            {
                /*
                * Encrypt the data if there is data to encrypt
                */
                if( VLT_HEADER_SIZE < pCmd->u16Len)
                {
                    status = EncryptCommandData( pCmd );
                }
            }
        }

        if( VLT_OK == status)
        {
            /*
            * Add the C-Mac to the command buffer and update the P3 value
            */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &(pCmd->pu8Data[pCmd->u16Len]), 
                &au8CMac[0], 
                SCP02_CMAC_RMAC_LEN);
            /*
            * P3 value was already adjusted for the C-MAC calculation
            */
            pCmd->u16Len += SCP02_CMAC_RMAC_LEN;
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltScp02Unwrap( VLT_MEM_BLOB *pRsp )
{
#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_PU8 pu8RMac = 0;
    VLT_PU8 pu8SW = 0;
    VLT_U8 au8RMacRcvd[SCP02_CMAC_RMAC_LEN];
    

    /*
    * Check that we were passed a valid response buffer
    */
    if( ( NULL == pRsp ) || ( NULL == pRsp->pu8Data ) )
    {
        return ESCP02UNWRAPNULLPARAM;
    }

    /*
    * If no channel is established don't remove anything
    */
    if( VLT_NO_CHANNEL == u8SecureChannelLevel )
    {
        status = VLT_OK;
    }
    else
    {
        if( VLT_SW_SIZE == pRsp->u16Len)
        {
            /*
            * Only a Status Word is present in the response data so don't 
            * attempt to unwrap the data
            */
            if( ( SW_INVALID_MAC_HIGH == pRsp->pu8Data[0] ) &&
                ( SW_INVALID_MAC_LOW == pRsp->pu8Data[1] ) )
            {
                /*
                * Close the secure channel. 
                */
                VltScp02Close();
                status = ESCP02INVLDCMAC;
            }
            else
            {
                status = VLT_OK;
            }
        }
        else
        {
            /*
            * Check if the channel level specifies if an R-MAC should be present
            */
            if( ( VLT_CMAC_RMAC == u8SecureChannelLevel ) ||
                ( VLT_CMAC_CENC_RMAC == u8SecureChannelLevel ) || 
                ( VLT_CMAC_CENC_RMAC_RENC == u8SecureChannelLevel) )
            {
                /*
                * A R-MAC will be present at the end of the data, but before
                * the status word.  Take a copy of it and move the status word
                * to the end of the data.  Also adjust the length of the buffer
                * to reflect this
                */
                pu8SW = &(pRsp->pu8Data[pRsp->u16Len - VLT_SW_SIZE]);
                pu8RMac = pu8SW - SCP02_CMAC_RMAC_LEN;
                /*
                * No need to check the return type as pointer has been validated
                */
                (void)host_memcpy( &au8RMacRcvd[0], pu8RMac, SCP02_CMAC_RMAC_LEN );
                (void)host_memcpy(pu8RMac, pu8SW, VLT_SW_SIZE );
                pRsp->u16Len -= SCP02_CMAC_RMAC_LEN;
            }

            /*
            * Check if the channel level requires the data to be decrypted
            */
            if( VLT_CMAC_CENC_RMAC_RENC == u8SecureChannelLevel )
            {
                /*
                * If only a status word is present there is no data to decrypt
                */
                if(VLT_SW_SIZE < pRsp->u16Len)
                {
                    status = DecryptResponseData( pRsp );

                    if(VLT_OK == status)
                    {
                        /* Put the status word at the end of the decrypted
                        * data and adjust the size to include it
                        */
                        /*
                        * No need to check the return type as pointer has been validated
                        */
                        (void)host_memcpy( &(pRsp->pu8Data[pRsp->u16Len]),
                            pu8SW, 
                            VLT_SW_SIZE);
                        pRsp->u16Len += VLT_SW_SIZE;

                    }
                }
                else
                {
                    status = VLT_OK;
                }

                if( VLT_OK != status )
                {
                    /*
                    * Close the channel as decryption failed
                    */
                    VltScp02Close();
                }
            }
            else
            {
                status = VLT_OK;
            }
            
            if(VLT_OK == status)
            {
                if( ( VLT_CMAC_RMAC == u8SecureChannelLevel ) ||
                    ( VLT_CMAC_CENC_RMAC == u8SecureChannelLevel ) || 
                    ( VLT_CMAC_CENC_RMAC_RENC == u8SecureChannelLevel) )
                {
                    /*
                    * Check that the R-MAC is valid
                    */
                    status = CalculateMac( pRsp, CALC_RMAC );

                    if(VLT_OK == status)
                    {
                        /*
                        * Check that the R-MAC is as expected
                        */
                        if( 0 != 
                            host_memcmp( &au8RMac[0],
                            &au8RMacRcvd[0],
                            SCP02_CMAC_RMAC_LEN ))
                        {
                            status = ESCP02INVLDRMAC;

                            /*
                            * Close the channel as the R-MAC received doesn't
                            * match that calculated
                            */
                            VltScp02Close();
                        }
                    }
                }
            }
        }
    }
    
    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

VLT_STS VltScp02GetChannelOverhead( VLT_U8 u8Mode, VLT_PU8 pu8Overhead )
{
#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
    /*
    * Check the input paramters are valid
    */
    if( NULL == pu8Overhead )
    {
        return ESCP02GETOVHDNULLPARAM;
    }

    if( ( SECURE_CHANNEL_SEND != u8Mode ) && 
        ( SECURE_CHANNEL_RECEIVE != u8Mode ) )
    {
        return ESCP02GETOVHDINVLDMODE;
    }
    /*
    * The following values have been altered to match the current behaviour of
    * the firmware.  The commented values are what are believed to be the 
    * correct values.  This is documented within JIRA issue SDAT98FW-277
    */
    switch( u8SecureChannelLevel )
    {
        /*
        * No channel means no extra overhead
        */
    case VLT_NO_CHANNEL:
        *pu8Overhead = 0;
        break;

        /*
        * CMAC
        */
    case VLT_CMAC:
        if( SECURE_CHANNEL_SEND == u8Mode)
        {
            *pu8Overhead = SCP02_CMAC_RMAC_LEN;
        }
        else
        {
            *pu8Overhead = 0;
        }
        break;

        /*
        * CMAC RMAC
        */
    case VLT_CMAC_RMAC:
        *pu8Overhead = SCP02_CMAC_RMAC_LEN;
        break;

        /*
        * CMAC and CENC
        */
    case VLT_CMAC_CENC:
        if( SECURE_CHANNEL_SEND == u8Mode)
        {
            /**pu8Overhead = SCP02_CENC_LEN + SCP02_CMAC_RMAC_LEN + 1;*/
            *pu8Overhead = SCP02_ENC_MAC_OVHD;
        }
        else
        {
            *pu8Overhead = 0;
        }
        break;

        /*
        * CMAC CENC RMAC
        */
    case VLT_CMAC_CENC_RMAC:
        if( SECURE_CHANNEL_SEND == u8Mode)
        {
            /**pu8Overhead = SCP02_CENC_LEN + SCP02_CMAC_RMAC_LEN + 1;*/
            *pu8Overhead = SCP02_ENC_MAC_OVHD;
        }
        else
        {
            *pu8Overhead = SCP02_CMAC_RMAC_LEN;
        }
        break;

        /*
        * CMAC CENC RMAC RENC
        */
    case VLT_CMAC_CENC_RMAC_RENC:
        /**pu8Overhead = SCP02_CENC_LEN + SCP02_CMAC_RMAC_LEN + 1;*/
        *pu8Overhead = SCP02_ENC_MAC_OVHD;
        break;

        /*
        * Should never get here as the u8SecureChannelLevel value is private
        * data and should only ever get one of the above values
        */
    default:
        break;
    }

    return( VLT_OK );
#else
    return( EMETHODNOTSUPPORTED );
#endif
}

#if( VLT_ENABLE_SCP02 == VLT_ENABLE )
static void ResetChannel( void )
{
    /*
    * Clear the Session Keys
    */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memset(&au8CMacKey[0], 0x00, SCP02_SESSION_KEY_LEN);
    (void)host_memset(&au8RMacKey[0], 0x00, SCP02_SESSION_KEY_LEN);
    (void)host_memset(&au8CEncKey[0], 0x00, SCP02_SESSION_KEY_LEN);

    /*
    * Reinitialise the MAC IV's
    */
    (void)host_memset( &au8CMacIV[0], 0x00, DES_INIT_VECT_LEN);
    (void)host_memset( &au8RMacIV[0], 0x00, DES_INIT_VECT_LEN);

    /*
    * Clear the currently stored MAC
    */
    (void)host_memset( &au8CMac[0], 0x00, SCP02_CMAC_RMAC_LEN );

    /*
    * Reset the channel level to No Channel
    */ 
    u8SecureChannelLevel = VLT_NO_CHANNEL;

    /*
    * Reset the state of the channel to not established
    */
    u8SecureChannelState = VLT_USER_NOT_AUTHENTICATED;
}

static VLT_STS DeriveSessionKeys(VLT_U16 u16SeqCnt, 
    KEY_BLOB* pSMac, 
    KEY_BLOB* pSEnc )
{
    static const VLT_U16 CMAC_INIT_VAL = 0x0101;
    static const VLT_U16 RMAC_INIT_VAL = 0x0102;
    static const VLT_U16 CENC_INIT_VAL = 0x0182;

    VLT_STS status = VLT_FAIL;

    /*
    * Check that the static keys passed in are valid
    */
    if( ( NULL == pSMac ) || ( NULL == pSEnc ) )
    {
        return ESCP02DERSESSKEYNULLPARAM;
    }
    
    /*
    * Calculate the C-MAC Key
    */
    status = TDesEdeCbc(&au8CMacKey[0], 
        CMAC_INIT_VAL,
        u16SeqCnt, 
        pSMac);
    
    /*
    * Calculate the R-MAC Key
    */
    if(VLT_OK == status)
    {        
        status = TDesEdeCbc(&au8RMacKey[0], 
            RMAC_INIT_VAL, 
            u16SeqCnt, 
            pSMac);
    }
 
    /*
    * Calculate the C-ENC Key
    */
    if(VLT_OK == status)
    { 
        status = TDesEdeCbc(&au8CEncKey[0], 
            CENC_INIT_VAL, 
            u16SeqCnt, 
            pSEnc);
    }
    
    return( status );
}

static VLT_STS TDesEdeCbc(VLT_PU8 pu8SessionKey, 
    VLT_U16 initVal, 
    VLT_U16 u16SeqCnt, 
    KEY_BLOB* pStaticKey)
{
    VLT_STS status = VLT_FAIL;
    CIPHER_PARAMS cipherParams;
    KEY_BLOB key;
    VLT_U8 au8IV[DES_INIT_VECT_LEN];
    VLT_U32 u32DataLen = 0;
    VLT_U8 u8SessKeyPos = 0;

    /*
    * Check the input parameters are valid
    */
    if( ( NULL == pu8SessionKey ) || ( NULL == pStaticKey ) )
    {
        return ESCP02TDESEDECBCNP;
    }

    /*
    * Initialise the IV to all zero
    */
    (void)host_memset( &au8IV[0], 0x00, DES_INIT_VECT_LEN );

    /*
    * Setup the message for Encrypt 3DES-EDE Algorithm in CBC Mode
    * Use buffer that the result is to be placed in to save on memory
    */
    (void)host_memset( pu8SessionKey, 0x00, SCP02_SESSION_KEY_LEN );

    /*
    * Add the initial value
    */
    pu8SessionKey[u8SessKeyPos++] = (VLT_U8)( ( initVal >> 8 ) & 0xFF );
    pu8SessionKey[u8SessKeyPos++] = (VLT_U8)( initVal & 0xFF );

    /*
    * Add the sequence count
    */
    pu8SessionKey[u8SessKeyPos++] = (VLT_U8)( ( u16SeqCnt >> 8 ) & 0xFF );
    pu8SessionKey[u8SessKeyPos++] = (VLT_U8)( u16SeqCnt & 0xFF );

    /*
    * Setup the Key to be passed for the Cipher
    */
    key.keySize = TDES_KEY_SIZE;
    key.keyType = VLT_KEY_TDES_2K;
    key.keyValue = pStaticKey->keyValue; 

    /*
    * Setup the Cipher Paramters to be
    * 3DES 2K EDE using CBC Mode with a NULL IV and no padding
    */
    cipherParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
    cipherParams.chainMode = BLOCK_MODE_CBC;
    cipherParams.pIV = &au8IV[0]; 
    cipherParams.paddingScheme = PADDING_NONE;

    /*
    * Initialise the Cipher with the correct key and parameters
    */ 
    status = CipherInit( VLT_ENCRYPT_MODE, &key, (VLT_PU8)&cipherParams );
    
    if(VLT_OK == status)
    {
        /*
        * Cipher the data
        */
        status = CipherDoFinal(pu8SessionKey, 
            SCP02_SESSION_KEY_LEN, 
            SCP02_SESSION_KEY_LEN,
            pu8SessionKey,
            &u32DataLen,
            SCP02_SESSION_KEY_LEN); 
    }

    /*
     * Close the Cipher
     */
    if(VLT_OK != status)
    {
        CipherClose();
    }
    else
    {
        status = CipherClose();
    }

    return ( status );
}

static VLT_STS Iso9797MacAlgo1(VLT_PU8 pu8Message, VLT_PU8 pu8AuthCrypto)
{
    VLT_STS status = VLT_FAIL;
    VLT_U8 au8IV[DES_INIT_VECT_LEN];
    KEY_BLOB key;
    SIGNER_PARAMS signerParams;
    VLT_U32 u32SignedLen = 0;

    /*
    * Check the input parameters
    */
    if( ( NULL == pu8Message ) || ( NULL == pu8AuthCrypto ) )
    {
        return ESCP02ISO9797MACALGO1;
    }

    /*
    * Initialise the IV to all zero
    */
    (void)host_memset( &au8IV[0], 0x00, DES_INIT_VECT_LEN );

    /*
    * Setup the Key to be passed for the Signer
    */
    key.keySize = TDES_KEY_SIZE;
    key.keyType = VLT_KEY_TDES_2K;
    key.keyValue = &au8CEncKey[0];

    /*
    * Setup the Signer Paramters to be
    * 3DES 2K EDE using CBC Mode with a NULL IV and padding method 2
    */
    signerParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG1;
    signerParams.pIV = &au8IV[0];
    signerParams.ivSize = DES_INIT_VECT_LEN;
    signerParams.paddingScheme = PADDING_ISO9797_METHOD2;

    /*
    * Initialise the signer with the appropriate key and parameters
    */
    status = SymmetricSignerInit( VLT_SIGN_MODE, &key, (VLT_PU8)&signerParams );
    
    /*
    * Sign the data
    */
    if(VLT_OK == status)
    {
        status = SymmetricSignerDoFinal(pu8Message, 
            CRYPTOGRAM_MESS_LEN,
            CRYPTOGRAM_MESS_LEN,
            pu8AuthCrypto,
            &u32SignedLen,
            VLT_SCPXX_CRYPTOGRAM_LENGTH);
    }

    /*
     * Close the Signer
     */
    if(VLT_OK != status)
    {
        SymmetricSignerClose();
    }
    else
    {
        status = SymmetricSignerClose();
    }

    return ( status );
}

static VLT_STS UpdateCMacIV(VLT_PU8 pu8Mac, VLT_PU8 pu8MacIV)
{
    VLT_STS status = VLT_FAIL;
    CIPHER_PARAMS cipherParams;
    KEY_BLOB key;
    VLT_U32 u32DataLen = 0;

    /*
    * Check the input parameter is valid
    */
    if( NULL == pu8Mac )
    {
        return ESCP02UPDTCMACIV;
    }

    /*
    * Setup the Key to be passed for the Cipher
    */
    key.keySize = DES_KEY_SIZE;
    key.keyType = VLT_KEY_DES;
    key.keyValue = &au8CMacKey[0]; 

    /*
    * Setup the Cipher Paramters to be
    * 3DES 2K EDE using CBC Mode with a NULL IV and no padding
    */
    cipherParams.algoID = VLT_ALG_CIP_DES;
    cipherParams.chainMode = BLOCK_MODE_ECB;
    cipherParams.pIV = pu8MacIV; 
    cipherParams.paddingScheme = PADDING_NONE;

    /*
    * Initialise the cipher with the appropriate key and parameters
    */ 
    status = CipherInit( VLT_ENCRYPT_MODE, &key, (VLT_PU8)&cipherParams );
    
    if(VLT_OK == status)
    {
        status = CipherDoFinal(pu8Mac, 
            SCP02_CMAC_RMAC_LEN, 
            SCP02_CMAC_RMAC_LEN,
            pu8MacIV,
            &u32DataLen,
            DES_INIT_VECT_LEN); 
    }    

     /*
     * Close the Cipher
     */
    if(VLT_OK != status)
    {
        CipherClose();
    }
    else
    {
        status = CipherClose();
    }

    return ( status );
}

static VLT_STS CalculateMac( VLT_MEM_BLOB *pCmd, VLT_U8 u8MacMode )
{
    VLT_STS status = VLT_FAIL;
    KEY_BLOB key;
    SIGNER_PARAMS signerParams;
    VLT_U32 u32SignedLen = 0;
    VLT_PU8 pu8MacIv = 0;
    VLT_PU8 pu8Mac = 0;

    /*
    * Check the input pointer is valid
    */
    if( NULL == pCmd )
    {
        return ESCP02ADDMACNULLPARAM;
    }

    /*
    * Check that a valid mode has been selected
    */
    if( CALC_RMAC < u8MacMode )
    {
        return ESCP02CALCMACINVLDMODE;
    }

    if( CALC_CMAC == u8MacMode)
    {
        pu8Mac = &au8CMac[0];
        pu8MacIv = &au8CMacIV[0];
        key.keyValue = &au8CMacKey[0];
    }
    else
    {
        pu8Mac = &au8RMac[0];
        pu8MacIv = &au8RMacIV[0];
        key.keyValue = &au8RMacKey[0];
    }

    /*
    * Setup the Key to be passed for the Signer
    */
    key.keySize = TDES_KEY_SIZE;
    key.keyType = VLT_KEY_TDES_2K;
    /* Key value setup above */   

    /*
    * Setup the Signer Paramters to be
    * 3DES 2K EDE using CBC Mode with a NULL IV and padding method 2
    */
    signerParams.algoID = VLT_ALG_SIG_MAC_ISO9797_ALG3;
    signerParams.pIV = pu8MacIv;
    signerParams.ivSize = DES_INIT_VECT_LEN;
    signerParams.paddingScheme = PADDING_ISO9797_METHOD2;

    /*
    * Initialise the Signer
    */
    status = SymmetricSignerInit( VLT_SIGN_MODE, &key, (VLT_PU8)&signerParams );
    if(VLT_OK == status)
    {
        if(u8MacMode == CALC_CMAC)
        {
            /*
            * Update the P3 value to include the C-MAC length
            */
            if( VLT_HEADER_SIZE == pCmd->u16Len )
            {
                /*
                * If only the header bytes are present make the P3 value
                * equal to the length of the CMAC
                */
                pCmd->pu8Data[VLT_APDU_P3_OFFSET] = SCP02_CMAC_RMAC_LEN;
            }
            else
            {
                /*
                * Further data is present so adjust the P3 value to add
                * the length of the CMAC to the current P3 value
                */
                pCmd->pu8Data[VLT_APDU_P3_OFFSET] += SCP02_CMAC_RMAC_LEN;
            }

        }
        else
        {
            /*
            * Use the C-MAC IV as part of the calculation
            */            
            status = SymmetricSignerUpdate( &au8CMacIV[0], 
                SCP02_CMAC_RMAC_LEN, 
                SCP02_CMAC_RMAC_LEN );
        }        
    }

    if( VLT_OK == status)
    {
        /*
        * Calculate the MAC
        */
        status = SymmetricSignerDoFinal( &(pCmd->pu8Data[0]), 
            pCmd->u16Len,
            pCmd->u16Capacity,
            pu8Mac,
            &u32SignedLen,
            SCP02_CMAC_RMAC_LEN);
    }

    if( VLT_OK == status)
    {
        /*
        * Update the MAC IV
        */
        if(u8MacMode == CALC_CMAC)
        {
            status = UpdateCMacIV(pu8Mac, pu8MacIv);
        }
        else
        {
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( pu8MacIv, pu8Mac, SCP02_CMAC_RMAC_LEN );
        }
    }  

    return( status );
}

static VLT_STS EncryptCommandData(VLT_MEM_BLOB* pCmd)
{
    VLT_STS status = VLT_FAIL;
    CIPHER_PARAMS cipherParams;
    KEY_BLOB key;
    VLT_U32 u32DataLen = 0;
    VLT_U8 au8IV[DES_INIT_VECT_LEN];

    /*
    * Check that we have been passed a valid command
    */
    if( NULL == pCmd )
    {
        return ESCP02ENCCMDDATANULLPARAM;
    }

    /*
    * Zero the Initialisation Vector
    */
    (void)host_memset( &au8IV[0], 0x00, DES_INIT_VECT_LEN ); 

    /*
    * Setup the Key to be passed for the Cipher
    */
    key.keySize = TDES_KEY_SIZE;
    key.keyType = VLT_KEY_TDES_2K;
    key.keyValue = &au8CEncKey[0]; 

    /*
    * Setup the Cipher Paramters to be
    * 3DES 2K EDE using CBC Mode with a NULL IV and Padding Method 2
    */
    cipherParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
    cipherParams.chainMode = BLOCK_MODE_CBC;
    cipherParams.pIV = &au8IV[0]; 
    cipherParams.paddingScheme = PADDING_ISO9797_METHOD2;

    /*
    * Initialise the cipher with the appropriate key and parameters
    */ 
    status = CipherInit( VLT_ENCRYPT_MODE, &key, (VLT_PU8)&cipherParams );
    
    if(VLT_OK == status)
    {
        /*
        * Encrpyt the data portion of the command.  Data Length is the P3 value
        * is the length of  a C-Mac which has already been added
        */
        status = CipherDoFinal( &(pCmd->pu8Data[VLT_APDU_DATA_OFFSET]),
            pCmd->u16Len - VLT_HEADER_SIZE,
            VLT_MAX_APDU_SND_DATA_SZ,
            &(pCmd->pu8Data[VLT_APDU_DATA_OFFSET]),
            &u32DataLen,
            VLT_MAX_APDU_SND_DATA_SZ );

        if( VLT_OK == status )
        {
            /*
            * Update the P3 to include the length of the C-Enc
            * Need to cast as the return is a VLT_U32 and P3 is VLT_U8
            */
            pCmd->pu8Data[VLT_APDU_P3_OFFSET] = 
                (VLT_U8)u32DataLen + SCP02_CMAC_RMAC_LEN;

            /*
            * Update the length to that of the encypted data.  P3
            * includes the C-MAC lengh as that was used to calculate
            * the C-MAC but the C-MAC hasn't been added to the buffer
            * yet.  Include the header size as well
            */
            pCmd->u16Len = (VLT_U8)u32DataLen + VLT_HEADER_SIZE;
        }
    }    

    /*
    * Close the Cipher
    */
    if(VLT_OK != status)
    {
        CipherClose();
    }
    else
    {
        status = CipherClose();
    }

    return ( status );
}

static VLT_STS DecryptResponseData(VLT_MEM_BLOB* pRsp)
{
    VLT_STS status = VLT_FAIL;
    CIPHER_PARAMS cipherParams;
    KEY_BLOB key;
    VLT_U32 u32DataLen = 0;
    VLT_U8 au8IV[DES_INIT_VECT_LEN];

    /*
    * Check that we have been passed a valid response
    */
    if( NULL == pRsp )
    {
        return ESCP02DECCMDDATANULLPARAM;
    }

    /*
    * Zero the Initialisation Vector
    */
    (void)host_memset( &au8IV[0], 0x00, DES_INIT_VECT_LEN ); 

    /*
    * Setup the Key to be passed for the Cipher
    */
    key.keySize = TDES_KEY_SIZE;
    key.keyType = VLT_KEY_TDES_2K;
    key.keyValue = &au8CEncKey[0]; 

    /*
    * Setup the Cipher Paramters to be
    * 3DES 2K EDE using CBC Mode with a NULL IV and Padding Method 2
    */
    cipherParams.algoID = VLT_ALG_CIP_TDES_2K_EDE;
    cipherParams.chainMode = BLOCK_MODE_CBC;
    cipherParams.pIV = &au8IV[0]; 
    cipherParams.paddingScheme = PADDING_ISO9797_METHOD2;

    /*
    * Initialise the cipher with the appropriate key and parameters
    */ 
    status = CipherInit( VLT_DECRYPT_MODE, &key, (VLT_PU8)&cipherParams );
    
    if(VLT_OK == status)
    {
        /*
        * Decrpyt the data portion of the command.  If channel has encrypted
        * data there will be a R-MAC at the end of it so don't include that
        * in the decryption
        */
        status = CipherDoFinal( &(pRsp->pu8Data[0]), 
            pRsp->u16Len - VLT_SW_SIZE, 
            VLT_MAX_APDU_RCV_DATA_SZ,
            &(pRsp->pu8Data[0]),
            &u32DataLen,
            VLT_MAX_APDU_SND_DATA_SZ );

        if( VLT_OK == status )
        {
            /*
            * Update the length of the buffer to reflect the decryption
            * Need to cast as the return is a VLT_U32 and P3 is VLT_U8
            */
            pRsp->u16Len = (VLT_U8)u32DataLen;
        }
    }    

    /*
    * Close the Cipher
    */
    if(VLT_OK != status)
    {
        CipherClose();
    }
    else
    {
        status = CipherClose();
    }

    return ( status );
}
#endif /* ( VLT_ENABLE_SCP02 == VLT_ENABLE ) */
