//---------------------------------------------------------------------------

#ifndef TanksSummaryFrameH
#define TanksSummaryFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "QComponent.h" //QString
//---------------------------------------------------------------------------

#define NUM_CABINETS 2
//grid's columns indexes
enum
{
	GRID_CHAM_NAME = 0,
	GRID_LEFT_RESIN,
	GRID_LEFT_STATUS,
	GRID_RIGHT_RESIN,
	GRID_RIGHT_STATUS,
	LAST_GRID_COLUMN = GRID_RIGHT_STATUS
};

class TTanksSummaryFrame : public TFrame
{
__published:	// IDE-managed Components
	TGroupBox *TanksGroupBox;
	TImageList *IconsImageList;
	TPanel *OpModePanel;
	TButton *btnStart;
private:
	TImage *imgGrid[NUM_CABINETS-1]; //in-between grid tables image

public:
	TGridPanel *grdTable[NUM_CABINETS];

private:	// User declarations

	void AddControlToGrid(int tableIdx, TControl *comp, int column, int row);
	void CreateGrid();

public:		// User declarations
	__fastcall TTanksSummaryFrame(TComponent* Owner);
	int TankIndexToMaterialColumn(int TankIndex) const;
	int TankIndexToRow(int TankIndex) const;
	int TankIndexToStatusColumn(int TankIndex) const;
	int TankIndexToTable(int TankIndex) const;
	void ResizeCabinetsInfo(int Height, int Width, const QString AuxImgPath, int PaddingBottom=0);
};
//---------------------------------------------------------------------------
extern PACKAGE TTanksSummaryFrame *TanksSummaryFrame;
//---------------------------------------------------------------------------
#endif
