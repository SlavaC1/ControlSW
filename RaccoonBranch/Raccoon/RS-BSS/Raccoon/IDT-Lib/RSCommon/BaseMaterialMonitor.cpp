#include "stdafx.h"
#include "BaseMaterialMonitor.h"
#include "UniqueLock.h"
#include "Utils.h"
#include "oids.h"
#include "osrng.h"

namespace RSCommon
{
	BaseMaterialMonitor::BaseMaterialMonitor()
		: m_bInitialized(false), m_ucCartridgesCount(0), m_pubKS(NULL)
	{
		pthread_mutex_init(&m_mtx, NULL);
	}

	BaseMaterialMonitor::~BaseMaterialMonitor()
	{
		pthread_mutex_destroy(&m_mtx);
		if (m_pubKS != NULL)
		{
			delete m_pubKS;
		}
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

	int BaseMaterialMonitor::SetPubKS(const unsigned char *aucPubKS)
	{
		if (m_pubKS != NULL)
		{
			delete m_pubKS;
		}

		m_pubKS = LoadPublicKey(aucPubKS);
		if (m_pubKS == NULL)
		{
			return INVALID_HOST_KEY;
		}

		return IDTLIB_SUCCESS;
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

	PubKey* BaseMaterialMonitor::LoadPublicKey(const unsigned char *aucPubKS)
	{
		PubKey* pubKey = NULL;

		try
		{
			LOG_EXT(LEVEL_INFO, "Loading host public key...");
			EC2NPoint Q;
			Q.identity = false;
			int offset = 0;
			Q.x.Decode(aucPubKS + offset, KEY_COEFF_SIZE);
			offset += KEY_COEFF_SIZE;
			Q.y.Decode(aucPubKS + offset, KEY_COEFF_SIZE);

			pubKey = new PubKey();
			pubKey->Initialize(CURVE, Q);

			AutoSeededRandomPool prng;
			bool ok = pubKey->Validate(prng, 3);
			if (!ok)
			{
				delete pubKey;
				LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)INVALID_HOST_KEY << ").");
				return NULL;
			}

			return pubKey;
		}
		catch (exception &e)
		{
			if (pubKey != NULL)
			{
				delete pubKey;
			}

			LOG_EXT(LEVEL_ERROR, "Invalid host key (error code 0x" << hex << (short)INVALID_HOST_KEY << "): " << e.what() << ".");
			return NULL;
		}
	}

	int BaseMaterialMonitor::UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume)
	{
		return UpdateConsumption(ucCartridgeNum, uiComsumption, puiNewVolume, false);
	}
}
