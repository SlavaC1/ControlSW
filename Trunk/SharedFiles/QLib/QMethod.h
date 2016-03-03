/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib method class.                                       *
 * Module Description: By using this class a dynamic method can be  *
 *                     created. This method implemented as a        *
 *                     "hidden" member in the class (the class user *
 *                     need only to know the standrad member        *
 *                     function). This method can be invoked by using
 *                     the strings parameters interface.            *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 01/07/2001                                           *
 * Last upate: 18/09/2003                                           *
 ********************************************************************/

#ifndef _Q_METHOD_H_
#define _Q_METHOD_H_

#include "QProperty.h"


/* Implementation notes
   --------------------
   1) Unfortunately C++ does not support variable number of template argumnets. Because of this
      limitation there is no elegant way (at least I couldn't find one) to implement method class
      that support different numbers of arguments. The simple and brute force solution that is used
      here is to define a different class for each number of arguments.
   2) A method can not be defined with a void return value.
*/

// Maximum arguments for a method
const int MAX_QMETHOD_ARGUMENTS = 8;

// Exception class for method objects
class EQMethod : public EQException {
  public:
    EQMethod(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// Method base class
class CQMethodObject : public CQObject {
  private:

    // Name of the method
    QString m_Name;

  public:

    // Basic method initialization
    void InitMethod(const QString& Name) {
      m_Name = Name;
    }

    // Return the method name
    QString& Name(void) {
      return m_Name;
    }

    // Return the number of method arguments
    virtual unsigned GetMethodArgumentsCount(void) {
      return 0;
    }

    // Return the method return type signature
    virtual TPropertyTypeID GetReturnTypeID(void) = 0;

    // Invoke the method
    virtual QString Invoke(const QString */*Args*/,unsigned /*ArgsNum*/) = 0;
};


// The custom RTTI class needs to know two types - the owner class type and the RTTI element value type
template <class TReturnType>
class CQCustomMethod : public CQMethodObject {
  protected:

    // Utility function for converting a value from type TRTTIValue to a string
    QString ValueToString(const TReturnType& v) {
      // return as QString
      return QValueToStr(v);
    }

    // Utility function for converting a value from type TRTTIValue to a string
    TReturnType StringToValue(const QString& s) {
      TReturnType TmpValue = QStrToValue<TReturnType>(s);

      return TmpValue;
    }

  public:
    // Constructor (default)
    CQCustomMethod(const QString& Name="") {
      InitMethod(Name);
    }

    // Return the method return type signature
    TPropertyTypeID GetReturnTypeID(void) {
      return TypeIDSelector<TReturnType>();
    }
};

// Specialized version of conversion values

template<>
QString CQCustomMethod<QString>::ValueToString(const QString& v)
{
  return v;
}

template<>
QString CQCustomMethod<QString>::StringToValue(const QString& v)
{
  return v;
}

// Implementation of a dynamic method wrapper - no arguments
template <class COwnerClass,class TReturnType>
class CQMethod0 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(void);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 0;
    }

