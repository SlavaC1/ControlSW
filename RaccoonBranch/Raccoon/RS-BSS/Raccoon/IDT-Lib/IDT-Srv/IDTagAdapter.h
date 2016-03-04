#ifndef IDTagAdapterH
#define IDTagAdapterH

#include "IDCertificate.h"
#include "CounterResponse.h"
#include "RSCommonDefinitions.h"

using namespace RSCommon;

namespace StratasysIDTSrv
{
	typedef int (*pfnInit)(int *cartridgesCount);
	typedef int (*pfnGetInPlaceStatus)(int *status);
	typedef int (*pfnSelectChannel)(int cartridgeNum);

	class IDTagAdapter
	{
	public:
		IDTagAdapter();
		virtual ~IDTagAdapter();

		virtual int InitHW(unsigned char *pucCartridgesCount);
		bool IsInitialized() { return m_bIsInitialized; }
		unsigned char GetCartridgesCount() { return m_ucCartridgesCount; }
		virtual int GetInPlaceStatus(int *pStatus);
		virtual int GetSerialNumber(unsigned char *aucSerialNumber, unsigned char *ucSerialNumberLength) = 0;
		virtual int SwitchToCartridge(unsigned char ucCartridgeNum);
#if (ENABLE_BURNING == 1)
		virtual int Login(const unsigned char* acPassword, unsigned char usPasswordLength) = 0;
		virtual int Configure() = 0;
		virtual int SetKeys(PubKey* pubKS) = 0;
		virtual int SetCounter(unsigned int uiCouterValue) = 0;
		virtual int WriteMaterialInfo(unsigned char* aucMaterialInfo, unsigned short usMaterialInfoLength) = 0;
		virtual int WriteSignature(unsigned char* aucSignature, unsigned short usSignatureLength) = 0;
		virtual int Logout() = 0;
#endif
		virtual int ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* pusMaterialInfoLength) = 0;
		virtual int ReadSignature(unsigned char* aucSignature, unsigned short* pusSignatureLength) = 0;
		virtual bool IsFileEmpty(const unsigned char *aucFile, unsigned short usLength) = 0;
		virtual int ReadPubKC(unsigned char* aucPubKC, unsigned short* pusKeyLength) = 0;
		virtual int GenerateCertificate(const unsigned char* aucIdd, unsigned short usIddLength, unsigned char* aucCertificate, unsigned short* pusCertificateLength) = 0;
		virtual int ChallengeHostDevice(const unsigned char* aucHostChallenge, unsigned short usHostChallengeLength, unsigned char* aucDeviceChallenge, unsigned short* pusDeviceChallengeLength, unsigned char* aucSignedDeviceChallenge, unsigned short* pusSignedDeviceChallengeLength) = 0;
		virtual int GetCurrentVolume(unsigned int *puiCurrentVolume) = 0;
		virtual int DecreaseConsumption(unsigned int uiConsumption, CounterResponse *newSignedVolume) = 0;

	protected:
		bool m_bIsInitialized;
		unsigned char m_ucCartridgesCount;
	};
}

#endif