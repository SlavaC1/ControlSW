#ifndef _TEMP_APP_PARAMS_H_
#define _TEMP_APP_PARAMS_H_

#include "AppParams.h"
#include "MiniSequencer.h"
#include "ModesManager.h"
#include <Classes.hpp> //bug 5719

class CTempAppParams : public CAppParams
{

	class CTempMiniSequencer : public CMiniSequencer
	{
	public:
		CTempMiniSequencer(const QString& Name);
		virtual QString EvaluateExpression(const QString Input);
                virtual void ProcessScript(TStrings * StrList); //bug 5719

	private:
		QString m_TempName;
	};
							   
public:
   CTempAppParams(QString Name, TInitStatus_E &StatusInit, bool DoRegister = true);
   ~CTempAppParams();
   TQErrCode EnterMode(const QString ModeName, QString Dir);
   void Init();

private:
   CTempMiniSequencer* m_TempMiniSequencer;
   CModesManager*      m_TempModesManager;
};

#endif /*_TEMP_APP_PARAMS_H_*/
