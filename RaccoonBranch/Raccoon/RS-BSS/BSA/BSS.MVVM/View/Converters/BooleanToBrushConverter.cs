using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace BSS.MVVM.View.Converters
{
    /// <summary>
    /// A converter from boolean value to its color representation.
    /// </summary>
    internal class BooleanToBrushConverter : IValueConverter
    {
        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value produced by the binding source.</param>
        /// <param name="targetType">The type of the binding target property.</param>
        /// <param name="parameter">The converter parameter to use.</param>
        /// <param name="culture">The culture to use in the converter.</param>
        /// <returns>
        /// A converted value. If the method returns null, the valid null value is used.
        /// </returns>
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                return ((bool)value) ? Brushes.Green : Brushes.Red;
            }
            catch
            {
                return null;
            }
        }

        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value that is produced by the binding target.</param>
        /// <param name="targetType">The type to convert to.</param>
        /// <param name="parameter">The converter parameter to use.</param>
        /// <param name="culture">The culture to use in the converter.</param>
        /// <returns>
        /// A converted value. If the method returns null, the valid null value is used.
        /// </returns>
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == Brushes.Green)
            {
                return true;
            }

            if (value == Brushes.Red)
            {
                return false;
            }

            return null;
        }
    }
}