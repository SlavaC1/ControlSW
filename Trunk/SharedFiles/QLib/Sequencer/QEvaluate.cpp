/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib.                                                   *
 * Module Description: Evaluate and execute an expression.          *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 27/04/2003                                           *
 * Last upate: 11/08/2003                                           *
 ********************************************************************/

#include <ctype>
#include "QEvaluate.h"
#include "QMonitor.h"


const int MAX_METHOD_ARGS = 10;


class CStringSerializer {
  private:
    QString m_Str;
    unsigned m_CurrentPos;

  public:
    CStringSerializer(const QString Str);
    void Assign(const QString Str);

    bool IsIdentifierChar(const char c);
    bool IsSpaceDelimeterChar(const char c);
    bool IsNumberChar(const char c);
    bool IsIdentifierNotNumberChar(const char c);

    bool GetNextChar(char& Chr);
    void UnGetChar(void);

    // Set internal index to point on first non-space character
    void SkipSpaces(void);

    // Return true if stream is done
    bool IsDone(void) {
      return (m_CurrentPos == m_Str.length());
    }
};

typedef enum {ttIdentifier,ttEqual,ttDot,ttLeftParenthesis,ttRightParenthesis,ttComma,
              ttNumber,ttString} TTokenType;

// Token base class
class CToken {
  protected:
    QString m_Str;

  public:
    virtual void GetToken(CStringSerializer& Serializer) = 0;

    const QString& Str(void) {
      return m_Str;
    }

    virtual TTokenType TokenType(void) = 0;
};

// Identifier token class
class CIdentifierToken : public CToken {
  public:
    TTokenType TokenType(void) {
      return ttIdentifier;
    }

    void GetToken(CStringSerializer& Serializer) {
      char c;
      bool StreamIsDone = false;

      for(;;)
      {
        if(Serializer.GetNextChar(c))
        {
          if(!Serializer.IsIdentifierChar(c))
            break;

          m_Str.append(1,c);
        } else
          {
            StreamIsDone = true;
            break;
          }
      }

      if(!StreamIsDone)
        Serializer.UnGetChar();
    }
};

// Number (integer or float) token class
class CNumberToken : public CToken {
  public:
    TTokenType TokenType(void) {
      return ttNumber;
    }

    void GetToken(CStringSerializer& Serializer) {
      char c;
      bool StreamIsDone = false;

      for(;;)
      {
        if(Serializer.GetNextChar(c))
        {
          if(!Serializer.IsNumberChar(c))
            break;

          m_Str.append(1,c);
        } else
          {
            StreamIsDone = true;
            break;
          }
      }

      if(!StreamIsDone)
      {
        // After a number must not appear an identifier
        if(Serializer.IsIdentifierChar(c))
          throw EQEvaluate("Invalid number format");

        Serializer.UnGetChar();
      }
    }
};

// Number (integer or float) token class
class CStringToken : public CToken {
  public:
    TTokenType TokenType(void) {
      return ttString;
    }

    void GetToken(CStringSerializer& Serializer) {
      char c;
      bool StringCloseFound = false;

      // Dump first token
      Serializer.GetNextChar(c);

      for(;;)
      {
        if(Serializer.GetNextChar(c))
        {
          if(c == '"')
          {
            StringCloseFound = true;
            break;
          }

          m_Str.append(1,c);
        } else
            break;
      }

      if(!StringCloseFound)
        throw EQEvaluate("String close mark not found");
    }
};

// Special character token class
class CSpecialCharToken : public CToken {
  private:
    TTokenType m_ActualTokenType;

  public:
    TTokenType TokenType(void) {
      return m_ActualTokenType;
    }

    void GetToken(CStringSerializer& Serializer) {
      char c;

      if(Serializer.GetNextChar(c))
        m_Str.append(1,c);

      switch(c)
      {
        case '=':
          m_ActualTokenType = ttEqual;
          break;

        case '.':
          m_ActualTokenType = ttDot;
          break;

        case ',':
          m_ActualTokenType = ttComma;
          break;

        case '(':
          m_ActualTokenType = ttLeftParenthesis;
          break;

        case ')':
          m_ActualTokenType = ttRightParenthesis;
          break;
      }
    }
};

