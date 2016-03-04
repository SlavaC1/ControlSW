
#include "SinglePointWeightCalibrationWizard.h"
#include "QMonitor.h"
#include "BackEndInterface.h"
#include "AppParams.h"
#include "AppLogFile.h"
#include "StableValueWizardPage.h"
#include "StableValueFrame.h"
#include "Q2RTApplication.h"
#include "WizardImages.h"
#include "LoadCellCalibration.h"
#include "CustomLoadCellCalibrationPage.h"
#include "CustomLoadCellWellcomeFrame.h"
#include "WeightsStatusWizardPage.h"
#include "MaintenanceCountersDefs.h" //itamar

void ExternalAppExecute(const char *FileName,const char *CmdLineParam);

#define SP_REMOVE_CATRIDGES_STR(Tank)  \
        QFormatStr(LOAD_STRING(IDS_REMOVE_RESIN_TYPE_CATRIDGES),TankToStr((TTankIndex)Tank).c_str(),LOAD_STRING((Tank == TYPE_TANK_WASTE) ? IDS_CONTAINER : IDS_CARTRIDGE))

#define SP_REMOVED_CATRIDGES_STR(Tank) \
        QFormatStr(LOAD_STRING(IDS_REMOVED_RESIN_TYPE_CATRIDGES),TankToStr((TTankIndex)Tank).c_str(),LOAD_STRING((Tank == TYPE_TANK_WASTE) ? IDS_CONTAINER : IDS_CARTRIDGE))

enum {spWelcomPage,    
	  spChooseMode,
	  #ifndef OBJET_MACHINE
	  spRemoveTank,
	  #endif
      spCalibrateTank,
	  spDonePage,
      
      spPagesCount // this must element must be the last in this enum
};        


