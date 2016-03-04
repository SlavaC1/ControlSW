#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <sstream>
#include "Utils.h"
#ifndef _WIN32
#include <cstring>
#endif

using namespace std;

namespace RSCommon
{
	pfnLogMessage _logMessageFunction = NULL;

	void SetLogMessageFunction(pfnLogMessage logMessageFunction)
	{
		_logMessageFunction = logMessageFunction;
	}

	void WriteLogMessage(const char* level, const char* message)
	{   /*
		timeb now;
		ftime(&now);
		time_t nowTime = now.time;
		struct tm* tmnow = localtime(&nowTime);

		char* buffer = new char[32];
		strftime(buffer, 32, "%d-%b-%Y %H:%M:%S.", tmnow);
		sprintf(buffer + strlen(buffer), "%03d", now.millitm);

		string formattedMessage = buffer;*/
		string formattedMessage;
		formattedMessage.append(MODULE_NAME);
		formattedMessage.append(level);
		formattedMessage.append(": ");

		formattedMessage.append(message);

		if (_logMessageFunction != NULL)
		{
			_logMessageFunction(formattedMessage.c_str());
		}
	}

	void HexStringToByteArray(const char* psz, unsigned short length, unsigned char pb[])
	{
		char buf[4];
		buf[2] = 0;
		for (unsigned short us = 0; us < length; us += 2)
		{
			buf[0] = psz[us];
			buf[1] = psz[us + 1];
			pb[us >> 1] = (unsigned char)strtoul(buf, NULL, 16);
		}
	}

	void ByteArrayToHexString(const unsigned char pb[], unsigned short length, char* psz)
	{
		char buffer[4];
		buffer[2] = 0;
		for (unsigned short us = 0; us < length; us++)
		{
			sprintf(buffer, "%*X", 2, pb[us]);
			if (buffer[0] == ' ') buffer[0] = '0';
			memcpy(psz + (us << 1), buffer, 2 * sizeof(char));
		}

		psz[length << 1] = 0;
	}

	const char* GetError(int errorCode)
	{
		switch (errorCode)
		{
		case FCB_INIT_FAILED:
			return "FCB init failed.";
		case FCB_SELECTION:
			return "FCB selection failed.";
		case FCB_GET_IN_PLACE_STATUS_FAILED:
			return "FCB get in-place status failed";
		case FCB_SELECT_CHANNEL_FAILED:
			return "FCB select channel failed";
		case FCB_NULL_PARAM:
			return "FCB null parameters";
		case HW_NOT_INITIALIZED:
			return "Hardware not initialized";

		case INVALID_CERTIFICATE_SIGNATURE:
			return "invalid certificate signature";
		case INVALID_TOKEN_SIGNATURE:
			return "invalid token signature";
		case INVALID_CERTIFICATE_FILE:
			return "invalid certificate file";
		case INVALID_HOST_KEY:
			return "invalid host key";
		case AUTHENTICATE_NULL_PARAMS:
			return "authenticate null params";
		case EMPTY_CERTIFICATE_FILE:
			return "blank tag";
		case EXCEPTION_CAUGHT:
			return "inner exception caught";

		case CARTRIDGE_NOT_AUTHENTICATED:
			return "cartridge not authenticated";
		case INVALID_VOLUME_SIGNATURE:
			return "invalid volume signature";
		case MATERIAL_OVERCONSUMPTION:
			return "material overconsumption";
		case GENERATE_RANDOM_FAILED:
			return "generate random failed";
		case CONSUMPTION_NULL_PARAMS:
			return "consumption null params";

		case LOGIN_FAILED:
			return "login failed";
		case IDT_LOCKED:
			return "tag is locked";
		case TRANSITION_FAILED:
			return "mode transition failed";
		case ACCESS_DENIED:
			return "access denied";
		case CONFIGURATION_FAILED:
			return "configuring failed";
		case KEY_GENERATION_FAILED:
			return "key generation failed";
		case COUNTER_SET_FAILED:
			return "counter setting failed";
		case CERTIFICATE_WRITE_FAILED:
			return "writing certificate failed";

		case FILE_SELECTION_FAILED:
			return "file selection failed";

		case WRITING_IDD_FILE_FAILED:
			return "writing IDD file failed";

		case WRITING_SIGNATURE_FILE_FAILED:
			return "writing signature file failed";

		case GET_INFO_FAILED:
			return "get info failed";

		case 0xCF0D:
			return "cannot send data to FCB";
		case 0xCF0F:
			return "cannot receive data from FCB";
		case 0x6581:
			return "tag memory error";
		case 0x65FF:
			return "tag communication error";

		default:
			return "Unknown error";
		}
	}	

	void CPSleep(unsigned int uiDelayMSec)
	{
#ifdef WIN32
		Sleep(uiDelayMSec);
#else
		usleep(uiDelayMSec * 1000);
#endif
	}
}
