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

		virtual int InitHW(unsigned char *pucCartridgesCount);
		virtual int SetPubKS(const unsigned char *aucPubKS);
		virtual bool IsBusy();
		virtual int GetInPlaceStatus(int *pStatus) = 0;
#if (ENABLE_BURNING == 1)
		///
		/// Sets a certificate into a specified IDT.
		/// This method is available on test only.
		///
		virtual int ResetCartridge(unsigned char ucCartridgeNum, const unsigned char* acPassword, unsigned char usPasswordLength, const unsigned char* aucPubKS) = 0;

		virtual int GetDeviceSerialNumber(unsigned char ucCartridgeNum, unsigned char* aucSerialNumber, unsigned char* pucSerialNumberLength) = 0;

		virtual int ReadTagPublicKey(unsigned char ucCartridgeNum, unsigned char* aucPubKC, unsigned short* pusKeyLength) = 0;

		virtual int BurnSignedIdd(unsigned char ucCartridgeNum, unsigned char* aucIdd, unsigned short usIddLength, unsigned char* aucSignature, unsigned short usSignatureLength) = 0;
#endif
		///
		/// Removes cartridge.
		///
		virtual int RemoveCartridge(unsigned char ucCartridgeNum);
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume) = 0;
		///
		/// Verifies material consumption.
		//
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume);

		pthread_mutex_t GetMutex() const;

#if (ENABLE_BURNING == 0)
	protected:
#endif
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume, bool bSkipAuthentication) = 0;

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

