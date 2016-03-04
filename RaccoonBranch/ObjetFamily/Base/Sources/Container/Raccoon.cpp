/********************************************************************
 *                        Stratasys LTD.                            *
 *                        --------------                            *
 * Project: Objet500.                                               *
 * Module:  Raccoon module                                          *
 * Module Description: Material cartridges identification and       *
 *                    authentication                                *
 *                                                                  *
 * Compilation: Standard C++                                        *
 *                                                                  *
 * Author: Slava Chuhovich                                          *
 * Start date: 25/08/2014                                           *
 * Last upate:                                                      *
 ********************************************************************/

#pragma hdrstop
#include "Raccoon.h"
#pragma package(smart_init)

#include "AppLogFile.h"
#include "Utils.h"
#include "GlobalDefs.h"

using namespace Raccoon;

// Singleton instance
CRaccoon* CRaccoon::m_SingletonInstance = NULL;

void CRaccoon::Init()
{
	if(! m_SingletonInstance)
		m_SingletonInstance = new CRaccoon();
}

void CRaccoon::DeInit()
{
	Q_SAFE_DELETE(m_SingletonInstance);
}

// Get singletone instance
CRaccoon* CRaccoon::Instance()
{
	return m_SingletonInstance;
}

//----------------------------------------------------------------------

// Constructor
CRaccoon::CRaccoon() : CQThread(true, "Raccoon")
{
	m_InPlaceStatus      = 0;
	m_InPlaceStatusPrev  = 0;
	m_ExistingCartridges = 0;

	// Setting logging callback method
	if(m_AppParams.IsLowLevelLoggingEnabled())
		RSCommon::SetLogMessageFunction(WriteToLogCallback);

	m_MaterialMonitor = CreateMaterialMonitor();
}

// Destructor
CRaccoon::~CRaccoon()
{
	Terminate();
	Q_SAFE_DELETE(m_MaterialMonitor);
}

void CRaccoon::InitHardwareAndStart()
{
	// Init Raccoon hardware
	int Status = m_MaterialMonitor->InitHW(&m_ExistingCartridges);
	if(Status != IDTLIB_SUCCESS)
	{
		QString str = QFormatStr("Raccoon hardware initialization failure. Status: %s (0x%X)", GetError(Status), Status);
		CQLog::Write(LOG_TAG_RACCOON, str.c_str());
        throw ERaccoon(str);
	}

	// Set public key. Need to set it only once and pass NULL aftewards where it's needeed

	/*BYTE key[SSYS_PUBLIC_KEY_SIZE];
	memset(key, 0, SSYS_PUBLIC_KEY_SIZE);

	HexStringToByteArray(PUBLIC_KEY.c_str(), PUBLIC_KEY.size(), key);
	*/
	
 /*	Status = m_MaterialMonitor->SetPubKS(PUBLIC_KEY);
	if(Status != IDTLIB_SUCCESS)
	{
		QString str = QFormatStr("Public key setting failure. Status: %s (0x%X)", GetError(Status), Status);
		CQLog::Write(LOG_TAG_RACCOON, str.c_str());
        throw ERaccoon(str);
	}
     */
	this->Resume();

	CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Raccoon system initialized. Number of possible cartridges: %d", m_ExistingCartridges));
}

//----------------------------------------------------------------------

void CRaccoon::Execute()
{
	while(! Terminated)
	{
		int Status = m_MaterialMonitor->GetInPlaceStatus(&m_InPlaceStatus);
		if(Status != IDTLIB_SUCCESS)
		{
			QString str = QFormatStr("GetInPlaceStatus failure. Status: %s (0x%X)", GetError(Status), Status);
			CQLog::Write(LOG_TAG_RACCOON, str.c_str());
		}		

#ifdef _DEBUG
		CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Tanks in-place: 0x%X", m_InPlaceStatus));
#endif		

		// Notify the container of insertion or removal. Only CContainer observer is actually currently registered
		for(TRaccoonObserversList::iterator it = m_InplaceChangeObservers.begin(); it != m_InplaceChangeObservers.end(); ++it)
		{
#ifdef _DEBUG
			if(FindWindow(0, "Tank1Inserted.txt - Notepad"))
                m_InPlaceStatus |= 1;
			if(FindWindow(0, "Tank1Removed.txt - Notepad"))
                m_InPlaceStatus &= ~(1);
#endif
            if(m_InPlaceStatus != m_InPlaceStatusPrev)
                (*it)->NotifyInplaceChange(m_InPlaceStatus);
			else
				continue;
		}

		m_InPlaceStatusPrev = m_InPlaceStatus;

		PausePoint(IN_PLACE_CHECK_PERIOD);
	}
}

