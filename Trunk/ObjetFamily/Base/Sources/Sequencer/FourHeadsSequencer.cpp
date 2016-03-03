/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Unidirection sequencer.                                  *
 * Module Description: Implementation of the specific Q2RT          *
 *                     four heads sequencer.                        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 13/06/2002                                           *
 ********************************************************************/

#include "MachineSequencer.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "Motor.h"
#include "LayerProcess.h"
#include "Layer.h"
#include "configuration.h"
#include "Q2RTApplication.h"
#include "PrintControl.h"
#include "FIFOPci.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "ExcelSequenceAnalyzer.h"

// Print sequencer  4 heads or (8 heads and 600 DPI in Y)
TQErrCode CMachineSequencer::DoubleInterlacePrintSequencer(CLayer *Layer)
{
    TQErrCode Err;
    m_Layer = Layer;
	try
	{
		InitLayerPrintingSequence();
// Perform the movement until the end of the layer
		while(IsIntermediatePass())
		{
			if(IsPassNotEmpty())
				PrintPass(); // When PrintPass() returns, X_Axis only starts it's last move backwards.
			else
				CQLog::Write(LOG_TAG_PRINT,"Pass Number = %d is empty",m_PassInLayer);

			IncrementPass();
			CalcIsPassNotEmpty();
			CalcRelativeYLocation(m_SliceNumModifier);
			if (IsIntermediatePass() && IsPassNotEmpty())
			{							
				PrepareForNextPassSequence();
				Err = CheckEndOfPass();
// In a case of FIFO not empty error, break current layer print
				if (Err == Q2RT_FIFO_NOT_EMPTY_ERROR)
					break;
// OHDB handle that should be performed after each pass
				if (Err != Q_NO_ERROR)
				{
					CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice %d.", m_Layer->GetSliceNumber());
					throw EMachineSequencer("CheckEndOfPass Error",Err);
				}
			} // if (IsIntermediatePass())		
			else if ( ! IsIntermediatePass())
			{
			   CheckForEndOfMovementSequence(AXIS_X);
			}
		}
		if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
			if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
		{
			CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck.: Slice%d.", m_Layer->GetSliceNumber());
			throw EMachineSequencer("CheckEndOfLayer Error",Err);
		}
    } // try{}
    catch(EQException& E)
    {
		return E.GetErrorCode();
    }
	catch(...)
	{
		return Q_NO_ERROR; // todo: send somthing like: Q_UNEXPECTED_ERROR;
	}
	 
    return Q_NO_ERROR;
}

void CMachineSequencer::PrintPass()
{
    TQErrCode Err;

// OHDB handle that should be performed before each pass
    if ((Err = PrepareOHDBPassConfig(m_Layer,m_LayerParms,m_PassInLayer)) != Q_NO_ERROR)
        throw EMachineSequencer("PrepareOHDBPassConfig Error",Err);

/////////////////////////////////////////////////////////////////////
// Step I/IV FW Pass: Go to X stop print position - print forward  //
/////////////////////////////////////////////////////////////////////

// --- X Axis --- //
    GoToAbsolutePositionSequence(AXIS_X,m_XStopPrintPosition);

		
// Step Move back DoublePassSpace (two pixels for four heads printing). 
    CalcRelativeYLocation(m_SliceNumModifier - m_SequencerBlock.DoublePassSpace);
    CalcYRightSideCorrection();
    YAxisBacklashSequence();

		
// --- Z Axis --- //
// Smart Roller vvvvvvvvvv
    bool IsAccumulatingMaterial = IsCurrSliceAccumulating();

    if (IsAccumulatingMaterial)
    {
        CQLog::Write(LOG_TAG_PRINT,"Smart Roller: Not removing material in this slice");
        MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_SequencerBlock.KeepZLocation);
    }
    else
    {
        CalcRelativeZLocation(m_SequencerBlock.CurrentHalfLayerHeightInUM);
        MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_ZLocation);
    }
// Smart Roller ^^^^^^^^^^
	CheckForEndOfMovementSequence(AXIS_Z);


// --- Y Axis --- //
	MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_YLocation);
    CheckForEndOfMovementSequence(AXIS_X);
    CheckForEndOfMovementSequence(AXIS_Y);

///////////////////////////////////////////////////
// Step II/IV RW Pass: Print in backward moving  //
///////////////////////////////////////////////////

