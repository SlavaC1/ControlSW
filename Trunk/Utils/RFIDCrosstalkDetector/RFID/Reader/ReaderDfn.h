#ifndef __ReaderDfn
#define __ReaderDfn


#define SW_VERSION "V3.20"

///////////////
//General codes
#define REQUEST_SET_PARAM				0
#define REQUEST_FIND_TOKEN				1
#define REQUEST_INVENTORY				-1
#define REQUEST_READ_MULTI				2
#define REQUEST_GET_INFO				3
#define REQUEST_WRITE_MULTI				4
#define REQUEST_WRITE_SINGLE			5
#define REQUEST_READ_SINGLE				6
#define REQUEST_LOCK					7
#define REQUEST_TRANSMITTER_ON			8
#define REQUEST_TRANSMITTER_OFF			9

//Error codes
#define ERROR_PORT_ALREADY_OPEN							5
#define ERROR_PORT_UNABLE_OPEN							-51
#define ERROR_NO_RESPONSE_FROM_READER					-52
#define ERROR_TOKEN_NOT_PRESENT							-53
#define SET_CHANNEL_FAIL								-54
#define ERROR_TAG_SELECT								4
#define LOCK_FAIL										6
#define READ_WRITE_ERROR								-1
#define ERROR_NONE										   0

#define CHANNELS						4

#endif