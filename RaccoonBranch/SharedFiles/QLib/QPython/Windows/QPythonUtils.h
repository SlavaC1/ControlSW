/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: General                                                 *
 * Module: QPython                                                  *
 * Module Description: Python related utils.                        *
 *                                                                  *
 * Compilation: Borland C++ builder, Python-For-Delphi components   *
 *                                                                  *
 * Author: Ran Peleg                                                *
 * Start date: 20/07/2003                                           *
 * Last upate: 20/07/2003                                           *
 ********************************************************************/

#include "PythonEngine.hpp"


// Automatic python reference decrement wrapper class
class CPyAutoDecRef {
  private:
    PPyObject m_PyObj;

  public:
    CPyAutoDecRef(void) {
      m_PyObj = NULL;
    }

    CPyAutoDecRef(const PPyObject PyObj) {
      m_PyObj = PyObj;
    }

    ~CPyAutoDecRef(void) {
      if(m_PyObj != NULL)
      {
        GetPythonEngine()->Py_DECREF(m_PyObj);
        m_PyObj = NULL;
      }
    }

    // Explicit get object
    PPyObject GetObj(void) {
      return m_PyObj;
    }

    // Cast to PPyObject
    operator PPyObject & () {
      return m_PyObj;
    }

    CPyAutoDecRef& operator = (PPyObject Obj) {
      m_PyObj = Obj;
      return *this;
    }
};