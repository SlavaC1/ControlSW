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
const int PRINT_ROLLER_CUBE_IMAGE_ID = 21;
const int BUMPER_SENSOR_IMAGE_ID     = 22;

// Head Alignment Wizard
const int PLACE_TRANSPARENCY_IMAGE_ID       = 31;
const int PRINTING_TRANSPARENCY_IMAGE_ID    = 32;
const int REMOVE_TRANSPARENCY_IMAGE_ID      = 33;
const int CHOOSE_HEADS_TO_ALIGN_IMAGE_ID    = 34;
const int CHOOSE_BEST_LINE_IMAGE_ID         = 35;
const int DO_YOU_WANT_TO_SAVE_DATA_IMAGE_ID = 36;
const int DO_YOU_WANT_TO_RUN_AGAIN_IMAGE_ID = 37;
const int SAVE_DATA_PAGE_IMAGE_ID           = 38;

// Loadcell Calibration Wizard
const int LC_DEFAULT_IMAGE_ID       = 50;
const int MAIN_SCREEN_IMAGE_ID      = 51;
const int MODEL_1_CARTIDGE_IMAGE_ID = 52;
const int MODEL_2_CARTIDGE_IMAGE_ID = 53;
const int SUPPORT_1_CARTIDGE_IMAGE_ID = 54;
const int SUPPORT_2_CARTIDGE_IMAGE_ID = 55;
const int MODEL_CARTIDGE_IMAGE_ID     = 56;
const int SUPPORT_CARTIDGE_IMAGE_ID   = 57;
const int LOAD_CELL_CARTIDGE_IMAGE_ID = 58;
const int LOAD_CELL_REMOVE_CARTIDGE_IMAGE_ID = 59;
const int LOAD_CELL_WEIGHT_STATUS_IMAGE_ID = 260;
const int LOAD_CELL_COMPLETED_IMAGE_ID = 261;
// Printing Position Wizard
const int DIRECTION_IMAGE_ID = 60;

// Resin Replacement Wizard
const int RESIN_REPLACEMENT_MAIN_IMAGE = 70;
const int INSERT_MODEL_CARTRIDGE_IMAGE = 71;
const int RESIN_REPLACEMENT_QUICK_MODE = 72;
const int MRW_RESUME_PICTURE_ID        = 73;
const int MRW_GRID_CABINET_IDENTIFY    = 74;
const int MRW_CUSTOM_SUCCESSFULLY_COMPLETED_IMAGE_ID = 75;

// Quick SHR
const int SCALES_IMAGE_ID                = 80;
const int HEAD_IN_OUT_IMAGE_ID           = 81;
const int LEVEL_HEADS_IMAGE_ID           = 82;
const int WEIGHT_TEST_IMAGE_ID           = 83;
const int HEAD_ORDER_IMAGE_ID            = 84;
const int REMOVE_TOOLS_IMAGE_ID          = 85;
const int VACUUM_TEST_IMAGE_ID           = 86;
const int EMPTYING_BLOCK_IMAGE_ID        = 87;
const int HEAD_NOT_INSERTED_IMAGE_ID     = 88;
const int HEDA_FACTORY_DATA_IMAGE_ID     = 89;
const int LAST_PHASE_MESSAGE_IMAGE_ID    = 90;
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
const int UV_RIGHT_SCANNING_IMAGE_ID  = 115;
const int UV_TRAY_EMPTY_IMAGE_ID      = 116;
const int UV_TURN_OFF_SENSOR_IMAGE_ID = 117;
const int UV_ENTER_VLAUE_IMAGE_ID     = 118;
const int UV_SENSOR_CALIBRATION		  = 119;

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
const int CH_MAIN_IMAGE_ID   = 150;
const int HEADS_HOT_IMAGE_ID = 151;

// Clean Wiper Wizard
const int CW_MAIN_IMAGE_ID          = 160;
const int WIZARD_COMPLETED_IMAGE_ID = 161;

