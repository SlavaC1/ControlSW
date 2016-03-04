/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Unidirection sequencer.                                  *
 * Module Description: Implementation of the specific Q2RT          *
 *                     Nozzles Test sequencer.                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger.                                         *
 * Start date: 27/06/2002                                           *
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

// Constants
//Timeouts


//--------------------------------------------------------------------------
TQErrCode CMachineSequencer::NozzlesTestSequencer(CLayer *Layer)
{
// Note1: all the Y backlash correction is performed in increase movement
//        and for axis Z in decrease movement.

//        Y movement depend of its real dpi.

// Constants
// ---------

// Local variables
// ---------------
   int PassInLayer;
   int YCloneCounter=0;
   long XStartPrintPosition=50; //In step
   long XStopPrintPosition=m_ParamsMgr->MaxPositionStep[AXIS_X];  //In step
   TQErrCode Err;
   long Location;

   //Parameters test
   if(m_ParamsMgr->PrintDirection != BIDIRECTION_PRINT_MODE)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Print direction is not marked as BIDERECTION_PRINT_MODE");
      FrontEndInterface->NotificationMessage("Nozzles test: Print direction is not marked as BIDERECTION_PRINT_MODE");
      return Q2RT_SEQUENCE_CANCELED;
      }
   if(m_ParamsMgr->ScatterEnabled)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test don't accept Scatter enabled option");
      FrontEndInterface->NotificationMessage("Nozzles test don't accept Scatter enabled option");
      return Q2RT_SEQUENCE_CANCELED;
      }

   CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Starting print of Layer:%d  ZPosition:%d",
                  Layer->GetTotalNumCounter(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare Y location
   // ------------------
   PassInLayer = 0;

   if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Check OCB error=%d",static_cast<int>(Err));
      return Err;
      }

   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   
   //The interlace can be Positive or Negatice according to print scan direction.
   long InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels *
      (Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN ? (INTERLACE_FW_DIRECTION) :
                                                               (INTERLACE_RW_DIRECTION));

   if ((Err = PrepareOHDBLayerConfig(Layer,LayerParms)) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Error in layerConfig-Error=:%d",
                                                    static_cast<int>(Err));
      return Err;
      }
   
   // Step X Start Position -  Go to X start print position
   // There is no need to move to XstartPosition because we are already before the
   // HW home and there is no meaning to move from Xhome to XstartPosition
   // and them the XStopPosition in the first pass

   //The layer finish prepare tray to next layer.
   if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
      return Err;

   while(YCloneCounter< m_ParamsMgr->NozzlesTestYOffset.Value())
      { 
      m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();
      
      if (Layer->GetTotalNumCounter() == 0)  // the first layer
         {
         // Y working in Y- direction there is no need to backlash correction
         // (last Y move was Y home position)
         Location = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                   static_cast<float>((Layer->GetYPosition()+YCloneCounter*2*
                      (m_ParamsMgr->NozzlesTestYCloneDistance_pixel/2))));

         KeepScanningCookingData(Location,
                           XStartPrintPosition,
                           XStopPrintPosition,
                           Layer->GetNoOfPasses(),
                           FORWARD_PRINT_SCAN);
                           
         if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
            return Err;
         }
      else
         {
         // Now we need to perform backlash
         Location = m_SequencerBlock.StartYPosition +
               QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                  static_cast<float>(Layer->GetYPosition()+YCloneCounter*2*
                  (m_ParamsMgr->NozzlesTestYCloneDistance_pixel/2)));

         if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
            return Err;
         
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
            // Avance position = A head - half pixel
            Location = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                         static_cast<float>(CONFIG_GetHeadsPrintWidht()-InterlacePixels));

            if((Err = m_Motors->GoWaitRelativePositionSequence(AXIS_Y, Location)) != Q_NO_ERROR)
               return Err;
            }

         // Step X Fw Pass: Go to X stop print position - print forward
         if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Step Move Half Pixel
         // The following command is to move FW half nozzle position with backslash
         // --------------------------------------------------------------------------
         Location = QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() *
                    static_cast<float>(InterlacePixels));
         if((Err = m_Motors->GoWaitToYRelativePositionWithBacklashSequence(Location)) != Q_NO_ERROR)
            return Err;

         // Step Prepare the axis Z to Roler High:
         //------------------------------------------
         Location = CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentLayerHeightInUM -
                                            m_ParamsMgr->Z_MoveToStartNewLayer_um);

         if ((Err = m_Motors->GoToRelativePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
            return Err;
         if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Z)) != Q_NO_ERROR)
            return Err;


         // Step X Rw Pass:// Print in backward moving
         if((Err = m_Motors->GoWaitAbsolutePositionSequence(AXIS_X,XStartPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Check if we have another pass in the same layer
         ++PassInLayer;
         if((PassInLayer < Layer->GetNoOfPasses()) ||
            ( (YCloneCounter + 1) < m_ParamsMgr->NozzlesTestYOffset.Value()))
            {
            Location = m_SequencerBlock.KeepZLocation + m_SequencerBlock.ZBacklash;
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
               CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice %d.",
                         Layer->GetSliceNumber());
               return Err;
               }
			 else
			    break;
            }

            } // end of // if there is another pass


         }  // PassInLayer < Layer->GetNoOfPasses()
         YCloneCounter++;
         PassInLayer=0;
      } // while(YCloneCounter< m_ParamsMgr->NozzlesTestYOffset.Value())
      
   if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck.: Slice%d.",
                      Layer->GetSliceNumber());
      return Err;
      }

   return Q_NO_ERROR;   
}


