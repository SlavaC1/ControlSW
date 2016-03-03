//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TanksSummaryFrame.h"
#include "GlobalDefs.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "FEResources.h"  //LOAD_STRING_ARRAY
#include "QStringList.h"  //LOAD_STRING_ARRAY
#include <dir.h> //getcwd

#define CHAMBERS_PER_CABINET (NUMBER_OF_CHAMBERS / NUM_CABINETS)
#define FOR_EACH_CABINET(j) for (int j = 0; (j) < NUM_CABINETS; ++(j))
#define FOR_EACH_CABINET_REVERSE(j) for (int j = NUM_CABINETS-1; (j) >= 0; --(j))
#define IS_NOT_LAST_ITERATION_CABINET(j) ((j) < NUM_CABINETS-1)
#define IS_NOT_LAST_ITERATION_CABINET_REVERSE(j) ((j) > 0)

TTanksSummaryFrame *TanksSummaryFrame;
//---------------------------------------------------------------------------
__fastcall TTanksSummaryFrame::TTanksSummaryFrame(TComponent* Owner)
	: TFrame(Owner)
{
	OpModePanel->BevelOuter = bvNone;
	OpModePanel->Caption    = "";

	CreateGrid();

	// Adding Left and Right titles
	FOR_EACH_CABINET(j) {
		TLabel *leftLabel      = new TLabel(grdTable[j]);
		leftLabel->Caption     = LOAD_STRING_ARRAY(0,IDS_CABINET_CARTRIDGE_POS_STRINGS); //"Left";
		leftLabel->Font->Size  = 10;
		leftLabel->Font->Style = TFontStyles() << fsBold << fsUnderline;
		leftLabel->Font->Color = clHighlight;
		AddControlToGrid(j, leftLabel, 1, 0);

		TLabel *rightLabel      = new TLabel(grdTable[j]);
		rightLabel->Caption     = LOAD_STRING_ARRAY(1,IDS_CABINET_CARTRIDGE_POS_STRINGS); //"Right";
		rightLabel->Font->Size  = 10;
		rightLabel->Font->Style = TFontStyles() << fsBold << fsUnderline;
		rightLabel->Font->Color = clHighlight;
		AddControlToGrid(j, rightLabel, 3, 0);
	}

	//based on the usage of getcwd in WizardViewer::GetPageImage() (WizardImages.h)
	char* CWD = getcwd(NULL, 100);
	QString innerImageBasePath = CWD;

	// Filling the grid with initial content
	int rowHeight = 100 / (CHAMBERS_PER_CABINET+1 /* +1 for header */);
	FOR_EACH_CABINET(j) {
		for (int i = FIRST_CHAMBER_TYPE; i < CHAMBERS_PER_CABINET; ++i)
		{
			grdTable[j]->RowCollection->Add();

			grdTable[j]->RowCollection->Items[i+1]->SizeStyle = ssPercent;
			grdTable[j]->RowCollection->Items[i+1]->Value     = rowHeight;
			int CurrentChamberIndex = i + ((j % 2 == 0) ? CHAMBERS_PER_CABINET : 0);

			TLabel *ChamberLabel  = new TLabel(grdTable[j]);
			ChamberLabel->Caption = ChamberToStr(static_cast<TChamberIndex>(NUMBER_OF_CHAMBERS - 1 - CurrentChamberIndex)).c_str();

			TLabel *LeftMaterialLabel      = new TLabel(grdTable[j]);
			LeftMaterialLabel->Font->Size  = 12;
			LeftMaterialLabel->Caption     = "";
			LeftMaterialLabel->AutoSize    = true;
			LeftMaterialLabel->Font->Style = TFontStyles() << fsBold;

			TLabel *RightMaterialLabel      = new TLabel(grdTable[j]);
			RightMaterialLabel->Font->Size  = 12;
			RightMaterialLabel->Caption     = "";
			RightMaterialLabel->AutoSize    = true;
			RightMaterialLabel->Font->Style = TFontStyles() << fsBold;

			TImage *LeftIcon 	  = new TImage(grdTable[j]);
			LeftIcon->Transparent = true;
			LeftIcon->ShowHint    = true;
			LeftIcon->Hint        = "";
			LeftIcon->Width       = 16;
			LeftIcon->Height      = 16;

			TImage *RightIcon 	   = new TImage(grdTable[j]);
			RightIcon->Transparent = true;
			RightIcon->ShowHint    = true;
			RightIcon->Hint        = "";
			RightIcon->Width       = 16;
			RightIcon->Height      = 16;

			IconsImageList->GetBitmap(IconsImageList->Count +1 /* STATUS_EMPTY */, LeftIcon->Picture->Bitmap);
			IconsImageList->GetBitmap(IconsImageList->Count +1 /* STATUS_EMPTY */, RightIcon->Picture->Bitmap);

			AddControlToGrid(j, RightIcon, 		 	GRID_RIGHT_STATUS,  i+1);
			AddControlToGrid(j, RightMaterialLabel, GRID_RIGHT_RESIN, 	i+1);
			AddControlToGrid(j, LeftIcon, 		    GRID_LEFT_STATUS, 	i+1);
			AddControlToGrid(j, LeftMaterialLabel,  GRID_LEFT_RESIN, 	i+1);
			AddControlToGrid(j, ChamberLabel, 	    GRID_CHAM_NAME, 	i+1); //col #0 must be the last op
		}

		//col #0 must be the last op
		try {
			QString innerImage = innerImageBasePath + LOAD_STRING_ARRAY(j, IDS_CABINETS_NAMES_STRINGS);
			TImage *cabinetCaption = new TImage(grdTable[j]);
			cabinetCaption->Transparent = true;
			cabinetCaption->Picture->LoadFromFile(innerImage.c_str());

			cabinetCaption->Width  = cabinetCaption->Picture->Width;
			cabinetCaption->Height = cabinetCaption->Picture->Height;
			cabinetCaption->Visible = true;
			cabinetCaption->Repaint();

			AddControlToGrid(j, cabinetCaption, 0, 0);
		} catch (...) {
		}
	}
	free(CWD);
}

