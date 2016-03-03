/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Head Status send message                                 *
 * Module Description: This class control the sending of status     * 
 *                     message to the OCB.                          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 5/05/2002                                            *
 ********************************************************************/

#ifndef _OCB_STATUS_H_
#define _OCB_STATUS_H_

#include "QThreadUtils.h"
#include "QThread.h"
#include "Purge.h"
#include "UvLamps.h"
#include "TrayHeater.h"
#include "Actuator.h"
#include "Door.h"
#include "Container.h"
#include "Power.h"
#include "HeadFilling.h"
#include "EvacuationAirFlow.h"

 
// Exception class for all the print control class
class EOCBStatus : public EQException {
  public:
    EOCBStatus(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
}; 
                           
//-----------------------------
class COCBStatusSender : public CQThread {
   private:
     CPurgeBase *m_PurgePtr;
     CUvLamps *m_UvLampsPtr;
     CActuatorBase *m_ActuatorPtr;
     CDoorBase *m_DoorPtr;
     CContainerBase *m_ContainerPtr;
     CPowerBase *m_PowerPtr;
	 CTrayBase *m_TrayPlacerPtr;
	 CEvacAirFlowBase *m_AirFlowPtr;	 
     CHeadFillingBase *m_HeadFillingPtr;
	 CContainerBase *m_Containers;
	 CTrayBase *m_TrayHeaterPtr;
     
     bool m_ResetAckOk;
     bool m_ResetMessageSent;
     bool m_SWVersionSent;
     bool m_A2D_SWVersionSent;
     bool m_HWVersionSent;
     bool m_DebugModeMessageSent;

     bool m_SWVersionAckOk;
     bool m_A2D_SWVersionAckOk;
     bool m_HWVersionAckOk;
     bool m_SWDebugModeAckOk;

     int m_OCB_SW_ExternalVersion;
     int m_OCB_SW_InternalVersion;
     int m_OCB_A2D_SW_ExternalVersion;
     int m_OCB_A2D_SW_InternalVersion;
	 int m_MSC_HW_Version[NUM_OF_MSC_CARDS];
	 int m_MSC_FW_Version[NUM_OF_MSC_CARDS];
	 int m_OCB_HW_Version;

	 int m_timeCounter;

     COCBProtocolClient *m_OCBClient;
     CAppParams    *m_ParamsMgr;
     
     static void WakeUpNotification(int,PVOID,unsigned,TGenericCockie);
     void AckToOCBNotification (int,int,int,TGenericCockie);
     static void ResetAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOCBSWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOCB_A2D_SWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOCBHWVersionAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);
     static void GetOCBDebugMessageAck(int TransactionID,PVOID Data,unsigned DataLength,TGenericCockie Cockie);

   protected:
     bool m_DebugMode;

     //Enter or exit debug mode message
     TQErrCode EnterOCBModeMessage(bool DebugMode);

   public:
    // Constructor
    COCBStatusSender(const QString& Name, 
                     CPurgeBase *PurgePtr,          
                     CUvLamps *UvLampsPtr,          
                     CActuatorBase *ActuatorPtr,    
                     CDoorBase *DoorPtr,            
                     CContainerBase *ContainerPtr,
                     CPowerBase *PowerPtr,          
					 CTrayBase *TrayPtr,
					 CEvacAirFlowBase *AirFlowPtr,					 
                     CHeadFillingBase *HeadFillingPtr,
					 CContainerBase *Containers,
					 CTrayBase *TrayHeater);
                      
    // Destructor
   virtual ~COCBStatusSender(void);

   //task procedure
   void Execute(void);
   void Cancel(void);

   DEFINE_V_METHOD(COCBStatusSender,TQErrCode,Reset);

   DEFINE_V_METHOD(COCBStatusSender,TQErrCode,GetOCBSWVersionMessage);
   DEFINE_V_METHOD(COCBStatusSender,TQErrCode,GetOCB_A2D_SWVersionMessage);
   DEFINE_V_METHOD(COCBStatusSender,TQErrCode,GetOCBHWVersionMessage);
   DEFINE_METHOD(COCBStatusSender,TQErrCode,DisplayOCBVersion);

   int GetOCBSWExternalVersion(void);
   int GetOCBSWInternalVersion(void);
   int GetOCB_A2D_SWExternalVersion(void);
   int GetOCB_A2D_SWInternalVersion(void);
   //Elad added , I2C HW+FW Version.
   int GetMSCHWVersion(int index);
   int GetMSCFWVersion(int index);
   int GetOCBHWVersion(void);

   DEFINE_METHOD(COCBStatusSender,TQErrCode,EnterOCBDebugMode);
   DEFINE_METHOD(COCBStatusSender,TQErrCode,ExitOCBDebugMode);
   DEFINE_METHOD(COCBStatusSender,bool,GetOCBDebugModeValue);
};




class COCBStatusSenderDummy : public COCBStatusSender {
protected:
   TQErrCode EnterOCBModeMessage(bool DebugMode);
public:
   // Constructor
   COCBStatusSenderDummy(const QString& Name,
                          CPurgeBase *PurgePtr,
                          CUvLamps *UvLampsPtr,
                          CActuatorBase *ActuatorPtr,
                          CDoorBase *DoorPtr,
                          CContainerBase *ContainerPtr,
                          CPowerBase *PowerPtr,
						  CTrayBase *TrayPtr,
						  CEvacAirFlowBase *AirFlowPtr,
                          CHeadFillingBase *HeadFillingPtr,
						  CContainerBase *Containers,
						  CTrayBase *TrayHeater);

   // Destructor
  virtual ~COCBStatusSenderDummy(void);

   void Cancel(void);

   TQErrCode Reset(void);
   TQErrCode GetOCBSWVersionMessage(void);
   TQErrCode GetOCBHWVersionMessage(void);
   TQErrCode GetOCB_A2D_SWVersionMessage(void);
};


#endif

