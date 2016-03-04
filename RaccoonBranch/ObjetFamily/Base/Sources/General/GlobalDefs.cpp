#include "GlobalDefs.h"
#include "AppParams.h"

#ifndef PARAMS_EXP_IMP /*{*/

#include "QComponent.h"
#include "ModesDefs.h"
#include "FEResources.h"
#include "MaintenanceCountersDefs.h"
#include <set>

QString ResinTypeDescription[NUMBER_OF_RESIN_TYPES] =
{
   "Support"
   ,"Model"
};
/*============================================================================
Pumps
============================================================================*/
extern const int PumpsCounterID[NUMBER_OF_PUMPS] =
{
#ifdef CONNEX_MACHINE /*{*/
                  SUPPORT_1_FILLING_COUNTER_ID
                 ,SUPPORT_2_FILLING_COUNTER_ID
                 ,MODEL_1_FILLING_M0_M1_COUNTER_ID
                 ,MODEL_2_FILLING_M2_M3_COUNTER_ID
                 ,MODEL_1_FILLING_M2_M3_COUNTER_ID
                 ,MODEL_2_FILLING_M0_M1_COUNTER_ID
#elif defined OBJET_TWO_TANK_MACHINE
                  SUPPORT_1_FILLING_COUNTER_ID
                 ,MODEL_1_FILLING_M0_M1_COUNTER_ID
#elif defined OBJET_MACHINE
                 SUPPORT_1_FILLING_COUNTER_ID
                ,SUPPORT_2_FILLING_COUNTER_ID
                ,MODEL_1_FILLING_M0_M1_COUNTER_ID
                ,MODEL_2_FILLING_M0_M1_COUNTER_ID
                ,MODEL_3_FILLING_M2_M3_COUNTER_ID
                ,MODEL_4_FILLING_M2_M3_COUNTER_ID
                ,MODEL_5_FILLING_M4_M5_COUNTER_ID
                ,MODEL_6_FILLING_M4_M5_COUNTER_ID
#else
                  SUPPORT_1_FILLING_COUNTER_ID
                 ,SUPPORT_2_FILLING_COUNTER_ID
                 ,MODEL_1_FILLING_M0_M1_COUNTER_ID
                 ,MODEL_2_FILLING_M2_M3_COUNTER_ID
#endif /*}*/
};

extern const int HeadsCounterID[TOTAL_NUMBER_OF_HEADS] =
{
  //OBJET_MACHINE config
   HEAD_S0_COUNTER_ID
  ,HEAD_S1_COUNTER_ID
  ,HEAD_M4_COUNTER_ID
  ,HEAD_M5_COUNTER_ID
  ,HEAD_M3_COUNTER_ID
  ,HEAD_M2_COUNTER_ID
  ,HEAD_M1_COUNTER_ID
  ,HEAD_M0_COUNTER_ID
};

extern const TActuatorIndex PumpsActuatorID[NUMBER_OF_PUMPS] =
{
#ifdef CONNEX_MACHINE
                  ACTUATOR_ID_SUPPORT_PUMP_2
                 ,ACTUATOR_ID_SUPPORT_PUMP_1
                 ,ACTUATOR_ID_MODEL2_H2_3_PUMP
                 ,ACTUATOR_ID_MODEL1_H2_3_PUMP
                 ,ACTUATOR_ID_MODEL2_H0_1_PUMP
                 ,ACTUATOR_ID_MODEL1_H0_1_PUMP
#elif defined OBJET_TWO_TANK_MACHINE
                  ACTUATOR_ID_SUPPORT_PUMP_1
                 ,ACTUATOR_ID_MODEL_PUMP_1
#elif defined OBJET_MACHINE
                 ACTUATOR_ID_SUPPORT_PUMP_2
                 ,ACTUATOR_ID_SUPPORT_PUMP_1
                 //,ACTUATOR_ID_SUPPORT_PUMP_H4_5_PUMP currently unavailable
                ,ACTUATOR_ID_MODEL1_H0_1_PUMP
                ,ACTUATOR_ID_MODEL2_H0_1_PUMP
                //,ACTUATOR_ID_MODEL1_2_H2_3_PUMP currently unavailable
                ,ACTUATOR_ID_MODEL3_H2_3_PUMP
                ,ACTUATOR_ID_MODEL4_H2_3_PUMP
                ,ACTUATOR_ID_MODEL5_H4_5_PUMP
                ,ACTUATOR_ID_MODEL6_H4_5_PUMP
#else
                  ACTUATOR_ID_SUPPORT_PUMP_1
                 ,ACTUATOR_ID_SUPPORT_PUMP_2
                 ,ACTUATOR_ID_MODEL_PUMP_1
                 ,ACTUATOR_ID_MODEL_PUMP_2
#endif
};

