#include "stdafx.h"
#include "IDTagAdapter.h"
#include "osrng.h"
#include "oids.h"
#include "IDTHWInf.h"
#include "Utils.h"

using namespace std;
using namespace CryptoPP;
using namespace RSCommon;

namespace StratasysIDTSrv
{
	IDTagAdapter::IDTagAdapter()
		: m_bIsInitialized(false), m_ucCartridgesCount(0)
	{

	}

	IDTagAdapter::~IDTagAdapter()
	{
		IdtClose();
	}

	int IDTagAdapter::InitHW(unsigned char *pucCartridgesCount)
	{
		int cartridgesCount;
		IdtClose();
		int returnValue = IdtInit(&cartridgesCount);
		if (returnValue != TRUE)
		{
			return FCB_INIT_FAILED;
		}

		m_ucCartridgesCount = *pucCartridgesCount = (unsigned char)(cartridgesCount);
		return IDTLIB_SUCCESS;
	}

	int IDTagAdapter::GetInPlaceStatus(int *pStatus)
	{
		int returnValue = IdtGetInPlaceStatus(pStatus);
		if (returnValue != TRUE)
		{
			return FCB_GET_IN_PLACE_STATUS_FAILED;
		}

		return IDTLIB_SUCCESS;
	}

	int IDTagAdapter::SwitchToCartridge(unsigned char ucCartridgeNum)
	{
		LOG_EXT(LEVEL_DEBUG, "Switch FCB mux...");
		if (ucCartridgeNum >= m_ucCartridgesCount)
		{
			return FCB_SELECT_CHANNEL_FAILED;
		}

		int returnValue = IdtSelectChannel(ucCartridgeNum);
		if (returnValue != TRUE)
		{
			return FCB_SELECT_CHANNEL_FAILED;
		}

		return IDTLIB_SUCCESS;
	}
}
