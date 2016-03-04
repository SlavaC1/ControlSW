/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib parameters stream for disk files.                   *
 * Module Description: Implement the base class CQParamsStream for  *
 *                     standard files I/O.                          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 04/07/2001                                           *
 * Last upate: 23/08/2001                                           *
 ********************************************************************/

#include "QMonitorParamsStream.h"
#include "QMonitor.h"


// Start the write to the monitor
void CQMonitorParamsStream::StartWrite(void)
{
  QMonitor.Print("Parameters list");
  QMonitor.Print("--------------------------------------------------------------");
}

// End the write to the monitor
void CQMonitorParamsStream::EndWrite(void)
{
  QMonitor.Print("--------------------------------------------------------------");
}

// Write a single parameter
void CQMonitorParamsStream::WriteParameter(CQParameterBase *Param)
{
  // If the parameter is hidden, don't show it at all
  if(Param->GetAttributes() & paHidden)
    return;

  if(Param->GetLimitsMode())
    // Parameter has limits
    QMonitor.Printf("%s = %s   Default: %s , Range: (%s - %s)",
                    Param->Name().c_str(),
                    Param->ValueAsString().c_str(),
                    Param->DefaultValueAsString().c_str(),
                    Param->MinLimitAsString().c_str(),
                    Param->MaxLimitAsString().c_str());
  else
    QMonitor.Printf("%s = %s   Default: %s",
                    Param->Name().c_str(),
                    Param->ValueAsString().c_str(),
                    Param->DefaultValueAsString().c_str());
}

// Read a single parameter
void CQMonitorParamsStream::ReadParameter(CQParameterBase */*Param*/)
{
  // Not implemented
}

