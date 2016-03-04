using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Properties;
using Hsm.Contracts;
using System;
using System.Net;
using System.Threading.Tasks;
using WcfInfras.Client;

namespace BSS.MVVM.Model.BusinessLogic
{
    public enum BssPeripheral
    {
        FCB,
        PLC,
        HSM,
        DB,
    }

    public static class ProxyInvokerUtils
    {
        #region Public Methods

        /// <summary>
        /// Creates a new instance of a proxy invoker.
        /// </summary>
        /// <typeparam name="T">Type of cotract.</typeparam>
        /// <param name="endpointName">Name of the endpoint.</param>
        /// <returns></returns>
        public static ProxyInvoker<T> CreateProxyInvoker<T>(string endpointName)
        {
            ProxyInvoker<T> proxyInvoker = new ProxyInvoker<T>(endpointName);
            proxyInvoker.Exception += (sender, e) =>
            {
                MessengerUtils.SendException(e.Exception);
            };
            proxyInvoker.Error += (sender, e) =>
            {
                MessengerUtils.SendErrorMessage(
                    String.Format("{0}:{1}{2}",
                    Resources.CannotCommunicateWithCBS, Environment.NewLine, e.Error));
            };

            return proxyInvoker;
        }

        #endregion Public Methods
    }

    public class CommunicationChangedEventArgs : EventArgs
    {
        #region Public Constructors

        public CommunicationChangedEventArgs(BssPeripheral device, bool isConnected)
        {
            this.Device = device;
            this.IsConnected = isConnected;
        }

        #endregion Public Constructors

        #region Public Properties

        public BssPeripheral Device { get; private set; }

        public bool IsConnected { get; private set; }

        #endregion Public Properties
    }

    /// <summary>
    /// Manages communication to BSS peripheral devices.
    /// </summary>
    public class CommunicationsManager : IDisposable
    {
        #region Private Fields

        /// <summary>
        /// Holds the connection status for each peripheral device.
        /// </summary>
        private bool[] connectionStatus;

        /// <summary>
        /// A reference to the in-place manager.
        /// </summary>
        private InPlaceManager inPlaceManager;

        /// <summary>
        /// <c>true</c> if this applicaion is rejected by central burning server; otherwise, <c>false</c>.
        /// </summary>
        private bool isRejected;