void TTanksSummaryFrame::ResizeCabinetsInfo(
	int Height, int Width, const QString AuxImgPath, int PaddingBottom)
{
	int newHeight = (Height - OpModePanel->Height -15 );
	int newWidth = Width -13; //right side of the grid's bevel
	int accumulatedImagesWidth = 0;
	FOR_EACH_CABINET(j) {
	
		grdTable[j]->Width		= newWidth / NUM_CABINETS;
		grdTable[j]->Height		= newHeight;
		grdTable[j]->Padding->Bottom = PaddingBottom;

		if (IS_NOT_LAST_ITERATION_CABINET(j)) {		
			if (AuxImgPath.empty()) {
				imgGrid[j]->Visible = false;
			}
			else
			{
				if (FileExists(AuxImgPath.c_str()))
				{
					try {
						TImage*& curr = imgGrid[j];
						curr->Transparent = true;
						//TransparentMode==tmAuto which means that
						//TransparentColor property is sampled from the bottom-leftmost pixel
						curr->Picture->LoadFromFile(AuxImgPath.c_str());
						curr->Width = imgGrid[j]->Picture->Width;
						curr->Height = imgGrid[j]->Picture->Height;
						accumulatedImagesWidth += curr->Width;
						curr->Visible = true;
						curr->Repaint();
					} catch (...) {
						imgGrid[j]->Visible = false;
					}
				} else
				{
					imgGrid[j]->Visible = false;
				}
			}
		}
	}

	//fix percentage-based width of columns by considering the image's relative width
	if (accumulatedImagesWidth > 0) {
		FOR_EACH_CABINET(j) {
			int delta = accumulatedImagesWidth / NUM_CABINETS;
			grdTable[j]->Width -= delta;
		}
	}
}