CStringSerializer::CStringSerializer(const QString Str)
{
  m_Str = Str;
  m_CurrentPos = 0;
}

void CStringSerializer::Assign(const QString Str)
{
  m_Str = Str;
  m_CurrentPos = 0;
}

bool CStringSerializer::GetNextChar(char& Chr)
{
  if(m_CurrentPos < m_Str.length())
  {
    Chr = m_Str[m_CurrentPos];
    m_CurrentPos++;
    return true;
  }

  return false;
}

void CStringSerializer::UnGetChar(void)
{
  if(m_CurrentPos > 0)
    m_CurrentPos--;
}

bool CStringSerializer::IsIdentifierChar(const char c)
{
  return (isalpha(c) || isdigit(c) || (c == '_'));
}

bool CStringSerializer::IsNumberChar(const char c)
{
  return (isdigit(c) || (c == '.') || (c == '-'));
}

bool CStringSerializer::IsIdentifierNotNumberChar(const char c)
{
  return (isalpha(c) || (c == '_'));
}

bool CStringSerializer::IsSpaceDelimeterChar(const char c)
{
  return ((c == ' ') || (c == '\t') || (c == '\n'));
}

void CStringSerializer::SkipSpaces(void)
{
  while(m_CurrentPos < m_Str.length())
  {
    if(!IsSpaceDelimeterChar(m_Str[m_CurrentPos]))
      break;

    m_CurrentPos++;
  }
}

QString CQEvaluate::Evaluate(const QString Expression)
{
  TTokenList TokensList = CreateTokensList(Expression);

  QString Ret;

  try
  {
    Ret = ExecuteTokenList(TokensList);

  } catch(...)
    {
      ClearTokensList(TokensList);
      throw;
    }

  ClearTokensList(TokensList);  
  return Ret;
}

// Return tokens list data structure
CQEvaluate::TTokenList CQEvaluate::CreateTokensList(const QString Expression)
{
  TTokenList TokenList;
  CStringSerializer StringSerializer(Expression);

  CToken *Token;

  for(;;)
  {
    Token = GetNextToken(StringSerializer);

    if(Token != NULL)
      TokenList.push_back(Token);
    else
      break;
  }

  return TokenList;
}

// Extract the next token from a string serializer
CToken *CQEvaluate::GetNextToken(CStringSerializer& StringSerializer)
{
  StringSerializer.SkipSpaces();

  if(StringSerializer.IsDone())
    return NULL;

  char c;
  CToken *Token;

  StringSerializer.GetNextChar(c);
  StringSerializer.UnGetChar();

  // Decide on token type according to the first token character
  switch(c)
  {
    case '=':
    case '.':
    case ',':
    case '(':
    case ')':
      Token = new CSpecialCharToken;
      break;

    default:
      // Test other cases
      if(StringSerializer.IsIdentifierNotNumberChar(c))
        Token = new CIdentifierToken;
      else
        if(StringSerializer.IsNumberChar(c))
          Token = new CNumberToken;
        else
          if(c == '"')
            Token = new CStringToken;
          else
          {
            QString CharStr(1,c);
            throw EQEvaluate("Invalid character in expression ('" + CharStr + "')");
          }
  }

  Token->GetToken(StringSerializer);

  return Token;
}

