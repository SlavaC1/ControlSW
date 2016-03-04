using BSS.MVVM.Model.BusinessLogic.Plc;
using System;
using System.Globalization;
using System.Linq;
using System.Windows.Data;
using System.Windows.Media;

namespace BSS.MVVM.View.Converters
{
    /// <summary>
    /// Converts traffic light color value to brush.
    /// </summary>
    internal class TrafficLightColorToBrushConverter : IValueConverter
    {
        private static BrushConverter brushConverter = new BrushConverter();

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
        /// <exception cref="System.NotImplementedException"></exception>
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            TrafficLightColor color = (TrafficLightColor)value;
            if (color == TrafficLightColor.Off)
            {
                return null;
            }

            return brushConverter.ConvertFromString(color.ToString());
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
            SolidColorBrush brush = value as SolidColorBrush;
            if (value == null)
            {
                return TrafficLightColor.Off;
            }

            var colorValues = brush.Color.GetNativeColorValues();
            if (colorValues.SequenceEqual(Brushes.Yellow.Color.GetNativeColorValues()))
            {
                return TrafficLightColor.Yellow;
            }

            if (colorValues.SequenceEqual(Brushes.Green.Color.GetNativeColorValues()))
            {
                return TrafficLightColor.Green;
            }

            if (colorValues.SequenceEqual(Brushes.Red.Color.GetNativeColorValues()))
            {
                return TrafficLightColor.Red;
            }

            return TrafficLightColor.Off;
        }
    }
}