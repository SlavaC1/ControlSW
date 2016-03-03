/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Menus handler)                         *
 * Module Description: VxWorks implementation of the CQMenusHandler *
 *                     class.                                       *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 10/09/2001                                           *
 ********************************************************************/

#include <ctype.h>
#include "QMenusHandler.h"
#include "QThreadUtils.h"


// Static pointer to the singleton object
CQMenusHandler *CQMenusHandler::m_MenusHandlerInstance = NULL;


// Private constructor - an instance must be created by the static function Init()
// Note: This thread is nor registered in the objects roster
CQMenusHandler::CQMenusHandler(CQSubMenu *UserRootMenu) : CQThread(false,"MenusHandler",true)
{
  // Remember the user root menu internally
  m_UserRootMenu = UserRootMenu;
}

// Destructor
CQMenusHandler::~CQMenusHandler(void)
{
  // Delete the user menus tree
  delete m_UserRootMenu;
}

// Thread execute function
void CQMenusHandler::Execute(void)
{
  // Activate the root menu (forever)
  for(;;)
    m_UserRootMenu->Activate();
}

// Init the menus handler (static function)
void CQMenusHandler::Init(CQSubMenu *UserRootMenu)
{
  m_MenusHandlerInstance = new CQMenusHandler(UserRootMenu);
}

// DeInit the menus handler (static function)
void CQMenusHandler::DeInit(void)
{
  delete m_MenusHandlerInstance;
}

// Enable/Disable the menus system (return old enabled state)
bool CQMenusHandler::Enable(bool EnableFlag)
{
  // Remember old value
  bool OldFlag = m_MenusHandlerInstance->m_Enabled;

  // Set the thread to suspend/resume according to the enable flag
  if(EnableFlag)
    m_MenusHandlerInstance->Resume();
  else
    m_MenusHandlerInstance->Suspend();

  return OldFlag;
}

// Wait for the compeletion of the menus handler thread
void CQMenusHandler::WaitForCompletion(void)
{
  m_MenusHandlerInstance->WaitFor();
}