CSinglePointWeightCalibrationWizard::CSinglePointWeightCalibrationWizard(void) : CQ2RTAutoWizard(IDS_SP_WIZARD, true, IN_PROCESS_IMAGE_ID)
{
  TPagePointers pagesVector(spPagesCount, 0);
  {
	#ifndef OBJET_MACHINE
	{
		CMessageWizardPage *pPage = new CMessageWizardPage(GetTitle(),GENERAL_PURPOSE_2_IMAGE_ID, wpPreviousDisabled);
		pagesVector[spWelcomPage] = pPage;
	}
	#endif
	#ifdef OBJET_MACHINE
	{
		CLoadCellWellcomeWizardPage *pPage = new CLoadCellWellcomeWizardPage(GetTitle(),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
		pPage->m_imageIndex = IDB_LOAD_CELL_WELCOME_TXT_IMAGE_ID;
		pagesVector[spWelcomPage] = pPage;
	}
	#endif
  }
  {
	#ifndef OBJET_MACHINE
	{
	CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage(LOAD_STRING(IDS_SP_CHOOSE_MODE),-1,wpNextOneChecked | wpPreviousDisabled | wpHelpNotVisible);
	for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
	  pPage->Strings.Add(QFormatStr(LOAD_STRING(IDS_SP_CALIBRATE_WEIGHT),TankToStr((TTankIndex)i).c_str()));
	pagesVector[spChooseMode] = pPage;
	}
	#endif
	#ifdef OBJET_MACHINE
	{
	 CCustomLoadCellCalibrationPage *pPage = new CCustomLoadCellCalibrationPage(LOAD_STRING(IDS_SP_CHOOSE_CARTRIDGES),MATERIALS_CABINET_PAGE_IMAGE_ID, wpPreviousDisabled);
	 pPage->m_SubTitle = "Remove each cartridge you selected from the cart.";
	 pagesVector[spChooseMode] = pPage;
	}
	#endif
  }
  {
  #ifndef OBJET_MACHINE
	{
    CCheckBoxWizardPage *pPage = new CCheckBoxWizardPage("",-1,wpNextWhenSelected | wpPreviousDisabled | wpClearOnShow | wpHelpNotVisible);
    pPage->Strings.Add("");
	pagesVector[spRemoveTank] = pPage;
	}
	#endif
  }
  {
	#ifndef OBJET_MACHINE
	{
	 CStableValueWizardPage *pPage = new CStableValueWizardPage(LOAD_STRING(IDS_SP_WEIGHT_STATUS),-1, wpNextDisabled ) ;
	 pPage->GroupBoxCaption = "";
	 pPage->UnitsCaption    = LOAD_STRING(IDS_AD);
	 pPage->CheckBoxText    = LOAD_STRING(IDS_SP_WEIGHT_STABLE);
	 pPage->CurrValue       = 0;
		pagesVector[spCalibrateTank] = pPage;
	}
	#endif
	#ifdef OBJET_MACHINE
	{
	 CWeightsStatusWizardPage *pPage = new CWeightsStatusWizardPage(LOAD_STRING(IDS_SP_WEIGHT_STATUS_MARK_STABLE),PREPARATIONS_IMAGE_ID, wpPreviousDisabled |wpNextDisabled ) ;
	 pPage->SetSubTitle(LOAD_QSTRING(IDS_SP_WEIGHT_STATUS_GUIDE_STABLE));
	 pPage->GroupBoxCaption = "";
	 pPage->UnitsCaption    = LOAD_STRING(IDS_AD);
	 pPage->CheckBoxText    = LOAD_STRING(IDS_SP_WEIGHT_STABLE);
		pagesVector[spCalibrateTank] = pPage;
	}
	#endif
  }
  {
	#ifndef OBJET_MACHINE
	{
		CMessageWizardPage *pPage = new CMessageWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
		pagesVector[spDonePage] = pPage;
	}
	#endif
	#ifdef OBJET_MACHINE
	{
        CLoadCellWellcomeWizardPage*pPage = new CLoadCellWellcomeWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
		pPage->m_imageIndex = IDB_LOAD_CELL_COMPLETE_TXT_IMAGE_ID;
		pagesVector[spDonePage] = pPage;
	}
	#endif
  }

  for(TPagePointersIterator p = pagesVector.begin(); p != pagesVector.end(); p++)
   	AddPage(*p);

};


// Start the wizard session event
void CSinglePointWeightCalibrationWizard::StartEvent()
{
}

void CSinglePointWeightCalibrationWizard::PageEnter(CWizardPage *WizardPage)
{
  switch(WizardPage->GetPageNumber())
  {
	 case spDonePage:
	 {
         m_BackEnd->ResetMaintenanceCounter(LOAD_CELL_CALIBRATION_COUNTER_ID);
     }
	 break;
	 case spChooseMode:
	{
	  #ifdef OBJET_MACHINE
	  {
		 if(m_needToResetData == true)
		{
			CCustomLoadCellCalibrationPage *Page = dynamic_cast<CCustomLoadCellCalibrationPage *>(WizardPage);
			Page->SetImageID(MATERIALS_CABINET_PAGE_IMAGE_ID);
			Page->ResetCheckBoxView(true);
			Page->m_SubTitle = "Remove each cartridge you selected from the cart.";
			Page->Title = LOAD_STRING(IDS_SP_CHOOSE_CARTRIDGES);
			Page->Refresh();
		}
		else
			{
			    CCustomLoadCellCalibrationPage *Page = dynamic_cast<CCustomLoadCellCalibrationPage *>(WizardPage);
				Page->SetImageID(CARTRIDGE_IMAGE_ID);
				Page->Title = "Remove the following cartridges";
				Page->m_SubTitle = "Before continuing,you must remove the cartridges shown.";
				Page->Refresh();
			}
	  }
	  #endif
	  break;
	}
	#ifndef OBJET_MACHINE
  {
	case spRemoveTank:
	{
		  CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
		  Page->Title      = SP_REMOVE_CATRIDGES_STR(m_CartridgeIndex);
		  Page->Strings[0] = SP_REMOVED_CATRIDGES_STR(m_CartridgeIndex);
		  Page->Refresh();
		break;
	 }
	}
		#endif
	case spCalibrateTank:
	{
	  #ifndef OBJET_MACHINE
	  {
		CStableValueWizardPage* Page = dynamic_cast<CStableValueWizardPage *>(WizardPage);
		Page->Title                  = TankToStr((TTankIndex)m_CartridgeIndex) + " " + LOAD_QSTRING(IDS_SP_WEIGHT_STATUS);
		// Update the Page...
		while (GetCurrentPageIndex() == spCalibrateTank)
	  {
		{
			// Retrieve Current & Average vacuum values...
			Page->CurrValue = (int)m_BackEnd->GetTankWeightInA2D(m_CartridgeIndex);
			Page->Refresh();
			YieldAndSleepWizardThread();
		}
	  }
	  #endif
	  #ifdef OBJET_MACHINE
	  {
	   CWeightsStatusWizardPage* Page = dynamic_cast<CWeightsStatusWizardPage *>(WizardPage);
	   Page->Title    =  LOAD_QSTRING(IDS_SP_WEIGHT_STATUS_MARK_STABLE);
		// Update the Page...
		while ((GetCurrentPageIndex() == spCalibrateTank) && (IsCancelled() != true))
		{
			for(int i = 0; i<TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
			{

			// Retrieve Current & Average vacuum values...
			Page->m_WeightsStatusWizardPageData[i]->m_CurrValue = (int)m_BackEnd->GetTankWeightInA2D(i);
			Page->m_WeightsStatusWizardPageData[i]->m_enable =  !m_SensorsToCalibrate[i];
			if(GetCurrentPageIndex() == spCalibrateTank)
				{
				Page->Refresh();
				YieldAndSleepWizardThread();
				}
			}
		}
		YieldAndSleepWizardThread();
		Page->Title    = "";
		break;
	  }
	  #endif
	  break;
	}
	break;
	default: break;
  }
}

void CSinglePointWeightCalibrationWizard::PageLeave(CWizardPage *WizardPage, TWizardPageLeaveReason LeaveReason)
{
  if (LeaveReason != lrGoNext)
  {
	 return;
  }

  switch(WizardPage->GetPageNumber())
  {
  case spWelcomPage:
	{
	  #ifdef OBJET_MACHINE
	  m_needToResetData = true;
	  #endif
	  break;
	}
	case spChooseMode:
	{
		#ifndef OBJET_MACHINE
		{
		CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
		 unsigned ChecksMask = Page->GetChecksMask();
		 for(int i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
			 m_SensorsToCalibrate[i] = (ChecksMask  & (1 << i));
         m_CartridgeIndex = 0;
		 if (m_SensorsToCalibrate[0] == false)
			FindNextToCalibrate();

		 break;
		}
		#endif
		#ifdef OBJET_MACHINE
		{
		CCustomLoadCellCalibrationPage *Page = dynamic_cast<CCustomLoadCellCalibrationPage *>(WizardPage);
		bool OutputArray[TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE]; //Model1L,Model2L,Model3L,SupportL,Model1R,Model2R,Model3R,SupportR
		if(	m_needToResetData == true)
		{
		    m_needToResetData = false;
			Page->Refresh();
			Page->GetCartridgesArray(OutputArray);
			SetSensorCalibrateArrayAccordingToEnum(OutputArray);
		}
		//check that all cartridges were removed
		bool goToChooseMode = false;
		for(int i = 0 ; i< TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
		 {
			 if(m_SensorsToCalibrate[i] == true)
			 {
				//If tank is not really removed - show remove tank page again
				if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation))
				{
					goToChooseMode = true;
				 //	 SetNextPage(spChooseMode);
				 Page->m_ActiveCheckBoxes[i] = true;
				}
				else
				if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation))
				{
				 Page->m_ActiveCheckBoxes[i] = false;
				}
			 }
			 else
               Page->m_ActiveCheckBoxes[i] = false;
		 }
		 if(goToChooseMode == true)
		 {
			Page->SetImageID(CARTRIDGE_IMAGE_ID);
			SetNextPage(spChooseMode);
		 }
		 else
			{
			 Page->SetImageID(MATERIALS_CABINET_PAGE_IMAGE_ID);
			  SetNextPage(spCalibrateTank);
			}
		break;
		}
		#endif
	break;
	}
 #ifndef OBJET_MACHINE
	{
		case spRemoveTank:
		{
		 CCheckBoxWizardPage *Page = dynamic_cast<CCheckBoxWizardPage *>(WizardPage);
		 bool IsInserted = m_BackEnd->IsRelevantTankInserted(m_CartridgeIndex);
		 //If tank is not really removed - show remove tank page again
		 if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && IsInserted == true)
		 {
			 SetNextPage(spRemoveTank);
			 break;
		 }
		 Page->SubTitle = "";
		 break;
		}
	}
 #endif
	case spCalibrateTank:
	{
		#ifndef OBJET_MACHINE
		{
		  float TankEmptyWeight = (int)m_BackEnd->GetTankWeightInA2D(m_CartridgeIndex);
		  float TankGain        = m_ParamMgr->WeightSensorGainArray[m_CartridgeIndex];
		   int   NewOffset       = - (TankGain * TankEmptyWeight);  // y = Gain*X + Offset

		   m_BackEnd->SetWeightSensorOffset(m_CartridgeIndex, NewOffset);
		   FindNextToCalibrate();
		   if (VALIDATE_TANK_INCLUDING_WASTE(m_CartridgeIndex))
		   SetNextPage(spRemoveTank);
		   else
			 m_CartridgeIndex = 0;
		  break;
		}
		#endif
		#ifdef OBJET_MACHINE
           for(int i = 0; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
				{
				 if ( m_SensorsToCalibrate[i]  == true)
				 {
					 float TankEmptyWeight = (int)m_BackEnd->GetTankWeightInA2D(i);
					 float TankGain        = m_ParamMgr->WeightSensorGainArray[i];
					 int   NewOffset       = - (TankGain * TankEmptyWeight);  // y = Gain*X + Offset

					  m_BackEnd->SetWeightSensorOffset(i, NewOffset);
				 }
				}
		#endif
		break;
	}
	default:break;
  }
}

void CSinglePointWeightCalibrationWizard::FindNextToCalibrate()
{
   for(int i = ++m_CartridgeIndex; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
   {
	   if ( m_SensorsToCalibrate[i] )
       {
          m_CartridgeIndex = i;
          return;
       }
   }
   m_CartridgeIndex = LAST_TANK_INCLUDING_WASTE_TYPE;
}

void CSinglePointWeightCalibrationWizard::HelpEvent(CWizardPage *WizardPage)
{
	Application->HelpFile = DefaultHelpFileName().c_str();

	switch(WizardPage->GetPageNumber())
	{
		case spChooseMode:
        	Application->HelpContext(15); break;
		case spWelcomPage:
			Application->HelpContext(10); break;
		case spCalibrateTank: Application->HelpContext(20); break;
        default: break;
	}
}
void CSinglePointWeightCalibrationWizard::SetSensorCalibrateArrayAccordingToEnum(bool SensorCalibrateArray[])
{
  for(int i= 0; i< TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
  {
	m_SensorsToCalibrate[i] = SensorCalibrateArray[i];
  }
}
