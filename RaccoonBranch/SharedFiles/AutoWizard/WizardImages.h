/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Generic                                                 *
 * Module: Automatic wizards generator                              *
 * Module Description: Wizard viewing frame.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/01/2003                                           *
 * Last upate: 07/03/2003                                           *
 ********************************************************************/

#ifndef WizardImagesH
#define WizardImagesH

//---------------------------------------------------------------------------
#include <dir.h>
#include "QTypes.h"
//---------------------------------------------------------------------------

// General
const int IN_PROCESS_IMAGE_ID             = 0;
const int SUCCESSFULLY_COMPLETED_IMAGE_ID = 1;
const int CAUTION_IMAGE_ID         		  = 2;
const int GENERAL_PURPOSE_1_IMAGE_ID      = 3;
const int GENERAL_PURPOSE_2_IMAGE_ID      = 4;
const int GENERAL_PURPOSE_3_IMAGE_ID      = 5; // In progress page...
const int GENERAL_PURPOSE_4_IMAGE_ID      = 6;
const int INSERT_THE_TRAY_IMAGE_ID        = 7;
const int CANCEL_PAGE_IMAGE_ID            = 8;
const int PREPARATIONS_IMAGE_ID           = 9;
const int MATERIALS_CABINET_PAGE_IMAGE_ID = 15;
const int CAUTION_QUESTION_IMAGE_ID       = 16;
const int CARTRIDGE_IMAGE_ID              = 17;
const int START_IMAGE_ID                  = 18;
const int QUESTION_IMAGE_ID            	  = 19;
const int HEADS_IMAGE_ID            	  = 20;

// Bumper Calibration Wizard
const int BUMPER_SENSOR_IMAGE_ID     = 21;

// Head Alignment Wizard
const int PLACE_TRANSPARENCY_IMAGE_ID       = 31;
const int REMOVE_TRANSPARENCY_IMAGE_ID      = 33;
const int CHOOSE_HEADS_TO_ALIGN_IMAGE_ID    = 34;
const int CHOOSE_BEST_LINE_IMAGE_ID         = 35;
const int SAVE_DATA_PAGE_IMAGE_ID           = 38;
// Printing Position Wizard

const int DIRECTION_IMAGE_ID = 60;

// Resin Replacement Wizard
const int RESIN_REPLACEMENT_MAIN_IMAGE = 70;
const int INSERT_MODEL_CARTRIDGE_IMAGE = 71;

// Quick SHR
const int LEVEL_HEADS_IMAGE_ID           = 82;
const int HEAD_ORDER_IMAGE_ID            = 84;
const int VACUUM_TEST_IMAGE_ID           = 86;

const int HEAD_ALIGNMENT_WIZARD_IMAGE_ID = 91;

// Support Replacement Wizard
const int SUPPORT_REPLACEMENT_MAIN_IMAGE = 100;
const int INSERT_SUPPORT_CARTRIDGE_IMAGE = 101;
const int SUPPORT_REPLACEMENT_QUICK_MODE = 102;

// UV Calibration Wizard
const int UV_WELCOME_PAGE_IMAGE_ID    = 110; // Default image
const int UV_LAMPS_WARMING_IMAGE_ID   = 111;
const int UV_LEFT_SCANNING_IMAGE_ID   = 112;
const int UV_PLACE_SENSOR_IMAGE_ID    = 113;
const int UV_RESET_SENSOR_IMAGE_ID    = 114;
const int UV_SENSOR_CALIBRATION       = 115;
const int UV_TRAY_EMPTY_IMAGE_ID      = 116;
const int UV_TURN_OFF_SENSOR_IMAGE_ID = 117;
const int UV_ENTER_VLAUE_IMAGE_ID     = 118;

// New UV Calibration Wizard
const int UV_WELCOME_PAGE_IMAGE_NEW_ID       = 1110; // Default image
const int UV_LAMPS_WARMING_IMAGE_NEW_ID      = 1111;
const int UV_LEFT_SCANNING_IMAGE_NEW_ID      = 1112;
const int UV_PLACE_SENSOR_IMAGE_NEW_ID       = 1113;
const int UV_SENSOR_SELECTION_IMAGE_NEW_ID   = 1115;
const int UV_TRAY_EMPTY_IMAGE_NEW_ID         = 1116;
const int UV_PREPARATIONS_IMAGE_NEW_ID       = 1117;
const int UV_ENTER_VALUE_IMAGE_NEW_ID        = 1118;
const int UV_EXTERNAL_SENSOR_CONNECTED_NEW_ID= 1119;
const int UV_LAMPS_STATE_NEW_ID              = 1120;
const int UV_STABILIZATION_TIME_NEW_ID       = 1121;
const int UV_READ_UV_VALUES_NEW_ID           = 1122;
const int UV_RESULTS_PAGE_NEW_ID             = 1123;
const int UV_FINALIZATIONS_PAGE_NEW_ID       = 1124;
const int UV_OPEN_INTERNAL_SENSOR_NEW_ID	 = 1125;
const int UV_REMOVE_SENSOR_IMAGE_NEW_ID		 = 1126;
const int UV_PLACE_EXT_SENSOR_IMAGE_NEW_ID	 = 1127;

