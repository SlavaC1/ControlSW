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
		virtual int Login() = 0;
		virtual int Configure() = 0;
		virtual int SetKeys(const unsigned char *aucPubKS) = 0;
		virtual int SetCounter(unsigned int uiCouterValue) = 0;
		virtual int GenerateCertificate(const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, unsigned char *aucCertificate, unsigned short *usCertificateLength) = 0;
		virtual int WriteCertificate(unsigned char *aucCertificate, unsigned short usCertificateLength) = 0;
		virtual int Logout() = 0;
#endif
		virtual int ReadIDCertificate(unsigned char *aucCertificate) = 0;
		virtual bool IsCertificateEmpty(const unsigned char *aucCertificate) = 0;
		virtual int ChallengeHostDevice(unsigned char ucCartridgeNum, const byte *aucHostChallenge, unsigned short usHostChallengeLength, byte *aucDeviceChallenge, unsigned short *pusDeviceChallengeLength, byte *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength) = 0;
		virtual int GetCurrentVolume(unsigned char ucCartridgeNum, unsigned int *puiCurrentVolume) = 0;
		virtual int DecreaseConsumption(unsigned char ucCartridgeNum, unsigned int uiConsumption, CounterResponse *newSignedVolume) = 0;

	protected:
		virtual void GenerateCertificate(const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char* aucQx, const unsigned char* aucQy, const unsigned char *aucPrvKS, unsigned char *aucCertificate, unsigned short *usCertificateLength);

		bool m_bIsInitialized;
		unsigned char m_ucCartridgesCount;
		unsigned char m_ucCurrentCartridgeNum;
	};
}

#endif