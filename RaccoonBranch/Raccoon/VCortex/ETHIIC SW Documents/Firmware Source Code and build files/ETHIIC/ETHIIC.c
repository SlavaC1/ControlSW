//*****************************************************************************
//
// enet_lwip.c - Sample WebServer Application using lwIP.
//
// Copyright (c) 2013-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "inc/hw_i2c.h"
#include "httpserver_raw/httpd.h"
#include "drivers/pinout.h"

#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_ints.h"
#include "swupdate.h"
#include "cgifuncs.h"
#include <string.h>



#define IN_PORT 4477
#define OUT_PORT 4478
#define IIC_TIMEOUT 20

#define MY_NAME "ETHIIC-1"
#define FW_VERSION "2.11 STSYS"
#define DEBUG_PRINT 0

#define PRINT_ON_DEBUG  if (DEBUG_PRINT) UARTprintf

#define NUM_I2C_CHANNELS 4
#define NUM_A2D_CHANNELS 20
#define NUM_D2A_CHANNELS 4
#define NUM_RS232_CHANNELS 4
#define NUM_CAN_CHANNELS 2
#define NUM_GPIO_CHANNELS 32

char g_stop = 0;
int g_count =0;
int g_lastVal = 0;

//#define READ_BYTES_PATCH

#define HWREG(x)                                                              \
        (*((volatile uint32_t *)(x)))

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Ethernet with lwIP (enet_lwip)</h1>
//!
//! This example application demonstrates the operation of the Tiva
//! Ethernet controller using the lwIP TCP/IP Stack.  DHCP is used to obtain
//! an Ethernet address.  If DHCP times out without obtaining an address,
//! AutoIP will be used to obtain a link-local address.  The address that is
//! selected will be shown on the UART.
//!
//! UART0, connected to the ICDI virtual COM port and running at 115,200,
//! 8-N-1, is used to display messages from this application. Use the
//! following command to re-build the any file system files that change.
//!
//!     ../../../../tools/bin/makefsfile -i fs -o enet_fsdata.h -r -h -q
//!
//! For additional details on lwIP, refer to the lwIP web page at:
//! http://savannah.nongnu.org/projects/lwip/
//
//*****************************************************************************

//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

//*****************************************************************************
//
// Interrupt priority definitions.  The top 3 bits of these values are
// significant with lower values indicating higher priority interrupts.
//
//*****************************************************************************
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

//*****************************************************************************
//
// SSI tag indices for each entry in the g_pcSSITags array.
//
//*****************************************************************************
#define SSI_INDEX_BOARDNAME  	0
#define SSI_INDEX_FORMVARS  	1
#define SSI_INDEX_IP_ADDRESS  	2
#define SSI_FIRMWARE_VERSION 	3
#define SSI_INDEX_MAC_ADDRESS 	4
#define SSI_READ1 	5
#define SSI_READ2 	6
#define SSI_READ3 	7
#define SSI_READ4 	8
#define SSI_WRITE1 	9
#define SSI_WRITE2 	10
#define SSI_WRITE3 	11
#define SSI_WRITE4 	12
#define SSI_ALL1 	13
#define SSI_ALL2 	14
#define SSI_ALL3 	15
#define SSI_ALL4 	16
#define SSI_RCVD1 	17
#define SSI_RCVD2 	18
#define SSI_RCVD3 	19
#define SSI_RCVD4 	20
#define SSI_RCVD5 	21
#define SSI_RCVD6 	22
#define SSI_RCVD7 	23
#define SSI_RCVD8 	24
#define SSI_RCVD9 	25
#define SSI_RCVD10 	26
#define SSI_RCVD11 	27
#define SSI_RCVD12 	28
#define SSI_TRNS1 	29
#define SSI_TRNS2 	30
#define SSI_TRNS3 	31
#define SSI_TRNS4 	32
#define SSI_TRNS5 	33
#define SSI_TRNS6 	34
#define SSI_TRNS7 	35
#define SSI_TRNS8 	36
#define SSI_TRNS9 	37
#define SSI_TRNS10 	38
#define SSI_TRNS11 	39
#define SSI_TRNS12 	40


//*****************************************************************************
//
// This array holds all the strings that are to be recognized as SSI tag
// names by the HTTPD server.  The server will call SSIHandler to request a
// replacement string whenever the pattern <!--#tagname--> (where tagname
// appears in the following array) is found in ".ssi", ".shtml" or ".shtm"
// files that it serves.
//
//*****************************************************************************
static const char *g_pcConfigSSITags[] =
{
    "myName",    	// SSI_INDEX_BOARDNAME
    "FormVars", 	// SSI_INDEX_FORMVARS
    "ipadrs",		// SSI_INDEX_IP_ADDRESS
    "fwVer",		// SSI_FIRMWARE_VERSION
    "macadrs",		// SSI_INDEX_MAC_ADDRESS
	"Read1",		//	SSI_READ1
	"Read2",		//	SSI_READ2
	"Read3",		//	SSI_READ3
	"Read4",		//	SSI_READ3
	"Write1",		//	SSI_WRITE1
	"Write2",		//	SSI_WRITE2
	"Write3",		//	SSI_WRITE3
	"Write4",		//	SSI_WRITE3
	"All1",			//	SSI_ALL1
	"All2",			//	SSI_ALL2
	"All3",			//	SSI_ALL3
	"All4",			//	SSI_ALL3
	"Rcvd1",		//	SSI_RCVD1
	"Rcvd2",		//	SSI_RCVD2
	"Rcvd3",		//	SSI_RCVD3
	"Rcvd4",		//	SSI_RCVD4
	"Rcvd5",		//	SSI_RCVD5
	"Rcvd6",		//	SSI_RCVD6
	"Rcvd7",		//	SSI_RCVD7
	"Rcvd8",		//	SSI_RCVD8
	"Rcvd9",		//	SSI_RCVD9
	"Rcvd10",		//	SSI_RCVD10
	"Rcvd11",		//	SSI_RCVD11
	"Rcvd12",		//	SSI_RCVD12
	"Trns1",		//	SSI_TRNS1
	"Trns2",		//	SSI_TRNS2
	"Trns3",		//	SSI_TRNS3
	"Trns4",		//	SSI_TRNS4
	"Trns5",		//	SSI_TRNS5
	"Trns6",		//	SSI_TRNS6
	"Trns7",		//	SSI_TRNS7
	"Trns8",		//	SSI_TRNS8
	"Trns9",		//	SSI_TRNS9
	"Trns10",		//	SSI_TRNS10
	"Trns11",		//	SSI_TRNS11
	"Trns12",		//	SSI_TRNS12
};


typedef struct __attribute__((__packed__)) {
	unsigned char checksum;
	unsigned char msgType;
	unsigned char slaveAddress;
	unsigned char withStop;
	short buffLen;
	char buff[1024];
}  IO_Ext_Msg;

#define IO_EXT_MSG_HEADER_LEN 6

enum IO_Ext_Msgtype
{
	INIT = 0,
	INIT_IIC = 1,
	WRITE_BYTES = 2,
	READ_BYTES = 3,
	WRITE_AND_READ_BYTE = 4,
	WAKEUP_IIC = 5,
	WRITE_OK = 6,
	READ_OK = 7,
	WAKEUP_OK = 8,
	SLAVE_NOT_READY = 9,
	WRITE_ERROR = 10,
	READ_ERROR = 11,
	NUM_MESSAGE_TYPES = 12,
} ;


