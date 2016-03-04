using BSS.MVVM.Properties;
using System;
using System.Globalization;
using System.Reflection;
using System.Windows.Data;

namespace BSS.MVVM.View.Converters
{
    /// <summary>
    /// Converts text to its localized value.
    /// </summary>
    internal class TextToLocalizedConverter : IValueConverter
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
            if (value == null)
            {
                return null;
            }

            var localizedProperty = typeof(Resources)
                .GetProperty(value.ToString(), BindingFlags.NonPublic | BindingFlags.Static);

            if (localizedProperty == null)
            {
                return null;
            }

            return localizedProperty
                .GetGetMethod(nonPublic: true)
                .Invoke(null, null).ToString();
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
        /// <exception cref="System.NotImplementedException"></exception>
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}