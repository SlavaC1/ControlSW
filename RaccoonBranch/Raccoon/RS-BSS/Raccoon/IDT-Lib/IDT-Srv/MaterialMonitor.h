#ifndef MaterialMonitorH
#define MaterialMonitorH

#include "IDTagAdapter.h"
#include "BaseMaterialMonitor.h"
#include "RSCommonDefinitions.h"

using namespace RSCommon;

namespace StratasysIDTSrv
{
#if defined(WIN32) || defined(_WIN32)
#	ifdef _LIB
#		define IDTSRV_LIB
#	elif IDTSRV_EXPORTS
#       define IDTSRV_LIB __declspec(dllexport)
#   elif defined(IDTSRV_IMPORT)
#       define IDTSRV_LIB __declspec(dllimport)
#   else
#       define IDTSRV_LIB
#   endif
#else
#   define IDTSRV_LIB
#endif

	typedef struct
	{
		const Verif* Verifier;
		const unsigned char *Message;
		unsigned short MessageLength;
		const unsigned char *SignedMessage;
		unsigned short SignedMessageLength;
		bool* Verified; 
	} VerifyParameters;

	class IDTSRV_LIB MaterialMonitor : public BaseMaterialMonitor
	{
	public:
		MaterialMonitor();
		virtual ~MaterialMonitor();

		virtual int InitHW(unsigned char *pucCartridgesCount);
		virtual int GetInPlaceStatus(int *pStatus);
#if (ENABLE_BURNING == 1)
		///
		/// Reset info on cartridge.
		/// This method is available on test only.
		///
		virtual int ResetCartridge(unsigned char ucCartridgeNum, const unsigned char* acPassword, unsigned char usPasswordLength, const unsigned char* aucPubKS);

		virtual int GetDeviceSerialNumber(unsigned char ucCartridgeNum, unsigned char* aucSerialNumber, unsigned char* pucSerialNumberLength);

		virtual int ReadTagPublicKey(unsigned char ucCartridgeNum, unsigned char* aucPubKC, unsigned short* pusKeyLength);

		virtual int BurnSignedIdd(unsigned char ucCartridgeNum, unsigned char* aucIdd, unsigned short usIddLength, unsigned char* aucSignature, unsigned short usSignatureLength);
#endif
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume);

#if (ENABLE_BURNING == 0)
	protected:
#endif
		/// <summary>
		/// Verifies cartridge material consumption, by decreasing volume to consume from cartridge current volume,
		/// checking if new value is equals to the expected value and verifying signature.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <param name="uiComsumption">The material volume to consume.</param>
		/// <param name="uiNewVolume">Cartridge new volume.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume, bool bSkipAuthentication);

		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, CounterResponse* pCounterResponse);

	protected:
		virtual int ResetCartridgeInfo(unsigned char ucCartridgeNum);

	private:

#if (ENABLE_BURNING == 1)
		int Login();
		int Configure();
		int GenerateKeys();
		int SetCounter(unsigned int couterValue);
		int GenerateCertificate(const unsigned char *aucIdd, unsigned short usIddLength, unsigned char *aucCertificate, unsigned short* pusCertificateLength);
		int WriteCertificate(unsigned char *aucCertificate, unsigned short* pusCertificateLength);
		int Logout();
#endif

		void InitCertificates();
		int ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* pusMaterialInfoLength);
		int ReadSignature(unsigned char* aucSignature, unsigned short* pusSignatureLength);
		void GenerateToken(unsigned char *aucToken, unsigned int blockSize);
		int ChallengeHostDevice(const unsigned char* aucHostChallenge, unsigned short usHostChallengeLength, unsigned char* aucDeviceChallenge, unsigned short* pusDeviceChallengeLength, unsigned char* aucSignedDeviceChallenge, unsigned short* pusSignedDeviceChallengeLength);

		VerifyParameters* CreateVerifyParameters(const Verif *verifier, const unsigned char *aucMessage, unsigned short usMessageLength, const unsigned char *aucSignedMessage, unsigned short usSignedMessageLength);
		int CreateVerifierThread(pthread_t* verifierThread, VerifyParameters* verifyParameters);

		int GetCurrentVolume(unsigned int *puiCurrentVolume);
		int DecreaseVolume(unsigned int uiConsumption, CounterResponse *newSignedVolume);
		bool VerifyVolume(const Verif *verifier, const CounterResponse &signedVolume);

		void SetVerifier(PubKey* publicKey);

	private:
		IDTagAdapter* m_tagAdapter;
		IdentificationData** m_certificates;
		Verif* m_veriferS;
		bool m_bVerified;
	};
}

extern "C" IDTSRV_LIB IMaterialMonitor* CreateMaterialMonitor();

#endif