/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Alternate sequencer.                                     *
 * Module Description: Implementation of the specific Q2RT          *
 *                     Alternate sequences.                         *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 7/07/2002                                            *
 ********************************************************************/
 
#include "MachineSequencer.h"
#include "Q2RTErrors.h"
#include "GlobalDefs.h"
#include "Power.h"
#include "Purge.h"
#include "Motor.h"
#include "LayerProcess.h"
#include "Layer.h"
#include "configuration.h"
#include "Q2RTApplication.h"
#include "PrintControl.h"
#include "FIFOPci.h"
#include "FrontEnd.h"
#include "AppLogFile.h"


// Constants


//Timeouts

// Print a single layer
TQErrCode CMachineSequencer::Eden260AlternatePrintSequencer(CLayer *Layer)
{
// Note: Alternate sequence
//
// Constants
// ---------
// Local variables
// ---------------
   int PassInLayer;
   long XStartPrintPosition; //In step
   long XStopPrintPosition;  //In step
   TQErrCode Err;
   long Location,YLocation,BacklashPosition=0;
   bool XDuringMovement=false;
   bool PassIsNotEmpty;
   long InterlacePixels;
   int PassDirection;

   CQLog::Write(LOG_TAG_PRINT,"%s: Alternate - Printing slice: %d  ZPosition: %d",
                m_MachineType.c_str(), Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare X PRINT start position and Stop Position
   // -------------------------------------------------
   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();

   // Y Acis Print direction. Calculate PassInLayer: if FW, set 0. If BW, Set to last pass number.
   // According to PassInLayer, YLocation is calculated.
   if(Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN)
      {
      PassDirection = 1;
      //The interlace can be Positive or Negatice according to print scan direction.
      InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_FW_DIRECTION;
      PassInLayer = 0;
      }
   else
      {
      PassDirection = -1;
      PassInLayer = Layer->GetNoOfPasses()-1;
      //The interlace can be Positive or Negatice according to print scan direction.
      InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_RW_DIRECTION;
      }

   XStartPrintPosition = CalculateXStartPosition(LayerParms);
   XStopPrintPosition = CalculateXStopPosition(LayerParms, Layer);

   // Test Negative slot
   // ------------------
   if(m_ParamsMgr->NegativeSlotTest)
      if(Layer->GetTotalNumCounter() != 0)
         if(!(Layer->GetTotalNumCounter() % m_ParamsMgr->NegativeSlotCounter))
            m_SequencerBlock.StartYPosition += m_ParamsMgr->NegativeSlotOffSet;

   // In case the current mode is 8 heads 300 dpi X 2, every odd slice add a half pixel
   // to the y position, to create simultaed double resultion printing.
   float EvenOddSlice = 0;
   if (m_ParamsMgr->Shift_600DPI)
     EvenOddSlice = Layer->GetSliceNumber() % 2 / 2.0;

   YLocation = m_SequencerBlock.StartYPosition +
         QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
            static_cast<float>(Layer->GetYPosition() +
                               CONFIG_GetHeadsPrintWidht() * PassInLayer + EvenOddSlice));

   if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
      return Err;
   if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
      return Err;
   KeepScanningCookingData(YLocation,
                           XStartPrintPosition,
                           XStopPrintPosition,
                           Layer->GetNoOfPasses(),
                           Layer->GetPrintScanDirection());

   if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Printing: Check OCB error=%d",static_cast<int>(Err));
      return Err;
      }

   if ((Err = PrepareOHDBLayerConfig(Layer,LayerParms)) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Printing:Error in layerConfig-Error=:%d",
                                                    static_cast<int>(Err));
      return Err;
      }

   //Wait for Y end of movement later

   //The layer finish prepare tray to next layer.
   if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
      return Err;

   if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
         return Err;

   PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

   // Perform the movement until the end of the layer
   while(PassInLayer < Layer->GetNoOfPasses() && PassInLayer >= 0)
      {
      // Calculate next Axis Y  move location
      YLocation = m_SequencerBlock.StartYPosition +
        QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
          (static_cast<float>(Layer->GetYPosition() +
              CONFIG_GetHeadsPrintWidht()*PassInLayer + InterlacePixels + EvenOddSlice)));

      if(PassIsNotEmpty)
         {
         // OHDB handle that should be performed before each pass
         if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
            return Err;

         // Step X Fw Pass: Go to X stop print position - print forward
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Step Move Half Pixel
         // The following command is to move FW half nozzle position
         // ---------------------------------------------------------
         //Verify if backlash is needed - (we want to use MoveAB to save time
         if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
            {
            if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
               return Err;
            }
         else
            {
            if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
               return Err;
            }

         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
           return Err;

         // Move axis Z to roler Height
         //------------------------------------------
         Location = m_SequencerBlock.KeepZLocation +
                    CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                                            m_ParamsMgr->Z_MoveToStartNewLayer_um);

         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;

         // Step X Rw Pass:// Print in backward moving
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
            return Err;
         XDuringMovement=true;
         }
      else
         CQLog::Write(LOG_TAG_PRINT,"Pass Number = %d is empty",PassInLayer);

      //Start new pass
      PassInLayer+=PassDirection;

      // Check if we have another pass in the same layer
      if(PassInLayer < Layer->GetNoOfPasses() && PassInLayer >= 0)
         {
         PassIsNotEmpty = CheckIfPassMaskIsNotEmpty(Layer,PassInLayer);

         // Avance position according to direction = A head*Direction - half pixel
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer + EvenOddSlice));

         if(PassIsNotEmpty)
            {
            //Verify if backlash is needed - (we want to use MoveAB to save time
            if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
               {
               if((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
                  return Err;
               if((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
                  return Err;
               if((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
                  return Err;
               }
            else
               {
               if((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
                  return Err;
               }
               
            if(XDuringMovement)
               {
               XDuringMovement=false;
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
                  return Err;
               }
            }


         if(PassIsNotEmpty)
            {
            // This is the end of the first FW RW pass move axis Z to next pass.
            // We need to move down the space of one layer but we need to move more
            // because Mechanism limitation.
            // Because YMask feature,  may be we did not need to move
            if(m_Motors->GetAxisLocation(AXIS_Z) != m_SequencerBlock.KeepZLocation)
               {
               Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
               if((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
                  return Err;
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
                  return Err;
               if((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
                  return Err;
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
                  return Err;
               }

            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;

         Err = CheckEndOfPass();

         // In a case of FIFO not empty error, break current layer print
         if(Err == Q2RT_FIFO_NOT_EMPTY_ERROR)
           break;

         // OHDB handle that should be performed after each pass
         if (Err != Q_NO_ERROR)
            {
            CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice %d.",
                      Layer->GetSliceNumber());
            return Err;
            }
            }
         } // end of // if there is another pass
      else
         {
         if(XDuringMovement)
            {
            XDuringMovement =false;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
               return Err;
            }
         }
      }  // PassInLayer < Layer->GetNoOfPasses()

   if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
     if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
        {
        CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck.: Slice%d.",
                        Layer->GetSliceNumber());
        return Err;
        }

   return Q_NO_ERROR;
}

TQErrCode CMachineSequencer::Double300DpiAlternate600PrintSequencer(CLayer *Layer)
{
   int PassInLayer;
   long XStartPrintPosition; //In step
   long XStopPrintPosition;  //In step
   TQErrCode Err;
   long Location;
   long YLocation;
   long BacklashPosition=0;
   bool XDuringMovement=false;
   bool PassIsNotEmpty;
   long InterlacePixels;
   int PassDirection;

   CQLog::Write(LOG_TAG_PRINT,"Double-300Dpi Alternate Sequencer:%d  ZPosition:%d",
                  Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare X PRINT start position and Stop Position
   // -------------------------------------------------
   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();

   if(Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN)
      {
      PassDirection = 1;
      //The interlace can be Positive or Negatice according to print scan direction.
      InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_FW_DIRECTION;
      PassInLayer = 0;
      }
   else
      {
      PassDirection = -1;
      PassInLayer = Layer->GetNoOfPasses()-1;
      //The interlace can be Positive or Negatice according to print scan direction.
      InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_RW_DIRECTION;
      }

   XStartPrintPosition = CalculateXStartPosition(LayerParms);
   XStopPrintPosition = CalculateXStopPosition(LayerParms, Layer);

   // Test Negative slot
   // ------------------
   if(m_ParamsMgr->NegativeSlotTest)
      if(Layer->GetTotalNumCounter() != 0)
         if(!(Layer->GetTotalNumCounter() % m_ParamsMgr->NegativeSlotCounter))
            m_SequencerBlock.StartYPosition += m_ParamsMgr->NegativeSlotOffSet;

   PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

   YLocation = m_SequencerBlock.StartYPosition +
         QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
            static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer));

   if(PassIsNotEmpty)
   {
      if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
         return Err;
      if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
         return Err;
   }

   //The layer finish prepare tray to next layer.
   if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
      return Err;
   m_SequencerBlock.CurrentHalfLayerHeightInUM = CalculateHalfLayerHeight();
   
   //Wait for end of movement later
   KeepScanningCookingData(YLocation,
                           XStartPrintPosition,
                           XStopPrintPosition,
                           Layer->GetNoOfPasses(),
                           Layer->GetPrintScanDirection());

   if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Printing:Check OCB error=%d",static_cast<int>(Err));
      return Err;
      }

   if ((Err = PrepareOHDBLayerConfig(Layer,LayerParms)) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Printing:Error in layerConfig-Error=:%d",
                                                    static_cast<int>(Err));
      return Err;
      }

   if(PassIsNotEmpty)
      if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
         return Err;

   // Perform the movement until the end of the layer
   while(PassInLayer < Layer->GetNoOfPasses() && PassInLayer >= 0)
      {
      if(PassIsNotEmpty)
         {
         // OHDB handle that should be performed before each pass
         if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
            return Err;

         // Step X Fw Pass: Go to X stop print position - print forward
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Step Move first interlace
         // -----------------------------------------------------------------------
         // Calculate next Axis Y  move location
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               (static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht()*PassInLayer + InterlacePixels)));

         if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
            {
            if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }

         // Step Prepare the axis Z to half layer size
         Location = m_SequencerBlock.KeepZLocation +
                    CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                                            m_ParamsMgr->Z_MoveToStartNewLayer_um);

         if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,Location)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
               return Err;

         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;

         // Step X Rw Pass:// Print in backward moving
         if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
            return Err;

         // OHDB handle that should be performed after each pass
         if ((Err = CheckEndOfPass()) != Q_NO_ERROR)
            {
             // In a case of FIFO not empty error, break current layer print
             if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
             {
               CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice%d.",
                         Layer->GetSliceNumber());
               return Err;
             }
			 else
			    break;
            }

         //Now complete the pass (completion to 600 dpi of this pass)
         //Return Z to start Z position with backlash.
         Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
            return Err;


         // Avance position = Y - m_SequencerBlock.DoublePassSpace(0.5) 300dpi (1 pixel in 600)
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer - m_SequencerBlock.DoublePassSpace));

         if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
            {
            if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
              return Err;
            }
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;

         // OHDB handle that should be performed before each pass
         if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
            return Err;

         // Step X Fw Pass: Go to X stop print position - print forward
         if((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Step Move back two pixels
         // The following command is to move FW half nozzle position with backslash
         // -----------------------------------------------------------------------
         // Calculate next Axis Y  move location
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               (static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht()*PassInLayer + InterlacePixels - m_SequencerBlock.DoublePassSpace)));

         if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
            {
            if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }

         // Step Prepare the axis Z to Roler High:
         //---------------------------------------
         Location = m_SequencerBlock.KeepZLocation +
                    CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                                            m_ParamsMgr->Z_MoveToStartNewLayer_um);

         if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,Location)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
            return Err;

         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;

         // Step X Rw Pass:// Print in backward moving
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
            return Err;
         XDuringMovement=true;
         }  // End of Y mask check
      else
         {
         CQLog::Write(LOG_TAG_PRINT,"Pass Number = %d is empty",PassInLayer);
         }

      //Start new pass
      PassInLayer+=PassDirection;

      // Check if we have another pass in the same layer
      if(PassInLayer < Layer->GetNoOfPasses() && PassInLayer >= 0)
         {
         PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

         // Avance position according to direction = A head*Direction - half pixel
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer));

         if(PassIsNotEmpty)
            {
            // This is the end of the first FW RW pass move axis Z to next pass.
            // We need to move down the space of one layer but we need to move more
            // because Mechanism limitation.
            // Because YMask feature,  may be we did not need to move
            if(m_Motors->GetAxisLocation(AXIS_Z) != m_SequencerBlock.KeepZLocation)
               {
               if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
                  {
                  Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;

                  if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
                      return Err;
                  if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
                     return Err;
                  if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,Location)) != Q_NO_ERROR)
                      return Err;
                  }
               else
                  {
                  Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
                  if((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,Location)) != Q_NO_ERROR)
                     return Err;

                  }

               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
                  return Err;

               if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Z,AXIS_X,m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
                   return Err;
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
                  return Err;
               if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
                   return Err;
               }
            else
               {
               if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
                  {
                  if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
                     return Err;
                  if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
                     return Err;
                  }

               if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,YLocation)) != Q_NO_ERROR)
                  return Err;
               }

            if(XDuringMovement)
               {
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
                  return Err;
               XDuringMovement=false;
               }
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }
         else
            {
            if(XDuringMovement)
               {
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
                  return Err;
               XDuringMovement=false;
               }
            }

         // OHDB handle that should be performed after each pass
         if ((Err = CheckEndOfPass()) != Q_NO_ERROR)
            {
             // In a case of FIFO not empty error, break current layer print
             if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
             {
               CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice%d.",
                         Layer->GetSliceNumber());
               return Err;
             }
			 else
			    break;
            return Err;
            }

         } // end of // if there is another pass
      else
         {
         if(XDuringMovement)
            {
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
               return Err;
            XDuringMovement=false;
            }
         }

      }  // PassInLayer < Layer->GetNoOfPasses()

   if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck.: Slice%d.",
                      Layer->GetSliceNumber());
      return Err;
      }

   return Q_NO_ERROR;
}




