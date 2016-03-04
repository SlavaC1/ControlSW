//---------------------------------------------------------------------------

#ifndef MainStatusScreenH
#define MainStatusScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LCDBaseScreen.h"
#include <ExtCtrls.hpp>
#include "EnhancedLabel.hpp"
#include <Graphics.hpp>
#include "EnhancedProgressBar.hpp"
#include "ThreeLayersGraphic.hpp"
#include <ComCtrls.hpp>
#include "EnhancedMenu.hpp"
#include <ImgList.hpp>
#include "GlobalDefs.h"
//---------------------------------------------------------------------------

typedef enum
{
   Container1 = 1,
   Container2 = 2
}TContainer;


class TMainStatusFrame : public TLCDBaseFrame
{
__published:	// IDE-managed Components
        TEnhancedLabel *CurrentStateLabel;
        TEnhancedLabel *PrintTimeLabel;
        TEnhancedLabel *SliceProgressLabel;
        TEnhancedLabel *HeightProgressLabel;
        TEnhancedLabel *PreviousJobLabel;
        TEnhancedLabel *SubStatusLabel;
        TEnhancedLabel *ServiceAlertLabel;
        TEnhancedProgressBar *PrintProgressBar;

        TTimer *S2DrainTimer;
        TTimer *M2DrainTimer;
        TTimer *M1DrainTimer;
        TTimer *S1DrainTimer;
        void __fastcall SelectSoftButtonClick(TObject *Sender);
        void __fastcall ServiceAlertLabelClick(TObject *Sender);
        void __fastcall TankGraphicsLayer2DrawEvent(TObject *Sender, TCanvas *DrawCanvas);
        void __fastcall DrainTimerTimer(TObject *Sender);
private:	// User declarations

  int        m_TankRectHeightArray      [TOTAL_NUMBER_OF_CONTAINERS];
  int        m_CurrentActiveTankArray   [NUMBER_OF_CHAMBERS];
  TTankIndex m_ActiveTankArray          [NUMBER_OF_CHAMBERS];

  bool    m_TankStatusArray         [TOTAL_NUMBER_OF_CONTAINERS];
  bool    m_TankVisibleArray        [TOTAL_NUMBER_OF_CONTAINERS];
  TLabel* m_ResinTypeLabelArray     [TOTAL_NUMBER_OF_CONTAINERS];
  TLabel* m_ResinNameLabelArray     [TOTAL_NUMBER_OF_CONTAINERS];
  TImage* m_DrainImageArray         [TOTAL_NUMBER_OF_CONTAINERS];
  TImage* m_TankEnableArray         [TOTAL_NUMBER_OF_CONTAINERS];

  void    RepaintContainer(TTankIndex Tank);
  bool    IsActive(TTankIndex Tank);
  TTimer  *m_DrainTimers[4];

public:		// User declarations
  TLabel*              m_ResinWeightLabelArray[TOTAL_NUMBER_OF_CONTAINERS];
  TThreeLayersGraphic* m_TankGraphicsArray    [TOTAL_NUMBER_OF_CONTAINERS];

        __fastcall TMainStatusFrame(TComponent* Owner);

  void SetContainerData(TTankIndex Tank, float FullPercent);
  void SetContainerStatus(TTankIndex Tank, bool Status);
  void UpdateTankEnabledStatus(TTankIndex Tank, bool Visible);
  void SetDisabledIconHint(TTankIndex Tank,  AnsiString Hint);  

  void SetContainerVisible(TTankIndex Tank, bool Visible);

  void SetActiveContainer        (TChamberIndex Chamber, TTankIndex ActiveTank);
  void UpdateResinType           (TTankIndex Tank,       AnsiString ResinType);
  void UpdateDrainPumpsStatus    (TChamberIndex Chamber, TTankIndex ActiveTank);
  void __fastcall DynamicOnDblClick         (TObject *Sender);
};

#endif
