/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: EOL 8051 h/w                                             *
 * Module Description: This class implement services related to     *
 *                     End OF Liquid verifications                  *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/

#ifndef _CONTAINER_H_
#define _CONTAINER_H_


#include "OCBCommDefs.h"
#include "QMessageQueue.h"
#include "ContainerBase.h"
#include "Raccoon.h"

typedef int TAG_PARAM_ID;

struct TContainerIdentifyNotificationMessage
{
	TTagIdentifyNotificationMessage MessageBody;
	TTankIndex                      TankIndex;
};

// Forward declaration:
class CContainersDispatcher;
class CConsumptionUpdater;

class CContainer : public CContainerBase, public Raccoon::IRaccoonObserver
{

private:

	bool m_FlagGetStatusUnderUse;

	//Flag ack ok
	bool LiquidTank_AckOk;

	bool m_WasteAlert;

	int m_TanksInPlacePrevStatus;

	//Liquid weight tank
	static void GetLiquidTanksWeightAckResponse(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

	//This procedure is the callback for Set Active Pumps
	static void SetActiveTankAckResponse(int TransactionId, PVOID Data,
	                                     unsigned DataLength, TGenericCockie Cockie);

	//This procedure is the callback for Set Drain Pumps
	static void SetDrainTankAckResponse(int TransactionId, PVOID Data,
	                                    unsigned DataLength, TGenericCockie Cockie);

	virtual void SetIdentificationCompletedCallback(TTankIndex Tank, TStateEnterCallback CallbackFunction, TGenericCockie Cockie);
	static void RemountSingleTankCallback(TTankIndex Tank, TGenericCockie Cockie);
	static void IdentificationCompletedCallback(TTankIndex Tank, TGenericCockie Cockie);

	// Replay Ack to OCb
	void AckToOcbNotification(int MessageID, int TransactionID, int AckStatus, TGenericCockie Cockie);

	void UpdateLiquidTankInserted(TTankIndex Tank, bool Inserted);

	CContainersDispatcher* m_ContainerDispatcher;
	CConsumptionUpdater*   m_ConsumptionUpdater;

	void UpdateTankStatus(int TankIndex, TTagIdentificationEventType Status, const unsigned int Weight = 0, const unsigned int SliceNum = 0);

public:
	// Constructor
	CContainer(const QString& Name);

	// Destructor
	~CContainer(void);

	TQErrCode GetIfAllLiquidTankAreInserted(void);

	//This procedure verify if liquid tank is inserted
	TQErrCode GetLiquidTankInsertedStatus();
	TQErrCode GetLiquidTankInsertedStatusFiltered(int /* TTankIndex */ Tank = ALL_TANKS);

	void Cancel(void);
	int GetTotalWeight(int);

	bool GetWasteWeightStatusDuringPrinting();
	bool GetWasteWeightStatusBeforePrinting();

	bool SetContainerGain(int /* TTankIndex */ Tank, float Gain);
	bool SetContainerOffset(int /* TTankIndex */ Tank, float Offset);

	bool SetActiveContainersBeforePrinting(bool PrintToLog);

	bool RemountSingleTankStatus(int /* TTankIndex */ Tank);
	bool WaitForIdentificationCompletion(int /* TTankIndex */ Tank, bool DontWaitIfTankIsOut, unsigned int timetowait);

	//Liquid weight tank
	TQErrCode GetLiquidTanksWeight(void);

	// Command to set CContainer Active Pumps
	TQErrCode SetActiveTank(TTankIndexArray&, bool PrintToLog = true);

	// Command to set CContainer Drain Pumps
	TQErrCode SetDrainTanks(TTankIndex* DrainPumps, bool PrintToLog = true);

	virtual void TanksStatusLogic(TTankIndex TankIndex);

	int  GetTankWeight(TChamberIndex, TTankIndex);

	virtual unsigned int GetLastInsertionTimeInTicks(int /* TTankIndex */);

	QString GetTagIDAsString(TTankIndex TankIndex);
	QString GetPipeMaterialType(TTankIndex TankIndex);
	unsigned long GetContainerExpirationDate(TTankIndex);
	bool IsContainerInserted(TTankIndex TankIndex);
	bool IsResinExpired(TTankIndex TankIndex);
	bool IsMaxPumpTimeExceeded(TTankIndex TankIndex);
	bool SendNotification(TContainerIdentifyNotificationMessage &Message);
	void HandleTankIdentificationDlgClose(TTankIndex TankIndex, int Result);
	TQErrCode SendTanksRemovalSignals();
	TQErrCode IgnoreTankInsertionAndRemovalSignals();
	TQErrCode AllowTankInsertionAndRemovalSignals();
	void SetAllowServiceMaterials(TTankIndex TankIndex, bool val);
	void SetSkipMachineResinTypeChecking(TTankIndex TankIndex, bool val);
	void NotifyInplaceChange(int AllTanksStatus);
	void NotifyConsumptionReachedZero(int TankIndex);
	void UpdateConsumption(TTankIndex TankIndex, const unsigned int Weight, const unsigned int Slices = 0);
	unsigned int GetTankCurrentWeightInTag(TTankIndex TankIndex);
	bool IsTankInOverconsumption(TTankIndex TankIndex);
	bool IsConsumptionUpdateNeeded(TTankIndex TankIndex);
	Raccoon::TTagInfo* GetTagInfo(TTankIndex TankIndex);
	void GetConsumptionData(TTankIndex TankIndex, int &Weight, int &Slice);
	void ResetConsumedData(TTankIndex TankIndex, int CurrentSlice);
	void InitRaccoon();
};//class CContainer


class CContainersDispatcher : public CQThread
{
private:

	typedef CQMessageQueue<TContainerIdentifyNotificationMessage> TContainerIDEventQueue;
	TContainerIDEventQueue* m_SMContainerIDEventQueue;

	CContainer* m_ContainerPtr;

public:

	CContainersDispatcher();
	~CContainersDispatcher();

	void SetContainerPtr(CContainer* cont_ptr);
	bool SendNotification(TContainerIdentifyNotificationMessage& Message);
	void Execute(void);
};

// Check each tank if weight needed for consumption update is accumulated and update
class CConsumptionUpdater : public CQThread
{
private:

	CContainer *m_ContainerPtr;
	int         m_CurrentTank;

public:

	CConsumptionUpdater();
	~CConsumptionUpdater();

	void SetContainerPtr(CContainer *cont_ptr);

	void Execute(void);
};

#endif

