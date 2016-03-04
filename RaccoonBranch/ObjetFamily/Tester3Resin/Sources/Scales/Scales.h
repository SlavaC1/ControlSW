

#ifndef _SCALES_H_
#define _SCALES_H_

#include "QException.h"
#include "QStdComPort.h"


// Exception class for CScales class
class EScales : public EQException {
  public:
    EScales(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};



class CScales {
  private:

  CQStdComPort m_ComPort;
  int m_ComNum;
  double m_LastWeight;
  int m_ScalesErrorNum;
  char m_Units;

  typedef enum {STABLE_WEIGH_DATA, UNSTABLE_WEIGH_DATA, OVERLOAD, ACK, ERROR_CODE, DATA_ERROR, UNITS_ERROR} TScaleData;

  // Receives and decodes the data from the scales
  TScaleData DecodeScalesData();

  // Get the weight and the units from the received data
  bool GetWeightAndUnits();

  // Display an error message
  void DisplayErrorMessage(TScaleData Error);


  public:
    // Default constructor
    CScales(void);

    // Initialization constructor
    CScales(int ComNum);

    // Destructor
    ~CScales(void);

    // Implementation functions
    // ------------------------
    // init the communication with the scales
    void Init(int ComNum);

    // Reset the scales (RE-ZERO scales command)
    void Reset();

    // requests the weighing data from the scales after it is stabilized
    double Weigh();

    // requests the weighing data from the scales immediately
    double WeighImmediately();

};

#endif

