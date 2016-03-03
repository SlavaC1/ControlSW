/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers (Menu)                                  *
 * Module Description: Windows implementation of the QMenu related  *
 *                     classes.                                     *
 *                                                                  *
 * Compilation: Win32, BCB                                          *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 02/08/2001                                           *
 * Last upate: 12/08/2001                                           *
 ********************************************************************/

#include "QMenu.h"


// Action class implementation
// ---------------------------------------------------------------------

// Constructor
CQMenuAction::CQMenuAction(const QString Title,char Accelerator,
                           TGenericCallback Callback,TGenericCockie Cockie)
{
  m_Callback = Callback;
  m_Cockie = Cockie;
  SetAttributes(Title,Accelerator);
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

bool CQMenuSeperator::Execute(void)
{
  // Non-implemented on windows
  return true;
}

// Submenu class implementation
// ---------------------------------------------------------------------

// Constructor
CQSubMenu::CQSubMenu(const QString Title,char /*Accelerator*/)
{
  // Create a windows menu and set it's caption
  m_WinMenuItem = new TMenuItem(NULL);

  m_WinMenuItem->Caption = Title.c_str();
}

// Destructor
CQSubMenu::~CQSubMenu(void)
{
  // Delete all items
  for(TMenuItemsList::iterator i = m_MenuItemsList.begin(); i != m_MenuItemsList.end(); ++i)
    delete (*i);
}

bool CQSubMenu::Execute(void)
{
  // Non-implemented on windows
  return true;  
}

// Add an action to the menu
void CQSubMenu::AddAction(TGenericCallback Callback,const QString Title,char Accelerator,TGenericCockie Cockie)
{
  // Create an action object and set it's title
  CQMenuAction *TempAction = new CQMenuAction(Title,Accelerator,Callback,Cockie);

  // Save the action in the list
  m_MenuItemsList.push_back(TempAction);

  // Create a new menu item
  TMenuItem *TmpWinMenuItem = new TMenuItem(NULL);
  TmpWinMenuItem->Caption = Title.c_str();
  TmpWinMenuItem->Tag = reinterpret_cast<int>(TempAction);
  TmpWinMenuItem->OnClick = WindowsMenuItemClick;

  // Add to the the parent menu item
  m_WinMenuItem->Add(TmpWinMenuItem);
}

// Add a seperator to the menu
void CQSubMenu::AddSeperator(void)
{
  // Create a seperator menu item
  TMenuItem *TmpWinMenuItem = new TMenuItem(NULL);
  TmpWinMenuItem->Caption = '-';

  // Add to the the parent menu item
  m_WinMenuItem->Add(TmpWinMenuItem);
}

// Add a submenu to the menu
void CQSubMenu::AddSubMenu(CQSubMenu *SubMenu)
{
  // Add to the items list
  m_MenuItemsList.push_back(SubMenu);

  SubMenu->Activate(m_WinMenuItem);
}

// Activate the menu
void CQSubMenu::Activate(TMenuItem *BaseMenuItem)
{
  BaseMenuItem->Add(m_WinMenuItem);
}

// Event handler for the windows menu events
void __fastcall CQSubMenu::WindowsMenuItemClick(TObject *Sender)
{
  // Get a pointer to the QMenuItem object
  TMenuItem *WinMenuItem = dynamic_cast<TMenuItem *>(Sender);
  CQMenuItem *QMenuItem = reinterpret_cast<CQMenuItem *>(WinMenuItem->Tag);

  // Execute the menu item  
  QMenuItem->Execute();
}



