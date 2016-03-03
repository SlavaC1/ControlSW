/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT back-end interface class.                           *
 *                                                                  *
 * Compilation: C++                                                 *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 07/02/2002                                           *
 * Last upate: 10/07/2003                                           *
 ********************************************************************/

#include "Actuator.h"
#include "AmbientTemperature.h"
#include "BackEndInterface.h"
#include "BITManager.h"
#include "Container.h"
#include "Door.h"
#include "EdenPCI.h"
#include "EdenPCISys.h"
#include "HeadFilling.h"
#include "HeadHeaters.h"
#include "HeadStatus.h"
#include "HeadVacuum.h"
#include "HostComm.h"
#include "MachineManager.h"
#include "MachineSequencer.h"
#include "MaintenanceCounters.h"
#include "ModesManager.h"
#include "Motor.h"
#include "OCBStatus.h"
#include "Power.h"
#include "PrintControl.h"
#include "Purge.h"
#include "Q2RTApplication.h"
#include "QVersionInfo.h"
#include "Roller.h"
#include "Tester.h"
#include "TrayHeater.h"
#include "UvLamps.h"
#include "rfid.h"
#include "Hasp.h"
#include "CBatchNumberTable.h"
#include "CActionsHistoryTable.h"

#ifdef MOTOR_MCB
#include "MotorMcb.h"
#endif
#include "LicenseManager.h"
#include "SignalTower.h"

// Pointer to the singleton object
CBackEndInterface *CBackEndInterface::m_InstancePtr = 0;


// Constructor
CBackEndInterface::CBackEndInterface(void) : CQComponent("BackEndInterface")
{
  INIT_METHOD(CBackEndInterface,MoveZAxisDownToLowestPosition);
  INIT_METHOD(CBackEndInterface,MoveZAxisDown);
  INIT_METHOD(CBackEndInterface,MoveZAxisDownWiperWizard);
  INIT_METHOD(CBackEndInterface,WaitForEndOfMovement);
  INIT_METHOD(CBackEndInterface,AllowServiceMaterials);
  INIT_METHOD(CBackEndInterface,IsServiceModelMaterial);
  INIT_METHOD(CBackEndInterface,IsServiceSupportMaterial);
  INIT_METHOD(CBackEndInterface,SkipMachineResinTypeChecking);
  INIT_METHOD(CBackEndInterface,UpdateTanks); //bug 6258
//OBJET_MACHINE feature
  INIT_METHOD(CBackEndInterface,SetMachineActionsDate);
  INIT_METHOD(CBackEndInterface,UpdateWeightAfterWizard);

  INIT_METHOD(CBackEndInterface,ActivateWaste);
  INIT_METHOD(CBackEndInterface,ActivateRollerAndPurgeWaste);
  INIT_METHOD(CBackEndInterface,ActivateSignalTower);
  INIT_METHOD(CBackEndInterface,EnableConfirmationBypass);
  INIT_METHOD(CBackEndInterface,DisableConfirmationBypass);
}

// Init/DeInit the singleton object
void CBackEndInterface::Init(void)
{
  if(m_InstancePtr == 0)
    m_InstancePtr = new CBackEndInterface;
}

void CBackEndInterface::DeInit(void)
{
  if(m_InstancePtr != 0)
  {
    delete m_InstancePtr;
    m_InstancePtr = 0;
  }
}

bool CBackEndInterface::EnableErrorMessageDuringStop(bool Enable)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
	return Mgr->EnableErrorMessageDuringStop(Enable);
  return false;
}



void CBackEndInterface::IgnorePowerByPass(bool Ignore)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    Mgr->IgnorePowerByPass(Ignore);
}

void CBackEndInterface::MarkWizardRunning(bool IsRunning)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    Mgr->MarkWizardRunning(IsRunning);
}

void CBackEndInterface::MarkPythonWizardRunning(bool IsRunning)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    Mgr->MarkPythonWizardRunning(IsRunning);
}

void CBackEndInterface::MarkPrintingWizardRunning(bool IsRunning)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    Mgr->MarkPrintingWizardRunning(IsRunning);
}

bool CBackEndInterface::IsWizardRunning(void)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    return Mgr->IsWizardRunning();

  return false;
}

bool CBackEndInterface::IsPythonWizardRunning(void)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    return Mgr->IsPythonWizardRunning();

  return false;
}

bool CBackEndInterface::IsPrintingWizardRunning(void)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
    return Mgr->IsPrintingWizardRunning();

  return false;
}

void CBackEndInterface::PFFPrintJob(char* FolderPath)
{
	CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
	Mgr->ExecutePFFPrintJob(FolderPath);
}

// Load a local file print job
void CBackEndInterface::LoadLocalPrintJob(const TFileNamesArray FileNames,
                                          int                   BitmapResolution,
                                          int                   QualityMode,
                                          int                   OperationMode,
                                          int                   SliceNum)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  Mgr->LoadLocalPrintJob(FileNames,BitmapResolution,QualityMode,OperationMode,SliceNum);
}

void CBackEndInterface::LoadLocalPrintJob(const TFileNamesArray FileNames1,
                                          const TFileNamesArray FileNames2,
                                          int                   BitmapResolution,
                                          int                   QualityMode,
                                          int                   OperationMode,
                                          int                   SliceNum1,
                                          int                   SliceNum2)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  Mgr->LoadLocalDoublePrintJob(FileNames1,FileNames2,BitmapResolution,QualityMode,OperationMode,SliceNum1,SliceNum2);
}

void CBackEndInterface::PausePrint(void)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  Mgr->PausePrint();
}

void CBackEndInterface::StopPrint(TQErrCode ErrCode)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  Mgr->StopPrint(ErrCode);
}

void CBackEndInterface::SetOnlineOffline(bool Online)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  Mgr->SetOnlineOffline(Online);
}

// Read/Write PCI register
ULONG CBackEndInterface::PCIReadDWORD(int BarNum,unsigned long Addr)
{
  return EdenPCI_ReadDWORD(BarNum,Addr);
}

void CBackEndInterface::PCIWriteDWORD(int BarNum,unsigned long Addr,unsigned long Data)
{
  EdenPCI_WriteDWORD(BarNum,Addr,Data);
}

void CBackEndInterface::FIFOSetReadMode(int ReadMode)
{
  EdenPCI_FIFOSetReadMode(ReadMode);
}

void CBackEndInterface::WriteDataToOHDBXilinx(BYTE Address,WORD Data)
{
  Q2RTApplication->GetTester()->WriteDataToOHDBXilinx(Address,Data);
}

//only for simulator using ext. memory
void CBackEndInterface::WriteDataToLotus(WORD Address,BYTE Data)
{
  Q2RTApplication->GetTester()->WriteDataToLotus(Address,Data);
}
BYTE CBackEndInterface::ReadDataFromLotus(WORD Address)
{
  return Q2RTApplication->GetTester()->ReadDataFromLotus(Address);
}
WORD CBackEndInterface::ReadDataFromOHDBXilinx(BYTE Address)
{
  return Q2RTApplication->GetTester()->ReadDataFromOHDBXilinx(Address);
}

// Goto Z axis tray out position - sequencer
void CBackEndInterface::GotoTrayRemovalPosition(void)
{
  Q2RTApplication->GetMachineManager()->GotoTrayRemovalPosition();
}

void CBackEndInterface::GotoTrayLowerPosition(void)
{
  Q2RTApplication->GetMachineManager()->GotoTrayLowerPosition();
}

// Goto head inspection position
void CBackEndInterface::GotoHeadInspectionPosition(void)
{
  Q2RTApplication->GetMachineManager()->GotoHeadInspectionPosition();
}


// Goto head inspection position
TQErrCode CBackEndInterface::GotoHeadInspectionPositionUnderWiperCalibration(void)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

   return ( Motor->GoWaitAbsolutePositionSequence(AXIS_Z,CONFIG_ConvertZmmToStep(CAppParams::Instance()->ZCleanHeadsPosition),MORE_TIME_TO_FINISH_MOVEMENT)) ;
}

TQErrCode CBackEndInterface::GotoPurgePositionEX(bool Blocking, T_AxesTable AxesTable )
{
  TQErrCode Err = Q_NO_ERROR;

  if (Blocking)
    Err = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GoToPurgeEx(AxesTable,true,false);
  else
    Err = Q2RTApplication->GetMachineManager()->GoToPurgePosition();
  return Err;
}

// Goto purge positon
TQErrCode CBackEndInterface::GotoPurgePosition(bool Blocking, bool PerformHome, bool AvoidContactWithBasket)
{
  TQErrCode Err = Q_NO_ERROR;
  if (Blocking)
	Err = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GoToPurgeEx(PerformHome,true,AvoidContactWithBasket);
  else
    Err = Q2RTApplication->GetMachineManager()->GoToPurgePosition();
  return Err;
}
// Do wipe
void CBackEndInterface::DoWipe(void)
{
  Q2RTApplication->GetMachineManager()->Wipe();
}

