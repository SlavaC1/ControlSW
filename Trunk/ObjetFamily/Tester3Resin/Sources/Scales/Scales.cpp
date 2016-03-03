
#include "Scales.h"
#include "QMonitor.h"


// Scales commands
const char RESET_CMD = 'R';
const char WEIGH_CMD = 'S';
const char WEIGH_IMMEDIATEL_CMD = 'Q';
const int SCALES_COMMAND_LENGTH = 3;

const int MAX_LENGTH_OF_SCALE_MSG = 16;
const int ASCII_ACK = 0x06;
const int WEIGHT_INDEX = 3;
const int UNITS_INDEX = 14;
const int ERROR_NUM_INDEX = 4;

// Default constructor
CScales::CScales(void)
{
}

// Initialization constructor
CScales::CScales(int ComNum)
{
  m_ComNum = ComNum;
  m_LastWeight = 0;
  m_ScalesErrorNum = -1;
  Init(m_ComNum);
}


// Destructor
CScales::~CScales(void)
{

}

// init the communication with the scales
void CScales::Init(int ComNum)
{
  m_ComPort.Init(ComNum);

}

// Reset the scales (RE-ZERO scales command)
void CScales::Reset()
{
  char Cmd[] = {RESET_CMD,'\r','\n'};
  try
  {
    m_ComPort.Flush();
    if (m_ComPort.Write(Cmd, SCALES_COMMAND_LENGTH) != SCALES_COMMAND_LENGTH)
      throw EScales("Communication error");

    TScaleData RetVal = DecodeScalesData();
    if (RetVal != ACK)
      DisplayErrorMessage(RetVal);
  }
  catch (...)
  {
    QMonitor.ErrorMessage("Communication error with the scales");
  }
}

// requests the weighing data from the scales after it is stabilized
double CScales::Weigh()
{
  char Cmd[] = {WEIGH_CMD,'\r','\n'};
  char Data[16];
  try
  {
    m_ComPort.Flush();
    if (m_ComPort.Write(Cmd, SCALES_COMMAND_LENGTH) != SCALES_COMMAND_LENGTH)
      throw EScales("Communication error");

    TScaleData RetVal = DecodeScalesData();
    if (RetVal != STABLE_WEIGH_DATA)
      DisplayErrorMessage(RetVal);

  }
  catch (...)
  {
    QMonitor.ErrorMessage("Communication error with the scales");
  }
  return m_LastWeight;
}

// requests the weighing data from the scales immediately
double CScales::WeighImmediately()
{
  char Cmd[] = {WEIGH_IMMEDIATEL_CMD,'\r','\n'};
  char Data[16];
  try
  {
    m_ComPort.Flush();
    if (m_ComPort.Write(Cmd, SCALES_COMMAND_LENGTH) != SCALES_COMMAND_LENGTH)
      throw EScales("Communication error");

    TScaleData RetVal = DecodeScalesData();
    if (RetVal != STABLE_WEIGH_DATA && RetVal != UNSTABLE_WEIGH_DATA)
      DisplayErrorMessage(RetVal);

  }
  catch (...)
  {
    QMonitor.ErrorMessage("Communication error with the scales");
  }
  return m_LastWeight;
}

// Receives and decodes the data from the scales
CScales::TScaleData CScales::DecodeScalesData()
{
  char Data[MAX_LENGTH_OF_SCALE_MSG];

  if (m_ComPort.Read(Data,1) != 1)
    return DATA_ERROR;

  switch(Data[0])
  {
    // Stable data
    case 'S':
      GetWeightAndUnits();
      if (m_Units != 'g')
        return UNITS_ERROR;

      return STABLE_WEIGH_DATA;

    // Unstable data
    case 'U':
      GetWeightAndUnits();
      if (m_Units != 'g')
        return UNITS_ERROR;

      return UNSTABLE_WEIGH_DATA;

    // Overload
    case 'O':
      GetWeightAndUnits();
      return OVERLOAD;

    // stable data (counting mode)
    case 'Q':
      m_ComPort.Read(Data,15);
      return DATA_ERROR;

    // Ack
    case ASCII_ACK:
      return ACK;

    // Error code
    case 'E':
      if (m_ComPort.Read(Data,5) != 5)
        return DATA_ERROR;

      m_ScalesErrorNum = atoi(&Data[ERROR_NUM_INDEX-1]);
      return ERROR_CODE;

    default:
      return DATA_ERROR;
  }
}


// Get the weight and the units from the received data
bool CScales::GetWeightAndUnits()
{
  char Data[MAX_LENGTH_OF_SCALE_MSG];

  if (m_ComPort.Read(Data,15) != 15)
    return false;

  m_LastWeight = atof(&Data[WEIGHT_INDEX - 1]);
  m_Units = Data[UNITS_INDEX - 1];
  return true;

}

// Display an error message
void CScales::DisplayErrorMessage(TScaleData Error)
{
  QString Msg;

  switch(Error)
  {
    case UNSTABLE_WEIGH_DATA:
      Msg = "The data is unstable";
      break;

    case OVERLOAD:
      Msg = "Overload";
      break;

    case ERROR_CODE:
      Msg = "Scales error (Error code " + QIntToStr(m_ScalesErrorNum) + ")";
      break;

    case DATA_ERROR:
      Msg = "Data error";
      break;

    case UNITS_ERROR:
      Msg = "Units error";
      break;

  }
  QMonitor.ErrorMessage(Msg);
}
