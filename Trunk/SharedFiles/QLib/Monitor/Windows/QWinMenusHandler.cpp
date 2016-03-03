/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Menus handler)                         *
 * Module Description: Windows implementation of the CQMenusHandler *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Win32, BCB                                          *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 05/08/2001                                           *
 ********************************************************************/

#include "QMenusHandler.h"
#include "QMonitorDlg.h"


// Static pointer to the singleton object
CQMenusHandler *CQMenusHandler::m_MenusHandlerInstance = NULL;


// Private constructor - an instance must be created by the static function Init()
CQMenusHandler::CQMenusHandler(CQSubMenu *UserRootMenu)
{
  // Remember the user root menu internally
  m_UserRootMenu = UserRootMenu;
                                                             
  // Attach the user root menu to the monitor base menu
  TMenuItem *m_MonitorBaseMenuItem = TQMonitorForm::Instance()->AddQMenuUserOptions();
  UserRootMenu->Activate(m_MonitorBaseMenuItem);
}

// Destructor
CQMenusHandler::~CQMenusHandler(void)
{
  // Delete the user menus tree
  delete m_UserRootMenu;
}

// Init the menus handler (static function)
void CQMenusHandler::Init(CQSubMenu *UserRootMenu)
{
  // Ignore double init
  if(m_MenusHandlerInstance == NULL)
    m_MenusHandlerInstance = new CQMenusHandler(UserRootMenu);
}

// DeInit the menus handler (static function)
void CQMenusHandler::DeInit(void)
{
  if(m_MenusHandlerInstance)
    delete m_MenusHandlerInstance;
}

// Enable/Disable the menus system (return old enabled state)
bool CQMenusHandler::Enable(bool EnableFlag)
{
  // Remember old value
  bool OldFlag = m_MenusHandlerInstance->m_Enabled;

  // Set new enable state
  m_MenusHandlerInstance->m_Enabled = EnableFlag;

  return OldFlag;
}