// Do fire all
void CBackEndInterface::DoFireAll(void)
{
  Q2RTApplication->GetMachineManager()->DoFireAll();
}

//Do test pattern
void CBackEndInterface::DoTestPattern(bool Blocking, bool PrintInCurrentZ, bool MoveAxisZInit)
{

  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayPlacerInstance();

  if( Tray->VerifyTrayInserted() != Q_NO_ERROR ) {
	return;
  }

  if (!Blocking)
    Q2RTApplication->GetMachineManager()->TestPattern(PrintInCurrentZ,MoveAxisZInit);
  else
    Q2RTApplication->GetMachineManager()->GetMachineSequencer()->TestPattern(PrintInCurrentZ,MoveAxisZInit);
}

//Do purge sequence
void CBackEndInterface::DoPurgeSequence(bool PerformSinglePurgeInSequence)
{
   Q2RTApplication->GetMachineManager()->Purge( PerformSinglePurgeInSequence );
}

//Do purge sequence
void CBackEndInterface::DoFireAllSequence(void)
{
   Q2RTApplication->GetMachineManager()->FireAll();
}

// Entry to maintenance state
void CBackEndInterface::EnterStandbyState(void)
{
  Q2RTApplication->GetMachineManager()->EnterStandbyState();
}

// Exit to maintenance state
void CBackEndInterface::ExitStandbyState(void)
{
  Q2RTApplication->GetMachineManager()->ExitStandbyState();
}

// Turn the power (of the peripheral units) on / off
void CBackEndInterface::TurnPower(bool On)
{
  CPowerBase *Power = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPowerInstance();

  Power->SetPowerOnOff(On);
}


// Wait procedure to Power On or Off notification
TQErrCode CBackEndInterface::WaitForPowerOnOff()
{
  CPowerBase *Power = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPowerInstance();

  return Power->WaitForPowerOnOff();
}


// Wait for the FPGA of the OHDB to load
TQErrCode CBackEndInterface::WaitForOhdbHwDrvToLoad()
{
    CHeadStatusSender * pOHDB = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadStatusInstance();

    if (!pOHDB)
    {
        return Q2RT_FATAL_ERROR;
    }

#ifndef _DEBUG
    TQErrCode rc = Q2RT_FATAL_ERROR;
    int       i  = 0;

    while (++i < 200)
    {
        pOHDB->GetOHDBHWVersionMessage();
        if (0 != pOHDB->GetOHDBHWVersion())
        {
            rc = Q_NO_ERROR;
            break;
        }

        Sleep(100);
    }

   return rc;
#else
   return Q_NO_ERROR;
#endif
}


// Reset the FPGA of the OHDB
TQErrCode CBackEndInterface::ResetOhdbHwDrv()
{
    CPrintControl * pPrnCtrl = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPrintControlInstance();

    if (!pPrnCtrl)
    {
        return Q2RT_FATAL_ERROR;
    }

    pPrnCtrl->ResetDriverCircuit();
    return Q_NO_ERROR;
}


// Lock /Unlock the door
void CBackEndInterface::LockDoor(bool Lock)
{
  EnableDoor(Lock);
}

// Enable / Disable door
TQErrCode CBackEndInterface::EnableDoor(bool Enable,bool showDialog)
{
  CDoorBase *Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();
  bool ret = Q_NO_ERROR;
  if (Enable)
    ret = Door->Enable();
  else
  {
    ret = Door->Disable();
    EnableMotor(false,AXIS_ALL);
  }
  return ret;
}

// return num of doors
BYTE CBackEndInterface::GetNumOfDoor()
{
  CDoorBase *m_Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();
  return m_Door->GetNumOfDoors();
}

// Cancel wait operation for door lock
void CBackEndInterface::CancelDoorLockWait(void)
{
  CDoorBase *Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();
  Door->CancelLockWait();
}

// Check if door is closed
TQErrCode CBackEndInterface::CheckIfDoorIsClosed(void)
{
  CDoorBase *Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();
  return Door->CheckIfDoorIsClosed();
}


QString CBackEndInterface::GetDoorMessage(void)
{
  CDoorBase *Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();
  return Door->GetDoorMessage();
}

QString CBackEndInterface::GetDoorStatusMessage(void)
{
	CDoorBase *Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();
	return Door->GetDoorStatusMessage();
}
// Reset the SW of the MCB
TQErrCode CBackEndInterface::MCBReset()
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  return Motors->SWReset();
}

TQErrCode CBackEndInterface::MCBSendRomFileLine(QString Line)
{
	CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

	return Motors->SendRomFileLine(Line);
}

// Get the SW and HW versions of the MCB
void CBackEndInterface::GetGeneralInformation()
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motors->GetGeneralInformation();
}

// Turn the tray heater on/off
void CBackEndInterface::TurnTrayHeater(bool On, int Temperature, bool ADu)
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayHeaterInstance();

  if (On)
    Tray->SetUserTrayTemperature(Temperature);

  Tray->SetTrayOnOff(On);
}

// Turn the ambient temperature monitoring on/off
void CBackEndInterface::TurnAmbientTempMonitor(bool On, int Temperature, bool ADu)
{
  CAmbientTemperatureBase *AmbientTemp = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetAmbientTemperatureInstance();

  if (On)
    AmbientTemp->SetUserTemperature(Temperature);

  AmbientTemp->TurnAmbientTemperature(On);
}

// Turn the roller on/off (and set the requested speed)
void CBackEndInterface::TurnRoller(bool On, int Speed)
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();
  CAppParams* ParamsMgr = CAppParams::Instance();

  if (On)
    Roller->SetRollerParms(Speed);

  Roller->SetRollerOnOff(On);
  //RSS, itamar 
  if(ParamsMgr->IsSuctionSystemExist)
        Actuators->ActivateRollerSuctionSystem(ParamsMgr->RollerSuctionValveOnTime,
                                                ParamsMgr->RollerSuctionValveOffTime,On);
}

// Turn the UV lamps on/off
void CBackEndInterface::TurnUVLamps(bool On)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  UVLamps->TurnOnOff(On);
}
void CBackEndInterface::TurnSpecifiedUVLamps(bool On,BYTE UvLampsMask)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  UVLamps->TurnSpecifiedOnOff(On,UvLampsMask);
}

// Command to set D/A value to the Uv lamp
void CBackEndInterface::SetUVD2AValue(BYTE LampID, USHORT D2A_Value)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();

  UVLamps->SetUVD2AValue(LampID, D2A_Value);
}

// Command to get the UV Sensor reading
int CBackEndInterface::GetUVSensorValue(bool InternalSensor)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  return UVLamps->GetUVSensorValue(InternalSensor);
}

// Command to get the UV Sensor max reading
int CBackEndInterface::GetUVSensorMaxValue(bool InternalSensor)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  return UVLamps->GetUVSensorMaxValue(InternalSensor);
}

// Command to get the UV Sensor max reading
int CBackEndInterface::GetUVSensorMaxDelta(bool InternalSensor)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  return UVLamps->GetUVSensorMaxDelta(InternalSensor);
}

// Command to get the UV Sensor sum reading
int CBackEndInterface::GetUVSensorSum(bool InternalSensor)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  return UVLamps->GetUVSensorSum(InternalSensor);
}

// Command to get the UV Sensor number of readings
int CBackEndInterface::GetUVNumOfReadings()
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();
  return UVLamps->GetUVNumOfReadings();
}

// Command to force the UV Sensor reading from OCB
TQErrCode CBackEndInterface::GetUVValue()
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();

  return UVLamps->GetUVValue();
}

// Command to get the UV Sensor reading
TQErrCode CBackEndInterface::SetUVSamplingParams(bool RestartSampling)
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();

  return UVLamps->SetUVSamplingParams(RestartSampling);
}

// Get the current tray temperature parameter
int CBackEndInterface::GetCurrentTrayTemperatureParameter()
{
  return CAppParams::Instance()->StartTrayTemperature;
}

// Get the current roller speed parameter
int CBackEndInterface::GetCurrentRollerSpeedParameter()
{
  return CAppParams::Instance()->RollerVelocity;
}

// Turn the head filling ("monitor") on/off
void CBackEndInterface::TurnHeadFilling(bool On)
{
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();

  if (On)
	HeadFilling->SetDefaultParms();

  HeadFilling->HeadFillingOnOff(On);
}

void CBackEndInterface::SetDefaultParamsToOCB()
{
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
  HeadFilling->SetDefaultParms();
}
                         
// Set the 'Report Temperature Error'
void CBackEndInterface::SetReportTemperatureError(bool Report)
{
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();

  HeadFilling->SetReportTemperatureError(Report);
}

// Purge
void CBackEndInterface::Purge(bool PerformHome /* true by default */, bool GotoPurgePos /* = true by default */)
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  // Go to purge position
  if(GotoPurgePos)
    Sequencer->GoToPurge(PerformHome,true);

  Sequencer->GetPurgeInstance()->Purge(0);
}

