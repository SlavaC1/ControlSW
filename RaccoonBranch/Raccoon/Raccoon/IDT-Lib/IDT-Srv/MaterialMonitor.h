#ifndef MaterialMonitorH
#define MaterialMonitorH

#include "IDTagAdapter.h"
#include "BaseMaterialMonitor.h"
#include "RSCommonDefinitions.h"
#include "REDApp_utils.h"

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

	enum RunningMethod
	{
		runVerifyTag,
		runVerifyChallenge
	};

	typedef struct
	{
		hasp_handle_t handle;
		const unsigned char *Message;
		unsigned short MessageLength;
		bool* Verified; 
		bool isActive;
		int tagNo;
		RunningMethod method;
		unsigned char *random;
	} VerifyParameters;


	class IDTSRV_LIB MaterialMonitor : public BaseMaterialMonitor
	{
	public:
		MaterialMonitor();
		virtual ~MaterialMonitor();

		virtual int InitHW(unsigned char *pucCartridgesCount);
		virtual int GetInPlaceStatus(int *pStatus);
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, bool isActive, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume);
		///
		/// Removes cartridge.
		///
		virtual int RemoveCartridge(unsigned char ucCartridgeNum);

		//ITAMAR RACCOON
		virtual int callCNOF(int SW, int SR, int LHO1200, int SRX, int IEF);
		virtual int callCEOP(int SOP, int SW, int SR, int LHO1200, int AF, int IEF);
		virtual int callCNOP(int SH, int SST, int SYO, int HPW, int SPEO, int CH);
		virtual int callCYSO(int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP);

	protected:

		/// <summary>
		/// Verifies cartridge material consumption, by decreasing volume to consume from cartridge current volume,
		/// checking if new value is equals to the expected value and verifying signature.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <param name="uiComsumption">The material volume to consume.</param>
		/// <param name="uiNewVolume">Cartridge new volume.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume, bool bSkipAuthentication);

	protected:
		virtual int ResetCartridgeInfo(unsigned char ucCartridgeNum);

	private:

		int ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* pusMaterialInfoLength);
		int ReadSignature(unsigned char* aucSignature, unsigned short* pusSignatureLength);
		void GenerateToken(unsigned char *aucToken, unsigned int blockSize);
		int ChallengeHostDevice(const unsigned char* aucHostChallenge, unsigned short usHostChallengeLength, unsigned char* aucDeviceChallenge, unsigned short* pusDeviceChallengeLength,
			unsigned char* aucSignedDeviceChallenge, unsigned short* pusSignedDeviceChallengeLength);

		VerifyParameters* CreateVerifyParameters(hasp_handle_t handle, const unsigned char *aucMessage, unsigned short usMessageLength, 
			bool isActive, int tagNo,  RunningMethod method, unsigned char* random);

		int GetCurrentVolume(unsigned int *puiCurrentVolume);
		int DecreaseVolume(unsigned int uiConsumption, CounterResponse *newSignedVolume, unsigned char * random=NULL);

		void SetVerifier(PubKey* publicKey);
		void BuildSignedWeight(CounterResponse *newSighedVolume,byte *signedWeight);
		int UpdateConsumptionRedInteg(unsigned char ucCartridgeNum,int consumption, unsigned int *puiCurrentVolume);


	private:
		IDTagAdapter* m_tagAdapter;
		bool m_bVerified;

		hasp_handle_t m_AoC_handle;         /* RED API handle */

	};
}

extern "C" IDTSRV_LIB IMaterialMonitor* CreateMaterialMonitor();

#endif