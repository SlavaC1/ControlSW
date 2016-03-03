#ifndef OCB_SIMULATOR_H
#define OCB_SIMULATOR_H


#include "OCBCommDefs.h"

const int OCB_MODEL   = 0;
const int OCB_SUPPORT = 1;
const int OCB_DOOR_UNLOCK = 0;
const int OCB_DOOR_LOCK = 1;
const int OCB_DOOR_OPEN_UNLOCKED = 0;
const int OCB_DOOR_CLOSE_UNLOCKED = 1;
const int OCB_DOOR_OPEN_LOCKED = 2;
const int OCB_DOOR_CLOSE_LOCKED = 3;

class COCBSimulator
{                      
  private:
    TOCBSetTrayTemperatureMessage m_SetTrayTemperature;
    TOCBSetPowerOnOffMessage m_PowerStatus;
    TOCBUvLampOnOffMessage m_UvLampsStatus;
    TOCBSetAmbientTemperatureParamsMessage m_AmbientTemperatureStatus;
//    TOCBSetEOLParamsMessage m_EolStatus;
    TOCBSetEOLOnOffMessage m_EolOnOff;
    TOCBHeadFillingControlOnOffMessage m_HeadsFillingControl;
    TOCBSetHeadFillingParamsMessage m_ModelHeadsFillingStatus;
    TOCBSetHeadFillingParamsMessage m_SupportHeadsFillingStatus;
    TOCBDoorStatusResponse m_DoorStatus;

  public:
    void SetTrayTemperature(TOCBSetTrayTemperatureMessage& SetTrayTemperature);
    TOCBTrayStatusResponse GetTrayStatus();

    void SetPowerOnOff(TOCBSetPowerOnOffMessage& PowerStatus);
    TOCBPowerStatusResponse GetPowerStatus();

    void SetUvLampsOnOff(TOCBUvLampOnOffMessage& UvLampsStatus);
    TOCBUvLampStatusResponse GetUvLampsStatus();

    TOCBPurgeStatusResponse GetPurgeStatus();

    void SetAmbientTemperatureLevel(TOCBSetAmbientTemperatureParamsMessage& AmbientTemperature);
    TOCBAmbientTemperatureStatusResponse GetAmbientTemperatureStatus();

    TOCBActuatorsStatusResponse GetActuatorsStatus();

    TOCBInputsStatusResponse GetInputsStatus();

//    void SetEolParams(TOCBSetEOLParamsMessage& EolParams);
//    TOCBEolStatusResponse GetEolStatus();

    void SetEolOnOff(TOCBSetEOLOnOffMessage& EolOnOff);

    void SetHeadsFillingControlOnOff(TOCBHeadFillingControlOnOffMessage& HeadsFiilingControl);

    void SetHeadsFillingParams(TOCBSetHeadFillingParamsMessage& HeadsFillingStatus);
    TOCBHeadFillingStatusResponse GetHeadsFillingStatus(BYTE MetrialType);

    void SetDoorStatus( BYTE DoorRequest, BYTE DoorStatus);
    TOCBDoorStatusResponse GetDoorStatus();

};

#endif