// Turn the head heaters on/off (and set the requested temperatures in A/D units)
void CBackEndInterface::TurnHeadHeaters(bool On, int* Temperature, int NumOfHeads, const bool& bKeepHeatingEvenOpenOrShortCircuit)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
  CFansBase*		Fans 		= Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetFansInstance();

  if (On)
  {
    Fans->ResetMiddleFanWarning();

    for (int i = 0; i < NumOfHeads; i++)
    {
      HeadHeaters->SetHeaterTemperature(i, Temperature[i]);
    }

	HeadHeaters->SetHeatersTemperature(bKeepHeatingEvenOpenOrShortCircuit);
  }

  HeadHeaters->SetDefaultOnOff(On);
}
void CBackEndInterface::HeadHeatersSetIgnoreError(const bool& bIgnoreError)
{
      CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
      HeadHeaters->SetIgnoreError(bIgnoreError);
}

bool CBackEndInterface::HeadHeatersIgnoreError()
{
     CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
     return HeadHeaters->Get_Ignore_Error();
}
// Get the current head heaters temperature parameters
void CBackEndInterface::GetCurrentHeadHeatersTempParameters(int *Params, bool IncludePreHeater)
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  for (int i = 0 ; i < TOTAL_NUMBER_OF_HEATERS; i++)
    Params[i] = ParamsMgr->HeatersTemperatureArray[i];
//OBJET_MACHINE, PreHeater
  /*if (IncludePreHeater == true)
	Params[PRE_HEATER] = ParamsMgr->HeatersTemperatureArray[PRE_HEATER];*/
}

// Wipe the heads
void CBackEndInterface::Wipe()
{
  Q2RTApplication->GetMachineManager()->Wipe();
}

// Enable/disable one of the motors
void CBackEndInterface::EnableMotor(bool Enable,TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->SetMotorEnableDisable(Axis, Enable);
}

// Disable T axis
void CBackEndInterface::DisableTAxisIfMachineIsInactive(void)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->DisableTAxisIfMachineIsInactive();
}

// Set parameters of Motor
void CBackEndInterface::SetMotorParameters(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->InitMotorAxisParameters(Axis);
}


// Go to the home position of one of the motors
void CBackEndInterface::GoToMotorHomePosition(TMotorAxis Axis, bool Blocking)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  if (!Blocking)
    Motor->MoveHome(Axis);
  else
    Motor->GoWaitHome(Axis);
}

// X loop motion motion for Tempo XL
void CBackEndInterface::XLoopMotorMotion(long MinPosition,long MaxPosition)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->XLoopMotion(MinPosition,MaxPosition,muSteps);
}

// Kill the motion of one of the motors
void CBackEndInterface::KillMotorMotion(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->KillMotion(Axis);
}

// Stop the motion of one of the motors
void CBackEndInterface::StopMotorMotion(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->Stop(Axis);
}

//Move one of the motors to an absulote position
TQErrCode CBackEndInterface::MoveMotorToAbsolutePosition(TMotorAxis Axis,TMotorPosition Position, bool Blocking,TMotorUnits Units)
{
  TQErrCode Err = Q_NO_ERROR;
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  if (!Blocking)
    Err = Motor->MoveToAbsolutePosition(Axis,Position,Units);
  else
    Err = Motor->GoWaitToAbsolutePosition(Axis,Position,Units, false);
  return Err;
}

//Move one of the motors to an absulote position
void CBackEndInterface::MoveZToAbsolutePositionWithoutCheckTray(long Position)
{
  Q2RTApplication->GetMachineManager()->GetMachineSequencer()->
     GetMotorsInstance()->MoveZToAbsolutePositionWithoutCheckTray(Position,muSteps);
}


//Move one of the motors to a relative position
TQErrCode CBackEndInterface::MoveMotorRelative(TMotorAxis Axis,long Offset,bool Blocking)
{
  TQErrCode Err = Q_NO_ERROR;
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  if(!Blocking)
     Err = Motor->MoveRelativePosition(Axis,Offset,muSteps);
  else
     Err = Motor->GoWaitToRelativePosition(Axis,Offset,muSteps);
  return Err;
}

// Move the motor of Z axis to the Z start position
void CBackEndInterface::MoveMotorToZStartPosition()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  Motor->MoveToAbsolutePosition(AXIS_Z, CAppParams::Instance()->Z_StartPrintPosition,muMM);
}

// Move the motor of Z axis to the Z tray out position
void CBackEndInterface::MoveMotorToZTrayOutPosition()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  Motor->MoveToAbsolutePosition(AXIS_Z, CAppParams::Instance()->ZAxisEndPrintPosition,muMM);
}

// Move the motor of Z axis down -  (if objet 1000 , the tray will move till the wheels position, and not to pins position)
TQErrCode CBackEndInterface::MoveZAxisDownToLowestPosition()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  return Motor->MoveToAbsolutePosition(AXIS_Z, CAppParams::Instance()->MaxPositionStep[AXIS_Z],muSteps);
}

// Move the motor of Z axis down -  (if objet 1000 , the tray will move till the pins position, and not to wheels position)ls)
TQErrCode CBackEndInterface::MoveZAxisDown()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  return Motor->MoveToAbsolutePosition(AXIS_Z, CAppParams::Instance()->MaxPositionStep[AXIS_Z] - CAppParams::Instance()->ZMaxPositionMargin ,muSteps);
}

TQErrCode CBackEndInterface::MoveZAxisDownWiperWizard()
{

  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  if(Q2RTApplication->GetMachineType() == (mtObjet1000 ))
  {
  	return Motor->MoveToAbsolutePosition(AXIS_Z, (CAppParams::Instance()->MaxPositionStep[AXIS_Z])/2,muSteps);
  }
  else
  {
        return Motor->MoveToAbsolutePosition(AXIS_Z, CAppParams::Instance()->MaxPositionStep[AXIS_Z],muSteps);
  }

}

// Move the motor of T axis to the wipe position
TQErrCode CBackEndInterface::MoveMotorToTWipePosition()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  return Motor->WipeWasteTank();
}

// Move the motor of T axis to the purge position
void CBackEndInterface::MoveMotorToTPurgePosition()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->OpenWasteTank();
}


// Set the speed for one of the motors and return the previous speed
long CBackEndInterface::SetMotorSpeed(TMotorAxis Axis, long Speed)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  long PreviousSpeed = Motor->GetAxisCurrentVelocity(Axis);
  Motor->SetVelocity(Axis, Speed,muSteps);

  return PreviousSpeed;
}

// Get the minimum and maximum positions of the motors
void CBackEndInterface::GetMotorsMinAndMaxPositions(long *Positions)
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  // fill the array with the minimum and maximum positions for each axis (excluding axis T)
  // (the minimum positions are int the odd cells and the maximum are int even cells)
  int j = 0;
  for (int i = 0 ; i < ParamsMgr->MinPositionStep.Size()-1; i++, j+=2)
    Positions[j] = ParamsMgr->MinPositionStep[i];

  j = 1;
  for (int i = 0 ; i < ParamsMgr->MaxPositionStep.Size()-1; i++, j+=2)
    Positions[j] = ParamsMgr->MaxPositionStep[i];
}

// Is one of the motor enabled
bool CBackEndInterface::IsMotorEnabled(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return Motor->IsMotorEnabled(Axis);
}

// Is one of the motor moving
bool CBackEndInterface::IsMotorMoving(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return Motor->IsMotorMoving(Axis);
}


// Is in one of the motor axis home already done
bool CBackEndInterface::IsMotorHomeDone(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return Motor->GetAxisIfHomeWasPerformed(Axis);
}


// Is in one of the motor axis home already done
bool CBackEndInterface::IsMotorHomeEverDone(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return Motor->GetAxisIfHomeWasEverPerformed(Axis);
}

// Send a request to get the current position of the motors
void CBackEndInterface::DisplayMotorsPosition()
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motor->GetAxisPosition();
}

// Get the current position of the motors
int CBackEndInterface::GetMotorsPosition(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return ((int)Motor->GetAxisLocation(Axis));
}

// wait for end of program and enable stop and kill
bool CBackEndInterface::WaitForMotorProgramFinish(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return ((int)Motor->WaitForProgramFinish(Axis));
}

// Get the current status of "passed the right limit"
int CBackEndInterface::MotorGetAxisRightLimit(TMotorAxis Axis)
{
  CMotorsBase* Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return ((int)Motor->GetAxisRightLimit(Axis));
}

// Get the current status of "passed the left limit"
int CBackEndInterface::MotorGetAxisLeftLimit(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return ((int)Motor->GetAxisLeftLimit(Axis));
}

// Causes the Axis_Status_Report_Message to be sent from the MCB.
TQErrCode CBackEndInterface::MotorGetLimitsStatus(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return ((int)Motor->GetLimitsStatus(Axis));
}


