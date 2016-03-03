
#ifndef ReaderThreadH
#define ReaderThreadH

#include "QThread.h"
#include "ReaderDsp.h"
#include "ReaderDefs.h"

class CReaderThread : public CQThread
{
private:

	TTestData   m_TestData;
	CReaderDsp *m_Reader;

	int ContainerToChannel(TContainer ContainerNum);

public:

	CReaderThread();
	~CReaderThread();

	void Execute();

	void SetTestData(TTestData &TestData);
};



















#endif
