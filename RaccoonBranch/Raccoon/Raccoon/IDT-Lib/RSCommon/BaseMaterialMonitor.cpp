#include "stdafx.h"
#include "BaseMaterialMonitor.h"
#include "UniqueLock.h"
#include "Utils.h"
#include "oids.h"
#include "osrng.h"

namespace RSCommon
{
	BaseMaterialMonitor::BaseMaterialMonitor()
		: m_bInitialized(false), m_ucCartridgesCount(0) /*, m_pubKS(NULL)*/
	{
		pthread_mutex_init(&m_mtx, NULL);
	}

	BaseMaterialMonitor::~BaseMaterialMonitor()
	{
		pthread_mutex_destroy(&m_mtx);
	}

	int BaseMaterialMonitor::InitHW(unsigned char *pucCartridgesCount)
	{
		UniqueLock uniqueLock(m_mtx);
		int returnValue = IDTLIB_SUCCESS;
		if (pucCartridgesCount == NULL)
		{
			returnValue = FCB_NULL_PARAM;
			LOG_EXT(LEVEL_FATAL, "Error initializing HW (error code 0x" << hex << (short)returnValue << ").");
			return returnValue;
		}

		m_bInitialized = false;
		return returnValue;
	}


	bool BaseMaterialMonitor::IsBusy()
	{
		if (pthread_mutex_trylock(&m_mtx) == 0)
		{
			pthread_mutex_unlock(&m_mtx);
			return false;
		}

		return true;
	}

	/// <summary>
	/// Resets cartridge information.
	/// </summary>
	/// <param name="ucCartridgeNum">The cartridge number.</param>
	/// <returns>0 on success, non-zero for failures.</returns>
	int BaseMaterialMonitor::RemoveCartridge(unsigned char ucCartridgeNum)
	{
		try
		{
			UniqueLock uniqueLock(m_mtx);
			LOG_EXT(LEVEL_INFO, "Removing cartridge #" << (unsigned short)ucCartridgeNum << "...");
			int returnValue = ResetCartridgeInfo(ucCartridgeNum);
			if (returnValue == IDTLIB_SUCCESS)
			{
				LOG_EXT(LEVEL_INFO, "Cartridge #" << (unsigned short)ucCartridgeNum << " was removed successfully.");
			}
			else
			{
				LOG_EXT(LEVEL_ERROR, "Cannot select cartridge #" << (unsigned short)ucCartridgeNum << ": cartridges count is " << (unsigned short)m_ucCartridgesCount << ".");
			}
			return returnValue;
		}
		catch (exception& e)
		{
			LOG_EXT(LEVEL_ERROR, "Exception caught: " << e.what() << ".");
			return EXCEPTION_CAUGHT;
		}
	}

	pthread_mutex_t BaseMaterialMonitor::GetMutex() const
	{
		return this->m_mtx;
	}


	int BaseMaterialMonitor::UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume)
	{
		return UpdateConsumption(ucCartridgeNum, uiComsumption, puiNewVolume, false);
	}
}