TQErrCode CMachineSequencer::NozzlesTestFourHeadSequencer(CLayer *Layer)
{
// Note1: all the Y backlash correction is performed in increase movement
//        and for axis Z in decrease movement.

//        Y movement depend of its real dpi.

// Constants
// ---------

// Local variables
// ---------------
   int PassInLayer;
   int YCloneCounter=0;
   long XStartPrintPosition=50; //In step
   long XStopPrintPosition=m_ParamsMgr->MaxPositionStep[AXIS_X];  //In step
   long YLocation = 0;
   long BacklashPosition = 0;
   TQErrCode Err;
   long Location;
   bool XDuringMovement=false;
   int PassDirection;

   if (m_ParamsMgr->GenerateExcelAnalyzer)
     CExcelSequenceAnalyzer::Init("SequencerAnalyzer.csv", 10, 10);
   else
     CExcelSequenceAnalyzerDummy::Init();

   CExcelSequenceAnalyzerBase *csv = CExcelSequenceAnalyzer::Instance();

   //char test[10];
   //strcpy (test, csv[0][0].c_str());
   //Parameters test

   if(m_ParamsMgr->PrintDirection != BIDIRECTION_PRINT_MODE)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Print direction is not marked as BIDERECTION_PRINT_MODE");
      FrontEndInterface->NotificationMessage("Nozzles test: Print direction is not marked as BIDERECTION_PRINT_MODE");
      return Q2RT_SEQUENCE_CANCELED;
      }
   if(m_ParamsMgr->ScatterEnabled)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test don't accept Scatter enabled option");
      FrontEndInterface->NotificationMessage("Nozzles test don't accept Scatter enabled option");
      return Q2RT_SEQUENCE_CANCELED;
      }

   if(Layer->GetPrintScanDirection() == FORWARD_PRINT_SCAN)
   {
      PassDirection = 1;
      PassInLayer = 0;
   }
   else
   {
      PassDirection = -1;
      PassInLayer = Layer->GetNoOfPasses()-1;
   }

   CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Starting print of Layer:%d  ZPosition:%d",
                  Layer->GetTotalNumCounter(),(int)m_ParamsMgr->LastSliceZPosition);

   // Prepare Y location
   // ------------------
   PassInLayer = 0;

   if ((Err = CheckStatusDuringPrinting()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Check OCB error=%d",static_cast<int>(Err));
      return Err;
      }

   TDPCPCLayerParams *LayerParms = Layer->GetDPCPCParams();
   
   //The interlace can be Positive or Negative according to print scan direction.

   long InterlacePixels = m_ParamsMgr->YInterlaceNoOfPixels * INTERLACE_RW_DIRECTION;

   if ((Err = PrepareOHDBLayerConfig(Layer,LayerParms)) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Nozzles test: Error in layerConfig-Error=:%d",
                                                    static_cast<int>(Err));
      return Err;
      }
   
   // Step X Start Position -  Go to X start print position
   // There is no need to move to XstartPosition because we are already before the
   // HW home and there is no meaning to move from Xhome to XstartPosition
   // and them the XStopPosition in the first pass

   //The layer finish prepare tray to next layer.
   if ((Err = PrepareTrayToPrintCurrentLayer(Layer)) != Q_NO_ERROR)
      return Err;

   while(YCloneCounter< m_ParamsMgr->NozzlesTestYOffset.Value())
      { 
      m_SequencerBlock.BufferLayerPrintPtr = Layer->GetBuffer();

      // this is the delta we need to add to each cloned slice according to number of iterations:
      int YCloneDelta = (YCloneCounter * m_ParamsMgr->NozzlesTestYCloneDistance_pixel);

      Location = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
                   static_cast<float>(Layer->GetYPosition()+YCloneDelta));


      csv->SetValueNext(SourceSequencer, Location);

      // Y working in Y- direction there is no need to backlash correction
      // (last Y move was Y home position)
      if (Layer->GetTotalNumCounter() == 0)  // the first layer
         {
         KeepScanningCookingData(Location,
                           XStartPrintPosition,
                           XStopPrintPosition,
                           Layer->GetNoOfPasses(),
                           FORWARD_PRINT_SCAN);

         if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
            return Err;
         }
      else
         {
         // Now we need to perform backlash
         if ((Err = m_Motors->GoWaitToYPositionWithBacklashSequence(Location)) != Q_NO_ERROR)
            return Err;
         
         }

   // Perform the movement until the end of the layer
   while(PassInLayer < Layer->GetNoOfPasses() && PassInLayer >= 0)
      {
         // OHDB handle that should be performed before each pass
         if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer)) != Q_NO_ERROR)
            return Err;

         // Step X Fw Pass: Go to X stop print position - print forward
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Step Move back DoublePassSpace (two pixels for four heads printing)
         // The following command is to move FW half nozzle position with backslash
         // -----------------------------------------------------------------------
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() * static_cast<float>
              (Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer - m_SequencerBlock.DoublePassSpace + YCloneDelta /*+ SliceNumModifier*/));

         csv->SetValueNext(SourceSequencer, YLocation);
         if(m_Motors->GetAbsoluteBacklashLocation(YLocation,BacklashPosition))
            {
            if ((Err = m_Motors->MoveABAbsPositionSequencer(AXIS_Y,AXIS_X,BacklashPosition)) != Q_NO_ERROR)
               return Err;
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_Y)) != Q_NO_ERROR)
               return Err;
            }

         // Step Prepare the axis Z to half layer size
         Location =  m_SequencerBlock.KeepZLocation +
                  CONFIG_ConvertZumToStep(m_SequencerBlock.CurrentHalfLayerHeightInUM -
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

         Err = CheckEndOfPass();

         // In a case of FIFO not empty error, break current layer print
         if(Err == Q2RT_FIFO_NOT_EMPTY_ERROR)
           break;

         // OHDB handle that should be performed after each pass
         if (Err != Q_NO_ERROR)
            {
            CQLog::Write(LOG_TAG_PRINT,"Error during checking OHDB for end pass.:Slice%d.",
                      Layer->GetSliceNumber());
            return Err;
            }

         //Now complete the pass (completion to 300 dpi of this pass)
         //Return Z to start Z position with backlash.
         Location = m_SequencerBlock.ZBacklash + m_SequencerBlock.KeepZLocation;
         if ((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_Z,Location)) != Q_NO_ERROR)
            return Err;

         // Advance position = two pixels + Delta interlace
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel() * static_cast<float>
              (Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer + static_cast<float>(InterlacePixels) + YCloneDelta/*+ SliceNumModifier*/));
         csv->SetValueNext(SourceSequencer, YLocation);

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
         if ((Err = PrepareOHDBPassConfig(Layer,LayerParms,PassInLayer,1)) != Q_NO_ERROR)
            return Err;

         // Step X Fw Pass: Go to X stop print position - print forward
         if((Err = m_Motors->GoToAbsolutePositionSequence(AXIS_X,XStopPrintPosition)) != Q_NO_ERROR)
            return Err;

         // Step Move back two pixels
         // The following command is to move FW half nozzle position with backslash
         // -----------------------------------------------------------------------
         YLocation -= QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*m_SequencerBlock.DoublePassSpace);
         csv->SetValueNext(SourceSequencer, YLocation);

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

      //Start new pass
      PassInLayer+=PassDirection;

      // Check if we have another pass in the same layer
      if(PassInLayer < Layer->GetNoOfPasses() && PassInLayer >= 0)
         {

         // Avance position according to direction = A head*Direction - half pixel
         YLocation = m_SequencerBlock.StartYPosition +
            QSimpleRound(m_ParamsMgr->GetYStepsPerPixel()*
               static_cast<float>(Layer->GetYPosition() + CONFIG_GetHeadsPrintWidht() * PassInLayer + YCloneDelta/*+ SliceNumModifier*/));

         csv->SetValueNext(SourceSequencer, YLocation);

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
            if ((Err = m_Motors->CheckForEndOfMovementSequence(AXIS_X)) != Q_NO_ERROR)
               return Err;
            XDuringMovement=false;
            }
         }

      }  // PassInLayer < Layer->GetNoOfPasses()
         YCloneCounter++;
         PassInLayer=0;
      } // while(YCloneCounter< m_ParamsMgr->NozzlesTestYOffset.Value())

   if ((Err = CheckEndOfLayer()) != Q_NO_ERROR)
      {
      CQLog::Write(LOG_TAG_PRINT,"Error in LayerEndCheck.: Slice%d.",
                      Layer->GetSliceNumber());
      return Err;
      }

   return Q_NO_ERROR;
}




