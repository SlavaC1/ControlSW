//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"

#include "NozzlesMap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TNozzlesMapFrame *NozzlesMapFrame;

//---------------------------------------------------------------------------

__fastcall TNozzlesMapFrame::TNozzlesMapFrame(TComponent* Owner)
	: TFrame(Owner)
{
    m_AllState = false;

	for(int i = 0; i < NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN; i++)
		m_ColumnsState[i] = false;

	for(int i = 0; i < NOZZLES_IN_GEN4_HEAD; i++)
		m_CheckboxesState[i] = false;

	CreateCheckboxes();
}
//---------------------------------------------------------------------------

void TNozzlesMapFrame::CreateCheckboxes()
{
	int top    = LeftLabel->Top + LeftLabel->Height + 5;
	int left   = 0;
	int xDelta = 15;
	int yDelta = 15;
	int index  = 0;
	int hint   = 1;
	int BtnTop = 0;

	for(int col = 0; col < NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN; col++)
	{
        // Split columns into 2 sections
		left += (col == NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN / 2) ? xDelta * 5 : xDelta;
		hint  = (col == NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN / 2) ? 1 : hint;

		for(int row = 0; row < NOZZLES_IN_COLUMN; row++, index++, hint++)
		{
			TCheckBox *CheckBox = new TCheckBox(this);

			CheckBox->Parent   = this;
			CheckBox->Top      = top + row * yDelta;
			CheckBox->Left     = left;
			CheckBox->Tag      = index;
			CheckBox->Hint     = hint;
			CheckBox->ShowHint = true;
            CheckBox->OnClick  = SetCheckboxEvent;

			m_Checkboxes[NOZZLES_IN_COLUMN * col + row] = CheckBox;

			BtnTop = CheckBox->Top + yDelta + 7;
		}

        // Column selection button
		TButton *Button = new TButton(this);
		Button->Parent  = this;
		Button->Top     = BtnTop;
		Button->Left    = left - 2;
		Button->Height  = 16;
		Button->Width   = 16;
		Button->Tag     = col;
		Button->Caption = "V";
		Button->OnClick = SelectColumnButtonClick;
		m_Buttons[col]  = Button;

		// Set / Clear all button
		if(col == NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN / 2)
		{
			TButton *Btn = new TButton(this);
			Btn->Parent  = this;
			Btn->Top     = BtnTop;
			Btn->Left    = left - 50;
			Btn->Height  = 16;
			Btn->Width   = 38;
			Btn->Tag     = -1;
			Btn->Caption = "all";
			Btn->OnClick = SetAllButtonClick;
		}
	}
}

void __fastcall TNozzlesMapFrame::SetCheckboxEvent(TObject *Sender)
{
	TCheckBox *CheckBox = dynamic_cast<TCheckBox *>(Sender);

	if(CheckBox)
    	m_CheckboxesState[CheckBox->Tag] = CheckBox->Checked;
}

// Set / clear all check boxes for the current head
void __fastcall TNozzlesMapFrame::SetAllButtonClick(TObject *Sender)
{
	for(int i = 0; i < NOZZLES_IN_GEN4_HEAD / NOZZLES_IN_COLUMN; i++)
	{
		m_ColumnsState[i] = (m_AllState) ? true : false;
		m_Buttons[i]->Click();
	}
	m_AllState = (m_AllState) ? false : true;
}

void TNozzlesMapFrame::SetAllState(bool State)
{
	m_AllState = State;
}

void __fastcall TNozzlesMapFrame::SelectColumnButtonClick(TObject *Sender)
{
	TButton *Btn = dynamic_cast<TButton *>(Sender);

	if(Btn)
	{
		int StartIndex, EndIndex;

        // Toggle state with each click
		bool NewState = (m_ColumnsState[Btn->Tag]) ? false : true;
		Btn->Caption  = (NewState) ? "X" : "V";

		ColumnToNozzleRange (StartIndex, EndIndex, Btn->Tag);
		SetCheckboxesInRange(StartIndex, EndIndex, NewState);

		m_ColumnsState[Btn->Tag] = NewState;
    }
}

// Column index to nozzle range conversion
void TNozzlesMapFrame::ColumnToNozzleRange(int &StartIndex, int &EndIndex, int Col)
{
	StartIndex = Col * NOZZLES_IN_COLUMN;
	EndIndex   = StartIndex + NOZZLES_IN_COLUMN;
}

void TNozzlesMapFrame::SetCheckboxesInRange(int Start, int End, bool State)
{
	for(int i = Start; i < End; i++)
	{
		m_Checkboxes[i]->Checked = State;
		m_CheckboxesState[i]     = State;
    }
}

// Return the state of all nozzles (checkboxes) of current head
bool* TNozzlesMapFrame::GetNozzles()
{
    return m_CheckboxesState;
}

void TNozzlesMapFrame::SetNozzles(bool *Nozzles)
{
	memcpy(m_CheckboxesState, Nozzles, sizeof(m_CheckboxesState));

	for(int i = 0; i < NOZZLES_IN_GEN4_HEAD; i++)
	{
		if(m_CheckboxesState[i])
			m_Checkboxes[i]->Checked = true;
		else
			m_Checkboxes[i]->Checked = false;
	}
}