// UV Replacement Wizard
const int UV_LAMP_IMAGE_ID        = 130;
const int SHADOW_BRACKET_IMAGE_ID = 131;
const int FULL_BLOCK_IMAGE_ID     = 132;
const int LAMPS_STATUS_IMAGE_ID   = 133;

// Vacuum Calibration Wizard
const int FIRST_PAGE_IMAGE_ID      = 140;
const int VACUUM_PANEL_IMAGE_ID    = 141;
const int VACUUM_7_CM_H2O_IMAGE_ID = 142;

// Clean Heads Wizard

// Clean Wiper Wizard
const int CW_MAIN_IMAGE_ID          = 160;

// Shutdown Wizard
const int SHD_CLEANING_FLUID_PICTURE_ID = 174;
const int SHD_INFORMATION_PICTURE_ID    = 175;

// Heads Service Wizard
const int HSW_HEADS_IN_PICTURE_ID       = 2005;
const int HSW_HEADS_OUT_PICTURE_ID      = 2006;
const int HSW_WEIGHT_TEST_PICTURE_ID    = 2008;
const int HSW_PLACE_PAPER_PICTURE_ID    = 2011;
const int HSW_REMOVE_PAPER_PICTURE_ID   = 2012;
const int HSW_SCALE_PLACE_PICTURE_ID    = 2013;
const int HSW_REMOVE_RESIN_PICTURE_ID = 2015;
// Wiper Calibration Wizard
const int WC_WELCOME_IID                = 3000;
const int WC_HOMING_IID                 = 3003;
const int WC_CALIBRATION_IID            = 3005;
const int WC_PURGE_WIPE_SELECTION_IID   = 3006;
const int WC_PURGING_IID                = 3007;
const int WC_WIPING_IID                 = 3008;
const int WC_ARE_RESULTSOK_IID          = 3010;
const int WC_T_HEIGHT_IID               = 3013;

// Tray Points Calibration Wizard
const int TP_TRAY_POINTS_IID				= 4000;
const int TP_TRAY_POINTS_REMOVE_COVER_IID 	= 4001;
const int TP_TRAY_POINTS_SCREWS_IID 	= 4002;
const int TP_TRAY_POINTS_TRAY_LEVEL_IID = 4003;

// Roller Tilt Wizard
const int RT_PLATE_SCREWS_IMAGE_ID      = 195;
const int RT_ADJUST_SCREWS_IMAGE_ID     = 196;

