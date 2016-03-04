using BSS.MVVM.Properties;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Provides interpretation method for IDT error codes.
    /// </summary>
    public static class IdtErrorInterpreter
    {
        /// <summary>
        /// Interprets the IDT error code.
        /// </summary>
        /// <param name="err">The error code.</param>
        /// <returns>The textual message that the error code represents.</returns>
        public static string Interpret(int err)
        {
            string errStr = err.ToString("X");
            var pi = typeof(Resources).GetProperty("IdtError_" + errStr, BindingFlags.NonPublic | BindingFlags.Static);
            if (pi == null)
            {
                return String.Format(Resources.IdtError, errStr);
            }

            return pi.GetGetMethod(nonPublic: true).Invoke(null, null).ToString();
        }
    }
}
