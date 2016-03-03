#include "GlobalDefs.h"
#include "AppParams.h"

#ifndef PARAMS_EXP_IMP /*{*/

#include "QComponent.h"
#include "ModesDefs.h"
#include "FEResources.h"
#include "MaintenanceCountersDefs.h"
#include "AppLogFile.h"
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
//OBJET_MACHINE config
				 SUPPORT_1_PUMP_COUNTER_ID
				,SUPPORT_2_PUMP_COUNTER_ID
				,MODEL_1_PUMP_COUNTER_ID
				,MODEL_2_PUMP_COUNTER_ID
				,MODEL_3_PUMP_COUNTER_ID
				,MODEL_4_PUMP_COUNTER_ID
				,MODEL_5_PUMP_COUNTER_ID
				,MODEL_6_PUMP_COUNTER_ID
#ifdef OBJET_MACHINE_KESHET
				,MODEL_7_PUMP_COUNTER_ID
				,MODEL_8_PUMP_COUNTER_ID
				,MODEL_9_PUMP_COUNTER_ID
				,MODEL_10_PUMP_COUNTER_ID
				,MODEL_11_PUMP_COUNTER_ID
				,MODEL_12_PUMP_COUNTER_ID
				,MODEL_13_PUMP_COUNTER_ID
				,MODEL_14_PUMP_COUNTER_ID
#endif
};

extern const int HeadsCounterID[TOTAL_NUMBER_OF_HEADS] =
{
   HEAD_S0_COUNTER_ID
  ,HEAD_S1_COUNTER_ID
//OBJET_MACHINE config
  ,HEAD_M5_COUNTER_ID
  ,HEAD_M4_COUNTER_ID
  ,HEAD_M3_COUNTER_ID
  ,HEAD_M2_COUNTER_ID
  ,HEAD_M1_COUNTER_ID
  ,HEAD_M0_COUNTER_ID
};

extern const int HeadsCounter_DC_ID[TOTAL_NUMBER_OF_HEADS] =
{
   HEAD_S0_DC_COUNTER_ID
  ,HEAD_S1_DC_COUNTER_ID
  ,HEAD_M5_DC_COUNTER_ID
  ,HEAD_M4_DC_COUNTER_ID
  ,HEAD_M3_DC_COUNTER_ID
  ,HEAD_M2_DC_COUNTER_ID
  ,HEAD_M1_DC_COUNTER_ID
  ,HEAD_M0_DC_COUNTER_ID
};

extern const TActuatorIndex PumpsActuatorID[NUMBER_OF_PUMPS] =
{
//OBJET_MACHINE config
				 ACTUATOR_ID_SUPPORT_PUMP_1
				,ACTUATOR_ID_SUPPORT_PUMP_2
				,ACTUATOR_ID_MODEL1_PUMP
				,ACTUATOR_ID_MODEL2_PUMP
				,ACTUATOR_ID_MODEL3_PUMP
				,ACTUATOR_ID_MODEL4_PUMP
				,ACTUATOR_ID_MODEL5_PUMP
				,ACTUATOR_ID_MODEL6_PUMP
#ifdef OBJET_MACHINE_KESHET
				,ACTUATOR_ID_MODEL7_PUMP
				,ACTUATOR_ID_MODEL8_PUMP
				,ACTUATOR_ID_MODEL9_PUMP
				,ACTUATOR_ID_MODEL10_PUMP
				,ACTUATOR_ID_MODEL11_PUMP
				,ACTUATOR_ID_MODEL12_PUMP
				,ACTUATOR_ID_MODEL13_PUMP
				,ACTUATOR_ID_MODEL14_PUMP
#endif
};

extern const int ResinFilterCounterID[NUMBER_OF_PUMPS] =
{
//OBJET_MACHINE config
				 SUPPORT_1_RESIN_FILTER_COUNTER_ID
				,SUPPORT_2_RESIN_FILTER_COUNTER_ID
				,MODEL_1_RESIN_FILTER_COUNTER_ID
				,MODEL_2_RESIN_FILTER_COUNTER_ID
				,MODEL_3_RESIN_FILTER_COUNTER_ID
				,MODEL_4_RESIN_FILTER_COUNTER_ID
				,MODEL_5_RESIN_FILTER_COUNTER_ID
				,MODEL_6_RESIN_FILTER_COUNTER_ID
#ifdef OBJET_MACHINE_KESHET
				,MODEL_7_RESIN_FILTER_COUNTER_ID
				,MODEL_8_RESIN_FILTER_COUNTER_ID
				,MODEL_9_RESIN_FILTER_COUNTER_ID
				,MODEL_10_RESIN_FILTER_COUNTER_ID
				,MODEL_11_RESIN_FILTER_COUNTER_ID
				,MODEL_12_RESIN_FILTER_COUNTER_ID
				,MODEL_13_RESIN_FILTER_COUNTER_ID
				,MODEL_14_RESIN_FILTER_COUNTER_ID
#endif
};

