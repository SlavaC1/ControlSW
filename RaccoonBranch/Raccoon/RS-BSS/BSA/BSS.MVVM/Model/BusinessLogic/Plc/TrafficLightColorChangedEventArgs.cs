using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    public class TrafficLightColorChangedEventArgs : EventArgs
    {
        public byte Index { get; private set; }

        public TrafficLightColor Color { get; private set; }

        public TrafficLightColorChangedEventArgs(byte index, TrafficLightColor color)
        {
            Index = index;
            Color = color;
        }
    }
}
