//---------------------------------------------------------------------------
#ifndef HistoryFileH
#define HistoryFileH

#include "QComponent.h"
#include "QUtils.h"
#include "QStringList.h"

class CHistoryFile : public CQComponent 
{
private:

	QString       m_FilePath;
	QString       m_Title;
	CQStringList *m_StringList;

public:

	CHistoryFile(const QString FileName);
	~CHistoryFile();

	void Append(const QString Text);
	void WriteMachineName();
	void WriteLine();
	void WriteDate();
	void SkipLines(int LinesToSkip);

};

//---------------------------------------------------------------------------
#endif
