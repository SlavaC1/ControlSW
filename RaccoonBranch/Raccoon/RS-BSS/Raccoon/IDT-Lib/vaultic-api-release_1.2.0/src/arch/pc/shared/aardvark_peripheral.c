/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2013. All Rights Reserved.
 * -------------------------------------------------------------------------- */

#include "aardvark_peripheral.h"
#include <stdio.h>

//
// If the pXmlReaderString is null and pSize not null and a value of zero it 
// means return to the pSize the total number of bytes required to read the
// entire XML string.
//
//
VLT_STS VltAardvarkDetectReaders( VLT_PU32 pSize, VLT_PU8 pXmlReaderString )
{
    VLT_STS status = VLT_FAIL;            
    // Has to be a signed integer to account for the library missing.
    int numOfReaders = 0; 
    VLT_U16 index = 0;
    VLT_U16 count = 0;
    const char* pXmlHeader = "<interface type=\"aardvark\">";   // 27, 0x1B
    const char* pXmlData = "<peripheral idx=\"%02d\">%10lu</peripheral>"; //34 , 0x22     
    const char* pXmlFooter = "</interface>"; //12 - 0x0C    
    VLT_U16 au16Dev[MAX_NO_OF_AARDVARKS];
    VLT_U32 au32Ids[MAX_NO_OF_AARDVARKS];
            
    if( NULL == pSize )
    {
        return( EDTAADRSIVLDPARAM );
    }

    if( ( *pSize != 0 ) && ( NULL == pXmlReaderString ) )
    {
        return( EDTAADRSIVLDSTR );
    }

    /*
     * If the number of devices found is more than we can handle then we return an 
     * appropriate error. Otherwise if the number returned is negative the aardvark.dll
     * is not in the specified path so we coerse the numOfReaders to 0. However, 
     * if the number is between 0 and the MAX_NO_OF_AADVARKS then we accept that and 
     * we build the appropriate device strings.
     */
    if( MAX_NO_OF_AARDVARKS < ( numOfReaders = aa_find_devices_ext( MAX_NO_OF_AARDVARKS, 
        &au16Dev[0], 
        MAX_NO_OF_AARDVARKS, 
        (u32*)&au32Ids[0] ) ) )
    {       
        return( EDTAADRSEXDMXM );                
    }
    else
    {
        if( 0 > numOfReaders )
        {
            numOfReaders = 0;
        }
    }

    /*
     * Total String count is:
     * Xml Header Size + ( Number of Readers * Xml Data Size) + Xml Footer Size + Null string terminator.
     */
    count = ( XML_AA_HEADER_SZ + ( numOfReaders * XML_AA_DATA_SZ ) + XML_AA_FOOTER_SZ );
    /*
     * Each entry is a 10 digit serial number
     */
    count += numOfReaders * 10 ; 

    if( NULL == pXmlReaderString  )
    {
        /*
         * Return to the caller the amount of space required to 
         * extract the reader string.
         */
        *pSize = count;
    }
    else
    {
        /*
         * Ensure the caller has passed us enough space to copy the 
         * readers xml string
         */
        if( *pSize < count )
        {
            return (EDTAADRSINSFLEN);
        }
        else
        {
            *pSize = 0;
            index = 0;

            /*
             * Add the XML header
             */
            *pSize = sprintf( (char*)&pXmlReaderString[*pSize], "%s", pXmlHeader );

            /*
             * Add the XML Data
             */
            while( index < numOfReaders )
            {
                *pSize += sprintf( (char*)&pXmlReaderString[*pSize], (const char*)pXmlData, index, (unsigned long)au32Ids[index] );
                ++index;
            }

            /*
             * Add the XML Footer
             */
            *pSize += sprintf( (char*)&pXmlReaderString[*pSize], "%s", pXmlFooter );
        }
    }

    return( VLT_OK );
}