/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib utilities.                                          *
 * Module Description: Multi language string translator.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/07/2003                                           *
 * Last upate: 27/07/2003                                           *
 ********************************************************************/

#include "QMultiLang.h"


class CQStringTranslator {
  private:
    typedef std::map<QString,QString> TStrLookup;

    TStrLookup m_StrLookup;

  public:
    // Constructor
    CQStringTranslator(CQTranslatorStringStream *TranslatorStream) {
      if(TranslatorStream != NULL)
      {
        TranslatorStream->Begin();

        CQTranslatorStringStream::TStrPair StrPair;

        // Initialize the strings map
        for(;;)
        {
          StrPair = TranslatorStream->GetNext();

          if(!StrPair.first.empty())
            m_StrLookup[StrPair.first] = StrPair.second;
          else
            break;
        }

        TranslatorStream->End();
      }
    }

    // Translate a single string
    QString TranslateStr(const QString& Str) {
      QString Result;
      
      TStrLookup::iterator i = m_StrLookup.find(Str);

      // Check if found
      if(i != m_StrLookup.end())
        Result = (*i).second;
      else
        Result = Str;

      // Not found, return input string
      return Result;
    }

} *gTranslator;


// Initialize the translator with a specific translator string stream
void QInitMultiLangTranslator(CQTranslatorStringStream *TranslatorStream)
{
  // Initialize the global translator object
  if(gTranslator == NULL)
    gTranslator = new CQStringTranslator(TranslatorStream);
}

// Clean-up
void QDeInitMultiLangTranslator(void)
{
  if(gTranslator)
    delete gTranslator;
}

// String replace translator
QString QXlt(const QString Str)
{
  return gTranslator->TranslateStr(Str);
}