// Turn on/off one of the OCB actuators
void CBackEndInterface::SetActuatorState(int ActuatorID, bool On)
{
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();

  Actuators->SetOnOff(ActuatorID, On ? 1 : 0);
}

//Get actuator status...
bool CBackEndInterface::GetActuatorOnOff (int ID)
{
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();

  return Actuators->GetActuatorOnOff(ID);
}

bool CBackEndInterface::IsWasteActive()
{
	CContainerBase *Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->IsWasteActive();
}

TQErrCode CBackEndInterface::ActivateRollerAndPurgeWaste(bool Activate)
{
	CContainerBase *Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->ActivateRollerAndPurgeWaste(Activate);
}

TQErrCode CBackEndInterface::ActivateWaste(bool Activate)
{
	CContainerBase *Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->ActivateWaste(Activate);
}

// Are the UV lamps turned on
bool CBackEndInterface::AreUVLampsTurnedOn()
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();

  return UVLamps->GetTurnStatus();
}

// Update (from OCB) the last UV command sent to the OCB.
bool CBackEndInterface::GetUVLampsLastCommand()
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();

  return UVLamps->GetUVLampsLastCommand();
}


TQErrCode CBackEndInterface::SetDefaultParmIgnitionTimeout()
{
  CUvLamps *UVLamps = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetUVLampsInstance();

  return UVLamps->SetDefaultParmIgnitionTimeout();
}

// Is the roller turned on
bool CBackEndInterface::IsRollerTurnedOn()
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  return Roller->GetRollerOnOffValue();
}

// Is the power turned on
bool CBackEndInterface::IsPowerTurnedOn()
{
  CPowerBase *Power = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPowerInstance();

  return Power->GetPowerOnOffValue();
}

// Jog one of the motors
void CBackEndInterface::JogMotor(TMotorAxis Axis, long Position, int JogCycles)
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motors->JogMoveToAbsolutePosition(Axis,Position,JogCycles,muSteps);
}

TQErrCode CBackEndInterface::WaitForEndOfSWReset(int AdditionalTime)
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  return Motors->WaitForEndOfSWReset(AdditionalTime);
}

// Wait for end of movement of one of the axes
TQErrCode CBackEndInterface::WaitForEndOfMovement(TMotorAxis Axis)
{
  return WaitForEndOfMovement(Axis,5);
}

TQErrCode CBackEndInterface::WaitForEndOfMovement(TMotorAxis Axis,int AdditionalTime)
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return Motors->WaitForEndOfMovement(Axis,AdditionalTime);
}

// Is the head filling turned on
bool CBackEndInterface::IsHeadFillingOn()
{
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();

  return HeadFilling->GetOnOff();
}

// Is the head heater turned on
bool CBackEndInterface::IsHeadHeaterOn()
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->IsHeaterOn();
}

//Mark that the form of Actuator Controll is on
void CBackEndInterface::MarkIfPollingControlIsOn(bool OnOff)
{
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();

  Actuators->MarkIfPollingControlIsOn(OnOff);;
}


// Set the heads voltages
void CBackEndInterface::SetHeadsVoltages(int *PotentiometerValues, int NumOfHeads)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  HeadHeaters->SetPrintingHeadsVoltage(PotentiometerValues);
}

// Get the current head voltages (in A/D units)
void CBackEndInterface::GetHeadsA2DVoltages(int *Voltages, int NumOfHeads)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  HeadHeaters->GetPrintingHeadsVoltages();

  for (int i = 0; i < NumOfHeads; i++)
    Voltages[i] = HeadHeaters->GetCurrentVoltageTable(i);

}

// Get the current head voltages
void CBackEndInterface::GetHeadsVoltages(float *Voltages, int NumOfHeads)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  HeadHeaters->GetPrintingHeadsVoltages();

  for (int i = 0; i < NumOfHeads; i++)
    Voltages[i] = HeadHeaters->GetCurrentHeadVoltage(i);

}

// Get the potentiometer values
void CBackEndInterface::GetHeadsPotentiometrValues(int *Values)
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  for (int i = 0 ; i < ParamsMgr->PotentiometerValues.Size(); i++)
    Values[i] = ParamsMgr->PotentiometerValues[i];
}

// Get the current value of a head potentiometer
int CBackEndInterface::GetCurrHeadPotentiometerValue(int HeadNum)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->GetSetVoltageTable(HeadNum);
}


// Get the requested head voltages
void CBackEndInterface::GetHeadRequestedVoltagesModel(float* ReqVoltages)
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  for (int i = 0 ; i < ParamsMgr->RequestedHeadVoltagesModel.Size(); i++)
	ReqVoltages[i] = ParamsMgr->RequestedHeadVoltagesModel[i];
}
// Get the requested head voltages
void CBackEndInterface::GetHeadRequestedVoltagesSupport(float* ReqVoltages)
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  for (int i = 0 ; i < ParamsMgr->RequestedHeadVoltagesSupport.Size(); i++)
    ReqVoltages[i] = ParamsMgr->RequestedHeadVoltagesSupport[i];
}
// Set the slice image preview callback
void CBackEndInterface::SetSlicePreviewUpdateEvent(TGenericCallback SlicePreviewUpdateEvent,
                                                   TGenericCockie Cockie)
{
  Q2RTApplication->GetHostComm()->SetSlicePreviewUpdateEvent(SlicePreviewUpdateEvent,Cockie);
}

// Request a model BMP for slice preview
PBYTE CBackEndInterface::RequestBMPAndLock(int Chamber, int& Width,int& Height,int& SliceNumber,TGenericCockie /*Cockie*/)
{
  return Q2RTApplication->GetHostComm()->RequestBMPAndLock(Chamber,Width,Height,SliceNumber);
}

// Release the slice preview BMP lock
void CBackEndInterface::ReleaseBMPBuffersLock(TGenericCockie /*Cockie*/)
{
  Q2RTApplication->GetHostComm()->ReleaseBMPBuffersLock();
}

// Import parameters configuration from file
void CBackEndInterface::ImportConfiguration(const QString FileName)
{
  Q2RTApplication->ImportConfiguration(FileName);
}

// Is the material cartridge inserted
bool CBackEndInterface::IsActiveLiquidTankInserted(int Chamber)
{
 
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  bool ret = Containers->IsActiveLiquidTankInserted(Chamber);
  return ret;
}

TTankIndex CBackEndInterface::GetActiveTankNum(TChamberIndex Chamber)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetActiveTankNum(Chamber);
}

// Is the waste container inserted
TQErrCode CBackEndInterface::GetLiquidTankInsertedStatus()
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetLiquidTankInsertedStatus();
}
// Is the model weight above the minimum level
bool CBackEndInterface::IsWeightOk(int Chamber)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->IsWeightOk((TChamberIndex)Chamber);
}

// Handle the closing of the cartridge error dialog
void CBackEndInterface::HandleCartridgeErrorDlgClose(int Result)
{
  CMachineManager*   Mgr       = Q2RTApplication->GetMachineManager();
  if (!Mgr)
     return;
  CMachineSequencer* Sequencer = Mgr->GetMachineSequencer();
  if (!Sequencer)
     return;
  Sequencer->HandleCartridgeDlgClose(Result);
}

unsigned int CBackEndInterface::GetLastInsertionTimeInTicks(TTankIndex Tank)
{
 CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
 return Containers->GetLastInsertionTimeInTicks(Tank);
}

float CBackEndInterface::GetTankWeightInA2D(int Tank)
{
  CContainerBase* Containers = NULL;
  if (Q2RTApplication &&
      Q2RTApplication->GetMachineManager() &&
      Q2RTApplication->GetMachineManager()->GetMachineSequencer())
     Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return (Containers ? Containers->GetTankWeightInA2D((TTankIndex)Tank) : 0);
}
// Handle the closing of the Tank Identification dialog.
void CBackEndInterface::HandleTankIdentificationDlgClose(TTankIndex Container, int Result)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->HandleTankIdentificationDlgClose(Container, Result);
}

float CBackEndInterface::GetTankWeightInGram(int Tank) // todo -oNobody -cNone : use TTankIndex here and elsewhere
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetTankWeightInGram((TTankIndex)Tank);
}

// Get the current model weight in grams
int CBackEndInterface::GetWeight(int Chamber, int Tank)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetWeight((TChamberIndex)Chamber, (TTankIndex)Tank);
}//GetWeight

// Get the current model total weight in grams
int CBackEndInterface::GetTotalWeight(int Chamber)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetTotalWeight(Chamber);
}

// Set the gain of the model weight sensor
void CBackEndInterface::SetWeightSensorGain(int Tank, float Gain)
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  CContainerBase* Containers   = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();

  Containers->SetContainerGain((TTankIndex)Tank, Gain);

  ParamsMgr->WeightSensorGainArray[Tank] = Gain;
  ParamsMgr->SaveSingleParameter(&ParamsMgr->WeightSensorGainArray);
}//SetWeightSensorGain

