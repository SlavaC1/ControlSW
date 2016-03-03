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


class CTesterBase : public CQComponent
{
  private:
  
  public:
    // Constructor
	CTesterBase(void);

    // Destructor
    virtual ~CTesterBase(void);

    // Read/Write PCI register
	DEFINE_V_METHOD_2(CTesterBase,ULONG,PCIReadDWORD,int /*BarNum*/,ULONG /*Addr*/)               = 0;
	DEFINE_V_METHOD_3(CTesterBase,int,PCIWriteDWORD,int /*BarNum*/,ULONG /*Addr*/,ULONG /*Data*/) = 0;
	DEFINE_V_METHOD_1(CTesterBase,int,FIFOSetReadMode,int /*ReadMode*/)                           = 0;
	DEFINE_V_METHOD_2(CTesterBase,int,WriteDataToOHDBXilinx,BYTE /*Address*/,WORD /*Data*/)       = 0;
	DEFINE_V_METHOD_1(CTesterBase,WORD,ReadDataFromOHDBXilinx,BYTE /*Address*/)                   = 0;
	DEFINE_V_METHOD_2(CTesterBase,int,WriteDataToLotus,WORD /*Address*/,int /*Data*/)             = 0;
	DEFINE_V_METHOD_1(CTesterBase,BYTE,ReadDataFromLotus,WORD /*Address*/)                        = 0;
};

class CTester : public CTesterBase
{
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

class CTesterDummy : public CTesterBase
{
  private:

  public:
    // Constructor
	CTesterDummy(void);

    // Destructor
    ~CTesterDummy(void);

    // Read/Write PCI register
	DEFINE_METHOD_2(CTesterDummy,ULONG,PCIReadDWORD,int /*BarNum*/,ULONG /*Addr*/);
	DEFINE_METHOD_3(CTesterDummy,int,PCIWriteDWORD,int /*BarNum*/,ULONG /*Addr*/,ULONG /*Data*/);
	DEFINE_METHOD_1(CTesterDummy,int,FIFOSetReadMode,int /*ReadMode*/);

	DEFINE_METHOD_2(CTesterDummy,int,WriteDataToOHDBXilinx,BYTE /*Address*/,WORD /*Data*/);
	DEFINE_METHOD_1(CTesterDummy,WORD,ReadDataFromOHDBXilinx,BYTE /*Address*/);

	DEFINE_METHOD_2(CTesterDummy,int,WriteDataToLotus,WORD /*Address*/,int /*Data*/);
	DEFINE_METHOD_1(CTesterDummy,BYTE,ReadDataFromLotus,WORD /*Address*/);
};

#endif

