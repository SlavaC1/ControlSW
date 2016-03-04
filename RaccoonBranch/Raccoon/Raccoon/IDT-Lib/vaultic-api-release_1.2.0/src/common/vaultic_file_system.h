/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \file vaultic_file_system.h
 * 
 * \defgroup VaultIcFileSystem Vault IC File System
 *
 * \brief Interface functions to VaultIC File System Service.
 * 
 * \par Description:
 *
 * This file declares the interface functions for the VaultIC File 
 * System service.  
 * 
 */
/*@{*/

#ifndef VAULTIC_FILE_SYSTEM_H
#define VAULTIC_FILE_SYSTEM_H

/**
 * \fn VltFsOpenFile( VLT_U16 u16FileNameLength,
 *  const VLT_U8* pu8FileName, 
 *  VLT_U8 u8TransactionMode,
 *  VLT_FS_ENTRY_PARAMS *pFsFileParams )
 *
 * \brief Used to open a file ready for reading or writing
 *
 * \par Description:
 *
 * This method is used to 'open' a file to allow it to be read from or wriiten
 * to.  If operations on the file are to be carried out within a transaction,
 * the u8TransactionMode parameter should be set to #VLT_TRANSACTION_ENABLED.
 *
 * \param u16FileNameLen    [IN]  The length of the file name.
 * \param pu8FileName       [IN]  The file name to open, optionally including any
*                                 modifications to the path
 * \param u8TransactionMode [IN]  Specifies whether further operations should be
 *                                carried out within a transaction to protect
 *                                against power loss.  Possible values are
 *                                 #VLT_TRANSACTION_DISABLED or 
 *                                #VLT_TRANSACTION_ENABLED
 * \param pFsFileParams     [OUT] The File Entry paramters for the file opened
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsOpenFile( VLT_U16 u16FileNameLength,
    const VLT_U8* pu8FileName, 
    VLT_U8 u8TransactionMode,
    VLT_FS_ENTRY_PARAMS *pFsFileParams );

/**
 * \fn VltFsCloseFile( void )
 *
 * \brief Used to close a file once it has been operated on.  This method must
 * be called if VltFsOpenFile() was called with the u8TransactionMode argument
 * set as #VLT_TRANSACTION_ENABLED to ensure that the transaction is ended and
 * that the updates to the file are commited to the file system.
 *
 * \par Description:
 *
 * This method is used to close a file following read/write operations.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsCloseFile( void );

/**
 * \fn VltFsCreate( VLT_U816 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    const VLT_FS_ENTRY_PARAMS *pFsEntryParams,
    VLT_U8 u8UserId )
 *
 * \brief Used to create files or folders within the file system
 *
 * \par Description:
 *
 * This method is used to create files or folders within the file system
 *
 * \param u16EntryNameLen [IN]  The length of the file/folder path being passed in
 * \param pu8EntryName    [IN]  The full name of the file/folder to be created
 * \param pFsEntryParams  [IN]  The parameters needed to create the file/folder
 * \param u8UserId        [IN]  The user id of the owner of the file system entry
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsCreate( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    const VLT_FS_ENTRY_PARAMS *pFsEntryParams,
    VLT_U8 u8UserId );

/**
 * \fn VltFsDelete( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    VLT_U8 u8Recursion )
 *
 * \brief Used to delete files or folders within the file system
 *
 * \par Description:
 *
 * This method is used to delete files or folders within the file system
 *
 * \param u16EntryNameLen [IN]  The length of the file/folder path being passed in
 * \param pu8EntryName    [IN]  The full name of the file/folder to be deleted
 * \param u8Recursion     [IN]  If the entry to be deleted is a folder, specifies
 *                              whether a recursive delete of files and folders
 *                              should take place.  Values are 
 *                              #VLT_NON_RECURSIVE_DELETE or #VLT_RECURSIVE_DELETE
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsDelete( VLT_U16 u16EntryNameLen,
    const VLT_U8* pu8EntryName,
    VLT_U8 u8Recursion );

/**
 * \fn VltFsReadFile( VLT_U32 u32Offset,
 *      VLT_PU8 pu8DataOut, 
 *      VLT_U32 u32DataLength )
 *
 * \brief Used to read data from an open file
 *
 * \par Description:
 *
 * This method is used to read data from a file previously opened with
 * VltFsOpenFile()
 *
 * If the requested length goes beyond the End of File, only the available data
 * are returned and a specific status (VLT_EOF) is returned.
 *
 * \param u32Offset      [IN]      The position from 0 within in the file to start 
 *                                 reading from. Don't use VLT_SEEK_TO_END, an 
 *                                 error of EFSRDSEEKFAILED will be returned.
 * \param pu8DataOut     [IN]      The length of the data to be read.
 * \param pu32DataLength [IN,OUT]  The buffer to place the data read from the Vault
 *                                 IC
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsReadFile( VLT_U32 u32Offset, 
    VLT_PU8 pu8DataOut, 
    VLT_PU32 pu32DataLength );

/**
 * \fn VltFsWriteFile(VLT_U32 u32Offset,
 *  VLT_PU8 pu8DataIn, 
 *  VLT_U32 u32DataLength,
 *  VLT_U8 u8ReclaimSpace );
 *
 * \brief Used to write data to an open file
 *
 * \par Description:
 *
 * This method is used to write data from a file previously opened with
 * VltFsOpenFile()
 *
 * \param u32Offset      [IN]  The position from 0 within in the file to start 
 *                             writing from.  Use constants VLT_SEEK_FROM_START,
 *                             VLT_SEEK_TO_END, or any value less than or equal
 *                             to the open file size.
 * \param pu8DataIn      [IN]  The buffer contaning the data to be written 
 *                             to the Vault IC
 * \param u32DataLength  [IN]  The length of the data to be written.
 * \param u8ReclaimSpace [IN]  Specifies whether any existing data proceeding
 *                             the written data should be deleted or not.
 *                             Use #VLT_NO_RECLAIM_SPACE or #VLT_RECLAIM_SPACE
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsWriteFile(VLT_U32 u32Offset,
    VLT_PU8 pu8DataIn, 
    VLT_U32 u32DataLength,
    VLT_U8 u8ReclaimSpace );


/**
 * \fn VLT_STS VltFsListFiles( VLT_U16 u16FolderNameLength,
 *  const VLT_U8* pu8FolderName,
 *  VLT_PU16 pu16ListRespLength,
 *  VLT_PU8 pu8RespData )
 *
 * \brief Used to list the files and folders within the specified folder
 *
 * \par Description:
 *
 * Returns a multi-string containing the names of files and directories located
 * in the currently selected directory.
 *
 * Files or directories with the hidden attribute are skipped out the listing.
 *
 * Note: A multi-string is a sequence of NULL-terminated strings. The sequence is
 * terminated by a NULL byte, therefore the multi-string ends up by two
 * consecutive bytes.
 *
 * Note: The order in which files are reported may be different before and after
 * a secure transaction performed on the selected directory.
 *
 * Note: The logged-in operator must have the List privilege on the selected
 * directory.
 *
 * A single NULL byte is returned if the current directory does not contain any
 * files or sub-directories.
 *
 * An error code is returned if the buffer passed in is not large enough to 
 * accomodate all of the data returned by the Vault IC.  The pu16ListRespLength
 * parameter is updated to the size of the buffer required to return the full 
 * directoty listing.  A partial directory listing will be contained within the
 * buffer, but this should be ignored.  A buffer of the size reported  back by 
 * the updated pu16ListRespLength should be constructed and passed as the input
 * paramteters to VltFsListFiles.
 *
 * \param u16FolderNameLength  [IN]     The length of the folder name
 * \param pu8FolderName        [IN]     The folder name to perform the listing on
 * \param pu16ListRespLength   [IN,OUT] On entry this holds the maximum size of
 *                                      the buffer. On exit it is set to the
 *                                      amount of buffer used.
 * \param pu8RespData          [IN]     Buffer to receive multi-string. 

 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsListFiles( VLT_U16 u16FolderNameLength,
    const VLT_U8* pu8FolderName,
    VLT_PU16 pu16ListRespLength,
    VLT_PU8 pu8RespData );

/**
 * \fn VltFsSetPrivileges( VLT_U16 u16EntryNameLength,
 *  const VLT_U8* pu8EntryName, 
 *  VLT_FS_ENTRY_PRIVILEGES* pFsEntryPrivileges )
 *
 * \brief Used to change the privileges on a file system entry
 *
 * \par Description:
 *
 * This method is used to change the access privileges on a file or folder
 *
 * \param u16EntryNameLen    [IN]  The length of the file/folder path being passed in
 * \param pu8EntryName       [IN]  The full name of the file/folder to be deleted
 * \param pFsEntryPrivileges [IN]  The updated access privileges to set on the file
 *                                 system entry.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsSetPrivileges( VLT_U16 u16EntryNameLength,
    const VLT_U8* pu8EntryName, 
    const VLT_FS_ENTRY_PRIVILEGES* pFsEntryPrivileges );

/**
 * \fn VltFsSetAttributes( VLT_U16 u16EntryNameLength,
 *  const VLT_U8* pu8EntryName, 
 *  VLT_FS_ENTRY_ATTRIBS* pFsEntryAttributes )
 *
 * \brief Used to change the attributes on a file system entry
 *
 * \par Description:
 *
 * This method is used to change the attributes on a file or folder
 *
 * \param u16EntryNameLen    [IN]  The length of the file/folder path being passed in
 * \param pu8EntryName       [IN]  The full name of the file/folder to be deleted
 * \param pFsEntryAttributes [IN]  The updated attributes to set on the file 
 *                                 system entry.
 *
 * \return Upon successful completion a VLT_OK status will be returned otherwise
 * the appropriate error code will be returned. Please note, status values 
 * larger than VLT_OK are errors that have originated in the API library while 
 * status values smaller than VLT_OK are the APDU status words that are returned 
 * by the Vault IC.
 *
 */
VLT_STS VltFsSetAttributes( VLT_U16 u16EntryNameLength,
    const VLT_U8* pu8EntryName, 
    const VLT_FS_ENTRY_ATTRIBS* pFsEntryAttributes );

#endif /*VAULTIC_FILE_SYSTEM_H*/
/*@}*/

