using RaccoonCLI;
using System;
using System.ComponentModel;
using System.Threading;
using System.Threading.Tasks;

using Timer = System.Timers.Timer;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Responsible for getting in-place status and notifying about status changes.
    /// </summary>
    public class InPlaceManager : IDisposable
    {
        #region Private Fields

        /// <summary>
        /// A referemce to the configuration parameters.
        /// </summary>
        private ConfigurationParameters configurationParameters;

        /// <summary>
        /// The in-place status
        /// </summary>
        private int inPlaceStatus;

        /// <summary>
        /// <c>true</c> when HW is ready; otherwise, <c>false</c>.
        /// </summary>
        private bool isReady;

        /// <summary>
        /// A reference to the material monitor.
        /// </summary>
        private MaterialMonitorWrapper materialMonitor;

        /// <summary>
        /// A timer for periodically invoking in-place status query.
        /// </summary>
        private Timer timer;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="InPlaceManager"/> class.
        /// </summary>
        /// <param name="materialMonitor">The material monitor.</param>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <exception cref="System.ArgumentNullException">
        /// materialMonitor
        /// or
        /// configurationParameters
        /// </exception>
        public InPlaceManager(MaterialMonitorWrapper materialMonitor, ConfigurationParameters configurationParameters)
        {
            if (materialMonitor == null)
            {
                throw new ArgumentNullException("materialMonitor");
            }

            if (configurationParameters == null)
            {
                throw new ArgumentNullException("configurationParameters");
            }

            this.materialMonitor = materialMonitor;
            this.configurationParameters = configurationParameters;
            this.configurationParameters.PropertyChanged += HandleConfigurationParameterChanged;

            CartridgesCount = Convert.ToByte(configurationParameters.ChassisCount);

            timer = new Timer(configurationParameters.InPlaceStatusInterval.TotalMilliseconds);
            timer.Elapsed += (sender, e) =>
            {
                GetInPlaceStatus();
            };
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when in-lace status is changed.
        /// </summary>
        public event EventHandler<InPlaceStatusChangedEventArgs> InPlaceStatusChanged;

        /// <summary>
        /// Occurs when HW initialization is completed.
        /// </summary>
        public event EventHandler IsReadyChanged;

        #endregion Public Events

        #region Public Properties

        /// <summary>
        /// Gets the cartridges count.
        /// </summary>
        /// <value>
        /// The cartridges count.
        /// </value>
        public byte CartridgesCount { get; private set; }

        /// <summary>
        /// Gets a value indicating if there are inserted cartridges.
        /// </summary>
        /// <value>
        /// <c>true</c> if there are inserted cartridges; otherwise, <c>false</c>.
        /// </value>
        public bool HasCartridges
        {
            get
            {
                if (!IsActive)
                {
                    GetInPlaceStatus();
                }

                return InPlaceStatus != 0;
            }
        }

        /// <summary>
        /// Gets the in-place status.
        /// </summary>
        /// <value>
        /// The in-place status.
        /// </value>
        public int InPlaceStatus
        {
            get
            {
                return inPlaceStatus;
            }
            private set
            {
                if (inPlaceStatus != value)
                {
                    int oldValue = inPlaceStatus;
                    inPlaceStatus = value;
                    InPlaceStatusChangedEventArgs e = new InPlaceStatusChangedEventArgs(oldValue, value);
                    OnInPlaceStatusChanged(e);
                }
            }
        }

        /// <summary>
        /// Gets a value indicating whether continuous getting in-place status is active.
        /// </summary>
        /// <value>
        ///   <c>true</c> if active; otherwise, <c>false</c>.
        /// </value>
        public bool IsActive
        {
            get
            {
                return timer.Enabled;
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Initializes IDT HW.
        /// </summary>
        public async Task<bool> InitHW()
        {
            Task<int> initHWTask = Task.Run(() =>
                {
                    byte cartridgesCount;
                    return materialMonitor.InitHW(out cartridgesCount);
                });

            await initHWTask.ConfigureAwait(continueOnCapturedContext: false);
            bool ok = (initHWTask.Result == 0);
            SetIsReady(value: ok);

            return ok;
        }

        /// <summary>
        /// Gets a value indicating whether IDT-HW is ready.
        /// </summary>
        /// <value>
        ///   <c>true</c> if IDT-HW is ready; otherwise, <c>false</c>.
        /// </value>
        public bool IsReady()
        {
            return isReady;
        }

        /// <summary>
        /// Sets the host public key into material monitor.
        /// </summary>
        /// <param name="pubKS">The host public key.</param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentNullException">pubKS</exception>
        public async Task<int> SetPubKS(byte[] pubKS)
        {
            if (pubKS == null)
            {
                throw new ArgumentNullException("pubKS");
            }

            return await Task<int>.Run(() =>
                {
                    return materialMonitor.SetPubKS(pubKS);
                }).ConfigureAwait(continueOnCapturedContext: false);
        }

        /// <summary>
        /// Starts continuous call to get in-place status.
        /// </summary>
        public void StartGetInPlaceStatus()
        {
            if (isReady)
            {
                timer.Start();
                GetInPlaceStatus();
            }
        }

        /// <summary>
        /// Stops continuous call to get in-place status.
        /// </summary>
        public void StopGetInPlaceStatus()
        {
            timer.Stop();
        }

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Raises the <see cref="E:InPlaceStatusChanged"/> event.
        /// </summary>
        /// <param name="e">The <see cref="InPlaceStatusChangedEventArgs"/> instance containing the event data.</param>
        protected virtual void OnInPlaceStatusChanged(InPlaceStatusChangedEventArgs e)
        {
            EventHandler<InPlaceStatusChangedEventArgs> temp = InPlaceStatusChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:IsReadyChanged"/> event.
        /// </summary>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        protected virtual void OnIsReadyChanged(EventArgs e)
        {
            EventHandler temp = IsReadyChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        #endregion Protected Methods

        #region Private Methods

        /// <summary>
        /// Gets the in-place status from material monitor.
        /// </summary>
        private void GetInPlaceStatus()
        {
            int newInPlaceStatus, err;
            bool isReadyForGettingInPlaceStatus = Monitor.TryEnter(materialMonitor);
            if (isReadyForGettingInPlaceStatus)
            {
                try
                {
                    err = materialMonitor.GetInPlaceStatus(out newInPlaceStatus);
                }
                finally
                {
                    Monitor.Exit(materialMonitor);
                }
            }
            else
            {
                newInPlaceStatus = err = 0;
            }

            SetIsReady(value: isReadyForGettingInPlaceStatus && (err == 0));
            InPlaceStatus = newInPlaceStatus & ((1 << CartridgesCount) - 1);
        }

        private void HandleConfigurationParameterChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "InPlaceStatusInterval")
            {
                bool enabled = timer.Enabled;
                timer.Enabled = false;
                timer.Interval = configurationParameters.InPlaceStatusInterval.TotalMilliseconds;
                timer.Enabled = enabled;
            }
        }

        /// <summary>
        /// Sets the isReady flag.
        /// </summary>
        /// <param name="value">if set to <c>true</c>, FCB is ready.</param>
        private void SetIsReady(bool value)
        {
            if (isReady != value)
            {
                isReady = value;
                if (!isReady)
                {
                    StopGetInPlaceStatus();
                }

                OnIsReadyChanged(EventArgs.Empty);

                if (isReady)
                {
                    StartGetInPlaceStatus();
                }
            }
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
                timer.Dispose();
                this.configurationParameters.PropertyChanged -= HandleConfigurationParameterChanged;
            }
        }

        #endregion IDisposable Members
    }
}