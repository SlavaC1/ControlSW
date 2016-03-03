//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "WeightsStatusFrame.h"
#include "FEResources.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWeightsStatusFrame *WeightsStatusFrame;
//---------------------------------------------------------------------------
__fastcall TWeightsStatusFrame::TWeightsStatusFrame(TComponent* Owner)
	: TFrame(Owner)
{
	#if defined(OBJET_1000)
	for(int i = 0; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	{
	  m_containersPanel[i]                 = new TPanel(this);
	  m_containersPanel[i]->Parent         = WeightsStatusGroupBox;
	  m_containersPanel[i]->Left           = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_LEFT);
	  m_containersPanel[i]->Width          = 77;
	  m_containersPanel[i]->Height         = 20;
	  m_containersPanel[i]->Top            = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_TOP);
	  m_containersPanel[i]->BevelOuter     = bvNone;
	  m_containersPanel[i]->BevelInner     = bvLowered;
	  m_containersPanel[i]->Caption        = "1780";
	  m_containersPanel[i]->Tag        	   = i;
	  m_containersPanel[i]->ParentBackground = true;
	}
	pgc1->Pages[1]->TabVisible = false; /*no need to show second tab*/
	#else
	for(int i = 0; i < TYPE_TANK_MODEL7; i++)
	{
	  m_containersPanel[i]                 = new TPanel(this);
	  m_containersPanel[i]->Parent         = WeightsStatusGroupBox;
	  m_containersPanel[i]->Left           = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_WEIGHT_LEFT);
	  m_containersPanel[i]->Width          = 45;
	  m_containersPanel[i]->Height         = 20;
	  m_containersPanel[i]->Top            = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_TOP);
	  m_containersPanel[i]->BevelOuter     = bvNone;
	  m_containersPanel[i]->BevelInner     = bvLowered;
	  m_containersPanel[i]->Caption        = "1780";
	  m_containersPanel[i]->Tag        	   = i;
	  m_containersPanel[i]->ParentBackground = true;
      }

	for(int i = TYPE_TANK_MODEL7; i < TOTAL_NUMBER_OF_CONTAINERS; i++)
	{
	  m_containersPanel[i]                 = new TPanel(this);
	  m_containersPanel[i]->Parent         = WeightsStatusGroupBox;
	  m_containersPanel[i]->Left           = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_WEIGHT_LEFT)+115;
	  m_containersPanel[i]->Width          = 45;
	  m_containersPanel[i]->Height         = 20;
	  m_containersPanel[i]->Top            = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_TOP);
	  m_containersPanel[i]->BevelOuter     = bvNone;
	  m_containersPanel[i]->BevelInner     = bvLowered;
	  m_containersPanel[i]->Caption        = "1780";
	  m_containersPanel[i]->Tag        	   = i;
	  m_containersPanel[i]->ParentBackground = true;
	}

	for(int i = TOTAL_NUMBER_OF_CONTAINERS; i < TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE; i++)
	{
	  m_containersPanel[i]                 = new TPanel(this);
	  m_containersPanel[i]->Parent         = WeightsStatusGroupBox;
	  m_containersPanel[i]->Left           = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_WEIGHT_LEFT);
	  m_containersPanel[i]->Width          = 45;
	  m_containersPanel[i]->Height         = 20;
	  m_containersPanel[i]->Top            = INT_FROM_RESOURCE_ARRAY(i,IDN_LOAD_CELL_CALIBRATION_CHBX_TOP);
	  m_containersPanel[i]->BevelOuter     = bvNone;
	  m_containersPanel[i]->BevelInner     = bvLowered;
	  m_containersPanel[i]->Caption        = "1780";
	  m_containersPanel[i]->Tag        	   = i;
	  m_containersPanel[i]->ParentBackground = true;
	}

	#endif
	//WasteRightPNL->Tag   = TYPE_TANK_WASTE_RIGHT;



	#if defined(OBJET_1000)
         int i = 0;
    int c=0;
	int TopStart    = 95;
	int LeftStart   = 9;
	int VerticalGap = 26;
	int MoveHeight  = 0;

	for( i = FIRST_TANK_INCLUDING_WASTE_TYPE; i < TYPE_TANK_MODEL7; i++)
	{
		TLabel *Label = new TLabel(this->WeightsStatusGroupBox);

		if(i == LAST_MODEL_TANK_TYPE)
			MoveHeight = -VerticalGap;

		if(! CAppParams::Instance()->DualWasteEnabled)
			if(i == LAST_MODEL_TANK_TYPE)
				LeftStart += 75;

		Label->Parent     = this->WeightsStatusGroupBox;
		Label->Left       = LeftStart + ((i % 2 == 0) ? 0 : 267);
		Label->Top        = TopStart  - MoveHeight;
		Label->Caption    = TankToStr(static_cast<TTankIndex>(i)).c_str();
		Label->Font->Size = 8;
		Label->Tag        = i;
		#ifdef LIMIT_TO_DM2
		if(i == TYPE_TANK_MODEL5 || i == TYPE_TANK_MODEL6)
			Label->Visible = false;
		#endif

		if(i % 2 != 0)
			MoveHeight += VerticalGap;
	}
	#endif


}
//---------------------------------------------------------------------------