extern const int SleepingChannelsCounterID[MAX_NUMBER_OF_MODEL_CHAMBERS-1] =
{
   CHANNEL_M1_SLEEPING_COUNTER_ID
  ,CHANNEL_M2_SLEEPING_COUNTER_ID
  ,CHANNEL_M3_SLEEPING_COUNTER_ID
  ,CHANNEL_M4_SLEEPING_COUNTER_ID
  ,CHANNEL_M5_SLEEPING_COUNTER_ID
  ,CHANNEL_M6_SLEEPING_COUNTER_ID
}; //size of (MAX_NUMBER_OF_MODEL_CHAMBERS-1) since MODEL7 is also support in DM6 and shouldn't be taken into account here

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
	{
		if(CAppParams::Instance())
		{
			if(CAppParams::Instance()->DualWasteEnabled)
				Str = LOAD_QSTRING_ARRAY(Tank,IDS_TANK_STRINGS_DUAL_WASTE);
			else
				Str = LOAD_QSTRING_ARRAY(Tank,IDS_TANK_STRINGS);
		}
		else
		{
			Str = LOAD_QSTRING_ARRAY(Tank,IDS_TANK_STRINGS);
		}
	}
	else if (Tank == NO_TANK)
	{
		Str = "'NO TANK'";
	}	
   	return Str;
}

/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
QString ChamberToStr(TChamberIndex Chamber, bool AsModeFunc, TOperationModeIndex Mode)
{
   QString Str;
   if(VALIDATE_CHAMBER_INCLUDING_WASTE(Chamber))
   {
#ifndef OBJET_MACHINE_KESHET
	 if (AsModeFunc && IS_MODEL_CHAMBER(Chamber) && Mode == SINGLE_MATERIAL_OPERATION_MODE)
		Str = LOAD_STRING(IDS_MODEL_STR);
	 else
#endif
        Str = LOAD_QSTRING_ARRAY(Chamber,IDS_CHAMBER_STRINGS);
   }
   return Str;
}
/*******************************************************************************************************************/
#pragma warn .8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

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
QString GetDoubleHeadName(int Head)
{
   QString Ret;
   if(VALIDATE_HEAD(Head))
	  Ret = LOAD_QSTRING_ARRAY(Head,IDS_DOUBLE_HEADS_STRINGS);
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
    int tom = CAppParams::Instance()->ThermistorsOperationMode;

    if (tom == LOW_THERMISTORS_MODE || tom == SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE)
        return true;

    if (tom == HIGH_AND_LOW_THERMISTORS_MODE || tom == HIGH_THERMISTORS_MODE || tom == SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE)
        return false;

    throw EQException(QFormatStr("Is_6_ModelHeads - Unrecognized ThermistorsOperationMode %d", tom));
//    return false;
}


TThermistorsOperationMode GetThermOpModeForHQHS(void)
{
    if (CAppParams::Instance()->HqHsUsesAllThermistors)
    {
        return HIGH_AND_LOW_THERMISTORS_MODE;
    }
    //else...
    return HIGH_THERMISTORS_MODE;
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
//OBJET_MACHINE config
   switch(Tank)
   {
   case TYPE_TANK_MODEL1:
	 ret = MODEL1_SEGMENT;
	 break;
   case TYPE_TANK_MODEL2:
	 ret = MODEL2_SEGMENT;
	 break;
   case TYPE_TANK_MODEL3:
	 ret = MODEL3_SEGMENT;
	 break;
   case TYPE_TANK_MODEL4:
	 ret = MODEL4_SEGMENT;
	 break;
   case TYPE_TANK_MODEL5:
	 ret = MODEL5_SEGMENT;
	 break;
   case TYPE_TANK_MODEL6:
	 ret = MODEL6_SEGMENT;
	 break;
   case TYPE_TANK_SUPPORT2:
     ret = S2_SEGMENT;
	 break;
   case TYPE_TANK_SUPPORT1:
	 ret = S1_SEGMENT;
	 break;
#ifdef OBJET_MACHINE_KESHET
	case TYPE_TANK_MODEL7:
	 ret = MODEL7_SEGMENT;
	 break;
   case TYPE_TANK_MODEL8:
	 ret = MODEL8_SEGMENT;
	 break;
   case TYPE_TANK_MODEL9:
	 ret = MODEL9_SEGMENT;
	 break;
   case TYPE_TANK_MODEL10:
	 ret = MODEL10_SEGMENT;
	 break;
   case TYPE_TANK_MODEL11:
	 ret = MODEL11_SEGMENT;
	 break;
   case TYPE_TANK_MODEL12:
	 ret = MODEL12_SEGMENT;
	 break;
   case TYPE_TANK_MODEL13:
	 ret = MODEL13_SEGMENT;
	 break;
   case TYPE_TANK_MODEL14:
	 ret = MODEL14_SEGMENT;
	 break;
#endif
   default:
	 break;
   }

   return ret;
}


