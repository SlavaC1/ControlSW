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
 * Author: Ran Peleg.                                               *
 * Start date: 16/08/2001                                           *
 * Last upate: 28/01/2002                                           *
 ********************************************************************/

#ifndef _Q2RT_ERRORS_H_
#define _Q2RT_ERRORS_H_

#include "QComponent.h"
#include "QErrors.h"
#include "QException.h"

class ETimeout : public EQException
{
public:
   ETimeout(QString msg) : EQException(msg) {}
};

#define DEFINE_ERROR(_err_, _desc_) _err_##_ERROR_ID,
enum
{
  #include "Q2RTErrorsDefsList.h"
  NUM_OF_Q2RT_USER_ERRORS,
};
#undef DEFINE_ERROR

#define DEFINE_ERROR(_err_, _desc_) const int Q2RT_##_err_ = Q_USER_ERROR - _err_##_ERROR_ID;
#include "Q2RTErrorsDefsList.h"
#define VALIDATE_Q2RT_ERROR(_err_)  ((Q_USER_ERROR-NUM_OF_Q2RT_USER_ERRORS < _err_) && (Q_USER_ERROR >= _err_))
#define TOGGLE_Q2RT_ERROR_ID(_err_)  (Q_USER_ERROR - _err_)
#undef DEFINE_ERROR


//This procedure receive an Error code and return the message text
QString PrintErrorMessage(int Err);

#endif

