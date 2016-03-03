
#pragma hdrstop
#include "Reader.h"
#pragma package(smart_init)

const BYTE HEADER_1  = 0x55;
const BYTE HEADER_2  = 0x33;
const BYTE WRITE     = 0x83;
const BYTE READ      = 0x03;
const BYTE DATA_SIZE = 0x01; // in words
const BYTE FOOTER    = 0xDA;

const int ADD_LOW_INDEX         = 4;
const int ADD_HIGH_INDEX        = 5;
const int WRITE_DATA_LOW_INDEX  = 6;
const int WRITE_DATA_HIGH_INDEX = 7;
const int READ_DATA_LOW_INDEX   = 4;
const int READ_DATA_HIGH_INDEX = 5;

const int READ_BUFFER_SIZE = 7;

CReader::CReader(int ComNum)
{
	m_ComPort = new CQStdComPort(ComNum, 115200);
	m_ComPort->SetReadTimeout(100);
}

CReader::~CReader()
{
	m_ComPort->DeInit();
	Q_SAFE_DELETE(m_ComPort);
}

void CReader::WriteToFPGA(WORD Add, WORD Data)
{
	BYTE WriteBufffer[] = {HEADER_1, HEADER_2, WRITE, DATA_SIZE, 0, 0, 0, 0, FOOTER};

	WriteBufffer[ADD_LOW_INDEX]         = static_cast<BYTE>(Add & 0xFF);
	WriteBufffer[ADD_HIGH_INDEX]        = static_cast<BYTE>((Add >> 8) & 0xFF);
	WriteBufffer[WRITE_DATA_LOW_INDEX]  = static_cast<BYTE>(Data & 0xFF);
	WriteBufffer[WRITE_DATA_HIGH_INDEX] = static_cast<BYTE>((Data >> 8) & 0xFF);

	m_ComPort->Write(WriteBufffer, sizeof(WriteBufffer));
}

WORD CReader::ReadFromFPGA(WORD Add)
{
	BYTE WriteBufffer[] = {HEADER_1, HEADER_2, READ, DATA_SIZE, 0, 0, FOOTER};

	WriteBufffer[ADD_LOW_INDEX]   = static_cast<BYTE>(Add & 0xFF);
	WriteBufffer[ADD_HIGH_INDEX]  = static_cast<BYTE>((Add >> 8) & 0xFF);

	m_ComPort->Write(WriteBufffer, sizeof(WriteBufffer));

	BYTE ReadBufffer[READ_BUFFER_SIZE] = {0};
	
	m_ComPort->Read(ReadBufffer, READ_BUFFER_SIZE);

	WORD ret = 0;
	ret  = ReadBufffer[READ_DATA_HIGH_INDEX];
	ret  = ret << 8;
	ret |= ReadBufffer[READ_DATA_LOW_INDEX];

	return ret;
}




