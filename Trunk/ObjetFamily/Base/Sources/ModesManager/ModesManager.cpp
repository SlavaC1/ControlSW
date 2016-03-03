/*******************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: Modes Manager                                            *
 * Module Description: Machine modes management.                    *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 10/03/2003                                           *
 * Last upate: 29/05/2003                                           *
 ********************************************************************/

#include <algorithm>
#include "ModesManager.h"
#include "QErrors.h"
#include "QFileSystem.h"
#include "QParamsContainer.h"
#include "QMonitor.h"
#include "Q2RTApplication.h"
#include "MiniSequencer.h"
#include "FrontEnd.h"
#include "AppLogFile.h"
#include "AppParams.h"
#include "ModesDefs.h"
#include "Q2RTErrors.h"
#include "QFileParamsStream.h"
#include "QPythonIntegrator.h"
#include "GlobalDefs.h"


const QString MODEL_DEFAULT_MATERIALS_MODES_DIR   = "ModelDefaultMaterials";
const QString SUPPORT_DEFAULT_MATERIALS_MODES_DIR = "SupportDefaultMaterials";

const QString MODEL_SERVICE_MATERIALS_MODES_DIR   = "ModelServiceMaterials";
const QString SUPPORT_SERVICE_MATERIALS_MODES_DIR = "SupportServiceMaterials";

const QString DEFAULT_MODES_DIR        = "DefaultModes";
const QString DEFAULT_MODE             = "DefaultMode";
const QString DEFAULT_MATERIALS_MODE   = "~DefaultMaterialsMode";
const QString PER_MACHINE_MODE_DEFAULT = "~PerMachineDefault";

