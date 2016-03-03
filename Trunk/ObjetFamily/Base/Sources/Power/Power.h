/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Power ON 8051 h/w                                        *
 * Module Description: This class implement services related to the *
 *                     power on 0\8051 h/w.                         *
 *                                                                  *
 * Compilation: Standard gedaleluia C++.                            *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 16/09/2001                                           *
 ********************************************************************/
 /* TBD -
 */

#ifndef _POWER_H_
#define _POWER_H_

#include "OCBProtocolClient.h"
#include "OCBCommDefs.h"
#include "QTimer.h"
#include "QMessageQueue.h"
#include "ErrorHandler.h"

const bool POWER_ON  = true;
const bool POWER_OFF = false;

const int OCB_POWER_ON_TIMEOUT_IN_SEC = 2;
//Elad added -4 OHDB/OCB power suppliers
const float   VS_MIN_VALUE  = 7.6;//OCB
const float   VS_MAX_VALUE  = 8.4;//OCB
const float  VCC_MIN_VALUE  = 4.75;//OCB+OHDB
const float  VCC_MAX_VALUE  = 5.25;//OCB+OHDB
const float _12V_MIN_VALUE  = 11.4;//OCB
const float  VDD_MIN_VALUE  = 11.4;//OHDB
const float _12V_MAX_VALUE  = 12.6;//OCB
const float  VDD_MAX_VALUE  = 12.6;//OHDB
const float _24V_MIN_VALUE  = 22.8;//OCB
const float V24_MIN_VALUE   = 22.8;//OHDB
const float _24V_MAX_VALUE  = 25.2;//OCB
const float  V24_MAX_VALUE  = 25.2;//OHDB
const float V_3_3_MAX_VALUE = 3.63;
const float V_3_3_MIN_VALUE = 2.97;
const float V_1_2_MAX_VALUE = 1.32;
const float V_1_2_MIN_VALUE = 1.08;
#if defined(OBJET_1000)
	const float VPP_MAX_VALUE  = 43.2;  //OHDB
	const float VPP_MIN_VALUE  = 36.8;//OHDB
#else
	const float VPP_MAX_VALUE  = 41.5;  //OHDB
	const float VPP_MIN_VALUE  = 37.5;//OHDB
#endif
const float MSC_24V_MAX_VALUE  = 26.4;//Cordilia
const float MSC_24V_MIN_VALUE  = 21.6;//Cordilia
const float MSC_7V_MAX_VALUE   = 7.7; //Cordilia
const float MSC_7V_MIN_VALUE   = 6.3; //Cordilia
const float MSC_5V_MAX_VALUE   = 5.5; //Cordilia
const float MSC_5V_MIN_VALUE   = 4.5; //Cordilia
const float MSC_3_3V_MAX_VALUE = 3.6; //Cordilia
const float MSC_3_3V_MIN_VALUE = 2.9; //Cordilia

const float CONVERT_VS_A2D_TO_VOLT  = 5.75 * 2.4 / 1024.0;
const float CONVERT_VCC_A2D_TO_VOLT = 5.75 * 2.4  / 1024.0;
const float CONVERT_12V_A2D_TO_VOLT = 5.75 * 2.4  / 1024.0;
const float CONVERT_24V_A2D_TO_VOLT = 11.0 * 2.4  / 1024.0;

const float CONVERT_MSC_24V_A2D_TO_VOLT  =       8.0/1000.0;
const float CONVERT_MSC_7V_A2D_TO_VOLT   =    2.3333/1000.0;
const float CONVERT_MSC_5V_A2D_TO_VOLT   = 1.6666667/1000.0;
const float CONVERT_MSC_3_3V_A2D_TO_VOLT =       1.1/1000.0;

typedef enum {
	VDD_POWER_SUPPLY = 0,
	VPP_POWER_SUPPLY,
        HEATER_24_POWER_SUPPLY,
        VCC_POWER_SUPPLY,
        A2D_3_3_POWER_SUPPLY,
        A2D_1_2_POWER_SUPPLY,
        NUMBER_OF_POWER_SUPPLY
} TPowerSupply;

const float MinPowerSupplyValue[NUMBER_OF_POWER_SUPPLY] =
{
   VDD_MIN_VALUE
  ,VPP_MIN_VALUE
  ,V24_MIN_VALUE
  ,VCC_MIN_VALUE
  ,V_3_3_MIN_VALUE
  ,V_1_2_MIN_VALUE
};

const float MaxPowerSupplyValue[NUMBER_OF_POWER_SUPPLY] =
{
   VDD_MAX_VALUE
  ,VPP_MAX_VALUE
  ,V24_MAX_VALUE
  ,VCC_MAX_VALUE
  ,V_3_3_MAX_VALUE
  ,V_1_2_MAX_VALUE
};
//Define the exact cell of the voltages status within the  sampling  buffer that embedded recieves from OCB.
enum TMscPowerReadings
{
	mscReading_24V,
	mscReading_7V,
	mscReading_5V,
	mscReading_3_3V,
	mscReadingsNum
};

typedef unsigned int UINT;

