/********************************************************************
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
 * Last upate: 17/03/2003                                           *
 ********************************************************************/

#ifndef _MODES_MANAGER_H_
#define _MODES_MANAGER_H_

#include "QComponent.h"
#include "QStringList.h"
#include "QParameter.h"
#include "Classes.hpp" //bug 5719
#include <map>


class CQParamsContainer;
class CQParameterBase;
class CMiniSequencer;

// Exception class for all the QLib RTTI elements
class EModesManager : public EQException {
  public:
    EModesManager(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};

//////////////////////////////////////
// Class representing a single mode //
//////////////////////////////////////
typedef std::vector<bool>             TBoolVector;
typedef std::vector<TParamAttributes> TAttributesVector;

class CModeItem : public CQObject
{

typedef std::map<QString,unsigned int> TKeyToIndexMap;
typedef TKeyToIndexMap::iterator TKeyToIndexMapIter;

private:
	bool            m_IsModeConst;
    QString         m_ModeFileName;
    QString         m_ModeName;
    CMiniSequencer *m_MiniSequencer;
    bool            m_LastInStackFlag;
    bool            m_UpdateConfirmationNeeded;

    // Observer callback for the attached parameter
    static void ParamChangeEvent(CQParameterBase *Param,TGenericCockie Cockie);	

protected:
    bool              m_ScratchFlag;
    TQStringVector    m_Keys;
    TQStringVector    m_Values;
    TBoolVector       m_IsElement;
	TAttributesVector m_Attributes;
    TKeyToIndexMap    m_TKeyToIndexMap;

    void Enter(CQParamsContainer *ParamsContainer, bool SetClear);
    void SaveToStream(void* pStream);
    void SaveSingleToStream(void* pStream,CQParameterBase *ParamToSave);

public:
    CModeItem(const QString& ModeFileName, CMiniSequencer *MiniSequencer);
	CModeItem(QString ModeName);
    virtual ~CModeItem(void);

    void SetModeConst(bool IsConst);
    virtual void Resize(int Size);
    virtual void Empty(void);   
    virtual void Refresh(void);        // Reload mode file content
	void Refresh(CQStringList &StringList);
    QString GetName(void);             // Return mode name
    QString GetFileName(void);         // Return mode file name (full path)
    void EnableConfirmationBypass();   // Allow bypassing the 'Are you sure you want to update..."
    void DisableConfirmationBypass();  // Don't allow bypassing the 'Are you sure you want to update..."
    void SetLastInStackFlag(bool Flag);// Set the "LastInStack" flag
    bool LastInStack(void); // Return true if this mode is the last in the modes stack
    bool IsHidden(void); // Return true if this mode file is hidden
    virtual void Enter(CQParamsContainer *ParamsContainer); // Apply the mode on a params container
    virtual void Activate(CQParamsContainer *ParamsContainer, int CurrentArrayIndex); // Activate the parameters of the mode on the container
    void Leave(CQParamsContainer *ParamsContainer); // Disable the mode on a params container
    virtual void Save(void); // Save current values to the mode file
    void ExecuteScript(void); // Execute script lines embedded in the mode

    TQStringVector GetModeKeys(void); // Execute script lines embedded in the mode
    TQStringVector GetModeValues(void); // Execute script lines embedded in the mode
    QString FindValueByKey(const QString Key);

	virtual void SaveSingle(CQParameterBase *ParamToSave);  // Save single parameter to mode file

	bool IsElement(int index);
	void SetParamsAttributes(CQParamsContainer *ParamsContainer, TParamAttributes Attribute, bool SetClear, CQParameterBase* Param = NULL);
};

/////////////////////////////////////////////////////////////////////////////
// Class representing a single 'virtual' mode created from materials files //
/////////////////////////////////////////////////////////////////////////////

class CChecksumModeItem : public CModeItem
{
public:
    CChecksumModeItem(const QString& ModeFileName, CMiniSequencer *MiniSequencer);
    virtual void Refresh(void); // Reload mode file content
    virtual void Save(void);    // Save current values to the mode file
	virtual void SaveSingle(CQParameterBase *ParamToSave);  // Save single parameter to mode file
};

class CVirtualModeItem : public CModeItem
{
public:
	CVirtualModeItem(CQStringList &StringList, QString ModeName);
	virtual void Enter(CQParamsContainer *ParamsContainer); // Apply the mode on a params container
};

/////////////////////////
// Modes manager class //
/////////////////////////
class CModesManager : public CQComponent
{
private:
	typedef std::map<unsigned int,QString> TMaterialsList;
	TMaterialsList m_ModelMaterialList;
	TMaterialsList m_SupportMaterialList;
	TMaterialsList m_ServiceModelMaterialList;
	TMaterialsList m_ServiceSupportMaterialList;

