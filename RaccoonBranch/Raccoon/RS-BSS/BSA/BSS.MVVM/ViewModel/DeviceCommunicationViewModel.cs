using BSS.MVVM.Model.BusinessLogic;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;
using System;
using System.Threading.Tasks;

namespace BSS.MVVM.ViewModel
{
    public class DeviceCommunicationViewModel : ViewModelBase
    {
        #region Private Fields

        private bool allowConnection;

        private Func<BssPeripheral, Task> connectAction;

        private BssPeripheral device;

        private bool isBusy;

        private bool isConnected;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceCommunicationViewModel"/> class.
        /// </summary>
        /// <param name="device">The device.</param>
        /// <param name="connectAction">The action to invoke when connect command is executed.</param>
        /// <param name="isConnectedFunction">The function to invoke when IsConnected shall be set.</param>
        public DeviceCommunicationViewModel(BssPeripheral device, Func<BssPeripheral, Task> connectAction)
        {
            this.device = device;
            this.connectAction = connectAction;
            
            ConnectCommand = new RelayCommand(async () =>
            {
                try
                {
                    IsBusy = true;
                    Task connectionTask = this.connectAction(this.device);
                    await connectionTask.ConfigureAwait(continueOnCapturedContext: false);
                }
                finally
                {
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            IsBusy = false;
                        });
                }
            },
            () =>
            {
                return this.connectAction != null && !IsBusy;
            });
        }

        #endregion Public Constructors

        #region Public Properties

        public bool AllowConnection
        {
            get
            {
                return allowConnection;
            }
            set
            {
                allowConnection = value && connectAction != null;
                RaisePropertyChanged(() => AllowConnection);
            }
        }

        /// <summary>
        /// Gets the connect command.
        /// </summary>
        /// <value>
        /// The connect command.
        /// </value>
        public RelayCommand ConnectCommand
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets or sets the device.
        /// </summary>
        /// <value>
        /// The device.
        /// </value>
        public BssPeripheral Device
        {
            get
            {
                return device;
            }
            set
            {
                device = value;
                RaisePropertyChanged(() => Device);
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether device is busy.
        /// </summary>
        /// <value>
        ///   <c>true</c> if device is busy; otherwise, <c>false</c>.
        /// </value>
        public bool IsBusy
        {
            get
            {
                return isBusy;
            }
            set
            {
                isBusy = value;
                RaisePropertyChanged(() => IsBusy);
                ConnectCommand.RaiseCanExecuteChanged();
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether device is connected.
        /// </summary>
        /// <value>
        /// <c>true</c> if this device is connected; otherwise, <c>false</c>.
        /// </value>
        public bool IsConnected
        {
            get
            {
                return isConnected;
            }
            set
            {
                isConnected = value;
                RaisePropertyChanged(() => IsConnected);
            }
        }

        #endregion Public Properties
    }
}