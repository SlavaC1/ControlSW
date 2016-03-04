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
 * Start date: 28/04/2002                                           *
 * Last upate: 29/04/2002                                           *
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


// Forward sequencer
TQErrCode CMachineSequencer::UniDirection150dpiPrintSequencer(CLayer *Layer)
{
// Note1: all the Y backlash correction is performed in increase movement
//        and for axis Z in decrease movement.
//        Y movement depend of its real dpi. 
//
// Constants
// ---------

// Local variables
// ---------------
   int PassInLayer;
   long XStartPrintPosition; //In step
   long XStopPrintPosition;  //In step
   TQErrCode Err;
   long Location;
   
// Code
// ----
   CQLog::Write(LOG_TAG_PRINT,"Unidirection150DPI: Starting print of slice:%d ZPosition:%d",
                Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare Y location
   // ------------------
   PassInLayer = 0;

   // Prepare X PRINT start position and Stop Position
   // -------------------------------------------------
   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();

   if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Unidirection150: Check OCB error=%d",
                                                    static_cast<int>(Err));
      return Err;
      }
   
   if ((Err = PrepareOHDBLayerConfig(Layer,LayerParms)) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Unidirection150: Error in layerConfig-Error=:%d",
                                                    static_cast<int>(Err));
      return Err;
      }

   XStartPrintPosition = CalculateXStartPosition(LayerParms);
   XStopPrintPosition = CalculateXStopPosition(LayerParms, Layer);
   
   // Step X Start Position -  Go to X start print position
   // There is no need to move to XstartPosition because we are already before the
   // HW home and there is no meaning to move from Xhome to XstartPosition
   // and them the XStopPosition in the first pass
   
   if (Layer->GetTotalNumCounter() == 0)  // the first layer
      {
      if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
         return Err;

      // Y working in Y- direction there is no need to backlash correction
      // (last Y move was Y home position)
      Location = m_SequencerBlock.StartYPosition +
         QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*static_cast<float>(Layer->GetYPosition()));
      
      KeepScanningCookingData(Location, XStartPrintPosition,
                              XStopPrintPosition, Layer->GetNoOfPasses(),FORWARD_PRINT_SCAN);

      if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
         return Err;
      }
   else
      {
      //Prepare Z axis layer height.
      if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
         return Err;

      // Now we need to perform backlash
      Location = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition()));

      if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
         return Err;

      KeepScanningCookingData(Location,
                              XStartPrintPosition,
                              XStopPrintPosition,
                              Layer->GetNoOfPasses(),
                              FORWARD_PRINT_SCAN);
      }


   // Perform the movement until the end of the layer
   while(PassInLayer < Layer->GetNoOfPasses())
      {
      // OHDB handle that should be performed before each pass
      if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
         return Err;

      //Move to Y forward pass print position if this is not the first pass in the layer
      // otherwise we already done this
      if(PassInLayer != 0)
         {
         Location = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
            static_cast<float>(CONFIG_GetHeadsPrintWidht()));

         if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Y, Location)) != Q_NO_ERROR)
            return Err;
         }

      // Step X Fw Pass: Go to X stop print position - print forward
      if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
         return Err;
      
      // Step Prepare the axis Z to Roler High:
      //------------------------------------------
      Location = CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                                         m_ParamsMgr->Z_MoveToStartNewLayer_um);

      if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Z,(Location))) != Q_NO_ERROR)
         return Err;

      // Step X Rw Pass:// backward moving      
      if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
         return Err;
      
      // Check if we have another pass in the same layer
      if(++PassInLayer < Layer->GetNoOfPasses())
         {
         // This is the end of the first FW RW pass move axis Z to next pass.
         // We need to move down the space of one layer but we need to move more
         // because Mechanism limitation.
         Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
         if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
            return Err;

         if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Z,
                                -m_SequencerBlock.ZBacklash)) != Q_NO_ERROR)
            return Err;

         // OHDB handle that should be performed after each pass
         if ((Err = CheckEndOfPass()) != Q_NO_ERROR)
            {
             if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
             {
            CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass. Slice%d.",
                      Layer->GetSliceNumber());
            return Err;
            }
			 else
			    break;
            }
         } // end of // if there is another pass
      }  // PassInLayer < Layer->GetNoOfPasses()

   if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck. Slice%d.",
                      Layer->GetSliceNumber());
      return Err;
      }

   return Q_NO_ERROR;
}


//--------------------------------------------------------------------------
TQErrCode CMachineSequencer::UniDirection300dpiPrintSequencer(CLayer *Layer)
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
   long Location;
   int NoOfPasses = 2*Layer->GetNoOfPasses();

