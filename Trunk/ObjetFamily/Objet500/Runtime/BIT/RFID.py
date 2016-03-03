################################################################################################
#
# 	Description: RFID test
#
# 	Version:	$Rev: 21923 $
# 	Date:		$Date: 2015-05-10 14:05:26 +0300 (Sun, 10 May 2015) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/RFID.py $ 
#
################################################################################################


from Q2RT_BIT import *

TAG_PRESENCE = 0
TEST_TIME_MS = 60000
READ_WRITE_FAIL = 0
if int(Application.MachineType) == OBJET_1000: #for ignoring M3!
   READ_WRITE = 6
   LAST_STEP = 13
   NUM_ACTIVE_TANKS_TO_IGNORE = 2
else:
   READ_WRITE = 16
   LAST_STEP = 33
   NUM_ACTIVE_TANKS_TO_IGNORE = 0

class RFIDTestSteps(SingleTest):
 def Start(Self):
     Self.ResDescription = ""
     Self.FailFlag       = False

 def End(Self):
    Log.Write(LOG_TAG_GENERAL,"RFIDTest::End")

 def GetSteps(Self):
  if int(Application.MachineType) == OBJET_1000: #for ignoring M3!
    Self.Steps = ['Tag Presents','S-1','S-2','M-1L','M-1R','M-2L','M-2R',
	'W/R RFID','S-1','S-2','M-1L','M-1R','M-2L','M-2R']
  else:
    Self.Steps = ['Tag Presents','S-1','S-2','M-1L','M-1R','M-2L','M-2R','M-3L','M-3R','M-4L','M-4R','M-5L','M-5R','M-6L','M-6R','S-3','S-4',
	'W/R RFID','S-1','S-2','M-1L','M-1R','M-2L','M-2R','M-3L','M-3R','M-4L','M-4R','M-5L','M-5R','M-6L','M-6R','S-3','S-4']
    #return ['RFID Tag presence','RFID Read-Write data']
  return Self.Steps
 def Execute(Self,Step):
   if Step == TAG_PRESENCE:
     Log.Write(LOG_TAG_GENERAL,"RFIDTest::RFID first step")
     for i in range (GlobalDefs.GetFirstTank(), GlobalDefs.GetLastTank()-NUM_ACTIVE_TANKS_TO_IGNORE, 1):
          if(Container.IsMicroSwitchInserted(i) == True):
             Container.ResetCounter(i)
     #QSleep(TEST_TIME_MS)
     CurrTime = TurnOnTime = QGetTicks()
     Log.Write(LOG_TAG_GENERAL,"RFIDTest::CurrTime = " + str(CurrTime) + "TurnOnTime "+ str(TurnOnTime))
     while (CurrTime - TurnOnTime) < TEST_TIME_MS:
        CurrTime = QGetTicks()
   elif Step <= READ_WRITE:
      Log.Write(LOG_TAG_GENERAL,"RFIDTest:: < READ_WRITE")
      Self.ResDescription = ""
      TankNumber = Step-1
      if(Container.IsMicroSwitchInserted(TankNumber) == True):
          if(Container.GetNumOfReconnections(TankNumber)>AppParams.RFReadersAddRemoveMax):
             Log.Write(LOG_TAG_GENERAL,"RFIDTest::too many reconnections")
             Self.SetActualVsRequested('<'+str(AppParams.RFReadersAddRemoveMax),str(Container.GetNumOfReconnections(TankNumber),'Not OK'))
             Self.FailFlag = True
             #Self.SetResultDescription(Self.ResDescription)
          else:
             Log.Write(LOG_TAG_GENERAL,"RFIDTest::no problem with reconnections")
             Self.SetActualVsRequested('<'+str(AppParams.RFReadersAddRemoveMax),str(Container.GetNumOfReconnections(TankNumber)),'OK')
      else:
           Self.SetActualVsRequested('','','Not tested(missing cartridge)')
   elif Step > READ_WRITE:
       Log.Write(LOG_TAG_GENERAL,"RFIDTest:: > READ_WRITE")
       TankNumber = Step-2-READ_WRITE 
       if(Step == READ_WRITE+1): 
	     Self.SetActualVsRequested('','','')  
       elif(Container.IsMicroSwitchInserted(TankNumber) == True):
             READ_WRITE_FAIL = 0
             READ_WRITE_CRITICAL_FAIL = 0
             RetCode = Q_NO_ERROR
             for t in range (0,AppParams.RFReadersReadWriteCycle,1):
                RetCode = Container.TagReadWriteFirstUsageTime(TankNumber)
                if(RetCode == Q_FAIL_ACTION):
                    READ_WRITE_FAIL += 1
                if(RetCode == Q_CRITICAL_FAIL_ACTION):
                    READ_WRITE_CRITICAL_FAIL = 1
                    t = AppParams.RFReadersReadWriteCycle
             if(READ_WRITE_CRITICAL_FAIL !=0):
                 Self.SetActualVsRequested('','','Not OK')
                 Self.FailFlag = True
                 Log.Write(LOG_TAG_GENERAL,"RFIDTest::RFID Read/Write problem")
             else:
               if(100*(AppParams.RFReadersReadWriteCycle-READ_WRITE_FAIL)/(AppParams.RFReadersReadWriteCycle) <= AppParams.RFReadersReadWriteThreshold):
                   Self.SetActualVsRequested('>'+str(AppParams.RFReadersReadWriteThreshold)+ '%',str(100*(AppParams.RFReadersReadWriteCycle-READ_WRITE_FAIL)/(AppParams.RFReadersReadWriteCycle))+'%','RFID Read/Write not stable')
                   Self.FailFlag = True
                   Log.Write(LOG_TAG_GENERAL,"RFIDTest::RFID Read/Write not stable")
               else:
			       #Self.SetActualVsRequested(str(AppParams.RFReadersReadWriteThreshold),str(READ_WRITE_FAIL),'RFID Read/Write OK')
                   Self.SetActualVsRequested('>'+str(AppParams.RFReadersReadWriteThreshold)+'%',str(100*(AppParams.RFReadersReadWriteCycle-READ_WRITE_FAIL)/(AppParams.RFReadersReadWriteCycle))+'%','Ok')
       else:
          Self.SetActualVsRequested('','','Not tested(missing cartridge)')

   if Step == LAST_STEP:
       Log.Write(LOG_TAG_GENERAL,"RFIDTest::Last step")
       if Self.FailFlag == True:
        #Self.SetActualVsRequested('','','Some problem with Read/Write from RFID')
         return trNoGo
        #else:
          #Self.SetActualVsRequested('','','No problem with Read/Write from RFID')
   
       return trGo
	   
def printEx(str, IndentCounter):
  indent = ""
  for i in range (IndentCounter):
    indent += "    "
  #print indent + str # remark so Monitor dialog will not pop

def GetTests():
  return [RFIDTestSteps('RF ID')]