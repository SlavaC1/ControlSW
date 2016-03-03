//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CartridgeErrorDlg.h"
#include "BackEndInterface.h"
#include "FrontEndControlIDs.h"
#include "SignalTower.h"
#include "AppLogFile.h"
#include "QThreadUtils.h"
#include "QTimer.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


const int SECONDS_IN_MINUTE = 60;
const int MINUTES_IN_HOUR   = 60;

TCartridgeErrorForm* CartridgeErrorForm;


//---------------------------------------------------------------------------
__fastcall TCartridgeErrorForm::TCartridgeErrorForm(TComponent* Owner)
  : TForm(Owner)
{
  m_SecondsCounter = 0;
  m_MinutesCounter = 0;
  m_HoursCounter = 0;
  m_DlgType = 0;
  m_WasteRemovalDetected= false;
  m_HandleStopAndResumePrinting = false;
  m_HandleStopPrinting = false;
  m_WastePrevOnOff = false;
  m_Modal = false;
  m_ReadyForPrint = false;
  m_BackEndInterface = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TCartridgeErrorForm::CartridgeStatusTimerTimer(TObject *Sender)
{
	if(! CAppParams::Instance()->DualWasteEnabled)
	{
	  if ((TChamberIndex)TYPE_CHAMBER_WASTE == m_Chamber)
	  {
		  WasteStatusTimerTimer(Sender);
		  return;
	  }
	}

 if (!VALIDATE_CHAMBER_INCLUDING_WASTE(m_Chamber))
    return;

  UpdateTanksState();

  int TanksEmptyNum = CountTanksEmptyNum();
  int TanksInNum    = CountTanksInNum();
  //CQLog::Write(LOG_TAG_GENERAL, "%s: Number of tanks is %d. Number of empty tanks is %d.", ChamberToStr(m_Chamber).c_str(), TanksInNum, TanksEmptyNum);

  if (m_ReadyForPrint)
  {
     if (m_HandleStopAndResumePrinting)
     {
         m_HandleStopAndResumePrinting = false;// bug 6174
         m_BackEndInterface->ResumePrint();
         CQLog::Write(LOG_TAG_GENERAL, "%s: Resuming printing", ChamberToStr(m_Chamber).c_str());
     }
     ReturnValue(mrOk);
  }
  m_ReadyForPrint = (TanksInNum > TanksEmptyNum); // There at least one non-empty cartridge inserted (of the relevant type)

}
//---------------------------------------------------------------------------
int TCartridgeErrorForm::OpenCartridgeErrorDialog(int DlgType, TChamberIndex Chamber)
{
  if (!VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
     return 0;
       
  m_Chamber = Chamber;
  m_DlgType = DlgType;

  m_SecondsCounter = 0;
  m_MinutesCounter = 0;
  m_HoursCounter   = 0;
  
  QString tmp;

  int i = FIRST_TANK_INCLUDING_WASTE_TYPE;

  //Add first cartridge string
  try
  {
    // Following while may throw the exception if there's no tank associated with 'Chamber'.
	while(m_BackEndInterface->IsChambersTank(Chamber, i) == false)
	{
	   if(i>=LAST_TANK_INCLUDING_WASTE_TYPE)
		 throw EQException(QFormatStr("No tank is associated with M%d chamber.", Chamber));
	   i++;
	}
    // Showing an "MRW was not completed" error-dialog is done at the catch block below.
    // (identified by a segment with "UNDEFINED" material)
	/*if (TYPE_CHAMBER_WASTE != m_Chamber)
	  if (QString(CAppParams::Instance()->TypesArrayPerPipe[(TTankIndex)i]).compare("UNDEFINED") == 0)
		throw EQException("MRW was not completed"); */
          
    if (TYPE_CHAMBER_WASTE == m_Chamber)
	{
	   if(CAppParams::Instance()->DualWasteEnabled)
	   {
          tmp = "";
		  for(int i = FIRST_WASTE_TANK_TYPE; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
			if(i < LAST_TANK_INCLUDING_WASTE_TYPE - 1)
				tmp += TankToStr(static_cast<TTankIndex>(i)) + " and/or ";
			else
				tmp += TankToStr(static_cast<TTankIndex>(i));
		  tmp += " cartridge(s)";
	   }
	   else
	   {
		  tmp = TankToStr(static_cast<TTankIndex>(i)) + " cartridge"; // cartridge in "singular".
	   }

       if (DlgType == FE_INSERT_CARTRIDGE)
       {
          ErrorLabel->Caption = "Waste cartridge removal has been detected";
          tmp = "insert " + tmp;
       }
       else
       {
          ErrorLabel->Caption = "Waste cartridge is full";
          tmp = "empty " + tmp;
       }
    }
    else
	{
       int AssociatedCartridges = CAppParams::Instance()->CartridgeRedundancy+1; //Num of cartridges associated to chamber
	   QString ChamberMaterial = CAppParams::Instance()->TypesArrayPerChamber[m_Chamber];
	   bool AdditionalMaterial = false;
	   //Create the string with the relevant cartridges
	   for (int t=0; t<AssociatedCartridges && i<LAST_TANK_INCLUDING_WASTE_TYPE; t++,i++)
	   {
		 //check that the cartridge has the same material as the its chamber
		 if(ChamberMaterial.compare(CAppParams::Instance()->TypesArrayPerPipe[i]) == 0)
		 {
		   if (m_BackEndInterface->IsChambersTank(Chamber, i))
		   {
			 if(!AdditionalMaterial) //first entrance
			 {
			  tmp = TankToStr((TTankIndex)i);
			  AdditionalMaterial = true;
			 }
			 else
			  tmp+= " and/or " + TankToStr((TTankIndex)i);
		   }
		 }
	   }
       tmp = tmp + " cartridge(s)"; // cartridge in "plural".    
	   ErrorLabel->Caption         = (QString(CAppParams::Instance()->TypesArrayPerChamber[m_Chamber]) + " Material Required").c_str();
	   tmp = ((FE_INSERT_CARTRIDGE == DlgType) ? "Insert " : "Replace ") + tmp;
    }
	InstructionsLabel->Caption    = QFormatStr("%s\nor press 'Stop' to terminate printing",tmp.c_str()).c_str();
  }
  catch(EQException& Exception)
  {
	  ErrorLabel->Caption = "No cartridge may be used";
	  InstructionsLabel->Caption = Exception.GetErrorMsg().c_str();
  }
  catch(...)
  {
    ErrorLabel->Caption = "No cartridge may be used";
	InstructionsLabel->Caption = QFormatStr(LOAD_STRING(IDS_NOT_COMPLETED), WIZARD_TITLE(IDS_RR_WIZARD), StopButton->Caption.c_str(), WIZARD_TITLE(IDS_RR_WIZARD)).c_str();
  }

  this->Caption = "Elapsed Time 00:00:00";
  ErrorLabel->Width             = this->Width;
  ErrorLabel->Left              = 0;
  InstructionsLabel->Width      = this->Width;
  InstructionsLabel->Left       = 0;
  CartridgeStatusTimer->Enabled = true;
  CartridgeStatusTimer->Interval = 500;  // should be >= DELAY_BETWEEN_EACH_OCB_STATUS_VERIFICATION_MIL  that is defined in OCBStatus.cpp
  ElapsedTimeTimer->Enabled     = true;
  m_ReadyForPrint               = false;
  m_TanksStatus.clear();
  
  for (int t = FIRST_TANK_INCLUDING_WASTE_TYPE; t < LAST_TANK_INCLUDING_WASTE_TYPE; t++)
    if (m_BackEndInterface->IsChambersTank(m_Chamber, t))
        m_TanksStatus[(TTankIndex)t] = TANK_OUT;

  UpdateTanksState();  

  //window is already opened
  if (Visible) //bug 5954
    return 0;
    
  if (m_Modal)
  {
    return ShowModal();
  }
  else
  {
    Show();
    return Q_NO_ERROR;
  }
}
//---------------------------------------------------------------------------
void TCartridgeErrorForm::UpdateTanksState(void)
{
	TTankStatus Status;
	TTankStatus OldStatus;
	
	for (int t = FIRST_TANK_INCLUDING_WASTE_TYPE; t < LAST_TANK_INCLUDING_WASTE_TYPE; t++)
	{
		if (m_TanksStatus.find(static_cast<TTankIndex>(t)) != m_TanksStatus.end())
		{
			OldStatus = m_TanksStatus[static_cast<TTankIndex>(t)];

			if(m_BackEndInterface->IsTankInAndEnabled(t))
			{
				if (!m_BackEndInterface->IsTankWeightOk(t, true))//Check for hysteresys
				{  // The tank seems to be empty

					// If the tank has been just inserted
					if (QTicksToSeconds(QGetTicks() - m_BackEndInterface->GetLastInsertionTimeInTicks(static_cast<TTankIndex>(t))) <
							CAppParams::Instance()->EmptyCartridgeDetectionDelayInSec)
					{
						Status = OldStatus; // Give the user the chance to release the tank, and weight sensor to respond
					}
					else  // The tank was inserted more then EmptyCartridgeDetectionDelayInSec seconds ago
					{
						Status = TANK_IN_EMPTY; // The tank is realy empty
					}  
				}
				else
				{
					if (QTicksToSeconds(QGetTicks() - m_BackEndInterface->GetLastInsertionTimeInTicks(static_cast<TTankIndex>(t))) <
							CAppParams::Instance()->NonEmptyCartridgeDetectionDelayInSec)
					{
						Status = OldStatus; // Give the user the chance to release the tank, and weight sensor to respond
					}
					else  // The tank was inserted more then NonEmptyCartridgeDetectionDelayInSec seconds ago
					{
						Status = TANK_IN; // The tank is realy non-empty
					}
				}
			}
			else
			{
				Status = TANK_OUT;
			}  

			if((Status != OldStatus) &&
					/* This is to prevent impossible IN_EMPTY->IN situatation
				The correct sequance is IN_EMPTY->OUT->IN */
					((OldStatus != TANK_IN_EMPTY) || (Status != TANK_IN)))
			{
				m_TanksStatus[(TTankIndex)t] = Status;
				break;
			}
		}
	}
}//UpdateTanksState
//---------------------------------------------------------------------------
int TCartridgeErrorForm::CountTanksEmptyNum(void)
{
	int ret = 0; 
	for(TTankIndex2TTankStatusMapIter iter = m_TanksStatus.begin(); iter!=m_TanksStatus.end(); ++iter)
	{
		if(TANK_IN_EMPTY == iter->second)
			ret++;
	}
	return ret;
}//CountTanksEmptyNum
//---------------------------------------------------------------------------
int TCartridgeErrorForm::CountTanksInNum(void)
{
	int ret = 0;
	for(TTankIndex2TTankStatusMapIter iter = m_TanksStatus.begin(); iter != m_TanksStatus.end(); ++iter)
	{
		if((TANK_IN_EMPTY == iter->second) || (TANK_IN == iter->second))
			ret++;
	}
	return ret;
}//CountTanksInNum
//---------------------------------------------------------------------------
TQErrCode TCartridgeErrorForm::ShowDialog(int DialogType, int Cartridge, bool Modal)
{
  m_Modal                       = Modal;
  ModalResult                   = mrNone;
  m_HandleStopAndResumePrinting = false;
  m_HandleStopPrinting          = true;
  m_WasteRemovalDetected        = false;

  m_BackEndInterface->HandleCartridgeErrorDlgClose(FE_CARTRIDGE_DLG_RESULT_NONE);
  m_BackEndInterface->ActivateSignalTower( ST_LIGHT_NC, ST_LIGHT_NC, ST_LIGHT_BLINK, true );
  if (Modal)
  {
    SendMessage(Handle,WM_SHOW_DIALOG,DialogType,Cartridge);
	m_BackEndInterface->ActivateSignalTower( ST_LIGHT_NC, ST_LIGHT_NC, ST_LIGHT_OFF, true );
    if (ModalResult == mrCancel)
      return Q2RT_CARTRIDGE_ERR_DLG_STOP;
    else
      return Q_NO_ERROR;
  }
  PostMessage(Handle,WM_SHOW_DIALOG,DialogType,Cartridge);
  return Q_NO_ERROR;
}
//---------------------------------------------------------------------------
TQErrCode TCartridgeErrorForm::InformStopToDlg()
{
  PostMessage(Handle,WM_INFORM_PRINTING_STOP, 0, 0.0);
  return Q_NO_ERROR;
}
// Message handler for the WM_SHOW_DIALOG message
void TCartridgeErrorForm::HandleShowDialogMessage(TMessage &Message)
{
  OpenCartridgeErrorDialog(Message.WParam, (TChamberIndex)Message.LParam);
}
//---------------------------------------------------------------------------
void TCartridgeErrorForm::SetHandleStopAndResume(bool HandleStopAndResume)
{
  m_WastePrevOnOff = m_BackEndInterface->IsWasteActive();

  // Turning off the Waste pump and the Roller pump
  m_BackEndInterface->ActivateRollerAndPurgeWaste(false);

  m_HandleStopAndResumePrinting = HandleStopAndResume;
  m_HandleStopPrinting          = m_HandleStopAndResumePrinting;
}
//---------------------------------------------------------------------------
TModalResult TCartridgeErrorForm::ReturnValue(TModalResult aModalResult)
{
  ModalResult = aModalResult;
  if (!m_Modal)
  {
     Close();
	 CQLog::Write(LOG_TAG_GENERAL, "%s: Done with %d value", ChamberToStr(m_Chamber).c_str(), aModalResult);
	 m_BackEndInterface->ActivateSignalTower( ST_LIGHT_NC, ST_LIGHT_NC, ST_LIGHT_OFF, true );
  }
  return aModalResult;
}




/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TCartridgeErrorForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  if (!m_Modal)
  {
    if (ModalResult == mrCancel)
	  m_BackEndInterface->HandleCartridgeErrorDlgClose(FE_CARTRIDGE_DLG_RESULT_CANCEL);
    else if (ModalResult == mrOk)
      m_BackEndInterface->HandleCartridgeErrorDlgClose(FE_CARTRIDGE_DLG_RESULT_OK);
  }

  CartridgeStatusTimer->Enabled = false;
  ElapsedTimeTimer->Enabled     = false;
}
//---------------------------------------------------------------------------
void __fastcall TCartridgeErrorForm::StopButtonClick(TObject *Sender)
{
  if (m_HandleStopPrinting)
     m_BackEndInterface->StopPrint(Q2RT_EMBEDDED_RESPONSE);

  ReturnValue(mrCancel);
}
//---------------------------------------------------------------------------
void __fastcall TCartridgeErrorForm::WasteStatusTimerTimer(TObject *Sender)
{
    if (m_DlgType == FE_INSERT_CARTRIDGE)
	{
	  if(m_BackEndInterface->AreRelevantTanksInserted(TYPE_CHAMBER_WASTE, ACTIVE_TANK)) // needs to be checked if working OK !!!
      {
        if (m_HandleStopAndResumePrinting)
        {
          m_HandleStopAndResumePrinting = false;// bug 6174
          m_BackEndInterface->ResumePrint();
          if(m_WastePrevOnOff)
            // Turning on the Waste pump and the Roller pump
			m_BackEndInterface->ActivateRollerAndPurgeWaste(true);
        }
        ReturnValue(mrOk);
      }
    }
    else
    {
	  if (!m_BackEndInterface->AreRelevantTanksInserted(TYPE_CHAMBER_WASTE, ACTIVE_TANK)) // needs to be checked if working OK !!!
        m_WasteRemovalDetected = true;

      // Redesign this check !!! (Slava) , OBJET_MACHINE_KESHET, needs refactorring
	  if (m_WasteRemovalDetected && (m_BackEndInterface->IsTankInAndEnabled(TYPE_TANK_WASTE_LEFT) ||
									 m_BackEndInterface->IsTankInAndEnabled(TYPE_TANK_WASTE_RIGHT))
	                             && m_BackEndInterface->IsWeightOk(TYPE_CHAMBER_WASTE))
      {
      	// todo -oNobody -cNone: Looks like an unreachable code
        if (m_HandleStopAndResumePrinting)
        {
          m_HandleStopAndResumePrinting = false;// bug 6174
          m_BackEndInterface->ResumePrint();

          if(m_WastePrevOnOff)
            // Turning on the Waste pump and the Roller pump
            m_BackEndInterface->ActivateRollerAndPurgeWaste(true);
        }
        ReturnValue(mrOk);
      }
    }
}
//---------------------------------------------------------------------------
void __fastcall TCartridgeErrorForm::FormCreate(TObject *Sender)
{
  m_BackEndInterface = CBackEndInterface::Instance();
}
//---------------------------------------------------------------------------
void __fastcall TCartridgeErrorForm::ElapsedTimeTimerTimer(TObject *Sender)
{

  AnsiString CurrentTime       = "";
  AnsiString SecondsPaddingStr = "0";
  AnsiString MinutesPaddingStr = "0";
  AnsiString HoursPaddingStr   = "0";

  // Updating the time
  m_SecondsCounter++;

  if (SECONDS_IN_MINUTE == m_SecondsCounter)
  {
    m_SecondsCounter = 0;
    m_MinutesCounter++;
  }

  // In case there are less than 10 seconds - pad with additional "0"
  SecondsPaddingStr = ((m_SecondsCounter < 10) ? "0" : "");
  if (MINUTES_IN_HOUR == m_MinutesCounter)
  {
    m_MinutesCounter = 0;
    m_HoursCounter++;
  }

  // In case there are less than 10 minutes - pad with additional "0"
  MinutesPaddingStr = ((m_MinutesCounter < 10) ? "0" : "");
  // In case there are less than 10 hours - pad with additional "0"
  HoursPaddingStr   = ((m_HoursCounter < 10) ? "0" : "");
  CurrentTime = "Elapsed Time ";
  // Hours
  CurrentTime += HoursPaddingStr   + IntToStr(m_HoursCounter) + ":";
  // Minutes
  CurrentTime += MinutesPaddingStr + IntToStr(m_MinutesCounter) + ":";
  // Seconds
  CurrentTime += SecondsPaddingStr + IntToStr(m_SecondsCounter);

  this->Caption = CurrentTime;

}
//---------------------------------------------------------------------------
// Message handler for the WM_INFORM_PRINTING_STOP message
void TCartridgeErrorForm::HandleInformStopToDlg(TMessage &Message)
{
  m_HandleStopAndResumePrinting = false;
  m_HandleStopPrinting          = false;
  ReturnValue(mrCancel);
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