typedef enum
{
	NUM_COMMANDS_RECEIVED = 0,
	NUM_OK_RETURNED = 1,
	NUM_ERRORS_RETURNED = 2,
	NUM_CHECKSUM_ERRORS = 3,
	NUM_STATISTIC_ITEMS = 4,
} ETHIC_statisticsItem;

typedef enum
{
	I2CMCS_OK = 0,
	I2CMCS_TIMEOUT = -1,
	I2CMCS_NO_ACK_FROM_SLAVE = -2,
	I2CMCS_WRITE_ERROR = -3,
	I2CMCS_READ_ERROR = -4,
	I2CMCS_ARBITRATION_LOST = -5,
	I2CMCS_UNKNOWN_ERROR = -6,
} I2CMCS_status ;

IO_Ext_Msg ioExtMsg;

int g_readStatistics[NUM_STATISTIC_ITEMS];
int g_writeStatistics[NUM_STATISTIC_ITEMS];
int g_totalStatistics[NUM_STATISTIC_ITEMS];
int g_returnedMessagesStatistics[NUM_MESSAGE_TYPES];
int g_receivedMessagesStatistics[NUM_MESSAGE_TYPES];


//*****************************************************************************
//
// The number of individual SSI tags that the HTTPD server can expect to
// find in our configuration pages.
//
//*****************************************************************************
#define NUM_CONFIG_SSI_TAGS     (sizeof(g_pcConfigSSITags) / sizeof (char *))

//*****************************************************************************
//
// Prototypes for the various CGI handler functions.
//
//*****************************************************************************
static char *ControlCGIHandler(int32_t iIndex, int32_t i32NumParams,
                               char *pcParam[], char *pcValue[]);
static char *SetTextCGIHandler(int32_t iIndex, int32_t i32NumParams,
                               char *pcParam[], char *pcValue[]);

//*****************************************************************************
//
// Prototype for the main handler used to process server-side-includes for the
// application's web-based configuration screens.
//
//*****************************************************************************
static int32_t SSIHandler(int32_t iIndex, char *pcInsert, int32_t iInsertLen);

//*****************************************************************************
//
// CGI URI indices for each entry in the g_psConfigCGIURIs array.
//
//*****************************************************************************
#define CGI_INDEX_CONTROL       0
#define CGI_INDEX_TEXT          1

//*****************************************************************************
//
// This array is passed to the HTTPD server to inform it of special URIs
// that are treated as common gateway interface (CGI) scripts.  Each URI name
// is defined along with a pointer to the function which is to be called to
// process it.
//
//*****************************************************************************
static const tCGI g_psConfigCGIURIs[] =
{
    { "/iocontrol.cgi", (tCGIHandler)ControlCGIHandler }, // CGI_INDEX_CONTROL
//    { "/settxt.cgi", (tCGIHandler)SetTextCGIHandler }     // CGI_INDEX_TEXT
};

//*****************************************************************************
//
// The number of individual CGI URIs that are configured for this system.
//
//*****************************************************************************
#define NUM_CONFIG_CGI_URIS     (sizeof(g_psConfigCGIURIs) / sizeof(tCGI))

//*****************************************************************************
//
// The file sent back to the browser by default following completion of any
// of our CGI handlers.  Each individual handler returns the URI of the page
// to load in response to it being called.
//
//*****************************************************************************
#define DEFAULT_CGI_RESPONSE    "/io_cgi.ssi"

//*****************************************************************************
//
// The file sent back to the browser in cases where a parameter error is
// detected by one of the CGI handlers.  This should only happen if someone
// tries to access the CGI directly via the broswer command line and doesn't
// enter all the required parameters alongside the URI.
//
//*****************************************************************************
#define PARAM_ERROR_RESPONSE    "/perror.htm"

#define JAVASCRIPT_HEADER                                                     \
    "<script type='text/javascript' language='JavaScript'><!--\n"
#define JAVASCRIPT_FOOTER                                                     \
    "//--></script>\n"

//*****************************************************************************
//
// Timeout for DHCP address request (in seconds).
//
//*****************************************************************************
#ifndef DHCP_EXPIRE_TIMER_SECS
#define DHCP_EXPIRE_TIMER_SECS  45
#endif

//*****************************************************************************
//
// The current name.
//
//*****************************************************************************
char g_myName[30] = MY_NAME;

//*****************************************************************************
//
// Clock devider for I2C speed. It is saved and used for I2C delays.
//
//*****************************************************************************
int g_I2C_devider;

//*****************************************************************************
//
// A flag to indicate busy for each function.
//
//*****************************************************************************
volatile uint8_t g_I2C_busy[NUM_I2C_CHANNELS];
volatile uint8_t g_A2D_busy[NUM_A2D_CHANNELS];
volatile uint8_t g_D2A_busy[NUM_D2A_CHANNELS];
volatile uint8_t g_RS232_busy[NUM_RS232_CHANNELS];
volatile uint8_t g_CAN_busy[NUM_CAN_CHANNELS];
volatile uint8_t g_GPIO_busy[NUM_GPIO_CHANNELS];

//*****************************************************************************
//
// The current IP address.
//
//*****************************************************************************
uint32_t g_ui32IPAddress;

//*****************************************************************************
//
// The current MAC address.
//
//*****************************************************************************
uint8_t g_pui8MACArray[8];
//*****************************************************************************
//
// The system clock frequency.
//
//*****************************************************************************
uint32_t g_ui32SysClock;
uint32_t g_ui32SysClockmS;
uint32_t g_ui32SysClockuS;

//*****************************************************************************
//
// Flag to note sw update needs to start
//
//*****************************************************************************
volatile char g_flagSWUpdate;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests iocontrol.cgi.
//
//*****************************************************************************
static char *
ControlCGIHandler(int32_t iIndex, int32_t i32NumParams, char *pcParam[],
                  char *pcValue[])
{
    int32_t i32boardName;
    bool bParamError;

    //
    // We have not encountered any parameter errors yet.
    //
    bParamError = false;

    //
    // Get each of the expected parameters.
    //
    i32boardName = FindCGIParameter("boardName", pcParam, i32NumParams);
//    i32Speed = GetCGIParam("speed_percent", pcParam, pcValue, i32NumParams, &bParamError);

    //
    // Was there any error reported by the parameter parser?
    //
    if(bParamError )
    {
        return(PARAM_ERROR_RESPONSE);
    }

    //
    // We got all the parameters and the values were within the expected ranges
    // so go ahead and make the changes.
    //
    if (i32boardName != -1)
    	strcpy(g_myName, pcParam[0] );

    //
    // Send back the default response page.
    //
    return(DEFAULT_CGI_RESPONSE);
}

//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests settxt.cgi.
//
//*****************************************************************************
static char *
SetTextCGIHandler(int32_t i32Index, int32_t i32NumParams, char *pcParam[],
                  char *pcValue[])
{
    long lStringParam;
    char pcDecodedString[48];

    //
    // Find the parameter that has the string we need to display.
    //
    lStringParam = FindCGIParameter("DispText", pcParam, i32NumParams);

    //
    // If the parameter was not found, show the error page.
    //
    if(lStringParam == -1)
    {
        return(PARAM_ERROR_RESPONSE);
    }

    //
    // The parameter is present. We need to decode the text for display.
    //
    DecodeFormString(pcValue[lStringParam], pcDecodedString, 48);

    //
    // Print sting over the UART
    //
    PRINT_ON_DEBUG(pcDecodedString);
    PRINT_ON_DEBUG("\n");

    //
    // Tell the HTTPD server which file to send back to the client.
    //
    return(DEFAULT_CGI_RESPONSE);
}

