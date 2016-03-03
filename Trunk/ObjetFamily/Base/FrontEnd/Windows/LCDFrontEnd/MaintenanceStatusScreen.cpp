//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <MainUnit.h>
#include "AppLogFile.h"
#include "MaintenanceStatusScreen.h"
#include "FrontEndControlIDs.h"
#include "AppParams.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EnhancedMenu"
#pragma link "LCDBaseScreen"
#pragma link "EnhancedLabel"
#pragma resource "*.dfm"

#include "FEResources.h"

//---------------------------------------------------------------------------
__fastcall TMaintenanceStatusFrame::TMaintenanceStatusFrame(TComponent* Owner)
        : TLCDBaseFrame(Owner)
{
  m_DisplayInA2DUnits           = false;
  m_WasteWeightCurrentStatusBmp = 0;
  m_ThermistorMode              = LOW_THERMISTORS_MODE;
  TWinControl* ParentPtr        = HeadsLiquidLabel->Parent;
  int          HeadNo;

  Width  = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_SCREEN_WIDTH);
  Height = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_SCREEN_HEIGHT);

  Panel1->Width  = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_SCREEN_WIDTH);
  Panel1->Height = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_SCREEN_HEIGHT);

  PaintBox1->Width  = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_SCREEN_WIDTH);
  PaintBox1->Height = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_SCREEN_HEIGHT);

   FOR_ALL_UV_LAMPS(l)
  {
     UVLampLabelArray[l]                    = new TEnhancedLabel(this);
     UVLampLabelArray[l]->Visible           = false;

     LOAD_BITMAP(UVLampLabelArray[l]->LeftPart,    IDB_HEADS_LABEL_COLD_LEFT);
     LOAD_BITMAP(UVLampLabelArray[l]->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);

     LOAD_BITMAP(UVLampLabelArray[l]->MiddlePart,  IDB_HEADS_LABEL_COLD_MIDDLE);
     LOAD_BITMAP(UVLampLabelArray[l]->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);

     LOAD_BITMAP(UVLampLabelArray[l]->RightPart,   IDB_HEADS_LABEL_COLD_RIGHT);
     LOAD_BITMAP(UVLampLabelArray[l]->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);

     UVLampLabelArray[l]->Parent            = ParentPtr;
	 UVLampLabelArray[l]->Left              = INT_FROM_RESOURCE_ARRAY(l,IDN_GRAPHIC_LAMP_LEFT);
     UVLampLabelArray[l]->Top               = INT_FROM_RESOURCE_ARRAY(l,IDN_GRAPHIC_LAMP_TOP);
     UVLampLabelArray[l]->Font->Name        = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
     UVLampLabelArray[l]->Font->Height      = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_HEIGHT);
	 UVLampLabelArray[l]->Font->Size        = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_SIZE);
	 UVLampLabelArray[l]->Font->Style      = TFontStyles()<< fsBold;
     UVLampLabelArray[l]->Alignment         = taCenter;

	 UVLampLabelArray[l]->Caption           = GetUVLampStr(l)[0];
     UVLampLabelArray[l]->ActiveBitmap      = 0;
     UVLampLabelArray[l]->MiddlePartsNum    = INT_FROM_RESOURCE_ARRAY(l,IDN_GRAPHIC_LAMP_MIDDLE_PARTS_NUM);
     UVLampLabelArray[l]->Width             = INT_FROM_RESOURCE_ARRAY(l,IDN_GRAPHIC_LAMP_WIDTH);
     UVLampLabelArray[l]->TransparentColor  = TRANSPARENT_COLOR;
     UVLampLabelArray[l]->Transparent       = true;
     UVLampLabelArray[l]->Visible           = true;
  }
  
  for (HeadNo=0; HeadNo<TOTAL_NUMBER_OF_HEATERS; HeadNo++)
  {
     HeadHeatersLabels[HeadNo]                    = new TEnhancedLabel(this);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->LeftPart,    IDB_HEADS_LABEL_COLD_LEFT);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->LeftPart2,   IDB_HEADS_LABEL_HOT_LEFT);

     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->MiddlePart,  IDB_HEADS_LABEL_COLD_MIDDLE);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->MiddlePart2, IDB_HEADS_LABEL_HOT_MIDDLE);

     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->RightPart,   IDB_HEADS_LABEL_COLD_RIGHT);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);
     LOAD_BITMAP(HeadHeatersLabels[HeadNo]->RightPart2,  IDB_HEADS_LABEL_HOT_RIGHT);

	 HeadHeatersLabels[HeadNo]->Parent            = ParentPtr;
	 HeadHeatersLabels[HeadNo]->Left              = INT_FROM_RESOURCE_ARRAY(HeadNo,IDN_GRAPHIC_HEATER_LEFT_SINGLE);
     HeadHeatersLabels[HeadNo]->Top               = INT_FROM_RESOURCE_ARRAY(HeadNo,IDN_GRAPHIC_HEATER_TOP);
     HeadHeatersLabels[HeadNo]->MiddlePartsNum    = INT_FROM_RESOURCE_ARRAY(HeadNo,IDN_GRAPHIC_HEATER_HEAD_MIDDLE_PARTS_NUM);
     HeadHeatersLabels[HeadNo]->Width             = INT_FROM_RESOURCE_ARRAY(HeadNo,IDN_GRAPHIC_HEATER_WIDTH);
     HeadHeatersLabels[HeadNo]->Font->Name        = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
     HeadHeatersLabels[HeadNo]->Font->Height      = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_HEIGHT);
	 HeadHeatersLabels[HeadNo]->Font->Size        = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_SIZE);
	 HeadHeatersLabels[HeadNo]->Font->Style       = TFontStyles()<< fsBold;
     HeadHeatersLabels[HeadNo]->Alignment         = taCenter;

     HeadHeatersLabels[HeadNo]->Caption           = "???";
     HeadHeatersLabels[HeadNo]->ActiveBitmap      = 0;
	 HeadHeatersLabels[HeadNo]->TransparentColor  = TRANSPARENT_COLOR;
     HeadHeatersLabels[HeadNo]->Transparent       = true;
  }

  // Preheaters
 // HeadHeatersLabels[BLOCK_0]->Font->Style = TFontStyles();
 // HeadHeatersLabels[BLOCK_1]->Font->Style = TFontStyles();

  for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
  {
     ResinLiquidLabelArray[i]                   = new TEnhancedLabel(this);
     ResinLiquidLabelArray[i]->Visible          = false;

	 LOAD_BITMAP(ResinLiquidLabelArray[i]->LeftPart,    IDB_STATUS_NONE_PANEL_LEFT);
	 LOAD_BITMAP(ResinLiquidLabelArray[i]->MiddlePart,  IDB_STATUS_NONE_PANEL_MIDDLE);
     LOAD_BITMAP(ResinLiquidLabelArray[i]->RightPart,   IDB_STATUS_NONE_PANEL_RIGHT);

     LOAD_BITMAP(ResinLiquidLabelArray[i]->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);
     LOAD_BITMAP(ResinLiquidLabelArray[i]->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);
     LOAD_BITMAP(ResinLiquidLabelArray[i]->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);

     ResinLiquidLabelArray[i]->Parent           = ParentPtr;
	 ResinLiquidLabelArray[i]->Left             = INT_FROM_RESOURCE_ARRAY(i,IDN_GRAPHIC_CHAMBER_LEFT);
	 ResinLiquidLabelArray[i]->Top              = HeadsLiquidLabel->Top;
  
     ResinLiquidLabelArray[i]->Height           = HeadsLiquidLabel->Height;
     ResinLiquidLabelArray[i]->ActiveBitmap     = 0;
     ResinLiquidLabelArray[i]->Font->Name       = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
     ResinLiquidLabelArray[i]->Font->Height     = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_HEIGHT);
	 ResinLiquidLabelArray[i]->Font->Size       = 8;//INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_SIZE);
	 ResinLiquidLabelArray[i]->Font->Style      = TFontStyles()<< fsBold;
     ResinLiquidLabelArray[i]->Font->Color      = clWhite;
	 ResinLiquidLabelArray[i]->Alignment        = taCenter;
     ResinLiquidLabelArray[i]->Caption          = LOAD_STRING_ARRAY(i,IDS_GRAPHIC_CHAMBER_STR_SINGLE);
	 ResinLiquidLabelArray[i]->MiddlePartsNum   = INT_FROM_RESOURCE(IDN_STATUS_MIDDLE_PARTS_NUM);
	 ResinLiquidLabelArray[i]->Width            = INT_FROM_RESOURCE(IDN_STATUS_WIDTH);
     ResinLiquidLabelArray[i]->TransparentColor = TRANSPARENT_COLOR;
     ResinLiquidLabelArray[i]->Transparent      = true;
     ResinLiquidLabelArray[i]->Visible          = true;
  }

  HeadsVacuumTemperatureLabel->Visible                = false;
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->LeftPart,    IDB_HEADS_LABEL_COLD_LEFT);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->MiddlePart,  IDB_HEADS_LABEL_COLD_MIDDLE);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->RightPart,   IDB_HEADS_LABEL_COLD_RIGHT);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->LeftPart2,   IDB_HEADS_LABEL_HOT_LEFT);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->MiddlePart2, IDB_HEADS_LABEL_HOT_MIDDLE);
  LOAD_BITMAP(HeadsVacuumTemperatureLabel->RightPart2,  IDB_HEADS_LABEL_HOT_RIGHT);

  HeadsVacuumTemperatureLabel->Parent            = ParentPtr;
  HeadsVacuumTemperatureLabel->ActiveBitmap      = 0;
  HeadsVacuumTemperatureLabel->MiddlePartsNum    = INT_FROM_RESOURCE(IDN_HEAD_HEATER_LABEL_MIDDLE_PARTS_NUM);

  HeadsVacuumTemperatureLabel->Left              = INT_FROM_RESOURCE(IDN_MAINTENANCE_RIGHTPART2_LEFT);
  HeadsVacuumTemperatureLabel->Top               = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TOP);
  HeadsVacuumTemperatureLabel->Width             = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TEMP_WIDTH);
  HeadsVacuumTemperatureLabel->Font->Name        = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  HeadsVacuumTemperatureLabel->Font->Color       = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);
  HeadsVacuumTemperatureLabel->Font->Height      = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_HEIGHT);
  HeadsVacuumTemperatureLabel->Font->Size        = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_SIZE);
  HeadsVacuumTemperatureLabel->Font->Style       = TFontStyles()<< fsBold;
  HeadsVacuumTemperatureLabel->Alignment         = taCenter;
  HeadsVacuumTemperatureLabel->Caption           = "???";

  HeadsVacuumTemperatureLabel->TransparentColor  = TRANSPARENT_COLOR;
  HeadsVacuumTemperatureLabel->Transparent       = true;
  HeadsVacuumTemperatureLabel->Visible           = true;

  AirFlowSpeedLabel->Visible                = false;
  LOAD_BITMAP(AirFlowSpeedLabel->LeftPart,    IDB_HEADS_LABEL_HOT_LEFT);   //0 = red
  LOAD_BITMAP(AirFlowSpeedLabel->MiddlePart,  IDB_HEADS_LABEL_HOT_MIDDLE);
  LOAD_BITMAP(AirFlowSpeedLabel->RightPart,   IDB_HEADS_LABEL_HOT_RIGHT);
  LOAD_BITMAP(AirFlowSpeedLabel->LeftPart1,   IDB_MAINTENANCE_LABEL_LEFT); //1 = yellow
  LOAD_BITMAP(AirFlowSpeedLabel->MiddlePart1, IDB_MAINTENANCE_LABEL_MIDDLE);
  LOAD_BITMAP(AirFlowSpeedLabel->RightPart1,  IDB_MAINTENANCE_LABEL_RIGHT);
  LOAD_BITMAP(AirFlowSpeedLabel->LeftPart2,   IDB_HEADS_LABEL_OK_LEFT);  //2 = green
  LOAD_BITMAP(AirFlowSpeedLabel->MiddlePart2, IDB_HEADS_LABEL_OK_MIDDLE);
  LOAD_BITMAP(AirFlowSpeedLabel->RightPart2,  IDB_HEADS_LABEL_OK_RIGHT);
  AirFlowSpeedLabel->ActiveBitmap           = 1;
  AirFlowSpeedLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_HEAD_HEATER_LABEL_MIDDLE_PARTS_NUM);
  AirFlowSpeedLabel->Width                  = INT_FROM_RESOURCE(IDN_MAINTENANCE_LABEL_WIDTH);
  AirFlowSpeedLabel->Font->Name             = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  AirFlowSpeedLabel->Font->Color            = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);
  AirFlowSpeedLabel->TransparentColor       = TRANSPARENT_COLOR;
  AirFlowSpeedLabel->Transparent            = true;
  AirFlowSpeedLabel->Top 					= INT_FROM_RESOURCE(IDN_EVACUATION_AIRFLOW_TOP);
  AirFlowSpeedLabel->Left                   = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART2_LEFT);
  AirFlowSpeedLabel->Caption          		= "???";