// Execute a token list using the objects roster
QString CQEvaluate::ExecuteTokenList(const TTokenList& TokenList)
{
  QString ObjectName,FieldName;
  QString Args[10];
 

  // If no token...
  if(TokenList.size() == 0)
    return "";

  // Minimum number of tokens is 5
  if(TokenList.size() < 3)
    throw EQEvaluate("Invalid expression");

  // The first token must be an identifier
  if(TokenList[0]->TokenType() != ttIdentifier)
    throw EQEvaluate("Invalid expression format");

  // The second token must be a '.'
  if(TokenList[1]->TokenType() != ttDot)
    throw EQEvaluate("Invalid expression format");

  // The third token must be an identifier
  if(TokenList[2]->TokenType() != ttIdentifier)
    throw EQEvaluate("Invalid expression format");

  QString Result;

  // If no more tokens are presented, interpret expression as a property evaluation
  if(TokenList.size() == 3)
    return EvaluateProperty(TokenList[0]->Str(),TokenList[2]->Str());

  // The expression can be an assignment or a method call
  if(TokenList.size() < 5)
    throw EQEvaluate("Invalid expression");

  // Is it assignmnet
  if(TokenList[3]->TokenType() == ttEqual)
  {
    Result = ExecutePropertyAssignment(TokenList[0]->Str(),TokenList[2]->Str(),TokenList[4]->Str());
  } else
      // Method call
      if(TokenList[3]->TokenType() == ttLeftParenthesis)
      {
        unsigned CurrentTokenNum = 4;
        bool GoOut = false;
        bool WaitForComma = false;

        // Prepare arguments list
        int i;
        for(i = 0; (i < MAX_METHOD_ARGS) && !GoOut; CurrentTokenNum++)
        {
          if(CurrentTokenNum == TokenList.size())
            throw EQEvaluate("Right parethesis could not be found");

          TTokenType TmpType = TokenList[CurrentTokenNum]->TokenType();

          switch(TmpType)
          {
            case ttRightParenthesis:
              GoOut = true;
              break;

            case ttIdentifier:
            case ttNumber:
            case ttString:
              if(!WaitForComma)
              {
                Args[i] = TokenList[CurrentTokenNum]->Str();
                i++;
                WaitForComma = true;
              } else
                  throw EQEvaluate("Method arguments must be seperated with comma");
              break;

            case ttComma:
              WaitForComma = false;
              break;
          }
        }

        // Execute method
        Result = ExecuteMethodCall(TokenList[0]->Str(),TokenList[2]->Str(),Args,i);
      } else
          // Error
          throw EQEvaluate("Invalid expression format");

  return Result;
}

// Execute a property assignment
QString CQEvaluate::ExecutePropertyAssignment(const QString ObjectName,const QString PropName,const QString PropValue)
{
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();
  CQComponent *Component = Roster->FindComponent(ObjectName);

  // If it is not an object, return the value input as output
  if(Component == NULL)
    throw EQEvaluate("Object \"" + ObjectName + "\" could not be found");

  CQPropertyObject *Property = Component->FindProperty(PropName);

  if(Property == NULL)
    throw EQEvaluate("Property \"" + PropName + "\" could not be found in object \"" + ObjectName + "\"");

  Property->AssignFromString(PropValue);

  return Property->ValueAsString();
}

// Return the value of a given property
QString CQEvaluate::EvaluateProperty(const QString ObjectName,const QString PropName)
{
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();
  CQComponent *Component = Roster->FindComponent(ObjectName);

  // If it is not an object, return the value input as output
  if(Component == NULL)
    throw EQEvaluate("Object \"" + ObjectName + "\" could not be found");

  CQPropertyObject *Property = Component->FindProperty(PropName);

  if(Property == NULL)
    throw EQEvaluate("Property \"" + PropName + "\" could not be found in object \"" + ObjectName + "\"");

  return Property->ValueAsString();
}

// Execute a method call
QString CQEvaluate::ExecuteMethodCall(const QString ObjectName,const QString MethodName,const QString *Args,int ArgsNum)
{
  CQObjectsRoster *Roster = CQObjectsRoster::Instance();

  // Invoke the method
  return Roster->InvokeMethod(ObjectName,MethodName,Args,ArgsNum);
}

// Free all the tokens in a tokens list
void CQEvaluate::ClearTokensList(TTokenList& TokenList)
{
  for(TTokenList::iterator i = TokenList.begin(); i != TokenList.end(); ++i)
    delete (*i); 
}
