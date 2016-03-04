#ifndef BaseMaterialMonitorH
#define BaseMaterialMonitorH

#include "IMaterialMonitor.h"
#include <pthread.h>

using namespace std;
using namespace CryptoPP;

namespace RSCommon
{
	class RSCOMMON_LIB BaseMaterialMonitor : public IMaterialMonitor
	{
	public:
		BaseMaterialMonitor();
		virtual ~BaseMaterialMonitor();

		virtual int InitHW(unsigned char *pucCartridgesCount) = 0;
		virtual int SetPubKS(const unsigned char *aucPubKS);
		virtual int GetInPlaceStatus(int *pStatus) = 0;
#if (ENABLE_BURNING == 1)
		///
		/// Sets a certificate into a specified IDT.
		/// This method is available on test only.
		///
		virtual int BurnIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, const unsigned char *aucPubKS) = 0;
#endif
		///
		/// Removes cartridge.
		///
		virtual int RemoveCartridge(unsigned char ucCartridgeNum);
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume) = 0;
		///
		/// Verifies material consumption.
		//
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume) = 0;

		pthread_mutex_t GetMutex() const;

	protected:
		virtual int ResetCartridgeInfo(unsigned char ucCartridgeNum) = 0;
		PubKey* LoadPublicKey(const unsigned char *aucPubKS);

		bool m_bInitialized;
		pthread_mutex_t m_mtx;
		unsigned char m_ucCartridgesCount;
		PubKey* m_pubKS;
	};
}

#endif