// Set the offset of the model weight sensor
void CBackEndInterface::SetWeightSensorOffset(int Tank, float Offset)
{
  CAppParams*     ParamsMgr  = CAppParams::Instance();
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();

  Containers->SetContainerOffset((TTankIndex)Tank, Offset);

  ParamsMgr->WeightSensorOffsetArray[Tank] = Offset;
  ParamsMgr->SaveSingleParameter(&ParamsMgr->WeightSensorOffsetArray);
}//SetWeightSensorOffset

int CBackEndInterface::GetRemainingWasteWeight()
{
	CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->GetRemainingWasteWeight();
}

// Turn the matrial cooling fans on/off (and set the requested duty cycle)
void CBackEndInterface::TurnCoolingFansOnOff(bool On, BYTE DutyCycle)
{
  CAmbientTemperatureBase *AmbientTemp = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetAmbientTemperatureInstance();

  if (On)
    AmbientTemp->SetMaterialCoolingFansDutyCycle(DutyCycle);

  AmbientTemp->SetMaterialCoolingFanOnOff(On);
}


// Get the current voltage of the VPP power supply
float CBackEndInterface::GetVppPowerSupply()
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->GetVppPowerSupply();
}


// Are the temperatures of the heads Ok
bool CBackEndInterface::AreHeadsTemperaturesOk()
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return  HeadHeaters->AreHeadsTemperaturesOk();
}


void CBackEndInterface::PrintHeatersTemperaturesToLog()
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  HeadHeaters->DebugDisplayHeadStatus();
}



// Is a specific heaters' temperature is Ok
bool CBackEndInterface::IsHeaterTemperatureOk (int HeaterNum)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return  HeadHeaters->IsHeaterTemperatureOk(HeaterNum);
}

void CBackEndInterface::WipeHeads(bool GoHome)
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  Sequencer->PerformWipe(GoHome);
}

bool CBackEndInterface::AreHeadsFilled()
{
  CHeadFillingBase* HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
      if(!HeadFilling->GetIfCurrentThermistorIsFull((TChamberIndex)i))
         return false;
  }
  return true;
}

bool CBackEndInterface::AreHeadsEmpty()
{
  CHeadFillingBase* HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
      if(HeadFilling->GetIfCurrentThermistorIsFull((TChamberIndex)i))
         return false;
  }
  return true;
}

bool CBackEndInterface::AreResinHeadsFilled(int Chamber)
{ 
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
  return (HeadFilling->GetIfCurrentThermistorIsFull(Chamber));
}

int CBackEndInterface::GetTrayTemperatureStatus()
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayHeaterInstance();

  return Tray->GetCurrentTrayInRangeStatus();
}

// Is the door locked?
bool CBackEndInterface::IsDoorLocked()
{
  CDoorBase *Door = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetDoorInstance();

  return Door->IsDoorReady();
}

// Is the tray in?
bool CBackEndInterface::IsTrayInserted()
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayPlacerInstance();
  return Tray->IsTrayInserted();
}

//Verify if tray is in until it is in or canceled
TQErrCode CBackEndInterface::VerifyTrayInserted(bool showDialog)
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayPlacerInstance();
  return Tray->VerifyTrayInserted(showDialog);
}

//Wait until tray inserted
void CBackEndInterface::BEI_WaitUntilTrayInserted()
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayPlacerInstance();
  Tray->WaitUntilTrayInserted();
}

// Get the speed of one of the motors
long CBackEndInterface::GetMotorSpeed(TMotorAxis Axis)
{
  CMotorsBase *Motor = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  return Motor->GetAxisCurrentVelocity(Axis);
}

// Get the current state of the machine
TMachineState CBackEndInterface::GetCurrentMachineState()
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  return (Mgr ? Mgr->GetCurrentState() : msIdle);
}

// Turn the hood fan Idle
void CBackEndInterface::SetHoodFanIdle()
{
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();

  Actuators->SetHoodFanIdle();
}

// Turn the hood fan on/off
void CBackEndInterface::SetHoodFanOnOff(bool OnOff)
{
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();

  Actuators->SetHoodFanOnOff(OnOff);
}

void CBackEndInterface::SetHoodFanParamOnOff (bool OnOff, int Voltage)
{
  CActuatorBase *Actuators = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetActuatorInstance();

  Actuators->SetHoodFanParamOnOff(OnOff, Voltage);
}

// Get the current value of head filling thermistor
int CBackEndInterface::GetCurrentResinFillThermistor(TChamberIndex Chamber)
{
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
  return HeadFilling->GetCurrentThermistorFull(Chamber);
}

bool CBackEndInterface::IsHeadFillingAllowed()
{
  CHeadFillingBase *HeadFilling = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadFillingInstance();
  return HeadFilling->IsHeadFillingAllowed();
}

// Modes enumeration and handling

void CBackEndInterface::RefreshModesList(void)
{
  CModesManager::Instance()->Refresh();
}

void CBackEndInterface::BeginModesEnumeration(void)
{
  CModesManager::Instance()->BeginEnumeration();
}

QString CBackEndInterface::GetNextModeName(QString Dir)
{
  return CModesManager::Instance()->GetNextModeName(Dir);
}

QString CBackEndInterface::GetModesStackPath(void)
{
  return CModesManager::Instance()->GetModesStackPath();
}

QString CBackEndInterface::GetCurrentModeName(void)
{
  return CModesManager::Instance()->GetCurrentModeName();
}


// Go into a mode (use mode name)
TQErrCode CBackEndInterface::EnterMode(const QString ModeName, const QString DirName)
{
  return CModesManager::Instance()->EnterMode(ModeName, DirName);
}

// Go one mode back
void CBackEndInterface::GoOneModeBack(void)
{
  CModesManager::Instance()->GoOneModeBack();
}

// Restore deault mode
void CBackEndInterface::GotoDefaultMode(void)
{
  CModesManager::Instance()->GotoDefaultMode();
}


// Get the size of modes stack
int CBackEndInterface::GetModesStackSize()
{
  return CModesManager::Instance()->GetModesStackSize();
}


// Save All the modes
void CBackEndInterface::SaveAllModes()
{
  CModesManager::Instance()->Save();
}

// Allow bypassing the 'Are you sure you want to update..."
TQErrCode CBackEndInterface::EnableConfirmationBypass(const QString ModeName, QString Dir)
{
  return CModesManager::Instance()->EnableConfirmationBypass(ModeName, Dir);
}

// Don't allow bypassing the 'Are you sure you want to update..."
TQErrCode CBackEndInterface::DisableConfirmationBypass(const QString ModeName, QString Dir)
{
  return CModesManager::Instance()->DisableConfirmationBypass(ModeName, Dir);
}



// Close the waste tank
void CBackEndInterface::CloseWasteTank()
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();

  Motors->CloseWasteTank();
}

// Get the current temperature of one of the heads
int CBackEndInterface::GetHeadTemperature(int HeadNumber)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->GetHeadTemperature(HeadNumber);
}

// Return a version string for various items
QString CBackEndInterface::GetVersion(int WhatToGet)
{
  QString Result = "";
  CMachineSequencer *Seq = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
  CHaspInterface* Hasp    = CHaspInterfaceWrapper::Instance();

  switch(WhatToGet)
  {
    case VERSION_APP:
      Result = QGetAppVersionStr();
      break;

    case VERSION_OCB:
      Result = QIntToStr(Seq->GetOCBStatusInstance()->GetOCBSWExternalVersion()) + "."
			   + QFormatStr("%02d", Seq->GetOCBStatusInstance()->GetOCBSWInternalVersion());
	  break;

	case VERSION_OCB_A2D:
	  Result = QIntToStr(Seq->GetOCBStatusInstance()->GetOCB_A2D_SWExternalVersion()) + "."
			   + QFormatStr("%02d", Seq->GetOCBStatusInstance()->GetOCB_A2D_SWInternalVersion());
      break;

    case VERSION_OHDB_SW:
      Result = QIntToStr(Seq->GetHeadStatusInstance()->GetOHDBSWExternalVersion()) + "."
               + QFormatStr("%02d", Seq->GetHeadStatusInstance()->GetOHDBSWInternalVersion());
      break;

    case VERSION_OHDB_HW:
      Result = QIntToStr(Seq->GetHeadStatusInstance()->GetOHDBHWVersion());
      break;

    case VERSION_OHDB_A2D:
      Result = QIntToStr(Seq->GetHeadStatusInstance()->GetOHDB_A2D_SWExternalVersion()) + "."
               + QFormatStr("%02d", Seq->GetHeadStatusInstance()->GetOHDB_A2D_SWInternalVersion());
      break;

    case VERSION_EDEN_PCI:
      Result = QIntToStr((int)(PCIReadDWORD(2,48) & 0xFF));
      break;

    case VERSION_MCB_SW:
      Result = Seq->GetMotorsInstance()->GetSWVersion();
      break;

    case VERSION_MCB_HW:
      Result = Seq->GetMotorsInstance()->GetHWVersion();
      break;

    case VERSION_HASP_API:
      if(Hasp)
		Result = Hasp->GetAPIVersion();
	  break;
    //Elad added , I2C HW+FW Version.
	case HW_FW_VERSION_MSC1:
	  Result = QIntToStr(Seq->GetOCBStatusInstance()->GetMSCHWVersion(MSC_CARD_1)) + "."
			   + QIntToStr(Seq->GetOCBStatusInstance()->GetMSCFWVersion(MSC_CARD_1));
	  break;
	case HW_FW_VERSION_MSC2:
	  Result = QIntToStr(Seq->GetOCBStatusInstance()->GetMSCHWVersion(MSC_CARD_2)) + "."
			   + QIntToStr(Seq->GetOCBStatusInstance()->GetMSCFWVersion(MSC_CARD_2));
	  break;
#ifdef OBJET_MACHINE_KESHET
	case HW_FW_VERSION_MSC3:
	  Result = QIntToStr(Seq->GetOCBStatusInstance()->GetMSCHWVersion(MSC_CARD_3)) + "."
			   + QIntToStr(Seq->GetOCBStatusInstance()->GetMSCFWVersion(MSC_CARD_3));
	  break;
	case HW_FW_VERSION_MSC4:
	  Result = QIntToStr(Seq->GetOCBStatusInstance()->GetMSCHWVersion(MSC_CARD_4)) + "."
			   + QIntToStr(Seq->GetOCBStatusInstance()->GetMSCFWVersion(MSC_CARD_3));
	  break;
#endif	  
  }

  return Result;
}


