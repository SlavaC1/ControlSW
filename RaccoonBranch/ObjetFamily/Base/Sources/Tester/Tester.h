/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Tester.                                                  *
 * Module Description: Tester class.                                *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 03/09/2001                                           *
 * Last upate: 30/01/2002                                           *
 ********************************************************************/

#ifndef _TESTER_H_
#define _TESTER_H_

#include "QComponent.h"
#include "QEvent.h"


// Exception class for the CTester class
class ETester : public EQException {
  public:
    ETester(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

class CTester : public CQComponent {
  private:
    WORD m_XilinxData;
    BYTE m_XilinxAddress;

    bool m_ReadXilinxAckOk;
    bool m_WriteXilinxAckOk;

    static void StatusHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie);

    static void AckHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie);    

  public:
    // Constructor
    CTester(void);

    // Destructor
    ~CTester(void);

    // Read/Write PCI register
    DEFINE_METHOD_2(CTester,ULONG,PCIReadDWORD,int /*BarNum*/,ULONG /*Addr*/);
    DEFINE_METHOD_3(CTester,int,PCIWriteDWORD,int /*BarNum*/,ULONG /*Addr*/,ULONG /*Data*/);
    DEFINE_METHOD_1(CTester,int,FIFOSetReadMode,int /*ReadMode*/);

    DEFINE_METHOD_2(CTester,int,WriteDataToOHDBXilinx,BYTE /*Address*/,WORD /*Data*/);
    DEFINE_METHOD_1(CTester,WORD,ReadDataFromOHDBXilinx,BYTE /*Address*/);   

	DEFINE_METHOD_2(CTester,int,WriteDataToLotus,WORD /*Address*/,int /*Data*/);
	DEFINE_METHOD_1(CTester,BYTE,ReadDataFromLotus,WORD /*Address*/);

};

#endif

