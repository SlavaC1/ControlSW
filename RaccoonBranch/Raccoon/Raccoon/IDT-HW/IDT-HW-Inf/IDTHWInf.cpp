#include "stdafx.h"
#include "IDTHWInf.h"
#include "IO_Extender.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "Utils.h"
#include "RSCommonDefinitions.h"

using namespace VCortex_IO_Extender;
using namespace RSCommon;

#define MAX_FCB_COUNT				4
#define MAX_CARTRIDGES_COUNT		32

#define BASE_I2C_ADDRESS			0X70
#define INSIDE_SECURE_I2C_ADDRESS	0x5F
#define SELECT_CHANNEL_CONFIG		0x20
#define DESELECT_CHANNEL_CONFIG		0x00

IOX_Handle handle = NULL;

static bool b_isOpened = false;
static unsigned char uc_cartridgesCount = 0;
static unsigned char uc_currentCartridgeNum = 0;
static unsigned int ui_timeout = 4000;
//
// VaultIC100 resynch command
static unsigned char auc_resynch_command[] =
{
	0x46, 0x00, 0x00, 0x46
};

//
// VaultIC100 resynch response
static unsigned char auc_resynch_response[] =
{
	0x47, 0x00, 0x00, 0x47
};

int SelectChannel(int FCBNum, int innerCartridgeNum);
int DeselectChannel(int FCBNum, int innerCartridgeNum);
int WakeupIICBus();
int GetSlaveAddress(int baseAddress, int FCBNum);
ostream& operator<<(ostream& out, const IO_Ext_Err value);

#define ENABLE_LOG	0

int IdtInit(int *cartridgesCount)
{
	if (b_isOpened)
	{
		*cartridgesCount = uc_cartridgesCount;
		return TRUE;
	}

#if (ENABLE_LOG == 1)
	LOG_EXT(LEVEL_DEBUG, "initializing device..."); 
#endif
	IOX_Handle tmpHandle = NULL;
	string deviceName("ETHIIC-1");
	IO_Ext_Err rc = IOX_initDevice(deviceName, &tmpHandle);
	if (rc != IO_S_OK)
	{
#if (ENABLE_LOG == 1)
		LOG_EXT(LEVEL_ERROR, "error initializing ethernet (returned " << (IO_Ext_Err)rc << ")."); 
#endif
		return FALSE;
	}

	handle = tmpHandle;

	IIC_Speeds speed = Baud100k;
	rc = IOX_initIIC(handle, speed);
	if (rc != IO_S_OK)
	{
#if (ENABLE_LOG == 1)
		LOG_EXT(LEVEL_ERROR, "error initializing IIC (returned " << (IO_Ext_Err)rc << ")."); 
#endif
		return FALSE;
	}

	int status = IO_S_OK;
	*cartridgesCount = 0;
	for (unsigned char FCBNum = 0; FCBNum < MAX_FCB_COUNT && *cartridgesCount < MAX_CARTRIDGES_COUNT; FCBNum++)
	{
		status = SelectChannel(FCBNum, 0);
		if (status != IO_S_OK)
		{
			break;
		}

		DeselectChannel(FCBNum, 0);
		*cartridgesCount += 8;
	}

	if (*cartridgesCount > MAX_CARTRIDGES_COUNT)
	{
		*cartridgesCount = MAX_CARTRIDGES_COUNT;
	}

	uc_cartridgesCount = *cartridgesCount;
	uc_currentCartridgeNum = 0;
	b_isOpened = true;
	return TRUE;
}

int IdtClose()
{
	b_isOpened = false;
	return TRUE;
}

int IdtGetInPlaceStatus(int *status)
{
#if (ENABLE_LOG == 1)
	LOG_EXT(LEVEL_DEBUG, "getting in-place status..."); 
#endif
	*status = 0;
	unsigned char buffer[256];
	memset(buffer, 0, 256);
	int ok, returnValue = TRUE;

	unsigned int timeout = ui_timeout;
	ui_timeout = 100;
	for (unsigned char cartridgeNum = 0; cartridgeNum < uc_cartridgesCount; cartridgeNum++)
	{
		// select channel:
		ok = IdtSelectChannel(cartridgeNum);
		if (ok == FALSE)
		{
			continue;
		}

		// resync command:
		memcpy(buffer, auc_resynch_command, sizeof(auc_resynch_command));
		ok = IdtSendData(sizeof(auc_resynch_command), (char *)buffer);
		if (ok == FALSE)
		{
			continue;
		}

		// get response:
		ok = IdtReceiveData(3, (char *)buffer);
		if (ok == FALSE)
		{
			continue;
		}

		// get data and checksum:
		ok = IdtReceiveData(buffer[2] + 1, (char *)(buffer + 3));
		if (ok == FALSE)
		{
			continue;
		}

		int cmpValue = memcmp(buffer, auc_resynch_response, sizeof(auc_resynch_response));
		ok = (cmpValue == 0);
		if (ok == FALSE)
		{
#if (ENABLE_LOG == 1)
			LOG_EXT(LEVEL_ERROR, "buffer returned corrupted from cartridge #" << (short)uc_currentCartridgeNum << ". In-place status failed.");
#endif
			returnValue = FALSE;
			break;
		}

		*status |= (1 << cartridgeNum);
	}

	ui_timeout = timeout;
	return returnValue;
}

int IdtSelectChannel(int cartridgeNum)
{
	int res = cartridgeNum >= 0 && cartridgeNum <= uc_cartridgesCount;
	if (res == TRUE)
	{
		uc_currentCartridgeNum = cartridgeNum;
	}

	return res;
}

