/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Application parameters.                                  *
 * Module Description: This class implement a parameters container  *
 *                     specific for the Q2RT application.           *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 22/08/2001                                           *
 * Last update: 25/09/2003                                           *
 ********************************************************************/

#include "AppLogFile.h"
#include "AppParams.h"
#include "ModesManager.h"
#include "QErrors.h"
#include "QFileWithChecksum.h"
#include "QMonitor.h"
#include "QVersionInfo.h"
#include "FrontEnd.h"
#include "MaintenanceCountersDefs.h"
#include "MaintenanceCounters.h"
#include "CSaveTransaction.h"
#include <algorithm>

#ifndef PARAMS_EXP_IMP
#include "FEResources.h"
#else
#define LOAD_STRING(resid)    ""
#define WIZARD_TITLE(_title_) ""
#endif

const char RECOVER_FILE_SIGNATURE[] = "SFPR";

// Machine configuration file names
QString APP_CFG_FILE_NAME = LOAD_STRING(IDS_APP_CFG_FILE_NAME);
QString RECOVER_FILE_NAME = LOAD_STRING(IDS_RECOVER_FILE_NAME);

// Structure for recover binary file
typedef struct {
  ULONG Signature;   // Must be 'SFPR'
  ULONG SliceNum;
  long ZPosition;
  float ModelHeight;
  int LastSliceSizeType;

} TRecoverFileStruct;

// Pointer to the singleton instance (static)
CAppParams *CAppParams::m_AppParamsInstance = NULL;


// Constructor - Initialize all the app parameters using the parameters manager
//               predefined macros (e.g. INIT_PARAM).
CAppParams::CAppParams(const QString CfgFileName, TInitStatus_E &StatusInit, bool DoRegister, QString Name) : CQParamsContainer(NULL,Name,DoRegister)
{
	m_CfgFileName = CfgFileName;
	m_UseDefaults = false;

	// Don't allow make any 'Savings' at this stage.
	m_EnableSaveEvents = false;
	// Create parameters stream
	m_ParamsStream = new CQParamsFileWithChecksumStream(m_CfgFileName, StatusInit);

	InitParams();

	// Assign the stream to the container
	AssignParamsStream(m_ParamsStream);

}

// Destructor
CAppParams::~CAppParams(void)
{
  delete m_ParamsStream;
}

// Return current layer height (dependent on the current X DPI)
float CAppParams::GetLayerHeight_um(int PrintDPI)
{
  float LayerHeight = (NozzleTest ? NozzlesTestdpiThickness_um : LayerHeightDPI_um);
  return LayerHeight;
}

// Factory functions
bool CAppParams::Init(const QString AppFilePath)
{
	TInitStatus_E StatusInit = INIT_FAIL_E; //init
	bool InitOK 			 = false;

	// Avoid allocating an instance if already allocated
	if(!m_AppParamsInstance)
	{
		// Remember the application path
		m_AppParamsInstance = new CAppParams(QConvertPath(AppFilePath + APP_CFG_FILE_NAME), StatusInit);


		//Handling the status of the constructor
		switch(StatusInit)
		{

			case  BOTH_FILES_ARE_MISSING_E:
			{

				QString str;
				str = "The files "+ QConvertPath(AppFilePath + APP_CFG_FILE_NAME) +" and " + QConvertPath(AppFilePath + APP_CFG_FILE_NAME) +".bak are either missing or empty.";


				str +=
					 "\n\r Without these files the application will not work properly. \n\r" \
					 "The Application will abort now.   Contact your Objet service provider.";

				QMonitor.ErrorMessageWaitOk(str);
			
				/* Enable this comment in order to create new q2rt.cfg with default parameters.
				str +=
					 "\n\r Press 'OK' to create the current file with default parameters,this action will run the application with default configuration.\n\r" \
					 "Press 'Cancel' to abort the program; Contact your Objet service provider.";

				// The backup file is also invalid, ask the user what to do
				if(QMonitor.AskOKCancel(str))
				{
					//TQErrCode saveAllParameters(CAppParams *); // This opens a backdoor to SaveAll function. See comments in CAppParams declaration
					saveAllParameters(CAppParams::Instance());

					StatusInit = INIT_OK_E;
					InitOK     = true;
					CQLog::Write(LOG_TAG_GENERAL,"New q2rt.cfg was created with defualt parameters!!!");
				}
				else
				{
					StatusInit = INIT_FAIL_E;
					InitOK     = false;
					CQLog::Write(LOG_TAG_GENERAL,"The user chose not to create New q2rt.cfg defualt parameters, the application will shut down!!!");
				}
			*/
			break;
			}
			case  BOTH_FILES_ARE_CORRUPTED_E:
			{

				QString str;
				str = "The files "+ QConvertPath(AppFilePath + APP_CFG_FILE_NAME) +" and " + QConvertPath(AppFilePath + APP_CFG_FILE_NAME) +".bak are both corrupted.";


				str +=
					 "\n\r Without these files the application will not work properly. \n\r" \
					 "The Application will abort now.   Contact your Objet service provider.";

				QMonitor.ErrorMessageWaitOk(str);

			break;
			}
			default:
			{
				StatusInit = INIT_OK_E;
				InitOK = true;
			break;
			}
		}


		if( InitOK ) {
			m_AppParamsInstance->LoadAll();
			m_AppParamsInstance->m_AppFilePath = AppFilePath;
		}
	}

	return InitOK;
}