// Shutdown Wizard
const int SHD_PROGRESS_PICTURE_ID           = 170;
const int SHD_SHUTDOWN_PROC_PICTURE_ID      = 171;
const int SHD_SHUTDOWN_PC_PICTURE_ID        = 172;
const int SHD_EXIT_PICTURE_ID               = 173;
const int SHD_CLEANING_FLUID_PICTURE_ID     = 174;
const int SHD_INFORMATION_PICTURE_ID        = 175;
const int SHD_INSERT_TANK_STEP_1_PICTURE_ID = 176;
const int SHD_INSERT_TANK_STEP_2_PICTURE_ID = 177;
const int SHD_INSERT_TANK_STEP_3_PICTURE_ID = 178;
const int SHD_INSERT_TANK_STEP_4_PICTURE_ID = 179;

// Heads Optimization Wizard
const int OPENING_PICTURE_ID           = 180;
const int PURGING_PICTURE_ID           = 181;
const int PLACE_PT_PAPER_PICTURE_ID    = 182;
const int MISSING_NOZZLES_PICTURE_ID   = 183;
const int PREPARE_FOR_WT_PICTURE_ID    = 184;
const int PRINTING_WT_PICTURE_ID       = 185;
const int ENTER_WEIGHT_TEST_PICTURE_ID = 186;

// Heads Service Wizard
const int HSW_QUESTION_PICTURE_ID       = 2000;
const int HSW_CARTRIDGE_PICTURE_ID      = 2001;
const int HSW_DOOR_PICTURE_ID           = 2002;
const int HSW_HEAD_ALIGNMENT_PICTURE_ID = 2003;
const int HSW_HEADS_PICTURE_ID          = 2004;
const int HSW_HEADS_IN_PICTURE_ID       = 2005;
const int HSW_HEADS_OUT_PICTURE_ID      = 2006;
const int HSW_IN_PROGRESS_PICTURE_ID    = 2007;
const int HSW_WEIGHT_TEST_PICTURE_ID    = 2008;
const int HSW_RESUME_PICTURE_ID         = 2009;
const int HSW_INSERT_TRAY_PICTURE_ID    = 2010;
const int HSW_PLACE_PAPER_PICTURE_ID    = 2011;
const int HSW_REMOVE_PAPER_PICTURE_ID   = 2012;
const int HSW_SCALE_PLACE_PICTURE_ID    = 2013;
const int HSW_HEADS_ARE_CLEAN_PICTURE_ID = 2014;
const int HSW_REMOVE_RESIN_PICTURE_ID = 2015;
// Wiper Calibration Wizard
const int WC_WELCOME_IID                = 3000;
const int WC_CLEAN_TRAY_IID             = 3001;
const int WC_CLOSE_DOOR_IID             = 3002;
const int WC_HOMING_IID                 = 3003;
const int WC_POSITION_IID               = 3004;
const int WC_CALIBRATION_IID            = 3005;
const int WC_PURGE_WIPE_SELECTION_IID   = 3006;
const int WC_PURGING_IID                = 3007;
const int WC_WIPING_IID                 = 3008;
const int WC_RESULTSCHECK_IID           = 3009;
const int WC_ARE_RESULTSOK_IID          = 3010;
const int WC_COMPLETED_IID              = 3011;
const int WC_CANCELED_IID               = 3012;
const int WC_T_HEIGHT_IID               = 3013;
const int WC_BLADE_SELECTION_IID        = 3014;

// Tray Points Calibration Wizard
const int TP_TRAY_POINTS_IID            = 4000;
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
			ImageName += "\\Wizards\\Bin\\General\\MainPicture.bmp";
			break;

		case SUCCESSFULLY_COMPLETED_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\General\\Succesfully_Completed.bmp";
			break;

		case CAUTION_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\General\\Close_the_door.bmp";
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
			ImageName += "\\Wizards\\Bin\\General\\insert_the_tray.bmp";
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

		case WC_CLEAN_TRAY_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Tray_is_Empty.bmp";
			break;

		case WC_CLOSE_DOOR_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Close_the_cover.bmp";
			break;

		case WC_T_HEIGHT_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Adjusting_T_Axis_height.bmp";
			break;

		case WC_HOMING_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Homing_all_axes.bmp";
			break;

		case WC_POSITION_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Preparing_to_calibration.bmp";
			break;

		case WC_CALIBRATION_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Moving_to_calibration_Position.bmp";
			break;

		case WC_PURGE_WIPE_SELECTION_IID:
		case WC_PURGING_IID:
		case WC_WIPING_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Calibration_Screen.bmp";
			break;

		case WC_RESULTSCHECK_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Inspection_Screen.bmp";
			break;

		case WC_ARE_RESULTSOK_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Test_Screen.bmp";
			break;

		case WC_COMPLETED_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Completed_Screen.bmp";
			break;

		case WC_CANCELED_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Wizard_Cancelled.bmp";
			break;

		case WC_BLADE_SELECTION_IID:
			ImageName += "\\Wizards\\Bin\\Wiper Calibration\\_Wiper_Blade_Selection.bmp";
			break;

