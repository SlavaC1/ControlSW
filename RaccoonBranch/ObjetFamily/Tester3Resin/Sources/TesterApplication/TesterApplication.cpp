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

#include "TesterApplication.h"
#include "QStdComPort.h"
#include "EdenProtocolClientIDs.h"
#include "OHDBProtocolClient.h"
#include "OCBProtocolClient.h"

// Pointer to the singleton instance (static)
CTesterApplication *CTesterApplication::m_TesterAppInstance = NULL;

// Constructor
CTesterApplication::CTesterApplication(void)
{
}

// Destructor
CTesterApplication::~CTesterApplication(void)
{
  //m_AppParams->DeInit();
  COHDBProtocolClient::DeInit();
  COCBProtocolClient::DeInit();
  for (unsigned i = 0; i < m_ComPortEnginesList.size(); i++)
  {
    m_ComPortEnginesList[i].ProtocolEngine->Stop();
		Q_SAFE_DELETE(m_ComPortEnginesList[i].ProtocolEngine);
  }

}


// Application init function (override)
bool CTesterApplication::AppInit(void)
{
  m_AppInifileName = (QString)AppFilePath + (QString)AppFileName + ".ini";
  m_AppParams->Init(m_AppInifileName.c_str());
  return true;
}

// Application start function (override)
void CTesterApplication::AppStart(void)
{
}

// Factory functions
void CTesterApplication::CreateInstance()
{
   // Avoid allocating an instance if already allocated
  if(!m_TesterAppInstance)
  {
    m_TesterAppInstance = new CTesterApplication();
    m_TesterAppInstance->Init();
  }
}

void CTesterApplication::DeleteInstance()
{
  // Deallocate instance
  if(m_TesterAppInstance)
  {
    Q_SAFE_DELETE(m_TesterAppInstance);
    m_TesterAppInstance = NULL;
  }
}

// Get an instance pointer
CTesterApplication* CTesterApplication::GetInstance()
{
  return m_TesterAppInstance;
}


// Open a com port, create a protocol engine for it, and update the com ports list
void CTesterApplication::InitComPort(int ComNum)
{
  // if this com port is already opened, return
  for (unsigned i = 0; i < m_ComPortEnginesList.size(); i++)
  {
    if (m_ComPortEnginesList[i].ComNum == ComNum)
      return;
  }

  TComPortEngine ComPortEngine;
  ComPortEngine.ComNum = ComNum;
  CQStdComPort *ComPort = new CQStdComPort(ComNum,OCB_COM_BAUD_RATE);
  ComPortEngine.ProtocolEngine = new CEdenProtocolEngine(ComPort);
  m_ComPortEnginesList.push_back(ComPortEngine);
  ComPortEngine.ProtocolEngine->Start();
}

// get a protocol engine that is assoicated with the specified com port
CEdenProtocolEngine* CTesterApplication::GetProtocolEngine(int ComNum)
{
  for (unsigned i = 0; i < m_ComPortEnginesList.size(); i++)
  {
    if (m_ComPortEnginesList[i].ComNum == ComNum)
    {
      return m_ComPortEnginesList[i].ProtocolEngine;
    }
  }
  return NULL;
}