// Exception class for all the QLib RTTI elements
class EPower : public EQException {
  public:
    EPower(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// C Power control class
class CPowerBase : public CQComponent{
  protected:
    COCBProtocolClient *m_OCBClient;

    DEFINE_VAR_PROPERTY(bool,PowerOnOff);
    DEFINE_VAR_PROPERTY(int,OnDelayParameter);
    DEFINE_VAR_PROPERTY(int,OffDelayParameter);
    
    // Flags to control the waiting and cancel operation
    bool m_Waiting;

    // Protection mutex for database
    CQMutex m_MutexDataBase;
    
    // Flag that permit to start send get status message each period time
    bool m_FlagCanSendGetStatusMsg;

   CErrorHandler *m_ErrorHandlerClient;

  public:
    // Constructor
    CPowerBase(const QString& Name);

    // Destructor
    ~CPowerBase(void);

    // Command to turn on/off the 8051 h/w.
    DEFINE_V_METHOD_1(CPowerBase,TQErrCode,SetPowerOnOff,bool) = 0;
                                                
    //This procedure is called in order to wait for 8051 to be turned on.
    DEFINE_V_METHOD(CPowerBase,TQErrCode,WaitForPowerOnOff) = 0;
    
    //This procedure send Get status message to OCB(8051).
    DEFINE_V_METHOD(CPowerBase,TQErrCode,GetStatus)=0;

	//This procedure send Get Power Suppliers status message to OCB(8051).Added by Elad
	DEFINE_V_METHOD(CPowerBase,TQErrCode,GetPowerSuppliersStatus)=0;

    //This procedure send Set parameters message to OCB(8051).
    DEFINE_V_METHOD_2(CPowerBase,TQErrCode,SetParms,int,int)=0;

    //This procedure send Set parameters message to OCB(8051).
    DEFINE_METHOD(CPowerBase,TQErrCode,SetDefaultParms);

    //This procedure update the value of power on off of OCB(8051).
    DEFINE_METHOD_1(CPowerBase,TQErrCode,UpdatePowerOnOffValue,bool);

    //This procedure get the value of power on off of OCB(8051).
    DEFINE_METHOD(CPowerBase,bool,GetPowerOnOffValue);

    //Test procedure
    DEFINE_METHOD(CPowerBase,TQErrCode,Test);

    virtual void Cancel(void)=0;
};


class CPower : public CPowerBase {

 private:
   bool m_CancelFlag;

   bool m_notificationRcvdMSCArr[NUM_OF_MSC_CARDS];

   // Protection mutex for the cancel flag
   CQMutex m_CancelFlagMutex;

   //Flag Power during command performance.
   bool m_FlagPowerOnOffUnderUse;
   bool m_FlagSetCommandUnderUse;
   bool m_FlagGetStatusUnderUse;
   bool m_FlagGetPowerSuppliersStatusUnderUse;

   // Semaphore That verify if Notify response was received.
   CQEvent m_SyncEventPoweredOnOff;

   //Flag ack ok
   bool OnOff_AckOk;
   bool SetParm_AckOk;

   //This procedure is the callback for Set Power OnOff OCB
   static void OnOffAckResponse (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   //This procedure is the callback for Set params message
   static void SetParmsAckResponse(int TransactionId,PVOID Data,
                                   unsigned DataLength,TGenericCockie Cockie);

   //Get status ack response
   static void GetStatusAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   //Ack for 4 suppliers status request from OCB-Added by Elad
   static void GetPowerSuppliersStatusAckResponse(int TransactionId,PVOID Data,
                                    unsigned DataLength,TGenericCockie Cockie);

   // Notification Call Backs
   static void NotificationPowerOnOff(int TransactionId,PVOID Data,
                                      unsigned DataLength,TGenericCockie Cockie);

   static void NotificationErrorMessage (int TransactionId,PVOID Data,
                                 unsigned DataLength,TGenericCockie Cockie);

   // Replay Ack to OCb
   void AckToOcbNotification (int MessageID,
                              int TransactionID,
                              int AckStatus,
                              TGenericCockie Cockie);

   // Tray Heater recovery procedure.
   void Recovery (void);

public:
    // Constructor
    CPower(const QString& Name);

    // Destructor
    ~CPower(void);
       
    // Command to turn on/off the 8051 h/w.
    TQErrCode SetPowerOnOff (bool);

    //This procedure is called in order to wait for 8051 to be turned on.
    TQErrCode WaitForPowerOnOff(void);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetStatus(void);

	//This procedure send Get status message to OCB(8051).Added by Elad.
	TQErrCode GetPowerSuppliersStatus(void);

	inline TQErrCode ConverteNCheckA2DPowerSuppliersValue(float &,float &,float &,float &);

	inline TQErrCode ConverteNCheckA2DMSCSuppliersValue(float &,float &,float &,float &,BYTE);

    //This procedure send Set parameters message to OCB(8051).
    TQErrCode SetParms (int,int);

    void Cancel(void);
};


class CPowerDummy : public CPowerBase {
public:
    // Constructor
    CPowerDummy(const QString& Name);

    // Destructor
    ~CPowerDummy(void);

        // Command to turn on/off the 8051 h/w.
    TQErrCode SetPowerOnOff (bool);
          
    //This procedure is called in order to wait for 8051 to be turned on.
    TQErrCode WaitForPowerOnOff (void);

    //This procedure send Get status message to OCB(8051).
    TQErrCode GetStatus(void);

    //This procedure send Set parameters message to OCB(8051).
    TQErrCode SetParms (int,int);

    //This procedure send Get status message to OCB(8051).Added by Elad.
	TQErrCode GetPowerSuppliersStatus(void);

    void Cancel(void){}
};


#endif