  public:
    // Constructor (default)
    CQMethod0(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString */*Args*/,unsigned ArgsNum) {
      // This method implementation can accept only 1 parameter
      if(ArgsNum != 0)
        throw EQMethod("Invalid number of arguments for method call (no arguments expected)");

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)());
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,const QString& Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 1 argument
template <class COwnerClass,class TReturnType,class TArg1>
class CQMethod1 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 1;
    }

  public:
    // Constructor (default)
    CQMethod1(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 1 parameter
      if(ArgsNum != 1)
        throw EQMethod("Invalid number of arguments for method call (1 argument expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,const QString& Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 2 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2>
class CQMethod2 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 2;
    }

  public:
    // Constructor (default)
    CQMethod2(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 2 parameters
      if(ArgsNum != 2)
        throw EQMethod("Invalid number of arguments for method call (2 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,const QString& Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 3 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3>
class CQMethod3 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 3;
    }

  public:
    // Constructor (default)
    CQMethod3(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 3 parameters
      if(ArgsNum != 3)
        throw EQMethod("Invalid number of arguments for method call (3 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,QString Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 4 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3,class TArg4>
class CQMethod4 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3,TArg4);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;
    CQVariableProperty<TArg4> m_Arg4;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 4;
    }

  public:
    // Constructor (default)
    CQMethod4(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 4 parameters
      if(ArgsNum != 4)
        throw EQMethod("Invalid number of arguments for method call (4 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);
      m_Arg4.AssignFromString(Args[3]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3,(TArg4)m_Arg4));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,QString Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 5 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3,class TArg4,class TArg5>
class CQMethod5 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3,TArg4,TArg5);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;
    CQVariableProperty<TArg4> m_Arg4;
    CQVariableProperty<TArg5> m_Arg5;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 5;
    }

  public:
    // Constructor (default)
    CQMethod5(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 5 parameters
      if(ArgsNum != 5)
        throw EQMethod("Invalid number of arguments for method call (5 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);
      m_Arg4.AssignFromString(Args[3]);
      m_Arg5.AssignFromString(Args[4]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3,(TArg4)m_Arg4,(TArg5)m_Arg5));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,const QString& Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 6 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3,class TArg4,class TArg5,class TArg6>
class CQMethod6 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3,TArg4,TArg5,TArg6);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;
    CQVariableProperty<TArg4> m_Arg4;
    CQVariableProperty<TArg5> m_Arg5;
    CQVariableProperty<TArg6> m_Arg6;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 6;
    }

  public:
    // Constructor (default)
    CQMethod6(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 6 parameters
      if(ArgsNum != 6)
        throw EQMethod("Invalid number of arguments for method call (6 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);
      m_Arg4.AssignFromString(Args[3]);
      m_Arg5.AssignFromString(Args[4]);
      m_Arg6.AssignFromString(Args[5]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3,
                                                          (TArg4)m_Arg4,(TArg5)m_Arg5,(TArg6)m_Arg6));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,QString Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 7 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3,class TArg4,class TArg5,class TArg6,class TArg7>
class CQMethod7 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3,TArg4,TArg5,TArg6,TArg7);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;
    CQVariableProperty<TArg4> m_Arg4;
    CQVariableProperty<TArg5> m_Arg5;
    CQVariableProperty<TArg6> m_Arg6;
    CQVariableProperty<TArg7> m_Arg7;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 7;
    }

  public:
    // Constructor (default)
    CQMethod7(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 7 parameters
      if(ArgsNum != 7)
        throw EQMethod("Invalid number of arguments for method call (7 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);
      m_Arg4.AssignFromString(Args[3]);
      m_Arg5.AssignFromString(Args[4]);
      m_Arg6.AssignFromString(Args[5]);
      m_Arg7.AssignFromString(Args[6]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3,
                                                          (TArg4)m_Arg4,(TArg5)m_Arg5,(TArg6)m_Arg6,(TArg7)m_Arg7));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,QString Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 6 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3,class TArg4,class TArg5,class TArg6,class TArg7,class TArg8>
class CQMethod8 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3,TArg4,TArg5,TArg6,TArg7,TArg8);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;
    CQVariableProperty<TArg4> m_Arg4;
    CQVariableProperty<TArg5> m_Arg5;
    CQVariableProperty<TArg6> m_Arg6;
    CQVariableProperty<TArg7> m_Arg7;
    CQVariableProperty<TArg8> m_Arg8;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 8;
    }

  public:
    // Constructor (default)
    CQMethod8(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 8 parameters
      if(ArgsNum != 8)
        throw EQMethod("Invalid number of arguments for method call (8 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);
      m_Arg4.AssignFromString(Args[3]);
      m_Arg5.AssignFromString(Args[4]);
      m_Arg6.AssignFromString(Args[5]);
      m_Arg7.AssignFromString(Args[6]);
      m_Arg8.AssignFromString(Args[7]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3,
                                                          (TArg4)m_Arg4,(TArg5)m_Arg5,(TArg6)m_Arg6,
                                                          (TArg7)m_Arg7,(TArg8)m_Arg8));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,QString Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Implementation of a dynamic method wrapper - 6 arguments
template <class COwnerClass,class TReturnType,class TArg1,class TArg2,class TArg3,class TArg4,class TArg5,class TArg6,class TArg7,class TArg8,class TArg9,class TArg10>
class CQMethod10 : public CQCustomMethod<TReturnType> {
  private:
    // Method pointer type
    typedef TReturnType (COwnerClass::*TQMethodFunction)(TArg1,TArg2,TArg3,TArg4,TArg5,TArg6,TArg7,TArg8,TArg9,TArg10);

    // Pointer to the owner of the method
    COwnerClass *m_Owner;

    // Pointer to the actual method
    TQMethodFunction m_MethodFunction;

    // Typed arguments
    CQVariableProperty<TArg1> m_Arg1;
    CQVariableProperty<TArg2> m_Arg2;
    CQVariableProperty<TArg3> m_Arg3;
    CQVariableProperty<TArg4> m_Arg4;
    CQVariableProperty<TArg5> m_Arg5;
    CQVariableProperty<TArg6> m_Arg6;
    CQVariableProperty<TArg7> m_Arg7;
    CQVariableProperty<TArg8> m_Arg8;
    CQVariableProperty<TArg9> m_Arg9;
    CQVariableProperty<TArg10> m_Arg10;

    // Return the number of method arguments
    unsigned GetMethodArgumentsCount(void) {
      return 10;
    }

  public:
    // Constructor (default)
    CQMethod10(void) {
      Init(0,"",0);
    }

    // Invoke the function by using string parameters
    QString Invoke(const QString *Args,unsigned ArgsNum) {
      // This method implementation can accept only 10 parameters
      if(ArgsNum != 10)
        throw EQMethod("Invalid number of arguments for method call (10 arguments expected)");

      // Assign the argument string to the parameter(s)
      m_Arg1.AssignFromString(Args[0]);
      m_Arg2.AssignFromString(Args[1]);
      m_Arg3.AssignFromString(Args[2]);
      m_Arg4.AssignFromString(Args[3]);
      m_Arg5.AssignFromString(Args[4]);
      m_Arg6.AssignFromString(Args[5]);
      m_Arg7.AssignFromString(Args[6]);
      m_Arg8.AssignFromString(Args[7]);
      m_Arg9.AssignFromString(Args[8]);
      m_Arg10.AssignFromString(Args[9]);

      // Call the function
      return ValueToString(((*m_Owner).*m_MethodFunction)((TArg1)m_Arg1,(TArg2)m_Arg2,(TArg3)m_Arg3,
                                                          (TArg4)m_Arg4,(TArg5)m_Arg5,(TArg6)m_Arg6,
                                                          (TArg7)m_Arg7,(TArg8)m_Arg8,(TArg9)m_Arg9,(TArg10)m_Arg10));
    }

    // Initialize internal members
    void Init(COwnerClass *Owner,QString Name,TQMethodFunction MethodFunction) {
      InitMethod(Name);
      m_Owner = Owner;
      m_MethodFunction = MethodFunction;
    }
};

// Define a dynamic method with no arguments
#define DEFINE_METHOD(ClassName,ReturnType,MethodName)   \
  CQMethod0<ClassName,ReturnType> MethodName##_QMETHOD_;  \
  ReturnType MethodName(void)

// Define a dynamic method with 1 argument
#define DEFINE_METHOD_1(ClassName,ReturnType,MethodName,Arg1)   \
  CQMethod1<ClassName,ReturnType,Arg1> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1)

