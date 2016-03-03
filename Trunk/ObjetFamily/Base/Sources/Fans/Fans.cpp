//---------------------------------------------------------------------------


#pragma hdrstop

#include "Fans.h"
#include "Q2RTErrors.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)



CFansBase::CFansBase(const QString& Name) : CQComponent(Name)
{
	m_ParamsMgr = CAppParams::Instance();
	m_OHDBClient = COHDBProtocolClient::Instance();
	m_ErrorHandlerClient = CErrorHandler::Instance();

	m_MainFanCriticalSpeedRetriesCounter = 0;
	m_MainFanWarningSpeedRetriesCounter  = 0;

	for (int i = 0 ; i < NUM_OF_FANS ; i++)
	{
    	m_FansData[i] = NA;
	}
}


CFans::CFans(const QString& Name) : CFansBase(Name)
{
	m_OHDBClient->InstallMessageHandler(OHDB_FANS_RECEIVE_DATA,
										&FansReceiveDataMsgHandler,
										reinterpret_cast<TGenericCockie>(this));
}



void CFans::FansReceiveDataMsgHandler(int TransactionId, PVOID Data, unsigned DataLength, TGenericCockie Cockie)
{
	static CFans *obj = reinterpret_cast<CFans *>(Cockie);
	static TFansDataMessage *Msg = static_cast<TFansDataMessage *>(Data);


	//Verify size of message
	if(DataLength != sizeof(TFansDataMessage))
	{
		FrontEndInterface->NotificationMessage("Fans Data message length wrong.");
		CQLog::Write(LOG_TAG_HEAD_HEATERS,"\"Fans Data message length wrong.");
		return;
	}

    obj->DataToRpm( Msg );

	obj->UpdateRightFans( Msg );
	obj->UpdateLeftFans( Msg );
	obj->UpdateMiddleFan( Msg );
}

void CFans::DataToRpm(
TFansDataMessage *Msg)
{
	for (int i = 0 ; i < NUM_OF_FANS ; i++)
	{
		m_FansData[i] = Msg->FansData[i] * 60; // According to HW formula - converting raw register data to RPM (in Keil we already divided by 2, to deal with byte and not word size data)...
	}
}


bool CFans::IsFansOn(WORD mask, WORD FansState)
{
	return (mask & FansState);
}

bool CFans::IsMaterialFansDutyCycle100(WORD DutyCycle)
{
	return ( ( MATERIAL_FANS_DUTY_CYCLE_100 == DutyCycle ) ? 1 : 0 );
}

TQErrCode CFans::IsError()
{
	TQErrCode Err = Q_NO_ERROR;

	if ( m_ParamsMgr->MainFanSpeedWarningRetries <= m_MainFanWarningSpeedRetriesCounter )
	{
		Err = Q2RT_HEAD_MAIN_FAN_SPEED_CRITICAL;
	}

    return Err;
}

void CFans::UpdateRightFans(TFansDataMessage *Msg)
{
	WORD MaterialFansState 		= Msg->FansData[MATERIAL_FANS_STATE];
	WORD MaterialFansDutyCycle	= Msg->FansData[MATERIAL_FANS_DUTY_CYCLE];

	// Update right material fans dialog edit boxes.
	if ( IsFansOn(RIGHT_FANS_MASK, MaterialFansState) )
	{
		if ( IsMaterialFansDutyCycle100(MaterialFansDutyCycle) )
		{
			FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_MATERIAL_FAN_1, m_FansData[RIGHT_MATERIAL_FAN_1], true);
			FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_MATERIAL_FAN_2, m_FansData[RIGHT_MATERIAL_FAN_2], true);
		}
		else
		{
			FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_MATERIAL_FAN_1, NA, true);
			FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_MATERIAL_FAN_2, NA, true);
		}
	}
	else
	{
		FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_MATERIAL_FAN_1, OFF, true);
		FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_MATERIAL_FAN_2, OFF, true);
	}

	// Update right UV fans dialog edit boxes.
	FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_UV_FAN_1, m_FansData[RIGHT_UV_FAN_1], true);
	FrontEndInterface->UpdateStatus(FE_FANS_RIGHT_UV_FAN_2, m_FansData[RIGHT_UV_FAN_2], true);
}


void CFans::UpdateLeftFans(TFansDataMessage *Msg)
{
	WORD MaterialFansState 		= Msg->FansData[MATERIAL_FANS_STATE];
	WORD MaterialFansDutyCycle	= Msg->FansData[MATERIAL_FANS_DUTY_CYCLE];

	// Update right material fans dialog edit boxes.
	if ( IsFansOn(LEFT_FANS_MASK, MaterialFansState) )
	{
		if ( IsMaterialFansDutyCycle100(MaterialFansDutyCycle) )
		{
			FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_1, m_FansData[LEFT_MATERIAL_FAN_1], true);
			FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_2, m_FansData[LEFT_MATERIAL_FAN_2], true);
			FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_3, m_FansData[LEFT_MATERIAL_FAN_3], true);
		}
		else
		{
			FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_1, NA, true);
			FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_2, NA, true);
			FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_3, NA, true);
		}
	}
	else
	{
		FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_1, OFF, true);
		FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_2, OFF, true);
		FrontEndInterface->UpdateStatus(FE_FANS_LEFT_MATERIAL_FAN_3, OFF, true);
	}

	// Update right UV fans dialog edit boxes.
	FrontEndInterface->UpdateStatus(FE_FANS_LEFT_UV_FAN_1, m_FansData[LEFT_UV_FAN_1], true);
	FrontEndInterface->UpdateStatus(FE_FANS_LEFT_UV_FAN_2, m_FansData[LEFT_UV_FAN_2], true);
}


