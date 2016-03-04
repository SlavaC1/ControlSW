using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Properties;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Views;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Security;

namespace BSS.MVVM.ViewModel
{
    public class CommunicationViewModel : ViewModelBase
    {
        #region Private Fields

        private bool allowConnection;

        private CommunicationsManager communicationsManager;

        private IDialogService dialogService;

        private string domain;

        private string userName;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="CommunicationViewModel"/> class.
        /// </summary>
        /// <param name="communicationsManager">A reference to the communication manager.</param>
        /// <exception cref="System.ArgumentNullException">
        /// communicationsManager
        /// </exception>
        public CommunicationViewModel(CommunicationsManager communicationsManager, string domain, IDialogService dialogService)
        {
            if (communicationsManager == null)
            {
                throw new ArgumentNullException("communicationsManager");
            }

            this.communicationsManager = communicationsManager;
            this.communicationsManager.RejectedChanged += HandleRejectedChanged;
            this.communicationsManager.CommunicationChanged += HandleCommunicationChanged;
            this.dialogService = dialogService;

            this.DeviceCommunications = new ObservableCollection<DeviceCommunicationViewModel>(
                Enum.GetValues(typeof(BssPeripheral))
                    .OfType<BssPeripheral>()
                    .Select(bssPeripheral => new DeviceCommunicationViewModel(bssPeripheral, communicationsManager.Connect)));

            this.AuthenticateCommand = new RelayCommand(Authenticate);
            this.Password = new SecureString();
            this.domain = domain;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets a value indicating whether devices reconnection is allowed.
        /// </summary>
        /// <value>
        ///   <c>true</c> if devices reconnection is allowed; otherwise, <c>false</c>.
        /// </value>
        public bool AllowConnection
        {
            get
            {
                return allowConnection;
            }
            set
            {
                allowConnection = value;
                AllowDevicesConnection();
                RaisePropertyChanged(() => AllowConnection);
            }
        }

        /// <summary>
        /// Gets the authenticate command.
        /// </summary>
        /// <value>
        /// The authenticate command.
        /// </value>
        public RelayCommand AuthenticateCommand
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the device communications.
        /// </summary>
        /// <value>
        /// The device communications.
        /// </value>
        public ObservableCollection<DeviceCommunicationViewModel> DeviceCommunications
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets or sets the domain.
        /// </summary>
        /// <value>
        /// The domain.
        /// </value>
        public string Domain
        {
            get
            {
                return domain;
            }
            set
            {
                domain = value;
                RaisePropertyChanged(() => Domain);
            }
        }

        /// <summary>
        /// Gets a value indicating whether application was rejected by server.
        /// </summary>
        /// <value>
        /// <c>true</c> if this instance is logged off; otherwise, <c>false</c>.
        /// </value>
        public bool IsRejected
        {
            get
            {
                return communicationsManager.IsRejected;
            }
        }

        /// <summary>
        /// Gets or sets the password.
        /// </summary>
        /// <value>
        /// The password.
        /// </value>
        public SecureString Password
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets or sets the name of the user.
        /// </summary>
        /// <value>
        /// The name of the user.
        /// </value>
        public string UserName
        {
            get
            {
                return userName;
            }
            set
            {
                userName = value;
                RaisePropertyChanged(() => UserName);
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Unregisters this instance from the Messenger class.
        /// <para>To cleanup additional resources, override this method, clean
        /// up and then call base.Cleanup().</para>
        /// </summary>
        public override void Cleanup()
        {
            this.communicationsManager.RejectedChanged -= HandleRejectedChanged;
            this.communicationsManager.CommunicationChanged -= HandleCommunicationChanged;
            base.Cleanup();
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Allows reconnection for all devices if <see cref="allowConnection"/> is <c>true</c> and client is autheticated.
        /// </summary>
        private void AllowDevicesConnection()
        {
            bool allow = this.allowConnection && !IsRejected;
            foreach (var deviceCommunication in DeviceCommunications)
            {
                deviceCommunication.AllowConnection = allow;
            }
        }

        /// <summary>
        /// Authenticates client according to provided username, password and domain.
        /// </summary>
        private async void Authenticate()
        {
            bool ok = await communicationsManager.Authenticate(new NetworkCredential(UserName, Password, Domain))
                .ConfigureAwait(continueOnCapturedContext: false);

            if (!ok)
            {
                await dialogService.ShowMessage(Resources.InvalidUserNamePassword, Resources.AppName)
                    .ConfigureAwait(continueOnCapturedContext: false);
            }

            AllowDevicesConnection();
        }

        private void HandleRejectedChanged(object sender, EventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                RaisePropertyChanged(() => IsRejected);
            });
        }

        private void HandleCommunicationChanged(object sender, CommunicationChangedEventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                DeviceCommunicationViewModel deviceVM = DeviceCommunications
                    .FirstOrDefault(vm => vm.Device == e.Device);

                if (deviceVM != null)
                {
                    deviceVM.IsConnected = e.IsConnected;
                }
            });
        }

        #endregion Private Methods
    }
}