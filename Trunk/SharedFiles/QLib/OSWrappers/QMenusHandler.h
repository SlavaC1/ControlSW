/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Menus handler)                         *
 * Module Description: Cross platform implementation of managing    *
 *                     class for monitor menus.                     *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/08/2001                                           *
 * Last upate: 09/09/2001                                           *
 ********************************************************************/

#ifndef _Q_MENUS_HANDLER_H_
#define _Q_MENUS_HANDLER_H_

#include "QMenu.h"
#include "QThread.h"


/* Implementation notes
   --------------------
   This class (menus handler) is implemented as a thread on VxWorks and as a simple object on
   windows. To use this class call the the static function Init() and to DeInit() for
   closing.
*/

class CQMenusHandler
#ifdef OS_WINDOWS
 : public CQObject
#elif defined(OS_VXWORKS)
 : public CQThread
#endif
  {
  private:
    // If true, the menus are enabled
    bool m_Enabled;

    // Pointer to the user root menu
    CQSubMenu *m_UserRootMenu;

#ifdef OS_WINDOWS
    TMenuItem *m_MonitorBaseMenuItem;
#elif defined(OS_VXWORKS)
    // Thread execute function
    void Execute(void);
#endif

    // Private constructor - an instance must be created by the static function Init()
    CQMenusHandler(CQSubMenu *UserRootMenu);

    // Static pointer to the singleton object
    static CQMenusHandler *m_MenusHandlerInstance;

  public:

    // Destructor
    ~CQMenusHandler(void);

    // Init the menus handler
    static void Init(CQSubMenu *UserRootMenu);

    // DeInit the menus handler
    static void DeInit(void);

    // Enable/Disable the menus system (return old enable state)
    static bool Enable(bool EnableFlag);

#ifdef OS_VXWORKS
    // Wait for the compeletion of the menus handler thread
    static void WaitForCompletion(void);
#endif
};

#endif
