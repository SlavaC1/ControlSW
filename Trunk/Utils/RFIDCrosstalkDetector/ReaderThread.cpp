#pragma hdrstop
#include "ReaderThread.h"
#pragma package(smart_init)

#include "DetectorUI.h"
#include "QThreadUtils.h"

const int COM_BAUD      = 9600;
const int COM_PARITY    = 0;
const int COM_DATA_BITS = 8;
const int COM_STOP_BITS = 1;

CReaderThread::CReaderThread() : CQThread(true, "Reader")
{
	m_Reader = new CReaderDsp();
}

CReaderThread::~CReaderThread()
{
	Q_SAFE_DELETE(m_Reader);
}

void CReaderThread::Execute()
{
	while(! Terminated)
	{
		if(m_Reader->Connect(m_TestData.SelectedCOM, COM_BAUD, COM_PARITY, COM_DATA_BITS, COM_STOP_BITS) == ERROR_SUCCESS)
		{
			int TestsDone = 0;

			while(TestsDone < m_TestData.TestingCycles)
			{
				for(int i = 0; i < NUM_OF_CONTAINERS; i++)
				{
					m_Reader->SetChannel(ContainerToChannel(static_cast<TContainer>(i)));

					if(m_Reader->CheckTags() > 0)
					{
						// Incrementing the 'count' of this 'Tag ID'.
						// If no such Tag ID found in the map, it is added and 	it's counter incremented
						__int64 TagID = m_Reader->GetTags()[0].GetUID();
						m_TestData.TestResults.ResultsPerTag[i][TagID]++;
					}
					else
					{
						continue;
					}

					QSleep(10);
				}

				TestsDone++;
				DetectorForm->UpdateProgress(TestsDone);
			}

			m_Reader->Disconnect();
			DetectorForm->ShowResults(m_TestData);
		}
		else // Handle the Connect error
		{
			DetectorForm->ReportError(QFormatStr("Unable to connect to RF reader on %s. Select different port.", COMPorts[m_TestData.SelectedCOM - 1].c_str()));
        }

		Suspend();
	}                                               
}

int CReaderThread::ContainerToChannel(TContainer ContainerNum)
{
	switch(ContainerNum)
	{
		case MODEL_1:   return 2;
		case MODEL_2:   return 3;
		case SUPPORT_1: return 1;
		case SUPPORT_2: return 4;
	}
	return -1;
}

void CReaderThread::SetTestData(TTestData &TestData)
{
	m_TestData = TestData;
}

