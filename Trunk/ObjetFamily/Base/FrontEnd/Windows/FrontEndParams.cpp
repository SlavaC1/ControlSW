/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Front-End parameters.                                    *
 * Module Description: This class implements a parameters container *
 *                     for the front end.                           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 06/02/2001                                           *
 * Last upate: 13/03/2003                                           *
 ********************************************************************/

#include "FrontEndParams.h"

// Pointer to the singleton instance (static)
CFrontEndParams *CFrontEndParams::m_AppParamsInstance = NULL;


// Constructor - Initialize all the app parameters using the parameters manager
//               predefined macros (e.g. INIT_PARAM).
CFrontEndParams::CFrontEndParams(const QString& CfgFileName) : CQParamsContainer(NULL,"FrontEndParams")
{
  INIT_PARAM_ARRAY_SAME_VALUE(LastFileNamesArray1,"?","Last settings",RD_LEVEL);
  INIT_PARAM_ARRAY_SAME_VALUE(LastFileNamesArray2,"?","Last settings",RD_LEVEL);
  INIT_PARAM(LastPFFDirectory,"?","Last settings",RD_LEVEL);
  INIT_PARAM(LastOperationMode,0,"Last settings",RD_LEVEL);
  INIT_PARAM(LastQualityMode,0,"Last settings",RD_LEVEL);
  INIT_PARAM(LastNumberOfSlices1,10,"Last settings",RD_LEVEL);
  INIT_PARAM(LastNumberOfSlices2,10,"Last settings",RD_LEVEL);

  INIT_PARAM(DevicesDlgLeftPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(DevicesDlgTopPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(DevicesDlgAlwaysOnTop,true,"Dialogs",RD_LEVEL);

  INIT_PARAM(HeadsDlgLeftPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(HeadsDlgTopPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(HeadsDlgAlwaysOnTop,true,"Dialogs",RD_LEVEL);

  INIT_PARAM(MotorsDlgLeftPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(MotorsDlgTopPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(MotorsDlgAlwaysOnTop,true,"Dialogs",RD_LEVEL);

  INIT_PARAM(ActuatorsDlgLeftPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(ActuatorsDlgTopPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(ActuatorsDlgAlwaysOnTop,true,"Dialogs",RD_LEVEL);

  INIT_PARAM(BITDlgLeftPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(BITDlgTopPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(BITDlgAlwaysOnTop,true,"Dialogs",RD_LEVEL);

  INIT_PARAM(OcbOhdbDlgLeftPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(OcbOhdbDlgTopPosition,-1,"Dialogs",RD_LEVEL);
  INIT_PARAM(OcbOhdbDlgAlwaysOnTop,true,"Dialogs",RD_LEVEL);


  INIT_PARAM_ARRAY_SAME_VALUE(ImageViewFormLeftArray,40,"Dialogs",RD_LEVEL);
  INIT_PARAM_ARRAY_SAME_VALUE(ImageViewFormTopArray,310,"Dialogs",RD_LEVEL);

  // Calibration Dlg Params
  INIT_PARAM_ARRAY_SAME_VALUE(FullA2DSetupWeightArray,0,"Calibration Dlg",RD_LEVEL);
  FullA2DSetupWeightArray.SetAttributes (paHidden | paBlockDefaults);
  INIT_PARAM_ARRAY_SAME_VALUE(EmptyA2DSetupWeightArray,0,"Calibration Dlg",RD_LEVEL);
  EmptyA2DSetupWeightArray.SetAttributes(paHidden | paBlockDefaults);
  INIT_PARAM_ARRAY_SAME_VALUE(FullSetupWeightArray,0,"Calibration Dlg",RD_LEVEL);
  FullSetupWeightArray.SetAttributes    (paHidden | paBlockDefaults);


#ifdef REMOTE_FRONT_END
  INIT_PARAM(InterafceComNum,2,"Communication",RD_LEVEL);
  INIT_PARAM(RemoteInterfaceBaudRate,38400,"Communication",RD_LEVEL);
  INIT_PARAM(RemoteMachineTcpIpAddress,"","Communication",RD_LEVEL);
  INIT_PARAM(TcpIpConnection,false,"Communication",RD_LEVEL);
#endif

  // Create parameters stream for the parameters file
  m_AssignParamsStream = new CQParamsFileStream(CfgFileName);

  // Assign the stream to the container
  AssignParamsStream(m_AssignParamsStream);
}

// Destructor
CFrontEndParams::~CFrontEndParams(void)
{
  delete m_AssignParamsStream;
}

// Factory functions
void CFrontEndParams::Init(const QString& CfgFileName)
{
  // Avoid allocating an instance if already allocated
  if(!m_AppParamsInstance)
  {
    m_AppParamsInstance = new CFrontEndParams(CfgFileName);
    m_AppParamsInstance->LoadAll();
  }
}

void CFrontEndParams::DeInit(void)
{
  // Deallocate instance
  if(m_AppParamsInstance)
  {
    m_AppParamsInstance->SaveAll();

    delete m_AppParamsInstance;
    m_AppParamsInstance = NULL;
  }
}

// Get a pointer to the singleton instance
CFrontEndParams *CFrontEndParams::Instance(void)
{
  return m_AppParamsInstance;
}


