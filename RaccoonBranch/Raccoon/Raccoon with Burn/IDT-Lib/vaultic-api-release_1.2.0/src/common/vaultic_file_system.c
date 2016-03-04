/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "vaultic_common.h"
#include "vaultic_file_system.h"
#include "vaultic_api.h"
#include "vaultic_comms.h"
#include "vaultic_apdu.h"

/**
 * Private Defs
 */
#define ST_TRANS_NOT_STARTED    (VLT_U8)0x00
#define ST_TRANS_STARTED        (VLT_U8)0x10

#define ST_FILE_NOT_OPENED      (VLT_U8)0x00
#define ST_FILE_OPENED          (VLT_U8)0x10

#define DIRECTORY_MASK          (VLT_U8)0x08
#define MAX_SELECT_LENGTH       (VLT_U8)0xFF
#define EOF_SEEK_LENGTH         (VLT_U32)0xFFFFFFFF

#define ATTRIB_READ_ONLY_MASK   (VLT_U8)0x01
#define ATTRIB_SYSTEM_MASK      (VLT_U8)0x02
#define ATTRIB_HIDDEN_MASK      (VLT_U8)0x04
#define ATTRIB_DIR_MASK         (VLT_U8)0x08

#define DIRECTORY_SEPARATOR     '/'
#define NULL_TERMINATOR         '\0'

#define VLT_USER0_MASK          (VLT_U8)0x01
#define VLT_USER1_MASK          (VLT_U8)0x02
#define VLT_USER2_MASK          (VLT_U8)0x04
#define VLT_USER3_MASK          (VLT_U8)0x08
#define VLT_USER4_MASK          (VLT_U8)0x10
#define VLT_USER5_MASK          (VLT_U8)0x20
#define VLT_USER6_MASK          (VLT_U8)0x40
#define VLT_USER7_MASK          (VLT_U8)0x80


#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )

    /*
    * Private Data
    */
    VLT_U8 u8TransactionState = ST_TRANS_NOT_STARTED;

    VLT_U8 u8FileOpenState = ST_FILE_NOT_OPENED;

    /*
    * Local methods
    */
    static VLT_STS SelectWorkingDirectory( VLT_U16 u16EntryNameLen,
        const VLT_U8* pu8EntryName,
        VLT_PU16 pu16EntryStartPos,
        VLT_PU8 pu8Length );

    static VLT_STS ConvertFilePrivByteToBitField( VLT_USER_ACCESS* pUserAccess,
        VLT_U8 u8AccessCondition );

    static VLT_STS ConvertAccessToPriv( VLT_FS_ENTRY_PRIVILEGES* pFilePriv,
        VLT_FILE_PRIVILEGES* pFileAccess );

    static VLT_STS ConvertFilePrivBitFieldToByte( const VLT_USER_ACCESS* pUserAccess,
        VLT_PU8 pu8AccessCondition );

    static VLT_STS ConvertPrivToAccess( const VLT_FS_ENTRY_PRIVILEGES* pFilePriv,
        VLT_FILE_PRIVILEGES* pFileAccess );

#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */ 