//----------------------------------------------------------------------

void CRaccoon::WriteToLogCallback(const char* str)
{
	CQLog::Write(LOG_TAG_RACCOON, str);
}

// Mark the cartidge as 'removed'
void CRaccoon::RemoveCartridge(const int CartridgeNum)
{
	int Status = m_MaterialMonitor->RemoveCartridge(CartridgeNum);
	if(Status != IDTLIB_SUCCESS)
	{
		QString str = QFormatStr("Error during cartridge %d removal. Status: %s (0x%X)", CartridgeNum, GetError(Status), Status);
		CQLog::Write(LOG_TAG_RACCOON, str.c_str());
		return;
	}

	CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Cartridge %s (%d) removed", TankToStr(static_cast<TTankIndex>(CartridgeNum)).c_str(), CartridgeNum));
}

//ITAMAR RACCOON
int CRaccoon::R_CNOF(int SW, int SR, int LHO1200, int SRX, int IEF)
{
	CQLog::Write(LOG_TAG_RACCOON, "CNOF Raccoon");
	return m_MaterialMonitor->callCNOF(SW, SR, LHO1200, SRX, IEF);
}
int CRaccoon::R_CEOP(int SOP, int SW, int SR, int LHO1200, int AF, int IEF)
{
	CQLog::Write(LOG_TAG_RACCOON, "CEOP Raccoon");
	return m_MaterialMonitor->callCEOP(SOP, SW, SR, LHO1200, AF, IEF);
}
int CRaccoon::R_CNOP(int SH, int SST, int SYO, int HPW, int SPEO, int CH)
{
	CQLog::Write(LOG_TAG_RACCOON, "CNOP Raccoon");
	return m_MaterialMonitor->callCNOP(SH, SST, SYO, HPW, SPEO, CH);
}
int CRaccoon::R_CYSO(int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP)
{
	CQLog::Write(LOG_TAG_RACCOON, "CYSO Raccoon");
	return m_MaterialMonitor->callCYSO(SH, SST, SYO, HPW, NGIP, NOP, SPEO, YINOP, YSINOP);
}

