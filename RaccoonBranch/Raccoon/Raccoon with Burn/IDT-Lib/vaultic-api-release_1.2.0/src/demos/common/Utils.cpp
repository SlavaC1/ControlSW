#include "Utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <iostream>



LONG getPCSCReaderList(PCSC_DEVICE_LIST &readerList)
{
	VLT_U32 stringSize = 0;
	std::string strReaderName = "";
    
	// Look for VaultIC TOKEN into the list of reader
	// Need to check if card present
    VLT_SW sw = VltFindPCSCDevices( &stringSize, NULL );
	if (VLT_OK != sw)
	{
		return SCARD_E_UNEXPECTED;
	}
	
	VLT_PU8 pXmlString = (VLT_PU8)NULL;
    
    if( NULL == ( pXmlString = (VLT_PU8)malloc(stringSize) ) ) 
    {
			return SCARD_E_UNEXPECTED;
    }
    
	// Now call the VltFindDevices again with all the appropriate parameters
	// On return the pXmlString will be filled with the device list.
    sw = VltFindPCSCDevices( &stringSize, pXmlString );
	if (VLT_OK != sw)
	{
		FREE(pXmlString);
		return SCARD_E_UNEXPECTED;
	}
	else
	{
		size_t startPos = 0;
        size_t endPos = 0;
		std::string strXMLDevs = (const char*)pXmlString;
		strXMLDevs.find("<interface type=\"pcsc\">");
        if( std::string::npos != startPos )
        {
            endPos = strXMLDevs.find("</interface>");
            if( std::string::npos == endPos )
            {
                FREE(pXmlString);
                return SCARD_E_UNEXPECTED;
            }
            
            std::string strPCSC( strXMLDevs.substr( startPos, ( endPos - startPos ) ) );
            
			char *pattern = (char*)malloc(32);
			memset(pattern,0x00,32);
            
			unsigned char idx = 0x00;
			sprintf_s(pattern,32,"<peripheral idx=\"%02x\">",idx);
			startPos = strPCSC.find(pattern);
            
			while(std::string::npos != startPos)
			{
				// Start of the reader name is after the opening tag
				startPos += strlen(pattern);
                
				// End position is at beginning of the closing tag
				endPos = strPCSC.find("</peripheral>",startPos);
				if( std::string::npos == endPos )
				{
					FREE(pattern);
                    FREE(pXmlString);
						return SCARD_E_UNEXPECTED;
				}
                
				// Get the reader name
				strReaderName = strPCSC.substr(startPos, ( endPos - startPos ) );
                
				readerList[idx] = strReaderName;
                
				idx+= 0x01;
                
				sprintf_s(pattern,32,"<peripheral idx=\"%02x\">",idx);
				startPos = strPCSC.find(pattern);
			}
            
			FREE(pattern);
            FREE(pXmlString);
			return SCARD_S_SUCCESS;
		}
	}
	return SCARD_E_UNEXPECTED;
}