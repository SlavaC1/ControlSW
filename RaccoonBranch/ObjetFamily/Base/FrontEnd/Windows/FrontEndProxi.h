/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT                                                    *
 * Module: Q2RT Front-End proxi class for remote interface with a   *
 *         UI frame.                                                *
 *                                                                  *
 * Compilation: BCB.                                                *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 05/02/2002                                           *
 * Last upate: 05/02/2002                                           *
 ********************************************************************/

#ifndef _FRONT_END_PROXI_

#include "QComponent.h"


class TMainUIFrame;

// Proxi class for front-end RPC communication
class CFrontEndProxi : public CQComponent {
  private:
    TMainUIFrame *m_UIFrame;

    DEFINE_METHOD_2(CFrontEndProxi,int,UpdateStatusInt,int /*ControlID*/,int /*Status*/);
    DEFINE_METHOD_2(CFrontEndProxi,int,UpdateStatusFloat,int /*ControlID*/,float /*Status*/);
    DEFINE_METHOD_2(CFrontEndProxi,int,UpdateStatusStr,int /*ControlID*/,QString /*Status*/);

    DEFINE_METHOD_2(CFrontEndProxi,int,EnableDisableControl,int /*ControlID*/,bool /*Status*/);

    DEFINE_METHOD_1(CFrontEndProxi,int,MonitorPrint,QString /*Msg*/);

    DEFINE_METHOD_1(CFrontEndProxi,int,NotificationMessage,QString /*Msg*/);
    DEFINE_METHOD_1(CFrontEndProxi,int,WarningMessage,QString /*Msg*/);
    DEFINE_METHOD_1(CFrontEndProxi,int,ErrorMessage,QString /*Msg*/);

  public:
    // Constructor
    CFrontEndProxi(TMainUIFrame *UIFrame);
};

#endif


