using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WpfLocalization
{
    /// <summary>
    /// Contains a reference to a <see cref="LocalizationCallback"/>.
    /// </summary>
    public class LocalizationCallbackReference
    {
        /// <summary>
        /// A reference to a <see cref="LocalizationCallback"/>.
        /// </summary>
        public event LocalizationCallback Callback;

        /// <summary>
        /// Gets the callback reference.
        /// </summary>
        /// <returns>The reference.</returns>
        internal LocalizationCallback GetCallback()
        {
            return Callback;
        }
    }
}