void CFans::UpdateMiddleFan(TFansDataMessage *Msg)
{
	WORD MainFansState  = Msg->FansData[MATERIAL_FANS_STATE];
	WORD MainFanSpeed 	= m_FansData[MIDDLE_FAN];
	bool MainFanOn		 = IsFansOn(MIDDLE_FANS_MASK, MainFansState);

	// Update right material fans dialog edit boxes.
	if ( MainFanOn )
	{
		FrontEndInterface->UpdateStatus(FE_FANS_MIDDLE_FAN, MainFanSpeed, true);
	}
	else
	{
        m_MainFanCriticalSpeedRetriesCounter = 0;
		m_MainFanWarningSpeedRetriesCounter  = 0;

		FrontEndInterface->UpdateStatus(FE_FANS_MIDDLE_FAN, OFF, true);
	}

	// Check if not Bypass flag, actions are made only if not Bypass.
	// If Bypass is on, tha take no action and return.
    // Take no action also is the fan is off.
	if ( m_ParamsMgr->MainFanSpeedAlertsBypassFlag || !(MainFanOn) )
	{
		return;
	}

	// If only below warning value, but higher than critical, increase only warning counter.
	if ( m_ParamsMgr->MainFanSpeedWarningValue > MainFanSpeed )
	{
		if ( m_ParamsMgr->MainFanSpeedWarningRetries > m_MainFanWarningSpeedRetriesCounter )
		{
			m_MainFanWarningSpeedRetriesCounter++;
			CQLog::Write(LOG_TAG_HEAD_HEATERS,"Print block main fan speed is below warning level. speed = %d RPM; warning retries = %d ",MainFanSpeed, m_MainFanWarningSpeedRetriesCounter);
		}
			// If below Critical value, increase both critical and warning counters.
		if ( m_ParamsMgr->MainFanSpeedCriticalValue > MainFanSpeed )
		{
			if ( m_ParamsMgr->MainFanSpeedCriticalRetries > m_MainFanCriticalSpeedRetriesCounter )
			{
				m_MainFanCriticalSpeedRetriesCounter++;
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"Print block main fan speed is below critical level. speed = %d RPM; critical retries = %d ",MainFanSpeed, m_MainFanCriticalSpeedRetriesCounter);
			}
		}
	}
	else // If speed OK, reset both counters.
	{
		m_MainFanCriticalSpeedRetriesCounter = 0;
		m_MainFanWarningSpeedRetriesCounter  = 0;
	}

	// Popup Critical Speed Error message and close all heating and stop printing.
	if ( m_ParamsMgr->MainFanSpeedCriticalRetries == m_MainFanCriticalSpeedRetriesCounter )
	{
		m_MainFanCriticalSpeedRetriesCounter++; // Increase the critical counter, so the error message will show only once...
		m_MainFanWarningSpeedRetriesCounter = m_ParamsMgr->MainFanSpeedWarningRetries + 1;

		m_ErrorHandlerClient->ReportError(	"Print block fan is not functioning properly.\nPrinting is cancelled to prevent overheating.\nContact customer support.",
											Q2RT_HEAD_MAIN_FAN_SPEED_CRITICAL,
											MainFanSpeed);

	}
	// Popup Warning message, consider the don't show this message again checkbox.
	else if (m_ParamsMgr->MainFanSpeedWarningRetries == m_MainFanWarningSpeedRetriesCounter)
	{
	   if (!m_ParamsMgr->MainFanSpeedDontShowWarningMessageAgainFlag)
	   {
//			if (!m_ParamsMgr->MainFanWarningSpeedShownAlready)
//			{
//				m_ParamsMgr->MainFanWarningSpeedShownAlready = true;
//				m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MainFanWarningSpeedShownAlready);
				m_MainFanWarningSpeedRetriesCounter++;
				// Call Warning message here.
				FrontEndInterface->WarningMessage("Print block fan speed is slower than required.\nInform your service engineer of this during the next scheduled maintenance visit.");
				CQLog::Write(LOG_TAG_HEAD_HEATERS,"Print block fan speed is slower than required. Value = %d RPM",MainFanSpeed);
//			}
	   }
//	   else if (!m_ParamsMgr->MainFanWarningSpeedShownAlready)
//	   {
//			m_ParamsMgr->MainFanWarningSpeedShownAlready = true;
////    		m_ParamsMgr->SaveSingleParameter(&m_ParamsMgr->MainFanWarningSpeedShownAlready);
//			m_MainFanWarningSpeedRetriesCounter++;
//			// Call Warning message here.
//			FrontEndInterface->WarningMessage("Print block fan speed is slower than required.\nInform your service engineer of this during the next scheduled maintenance visit.");
//       		CQLog::Write(LOG_TAG_HEAD_HEATERS,"Print block fan speed is slower than required. Value = %d RPM",MainFanSpeed);
//	   }
	}
}

void CFans::ResetMiddleFanWarning()
{
 	m_MainFanWarningSpeedRetriesCounter  = 0;
	m_MainFanCriticalSpeedRetriesCounter = 0;
}




