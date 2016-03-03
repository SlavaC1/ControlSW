/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib.                                                   *
 * Module Description: QLib generic sequencer.                      *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 30/08/2001                                           *
 * Last upate: 30/08/2001                                           *
 ********************************************************************/

#include "QSequencer.h"

// Constructor
CQSequencer::CQSequencer(const QString& Name) : CQComponent(Name)
{
  INIT_PROPERTY(CQSequencer,Canceled,SetCanceled,GetCanceled);

  m_CancelFlag = false;
}

// Destructor
CQSequencer::~CQSequencer(void)
{
}

// Cancel the current sequence
void CQSequencer::Cancel(void)
{
  // Set the cancel flag
  m_CancelFlag = true;

  // Call the user version of OnCancel
  OnCancel();
}




