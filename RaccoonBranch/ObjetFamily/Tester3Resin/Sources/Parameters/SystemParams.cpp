/********************************************************************
 *                Quadra 1Application parameters set                *
 *                ----------------------------------                *
 * Module description: This module define the set of parameters     *
 *                     required for the application.                *
 *                                                                  *
 * Compilation: Standard C++ / BCB.                                 *
 *                                                                  *
 * Author: .                                               *
 * Start date: 21/8/2000                                            *
 * Last upate: 21/08/2000                                           *
 ********************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "SystemParams.h"


// Constructor - Initialize all the app parameters using the parameters manager
//               predefined macros (e.g. INIT_PARAM).
CSystemParams::CSystemParams(const QString& IniFileName) : CQParamsContainer(NULL,"")
{
  INIT_PARAM(Date,"","General");
  INIT_PARAM(Time,"","General");
  INIT_PARAM(OperatorName,"","General");
  INIT_PARAM(System_SN,"","General");
  INIT_PARAM(Containers_SN,"","General");
  INIT_PARAM(LCD_SN,"","General");
  INIT_PARAM(MCB_SN,"","General");
  INIT_PARAM(OCB_SN,"","General");
  INIT_PARAM(OHDB_SN,"","General");

  INIT_PARAM(EmbeddedComNum,0,"Communication");
  INIT_PARAM(EmbeddedConnectionKind,NONE,"Communication");
  EmbeddedConnectionKind.SetDescription(" 0 - not connected, 1 - Direct connection");
  INIT_PARAM(ContainersComNum,0,"Communication");
  INIT_PARAM(ContainersConnectionKind,NONE,"Communication");
  ContainersConnectionKind.SetDescription(" 0 - Not connected, 1 - Direct connection, 2 - Through embedded");
  INIT_PARAM(LCDComNum,0,"Communication");
  INIT_PARAM(LCDConnectionKind,NONE,"Communication");
  LCDConnectionKind.SetDescription(" 0 - Not connected, 1 - Direct connection, 2 - Through embedded");
  INIT_PARAM(MCBComNum,0,"Communication");
  INIT_PARAM(MCBConnectionKind,NONE,"Communication");
  MCBConnectionKind.SetDescription(" 0 - Not connected, 1 - Direct connection, 2 - Through embedded");
  INIT_PARAM(OCBComNum,0,"Communication");
  INIT_PARAM(OCBConnectionKind,NONE,"Communication");
  OCBConnectionKind.SetDescription(" 0 - Not connected, 1 - Direct connection, 2 - Through embedded");
  INIT_PARAM(OHDBComNum,0,"Communication");
  INIT_PARAM(OHDBConnectionKind,NONE,"Communication");
  OHDBConnectionKind.SetDescription(" 0 - Not connected, 1 - Direct connection, 2 - Through embedded, 3 - Through OCB");
  INIT_PARAM(OCBSimComNum,0,"Communication");
  INIT_PARAM(OCBSimConnectionKind,NONE,"Communication");
  OCBSimConnectionKind.SetDescription(" 0 - Not connected, 1 - Direct connection");

  // Create parameters stream for the parameters file
  m_ParamsStream = new CQParamsFileStream(IniFileName);

  // Assign the stream to the container
  AssignParamsStream(m_ParamsStream);

}

         

CSystemParams::~CSystemParams(void)
{
  SaveAll();
	Q_SAFE_DELETE(m_ParamsStream);
}


void CSystemParams::AssignToINIFile(QString FileName)
{
  Q_SAFE_DELETE(m_ParamsStream);

  // Create parameters stream for the parameters file
  m_ParamsStream = new CQParamsFileStream(FileName);

  // Assign the stream to the container
  AssignParamsStream(m_ParamsStream);

}


// End of "SystemParams.cpp"