        /// <summary>
        /// A reference to the PLC wrapper.
        /// </summary>
        private IPlc plcWrapper;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="CommunicationsManager"/> class.
        /// </summary>
        /// <param name="inPlaceManager">The in place manager.</param>
        /// <param name="plcWrapper">The PLC wrapper.</param>
        /// <exception cref="ArgumentNullException">
        /// inPlaceManager
        /// or
        /// plcWrapper
        /// </exception>
        public CommunicationsManager(InPlaceManager inPlaceManager, IPlc plcWrapper)
        {
            if (inPlaceManager == null)
            {
                throw new ArgumentNullException("inPlaceManager");
            }

            if (plcWrapper == null)
            {
                throw new ArgumentNullException("plcWrapper");
            }

            this.inPlaceManager = inPlaceManager;
            this.plcWrapper = plcWrapper;
            connectionStatus = new bool[Enum.GetValues(typeof(BssPeripheral)).Length];

            this.inPlaceManager.IsReadyChanged += HandleIsFcbReadyChanged;
            this.plcWrapper.IsReadyChanged += HandleIsPlcReadyChanged;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether applicaion is rejected by central burning server.
        /// </summary>
        /// <value>
        /// <c>true</c> if this applicaion is rejected by central burning server; otherwise, <c>false</c>.
        /// </value>
        public bool IsRejected
        {
            get
            {
                return isRejected;
            }
            private set
            {
                isRejected = value;
                OnRejectedChanged(EventArgs.Empty);
            }
        }

        #endregion Public Properties

        #region Public Events

        /// <summary>
        /// Occurs when communication with a peripheral device changed.
        /// </summary>
        public event EventHandler<CommunicationChangedEventArgs> CommunicationChanged;

        /// <summary>
        /// Occurs when applicaion is rejected by central burning server.
        /// </summary>
        public event EventHandler RejectedChanged;

        #endregion Public Events

        #region Public Methods

        /// <summary>
        /// Authenticates client using the specified windows credential.
        /// </summary>
        /// <param name="windowsCredential">The windows credential.</param>
        /// <returns><c>true</c> if client was authenticated successfully; otherwise, <c>false</c>.</returns>
        public async Task<bool> Authenticate(NetworkCredential windowsCredential)
        {
            if (IsRejected)
            {
                MessengerUtils.SendInfoMessage(Resources.SendingClientCredential);
                ProxyInvokerCredentials.ClientCredential = windowsCredential;
                await Connect(BssPeripheral.HSM);
                bool connected = GetDeviceConnectionStatus(BssPeripheral.HSM);
                if (connected)
                {
                    await Connect(BssPeripheral.DB);
                }
            }

            return !IsRejected;
        }

        /// <summary>
        /// Connects the specified target device.
        /// </summary>
        /// <param name="targetDevice">The target device.</param>
        public async Task Connect(BssPeripheral targetDevice)
        {
            bool ok;
            try
            {
                switch (targetDevice)
                {
                    case BssPeripheral.FCB:
                        ok = await inPlaceManager.InitHW();
                        break;

                    case BssPeripheral.PLC:
                        ok = await plcWrapper.Init();
                        break;

                    case BssPeripheral.HSM:
                        ok = await ConnectToHsmAsync();
                        SetDeviceConnectionStatus(BssPeripheral.HSM, isConnected: ok);
                        break;

                    case BssPeripheral.DB:
                        ok = await ConnectToDBAsync();
                        SetDeviceConnectionStatus(BssPeripheral.DB, isConnected: ok);
                        break;

                    default:
                        ok = false;
                        break;
                }
            }
            catch (Exception ex)
            {
                ok = false;
                MessengerUtils.SendException(ex);
            }

            if (ok)
            {
                MessengerUtils.SendInfoMessage(String.Format(Resources.ConnectionOK, targetDevice));
            }
            else
            {
                MessengerUtils.SendErrorMessage(String.Format(Resources.ConnectionFailed, targetDevice));
            }
        }

        /// <summary>
        /// Sets main view-model to its initial state.
        /// </summary>
        /// <returns></returns>
        public async Task ConnectAll()
        {
            var connectToFcbTask = Connect(BssPeripheral.FCB);
            var connectToPlcTask = Connect(BssPeripheral.PLC);
            var connectToCbsTask = ConnectToCbsAsync();

            await Task.WhenAll(connectToFcbTask, connectToPlcTask, connectToCbsTask);
        }

        /// <summary>
        /// Gets connection status with a device.
        /// </summary>
        /// <param name="bssPeripheral">The device.</param>
        /// <returns></returns>
        public bool GetDeviceConnectionStatus(BssPeripheral bssPeripheral)
        {
            return connectionStatus[(int)bssPeripheral];
        }

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Raises the <see cref="E:CommunicationChanged" /> event.
        /// </summary>
        /// <param name="e">The <see cref="CommunicationChangedEventArgs"/> instance containing the event data.</param>
        protected virtual void OnCommunicationChanged(CommunicationChangedEventArgs e)
        {
            EventHandler<CommunicationChangedEventArgs> temp = CommunicationChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:RejectedChanged" /> event.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnRejectedChanged(EventArgs e)
        {
            EventHandler temp = RejectedChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        #endregion Protected Methods

        #region Private Methods

        /// <summary>
        /// Connects to CBS asynchronously.
        /// </summary>
        /// <returns></returns>
        private async Task<bool> ConnectToCbsAsync()
        {
            bool connected = await ConnectToHsmAsync();
            SetDeviceConnectionStatus(BssPeripheral.HSM, connected);
            if (!connected)
            {
                return false;
            }

            connected = await ConnectToDBAsync();
            SetDeviceConnectionStatus(BssPeripheral.DB, connected);

            return connected;
        }

        /// <summary>
        /// Connects to DB.
        /// </summary>
        /// <returns></returns>
        private async Task<bool> ConnectToDBAsync()
        {
            using (ProxyInvoker<IReport> reportProxyInvoker = ProxyInvokerUtils.CreateProxyInvoker<IReport>("ReportEndPoint"))
            {
                reportProxyInvoker.CreateProxy();
                Task<string> GetMaterialNameTask = null;
                bool isConnected = await reportProxyInvoker.InvokeAsync(report =>
                {
                    GetMaterialNameTask = report.GetMaterialNameAsync(0);
                    return GetMaterialNameTask;
                });

                return isConnected;
            }
        }

        /// <summary>
        /// Connects to HSM asynchronously.
        /// </summary>
        /// <returns></returns>
        private async Task<bool> ConnectToHsmAsync()
        {
            using (ProxyInvoker<IHsm> hsmProxyInvoker = ProxyInvokerUtils.CreateProxyInvoker<IHsm>("HsmEndPoint"))
            {
                hsmProxyInvoker.CreateProxy();
                Task<byte[]> GetPublicKeyTask = null;

                bool isConnected = await hsmProxyInvoker.InvokeAsync((hsm =>
                    {
                        GetPublicKeyTask = hsm.GetPublicKeyAsync();
                        return GetPublicKeyTask;
                    }));

                IsRejected = hsmProxyInvoker.IsRejected;
                if (GetPublicKeyTask.Status == TaskStatus.Faulted)
                {
                    return false;
                }

                byte[] pubKS = await GetPublicKeyTask.ConfigureAwait(continueOnCapturedContext: false);
                if (pubKS == null)
                {
                    return false;
                }

                int err = await inPlaceManager.SetPubKS(pubKS);
                if (err != 0)
                {
                    MessengerUtils.SendException(new InvalidOperationException(IdtErrorInterpreter.Interpret(err)));
                }

                return err == 0;
            }
        }

        private void HandleIsFcbReadyChanged(object sender, EventArgs e)
        {
            SetDeviceConnectionStatus(BssPeripheral.FCB, inPlaceManager.IsReady());
        }

        private void HandleIsPlcReadyChanged(object sender, EventArgs e)
        {
            SetDeviceConnectionStatus(BssPeripheral.PLC, plcWrapper.IsReady());
        }

        private void SetDeviceConnectionStatus(BssPeripheral bssPeripheral, bool isConnected)
        {
            connectionStatus[(int)bssPeripheral] = isConnected;
            OnCommunicationChanged(new CommunicationChangedEventArgs(bssPeripheral, isConnected));
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(dispose: true);
        }

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool dispose)
        {
            if (dispose)
            {
                this.inPlaceManager.IsReadyChanged -= HandleIsFcbReadyChanged;
                this.plcWrapper.IsReadyChanged -= HandleIsPlcReadyChanged;
            }
        }

        #endregion IDisposable Members
    }
}