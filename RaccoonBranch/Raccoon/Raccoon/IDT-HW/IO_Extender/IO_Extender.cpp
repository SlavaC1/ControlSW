// This is the main DLL file.

#include "stdafx.h"
#define IOEXTENDER_EXPORTS
#include "IO_Extender.h"

#define IN_PORT 4478
#define OUT_PORT 4477

#define MAX_RETRIES 3

namespace VCortex_IO_Extender 
{

	struct DeviceInfo
	{
		string logicalName;
		sockaddr_in ipAddr;
	};

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
	} ;

	SOCKET g_sendingSocket ;
	SOCKET g_receivingSocket;
	bool g_initDone = false;
	vector<DeviceInfo> g_devicesList;

#pragma pack(push, 1)
	class IO_Ext_Msg {
	public:
		unsigned char checksum;
		unsigned char msgType;
		unsigned char slaveAddress;
		unsigned char withStop;
		short buffLen;
		char buff[1024];
	} ;
#pragma pack(pop)

#define IO_EXT_MSG_HEADER_LEN 6

	IO_Ext_Err handleToNum(const IOX_Handle handle, int& deviceNum);

	unsigned char calcChecksum(IO_Ext_Msg msg, bool withData)
	{
		unsigned char checkSum = 0;
		checkSum += msg.msgType;
		checkSum += msg.slaveAddress;
		checkSum += msg.withStop;
		checkSum += msg.buffLen >> 8;
		checkSum += msg.buffLen & 0x00ff;

		if (withData)
		{
			for (int i=0; i<msg.buffLen; i++)
				checkSum += msg.buff[i];
		}

		return checkSum;
	}


	void IOX_init(){
		// protect against calls from mulspeedtiple threads
		if (g_initDone)
			return;

		//-----------------------------------------------
		// Initialize Winsock
		int rc = 0;
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc != NO_ERROR) 
		{
			return ;
		}
		//-----------------------------------------------
		// Create a receiver socket to receive datagrams
		g_receivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (g_receivingSocket == INVALID_SOCKET) 
		{
			return ;
		}
		//-----------------------------------------------
		// Create a sending socket to receive datagrams
		g_sendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (g_receivingSocket == INVALID_SOCKET) 
		{
			return ;
		}

		//-----------------------------------------------
		// Bind the receiver socket to any address and the specified port.
		sockaddr_in recvAddr;
		recvAddr.sin_family = AF_INET;
		recvAddr.sin_port = htons(IN_PORT);
		recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		rc = bind(g_receivingSocket, (SOCKADDR *) &recvAddr, sizeof (recvAddr));
		if (rc != 0) 
		{
			return ;
		}

		g_initDone = true;
	}

	//
	// This function inits a device using its logical name. The returned handle will be used in sending and receiving data from the device
	//
	// Input parameters:
	// - deviceName:	Name of the device. The device should be set to answer to this name
	//
	// - Output parameters:
	// - handle:		Keep this handle for next read or write functions to this device
	IO_Ext_Err IOX_initDevice(const string deviceName, IOX_Handle* handle)
	{
		// Init global variables
		IOX_init();

		IO_Ext_Msg ioExtOutMsg;

		ioExtOutMsg.msgType = (char)INIT;

		ioExtOutMsg.buffLen = deviceName.size();
		for (int i=0; i<ioExtOutMsg.buffLen; i++)
			ioExtOutMsg.buff[i] = deviceName[i];

		ioExtOutMsg.checksum = calcChecksum(ioExtOutMsg, true);
		
		int msgLen = ioExtOutMsg.buffLen + IO_EXT_MSG_HEADER_LEN;

		//---------------------------------------------
		// Set up the RecvAddr structure with the IP address of BROADCAST
		// and the specified port number.
		sockaddr_in recvAddr;
		recvAddr.sin_family = AF_INET;
		recvAddr.sin_port = htons(OUT_PORT);
		//		recvAddr.sin_addr.s_addr = INADDR_BROADCAST;
		recvAddr.sin_addr.s_addr = inet_addr("10.0.0.131");

		int numRetries = 0;
		int rc = 0;
		char buff[200];
		int buffLen = 200;

		sockaddr_in senderAddr;
		int senderAddrSize = sizeof (senderAddr);

		bool val1 = true;
		rc = setsockopt(g_sendingSocket, SOL_SOCKET, SO_BROADCAST,(char*)&val1,sizeof(val1));
		if (rc != 0) {
			return E_OS_COMMUNICATION_ERROR;
		}

		//Send init command and wait for device to respond
		while (numRetries<MAX_RETRIES)
		{
			//---------------------------------------------
			// Send a datagram to the device
			rc = sendto(g_sendingSocket, (const char *)&ioExtOutMsg, IO_EXT_MSG_HEADER_LEN + ioExtOutMsg.buffLen, 0, (SOCKADDR *)&recvAddr, sizeof (recvAddr));
			if (rc == SOCKET_ERROR) {
				return E_OS_COMMUNICATION_ERROR;
			}

			// Wait for message with 100 mSec time limit, 3 retries
			DWORD to = 1000;
			rc = setsockopt(g_receivingSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(to));
			if (rc != 0) {
				return E_OS_COMMUNICATION_ERROR;
			}

			rc = recvfrom(g_receivingSocket, buff, buffLen, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);

			IO_Ext_Msg* inMsg = (IO_Ext_Msg*)buff;
			// if receive failed, try again
			if (rc != IO_EXT_MSG_HEADER_LEN) 
				numRetries++;
			// Check received content
			else if ( (INIT != inMsg->msgType) || (ioExtOutMsg.slaveAddress != inMsg->slaveAddress) || (0 != inMsg->buffLen) )
				numRetries++;
			else
				break;

		}

		if (numRetries == MAX_RETRIES)
			return E_DEVICE_NOT_FOUND;

		DeviceInfo deviceInfo;
		deviceInfo.ipAddr = senderAddr;
		deviceInfo.logicalName = deviceName;

		g_devicesList.push_back(deviceInfo);

		// the handle is the position in the vector.
		*handle = &g_devicesList[g_devicesList.size() - 1];

		return IO_S_OK;
	}

	//
	// This function inits an I2C connection on a device
	//
	// Input parameters:
	// - handle:		The unique handle for this device, that we got from the init() function
	// - speed:			IIC bus speed. 100k, 400k, 1M or 4M bits per second
	//
	IO_Ext_Err IOX_initIIC( const IOX_Handle handle, const IIC_Speeds speed)
	{
		// Init global variables
		IOX_init();

		int deviceNum;
		IO_Ext_Err irc = handleToNum(handle, deviceNum);
		if (irc != IO_S_OK)
			return irc;

		IO_Ext_Msg ioExtOutMsg;
		ioExtOutMsg.msgType = (char)INIT_IIC;

		// Put speed in slave address
		ioExtOutMsg.slaveAddress = speed;
		ioExtOutMsg.buffLen = 0;

		ioExtOutMsg.checksum = calcChecksum(ioExtOutMsg, false);

		int numRetries = 0;
		int rc = 0;
		sockaddr_in recvAddr = g_devicesList[deviceNum].ipAddr;

		char recBuff[1024];
		int recBuffLen = 1024;
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof (senderAddr);

		// Wait for device response
		while (numRetries<MAX_RETRIES)
		{
			//---------------------------------------------
			// Send a datagram to the device
			rc = sendto(g_sendingSocket, (const char *)&ioExtOutMsg, IO_EXT_MSG_HEADER_LEN + ioExtOutMsg.buffLen, 0, (SOCKADDR *)&recvAddr, sizeof (recvAddr));
			if (rc == SOCKET_ERROR) {
				return E_OS_COMMUNICATION_ERROR;
			}

			// Wait for message with 100 mSec time limit, 3 retries
			DWORD to = 1000;
			rc = setsockopt(g_receivingSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(to));
			if (rc != 0) {
				return E_OS_COMMUNICATION_ERROR;
			}

			rc = recvfrom(g_receivingSocket, recBuff, recBuffLen, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			IO_Ext_Msg* inMsg = (IO_Ext_Msg*)recBuff;
			// if receive failed, try again
			if (rc != IO_EXT_MSG_HEADER_LEN)
				numRetries++;
			// Check that the one answered is the one approached
			else if ( memcmp((char*)&senderAddr, (char *)&recvAddr, sizeof(recvAddr)) )
				numRetries++;
			// Check received content
			else if (ioExtOutMsg.msgType != inMsg->msgType) 
				numRetries++;
			else
				break;
		}

		// If no response was received - return error
		if (numRetries == MAX_RETRIES)
			return E_INIT_FAILED;

		// Otherwise all is under control
		return IO_S_OK;
	}	


	IO_Ext_Err handleToNum(const IOX_Handle handle, int& deviceNum)
	{
		for (size_t i=0; i<g_devicesList.size(); i++)
		{
			if (&g_devicesList[i] == handle)
			{
				deviceNum = i;
				return IO_S_OK;
			}
		}

		return E_HANDLE_NOT_FOUND;
	}

	IO_Ext_Err IOX_writeBytes(const IOX_Handle handle, const int slaveAddress, const BYTE* buff, const int buffLen, bool withStop)
	{

		// Init global variables
		IOX_init();

		int deviceNum;
		IO_Ext_Err irc = handleToNum(handle, deviceNum);
		if (irc != IO_S_OK)
			return irc;

		IO_Ext_Msg ioExtOutMsg;
		ioExtOutMsg.msgType = (char)WRITE_BYTES;
		if ((slaveAddress>255) | (slaveAddress<0))
			return E_WRONG_SLAVE_ADDRESS;

		ioExtOutMsg.slaveAddress = slaveAddress;
		ioExtOutMsg.withStop = withStop;
		ioExtOutMsg.buffLen = buffLen;
		for (int i=0; i<buffLen; i++)
			ioExtOutMsg.buff[i] = buff[i];

		ioExtOutMsg.checksum = calcChecksum(ioExtOutMsg, true);
		int numRetries = 0;
		int rc = 0;
		sockaddr_in recvAddr = g_devicesList[deviceNum].ipAddr;

		char recBuff[1024];
		int recBuffLen = 1024;
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof (senderAddr);

		// Wait for device response
		while (numRetries<MAX_RETRIES)
		{
			//---------------------------------------------
			// Send a datagram to the device
			rc = sendto(g_sendingSocket, (const char *)&ioExtOutMsg, IO_EXT_MSG_HEADER_LEN + ioExtOutMsg.buffLen, 0, (SOCKADDR *)&recvAddr, sizeof (recvAddr));
			if (rc == SOCKET_ERROR) {
				return E_OS_COMMUNICATION_ERROR;
			}

			// Wait for message with 100 mSec time limit, 3 retries
			DWORD to = 1000;
			rc = setsockopt(g_receivingSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(to));
			if (rc != 0) {
				return E_OS_COMMUNICATION_ERROR;
			}

			rc = recvfrom(g_receivingSocket, recBuff, recBuffLen, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			IO_Ext_Msg* inMsg = (IO_Ext_Msg*)recBuff;
			// if receive failed, try again
			if (rc != IO_EXT_MSG_HEADER_LEN)
				numRetries++;
			// Check that the one answered is the one approached
			else if ( memcmp((char*)&senderAddr, (char *)&recvAddr, sizeof(recvAddr)) )
				numRetries++;
			// Check received content
			else if (ioExtOutMsg.slaveAddress != inMsg->slaveAddress) 
				numRetries++;
			else
				break;

		}

		// If no response was received - return error
		if (numRetries == MAX_RETRIES)
			return E_WRITE_OP_FAILED;

		// Check received content
		// Check if slave is not ready 
		if ( SLAVE_NOT_READY == ((IO_Ext_Msg*)recBuff)->msgType  )
			return E_SLAVE_NOT_READY;

		// Otherwise all is under control
		return IO_S_OK;
	}	

	IO_Ext_Err IOX_writeAndReadByte(const IOX_Handle handle, const int slaveAddress, const BYTE byteToSend, BYTE& receivedByte)
	{

		// Init global variables
		IOX_init();

		int deviceNum;
		IO_Ext_Err irc = handleToNum(handle, deviceNum);
		if (irc != IO_S_OK)
			return irc;

		IO_Ext_Msg ioExtOutMsg;
		ioExtOutMsg.msgType = (char)WRITE_AND_READ_BYTE;

		if ((slaveAddress>255) | (slaveAddress<0))
			return E_WRONG_SLAVE_ADDRESS;

		ioExtOutMsg.slaveAddress = slaveAddress;
		ioExtOutMsg.buffLen = 1;
		ioExtOutMsg.buff[0] = byteToSend;

		ioExtOutMsg.checksum = calcChecksum(ioExtOutMsg, true);

		int msgLen = ioExtOutMsg.buffLen + IO_EXT_MSG_HEADER_LEN;
		int numRetries = 0;
		int rc = 0;
		sockaddr_in recvAddr = g_devicesList[deviceNum].ipAddr;

		char recBuff[1024];
		int recBuffLen = 1024;
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof (senderAddr);

		// Wait for device response
		while (numRetries<MAX_RETRIES)
		{
			//---------------------------------------------
			// Send a datagram to the device
			rc = sendto(g_sendingSocket, (const char *)&ioExtOutMsg, IO_EXT_MSG_HEADER_LEN + ioExtOutMsg.buffLen, 0, (SOCKADDR *)&recvAddr, sizeof (recvAddr));
			if (rc == SOCKET_ERROR) {
				return E_OS_COMMUNICATION_ERROR;
			}

			// Wait for message with 100 mSec time limit, 3 retries
			DWORD to = 1000;
			rc = setsockopt(g_receivingSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(to));
			if (rc != 0) {
				return E_OS_COMMUNICATION_ERROR;
			}

			rc = recvfrom(g_receivingSocket, recBuff, recBuffLen, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			IO_Ext_Msg* inMsg = (IO_Ext_Msg*)recBuff;
			// if receive failed, try again
			if (rc != IO_EXT_MSG_HEADER_LEN + ioExtOutMsg.buffLen)
				numRetries++;
			// Check that the one answered is the one approached
			else if ( memcmp((char*)&senderAddr, (char *)&recvAddr, sizeof(recvAddr)) )
				numRetries++;
			// Check received content
			else if (ioExtOutMsg.slaveAddress != inMsg->slaveAddress) 
				numRetries++;
			else
				break;

		}

		// If no response was received - return error
		if (numRetries ==  MAX_RETRIES)
			return E_WRITE_OP_FAILED;

		// Check received content
		if (WRITE_ERROR == ((IO_Ext_Msg*)recBuff)->msgType)
			return E_WRITE_OP_FAILED;

		if (READ_ERROR == ((IO_Ext_Msg*)recBuff)->msgType)
			return E_READ_OP_FAILED;

		// Check if slave is not ready 
		if ( SLAVE_NOT_READY == ((IO_Ext_Msg*)recBuff)->msgType )
			return E_SLAVE_NOT_READY;

		// Otherwise all is under control
		receivedByte = ((IO_Ext_Msg*)recBuff)->buff[0];

		return IO_S_OK;

	}

	IO_Ext_Err IOX_readBytes(const IOX_Handle handle, const int slaveAddress, BYTE* buff, const int buffLen)
	{
		// Init global variables
		IOX_init();

		int deviceNum;
		IO_Ext_Err irc = handleToNum(handle, deviceNum);
		if (irc != IO_S_OK)
			return irc;

		IO_Ext_Msg ioExtOutMsg;
		ioExtOutMsg.msgType = (char)READ_BYTES;
		if ((slaveAddress>255) | (slaveAddress<0))
			return E_WRONG_SLAVE_ADDRESS;

		ioExtOutMsg.slaveAddress = slaveAddress;
//		ioExtOutMsg.withStop = withStop;
		ioExtOutMsg.buffLen = buffLen;

		ioExtOutMsg.checksum = calcChecksum(ioExtOutMsg, false);

		int numRetries = 0;
		int rc = 0;
		sockaddr_in recvAddr = g_devicesList[deviceNum].ipAddr;

		char recBuff[1024];
		int recBuffLen = 1024;
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof (senderAddr);

		// Wait for device response
		while (numRetries<MAX_RETRIES)
		{
			//---------------------------------------------
			// Send a datagram to the device
			rc = sendto(g_sendingSocket, (const char *)&ioExtOutMsg, IO_EXT_MSG_HEADER_LEN, 0, (SOCKADDR *)&recvAddr, sizeof (recvAddr));
			if (rc == SOCKET_ERROR) {
				return E_OS_COMMUNICATION_ERROR;
			}

			// Wait for message with 100 mSec time limit, 3 retries
			DWORD to = 1000;
			rc = setsockopt(g_receivingSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(to));
			if (rc != 0) {
				return E_OS_COMMUNICATION_ERROR;
			}

			rc = recvfrom(g_receivingSocket, recBuff, recBuffLen, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			IO_Ext_Msg* inMsg = (IO_Ext_Msg*)recBuff;
			// if receive failed, try again
			if (rc < 0)
				numRetries++;
			// Check that the one answered is the one approached
			else if ( memcmp((char*)&senderAddr, (char *)&recvAddr, sizeof(recvAddr)) )
				numRetries++;
			// Check received content
			else if (ioExtOutMsg.slaveAddress != inMsg->slaveAddress) 
				numRetries++;
			else
				break;

		}

		// If no response was received - return error
		if (numRetries == MAX_RETRIES)
			return E_READ_OP_FAILED;

		// Check received content
		// Check if slave is not ready 
		if ( SLAVE_NOT_READY == ((IO_Ext_Msg*)recBuff)->msgType  )
			return E_SLAVE_NOT_READY;

		// Otherwise all is under control
		for (int i=0; i<buffLen; i++)
			buff[i] = ((IO_Ext_Msg*)recBuff)->buff[i];

		return IO_S_OK;
	}

	IO_Ext_Err IOX_wakeupIIC(const IOX_Handle handle)
	{
		// Init global variables
		IOX_init();

		int deviceNum;
		IO_Ext_Err irc = handleToNum(handle, deviceNum);
		if (irc != IO_S_OK)
			return irc;

		IO_Ext_Msg ioExtOutMsg;
		ioExtOutMsg.msgType = (char)WAKEUP_IIC;


		ioExtOutMsg.slaveAddress = 0;
		ioExtOutMsg.buffLen = 0;

		ioExtOutMsg.checksum = calcChecksum(ioExtOutMsg, false);

		int numRetries = 0;
		int rc = 0;
		sockaddr_in recvAddr = g_devicesList[deviceNum].ipAddr;

		char recBuff[1024];
		int recBuffLen = 1024;
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof (senderAddr);

		// Wait for device response
		while (numRetries<MAX_RETRIES)
		{
			//---------------------------------------------
			// Send a datagram to the device
			rc = sendto(g_sendingSocket, (const char *)&ioExtOutMsg, IO_EXT_MSG_HEADER_LEN, 0, (SOCKADDR *)&recvAddr, sizeof (recvAddr));
			if (rc == SOCKET_ERROR) {
				return E_OS_COMMUNICATION_ERROR;
			}

			// Wait for message with 100 mSec time limit, 3 retries
			DWORD to = 1000;
			rc = setsockopt(g_receivingSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&to,sizeof(to));
			if (rc != 0) {
				return E_OS_COMMUNICATION_ERROR;
			}

			rc = recvfrom(g_receivingSocket, recBuff, recBuffLen, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			IO_Ext_Msg* inMsg = (IO_Ext_Msg*)recBuff;
			// if receive failed, try again
			if (rc < 0)
				numRetries++;
			// Check that the one answered is the one approached
			else if ( memcmp((char*)&senderAddr, (char *)&recvAddr, sizeof(recvAddr)) )
				numRetries++;
			// Check received content
			else if (inMsg->msgType != WAKEUP_OK) 
				numRetries++;
			else
				break;

		}

		// If no response was received - return error
		if (numRetries == MAX_RETRIES)
			return E_READ_OP_FAILED;

		// Check received content
		// Check if slave is not ready 
		if ( SLAVE_NOT_READY == ((IO_Ext_Msg*)recBuff)->msgType  )
			return E_SLAVE_NOT_READY;

		// Otherwise all is under control
		return IO_S_OK;
	}
}