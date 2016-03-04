using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Properties;
using System;
using System.Globalization;
using System.Windows.Data;

namespace BSS.MVVM.View.Converters
{
    /// <summary>
    /// Converts traffic light color value to textual message.
    /// </summary>
    internal class TrafficLightColorToTextConverter : IValueConverter
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
            TrafficLightColor trafficLightColor = (TrafficLightColor)value;
            switch (trafficLightColor)
            {
                case TrafficLightColor.Off:
                    return Resources.ChassisEmpty;

                case TrafficLightColor.Red:
                    return Resources.OperationFailed;

                case TrafficLightColor.Yellow:
                    return Resources.CartridgeInserted;

                case TrafficLightColor.Green:
                    return Resources.OperationCompleted;

                default:
                    return String.Empty;
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
        /// <exception cref="System.ArgumentNullException">value</exception>
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
            {
                throw new ArgumentNullException("value");
            }

            string text = value.ToString();

            if (text == Resources.ChassisEmpty)
            {
                return TrafficLightColor.Off;
            }

            if (text == Resources.CartridgeInserted)
            {
                return TrafficLightColor.Yellow;
            }

            if (text == Resources.OperationCompleted)
            {
                return TrafficLightColor.Green;
            }

            if (text == Resources.OperationFailed)
            {
                return TrafficLightColor.Red;
            }

            return new ArgumentException("value does not represent a valid text.", "value");
        }
    }
}