//*****************************************************************************
//
// This function is called by the HTTP server whenever it encounters an SSI
// tag in a web page.  The iIndex parameter provides the index of the tag in
// the g_pcConfigSSITags array. This function writes the substitution text
// into the pcInsert array, writing no more than iInsertLen characters.
//
//*****************************************************************************
static int32_t
SSIHandler(int32_t iIndex, char *pcInsert, int32_t iInsertLen)
{
    //
    // Which SSI tag have we been passed?
    //
	PRINT_ON_DEBUG("SSIHandler tag %d len %d\n", iIndex, iInsertLen);
    switch(iIndex)
    {
        case SSI_INDEX_BOARDNAME:
        	strcpy(pcInsert, g_myName);
//            io_get_ledstate(pcInsert, iInsertLen);
            break;

        case SSI_INDEX_FORMVARS:
            usnprintf(pcInsert, iInsertLen,
//                    "%sls=%d;\nsp=%s;\n%s",
                    "%s;\nsp=%s;\n%s",
                    JAVASCRIPT_HEADER,
//                    io_is_led_on(),
                    "10.0.0.131",
//                    io_get_animation_speed(),
                    JAVASCRIPT_FOOTER);
            break;

        case SSI_INDEX_IP_ADDRESS:
            //
            // Convert the IP Address into a string.
            //
            usprintf(pcInsert, "%d.%d.%d.%d", g_ui32IPAddress & 0xff, (g_ui32IPAddress >> 8) & 0xff,
                    (g_ui32IPAddress >> 16) & 0xff, (g_ui32IPAddress >> 24) & 0xff);

//        	sprintf(pcInsert, "10.0.0.131");
//            io_get_animation_speed_string(pcInsert, iInsertLen);
            break;

        case SSI_INDEX_MAC_ADDRESS:
            //
            // Convert the MAC Address into a string.
            //
            usprintf(pcInsert, "%02x-%002x-%02x-%02x-%02x-%02x", g_pui8MACArray[0], g_pui8MACArray[1], g_pui8MACArray[2], g_pui8MACArray[3], g_pui8MACArray[4], g_pui8MACArray[5]);
            break;
        case SSI_FIRMWARE_VERSION:
        	usprintf(pcInsert, "%s", FW_VERSION);
        	break;
        case SSI_READ1:
        	usprintf(pcInsert, "%d", g_readStatistics[0]);
        	break;
        case SSI_READ2:
        	usprintf(pcInsert, "%d", g_readStatistics[1]);
        	break;
        case SSI_READ3:
        	usprintf(pcInsert, "%d", g_readStatistics[2]);
        	break;
        case SSI_READ4:
        	usprintf(pcInsert, "%d", g_readStatistics[3]);
        	break;
        case SSI_WRITE1:
        	usprintf(pcInsert, "%d", g_writeStatistics[0]);
        	break;
        case SSI_WRITE2:
        	usprintf(pcInsert, "%d", g_writeStatistics[1]);
        	break;
        case SSI_WRITE3:
        	usprintf(pcInsert, "%d", g_writeStatistics[2]);
        	break;
        case SSI_WRITE4:
        	usprintf(pcInsert, "%d", g_writeStatistics[3]);
        	break;
        case SSI_ALL1:
        	usprintf(pcInsert, "%d", g_totalStatistics[0]);
        	break;
        case SSI_ALL2:
        	usprintf(pcInsert, "%d", g_totalStatistics[1]);
        	break;
        case SSI_ALL3:
        	usprintf(pcInsert, "%d", g_totalStatistics[2]);
        	break;
        case SSI_ALL4:
        	usprintf(pcInsert, "%d", g_totalStatistics[3]);
        	break;
        case SSI_RCVD1:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[0]);
        	break;
        case SSI_RCVD2:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[1]);
        	break;
        case SSI_RCVD3:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[2]);
        	break;
        case SSI_RCVD4:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[3]);
        	break;
        case SSI_RCVD5:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[4]);
        	break;
        case SSI_RCVD6:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[5]);
        	break;
        case SSI_RCVD7:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[6]);
        	break;
        case SSI_RCVD8:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[7]);
        	break;
        case SSI_RCVD9:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[8]);
        	break;
        case SSI_RCVD10:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[9]);
        	break;
        case SSI_RCVD11:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[10]);
        	break;
        case SSI_RCVD12:
        	usprintf(pcInsert, "%d", g_receivedMessagesStatistics[11]);
        	break;
        case SSI_TRNS1:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[0]);
        	break;
        case SSI_TRNS2:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[1]);
        	break;
        case SSI_TRNS3:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[2]);
        	break;
        case SSI_TRNS4:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[3]);
        	break;
        case SSI_TRNS5:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[4]);
        	break;
        case SSI_TRNS6:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[5]);
        	break;
        case SSI_TRNS7:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[6]);
        	break;
        case SSI_TRNS8:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[7]);
        	break;
        case SSI_TRNS9:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[8]);
        	break;
        case SSI_TRNS10:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[9]);
        	break;
        case SSI_TRNS11:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[10]);
        	break;
        case SSI_TRNS12:
        	usprintf(pcInsert, "%d", g_returnedMessagesStatistics[11]);
        	break;

       default:
            usnprintf(pcInsert, iInsertLen, "??");
            break;
    }

	PRINT_ON_DEBUG("SSIHandler result %s\n", pcInsert);

    //
    // Tell the server how many characters our insert string contains.
    //
    return(strlen(pcInsert));
}
//*****************************************************************************
//
// Display an lwIP type IP Address.
//
//*****************************************************************************
void
DisplayIPAddress(uint32_t ui32Addr)
{
    char pcBuf[16];

    //
    // Convert the IP Address into a string.
    //
    usprintf(pcBuf, "%d.%d.%d.%d", ui32Addr & 0xff, (ui32Addr >> 8) & 0xff,
            (ui32Addr >> 16) & 0xff, (ui32Addr >> 24) & 0xff);

    //
    // Display the string.
    //
    PRINT_ON_DEBUG(pcBuf);
}


// my update callback function.

void
SoftwareUpdateCallback(void)
{
    PRINT_ON_DEBUG("UpdateCallback\n");
    g_flagSWUpdate = 1;
//    HWREGBITW(&g_ulFlags, FLAG_SWUPDATE) = 1;
}


//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************
void
lwIPHostTimerHandler(void)
{
    uint32_t ui32Idx, ui32NewIPAddress;

    //
    // Get the current IP address.
    //
    ui32NewIPAddress = lwIPLocalIPAddrGet();

    //
    // See if the IP address has changed.
    //
    if(ui32NewIPAddress != g_ui32IPAddress)
    {
        //
        // See if there is an IP address assigned.
        //
        if(ui32NewIPAddress == 0xffffffff)
        {
            //
            // Indicate that there is no link.
            //
            PRINT_ON_DEBUG("Waiting for link.\n");
        }
        else if(ui32NewIPAddress == 0)
        {
            //
            // There is no IP address, so indicate that the DHCP process is
            // running.
            //
            PRINT_ON_DEBUG("Waiting for IP address.\n");
        }
        else
        {
            //
            // Display the new IP address.
            //
            PRINT_ON_DEBUG("IP Address: ");
            DisplayIPAddress(ui32NewIPAddress);
            PRINT_ON_DEBUG("\nOpen a browser and enter the IP address.\n");

            //
			// SoftwareUpdateInit
			//
			PRINT_ON_DEBUG("Calling UpdateInit\n");
			SoftwareUpdateInit(SoftwareUpdateCallback);
        }

        //
        // Save the new IP address.
        //
        g_ui32IPAddress = ui32NewIPAddress;

        //
        // Turn GPIO off.
        //
        MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    }

    //
    // If there is not an IP address.
    //
    if((ui32NewIPAddress == 0) || (ui32NewIPAddress == 0xffffffff))
    {
        //
        // Loop through the LED animation.
        //

        for(ui32Idx = 1; ui32Idx < 17; ui32Idx++)
        {

            //
            // Toggle the GPIO
            //
            MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,
                    (MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) ^
                     GPIO_PIN_1));

            SysCtlDelay(g_ui32SysClock/(ui32Idx << 1));
        }
    }
}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);
    g_ui32SysClockmS += SYSTICKMS;
}

