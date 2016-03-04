using System;
using System.Collections.Generic;

using System.Text;
using System.Globalization;

namespace WpfLocalization
{
    /// <summary>
    /// Provides a localized value.
    /// </summary>
    /// <param name="culture">The culture to use for formatting.</param>
    /// <param name="uiCulture">The culture to use for language.</param>
    /// <returns>The localized value.</returns>
    public delegate object LocalizationCallback(CultureInfo culture, CultureInfo uiCulture, object parameter);
}
