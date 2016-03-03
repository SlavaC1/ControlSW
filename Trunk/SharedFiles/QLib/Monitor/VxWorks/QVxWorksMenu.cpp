/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Menu)                                  *
 * Module Description: VxWorks implementation of the QMenu related  *
 *                     classes.                                     *
 *                                                                  *
 * Compilation: Standard C++, VxWorks                               *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 09/09/2001                                           *
 ********************************************************************/

#include <stdio.h>
#include "QMenu.h"
#include "QMonitor.h"


// Menu special key codes
const int CANCEL_KEY1 = 27;  // (ESC character)
const int CANCEL_KEY2 = '<';
const int HELP_KEY    = '?';
const int ENTER_KEY   = 10;


// Action class implementation
// ---------------------------------------------------------------------

// Constructor
CQMenuAction::CQMenuAction(const QString Title,char ACcelerator,
                           TGenericCallback Callback,TGenericCockie Cockie)
{
  m_Callback = Callback;
  m_Cockie = Cockie;
  SetAttributes(Title,ACcelerator);
}

// Execute the action
bool CQMenuAction::Execute(void)
{
  if(m_Callback)
    (*m_Callback)(m_Cockie);

  return true;    
}

// Seperator class implementation
// ---------------------------------------------------------------------

CQMenuSeperator::CQMenuSeperator(void)
{
  // Set the title and accelerator of the seperator
  SetAttributes("- - - - - - - - - - - - - - - - - - - - - - - - - - -",0);
}

bool CQMenuSeperator::Execute(void)
{
}

// Submenu class implementation
// ---------------------------------------------------------------------

// Constructor
CQSubMenu::CQSubMenu(const QString Title,char Accelerator)
{
  // Remember the title
  SetAttributes(Title,Accelerator);
}

// Destructor
CQSubMenu::~CQSubMenu(void)
{
  // Delete all items
  for(TMenuItemsList::iterator i = m_MenuItemsList.begin(); i != m_MenuItemsList.end(); i++)
    delete (*i);
}

// Draw the menu
bool CQSubMenu::Execute(void)
{
  // If no items presented in the menu return immediately
  if(m_MenuItemsList.empty())
    return true;

  Redraw();

  for(;;)
  {
    // Get a command character
    char Cmd = QMonitor.GetChar();

    Cmd = toupper(Cmd);

    if(Cmd == ENTER_KEY)
      continue;

    /* Check if the user wants to cancel the menu */
    switch(Cmd)
    {
      case CANCEL_KEY1:
      case CANCEL_KEY2:
        return false;

      /* Redraw the current menu */
      case HELP_KEY:
        Redraw();
        continue;
    }

    bool AcceleratorFound = false;

    /* Search the user key in the hotkeys list */
    for(TMenuItemsList::iterator i = m_MenuItemsList.begin(); i != m_MenuItemsList.end(); i++)
      if((*i)->GetAccelerator() == toupper(Cmd))
      {
        AcceleratorFound = true;

        // Execute the menu item
        if((*i)->Execute() == false)
          Redraw();
        break;
      }

      // Invalid command, print error (non-maskable)
      if(!AcceleratorFound)
        QMonitor.Print("Invalid command.",false);
  }
}

// Draw the menu on the monitor
void CQSubMenu::Redraw(void)
{
  QMonitor.Print("|------------------------------------------------------",false);
  QMonitor.PrintfNonMaskable("| %s",GetTitle().c_str());
  QMonitor.Print("|------------------------------------------------------",false);

  for(TMenuItemsList::iterator i = m_MenuItemsList.begin(); i != m_MenuItemsList.end(); i++)
  {
    // If the accelerator is zero, don't draw the item with the accelerator option
    if((*i)->GetAccelerator() == 0)
      QMonitor.PrintfNonMaskable("| %s",(*i)->GetTitle().c_str());
    else
      QMonitor.PrintfNonMaskable("| [%c] | %s",(*i)->GetAccelerator(),(*i)->GetTitle().c_str());
  }

  QMonitor.Print("|------------------------------------------------------",false);
}

// Add an action to the menu
void CQSubMenu::AddAction(TGenericCallback Callback,const QString Title,char Accelerator,TGenericCockie Cockie)
{
  // Create an action object and set it's title
  CQMenuAction *TempAction = new CQMenuAction(Title,Accelerator,Callback,Cockie);

  // Save the action in the list
  m_MenuItemsList.push_back(TempAction);
}

// Add a seperator to the menu
void CQSubMenu::AddSeperator(void)
{
  // Create a sperator object
  CQMenuSeperator *TempSeperator = new CQMenuSeperator;

  // Add the seperator in the items list
  m_MenuItemsList.push_back(TempSeperator);
}

// Add a submenu to the menu
void CQSubMenu::AddSubMenu(CQSubMenu *SubMenu)
{
  m_MenuItemsList.push_back(SubMenu);
}

// Activate the menu
void CQSubMenu::Activate(void)
{
  Execute();
}