///////////////////////////////
// Bumper Calibration Wizard //
///////////////////////////////
		case PRINT_ROLLER_CUBE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Bumper Calibration\\PrintRollerCube.bmp";
			break;

		case BUMPER_SENSOR_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Bumper Calibration\\BumperSensor.bmp";
			break;

///////////////////////////
// Head Alignment Wizard //
///////////////////////////
		case PLACE_TRANSPARENCY_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Head Alignment\\PlaceTransparency.bmp";
			break;

		case PRINTING_TRANSPARENCY_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Head Alignment\\PrintingTransparency.bmp";
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

		case DO_YOU_WANT_TO_SAVE_DATA_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Head Alignment\\DoYouWantToSaveData.bmp";
			break;

		case DO_YOU_WANT_TO_RUN_AGAIN_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Head Alignment\\RunWizardAgain.bmp";
			break;

		case SAVE_DATA_PAGE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Head Alignment\\SaveData.bmp";
			break;

/////////////////////////////////
// Loadcell Calibration Wizard //
/////////////////////////////////
		case LOAD_CELL_CARTIDGE_IMAGE_ID:     //TRIPLEX
			ImageName += "\\Wizards\\Bin\\General\\Preparations.bmp";
			break;
		case LOAD_CELL_REMOVE_CARTIDGE_IMAGE_ID :  //TRIPLEX
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Remove_Cartridges.bmp";
			break;
		case LC_DEFAULT_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\DefaultPicture.bmp";
			break;
		case LOAD_CELL_WEIGHT_STATUS_IMAGE_ID :  //TRIPLEX
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Status.bmp";
			break;
		case LOAD_CELL_COMPLETED_IMAGE_ID:  //TRIPLEX
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Succesfully_Completed.bmp";
			break;
		case MAIN_SCREEN_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\MainScreen.bmp";
			break;
		case MODEL_1_CARTIDGE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Model1Cartridge.bmp";
			break;

		case MODEL_2_CARTIDGE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Model2Cartridge.bmp";
			break;

		case SUPPORT_1_CARTIDGE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Support1Cartridge.bmp";
			break;

		case SUPPORT_2_CARTIDGE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\Support2Cartridge.bmp";
			break;

		case MODEL_CARTIDGE_IMAGE_ID:
			// todo -oNobody -cNone: make sure this is the right image
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\ModelCartridge.bmp";
			break;

		case SUPPORT_CARTIDGE_IMAGE_ID:
			// todo -oNobody -cNone: make sure this is the right image
			ImageName += "\\Wizards\\Bin\\Loadcell Calibration\\SupportCartridge.bmp";
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

		case RESIN_REPLACEMENT_QUICK_MODE:
			ImageName += "\\Wizards\\Bin\\Resin Replacement\\QuickMode.bmp";
			break;

		case MRW_RESUME_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Resin Replacement\\Resume.bmp";
			break;

		case MRW_GRID_CABINET_IDENTIFY:
			ImageName += "\\Wizards\\Bin\\Resin Replacement\\CabinetIdentify.bmp";
			break;

		case MRW_CUSTOM_SUCCESSFULLY_COMPLETED_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Resin Replacement\\Succesfully_Completed_Custom.bmp";
			break;