int IdtSendData(int bufferSize, char *buffer)
{
#if (ENABLE_LOG == 1)
	char* bufferStr = new char[bufferSize * 2 + 1];
	ByteArrayToHexString((unsigned char*)buffer, (unsigned short)bufferSize, bufferStr);
	LOG_EXT(LEVEL_DEBUG, "sending buffer to cartridge #" << (short)uc_currentCartridgeNum << ": " << bufferStr);
	delete[] bufferStr;
#endif

	int FCBNum = uc_currentCartridgeNum >> 3, innerCartridgeNum = uc_currentCartridgeNum & 0x07;
	int slaveAddress = GetSlaveAddress(INSIDE_SECURE_I2C_ADDRESS, 0);
	int res = SelectChannel(FCBNum, innerCartridgeNum);
	if (res == IO_S_OK)
	{
		res = WakeupIICBus();
		if (res == IO_S_OK)
		{
			CPSleep(1);
			res = IOX_writeBytes(handle, slaveAddress, (unsigned char *)buffer, bufferSize, true);
		}

		DeselectChannel(FCBNum, innerCartridgeNum);
	}

#if (ENABLE_LOG == 1)
	if (res != IO_S_OK)
	{
		LOG_EXT(LEVEL_ERROR, "error sending buffer to cartridge #" << (short)uc_currentCartridgeNum << " (returned " << (IO_Ext_Err)res << ").");
	}
	else
	{
		LOG_EXT(LEVEL_DEBUG, "buffer was sent to cartridge #" << (short)uc_currentCartridgeNum << " successfully.");
	}
#endif

	return (res == IO_S_OK);
}

int IdtReceiveData(int bufferSize, char *buffer)
{
#if (ENABLE_LOG == 1)
	LOG_EXT(LEVEL_DEBUG, "start receiving buffer from cartridge #" << (short)uc_currentCartridgeNum << "...");
#endif

	int FCBNum = uc_currentCartridgeNum >> 3, innerCartridgeNum = uc_currentCartridgeNum & 0x07;
	int slaveAddress = GetSlaveAddress(INSIDE_SECURE_I2C_ADDRESS, 0);
	int res = SelectChannel(FCBNum, innerCartridgeNum);
	bool error = (res != IO_S_OK);
	bool timeout = false;
	clock_t start = clock();
	if (!error)
	{
		do
		{
			res = IOX_readBytes(handle, slaveAddress, (unsigned char *)buffer, bufferSize);
			error = (res != IO_S_OK);
			timeout = (unsigned int)(clock() - start) > ui_timeout;
			CPSleep(20);
		} while (error && !timeout);

		DeselectChannel(FCBNum, innerCartridgeNum);
	}

#if (ENABLE_LOG == 1)
	if (!error && !timeout)
	{
		char* bufferStr = new char[bufferSize * 2 + 1];
		ByteArrayToHexString((unsigned char*)buffer, (unsigned short)bufferSize, bufferStr);
		LOG_EXT(LEVEL_DEBUG, "received buffer from cartridge #" << (short)uc_currentCartridgeNum << ": " << bufferStr);
		delete[] bufferStr;
	}
	else
	{
		std::ostringstream oss;
		oss << "error receiving buffer from cartridge #" << (short)uc_currentCartridgeNum << " (returned ";
		if (timeout)
		{
			oss << "timeout";
		}
		else
		{
			oss << (IO_Ext_Err)res;
		}

		LOG_EXT(LEVEL_ERROR,  oss << ").");
	}
#endif

	return (res == IO_S_OK);
}

int SelectChannel(int FCBNum, int innerCartridgeNum)
{
	int slaveAddress = GetSlaveAddress(BASE_I2C_ADDRESS, FCBNum);
	unsigned char channelConfig[2];
	channelConfig[0] = (unsigned char)(1 << innerCartridgeNum);
	channelConfig[1] = SELECT_CHANNEL_CONFIG;
	return IOX_writeBytes(handle, slaveAddress, channelConfig, 2, true);
}

int DeselectChannel(int FCBNum, int innerCartridgeNum)
{
	int slaveAddress = GetSlaveAddress(BASE_I2C_ADDRESS, FCBNum);
	unsigned char channelConfig[2];
	channelConfig[0] = 0;
	channelConfig[1] = DESELECT_CHANNEL_CONFIG;
	return IOX_writeBytes(handle, slaveAddress, channelConfig, 2, true);
}

int WakeupIICBus()
{
	return IOX_wakeupIIC(handle);
}

int GetSlaveAddress(int baseAddress, int FCBNum)
{
	//return (baseAddress + FCBNum) >> 1;
	return baseAddress + FCBNum;
}

std::ostream& operator<<(std::ostream& out, const IO_Ext_Err value){
	const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
	switch(value){
		PROCESS_VAL(IO_S_OK);
		PROCESS_VAL(E_DEVICE_NOT_FOUND);
		PROCESS_VAL(E_MORE_THAN_ONE_DEVICE_FOUND);
		PROCESS_VAL(E_OS_COMMUNICATION_ERROR);
		PROCESS_VAL(E_WRITE_OP_FAILED);
		PROCESS_VAL(E_READ_OP_FAILED);
		PROCESS_VAL(E_WRONG_SLAVE_ADDRESS);
		PROCESS_VAL(E_HANDLE_NOT_FOUND);
		PROCESS_VAL(E_SLAVE_NOT_READY);
		PROCESS_VAL(E_INIT_FAILED);
	}
#undef PROCESS_VAL

	return out << s;
}