// --- X Axis --- //
    GoWaitAbsolutePositionSequence(AXIS_X,m_XStartPrintPosition);
    CheckEndOfPassAndNotifyError();
    ZAxisBacklashSequence();


// Advance position = two pixels + Delta interlace
    CalcRelativeYLocation(static_cast<float>(m_InterlacePixels) + m_SliceNumModifier);
    YAxisBacklashSequence();

// --- Y Axis --- //
	GoToAbsolutePositionSequence(AXIS_Y,m_YLocation);

    if (m_SequencerBlock.ZBacklash)
		CheckForEndOfMovementSequence(AXIS_Z);

// --- Z Axis --- //
	GoToAbsolutePositionSequence(AXIS_Z,m_SequencerBlock.KeepZLocation);
	CheckForEndOfMovementSequence(AXIS_Z);
	CheckForEndOfMovementSequence(AXIS_Y);

// OHDB handle that should be performed before each pass
    if ((Err = PrepareOHDBPassConfig(m_Layer,m_LayerParms,m_PassInLayer,1)) != Q_NO_ERROR)
        throw EMachineSequencer("PrepareOHDBPassConfig Error",Err);

//////////////////////////////////////////////////////////////////////
// Step III/IV FW Pass: Go to X stop print position - print forward //
//////////////////////////////////////////////////////////////////////

// --- X Axis --- //
	GoToAbsolutePositionSequence(AXIS_X,m_XStopPrintPosition);

// Step Move back two pixels. The following command is to move FW half nozzle position with backslash
    CalcRelativeYLocation(static_cast<float>(m_InterlacePixels) - m_SequencerBlock.DoublePassSpace + m_SliceNumModifier);
    CalcYRightSideCorrection();
    YAxisBacklashSequence();

// --- Z Axis --- //
// Smart Roller vvvvvvvvvv
    if (IsAccumulatingMaterial)
    {
        MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_SequencerBlock.KeepZLocation);
    }
    else
    {
        CalcRelativeZLocation(m_SequencerBlock.CurrentLayerHeightInUM);
        MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_ZLocation);
    }
// Smart Roller ^^^^^^^^^^
	CheckForEndOfMovementSequence(AXIS_Z);

// --- Y Axis --- //
	MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_YLocation);
	CheckForEndOfMovementSequence(AXIS_Y);
	CheckForEndOfMovementSequence(AXIS_X);

////////////////////////////////////////////////////
// Step IV/IV RW Pass:// Print in backward moving //
////////////////////////////////////////////////////

// --- X Axis --- //
	GoToAbsolutePositionSequence(AXIS_X,m_XStartPrintPosition);
    m_XDuringMovement=true;
}


void CMachineSequencer::PrepareForNextPassSequence()
{
    if(m_Motors->GetAxisLocation(AXIS_Z) != m_SequencerBlock.KeepZLocation)
    {
        if(m_Motors->GetAbsoluteBacklashLocation(m_YLocation,m_BacklashPosition))
        {
			MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_BacklashPosition);
			CheckForEndOfMovementSequence(AXIS_Y);
        }
        if (m_SequencerBlock.ZBacklash)
        {
            m_ZLocation = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
			MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_ZLocation);
			CheckForEndOfMovementSequence(AXIS_Z);
        }	
		MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_YLocation);
		CheckForEndOfMovementSequence(AXIS_Y);
		MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_SequencerBlock.KeepZLocation);				
    }
    else
    {
        if(m_Motors->GetAbsoluteBacklashLocation(m_YLocation,m_BacklashPosition))
        {
			MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_BacklashPosition);
			CheckForEndOfMovementSequence(AXIS_Y);
        }
		MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_YLocation);			
    }

    if(m_XDuringMovement)
    {
		CheckForEndOfMovementSequence(AXIS_X);
        m_XDuringMovement=false;
    }
	CheckForEndOfMovementSequence(AXIS_Z);
}

