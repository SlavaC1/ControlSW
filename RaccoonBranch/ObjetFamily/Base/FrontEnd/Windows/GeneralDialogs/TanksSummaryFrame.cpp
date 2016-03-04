//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TanksSummaryFrame.h"
#include "GlobalDefs.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTanksSummaryFrame *TanksSummaryFrame;
//---------------------------------------------------------------------------
__fastcall TTanksSummaryFrame::TTanksSummaryFrame(TComponent* Owner)
	: TFrame(Owner)
{
	OpModePanel->BevelOuter = bvNone;
	OpModePanel->Caption    = "";

	CreateGrid();

	// Adding Left and Right titles
	TLabel *LeftLabel      = new TLabel(grdTable);
	LeftLabel->Caption     = "Left";
	LeftLabel->Font->Size  = 10;
	LeftLabel->Font->Style = TFontStyles() << fsBold << fsUnderline;
	LeftLabel->Font->Color = clHighlight;
	AddControlToGrid(LeftLabel, 1, 0);

	TLabel *RightLabel      = new TLabel(grdTable);
	RightLabel->Caption     = "Right";
	RightLabel->Font->Size  = 10;
	RightLabel->Font->Style = TFontStyles() << fsBold << fsUnderline;
	RightLabel->Font->Color = clHighlight;
	AddControlToGrid(RightLabel, 3, 0);

	// Filling the grid with initial content
	for(int i = FIRST_CHAMBER_TYPE; i < NUMBER_OF_CHAMBERS; i++)
	{
		grdTable->RowCollection->Add();

		grdTable->RowCollection->Items[i+1]->SizeStyle = ssPercent;
		grdTable->RowCollection->Items[i+1]->Value     = 100 / NUMBER_OF_CHAMBERS;

		int CurrentChamberIndex = FIRST_CHAMBER_TYPE + i;

		TLabel *ChamberLabel  = new TLabel(grdTable);
		ChamberLabel->Caption = ChamberToStr(static_cast<TChamberIndex>(NUMBER_OF_CHAMBERS - 1 - CurrentChamberIndex)).c_str();

		TLabel *LeftMaterialLabel      = new TLabel(grdTable);
		LeftMaterialLabel->Font->Size  = 12;
		LeftMaterialLabel->Caption     = "";
		LeftMaterialLabel->AutoSize    = true;
		LeftMaterialLabel->Font->Style = TFontStyles() << fsBold;

		TLabel *RightMaterialLabel      = new TLabel(grdTable);
		RightMaterialLabel->Font->Size  = 12;
		RightMaterialLabel->Caption     = "";
		RightMaterialLabel->AutoSize    = true;
		RightMaterialLabel->Font->Style = TFontStyles() << fsBold;

		TImage *LeftIcon 	  = new TImage(grdTable);
		LeftIcon->Transparent = true;
		LeftIcon->ShowHint    = true;
		LeftIcon->Hint        = "";
		LeftIcon->Width       = 16;
		LeftIcon->Height      = 16;

		TImage *RightIcon 	   = new TImage(grdTable);
		RightIcon->Transparent = true;
		RightIcon->ShowHint    = true;
		RightIcon->Hint        = "";
		RightIcon->Width       = 16;
		RightIcon->Height      = 16;

		IconsImageList->GetBitmap(3 /* STATUS_EMPTY */, LeftIcon->Picture->Bitmap);
		IconsImageList->GetBitmap(3 /* STATUS_EMPTY */, RightIcon->Picture->Bitmap);

		AddControlToGrid(RightIcon, 		 GRID_RIGHT_STATUS, i+1);
		AddControlToGrid(RightMaterialLabel, GRID_RIGHT_RESIN, 	i+1);
		AddControlToGrid(LeftIcon, 		     GRID_LEFT_STATUS, 	i+1);
		AddControlToGrid(LeftMaterialLabel,  GRID_LEFT_RESIN, 	i+1);
		AddControlToGrid(ChamberLabel, 	     GRID_CHAM_NAME, 	i+1); //col #0 must be the last op
	}
}

void TTanksSummaryFrame::CreateGrid()
{
	grdTable = new TGridPanel(TanksGroupBox);

	grdTable->Parent     = TanksGroupBox;
	grdTable->BevelOuter = bvNone;
	grdTable->Align      = alClient;

    // By default TGridPanel has 2 rows and 2 columns. We add additional 3 columns here. Rows will be added in a loop.
	grdTable->ColumnCollection->Add();
	grdTable->ColumnCollection->Add();
	grdTable->ColumnCollection->Add();

	grdTable->RowCollection->Items[0]->SizeStyle = ssAbsolute;
	grdTable->RowCollection->Items[0]->Value     = 35;

	grdTable->ColumnCollection->Items[0]->SizeStyle = ssAbsolute; // chambers
	grdTable->ColumnCollection->Items[0]->Value     = 40;
	grdTable->ColumnCollection->Items[1]->SizeStyle = ssPercent;  // left material
	grdTable->ColumnCollection->Items[1]->Value     = 50;
	grdTable->ColumnCollection->Items[2]->SizeStyle = ssAbsolute; // left icon
	grdTable->ColumnCollection->Items[2]->Value     = 30;
	grdTable->ColumnCollection->Items[3]->SizeStyle = ssPercent;  // right material
	grdTable->ColumnCollection->Items[3]->Value     = 50;
	grdTable->ColumnCollection->Items[4]->SizeStyle = ssAbsolute; // right icon
	grdTable->ColumnCollection->Items[4]->Value     = 30;
}

void TTanksSummaryFrame::AddControlToGrid(TControl *comp, int column, int row)
{
    //Auto inserts the component to the next available cell in grid. The first cell is available, so it will be inserted into [0][0]
	comp->Parent = grdTable;

	// Moves the component in [0][0] to the required position by Row and Column
	grdTable->ControlCollection->ControlItems[0][0]->Row      = row;
	grdTable->ControlCollection->ControlItems[0][row]->Column = column;
}

int TTanksSummaryFrame::TankIndexToMaterialColumn(int TankIndex)
{
	return (TankIndex % 2 == 0) ? GRID_LEFT_RESIN : GRID_RIGHT_RESIN;
}

int TTanksSummaryFrame::TankIndexToRow(int TankIndex)
{
	return 4 - TankIndex / 2;
}

int TTanksSummaryFrame::TankIndexToStatusColumn(int TankIndex)
{
	return (TankIndex % 2 == 0) ? GRID_LEFT_STATUS : GRID_RIGHT_STATUS;
}
//---------------------------------------------------------------------------
