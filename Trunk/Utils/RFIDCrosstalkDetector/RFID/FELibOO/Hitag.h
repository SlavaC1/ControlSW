#ifndef __CHITAG
#define __CHITAG



#include "SerialPort.h"

#define         OPERATING_MODE      0
#define         KEY_INIT_MODE       1

////////////////////////////////////////////////////////////////
class CHitag
{
public:
	CHitag();
	~CHitag();

	DWORD	SP_Init( char* sComPort );
	DWORD	SP_Exit( void );


DWORD HT_ExecuteCmd    ( unsigned char* SndBuf, 
                                              unsigned char SndLen, 
                                              unsigned char* RcvBuf, 
                                              unsigned char* RcvLen,
                                              unsigned char Mode = OPERATING_MODE );

DWORD HT_SendData      ( unsigned char* SndBuf, 
                                              unsigned char SndLen, 
                                              unsigned char Mode = OPERATING_MODE );

DWORD HT_ReceiveData   ( unsigned char* RcvBuf, 
                                              unsigned char* RcvLen,
                                              unsigned char Mode = OPERATING_MODE );

////////////////////////////////////////////////////////////
//HITAG1
BYTE   HT1_MutualAuthent ( BYTE KeyInfo );
BYTE   HT1_ReadBlock     ( BYTE Crypto, BYTE PageNo, BYTE *Data );
BYTE   HT1_GetSnr        ( DWORD *Snr, BYTE *More );
BYTE   HT1_HaltSelected  ();
BYTE   HT1_ReadPage      ( BYTE Crypto, BYTE PageNo, BYTE *Data );
BYTE   HT1_SelectSnr     ( DWORD Snr, DWORD *OTP );
BYTE   HT1_SelectLast    ();
BYTE   HT1_TagAuthent    ( BYTE KeyInfo );
BYTE   HT1_WriteBlock    ( BYTE Crypto, BYTE PageNo, BYTE *Data );
BYTE   HT1_PollTags      ( BYTE Mode, BYTE *Data, BYTE *More );
BYTE   HT1_WritePage     ( BYTE Crypto, BYTE PageNo, BYTE *Data );
BYTE   HT1_PollKbTags    ( BYTE Mode, BYTE *Data );
BYTE   HT1_GetSnr_Adv    ( DWORD *Snr, BYTE *More );
BYTE   HT1_GetSnrNewSelect       ( DWORD *Snr, DWORD* OTP, BYTE *More );
BYTE   HT1_GetSnrSelect          ( DWORD *Snr, DWORD* OTP, BYTE *More );
BYTE   HT1_GetSnrSelectHalt      ( DWORD *Snr, BYTE *More );
BYTE   HT1_GetSnrNewSelectHalt   ( DWORD *Snr, BYTE *More );
BYTE   HT1_SelectSnrDirect       ( DWORD Snr, DWORD *OTP );

////////////////////////////////////////////////////////////
//HITAG2
BYTE   HT2_ReadMiro      ( BYTE *Data );
BYTE   HT2_PollTags      ( BYTE Mode, BYTE *Data, BYTE *More );
BYTE   HT2_GetSnr        ( BYTE Mode, DWORD *Snr, BYTE *Config );
BYTE   HT2_GetSnrReset   ( BYTE Mode, DWORD *Snr, BYTE *Config );
BYTE   HT2_HaltSelected  ();
BYTE   HT2_ReadPage      ( BYTE PageNo, BYTE *Data );
BYTE   HT2_ReadPageInv   ( BYTE PageNo, BYTE *Data );
BYTE   HT2_WritePage     ( BYTE PageNo, BYTE *Data );
BYTE   HT2_ReadPit       ( BYTE *Data );
BYTE   HT2_PollKbTags    ( BYTE Mode, BYTE *Data );
BYTE   HT2_ReadPublicB   ( BYTE *Data );
BYTE   HT2_ReadTTF       ( BYTE Mode, BYTE *Data );

////////////////////////////////////////////////////////////
//HitagKeyInit
BYTE   KI_Reset              ();
BYTE   KI_ReadSecret_HT2     ( BYTE Num, DWORD *Data );
BYTE   KI_WriteSecret_HT2    ( BYTE Num, DWORD OldData, DWORD NewData );
BYTE   KI_ReadEEPROM         ( BYTE Num, DWORD *Data );
BYTE   KI_WriteEEPROM        ( BYTE Num, DWORD OldData, DWORD NewData );
BYTE   KI_WriteSerNum        ( BYTE *Snr );
BYTE   KI_WritePitSecurity   ( BYTE Mode );
BYTE   KI_WritePitPassword   ( BYTE *Data );
BYTE   KI_ReadControl        ( BYTE *Data );
BYTE   KI_WriteControl       ( BYTE ControlRW, BYTE ControlWO );
BYTE   KI_ReadControl_HT2    ( BYTE *Data );
BYTE   KI_WriteControl_HT2   ( BYTE Control );
BYTE   KI_ReadControl_HTS    ( BYTE *Data );
BYTE   KI_WriteControl_HTS   ( BYTE Control );

////////////////////////////////////////////////////////////
//HitagRWD
BYTE   RWD_SetPowerDown  ( BYTE Mode );
BYTE   RWD_ReadEEData    ( BYTE Addr, BYTE BytesToRead, BYTE *Data );
BYTE   RWD_StartFFT      ();
BYTE   RWD_SetBCD        ( BYTE BitClockData );
BYTE   RWD_ReadInput     ( BYTE *Input );
BYTE   RWD_KeyInitMode   ( DWORD Pwd );
BYTE   RWD_SetHFMode     ( BYTE Mode );
DWORD  RWD_SetOutput     ( BYTE Output );
BYTE   RWD_Reset         ();
BYTE   RWD_GetVersion    ( BYTE *Data );
BYTE   RWD_ConfigPorts   ( BYTE Config );
BYTE   RWD_WriteEEData   ( BYTE Addr, BYTE BytesToWrite, BYTE *Data );
BYTE   RWD_ReadBCD       ( BYTE *Input );
BYTE   RWD_HFReset       ();
BYTE   RWD_ReadPorts     ( BYTE *Input );
BYTE   RWD_WritePorts    ( BYTE Output, BYTE Mode );
BYTE   RWD_ReadLRStatus  ();
BYTE   RWD_PollKbTags    ( BYTE Mode, BYTE *Data );
BYTE   RWD_SetModuleAddr ( BYTE Addr, BYTE *Snr );
BYTE   RWD_SetProxTrmTime( BYTE T0, BYTE T1, BYTE TP );
BYTE   RWD_SetBCDOffset  ( BYTE BitClockDataOffset );
BYTE   RWD_StopCommand   ();
BYTE   RWD_SetBaudrate   ( BYTE Mode );

////////////////////////////////////////////////////////////
//HitagS
BYTE   HTS_GetSnrReset   ( BYTE Mode, DWORD *Snr, BYTE *More );
BYTE   HTS_SelectSnrReset( BYTE Mode, DWORD Snr, DWORD *OTP );
BYTE   HTS_TagAuthent    ();

////////////////////////////////////////////////////////////
//HitagVegas
BYTE   HTV_ReadAllPage   ( BYTE Mode, BYTE PageNo, BYTE *Data, WORD *DataLen );
BYTE   HTV_GetDspVersion ( BYTE *Data );

protected:
	CSerialPort*	pSerialPort;
BYTE            CalcOperatingModeBCC( unsigned char* Buf, 
                                      unsigned char Len );
BYTE            CalcKeyInitModeBCC( unsigned char* Buf, 
                                    unsigned char Len );

BYTE            ht1SndBuf[ 256 ];
BYTE            ht1SndLen;
BYTE            ht1RcvBuf[ 256 ];
BYTE            ht1RcvLen;

BYTE            ht2SndBuf[ 256 ];
BYTE            ht2SndLen;
BYTE            ht2RcvBuf[ 256 ];
BYTE            ht2RcvLen;

BYTE            htkiSndBuf[ 256 ];
BYTE            htkiSndLen;
BYTE            htkiRcvBuf[ 256 ];
BYTE            htkiRcvLen;

BYTE            htrSndBuf[ 256 ];
BYTE            htrSndLen;
BYTE            htrRcvBuf[ 256 ];
BYTE            htrRcvLen;

BYTE            htsSndBuf[ 256 ];
BYTE            htsSndLen;
BYTE            htsRcvBuf[ 256 ];
BYTE            htsRcvLen;

BYTE            htvSndBuf[ 256 ];
BYTE            htvSndLen;
BYTE            htvRcvBuf[ 256 ];
BYTE            htvRcvLen;



};


#endif