void CMachineSequencer::InitLayerPrintingSequence()
{
// Note1: all the Y backlash correction is performed in increase movement
//        and for axis Z in decrease movement.
    m_ZLocation=0;
    m_YLocation=0;
    m_BacklashPosition=0;
    m_XDuringMovement=false;

// In case the current mode is 4 heads 300 dpi, every odd slice add two pixels
// to the y position, to cause same Y positions to be printed in oposite direction on each slice
    m_SliceNumModifier = 0;

    TQErrCode Err;

    if (m_ParamsMgr->GenerateExcelAnalyzer)
        CExcelSequenceAnalyzer::Init("SequencerAnalyzer.csv", 10, 10);
    else
        CExcelSequenceAnalyzerDummy::Init();

    m_csv = CExcelSequenceAnalyzer::Instance();

    QString String = (m_ParamsMgr->PrintSequenceVersion == FOUR_HEADS_300_X_2_SEQUENCER) ?
        "Four Heads 300 x 2 Alternate Sequencer" : "Four Heads Alternate Sequencer";

    CQLog::Write(LOG_TAG_PRINT," %s slice:%d  ZPosition:%d",String.c_str(),
        m_Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

// Prepare X PRINT start position and Stop Position
// -------------------------------------------------
    m_LayerParms = m_Layer->GetDPCPCParams();
    m_SequencerBlock.BufferLayerPrintPtr = m_Layer->GetBuffer();

    if(m_Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN)
    {
        m_PassDirection = 1;
//The interlace can be Positive or Negatice according to print scan direction.
        m_InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_RW_DIRECTION;
        m_PassInLayer = 0;
    }
    else
    {
        m_PassDirection = -1;
        m_PassInLayer = m_Layer->GetNoOfPasses()-1;
//The interlace can be Positive or Negatice according to print scan direction.
// correction: use only negative m_InterlacePixels for FW and BW
// m_InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_RW_DIRECTION;
        m_InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_RW_DIRECTION;
    }

    m_XStartPrintPosition = CalculateXStartPosition(m_LayerParms);
    m_XStopPrintPosition  = CalculateXStopPosition(m_LayerParms, m_Layer);

// Test Negative slot
// ------------------
    if(m_ParamsMgr->NegativeSlotTest)
        if(m_Layer->GetTotalNumCounter() != 0)
            if(!(m_Layer->GetTotalNumCounter() % m_ParamsMgr->NegativeSlotCounter))
                m_SequencerBlock.StartYPosition += m_ParamsMgr->NegativeSlotOffSet;

// Note: for slice num modifiers: {0, +0.5, 0, -0.5} we should use: (in CLayerProcess::FillLayerBidirection300_4Heads)
//   if(m_TotalLayersCounter % 4 == 3 && m_ParamsMgr->Shift_600DPI) // on each 4th layer
//     CurrentY -= 1;
//  for Slice num modifiers: {0, -0.5, 0, +0.5} we should use: CurrentY += 1.

    if (m_ParamsMgr->PrintSequenceVersion == FOUR_HEADS_300_X_2_SEQUENCER &&
        m_ParamsMgr->Shift_600DPI)
    {
        switch (m_Layer->GetSliceNumber() % 4)
        {
            case 0:
                m_SliceNumModifier = 0; break;
            case 1:
                m_SliceNumModifier = +0.5; break;
            case 2:
                m_SliceNumModifier = 0; break;
            case 3:
                if (m_ParamsMgr->Shift_600DPI_Cycle_4)
                    m_SliceNumModifier = -0.5;
                else
                    m_SliceNumModifier = +0.5;
                break;
        }
    }

    CalcRelativeYLocation(m_SliceNumModifier);

    CalcIsPassNotEmpty();
    
    if(IsPassNotEmpty())
    {
		YAxisBacklashSequence();
		GoToAbsolutePositionSequence(AXIS_Y,m_YLocation);
    }

    if ((Err = PrepareTrayToPrintCurrentLayer(m_Layer)) != Q_NO_ERROR)
        throw EMachineSequencer("PrepareTrayToPrintCurrentLayer Error",Err);
		
    m_SequencerBlock.CurrentHalfLayerHeightInUM = CalculateHalfLayerHeight();

    KeepScanningCookingData(m_YLocation,
        m_XStartPrintPosition,
        m_XStopPrintPosition,
        m_Layer->GetNoOfPasses(),
        m_Layer->GetPrintScanDirection());

    if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_PRINT,"Printing:Check OCB error=%d",static_cast<int>(Err));
        throw EMachineSequencer("CheckStatusDuringPrinting Error",Err);
    }
    if ((Err = PrepareOHDBLayerConfig(m_Layer,m_LayerParms)) != Q_NO_ERROR)
    {
        CQLog::Write(LOG_TAG_PRINT,"Printing:Error in layerConfig-Error=:%d",
            static_cast<int>(Err));
        throw EMachineSequencer("PrepareOHDBLayerConfig Error",Err);
    }
    if(IsPassNotEmpty())
		CheckForEndOfMovementSequence(AXIS_Y);
}

