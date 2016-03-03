#ifndef _PARAMS_BLOCK_H_
#define _PARAMS_BLOCK_H_

#include "QParamsContainer.h"
#include "QFileParamsStream.h"


class CParamsBlock : public CQParamsContainer {
  private:
    CQParamsFileStream *m_AssignedParamsStream;

  public:
    // Constructor
    CParamsBlock(const QString CfgFileName);

    // Destructor
    ~CParamsBlock(void);

    // Parameters declarations starts here
    // -----------------------------------
    DEFINE_PARAM(QString,FromDateTime);
    DEFINE_PARAM(QString,ToDateTime);
    DEFINE_PARAM(QString,IncludeTagsStr);
    DEFINE_PARAM(QString,ExcludeTagsStr);
    DEFINE_PARAM(QString,IncludeAnyStringStr);
    DEFINE_PARAM(QString,ExcludeAnyStringStr);
    DEFINE_PARAM(QString,IncludeRegExStr);
    DEFINE_PARAM(QString,ExcludeRegExStr);
    DEFINE_PARAM(int,FromRangeOption);
    DEFINE_PARAM(int,ToRangeOption);
    DEFINE_PARAM(int,FromSessionCounter);
    DEFINE_PARAM(int,ToSessionCounter);
    DEFINE_PARAM(bool,IncludeErrors);
    DEFINE_PARAM(bool,ExcludeErrors);
    DEFINE_PARAM(bool,IncludeWarnings);
    DEFINE_PARAM(bool,ExcludeWarnings);
    DEFINE_PARAM(bool,IncludeTags);
    DEFINE_PARAM(bool,ExcludeTags);
    DEFINE_PARAM(bool,IncludeAnyString);
    DEFINE_PARAM(bool,ExcludeAnyString);
    DEFINE_PARAM(bool,IncludeRegEx);
    DEFINE_PARAM(bool,ExcludeRegEx);
};

#endif