void CAppParams::DeInit(void)
{
  // Deallocate instance
  if(m_AppParamsInstance)
  {
    delete m_AppParamsInstance;
    m_AppParamsInstance = NULL;
  }
}

// Get a pointer to the singleton instance
CAppParams *CAppParams::Instance(void)
{
  return m_AppParamsInstance;
}

void CAppParams::ParamErrorsCallback(TFileParamsStreamError Error,TGenericCockie /*Cockie*/)
{
  static const char *ErrorMessages[] = {"Params file \"open for write\" error",
                                        "Params file \"open for read\" error",
                                        "Param write error",
                                        "Params file close after write error",
                                        "Params file close after read error",
                                        "Params file seek error",
                                        "Recover file read error"};

  QMonitor.ErrorMessage(ErrorMessages[(int)Error]);
}

// Save recover parameters to a binary file
void CAppParams::SaveRecoverBinFile()
{
  ULONG Signature;
  CSaveTransaction ST;
  TRecoverFileStruct RecoverFileStruct;

  ST.LoadFiles();
  FILE * file = ST.GetBackupFile();
  if(file!=NULL)
  {
	  RecoverFileStruct.SliceNum = SliceNumber;
	  RecoverFileStruct.ZPosition = LastSliceZPosition;
	  RecoverFileStruct.ModelHeight = ModelHeight.Value();
	  RecoverFileStruct.LastSliceSizeType = LastSliceSizeType.Value();
	  fprintf(file,"%u %d %f %d",RecoverFileStruct.SliceNum,RecoverFileStruct.ZPosition,RecoverFileStruct.ModelHeight,RecoverFileStruct.LastSliceSizeType);
	  ST.Commit();
  }
}

