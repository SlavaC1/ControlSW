/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Shared utils.                                           *
 * Module: Hysteresis.                                              *
 * Module Description: Implementation of a simple hysteresis logic. *
 *                                                                  *
 * Compilation: Standard C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 29/08/2001                                           *
 * Last upate: 29/08/2001                                           *
 ********************************************************************/

#ifndef _HYSTERESIS_VALUE_H_
#define _HYSTERESIS_VALUE_H_


// Utility class for implementing simple hysteresis logic
template <class T>
class CHysteresis {
  private:
    // Left and right limits on the X axis
    T m_LeftValue;
    T m_RightValue;

    // Current value
    T m_CurrentValue;

    // This flag remember the current state
    bool m_On;

  public:
    // Constructor
    CHysteresis(T LeftValue=0,T RightValue=0) {
      SetRange(LeftValue,RightValue);
    }

    void SetRange(T LeftValue,T RightValue) {
      // Initialize members
      m_LeftValue = LeftValue;
      m_RightValue = RightValue;

      m_CurrentValue = 0;
      m_On = false;
    }

    // Assignment of a new value
    inline CHysteresis<T>& operator = (const T& Value) {
      // Save current value
      m_CurrentValue = Value;

      // Hysteresis logic test
      if(Value >= m_RightValue)
        m_On = true;
      else
        if(Value < m_LeftValue)
         m_On = false;

      return *this;
    }

    // Current value (cast to type T)
    inline operator T& () {
      return m_CurrentValue;
    }

    // Return true is the value is inside the hysteresis area
    inline bool IsOn(void) {
      return m_On;
    }
};

#endif