QString GetPageImage(int ImageID)
{
  char* CWD = getcwd(NULL, 100);
  QString ImageName = CWD;

  switch(ImageID)
  {
/////////////
// General //
/////////////
	case IN_PROCESS_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\InProcess.bmp";
	  break;

    case SUCCESSFULLY_COMPLETED_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\General\\Succesfully_Completed.bmp";
      break;

	case CAUTION_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Caution.bmp";
      break;

	case GENERAL_PURPOSE_1_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\General\\General_Purpose.bmp";
	  break;

    case GENERAL_PURPOSE_2_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\General\\General_Purpose2.bmp";
      break;

    case GENERAL_PURPOSE_3_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\General\\General_Purpose3.bmp";
      break;

    case GENERAL_PURPOSE_4_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\General\\General_Purpose4.bmp";
      break;

    case INSERT_THE_TRAY_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\General\\InsertTray.bmp";
      break;

	case CANCEL_PAGE_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Cancel.bmp";
	  break;
	case MATERIALS_CABINET_PAGE_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Materials_Cabinet.bmp";
	  break;
	case CAUTION_QUESTION_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Caution_Question.bmp";
	  break;
	case CARTRIDGE_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Cartridge.bmp";
	  break;
	case START_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Start.bmp";
	  break;
	case QUESTION_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Question.bmp";
	  break;
	case HEADS_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Heads.bmp";
	  break;
	case PREPARATIONS_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\General\\Preparations.bmp";
	  break;
///////////////////////////////
// Wiper Calibration Wizard  //
///////////////////////////////
    case WC_WELCOME_IID:
      ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_calibration_wizard.bmp";
      break;

    case WC_T_HEIGHT_IID:
      ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Adjusting_T_Axis_height.bmp";
      break;

    case WC_HOMING_IID:
      ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Homing_all_axes.bmp";
      break;

    case WC_CALIBRATION_IID:
      ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Moving_to_calibration_Position.bmp";
      break;

    case WC_PURGE_WIPE_SELECTION_IID:
    case WC_PURGING_IID:
    case WC_WIPING_IID:
      ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Calibration_Screen.bmp";
      break;

    case WC_ARE_RESULTSOK_IID:
      ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Test_Screen.bmp";
      break;

///////////////////////////////
// Bumper Calibration Wizard //
///////////////////////////////

    case BUMPER_SENSOR_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Bumper Calibration\\BumperSensor.bmp";
      break;

///////////////////////////
// Head Alignment Wizard //
///////////////////////////

    case PLACE_TRANSPARENCY_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Head Alignment\\PlaceTransparency.bmp";
      break;

    case REMOVE_TRANSPARENCY_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Head Alignment\\RemoveTransparency.bmp";
      break;

    case CHOOSE_HEADS_TO_ALIGN_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Head Alignment\\ChooseHeadToAlign.bmp";
      break;

    case CHOOSE_BEST_LINE_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Head Alignment\\ChooseBestLine.bmp";
      break;

    case SAVE_DATA_PAGE_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Head Alignment\\SaveData.bmp";
      break;

//////////////////////////////
// Printing Position Wizard //
//////////////////////////////
    case DIRECTION_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Printing Position\\Direction.bmp";
      break;

//////////////////////////////
// Resin Replacement Wizard //
//////////////////////////////
    case RESIN_REPLACEMENT_MAIN_IMAGE:
      ImageName += "\\Wizards\\Bin\\Resin Replacement\\MainPage.bmp";
      break;

    case INSERT_MODEL_CARTRIDGE_IMAGE:
      ImageName += "\\Wizards\\Bin\\Resin Replacement\\InsertCartridge.bmp";
      break;


///////////////
// Quick SHR //
///////////////

    case LEVEL_HEADS_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\LevelHeads.bmp";
      break;


    case HEAD_ORDER_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadOrder.bmp";
      break;


    case VACUUM_TEST_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\VacuumTest.bmp";
      break;


    case HEAD_ALIGNMENT_WIZARD_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Head Alignment\\Start.bmp";
      break;

////////////////////////////////
// Support Replacement Wizard //
////////////////////////////////
    case SUPPORT_REPLACEMENT_MAIN_IMAGE:
      ImageName += "\\Wizards\\Bin\\Support Replacement\\MainImg.bmp";
      break;

    case INSERT_SUPPORT_CARTRIDGE_IMAGE:
      ImageName += "\\Wizards\\Bin\\Support Replacement\\InsertCartridge.bmp";
      break;

    case SUPPORT_REPLACEMENT_QUICK_MODE:
      ImageName += "\\Wizards\\Bin\\Support Replacement\\QuickMode.bmp";
      break;

///////////////////////////
// UV Calibration Wizard //
///////////////////////////
    case UV_WELCOME_PAGE_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\WelcomePage.bmp";
      break;
	  
	case UV_TRAY_EMPTY_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\TrayEmpty.bmp";
      break;

	case UV_PLACE_SENSOR_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\PlaceSensor.bmp";
      break;

    case UV_LAMPS_WARMING_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\WarmingLamps.bmp";
      break;

    case UV_SENSOR_CALIBRATION:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\UVSensorCalibration.bmp";
      break;

    case UV_LEFT_SCANNING_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\ScanningLeftLamp.bmp";
      break;

    case UV_RESET_SENSOR_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\ResetSensor.bmp";
      break;

    case UV_TURN_OFF_SENSOR_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\TurnOffUVSensor.bmp";
      break;

    case UV_ENTER_VLAUE_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\EnterUVValue.bmp";
      break;

///////////////////////////////
// UV Calibration NEW Wizard //
///////////////////////////////
	case UV_WELCOME_PAGE_IMAGE_NEW_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\WelcomePage.bmp";
      break;
    case UV_LAMPS_WARMING_IMAGE_NEW_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\WarmingLamps.bmp";
      break;
    case UV_LEFT_SCANNING_IMAGE_NEW_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\ScanningLeftLamp.bmp";
      break;
    case UV_PLACE_SENSOR_IMAGE_NEW_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\PlaceSensor.bmp";
      break;
	case UV_SENSOR_SELECTION_IMAGE_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\UVSensorSelection.bmp";
      break;
	case UV_TRAY_EMPTY_IMAGE_NEW_ID:
		ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\TrayEmpty.bmp";
		break;
	case UV_PREPARATIONS_IMAGE_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\Preparations.bmp";
			break;
	case UV_ENTER_VALUE_IMAGE_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\EnterUVValue.bmp";
			break;
	case UV_EXTERNAL_SENSOR_CONNECTED_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\SensorConnected.bmp";
      break;
    case UV_LAMPS_STATE_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\CheckLampsState.bmp";
      break;
    case UV_STABILIZATION_TIME_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\IStabilizationTime.bmp";
      break;
    case UV_READ_UV_VALUES_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\ReadingUVValues.bmp";
	  break;
    case UV_RESULTS_PAGE_NEW_ID:
      ImageName += "\\Wizards\\Bin\\UV Calibration\\Results.bmp";
      break;
	case UV_OPEN_INTERNAL_SENSOR_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\OpenUVInternalSensor.bmp";
	  break;
	case UV_REMOVE_SENSOR_IMAGE_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\RemoveSensor.bmp";
	  break;
	case UV_PLACE_EXT_SENSOR_IMAGE_NEW_ID:
	  ImageName += "\\Wizards\\Bin\\UV Calibration\\PlaceSensor.bmp";
	  break;

///////////////////////////
// UV Replacement Wizard //
///////////////////////////
    case UV_LAMP_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Replacement\\UVLampImg.bmp";
      break;

    case SHADOW_BRACKET_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Replacement\\ShadowBracket.bmp";
      break;

    case FULL_BLOCK_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Replacement\\FullBlock.bmp";
      break;

    case LAMPS_STATUS_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\UV Replacement\\LampsStatus.bmp";
      break;

///////////////////////////////
// Vacuum Calibration Wizard //
///////////////////////////////
    case FIRST_PAGE_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Vacuum Calibration\\FirstPage.bmp";
      break;

    case VACUUM_PANEL_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Vacuum Calibration\\VacuumPanel.bmp";
      break;

    case VACUUM_7_CM_H2O_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Vacuum Calibration\\7cmH2O.bmp";
      break;

////////////////////////
// Clean Wiper Wizard //
////////////////////////
    case CW_MAIN_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Clean Wiper\\MainImg.bmp";
      break;

////////////////////////
// Roller Tilt Wizard //
////////////////////////
    case RT_PLATE_SCREWS_IMAGE_ID:
	  ImageName += "\\Wizards\\Bin\\Roller Tilt\\PlateScrews.bmp";
      break;

    case RT_ADJUST_SCREWS_IMAGE_ID:
      ImageName += "\\Wizards\\Bin\\Roller Tilt\\AdjustScrew.bmp";
      break;

/////////////////////
// Shutdown Wizard //
/////////////////////
    case SHD_CLEANING_FLUID_PICTURE_ID:
      ImageName += "\\Wizards\\Bin\\Shut Down\\CleaningFluid.bmp";
      break;

    case SHD_INFORMATION_PICTURE_ID:
      ImageName += "\\Wizards\\Bin\\Shut Down\\Information.bmp";
      break;

	  

/////////////////////////
// Heads Service Wizard //
////////////////////////
	case HSW_HEADS_IN_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadsIn.bmp";
		break;
		
	case HSW_HEADS_OUT_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadsOut.bmp";
		break;
	case HSW_PLACE_PAPER_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\PaperPlace.bmp";
		break;
	case HSW_REMOVE_PAPER_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\RemovePaper.bmp";
		break;
	case HSW_SCALE_PLACE_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\ScalePlace.bmp";
		break;
	case HSW_WEIGHT_TEST_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\WeightTest.bmp";
		break;
	case HSW_REMOVE_RESIN_PICTURE_ID:
		ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\RemoveResin.bmp";
		break;

////////////////////////////////////
// Tray Points Calibration Wizard //
////////////////////////////////////

    case TP_TRAY_POINTS_IID:
        ImageName += "\\Wizards\\Bin\\Tray Calibration\\Tray-Points.bmp";
		break;
	case TP_TRAY_POINTS_REMOVE_COVER_IID:
		ImageName += "\\Wizards\\Bin\\Tray Calibration\\remove-cover.bmp";
		break;
	case TP_TRAY_POINTS_SCREWS_IID:
		ImageName += "\\Wizards\\Bin\\Tray Calibration\\tray-screws.bmp";
		break;
	case TP_TRAY_POINTS_TRAY_LEVEL_IID:
		ImageName += "\\Wizards\\Bin\\Tray Calibration\\adjust-level.bmp";
		break;




    default:
      ImageName += "No such image ID";
      break;
  }

  free(CWD);
  return ImageName;
}


#endif