// Define a dynamic method with 2 arguments
#define DEFINE_METHOD_2(ClassName,ReturnType,MethodName,Arg1,Arg2)   \
  CQMethod2<ClassName,ReturnType,Arg1,Arg2> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2)

// Define a dynamic method with 3 arguments
#define DEFINE_METHOD_3(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3)   \
  CQMethod3<ClassName,ReturnType,Arg1,Arg2,Arg3> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2,Arg3)

// Define a dynamic method with 4 arguments
#define DEFINE_METHOD_4(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4)   \
  CQMethod4<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2,Arg3,Arg4)

// Define a dynamic method with 5 arguments
#define DEFINE_METHOD_5(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5)   \
  CQMethod5<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5)

// Define a dynamic method with 6 arguments
#define DEFINE_METHOD_6(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)   \
  CQMethod6<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)

// Define a dynamic method with 7 arguments
#define DEFINE_METHOD_7(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)   \
  CQMethod7<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)

// Define a dynamic method with 8 arguments
#define DEFINE_METHOD_8(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)   \
  CQMethod8<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> MethodName##_QMETHOD_;   \
  ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)

// The following macros are used to define virtual methods with RTTI info

// Define a dynamic virtual method with no arguments
#define DEFINE_V_METHOD(ClassName,ReturnType,MethodName)   \
  CQMethod0<ClassName,ReturnType> MethodName##_QMETHOD_;  \
  virtual ReturnType MethodName(void)

// Define a dynamic virtual method with 1 argument
#define DEFINE_V_METHOD_1(ClassName,ReturnType,MethodName,Arg1)   \
  CQMethod1<ClassName,ReturnType,Arg1> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1)

// Define a dynamic virtual method with 2 arguments
#define DEFINE_V_METHOD_2(ClassName,ReturnType,MethodName,Arg1,Arg2)   \
  CQMethod2<ClassName,ReturnType,Arg1,Arg2> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2)

// Define a dynamic virtual method with 3 arguments
#define DEFINE_V_METHOD_3(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3)   \
  CQMethod3<ClassName,ReturnType,Arg1,Arg2,Arg3> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3)

// Define a dynamic virtual method with 4 arguments
#define DEFINE_V_METHOD_4(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4)   \
  CQMethod4<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3,Arg4)

// Define a dynamic method with 5 arguments
#define DEFINE_V_METHOD_5(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5)   \
  CQMethod5<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5)

// Define a dynamic virtual method with 6 arguments
#define DEFINE_V_METHOD_6(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)   \
  CQMethod6<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)

// Define a dynamic virtual method with 7 arguments
#define DEFINE_V_METHOD_7(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)   \
  CQMethod7<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)

// Define a dynamic virtual method with 8 arguments
#define DEFINE_V_METHOD_8(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)   \
  CQMethod8<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)

#define DEFINE_V_METHOD_10(ClassName,ReturnType,MethodName,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,Arg10)   \
  CQMethod10<ClassName,ReturnType,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,Arg10> MethodName##_QMETHOD_;   \
  virtual ReturnType MethodName(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,Arg10)

  // NOTE! When adding new DEFINE_V_METHOD_xxx for more agruments, also update: MAX_QMETHOD_ARGUMENTS
//       otherwise it would not be possible to invoke from Python.

// Initialize a dynamic method (same for all methods)
#define INIT_METHOD(ClassName,MethodName)   \
  MethodName##_QMETHOD_.Init(this,#MethodName,&ClassName::MethodName);   \
  this->RegisterMethod(&MethodName##_QMETHOD_)

// Initialize a dynamic method without registering in a component (same for all methods)
#define INIT_METHOD_N(ClassName,MethodName)   \
  MethodName##_QMETHOD_.Init(this,#MethodName,&ClassName::MethodName)

#endif

