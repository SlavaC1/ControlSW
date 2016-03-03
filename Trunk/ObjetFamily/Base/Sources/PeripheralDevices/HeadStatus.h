/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Head Status send message                                 *
 * Module Description: This class control the sending of status     * 
 *                     message to the OHDB.                          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 28/02/2002                                           *
 ********************************************************************/

#ifndef _HEAD_STATUS_H_
#define _HEAD_STATUS_H_

#include "QThreadUtils.h"
#include "QThread.h"
#include "OHDBProtocolClient.h"
#include "HeadFilling.h"
#include "HeadHeaters.h"
#include "Roller.h"
#include "Power.h"
#include "AmbientTemperature.h"
#include "HeadVacuum.h"

// Exception class for all the print control class
class EOHDBStatus : public EQException {
  public:
    EOHDBStatus(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


//-----------------------------
// Head Heater + Vacuum + Head temperature sent message ping
class CHeadStatusSender : public CQThread {
   private:
     CRollerBase *m_RollerPtr;
     CHeadFillingBase *m_HeadFillingPtr;
     CHeadHeatersBase *m_HeadHeatersPtr;
     CPowerBase *m_PowerPtr;
	 CAmbientTemperatureBase *m_AmbientPtr;
     CHeadVacuumBase *m_VacuumPtr;

     int m_ResetWarmCounter;
     int m_ResetColdCounter;

     bool m_ResetAckOk;
     bool m_ResetMessageSent;
     bool m_WrongMessageAck;
     bool m_DebugModeMessageSent;

     bool m_SWVersionSent;
     bool m_A2D_SWVersionSent;
     bool m_HWVersionSent;
     bool m_SWVersionAckOk;
     bool m_A2D_SWVersionAckOk;
     bool m_HWVersionAckOk;
     bool m_SWDebugModeAckOk;

     int m_OHDB_SW_ExternalVersion;
     int m_OHDB_SW_InternalVersion;
     int m_OHDB_A2D_SW_ExternalVersion;
     int m_OHDB_A2D_SW_InternalVersion;
     int m_OHDB_HW_Version;

	 int m_timeCounter;

     COHDBProtocolClient *m_OHDBClient;

     static void WakeUpNotification(int,PVOID,unsigned,TGenericCockie);
     void AckToOHDBNotification (int,int,int,TGenericCockie);
     static void ResetAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOHDBSWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOHDB_A2D_SWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOHDBHWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOHDBDebugMessageAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

     TQErrCode WrongMessageTest(void);
     static void WrongMessageTestAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

   protected:
     bool m_DebugMode;

     //Enter or exit debug mode message
     TQErrCode EnterOHDBModeMessage(bool DebugMode);

   public:
    // Constructor
    CHeadStatusSender(const QString& Name, 
                      CRollerBase *RollerPtr,           
                      CHeadFillingBase *HeadFillingPtr,
                      CHeadHeatersBase *HeadHeatersPtr,
                      CPowerBase *PowerPtr,
                      CAmbientTemperatureBase *AmbientPtr,
					  CHeadVacuumBase *VacuumPtr);
                      
    // Destructor
   virtual ~CHeadStatusSender(void);

   void ClearOHDBResetCount(void);
   void DisplayOHDBResetCounter(void);
   DEFINE_V_METHOD(CHeadStatusSender,TQErrCode,Reset);

   DEFINE_V_METHOD(CHeadStatusSender,TQErrCode,GetOHDBSWVersionMessage);
   DEFINE_V_METHOD(CHeadStatusSender,TQErrCode,GetOHDB_A2D_SWVersionMessage);
   DEFINE_V_METHOD(CHeadStatusSender,TQErrCode,GetOHDBHWVersionMessage);
   DEFINE_METHOD(CHeadStatusSender,TQErrCode,DisplayOHDBVersion);

   int GetOHDBSWExternalVersion(void);
   int GetOHDBSWInternalVersion(void);
   int GetOHDB_A2D_SWExternalVersion(void);
   int GetOHDB_A2D_SWInternalVersion(void);

   int GetOHDBHWVersion(void);

   //task procedure
   void Execute(void);
   void Cancel(void);

   //For debbugging
   bool m_CanSendIsHeadTemperatureOkMessage;

   DEFINE_METHOD(CHeadStatusSender,TQErrCode,EnterOHDBDebugMode);
   DEFINE_METHOD(CHeadStatusSender,TQErrCode,ExitOHDBDebugMode);
   DEFINE_METHOD(CHeadStatusSender,bool,GetOHDBDebugModeValue);
};        


class CHeadStatusSenderDummy : public CHeadStatusSender {
protected:
   TQErrCode EnterOHDBModeMessage(bool DebugMode);
public:
   // Constructor
   CHeadStatusSenderDummy(const QString& Name,              
                          CRollerBase *RollerPtr,           
                          CHeadFillingBase *HeadFillingPtr, 
                          CHeadHeatersBase *HeadHeatersPtr,
                          CPowerBase *PowerPtr,
                          CAmbientTemperatureBase *AmbientPtr,
						  CHeadVacuumBase *VacuumPtr);

   // Destructor
   virtual ~CHeadStatusSenderDummy(void);

   void Cancel(void);

   TQErrCode Reset(void);
   TQErrCode GetOHDBSWVersionMessage(void);
   TQErrCode GetOHDB_A2D_SWVersionMessage(void);
   TQErrCode GetOHDBHWVersionMessage(void);
};


#endif

