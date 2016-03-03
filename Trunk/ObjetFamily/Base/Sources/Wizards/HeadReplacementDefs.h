#ifndef _HEAD_REPLACMENT_DEFS_H_
#define _HEAD_REPLACMENT_DEFS_H_

const int   PASS_TIME                                = 3; // Sec
const int   PASS_PER_LAYER                           = 8; // Interlace
const int   SECONDS_IN_MINUTE                        = 60;
const int   TIME_ROUND_QUANTIZATION                  = 5;   // Minutes

#define GET_MATERIAL_FACTOR(_HEAD_,_OM_) \
(m_ParamMgr->MaterialsWeightFactorArray[GetHeadsChamber( _HEAD_ , _OM_)]* \
((m_ParamMgr->IsModelHead(_HEAD_)) ? m_ParamMgr->MaterialsWeightFactorPerModeArrayModel[_OM_] : m_ParamMgr->MaterialsWeightFactorPerModeArraySupport[_OM_]))

#if defined OBJET_500 || defined OBJET_350 //added OBJET_500, check if necessary
const int X_REPLACEMENT_POSITION   = 9000;
const int Y_REPLACEMENT_POSITION   = 6000;
#endif

#ifdef OBJET_260
const int X_REPLACEMENT_POSITION   = 4100;
const int Y_REPLACEMENT_POSITION   = 31000;
#endif

#endif