//OBJET_MACHINE_KESHET_OPEN_ISSUE
TChamberIndex GetHeadsChamber(int/*THeadIndex*/ Head, int PrintingOperationMode, bool AsModeFunc)
{
   if (!VALIDATE_HEAD(Head))
      return TYPE_CHAMBER_SUPPORT;

   TChamberIndex ret = HeadToChamber[Head];
   //in single mode, each two logical heads use the same chamber (flooded)
   if (AsModeFunc==true && PrintingOperationMode==SINGLE_MATERIAL_OPERATION_MODE)
   {
      if(ret==TYPE_CHAMBER_MODEL2)
		  ret = TYPE_CHAMBER_MODEL1;
#ifdef OBJET_MACHINE_KESHET
	  if(ret==TYPE_CHAMBER_MODEL4)
		  ret = TYPE_CHAMBER_MODEL3;
	  if(ret==TYPE_CHAMBER_MODEL6)
		  ret = TYPE_CHAMBER_MODEL5;
	  if(ret==TYPE_CHAMBER_MODEL7)
		  ret = TYPE_CHAMBER_SUPPORT;
#else
	  else if(ret==TYPE_CHAMBER_MODEL3)
		  ret = TYPE_CHAMBER_SUPPORT;
#endif		  
   }
   return ret;
}


TChamberIndex Head2BmpIndex(int Head, int PrintingOperationMode, bool AsModeFunc)
{
   if (!VALIDATE_HEAD(Head))
   {
      CQLog::Write(LOG_TAG_GENERAL, "Error: invalid head ID %d, using support BMP", Head);
      return TYPE_CHAMBER_SUPPORT;
   }

   TChamberIndex ret = HeadToChamber[Head];
   //in single mode, each two logical heads use the same chamber (flooded)
   if (AsModeFunc && (SINGLE_MATERIAL_OPERATION_MODE == PrintingOperationMode))
   {
      if      (TYPE_CHAMBER_MODEL2 == ret)
      {
          ret = TYPE_CHAMBER_MODEL1;
      }
#ifdef OBJET_MACHINE_KESHET
/**** in DM3 the chamber number is meaningless and becomes a numeric index 0..3 ****/
      else if ((TYPE_CHAMBER_MODEL4 == ret) || (TYPE_CHAMBER_MODEL3 == ret))
      {
          ret = TYPE_CHAMBER_MODEL2;
      }
      else if ((TYPE_CHAMBER_MODEL6 == ret) || (TYPE_CHAMBER_MODEL5 == ret))
      {
          ret = TYPE_CHAMBER_MODEL3;
      }
      else if (TYPE_CHAMBER_MODEL7 == ret)
      {
          ret = TYPE_CHAMBER_SUPPORT;
      }
#else
      else if (TYPE_CHAMBER_MODEL3 == ret)
      {
          ret = TYPE_CHAMBER_SUPPORT;
      }
#endif
   }

   return ret;
}