void CBackEndInterface::SetAndMeasureHeadVoltage(int HeadNumber, float ReqVoltage)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  HeadHeaters->SetAndMeasureHeadVoltage(HeadNumber, ReqVoltage);
}

void CBackEndInterface::ReadHeadParametersFromE2PROM(int HeadNumber, THeadParameters* HeadParameters)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->ReadHeadParametersFromE2PROM(HeadNumber, HeadParameters);
}

void CBackEndInterface::SaveHeadParametersOnE2PROM(int HeadNumber, THeadParameters* HeadParameters)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->SaveHeadParametersOnE2PROM(HeadNumber, HeadParameters);
}

void CBackEndInterface::SaveHeadHeaterParameter(int HeadNumber,int Value)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
  HeadHeaters->SaveHeadHeaterParameter(HeadNumber,Value);
}

int CBackEndInterface::GetHeadHeaterParameter(int HeadNumber)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
  return HeadHeaters->GetHeadHeaterParameter(HeadNumber);
}

BYTE CBackEndInterface::ReadFromE2PROM(int HeadNumber, USHORT Address)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return HeadHeaters->ReadFromE2PROM(HeadNumber, Address);
}

void CBackEndInterface::WriteToE2PROM(int HeadNumber, USHORT Address,BYTE Data)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  HeadHeaters->WriteToE2PROM(HeadNumber, Address, Data);

}

bool CBackEndInterface::AreHeadHeatersThermistorsValid(bool DuringPrint)
{
  CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

  return !(HeadHeaters->CheckThermistorsStatus(DuringPrint));
}

int CBackEndInterface::GetAverageVacuumValue_A2D()
{
  CHeadVacuumBase *HeadVacuum = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadVacuumInstance();

  return HeadVacuum->GetVacuumAverageSensorValue_A2D();
}

float CBackEndInterface::GetAverageVacuumValue()
{
  CHeadVacuumBase *HeadVacuum = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadVacuumInstance();

  return HeadVacuum->GetVacuumAverageSensorValue_CmH2O();
}


int CBackEndInterface::GetCurrentVacuumValue_A2D()
{
  CHeadVacuumBase *HeadVacuum = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadVacuumInstance();

  return HeadVacuum->GetLastVacuumA2DValue();
}


// Resume printing after pause
void CBackEndInterface::ResumePrint(void)
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  try
  {
    Mgr->StartPrint();
  }
  catch(EQException& Exception)
  {
    QMonitor.ErrorMessage(Exception.GetErrorMsg());
  }
}


void CBackEndInterface::EnableStandByMode()
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
     Mgr->EnableStandbyPhase();
}

void CBackEndInterface::DisableStandByMode()
{
  CMachineManager *Mgr = Q2RTApplication->GetMachineManager();
  if (Mgr)
     Mgr->DisableStandbyPhase();
}


void CBackEndInterface::ResetDriverState()
{
    CPrintControl * pPrtCtrl = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPrintControlInstance();

    if (pPrtCtrl)
    {
        pPrtCtrl->ResetDriverStateMachine();
    }
}


#ifdef MOTOR_MCB

//Diagnostic Procedures
int CBackEndInterface::MCBDiagRead(int Parameter)
{
  CMotorsBase *Motors = dynamic_cast<CMotorsBase *>(Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance());
  return Motors->GetDiagSWValue(Parameter);
}

TQErrCode CBackEndInterface::MCBDiagWrite(int Parameter,int Value)
{
  CMotorsBase *Motors = dynamic_cast<CMotorsBase *>(Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance());

  return Motors->DiagSWWrite(Parameter,Value);
}

void CBackEndInterface::MCBDownload(void)
{
  CMotorsBase *Motors = dynamic_cast<CMotorsBase *>(Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance());

  Motors->DownloadStart();
}

TQErrCode CBackEndInterface::MCBEnterDiag(void)
{
  CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
  return Motors->DiagModeStart();
}

TQErrCode CBackEndInterface::DiagInitiateDataLogCounter(int InitialType)
{
	CMotorsBase *Motors = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetMotorsInstance();
	return Motors->DiagInitiateDataLogCounter(InitialType);
}

#endif

// Stop a sequence
void CBackEndInterface::StopSequence(TMachineState State)
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  Sequencer->StopSequence(State);
}

// BIT related functions
void CBackEndInterface::DoTest(const QString TestGroupName,const QString TestName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->DoTest(TestGroupName,TestName);
}

void CBackEndInterface::BeginTestGroupsEnumeration(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->BeginTestGroupsEnumeration();
}

QString CBackEndInterface::GetNextTestGroup(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetNextTestGroup();
}

void CBackEndInterface::BeginTestsEnumeration(QString GroupName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->BeginTestsEnumeration(GroupName);
}

QString CBackEndInterface::GetNextTest(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetNextTest();
}

int CBackEndInterface::GetNextTestAttributes(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetNextTestAttributes();
}

int /*TTestResult*/ CBackEndInterface::GetLastTestResult(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetLastTestResult();
}

QString CBackEndInterface::GetLastTestErrorDescription(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetLastTestErrorDescription();
}

void CBackEndInterface::ResetTest(const QString TestGroupName,const QString TestName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->ResetTest(TestGroupName,TestName);
}

void CBackEndInterface::ResetAllTests(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->ResetAllTests();
}

// Refresh BIT tests
void CBackEndInterface::RefreshBIT(void)
{
  Q2RTApplication->RefreshBIT();
}

void CBackEndInterface::BeginBITSetsEnumeration(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->BeginBITSetsEnumeration();
}

QString CBackEndInterface::GetNextBITSet(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetNextBITSet();
}

bool CBackEndInterface::BeginTestsInSetEnumeration(const QString SetName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->BeginTestsInSetEnumeration(SetName);
}

QString CBackEndInterface::GetNextTestInSet(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetNextTestInSet();
}

int /*TTestResult*/ CBackEndInterface::RunBITSet(const QString BITsetName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->RunBITSet(BITsetName);
}

void CBackEndInterface::CreateBITReportFile(const QString ReportFileName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->CreateReportFile(ReportFileName);
}

void CBackEndInterface::StartBITSetDefine(const QString SetName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->StartBITSetDefine(SetName);
}

void CBackEndInterface::AddToBITSet(const QString TestGroupName,const QString TestName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->AddToBITSet(TestGroupName,TestName);
}

void CBackEndInterface::SaveBITSet(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  BITManager->SaveBITSet();
}

// Begin test execution
int /*TTestResult*/ CBackEndInterface::StartTest(const QString TestGroupName,const QString TestName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->StartTest(TestGroupName,TestName);
}

// Execute next step in a test
int /*TTestResult*/ CBackEndInterface::ExecNextTestStep(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->ExecNextStep();
}

// Finish test execution
int /*TTestResult*/ CBackEndInterface::EndTest(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->EndTest();
}

// Get total number of steps in current active test
int CBackEndInterface::GetCurrentTestStepsNum(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetCurrentTestStepsNum();
}

// Get current step name
QString CBackEndInterface::GetCurrentTestStepName(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetCurrentStepName();
}

int CBackEndInterface::BeginTestResultExEnumeration(const QString TestGroupName,const QString TestName)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->BeginTestResultExEnumeration(TestGroupName,TestName);
}

