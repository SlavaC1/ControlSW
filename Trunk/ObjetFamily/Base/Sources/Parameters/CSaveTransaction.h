//---------------------------------------------------------------------------

#ifndef CSaveTransactionH
#define CSaveTransactionH

#include "Q2RTApplication.h"

#include <stdlib.h>
#include <stdio.h>

class CSaveTransaction
{
    enum State
    {
        // some arbitrary bit patterns
        stDataInA = 0xC6,
        stDataInB = 0x3A
    };
public:
	CSaveTransaction();
	~CSaveTransaction();
	void Commit();

	FILE * GetDataFile () { return m_dataFile; }
	FILE * GetBackupFile () { return m_backupFile; }
	void CreateFiles();
	void LoadFiles();

private:
	bool m_commit;
	FILE * m_swFile;
	FILE * m_dataFile;
	FILE * m_backupFile;
	State m_state;

	QString m_stSwFilePath;
	QString m_stAFilePath;
	QString m_stBFilePath;
};

//---------------------------------------------------------------------------
#endif
