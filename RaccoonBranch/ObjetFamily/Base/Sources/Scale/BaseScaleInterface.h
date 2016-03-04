

#ifndef _HSW_BASE_SCALES_H_
#define _HSW_BASE_SCALES_H_

#include "QException.h"
#include "QStdComPort.h"
#include "QMonitor.h"

// Exception class for CScales class
class EScales : public EQException {
  public:
	EScales(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};



class CBaseScales : public CQComponent
{
  private:
	CQStdComPort m_ComPort;
	int m_ComNum;
  // Receives and decodes the data from the scales
  public:

	// Initialization constructor
	CBaseScales(int ComNum);

	// Destructor
	virtual ~CBaseScales(void);

	// Implementation functions
	// ------------------------
	// init the communication with the scales
	void Init(int ComNum);
	void DeInit();
	void Write(char* Cmd,int size);
	int Read (char* msg, int size);

	// Reset the scales (RE-ZERO scales command)
	virtual void Reset(QString &errMsg) = 0;
	virtual void CheckCommunication(QString &errMsg) = 0;
	virtual void DisplayErrorMessage(int Error,QString &errMsg) = 0;
	virtual void ReadMsgFromPort(std::vector<char> &msgFromPort) = 0;
	virtual float GetWeight(QString &errMsg) = 0;
	virtual float GetImmWeight(QString &errMsg) = 0;
};

#endif
