#include "ParamsBlock.h"


CParamsBlock::CParamsBlock(const QString CfgFileName) : CQParamsContainer(NULL,"ParamsBlock")
{
  INIT_PARAM(FromDateTime,"","Options");
  INIT_PARAM(ToDateTime,"","Options");
  INIT_PARAM(IncludeTagsStr,"","Options");
  INIT_PARAM(ExcludeTagsStr,"","Options");
  INIT_PARAM(IncludeAnyStringStr,"","Options");
  INIT_PARAM(ExcludeAnyStringStr,"","Options");
  INIT_PARAM(IncludeRegExStr,"","Options");
  INIT_PARAM(ExcludeRegExStr,"","Options");
  INIT_PARAM(FromRangeOption,0,"Options");
  INIT_PARAM(ToRangeOption,0,"Options");
  INIT_PARAM(FromSessionCounter,0,"Options");
  INIT_PARAM(ToSessionCounter,0,"Options");
  INIT_PARAM(IncludeErrors,false,"Options");
  INIT_PARAM(ExcludeErrors,false,"Options");
  INIT_PARAM(IncludeWarnings,false,"Options");
  INIT_PARAM(ExcludeWarnings,false,"Options");
  INIT_PARAM(IncludeTags,false,"Options");
  INIT_PARAM(ExcludeTags,false,"Options");
  INIT_PARAM(IncludeAnyString,false,"Options");
  INIT_PARAM(ExcludeAnyString,false,"Options");
  INIT_PARAM(IncludeRegEx,false,"Options");
  INIT_PARAM(ExcludeRegEx,false,"Options");

  // Create parameters stream for the parameters file
  m_AssignedParamsStream = new CQParamsFileStream(CfgFileName);

  // Assign the stream to the container
  AssignParamsStream(m_AssignedParamsStream);
}

// Destructor
CParamsBlock::~CParamsBlock(void)
{
  delete m_AssignedParamsStream;
}

