using BSS.MVVM.Properties;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace BSS.MVVM.View.Converters
{
    public class DeviceSimulatorConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            bool useSimulator = (bool)value;
            if (useSimulator)
            {
                return Resources.Simulator;
            }

            return Resources.Device;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string str = value as string;
            if (str == Resources.Simulator)
            {
                return true;
            }

            if (str == Resources.Device)
            {
                return false;
            }

            return null;
        }
    }
}