TChamberIndex GetHeadsChamberForHSW(int/*THeadIndex*/ Head, int PrintingOperationMode, bool AsModeFunc)
{
   if (!VALIDATE_HEAD(Head))
      return TYPE_CHAMBER_SUPPORT;
   TChamberIndex ret = HeadToChamber[Head];
  if (AsModeFunc==true && PrintingOperationMode==SINGLE_MATERIAL_OPERATION_MODE)
   {
      if(ret==TYPE_CHAMBER_MODEL7)
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

#ifdef OBJET_MACHINE_KESHET
	case TYPE_CHAMBER_MODEL3:
		Ret = TYPE_CHAMBER_MODEL4;
		break;
	case TYPE_CHAMBER_MODEL4:
		Ret = TYPE_CHAMBER_MODEL3;
		break;
	case TYPE_CHAMBER_MODEL5:
		Ret = TYPE_CHAMBER_MODEL6;
		break;
	case TYPE_CHAMBER_MODEL6:
		Ret = TYPE_CHAMBER_MODEL5;
		break;
	case TYPE_CHAMBER_MODEL7:
		Ret = TYPE_CHAMBER_SUPPORT;
		break;
	case TYPE_CHAMBER_SUPPORT:
		Ret = TYPE_CHAMBER_MODEL7;
		break;
#else
	case TYPE_CHAMBER_MODEL3:
		Ret = TYPE_CHAMBER_SUPPORT;
		break;
	case TYPE_CHAMBER_SUPPORT:
		Ret = TYPE_CHAMBER_MODEL3;
		break;
#endif
	case TYPE_CHAMBER_WASTE:
		Ret = TYPE_CHAMBER_WASTE;
		break;
	default:
		//Problem fetching the sibling tank. Perhaps GetActiveTank is NO_TANK
		CQLog::Write(LOG_TAG_GENERAL ,"Invalid tank ID");
		Ret = (TChamberIndex)NO_TANK;
	}

	return Ret;
}

TTankIndex GetSiblingTank(TTankIndex TankIndex)
{
	TTankIndex Ret = NO_TANK;

	switch(TankIndex)
	{
		case TYPE_TANK_SUPPORT1: Ret = TYPE_TANK_SUPPORT2; break;
		case TYPE_TANK_SUPPORT2: Ret = TYPE_TANK_SUPPORT1; break;
		case TYPE_TANK_MODEL1:	 Ret = TYPE_TANK_MODEL2; break;
		case TYPE_TANK_MODEL2:   Ret = TYPE_TANK_MODEL1; break;
		case TYPE_TANK_MODEL3:   Ret = TYPE_TANK_MODEL4; break;
		case TYPE_TANK_MODEL4:   Ret = TYPE_TANK_MODEL3; break;
		case TYPE_TANK_MODEL5:   Ret = TYPE_TANK_MODEL6; break;
		case TYPE_TANK_MODEL6:   Ret = TYPE_TANK_MODEL5; break;
#ifdef OBJET_MACHINE_KESHET
		case TYPE_TANK_MODEL7:	 Ret = TYPE_TANK_MODEL8; break;
		case TYPE_TANK_MODEL8:   Ret = TYPE_TANK_MODEL7; break;
		case TYPE_TANK_MODEL9:   Ret = TYPE_TANK_MODEL10; break;
		case TYPE_TANK_MODEL10:   Ret = TYPE_TANK_MODEL9; break;
		case TYPE_TANK_MODEL11:	 Ret = TYPE_TANK_MODEL12; break;
		case TYPE_TANK_MODEL12:   Ret = TYPE_TANK_MODEL11; break;
		case TYPE_TANK_MODEL13:   Ret = TYPE_TANK_MODEL14; break;
		case TYPE_TANK_MODEL14:   Ret = TYPE_TANK_MODEL13; break;
#endif		
		//Caution: In case DualWaste is enabled, Waste Right is also associated with Type_Tank_Model6
		case TYPE_TANK_WASTE_LEFT:
		 if(CAppParams::Instance()->DualWasteEnabled)
		 {
		   Ret = TYPE_TANK_WASTE_RIGHT;
		 }
		 else
		 {
		   Ret =  TYPE_TANK_WASTE_LEFT;
		 }
		 break;
		case TYPE_TANK_WASTE_RIGHT:  
		  Ret =  TYPE_TANK_WASTE_LEFT;break;
		default:
			//Problem fetching the sibling chamber. Perhaps GetActiveTank is NO_TANK
			//CQLog::Write(LOG_TAG_GENERAL ,"Invalid tank ID");
			Ret = NO_TANK;
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
#ifdef OBJET_MACHINE_KESHET
	case TYPE_TANK_MODEL13:
	case TYPE_TANK_MODEL14:
#else
	case TYPE_TANK_MODEL5:
	case TYPE_TANK_MODEL6:	
#endif
		Ret = SUPPORT_MODEL_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL1:
	case TYPE_TANK_MODEL2:
	case TYPE_TANK_MODEL3:
	case TYPE_TANK_MODEL4:
		Ret = M1_M2_CHAMBER_THERMISTOR;
		break;
#ifdef OBJET_MACHINE_KESHET
	case TYPE_TANK_MODEL5:
	case TYPE_TANK_MODEL6:
	case TYPE_TANK_MODEL7:
	case TYPE_TANK_MODEL8:
		Ret = M3_M4_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL9:
	case TYPE_TANK_MODEL10:
	case TYPE_TANK_MODEL11:
	case TYPE_TANK_MODEL12:
		Ret = M5_M6_CHAMBER_THERMISTOR;
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
//OBJET_MACHINE config
	case TYPE_TANK_MODEL3:
	case TYPE_TANK_MODEL4:
		Ret = M2_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL5:
	case TYPE_TANK_MODEL6:
		Ret = M3_CHAMBER_THERMISTOR;
		break;
#ifdef OBJET_MACHINE_KESHET
	case TYPE_TANK_MODEL7:
	case TYPE_TANK_MODEL8:
		Ret = M4_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL9:
	case TYPE_TANK_MODEL10:
		Ret = M5_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL11:
	case TYPE_TANK_MODEL12:
		Ret = M6_CHAMBER_THERMISTOR;
		break;
	case TYPE_TANK_MODEL13:
	case TYPE_TANK_MODEL14:
		Ret = M7_CHAMBER_THERMISTOR;
		break;
#endif
	default:
		break;
	}
	
	return Ret;
}