void CMachineSequencer::CalcRelativeYLocation(float move_size)
{
  m_YLocation = m_SequencerBlock.StartYPosition +
				QSimpleRound (m_ParamsMgr->GetYStepsPerPixel() *
				static_cast<float> (m_Layer->GetYPosition() +
				CONFIG_GetHeadsPrintWidht() * m_PassInLayer + move_size));
}

void CMachineSequencer::CalcRelativeZLocation(float move_size)
{
  m_ZLocation = m_SequencerBlock.KeepZLocation +
                CONFIG_ConvertZumToStep(move_size -
                m_ParamsMgr->Z_MoveToStartNewLayer_um);
}


// Smart Roller vvvvvvvvv
bool  CMachineSequencer::IsCurrSliceAccumulating() const
{
  /*  return ((m_Layer)                                                                                       &&
			((m_Layer->GetSliceNumber() % m_ParamsMgr->SmartRollerTotalSlicesPerCycle)                    <
			 (m_ParamsMgr->SmartRollerTotalSlicesPerCycle - m_ParamsMgr->SmartRollerRemovalSlicesPerCycle) )  );*/
			 return false;
}
// Smart Roller ^^^^^^^^^


void CMachineSequencer::YAxisBacklashSequence()
{
    if(m_Motors->GetAbsoluteBacklashLocation(m_YLocation,m_BacklashPosition))
    {
		MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,m_BacklashPosition);
		CheckForEndOfMovementSequence(AXIS_Y);
    }
}

void CMachineSequencer::ZAxisBacklashSequence()
{
    m_ZLocation = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
    if (m_SequencerBlock.ZBacklash)
    {
		GoToAbsolutePositionSequence(AXIS_Z,m_ZLocation);
    }
}

bool CMachineSequencer::IsIntermediatePass()
{
    if (m_PassInLayer >= 0)
    {
       return ((unsigned)m_PassInLayer < m_Layer->GetNoOfPasses());
    }
    return false;
}
void CMachineSequencer::IncrementPass()
{
  m_PassInLayer += m_PassDirection;
}
bool CMachineSequencer::IsPassNotEmpty()
{
  return m_PassIsNotEmpty;
}
void CMachineSequencer::CalcYRightSideCorrection()
{
  m_YLocation += m_ParamsMgr->Y_RightSideMovementCorrectionStep;
}

// MotorSequences wrappers that throws exceptions
void CMachineSequencer::MoveABAbsPositionSequencer(TMotorAxis Axis_A, TMotorAxis Axis_B, long Destination)
{
    if (AXIS_Y == Axis_A)
		m_csv->SetValueNext(SourceSequencer, m_YLocation);
	
    TQErrCode Err;
    if ((Err = m_Motors->MoveABAbsPositionSequencer(Axis_A, Axis_B, Destination)) != Q_NO_ERROR)
        throw EMachineSequencer("MoveABAbsPositionSequencer Error",Err);
}
void CMachineSequencer::GoToAbsolutePositionSequence(TMotorAxis Axis,long Destination)
{
    if (AXIS_Y == Axis)
		m_csv->SetValueNext(SourceSequencer, m_YLocation);

    TQErrCode Err;
    if ((Err = m_Motors->GoToAbsolutePositionSequence(Axis, Destination)) != Q_NO_ERROR)
        throw EMachineSequencer("GoToAbsolutePositionSequence Error",Err);
}
void CMachineSequencer::CheckForEndOfMovementSequence(TMotorAxis Axis)
{
    TQErrCode Err;
    if ((Err = m_Motors->CheckForEndOfMovementSequence(Axis)) != Q_NO_ERROR)
        throw EMachineSequencer("CheckForEndOfMovementSequence Error",Err);
}
void CMachineSequencer::GoWaitAbsolutePositionSequence(TMotorAxis Axis,long Destination)
{
    if (AXIS_Y == Axis)
		m_csv->SetValueNext(SourceSequencer, m_YLocation);

    TQErrCode Err;
    if ((Err = m_Motors->GoWaitAbsolutePositionSequence(Axis, Destination)) != Q_NO_ERROR)
        throw EMachineSequencer("GoWaitAbsolutePositionSequence Error",Err);
}