void TTanksSummaryFrame::CreateGrid()
{
	int forwardCounter = 0; //used to count iteration number onward, starting from 0
//	FOR_EACH_CABINET_REVERSE(j) {
	FOR_EACH_CABINET(j) {

		grdTable[j] = new TGridPanel(TanksGroupBox);
		TGridPanel*& currGrid = grdTable[j];

		currGrid->Parent     = TanksGroupBox;
		currGrid->BevelOuter = bvLowered; //bvNone;
		currGrid->BorderStyle = bsSingle;
		currGrid->VerticalAlignment = taVerticalCenter;

		//Changing the width/height won't affect the outcome as long as the frame's size
		//is not preserved when put inside another frame/form (such as CustomResinSelectSummaryFrame).
		//On the other hand, the Align property uses the parent's (container) properties.
		currGrid->Align      = alLeft; //alTop; //alClient is suitable for a single grid
		currGrid->Padding->Left  = 3; //So that longer captions won't colide with the bevel
//		currGrid->Padding->Bottom = 10;

		// By default TGridPanel has 2 rows and 2 columns. We add additional 3 columns here. Rows will be added in a loop.
		currGrid->ColumnCollection->Add();
		currGrid->ColumnCollection->Add();
		currGrid->ColumnCollection->Add();

		//Header: Left/Right
		currGrid->RowCollection->Items[0]->SizeStyle = ssAbsolute;//ssPercent;//ssAbsolute;
		currGrid->RowCollection->Items[0]->Value     = 35;//100 / (CHAMBERS_PER_CABINET+1);//35;

		currGrid->ColumnCollection->Items[GRID_CHAM_NAME]->SizeStyle = ssAbsolute; // chambers
		currGrid->ColumnCollection->Items[GRID_CHAM_NAME]->Value     = 50;
		currGrid->ColumnCollection->Items[GRID_LEFT_RESIN]->SizeStyle = ssPercent;  // left material
		currGrid->ColumnCollection->Items[GRID_LEFT_RESIN]->Value     = 50;
		currGrid->ColumnCollection->Items[GRID_LEFT_STATUS]->SizeStyle = ssAbsolute; // left icon
		currGrid->ColumnCollection->Items[GRID_LEFT_STATUS]->Value     = 30;
		currGrid->ColumnCollection->Items[GRID_RIGHT_RESIN]->SizeStyle = ssPercent;  // right material
		currGrid->ColumnCollection->Items[GRID_RIGHT_RESIN]->Value     = 50;
		currGrid->ColumnCollection->Items[GRID_RIGHT_STATUS]->SizeStyle = ssAbsolute; // right icon
		currGrid->ColumnCollection->Items[GRID_RIGHT_STATUS]->Value     = 34;

		//Add a place holder for an image
		//if (IS_NOT_LAST_ITERATION_CABINET_REVERSE(j))
		if (IS_NOT_LAST_ITERATION_CABINET(j))
		{    
			imgGrid[forwardCounter] = new TImage(TanksGroupBox);
			TImage*& curr = imgGrid[forwardCounter];
			//Place the image on grid, visually
			curr->Parent  = TanksGroupBox;
			curr->Visible = false;
			curr->Align = alLeft;
			curr->Left = currGrid->Left + currGrid->Width + 4;
			
			++forwardCounter;
        }
	}
}

void TTanksSummaryFrame::AddControlToGrid(int tableIdx, TControl *comp, int column, int row)
{
    //Auto inserts the component to the next available cell in grid. The first cell is available, so it will be inserted into [0][0]
	comp->Parent = grdTable[tableIdx];

	// Moves the component in [0][0] to the required position by Row and Column
	grdTable[tableIdx]->ControlCollection->ControlItems[0][0]->Row      = row;
	grdTable[tableIdx]->ControlCollection->ControlItems[0][row]->Column = column;
}

int TTanksSummaryFrame::TankIndexToMaterialColumn(int TankIndex) const
{
	return (TankIndex % 2 == 0) ? GRID_LEFT_RESIN : GRID_RIGHT_RESIN;
}

int TTanksSummaryFrame::TankIndexToRow(int TankIndex) const
{
	return CHAMBERS_PER_CABINET - ((TankIndex / 2) % CHAMBERS_PER_CABINET);
}

int TTanksSummaryFrame::TankIndexToStatusColumn(int TankIndex) const
{
	return (TankIndex % 2 == 0) ? GRID_LEFT_STATUS : GRID_RIGHT_STATUS;
}

int TTanksSummaryFrame::TankIndexToTable(int TankIndex) const
{
	return ((TankIndex / 2) / CHAMBERS_PER_CABINET);
}
//---------------------------------------------------------------------------