QString CBackEndInterface::GetNextTestResultEx(void)
{
  CBITManager *BITManager = Q2RTApplication->GetBITManager();
  return BITManager->GetNextTestResultEx();
}

bool CBackEndInterface::EnableDisableVacuumErrorHandling(bool Enable)
{
  CHeadVacuumBase *HeadVacuum = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadVacuumInstance();
  if (NULL == HeadVacuum)
  {
      return false;
  }
  return HeadVacuum->EnableDisableErrorHandling(Enable);
}

void CBackEndInterface::EnableDisableBumperPeg(bool Enable)
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  Roller->EnableDisableBumperPeg(Enable);
}


// Enable/disable the bumper
void CBackEndInterface::EnableDisableBumper(bool Enable)
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  Roller->BumperEnableDisable(Enable);
}

// Set the bumper parameters
void CBackEndInterface::SetBumperParams(int Sensitivity,int ResetTime,int ImpactCounter)
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  Roller->SetBumperParams(Sensitivity, ResetTime, ImpactCounter);
}

// Get the bumper impact status
bool CBackEndInterface::GetBumperImpactStatus()
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  return Roller->GetBumperImpact();
}

// Reset the bumper impact status
void CBackEndInterface::ResetBumperImpactStatus()
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  Roller->ResetBumperImpact();
}
void CBackEndInterface::ResetBumperImpactCounter()
{
  CRollerBase *Roller = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetRollerInstance();

  Roller->ResetBumperImpactCounter();
}

// Set the start and end PEG and the number of fires
void CBackEndInterface::SetLayerPrintParameters(int StartPEG,int EndPEG,int NoOfFires)
{
  CPrintControl *PrintControl = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetPrintControlInstance();

  PrintControl->SetLayerParams(StartPEG, EndPEG, NoOfFires);
}

// Enable a maintenance counter
void CBackEndInterface::EnableDisableMaintenanceCounter(int CounterNumber, bool OnOff)
{
  CMaintenanceCounters* MaintenanceCounters = CMaintenanceCounters::GetInstance();
  if (!MaintenanceCounters)
     return;
  if (OnOff)
    MaintenanceCounters->EnableCounter(CounterNumber);
  else
    MaintenanceCounters->DisableCounter(CounterNumber);
}

// Get the elapsed seconds of a maintenance counter
ULONG CBackEndInterface::GetMaintenanceCounterElapsedSeconds(int CounterNumber)
{
  return CMaintenanceCounters::GetInstance()->GetElapsedSeconds(CounterNumber);
}

// Reset a maintenance counter
void CBackEndInterface::ResetMaintenanceCounter(int CounterNumber)
{
  CMaintenanceCounters::GetInstance()->ResetCounter(CounterNumber);
}

// Set a maintenance counter
void CBackEndInterface::SetMaintenanceCounter(int CounterNumber, long Secs)
{
  CMaintenanceCounters::GetInstance()->SetCounterTime(CounterNumber,Secs);
}

// advance a maintenance counter
void CBackEndInterface::AdvanceCounterBySeconds(int CounterNumber, unsigned Seconds)
{
  CMaintenanceCounters::GetInstance()->AdvanceCounterBySeconds( CounterNumber, Seconds);
}


TQErrCode CBackEndInterface::UpdateTanks(int Tank)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->UpdateTanks(static_cast<TTankIndex>(Tank));
  return Q_NO_ERROR;
}

void CBackEndInterface::UpdateChambers(bool UpdateActiveTherms)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->UpdateChambers(UpdateActiveTherms);
}

bool CBackEndInterface::IsRelevantTankInserted(int Tank)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->IsRelevantTankInserted(static_cast<TTankIndex>(Tank));
}

bool CBackEndInterface::IsMicroSwitchInserted(int Tank)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->IsMicroSwitchInserted(static_cast<TTankIndex>(Tank));
}


bool CBackEndInterface::AreRelevantTanksInserted(int Chamber, int Tank)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->AreRelevantTanksInserted((TChamberIndex)Chamber, (TTankIndex)Tank);
}

bool CBackEndInterface::IsChambersTank(int Chamber, int Tank)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->IsChambersTank((TChamberIndex)Chamber, (TTankIndex)Tank);
}

bool CBackEndInterface::IsTankInAndEnabled(int Tank)
{
  if ((FIRST_WASTE_TANK_TYPE < Tank) && !(CAppParams::Instance()->DualWasteEnabled))
  {
      return false; /* ignore this tank */
  }

  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  if (NULL == Containers)
  {
      return false;
  }
  return Containers->IsTankInAndEnabled(static_cast<TTankIndex>(Tank));
}


int CBackEndInterface::GetTotalNumberOfSlices()
{
  CMachineManager *MachineMgr = Q2RTApplication->GetMachineManager();

  return MachineMgr->GetTotalNumberOfSlices();
}

bool CBackEndInterface::IsPrintCompleted()
{
  CMachineManager *MachineMgr = Q2RTApplication->GetMachineManager();

  return MachineMgr->IsPrintCompleted();
}


int CBackEndInterface::TrayConvertCelciusToA2D(int Value)
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayHeaterInstance();

  return Tray->ConvertCelciusToA2D(Value);
}


int CBackEndInterface::TrayConvertA2DToCelcius(int Value)
{
  CTrayBase *Tray = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetTrayHeaterInstance();

  return Tray->ConvertA2DToCelcius(Value);
}


int CBackEndInterface::AmbientConvertCelciusToA2D(int Value)
{
  CAmbientTemperatureBase *AmbientTemp = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetAmbientTemperatureInstance();

  return AmbientTemp->ConvertCelciusToA2D(Value);
}


int CBackEndInterface::AmbientConvertA2DToCelcius(int Value)
{
  CAmbientTemperatureBase *AmbientTemp = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetAmbientTemperatureInstance();

  return AmbientTemp->ConvertA2DToCelcius(Value);
}

void CBackEndInterface::WriteToHOWHistoryFile(QString Str)
{
   CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

   Sequencer->WriteToHOWHistoryFile(Str);
}

void CBackEndInterface::WriteToUVHistoryFile(QString Str)
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  Sequencer->WriteToUVHistoryFile(Str);
}

void CBackEndInterface::OpenUVLampsHistoryFile()
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  Sequencer->OpenUVLampsHistoryFile();
}

void CBackEndInterface::CloseUVLampsHistoryFile()
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();

  Sequencer->CloseUVLampsHistoryFile();
}


void CBackEndInterface::ForceFileCreation(QString FullFileName, QString FileInitialContent)
{
  QString DirName = ExtractFilePath(FullFileName.c_str()).c_str();
  if (FileExists(FullFileName.c_str()) == false)
  {
    ForceDirectories(DirName.c_str());
    ofstream* File = new ofstream(FullFileName.c_str());
    *File << FileInitialContent;
    delete File;
  }
}

QString CBackEndInterface::GetContainerRFTagID(TTankIndex CartridgeID)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetTagIDAsString(CartridgeID);
}

QString CBackEndInterface::GetTankMaterialType(TTankIndex CartridgeID)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetTankMaterialType(CartridgeID);
}

QString CBackEndInterface::GetMachineMaterialType(TTankIndex TankIndex)
{
  CAppParams *ParamManager = CAppParams::Instance();
  return ParamManager->TypesArrayPerTank[TankIndex];
  /*
  TChamberIndex ti = TankToStaticChamber(TankIndex);
  if (NUMBER_OF_CHAMBERS == ti)
	return "";
  else
  	return ParamManager->TypesArrayPerChamber[ti];
  */
}

QString CBackEndInterface::GetPipeMaterialType(TTankIndex CartridgeID)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetPipeMaterialType(CartridgeID);
}

TQErrCode CBackEndInterface::AllowServiceMaterials(int/*TTankIndex*/ tank, bool Allow)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->SetAllowServiceMaterials(static_cast<TTankIndex>(tank), Allow);
  return Q_NO_ERROR;
}
TQErrCode CBackEndInterface::SkipMachineResinTypeChecking(int/*TTankIndex*/ CartridgeID, bool skip_tf)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->SetSkipMachineResinTypeChecking(static_cast<TTankIndex>(CartridgeID), skip_tf);
  return Q_NO_ERROR;
}

unsigned long CBackEndInterface::GetContainerExpirationDate(TTankIndex CartridgeID)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->GetContainerExpirationDate(CartridgeID);
}

bool CBackEndInterface::IsResinExpired(TTankIndex CartridgeID)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->IsResinExpired(CartridgeID);
}

void CBackEndInterface::LogParams(QString FileName)
{
  CAppParams::Instance()->LogParameters(FileName);
}

void CBackEndInterface::SendTanksRemovalSignals()
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->SendTanksRemovalSignals();
}

void CBackEndInterface::IgnoreTankInsertionAndRemovalSignals()
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->IgnoreTankInsertionAndRemovalSignals();
}

