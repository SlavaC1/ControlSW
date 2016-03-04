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
 * Last upate: 06/02/2001                                           *
 ********************************************************************/

#ifndef _FRONT_END_PARAMS_H_
#define _FRONT_END_PARAMS_H_

#include "QParamsContainer.h"
#include "QFileParamsStream.h"
#include "GlobalDefs.h"

// This class is implemented as a singleton with Init/DeInit factory static functions
class CFrontEndParams : public CQParamsContainer {
  private:
    // Pointer to the singleton instance
    static CFrontEndParams *m_AppParamsInstance;

    // File parameters stream associated with the application parameters container
    CQParamsFileStream *m_AssignParamsStream;

    // Private constructor prevents un-authorized initialization
    CFrontEndParams(const QString& CfgFileName);

  public:
    // Destructor
    ~CFrontEndParams(void);

    // Factory functions
    static void Init(const QString& CfgFileName);
    static void DeInit(void);

    // Get a pointer to the singleton instance
    static CFrontEndParams *Instance(void);

    // Parameters declarations starts here
	// -----------------------------------
    DEFINE_PARAM(int,LastOperationMode);
    DEFINE_PARAM(int,LastQualityMode);
    DEFINE_PARAM(int,LastNumberOfSlices1);
    DEFINE_PARAM(int,LastNumberOfSlices2);
    DEFINE_PARAM(int,DevicesDlgLeftPosition);
    DEFINE_PARAM(int,DevicesDlgTopPosition);
    DEFINE_PARAM(bool,DevicesDlgAlwaysOnTop);
    DEFINE_PARAM(int,HeadsDlgLeftPosition);
    DEFINE_PARAM(int,HeadsDlgTopPosition);
    DEFINE_PARAM(bool,HeadsDlgAlwaysOnTop);
    DEFINE_PARAM(int,MotorsDlgLeftPosition);
    DEFINE_PARAM(int,MotorsDlgTopPosition);
    DEFINE_PARAM(bool,MotorsDlgAlwaysOnTop);
	DEFINE_PARAM(int,ActuatorsDlgLeftPosition);
	DEFINE_PARAM(int,ActuatorsDlgTopPosition);
	DEFINE_PARAM(bool,ActuatorsDlgAlwaysOnTop);
	DEFINE_PARAM(int,OcbOhdbDlgLeftPosition);
	DEFINE_PARAM(int,OcbOhdbDlgTopPosition);
	DEFINE_PARAM(bool,OcbOhdbDlgAlwaysOnTop);
    DEFINE_PARAM(int,BITDlgLeftPosition);
    DEFINE_PARAM(int,BITDlgTopPosition);
    DEFINE_PARAM(bool,BITDlgAlwaysOnTop);
    DEFINE_ARRAY_PARAM(int,NUMBER_OF_CHAMBERS,ImageViewFormLeftArray);
    DEFINE_ARRAY_PARAM(int,NUMBER_OF_CHAMBERS,ImageViewFormTopArray);

    // Calibration Dlg Params
    DEFINE_ARRAY_PARAM(float, TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE, FullA2DSetupWeightArray);
    DEFINE_ARRAY_PARAM(float, TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE, EmptyA2DSetupWeightArray);
    DEFINE_ARRAY_PARAM(float, TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE, FullSetupWeightArray);

    DEFINE_ARRAY_PARAM(QString, NUMBER_OF_CHAMBERS, LastFileNamesArray1);
    DEFINE_ARRAY_PARAM(QString, NUMBER_OF_CHAMBERS, LastFileNamesArray2);

#ifdef REMOTE_FRONT_END
    DEFINE_PARAM(int,InterafceComNum);
    DEFINE_PARAM(int,RemoteInterfaceBaudRate);
    DEFINE_PARAM(bool,TcpIpConnection);
    DEFINE_PARAM(QString,RemoteMachineTcpIpAddress);
#endif
};

#endif

