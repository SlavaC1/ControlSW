//#include "stdafx.h"
#include "Hitag.h"


CHitag::CHitag()
{
	pSerialPort = new CSerialPort();
}

CHitag::~CHitag()
{
	if(pSerialPort)
		delete pSerialPort;
}

DWORD CHitag::SP_Init( char* sComPort )
{
	return pSerialPort->SP_Init( sComPort );
}

DWORD CHitag::SP_Exit( void )
{
	return pSerialPort->SP_Exit();
}
//----------------------------------------------------------------------------
DWORD CHitag::HT_ExecuteCmd( unsigned char* SndBuf, 
                               unsigned char SndLen, 
                               unsigned char* RcvBuf, 
                               unsigned char* RcvLen,
                               unsigned char Mode /*= OPERATING_MODE*/ )
//----------------------------------------------------------------------------
{   // Append BCC
    switch( Mode )
    {   case    OPERATING_MODE:
            SndBuf[ SndLen ] = CalcOperatingModeBCC( SndBuf, SndLen );
            SndLen++;
            break;
        case    KEY_INIT_MODE:
            SndBuf[ SndLen ] = CalcKeyInitModeBCC( SndBuf, SndLen );
            SndLen++;
            break;
        default:
            break;
    }
    // Execute
    *RcvLen = ( unsigned char )0xFF;
    memset( RcvBuf, 0x0, 256 );
    DWORD   status = pSerialPort->SP_Exchange( SndBuf, SndLen, RcvBuf, RcvLen, TRUE );
    // Check the returned BCC
    switch( Mode )
    {   case    OPERATING_MODE:
            if( RcvBuf[ RcvBuf[ 0 ] ] != CalcOperatingModeBCC( RcvBuf, RcvBuf[ 0 ] ) || *RcvLen == 0 )
            {   return( ESERIELL );
            }
            return( RcvBuf[ 1 ] );
        case    KEY_INIT_MODE:
            if( RcvBuf[ RcvBuf[ 0 ] ] != CalcKeyInitModeBCC( RcvBuf, RcvBuf[ 0 ] ) )
            {   return( ESERIELL );
            }
            return( RcvBuf[ 1 ] );
        default:
            break;
    }
    //
    return( status );
}

//----------------------------------------------------------------------------
DWORD CHitag::HT_SendData( unsigned char* SndBuf, 
                             unsigned char SndLen, 
                             unsigned char Mode /*= OPERATING_MODE*/ )
//----------------------------------------------------------------------------
{   // Append BCC
    switch( Mode )
    {   case    OPERATING_MODE:
            SndBuf[ SndLen ] = CalcOperatingModeBCC( SndBuf, SndLen );
            SndLen++;
            break;
        case    KEY_INIT_MODE:
            SndBuf[ SndLen ] = CalcKeyInitModeBCC( SndBuf, SndLen );
            SndLen++;
            break;
        default:
            break;
    }
    // Send
    return( pSerialPort->SP_Send( SndBuf, SndLen ) );
}

//----------------------------------------------------------------------------
DWORD CHitag::HT_ReceiveData( unsigned char* RcvBuf, 
                                unsigned char* RcvLen,
                                unsigned char Mode /*= OPERATING_MODE*/ )
//----------------------------------------------------------------------------
{   // Receive
    *RcvLen = ( unsigned char )0xFF;
    memset( RcvBuf, 0x0, 256 );
    DWORD   status = pSerialPort->SP_Receive( RcvBuf, RcvLen, TRUE );
    // Check the returned BCC
    switch( Mode )
    {   case    OPERATING_MODE:
            if( RcvBuf[ RcvBuf[ 0 ] ] != CalcOperatingModeBCC( RcvBuf, RcvBuf[ 0 ] ) )
            {   return( ESERIELL );
            }
            return( RcvBuf[ 1 ] );
        case    KEY_INIT_MODE:
            if( RcvBuf[ RcvBuf[ 0 ] ] != CalcKeyInitModeBCC( RcvBuf, RcvBuf[ 0 ] ) )
            {   return( ESERIELL );
            }
            return( RcvBuf[ 1 ] );
        default:
            break;
    }
    //
    return( status );
}



