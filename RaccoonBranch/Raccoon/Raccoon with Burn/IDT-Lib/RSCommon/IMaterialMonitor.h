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
		/// <summary>
		/// Sets Stratasys public key to use on cartridge authentication.
		/// </summary>
		/// <param name="aucPubKS">Stratasys public key.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int SetPubKS(const unsigned char *aucPubKS) = 0;
		/// <summary>
		/// Gets in-place status of all cartridges.
		/// </summary>
		/// <param name="pStatus">A 32-bit integer representing status of all cartridges.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int GetInPlaceStatus(int *pStatus) = 0;
#if (ENABLE_BURNING == 1)
		/// <summary>
		/// Generates certificate from material information and burns it into tag.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <param name="aucMaterialInfo">The material information.</param>
		/// <param name="usMaterialInfoLength">The material information length.</param>
		/// <param name="aucPrvKS">SSYS private key.</param>
		/// <param name="aucPubKS">SSYS public key.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int BurnIDC(unsigned char ucCartridgeNum, const unsigned char *aucMaterialInfo, unsigned short usMaterialInfoLength, const unsigned char *aucPrvKS, const unsigned char *aucPubKS) = 0;
#endif
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
		/// <param name="aucPubKS">Stratasys public key, null to use preset key.</param>
		/// <param name="aucIdd">The tag identification data (output parameter).</param>
		/// <param name="usIddLength">The tag identification data length (output parameter).</param>
		/// <param name="uiCurrentVolume">The tag current volume (output parameter).</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume) = 0;

		/// <summary>
		/// Verifies cartridge material consumption, by decreasing volume to consume from cartridge current volume,
		/// checking if new value is equals to the expected value and verifying signature.
		/// </summary>
		/// <param name="ucCartridgeNum">The cartridge number.</param>
		/// <param name="uiComsumption">The material volume to consume.</param>
		/// <param name="uiNewVolume">Cartridge new volume.</param>
		/// <returns>0 on success, non-zero for failures.</returns>
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume) = 0;
	};
}

#endif