void sleep(int mSec)
{
	uint32_t startTime = g_ui32SysClockmS;
	while ((g_ui32SysClockmS - startTime) < mSec);
}

void sleepI2C()
{
	int i;
	for (i=0; i<10*g_I2C_devider; i++);
//	for (i=0; i<1000; i++);
}

struct udp_pcb * inUDP;
struct udp_pcb * outUDP;
int lSocket;
//struct sockaddr_in sLocalAddr, sDestAddr;

unsigned char calcChecksum(IO_Ext_Msg msg, bool withData)
{
	unsigned char checkSum = 0;
	checkSum += msg.msgType;
	checkSum += msg.slaveAddress;
	checkSum += msg.withStop;
	checkSum += msg.buffLen >> 8;
	checkSum += msg.buffLen & 0x00ff;

	int i;
	if (withData)
	{
		for (i=0; i<msg.buffLen; i++)
		{
			checkSum += msg.buff[i];
		}
	}

	return checkSum;
}

void
z_I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32I2CClk, int speed, bool bFast)
{
    uint32_t ui32SCLFreq;
    uint32_t ui32TPR;

    //
    // Check the arguments.
    //
//    ASSERT(_I2CBaseValid(ui32Base));

    //
    // Must enable the device before doing anything else.
    //
    I2CMasterEnable(ui32Base);

    //
    // Get the desired SCL speed.
    //
    if(bFast == true)
    {
        ui32SCLFreq = 400000;
    }
    else
    {
        ui32SCLFreq = 100000;
    }

    //
    // Compute the clock divider that achieves the fastest speed less than or
    // equal to the desired speed.  The numerator is biased to favor a larger
    // clock divider so that the resulting clock is always less than or equal
    // to the desired clock, never greater.
    //
    ui32TPR = ((ui32I2CClk + (2 * 10 * ui32SCLFreq) - 1) /
               (2 * 10 * ui32SCLFreq)) - 1;
    HWREG(ui32Base + I2C_O_MTPR) = ui32TPR;

    //
    // Check to see if this I2C peripheral is High-Speed enabled.  If yes, also
    // choose the fastest speed that is less than or equal to 3.4 Mbps.
    //
    if(HWREG(ui32Base + I2C_O_PP) & I2C_PP_HS)
    {

 //       ui32TPR = ((ui32I2CClk + (2 * 3 * 340000) - 1) / (2 * 3 * 340000)) - 1;
    	if (0 == speed)
    		g_I2C_devider = 59;
    	else if (1 == speed)
    		g_I2C_devider = 14;

    	HWREG(ui32Base + I2C_O_MTPR) = g_I2C_devider | I2C_MASTER_GLITCH_FILTER_4; // I2C_MTPR_HS | ui32TPR;
    }
}

int g_pos = 0;
int g_pos2 = 0;
int g_lastErr = 0;
int g_lastErr2 = 0;

// Wait for the IIC bus to clear
I2CMCS_status waitIICBus()
{

	// Wait for the current byte to be transmitted. Then whait for the
	// I2C master to be idle.
	uint32_t startTime = g_ui32SysClockmS;
	uint32_t now = g_ui32SysClockmS;
	while ( !(I2CMasterIntStatusEx(I2C0_BASE, false) & I2C_MASTER_INT_DATA) )
	{
		// limit the wait, not to get stuck on this
		now = g_ui32SysClockmS;
		if( (now-startTime) > IIC_TIMEOUT )
		{
			return I2CMCS_TIMEOUT;
			break;
		}
	};

	startTime = g_ui32SysClockmS;

	while (   I2CMasterBusy(I2C0_BASE)  )
	{
		// limit the wait, not to get stuck on this
		now = g_ui32SysClockmS;
		if( (now-startTime) > IIC_TIMEOUT )
		{
			return I2CMCS_TIMEOUT;
			break;
		}
	}

	// Check for I2C Errors by reading raw interrupt status
	int NAckState = (I2CMasterIntStatusEx(I2C0_BASE, false) & I2C_MASTER_INT_NACK);
	I2CMasterIntClearEx(I2C0_BASE, I2C_MASTER_INT_NACK|I2C_MASTER_INT_DATA);
	// I2C status can not be used, because the NACK status will be overwritten
	// by the I2CMasterBusy() function.

	if (NAckState)
		return I2CMCS_NO_ACK_FROM_SLAVE;
	else
		return I2CMCS_OK;
}
#if 0
	uint32_t startTime = g_ui32SysClockmS;
	uint32_t now = g_ui32SysClockmS;

	int err;

	g_pos2 = 0;

	// Wait until busy first
	int i;
	for (i=0; i<120; i++)
	{

	}
/*
	while (true)
	{
		g_pos2 = 0;
		err = HWREG(I2C0_BASE + I2C_O_MCS);
		g_lastErr = err;
		if (err & I2C_MCS_ADRACK)
			return I2CMCS_NO_ADDR_ACK_FROM_SLAVE;
		g_pos2=1;

		// If data
		if (err & I2C_MCS_DATACK)
			return I2CMCS_NO_DATA_ACK_FROM_SLAVE;
		g_pos2=2;

		if (err & I2C_MCS_CLKTO)
			return I2CMCS_TIMEOUT;
		g_pos2=3;


		if (err & I2C_MCS_ERROR)
		{
			if (err & I2C_MCS_ARBLST)
				return I2CMCS_ARBITRATION_LOST;
			else
				return I2CMCS_UNKNOWN_ERROR;
		}
		g_pos2=4;

		if ((err & I2C_MCS_BUSY))
			break;
		g_pos2=5;

		// limit the wait, not to get stuck on this
		now = g_ui32SysClockmS;
		if( (now-startTime) > IIC_TIMEOUT )
		{
			g_pos2 = 6;
			break;
		}
	}
*/
	g_pos = 100;

	startTime = g_ui32SysClockmS;
