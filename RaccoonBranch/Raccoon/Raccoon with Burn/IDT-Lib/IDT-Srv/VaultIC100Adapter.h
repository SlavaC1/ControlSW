#pragma once

#include "IDTagAdapter.h"
#include "vaultic_common.h"
#include "vaultic_lib.h"

using namespace std;

namespace StratasysIDTSrv
{

#ifdef ENABLE_USB
	/*******************************************************
	Error Codes:
	*******************************************************/
#define VLT_FIND_DEVS_MALLOC_FAIL			0xE116
#define VLT_MALFORMED_XML_AARDVARK_INTFC	0xE116
#define VLT_NO_READER_CONNECTED				0xE116
#endif

	//
	// Host ID key define, note the total length must be 16
	static unsigned char aucHostID[] =
	{
		'S','t','r','a','t','a','s','y','s','P','r','i','n','t','e','r'
	};

	class VaultIC100Adapter :
		public IDTagAdapter
	{
	public:
		VaultIC100Adapter();
		~VaultIC100Adapter();

		virtual int InitHW(unsigned char *pucCartridgesCount);
		virtual int GetInPlaceStatus(int *pStatus);
		virtual int GetSerialNumber(unsigned char *aucSerialNumber, unsigned char *ucSerialNumberLength);
		virtual int SwitchToCartridge(unsigned char ucCartridgeNum);
#if (ENABLE_BURNING == 1)
		virtual int Login();
		virtual int Configure();
		virtual int SetKeys(const unsigned char *aucPubKS);
		virtual int SetCounter(unsigned int uiCouterValue);
		virtual int GenerateCertificate(const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, unsigned char *aucCertificate, unsigned short *usCertificateLength);
		virtual int WriteCertificate(unsigned char *aucCertificate, unsigned short usCertificateLength);
		virtual int Logout();
#endif
		virtual int ReadIDCertificate(unsigned char *aucCertificate);
		virtual bool IsCertificateEmpty(const unsigned char *aucCertificate);
		virtual int ChallengeHostDevice(unsigned char ucCartridgeNum, const byte *aucHostChallenge, unsigned short usHostChallengeLength, byte *aucDeviceChallenge, unsigned short *pusDeviceChallengeLength, byte *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength);
		virtual int GetCurrentVolume(unsigned char ucCartridgeNum, unsigned int *puiCurrentVolume);
		virtual int DecreaseConsumption(unsigned char ucCartridgeNum, unsigned int uiConsumption, CounterResponse *newSignedVolume);

	private:
#ifdef ENABLE_USB
		int ParseDeviceSerialNumber(unsigned int *puiDeviceSerialNumber);
		int ParseDeviceSerialNumber(string strXmlDevs, unsigned int *puiDeviceSerialNumber);
#endif
		int InitLibrary(unsigned char ucCartridgeNum, unsigned int uiDeviceSerialNumber);
		void Cleanup();

		VAULTIC_API *m_theBaseApi;
		VLT_TARGET_INFO *m_cartridgeInfo;
		bool m_bUseUsb;
	};

	enum enDeviceKeyTypes
	{
		enDeviceKeyGroup = 1, 
		enHostKeyGroup = 2,
		enPublicKeyIndex = 1, 
		enPrivateKeyIndex = 2,
		enDomainParamsKeyIndex = 3,
		enHostDeviceIDKeyIndex = 4,    
	};
}
