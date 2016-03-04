using System;
using System.Globalization;
using System.Windows.Data;

namespace BSS.MVVM.View.Bindings
{
    /// <summary>
    /// A binding awares of current culture.
    /// </summary>
    public class CultureAwareBinding : Binding
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BSS.MVVM.View.Bindings.CultureAwareBinding"/> class.
        /// </summary>
        public CultureAwareBinding()
        {
            ConverterCulture = CultureInfo.CurrentCulture;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="BSS.MVVM.View.Bindings.CultureAwareBinding"/> class with an initial path.
        /// </summary>
        /// <param name="path">The initial <see cref="P:System.Windows.Data.Binding.Path" /> for the binding.</param>
        public CultureAwareBinding(string path)
            : base(path)
        {
            ConverterCulture = CultureInfo.CurrentCulture;
        }

        #endregion Public Constructors
    }
}