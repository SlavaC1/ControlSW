//---------------------------------------------------------------------------

#ifndef MaintenanceStatusScreenH
#define MaintenanceStatusScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "EnhancedMenu.hpp"
#include "LCDBaseScreen.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "EnhancedLabel.hpp"
#include <jpeg.hpp>
//---------------------------------------------------------------------------

class TMaintenanceStatusFrame : public TLCDBaseFrame
{
__published:	// IDE-managed Components
        TEnhancedLabel *HeadsLabel;
        TEnhancedLabel *TrayLabel;
        TEnhancedLabel *AmbientLabel;
        TEnhancedLabel *UVLampsLabel;
        TEnhancedLabel *HeadsLiquidLabel;
        TEnhancedLabel *TrayTemperatureLabel;
        TEnhancedLabel *AmbientTemperatureLabel;
        TEnhancedLabel *HeadsVacuumLabel;
        TEnhancedLabel *HeadsVacuumTemperatureLabel;
	TEnhancedLabel *PreHeaterFrontLabel;
        TEnhancedLabel *WasteEnhancedLabel;
        TEnhancedLabel *WasteWeightEnhancedLabel;
        TEnhancedLabel *SupportHeaterLabel;
        TEnhancedLabel *ModelHeaterLabel;
	TEnhancedLabel *PreHeaterRearLabel;
	TEnhancedLabel *AirFlowLabel;
	TEnhancedLabel *AirFlowSpeedLabel;
	TEnhancedLabel *PreHeaterLabel;

        void __fastcall SelectSoftButtonClick(TObject *Sender);
private:	// User declarations

  //Members variables
  TEnhancedLabel* HeadHeatersLabels    [TOTAL_NUMBER_OF_HEATERS];
  TEnhancedLabel* ResinLiquidLabelArray[NUMBER_OF_CHAMBERS];
  TEnhancedLabel* UVLampLabelArray     [NUMBER_OF_UV_LAMPS];

  int                       m_WasteWeightCurrentStatusBmp;
  bool                      m_DisplayInA2DUnits;
  TThermistorsOperationMode m_ThermistorMode;

  TChamberIndex  Thermistor2ChamberDisplay(CHAMBERS_THERMISTORS_EN origThermistor, int option);

  //Members function
  
public:		// User declarations
  __fastcall TMaintenanceStatusFrame(TComponent* Owner);
  __fastcall ~TMaintenanceStatusFrame(void);

  void SetDisplayMode(bool DisplayInA2DUnits);
	void UpdateStatusIndicators(int ControlID,long Value);
	void UpdateAmbientTemperatureLabel(long Value);
	void UpdateTrayTemperatureLabel(long Value);
	void UpdateEvacAirFlowLabel(long Value, bool IsRawValue);
	void UpdateEvacAirFlowColorState(long value);
	void SetLabelsPosition();
	void HideWasteBar();
	void RefreshEvacuationVisibility();
};

#endif
