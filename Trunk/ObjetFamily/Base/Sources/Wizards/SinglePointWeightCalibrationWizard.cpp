
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
      spCalibrateTank,
	  spDonePage,
      spPagesCount // this must element must be the last in this enum
};        


CSinglePointWeightCalibrationWizard::CSinglePointWeightCalibrationWizard(void) : CQ2RTAutoWizard(IDS_SP_WIZARD, true, IN_PROCESS_IMAGE_ID)
{

	memset (m_SensorsToCalibrate, 0, sizeof(m_SensorsToCalibrate))  ;
	m_TwoTanksPerMaterial = false;
	m_CartridgeIndex =0 ;
	m_needToResetData = false;
	if(CAppParams::Instance()->DualWasteEnabled == true)
		m_NumOfContainersIncludingWaste = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;
	else
		m_NumOfContainersIncludingWaste = TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE-1;

	TPagePointers pagesVector(spPagesCount, 0);
	{
//OBJET_MACHINE adjustment
	  CLoadCellWellcomeWizardPage *pPage = new CLoadCellWellcomeWizardPage(GetTitle(),IN_PROCESS_IMAGE_ID,wpPreviousDisabled | wpHelpNotVisible);
	  pPage->m_imageIndex = IDB_LOAD_CELL_WELCOME_TXT_IMAGE_ID;
	  pagesVector[spWelcomPage] = pPage;
	}

//OBJET_MACHINE adjustment
	{
	  CCustomLoadCellCalibrationPage *pPage = new CCustomLoadCellCalibrationPage(LOAD_STRING(IDS_SP_CHOOSE_CARTRIDGES),LOAD_CELL_CARTIDGE_IMAGE_ID, wpPreviousDisabled);
	  pPage->m_SubTitle = "Remove each cartridge you selected from the cabinet.";
	  pagesVector[spChooseMode] = pPage;
	}

//OBJET_MACHINE adjustment
	{
	  CWeightsStatusWizardPage *pPage = new CWeightsStatusWizardPage(LOAD_STRING(IDS_SP_WEIGHT_STATUS_MARK_STABLE),PREPARATIONS_IMAGE_ID, wpPreviousDisabled |wpNextDisabled ) ;
	  pPage->SetSubTitle(LOAD_QSTRING(IDS_SP_WEIGHT_STATUS_GUIDE_STABLE));
	  pPage->GroupBoxCaption = "";
	  pPage->UnitsCaption    = LOAD_STRING(IDS_AD);
	  pPage->CheckBoxText    = LOAD_STRING(IDS_SP_WEIGHT_STABLE);
	  pagesVector[spCalibrateTank] = pPage;
	}

	{
	  CLoadCellWellcomeWizardPage*pPage = new CLoadCellWellcomeWizardPage(WIZARD_COMPLETED_STR,SUCCESSFULLY_COMPLETED_IMAGE_ID,wpDonePage | wpPreviousDisabled | wpCancelDisabled | wpHelpNotVisible);
	  pPage->m_imageIndex = IDB_LOAD_CELL_COMPLETE_TXT_IMAGE_ID;
	  pagesVector[spDonePage] = pPage;
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
//OBJET_MACHINE adjustment
			if(m_needToResetData == true)
			{
				CCustomLoadCellCalibrationPage *Page = dynamic_cast<CCustomLoadCellCalibrationPage *>(WizardPage);
				Page->SetImageID(LOAD_CELL_WEIGHT_STATUS_IMAGE_ID);
				Page->ResetCheckBoxView(true);

				if ( CAppParams::Instance()->CartridgeDrawerExists == true )
				{
					Page->m_SubTitle = "Remove each cartridge you selected and close the panels.";
				}
				else if(CAppParams::Instance()->NumOfMaterialCabinets > 0)
				{
					Page->m_SubTitle = "Remove each cartridge you selected from the cabinet.";
					Page->SetImageID( LOAD_CELL_CARTIDGE_IMAGE_ID );
				}
				else
				{
					Page->m_SubTitle = "Remove each cartridge you selected.";
				}

				Page->Title = LOAD_STRING(IDS_SP_CHOOSE_CARTRIDGES);
				Page->Refresh();
			}
			else
			{
				CCustomLoadCellCalibrationPage *Page = dynamic_cast<CCustomLoadCellCalibrationPage *>(WizardPage);
				Page->SetImageID(LOAD_CELL_WEIGHT_STATUS_IMAGE_ID);

				if ( CAppParams::Instance()->CartridgeDrawerExists == true )
				{
					Page->m_SubTitle = "Remove the following cartridges and close the panels.";
				}
				else if(CAppParams::Instance()->NumOfMaterialCabinets > 0)
				{
					Page->m_SubTitle = "Remove the following cartridges from the cart.";
					Page->SetImageID( LOAD_CELL_CARTIDGE_IMAGE_ID );
				}
				else
				{
					Page->m_SubTitle = "Remove the following cartridges.";
				}

				Page->Title = "Remove the following cartridges";
				Page->Refresh();
			}
			break;
		}
		case spCalibrateTank:
		{
//OBJET_MACHINE adjustment
			CWeightsStatusWizardPage* Page = dynamic_cast<CWeightsStatusWizardPage *>(WizardPage);
			Page->Title                    =  LOAD_QSTRING(IDS_SP_WEIGHT_STATUS_MARK_STABLE);
			
			// Update the Page...
			while ((GetCurrentPageIndex() == spCalibrateTank) && (IsCancelled() != true))
			{
				for(int i = 0; i < m_NumOfContainersIncludingWaste; i++)
				{
					// Retrieve Current & Average vacuum values...
					Page->m_WeightsStatusWizardPageData[i]->m_CurrValue = (int)m_BackEnd->GetTankWeightInA2D(i);
					Page->m_WeightsStatusWizardPageData[i]->m_enable    = ! m_SensorsToCalibrate[i];
					
					if(GetCurrentPageIndex() == spCalibrateTank)
					{
						Page->Refresh();
						YieldAndSleepWizardThread();
					}
				}
			}
			
			YieldAndSleepWizardThread();
			Page->Title = "";
			break;
		}
			
		default: 
			break;		
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
//OBJET_MACHINE adjustment
	  m_needToResetData = true;
	  break;
	}
	case spChooseMode:
	{
//OBJET_MACHINE adjustment
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
                    for(int i = 0 ; i< m_NumOfContainersIncludingWaste;i++)
                     {
                             if(m_SensorsToCalibrate[i] == true)
                             {
                                     bool IsInserted = m_BackEnd->IsMicroSwitchInserted(i);
                                    //If tank is not really removed - show remove tank page again
                                    if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && IsInserted == true)
                                    {
                                            goToChooseMode = true;
                                     //	 SetNextPage(spChooseMode);
                                     Page->m_ActiveCheckBoxes[i] = true;
                                    }
                                    else
                                    if(CHECK_NOT_EMULATION(m_ParamMgr->OCB_Emulation) && IsInserted == false)
                                    {
                                     Page->m_ActiveCheckBoxes[i] = false;
                                    }
                             }
                             else
                                 Page->m_ActiveCheckBoxes[i] = false;
                     }
                     if(goToChooseMode == true)
                     {
                            Page->SetImageID(LOAD_CELL_REMOVE_CARTIDGE_IMAGE_ID);
                            SetNextPage(spChooseMode);
                     }
                     else
                     {
                             Page->SetImageID(LOAD_CELL_CARTIDGE_IMAGE_ID);
                              SetNextPage(spCalibrateTank);
                     }
					break;
	}
	case spCalibrateTank:
	{
//OBJET_MACHINE adjustment
           for(int i = 0; i < LAST_TANK_INCLUDING_WASTE_TYPE; i++)
				{
				 if ( m_SensorsToCalibrate[i]  == true)
				 {
					 float TankEmptyWeight = (int)m_BackEnd->GetTankWeightInA2D(i);
					 float TankGain        = m_ParamMgr->WeightSensorGainArray[i];
					 float NewOffset       = - (TankGain * TankEmptyWeight);  // y = Gain*X + Offset

					  m_BackEnd->SetWeightSensorOffset(i, NewOffset);
				 }
				}
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
		case spChooseMode:Application->HelpContext(15); break;
		case spWelcomPage: Application->HelpContext(10); break;
		case spCalibrateTank: Application->HelpContext(20); break;
        default: break;
	}
}
void CSinglePointWeightCalibrationWizard::SetSensorCalibrateArrayAccordingToEnum(bool SensorCalibrateArray[])
{
  for(int i= 0; i< m_NumOfContainersIncludingWaste;i++)
  {
	m_SensorsToCalibrate[i] = SensorCalibrateArray[i];
  }
}
