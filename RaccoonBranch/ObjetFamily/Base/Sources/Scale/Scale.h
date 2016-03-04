

#ifndef _HSW_SCALES_H_
#define _HSW_SCALES_H_

#include "BaseScaleInterface.h"
#include "QThreadUtils.h"

class CScales :public CBaseScales{
  private:
  typedef enum {ACEPTED_AND_INPROGRESS,
				COMPLETED,
				ZERO_RANGE,
				TIMEOUT_FOR_STABLE_RESULT,
				CAN_NOT_EXECUTE,
				TARE_RANGE_OVERFLOW,
				READ_AGAIN,
				DATA_ERROR} TScaleData;
 TScaleData m_currentStatus;
  public:

    // Initialization constructor
	CScales(int ComNum):CBaseScales(ComNum)
	{
	  m_currentStatus = COMPLETED;
    }
	void SetCurrentStatus(TScaleData status)
	{
       m_currentStatus = status;
	}
	TScaleData GetCurrentStatus()
	{
		return  m_currentStatus;
    }
    // Destructor
	virtual ~CScales(void);
	// check scale communication
	virtual void CheckCommunication(QString &errMsg);
	// Reset the scales (RE-ZERO scales command)
	virtual void Reset(QString &errMsg) ;
	void Tarring(QString &errMsg);
	// requests the weighing data from the scales after it is stabilized
	virtual float GetWeight(QString &errMsg);
    virtual float GetImmWeight(QString &errMsg);
	virtual void DisplayErrorMessage(int Error,QString &errMsg);

	// Receives and decodes the data from the scales
	float AnalizeScaleData(const char cmd);
	virtual void ReadMsgFromPort(std::vector<char> &msgFromPort) ;
};
class CScalesDummy:public CBaseScales{
  private:
  typedef enum {ACEPTED_AND_INPROGRESS,
				COMPLETED,
				ZERO_RANGE,
				TIMEOUT_FOR_STABLE_RESULT,
				CAN_NOT_EXECUTE,
				TARE_RANGE_OVERFLOW,
				READ_AGAIN,
				DATA_ERROR} TScaleData;
 TScaleData m_currentStatus;
  public:

    // Initialization constructor
	CScalesDummy(int ComNum):CBaseScales(-1)
	{
	  m_currentStatus = COMPLETED;
    }
	void SetCurrentStatus(TScaleData status)
	{
       m_currentStatus = status;
	}
	TScaleData GetCurrentStatus()
	{
		return  m_currentStatus;
    }
    // Destructor
	virtual ~CScalesDummy(void)
	{

    }
	// check scale communication
	virtual void CheckCommunication(QString &errMsg)
	{
	  errMsg = "";
    }
	// Reset the scales (RE-ZERO scales command)
	virtual void Reset(QString &errMsg)
	{
	 errMsg = "";
    }
	void Tarring(QString &errMsg)
	{
		errMsg = "";
	}
	// requests the weighing data from the scales after it is stabilized
	virtual float GetWeight(QString &errMsg)
	{
		errMsg = "";
		return 1.5;
    }
	virtual float GetImmWeight(QString &errMsg)
	{
		errMsg = "";
		return 1.5;
    }
	virtual void DisplayErrorMessage(int Error,QString &errMsg)
	{

    }

	// Receives and decodes the data from the scales
	float AnalizeScaleData(const char cmd)
	{
		return 0.5;
    }
	virtual void ReadMsgFromPort(std::vector<char> &msgFromPort)
	{

    }
};
#endif

