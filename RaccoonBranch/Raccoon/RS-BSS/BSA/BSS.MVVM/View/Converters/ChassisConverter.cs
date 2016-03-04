using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Properties;
using System;
using System.ComponentModel;
using System.Globalization;
using System.Windows.Data;

namespace BSS.MVVM.View.Converters
{
    internal class ChassisConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                int index = System.Convert.ToInt32(value);
                return String.Format(Resources.ChassisNumber, index);
            }
            catch
            {
                return null;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}