void CBackEndInterface::AllowTankInsertionAndRemovalSignals()
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->AllowTankInsertionAndRemovalSignals();
}

void CBackEndInterface::RemountSingleTankStatus(TTankIndex CartridgeID)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->RemountSingleTankStatus(CartridgeID);
}

bool CBackEndInterface::WaitForIdentificationCompletion(TTankIndex CartridgeID, bool DontWaitIfTankIsOut, unsigned int timetowait)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  return Containers->WaitForIdentificationCompletion(CartridgeID, DontWaitIfTankIsOut, timetowait);
}
//Get the support's resin type by its index
QString CBackEndInterface::GetSupportResinType(int resinIndex)
{
  return CModesManager::Instance()->GetSupportResinType(resinIndex);
}

bool CBackEndInterface::IsServiceSupportMaterial(QString material)
{
  return CModesManager::Instance()->IsServiceSupportMaterial(material);
}

bool CBackEndInterface::IsServiceModelMaterial(QString material)
{
  return CModesManager::Instance()->IsServiceModelMaterial(material);
}
bool CBackEndInterface::IsModelMaterial(QString material)
{
  return CModesManager::Instance()->IsModelMaterial(material);
}
bool CBackEndInterface::IsSupportMaterial(QString material)
{
  return CModesManager::Instance()->IsSupportMaterial(material);
}
void CBackEndInterface::SetUVWizardLimited(bool Limited)
{
  CMachineSequencer *Sequencer = Q2RTApplication->GetMachineManager()->GetMachineSequencer();
  Sequencer->SetUVWizardLimited(Limited);
}

bool CBackEndInterface::IsServiceMaterial(QString Material)
{
  return (IsServiceSupportMaterial(Material) || IsServiceModelMaterial(Material));
}

float CBackEndInterface::EstimateHeadVoltage(int PotVal)
{
    CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
    return HeadHeaters->EstimateHeadVoltage(PotVal);
}

int CBackEndInterface::CalculatePotValue(float ReqVoltage)
{
    CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();
    return HeadHeaters->CalculatePotValue(ReqVoltage);
}

void CBackEndInterface::DispatchHelp(int Tag, QString Path)
{
    Q2RTApplication->GetMachineManager()->GetMachineSequencer()->DispatchHelp(Tag, Path);
}

bool CBackEndInterface::IsMaxPumpTimeExceeded(TTankIndex Tank)
{
	CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->IsMaxPumpTimeExceeded(Tank);
}

void CBackEndInterface::CheckHaspPlugValidityDirectly()
{
    CHaspInterfaceWrapper::Instance()->CheckPlugValidityDirectly();
}

THaspPlugInfo CBackEndInterface::GetHaspPlugInfo()
{
   return CHaspInterfaceWrapper::Instance()->GetPlugInfo();
}

int CBackEndInterface::GetHaspDaysToExpiration()
{
    return CHaspInterfaceWrapper::Instance()->GetDaysToExpiration();
}

// The calculated time is in seconds. 0 - Disconnected / Expired / Invalid, -1 - Not time type license
int CBackEndInterface::CalculateHaspTimeToExpiration()
{
    return CHaspInterfaceWrapper::Instance()->CalculateTimeToExpiration();
}

THaspConnectionStatus CBackEndInterface::GetHaspConnectionStatus()
{
    if(! CHaspInterfaceWrapper::Instance())
        return hsDisconnected;
    return CHaspInterfaceWrapper::Instance()->GetConnectionStatus();
}

bool CBackEndInterface::ArePipesWithCleanser()
{
  CAppParams* ParamsMgr = CAppParams::Instance();
  bool ret = false;
  for (int i = FIRST_MODEL_SEGMENT; i < LAST_MODEL_SEGMENT && !ret; i++)
     ret |= (ParamsMgr->MRW_TypesArrayPerSegment[i].c_str() == MODEL_CLEANSER);
  for (int i = FIRST_SUPPORT_SEGMENT; i < LAST_SUPPORT_SEGMENT && !ret; i++)
     ret |= (ParamsMgr->MRW_TypesArrayPerSegment[i].c_str() == SUPPORT_CLEANSER);
  return ret;
}

bool CBackEndInterface::IsTankWeightOk(int Tank, bool CheckHysteresys)
{
	CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->IsTankWeightOk(static_cast<TTankIndex>(Tank), CheckHysteresys);
}

void CBackEndInterface::PauseOCBStatusThread()
{
	Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetOCBStatusInstance()->Pause();
}

void CBackEndInterface::PauseHeadStatusThread()
{
	Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadStatusInstance()->Pause();
}

void CBackEndInterface::ResumeOCBStatusThread()
{
	Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetOCBStatusInstance()->Resume();
}

void CBackEndInterface::ResumeHeadStatusThread()
{
	Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadStatusInstance()->Resume();
}

//OBJET_MACHINE feature
bool CBackEndInterface::UpdateWeightAfterWizard()
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->UpdateWeightAfterWizard();
  return true;
}

bool CBackEndInterface::UpdateWeightAfterMaterialConsumption(TTankIndex tankIndex)
{
  CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
  Containers->UpdateWeightAfterWizard(tankIndex);
  return true;
}

void CBackEndInterface::ShowMaterialBatchNoReport()
{
  CBatchNoTable::GetInstance()->ShowMaterialBatchNoReport();
}

bool CBackEndInterface::SetMachineActionsDate(QString name)
{
  return CActionsHistoryTable::GetInstance()->AddDate(name);
}

void CBackEndInterface::ShowMachineActionsReport()
{
  CActionsHistoryTable::GetInstance()->ShowMachineActionsReport();
}

TPackagesList CBackEndInterface::GetLicensedPackagesList()
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->GetLicensedPackages();
}

TLicensedMaterialsList CBackEndInterface::GetLicensedMaterialsList()
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->GetLicensedMaterials();
}

TLicensedMaterialsList CBackEndInterface::GetLicensedDigitalMaterialsList()
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->GetLicensedDigitalMaterials();
}

TTrayPropertiesList CBackEndInterface::GetLicensedTrayPropertiesList()
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->GetLicensedTrayProperties();
}

TAdditionalHostParamsList CBackEndInterface::GetLicensedAdditionalHostParamsList()
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->GetLicensedAdditionalHostParams();
}

TLicensedModesList CBackEndInterface::GetLicensedModesList()
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->GetLicensedModes();
}

bool CBackEndInterface::IsMaterialLicensed(QString MaterialName)
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->IsMaterialLicensed(MaterialName);
}

bool CBackEndInterface::IsModeLicensed(QString ModeName)
{
   CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
   return LicenseManger->IsModeLicensed(ModeName);
}

QString CBackEndInterface::GetPackagesDatabaseVersion()
{
	CLicenseManager *LicenseManger = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetLicenseManager();
	return LicenseManger->GetPackagesDatabaseVersion();
}

void CBackEndInterface::GetPCIDriverVersion(QString &Label, QString &Hint)
{
    CAppParams *ParamsMgr = CAppParams::Instance();
	if(CHECK_EMULATION(ParamsMgr->DataCard_Emulation))
	{
		Label = "0";
		Hint  = "Emulation";
		return;
    }

	TQStringVector tokens;
	const QString DELIMITER = ";";

	Tokenize(EdenPCISystem_GetKernelPluginVersion().c_str(), tokens, DELIMITER);

	Label = tokens[0];
	Hint  = tokens[1];
}

bool CBackEndInterface::GetWasteWeightStatusBeforePrinting()
{
	CContainerBase* Containers = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetContainerInstance();
	return Containers->GetWasteWeightStatusBeforePrinting();
}

TQErrCode CBackEndInterface::ActivateSignalTower(BYTE Red, BYTE Green, BYTE Yellow, bool ValidateState)
//TQErrCode CBackEndInterface::ActivateSignalTower(TSignalTowerLightState Red, TSignalTowerLightState Green, TSignalTowerLightState Yellow, bool ValidateState)
{
	return Q2RTApplication->GetMachineManager()->GetMachineSequencer()->ActivateSignalTower( Red, Green, Yellow, ValidateState);
}

BYTE CBackEndInterface::GetSignalTowerLightState(BYTE light)
{
	return (BYTE)( Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetSignalTower()->GetLightState( (TLightSignal)light ) );
}

void CBackEndInterface::SendOHDBCleanHeadWizardStartedNotification()
{
	CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

	HeadHeaters->SendHeadsCleaningWizardStartedNotification();
}

void CBackEndInterface::SendOHDBCleanHeadWizardEndedNotification()
{
	CHeadHeatersBase *HeadHeaters = Q2RTApplication->GetMachineManager()->GetMachineSequencer()->GetHeadHeatersInstance();

	HeadHeaters->SendHeadsCleaningWizardEndedNotification();
}

void CBackEndInterface::SetSleepingChannels()
{
	CMachineManager *Mgr = Q2RTApplication->GetMachineManager();

    Mgr->SetSleepingChannels();
}
