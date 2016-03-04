#include "stdafx.h"
#include "BaseMaterialMonitor.h"
#include "UniqueLock.h"
#include "Utils.h"

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
		unsigned char buffer[PUBLIC_KEY_SIZE];
		PubKey* pubKey = NULL;

		try
		{
			LOG_EXT(LEVEL_INFO, "Loading host public key...");
			memcpy(buffer, aucPubKS, PUBLIC_KEY_SIZE);
			pubKey = new PubKey();
			pubKey->Load(StringSource(buffer, PUBLIC_KEY_SIZE, true /*pump all*/).Ref());
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
}