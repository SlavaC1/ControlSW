#ifndef BaseMaterialMonitorH
#define BaseMaterialMonitorH

#include "IMaterialMonitor.h"
#include <pthread.h>

using namespace std;

namespace RSCommon
{
	class RSCOMMON_LIB BaseMaterialMonitor : public IMaterialMonitor
	{
	public:
		BaseMaterialMonitor();
		virtual ~BaseMaterialMonitor();

		virtual int InitHW(unsigned char *pucCartridgesCount);
		virtual bool IsBusy();
		virtual int GetInPlaceStatus(int *pStatus) = 0;
		///
		/// Removes cartridge.
		///
		virtual int RemoveCartridge(unsigned char ucCartridgeNum);
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, bool isActive, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume) = 0;
		///
		/// Verifies material consumption.
		//
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume);

		pthread_mutex_t GetMutex() const;
		virtual int callCNOF(int SW, int SR, int LHO1200, int SRX, int IEF) = 0;
		virtual int callCEOP(int SOP, int SW, int SR, int LHO1200, int AF, int IEF) = 0;
		virtual int callCNOP(int SH, int SST, int SYO, int HPW, int SPEO, int CH) = 0;
		virtual int callCYSO(int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP) = 0;

	protected:
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume, bool bSkipAuthentication) = 0;

	protected:
		virtual int ResetCartridgeInfo(unsigned char ucCartridgeNum) = 0;

		bool m_bInitialized;
		pthread_mutex_t m_mtx;
		unsigned char m_ucCartridgesCount;
	};
}

#endif