// Load recover parameters from binary file
void CAppParams::LoadRecoverBinFile()
{
  ULONG Signature;
  TRecoverFileStruct RecoverFileStruct;
  CSaveTransaction ST;
  
  try
  {
	ST.CreateFiles();
	ST.LoadFiles();
	FILE * file = ST.GetDataFile();
	if(file!=NULL)
	{
		fscanf(file,"%u %d %f %d",&RecoverFileStruct.SliceNum,&RecoverFileStruct.ZPosition,&RecoverFileStruct.ModelHeight,&RecoverFileStruct.LastSliceSizeType);
		SliceNumber = RecoverFileStruct.SliceNum;
		LastSliceZPosition = RecoverFileStruct.ZPosition;
		ModelHeight = RecoverFileStruct.ModelHeight;
		LastSliceSizeType = RecoverFileStruct.LastSliceSizeType;
	}
  }
  catch(EQException& Err)
  {
	QMonitor.ErrorMessage(Err.GetErrorMsg());
  }
}
/*
// Save recover parameters to a binary file
void CAppParams::SaveRecoverBinFile(void)
{
  TRecoverFileStruct RecoverFileStruct;

  CQFileWithCheckSum FileWithChecksum(QConvertPath(m_AppFilePath + RECOVER_FILE_NAME));

  memcpy(&(RecoverFileStruct.Signature),RECOVER_FILE_SIGNATURE,sizeof(RecoverFileStruct.Signature));
  RecoverFileStruct.SliceNum = SliceNumber;
  RecoverFileStruct.ZPosition = LastSliceZPosition;
  RecoverFileStruct.ModelHeight = ModelHeight.Value();
  RecoverFileStruct.LastSliceSizeType = LastSliceSizeType.Value();

  try
  {
    FileWithChecksum.BeginWrite();
    FileWithChecksum.Write((BYTE*)&RecoverFileStruct,sizeof(RecoverFileStruct));
    FileWithChecksum.EndWrite();

  } catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }
}

// Load recover parameters from binary file
void CAppParams::LoadRecoverBinFile(void)
{
  CQFileWithCheckSum FileWithChecksum(QConvertPath(m_AppFilePath + RECOVER_FILE_NAME));
  TRecoverFileStruct RecoverFileStruct;

  try
  {
    if(FileWithChecksum.BeginRead() == Q_NO_ERROR)
    {
      FileWithChecksum.Read((BYTE *)&RecoverFileStruct,sizeof(RecoverFileStruct));
      FileWithChecksum.EndRead();

      // Assign to parameters
	  SliceNumber = RecoverFileStruct.SliceNum;
	  LastSliceZPosition = RecoverFileStruct.ZPosition;
	  ModelHeight = RecoverFileStruct.ModelHeight;
	  LastSliceSizeType = RecoverFileStruct.LastSliceSizeType;
    }
  } catch(EQException& Err)
    {
      QMonitor.ErrorMessage(Err.GetErrorMsg());
    }
}*/

bool CAppParams::IsPrintingAllowed(QString& Msg)
{
  Msg = "";
  if (MRW_WithinWizard)
     Msg = QFormatStr(LOAD_STRING(IDS_NOT_COMPLETED), WIZARD_TITLE(IDS_RR_WIZARD), "Ok", WIZARD_TITLE(IDS_RR_WIZARD));
  return !(MRW_WithinWizard);
}

void CAppParams::ChamberTankModeChangeObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
	//Instance->ChamberTankRelation.DisableObservers();
	CAppParams *Instance = reinterpret_cast<CAppParams *>(Cockie);
	for(int i=0;i<NUMBER_OF_CHAMBERS_INCLUDING_WASTE*TOTAL_NUMBER_OF_CONTAINERS_INCLUDING_WASTE;i++)
		Instance->ChamberTankRelation[i] = ChamberTankRelationArr[Instance->TanksOperationMode][i];
	//Instance->ChamberTankRelation.EnableObservers();
}

void CAppParams::ChamberTankModeSaveObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
    CAppParams *Instance = reinterpret_cast<CAppParams *>(Cockie);
	Instance->SaveSingleParameter(&Instance->ChamberTankRelation);
}
#ifndef PARAMS_EXP_IMP /*{*/
void CAppParams::ThermistorsModeChangeObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
	CAppParams *Instance = reinterpret_cast<CAppParams *>(Cockie);

	if(Instance->ThermistorsOperationMode > SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE)
		return;

	for(int i = 0; i < NUM_OF_CHAMBERS_THERMISTORS; i++)
		Instance->ActiveThermistors[i] = 0;

	switch(Instance->ThermistorsOperationMode)
	{
		case HIGH_THERMISTORS_MODE:
		{
			Instance->ActiveThermistors[SUPPORT_M3_CHAMBER_THERMISTOR] = 1;
		    Instance->ActiveThermistors[M1_M2_CHAMBER_THERMISTOR]      = 1;
			break;
		}

		case LOW_THERMISTORS_MODE:
		{
			Instance->ActiveThermistors[SUPPORT_CHAMBER_THERMISTOR] = 1;
			Instance->ActiveThermistors[M1_CHAMBER_THERMISTOR]      = 1;
			Instance->ActiveThermistors[M2_CHAMBER_THERMISTOR]      = 1;
			Instance->ActiveThermistors[M3_CHAMBER_THERMISTOR]      = 1;
			break;
		}

		case SUPPORT_HIGH_MODEL_LOW_THERMISTORS_MODE:
		{
			Instance->ActiveThermistors[SUPPORT_M3_CHAMBER_THERMISTOR] = 1;
			Instance->ActiveThermistors[M1_CHAMBER_THERMISTOR]         = 1;
			Instance->ActiveThermistors[M2_CHAMBER_THERMISTOR]         = 1;
			break;
		}

		case SUPPORT_LOW_MODEL_HIGH_THERMISTORS_MODE:
		{
			Instance->ActiveThermistors[SUPPORT_CHAMBER_THERMISTOR] = 1;
			Instance->ActiveThermistors[M1_M2_CHAMBER_THERMISTOR]   = 1;
			Instance->ActiveThermistors[M3_CHAMBER_THERMISTOR]      = 1;
			break;
        }
	}
	Instance->SaveSingleParameter(&Instance->ActiveThermistors);
	FrontEndInterface->UpdateStatus(FE_HEADS_OPERATION_MODE,0,true);
	CQLog::Write(LOG_TAG_GENERAL,QFormatStr("ThermistorsOperationMode value is: %s",Instance->ThermistorsOperationMode.GetValueStringList()[Instance->ThermistorsOperationMode].c_str()));
}
#endif
#if defined EDEN_500 || defined EDEN_350 || defined EDEN_350_V || defined CONNEX_MACHINE || defined OBJET_MACHINE //itamar objet check!!!
void CAppParams::TrayEndPositionChangeObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
    CAppParams *Instance = reinterpret_cast<CAppParams *>(Cockie);
    Instance->ZAxisEndPrintPosition = Instance->Z_StartPrintPosition.Value();
}

