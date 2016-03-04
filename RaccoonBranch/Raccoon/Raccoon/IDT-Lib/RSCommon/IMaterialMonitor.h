#ifndef IMaterialMonitorH
#define IMaterialMonitorH

#include "stdafx.h"
#include "RSCommonDefinitions.h"

namespace RSCommon
{
	class RSCOMMON_LIB IMaterialMonitor
	{
	public:
		IMaterialMonitor();
		virtual ~IMaterialMonitor();

		/// <summary>
		/// Initializes underlying HW.
		/// </summary>
		/// <param name="pucCartridgesCount">Number of connected cartridges.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int InitHW(unsigned char *pucCartridgesCount) = 0;


		virtual bool IsBusy() = 0;

		/// <summary>
		/// Gets in-place status of all cartridges.
		/// </summary>
		/// <param name="pStatus">A 32-bit integer representing status of all cartridges.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int GetInPlaceStatus(int *pStatus) = 0;
		
		/// <summary>
		/// Removes cartridge.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int RemoveCartridge(unsigned char ucCartridgeNum) = 0;

		/// <summary>
		/// Authenticates cartridge by verifying its certificate and its signature of a random token.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <param name="isActive">Tag active or not.</param>
		/// <param name="aucIdd">The tag identification data (output parameter).</param>
		/// <param name="usIddLength">The tag identification data length (output parameter).</param>
		/// <param name="uiCurrentVolume">The tag current volume (output parameter).</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, bool isActive, unsigned char *aucIdd, unsigned short* pusIddLength, unsigned int* puiCurrentVolume) = 0;

		/// <summary>
		/// Verifies cartridge material consumption, by decreasing volume to consume from cartridge current volume,
		/// checking if new value is equals to the expected value and verifying signature.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <param name="uiComsumption">The material volume to consume.</param>
		/// <param name="uiNewVolume">Cartridge new volume.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *puiNewVolume) = 0;
        //ITAMAR RACCOON
		virtual int callCNOF(int SW, int SR, int LHO1200, int SRX, int IEF) = 0;
		virtual int callCEOP(int SOP, int SW, int SR, int LHO1200, int AF, int IEF) = 0;
		virtual int callCNOP(int SH, int SST, int SYO, int HPW, int SPEO, int CH) = 0;
		virtual int callCYSO(int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP) = 0;
	};
}

#endif