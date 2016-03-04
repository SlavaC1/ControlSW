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
		virtual int ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* pusMaterialInfoLength);
		virtual int ReadSignature(unsigned char* aucSignature, unsigned short* pusSignatureLength);
		virtual bool IsFileEmpty(const unsigned char *aucFile, unsigned short usLength);
		virtual int ReadPubKC(unsigned char* aucPubKC, unsigned short* pusKeyLength);
		virtual int GenerateCertificate(const unsigned char* aucIdd, unsigned short usIddLength, unsigned char *aucCertificate, unsigned short* pusCertificateLength);
		virtual int ChallengeHostDevice(const unsigned char* aucHostChallenge, unsigned short usHostChallengeLength, unsigned char* aucDeviceChallenge, unsigned short* pusDeviceChallengeLength, unsigned char* aucSignedDeviceChallenge, unsigned short* pusSignedDeviceChallengeLength);
		virtual int GetCurrentVolume(unsigned int *puiCurrentVolume);
		virtual int DecreaseConsumption(unsigned int uiConsumption, CounterResponse *newSignedVolume, unsigned char *random=NULL);
//		virtual int DecreaseConsumption(unsigned int uiConsumption, CounterResponse *newSignedVolume);

	private:
#ifdef ENABLE_USB
		bool InitUsbDevice();
		int ParseDeviceSerialNumber(unsigned int *puiDeviceSerialNumber);
		int ParseDeviceSerialNumber(string strXmlDevs, unsigned int *puiDeviceSerialNumber);
#endif
		int GetPaddedLength(int length, int padding) { return (int)(ceil(length / (float)padding) * padding); }
		int InitLibrary(unsigned char ucCartridgeNum, unsigned int uiDeviceSerialNumber);
		int SelectError(int returnedError, int suggestedValue);
		void Cleanup();

		VAULTIC_API *m_theBaseApi;
		VLT_TARGET_INFO *m_cartridgeInfo;
		bool m_bIsFcbInitialized;
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