///////////////
// Quick SHR //
///////////////
		case SCALES_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\ScalesImg.bmp";
			break;

		case HEAD_IN_OUT_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\HeadsInOut.bmp";
			break;

		case LEVEL_HEADS_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\LevelHeads.bmp";
			break;

		case WEIGHT_TEST_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\WeightTest.bmp";
			break;

		case HEAD_ORDER_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\HeadOrder.bmp";
			break;

		case REMOVE_TOOLS_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\RemoveTools.bmp";
			break;

		case VACUUM_TEST_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\VacuumTest.bmp";
			break;

		case EMPTYING_BLOCK_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\EmptyingBlock.bmp";
			break;

		case HEAD_NOT_INSERTED_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\HeadsNotInserted.bmp";
			break;

		case HEDA_FACTORY_DATA_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\SHR\\HeadsFactory.bmp";
			break;

		case LAST_PHASE_MESSAGE_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\General\\MainPicture.bmp";
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
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\Preparations.bmp";
			break;
		case UV_ENTER_VALUE_IMAGE_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\EnterUVValue.bmp";
			break;
		case UV_EXTERNAL_SENSOR_CONNECTED_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\SensorConnected.bmp";
			break;
		case UV_LAMPS_STATE_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\CheckLampsState.bmp";
			break;
		case UV_STABILIZATION_TIME_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\StabilizationTime.bmp";
			break;
		case UV_READ_UV_VALUES_NEW_ID:
			ImageName += "\\Wizards\\Bin\\UV Calibration\\InternalSensorWizard\\ReadingUVValues.bmp";
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
// Clean Heads Wizard //
////////////////////////
		case CH_MAIN_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Clean Heads\\MainImg.bmp";
			break;

		case HEADS_HOT_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Clean Heads\\HeadsHot.bmp";
			break;

////////////////////////
// Clean Wiper Wizard //
////////////////////////
		case CW_MAIN_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Clean Wiper\\MainImg.bmp";
			break;

		case WIZARD_COMPLETED_IMAGE_ID:
			ImageName += "\\Wizards\\Bin\\Clean Wiper\\WizardCompleted.bmp";
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
		case SHD_PROGRESS_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\Progress.bmp";
			break;

		case SHD_SHUTDOWN_PROC_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\ShutdownInProcess.bmp";
			break;

		case SHD_SHUTDOWN_PC_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\ShutdownPC.bmp";
			break;

		case SHD_EXIT_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\Exit.bmp";
			break;

		case SHD_CLEANING_FLUID_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\CleaningFluid.bmp";
			break;

		case SHD_INFORMATION_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\Information.bmp";
			break;

		case SHD_INSERT_TANK_STEP_1_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\InsertTanksStep1.bmp";
			break;

		case SHD_INSERT_TANK_STEP_2_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\InsertTanksStep2.bmp";
			break;

		case SHD_INSERT_TANK_STEP_3_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\InsertTanksStep3.bmp";
			break;

		case SHD_INSERT_TANK_STEP_4_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Shut Down\\InsertTanksStep4.bmp";
			break;


///////////////////////////////
// Heads Optimization Wizard //
///////////////////////////////
		case OPENING_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Opening_screen.bmp";
			break;

		case PURGING_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Purging.bmp";
			break;

		case PLACE_PT_PAPER_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Place_Pattern_Test_Paper.bmp";
			break;

		case MISSING_NOZZLES_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Enter_missing_nozzles.bmp";
			break;

		case PREPARE_FOR_WT_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Preparation_for_weight_test.bmp";
			break;

		case PRINTING_WT_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Printing_Weight_Test.bmp";
			break;

		case ENTER_WEIGHT_TEST_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Optimization\\Enter_weight_test.bmp";
			break;


/////////////////////////
// Heads Service Wizard //
////////////////////////

		case HSW_QUESTION_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\Question.bmp";
			break;

		case HSW_CARTRIDGE_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\Cartridge.bmp";
			break;

		case HSW_DOOR_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\CloseDoor.bmp";
			break;

		case HSW_HEAD_ALIGNMENT_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadAlignment.bmp";
			break;

		case HSW_HEADS_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\Heads.bmp";
			break;
		case HSW_HEADS_ARE_CLEAN_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadsAreClean.bmp";
			break;
		case HSW_HEADS_IN_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadsIn.bmp";
			break;

		case HSW_HEADS_OUT_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\HeadsOut.bmp";
			break;

		case HSW_IN_PROGRESS_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\InProgess.bmp";
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

		case HSW_RESUME_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\Resume.bmp";
			break;

		case HSW_INSERT_TRAY_PICTURE_ID:
			ImageName += "\\Wizards\\Bin\\Heads Service Wizard\\InsertTray.bmp";
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
