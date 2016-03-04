#ifndef _VAULT_IC_SAMPLE_CODE_H_
#define _VAULT_IC_SAMPLE_CODE_H_

#include <string>
    //
    // Defines Used for setting up array lengths and variables.
    #define VLT_REQUIRED_MANUF_PWD_LENGTH       32
    #define VLT_TWI_ADDRESS                     0x5F
    #define VLT_TWI_COMMS_SPEED                 100
    #define VLT_AARDVARK_TWI_MAX_BUS_TIME_OUT   450
    #define VLT_TWI_WAIT_TIME                   4000
    #define VLT_SELF_TEST_WAIT_TIME             1500
    #define MAX_WRITE_SIZE                      255
    

    //
    // Variables used by the VaultIC 100 Sample Code projects for 
    // establishing TWI comms. 
    static unsigned char ucRequiredPasswordLength = VLT_REQUIRED_MANUF_PWD_LENGTH;
    static unsigned char ucTwiAddress = VLT_TWI_ADDRESS;
    static unsigned char ucHostDevIdLength = VLT_HOST_DEV_ID_STRING_LENGTH;
    static unsigned short usBitRate = VLT_TWI_COMMS_SPEED;
    static unsigned short usBusTimeOut = VLT_AARDVARK_TWI_MAX_BUS_TIME_OUT; 
    static unsigned short usTwiTimeOut = VLT_TWI_WAIT_TIME;
    static unsigned short usStartupDelay = VLT_SELF_TEST_WAIT_TIME;
    

    //
    // Host ID key define, note the total length must be 16
    static unsigned char aucHostID[] =
    {
        'I',' ','a','m',' ','t','h','e',' ','h','o','s','t',' ',' ','!'
    };
    
    //
    // Device ID key define, note the total length must be 16
    static unsigned char aucDeviceID[] =
    {
        'I',' ','a','m',' ','t','h','e',' ','d','e','v','i','c','e','!'
    };

	void HexStringToByteArray(std::string psz, unsigned char pb[])
	{
		char buf[4];
		buf[2] = 0;
		int idx =0;
		for(unsigned int i=0;i< psz.length();i=i+2)
		{
			buf[0] = (char)psz[i];
			buf[1] = (char)psz[i+1];
			pb[idx++] = (BYTE)strtoul(buf, NULL, 16);
		}


		/*while (psz[0] != 0 && psz[1] != 0)
		{
			buf[0] = (char)psz[0];
			buf[1] = (char)psz[1];

			*pb++ = (BYTE)strtoul(buf, NULL, 16);
			psz += 2;
		}*/
	}

#endif
