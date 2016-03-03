/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Unidirection sequencer.                                  *
 * Module Description: Implementation of the specific Q2RT          *
 *                     unidirection sequences.                      *
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

// Print a single layer - Eden 260
TQErrCode CMachineSequencer::BacklashFastPrintSequencerVersion(CLayer *Layer)
{
// Note1: all the Y backlash correction is performed in increase movement
//        and for axis Z in decrease movement.

//        Y movement depend of its real dpi.
//        Backlash is performed if movement is small and not according to
//        the direction of last movement
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

   CQLog::Write(LOG_TAG_PRINT,"%s: Backlash-Fast Print version: Printing slice: %d  ZPosition: %d",
                  m_MachineType.c_str(), Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare Y location
   // ------------------
   PassInLayer = 0;

   // Prepare X PRINT start position and Stop Position
   // -------------------------------------------------
   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();

   //The interlace can be Positive or Negatice according to print scan direction.
   long InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * 
      (Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN ? (INTERLACE_FW_DIRECTION) :
                                                               (INTERLACE_RW_DIRECTION));

   XStartPrintPosition = CalculateXStartPosition(LayerParms);
   XStopPrintPosition = CalculateXStopPosition(LayerParms, Layer);

   // Test Negative slot
   // ------------------
   if(m_ParamsMgr->NegativeSlotTest)
      if(Layer->GetTotalNumCounter() != 0)
         if(!(Layer->GetTotalNumCounter() % m_ParamsMgr->NegativeSlotCounter))
            m_SequencerBlock.StartYPosition += m_ParamsMgr->NegativeSlotOffSet;

   YLocation = m_SequencerBlock.StartYPosition +
        QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                                    static_cast<float>(Layer->GetYPosition()));

   if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
      return Err;
   if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
      return Err;
   KeepScanningCookingData(YLocation,
                           XStartPrintPosition,
                           XStopPrintPosition,
                           Layer->GetNoOfPasses(),
                           FORWARD_PRINT_SCAN);

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

   //Wait for end of movement later

   //The layer finish prepare tray to next layer.
   if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
      return Err;

   if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
         return Err;

   PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

   // Perform the movement until the end of the layer
   while(PassInLayer < Layer->GetNoOfPasses())
      {
      // Calculate next Axis Y  move location
      YLocation = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() *
                          static_cast<float>(InterlacePixels)) + YLocation;

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

         // Step Prepare the axis Z to Roler High:
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

      // Check if we have another pass in the same layer
      if(++PassInLayer < Layer->GetNoOfPasses())
         {
         PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

         // Avance position = A head - half pixel
         YLocation = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                      static_cast<float>(CONFIG_GetHeadsPrintWidht()-InterlacePixels)) +
                      YLocation;

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
            }

         if(XDuringMovement)
            {
            XDuringMovement=false;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
               return Err;
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
            }

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


// Print a single layer
TQErrCode CMachineSequencer::Eden330FastPrintSequencer(CLayer *Layer)
{
// Note1: all the Y backlash correction is performed in increase movement
//        and for axis Z in decrease movement.

//        Y movement depend of its real dpi.

// Constants
// ---------
// Local variables
// ---------------
   int PassInLayer;
   long XStartPrintPosition; //In step
   long XStopPrintPosition;  //In step
   TQErrCode Err;
   long Location,YLocation;
   bool PassIsNotEmpty;

   CQLog::Write(LOG_TAG_PRINT,"%s: Backlash-Fast Print version: Printing slice: %d  ZPosition: %d",
                  m_MachineType.c_str(), Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare Y location
   // ------------------
   PassInLayer = 0;

   // Prepare X PRINT start position and Stop Position
   // -------------------------------------------------
   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();

   //The interlace can be Positive or Negatice according to print scan direction.
   long InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * 
      (Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN ? (INTERLACE_FW_DIRECTION) :
                                                               (INTERLACE_RW_DIRECTION));

   XStartPrintPosition = CalculateXStartPosition(LayerParms);
   XStopPrintPosition = CalculateXStopPosition(LayerParms, Layer);

   // Test Negative slot
   // ------------------
   if(m_ParamsMgr->NegativeSlotTest)
      if(Layer->GetTotalNumCounter() != 0)
         if(!(Layer->GetTotalNumCounter() % m_ParamsMgr->NegativeSlotCounter))
            m_SequencerBlock.StartYPosition += m_ParamsMgr->NegativeSlotOffSet;

   if (Layer->GetTotalNumCounter() == 0)  // the first layer
      {
      // Y working in Y- direction there is no need to backlash correction
      // (last Y move was Y home position)
      YLocation = m_SequencerBlock.StartYPosition +
         QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*static_cast<float>(Layer->GetYPosition()));

      if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
         return Err;
      if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
         return Err;

      KeepScanningCookingData(YLocation,
                              XStartPrintPosition,
                              XStopPrintPosition,
                              Layer->GetNoOfPasses(),
                              FORWARD_PRINT_SCAN);

      //Wait for end of movement later

      //The layer finish prepare tray to next layer.
      if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
         return Err;
      }
   else
      {
      // Now we need to perform backlash
      YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition()));
      int BacklashPosition=0;
      if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition)) //backlash is needed
         {
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,BacklashPosition)) != Q_NO_ERROR)
            return Err;
         //The layer finish prepare tray to next layer.
         if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
            return Err;
         }
      else
         {
         //The layer finish prepare tray to next layer.
         if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
            return Err;
         }

      if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
         return Err;
      //Wait for end of movement later
                          
      KeepScanningCookingData(YLocation,
                              XStartPrintPosition,
                              XStopPrintPosition,
                              Layer->GetNoOfPasses(),
                              FORWARD_PRINT_SCAN);
      }

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

   if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
         return Err;

   PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

   // Perform the movement until the end of the layer
   while(PassInLayer < Layer->GetNoOfPasses())
      {
      // Calculate next Axis Y  move location
      YLocation = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() *
                          static_cast<float>(InterlacePixels)) + YLocation;

      if(PassIsNotEmpty)
         {
         // OHDB handle that should be performed before each pass
         if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
            return Err;

         // Step X Fw Pass: Go to X stop print position - print forward
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
            return Err;

      // Step Move Half Pixel
      // The following command is to move FW half nozzle position
      // ---------------------------------------------------------
         if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
            return Err;

         // Step Prepare the axis Z to Roler High:
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
         if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
            return Err;
         }  // End of Y mask check
      else
         CQLog::Write(LOG_TAG_PRINT,"Pass Number = %d is empty",(PassInLayer));

      // Check if we have another pass in the same layer
      if(++PassInLayer < Layer->GetNoOfPasses())
         {
         PassIsNotEmpty = CheckIfPassMaskIsNotEmpty (Layer,PassInLayer);

         // Avance position = A head - half pixel
         YLocation = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                      static_cast<float>(CONFIG_GetHeadsPrintWidht()-InterlacePixels)) +
                      YLocation;

         if(PassIsNotEmpty)
            {
            if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
               return Err;

            // This is the end of the first FW RW pass move axis Z to next pass.
            // We need to move down the space of one layer but we need to move more
            // because Mechanism limitation.
            // Because YMask feature,  may be we did not need to move
            if(m_Motors->GetAxisLocation(AXIS_Z) != m_SequencerBlock.KeepZLocation)
               {
               Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
               if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
                  return Err;

               if((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,
                                         m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
                  return Err;
               if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
                  return Err;
               }
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }

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

         } // end of // if there is another pass
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



