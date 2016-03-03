
#include "OCBSimulator.h"

void COCBSimulator::SetTrayTemperature(TOCBSetTrayTemperatureMessage&  SetTrayTemperature)
{
  m_SetTrayTemperature = SetTrayTemperature;
}

TOCBTrayStatusResponse COCBSimulator::GetTrayStatus()
{
  TOCBTrayStatusResponse TrayStatus;
  TrayStatus.MessageID = OCB_TRAY_STATUS;
  TrayStatus.CurrentTrayTemp = m_SetTrayTemperature.RequiredTrayTemperature;
  TrayStatus.SetTrayTemp = m_SetTrayTemperature.RequiredTrayTemperature;
  TrayStatus.ActiveMargine = m_SetTrayTemperature.ActiveMargine;
  return TrayStatus;
}

void COCBSimulator::SetPowerOnOff(TOCBSetPowerOnOffMessage& PowerStatus)
{
  m_PowerStatus = PowerStatus;
}

TOCBPowerStatusResponse COCBSimulator::GetPowerStatus()
{
  TOCBPowerStatusResponse PowerStatus;
  PowerStatus.MessageID = OCB_POWER_STATUS;
  PowerStatus.PowerOnOff = m_PowerStatus.PowerOnOff;
  return PowerStatus;
}


void COCBSimulator::SetUvLampsOnOff(TOCBUvLampOnOffMessage& UvLampsStatus)
{
  m_UvLampsStatus = UvLampsStatus;
}

TOCBUvLampStatusResponse COCBSimulator::GetUvLampsStatus()
{
  TOCBUvLampStatusResponse LampsStatus;
  LampsStatus.MessageID = OCB_UV_LAMPS_STATUS;
  LampsStatus.OnOff = m_UvLampsStatus.OnOff;
  LampsStatus.LeftOnOff = m_UvLampsStatus.OnOff;
  LampsStatus.RightOnOff = m_UvLampsStatus.OnOff;;
  return LampsStatus;
}

TOCBPurgeStatusResponse COCBSimulator::GetPurgeStatus()
{
  TOCBPurgeStatusResponse PurgeStatus;
  PurgeStatus.MessageID = OCB_PURGE_STATUS;
  PurgeStatus.PurgeStatus = 0;
  return PurgeStatus;
}                      

void COCBSimulator::SetAmbientTemperatureLevel(TOCBSetAmbientTemperatureParamsMessage& AmbientTemperature)
{
  m_AmbientTemperatureStatus = AmbientTemperature;
}

TOCBAmbientTemperatureStatusResponse COCBSimulator::GetAmbientTemperatureStatus()
{
  TOCBAmbientTemperatureStatusResponse AmbientTemperature;
  AmbientTemperature.MessageID = OCB_AMBIENT_TEMPERATURE_STATUS;
  AmbientTemperature.AmbientTemperature = m_AmbientTemperatureStatus.AmbientTemperature;
  AmbientTemperature.LowAmbientTemperature = m_AmbientTemperatureStatus.LowAmbientTemperature;
  AmbientTemperature.HighAmbientTemperature = m_AmbientTemperatureStatus.HighAmbientTemperature;
  return AmbientTemperature;
}

TOCBActuatorsStatusResponse COCBSimulator::GetActuatorsStatus()
{
  TOCBActuatorsStatusResponse ActuatorsStatus;
  ActuatorsStatus.MessageID = OCB_ACTUATOR_STATUS;
  ActuatorsStatus.ActuatorsArray[0] = 0;
  ActuatorsStatus.ActuatorsArray[1] = 0;
  ActuatorsStatus.ActuatorsArray[2] = 0;
  ActuatorsStatus.ActuatorsArray[3] = 0;
  return ActuatorsStatus;

}