//  AirFlowSpeedLabel->Visible                = true;

  AirFlowLabel->Visible                		= false;
  LOAD_BITMAP(AirFlowLabel->LeftPart,         IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(AirFlowLabel->MiddlePart,       IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(AirFlowLabel->RightPart,        IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  AirFlowLabel->Left             	 	    = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART1_LEFT);
  AirFlowLabel->Top             	 	    = INT_FROM_RESOURCE(IDN_EVACUATION_AIRFLOW_TOP)-1;
  AirFlowLabel->MiddlePartsNum              = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  AirFlowLabel->Font->Name                  = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  AirFlowLabel->Font->Color                 = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  AirFlowLabel->TransparentColor            = TRANSPARENT_COLOR;
  AirFlowLabel->Transparent                 = true;
  AirFlowLabel->Caption 					= LOAD_STRING(IDS_EVACUATION);  

  TrayTemperatureLabel->Visible                = false;
  LOAD_BITMAP(TrayTemperatureLabel->LeftPart,    IDB_HEADS_LABEL_COLD_LEFT);
  LOAD_BITMAP(TrayTemperatureLabel->MiddlePart,  IDB_HEADS_LABEL_COLD_MIDDLE);
  LOAD_BITMAP(TrayTemperatureLabel->RightPart,   IDB_HEADS_LABEL_COLD_RIGHT);
  LOAD_BITMAP(TrayTemperatureLabel->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);
  LOAD_BITMAP(TrayTemperatureLabel->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);
  LOAD_BITMAP(TrayTemperatureLabel->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);
  LOAD_BITMAP(TrayTemperatureLabel->LeftPart2,   IDB_HEADS_LABEL_HOT_LEFT);
  LOAD_BITMAP(TrayTemperatureLabel->MiddlePart2, IDB_HEADS_LABEL_HOT_MIDDLE);
  LOAD_BITMAP(TrayTemperatureLabel->RightPart2,  IDB_HEADS_LABEL_HOT_RIGHT);
  TrayTemperatureLabel->ActiveBitmap           = 0;
  TrayTemperatureLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_HEAD_HEATER_LABEL_MIDDLE_PARTS_NUM);
  TrayTemperatureLabel->Width                  = 136;
  TrayTemperatureLabel->Visible                = true;
  TrayTemperatureLabel->Font->Name             = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  TrayTemperatureLabel->Font->Color            = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);
  TrayTemperatureLabel->TransparentColor       = TRANSPARENT_COLOR;
  TrayTemperatureLabel->Transparent            = true;
  TrayTemperatureLabel->Left                   = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART2_LEFT);

  AmbientTemperatureLabel->Visible                = false;
  LOAD_BITMAP(AmbientTemperatureLabel->LeftPart,    IDB_HEADS_LABEL_COLD_LEFT);
  LOAD_BITMAP(AmbientTemperatureLabel->MiddlePart,  IDB_HEADS_LABEL_COLD_MIDDLE);
  LOAD_BITMAP(AmbientTemperatureLabel->RightPart,   IDB_HEADS_LABEL_COLD_RIGHT);
  LOAD_BITMAP(AmbientTemperatureLabel->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);
  LOAD_BITMAP(AmbientTemperatureLabel->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);
  LOAD_BITMAP(AmbientTemperatureLabel->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);
  LOAD_BITMAP(AmbientTemperatureLabel->LeftPart2,   IDB_HEADS_LABEL_HOT_LEFT);
  LOAD_BITMAP(AmbientTemperatureLabel->MiddlePart2, IDB_HEADS_LABEL_HOT_MIDDLE);
  LOAD_BITMAP(AmbientTemperatureLabel->RightPart2,  IDB_HEADS_LABEL_HOT_RIGHT);
  AmbientTemperatureLabel->ActiveBitmap           = 0;
  AmbientTemperatureLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_HEAD_HEATER_LABEL_MIDDLE_PARTS_NUM);
  AmbientTemperatureLabel->Width                  = 136;
  AmbientTemperatureLabel->Font->Name             = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  AmbientTemperatureLabel->Font->Color            = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);
  AmbientTemperatureLabel->Font->Height      	  = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_HEIGHT);
  AmbientTemperatureLabel->Font->Size        	  = INT_FROM_RESOURCE(IDN_MAINTENANCE_STATUS_FONT_SIZE);
  AmbientTemperatureLabel->Font->Style       	  = TFontStyles()<< fsBold;
  AmbientTemperatureLabel->Alignment         	  = taCenter;
  AmbientTemperatureLabel->Caption           	  = "???";
  AmbientTemperatureLabel->TransparentColor       = TRANSPARENT_COLOR;
  AmbientTemperatureLabel->Transparent            = true;
  AmbientTemperatureLabel->Left                   = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART2_LEFT);
  AmbientTemperatureLabel->Top                    = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TOP);
  AmbientTemperatureLabel->Visible                = true;

  LOAD_BITMAP(WasteWeightEnhancedLabel->LeftPart,    IDB_STATUS_NONE_PANEL_LEFT);
  LOAD_BITMAP(WasteWeightEnhancedLabel->MiddlePart,  IDB_STATUS_NONE_PANEL_MIDDLE);
  LOAD_BITMAP(WasteWeightEnhancedLabel->RightPart,   IDB_STATUS_NONE_PANEL_RIGHT);
  LOAD_BITMAP(WasteWeightEnhancedLabel->LeftPart1,   IDB_HEADS_LABEL_OK_LEFT);
  LOAD_BITMAP(WasteWeightEnhancedLabel->MiddlePart1, IDB_HEADS_LABEL_OK_MIDDLE);
  LOAD_BITMAP(WasteWeightEnhancedLabel->RightPart1,  IDB_HEADS_LABEL_OK_RIGHT);
  LOAD_BITMAP(WasteWeightEnhancedLabel->LeftPart2,   IDB_HEADS_LABEL_HOT_LEFT);
  LOAD_BITMAP(WasteWeightEnhancedLabel->MiddlePart2, IDB_HEADS_LABEL_HOT_MIDDLE);
  LOAD_BITMAP(WasteWeightEnhancedLabel->RightPart2,  IDB_HEADS_LABEL_HOT_RIGHT);
  WasteWeightEnhancedLabel->ActiveBitmap           = 0;
  WasteWeightEnhancedLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_HEAD_HEATER_LABEL_MIDDLE_PARTS_NUM);
  WasteWeightEnhancedLabel->Width                  = 136;
  WasteWeightEnhancedLabel->Font->Name             = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  WasteWeightEnhancedLabel->Font->Color            = LOAD_COLOR(IDC_HEAD_HEATER_LABEL_FONT_COLOR);
  WasteWeightEnhancedLabel->Font->Style      = TFontStyles()<< fsBold;
  WasteWeightEnhancedLabel->TransparentColor       = TRANSPARENT_COLOR;
  WasteWeightEnhancedLabel->Left                   = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART2_LEFT);
  WasteWeightEnhancedLabel->Top                    = ResinLiquidLabelArray[0]->Top+40;
                                                     
  LOAD_BITMAP(WasteEnhancedLabel->LeftPart,    IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(WasteEnhancedLabel->MiddlePart,  IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(WasteEnhancedLabel->RightPart,   IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  WasteEnhancedLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  WasteEnhancedLabel->Font->Name             = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  WasteEnhancedLabel->Font->Color            = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  WasteEnhancedLabel->TransparentColor       = TRANSPARENT_COLOR;
  WasteEnhancedLabel->Transparent            = true;
  WasteEnhancedLabel->Left                   = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART1_LEFT);
  WasteEnhancedLabel->Top                    = ResinLiquidLabelArray[0]->Top+40;
  WasteEnhancedLabel->Caption                = ChamberToStr(TYPE_CHAMBER_WASTE).c_str();
  WasteEnhancedLabel->Caption               += " (gr)";

  LOAD_BITMAP(HeadsLabel->LeftPart,            IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(HeadsLabel->MiddlePart,          IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(HeadsLabel->RightPart,           IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  HeadsLabel->Left                           = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART1_LEFT);
  HeadsLabel->Top                            = INT_FROM_RESOURCE(IDN_HEADS_LABEL_TOP);
  HeadsLabel->MiddlePartsNum                 = INT_FROM_RESOURCE(IDN_HEADS_LABEL_MIDDLE_PARTS_NUM);
  HeadsLabel->Font->Name                     = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  HeadsLabel->Font->Color                    = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  HeadsLabel->TransparentColor               = TRANSPARENT_COLOR;
  HeadsLabel->Transparent                    = true;

  LOAD_BITMAP(AmbientLabel->LeftPart,          IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(AmbientLabel->MiddlePart,        IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(AmbientLabel->RightPart,         IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  AmbientLabel->MiddlePartsNum               = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  AmbientLabel->Font->Name                   = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  AmbientLabel->Font->Color                  = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  AmbientLabel->TransparentColor             = TRANSPARENT_COLOR;
  AmbientLabel->Transparent                  = true;
  AmbientLabel->Left                         = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART1_LEFT);
  AmbientLabel->Top                          = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TOP);
  AmbientLabel->Caption                      = LOAD_STRING(IDS_AMBIENT);
  AmbientLabel->Width                        = INT_FROM_RESOURCE(IDN_MAINTENANCE_LABEL_WIDTH);
  AmbientLabel->Font->Style      = TFontStyles()<< fsBold;
  AmbientLabel->Font->Size = 11;
  LOAD_BITMAP(UVLampsLabel->LeftPart,          IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(UVLampsLabel->MiddlePart,        IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(UVLampsLabel->RightPart,         IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  UVLampsLabel->MiddlePartsNum               = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  UVLampsLabel->Font->Name                   = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  UVLampsLabel->Font->Color                  = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  UVLampsLabel->TransparentColor             = TRANSPARENT_COLOR;
  UVLampsLabel->Transparent                  = true;
  UVLampsLabel->Left                         = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART1_LEFT);

  LOAD_BITMAP(TrayLabel->LeftPart,             IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(TrayLabel->MiddlePart,           IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(TrayLabel->RightPart,            IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  TrayLabel->MiddlePartsNum                  = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  TrayLabel->Font->Name                      = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  TrayLabel->Font->Color                     = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  TrayLabel->TransparentColor                = TRANSPARENT_COLOR;
  TrayLabel->Transparent                     = true;

  LOAD_BITMAP(HeadsLiquidLabel->LeftPart,      IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(HeadsLiquidLabel->MiddlePart,    IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(HeadsLiquidLabel->RightPart,     IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  HeadsLiquidLabel->MiddlePartsNum           = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  HeadsLiquidLabel->Font->Name               = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  HeadsLiquidLabel->Font->Color              = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  HeadsLiquidLabel->TransparentColor         = TRANSPARENT_COLOR;
  HeadsLiquidLabel->Transparent              = true;
 // HeadsLiquidLabel->Left                     = INT_FROM_RESOURCE(IDN_MAINTENANCE_RIGHTPART1_LEFT);
  HeadsLiquidLabel->Left                     = INT_FROM_RESOURCE(IDN_MAINTENANCE_LEFTPART1_LEFT )+5;
  HeadsLiquidLabel->Top                      = ResinLiquidLabelArray[0]->Top;

  LOAD_BITMAP(HeadsVacuumLabel->LeftPart,      IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(HeadsVacuumLabel->MiddlePart,    IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(HeadsVacuumLabel->RightPart,     IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  HeadsVacuumLabel->MiddlePartsNum           = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  HeadsVacuumLabel->Font->Name               = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  HeadsVacuumLabel->Font->Color              = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  HeadsVacuumLabel->TransparentColor         = TRANSPARENT_COLOR;
  HeadsVacuumLabel->Transparent              = true;
  HeadsVacuumLabel->Left                     = INT_FROM_RESOURCE(IDN_MAINTENANCE_RIGHTPART1_LEFT);
  HeadsVacuumLabel->Top                      = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TOP);
//OBJET_MACHINE, PreHeater

		LOAD_BITMAP(PreHeaterLabel->LeftPart,        IDB_MAINTENANCE_STATUS_LABEL_LEFT);
		LOAD_BITMAP(PreHeaterLabel->MiddlePart,      IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
		LOAD_BITMAP(PreHeaterLabel->RightPart,       IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
		PreHeaterLabel->MiddlePartsNum             = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
		PreHeaterLabel->Font->Name                 = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
		PreHeaterLabel->Font->Color                = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
		PreHeaterLabel->TransparentColor           = TRANSPARENT_COLOR;
		PreHeaterLabel->Transparent                = true;
		PreHeaterLabel->Left                       = INT_FROM_RESOURCE(IDN_MAINTENANCE_RIGHTPART1_LEFT);
		PreHeaterLabel->Top                        = HeadHeatersLabels[PRE_HEATER]->Top;
		PreHeaterLabel->Font->Style      = TFontStyles()<< fsBold;

   /*
  LOAD_BITMAP(PreHeaterFrontLabel->LeftPart,        IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(PreHeaterFrontLabel->MiddlePart,      IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(PreHeaterFrontLabel->RightPart,       IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  PreHeaterFrontLabel->MiddlePartsNum             = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM)+2;
  PreHeaterFrontLabel->Font->Name                 = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  PreHeaterFrontLabel->Font->Color                = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  PreHeaterFrontLabel->TransparentColor           = TRANSPARENT_COLOR;
  PreHeaterFrontLabel->Transparent                = true;
  PreHeaterFrontLabel->Left                       = INT_FROM_RESOURCE(IDN_MAINTENANCE_RIGHTPART1_LEFT);
  PreHeaterFrontLabel->Top                        = HeadHeatersLabels[BLOCK_0]->Top;

  LOAD_BITMAP(PreHeaterRearLabel->LeftPart,        IDB_MAINTENANCE_STATUS_LABEL_LEFT);
  LOAD_BITMAP(PreHeaterRearLabel->MiddlePart,      IDB_MAINTENANCE_STATUS_LABEL_MIDDLE);
  LOAD_BITMAP(PreHeaterRearLabel->RightPart,       IDB_MAINTENANCE_STATUS_LABEL_RIGHT);
  PreHeaterRearLabel->MiddlePartsNum             = INT_FROM_RESOURCE(IDN_WASTE_LABEL_MIDDLE_PARTS_NUM);
  PreHeaterRearLabel->Font->Name                 = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  PreHeaterRearLabel->Font->Color                = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  PreHeaterRearLabel->TransparentColor           = TRANSPARENT_COLOR;
  PreHeaterRearLabel->Transparent                = true;
  PreHeaterRearLabel->Left                       = INT_FROM_RESOURCE(IDN_MAINTENANCE_RIGHTPART1_LEFT);
  PreHeaterRearLabel->Top                        = HeadHeatersLabels[BLOCK_1]->Top;
  */
/*  LOAD_BITMAP(SupportHeaterLabel->LeftPart,    IDB_FRAME_ONLY_LEFT);
  LOAD_BITMAP(SupportHeaterLabel->MiddlePart,  IDB_FRAME_ONLY_MIDDLE);
  LOAD_BITMAP(SupportHeaterLabel->RightPart,   IDB_FRAME_ONLY_RIGHT);
  SupportHeaterLabel->MiddlePartsNum         = INT_FROM_RESOURCE(IDN_HEAD_TYPE_MIDDLE_PARTS_NUM);
  SupportHeaterLabel->Left                   = INT_FROM_RESOURCE(IDN_SUPPORT_HEAD_TYPE_LEFT_SINGLE);

  SupportHeaterLabel->Top                    = HeadHeatersLabels[FIRST_SUPPORT_HEAD]->Top;
  SupportHeaterLabel->Width                  = INT_FROM_RESOURCE(IDN_HEAD_TYPE_WIDTH);
  SupportHeaterLabel->Font->Name             = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  SupportHeaterLabel->Font->Color            = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  SupportHeaterLabel->TransparentColor       = TRANSPARENT_COLOR;
  SupportHeaterLabel->Transparent            = true;
  SupportHeaterLabel->Font->Style      = TFontStyles()<< fsBold;
  LOAD_BITMAP(ModelHeaterLabel->LeftPart,      IDB_FRAME_ONLY_LEFT);
  LOAD_BITMAP(ModelHeaterLabel->MiddlePart,    IDB_FRAME_ONLY_MIDDLE);
  LOAD_BITMAP(ModelHeaterLabel->RightPart,     IDB_FRAME_ONLY_RIGHT);
  ModelHeaterLabel->MiddlePartsNum           = INT_FROM_RESOURCE(IDN_HEAD_TYPE_MIDDLE_PARTS_NUM);
  ModelHeaterLabel->Width                    = INT_FROM_RESOURCE(IDN_HEAD_TYPE_WIDTH);
  ModelHeaterLabel->Left                     = INT_FROM_RESOURCE(IDN_MODEL_HEAD_TYPE_LEFT);
  ModelHeaterLabel->Top                      = HeadHeatersLabels[FIRST_MODEL_HEAD]->Top;
  ModelHeaterLabel->Font->Name               = LOAD_STRING(IDS_MAINTENANCE_STATUS_FONT_NAME);
  ModelHeaterLabel->Font->Color              = LOAD_COLOR(IDC_MAINTENANCE_STATUS_FONT_COLOR);
  ModelHeaterLabel->TransparentColor         = TRANSPARENT_COLOR;
  ModelHeaterLabel->Transparent              = true;
  ModelHeaterLabel->Font->Style      = TFontStyles()<< fsBold;*/
  SupportHeaterLabel->Visible = false;
  ModelHeaterLabel->Visible   = false;
}


//---------------------------------------------------------------------------
__fastcall TMaintenanceStatusFrame::~TMaintenanceStatusFrame(void)
{}


//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::UpdateStatusIndicators(int ControlID,long Value)
{
  switch(ControlID)
  {

    case FE_CURRENT_AMBIENT_STATUS:
         AmbientTemperatureLabel->ActiveBitmap = Value;
         break;

    case FE_CURRENT_AMBIENT_TEMPERATURE:
         if (m_DisplayInA2DUnits == false)
           UpdateAmbientTemperatureLabel(Value);
         break;
    case FE_CURRENT_AMBIENT_IN_A2D:
		 if (m_DisplayInA2DUnits)
		   UpdateAmbientTemperatureLabel(Value);
		 break;

	case FE_CURRENT_EVAC_AIRFLOW_STATUS:
			UpdateEvacAirFlowColorState(Value);
		 break;
	case FE_CURRENT_EVAC_AIRFLOW_SPEED:
			UpdateEvacAirFlowLabel(Value, false);
		 break;
	case FE_CURRENT_EVAC_AIRFLOW_IN_A2D:
			UpdateEvacAirFlowLabel(Value, true);
		 break;

    case FE_CURRENT_TRAY_TEMPERATURE:
         if (m_DisplayInA2DUnits == false)
           UpdateTrayTemperatureLabel(Value);
         break;

    case FE_CURRENT_TRAY_IN_A2D:
         if (m_DisplayInA2DUnits)
           UpdateTrayTemperatureLabel(Value);
         break;

    case FE_CURRENT_TRAY_STATUS:
         TrayTemperatureLabel->ActiveBitmap = Value;
         break;

    case FE_CURRENT_VACUUM_STATUS:
         HeadsVacuumTemperatureLabel->ActiveBitmap = (Value) ? 1 : 2;
         break;

    case FE_CURRENT_VACUUM_AVERAGE_CMH2O_VALUE:
         // Display only if not displaying A/D Units
         if (m_DisplayInA2DUnits == false)
         {
            float NewValue =  *((float *)& Value); //DO NOT change it to"float NewValue = (float)Value;"
            HeadsVacuumTemperatureLabel-> Caption = Format("%.1f%",ARRAYOFCONST((NewValue)));
         }
         break;

    case FE_CURRENT_VACUUM_AVERAGE_VALUE:
         // Display only if displaying in A/D Units
         if (m_DisplayInA2DUnits == true)
            HeadsVacuumTemperatureLabel->Caption = IntToStr(Value) + " (A/D)";
         break;


    CASE_MESSAGE_ALL_LAMPS(FE_CURRENT_UV_LAMP_STATUS)
         UVLampLabelArray[ControlID - FE_CURRENT_UV_LAMP_STATUS_BASE]->ActiveBitmap = (Value != 0L) ? 1 : 0;
         break;

	case FE_CURRENT_TANK_WEIGHT_WASTE_TANK_LEFT:
         WasteWeightEnhancedLabel->Caption = IntToStr(Value);
         break;
      
	case FE_CURRENT_TANK_STATUS_WASTE_TANK_LEFT:
         WasteWeightEnhancedLabel->ActiveBitmap = ((Value != 0L) ? 1 : 2);
         m_WasteWeightCurrentStatusBmp = WasteWeightEnhancedLabel->ActiveBitmap;
         break;

    case FE_TANK_EXISTENCE_STATUS_WASTE_TANK_LEFT:
         if (Value == 0L)
         {
            WasteWeightEnhancedLabel->ActiveBitmap = 0;
            WasteWeightEnhancedLabel->Caption      = "";
         }
         else
			WasteWeightEnhancedLabel->ActiveBitmap = m_WasteWeightCurrentStatusBmp;
		 break;

	case FE_HEADS_OPERATION_MODE:

        m_ThermistorMode = (TThermistorsOperationMode)Value;
        SetLabelsPosition();

    break;


    default:
	{
	   int Tmp;
	   if (VALIDATE_CHAMBER_THERMISTOR(Tmp = ControlID - FE_CURRENT_HEAD_FILLING_IS_FILLED_BASE))
	   {
         TColor        FontColor   = clWhite;
         int           activeBmp   = 0;
         TChamberIndex currChamber1 = Thermistor2ChamberDisplay((CHAMBERS_THERMISTORS_EN)Tmp, 0);
         TChamberIndex currChamber2 = Thermistor2ChamberDisplay((CHAMBERS_THERMISTORS_EN)Tmp, 1);

         if ((NUMBER_OF_CHAMBERS == currChamber1) || (NUMBER_OF_CHAMBERS == currChamber2))
         { // error
           break;
         }

         if (Value > 1L) //update only active thermistors
         {
           break;
         }

/* DM6 *//* if ((m_ThermistorMode != LOW_THERMISTORS_MODE) != (Tmp >= NUM_OF_DM_CHAMBERS_THERMISTORS))
         { // not one of the thermistors we are interested in right now
           break;
         }*/

         if (Value == 1L)
         { // light up the indicator
           FontColor = clBlack;
           activeBmp = 1;
         }

         ResinLiquidLabelArray[currChamber1]->ActiveBitmap = activeBmp;
         ResinLiquidLabelArray[currChamber1]->Font->Color  = FontColor;
         ResinLiquidLabelArray[currChamber2]->ActiveBitmap = activeBmp;
         ResinLiquidLabelArray[currChamber2]->Font->Color  = FontColor;
       }
       else if (VALIDATE_HEATER_INCLUDING_PREHEATER(Tmp = ControlID - FE_HEAD_HEATERS_STATUS_BASE))
       {
         HeadHeatersLabels[Tmp]->ActiveBitmap = Value;
       }
       else if (VALIDATE_HEATER_INCLUDING_PREHEATER(Tmp = ControlID - FE_HEAD_HEATER_CELSIUS_BASE))
	   {
		 HeadHeatersLabels[Tmp]->Caption = IntToStr(Value);
//OBJET_MACHINE, PreHeater
		 if ((Tmp == PRE_HEATER) && !PreHeaterLabel->Visible)
		 {
			PreHeaterLabel->Visible                = true;
			HeadHeatersLabels[PRE_HEATER]->Visible = true;
		 }
       }
       break;
    }
  }
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::SetDisplayMode(bool DisplayInA2DUnits)
{
  m_DisplayInA2DUnits = DisplayInA2DUnits;
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::UpdateAmbientTemperatureLabel(long Value)
{
  AmbientLabel->Caption = (LOAD_QSTRING(IDS_AMBIENT) + "(" + LOAD_STRING((m_DisplayInA2DUnits ? IDS_AD : IDS_CEL)) + ")").c_str();
  AmbientTemperatureLabel->Caption = IntToStr(Value);
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::UpdateEvacAirFlowLabel (long Value, bool IsRawValue)
{
	if (CAppParams::Instance()->EvacuationIndicatorEnabled)
	{
//		AirFlowLabel->Visible = true;
//		AirFlowSpeedLabel->Visible = true;
		//AirFlowLabel->Caption = (LOAD_QSTRING(IDS_EVACUATION) + " (" + LOAD_STRING((m_DisplayInA2DUnits ? IDS_AD : IDS_UNITS_SPEED)) + ")").c_str();
		if (IsRawValue)
		{
			if (m_DisplayInA2DUnits)
				AirFlowSpeedLabel->Caption = IntToStr(Value);
		}
		else //value is actually in speed units [m/sec]
		{
			if (!m_DisplayInA2DUnits) {
				float NewValue = *((float *)&Value); //Do Not change it to "float NewValue = (float)Value;"
				AirFlowSpeedLabel->Caption = Format("%.2f%",ARRAYOFCONST((NewValue)));
			}
		}
	}
/*	else
	{
		AirFlowLabel->Visible = false;
		AirFlowSpeedLabel->Visible = false;
	}
*/
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::UpdateEvacAirFlowColorState(long Value)
{
	if (0L == Value)
		AirFlowSpeedLabel->ActiveBitmap = 1;
	else
		AirFlowSpeedLabel->ActiveBitmap = (0L < Value) ? 2 : 0;
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::UpdateTrayTemperatureLabel(long Value)
{
  TrayLabel->Caption = (LOAD_QSTRING(IDS_TRAY) + " (" + LOAD_STRING((m_DisplayInA2DUnits ? IDS_AD : IDS_CEL)) + ")").c_str();
  if (! CAppParams::Instance()->TrayHeaterEnabled)
  {
	TrayTemperatureLabel->Caption = "Bypass";
	TrayTemperatureLabel->Visible = false;
	TrayLabel->Visible            = false;
  }
  else
  {
    TrayTemperatureLabel->Caption = IntToStr(Value);
  }
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::SetLabelsPosition()
{
    int captionParamID = -1;

	if(Is_6_ModelHeads())
	{
		SupportHeaterLabel->Left = INT_FROM_RESOURCE(IDN_SUPPORT_HEAD_TYPE_LEFT_DM);
		for (int HeadNo = 0; HeadNo < TOTAL_NUMBER_OF_HEATERS; HeadNo++)
			HeadHeatersLabels[HeadNo]->Left = INT_FROM_RESOURCE_ARRAY(HeadNo,IDN_GRAPHIC_HEATER_LEFT_DM);
        captionParamID = IDS_GRAPHIC_CHAMBER_STR_DM;
	}
	else
	{
        SupportHeaterLabel->Left = INT_FROM_RESOURCE(IDN_SUPPORT_HEAD_TYPE_LEFT_SINGLE);
		for (int HeadNo = 0; HeadNo < TOTAL_NUMBER_OF_HEATERS; HeadNo++)
			HeadHeatersLabels[HeadNo]->Left = INT_FROM_RESOURCE_ARRAY(HeadNo,IDN_GRAPHIC_HEATER_LEFT_SINGLE);
        captionParamID = IDS_GRAPHIC_CHAMBER_STR_SINGLE;
	}
#ifdef OBJET_MACHINE_KESHET
     if ((HIGH_THERMISTORS_MODE == m_ThermistorMode) || (HIGH_AND_LOW_THERMISTORS_MODE == m_ThermistorMode))
     {
         captionParamID = IDS_GRAPHIC_CHAMBER_STR_DM3;

     }
#endif

    for (int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; ++i)
    {
        QString currCaption = LOAD_STRING_ARRAY(i,captionParamID);
        ResinLiquidLabelArray[i]->Caption = currCaption.c_str();
        ResinLiquidLabelArray[i]->Visible = (currCaption.c_str()[0] != '-') || (currCaption.c_str()[1] != '\0');
    }
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::HideWasteBar()
{
	WasteWeightEnhancedLabel->Visible = false;
	WasteEnhancedLabel->Visible       = false;

	AmbientTemperatureLabel->Top = ResinLiquidLabelArray[0]->Top;
	AmbientLabel->Top            = ResinLiquidLabelArray[0]->Top;
	UVLampsLabel->Top            = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TOP);
	FOR_ALL_UV_LAMPS(i)
	{
		UVLampLabelArray[i]->Top = INT_FROM_RESOURCE(IDN_HEAD_VACUUM_TOP);
	}
}
//---------------------------------------------------------------------------
void TMaintenanceStatusFrame::RefreshEvacuationVisibility()
{
	//If double-waste isn't enabled (i.e. single waste), there's not
	//enough visible space to occupy the Evacuation Sensor's label -
	//so don't display it altogether.
	bool show = (CAppParams::Instance()->EvacuationIndicatorEnabled) &&
				(CAppParams::Instance()->DualWasteEnabled);

	AirFlowSpeedLabel->Visible = show;
	AirFlowLabel->Visible = show;
}

//! convert thermistor ID to chamber ID for display purposes
TChamberIndex TMaintenanceStatusFrame::Thermistor2ChamberDisplay(CHAMBERS_THERMISTORS_EN origThermistor, int option)
{
    switch (origThermistor)
    {
        case SUPPORT_CHAMBER_THERMISTOR:        return TYPE_CHAMBER_SUPPORT;  // leftmost
        case M1_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL1;   // rightmost
        case M2_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL2;   // second from right
        case M3_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL3;
#ifdef OBJET_MACHINE_KESHET
        case M4_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL4;
        case M5_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL5;
        case M6_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL6;
        case M7_CHAMBER_THERMISTOR:             return TYPE_CHAMBER_MODEL7;  // second from left // support high dm6v2
#endif
        case SUPPORT_MODEL_CHAMBER_THERMISTOR:
#ifdef OBJET_MACHINE_KESHET
//                                                return TYPE_CHAMBER_SUPPORT; // DM6
                                                return ((0 == option) ? TYPE_CHAMBER_SUPPORT : TYPE_CHAMBER_MODEL7); // DM6
#else
                                                return ((0 == option) ? SUPPORT_CHAMBER_THERMISTOR : M3_CHAMBER_THERMISTOR);
#endif
        case M1_M2_CHAMBER_THERMISTOR:
#ifdef OBJET_MACHINE_KESHET
                                                return TYPE_CHAMBER_MODEL4;
#else
                                                return ((0 == option) ? M1_CHAMBER_THERMISTOR : M2_CHAMBER_THERMISTOR);
#endif
#ifdef OBJET_MACHINE_KESHET
        case M3_M4_CHAMBER_THERMISTOR:          return TYPE_CHAMBER_MODEL5;
        case M5_M6_CHAMBER_THERMISTOR:          return TYPE_CHAMBER_MODEL6;
#endif
    }
//        default:
    CQLog::Write(LOG_TAG_GENERAL, "ERROR! unspported thermistor ID %d (option %d)",origThermistor, option);
    return NUMBER_OF_CHAMBERS;
}




/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TMaintenanceStatusFrame::SelectSoftButtonClick(TObject *Sender)
{
  SelectSoftButton->ItemIndex = -1;
  MainForm->GetUIFrame()->NavigateBackward();
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/