//	sleepI2C();
	while (true)
	{
		g_pos = 100;
		err = HWREG(I2C0_BASE + I2C_O_MCS);
		g_lastErr2 = err;
		if (err & I2C_MCS_ADRACK)
			return I2CMCS_NO_ACK_FROM_SLAVE;
		g_pos=101;

		// If data
		if (err & I2C_MCS_DATACK)
			return I2CMCS_NO_ACK_FROM_SLAVE;
		g_pos=102;

		if (err & I2C_MCS_CLKTO)
			return I2CMCS_TIMEOUT;
		g_pos=103;

		if (err & I2C_MCS_ARBLST)
			return I2CMCS_ARBITRATION_LOST;
		g_pos=104;
		if (err & I2C_MCS_ERROR)
		{
			return I2CMCS_UNKNOWN_ERROR;
		}

		if (!(err & I2C_MCS_BUSY))
		{
			g_pos=105;
			if (checkNack)
			{
				// Check for I2C Errors by reading raw interrupt status
				if (I2CMasterIntStatusEx(I2C0_BASE, false) & I2C_MASTER_INT_NACK)
				{
					I2CMasterIntClearEx(I2C0_BASE, I2C_MASTER_INT_NACK|I2C_MASTER_INT_DATA);
					return I2CMCS_NO_ACK_FROM_SLAVE;
				}
			}

			g_pos=106;

			return I2CMCS_OK;
		}

		now = g_ui32SysClockmS;
		if( (now-startTime) > IIC_TIMEOUT )
			return I2CMCS_TIMEOUT;
	}
}

#endif

int g_errCount = 0;
int g_okCount = 0;

I2CMCS_status i2c0_write(unsigned char addr, char* txData, unsigned char N, bool withStop)
{
	I2CMCS_status rc;
	int i;

	if ( g_I2C_busy[0] )
		return I2CMCS_WRITE_ERROR;

	g_I2C_busy[0] = 1;

	// Write slave address to I2CMSA
	I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // false means write

	if (N==1)
	{
		I2CMasterDataPut(I2C0_BASE, txData[0]);

		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
		rc = waitIICBus();
		if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; return rc; }
	}

	else if (N>1)
	{
		//put data to I2CMDR
		I2CMasterDataPut(I2C0_BASE, txData[0]);

		//Initiate send of data from the MCU
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
		rc = waitIICBus();

		if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP); return rc; }

		for (i=1; i<N-1; i++)
		{
			//put data to I2CMDR
			I2CMasterDataPut(I2C0_BASE, txData[i]);

//			IntMasterDisable();
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
			rc = waitIICBus();
//			IntMasterEnable();
			if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP); return rc; }
		}

		// Send last byte
		//put data to I2CMDR
		I2CMasterDataPut(I2C0_BASE, txData[i]);

		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
		rc = waitIICBus();
		if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP); return rc; }
	}

	g_I2C_busy[0] = 0;
	return I2CMCS_OK;
}

I2CMCS_status i2c0_read(unsigned char addr, char* rxData, unsigned char N, bool withStop)
{
	uint8_t i;
	I2CMCS_status rc;

	if ( g_I2C_busy[0] )
		return I2CMCS_READ_ERROR;

	g_I2C_busy[0] = 1;


	I2CMasterSlaveAddrSet(I2C0_BASE, addr, true);

	if (N==1)
    {
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
		rc = waitIICBus();
		if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; return rc; }

		// read the data
		rxData[0]=I2CMasterDataGet(I2C0_BASE);
    }
	else if (N>1)
	{

//		IntMasterDisable();
		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		rc = waitIICBus();
//		IntMasterEnable();
		if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP); return rc; }

		rxData[0]=I2CMasterDataGet(I2C0_BASE);

		for (i=1; i<N-1; i++)
		{
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
			rc = waitIICBus();
			if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP); return rc; }

			rxData[i]=I2CMasterDataGet(I2C0_BASE);
		}

		// read last byte
//		if (withStop)
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
//		else
//			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

		rc = waitIICBus();
		if (rc!=I2CMCS_OK) { g_I2C_busy[0] = 0; I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP); return rc; }

		rxData[i]=I2CMasterDataGet(I2C0_BASE);
	}

	g_I2C_busy[0] = 0;
	return I2CMCS_OK;
}

void wakeupI2CBus()
{
	if ( g_I2C_busy[0] )
		return ;

	g_I2C_busy[0] = 1;

//	// Set the IO to 1 on both pins
//	GPIO_PORTB_AHB_DATA_R |= (0x04);
//	GPIO_PORTB_AHB_DATA_R |= (0x08);

	// Disable I2C master
    I2CMasterDisable(I2C0_BASE);

	// set I2C pins as IO
	GPIOPinConfigure(0x00010800);
	GPIOPinConfigure(0x00010C00);

	GPIO_PORTB_AHB_DIR_R = 0x0C;
	GPIO_PORTB_AHB_DEN_R = 0x0C;

	GPIO_PORTB_AHB_AFSEL_R = 0;

	GPIO_PORTB_AHB_DATA_R |= (0x04);
	GPIO_PORTB_AHB_DATA_R |= (0x08);
	sleepI2C();

	GPIO_PORTB_AHB_DATA_R &= ~(0x04);
	sleepI2C();
	GPIO_PORTB_AHB_DATA_R &= ~(0x08);
	sleepI2C();

	int i;
	for (i=0; i<10; i++)
	{
		GPIO_PORTB_AHB_DATA_R |= (0x04);
		sleepI2C();
		GPIO_PORTB_AHB_DATA_R &= ~(0x04);
		sleepI2C();

		if (i==6)
		{
			GPIO_PORTB_AHB_DATA_R |= (0x08);
		}
	}

	sleep(1);

	// restore I2C settings
	GPIOPinConfigure(GPIO_PB2_I2C0SCL); // 0x00010802
	GPIOPinConfigure(GPIO_PB3_I2C0SDA); // 0x00010C02

	//
	// Select the I2C function for these pins.  This function will also
	// configure the GPIO pins pins for I2C operation, setting them to
	// open-drain operation with weak pull-ups.  Consult the data sheet
	// to see which functions are allocated per pin.
	// TODO: change this to select the port/pin you are using.
	//
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

    I2CMasterEnable(I2C0_BASE);

	g_I2C_busy[0] = 0;

}