bool IsTankInSupportBlockHalf(TTankIndex Tank)
{
    if(!VALIDATE_TANK(Tank))
		return false;

	switch(Tank)
	{
		case TYPE_TANK_SUPPORT1:
		case TYPE_TANK_SUPPORT2:
#ifdef OBJET_MACHINE_KESHET
		case TYPE_TANK_MODEL13:
		case TYPE_TANK_MODEL14:
#else
		case TYPE_TANK_MODEL5:
		case TYPE_TANK_MODEL6:
#endif	
			return true;
	}
   return false;
}

bool IsTankInModelBlockHalf(TTankIndex Tank)
{
	if(!VALIDATE_TANK(Tank))
		return false;
	return !IsTankInSupportBlockHalf(Tank);
}

// Returns the physical chamber connected to the given tank, REGARDLESS of current operation mode
//[@Assumption REDUNDANCY_MAX_NUM_TANKS_PER_CHAMBER == 2]
TChamberIndex TankToStaticChamber(TTankIndex Tank)
{
	// If ChamberTankRelationArr contains one connection ('1')
	// of a given single tank to multiple chambers -- choose the right (physical) chamber.
	// Check the default chamber index: (Tank index / 2)  (equals to Tank >> 1) 
#ifdef OBJET_MACHINE_KESHET
	if (ChamberTankRelationArr[S_M1_M7_ACTIVE_TANKS_MODE][(Tank >> 1) + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
#else
	if (ChamberTankRelationArr[S_M1_M2_M3_ACTIVE_TANKS_MODE][(Tank >> 1) + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
#endif
	return static_cast<TChamberIndex>(Tank >> 1);

	
   for(int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; ++i)
#ifdef OBJET_MACHINE_KESHET
	 if (ChamberTankRelationArr[S_M1_M7_ACTIVE_TANKS_MODE][i + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
#else
	 if (ChamberTankRelationArr[S_M1_M2_M3_ACTIVE_TANKS_MODE][i + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
#endif
		 return static_cast<TChamberIndex>(i);

   //There is a tank that is not connected physically to any chamber
   throw EInvalidResult("Internal error: ChamberTankRelationArr is not defined correctly");
}

/* Warning! Read Container's CContainerBase::UpdateChambers info first. */
TChamberIndex TankToChamber(TTankIndex Tank)
{
   CAppParams *ParamManager = CAppParams::Instance();
   for(int i = FIRST_CHAMBER_TYPE_INCLUDING_WASTE; i < LAST_CHAMBER_TYPE_INCLUDING_WASTE; ++i)
	  if (ParamManager->ChamberTankRelation[i + (Tank*NUMBER_OF_CHAMBERS_INCLUDING_WASTE)])
		 return static_cast<TChamberIndex>(i);

   //If the tank is not active/ unrelated to all chambers, then ANY index result will be bad
   return NUMBER_OF_CHAMBERS;
}

TSegmentIndex TankToShortSegment(TTankIndex Tank)
{
	return static_cast<TSegmentIndex>(Tank);
}

TSegmentIndex ChamberToLongSegment(TChamberIndex Chamber)
{
	if (VALIDATE_CHAMBER_MODEL(Chamber))
		return static_cast<TSegmentIndex>((int)(FIRST_SHARED_SEGMENT + (int)Chamber) -1);
	else
		return NUMBER_OF_SEGMENTS;
}

bool IsChamberFlooded(TChamberIndex ChamberIndex)
{
	if(ChamberIndex == NUMBER_OF_CHAMBERS)
		return true;

	//There cannot be a chamber that is not active and is not flooded. Hence -
	CAppParams *ParamManager = CAppParams::Instance();

	if (Chamber2TankOperationModeRelation[ParamManager->TanksOperationMode][ChamberIndex] == 1)
		return false;
	else
		return true;
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

int	GetHeadsNumPerMaterial()
{
	//Get the number of heads that print the same material
#ifdef OBJET_MACHINE_KESHET
	return (SINGLE_MATERIAL_OPERATION_MODE == CAppParams::Instance()->PrintingOperationMode) ? 2 : 1;
#else
	return (SINGLE_MATERIAL_OPERATION_MODE == CAppParams::Instance()->PrintingOperationMode) ? 4 : 2;
#endif
}

//OBJET_MACHINE 
int GetActiveChambersNum()
{
  int ret = NUMBER_OF_CHAMBERS;
  switch(CAppParams::Instance()->PrintingOperationMode)
  {
      case SINGLE_MATERIAL_OPERATION_MODE:
           ret = NUM_OF_SM_CHAMBERS_THERMISTORS;//2 in Triplex, 4 in Keshet
           break;
	  case DIGITAL_MATERIAL_OPERATION_MODE:
		   ret = NUM_OF_DM_CHAMBERS_THERMISTORS;//4 in Triplex, 8 in Keshet
           break;
  }
  return ret;
}

TTankIndex GetSpecificTank(TChamberIndex Chamber, int Num)
{
	int occurence = Num;
	
	for (int t = FIRST_TANK_TYPE; t < LAST_TANK_TYPE; t++)
	{
#ifdef OBJET_MACHINE_KESHET
	   if (ChamberTankRelationArr[S_M1_M7_ACTIVE_TANKS_MODE][Chamber + t*NUMBER_OF_CHAMBERS_INCLUDING_WASTE] == true)
#else
	   if (ChamberTankRelationArr[S_M1_M2_M3_ACTIVE_TANKS_MODE][Chamber + t*NUMBER_OF_CHAMBERS_INCLUDING_WASTE] == true)
#endif
	   {
			--occurence;
			if (0 <= occurence) return (TTankIndex)t;
	   }
	}
	return NO_TANK;
}


char * GetTankCleanserLabel(TTankIndex aTank)
{
    if ((aTank < FIRST_TANK_TYPE) || (aTank >= LAST_TANK_TYPE))
    {
        return NULL;
    }

    return (IS_MODEL_TANK(aTank) ? MODEL_CLEANSER : SUPPORT_CLEANSER);
}


//called only from MRW
/*int OperationModeToActiveTankNum(int OperationMode)
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
}*/

int OperationModeToBitmapsNums(int OperationMode)
{
  int ret = 0;
  switch(OperationMode)
  {
      case SINGLE_MATERIAL_OPERATION_MODE:
           ret = NUM_OF_SM_CHAMBERS_THERMISTORS;//2 in Triplex, 4 in Keshet
           break;
	  case DIGITAL_MATERIAL_OPERATION_MODE:
		   ret = NUM_OF_DM_CHAMBERS_THERMISTORS;//4 in Triplex, 8 in Keshet
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
      case HIGH_THERMISTORS_MODE:
      case HIGH_AND_LOW_THERMISTORS_MODE:
           ret = (NUM_OF_SM_CHAMBERS_THERMISTORS-MAX_NUMBER_OF_SUPPORT_CHAMBERS);//1 in Triplex, 3 in Keshet
           break;
      case LOW_THERMISTORS_MODE:
           ret = (NUM_OF_DM_CHAMBERS_THERMISTORS-MAX_NUMBER_OF_SUPPORT_CHAMBERS);//3 in Triplex, 7 in Keshet
           break;
      case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE:  //currently, relevant only for Triplex
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
   INIT_METHOD(CAppGlobalDefs,GetTankStr);
   INIT_METHOD(CAppGlobalDefs,GetChamberActivePumpID);
   INIT_METHOD(CAppGlobalDefs,GetChamberActivePumpActuatorID);
   INIT_METHOD(CAppGlobalDefs,GetFirstQualityMode);
   INIT_METHOD(CAppGlobalDefs,GetLastQualityMode);
   INIT_METHOD(CAppGlobalDefs,GetFirstUserLevelOperationMode);
   INIT_METHOD(CAppGlobalDefs,GetLastUserLevelOperationMode);
   INIT_METHOD(CAppGlobalDefs,GetAppModeAccessibility);
   INIT_METHOD(CAppGlobalDefs,GetModeDirStr);
   INIT_METHOD(CAppGlobalDefs,GetModeName);
   INIT_METHOD(CAppGlobalDefs,GetFirstThermistor);
   INIT_METHOD(CAppGlobalDefs,GetLastThermistor);

   AllModesNames        [HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "High Speed";
   AllModesNames        [HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "High Mix";
   AllModesShortNames   [HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "HS";
   AllModesShortNames   [HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "HM";
   AllModesAccessibility[HS_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = true;
   AllModesAccessibility[HS_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = true;

   AllModesNames        [HQ_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "TEMP";
   AllModesNames        [HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "High Quality";
   AllModesShortNames   [HQ_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = "TEMP";
   AllModesShortNames   [HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = "HQ";
   AllModesAccessibility[HQ_INDEX][SINGLE_MATERIAL_OPERATION_MODE] = false;
   AllModesAccessibility[HQ_INDEX][DIGITAL_MATERIAL_OPERATION_MODE] = true;
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
int CAppGlobalDefs::GetFirstThermistor()
{
   return (int)SUPPORT_CHAMBER_THERMISTOR;
}
int CAppGlobalDefs::GetLastThermistor()
{
   return (int)NUM_OF_CHAMBERS_THERMISTORS;
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

QString CAppGlobalDefs::GetTankStr(int Tank)  //luda
{
   return TankToStr((TTankIndex)Tank);
}
 QString CAppGlobalDefs::GetChamberThermistorToStr(int ChamberThermistor)
{
	return  ChamberThermistorToStr(ChamberThermistor);
}

TPumpIndex CAppGlobalDefs::GetChamberActivePumpID(int /*TChamberIndex*/ Chamber)
{
	TPumpIndex ret = static_cast<TPumpIndex>(0);
	
   int tank = CAppParams::Instance()->ActiveTanks[Chamber];
   switch(Chamber)
   {
       case TYPE_CHAMBER_SUPPORT:
			ret = ((TTankIndex)tank == TYPE_TANK_SUPPORT1) ? SUPPORT_PUMP1 : SUPPORT_PUMP2;
            break;
//OBJET_MACHINE config
       case TYPE_CHAMBER_MODEL1:
			ret = ((TTankIndex)tank == TYPE_TANK_MODEL1) ? MODEL1_PUMP : MODEL2_PUMP;
            break;
       case TYPE_CHAMBER_MODEL2:
			ret = ((TTankIndex)tank == TYPE_TANK_MODEL3) ? MODEL3_PUMP : MODEL4_PUMP;
            break;
       case TYPE_CHAMBER_MODEL3:
            ret = ((TTankIndex)tank == TYPE_TANK_MODEL5) ? MODEL5_PUMP : MODEL6_PUMP;
			break;
#ifdef OBJET_MACHINE_KESHET
	   case TYPE_CHAMBER_MODEL4:
			ret = ((TTankIndex)tank == TYPE_TANK_MODEL7) ? MODEL7_PUMP : MODEL8_PUMP;
			break;
	   case TYPE_CHAMBER_MODEL5:
			ret = ((TTankIndex)tank == TYPE_TANK_MODEL9) ? MODEL9_PUMP : MODEL10_PUMP;
			break;
	   case TYPE_CHAMBER_MODEL6:
			ret = ((TTankIndex)tank == TYPE_TANK_MODEL11) ? MODEL11_PUMP : MODEL12_PUMP;
			break;
	   case TYPE_CHAMBER_MODEL7:
			ret = ((TTankIndex)tank == TYPE_TANK_MODEL13) ? MODEL13_PUMP : MODEL14_PUMP;
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
//OBJET_MACHINE config
         CASE_ID_RETURN_STR(SUPPORT_PUMP2)
         CASE_ID_RETURN_STR(SUPPORT_PUMP1)
		 CASE_ID_RETURN_STR(MODEL1_PUMP)
		 CASE_ID_RETURN_STR(MODEL2_PUMP)
		 CASE_ID_RETURN_STR(MODEL3_PUMP)
		 CASE_ID_RETURN_STR(MODEL4_PUMP)
		 CASE_ID_RETURN_STR(MODEL5_PUMP)
		 CASE_ID_RETURN_STR(MODEL6_PUMP)
#ifdef OBJET_MACHINE_KESHET
		 CASE_ID_RETURN_STR(MODEL7_PUMP)
		 CASE_ID_RETURN_STR(MODEL8_PUMP)
		 CASE_ID_RETURN_STR(MODEL9_PUMP)
		 CASE_ID_RETURN_STR(MODEL10_PUMP)
		 CASE_ID_RETURN_STR(MODEL11_PUMP)
		 CASE_ID_RETURN_STR(MODEL12_PUMP)
		 CASE_ID_RETURN_STR(MODEL13_PUMP)
		 CASE_ID_RETURN_STR(MODEL14_PUMP)
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
		case TYPE_TANK_MODEL1:	 ret = MODEL1_PUMP; break;
		case TYPE_TANK_MODEL2:   ret = MODEL2_PUMP; break;
		case TYPE_TANK_MODEL3:   ret = MODEL3_PUMP; break;
		case TYPE_TANK_MODEL4:   ret = MODEL4_PUMP; break;
		case TYPE_TANK_MODEL5:   ret = MODEL5_PUMP; break;
		case TYPE_TANK_MODEL6:   ret = MODEL6_PUMP; break;
#ifdef OBJET_MACHINE_KESHET
		case TYPE_TANK_MODEL7:	  ret = MODEL7_PUMP; break;
		case TYPE_TANK_MODEL8:    ret = MODEL8_PUMP; break;
		case TYPE_TANK_MODEL9:    ret = MODEL9_PUMP; break;
		case TYPE_TANK_MODEL10:   ret = MODEL10_PUMP; break;
		case TYPE_TANK_MODEL11:   ret = MODEL11_PUMP; break;
		case TYPE_TANK_MODEL12:   ret = MODEL12_PUMP; break;
		case TYPE_TANK_MODEL13:   ret = MODEL13_PUMP; break;
		case TYPE_TANK_MODEL14:   ret = MODEL14_PUMP; break;
#endif
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
		case MODEL1_PUMP:	    ret = TYPE_TANK_MODEL1; break;
		case MODEL2_PUMP:        ret = TYPE_TANK_MODEL2; break;
		case MODEL3_PUMP:        ret = TYPE_TANK_MODEL3; break;
		case MODEL4_PUMP:        ret = TYPE_TANK_MODEL4; break;
		case MODEL5_PUMP:        ret = TYPE_TANK_MODEL5; break;
		case MODEL6_PUMP:        ret = TYPE_TANK_MODEL6; break;
#ifdef OBJET_MACHINE_KESHET
		case MODEL7_PUMP:	    ret = TYPE_TANK_MODEL7; break;
		case MODEL8_PUMP:        ret = TYPE_TANK_MODEL8; break;
		case MODEL9_PUMP:        ret = TYPE_TANK_MODEL9; break;
		case MODEL10_PUMP:        ret = TYPE_TANK_MODEL10; break;
		case MODEL11_PUMP:        ret = TYPE_TANK_MODEL11; break;
		case MODEL12_PUMP:        ret = TYPE_TANK_MODEL12; break;
		case MODEL13_PUMP:        ret = TYPE_TANK_MODEL13; break;
		case MODEL14_PUMP:        ret = TYPE_TANK_MODEL14; break;
#endif		
		default:
			throw EQException("Invalid pump ID");
    }
			
	return ret;
}

//OBJET_MACHINE config
#ifdef OBJET_MACHINE_KESHET
const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS] =
							 {TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_MODEL7,TYPE_CHAMBER_MODEL6,TYPE_CHAMBER_MODEL5,
							  TYPE_CHAMBER_MODEL4, TYPE_CHAMBER_MODEL3, TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL1};
#else
const TChamberIndex HeadToChamber[TOTAL_NUMBER_OF_HEADS] =
							 {TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_SUPPORT,TYPE_CHAMBER_MODEL3,TYPE_CHAMBER_MODEL3,
							  TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL2, TYPE_CHAMBER_MODEL1, TYPE_CHAMBER_MODEL1};
#endif


//OBJET_MACHINE config
//distance between nozzles in Y in 300DPI resolution
#ifdef OBJET_MACHINE_KESHET
const int mapYOffset[] = {0,2,0,2,0,2,0,2};
#else
const int mapYOffset[] = {0,4,2,6,0,4,2,6};
#endif

// Must not be used in Keshet, due to different resolutions !!!!!
const int DPI[NUMBER_OF_QUALITY_MODES] = {1200, 600};

const int MICRON_IN_HEAD_Y  = (CM_PER_INCH / DPI_Y_PER_HEAD * NOZZLES_PER_HEAD) * MICRON_IN_CM; /*65024*/
