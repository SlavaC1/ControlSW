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

#include "Tester.h"
#include "EdenPCI.h"
#include "EdenPCISys.h"
#include "OHDBProtocolClient.h"
#include "OHDBCommDefs.h"
#include "OCBCommDefs.h"
#include "AppLogFile.h"
#include "FrontEnd.h"

CTesterBase::CTesterBase() : CQComponent("Tester")
{
}

CTesterBase::~CTesterBase()
{
}

// Constructor
CTester::CTester(void)
{
    m_XilinxData = 0 ;
    m_XilinxAddress = 0;
    m_ReadXilinxAckOk = false;
    m_WriteXilinxAckOk = false;
    INIT_METHOD(CTester,PCIReadDWORD);
    INIT_METHOD(CTester,PCIWriteDWORD);
    INIT_METHOD(CTester,FIFOSetReadMode);
    INIT_METHOD(CTester,WriteDataToOHDBXilinx);
    INIT_METHOD(CTester,ReadDataFromOHDBXilinx);
}

// Destructor
CTester::~CTester(void)
{
}

// Read/Write PCI register
ULONG CTester::PCIReadDWORD(int BarNum,ULONG Addr)
{
    return EdenPCI_ReadDWORD(BarNum,Addr);
}

int CTester::PCIWriteDWORD(int BarNum,ULONG Addr,ULONG Data)
{
    EdenPCI_WriteDWORD(BarNum,Addr,Data);
    return 0;
}

int CTester::FIFOSetReadMode(int ReadMode)
{
    EdenPCI_FIFOSetReadMode(ReadMode);
    return 0;
}


int CTester::WriteDataToOHDBXilinx(BYTE Address,WORD Data)
{
    TOHDBXilinxWriteMessage Msg;
    Msg.MessageID = OHDB_WRITE_DATA_TO_XILINX;
    Msg.Address = Address;
    Msg.Data = Data;

    COHDBProtocolClient *ProtocolClient = COHDBProtocolClient::Instance();

    m_WriteXilinxAckOk = false;

    if (ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBXilinxWriteMessage),
            AckHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"WriteDataToOHDBXilinx\" message");
        throw ETester("OHDB didn't get ack for \"WriteDataToOHDBXilinx\" message");
    }

    // Check if we really got ack
    if(!m_WriteXilinxAckOk)
        throw ETester("Tester:\"WriteDataToOHDBXilinx\" Invalid reply from OHDB");

    return 0;
}

void CTester::AckHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CTester *InstancePtr = reinterpret_cast<CTester *>(Cockie);

    // Build the read data message
    TOHDBAck *Msg = static_cast<TOHDBAck *>(Data);

    if(Msg->MessageID != OHDB_ACK)
        return;

    switch (Msg->RespondedMessageID)
    {
    case OHDB_WRITE_DATA_TO_XILINX:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_WriteXilinxAckOk=true;
        break;
    case OCB_WRITE_LOTUS_REGISTER:
        if (Msg->AckStatus == OHDB_Ack_Success)
            InstancePtr->m_WriteXilinxAckOk=true;
        break;
    default:
        return;
    }
}

WORD CTester::ReadDataFromOHDBXilinx(BYTE Address)
{
    COHDBProtocolClient *ProtocolClient = COHDBProtocolClient::Instance();

    TOHDBXilinxReadMessage Msg;

    Msg.MessageID = OHDB_READ_FROM_XILINX;
    Msg.Address = (BYTE) Address;

    m_ReadXilinxAckOk = false;

    if (ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TOHDBXilinxReadMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"OHDB didn't get ack for \"ReadDataFromOHDBXilinx\" message");
        throw ETester("OHDB didn't get ack for \"ReadDataFromOHDBXilinx\" message");
    }

    // Check if we really got ack
    if(!m_ReadXilinxAckOk)
        throw ETester("Tester:ReadDataFromOHDBXilinx Invalid reply from OHDB");

    if (m_XilinxAddress != Address)
        throw ETester("OHDB received wrong address");

    return m_XilinxData;
}