// Perform authentication for specific cartridge
void CRaccoon::Authenticate(TTagInfo *TagInfo)
{
#ifdef _DEBUG
	if(FindWindow(0, "Tank1AuthFailed.txt - Notepad"))
		throw ERaccoon("Tank 1 authentication failed");
#endif

	unsigned char  idd[IDD_SIZE];
	unsigned short iddLength;

	memset(idd, 0, IDD_SIZE);

	int Status = m_MaterialMonitor->AuthenticateCartridge(TagInfo->TankIndex, true, idd, &iddLength, &(TagInfo->CurrentWeight));
	if(Status != IDTLIB_SUCCESS)
	{
		QString str = QFormatStr("Error during cartridge %d authentication. Status: %s (0x%X)", TagInfo->TankIndex, GetError(Status), Status);
		CQLog::Write(LOG_TAG_RACCOON, str.c_str());

#ifdef _DEBUG
		if(FindWindow(0, "Tank1Inserted.txt - Notepad"))
		{
			TagInfo->TankIndex                   = 0;
			TagInfo->SerialNumber                = 1;
			TagInfo->CurrentWeight               = 3000;
			TagInfo->MaterialInfo.TagStructID    = 123456;
			TagInfo->MaterialInfo.MaterialType   = 720;
			TagInfo->MaterialInfo.MfgDateTime    = 0;
			TagInfo->MaterialInfo.ExpirationDate = 0;
			TagInfo->MaterialInfo.InitialWeight  = 3600;
			TagInfo->InitialWeight_mGr           = TagInfo->MaterialInfo.InitialWeight * 1000;
			memset(TagInfo->MaterialInfo.BatchNo, 0, sizeof(TagInfo->MaterialInfo.BatchNo));
		}
		else
			throw ERaccoon(str);
#else
		throw ERaccoon(str);
#endif
	}          

	// Decode material info from buffer
	unsigned short MaterialInfoLength = (unsigned short)TagInfo->MaterialInfo.Decode(idd);

	// Copy tag serial number (not a part of MaterialInfo class) from buffer
	BYTE SerialNumberSize;
	BYTE SerialNumber[MAX_SERIAL_NUMBER_SIZE];

	memcpy(&SerialNumberSize, idd + MaterialInfoLength, 1);
	memcpy(SerialNumber,      idd + MaterialInfoLength + 1, SerialNumberSize);

	// Reverse the byte order
	TagInfo->SerialNumber = 0;
	for(int i = 0; i < SerialNumberSize; i++)
	{
		TagInfo->SerialNumber = TagInfo->SerialNumber << 8;
		TagInfo->SerialNumber = TagInfo->SerialNumber | SerialNumber[i];
	}

	// Store initial weight in mGr
    TagInfo->InitialWeight_mGr = TagInfo->MaterialInfo.InitialWeight * 1000;

	CQLog::Write(LOG_TAG_RACCOON, QFormatStr("Cartridge %s (%d) authenticated", TankToStr(static_cast<TTankIndex>(TagInfo->TankIndex)).c_str(), TagInfo->TankIndex));
}

// Update material consumption
void CRaccoon::UpdateConsumption(TTagInfo *TagInfo, const unsigned int Consumption_mGr)
{
	/*check what I need to put in last value*/
	int Status = m_MaterialMonitor->UpdateConsumption(TagInfo->TankIndex, Consumption_mGr, &(TagInfo->CurrentWeight));

	if(Status != IDTLIB_SUCCESS)
	{
		QString str = QFormatStr("Error during cartridge %d consumption update. Status: %s (0x%X)", TagInfo->TankIndex, GetError(Status), Status);

		if(Status == MATERIAL_OVERCONSUMPTION)
		{
			for(TRaccoonObserversList::iterator it = m_InplaceChangeObservers.begin(); it != m_InplaceChangeObservers.end(); ++it)
			{
                // Zero the current weight in tag in case of overconsumption
				UpdateConsumption(TagInfo, TagInfo->CurrentWeight);

				CQLog::Write(LOG_TAG_RACCOON, QFormatStr("No remaining cartridge %s (%d) weight after consumption update. Cartridge removed", TankToStr(static_cast<TTankIndex>(TagInfo->TankIndex)).c_str(), TagInfo->TankIndex));
				(*it)->NotifyConsumptionReachedZero(TagInfo->TankIndex);
			}
		}

		CQLog::Write(LOG_TAG_RACCOON, str.c_str());
		return;
	}

	QString str = QFormatStr("Consumption update. Cartridge: %s (%d), Weight to reduce: %.2f Gr, Remaining weight: %.2f Gr", TankToStr(static_cast<TTankIndex>(TagInfo->TankIndex)).c_str(),
	                         TagInfo->TankIndex, MilliGramsToGrams(Consumption_mGr),
	                         MilliGramsToGrams(TagInfo->CurrentWeight));
	CQLog::Write(LOG_TAG_RACCOON, str.c_str());
}

//----------------------------------------------------------------------

void CRaccoon::AddInplaceChangeObserver(IRaccoonObserver *Observer)
{
	m_InplaceChangeObservers.push_back(Observer);
}

//----------------------------------------------------------------------

bool CRaccoon::IsBusy()
{
   return m_MaterialMonitor->IsBusy();
}
