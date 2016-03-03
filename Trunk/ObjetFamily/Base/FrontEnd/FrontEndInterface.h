/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT front-end interface.                                *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 28/01/2002                                           *
 * Last upate: 05/02/2002                                           *
 ********************************************************************/

#ifndef _FRONT_END_INTERFACE_H_
#define _FRONT_END_INTERFACE_H_

#include <map>
#include "QComponent.h"

class CRemoteFrontEndInterface;

#if defined(OS_WINDOWS) && !defined(EDEN_CL)
class TMainUIFrame;
#endif

typedef std::map<int,int> TIntShadow;
typedef std::map<int,float> TFloatShadow;
typedef std::map<int,QString> TStringShadow;
typedef std::map<int,bool> TEnableDisableShadow;

// Base class for the front-end interface
class CFrontEndInterface : public CQComponent
{
  private:
     unsigned int m_tidComm; //This member holds the TID of the comm. thread, (QA purposes)

  private:
    // Update a status in the front end (3 versions)
    void DoUpdateStatus(int ControlID,int Status);
    void DoUpdateStatus(int ControlID,float Status);
    void DoUpdateStatus(int ControlID,QString Status);

    void UpdateShadow(int ControlID,int Status);
    void UpdateShadow(int ControlID,float Status);
    void UpdateShadow(int ControlID,QString Status);
    void UpdateShadow(int ControlID,bool Status);

    // Enable/Disable a front-end UI control
    void DoEnableDisableControl(int ControlID,bool Enable);

  protected:
    // Each control type has a shadow array containing the last updated status
    TIntShadow m_IntShadow;
    TFloatShadow m_FloatShadow;
    TStringShadow m_StringShadow;
    TEnableDisableShadow m_EnableDisableShadow;
    bool                 m_DisplayMaterialWarningEnable;

    CRemoteFrontEndInterface *m_RemoteFrontEndInterface;

    DEFINE_VAR_PROPERTY(bool,RemoteModeEnabled);

#if defined(OS_WINDOWS) && !defined(EDEN_CL)
    TMainUIFrame *m_UIFrame;
    ULONG m_GUIMainThread;
#endif

  public:
    // Constructor
#if defined(OS_WINDOWS) && !defined(EDEN_CL)
    CFrontEndInterface(TMainUIFrame *UIFrame);
#endif
    CFrontEndInterface(void);

    // Destructor
    ~CFrontEndInterface(void);

	void RemoveStatus(int ControlID);
	
    // Update a status in the front end (3 versions)
    void UpdateStatus(int ControlID); //not refreshable
    void UpdateStatus(int ControlID,int     Status,bool ForceRefresh = false);
    void UpdateStatus(int ControlID,float   Status,bool ForceRefresh = false);
    void UpdateStatus(int ControlID,QString Status,bool ForceRefresh = false);

    // Enable/Disable a front-end UI control
    void EnableDisableControl(int ControlID,bool Enable,bool ForceRefresh = false);

    void MonitorPrint(QString Msg);

    // Show notification message
    void NotificationMessage(QString Msg);

    // Show warning message
    void WarningMessage(QString Msg);

    // Show error message
    void ErrorMessage(QString Msg);

    // Refresh all the statuses
    DEFINE_METHOD(CFrontEndInterface,TQErrCode,RefreshStatus);

    // Give the UI thread some time to run
    DEFINE_METHOD(CFrontEndInterface,TQErrCode,YieldUIThread);

    // Show the 'Cartridge error' dialog
    TQErrCode ShowCartridgeError(int DialogType, int Cartridge, bool Modal);
    TQErrCode InformStopToCartridgeDlg();
    TQErrCode ShowMaterialWarning(int Value);
    void EnableMaterialWarning(bool Enable);

    // TankIDNotice Dialog related functions:
    TQErrCode ShowTankIDNotice(int DialogType, int Cartridge, int Modal);
    TQErrCode SendMsgToTankIDNoticeDlg(int Cartridge, int Msg, bool Blocking);//bug 6258
	DEFINE_METHOD_1(CFrontEndInterface,TQErrCode,HideTankIDNoticeDlg,int);
    TQErrCode SuppressDialogPopupsTankIDNotice(int Cartridge);
    TQErrCode AllowDialogPopupsTankIDNotice(int Cartridge);
    QString GetTankIDNoticeText(int Cartridge);
	void SetDisabledIconHint(QString HintText);
	void LoadBitmap32(/* Graphics::TBitmap */ void *BMP, int ResourceID, /* TColor */ int BackGroundColor);
    static bool IsWizardRunning(void);
	static void CancelWizard(QString Msg);
};

#endif
