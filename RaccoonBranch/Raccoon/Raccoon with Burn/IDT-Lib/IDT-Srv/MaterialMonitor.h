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
		/// Sets a certificate into a specified IDT.
		/// This method is available on test only.
		///
		virtual int BurnIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, const unsigned char *aucPubKS);
		int EraseCartridge(unsigned char ucCartridgeNum);
		int ModifyIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, const unsigned char *aucPubKS);
#endif
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume);
		///
		/// Verifies material consumption.
		//
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume);

	protected:
		virtual int ResetCartridgeInfo(unsigned char ucCartridgeNum);

	private:
		int ReadIDCertificate(unsigned char *aucCertificate);
		void GenerateToken(unsigned char *aucToken, unsigned int blockSize);
		int ChallengeHostDevice(unsigned char ucCartridgeNum, const unsigned char *aucHostChallenge, unsigned short usHostChallengeLength, unsigned char *aucDeviceChallenge, unsigned short *pusDeviceChallengeLength, unsigned char *aucSignedDeviceChallenge, unsigned short *pusSignedDeviceChallengeLength);

		VerifyParameters* CreateVerifyParameters(const Verif *verifier, const unsigned char *aucMessage, unsigned short usMessageLength, const unsigned char *aucSignedMessage, unsigned short usSignedMessageLength);
		int CreateVerifierThread(pthread_t* verifierThread, VerifyParameters* verifyParameters);

		int GetCurrentVolume(unsigned char ucCartridgeNum, unsigned int *puiCurrentVolume);
		int DecreaseVolume(unsigned char ucCartridgeNum, unsigned int uiConsumption, CounterResponse *newSignedVolume);
		bool VerifyVolume(const Verif *verifier, const CounterResponse &signedVolume);

		void SetVerifier(PubKey* publicKey);

	private:
		IDTagAdapter* m_tagAdapter;
		IDCertificate** m_certificates;
		Verif* m_veriferS;
		bool m_bVerified;

#if (ENABLE_BURNING == 1)
		bool m_bCorrupt;
#endif

	};
}

extern "C" IDTSRV_LIB IMaterialMonitor* CreateMaterialMonitor();

#endif