QString WEIGHT_TEST       [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
QString CALIBRATE_FOR_HSW [NUMBER_OF_QUALITY_MODES];
QString PRINT_MODE        [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
QString PER_MACHINE_MODE  [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
QString HSW_WEIGHT_TEST   [NUMBER_OF_QUALITY_MODES];

QString MODEL_MATERIALS_MODES_DIR   [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];
QString SUPPORT_MATERIALS_MODES_DIR [NUMBER_OF_QUALITY_MODES][NUM_OF_OPERATION_MODES];

QString ChangeArrayElementValueString(QString OldValue, unsigned Index, QString NewElementValue);

// Class CModesManager implementation
// ********************************************************************

CModesManager *CModesManager::m_SingletonInstance = NULL;

bool CModeItemCompare(CModeItem *A,CModeItem *B)
{
  return (A->GetName() > B->GetName());
}

void CModesManager::Init(CQParamsContainer *AttachedContainer, QString AppPath, CMiniSequencer *MiniSequencer)
{
  if(m_SingletonInstance == NULL)
    m_SingletonInstance = new CModesManager(AttachedContainer, AppPath, MiniSequencer);
}

void CModesManager::DeInit(void)
{
  if(m_SingletonInstance != NULL)
    Q_SAFE_DELETE(m_SingletonInstance);
}

CModesManager *CModesManager::Instance(void)
{
  return m_SingletonInstance;
}

// Constructor
CModesManager::CModesManager(CQParamsContainer *AttachedContainer, QString AppPath, CMiniSequencer *MiniSequencer, QString Name,  bool RefreshUI) :
CQComponent(Name),
m_StackZerroLevel(0)
{
	INIT_METHOD(CModesManager,GetModesStackSize);
	INIT_METHOD(CModesManager,BeginEnumeration);
	INIT_METHOD(CModesManager,GetNextModeName);
	INIT_METHOD(CModesManager,GetModesStackPath);
	INIT_METHOD(CModesManager,EnterMode);
	INIT_METHOD(CModesManager,ActivateMode);
	INIT_METHOD(CModesManager,GoOneModeBack);
	INIT_METHOD(CModesManager,ExitAllModes);
	INIT_METHOD(CModesManager,GotoDefaultMode);
	INIT_METHOD(CModesManager,GetCurrentModeName);
	INIT_METHOD(CModesManager,Save);
	INIT_METHOD(CModesManager,EnableConfirmationBypass);
	INIT_METHOD(CModesManager,DisableConfirmationBypass);
	INIT_METHOD(CModesManager,LoadSelectedMaterials);
	INIT_METHOD(CModesManager,GetModesDirectoriesList);

	m_EnumerationIndex  = 0;
	m_AttachedContainer = AttachedContainer;
	m_ModesList         = NULL;	
	m_MiniSequencer     = MiniSequencer;
	m_AppPath           = AppPath;       
	
    FOR_ALL_QUALITY_MODES(qm)
	{
		CALIBRATE_FOR_HSW[qm] = "~" + GetModeShortStr(qm,FIRST_QUALITY_MODE) + " HSW Calibrate";
		HSW_WEIGHT_TEST[qm] = "~" + GetModeShortStr(qm,FIRST_QUALITY_MODE) + " HSW Weight Test";
		FOR_ALL_OPERATION_MODES(om)
		{
			PRINT_MODE       [qm][om] = GetModeStr(qm,om) + " Mode";
			PER_MACHINE_MODE [qm][om] = "~PerMachine_" + GetModeShortStr(qm,om);
			WEIGHT_TEST      [qm][om] = GetModeShortStr(qm,om) + " Weight Test";
			
			MODEL_MATERIALS_MODES_DIR[qm][om]   = "Model" + GetModeShortStr(qm,om) + "Materials";
			SUPPORT_MATERIALS_MODES_DIR[qm][om] = "Support" + GetModeShortStr(qm,om) + "Materials";
		}
	}
	// Initializing the modes vectors
	Refresh();
	
	// Setting Hiden and No Save to all parameters in operation modes 
	for(int om = FIRST_OPERATION_MODE; om < LAST_OPERATION_MODE; om++)
    {
        CModeItem *Mode = FindModeByName(OPERATION_MODE_NAME(om),GENERAL_MODES_DIR);
        Mode->SetModeConst(true);
        Mode->SetParamsAttributes(m_AttachedContainer, paHidden, true);
    }

    m_RefreshUI = RefreshUI; 
}


// Create the lists of materials
void CModesManager::EnumerateMaterialModes()
{
	CAppParams *ParamsMgr = dynamic_cast<CAppParams*>(m_AttachedContainer);
	QString ModeName;
	unsigned int MaterialIndex = 0;	

	BeginEnumeration();

	while((ModeName = GetNextModeName(MODEL_DEFAULT_MATERIALS_MODES_DIR)) != "")
	{
		// Check if the mode file is not hidden
		if(ModeName[0] != '~')
		{
			// Add to the 'Materials' list
			EnterMode(ModeName, MODEL_DEFAULT_MATERIALS_MODES_DIR);
			MaterialIndex = ParamsMgr->ResinIndex;

			if (MaterialIndex == 0)
				CQLog::Write(LOG_TAG_GENERAL,"%s: Error. Could not attach an index number to Material Type: %s", Name().c_str(), ModeName.c_str());

				 if(!Q2RTApplication->GetSolubleSupportAllowed() && MaterialIndex==SUP706_RESIN_ID)
			{
			  CQLog::Write(LOG_TAG_GENERAL,"%s unsupported", ModeName.c_str());
			}
			else
			{
				m_ModelMaterialList.insert(value_type(MaterialIndex,ModeName));


			GoOneModeBack();
			}
		}
	}

	BeginEnumeration();

	//Create Model Service materials index list
	while((ModeName = GetNextModeName(MODEL_SERVICE_MATERIALS_MODES_DIR)) != "")
	{
		// Check if the mode file is not hidden
		if(ModeName[0] != '~')
		{
			// Add to the 'Materials' list
			EnterMode(ModeName, MODEL_SERVICE_MATERIALS_MODES_DIR);
			MaterialIndex = ParamsMgr->ResinIndex;

			if (MaterialIndex == 0)
				CQLog::Write(LOG_TAG_GENERAL,"%s: Error. Could not attach an index number to Material Type: %s", Name().c_str(),ModeName.c_str());

			m_ServiceModelMaterialList.insert(value_type(MaterialIndex,ModeName));

			GoOneModeBack();
		}
	}

	BeginEnumeration();

	// Repeat for the Support Materials: (found in a different Modes directory)
	while((ModeName = GetNextModeName(SUPPORT_DEFAULT_MATERIALS_MODES_DIR)) != "")
	{
		// Check if the mode file is not hidden
		if(ModeName[0] != '~')
		{
			// Add to the 'Materials' list
			EnterMode(ModeName, SUPPORT_DEFAULT_MATERIALS_MODES_DIR);
			MaterialIndex = ParamsMgr->ResinIndex;

			if (MaterialIndex == 0)
				CQLog::Write(LOG_TAG_GENERAL,"%s: Error. Could not attach an index number to Material Type: %s", Name().c_str(), ModeName.c_str());

			m_SupportMaterialList.insert(value_type(MaterialIndex,ModeName));

			GoOneModeBack();
		}
	}

	BeginEnumeration();

	//Create Support Service materials index list
	while((ModeName = GetNextModeName(SUPPORT_SERVICE_MATERIALS_MODES_DIR)) != "")
	{
		// Check if the mode file is not hidden
		if(ModeName[0] != '~')
		{
			// Add to the 'Materials' list
			EnterMode(ModeName, SUPPORT_SERVICE_MATERIALS_MODES_DIR);
			MaterialIndex = ParamsMgr->ResinIndex;

			if (MaterialIndex == 0)
				CQLog::Write(LOG_TAG_GENERAL, "%s: Error. Could not attach an index number to Material Type: %s", Name().c_str(),ModeName.c_str());

			m_ServiceSupportMaterialList.insert(value_type(MaterialIndex,ModeName));

			GoOneModeBack();
		}
	}

	// Write the found Materials to Log:
	for(TMaterialsList::iterator iter = m_ModelMaterialList.begin(); iter != m_ModelMaterialList.end(); ++iter)
		CQLog::Write(LOG_TAG_GENERAL,"%s: Identified Model Material: %d %s",Name().c_str(),iter->first, iter->second.c_str());

	// Write the found Service Model Materials:
	for(TMaterialsList::iterator iter = m_ServiceModelMaterialList.begin(); iter != m_ServiceModelMaterialList.end(); ++iter)
		CQLog::Write(LOG_TAG_GENERAL,"%s: Identified Model Service Material: %d %s",Name().c_str(),iter->first, iter->second.c_str());

	// Support Materials:
	for(TMaterialsList::iterator iter = m_SupportMaterialList.begin(); iter != m_SupportMaterialList.end(); ++iter)
		CQLog::Write(LOG_TAG_GENERAL,"%s: Identified Support Material: %d %s",Name().c_str(),iter->first, iter->second.c_str());

	// Support Service Materials:
	for(TMaterialsList::iterator iter = m_ServiceSupportMaterialList.begin(); iter != m_ServiceSupportMaterialList.end(); ++iter)
		CQLog::Write(LOG_TAG_GENERAL,"%s: Identified Support Service Material: %d %s",Name().c_str(),iter->first, iter->second.c_str());
}

QString CModesManager::GetModelResinType(int resin_index)
{
	TMaterialsList::iterator iter = m_ModelMaterialList.find(resin_index);

	if (iter != m_ModelMaterialList.end())
		return iter->second;

	return QString("Unsupported");
}

QString CModesManager::GetServiceModelResinType(int resin_index)
{
	TMaterialsList::iterator iter = m_ServiceModelMaterialList.find(resin_index);

	if (iter != m_ServiceModelMaterialList.end())
		return iter->second;

	return QString("Unsupported");
}

QString CModesManager::GetSupportResinType(int resin_index)
{
	TMaterialsList::iterator iter = m_SupportMaterialList.find(resin_index);

	if (iter != m_SupportMaterialList.end())
		return iter->second;

	return QString("Unsupported");
}

QString CModesManager::GetServiceSupportResinType(int resin_index)
{
	TMaterialsList::iterator iter = m_ServiceSupportMaterialList.find(resin_index);

	if (iter != m_ServiceSupportMaterialList.end())
		return iter->second;

	return QString("Unsupported");
}
bool CModesManager::IsModelMaterial(QString material)
{
	for(TMaterialsList::iterator iter = m_ModelMaterialList.begin(); iter != m_ModelMaterialList.end(); ++iter)
		if (iter->second == material)
			return true;
	return false;
}
bool CModesManager::IsSupportMaterial(QString material)
{
	for(TMaterialsList::iterator iter = m_SupportMaterialList.begin(); iter != m_SupportMaterialList.end(); ++iter)
		if (iter->second == material)
			return true;
	return false;
}
// Indicates if the material is Service material (Model)
bool CModesManager::IsServiceModelMaterial(QString material)
{
	for(TMaterialsList::iterator iter = m_ServiceModelMaterialList.begin(); iter != m_ServiceModelMaterialList.end(); ++iter)
		if (iter->second == material)
			return true;
	return false;
}

// Indicates if the material is Service material (Support)
bool CModesManager::IsServiceSupportMaterial(QString material)
{
	for(TMaterialsList::iterator iter = m_ServiceSupportMaterialList.begin(); iter != m_ServiceSupportMaterialList.end(); ++iter)
		if (iter->second == material)
			return true;
	return false;
}

// Destructor
CModesManager::~CModesManager(void)
{
  ClearModesList();
  ClearVirtualModesList();  
}

void CModesManager::InitFoldersList()
{
  TQFileSearchRec SearchRec;
  QString         PathStr;
  QString         ModesDir;
  CAppParams*     ParamsMgr = dynamic_cast<CAppParams*>(m_AttachedContainer);

  // Clear the folders vector
  m_ModesDirsList.clear();

  ModesDir = ParamsMgr->ModesDir;
  m_ModeFilesDirectory = m_AppPath + ModesDir + "\\";

  // Searching for Modes folders.
  PathStr = m_ModeFilesDirectory + "*.*";
  if(QFindFirstDir(PathStr,SearchRec))
  {
    do
    {
      QString ModeFileName = SearchRec.Name.c_str();

      // Insert new Folder to the folders list
      m_ModesDirsList.push_back(ModeFileName);
    } while(QFindNext(SearchRec));
  }
  QFindClose(SearchRec);
}

// Refresh mode files list
TQErrCode CModesManager::Refresh(void)
{
  TQFileSearchRec SearchRec;
  QString PathStr;

  // Before refreshing, make sure that we are in default mode
  ExitAllModes();
  ClearModesList();

  // Initating the folders list
  InitFoldersList();

  // For each Mode Directory build a different vector
  for (unsigned i = 0; i < m_ModesDirsList.size(); i++)
  {
    // The new Modes Item - includes the DirId and the Modes Vector
    TModesStruct Item;
    Item.Dir = m_ModesDirsList[i];

    // Pointer to a single mode class
    CModeItem *ModeItem = NULL;

    PathStr = m_ModeFilesDirectory + m_ModesDirsList[i] + "\\" + "*.cfg";
    // Enumerate modes directory
    if(QFindFirst(PathStr,faAnyFile,SearchRec))
      do
      {
        QString ModeFileName = SearchRec.Name.c_str();
        // Create a "mode" object for each file
        if (ModeFileName == (PER_MACHINE_MODE_DEFAULT + ".cfg"))
           ModeItem = new CChecksumModeItem(m_ModeFilesDirectory + m_ModesDirsList[i] + "\\"  + ModeFileName, m_MiniSequencer);
        else
           ModeItem = new CModeItem(m_ModeFilesDirectory + m_ModesDirsList[i] + "\\"  + ModeFileName, m_MiniSequencer);
        Item.ModesList.push_back(ModeItem);
      } while(QFindNext(SearchRec));

    PathStr = m_ModeFilesDirectory + m_ModesDirsList[i] + "\\" + "*.cfg.bak";
    if(QFindFirst(PathStr,faAnyFile,SearchRec))
      do
      {
        QString ModeFileName = SearchRec.Name.SubString(0,SearchRec.Name.Length()-4).c_str();
        TModesList::iterator Iter = Item.ModesList.begin();
        for (; Iter != Item.ModesList.end(); ++Iter)
            if ((*Iter)->GetName() == ModeFileName)
                break;
        if (Iter == Item.ModesList.end())
        {
           ModeItem = new CChecksumModeItem(m_ModeFilesDirectory + m_ModesDirsList[i] + "\\"  + ModeFileName, m_MiniSequencer);
           Item.ModesList.push_back(ModeItem);
        }
      } while(QFindNext(SearchRec));

    QFindClose(SearchRec);

    std::sort(Item.ModesList.begin(),Item.ModesList.end(),CModeItemCompare);

    // Adding the new Modes item - Dir name & matching Modes Vector
    m_ModesStructList.push_back(Item);
  }
  return Q_NO_ERROR;
}

int CModesManager::GetModesStackSize()
{
  return m_ModesStack.size();
}


// Start modes enumeration
TQErrCode CModesManager::BeginEnumeration(void)
{
  m_EnumerationIndex = 0;
  return Q_NO_ERROR;
}

// Get next mode name
QString CModesManager::GetNextModeName(QString Dir)
{
  m_ModesList = NULL;

  // Finding the right modes directory
  for (TModesStructList::iterator j = m_ModesStructList.begin(); j != m_ModesStructList.end(); ++j)
  {
    if (j->Dir == Dir)
      m_ModesList = &(j->ModesList);
  }

  if (m_ModesList == NULL)
    throw EModesManager ("Couldn't find Modes Directory with such ID");

  if(m_EnumerationIndex < m_ModesList->size())
    return (*m_ModesList)[m_EnumerationIndex++]->GetName();

  return "";
}

void CModesManager::ClearModesList()
{
  // Clearing all the directories
  for (TModesStructList::iterator j = m_ModesStructList.begin(); j != m_ModesStructList.end(); ++j)
  {
    m_ModesList = &(j->ModesList);

    for(TModesList::iterator i = m_ModesList->begin(); i != m_ModesList->end(); ++i)
      delete (*i);

    m_ModesList->clear();
  }
  m_ModesStructList.clear();  
}

void CModesManager::ClearVirtualModesList()
{
	for(TVirtualModesList::iterator i = m_VirtualModesList.begin(); i != m_VirtualModesList.end(); ++i)
		delete (*i);

	m_VirtualModesList.clear();
}

QString CModesManager::GetModesDirectoriesList(void)
{
  QString ret;
  for (TModesStructList::iterator j = m_ModesStructList.begin(); j != m_ModesStructList.end(); ++j)
    ret += j->Dir + ",";
  return ret;
}

// Find a mode by its name (return an index to the modes list or -1 if not found)
CModeItem* CModesManager::FindModeByName(const QString ModeName, QString Dir)
{
  m_ModesList = NULL;

  // Finding the right modes directory
  for (TModesStructList::iterator j = m_ModesStructList.begin(); j != m_ModesStructList.end(); ++j)
  {
    if (j->Dir == Dir)
      m_ModesList = &(j->ModesList);
  }

  if (m_ModesList == NULL)
    throw EModesManager ("Couldn't find '" + Dir + "' Directory");

  for(unsigned i = 0; i < m_ModesList->size(); i++)
    if((*m_ModesList)[i]->GetName() == ModeName)
      return (*m_ModesList)[i];

  return NULL;
}

// Create a string representation of the modes stack
QString CModesManager::GetModesStackPath(void)
{
  QString ret;
//  unsigned ModesStackSize = GetModesStackSize();
  TQStringVector tokens;
  for(unsigned i = 1; i < static_cast<unsigned>(GetModesStackSize()); i++)
    if(!m_ModesStack[i]->IsHidden())
      tokens.push_back(m_ModesStack[i]->GetName());
  JoinTokens(ret, tokens, "\\");
  return ret;
}

// Updating the parameters in the Parameter Manager according to the Mode,
// without placing the mode in the Modes Stack.
TQErrCode CModesManager::ActivateMode(QString ModeName, QString Dir)
{
  return ActivateMode(ModeName,Dir,-1);
}

TQErrCode CModesManager::ActivateMode(QString ModeName, QString Dir, int CurrentArrayIndex)
{
  // Search for the mode
  CModeItem *Mode = FindModeByName(ModeName, Dir);

  if(Mode == NULL)
    throw EModesManager("Mode \"" + ModeName + "\" could not be found");

  CQLog::Write(LOG_TAG_GENERAL, Name() + ": '" + ModeName + "' Mode was activated");

  // Activate the new mode
  Mode->Activate(m_AttachedContainer, CurrentArrayIndex);

  // Execute script lines embedded in the mode
  Mode->ExecuteScript();

  // Refresh the front-end display
  RefreshUI();

  return Q_NO_ERROR;
}

// Updating the parameters in the Parameter Manager according to the Mode,
// and placing the mode in the Modes Stack.
TQErrCode CModesManager::EnterMode(const QString ModeName, QString Dir)
{
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: FindModeByName");

  // Search for the mode
  CModeItem *Mode = FindModeByName(ModeName, Dir);

  if(Mode == NULL)
    throw EModesManager("Mode \"" + ModeName + "\" could not be found");
  return EnterMode(Mode);
}

TQErrCode CModesManager::EnterMode(CModeItem *Mode)
{
  TQErrCode Err = Q_NO_ERROR;
  QString ErrStr;

  try
  {
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: ModeEnter");
  Mode->Enter(m_AttachedContainer);

  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: ClearModeStack");
  for(TModesList::iterator i = m_ModesStack.begin(); i != m_ModesStack.end(); ++i)
    (*i)->SetLastInStackFlag(false);

  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: SetModeStackTop");
  Mode->SetLastInStackFlag(true);

//  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: PushMode");
  // Add the mode to the modes stack
  m_ModesStack.push_back(Mode);

//  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: ExecuteScript");
  // Execute script lines embedded in the mode
  Mode->ExecuteScript();

//  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: RefreshUI");
  // Refresh the front-end display
  RefreshUI();
  }
  catch(EQPython &e)
  {
	CQLog::Write(LOG_TAG_GENERAL, "Python error: " + e.GetErrorMsg());
	throw EQPython("The wizard encountered a problem (running the Python script) and will close. Please restart the printer application and run the wizard again.");
  }
  catch(EQException& err)
  {
     ErrStr = "Mode:" + Mode->GetName() + ": " + err.GetErrorMsg();
  }
  catch(...)
  {
     ErrStr = "Mode:" + Mode->GetName() + ": Undefined Error";
  }

  if (ErrStr != "")
  {
     Err = Q2RT_PARAM_ILLEGAL_VALUE;
     QMonitor.ErrorMessageWaitOk(ErrStr);
     throw EModesManager (ErrStr);
  }
  CQLog::Write(LOG_TAG_GENERAL, Name() + ": '" + Mode->GetName() + "' Mode was entered");
  return Err;
}

TQErrCode CModesManager::EnableConfirmationBypass(const QString ModeName, QString Dir)
{
  // Search for the mode
  CModeItem *Mode = FindModeByName(ModeName, Dir);

  if(Mode == NULL)
    throw EModesManager("Mode \"" + ModeName + "\" could not be found");

  // Enter the new mode
  Mode->EnableConfirmationBypass();

  return Q_NO_ERROR;
}

TQErrCode CModesManager::DisableConfirmationBypass(const QString ModeName, QString Dir)
{
  // Search for the mode
  CModeItem *Mode = FindModeByName(ModeName, Dir);

  if(Mode == NULL)
    throw EModesManager("Mode \"" + ModeName + "\" could not be found");

  // Enter the new mode
  Mode->DisableConfirmationBypass();

  return Q_NO_ERROR;
}

void CModesManager::SetStackZerroLevel(void)
{
   m_StackZerroLevel = GetModesStackSize();
}

TQErrCode CModesManager::GoOneModeBack(void)
{
  bool ChangeFlag = false;
  unsigned StackZerroLevel = m_StackZerroLevel; //to avoid mutex issues
  // Remove all last hiden modes in stack
  while(static_cast<unsigned>(GetModesStackSize()) > StackZerroLevel)
  {
    if(!m_ModesStack[GetModesStackSize() - 1]->IsHidden())
      break;

    m_ModesStack[GetModesStackSize() - 1]->Leave(m_AttachedContainer);
    m_ModesStack.pop_back();
    ChangeFlag = true;
  }

  // Remove last non-hiden item
  if(static_cast<unsigned>(GetModesStackSize()) > StackZerroLevel)
  {
    m_ModesStack[GetModesStackSize() - 1]->Leave(m_AttachedContainer);
    m_ModesStack.pop_back();
    ChangeFlag = true;
  }

  // Update the "Last In Stack" flag
  if(static_cast<unsigned>(GetModesStackSize()) > StackZerroLevel)
    m_ModesStack[GetModesStackSize() - 1]->SetLastInStackFlag(true);

  if(ChangeFlag)
    // Refresh the front-end display
    RefreshUI();

  CQLog::Write(LOG_TAG_GENERAL, Name() + ": Going one mode back");
  return Q_NO_ERROR;
}

TQErrCode CModesManager::ExitAllModes(void)
{
  bool ChangeFlag = false;

  while(GetModesStackSize() > 0)
  {
    m_ModesStack[m_ModesStack.size() - 1]->Leave(m_AttachedContainer);
    m_ModesStack.pop_back();
    ChangeFlag = true;
  }
  
  ClearVirtualModesList();

  if(ChangeFlag)
    // Refresh the front-end display
    RefreshUI();

  return Q_NO_ERROR;
}

TQErrCode CModesManager::GotoDefaultMode(void)
{
  CAppParams*      ParamsMgr = dynamic_cast<CAppParams*>(m_AttachedContainer);
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: default mode. mode mgr = 0x%x; Param mgr = 0x%x", this, ParamsMgr);
  ExitAllModes();
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: def1");
  EnterMode(DEFAULT_MODE, DEFAULT_MODES_DIR);
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: def2");
  EnterMode(OPERATION_MODE_NAME(ParamsMgr->PipesOperationMode), GENERAL_MODES_DIR);
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: def3");
  EnterMode(DEFAULT_MATERIALS_MODE, DEFAULT_MODES_DIR);
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: def4");
  SetStackZerroLevel();
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: def5");
  RefreshUI();
  return Q_NO_ERROR;
}

QString CModesManager::GetCurrentModeName(void)
{
  // Find the first mode name that is not hidden
  for(int i = GetModesStackSize() - 1; i >= 0; i--)
    if(!m_ModesStack[i]->IsHidden())
      return m_ModesStack[i]->GetName();

  return "";
}

// Save all the modes
TQErrCode CModesManager::Save(void)
{
  for(TModesList::iterator i = m_ModesStack.begin(); i != m_ModesStack.end(); ++i)
    (*i)->Save();

  return Q_NO_ERROR;
}

// Save single parameter
TQErrCode CModesManager::SaveSingleParameter(CQParameterBase *ParamToSave)
{
  for(TModesList::iterator i = m_ModesStack.begin(); i != m_ModesStack.end(); ++i)
    (*i)->SaveSingle(ParamToSave);

  return Q_NO_ERROR;
}

TQErrCode CModesManager::LoadSelectedMaterials(QString SelectedMode)
{
	TQErrCode ret = Q_NO_ERROR;
	
	QString ModelMaterialsDir   = "";
	QString SupportMaterialsDir = "";
	QString Suffix              = "";
	QString DefsDir             = ""; 
	
	CAppParams *ParamsMgr = dynamic_cast<CAppParams*>(m_AttachedContainer);
	
	int QualityMode,OperationMode;
	
	if(SelectedMode == "Default")
	{
		ModelMaterialsDir   = MODEL_DEFAULT_MATERIALS_MODES_DIR;
		SupportMaterialsDir = SUPPORT_DEFAULT_MATERIALS_MODES_DIR;
		DefsDir             = DEFAULT_MODES_DIR;
		Suffix              = "";
	}
	else if (GetModeFromShortStr(SelectedMode, QualityMode, OperationMode))
	{
		ModelMaterialsDir   = MODEL_MATERIALS_MODES_DIR[QualityMode][OperationMode];
		SupportMaterialsDir = SUPPORT_MATERIALS_MODES_DIR[QualityMode][OperationMode];
		DefsDir             = MACHINE_QUALITY_MODES_DIR(QualityMode,OperationMode);
		Suffix              = "_" + SelectedMode;
	}
	else
		throw EModesManager (QFormatStr("Can't find mode: %s", SelectedMode.c_str()));
	
	bool Service = false;
	
	for(int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; i++)
	{
		if(IsServiceModelMaterial(ParamsMgr->TypesArrayPerChamber[i]))
		{
			Service = true;
			break;
		}
	}
	
	for(int i = FIRST_SUPPORT_CHAMBER_INDEX; i < LAST_SUPPORT_CHAMBER_INDEX && !Service; i++)
	{
		if(IsServiceSupportMaterial(ParamsMgr->TypesArrayPerChamber[i]))
		{
			Service = true;
			break;
		}
	}
	
	if (Service)	
	{
		ModelMaterialsDir   = MODEL_SERVICE_MATERIALS_MODES_DIR;
		SupportMaterialsDir = SUPPORT_SERVICE_MATERIALS_MODES_DIR;
		DefsDir             = DEFAULT_MODES_DIR;
		Suffix              = "";
	}
	
	try
	{
		ret = CalculateMaterialActivation(Suffix, ModelMaterialsDir, SupportMaterialsDir, DefsDir);
	}
    catch(EModesManager &err)
    {  
        CQLog::Write(LOG_TAG_GENERAL, "Modes manager: " + err.GetErrorMsg());
		QMonitor.ErrorMessage(err.GetErrorMsg());
	}
	
    return ret;
}

TQErrCode CModesManager::CalculateMaterialActivation(QString Suffix, QString ModelMaterialsDir, QString SupportMaterialsDir, QString DefsDir)
{
	QString          ArrayParamValue, ParamValue;
	CAppParams*      ParamsMgr       = dynamic_cast<CAppParams*>(m_AttachedContainer);


	CModeItem*       MaterialModes [NUMBER_OF_CHAMBERS] = {NULL};
	CModeItem*       MaterialsDefsMode = FindModeByName(LOAD_STRING(IDS_MATERIAL_DEFS_MODE) + Suffix, DefsDir);

	// Get the Material-Mode item pointer according to the Material name (material names are in: TypesArrayPerChamber).
	if (DirectoryExists((m_ModeFilesDirectory + ModelMaterialsDir).c_str()))
		for(int i = FIRST_MODEL_CHAMBER_INDEX; i < LAST_MODEL_CHAMBER_INDEX; i++)
			MaterialModes[i] = FindModeByName(ParamsMgr->TypesArrayPerChamber[i] + Suffix.c_str(), ModelMaterialsDir);
	if (DirectoryExists((m_ModeFilesDirectory + SupportMaterialsDir).c_str()))
		for(int i = FIRST_SUPPORT_CHAMBER_INDEX; i < LAST_SUPPORT_CHAMBER_INDEX; i++)
			MaterialModes[i] = FindModeByName(ParamsMgr->TypesArrayPerChamber[i] + Suffix.c_str(), SupportMaterialsDir);

	TQStringVector   ParamsList;
	TQStringVector   ParamValues;
	CQStringList VirtualModeParams;
	VirtualModeParams.Clear();
	int Count = 0;

	// Assign parameters from material modes to Virtual mode
	for(int i = FIRST_CHAMBER_TYPE ; i < LAST_CHAMBER_TYPE ; i++)
	{
		if (MaterialModes[i])
		{
			AssignParamsFromMode(&VirtualModeParams, MaterialModes[i], i);
			Count++;
		}
	}
	// Calculate values of common parameters for both modes with python formulas
	//--------------------------------------------
	// This could happen in HQ or HS modes
	if ((Count > 1) && MaterialsDefsMode)
	{		      
		TQStringVector   PythonValuesList = MaterialsDefsMode->GetModeValues();
		ParamsList = MaterialsDefsMode->GetModeKeys();
		CQParameterBase* Param;
		for(unsigned j = 0; j < ParamsList.size(); j++)
		{
			//if no such parameter existing - ignore
			if ((Param = dynamic_cast<CQParameterBase *>(ParamsMgr->FindProperty(ParamsList[j]))) == NULL)
				continue;
			// Create a pair of values in ParamValues array: For each chamber, put the value of ParamsList[j] parameter.
			//   The Python function, calculating the *final Value* is activated later on this pair.
			ParamValues.clear();
			for(int i = FIRST_CHAMBER_TYPE; i < LAST_CHAMBER_TYPE; i++)
			{
				ParamValue = (MaterialModes[i] ? MaterialModes[i]->FindValueByKey(ParamsList[j]) : (QString)"" );
				// Don't add parameter from Q2RT for support (default value) if it's not in support material file
				if (IS_MODEL_CHAMBER(i))
					ParamValues.push_back(("" == ParamValue) ? Param->ValueAsString() : ParamValue);
				else if (ParamValue != "")
					ParamValues.push_back(ParamValue); 
			}		
			ArrayParamValue = ParamValues[0];
			for(unsigned i = 1; i < ParamValues.size(); i++)
				ArrayParamValue += APP_PARAMS_DELIMETER + ParamValues[i];

			ParamsMgr->MaterialPythonArgs = ArrayParamValue;
			ParamsMgr->MaterialPythonArraySize = ParamValues.size();
			
			 // Materials common parameters *final Value* calculation is no longer done on Python scripts !!!	
			ApplyMaterialsFunction(PythonValuesList[j]);			
				
			CheckExistanceAndAddParam(&VirtualModeParams, ParamsList[j], ParamsList[j] + "=" + ParamsMgr->MaterialPythonRes.ValueAsString());
		}		
	}
	
	// Creating and entering new 'virtual' mode
	//------------------------------------------
	CVirtualModeItem *VirtualMode = new CVirtualModeItem(VirtualModeParams, "~VirtualMode" + Suffix);
	m_VirtualModesList.push_back(VirtualMode);
	EnterMode(VirtualMode);
	return Q_NO_ERROR;
}

// Apply material function loaded from 'MeterialDefs' files
void CModesManager::ApplyMaterialsFunction(QString FunctionName)
{
	CAppParams* ParamsMgr = dynamic_cast<CAppParams*>(m_AttachedContainer);
	
	TQStringVector Tokens;
	std::vector<float> ParamsVector;
		
	Tokenize(ParamsMgr->MaterialPythonArgs, Tokens, APP_PARAMS_DELIMETER);	
	
	// Calculate max value from vector
	if(QStrToUpper(FunctionName).compare(QStrToUpper("~max.py")) == 0)
	{
		for(unsigned int i = 0; i < Tokens.size(); i++)
			ParamsVector.push_back(QStrToFloat(Tokens[i]));
		ParamsMgr->MaterialPythonRes = QFloatToStr(*std::max_element(ParamsVector.begin(), ParamsVector.end()));
	} // Calculate min value from vector
	else if(QStrToUpper(FunctionName).compare(QStrToUpper("~min.py")) == 0)
	{		
		for(unsigned int i = 0; i < Tokens.size(); i++)
			ParamsVector.push_back(QStrToFloat(Tokens[i]));
		ParamsMgr->MaterialPythonRes = QFloatToStr(*std::min_element(ParamsVector.begin(), ParamsVector.end()));
	} // Append values of several string type parameters into one string 
	else if(QStrToUpper(FunctionName).compare(QStrToUpper("~SmartAppend.py")) == 0)
	{
		QString res = "";
		for(unsigned int i = 0; i < Tokens.size(); i++)
			if(static_cast<int>(res.find(Tokens[i])) == -1)
				res += Tokens[i] + "\n\r"; 
		ParamsMgr->MaterialPythonRes = res;
	} // Create minimal array from set of arrays
	else if(QStrToUpper(FunctionName).compare(QStrToUpper("~ArrayMin.py")) == 0)
	{
		for(unsigned int i = 0; i < Tokens.size(); i++)
			ParamsVector.push_back(QStrToFloat(Tokens[i]));
			
		int ElementSize = ParamsVector.size() / QStrToInt(ParamsMgr->MaterialPythonArraySize.ValueAsString());
		
		std::vector<float> temp;
		QString res = "";
		for(unsigned int i = 0; i < static_cast<unsigned>(ElementSize); i++)
		{
			for(unsigned int j = i; j < ParamsVector.size(); )
			{
				temp.push_back(ParamsVector[j]);			
				j += ElementSize;
			}
			res += QFloatToStr(*std::min_element(temp.begin(), temp.end())) + APP_PARAMS_DELIMETER;
			temp.clear();
		}		
		res.erase(res.end() - 1, res.end());
		ParamsMgr->MaterialPythonRes = res;
	}
	else
	{
		throw EModesManager (QFormatStr("Modes manager: Incorrect materials function: %s", FunctionName.c_str()));
	}
}

void CModesManager::AssignParamsFromMode(CQStringList *VirtualModeParams, CModeItem *Mode, int ChamberIndex)
{
	QString          ParamValue;
	CAppParams*      ParamsMgr = dynamic_cast<CAppParams*>(m_AttachedContainer);
	CQParameterBase* Param;
	
	TQStringVector ParamsList = Mode->GetModeKeys();
		
	for(unsigned j = 0; j < ParamsList.size(); j++)
	{
		//if no such parameter existing - ignore
		if ((Param = dynamic_cast<CQParameterBase *>(ParamsMgr->FindProperty(ParamsList[j]))) == NULL)
			continue;
		
		CQArrayParameterBase* ArrayParam = dynamic_cast<CQArrayParameterBase *>(Param);
		
		if (ArrayParam != NULL && Mode->IsElement(j))
		{
			ParamValue = ChangeArrayElementValueString(ArrayParam->ValueAsString(), ChamberIndex, Mode->GetModeValues()[j]);
			CheckExistanceAndAddParam(VirtualModeParams, ParamsList[j], ParamsList[j] + "=" + ParamValue, ChamberIndex);
		}
		else
		{
			ParamValue = (Mode ? Mode->FindValueByKey(ParamsList[j]) : (QString)"");
			CheckExistanceAndAddParam(VirtualModeParams, ParamsList[j], ParamsList[j] + "=" + ((ParamValue == "") ? Param->ValueAsString() : ParamValue));
		}		
	}
}

void CModesManager::CheckExistanceAndAddParam(CQStringList *VirtualModeParams, QString ParamToCheck, QString ParamStringToAdd, int ChamberIndex)
{
	bool ParamAlreadyInMode = false;
	int index;
	
	QString Key, Value;	
	
	// If this parameter is already in the list, we'll overwrite the prevous value, otherwise add it to the list
	for (unsigned i = 0 ; i < VirtualModeParams->Count() ; i++)
	{		
		if(QBreakKeyAndValueStr((*VirtualModeParams)[i],Key,Value))
		{
			if (Key == ParamToCheck)
			{
				ParamAlreadyInMode = true;
				index = i;
				break;
			}
		}			
	}
	
	if (ParamAlreadyInMode)
	{
		if (ChamberIndex != -1)
		{
			QString NewKey, NewValue;

			CQStringList str;			
			str.Add(ParamStringToAdd);
			
			QBreakKeyAndValueStr(str[0], NewKey, NewValue);
			
			TQStringVector OldValueTokens;
			Tokenize(NewValue, OldValueTokens, APP_PARAMS_DELIMETER);
			
			ParamStringToAdd = Key + "=" + ChangeArrayElementValueString(Value, ChamberIndex, OldValueTokens[ChamberIndex]);
		}
		
		VirtualModeParams->SetItem(index, ParamStringToAdd);
	}
	else			
		VirtualModeParams->Add(ParamStringToAdd);	
}

void CModesManager::RefreshUI()
{
	if(m_RefreshUI)
		FrontEndInterface->UpdateStatus(FE_REFRESH_MODES_DISPLAY,0,true);
}

// Class CModeItem implementation
// ********************************************************************

const int SCRIPT_PARAMS_NUM = 5;

// Constructor
CModeItem::CModeItem(QString ModeName) :
m_IsModeConst(false),
m_ModeName(ModeName),
m_MiniSequencer(NULL),
m_ScratchFlag(false),
m_LastInStackFlag(false),
m_UpdateConfirmationNeeded(false)
{}

CModeItem::CModeItem(const QString& ModeFileName, CMiniSequencer *MiniSequencer) :
m_IsModeConst(false),
m_ModeFileName(ModeFileName),
m_ModeName(QExtractFileNameWithoutExt(ModeFileName)),
m_MiniSequencer(MiniSequencer),
m_LastInStackFlag(false),
m_UpdateConfirmationNeeded(true),
m_ScratchFlag(false)
{
  Refresh();
}

CVirtualModeItem::CVirtualModeItem(CQStringList &StringList, QString ModeName) : CModeItem(ModeName)
{	
	Refresh(StringList);
}

// Destructor
CModeItem::~CModeItem(void){}

CChecksumModeItem::CChecksumModeItem(const QString& ModeFileName, CMiniSequencer *MiniSequencer) :
CModeItem(ModeFileName,MiniSequencer)
{
  //The only option application is not continues is both missing (irrelevant in modes)
  //both corrupted generates new checksum
  Refresh();
}
// Reload mode file content
// If no stream is assigned raise error
void CChecksumModeItem::Refresh(void)
{
  CQStringList Content;
  TInitStatus_E InitStatus;
  CQParamsFileWithChecksumStream ParamsStream(GetFileName(), InitStatus, false, true);
  if((InitStatus == BOTH_FILES_ARE_MISSING_E)||(InitStatus == BOTH_FILES_ARE_CORRUPTED_E))
    throw EModesManager ("The files "+ GetFileName() +" and " + GetFileName() +".bak are both missing or corrupted.\n\r Without these files the application will not work properly. \n\r");
  ParamsStream.LoadFileIntoStringList(GetFileName(),Content);
  CModeItem::Refresh(Content);
}

void CModeItem::Refresh(void)
{
  CQStringList Content;
  CQParamsFileStream ParamsStream(GetFileName());
  ParamsStream.LoadFileIntoStringList(GetFileName(),Content);
  Refresh(Content);
}

void CModeItem::Refresh(CQStringList &StringList)
{
  Empty();
  Resize(StringList.Count());
  int i = 0;
  TQStringMap KeyToValueMap;
  QString Key,Value,ArrayMark = "[]";
  CQParamsFileStream::SplitToKeysAndValues(StringList, KeyToValueMap, StringList.Count());
  for(TQStringMap::iterator Iter = KeyToValueMap.begin(); Iter != KeyToValueMap.end(); ++Iter, i++)
  {
     Key = Iter->first;
     Value = Iter->second;
     m_Values[i] = Value;
	 if (m_IsElement[i] = (static_cast<int>(Key.find(ArrayMark, 0)) != -1))
        Key = Key.substr(0,Key.size()-ArrayMark.size());
     m_Keys[i] = Key;
     m_TKeyToIndexMap[Key] = i;
  }
}

void CModeItem::Empty()
{
  m_Keys.clear();
  m_Values.clear();
  m_IsElement.clear();
  m_TKeyToIndexMap.clear();
}

void CModeItem::Resize(int Size)
{
  m_Keys.resize (Size);
  m_Values.resize (Size);
  m_IsElement.resize (Size);
}

QString ChangeArrayElementValueString(QString OldValue, unsigned Index, QString NewElementValue)
{      
   QString ret;
   TQStringVector tokens;
   Tokenize(OldValue, tokens, APP_PARAMS_DELIMETER);
   if (tokens.size() > Index)
      tokens[Index] = NewElementValue;
   JoinTokens(ret, tokens, APP_PARAMS_DELIMETER);
   return ret;
}

void CModeItem::SetParamsAttributes(CQParamsContainer *ParamsContainer, TParamAttributes Attribute, bool SetClear, CQParameterBase* Param)
{		
  if (!Param)
  {
	for(unsigned j = 0; j < m_Keys.size(); j++)
                Param = dynamic_cast<CQParameterBase *>(ParamsContainer->FindProperty(m_Keys[j]));
		if (Param)
           	      SetParamsAttributes(ParamsContainer,Attribute,SetClear,Param);
    return;
  }
  if (SetClear)
	 Param->AddAttribute(Attribute);
  else
	 Param->RemoveAttribute(Attribute);
}

void CModeItem::SetModeConst(bool IsConst)
{
   m_IsModeConst = IsConst;
}

// Apply the mode on a params container
void CModeItem::Enter(CQParamsContainer *ParamsContainer)
{
	Enter(ParamsContainer, false);
}

// Apply the mode on a params container
void CVirtualModeItem::Enter(CQParamsContainer *ParamsContainer)
{
	CModeItem::Enter(ParamsContainer, true);
}

void CModeItem::Enter(CQParamsContainer *ParamsContainer, bool SetClear)
{
  m_Attributes.clear();
  m_Attributes.resize (m_Keys.size());
  
  CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: mode attrib cleared");
  // Process each entry in the "Keys" array
  for(unsigned i = 0; i < m_Keys.size(); i++)
  {
    // Search for a matching parameter in the container
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(ParamsContainer->FindProperty(m_Keys[i]));
	
	// If found...
    if(Param != NULL && m_IsElement[i] == false)
    {
      CQLog::Write(LOG_TAG_TRAY_HEATERS, "ThermistorOpMode debugging: param %s/%s", Param->GroupName().c_str(), Param->Description().c_str());
      // Remember prevoius attributes values of the parameter
      m_Attributes[i] = Param->GetAttributes();
	  
	  SetParamsAttributes(ParamsContainer, paFromMaterialMode, SetClear, Param);

      // Push the current parameter value and assign a new one
      Param->Push();
      Param->DisableObservers();
      Param->AssignFromString(m_Values[i]);
      Param->EnableObservers();

      // Attach an observer event
      if (!m_IsModeConst)
         Param->AddGenericObserver(ParamChangeEvent,reinterpret_cast<TGenericCockie>(this));
    }
  }
}

// Apply the mode on a params container
void CModeItem::Activate(CQParamsContainer *ParamsContainer, int CurrentArrayIndex)
{
  CAppParams* ParamsMgr = dynamic_cast<CAppParams*>(ParamsContainer);
  // Process each entry in the "Keys" array
  for(unsigned i = 0; i < m_Keys.size(); i++)
  {
    //If this is array element item - assign only the element value
    CQArrayParameterBase* ArrayParam = dynamic_cast<CQArrayParameterBase *>(ParamsContainer->FindProperty(m_Keys[i]));
    // If found array's element line...
    if(ArrayParam != NULL && m_IsElement[i] == true)
    {
       ArrayParam->AssignFromString(ChangeArrayElementValueString(ArrayParam->ValueAsString(),
                                                                  CurrentArrayIndex,
                                                                  m_Values[i]));
       ParamsMgr->SaveSingleParameter(ArrayParam);
       continue;
    }

    //otherwise - assign the parameter value(including arrays param)
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(ParamsContainer->FindProperty(m_Keys[i]));

    // If found...
    if(Param != NULL)
    {
      // Push the current parameter value and assign a new one
      Param->AssignFromString(m_Values[i]);
      ParamsMgr->SaveSingleParameter(Param);
    }
  }
}

// Disable the mode on a params container
void CModeItem::Leave(CQParamsContainer *ParamsContainer)
{
  // Process each entry in the "Keys" array
  for(unsigned i = 0; i < m_Keys.size(); i++)
  {
    // Search for a matching parameter in the container
    CQParameterBase *Param = dynamic_cast<CQParameterBase *>(ParamsContainer->FindProperty(m_Keys[i]));
	
	// If found...
    if(Param != NULL && m_IsElement[i] == false)
    {
      // Return the previous value of the attribute
      Param->SetAttributes(m_Attributes[i]);

      // Unregister the observer...
      Param->UnregisterGenericObserver(ParamChangeEvent);

      Param->DisableObservers();
      Param->Pop();
      Param->EnableObservers();
    }
  }
}

// Save current values to the mode file
void CChecksumModeItem::Save(void)    // Save current values to the mode file
{
    TInitStatus_E StatusInit = INIT_OK_E; //init
    CQParamsFileWithChecksumStream Stream(GetFileName(), StatusInit);
    SaveToStream(&Stream);
}

void CModeItem::SaveToStream(void* pStream)
{
	if (! m_ScratchFlag) // If the scratch flag is not set, do nothing
		return;
	if (! pStream)
		return;
    CQParamsFileStream* Stream = (CQParamsFileStream*)pStream;
    for(unsigned j = 0; j < m_Keys.size(); j++)
	   Stream->SetValueToParameter(m_Keys[j], m_Values[j], m_Attributes[j]);
    Stream->Save();
}

void CModeItem::SaveSingleToStream(void* pStream,CQParameterBase *ParamToSave)
{
   if (! m_ScratchFlag) // If the scratch flag is not set, do nothing
   	return;
   if (! ParamToSave)
   	return;
   if (! pStream)
   	return;
   TKeyToIndexMapIter Iter = m_TKeyToIndexMap.find(ParamToSave->Name());
   if (Iter != m_TKeyToIndexMap.end())
   {
    CQParamsFileStream* Stream = (CQParamsFileStream*)pStream;
    Stream->SetValueToParameter(m_Keys[Iter->second], m_Values[Iter->second], m_Attributes[Iter->second]);
    Stream->Save();
   }
}

void CChecksumModeItem::SaveSingle(CQParameterBase *ParamToSave)  // Save single parameter to mode file
{
    TInitStatus_E StatusInit = INIT_OK_E; //init
    CQParamsFileWithChecksumStream Stream(GetFileName(), StatusInit);
    SaveSingleToStream(&Stream,ParamToSave);
}

void CModeItem::Save(void)
{
    CQParamsFileStream Stream(GetFileName());
    SaveToStream(&Stream);
}

// Save single parameter to mode file
void CModeItem::SaveSingle(CQParameterBase *ParamToSave)
{
    CQParamsFileStream Stream(GetFileName());
    SaveSingleToStream(&Stream,ParamToSave);
}

// Observer callback for the attached parameter
void CModeItem::ParamChangeEvent(CQParameterBase *Param,TGenericCockie Cockie)
{
	// Get a pointer to the mode item object instance
	CModeItem *ModeItem = reinterpret_cast<CModeItem *>(Cockie);
	bool UpdateParam = !ModeItem->m_IsModeConst;

    // when the user changes a topmost mode parameter he means to change it, so we don't ask about it, we change it tacitly (According to Chaim K.).
	if (  UpdateParam && ! ModeItem->LastInStack() &&  ModeItem->m_UpdateConfirmationNeeded
  		  && (ModeItem->GetName() != PER_MACHINE_MODE_DEFAULT))
		UpdateParam = QMonitor.AskYesNo("Do you want the change you made to the \"" + Param->Name() + "\" parameter to also apply in the \"" + ModeItem->GetName() + "\" mode?");

	if(UpdateParam)
	{
       TKeyToIndexMapIter Iter = ModeItem->m_TKeyToIndexMap.find(Param->Name());
       if (Iter != ModeItem->m_TKeyToIndexMap.end())
	   {
			ModeItem->m_Values[Iter->second] = Param->ValueAsString();
			ModeItem->m_ScratchFlag = true; // Remember that something has changed
	   }
	}
}

QString CModeItem::FindValueByKey(const QString Key)
{
   QString Value;
   TKeyToIndexMapIter Iter = m_TKeyToIndexMap.find(Key);
   if (Iter != m_TKeyToIndexMap.end())
      Value = m_Values[Iter->second];
   return Value;
}

// Execute script lines embedded in the mode
void CModeItem::ExecuteScript(void)
{
  for(int i = 1; i <= SCRIPT_PARAMS_NUM; i++)
  {
    QString KeyName = QFormatStr("RunScript%d",i);
    QString Tmp = FindValueByKey(KeyName);

    // If found...
    if(Tmp != "")
      m_MiniSequencer->EvaluateExpression(Tmp);
  }
}

// Return true if this mode file is hidden
bool CModeItem::IsHidden(void)
{
  return IS_MODE_HIDDEN(m_ModeName);
}

QString CModeItem::GetName(void)
{
  return m_ModeName;
}

QString CModeItem::GetFileName(void)
{
  return m_ModeFileName;
}

void CModeItem::EnableConfirmationBypass()
{
  m_UpdateConfirmationNeeded = false;
}

void CModeItem::DisableConfirmationBypass()
{
  m_UpdateConfirmationNeeded = true;
}

void CModeItem::SetLastInStackFlag(bool Flag)
{
   m_LastInStackFlag = Flag;
}

bool CModeItem::LastInStack(void)
{
   return m_LastInStackFlag;
}

TQStringVector CModeItem::GetModeKeys(void)
{
   return m_Keys;
}

TQStringVector CModeItem::GetModeValues(void)
{
   return m_Values;
}

bool CModeItem::IsElement(int index)
{
	return m_IsElement[index];
}

