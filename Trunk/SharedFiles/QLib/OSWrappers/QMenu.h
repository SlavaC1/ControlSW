/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Menu)                                  *
 * Module Description: Cross platform simple menus services.        *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 09/09/2001                                           *
 ********************************************************************/

#ifndef _Q_MENU_H_
#define _Q_MENU_H_

#ifdef OS_WINDOWS

// In windows, this module must be compiled with C++ builder
#ifndef __BCPLUSPLUS__
#error QLib: The QMenus module must be compiled with Borland C++ Builder
#endif

// VCL menus header
#include <menus.hpp>
#endif

#include <vector>
#include "QMonitor.h"


// Abstract base menu item class
class CQMenuItem : public CQObject {
  private:
    QString m_Title;
    char m_Accelerator;

  public:
    void SetAttributes(const QString Title,char Accelerator) {
      m_Title = Title;
      m_Accelerator = Accelerator;
    }

    // Return the title string
    QString& GetTitle(void) {
      return m_Title;
    }

    // Return the accelerator character
    char GetAccelerator(void) {
      return m_Accelerator;
    }

    // Execute the menu item action (return false if canceled)
    virtual bool Execute(void) = 0;
};

// Action class
class CQMenuAction : public CQMenuItem {
  private:
    // Callback to the menu action implementation
    TGenericCallback m_Callback;
    TGenericCockie m_Cockie;

  public:
    // Constructor
    CQMenuAction(const QString Title,char Accelerator = 0,
                 TGenericCallback Callback = 0,TGenericCockie Cockie = 0);

    bool Execute(void);
};

// Seperator class
class CQMenuSeperator : public CQMenuItem {
  public:
    // Constructor
    CQMenuSeperator(void);

    bool Execute(void);
};

// Submenu class
class CQSubMenu : public CQMenuItem {
  private:
    // A sub menu hold a list of menu items
    typedef std::vector<CQMenuItem *> TMenuItemsList;

    // Menu items list
    TMenuItemsList m_MenuItemsList;

#ifdef OS_WINDOWS
    // The actual menu item for windows
    TMenuItem *m_WinMenuItem;
#elif defined(OS_VXWORKS)

    // Draw the menu on the monitor
    void Redraw(void);
#endif

    bool Execute(void);

  public:

    // Constructor
    CQSubMenu(const QString Title,char Accelerator = 0);

    // Destructor
    ~CQSubMenu(void);

    // Add an action to the menu
    void AddAction(TGenericCallback Callback,const QString Title,char Accelerator = 0,
                   TGenericCockie Cockie = 0);

    // Add a seperator to the menu
    void AddSeperator(void);

    // Add a submenu to the menu
    void AddSubMenu(CQSubMenu *SubMenu);

    // Activate the menu
#ifdef OS_WINDOWS
    void Activate(TMenuItem *BaseMenuItem);

    // VCL event for menu click (for all menu items)
    void __fastcall WindowsMenuItemClick(TObject *Sender);
#elif defined(OS_VXWORKS)

    void Activate(void);
#endif
};

#endif


