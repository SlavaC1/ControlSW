/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_apdu.h"
#include "vaultic_api.h"
#include "vaultic_comms.h"
#include "vaultic_utils.h"
#include "vaultic_mem.h"
#include "vaultic_command.h"

/**
 * Externs 
 */
extern VLT_MEM_BLOB Command;                            /* declared in vaultic_api.c */
extern VLT_MEM_BLOB Response;                           /* declared in vaultic_api.c */

/* -------------------------------------------------------------------------- */
/**
 * \fn VltGetResponse
 *
 * \brief Issues a Get Response command.
 *
 * \param[in]  command  Command blob.
 * \param[in]  response Response blob.
 * \param[out] pSW      Status word.
 *
 * \return Status.
 */
static VLT_STS VltGetResponse(VLT_MEM_BLOB *command, 
    VLT_MEM_BLOB *response,
    VLT_PSW pSW)
{
    VLT_STS status;
    VLT_PU8 pu8Data;

    if ((*pSW & 0xFF00) != VLT_STATUS_GET_RESPONSE)
    {
        return EGTBADSW;
    }

    /* Build APDU for Get Response */

    pu8Data = command->pu8Data;

    *(pu8Data++) = 0x00;
    *(pu8Data++) = VLT_INS_GET_RESPONSE;
    *(pu8Data++) = 0x00;
    *(pu8Data++) = 0x00;
    *(pu8Data++) = (VLT_U8) LEXP(*pSW & 0xFF);

    /* Send the command */
    *pSW = VLT_STATUS_NONE;
    command->u16Len = (VLT_U16) (pu8Data - command->pu8Data);
    status = VltCommsDispatchCommand(command, response);
    if (VLT_OK != status)
    {
        return status;
    }

    if (response->u16Len < VLT_SW_SIZE)
    {
        return EGTINVLDSWSZ;
    }

    *pSW = VltEndianReadPU16(response->pu8Data + response->u16Len - VLT_SW_SIZE);

    return status;
}

/* -------------------------------------------------------------------------- */

VLT_STS VltCommand(VLT_MEM_BLOB *command,
    VLT_MEM_BLOB *response,
    VLT_U16 u16Send,
    VLT_U16 u16Require,
    VLT_PSW pSW)
{
    VLT_STS status;
    VLT_U16 u16Len;    
    VLT_U8 u8SWHi;
    VLT_U8 abHeader[VLT_APDU_TYPICAL_HEADER_SZ];

    *pSW = VLT_STATUS_NONE;

    /* Save a copy of the APDU in case it needs to be re-issued. */
    /*
    * No need to check the return type as pointer has been validated
    */
    (void)host_memcpy( abHeader, command->pu8Data, NELEMS(abHeader) );

    do
    {
        /* Send the command */

        command->u16Len = u16Send;
        status = VltCommsDispatchCommand(command, response);
        if( status != VLT_OK )
        {
            return status;
        }

        if( response->u16Len < VLT_SW_SIZE )
        {
            return ECINVLDSWSZ;
        }

        u16Len = response->u16Len - VLT_SW_SIZE;

        *pSW = VltEndianReadPU16(response->pu8Data + u16Len);

        /* We may have an immediate response, or be instructed to call Get
         * Response, or be instructed to re-issue the command with a corrected
         * P3 value.
         */
        u8SWHi = *pSW >> 8;

        switch (u8SWHi)
        {
        case VLT_STATUS_GET_RESPONSE >> 8:            

            /* Retrieve response data then check its size. */
            status = VltGetResponse(command, response, pSW);
            if (status != VLT_OK)
            {
                return status;
            }

            u16Len = response->u16Len - VLT_SW_SIZE;

            break;

        case VLT_STATUS_REISSUE >> 8:

            /* Re-issue the command with the corrected P3. */
            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy(command->pu8Data, abHeader, VLT_APDU_TYPICAL_HEADER_SZ - 1);
            command->pu8Data[4] = *pSW & 0xFF;
            *pSW = VLT_STATUS_NONE;

            break;

        default:
            break;
        }
    }
    while (u8SWHi == (VLT_STATUS_REISSUE >> 8));

    if( u16Len < u16Require )
    {
        return ECINVLDRSP;
    }

    return status;
}

/* -------------------------------------------------------------------------- */

VLT_STS VltCase4(VLT_U8 u8Ins,
    VLT_U8 u8P2,
    VLT_U32 u32SrcLen,
    const VLT_U8 *pu8Src,
    VLT_PU32 pu32DstLen,
    VLT_PU8 pu8Dst,
    VLT_PSW pSW)
{
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16MaxChunk;
    VLT_U32 u32Remaining;
    VLT_PU8 pu8Out;
    VLT_PU8 pu8OutEnd;
    VLT_U16 u16Idx; 

    if ((0 == u32SrcLen && NULL != pu8Src) ||
        (0 != u32SrcLen && NULL == pu8Src) ||
        NULL == pu32DstLen  ||
        0    == *pu32DstLen ||
        NULL == pu8Dst      ||
        NULL == pSW)
    {
        return EC4NULLPARA;
    }

    *pSW = VLT_STATUS_NONE;

    /* We need to split the data up into chunks, the size of which the comms
     * layer tells us. */

    u16MaxChunk  = VltCommsGetMaxSendSize();

    u32Remaining = u32SrcLen;

    pu8Out = pu8Dst;
    pu8OutEnd = pu8Dst + *pu32DstLen;

    do
    {
        VLT_U16 u16Chunk;

        /* Build APDU. We have to do this on every iteration as the output of
         * the previous iteration will have overwritten it (assuming a shared
         * buffer). */
        u16Idx = VLT_APDU_DATA_OFFSET ;

        if (u32Remaining > u16MaxChunk)
        {
            u16Chunk = u16MaxChunk;
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_CHAINING;
        }
        else
        {
            u16Chunk = (VLT_U16) u32Remaining;
            Command.pu8Data[ VLT_APDU_CLASS_OFFSET ] = VLT_CLA_NO_CHANNEL; 
        }
        Command.pu8Data[ VLT_APDU_INS_OFFSET ] = u8Ins;
        Command.pu8Data[ VLT_APDU_P1_OFFSET ] = 0;
        Command.pu8Data[ VLT_APDU_P2_OFFSET ] = u8P2;
        Command.pu8Data[ VLT_APDU_P3_OFFSET ] = LIN(WRAPPED_BYTE(u16Chunk));

        if( 0 != u16Chunk )
        {
            /* Build Data In */

            /*
            * No need to check the return type as pointer has been validated
            */
            (void)host_memcpy( &Command.pu8Data[u16Idx], pu8Src, u16Chunk );
            u16Idx += u16Chunk;
            pu8Src += u16Chunk;
        }

        /* Send the command */

        status = VltCommand( &Command, &Response, u16Idx, 0, pSW );

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

        /*
        * No need to check the return type as pointer has been validated
        */
        (void)host_memcpy( pu8Out, Response.pu8Data, Response.u16Len );
        pu8Out += Response.u16Len;

        /* Check response code */
        switch( *pSW )
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

        u32Remaining -= u16Chunk;
    }
    while (u32Remaining || *pSW == VLT_STATUS_RESPONDING);

    /* Report the final amount of data produced */
    *pu32DstLen = (VLT_U32)(pu8Out - pu8Dst);

    return status;
}
