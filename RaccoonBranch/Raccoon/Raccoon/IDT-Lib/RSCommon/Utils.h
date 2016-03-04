#ifndef UtilsH
#define UtilsH

#include "stdafx.h"
#include <stdlib.h>
#include "RSCommonDefinitions.h"

#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif

namespace RSCommon
{
#define MODULE_NAME "Raccoon driver: "

	EXTERN_C RSCOMMON_LIB void SetLogMessageFunction(void(*logMessageFunction)(const char*));

	EXTERN_C RSCOMMON_LIB void WriteLogMessage(const char* level, const char* message);

	EXTERN_C RSCOMMON_LIB void HexStringToByteArray(const char* psz, unsigned short length, unsigned char pb[]);

	EXTERN_C RSCOMMON_LIB void ByteArrayToHexString(const unsigned char pb[], unsigned short length, char* psz);

	EXTERN_C RSCOMMON_LIB const char* GetError(int errorCode);

	EXTERN_C RSCOMMON_LIB void CPSleep(unsigned int uiDelayMSec);
}

#endif
