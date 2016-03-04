using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Plc;
using GalaSoft.MvvmLight;

namespace BSS.MVVM.ViewModel
{
    public class IdtStatusViewModel : ViewModelBase
    {
        #region Private Fields

        private IdtOperation operation;

        private byte cartridgeNumber;

        private object data;

        private sbyte progress;

        private TrafficLightColor trafficLightColor;

        #endregion Private Fields

        #region Public Constructors

        public IdtStatusViewModel()
        {
            progress = -1;
        }

        #endregion Public Constructors

        #region Public Properties

        public IdtOperation Operation
        {
            get
            {
                return operation;
            }
            set
            {
                operation = value;
                RaisePropertyChanged(() => Operation);
            }
        }

        public byte CartridgeNumber
        {
            get
            {
                return cartridgeNumber;
            }
            set
            {
                cartridgeNumber = value;
                RaisePropertyChanged(() => CartridgeNumber);
            }
        }

        public object Data
        {
            get
            {
                return data;
            }
            set
            {
                data = value;
                RaisePropertyChanged(() => Data);
            }
        }

        public sbyte Progress
        {
            get
            {
                return progress;
            }
            set
            {
                progress = value;
                RaisePropertyChanged(() => Progress);
            }
        }

        public TrafficLightColor TrafficLightColor
        {
            get
            {
                return trafficLightColor;
            }
            set
            {
                trafficLightColor = value;
                RaisePropertyChanged(() => TrafficLightColor);
            }
        }

        #endregion Public Properties
    }
}