void CAppParams::TrayEndPositionSaveObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
    CAppParams *Instance = reinterpret_cast<CAppParams *>(Cockie);
    Instance->SaveSingleParameter(&Instance->ZAxisEndPrintPosition);
}
#endif

// Return the Y steps per pixel according to the current Y resolution
float CAppParams::GetYStepsPerPixel(void)
{
  return (YStepsPerPixel * 300.0f / DPI_InYAxis);
}

void CAppParams::BeforeSaveEvent()
{
}

void CAppParams::AfterSaveEvent(CQParameterBase *ParamToSave)
{
#ifndef PARAMS_EXP_IMP
  if (m_EnableSaveEvents)
  {
    if (ParamToSave)
        CModesManager::Instance()->SaveSingleParameter(ParamToSave);
    else
        CModesManager::Instance()->Save();
  }
#endif
}


// Get the Config File path
QString CAppParams::GetConfigPath(void)
{
  return QConvertPath(m_AppFilePath + APP_CFG_FILE_NAME);
}

// Turn on or off the operation of the "save" event callbacks (return last state)
void CAppParams::EnableDisableSaveEvents(bool Enable)
{
  m_EnableSaveEvents = Enable;
}

void CAppParams::SortProperties(void)
{
  TQStringVector       PropertyNames;
  TPropertiesContainer OldProperties;
  for(TPropertiesContainer::iterator i = Properties.begin(); i != Properties.end(); i++)
  {
     PropertyNames.push_back(QStrToUpper((*i)->Name()));
     OldProperties.push_back(*i);
  }
  sort(PropertyNames.begin(),PropertyNames.end());
  Properties.clear();

  for(TQStringVectorIter name = PropertyNames.begin(); name != PropertyNames.end(); name++)
      for(TPropertiesContainer::iterator i = OldProperties.begin(); i != OldProperties.end(); i++)
          if(QStrToUpper((*i)->Name()) == *name)
             Properties.push_back(*i);
}

void CAppParams::PMIntervalObserver(CQParameterBase *Param,TGenericCockie Cockie)
{
  CAppParams *Instance = reinterpret_cast<CAppParams *>(Cockie);

  // change the warning of the maintenance counter of the PM according to the new value of PMIntervalHours
  int PMInterval = Instance->PMIntervalHours;
  int PMWarning = Instance->NextPMTypeIsSmall ? PMInterval : ( PMInterval * 2 );

  CMaintenanceCounters* MC = CMaintenanceCounters::GetInstance();
  MC->SetWarningTimeInSecs(SERVICE_COUNTER_ID, PMWarning * 3600);
  MC->SetUserWarning(SERVICE_COUNTER_ID, PMWarning * 3600);
  MC->SaveToFile();
}