//----------------------------------------------------------------------------
// Implementation of NOT Exported Functions
//----------------------------------------------------------------------------
BYTE CHitag::CalcOperatingModeBCC( unsigned char* Buf, 
                           unsigned char Len )
{   //
    Buf[ Len ] = 0;
    for( int i = 0; i < Len; i++ )
    {   Buf[ Len ] ^= Buf[ i ];
    }
    return( Buf[ Len ] );
}

BYTE CHitag::CalcKeyInitModeBCC( unsigned char* Buf, 
                         unsigned char Len )
{   //
    Buf[ Len ] = 0;
    for( int i = 0; i < Len; i++ )
    {   Buf[ Len ] += Buf[ i ];
    }
    return( Buf[ Len ] );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_MutualAuthent( BYTE KeyInfo )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x03;
    ht1SndBuf[ 1 ] = HT1_MUTUAL_AUTHENT;
    ht1SndBuf[ 2 ] = KeyInfo;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht1RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_ReadBlock( BYTE Crypto, BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x04;
    ht1SndBuf[ 1 ] = HT1_READ_BLOCK;
    ht1SndBuf[ 2 ] = Crypto;
    ht1SndBuf[ 3 ] = PageNo;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht1RcvBuf[ 2 ], ht1RcvBuf[ 0 ] - 2 );
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_GetSnr( DWORD *Snr, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_GET_SNR;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            More[ 0 ] = ht1RcvBuf[ 2 + sizeof( DWORD ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_HaltSelected()
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_HALT_SELECTED;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht1RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_ReadPage( BYTE Crypto, BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x04;
    ht1SndBuf[ 1 ] = HT1_READ_PAGE;
    ht1SndBuf[ 2 ] = Crypto;
    ht1SndBuf[ 3 ] = PageNo;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht1RcvBuf[ 2 ], ht1RcvBuf[ 0 ] - 2 );
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_SelectSnr( DWORD Snr, DWORD *OTP )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x06;
    ht1SndBuf[ 1 ] = HT1_SELECT_SNR;
    memcpy( &ht1SndBuf[ 2 ], &Snr, sizeof( DWORD ) );
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( OTP, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_SelectLast()
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_SELECT_LAST;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht1RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_TagAuthent( BYTE KeyInfo )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x03;
    ht1SndBuf[ 1 ] = HT1_TAG_AUTHENT;
    ht1SndBuf[ 2 ] = KeyInfo;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht1RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_WriteBlock( BYTE Crypto, BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x04 + ( 16 - ( 4 * ( PageNo % 4 ) ) );
    ht1SndBuf[ 1 ] = HT1_WRITE_BLOCK;
    ht1SndBuf[ 2 ] = Crypto;
    ht1SndBuf[ 3 ] = PageNo;
    memcpy( &ht1SndBuf[ 4 ], Data, ht1SndBuf[ 0 ] - 4 );
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht1RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_PollTags( BYTE Mode, BYTE *Data, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x03;
    ht1SndBuf[ 1 ] = HT1_POLL_TAGS;
    ht1SndBuf[ 2 ] = Mode;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > 3 )
        {   memcpy( Data, &ht1RcvBuf[ 3 ], ht1RcvBuf[ 0 ] - 3 );
            More[ 0 ] = ht1RcvBuf[ 3 + sizeof( DWORD ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_WritePage( BYTE Crypto, BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x08;
    ht1SndBuf[ 1 ] = HT1_WRITE_PAGE;
    ht1SndBuf[ 2 ] = Crypto;
    ht1SndBuf[ 3 ] = PageNo;
    memcpy( &ht1SndBuf[ 4 ], Data, 4 );
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht1RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_PollKbTags( BYTE Mode, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x03;
    ht1SndBuf[ 1 ] = HT1_POLL_KB_TAGS;
    ht1SndBuf[ 2 ] = Mode;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht1RcvBuf[ 2 ], ht1RcvBuf[ 0 ] - 2 );
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_GetSnr_Adv( DWORD *Snr, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_GET_SNR_ADV;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            More[ 0 ] = ht1RcvBuf[ 2 + sizeof( DWORD ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_GetSnrNewSelect( DWORD *Snr, DWORD *OTP, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_GET_SNR_NEW_SELECT;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > ( 2 * sizeof( DWORD ) ) )
        {   memcpy( Snr, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            memcpy( OTP, &ht1RcvBuf[ 2 + sizeof( DWORD ) ], sizeof( DWORD ) );
            More[ 0 ] = ht1RcvBuf[ 2 + ( 2 * sizeof( DWORD ) ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_GetSnrSelect( DWORD *Snr, DWORD *OTP, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_GET_SNR_SELECT;
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > ( 2 * sizeof( DWORD ) ) )
        {   memcpy( Snr, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            memcpy( OTP, &ht1RcvBuf[ 2 + sizeof( DWORD ) ], sizeof( DWORD ) );
            More[ 0 ] = ht1RcvBuf[ 2 + ( 2 * sizeof( DWORD ) ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_GetSnrSelectHalt( DWORD *Snr, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_GET_SNR_SELECT_HALT;

    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            More[ 0 ] = ht1RcvBuf[ 2 + sizeof( DWORD ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_GetSnrNewSelectHalt( DWORD *Snr, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x02;
    ht1SndBuf[ 1 ] = HT1_GET_SNR_NEW_SELECT_HALT;

    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            More[ 0 ] = ht1RcvBuf[ 2 + sizeof( DWORD ) ];
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT1_SelectSnrDirect( DWORD Snr, DWORD *OTP )
//----------------------------------------------------------------------------
{   //
    ht1SndBuf[ 0 ] = 0x06;
    ht1SndBuf[ 1 ] = HT1_SELECT_SNR_DIRECT;
    memcpy( &ht1SndBuf[ 2 ], &Snr, sizeof( DWORD ) );
    ht1SndLen = ht1SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht1SndBuf, ht1SndLen, ht1RcvBuf, &ht1RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht1RcvBuf[ 0 ] >= sizeof( DWORD ) )
        {   memcpy( OTP, &ht1RcvBuf[ 2 ], sizeof( DWORD ) );
            return( ht1RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

BYTE CHitag::HT2_ReadMiro( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x02;
    ht2SndBuf[ 1 ] = HT2_READ_MIRO;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], ht2RcvBuf[ 0 ] - 2 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_PollTags( BYTE Mode, BYTE *Data, BYTE *More )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x03;
    ht2SndBuf[ 1 ] = HT2_POLL_TAGS;
    ht2SndBuf[ 2 ] = Mode;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 3 )
        {   memcpy( Data, &ht2RcvBuf[ 3 ], ht2RcvBuf[ 0 ] - 3 );
            More[ 0 ] = ht2RcvBuf[ 3 + sizeof( DWORD ) ];
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_GetSnr( BYTE Mode, DWORD *Snr, BYTE *Config )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x03;
    ht2SndBuf[ 1 ] = HT2_GET_SNR;
    ht2SndBuf[ 2 ] = Mode;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &ht2RcvBuf[ 2 ], sizeof( DWORD ) );
            Config[ 0 ] = ht2RcvBuf[ 2 + sizeof( DWORD ) ];
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_GetSnrReset( BYTE Mode, DWORD *Snr, BYTE *Config )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x04;
    ht2SndBuf[ 1 ] = HT2_GET_SNR_RESET;
    ht2SndBuf[ 2 ] = Mode;
    ht2SndBuf[ 3 ] = 0x4D;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &ht2RcvBuf[ 2 ], sizeof( DWORD ) );
            Config[ 0 ] = ht2RcvBuf[ 2 + sizeof( DWORD ) ];
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_HaltSelected()
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x02;
    ht2SndBuf[ 1 ] = HT2_HALT_SELECTED;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht2RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_ReadPage( BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x03;
    ht2SndBuf[ 1 ] = HT2_READ_PAGE;
    ht2SndBuf[ 2 ] = PageNo;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 4 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], 4 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_ReadPageInv( BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x03;
    ht2SndBuf[ 1 ] = HT2_READ_PAGE_INV;
    ht2SndBuf[ 2 ] = PageNo;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 4 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], 4 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_WritePage( BYTE PageNo, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x07;
    ht2SndBuf[ 1 ] = HT2_WRITE_PAGE;
    ht2SndBuf[ 2 ] = PageNo;
    memcpy( &ht2SndBuf[ 3 ], Data, 4 );
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( ht2RcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_ReadPit( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x02;
    ht2SndBuf[ 1 ] = HT2_READ_PIT;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], ht2RcvBuf[ 0 ] - 2 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_PollKbTags( BYTE Mode, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x03;
    ht2SndBuf[ 1 ] = HT2_POLL_KB_TAGS;
    ht2SndBuf[ 2 ] = Mode;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], ht2RcvBuf[ 0 ] - 2 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_ReadPublicB( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x02;
    ht2SndBuf[ 1 ] = HT2_READ_PUBLICB;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], ht2RcvBuf[ 0 ] - 2 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HT2_ReadTTF( BYTE Mode, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    ht2SndBuf[ 0 ] = 0x03;
    ht2SndBuf[ 1 ] = HT2_READ_TTF;
    ht2SndBuf[ 2 ] = Mode;
    ht2SndLen = ht2SndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( ht2SndBuf, ht2SndLen, ht2RcvBuf, &ht2RcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( ht2RcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &ht2RcvBuf[ 2 ], ht2RcvBuf[ 0 ] - 2 );
            return( ht2RcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

BYTE CHitag::KI_Reset()
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x02;
    htkiSndBuf[ 1 ] = KI_RESET;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_ReadSecret_HT2( BYTE Num, DWORD *Data )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x03;
    htkiSndBuf[ 1 ] = KI_READ_SECRET_HT2;
    htkiSndBuf[ 2 ] = Num;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   if( htkiRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htkiRcvBuf[ 2 ], htkiRcvBuf[ 0 ] - 2 );
            return( htkiRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WriteSecret_HT2( BYTE Num, DWORD OldData, DWORD NewData )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x0B;
    htkiSndBuf[ 1 ] = KI_WRITE_SECRET_HT2;
    htkiSndBuf[ 2 ] = Num;
    memcpy( &htkiSndBuf[ 3 ], &OldData, sizeof( DWORD ) );
    memcpy( &htkiSndBuf[ 3 + sizeof( DWORD ) ], &NewData, sizeof( DWORD ) );
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_ReadEEPROM( BYTE Num, DWORD *Data )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x03;
    htkiSndBuf[ 1 ] = KI_READ_EE_DATA;
    htkiSndBuf[ 2 ] = Num;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   if( htkiRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htkiRcvBuf[ 2 ], htkiRcvBuf[ 0 ] - 2 );
            return( htkiRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WriteEEPROM( BYTE Num, DWORD OldData, DWORD NewData )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x0B;
    htkiSndBuf[ 1 ] = KI_WRITE_EE_DATA;
    htkiSndBuf[ 2 ] = Num;
    memcpy( &htkiSndBuf[ 3 ], &OldData, sizeof( DWORD ) );
    memcpy( &htkiSndBuf[ 3 + sizeof( DWORD ) ], &NewData, sizeof( DWORD ) );
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WriteSerNum( BYTE *Snr )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x0D;
    htkiSndBuf[ 1 ] = KI_WRITE_SERNUM;
    memcpy( &htkiSndBuf[ 2 ], Snr, 11 );
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WritePitSecurity( BYTE Mode )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x03;
    htkiSndBuf[ 1 ] = KI_WRITE_PIT_SECURITY;
    htkiSndBuf[ 2 ] = Mode;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WritePitPassword( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x09;
    htkiSndBuf[ 1 ] = KI_WRITE_PIT_PASSWORD;
    memcpy( &htkiSndBuf[ 2 ], Data, 7 );
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WriteControl( BYTE ControlRW, BYTE ControlWO )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x04;
    htkiSndBuf[ 1 ] = KI_WRITE_CONTROL;
    htkiSndBuf[ 2 ] = ControlRW;
    htkiSndBuf[ 3 ] = ControlWO;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_ReadControl( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x02;
    htkiSndBuf[ 1 ] = KI_READ_CONTROL;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   if( htkiRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htkiRcvBuf[ 2 ], htkiRcvBuf[ 0 ] - 2 );
            return( htkiRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_ReadControl_HT2( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x02;
    htkiSndBuf[ 1 ] = KI_READ_CONTROL_HT2;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   if( htkiRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htkiRcvBuf[ 2 ], htkiRcvBuf[ 0 ] - 2 );
            return( htkiRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WriteControl_HT2( BYTE Control )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x03;
    htkiSndBuf[ 1 ] = KI_WRITE_CONTROL_HT2;
    htkiSndBuf[ 0 ] = Control;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_ReadControl_HTS( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x02;
    htkiSndBuf[ 1 ] = KI_READ_CONTROL_HTS;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   if( htkiRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htkiRcvBuf[ 2 ], htkiRcvBuf[ 0 ] - 2 );
            return( htkiRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::KI_WriteControl_HTS( BYTE Control )
//----------------------------------------------------------------------------
{   //
    htkiSndBuf[ 0 ] = 0x03;
    htkiSndBuf[ 1 ] = KI_WRITE_CONTROL_HTS;
    htkiSndBuf[ 0 ] = Control;
    htkiSndLen = htkiSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htkiSndBuf, htkiSndLen, htkiRcvBuf, &htkiRcvLen, KEY_INIT_MODE ) == ERROR_SUCCESS )
    {   return( htkiRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

BYTE CHitag::RWD_SetPowerDown( BYTE Mode )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_SET_POWER_DOWN;
    htrSndBuf[ 2 ] = Mode;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_ReadEEData( BYTE Addr, BYTE BytesToRead, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x04;
    htrSndBuf[ 1 ] = RWD_EE_READ;
    htrSndBuf[ 2 ] = Addr;
    htrSndBuf[ 3 ] = BytesToRead;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( htrRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htrRcvBuf[ 2 ], htrRcvBuf[ 0 ] - 2 );
            return( htrRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_StartFFT()
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_FFT_COMMAND;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_SetBCD( BYTE BitClockData )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_SET_BCD;
    htrSndBuf[ 2 ] = BitClockData;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_ReadInput( BYTE *Input )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_READ_INPUT;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   Input[ 0 ] = htrRcvBuf[ 2 ];
        return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_KeyInitMode( DWORD Pwd )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x06;
    htrSndBuf[ 1 ] = RWD_KEY_INIT_MODE;
    memcpy( &htrSndBuf[ 2 ], &Pwd, sizeof( DWORD ) );
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_SetHFMode( BYTE Mode )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_SET_HF_MODE;
    htrSndBuf[ 2 ] = Mode;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
DWORD CHitag::RWD_SetOutput( BYTE Output )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_SET_OUTPUT;
    htrSndBuf[ 2 ] = Output;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_Reset()
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_RESET_SYS;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_GetVersion( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_GET_VERSION;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( htrRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htrRcvBuf[ 2 ], htrRcvBuf[ 0 ] - 2 );
            return( htrRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_ConfigPorts( BYTE Config )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_CONFIG_PORTS;
    htrSndBuf[ 2 ] = Config;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_WriteEEData( BYTE Addr, BYTE BytesToWrite, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = BytesToWrite + 4;
    htrSndBuf[ 1 ] = RWD_EE_WRITE;
    htrSndBuf[ 2 ] = Addr;
    htrSndBuf[ 3 ] = BytesToWrite;
    memcpy( &htrSndBuf[ 4 ], Data, htrSndBuf[ 0 ] - 4 );
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_ReadBCD( BYTE *Input )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_READ_BCD;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   Input[ 0 ] = htrRcvBuf[ 2 ];
        return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_HFReset()
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_HF_RESET;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_ReadPorts( BYTE *Input )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_READ_PORTS;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   Input[ 0 ] = htrRcvBuf[ 2 ];
        return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_WritePorts( BYTE Output, BYTE Mode )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x04;
    htrSndBuf[ 1 ] = RWD_WRITE_PORTS;
    htrSndBuf[ 2 ] = Output;
    htrSndBuf[ 3 ] = Mode;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_ReadLRStatus()
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_READ_LR_STATUS;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_PollKbTags( BYTE Mode, BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_POLL_KB_TAGS;
    htrSndBuf[ 2 ] = Mode;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( htrRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htrRcvBuf[ 2 ], htrRcvBuf[ 0 ] - 2 );
            return( htrRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_SetModuleAddr( BYTE Addr, BYTE *Snr )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x0E;
    htrSndBuf[ 1 ] = RWD_SETMODULEADDR;
    memcpy( &htrSndBuf[ 2 ], Snr, 11 );
    htrSndBuf[ 13 ] = Addr;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_SetProxTrmTime( BYTE T0, BYTE T1, BYTE TP )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x05;
    htrSndBuf[ 1 ] = RWD_SET_PROX_TRM_TIME;
    htrSndBuf[ 2 ] = T0;
    htrSndBuf[ 3 ] = T1;
    htrSndBuf[ 4 ] = TP;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_SetBCDOffset( BYTE BitClockDataOffset )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_SET_BCD_OFFSET;
    htrSndBuf[ 0 ] = BitClockDataOffset;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_StopCommand()
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x02;
    htrSndBuf[ 1 ] = RWD_STOP_CMD;
    htrSndLen = htrSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htrRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::RWD_SetBaudrate( BYTE Mode )
//----------------------------------------------------------------------------
{   //
    htrSndBuf[ 0 ] = 0x03;
    htrSndBuf[ 1 ] = RWD_SET_BAUDRATE;
    htrSndBuf[ 2 ] = Mode;
    htrSndLen = htrSndBuf[ 0 ];
    //
    unsigned long br;
    switch( Mode )
    {   case    0: br = 4800;   break;
        case    1: br = 9600;   break;
        case    2: br = 14400;  break;
        case    3: br = 19200;  break;
        case    4: br = 38400;  break;
        case    5: br = 57600;  break;
        case    6: br = 115200; break;
        default:
            return( ESERIELL );
    }
    //
    if( HT_ExecuteCmd( htrSndBuf, htrSndLen, htrRcvBuf, &htrRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   //
        if( pSerialPort->SP_SetBaudrate( br ) == ERROR_SUCCESS )
        {   return( EOK );
        }
    }
    //
    return( ESERIELL );
}

BYTE CHitag::HTS_GetSnrReset( BYTE Mode, DWORD *Snr, BYTE *More )
//----------------------------------------------------------------------------
{   //
    htsSndBuf[ 0 ] = 0x03;
    htsSndBuf[ 1 ] = HTS_GET_SNR_RESET;
    htsSndBuf[ 2 ] = Mode;
    htsSndLen = htsSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htsSndBuf, htsSndLen, htsRcvBuf, &htsRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( htsRcvBuf[ 0 ] > sizeof( DWORD ) )
        {   memcpy( Snr, &htsRcvBuf[ 2 ], sizeof( DWORD ) );
            More[ 0 ] = htsRcvBuf[ 2 + sizeof( DWORD ) ];
            return( htsRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HTS_SelectSnrReset( BYTE Mode, DWORD Snr, DWORD *OTP )
//----------------------------------------------------------------------------
{   //
    htsSndBuf[ 0 ] = 0x07;
    htsSndBuf[ 1 ] = HTS_SELECT_SNR_RESET;
    memcpy( &htsSndBuf[ 2 ], &Snr, sizeof( DWORD ) );
    htsSndBuf[ 2 + sizeof( DWORD ) ] = Mode;
    htsSndLen = htsSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htsSndBuf, htsSndLen, htsRcvBuf, &htsRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( htsRcvBuf[ 0 ] >= sizeof( DWORD ) )
        {   memcpy( OTP, &htsRcvBuf[ 2 ], sizeof( DWORD ) );
            return( htsRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HTS_TagAuthent()
//----------------------------------------------------------------------------
{   //
    htsSndBuf[ 0 ] = 0x02;
    htsSndBuf[ 1 ] = HTS_TAG_AUTHENT;
    htsSndLen = htsSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htsSndBuf, htsSndLen, htsRcvBuf, &htsRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   return( htsRcvBuf[ 1 ] );
    }
    //
    return( ESERIELL );
}

BYTE CHitag::HTV_ReadAllPage( BYTE Mode, BYTE PageNo, BYTE *Data, WORD *DataLen )
//----------------------------------------------------------------------------
{   //
    htvSndBuf[ 0 ] = 0x04;
    htvSndBuf[ 1 ] = VEGAS_READ_ALL_PAGE;
    htvSndBuf[ 2 ] = Mode;
    htvSndBuf[ 3 ] = PageNo;
    htvSndLen = htvSndBuf[ 0 ];
    //
    DWORD   status = HT_ExecuteCmd( htvSndBuf, htvSndLen, htvRcvBuf, &htvRcvLen, OPERATING_MODE );
    *DataLen = 0;
    while( status == ERROR_SUCCESS )
    {   if( htvRcvBuf[ 0 ] == 2 )
        {   // This is the last frame
            return( htvRcvBuf[ 1 ] );
        }
        if( htvRcvBuf[ 0 ] > 2 )
        {   memcpy( &Data[ ( *DataLen ) * 4 ], &htvRcvBuf[ 2 ], htvRcvBuf[ 0 ] - 2 );
        }else
        {   return( ESERIELL );
        }
        *DataLen += 4;
        // Several frames are returned by the reader
        status = HT_ReceiveData( htvRcvBuf, &htvRcvLen );
    }
    //
    return( ESERIELL );
}

//----------------------------------------------------------------------------
BYTE CHitag::HTV_GetDspVersion( BYTE *Data )
//----------------------------------------------------------------------------
{   //
    htvSndBuf[ 0 ] = 0x02;
    htvSndBuf[ 1 ] = VEGAS_GET_DSP_VERSION;
    htvSndLen = htvSndBuf[ 0 ];
    //
    if( HT_ExecuteCmd( htvSndBuf, htvSndLen, htvRcvBuf, &htvRcvLen, OPERATING_MODE ) == ERROR_SUCCESS )
    {   if( htvRcvBuf[ 0 ] > 2 )
        {   memcpy( Data, &htvRcvBuf[ 2 ], htvRcvBuf[ 0 ] - 2 );
            return( htvRcvBuf[ 1 ] );
        }else
        {   return( ESERIELL );
        }
    }
    //
    return( ESERIELL );
}



