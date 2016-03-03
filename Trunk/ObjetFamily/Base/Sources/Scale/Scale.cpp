
#include "Scale.h"



// Scales commands
const char RESET_CMD = 'Z'; /*set zero weight, taring*/
const char TARRING_CMD = 'T'; /*set zero weight, taring*/
const char STABLE_WEIGHT_CMD = 'S'; /*return the weight in basic unit */
const char IMEDIATELY_WEIGHT_CMD = 'I';
const int SCALES_COMMAND_LENGTH = 10;

/*-----------------------------------------------------------------------------*/
// Destructor
CScales::~CScales(void)
{

}
void CScales::CheckCommunication(QString &errMsg)
{
	// send some command for checking communication
   //	Reset(errMsg);
   GetImmWeight(errMsg);
}
/*-----------------------------------------------------------------------------*/
// Reset the scales (RE-ZERO scales command)
void CScales::Reset(QString &errMsg)
{

  try
  {
	char Cmd[] = {RESET_CMD,'\r','\n'};
	Write(Cmd, SCALES_COMMAND_LENGTH);
	TScaleData status;
	AnalizeScaleData(RESET_CMD);
	status = GetCurrentStatus();
	if(status == READ_AGAIN)
	{
		AnalizeScaleData(RESET_CMD);
		status = GetCurrentStatus();
	}

	if (status != COMPLETED)
	{
	  if(status == ZERO_RANGE)
	  {
        QSleep(1000);
		Tarring(errMsg);
	  }
	  else
		DisplayErrorMessage(status,errMsg);
	}
  }
  catch (...)
  {
	errMsg = "Communication error with the scales";
  }
}
void CScales::Tarring(QString &errMsg)
{

  try
  {
	char Cmd[] = {TARRING_CMD,'\r','\n'};
	Write(Cmd, TARRING_CMD);
	TScaleData status;
	AnalizeScaleData(TARRING_CMD);
	status = GetCurrentStatus();
	if(status == READ_AGAIN)
	{
		AnalizeScaleData(TARRING_CMD);
		status = GetCurrentStatus();
	}
	if (status != COMPLETED)
	{
	  if(status ==TARE_RANGE_OVERFLOW)
	  {
        QSleep(1000);
		Reset(errMsg);
	  }
	  else
		DisplayErrorMessage(status,errMsg);
	}
  }
  catch (...)
  {
  //	QMonitor.ErrorMessage("Communication error with the scales");
   errMsg = "Communication error with the scales";
  }
}
/*-----------------------------------------------------------------------------*/
// Return stabilized weight in basic unit
float CScales::GetWeight(QString &errMsg)
{

  float weight = 0;
  try
  {
   char Cmd[] = {STABLE_WEIGHT_CMD,'\r','\n'};
	Write(Cmd, SCALES_COMMAND_LENGTH);
	TScaleData status;
	weight = AnalizeScaleData(STABLE_WEIGHT_CMD);
	 status = GetCurrentStatus();
	int count = 0;
	while(status == READ_AGAIN && count <3)
	{
		weight = AnalizeScaleData(STABLE_WEIGHT_CMD);
		status = GetCurrentStatus();
		count ++ ;
	}

	if ( status != COMPLETED)
	  DisplayErrorMessage(status,errMsg);
  }
  catch (...)
  {
   //	QMonitor.ErrorMessage("Communication error with the scales");
	errMsg = "Communication error with the scales";
  }
  return weight;
}
/*-----------------------------------------------------------------------------*/
// Return stabilized weight in basic unit
float CScales::GetImmWeight(QString &errMsg)
{

  float weight = 0.0;
  try
  {
  char Cmd[] = {STABLE_WEIGHT_CMD,IMEDIATELY_WEIGHT_CMD,'\r','\n'};
	Write(Cmd, SCALES_COMMAND_LENGTH);
	TScaleData status;
	weight = AnalizeScaleData(STABLE_WEIGHT_CMD);
	 status = GetCurrentStatus();
	if(status == DATA_ERROR)
	{
		QSleep(1000);
		Write(Cmd, SCALES_COMMAND_LENGTH);
        weight = AnalizeScaleData(STABLE_WEIGHT_CMD);
	    status = GetCurrentStatus();
	}
	int count = 0;
	while(status == READ_AGAIN && count <3)
	{
		weight = AnalizeScaleData(STABLE_WEIGHT_CMD);
		status = GetCurrentStatus();
		count ++;
	}

	if ( status != COMPLETED)
	  DisplayErrorMessage(status,errMsg);
  }
  catch (...)
  {
   //	QMonitor.ErrorMessage("Communication error with the scales");
	errMsg = "Communication error with the scales";
  }
  return weight;
}
/*-----------------------------------------------------------------------------*/
// Analize Scale Data
float CScales::AnalizeScaleData(const char orgMsg)
{
 
  float weight = 0;
  std::vector<char> data;
  ReadMsgFromPort(data);
  TScaleData status = GetCurrentStatus();
  if(status == DATA_ERROR)
  {
	return weight;
  }
 int index = 0;
 for(std::vector<char>::iterator i = data.begin(); i < data.end(); ++i)
 {
  if((*i =='E') && (index ==0))
  {
    SetCurrentStatus(READ_AGAIN);
	 return weight;
  }
  if((*i!= orgMsg) && (index ==0))
  {
   SetCurrentStatus(DATA_ERROR);
	return weight;
  }
  if((*i!= ' ') && (*i!= 'I') && (index ==1))
  {
	SetCurrentStatus(DATA_ERROR);
	return weight;
  }
  if(index ==2)
  {
	if(*i == 'A')
	  {
		SetCurrentStatus(READ_AGAIN);
		return weight;
	  }
	else
	if(*i == 'D')
	 {
		SetCurrentStatus(COMPLETED);
		return weight;
	 }
	else
	 if(*i == '^')
	 {
	  SetCurrentStatus(ZERO_RANGE);
		return weight;
	 }
	 else
	  if(*i == 'I')
	  {
	   SetCurrentStatus(CAN_NOT_EXECUTE);
		return weight;
	  }
	  else
		if(*i == 'E')
		{
		 SetCurrentStatus(TIMEOUT_FOR_STABLE_RESULT );
		 return weight;
		}
	  else
		if(*i == 'v')
		 {
		 SetCurrentStatus(TARE_RANGE_OVERFLOW);
		 return weight;
		 }
  }
  if(index==6)
  {
	QString weightStr = "";
	 while(index!=15)
	 {
	 if(i >= data.end())
	 	break;
	 if((*i != '[')&&(*i != ']'))
		weightStr += (*i);
	 index++;
	 ++i;
	 }
	 //weight = QStrToFloat(weightStr.c_str());
	 weight = atof(weightStr.c_str());
	 SetCurrentStatus(COMPLETED);
	 return weight;
  }
 index ++; 
 }
 return weight;
}
/*-----------------------------------------------------------------------------*/
void CScales::ReadMsgFromPort(std::vector<char> &msgFromPort)
{
  char data[2];
  bool cont = true;
  QSleep(1000);
  while(cont == true)
  {
	 if (Read(data,1) != 1)
	 {
		SetCurrentStatus(DATA_ERROR);
		return ;
	 }
	 if(data[0] == '\n')
		cont = false;
	 else
		msgFromPort.push_back(data[0]);
  }
 SetCurrentStatus(COMPLETED);
}
/*-----------------------------------------------------------------------------*/
// Display an error message
void CScales::DisplayErrorMessage(int Error,QString &errMsg)
{
  switch((TScaleData)Error)
  {
	case ACEPTED_AND_INPROGRESS :
	  errMsg = "Command is eccepted and in progress";
	  break;
	case COMPLETED :
	  errMsg = "Command is completed";
	  break;
	case ZERO_RANGE :
	  errMsg = "Zero range";
	  break;
	case TIMEOUT_FOR_STABLE_RESULT :
	  errMsg = "Timeout to stabilise";
	  break;
	case CAN_NOT_EXECUTE :
		errMsg = "Problem to execute the command";
		break;
	case TARE_RANGE_OVERFLOW:
		errMsg = "Tare range is overflow";
		break;
	case DATA_ERROR:
		 errMsg = "Command is failed , data error";
		break;
	default:
		errMsg = "Some problem";
		break;
  }
}