void udpReceiver(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{

	err_t err;

//	struct ip_addr forward_ip;
//	IP4_ADDR(&forward_ip, 255, 255, 255, 255);

	g_totalStatistics[NUM_COMMANDS_RECEIVED]++;

	PRINT_ON_DEBUG("UDP message received on port %d.\n", IN_PORT);
	if (p != NULL) {
		memcpy(&ioExtMsg, p->payload, p->len);

		if (ioExtMsg.msgType == INIT)
		{
			g_receivedMessagesStatistics[ioExtMsg.msgType]++;

			// Check data, discard if not received right
			if (calcChecksum(ioExtMsg, true) != ioExtMsg.checksum)
			{
				PRINT_ON_DEBUG("Checksum error in INIT, sb %d is %d\n", ioExtMsg.checksum, calcChecksum(ioExtMsg, true));
				pbuf_free(p);
				g_totalStatistics[NUM_CHECKSUM_ERRORS]++;
				return;
			}

			ioExtMsg.buff[ioExtMsg.buffLen] = 0;
			PRINT_ON_DEBUG("UDP message INIT with name %s\n", ioExtMsg.buff) ;
			if (!strcmp(ioExtMsg.buff, g_myName)){
				ioExtMsg.buffLen = 0;
				struct pbuf* outBuff = pbuf_alloc(PBUF_TRANSPORT, IO_EXT_MSG_HEADER_LEN, PBUF_RAM);
				memcpy (outBuff->payload, &ioExtMsg, IO_EXT_MSG_HEADER_LEN);

				err = udp_sendto(pcb, outBuff, addr/*IP_ADDR_BROADCAST*/, OUT_PORT); //dest port
				PRINT_ON_DEBUG("UDP message sent on port %d. err = %d\n", OUT_PORT, err);
				pbuf_free(outBuff); //De-allocate packet buffer
				g_returnedMessagesStatistics[ioExtMsg.msgType]++;
				g_totalStatistics[NUM_OK_RETURNED]++;
			}
		}
		else if (ioExtMsg.msgType == INIT_IIC)
		{
			g_receivedMessagesStatistics[ioExtMsg.msgType]++;

			// Check data, discard if not received right
			if (calcChecksum(ioExtMsg, false) != ioExtMsg.checksum)
			{
				PRINT_ON_DEBUG("Checksum error in INIT_IIC\n");
				pbuf_free(p);
				g_totalStatistics[NUM_CHECKSUM_ERRORS]++;
				return;
			}

			int speed = ioExtMsg.slaveAddress;
			PRINT_ON_DEBUG("UDP message INIT_IIC with speed %d\n", speed) ;
			// Init I2C device
			switch (speed) {
			case 0:
				z_I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), 0, false);
				PRINT_ON_DEBUG("IIC Speed set to 100kBaud\n") ;
				break;
			case 1:
				z_I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), 1, false);
				PRINT_ON_DEBUG("IIC Speed set to 400kBaud\n") ;
				break;
			case 2:
			case 3:
			default:
//					I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
//				I2CMasterInitExpClk(I2C0_BASE, g_ui32SysClock, true);
				z_I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), 1, false);
				PRINT_ON_DEBUG("IIC Speed set to 400kBaud\n") ;
				break;
			}

			ioExtMsg.buffLen = 0;
			struct pbuf* outBuff = pbuf_alloc(PBUF_TRANSPORT, IO_EXT_MSG_HEADER_LEN, PBUF_RAM);
			memcpy (outBuff->payload, &ioExtMsg, IO_EXT_MSG_HEADER_LEN);

			err = udp_sendto(pcb, outBuff, addr/*IP_ADDR_BROADCAST*/, OUT_PORT); //dest port
			PRINT_ON_DEBUG("UDP message sent on port %d. err = %d\n", OUT_PORT, err);
			pbuf_free(outBuff); //De-allocate packet buffer
			g_returnedMessagesStatistics[ioExtMsg.msgType]++;
			g_totalStatistics[NUM_OK_RETURNED]++;

		}

		else if (ioExtMsg.msgType == READ_BYTES)
		{
			g_receivedMessagesStatistics[ioExtMsg.msgType]++;
			g_readStatistics[NUM_COMMANDS_RECEIVED]++;
			PRINT_ON_DEBUG("UDP message READ_BYTES num of bytes %d slave address %d\n", ioExtMsg.buffLen, ioExtMsg.slaveAddress) ;

			// Check data, discard if not received right
			if (calcChecksum(ioExtMsg, false) != ioExtMsg.checksum)
			{
				PRINT_ON_DEBUG("Checksum error in READ_BYTES\n");
				pbuf_free(p);
				g_readStatistics[NUM_CHECKSUM_ERRORS]++;
				g_totalStatistics[NUM_CHECKSUM_ERRORS]++;
				return;
			}

			// Check parameters or busy state
			if ((ioExtMsg.buffLen <0) || (ioExtMsg.buffLen > 1000) )
			{
				ioExtMsg.msgType = READ_ERROR;
				PRINT_ON_DEBUG("UDP message READ_BYTES bufflen %d error \n", ioExtMsg.buffLen) ;
				ioExtMsg.buffLen = 0;
				g_readStatistics[NUM_ERRORS_RETURNED]++;
				g_totalStatistics[NUM_ERRORS_RETURNED]++;
			}
			else
			{
				I2CMCS_status rc = i2c0_read(ioExtMsg.slaveAddress, ioExtMsg.buff, ioExtMsg.buffLen, ioExtMsg.withStop);
				if ( (rc == I2CMCS_OK) && (ioExtMsg.buffLen == 3) && (ioExtMsg.buff[0] == 01) )
				{
					int stophere =1;
				}
				switch (rc)
				{
					case I2CMCS_OK:
						ioExtMsg.msgType = READ_OK;
						g_readStatistics[NUM_OK_RETURNED]++;
						g_totalStatistics[NUM_OK_RETURNED]++;
#ifdef READ_BYTES_PATCH
						if  (ioExtMsg.buffLen == 3)
						{
							if ( (ioExtMsg.buff[0] == ioExtMsg.buff[1]) && (ioExtMsg.buff[1] == ioExtMsg.buff[2]) && (ioExtMsg.buff[0] != 0) )
							{
								g_count++;
								g_lastVal = ioExtMsg.buff[0];
								while(g_stop){}
								ioExtMsg.msgType = SLAVE_NOT_READY;
								g_readStatistics[NUM_ERRORS_RETURNED]++;
								g_totalStatistics[NUM_ERRORS_RETURNED]++;
							}
						}
#endif
						break;
					case I2CMCS_NO_ACK_FROM_SLAVE:
					case I2CMCS_ARBITRATION_LOST:
						ioExtMsg.msgType = SLAVE_NOT_READY;
						g_readStatistics[NUM_ERRORS_RETURNED]++;
						g_totalStatistics[NUM_ERRORS_RETURNED]++;
						break;
					case I2CMCS_UNKNOWN_ERROR:
					case I2CMCS_TIMEOUT:
					case I2CMCS_READ_ERROR:
					default:
						ioExtMsg.msgType = READ_ERROR;
						g_readStatistics[NUM_ERRORS_RETURNED]++;
						g_totalStatistics[NUM_ERRORS_RETURNED]++;
						break;
				}
			}

			struct pbuf* outBuff = pbuf_alloc(PBUF_TRANSPORT, IO_EXT_MSG_HEADER_LEN + ioExtMsg.buffLen, PBUF_RAM);
			memcpy (outBuff->payload, &ioExtMsg, IO_EXT_MSG_HEADER_LEN + ioExtMsg.buffLen);

			err = udp_sendto(pcb, outBuff, addr/*IP_ADDR_BROADCAST*/, OUT_PORT); //dest port
			PRINT_ON_DEBUG("UDP message sent on port %d. err = %d\n", OUT_PORT, err);
			pbuf_free(outBuff); //De-allocate packet buffer
			g_returnedMessagesStatistics[ioExtMsg.msgType]++;

		}
		else if (ioExtMsg.msgType == WRITE_BYTES)
		{
			g_receivedMessagesStatistics[ioExtMsg.msgType]++;
			g_writeStatistics[NUM_COMMANDS_RECEIVED]++;
			PRINT_ON_DEBUG("UDP message WRITE_BYTES num of bytes %d slave address %d\n", ioExtMsg.buffLen, ioExtMsg.slaveAddress) ;

			if (calcChecksum(ioExtMsg, true) != ioExtMsg.checksum)
			{
				PRINT_ON_DEBUG("Checksum error in WRITE_BYTES\n");
				pbuf_free(p);
				g_writeStatistics[NUM_CHECKSUM_ERRORS]++;
				g_totalStatistics[NUM_CHECKSUM_ERRORS]++;
				return;
			}

			PRINT_ON_DEBUG("UDP message WRITE_BYTES num of bytes %d slave address %d\n", ioExtMsg.buffLen, ioExtMsg.slaveAddress) ;
			// Check parameters or busy state
			if ((ioExtMsg.buffLen <0) || (ioExtMsg.buffLen > 1000) )
			{
				ioExtMsg.msgType = WRITE_ERROR;
				PRINT_ON_DEBUG("UDP message WRITE_BYTES bufflen %d error \n", ioExtMsg.buffLen) ;
				ioExtMsg.buffLen = 0;
				g_writeStatistics[NUM_ERRORS_RETURNED]++;
				g_totalStatistics[NUM_ERRORS_RETURNED]++;
			}
			else
			{
				I2CMCS_status rc = i2c0_write(ioExtMsg.slaveAddress, ioExtMsg.buff, ioExtMsg.buffLen, ioExtMsg.withStop);

				switch (rc)
				{
					case I2CMCS_OK:
						ioExtMsg.msgType = WRITE_OK;
						g_writeStatistics[NUM_OK_RETURNED]++;
						g_totalStatistics[NUM_OK_RETURNED]++;
						break;
					case I2CMCS_NO_ACK_FROM_SLAVE:
					case I2CMCS_ARBITRATION_LOST:
						ioExtMsg.msgType = SLAVE_NOT_READY;
						g_writeStatistics[NUM_ERRORS_RETURNED]++;
						g_totalStatistics[NUM_ERRORS_RETURNED]++;
						break;
					case I2CMCS_TIMEOUT:
					case I2CMCS_WRITE_ERROR:
					case I2CMCS_UNKNOWN_ERROR:
					default:
						ioExtMsg.msgType = WRITE_ERROR;
						g_writeStatistics[NUM_ERRORS_RETURNED]++;
						g_totalStatistics[NUM_ERRORS_RETURNED]++;
						break;
				}
			}
			ioExtMsg.buffLen = 0;
			struct pbuf* outBuff = pbuf_alloc(PBUF_TRANSPORT, IO_EXT_MSG_HEADER_LEN, PBUF_RAM);
			memcpy (outBuff->payload, &ioExtMsg, IO_EXT_MSG_HEADER_LEN);

			err = udp_sendto(pcb, outBuff, addr/*IP_ADDR_BROADCAST*/, OUT_PORT); //dest port
			PRINT_ON_DEBUG("UDP message sent on port %d. err = %d\n", OUT_PORT, err);
			pbuf_free(outBuff); //De-allocate packet buffer
			g_returnedMessagesStatistics[ioExtMsg.msgType]++;

		}
		else if (ioExtMsg.msgType == WRITE_AND_READ_BYTE)
		{
			g_receivedMessagesStatistics[ioExtMsg.msgType]++;

			PRINT_ON_DEBUG("UDP message WRITE_AND_READ_BYTE slave address %d byte value %d\n", ioExtMsg.slaveAddress, ioExtMsg.buff[0]) ;

			if (calcChecksum(ioExtMsg, true) != ioExtMsg.checksum)
			{
				PRINT_ON_DEBUG("Checksum error in WRITE_AND_READ_BYTE\n");
				pbuf_free(p);
				g_totalStatistics[NUM_CHECKSUM_ERRORS]++;
				return;
			}

			I2CMCS_status rc = i2c0_write(ioExtMsg.slaveAddress, ioExtMsg.buff, 1, true);
			if (rc == I2CMCS_OK)
			{
				ioExtMsg.buff[0] = 0;
				rc = i2c0_read(ioExtMsg.slaveAddress, ioExtMsg.buff, 1, true);
			}

			switch (rc)
			{
				case I2CMCS_OK:
					ioExtMsg.msgType = WRITE_OK;
					g_totalStatistics[NUM_OK_RETURNED]++;
					break;
				case I2CMCS_NO_ACK_FROM_SLAVE:
				case I2CMCS_ARBITRATION_LOST:
					ioExtMsg.msgType = SLAVE_NOT_READY;
					g_totalStatistics[NUM_ERRORS_RETURNED]++;
				case I2CMCS_TIMEOUT:
				case I2CMCS_WRITE_ERROR:
				case I2CMCS_READ_ERROR:
				case I2CMCS_UNKNOWN_ERROR:
				default:
					ioExtMsg.msgType = WRITE_ERROR;
					g_totalStatistics[NUM_ERRORS_RETURNED]++;
					break;
			}

			ioExtMsg.buffLen = 1;
			struct pbuf* outBuff = pbuf_alloc(PBUF_TRANSPORT, IO_EXT_MSG_HEADER_LEN+ioExtMsg.buffLen, PBUF_RAM);
			memcpy (outBuff->payload, &ioExtMsg, IO_EXT_MSG_HEADER_LEN+ioExtMsg.buffLen);

			err = udp_sendto(pcb, outBuff, addr/*IP_ADDR_BROADCAST*/, OUT_PORT); //dest port
			PRINT_ON_DEBUG("UDP message sent on port %d. err = %d\n", OUT_PORT, err);
			pbuf_free(outBuff); //De-allocate packet buffer
			g_returnedMessagesStatistics[ioExtMsg.msgType]++;

		}
		else if (ioExtMsg.msgType == WAKEUP_IIC)
		{
			g_receivedMessagesStatistics[ioExtMsg.msgType]++;

			PRINT_ON_DEBUG("UDP message WAKEUP_IIC slave address %d byte value %d\n", ioExtMsg.slaveAddress, ioExtMsg.buff[0]) ;
			if (calcChecksum(ioExtMsg, false) != ioExtMsg.checksum)
			{
				PRINT_ON_DEBUG("Checksum error in WAKEUP_IIC\n");
				pbuf_free(p);
				g_totalStatistics[NUM_CHECKSUM_ERRORS]++;
				return;
			}

			wakeupI2CBus();

			ioExtMsg.msgType = WAKEUP_OK;
			ioExtMsg.buffLen = 0;
			ioExtMsg.buff[0] = 0;
			struct pbuf* outBuff = pbuf_alloc(PBUF_TRANSPORT, IO_EXT_MSG_HEADER_LEN, PBUF_RAM);
			memcpy (outBuff->payload, &ioExtMsg, IO_EXT_MSG_HEADER_LEN);

			err = udp_sendto(pcb, outBuff, addr, OUT_PORT); //dest port
			PRINT_ON_DEBUG("UDP message sent on port %d. err = %d\n", OUT_PORT, err);
			pbuf_free(outBuff); //De-allocate packet buffer
			g_returnedMessagesStatistics[ioExtMsg.msgType]++;
			g_totalStatistics[NUM_OK_RETURNED]++;

		}

		pbuf_free(p);
	}
}



