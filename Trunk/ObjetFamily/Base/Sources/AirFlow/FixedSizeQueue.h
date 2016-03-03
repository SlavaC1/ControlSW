//---------------------------------------------------------------------------
/* This is a basic (thread-safe) cyclic array-based queue of integers
 * (can be generalized for void* typed data and/or usage of a template).
 */

#ifndef FixedSizeQueueH
#define FixedSizeQueueH
//---------------------------------------------------------------------------
#include "QMutex.h"

class CFixedSizeQueue{
private:
	int m_Size;
protected:
	int m_Head; //index where new values can enter the queue (i.e. the oldest entry)
	int* m_Data;

    CQMutex m_MutexArrGuard;	

public:
	CFixedSizeQueue(unsigned int Size, int Datum=0);
	//We don't want to prevent the deletion of an instance through a base class pointer, 
	//so no need to make the base class' d'tor protected and nonvirtual.
	//Also, we want to force calling this d'tor (to dealloc mem) so no need for the virtual modifer.
	~CFixedSizeQueue(); 

	int Insert(int Datum);
	//returns the datum found Idx cells from the array's head (i.e. from the oldest entry), 0-based
	int operator[](unsigned int Idx);
	int GetSize() const { return m_Size; }
};
#endif