	typedef TMaterialsList::value_type value_type;	
	
    typedef std::vector<CModeItem *> TModesList;
	typedef std::vector<CVirtualModeItem *> TVirtualModesList;
	
    // TModesList is a vector of Modes from a single directory
    TModesList *m_ModesList;
	
	TVirtualModesList m_VirtualModesList;

    // Each modes directory represents by Dir name Modes Vector
    typedef struct ModesStruct{QString    Dir;
                              TModesList  ModesList;
                              }TModesStruct;

    typedef std::vector<TModesStruct> TModesStructList;
    // List of ALL the Modes divided to their directories,
    TModesStructList m_ModesStructList;

    // The active Modes Stack
    TModesList m_ModesStack;

    typedef std::vector<QString> TModesDirsList;
    TModesDirsList m_ModesDirsList;

    QString               m_ModeFilesDirectory;
    QString               m_AppPath;
    CMiniSequencer*       m_MiniSequencer;
    unsigned              m_EnumerationIndex;
    CQParamsContainer*    m_AttachedContainer;
    static CModesManager* m_SingletonInstance;
    unsigned              m_StackZerroLevel;    

    void ClearModesList();
	void ClearVirtualModesList();
    void InitFoldersList();
    void SetStackZerroLevel(void);

    // Find a mode by its name (return an index to the modes list or -1 if not found)
    CModeItem *FindModeByName(const QString ModeName, QString Dir);
    TQErrCode CalculateMaterialActivation(QString Suffix, QString ModelMaterialsDir, QString SupportMaterialsDir, QString DefsDir);
	
	
	void CheckExistanceAndAddParam(CQStringList *VirtualModeParams, QString ParamToCheck, QString ParamStringToAdd, int ChamberIndex = -1);
	void AssignParamsFromMode(CQStringList *VirtualModeParams, CModeItem *Mode, int ChamberIndex);
	
	TQErrCode EnterMode(CModeItem *Mode);
	bool m_RefreshUI;
	void RefreshUI();
	void ApplyMaterialsFunction(QString FunctionName);

public:

    CModesManager(CQParamsContainer *AttachedContainer, QString AppPath, CMiniSequencer *MiniSequencer, QString Name="ModesManager", bool RefreshUI = true);
    ~CModesManager(void);
    static void Init(CQParamsContainer *AttachedContainer, QString m_AppPath, CMiniSequencer* MiniSequencer);
    static void DeInit(void);
    static      CModesManager *Instance(void);
    TQErrCode   ActivateMode(QString ModeName, QString Directory, int CurrentArrayIndex);    

    // Refresh mode files list
    DEFINE_METHOD(CModesManager,TQErrCode,Refresh);

    DEFINE_METHOD(CModesManager,int,GetModesStackSize);
    DEFINE_METHOD(CModesManager,TQErrCode,BeginEnumeration);
    DEFINE_METHOD(CModesManager,QString,GetModesStackPath);
    DEFINE_METHOD(CModesManager,TQErrCode,GoOneModeBack);
	DEFINE_METHOD(CModesManager,TQErrCode,ExitAllModes);
    DEFINE_METHOD(CModesManager,TQErrCode,GotoDefaultMode);
    DEFINE_METHOD(CModesManager,QString,GetCurrentModeName);
    DEFINE_METHOD(CModesManager,QString,GetModesDirectoriesList);
    DEFINE_METHOD(CModesManager,TQErrCode,Save);
    DEFINE_METHOD_1(CModesManager,QString,GetNextModeName, QString /*Directory Name*/);
	DEFINE_V_METHOD_1(CModesManager,TQErrCode,LoadSelectedMaterials,QString);
    DEFINE_METHOD_2(CModesManager,TQErrCode,EnterMode,QString /*ModeName*/, QString /*Directory Name*/);
    DEFINE_METHOD_2(CModesManager,TQErrCode,ActivateMode,QString /*ModeName*/, QString /*Directory Name*/);
    DEFINE_METHOD_2(CModesManager,TQErrCode,EnableConfirmationBypass,QString /*ModeName*/, QString /*Directory Name*/);
    DEFINE_METHOD_2(CModesManager,TQErrCode,DisableConfirmationBypass,QString /*ModeName*/, QString /*Directory Name*/);

    TQErrCode SaveSingleParameter(CQParameterBase *ParamToSave);
	
	void EnumerateMaterialModes();
	
	QString GetModelResinType(int resin_index);
	QString GetServiceModelResinType(int resin_index);
	QString GetSupportResinType(int resin_index);
	QString GetServiceSupportResinType(int resin_index);
	
	bool IsServiceModelMaterial(QString material);
	bool IsServiceSupportMaterial(QString material);
	bool IsSupportMaterial(QString material);
	bool IsModelMaterial(QString material);
};

#endif