/*============================================================================
Drain pumps
============================================================================*/
#ifndef OBJET_MACHINE
const int DrainPumpsCounterID[TOTAL_NUMBER_OF_CONTAINERS] =
{
#ifdef OBJET_TWO_TANK_MACHINE  /*{*/
                 SUPPORT_DRAIN_PUMP_1_COUNTER_ID
                ,MODEL_DRAIN_PUMP_1_COUNTER_ID
#elif defined OBJET_FOUR_TANK_MACHINE
                 SUPPORT_DRAIN_PUMP_1_COUNTER_ID
                ,SUPPORT_DRAIN_PUMP_2_COUNTER_ID
                ,MODEL_DRAIN_PUMP_1_COUNTER_ID
                ,MODEL_DRAIN_PUMP_2_COUNTER_ID
#endif /*}*/
};
#endif

QString AllModesNames         [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
QString AllModesShortNames    [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
bool    AllModesAccessibility [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];


/*============================================================================
Methods
============================================================================*/
//----ToStrings Conversions----------------------
QString TankToStr(TTankIndex Tank)
{
   QString Str;
   if (VALIDATE_TANK_INCLUDING_WASTE(Tank))
      Str = LOAD_QSTRING_ARRAY(Tank,IDS_TANK_STRINGS);
   else if (Tank == NO_TANK)
      Str = "'NO TANK'";
   return Str;
}

QString ChamberToStr(TChamberIndex Chamber, bool AsModeFunc, TOperationModeIndex Mode)
{
   QString Str;
   if(VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
   {
     if (AsModeFunc && IS_MODEL_CHAMBER(Chamber) && Mode == SINGLE_MATERIAL_OPERATION_MODE)
        Str = LOAD_STRING(IDS_MODEL_STR);
     else

        Str = LOAD_QSTRING_ARRAY(Chamber,IDS_CHAMBER_STRINGS);
   }
   return Str;
}

QString ChamberThermistorToStr(int ChamberThermistor)
{
   QString Str;
   if(VALIDATE_CHAMBER_THERMISTOR(ChamberThermistor))
   {
	 Str = LOAD_QSTRING_ARRAY(ChamberThermistor,IDS_CHAMBERS_THERMISTOR_STRINGS);
   }
   return Str;
}

QString GetHeaterStr(int Heater)
{
   QString Ret;
   if(VALIDATE_HEATER_INCLUDING_PREHEATER(Heater))
      Ret = LOAD_QSTRING_ARRAY(Heater,IDS_HEATERS_STRINGS);
   return Ret;
}

QString GetHeadName(int Head)
{
   QString Ret;
   if(VALIDATE_HEAD(Head))
      Ret = LOAD_QSTRING_ARRAY(Head,IDS_HEADS_STRINGS);
   return Ret;
}

QString GetUVLampStr(int/*TUVLampIndex*/ Lamp)
{
   QString Ret;
   if(VALIDATE_UV_LAMP(Lamp))
      Ret = LOAD_QSTRING_ARRAY(Lamp,IDS_UVLAMPS_STRINGS);
   return Ret;
}

QString GetQualityModeStr(TQualityModeIndex QualityMode)
{
   QString Ret;
   if(VALIDATE_QUALITY_MODE(QualityMode))
      Ret = LOAD_QSTRING_ARRAY(QualityMode,IDS_LONG_QMODES_STRINGS);
   return Ret;
}

bool GetModeAccessibility(int QualityMode, int OperationMode)
{
   bool Ret = false;
   if(VALIDATE_QUALITY_MODE(QualityMode))
      if(VALIDATE_OPERATION_MODE(OperationMode))
         Ret = AllModesAccessibility[QualityMode][OperationMode];
   return Ret;
}

QString GetModeStr(int QualityMode, int OperationMode)
{
   QString Ret = "Default";
   if(VALIDATE_QUALITY_MODE(QualityMode))
      if(VALIDATE_OPERATION_MODE(OperationMode))
         Ret = AllModesNames[QualityMode][OperationMode];
   return Ret;
}

QString GetModeShortStr(int QualityMode, int OperationMode)
{
   QString Ret = "Default";
   if(VALIDATE_QUALITY_MODE(QualityMode))
      if(VALIDATE_OPERATION_MODE(OperationMode))
         Ret = AllModesShortNames[QualityMode][OperationMode];
   return Ret;
}

bool GetModeFromShortStr(QString ShortStr, int& QualityMode, int& OperationMode)
{
   for(QualityMode = 0; QualityMode < NUMBER_OF_QUALITY_MODES; QualityMode++)
      for(OperationMode = 0; OperationMode < NUM_OF_OPERATION_MODES; OperationMode++)
         if (AllModesShortNames[QualityMode][OperationMode].compare(ShortStr) == 0)
            return true;
   return false;
}

bool Is_6_ModelHeads()
{
	bool ret = false;
	int tom = CAppParams::Instance()->ThermistorsOperationMode;

	if(tom == LOW_THERMISTORS_MODE || tom == SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE)
		ret = true;
	else
	if(tom == HIGH_THERMISTORS_MODE || tom == SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE)
		ret = false;

	return ret;
}

QString OperationModeToStr(int OperationMode)
{
  QString ret;
  if (!VALIDATE_OPERATION_MODE(OperationMode))
      throw EQException("OperationModeToStr - Invalid OperationMode Index");
  ret = LOAD_QSTRING_ARRAY(OperationMode,IDS_OMODES_STRINGS);
  return ret;
}//OperationModeToStr


//----Types Relation Conversions----------------------
TSegmentIndex GetTankAdjacentSegment(TTankIndex Tank)
{
   TSegmentIndex ret = static_cast<TSegmentIndex>(0);

   if(!VALIDATE_TANK(Tank))
	return ret;
#ifdef CONNEX_MACHINE
   switch(Tank)
   {
   case TYPE_TANK_MODEL1:
     ret = MODEL2_H2_3_SEGMENT;
	 break;
   case TYPE_TANK_MODEL2:
     ret = MODEL1_H0_1_SEGMENT;
	 break;
   case TYPE_TANK_SUPPORT2:
     ret = S1_SEGMENT;
	 break;
   case TYPE_TANK_SUPPORT1:
     ret = S2_SEGMENT;
	 break;
   default:
	 break;
   }
#elif defined OBJET_MACHINE  
   switch(Tank)
   {
   case TYPE_TANK_MODEL1:
     ret = MODEL1_H0_1_SEGMENT;
	 break;
   case TYPE_TANK_MODEL2:
     ret = MODEL2_H0_1_SEGMENT;
	 break;
   case TYPE_TANK_MODEL3:
     ret = MODEL3_H2_3_SEGMENT;
	 break;
   case TYPE_TANK_MODEL4:
     ret = MODEL4_H2_3_SEGMENT;
	 break;
   case TYPE_TANK_MODEL5:
     ret = MODEL5_H4_5_SEGMENT;
	 break;
   case TYPE_TANK_MODEL6:
     ret = MODEL6_H4_5_SEGMENT;
	 break;
   case TYPE_TANK_SUPPORT2:
     ret = S2_SEGMENT;
	 break;
   case TYPE_TANK_SUPPORT1:
     ret = S1_SEGMENT;
	 break;
   default:
	 break;
   } 
#endif

   return ret;
}

TChamberIndex GetHeadsChamber(int/*THeadIndex*/ Head, int PrintingOperationMode, bool AsModeFunc)
{
   if (!VALIDATE_HEAD(Head))
      return TYPE_CHAMBER_SUPPORT;
      
   TChamberIndex ret = HeadToChamber[Head];
#if defined (CONNEX_MACHINE)
   //There is only one model bitmap in case of single mode or weight test print
   if (AsModeFunc == true && IS_MODEL_CHAMBER(ret))
      if (PrintingOperationMode == SINGLE_MATERIAL_OPERATION_MODE)
          ret = TYPE_CHAMBER_MODEL1;
#elif defined (OBJET_MACHINE) //itamar objet check!!! 1 bmp for the left 4 heads, 1 bmp for the right 4 heads
   //There is only one model bitmap in case of single mode or weight test print
   if (AsModeFunc==true && PrintingOperationMode==SINGLE_MATERIAL_OPERATION_MODE)
   {
      if(ret==TYPE_CHAMBER_MODEL2)
          ret = TYPE_CHAMBER_MODEL1;
      else if(ret==TYPE_CHAMBER_MODEL3)
          ret = TYPE_CHAMBER_SUPPORT;
   }
#endif
   return ret;
}
TChamberIndex GetHeadsChamberForHSW(int/*THeadIndex*/ Head, int PrintingOperationMode, bool AsModeFunc)
{
   if (!VALIDATE_HEAD(Head))
      return TYPE_CHAMBER_SUPPORT;
      
   TChamberIndex ret = HeadToChamber[Head];
  if (AsModeFunc==true && PrintingOperationMode==SINGLE_MATERIAL_OPERATION_MODE)
   {
      if(ret==TYPE_CHAMBER_MODEL3)
          ret = TYPE_CHAMBER_SUPPORT;
   }
   return ret;
}

TChamberIndex GetSiblingChamber(TChamberIndex Chamber)
{
	TChamberIndex Ret = static_cast<TChamberIndex>(0);

	switch(Chamber)
	{
	case TYPE_CHAMBER_MODEL1:
		Ret = TYPE_CHAMBER_MODEL2;
		break;
	case TYPE_CHAMBER_MODEL2:
		Ret = TYPE_CHAMBER_MODEL1;
		break;
	case TYPE_CHAMBER_MODEL3:
		Ret = TYPE_CHAMBER_SUPPORT;
		break;
	case TYPE_CHAMBER_SUPPORT:
		Ret = TYPE_CHAMBER_MODEL3;
		break;
	case TYPE_CHAMBER_WASTE:
		Ret = TYPE_CHAMBER_WASTE;
		break;

	default:
			throw EQException("Invalid chamber ID");
	}

	return Ret;
}

TTankIndex GetSiblingTank(TTankIndex TankIndex)
{
	TTankIndex Ret = static_cast<TTankIndex>(0);

	switch(TankIndex)
	{
		case TYPE_TANK_SUPPORT1: Ret = TYPE_TANK_SUPPORT2; break;
		case TYPE_TANK_SUPPORT2: Ret = TYPE_TANK_SUPPORT1; break;
		case TYPE_TANK_MODEL1:	 Ret = TYPE_TANK_MODEL2; break;
		case TYPE_TANK_MODEL2:   Ret = TYPE_TANK_MODEL1; break;
#ifdef OBJET_MACHINE
		case TYPE_TANK_MODEL3:   Ret = TYPE_TANK_MODEL4; break;
		case TYPE_TANK_MODEL4:   Ret = TYPE_TANK_MODEL3; break;
		case TYPE_TANK_MODEL5:   Ret = TYPE_TANK_MODEL6; break;
		case TYPE_TANK_MODEL6:   Ret = TYPE_TANK_MODEL5; break;
#endif
		default:
			throw EQException("Invalid tank ID");
	}

	return Ret;
}

CHAMBERS_THERMISTORS_EN GetTankRelatedHighThermistor(TTankIndex Tank)
{
	CHAMBERS_THERMISTORS_EN Ret = static_cast<CHAMBERS_THERMISTORS_EN>(0);

	if(!VALIDATE_TANK(Tank))
		return Ret;

	switch(Tank)
	{
	case TYPE_TANK_SUPPORT1:
	case TYPE_TANK_SUPPORT2:
		Ret = SUPPORT_M3_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL1:
	case TYPE_TANK_MODEL2:
		Ret = M1_M2_CHAMBER_THERMISTOR;
		break;
#ifdef OBJET_MACHINE
	case TYPE_TANK_MODEL3:
	case TYPE_TANK_MODEL4:
		Ret = M1_M2_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL5:
	case TYPE_TANK_MODEL6:
		Ret = SUPPORT_M3_CHAMBER_THERMISTOR;
		break;
#endif
	default:
		break;
	}
	
	return Ret;
}

CHAMBERS_THERMISTORS_EN GetTankRelatedLowThermistor(TTankIndex Tank)
{
	CHAMBERS_THERMISTORS_EN Ret = static_cast<CHAMBERS_THERMISTORS_EN>(0);

	if(!VALIDATE_TANK(Tank))
		return Ret;

	switch(Tank)
	{
	case TYPE_TANK_SUPPORT1:
	case TYPE_TANK_SUPPORT2:
		Ret = SUPPORT_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL1:
	case TYPE_TANK_MODEL2:
		Ret = M1_CHAMBER_THERMISTOR;
		break;
#ifdef OBJET_MACHINE
	case TYPE_TANK_MODEL3:
	case TYPE_TANK_MODEL4:
		Ret = M2_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL5:
	case TYPE_TANK_MODEL6:
		Ret = M3_CHAMBER_THERMISTOR;
		break;
#endif
	default:
		break;
	}
	
	return Ret;
}

bool IsTankInSupportBlockHalf(TTankIndex Tank)
{
	bool ret = false;

	if(!VALIDATE_TANK(Tank))
		return ret;

	switch(Tank)
	{
	case TYPE_TANK_SUPPORT1:
	case TYPE_TANK_SUPPORT2:
#ifdef OBJET_MACHINE
	case TYPE_TANK_MODEL5:
	case TYPE_TANK_MODEL6:
#endif
		return true;
	}
   return false;
}

bool IsTankInModelBlockHalf(TTankIndex Tank)
{
	bool ret = false;

	if(!VALIDATE_TANK(Tank))
		return ret;

	switch(Tank)
	{
	case TYPE_TANK_MODEL1:
	case TYPE_TANK_MODEL2:
#ifdef OBJET_MACHINE
	case TYPE_TANK_MODEL3:
	case TYPE_TANK_MODEL4:
#endif
		return true;
	}
   return false;
}



int GetDrainPumpCounterID(TTankIndex Tank)
{
   int Ret = 0;
#ifndef OBJET_MACHINE
   if(VALIDATE_TANK(Tank))
      Ret = DrainPumpsCounterID[Tank];
#endif	  
   return Ret;
}

TChamberIndex TankToStaticChamber(TTankIndex Tank)
{
   for(int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; ++i)
	  if (ChamberTankRelationArr[S_M1_M2_M3_ACTIVE_TANKS_MODE]
		  					    [i + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
		 return (TChamberIndex)i;

   //There is a tank that is not connected physically to any chamber
   throw EInvalidResult("Internal error: ChamberTankRelationArr is not defined correctly");
}

/* Warning! Read Container's CContainerBase::UpdateChambers info first. */
TChamberIndex TankToChamber(TTankIndex Tank)
{
   CAppParams *ParamManager = CAppParams::Instance();
   for(int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; ++i)
	  if (ParamManager->ChamberTankRelation[i + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
		 return (TChamberIndex)i;

   //If the tank is not active/ unrelated to all chambers, then ANY index result will be bad
   return NUMBER_OF_CHAMBERS;
}

TSegmentIndex TankToShortSegment(TTankIndex Tank)
{
	return static_cast<TSegmentIndex>(Tank);
}

bool IsChamberFlooded(TChamberIndex ChamberIndex)
{
	if(ChamberIndex == NUMBER_OF_CHAMBERS)
		return true;
		
	CAppParams *ParamManager = CAppParams::Instance();
	bool ChamberFloodingMapping[NUMBER_OF_CHAMBERS][NUM_OF_TANKS_OPERATION_MODES] =
			{{0,0,0,0,0,0},
			 {0,1,0,0,0,1},
			 {1,0,0,0,1,0},
			 {1,1,0,1,0,0}};

	if(ChamberFloodingMapping[ChamberIndex][ParamManager->TanksOperationMode] == 1)
		return true;
	else
		return false;
}

bool IsPumpRelatedToTank(TPumpIndex Pump, TTankIndex Tank)
{
   CAppParams *ParamManager = CAppParams::Instance();

   int pos = (int)Pump + (NUMBER_OF_PUMPS * (int)Tank);
   if (ParamManager->PumpTankRelation[pos])
     return true;

   return false;
}

//Checks if the tank is related to the Dynamic (!) chamber.
//i.e., if the chamber is flooded, then its physical tanks ARE not related to the chamber
bool IsChamberRelatedToTank(TChamberIndex Chamber, TTankIndex Tank)
{
   CAppParams *ParamManager = CAppParams::Instance();

   int pos = (int)Chamber + (NUMBER_OF_CHAMBERS_INCLUDING_WASTE * (int)Tank);
   if (ParamManager->ChamberTankRelation[pos])
     return true;

   return false;
}

bool IsSegmentRelatedToTank(TSegmentIndex Segment, TTankIndex Tank)
{
   CAppParams *ParamManager = CAppParams::Instance();

   int pos = (int)Segment + (NUMBER_OF_SEGMENTS * (int)Tank);
   if (ParamManager->SegmentTankRelation[pos])
     return true;

   return false;
}

int	GetHeadsNum()
{
	return (SINGLE_MATERIAL_OPERATION_MODE == CAppParams::Instance()->PrintingOperationMode) ? 8 : 4;
}

#if defined CONNEX_MACHINE || defined OBJET_MACHINE //itamar objet check!!!
int GetActiveChambersNum()
{
  int ret = NUMBER_OF_CHAMBERS;
  switch(CAppParams::Instance()->PrintingOperationMode)
  {
     case SINGLE_MATERIAL_OPERATION_MODE:
          ret = SM_RESIN_NUM;
          break;
     case DIGITAL_MATERIAL_OPERATION_MODE:
		  ret = MM_RESIN_NUM;//3 in connex, 4 in objet
          break;
  }
  return ret;
}

TTankIndex GetSpecificTank(TChamberIndex Chamber, int Num)
{
	int occurence = Num;
	
	for (int t = FIRST_TANK_TYPE; t < LAST_TANK_TYPE; t++)
	{
	   if (ChamberTankRelationArr[S_M1_M2_M3_ACTIVE_TANKS_MODE]
				[Chamber + t*NUMBER_OF_CHAMBERS_INCLUDING_WASTE] == true)
	   {
			--occurence;
			if (0 <= occurence) return (TTankIndex)t;
	   }
	}
	return NO_TANK;
}

//called only from MRW
int OperationModeToActiveTankNum(int OperationMode)
{
  int ret;
  switch(OperationMode)
  {
     case SINGLE_MATERIAL_OPERATION_MODE:
          ret = SM_MODEL_NUM;
          break;
     case DIGITAL_MATERIAL_OPERATION_MODE:
          ret = MM_MODEL_NUM;
          break;
     default:
          ret = -1;
  }
  return ret;
}

int OperationModeToBitmapsNums(int OperationMode)
{
  int ret = 0;
  switch(OperationMode)
  {
      case SINGLE_MATERIAL_OPERATION_MODE:
           ret = SM_RESIN_NUM;
           break;
      case DIGITAL_MATERIAL_OPERATION_MODE:
		   ret = MM_RESIN_NUM;//3 in connex, 4 in objet
           break;
  }
  return ret;
}

//The thermistors' state in objet indicates the number of active tanks in the system  
int ModelActiveContainersNum(void)
{
  int ret = 0;
  CAppParams*        ParamManager = CAppParams::Instance();
  switch(ParamManager->ThermistorsOperationMode)
  {
	  case HIGH_THERMISTORS_MODE: //1 Eden mode
		   ret = SM_MODEL_NUM;
		   break;
	  case LOW_THERMISTORS_MODE: //3  Objet mode
		   ret = MM_MODEL_NUM;
		   break;
	  case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE:  //2 Connex mode
	  case SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE:
		   ret = MM_MODEL_NUM-1;
		   break;
  }
  return ret;
}


int SupportActiveContainersNum(void)
{
   return MAX_NUMBER_OF_SUPPORT_CHAMBERS;
}
#endif

bool ValidateChamber(int Chamber)
{
   return ((FIRST_CHAMBER_TYPE <= Chamber) && (LAST_CHAMBER_TYPE > Chamber));
}

bool ValidateHeaterIncludingPreheater(int h)
{
   return ((0 <= h) && (TOTAL_NUMBER_OF_HEATERS > h));

}

bool ValidateChamberThermistor(int ct)
{
   return ((0 <= ct) && (NUM_OF_CHAMBERS_THERMISTORS > ct));
}


/*============================================================================
CAppGlobalDefs
Class for Python wizards to use
============================================================================*/

CAppGlobalDefs* CAppGlobalDefs::m_Instance = NULL;

CAppGlobalDefs::CAppGlobalDefs() : CQComponent("GlobalDefs")
{
  //bug 6258
   INIT_METHOD(CAppGlobalDefs,GetFirstModelTank); 
   INIT_METHOD(CAppGlobalDefs,GetLastModelTank); 
   INIT_METHOD(CAppGlobalDefs,GetFirstSupportTank); 
   INIT_METHOD(CAppGlobalDefs,GetLastSupportTank); 
   INIT_METHOD(CAppGlobalDefs,GetFirstTank); 
   INIT_METHOD(CAppGlobalDefs,GetLastTank); 

   INIT_METHOD(CAppGlobalDefs,GetFirstChamber);
   INIT_METHOD(CAppGlobalDefs,GetLastChamber);
   INIT_METHOD(CAppGlobalDefs,GetFirstModelChamber);
   INIT_METHOD(CAppGlobalDefs,GetLastModelChamber);
   INIT_METHOD(CAppGlobalDefs,GetChamberStr);
   INIT_METHOD(CAppGlobalDefs,GetChamberActivePumpID);
   INIT_METHOD(CAppGlobalDefs,GetChamberActivePumpActuatorID);
   INIT_METHOD(CAppGlobalDefs,GetFirstQualityMode);
   INIT_METHOD(CAppGlobalDefs,GetLastQualityMode);
   INIT_METHOD(CAppGlobalDefs,GetFirstUserLevelOperationMode);
   INIT_METHOD(CAppGlobalDefs,GetLastUserLevelOperationMode);
   INIT_METHOD(CAppGlobalDefs,GetAppModeAccessibility);
   INIT_METHOD(CAppGlobalDefs,GetModeDirStr);
   INIT_METHOD(CAppGlobalDefs,GetModeName);

#ifndef EDEN_350
   AllModesNames        [HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "High Speed";
   AllModesNames        [HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "Digital Materials";
   AllModesShortNames   [HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "HS";
   AllModesShortNames   [HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "DM";
   AllModesAccessibility[HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = true;
   AllModesAccessibility[HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = true;
#ifdef OBJET_MACHINE
   //AllModesNames        [HS_INDEX][DIGITAL_MATERIAL_3_OPERATION_MODE] = "Digital Materials 3";
   //AllModesShortNames   [HS_INDEX][DIGITAL_MATERIAL_3_OPERATION_MODE] = "DM3";
   //AllModesAccessibility   [HS_INDEX][DIGITAL_MATERIAL_3_OPERATION_MODE] = true;
#endif

#endif
   AllModesNames        [HQ_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "High Quality";
   AllModesNames        [HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "DJ1";
   AllModesShortNames   [HQ_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "HQ";
   AllModesShortNames   [HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "DJ1";
   AllModesAccessibility[HQ_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = true;
   AllModesAccessibility[HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = false;

#ifdef OBJET_MACHINE
   //AllModesNames        [HQ_INDEX][DIGITAL_MATERIAL_3_OPERATION_MODE] = "DJ1";
   //AllModesShortNames   [HQ_INDEX][DIGITAL_MATERIAL_3_OPERATION_MODE] = "DJ1";
   //AllModesAccessibility[HQ_INDEX][DIGITAL_MATERIAL_3_OPERATION_MODE] = false;
#endif
}

void CAppGlobalDefs::Init(void)
{}

CAppGlobalDefs* CAppGlobalDefs::Instance(void)
{
   if(!m_Instance)
      m_Instance = new CAppGlobalDefs();
   return m_Instance;
}

void CAppGlobalDefs::DeInit()
{
   if(m_Instance)
      delete m_Instance;
}

int CAppGlobalDefs::GetFirstModelChamber()
{
   return (int)FIRST_MODEL_CHAMBER_INDEX;
}

int CAppGlobalDefs::GetLastModelChamber()
{
   return (int)LAST_MODEL_CHAMBER_INDEX;
}

int CAppGlobalDefs::GetFirstChamber()
{
   return (int)FIRST_CHAMBER_TYPE;
}

//bug 6258
int CAppGlobalDefs::GetFirstModelTank()
{
   return (int)FIRST_MODEL_TANK_TYPE;
}

int CAppGlobalDefs::GetLastModelTank()
{
   return (int)LAST_MODEL_TANK_TYPE;
}

int CAppGlobalDefs::GetFirstSupportTank()
{
   return (int)FIRST_SUPPORT_TANK_TYPE;
}

int CAppGlobalDefs::GetLastSupportTank()
{
   return (int)LAST_SUPPORT_TANK_TYPE;
}

int CAppGlobalDefs::GetFirstTank()
{
   return (int)FIRST_TANK_TYPE;
}

int CAppGlobalDefs::GetLastTank()
{
   return (int)LAST_TANK_TYPE;
}

//bug 6258

int CAppGlobalDefs::GetLastChamber()
{
   return (int)LAST_CHAMBER_TYPE;
}

QString CAppGlobalDefs::GetChamberStr(int Chamber)
{
   return ChamberToStr((TChamberIndex)Chamber);
}

TPumpIndex CAppGlobalDefs::GetChamberActivePumpID(int /*TChamberIndex*/ Chamber)
{
	TPumpIndex ret = static_cast<TPumpIndex>(0);
	
   int tank = CAppParams::Instance()->ActiveTanks[Chamber];
   switch(Chamber)
   {
//#if !defined (CONNEX_MACHINE) && !defined (OBJET_MACHINE)  //itamar objet should not be in comment!!!
       //case TYPE_CHAMBER_MODEL1:
//#endif
       case TYPE_CHAMBER_SUPPORT:
            ret = ((TTankIndex)tank == TYPE_TANK_SUPPORT1) ? SUPPORT_PUMP1 : SUPPORT_PUMP2;
            break;
#ifdef CONNEX_MACHINE
       case TYPE_CHAMBER_MODEL1:
            ret = ((TTankIndex)tank == TYPE_TANK_MODEL1) ? MODEL2_H0_1_PUMP : MODEL1_H0_1_PUMP;
            break;
       case TYPE_CHAMBER_MODEL2:
            ret = ((TTankIndex)tank == TYPE_TANK_MODEL1) ? MODEL2_H2_3_PUMP : MODEL1_H2_3_PUMP;
            break;
#elif defined OBJET_MACHINE
       case TYPE_CHAMBER_MODEL1:
            ret = ((TTankIndex)tank == TYPE_TANK_MODEL1) ? MODEL1_H0_1_PUMP : MODEL2_H0_1_PUMP;
            break;
       case TYPE_CHAMBER_MODEL2:
            ret = ((TTankIndex)tank == TYPE_TANK_MODEL3) ? MODEL3_H2_3_PUMP : MODEL4_H2_3_PUMP;
            break;
       case TYPE_CHAMBER_MODEL3:
            ret = ((TTankIndex)tank == TYPE_TANK_MODEL5) ? MODEL5_H4_5_PUMP : MODEL6_H4_5_PUMP;
            break;
#endif
       default:
            throw EQException("CAppGlobalDefs::GetChamberActivePumpID - Invalid Chamber Index");
   }
   return ret;
}

int CAppGlobalDefs::GetChamberActivePumpActuatorID(int Chamber)
{
	return PumpsActuatorID[GetChamberActivePumpID( (TChamberIndex)Chamber )];
}

int CAppGlobalDefs::GetFirstQualityMode()
{
   return (int)FIRST_QUALITY_MODE;
}

int CAppGlobalDefs::GetLastQualityMode()
{
   return (int)LAST_QUALITY_MODE;
}

int CAppGlobalDefs::GetFirstUserLevelOperationMode()
{
   return FIRST_OPERATION_MODE;
}

int CAppGlobalDefs::GetLastUserLevelOperationMode()
{
   return LAST_OPERATION_MODE;
}

bool CAppGlobalDefs::GetAppModeAccessibility(int QualityMode, int OperationMode)
{
   return GetModeAccessibility(QualityMode,OperationMode);
}

QString CAppGlobalDefs::GetModeDirStr(int QualityMode, int OperationMode)
{
   return MACHINE_QUALITY_MODES_DIR(QualityMode,OperationMode);
}

QString CAppGlobalDefs::GetModeName(int QualityMode, int OperationMode)
{
   return GetModeStr(QualityMode,OperationMode);
}

#endif /*}*/


#define CASE_ID_RETURN_STR(id) case id: return #id;
QString PumpIndexToDescription(TPumpIndex i)
{
	switch(i)
    {
#ifdef CONNEX_MACHINE
         CASE_ID_RETURN_STR(SUPPORT_PUMP2)
         CASE_ID_RETURN_STR(SUPPORT_PUMP1)
         CASE_ID_RETURN_STR(MODEL2_H2_3_PUMP)
         CASE_ID_RETURN_STR(MODEL1_H2_3_PUMP)
         CASE_ID_RETURN_STR(MODEL2_H0_1_PUMP)
         CASE_ID_RETURN_STR(MODEL1_H0_1_PUMP)
#elif defined OBJET_MACHINE
         CASE_ID_RETURN_STR(SUPPORT_PUMP2)
         CASE_ID_RETURN_STR(SUPPORT_PUMP1)
         CASE_ID_RETURN_STR(MODEL1_H0_1_PUMP)
         CASE_ID_RETURN_STR(MODEL2_H0_1_PUMP)
         CASE_ID_RETURN_STR(MODEL3_H2_3_PUMP)
         CASE_ID_RETURN_STR(MODEL4_H2_3_PUMP)
         CASE_ID_RETURN_STR(MODEL5_H4_5_PUMP)
         CASE_ID_RETURN_STR(MODEL6_H4_5_PUMP)
         //CASE_ID_RETURN_STR(SUPPORT_PUMP_H4_5_PUMP)
         //CASE_ID_RETURN_STR(MODEL1_2_H2_3_PUMP)
#elif OBJET_TWO_TANK_MACHINE
         CASE_ID_RETURN_STR(SUPPORT_PUMP1)
         CASE_ID_RETURN_STR(MODEL_PUMP_1)
#else
         CASE_ID_RETURN_STR(SUPPORT_PUMP1)
         CASE_ID_RETURN_STR(SUPPORT_PUMP2)
         CASE_ID_RETURN_STR(MODEL_PUMP_1)
         CASE_ID_RETURN_STR(MODEL_PUMP_2)
#endif
		default:
			throw EQException("Invalid pump ID");
     }
};

// This relation is constant on Objet machines
TPumpIndex TankToPump(TTankIndex TankIndex)
{
	TPumpIndex ret;

	switch(TankIndex)
	{
		case TYPE_TANK_SUPPORT1: ret = SUPPORT_PUMP1; break;
		case TYPE_TANK_SUPPORT2: ret = SUPPORT_PUMP2; break;
		case TYPE_TANK_MODEL1:	 ret = PUMP_2; break;
		case TYPE_TANK_MODEL2:   ret = PUMP_3; break;
		case TYPE_TANK_MODEL3:   ret = PUMP_4; break;
		case TYPE_TANK_MODEL4:   ret = PUMP_5; break;
		case TYPE_TANK_MODEL5:   ret = PUMP_6; break;
		case TYPE_TANK_MODEL6:   ret = PUMP_7; break;
		
		default:
			throw EQException("Invalid tank ID");
    }
			
	return ret;
}

TTankIndex PumpToTank(TPumpIndex PumpIndex)
{
	TTankIndex ret;

	switch(PumpIndex)
	{
		case SUPPORT_PUMP1: ret = TYPE_TANK_SUPPORT1; break;
		case SUPPORT_PUMP2: ret = TYPE_TANK_SUPPORT2; break;
		case PUMP_2:	    ret = TYPE_TANK_MODEL1; break;
		case PUMP_3:        ret = TYPE_TANK_MODEL2; break;
		case PUMP_4:        ret = TYPE_TANK_MODEL3; break;
		case PUMP_5:        ret = TYPE_TANK_MODEL4; break;
		case PUMP_6:        ret = TYPE_TANK_MODEL5; break;
		case PUMP_7:        ret = TYPE_TANK_MODEL6; break;
		
		default:
			throw EQException("Invalid pump ID");
    }
			
	return ret;
}

#ifdef EDEN_250
const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS] =
                              {TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_MODEL1,TYPE_CHAMBER_MODEL1};
#elif defined OBJET_MACHINE
const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS] =
                             {TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_MODEL3,TYPE_CHAMBER_MODEL3,
                              TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL1, TYPE_CHAMBER_MODEL1};
#else
const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS] =
                             {TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_SUPPORT,
                              TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL1, TYPE_CHAMBER_MODEL1};
#endif

#if defined CONNEX_MACHINE || defined OBJET_MACHINE
  const int mapYOffset[] = {0,2,1,3,0,2,1,3};
#elif defined EDEN_250
  const int mapYOffset[] = {0,2,0,2,0,2,0,2};
#else
  const int mapYOffset[] = {0,1,2,3,0,1,2,3};
#endif

const int DPI[NUMBER_OF_QUALITY_MODES] = {1200, 600};
const int MICRON_IN_HEAD_Y  = (CM_PER_INCH / DPI_Y_PER_HEAD * NOZZLES_IN_HEAD) * MICRON_IN_CM; /*65024*/


float MilliGramsToGrams(int Weight)
{
	return ((float)Weight / 1000.0);
}

int GramsToMilliGrams(float Weight)
{
	return (Weight * 1000.0);
}

int NanoGramsToMilliGrams(unsigned int Weight)
{
	return (Weight / 1000000);
}
