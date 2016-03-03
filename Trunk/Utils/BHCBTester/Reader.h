//---------------------------------------------------------------------------

#ifndef ReaderH
#define ReaderH

#include "QUtils.h"
#include "QStdComPort.h"

class CReader
{
private:
	CQStdComPort *m_ComPort;
	
public:
	CReader(int ComNum);
	~CReader();

	void WriteToFPGA (WORD Add, WORD Data);
	WORD ReadFromFPGA(WORD Add);
};


#endif