int
main(void)
{
    uint32_t ui32User0, ui32User1;

    g_flagSWUpdate = 0;

    int i;

    // Init IO channels busy flags
    for (i=0; i<NUM_I2C_CHANNELS; i++) g_I2C_busy[i] = 0;
    for (i=0; i<NUM_A2D_CHANNELS; i++) g_A2D_busy[i] = 0;
    for (i=0; i<NUM_D2A_CHANNELS; i++) g_D2A_busy[i] = 0;
    for (i=0; i<NUM_RS232_CHANNELS; i++) g_RS232_busy[i] = 0;
    for (i=0; i<NUM_CAN_CHANNELS; i++) g_CAN_busy[i] = 0;
    for (i=0; i<NUM_GPIO_CHANNELS; i++) g_GPIO_busy[i] = 0;


    // Init statistics flags
    for (i=0; i<NUM_STATISTIC_ITEMS; i++) g_readStatistics[i] = 0;
    for (i=0; i<NUM_STATISTIC_ITEMS; i++) g_writeStatistics[i] = 0;
    for (i=0; i<NUM_STATISTIC_ITEMS; i++) g_totalStatistics[i] = 0;
    for (i=0; i<NUM_MESSAGE_TYPES; i++) g_returnedMessagesStatistics[i] = 0;
    for (i=0; i<NUM_MESSAGE_TYPES; i++) g_receivedMessagesStatistics[i] = 0;

    //
    // Make sure the main oscillator is enabled because this is required by
    // the PHY.  The system must have a 25MHz crystal attached to the OSC
    // pins. The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    //
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    //
    // Configure the device pins.
    //
    PinoutSet(true, false);

    //
    // Configure UART.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);

	//
    // Clear the terminal and print banner.
    //
    PRINT_ON_DEBUG("\033[2J\033[H");
    PRINT_ON_DEBUG("VCortex IO Extender device\n\n");
//    char myName[] = MY_NAME;
    PRINT_ON_DEBUG("Current name is: ");
    PRINT_ON_DEBUG(g_myName);
    PRINT_ON_DEBUG("  Firmware version: ");
    PRINT_ON_DEBUG(FW_VERSION);
    PRINT_ON_DEBUG("\n\n");

    //
	// The I2C0 peripheral must be enabled before use.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	// For this example I2C0 is used with PortB[3:2].  The actual port and
	// pins used may be different on your part, consult the data sheet for
	// more information.  GPIO port B needs to be enabled so these pins can
	// be used.
	// TODO: change this to whichever GPIO port you are using.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//
	// Configure the pin muxing for I2C0 functions on port B2 and B3.
	// This step is not necessary if your part does not support pin muxing.
	// TODO: change this to select the port/pin you are using.
	//
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	//
	// Select the I2C function for these pins.  This function will also
	// configure the GPIO pins pins for I2C operation, setting them to
	// open-drain operation with weak pull-ups.  Consult the data sheet
	// to see which functions are allocated per pin.
	// TODO: change this to select the port/pin you are using.
	//
//	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

	//
    // Configure Port N1 for as an output for the animation LED.
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    //
    // Initialize LED to OFF (0)
    //
    MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ~GPIO_PIN_1);

    //
    // Configure SysTick for a periodic interrupt.
    //
    MAP_SysTickPeriodSet(g_ui32SysClock / SYSTICKHZ);
    MAP_SysTickEnable();
    MAP_SysTickIntEnable();


    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.  The MAC address will be stored in the non-volatile
    // USER0 and USER1 registers.
    //
    MAP_FlashUserGet(&ui32User0, &ui32User1);
    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        //
        // We should never get here.  This is an error if the MAC address has
        // not been programmed into the device.  Exit the program.
        // Let the user know there is no MAC address
        //