BYTE CTester::ReadDataFromLotus(WORD Address)
{
    COHDBProtocolClient *ProtocolClient = COHDBProtocolClient::Instance();

    TLotusReadMessage Msg;
    Msg.MessageID = OCB_READ_LOTUS_REGISTER;

    Msg.Address = Address;

    m_ReadXilinxAckOk = false;

    if (ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TLotusReadMessage),
            StatusHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"Lotus didn't get ack for \"ReadDataFromLotus\" message");
        throw ETester("Lotus didn't get ack for \"ReadDataFromLotus\" message");
    }

    // Check if we really got ack
    if(!m_ReadXilinxAckOk)
        throw ETester("Tester:ReadDataFromLotus Invalid reply from Lotus");

//  if (m_LotusAddress != Address)
    //	  ETester("Lotus received wrong address");

    return m_XilinxData;
}

int CTester::WriteDataToLotus(WORD Address, int Data)
{
    TLotusWriteLotusRegisterMessage Msg;
    Msg.MessageID = OCB_WRITE_LOTUS_REGISTER;
    Msg.Address = Address;
    Msg.Data = Data;

    COHDBProtocolClient *ProtocolClient = COHDBProtocolClient::Instance();
    m_WriteXilinxAckOk = false;

    if (ProtocolClient->SendInstallWaitReply(&Msg,
            sizeof(TLotusWriteLotusRegisterMessage),
            AckHandler,
            reinterpret_cast<TGenericCockie>(this)) != QLib::wrSignaled)
    {
        CQLog::Write(LOG_TAG_PRINT,"Lotus didn't get ack for \"WriteDataToLotus\" message");
        throw ETester("Lotus didn't get ack for \"WriteDataToLotus\" message");
    }

    // Check if we really got ack
    if(!m_WriteXilinxAckOk)
        throw ETester("Tester:\"WriteDataToLotus\" Invalid reply from Lotus");

    return 0;
}

void CTester::StatusHandler(int TransactionId,PVOID Data,unsigned DataLength,TGenericCockie Cockie)
{
    CTester *InstancePtr = reinterpret_cast<CTester *>(Cockie);

    // Build the read data message
    TOHDBGenericMessage *Response = static_cast<TOHDBGenericMessage *>(Data);

    switch (Response->MessageID)
    {
    case OHDB_XILINX_DATA:
    {
        TOHDBXilinxResponse *XilinxResponse = static_cast<TOHDBXilinxResponse *>(Data);

        InstancePtr->m_XilinxData = XilinxResponse->Data;
        InstancePtr->m_XilinxAddress = XilinxResponse->Address;

        // Trigger the event
        InstancePtr->m_ReadXilinxAckOk = true;
        break;
    }
    case READ_FROM_REGISTER_RESPONSE:
    {
        TLotusRegisterDataResponse *ExtMemResponse = static_cast<TLotusRegisterDataResponse *>(Data);

        InstancePtr->m_XilinxData = ExtMemResponse->Data;
        InstancePtr->m_XilinxAddress = ExtMemResponse->Address;

        // Trigger the event
        InstancePtr->m_ReadXilinxAckOk = true;
        break;
    }
    default:
        return;
    }
}

// ---------------------------------------------------------------------------

CTesterDummy::CTesterDummy()
{
}

CTesterDummy::~CTesterDummy()
{   
}

ULONG CTesterDummy::PCIReadDWORD(int BarNum,ULONG Addr)
{
	return 0;
}

int CTesterDummy::PCIWriteDWORD(int BarNum,ULONG Addr,ULONG Data)
{
    return 0;
}

int CTesterDummy::FIFOSetReadMode(int ReadMode)
{
    return 0;
}

int CTesterDummy::WriteDataToOHDBXilinx(BYTE Address,WORD Data)
{
    return 0;
}

WORD CTesterDummy::ReadDataFromOHDBXilinx(BYTE Address)
{
    return 0;
}

BYTE CTesterDummy::ReadDataFromLotus(WORD Address)
{
    return 0;
}

int CTesterDummy::WriteDataToLotus(WORD Address, int Data)
{
    return 0;
}
