//---------------------------------------------------------------------------


#pragma hdrstop

#include "FixedSizeQueue.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

CFixedSizeQueue::CFixedSizeQueue(unsigned int Size, int Datum)
: m_Size(Size), m_Head(0)
{
	m_Data = new int[Size];
	if (m_Data)
		for (unsigned int i = 0; i<Size;)
			m_Data[i++]=Datum;
}

CFixedSizeQueue::~CFixedSizeQueue()
{
	if (!m_Data) delete[] m_Data;
}

int CFixedSizeQueue::Insert(int Datum)
{
	int oldValue;
	m_MutexArrGuard.WaitFor();
		oldValue = m_Data[m_Head];
		m_Data[m_Head++] = Datum;
		if (m_Head == m_Size)
			m_Head = 0; //confine to queue's bounds; faster than modulus
	m_MutexArrGuard.Release();
	return oldValue;
}

	//returns the nth 'newest' datum found Idx cells from the array's head, 0-based
int CFixedSizeQueue::operator[](unsigned int Idx)
{
	if (Idx >= (unsigned)m_Size) throw EQException("Out of bounds");
	int datum;
	m_MutexArrGuard.WaitFor();
		int tmpIdx = (m_Head -1 -Idx);
		if (tmpIdx<0) tmpIdx += m_Size;
		datum = m_Data[tmpIdx];
	m_MutexArrGuard.Release();
	return datum;
}

