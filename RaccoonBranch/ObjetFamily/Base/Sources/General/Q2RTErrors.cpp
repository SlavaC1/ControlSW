/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT errors.                                             *
 * Module Description: Q2RT application related errors (starting    *
 *                     from Q_USER_ERROR downwards).                *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Gedalia Trejger                                          *
 * Start date: 11/09/2002                                           *
 ********************************************************************/

#include "Q2RTErrors.h"
#include "QMonitor.h"

#define DEFINE_ERROR(_err_, _desc_) _desc_,
char* Q2RTErrorsDescription[NUM_OF_Q2RT_USER_ERRORS+1] =
{
#include "Q2RTErrorsDefsList.h"
"Error message not defined "
};
#undef DEFINE_ERROR

//This procedure receive an Axis and return the value in string:X,Y,Z, or T
QString PrintErrorMessage(TQErrCode Err)
{
  QString str = (VALIDATE_Q2RT_ERROR(Err) ? Q2RTErrorsDescription[TOGGLE_Q2RT_ERROR_ID(Err)] :
                                            Q2RTErrorsDescription[NUM_OF_Q2RT_USER_ERRORS]);
  return str + QIntToStr(Err);
}