TOCBInputsStatusResponse COCBSimulator::GetInputsStatus()
{
  TOCBInputsStatusResponse InputsStatus;
  InputsStatus.MessageID = OCB_INPUT_STATUS;
  InputsStatus.InputsArray[0] = 0;
  InputsStatus.InputsArray[1] = 0;
  return InputsStatus;
}
/*
void COCBSimulator::SetEolParams(TOCBSetEOLParamsMessage& EolParams)
{
  m_EolStatus = EolParams;
}
*/
/*
TOCBEolStatusResponse COCBSimulator::GetEolStatus()
{
  TOCBEolStatusResponse EolStatus;
  EolStatus.MessageID = OCB_EOL_STATUS;
  EolStatus.EolOnOff = m_EolOnOff.EOLOnOff;
  EolStatus.ModelEndLowLimit =  m_EolStatus.ModelEndLowLimit;
  EolStatus.ModelStartHighLimit =  m_EolStatus.ModelStartHighLimit;
  EolStatus.ModelDeltaOk =  m_EolStatus.ModelDeltaOk;
  EolStatus.ModelDeltaCritical =  m_EolStatus.ModelDeltaCritical;
  EolStatus.ModelSuspendCycles =  m_EolStatus.ModelSuspendCycles;
  EolStatus.SupportEndLowLimit =  m_EolStatus.SupportEndLowLimit;
  EolStatus.SupportStartHighLimit =  m_EolStatus.SupportStartHighLimit;
  EolStatus.SupportDeltaOk =  m_EolStatus.SupportDeltaOk;
  EolStatus.SupportDeltaCritical =  m_EolStatus.SupportDeltaCritical;
  EolStatus.SupportSuspendCycles =  m_EolStatus.SupportSuspendCycles;
  EolStatus.ModelStartReading = 0;
  EolStatus.ModelStopReading = 0;
  EolStatus.SupportStartReading = 0;
  EolStatus.SupportStopReading = 0;

  return EolStatus;
}
*/
void COCBSimulator::SetEolOnOff(TOCBSetEOLOnOffMessage& EolOnOff)
{
  m_EolOnOff = EolOnOff;
}

void COCBSimulator::SetHeadsFillingControlOnOff(TOCBHeadFillingControlOnOffMessage& HeadsFiilingControl)
{
  m_HeadsFillingControl = HeadsFiilingControl;
}

void COCBSimulator::SetHeadsFillingParams(TOCBSetHeadFillingParamsMessage& HeadsFillingStatus)
{
/*  if (HeadsFillingStatus.MaterialType == OCB_MODEL)
    m_ModelHeadsFillingStatus = HeadsFillingStatus;
  else if (HeadsFillingStatus.MaterialType == OCB_SUPPORT)
    m_SupportHeadsFillingStatus = HeadsFillingStatus;
*/
}

TOCBHeadFillingStatusResponse COCBSimulator::GetHeadsFillingStatus(BYTE MetrialType)
{
/*
  TOCBSetHeadFillingParamsMessage HeadsFillingParams;
  TOCBHeadFillingStatusResponse HeadsFillingStatus;

  if (MetrialType == OCB_MODEL)
    HeadsFillingParams = m_ModelHeadsFillingStatus;
  else if (MetrialType == OCB_SUPPORT)
    HeadsFillingParams = m_SupportHeadsFillingStatus;

  HeadsFillingStatus.MessageID = OCB_HEADS_FILLING_STATUS;
  HeadsFillingStatus.HeadFillingControlOnOff = m_HeadsFillingControl.HeadFillingControlOnOff;
  HeadsFillingStatus.MaterialType = HeadsFillingParams.MaterialType;
  HeadsFillingStatus.ThermistorFull = HeadsFillingParams.ThermistorFullLowThreshold;
  HeadsFillingStatus.ThermistorHalfFull = HeadsFillingParams.ThermistorFullHighThreshold;
  return HeadsFillingStatus;
*/
}

void COCBSimulator::SetDoorStatus( BYTE DoorRequest, BYTE DoorStatus)
{
  m_DoorStatus.MessageID = OCB_DOOR_STATUS;
  for(int i= 0 ; i < MAX_NUM_OF_DOORS; ++i)
  {
	m_DoorStatus.DoorStatus[i]= DoorStatus;
  }
}

TOCBDoorStatusResponse COCBSimulator::GetDoorStatus()
{
  return m_DoorStatus;
}