// Code
// ---- 
   CQLog::Write(LOG_TAG_PRINT,"Unidirection300dpi: Starting print of slice:%d ZPosition:%d",
                Layer->GetSliceNumber(),(int)m_ParamsMgr->LastSliceZPosition);

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
   
   if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Unidirection300dpi: Check OCB error=%d",
                                                    static_cast<int>(Err));
      return Err;
      }
   
   if ((Err = PrepareOHDBLayerConfig(Layer,LayerParms)) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Unidirection300dpi: Error in layerConfig-Error=:%d",
                                                    static_cast<int>(Err));
      return Err;
      }

   XStartPrintPosition = CalculateXStartPosition(LayerParms);
   XStopPrintPosition = CalculateXStopPosition(LayerParms, Layer);
   
   // Step X Start Position -  Go to X start print position
   // There is no need to move to XstartPosition because we are already before the
   // HW home and there is no meaning to move from Xhome to XstartPosition
   // and them the XStopPosition in the first pass
   
   if (Layer->GetTotalNumCounter() == 0)  // the first layer
      {
      // Y working in Y- direction there is no need to backlash correction
      // (last Y move was Y home position)
      Location = m_SequencerBlock.StartYPosition +
         QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*static_cast<float>(Layer->GetYPosition()));
      
      KeepScanningCookingData(Location, XStartPrintPosition,
                              XStopPrintPosition, Layer->GetNoOfPasses(),FORWARD_PRINT_SCAN);

      if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
         return Err;
      }
   else
      {
      // Now we need to perform backlash
      Location = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition()));

      if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
         return Err;

      KeepScanningCookingData(Location + m_Motors->GetYBacklashMoveInSteps(),
                              XStartPrintPosition,
                              XStopPrintPosition,
                              Layer->GetNoOfPasses(),
                              FORWARD_PRINT_SCAN);
      }

   //Prepare Z Axis to next layer.
   if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
      return Err;

   // Perform the movement until the end of the layer
   while(PassInLayer < NoOfPasses)
      {
      // OHDB handle that should be performed before each pass
      if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
         return Err;

      //Move to Y forward pass print position if this is not the first pass in the layer
      // otherwise we already done this
      if(PassInLayer != 0)
         {
         //Verify if the next movement should be a pixel or a 
         // Head - pixel. Depend if PassInLayer is odd or even
         if((PassInLayer /2)*2 == PassInLayer)
            {
            //even - Move A Head - one pixel
            Location = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(CONFIG_GetHeadsPrintWidht()-InterlacePixels));

            if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Y, Location)) != Q_NO_ERROR)
               return Err;
            }
         else
            {
            // odd - Now is time to move one pixel but with backlash
            Location = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() *
                    static_cast<float>(InterlacePixels));
            if((Err = m_Motors->GoWaitToYRelativePositionWithBacklashSequence(Location)) != Q_NO_ERROR)
               return Err;
            }
         }

      // Step X Fw Pass: Go to X stop print position - print forward
      if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
         return Err;
      
      // Step Prepare the axis Z to Roler High:
      //------------------------------------------
      Location = CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                       m_ParamsMgr->Z_MoveToStartNewLayer_um);

      if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Z,(Location))) != Q_NO_ERROR)
         return Err;

      // Step X Rw Pass:// backward moving      
      if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
         return Err;
      
      // Check if we have another pass in the same layer
      if (++PassInLayer < NoOfPasses)
            {
            // This is the end of the first FW RW pass move axis Z to next pass.
            // We need to move down the space of one layer but we need to move more
            // because Mechanism limitation.
            Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
            if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
               return Err;
         
            if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Z,
                                       -m_SequencerBlock.ZBacklash)) != Q_NO_ERROR)
               return Err;
            
            // OHDB handle that should be performed after each pass
            if ((Err = CheckEndOfPass()) != Q_NO_ERROR)
               {
             if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
             {
               CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass. Slice%d.",
                      Layer->GetSliceNumber());
               return Err;
               }
			 else
			    break;
               }

            } // end of // if there is another pass
      }  // PassInLayer < NoOfPasses

   if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck. Slice%d.",Layer->GetSliceNumber());
      return Err;
      }

   return Q_NO_ERROR;
}


// Print a single layer
TQErrCode CMachineSequencer::Eden330Unidirection300dpiPrintSequencer(CLayer *Layer)
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
   long Location,YLocation,BacklashPosition=0;
   int NoOfPasses = 2*Layer->GetNoOfPasses();

   CQLog::Write(LOG_TAG_PRINT,"%s: Unidirection 300 dpi: Printing slice: %d  ZPosition: %d",
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

   // Perform the movement until the end of the layer
   while(PassInLayer < NoOfPasses)
      {
      // OHDB handle that should be performed before each pass
      if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
         return Err;

      //Move to Y forward pass print position if this is not the first pass in the layer
      // otherwise we already done this
      if(PassInLayer != 0)
         {
         //Verify if the next movement should be a pixel or a
         // Head - pixel. Depend if PassInLayer is odd or even
         if(PassInLayer % 2 == 0)
            {
            //even - Move A Head - one pixel
            YLocation += QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(CONFIG_GetHeadsPrintWidht()-InterlacePixels));
            if((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y, YLocation)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }
         else
            {
            // odd - Now is time to move one pixel but with backlash
            YLocation += QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() *
              static_cast<float>(InterlacePixels));
            if ((Err = m_Motors->PerformBacklashIfNeeded(YLocation)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Y,YLocation)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }
         }

      // Step X Fw Pass: Go to X stop print position - print forward
      if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
         return Err;
      if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
         return Err;

      // Step Prepare the axis Z to Roler High:
      //------------------------------------------
      Location = m_SequencerBlock.KeepZLocation +
                 CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                                         m_ParamsMgr->Z_MoveToStartNewLayer_um);

      if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
         return Err;

      // Step X Rw Pass:// Print in backward moving
      if ((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
         return Err;

      // Check if we have another pass in the same layer
      if(++PassInLayer < NoOfPasses)
         {
         // This is the end of the first FW RW pass move axis Z to next pass.
         // We need to move down the space of one layer but we need to move more
         // because Mechanism limitation.
         Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
         if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
            return Err;

         if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_Z,
                                   m_SequencerBlock.KeepZLocation)) != Q_NO_ERROR)
            return Err;

         // OHDB handle that should be performed after each pass
         if ((Err = CheckEndOfPass()) != Q_NO_ERROR)
            {
             if(Err != Q2RT_FIFO_NOT_EMPTY_ERROR)
             {
            CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice %d.",
                      Layer->GetSliceNumber());
            return Err;
            }
			 else
			    break;
            }

         } // end of // if there is another pass
      }  // PassInLayer < NoOfPasses

   if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck.: Slice%d.",
                      Layer->GetSliceNumber());
      return Err;
      }

   return Q_NO_ERROR;
}






