//---------------------------------------------------------------------------

#ifndef AddRemoveRFIDCounterH
#define AddRemoveRFIDCounterH
#include "QTypes.h"
#include "GlobalDefs.h"
class CAddRemoveRFIDCounter
{
private:
	int m_NumOfReconnections;
public:
	int GetNumOfReconnections();
	void ResetCounter();
	void IncCounter();
	CAddRemoveRFIDCounter();
	~CAddRemoveRFIDCounter();
};
//---------------------------------------------------------------------------
#endif
