#pragma once

#include "stdafx.h"
#include "BaseMaterialMonitor.h"
#include "Behavior.h"
#include <pthread.h>

using namespace RSCommon;

namespace RSSim
{

#if defined(WIN32) || defined(_WIN32)
#   ifdef RSSIM_EXPORTS
#       define RSSIM_LIB __declspec(dllexport)
#   elif defined(RSSIM_IMPORT)
#       define RSSIM_LIB __declspec(dllimport)
#   else
#       define RSSIM_LIB
#   endif
#else
#   define RSSIM_LIB
#endif

	static void *LoadBehaviorTimeline(void* arg);
	static void LoadBehavior(void* arg, const char* fileName);

	class MaterialMonitorSim : public BaseMaterialMonitor
	{
	public:
		MaterialMonitorSim();
		virtual ~MaterialMonitorSim();

		virtual int InitHW(char* pcHsmTarget, unsigned short pcHsmTargetLength, unsigned char *pucCartridgesCount);
		virtual int GetInPlaceStatus(int *pStatus);
		///
		/// Authenticates cartridge.
		///
		virtual int AuthenticateCartridge(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume);
		///
		/// Verifies material consumption.
		//
		virtual int UpdateConsumption(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume);

		string GetTimelineFile() const;
		pthread_cond_t GetSimLifetimeCV() const;
		pthread_mutex_t GetSimLifetimeMutex() const;
		Behavior* GetBehavior() const;
		void SetBehavior(Behavior* behavior);

	protected:
		virtual int ResetCartridgeInfo(unsigned char ucCartridgeNum);

	private:
		int ReadCounter(unsigned char ucCartridgeNum, unsigned int *uiCurrentVolume);
		int WriteCounter(unsigned char ucCartridgeNum, unsigned int uiNewVolume);

		static string GetCartridgeFileName(unsigned char ucCartridgeNum, const char* aucBaseFileName);
		static bool IsCartridgeOn(unsigned char ucCartridgeNum, int status);
		int GetAuthenticateCartridgeBehavior(unsigned char ucCartridgeNum, const unsigned char *aucPubKS, unsigned char *aucIdd, unsigned short *usIddLength, unsigned int *uiCurrentVolume);
		int GetUpdateConsumptionBehavior(unsigned char ucCartridgeNum, unsigned int uiComsumption, unsigned int *uiNewVolume);

    private:
		int m_authenticated;
		string m_timelineFile;
		Behavior* m_behavior;

		pthread_t m_thLoadBehaviorTimeline;
		pthread_attr_t m_thLoadBehaviorTimelineAttr;
		pthread_cond_t m_simLifetimeCV;
		pthread_mutex_t m_simLifetimeMutex;
	};
}

extern "C" RSSIM_LIB IMaterialMonitor* CreateMaterialMonitor();
