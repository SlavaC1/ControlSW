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
//---------------------------------------------------------------------------

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

private:	// User declarations

	void AddControlToGrid(TControl *comp, int column, int row);
	void CreateGrid();

public:		// User declarations

	TGridPanel *grdTable;
	__fastcall TTanksSummaryFrame(TComponent* Owner);
	int TankIndexToMaterialColumn(int TankIndex);
	int TankIndexToRow(int TankIndex);
	int TankIndexToStatusColumn(int TankIndex);
};
//---------------------------------------------------------------------------
extern PACKAGE TTanksSummaryFrame *TanksSummaryFrame;
//---------------------------------------------------------------------------
#endif