VLT_STS VltFsOpenFile( VLT_U16 u16FileNameLength,
    const VLT_U8* pu8FileName, 
    VLT_U8 u8TransactionMode,
    VLT_FS_ENTRY_PARAMS *pFsFileParams )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_SELECT selectInfo;
    VLT_U16 u16StartPos = 0;
    VLT_U8 u8EntryNameLen = 0;

    /*
    * Validate the input Parameters
    */
    if( ( NULL == pu8FileName ) || ( NULL == pFsFileParams ) )
    {
        return( EFSOPNNULLPARAM );
    }

    /*
    * Check that the length of the file name is valid
    */
    if( 0 == u16FileNameLength )
    {
        status = EFSOPNINVLDNMLEN;
    }
    else
    {
        status = VLT_OK;
    }

    /*
    * Check if a file has already been opened
    */
    if( ST_FILE_OPENED == u8FileOpenState )
    {
        status = EFSOPNALRDYOPN;
    }

    if( VLT_OK == status )
    {
        /*
        * Check that the transaction mode is valid
        */
        if( ( VLT_TRANSACTION_DISABLED != u8TransactionMode ) &&
            ( VLT_TRANSACTION_ENABLED != u8TransactionMode) )
        {
            status = EFSOPNINVLDTRNMD;
        }
        else
        {
            status = SelectWorkingDirectory( u16FileNameLength, 
                pu8FileName, 
                &u16StartPos, 
                &u8EntryNameLen );

            if( ( VLT_OK == status ) && 
                ( VLT_TRANSACTION_ENABLED == u8TransactionMode ) )
            {
                status = VltBeginTransaction();
                if( VLT_OK == status)
                {
                    /*
                    * Update the transaction state to show that
                    * a transaction has been started
                    */
                    u8TransactionState = ST_TRANS_STARTED;
                }
            }
        }
    }

    /*
    * Select the file
    */
    if( VLT_OK == status )
    {
        status = VltSelectFileOrDirectory( &pu8FileName[u16StartPos], 
            u8EntryNameLen, 
            &selectInfo );

        if( VLT_OK == status )
        {
            if( DIRECTORY_MASK != ( DIRECTORY_MASK & selectInfo.u8FileAttribute ) )
            {

                /*
                * Convert the File Access bit field structure to file priviliges
                * structure
                */
                status = ConvertAccessToPriv( &(pFsFileParams->filePriv), 
                    &(selectInfo.FileAccess) );

                if( VLT_OK == status)
                {
                    /*
                    * Populate the File Entry Parameters structure
                    */
                    pFsFileParams->u32FileSize = selectInfo.u32FileSize;
                    pFsFileParams->u8EntryType = VLT_FILE_ENTRY;

                    /*
                    * Decipher the File Attributes
                    */
                    if( ATTRIB_READ_ONLY_MASK == 
                        ( ATTRIB_READ_ONLY_MASK & selectInfo.u8FileAttribute ) )
                    {
                        pFsFileParams->attribs.readOnly = 1;
                    }
                    else
                    {
                        pFsFileParams->attribs.readOnly = 0;
                    }

                    if( ATTRIB_SYSTEM_MASK == 
                        ( ATTRIB_SYSTEM_MASK & selectInfo.u8FileAttribute ) )
                    {
                        pFsFileParams->attribs.system = 1;
                    }
                    else
                    {
                        pFsFileParams->attribs.system = 0;
                    }

                    if( ATTRIB_HIDDEN_MASK == 
                        ( ATTRIB_HIDDEN_MASK & selectInfo.u8FileAttribute ) )
                    {
                        pFsFileParams->attribs.hidden = 1;
                    }
                    else
                    {
                        pFsFileParams->attribs.hidden = 0;
                    }

                    /*
                    * Update the File Open state to opened
                    */
                    u8FileOpenState = ST_FILE_OPENED;
                }
            }
            else
            {
                if( ST_TRANS_STARTED == u8TransactionState )
                {
                    /*
                    * Close the transaction as a file wasn't selected
                    */
                    status = VltEndTransaction();

                    u8TransactionState = ST_TRANS_NOT_STARTED;
                }

                status = EFSOPNNOTFILE;
            }
        }
        else
        {
            if( VLT_STATUS_TRANSACTION_MEM_FAILURE == status )
            {
                /*
                * Make sure that the transaction state is in step with the 
                * Vault IC
                */
                u8TransactionState = ST_TRANS_NOT_STARTED;
            }
            else
            {
                if( ST_TRANS_STARTED == u8TransactionState )
                {
                    /*
                    * Close the transaction as a file wasn't selected
                    */
                    status = VltEndTransaction();

                    u8TransactionState = ST_TRANS_NOT_STARTED;
                }

                status = EFSOPNSLCTFLD;
            }
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsCloseFile( void )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Check if a file has been opened
    */
    if( ST_FILE_OPENED == u8FileOpenState )
    {
        /*
        * Check if a transaction was in progress
        */
        if( ST_TRANS_STARTED == u8TransactionState )
        {
            /*
            * It was, so call VltEndTransaction to commit the changes to the 
            * file system
            */
            status = VltEndTransaction();

            /*
            * Clear the state of the transaction
            */
            u8TransactionState = ST_TRANS_NOT_STARTED;
        }
        else
        {
            /*
            * Nothing needs to be done so set the status as VLT_OK
            */
            status = VLT_OK;
        }

        u8FileOpenState = ST_FILE_NOT_OPENED;

    }
    else
    {
        /*
        * No file was opened
        */
        status = EFSCLSNOFILEOPN;;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsCreate( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    const VLT_FS_ENTRY_PARAMS *pFsEntryParams,
    VLT_U8 u8UserId )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16StartPos = 0;
    VLT_U8 u8NameLen = 0;
    VLT_FILE_PRIVILEGES filePrivileges;

    /*
    * Validate the input pointers
    */
    if( ( NULL == pu8EntryName ) ||
        ( NULL == pFsEntryParams ) )
    {
        return( EFSCRTNULLPARAMS );
    }

    /*
    * Check that the length of the file name is valid
    */
    if( 0 == u16EntryNameLen )
    {
        status = EFSCRTINVLDNMLEN;
    }
    else
    {
        status = VLT_OK;
    }

    /*
    * A file can only be created if a file is not currently open
    */
    if( ST_FILE_OPENED == u8FileOpenState )
    {
        status = EFSCRTFILEALRDYOPN;
    }

    if( VLT_OK == status )
    {
        /*
        * Validate the entry type is valid
        */
        if( ( VLT_FOLDER_ENTRY != pFsEntryParams->u8EntryType ) &&
            ( VLT_FILE_ENTRY  != pFsEntryParams->u8EntryType ) )
        {
            status = EFSCRTINVLDENTRYTYP;
        }
        else
        {
            status = SelectWorkingDirectory( u16EntryNameLen, 
                pu8EntryName, 
                &u16StartPos, 
                &u8NameLen );
        }
    }

    if( VLT_OK == status )
    {
        VLT_U8 u8Attributes = 0;

        /*
        * Check what the attributes have to be set to and do it
        */
        if( 1 == pFsEntryParams->attribs.readOnly )
        {
            u8Attributes |= ATTRIB_READ_ONLY_MASK; 
        }

        if( 1 == pFsEntryParams->attribs.system )
        {
            u8Attributes |= ATTRIB_SYSTEM_MASK;
        }

        if( 1 == pFsEntryParams->attribs.hidden )
        {
            u8Attributes |= ATTRIB_HIDDEN_MASK;
        }

        /*
        * Convert the File Priviliges structure to File Access bit field
        * structure
        */
        status = ConvertPrivToAccess( &(pFsEntryParams->filePriv ),
            &filePrivileges );

        if( VLT_OK == status )
        {

            /*
            * Entry type has been set to either file or folder
            */
            if( VLT_FILE_ENTRY == pFsEntryParams->u8EntryType )
            {
                status = VltCreateFile( u8UserId,
                    pFsEntryParams->u32FileSize,
                    &filePrivileges,
                    u8Attributes,
                    u8NameLen,
                    &(pu8EntryName[u16StartPos]) );
            }
            else
            {
                /*
                * As this is a folder, update the attributes to reflect this 
                */
                u8Attributes |= ATTRIB_DIR_MASK;

                status = VltCreateFolder( u8UserId,
                    &filePrivileges,
                    u8Attributes,
                    u8NameLen,
                    &(pu8EntryName[u16StartPos]) );
            }
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsDelete( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    VLT_U8 u8Recursion )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_SELECT selectInfo;
    VLT_U16 u16StartPos = 0;
    VLT_U8 u8NameLen = 0;

    /*
    * Check that the pointer to the entry name is valid
    */
    if( NULL == pu8EntryName )
    {
        return( FSDLTNULLPARAMS );
    }

    /*
    * Check that the length of the file name is valid
    */
    if( 0 == u16EntryNameLen )
    {
        status =  EFSDLTINVLDNMLEN;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        /*
        * A file can only be deleted if a file is not currently open
        */
        if( ST_FILE_OPENED == u8FileOpenState )
        {
            status = EFSDLTFILEALRDYOPN;
        }
    }

    if( VLT_OK == status )
    {
        /*
        * Check that a valid recursive mode was specified
        */
        if( ( VLT_NON_RECURSIVE_DELETE != u8Recursion ) &&
            ( VLT_RECURSIVE_DELETE  != u8Recursion ) )
        {
            status = EFSDLTINVLDENTRYTYP;
        }
        else
        {
            status = VLT_OK;
        }
    }

    if( VLT_OK == status)
    {
        status = SelectWorkingDirectory( u16EntryNameLen, 
            pu8EntryName, 
            &u16StartPos, 
            &u8NameLen );
    }

    if( VLT_OK == status )
    {
        status = VltSelectFileOrDirectory( &(pu8EntryName[u16StartPos]), 
            u8NameLen, 
            &selectInfo );
    }

    if( VLT_OK == status)
    {
        /*
        * Check if the item to be deleted is a file or a folder
        */
        if( DIRECTORY_MASK != ( DIRECTORY_MASK & selectInfo.u8FileAttribute ) )
        {
            /*
            * Entry is a file.  Make sure that they didn't specify recursive
            * delete
            */
            if( VLT_RECURSIVE_DELETE == u8Recursion )
            {
                status = EFSDLTFILERECSV;
            }
            else
            {
                status = VltDeleteFile();
            }
        }
        else
        {
            /*
            * Entry is a folder.  Make sure that recursive delete has not been
            * specified when a transaction has been put in place
            */
            if( ( VLT_RECURSIVE_DELETE == u8Recursion ) && 
                ( ST_TRANS_STARTED == u8TransactionState ) )
            {
                status = EFSDLTFOLRECSV;
            }

            if( VLT_OK == status )
            {
                status = VltDeleteFolder( u8Recursion );
            }
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsReadFile(VLT_U32 u32Offset, 
    VLT_PU8 pu8DataOut, 
    VLT_PU32 pu32DataLength )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U32 u32RemaingBytes = 0;
    VLT_U32 u32CurrentPos = 0;
    const VLT_U16 u16MaxRead = VLT_MAX_APDU_RCV_DATA_SZ;

    /*
    * Check that the input parameters are valid
    */
    if( (NULL == pu8DataOut ) || ( NULL == pu32DataLength ) )
    {
        return( EFSRDNULLPARAMS );
    }
    else if ( 0 == *pu32DataLength )
    {
        return( EFSRDINVDLEN );
    }
    else
    {
        status = VLT_OK;
    }

    /*
    * Set the number of bytes still to be read
    */
    u32RemaingBytes = *pu32DataLength;

    /*
    * Check if a file has been opened
    */
    if( ST_FILE_OPENED == u8FileOpenState )
    {
        /* Seek to the requested position */
        status = VltSeekFile( u32Offset );

        if ( VLT_OK !=  status )
        {
            return ( EFSRDSEEKFAILED );
        }       

        while ( (VLT_OK == status) && u32RemaingBytes )
        {
            VLT_U16 u16ReadSize = 0;
            /*
            * If the remaining size is gretaer than the maximum size receive the data
            * by multiple VltReadFile calls.  
            */
            if( u32RemaingBytes > u16MaxRead)
            {
                u16ReadSize = u16MaxRead;
            }
            else
            {
                u16ReadSize = (VLT_U8)u32RemaingBytes;
            }
            status = VltReadFile(&u16ReadSize, &(pu8DataOut[u32CurrentPos]) );

            /*
            * If we reach the end of file before the requested number of bytes have
            * been read update as we would if the read was successful
            */
            if( ( VLT_OK == status ) || ( VLT_EOF == status ) )
            {
                u32RemaingBytes -= u16ReadSize;
                u32CurrentPos += u16ReadSize;                                                
            }
        }

        if( ( VLT_OK == status ) || ( VLT_EOF == status) )
        {
            /*
            * Report the number of bytes actually read
            */
            *pu32DataLength = u32CurrentPos;
        }
    }
    else
    {
        status = EFSRDNOFILEOPN;
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsWriteFile(VLT_U32 u32Offset, 
    VLT_PU8 pu8DataIn, 
    VLT_U32 u32DataLength,
    VLT_U8 u8ReclaimSpace )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;

    /*
    * Check that the buffer to the output data is valid
    */
    if( NULL == pu8DataIn)
    {
        return( EFSWRTNULLPARAMS );
    }

    if ( 0 == u32DataLength )
    {
        return( EFSWRINVDLEN );
    }

    if( ( VLT_NO_RECLAIM_SPACE != u8ReclaimSpace ) && 
        ( VLT_RECLAIM_SPACE != u8ReclaimSpace ) )
    {
        status = EFSWRTINVLDRCLMMD;
    }
    else
    {
        /*
        * Check if a file has been opened
        */
        if( ST_FILE_OPENED == u8FileOpenState )
        {
            VLT_U32 u32RemaingBytes = u32DataLength;
            VLT_U32 u32CurrentPos = 0;
            VLT_U16 u16MaxWriteSize = VltCommsGetMaxSendSize();

            status = VLT_OK;


            /* Seek to the requested position */
            status = VltSeekFile( u32Offset );

            if ( ( VLT_OK ==  status ) ||
                 ( VLT_EOF == status ) )
            {
                status = VLT_OK;
            }
            else
            {
                return ( EFSWRSEEKFAILED );
            }

            while ( ( VLT_OK == status ) && u32RemaingBytes )
            {
                VLT_U16 u16WriteSize = 0;
                VLT_U8 u8RecSpace = VLT_NO_RECLAIM_SPACE;

                /*
                * If the remaining size is gretaer than the maximum size send the data
                * by multiple VltWriteFile calls.  
                */
                if( u32RemaingBytes > u16MaxWriteSize )
                {
                    /*
                    * Further write operations will be required so don't reclaim
                    * space
                    */
                    u16WriteSize = u16MaxWriteSize;
                    u8RecSpace = VLT_NO_RECLAIM_SPACE;
                }
                else
                {
                    /*
                    * Setup the last write as the remaining number of bytes will
                    * fit into one more write, so now set the recalim to what was
                    * passed in
                    */
                    u16WriteSize = (VLT_U8)u32RemaingBytes;
                    u8RecSpace = u8ReclaimSpace;
                }

                status = VltWriteFile( &(pu8DataIn[u32CurrentPos]),
                    (VLT_U8)u16WriteSize,
                    u8RecSpace );

                if( VLT_OK == status )
                {
                    u32RemaingBytes -= u16WriteSize;
                    u32CurrentPos += u16WriteSize;
                }
            }
        }
        else
        {
            /*
            * A file has not been opened
            */
            status = EFSWRTNOFILEOPN;
        }
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}


VLT_STS VltFsListFiles( VLT_U16 u16FolderNameLength,
    const VLT_U8* pu8FolderName,
    VLT_PU16 pu16ListRespLength,
    VLT_PU8 pu8RespData )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16StartPos = 0;
    VLT_U8 u8EntryNameLen = 0;
    VLT_SELECT selectInfo;

    /*
    * Check that the input parameters are valid
    */
    if( ( NULL == pu8FolderName ) ||
        ( NULL == pu8RespData ) ||
        ( NULL == pu16ListRespLength ) )
    {
        return( EFSLSTNULLPARAMS );
    }

    /*
    * Check that the folder name length is valid
    */
    if( 0 == u16FolderNameLength )
    {
        status = EFSLSTINVLDLEN;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        /*
        * Check that the response buffer is not zero
        */
        if( 0 == *pu16ListRespLength )
        {
            status = EFSLSTINVLDRESPLEN;
        }
    }

    if( VLT_OK == status )
    {

        if( ST_FILE_OPENED == u8FileOpenState )
        {
            status = EFSLSTALRDYOPN;
        }
        else
        {
            /*
            * Select the directory above the one of interest, and we will get
            * the start position and length of the name of the folder we are
            * interested in
            */
            status = SelectWorkingDirectory( u16FolderNameLength, 
                pu8FolderName, 
                &u16StartPos, 
                &u8EntryNameLen );
        }
    }

    if( VLT_OK == status )
    {
        status = VltSelectFileOrDirectory( &(pu8FolderName[u16StartPos]),
            u8EntryNameLen,
            &selectInfo );
    }

    if( VLT_OK == status )
    {
        /*
        * Check that the entry is a folder.  Can't list on a file
        */
        if( DIRECTORY_MASK != ( DIRECTORY_MASK & selectInfo.u8FileAttribute ) )
        {
            status = EFSLSTENTRYNOTFLDR;
        }
    }

    if( VLT_OK == status )
    {
        /*
        * Now call the VltListFiles method
        */
        status = VltListFiles( pu16ListRespLength, pu8RespData );
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsSetPrivileges( VLT_U16 u16EntryNameLength,
    const VLT_U8* pu8EntryName, 
    const VLT_FS_ENTRY_PRIVILEGES* pFsEntryPrivileges )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_OK;
    VLT_U16 u16StartPos = 0;
    VLT_U8 u8EntryLength = 0;
    VLT_SELECT selectInfo;

    /*
    * Validate the input parameters
    */
    if( ( NULL == pu8EntryName ) ||
        ( NULL == pFsEntryPrivileges ) )
    {
        return( EFSSTPRIVNULLPARAM );
    }

    /*
    * Check for a valid length
    */
    if( 0 == u16EntryNameLength )
    {
        status = EFSSTPRIVINVLDLEN;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        if( ST_FILE_OPENED == u8FileOpenState )
        {
            status = EFSSETPRIVALRDYOPN;
        }
        else
        {
            /*
            * Select the working directory for the file system entry
            */
            status = SelectWorkingDirectory( u16EntryNameLength, 
                pu8EntryName,
                &u16StartPos,
                &u8EntryLength );
        }

    }

    if( VLT_OK == status )
    {
        /*
        * Select the file system entry
        */
        status = VltSelectFileOrDirectory( &(pu8EntryName[u16StartPos]),
            u8EntryLength,
            &selectInfo );
    }

    if( VLT_OK == status )
    {
        status = ConvertPrivToAccess( pFsEntryPrivileges, &selectInfo.FileAccess );
    }

    if( VLT_OK == status )
    {
        status = VltSetPrivileges( &(selectInfo.FileAccess) );
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}

VLT_STS VltFsSetAttributes( VLT_U16 u16EntryNameLength,
    const VLT_U8* pu8EntryName, 
    const VLT_FS_ENTRY_ATTRIBS* pFsEntryAttributes )
{
#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
    VLT_STS status = VLT_OK;
    VLT_U16 u16StartPos = 0;
    VLT_U8 u8EntryLength = 0;
    VLT_SELECT selectInfo;
    VLT_U8 u8Attributes = 0;
    

    /*
    * Validate the input parameters
    */
    if( ( NULL == pu8EntryName ) ||
        ( NULL == pFsEntryAttributes ) )
    {
        return( EFSSTATTRNULLPARAM );
    }

    /*
    * Check for a valid length
    */
    if( 0 == u16EntryNameLength )
    {
        status = EFSSTATTRBINVLDLEN;
    }
    else
    {
        status = VLT_OK;
    }

    if( VLT_OK == status )
    {
        if( ST_FILE_OPENED == u8FileOpenState )
        {
            status = EFSSETATTRALRDYOPN;
        }
        else
        {
            /*
            * Select the working directory for the file system entry
            */
            status = SelectWorkingDirectory( u16EntryNameLength, 
                pu8EntryName,
                &u16StartPos,
                &u8EntryLength );
        }

    }

    if( VLT_OK == status )
    {
        /*
        * Select the file system entry
        */
        status = VltSelectFileOrDirectory( &(pu8EntryName[u16StartPos]),
            u8EntryLength,
            &selectInfo );
    }

    if( VLT_OK == status )
    {
        /*
        * Check what the attributes have to be set to and do it
        */
        if( 1 == pFsEntryAttributes->readOnly )
        {
            u8Attributes |= ATTRIB_READ_ONLY_MASK; 
        }

        if( 1 == pFsEntryAttributes->system )
        {
            u8Attributes |= ATTRIB_SYSTEM_MASK;
        }

        if( 1 == pFsEntryAttributes->hidden )
        {
            u8Attributes |= ATTRIB_HIDDEN_MASK;
        }
    }

    if( VLT_OK == status )
    {
        status = VltSetAttributes( u8Attributes );
    }

    return( status );
#else
    return( EMETHODNOTSUPPORTED );
#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE ) */
}


#if( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )
static VLT_STS SelectWorkingDirectory( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    VLT_PU16 pu16EntryStartPos,
    VLT_PU8 pu8Length  )
{
    VLT_STS status = VLT_FAIL;
    VLT_U16 u16CurrentPos = 0;
    VLT_U8 u8Done = FALSE;

    /*
    * Validate the input parameters
    */
    if( ( NULL == pu8EntryName ) ||
        ( NULL == pu16EntryStartPos ) ||
        ( NULL == pu8Length ) )
    {
        return( EFSSWDNULLPARAMS );
    }

    /*
    * Check if the entry passed in is just the root directory.  If it is don't
    * attempt to select the working directory, just set up the entry start 
    * position and length
    */
    if( ( ( 1 == u16EntryNameLen ) && 
        ( DIRECTORY_SEPARATOR == pu8EntryName[0] ) ) ||
        ( ( 2 == u16EntryNameLen ) &&
        ( DIRECTORY_SEPARATOR == pu8EntryName[0] ) && 
        ( NULL_TERMINATOR == pu8EntryName[1]) ) )
    {
        *pu16EntryStartPos = 0;
        *pu8Length = (VLT_U8)u16EntryNameLen;

        return( VLT_OK );
    }

    /*
    * The current position should be at the end of the entry name, which is 1
    * less than the length due to array indexing from zero
    */
    u16CurrentPos = u16EntryNameLen - 1;

    /*
    * If the last character of the entry name is a NULL, which it should be,
    * move the position one back to the last character
    */
    if( NULL_TERMINATOR == pu8EntryName[u16CurrentPos] )
    {
        u16CurrentPos--;
    }
    
    /*
    * Find the entry of interest.  Do this by starting at the end of the entry
    * name and working back until we come across a '/'
    */
    if( ( DIRECTORY_SEPARATOR == pu8EntryName[u16CurrentPos] ) &&
        ( 0 != u16CurrentPos ) )
    {
        /*
        * Check whether an extra '/' has been put at the end of the path
        */
        u16CurrentPos--;
    }

    while( FALSE == u8Done )
    {
        if( DIRECTORY_SEPARATOR == pu8EntryName[u16CurrentPos] )
        {
            /*
            * There is a path before the name of the entry, so adjust the
            * entry start position ahead of the '/' character
            * 
            */
            u8Done = TRUE;
            *pu16EntryStartPos = u16CurrentPos + 1;
        }
        else
        {
            if(0 != u16CurrentPos )
            {
                u16CurrentPos--;
            }
            else
            {
                /*
                * There is no directory information contained so set the start
                * position at the beginning.  No Select call is required
                */
                u8Done = TRUE;
                *pu16EntryStartPos = u16CurrentPos;
            }
        }
    }

    /*
    * Calculate the length of the entry name
    */
    if( MAX_SELECT_LENGTH < u16EntryNameLen - *pu16EntryStartPos )
    {
        status = EFSSWDENTRYTOOLONG;
    }
    else
    {
        *pu8Length = u16EntryNameLen - *pu16EntryStartPos;
        status = VLT_OK;
    }

    if( (VLT_OK == status ) && ( 0 != *pu16EntryStartPos ) )
    {
        /*
        * Path information is present so at least 1 select operation will be 
        * required
        */
        VLT_U16 u16MaxSendSize = VltCommsGetMaxSendSize();
        VLT_U16 u16StartPos = 0;
        VLT_U16 u16EndPos = 0;
        VLT_SELECT selectInfo;

        if( MAX_SELECT_LENGTH < u16MaxSendSize)
        {
            /*
            * The select file cannot deal with data size greater than 255, so 
            * make sure that we never attempt to send more than that
            */
            u16MaxSendSize = MAX_SELECT_LENGTH;
        }

        /*
        * Reduce the max send size by 1 so that there is space to add a NULL
        */
        u16MaxSendSize--;

        u16StartPos = 0;

        while( u16StartPos < *pu16EntryStartPos )
        {
            VLT_U16 u16Length = 0;

            /*
            * Work back from the maximum size that can be sent looking for a 
            * directory seperator character.  1 is removed from the size as we
            * need to adjust for the array indexing from 0
            */
            u16EndPos = u16StartPos + ( u16MaxSendSize - 1 );

            /*
            * Check that the end position hasn't been advanced past the start
            * of the entry
            */
            if( ( (*pu16EntryStartPos) - 1 ) < u16EndPos )
            {
                u16EndPos = (*pu16EntryStartPos) - 1;
            }

            /*
            * Work back looking for a directory seperator
            */
            while( ( DIRECTORY_SEPARATOR != pu8EntryName[u16EndPos] ) && 
                ( u16StartPos != u16EndPos ) )
            {
                u16EndPos--;
            }

            /*
            * If we haven't found one then the path is invalid
            */
            if( ( u16StartPos == u16EndPos ) &&
                ( DIRECTORY_SEPARATOR != pu8EntryName[u16EndPos] ) )
            {
                /*
                * Set the error code and break out of the while loop
                */
                status = EFSSWDINVLDPTH;
                break;
            }

            /*
            * Move the end position to one position past the directory
            * seperator
            */
            if( DIRECTORY_SEPARATOR == pu8EntryName[u16EndPos] )
            {
                u16EndPos++;
            }

            /*
            * Calculate the length of the path, and verify that it can be
            * sent in one VltSelectFileOrDirectory command.  The addition
            * of 1 is due to array indexing starting at 0
            */
            if( u16EndPos == u16StartPos )
            {
                u16Length = 1;
            }
            else
            {
                u16Length = u16EndPos - u16StartPos ;
            }

            if( u16MaxSendSize < u16Length )
            {
                /*
                * The length is greater than what can be send down in one
                * command so set the error code and break out of the while loop
                */
                status = EFSSWDINVLDPTHLEN;
                break;
            }

            status = VltSelectFileOrDirectory( &(pu8EntryName[u16StartPos]),
                (VLT_U8)u16Length, 
                &selectInfo );

            if( VLT_OK == status )
            {
                u16StartPos = u16EndPos;
            }
            else
            {
                /*
                * The call to VltSelectFileOrDirectory was not successful, so
                * break out of the while loop
                */
                break;
            }
        }        
    }

    return( status );
}

static VLT_STS ConvertFilePrivByteToBitField( VLT_USER_ACCESS* pUserAccess,
    VLT_U8 u8AccessCondition )
{
    /*
    * Validate the User Access Pointer is valid
    */
    if( NULL == pUserAccess )
    {
        return( EFSCFPBYTBTNULLPARAM );
    }
    /*
    * User 0
    */
    if( VLT_USER0_MASK == ( u8AccessCondition & VLT_USER0_MASK ) )
    {
        pUserAccess->user0 = 1;
    }
    else
    {
        pUserAccess->user0 = 0;
    }

    /*
    * User 1
    */
    if( VLT_USER1_MASK == ( u8AccessCondition & VLT_USER1_MASK ) )
    {
        pUserAccess->user1 = 1;
    }
    else
    {
        pUserAccess->user1 = 0;
    }

    /*
    * User 2
    */
    if( VLT_USER2_MASK == ( u8AccessCondition & VLT_USER2_MASK ) )
    {
        pUserAccess->user2 = 1;
    }
    else
    {
        pUserAccess->user2 = 0;
    }

    /*
    * User 3
    */
    if( VLT_USER3_MASK == ( u8AccessCondition & VLT_USER3_MASK ) )
    {
        pUserAccess->user3 = 1;
    }
    else
    {
        pUserAccess->user3 = 0;
    }

    /*
    * User 4
    */
    if( VLT_USER4_MASK == ( u8AccessCondition & VLT_USER4_MASK ) )
    {
        pUserAccess->user4 = 1;
    }
    else
    {
        pUserAccess->user4 = 0;
    }

    /*
    * User 5
    */
    if( VLT_USER5_MASK == ( u8AccessCondition & VLT_USER5_MASK ) )
    {
        pUserAccess->user5 = 1;
    }
    else
    {
        pUserAccess->user5 = 0;
    }

    /*
    * User 6
    */
    if( VLT_USER6_MASK == ( u8AccessCondition & VLT_USER6_MASK ) )
    {
        pUserAccess->user6 = 1;
    }
    else
    {
        pUserAccess->user6 = 0;
    }

    /*
    * User 7
    */
    if( VLT_USER7_MASK == ( u8AccessCondition & VLT_USER7_MASK ) )
    {
        pUserAccess->user7 = 1;
    }
    else
    {
        pUserAccess->user7 = 0;
    }

    return( VLT_OK );
}

static VLT_STS ConvertAccessToPriv( VLT_FS_ENTRY_PRIVILEGES* pFilePriv,
    VLT_FILE_PRIVILEGES* pFileAccess )
{
    /*
    * Validate the Input Pointers
    */
    if( ( NULL == pFilePriv ) || ( NULL == pFileAccess ) )
    {
        return( EFSCONVACCPRV );
    }

    ConvertFilePrivByteToBitField( &(pFilePriv->readPrivilege), pFileAccess->u8Read );
    ConvertFilePrivByteToBitField( &(pFilePriv->writePrivilege), pFileAccess->u8Write );
    ConvertFilePrivByteToBitField( &(pFilePriv->deletePrivilege), pFileAccess->u8Delete );
    ConvertFilePrivByteToBitField( &(pFilePriv->executePrivilege), pFileAccess->u8Execute );

    return( VLT_OK );
}

static VLT_STS ConvertFilePrivBitFieldToByte( const VLT_USER_ACCESS* pUserAccess,
    VLT_PU8 pu8AccessCondition )
{
    /*
    * Validate the inpute pointers
    */
    if( ( NULL == pUserAccess ) || ( NULL == pu8AccessCondition) )
    {
        return( EFSCFPBTBYTNULLPARAM );
    }

    /*
    * First zero the Access Condition byte
    */
    *pu8AccessCondition = 0x00;

    /*
    * User 0
    */
    if( 1 == pUserAccess->user0 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER0_MASK;
    }

    /*
    * User 1
    */
    if( 1 == pUserAccess->user1 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER1_MASK;
    }

    /*
    * User 2
    */
    if( 1 == pUserAccess->user2 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER2_MASK;
    }

    /*
    * User 3
    */
    if( 1 == pUserAccess->user3 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER3_MASK;
    }

    /*
    * User 4
    */
    if( 1 == pUserAccess->user4 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER4_MASK;
    }

    /*
    * User 5
    */
    if( 1 == pUserAccess->user5 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER5_MASK;
    }

    /*
    * User 6
    */
    if( 1 == pUserAccess->user6 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER6_MASK;
    }

    /*
    * User 7
    */
    if( 1 == pUserAccess->user7 )
    {
        *pu8AccessCondition = *pu8AccessCondition | VLT_USER7_MASK;
    }

    return( VLT_OK );
}

static VLT_STS ConvertPrivToAccess( const VLT_FS_ENTRY_PRIVILEGES* pFilePriv,
    VLT_FILE_PRIVILEGES* pFileAccess )
{

    /*
    * Validate the Input Pointers
    */
    if( ( NULL == pFilePriv ) || ( NULL == pFileAccess ) )
    {
        return( EFSCONVPRVACC );
    }

    ConvertFilePrivBitFieldToByte( &(pFilePriv->readPrivilege), &(pFileAccess->u8Read) );
    ConvertFilePrivBitFieldToByte( &(pFilePriv->writePrivilege), &(pFileAccess->u8Write) );
    ConvertFilePrivBitFieldToByte( &(pFilePriv->deletePrivilege), &(pFileAccess->u8Delete) );
    ConvertFilePrivBitFieldToByte( &(pFilePriv->executePrivilege), &(pFileAccess->u8Execute) );

    return( VLT_OK );
}

#endif /* ( VLT_ENABLE_FILE_SYSTEM == VLT_ENABLE )*/
