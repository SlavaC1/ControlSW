// IO_Extender.h

#pragma once
#include <string.h>
#include <vector>

#include <winsock2.h>
#include <Ws2tcpip.h>

//#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>

#ifdef IOEXTENDER_EXPORTS

#define IOEXTENDER_API __declspec(dllexport)

#else

#define IOEXTENDER_API __declspec(dllimport)

#endif

#ifdef __cplusplus 
extern "C" {
#endif

using namespace std;
using namespace log4cplus;

namespace VCortex_IO_Extender {

	typedef void * IOX_Handle;

	enum IO_Ext_Err
	{
		IO_S_OK = 0,
		E_DEVICE_NOT_FOUND = -1,
		E_MORE_THAN_ONE_DEVICE_FOUND = -2,
		E_OS_COMMUNICATION_ERROR = -3,
		E_WRITE_OP_FAILED = -4,
		E_READ_OP_FAILED = -5,
		E_WRONG_SLAVE_ADDRESS = -6,
		E_HANDLE_NOT_FOUND = -7,
		E_SLAVE_NOT_READY = -8,
		E_INIT_FAILED = -9,
	} ;

	enum IIC_Speeds
	{
		Baud100k = 0,
		Baud400k = 1,
		Baud1M = 2,
		Baud3_4M = 3
	};



		//
		// This function inits a device using its logical name. The returned handle will be used in sending and receiving data from the device
		//
		// Input parameters:
		// - deviceName:	Name of the device. The device should be set to answer to this name
		//
		// - Output parameters:
		// - handle:		Keep this handle for next read or write functions to this device

		IOEXTENDER_API IO_Ext_Err IOX_initDevice(const string deviceName, IOX_Handle* handle);
		//
		// This function inits an I2C connection on a device
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		// - speed:			IIC bus speed. 100k, 400k, 1M or 4M bits per second
		//
		IOEXTENDER_API IO_Ext_Err IOX_initIIC( const IOX_Handle handle, const IIC_Speeds speed);

		//
		// This function writes bytes into a slave connected to the device. buff holds the bytes to write, and buffLen is the number of bytes to
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		// - slaveAddress:	The address of the slave connected to this device, to which we want to write the data
		// - buff:			Contains the data to be written. Allocated and filled by the caller
		// - buffLen:		Length of the data to write
		// - withStop:		Issue stop bit at the end - for address settings of PROM devices
		//					This flag is active only on multiple bytes send
		IOEXTENDER_API IO_Ext_Err IOX_writeBytes(const IOX_Handle handle, const int slaveAddress, const BYTE* buff, const int buffLen, bool withStop);

		//
		// This function reads bytes from a slave connected to the device. buff will holds the bytes that were read, and buffLen is the number of bytes to read
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		// - slaveAddress:	The address of the slave connected to this device, to which we want to write the data
		// - buffLen:		Length of the data to read
		//
		// - Output parameters:
		// - buff:			The bytes that are read from the slave are copied into buff. buff is allocated by the caller, with minimum size of bufflen
		IOEXTENDER_API IO_Ext_Err IOX_readBytes(const IOX_Handle handle, const int slaveAddress, BYTE* buff, const int buffLen);

		//
		// This function write byte into a slave and then reads it back
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		// - slaveAddress:	The address of the slave connected to this device, to which we want to write the data
		// - byteToSend:	Byte to be written. 
		//
		// - Output parameters:
		// - receivedByte:	Byte that was read back
		IOEXTENDER_API IO_Ext_Err IOX_writeAndReadByte(const IOX_Handle handle, const int slaveAddress, const BYTE byteToSend, BYTE& receivedByte);

		//
		// This function closes the handle and releases all system resources bound to it.
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		IOEXTENDER_API IO_Ext_Err IOX_closeDevice(const IOX_Handle handle);

		//
		// Get some statistics on the communication performed to  and from the device
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		IOEXTENDER_API IO_Ext_Err IOX_getCommStat(const IOX_Handle handle, int* numCommands, int* numRetries);


		//
		// Set log level and output media
		//
		// Input parameters:
		// - logLevel:		TRACE - DEBUG - INFO - WARN - ERROR - FATAL (Each level includes the ones on its right)
		IOEXTENDER_API IO_Ext_Err IOX_setLog(const LogLevel logLevel, char* logFileName);
		
		//
		// Special function for Stratasys device to start IIC bus
		//
		// Input parameters:
		// - handle:		The unique handle for this device, that we got from the init() function
		IOEXTENDER_API IO_Ext_Err IOX_wakeupIIC(const IOX_Handle handle);

}

#ifdef __cplusplus
}
#endif

