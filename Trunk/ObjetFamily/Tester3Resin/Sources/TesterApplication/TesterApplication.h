/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Tester                                                  *
 * Module: Tester Application class.                                *
 * Module Description: This class inherits from the CQApplication   *
 *                     class and implement the specific Tester      *
 *                     initializations.                             *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author:                                                          *
 * Start date: 22/08/2001                                           *
 * Last upate: 22/08/2001                                           *
 ********************************************************************/

#ifndef _TESTER_APPLICATION_H_
#define _TESTER_APPLICATION_H_

#include "QApplication.h"
#include "SystemParams.h"
#include "AppParams.h"
#include "EdenProtocolEngine.h"
#include "AppLogFile.h"
#include <vector>

struct TLogFile
{
	QString FileName;
	time_t FileDateTime;
};

typedef std::vector<TLogFile> TLogFileList;

bool operator < (const TLogFile& A,const TLogFile& B)
{
  return (A.FileDateTime < B.FileDateTime);
}


typedef struct {
  int ComNum;
  CEdenProtocolEngine *ProtocolEngine;

} TComPortEngine;

typedef std::vector<TComPortEngine> TComPortEnginesList;


// Exception class for the application
class ETesterApplication : public EQException {
  public:
    ETesterApplication(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};



class CTesterApplication : public CQApplication {
  private:

    // Application init function (override)
    bool AppInit(void);

    // Application start function (override)
    void AppStart(void);

    // private Constructor
    CTesterApplication(void);

    //  private Destructor
    ~CTesterApplication(void);

    // Pointer to the singleton instance
    static CTesterApplication *m_TesterAppInstance;


    // pointer to the application parameters manager instance
    CAppParams* m_AppParams;

    // the application Ini file name
    QString m_AppInifileName;

    // list of the opened com ports and their protocol engines
	TComPortEnginesList m_ComPortEnginesList;

	void InitLogFile();
	QString GetLastLogFileName();

  public:

    // Factory functions
    static void CreateInstance();

    static void DeleteInstance();

    // Get an instance pointer
    static CTesterApplication *GetInstance();

    // Open a com port, create a protocol engine for it, and update the com ports list
    void InitComPort(int ComNum);

    // get a protocol engine that is assoicated with the specified com port
    CEdenProtocolEngine* GetProtocolEngine(int ComNum);


};

#endif