//        PRINT_ON_DEBUG("No MAC programmed!\n");
        g_pui8MACArray[0] = 0;
          g_pui8MACArray[1] = 0x1a;
          g_pui8MACArray[2] = 0xb6;
          g_pui8MACArray[3] = 0x02;
          g_pui8MACArray[4] = 0xc6;
          g_pui8MACArray[5] = 0xbc;
//        while(1)
//        {
//        }
    }
    else
    {
        //
        // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
        // address needed to program the hardware registers, then program the MAC
        // address into the Ethernet Controller registers.
        //
        g_pui8MACArray[0] = ((ui32User0 >>  0) & 0xff);
        g_pui8MACArray[1] = ((ui32User0 >>  8) & 0xff);
        g_pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
        g_pui8MACArray[3] = ((ui32User1 >>  0) & 0xff);
        g_pui8MACArray[4] = ((ui32User1 >>  8) & 0xff);
        g_pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);
    }

    //
    // Tell the user what we are doing just now.
    //
    PRINT_ON_DEBUG("Initializing IP.\n");





    //
    // Initialize the lwIP library, w/o DHCP.
    //

    lwIPInit(g_ui32SysClock, g_pui8MACArray,
		10u<<24 | 0u<<16 | 0u<<8 | 131, // IP
		255u<<24 | 255<<16 | 255<<8 | 0, // Subnet
		255u<<24 | 255<<16 | 255<<8 | 0, // Gateway);
		IPADDR_USE_STATIC);
//    lwIPInit(g_ui32SysClock, g_pui8MACArray, 0, 0, 0, IPADDR_USE_DHCP);



    //
    // Setup the device locator service.
    //
    LocatorInit();
    LocatorMACAddrSet(g_pui8MACArray);
    LocatorAppTitleSet("EK-TM4C1294XL enet_io");

    //
    // Initialize a sample httpd server.
    //
    httpd_init();

    //
    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    //
    MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
    MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

    //
	// Pass our tag information to the HTTP server.
	//
	http_set_ssi_handler((tSSIHandler)SSIHandler, g_pcConfigSSITags,
			NUM_CONFIG_SSI_TAGS);

	//
	// Pass our CGI handlers to the HTTP server.
	//
	http_set_cgi_handlers(g_psConfigCGIURIs, NUM_CONFIG_CGI_URIS);

    // Init UDP part
    udp_init();  // Clears the UDP PCB list
    inUDP = udp_new();
    outUDP = udp_new();

    err_t udpErr = udp_bind(inUDP, IP_ADDR_ANY, IN_PORT) ;

    // Specify a callback function that should be called when a UDP datagram is received.
    udp_recv(inUDP, udpReceiver, NULL) ;
//    lSocket = lwip_socket(AF_INET, SOCK_DGRAM, 0);
//
//	memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
//	memset((char *)&sDestAddr, 0, sizeof(sDestAddr));
//
//	/*Destination*/
//	sDestAddr.sin_family = AF_INET;
//	sDestAddr.sin_len = sizeof(sDestAddr);
//	sDestAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
//	sDestAddr.sin_port = OUT_PORT;
//
//	/*Source*/
//	sLocalAddr.sin_family = AF_INET;
//	sLocalAddr.sin_len = sizeof(sLocalAddr);
//	sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	sLocalAddr.sin_port = IN_PORT;
//
//	 lwip_bind(lSocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr));
//
//	 char buffer[1024];
//
//	 while (1) {
//		 PRINT_ON_DEBUG("UDP Msg Received\n");
//		 int nbytes=lwip_recv(lSocket, buffer, sizeof(buffer) ,8);
//	     if (nbytes>0){
//	    	 lwip_sendto(lSocket, buffer, nbytes, 0, (struct sockaddr *)&sDestAddr, sizeof(sDestAddr));
//	     }
//	 }
    //
    	volatile uint32_t ui32Loop;

       //
       // Enable the GPIO port that is used for the on-board LED.
       //
//       SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOK;

       //
       // Do a dummy read to insert a few cycles after enabling the peripheral.
       //
//       ui32Loop = SYSCTL_RCGC2_R;

     // Enable the GPIO pin for the LED (PG2).  Set the direction as output, and
     // enable the GPIO pin for digital function.
     //
     GPIO_PORTK_DIR_R = 0x10;
     GPIO_PORTK_DEN_R = 0x10;

    // Loop forever.  All the work is done in interrupt handlers.
    //
    while(1)
    {

          //
          // Turn on the LED.
          //
          GPIO_PORTK_DATA_R |= 0x10;

          //
          // Delay for a bit.
          //

          sleep(500);

          //
          // Turn off the LED.
          //
          GPIO_PORTK_DATA_R &= ~(0x10);

          //
          // Delay for a bit.
          //
          sleep(500);

          if (g_flagSWUpdate)	// if magic packet arrives...
                {
                    PRINT_ON_DEBUG("Calling UpdateBegin\n");
                    SoftwareUpdateBegin(g_ui32SysClock);
                }
    }
}
