﻿using BSS.Contracts;
using System;
using System.Globalization;
using System.Linq;
using System.Windows.Data;

namespace BSS.MVVM.View.Converters
{
    /// <summary>
    /// Converts byte array to hexadecimal string.
    /// </summary>
    internal class ByteArrayToHexStringConverter : IValueConverter
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
            byte[] byteArray = value as byte[];
            if (byteArray == null)
            {
                return String.Empty;
            }

            return byteArray.ToHexString();
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
            string hexString = value as string;
            if (hexString == null)
            {
                return null;
            }

            return hexString.ToByteArray();
        }
    }
}